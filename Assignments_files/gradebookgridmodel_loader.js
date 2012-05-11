// called to load model with server data
Gradebook.GridModel.prototype.requestLoadData = function(forceFlush)
{
  this.lastUpdateTS = new Date().getTime();
  this.gradebookService.requestLoadData((this._loadDataFromJSON).bind(this), (this._reportError).bind(this), (this._reportException).bind(this), forceFlush);
};

// called to update model with server data
Gradebook.GridModel.prototype.requestUpdateData = function()
{
  var timeSinceLastUpdate = new Date().getTime() - this.lastUpdateTS;
  // don't update if window is resizing and we've reloaded in the last 5
  // minutes
  if (!this.usingCachedBook && this.resizingWindow && (timeSinceLastUpdate < 5 * 60 * 1000))
  {
    this.fireModelChanged();
    return;
  }
  this.lastUpdateTS = new Date().getTime();
  var customViewId = null;
  if (this.currentCustomView && this.currentCustomView.usesGroups())
  {
    customViewId = this.currentCustomView.id;
  }
  this.gradebookService.requestUpdateData(this.version, this.lastUserChangeTS, this.usersHash, customViewId, (this._updateDataFromJSON).bind(this),
      (this._reportError).bind(this), (this._reportException).bind(this));
};

function registerScoreProviderActionController( controller, ctrlJsName )
{
  var model = Gradebook.getModel();
  for (var i in model.scoreProviderActionsMap)
  {
    if ( model.scoreProviderActionsMap.hasOwnProperty( i ) )
    {
      var action = model.scoreProviderActionsMap[i];
      if (action.controlLogic && action.controlLogic.indexOf( ctrlJsName ) > -1)
      {
        action.controller = controller;
        return;
      }
    }
  }
}

// callback when initializing this gradebook model with server data
Gradebook.GridModel.prototype._loadDataFromJSON = function(reply)
{
  var jsonBook;
  try
  {
    if (typeof (JSON) === 'object' && typeof (JSON.parse) === 'function')
    {
      jsonBook = JSON.parse(reply.responseText);
    }
    else
    {
      jsonBook = eval('(' + reply.responseText + ')');
    }
  }
  catch (e)
  {
    this.fireModelError(e, reply.responseText);
    return;
  }
  if (jsonBook.cachedBook)
  {
    jsonBook = jsonBook.cachedBook;
    this.usingCachedBook = true;
  }
  try
  {
    this.schemaMap = [];
    var i, len;
    for (i = 0; i < jsonBook.schemas.length; i++)
    {
      jsonBook.schemas[i] = this._createSchema(jsonBook.schemas[i].type, jsonBook.schemas[i]);
      this.schemaMap[jsonBook.schemas[i].id] = jsonBook.schemas[i];
    }
    this.colDefMap = [];
    for (i = 0; i < jsonBook.colDefs.length; i++)
    {
      jsonBook.colDefs[i] = this._createColDef(jsonBook.colDefs[i], this, this.schemaMap);
      this.colDefMap[jsonBook.colDefs[i].id] = i;
    }

    // embelish 1st cell of each row with some flags
    this.rowUserIdMap = [];
    if (jsonBook.rows)
    {
      for (i = 0, len = jsonBook.rows.length; i < len; i++)
      {
        var c = jsonBook.rows[i][0];
        c.isRowChecked = false;
        c.isHidden = false;
        c.isAvailable = c.avail;
        c.comput_err = false;
        this.rowUserIdMap[c.uid] = i;
      }
    }

    this.customViewMap = [];
    if (jsonBook.customViews)
    {
      for (i = 0; i < jsonBook.customViews.length; i++)
      {
        jsonBook.customViews[i] = new Gradebook.CustomView(jsonBook.customViews[i], this);
        this.customViewMap[jsonBook.customViews[i].id] = i;
      }
    }
    this.groupsMap = [];
    if (jsonBook.groups)
    {
      for (i = 0; i < jsonBook.groups.length; i++)
      {
        this.groupsMap[jsonBook.groups[i].id] = i;
      }
    }
    this.scoreProvidersMap = [];
    this.scoreProviderActionsMap = [];
    if (jsonBook.scoreProviders)
    {
      for ( i = 0; i < jsonBook.scoreProviders.length; i++)
      {
        this.scoreProvidersMap[jsonBook.scoreProviders[i].handle] = jsonBook.scoreProviders[i];
        var actions = jsonBook.scoreProviders[i].actions;
        if (actions)
        {
          for (var j = 0; j < actions.length; j++)
          {
            this.scoreProviderActionsMap[actions[j].id] = actions[j];
            if (actions[j].controlLogic)
            {
                $$('head')[0].appendChild( new Element('script', { type: 'text/javascript', src: actions[j].controlLogic } ) );
            }
          }
        }
      }
    }
    this.gridColorScheme = jsonBook.colorscheme;
    this._buildCategoryNameMap(jsonBook);
    Object.extend(this, jsonBook); // assign json properties to this object
    this._buildGradingPeriodMap();
    this._setStudentInfoLayout();
    this.setCurrentView(this.initialView);
    this._updateVisibleRows(jsonBook);
    this.sortColumns();
    if (this.colDefMap.LN !== undefined)
    {
      this.sort( this.getSortIndex( 'LN' ), 'ASC', this.getSortIndex( 'FN' ) );
    }
    this.lastLogEntryTS = jsonBook.lastLogEntryTS;

    if (!this.usingCachedBook)
    {
      this.initialView = null;
      this.fireModelChanged();
    }
    else
    {
      this.requestUpdateData();
    }
  }
  catch (e2)
  {
    this.fireModelError(e2);
  }
};

// callback when updating this gradebook model with server data
Gradebook.GridModel.prototype._updateDataFromJSON = function(reply)
{
  var jsonBook;
  try
  {
    if (typeof (JSON) === 'object' && typeof (JSON.parse) === 'function')
    {
      jsonBook = JSON.parse(reply.responseText);
    }
    else
    {
      jsonBook = eval('(' + reply.responseText + ')');
    }
  }
  catch (e)
  {
    this.fireModelError(e, reply.responseText);
    return;
  }
  try
  {
    // need to reinitialize if new users added to pick up existing grades
    // when a user is re-enabled
    if (this._hasNewUsers(jsonBook))
    {
      this.requestLoadData(true /*
                                 * force flush since extra users cannot be
                                 * loaded by delta
                                 */);
      return;
    }
    this.version = jsonBook.version;
    this.lastUserChangeTS = jsonBook.lastUserChangeTS;
    this.usersHash = jsonBook.usersHash;
    this.numFrozenColumns = jsonBook.numFrozenColumns;
    this.gradingPeriods = jsonBook.gradingPeriods;
    this.categories = jsonBook.categories;
    this._buildCategoryNameMap(jsonBook);
    this.studentInfoLayouts = jsonBook.studentInfoLayouts;
    this.pubColID = jsonBook.pubColID;
    this.defView = jsonBook.defView;

    var i, len;

    if (jsonBook.schemas)
    {
      for (i = 0; i < jsonBook.schemas.length; i++)
      {
        // create a new schema if one with same id does not already exists
        var schema = this.schemaMap[jsonBook.schemas[i].id];
        if (schema === undefined)
        {
          schema = this._createSchema(jsonBook.schemas[i].type, jsonBook.schemas[i]);
          this.schemaMap[jsonBook.schemas[i].id] = schema;
        }
        else
        {
          Object.extend(schema, jsonBook.schemas[i]);
        }
      }
    }
    if (jsonBook.groups)
    {
      if (!this.groupsMap || !this.groups || this.groups.length === 0)
      {
        this.groupsMap =
          [];
        this.groups = jsonBook.groups;
        for (i = 0; i < jsonBook.groups.length; i++)
        {
          this.groupsMap[jsonBook.groups[i].id] = i;
        }
      }
      else
      {
        for (i = 0; i < jsonBook.groups.length; i++)
        {
          var group = this.groupsMap[jsonBook.groups[i].id];
          if (group === undefined)
          {
            this.groupsMap[jsonBook.groups[i].id] = this.groups.length;
            this.groups.push(jsonBook.groups[i]);
          }
          else
          {
            this.groups[group] = jsonBook.groups[i];
          }
        }
      }
    }

    if (jsonBook.colDefs)
    {
      for (i = 0; i < jsonBook.colDefs.length; i++)
      {
        // create a new colDef if one with same id does not already exists
        var colIndex = this.colDefMap[jsonBook.colDefs[i].id];
        if (!colIndex)
        {
          if (jsonBook.colDefs[i].deleted)
          {
            continue;
          }
          this.colDefMap[jsonBook.colDefs[i].id] = this.colDefs.length;
          this.colDefs.push(this._createColDef(jsonBook.colDefs[i], this, this.schemaMap));
        }
        else
        {
          // we should actually discard the previous version and replace it with
          // the new one
          // however all cells hold a ref to the object making this
          // impractical. With incoming
          // refactoring that should not be an issue since col def will
          // always be looked up by
          // cell index in the row - right now only delete the src since it
          // is omitted from payload
          // when absent
          var colDef = this.colDefs[colIndex];
          colDef.comput_err = false;
          if (colDef.src)
          {
            delete colDef.src;
          }
          Object.extend(colDef, jsonBook.colDefs[i]);
          // clear all grades in column if computation error for column
          if (jsonBook.colDefs[i].comput_err)
          {
            var grades = this._getGradesForItemId(jsonBook.colDefs[i].id, true);
            for ( var g = 0; g < grades.length; g++)
            {
              grades[g].initialize(grades[g].colDef, grades[g].metaData);
            }
          }
          if (colDef.deleted)
          {
            this.colDefMap[colDef.id] = null;
          }
          if (colDef.sid)
          {
            colDef.primarySchema = this.schemaMap[colDef.sid];
          }
          if (colDef.ssid && colDef.ssid.length > 0)
          {
            colDef.secondarySchema = this.schemaMap[colDef.ssid];
          }
          else
          {
            colDef.secondarySchema = null;
          }
        }
      }
    }
    // need to add any new row data?
    if (this.rows && this.rows.length > 0)
    {
      var numNewCols = this.colDefs.length - this.rows[0].length;
      if (this.rows.length > 0 && numNewCols > 0)
      {
        for (i = 0; i < this.rows.length; i++)
        {
          for ( var c = 0; c < numNewCols; c++)
          {
            this.rows[i].push(
            {}); // add empty cell for each new column
          }
        }
      }
    }

    var tempArray;

    if (jsonBook.rows)
    {
      // users changed, need to resync
      if (jsonBook.type == "delta_with_user")
      {
        // remove rows from model that are not in json data
        tempArray = [];
        for (i = 0; i < this.rows.length; i++)
        {
          if (this._containsUser(jsonBook.rows, this.rows[i][0].uid))
          {
            tempArray.push(this.rows[i]);
          }
        }
        this.rows = tempArray;
      }
      this.rowUserIdMap = [];
      for (i = 0, len = this.rows.length; i < len; i++)
      {
        this.rowUserIdMap[this.rows[i][0].uid] = i;
      }

      // update rows
      for (i = 0; i < jsonBook.rows.length; i++)
      {
        var row = this.getRowByUserId(jsonBook.rows[i][0].uid);
        if (!row)
        {
          GradebookUtil.error('Can not update non-existing row for user id: ' + jsonBook.rows[i][0].uid);
        }
        else
        {
          this._updateRowDataFromJSON(row, jsonBook.rows[i], this.colDefs, this.colDefMap);
        }
      }
    }
    this._buildGradingPeriodMap();
    if (jsonBook.customViews)
    {
      for (i = 0; i < jsonBook.customViews.length; i++)
      {
        // create a new custom view if one with same id does not already exists
        var idx = this.customViewMap[jsonBook.customViews[i].id];
        if (idx === undefined)
        {
          this.customViewMap[jsonBook.customViews[i].id] = this.customViews.length;
          this.customViews.push(new Gradebook.CustomView(jsonBook.customViews[i], this));
        }
        else
        {
          this.customViews[idx] = new Gradebook.CustomView(jsonBook.customViews[i], this);
        }
      }
    }
    if ( jsonBook.colorscheme )
    {
      this.gridColorScheme = jsonBook.colorscheme;
    }
    // remove any custom views not in customViewIds
    if (this.customViews)
    {
      tempArray = [];
      this.customViewMap = [];
      for (i = 0; i < this.customViews.length; i++)
      {
        if (jsonBook.customViewIds.indexOf(Number(this.customViews[i].id)) != -1)
        {
          this.customViewMap[this.customViews[i].id] = tempArray.length;
          tempArray.push(this.customViews[i]);
        }
      }
      this.customViews = tempArray;
    }
    this._setStudentInfoLayout();
    if (this.initialView || this.usingCachedBook)
    {
      this.setCurrentView(this.initialView);
      this.initialView = null;
    }
    this.lastLogEntryTS = jsonBook.lastLogEntryTS;
    this._updateVisibleRows(jsonBook);
    this.sortColumns();
    this.reSort();
    this.usingCachedBook = false;
    this.checkedNoStudents(); // do this last, it will fireModelChanged
  }
  catch (e2)
  {
    this.fireModelError(e2);
  }
};

Gradebook.GridModel.prototype._updateRowDataFromJSON = function(thisRow, jsonRow, colDefs, colDefMap)
{

  for ( var i = 0; i < jsonRow.length; i++)
  {
    var colIndex = colDefMap[jsonRow[i].c];
    colDefs[colIndex].comput_err = false;
    var currentCell = thisRow[colIndex];
    // reset any property that is not always part of the cell data
    if (currentCell.mp)
    {
      delete currentCell.mp;
    }
    if (currentCell.x)
    {
      delete currentCell.x;
    }
    if (currentCell.ax)
    {
      delete currentCell.ax;
    }
    if (currentCell.excluded)
    {
      delete currentCell.excluded;
    }
    if (currentCell.numAtt)
    {
      delete currentCell.numAtt;
    }
    if (currentCell.or)
    {
      delete currentCell.or;
    }
    if (currentCell.orBefAtt)
    {
      delete currentCell.orBefAtt;
    }
    if (currentCell.ip)
    {
      delete currentCell.ip;
    }
    if (currentCell.ng)
    {
      delete currentCell.ng;
    }
    if (currentCell.na)
    {
      delete currentCell.na;
    }
    delete currentCell.attemptsInfo;
    Object.extend(currentCell, jsonRow[i]);
  }
  thisRow[0].isAvailable = thisRow[0].avail;
};

Gradebook.GridModel.prototype._createColDef = function(jsonColDef, model, schemaMap)
{
  if (jsonColDef.type == "s")
  {
    return new Gradebook.StudentAttributeColDef(jsonColDef, model, schemaMap);
  }
  else
  {
    return new Gradebook.GradeColDef(jsonColDef, model, schemaMap);
  }
};

Gradebook.GridModel.prototype._createSchema = function(type, jsonSchema)
{
  if (type == "S")
  {
    return new Gradebook.NumericSchema(jsonSchema, this);
  }
  else if (type == "X")
  {
    return new Gradebook.TextSchema(jsonSchema, this);
  }
  else if (type == "P")
  {
    return new Gradebook.PercentageSchema(jsonSchema, this);
  }
  else if (type == "C")
  {
    return new Gradebook.CompleteIncompleteSchema(jsonSchema, this);
  }
  else if (type == "T")
  {
    return new Gradebook.LetterSchema(jsonSchema, this);
  }
  else
  {
    return null;
  }

};

Gradebook.GridModel.prototype._buildGradingPeriodMap = function()
{
  this.gradingPeriodMap =
    [];
  if (this.gradingPeriods)
  {
    for ( var i = 0, len = this.gradingPeriods.length; i < len; i++)
    {
      this.gradingPeriodMap[this.gradingPeriods[i].id] = this.gradingPeriods[i];
    }
    this.gradingPeriods.sort(function(a, b)
    {
      var aa = a.name.toLowerCase();
      var bb = b.name.toLowerCase();
      if (aa == bb)
      {
        return 0;
      }
      else if (aa < bb)
      {
        return -1;
      }
      else
      {
        return 1;
      }
    });
  }
};

Gradebook.GridModel.prototype._setStudentInfoLayout = function()
{
  // set pos & gbvis for student attribute columns from studentInfoLayouts
  for ( var i = 0; i < this.studentInfoLayouts.length; i++)
  {
    var colIndex = this.colDefMap[this.studentInfoLayouts[i].id];
    if (colIndex === undefined)
    {
      continue;
    }
    var colDef = this.colDefs[colIndex];
    colDef.gbvis = this.studentInfoLayouts[i].gbvis;
    colDef.pos = this.studentInfoLayouts[i].pos;
  }
};

Gradebook.GridModel.prototype._updateVisibleRows = function(jsonBook)
{
  var showAll = (!jsonBook.hiddenStudentIds || jsonBook.hiddenStudentIds.length === 0);
  this.visibleRows =
    [];
  var rows = this.rows;
  // loop through rows and set hidden flag for each row, add to visibleRows
  // if not hidden
  for ( var i = 0, len = rows.length; i < len; i++)
  {
    var row = rows[i];
    var isHidden = !showAll && (jsonBook.hiddenStudentIds.indexOf(Number(row[0].uid)) != -1);
    row[0].isHidden = isHidden;
    if (!isHidden)
    {
      this.visibleRows.push(row);
    }
  }
  this._applyCustomView();

};

Gradebook.GridModel.prototype._buildCategoryNameMap = function(jsonBook)
{
  this.catNameMap =
    [];
  if (jsonBook.categories)
  {
    for ( var i = 0; i < jsonBook.categories.length; i++)
    {
      this.catNameMap[jsonBook.categories[i].id] = jsonBook.categories[i].name;
    }
  }
};

Gradebook.GridModel.prototype._initMessages = function()
{
  if (this.messages)
  {
    return;
  }
  this.loadingLocalizedMessages = true;
  this.gradebookService.requestLoadMessages((this._onMessageLoaded).bind(this), (this._reportError).bind(this), (this._reportException).bind(this));
};

Gradebook.GridModel.prototype._onMessageLoaded = function(reply)
{
  var messagesJSON = eval('(' + reply.responseText + ')');
  this.messages = messagesJSON.gradebook2Messages;
  delete this.loadingLocalizedMessages;
};
