/**
 *  Gradebook data grid
 *
 *  PORTIONS OF THIS FILE ARE BASED ON RICO LIVEGRID 1.1.2
 *
 *  Copyright 2005 Sabre Airline Solutions
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 *  file except in compliance with the License. You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the
 *  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *  @author "Bill Richard"
 *  @version
 *
 *
 **/

Gradebook.GridModel = Class.create();

Gradebook.GridModel.prototype =
{

  initialize : function(gradebookService)
  {
    this.gradebookService = gradebookService;
    this.courseId = gradebookService.courseId;
    this.rows = [];
    this.colDefs = [];
    this.colOrderMap = [];
    this.customViews = [];
    this.listeners = [];
    this.accessibleMode = false;
    this.resizingWindow = false;
    this.minimumRows = 10;
    this.isolatedStudentId = '';
    this.floatLocaleFormat= null;
    // singleton on this document
    this._initMessages();
    this.gridColorScheme = null;
    // stuff that needs to be stored to survive page refresh but not used by the model
    this.store = {};
    window.gbModel = this;
  },
  
  getFloatLocaleFormat: function()
  {
    return this.floatLocaleFormat;
  },
  
  setFloatLocaleFormat: function( floatLocaleFormat )
  {
    this.floatLocaleFormat = floatLocaleFormat;
    NumberFormatter.needToConvert = ( this.floatLocaleFormat.separator == ',' );
  },
  
  getNumberFormatter: function()
  {
    return NumberFormatter;
  },
  
  getObject: function( name )
  {
    return this.store[ name ];
  },
  
  newObject: function( name )
  {
    var newObject = {};
    this.store[ name ] = newObject;
    return newObject;
  },

  setObject: function( name, object )
  {
    this.store[ name ] = object;
    return object;
  },
  
  removeObject: function( name )
  {
    delete this.store[ name ];
  },
  
  newArray: function( name )
  {
    this[ name ] = [];
    return this[ name ];
  },

  getCustomViews : function()
  {
    this.customViews.sort(function(a, b)
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
    return this.customViews;
  },

  // notify registered listeners that model data has changed
  fireModelChanged : function()
  {
    if (!this.messages && this.loadingLocalizedMessages)
    {
      // wait for the messages to be loaded before to do a reload
      window.setTimeout(this.fireModelChanged.bind(this), 50);
    }
    else
    {
      for ( var i = 0; i < this.listeners.length; i++)
      {
        this.listeners[i].modelChanged();
      }
    }
  },

  // notify registered listeners that model error has occured
  fireModelError : function(exception, serverReply)
  {
    for ( var i = 0; i < this.listeners.length; i++)
    {
      if (this.listeners[i].modelError)
      {
        this.listeners[i].modelError(exception, serverReply);
      }
    }
  },

  addModelListener : function(listener)
  {
    this.listeners.push(listener);
  },

  removeModelListeners : function()
  {
    this.listeners =
      [];
  },

  updateGrade : function(newValue, newTextValue, userId, colDefId)
  {
    this.gradebookService.updateGrade((this.updateGradeCallback).bind(this), this.version, newValue, newTextValue, userId, colDefId);
  },

  clearAll : function(isDelete, userId, colDefId)
  {
    this.gradebookService.clearAll((this.updateGradeCallback).bind(this), this.version, isDelete, userId, colDefId);
  },

  clearSelected : function(attemptIds, isDelete, userId, colDefId)
  {
    this.gradebookService.clearSelected((this.updateGradeCallback).bind(this), this.version, attemptIds, isDelete, userId, colDefId);
  },

  deleteColumn : function(colDefId)
  {
    this.gradebookService.deleteColumn(colDefId);
  },

  modifyColumn : function(colDefId, colType)
  {
    this.gradebookService.modifyColumn(colDefId, colType);
  },

  viewItemStats : function(itemId)
  {
    this.gradebookService.viewItemStats(itemId);
  },

  viewSingleStudentGrades : function(userId)
  {
    this.isolatedStudentId = userId;
    this.gradebookService.reloadGrid();
  },

  restoreFromSingleStudentView : function()
  {
    this.isolatedStudentId = '';
    this.gradebookService.reloadGrid();
  },

  viewStudentStats : function(userId)
  {
    this.gradebookService.viewStudentStats(userId);
  },

  viewAdaptiveRelease : function(userName)
  {
    this.gradebookService.viewAdaptiveRelease(userName);
  },

  hideColumn : function(colDefId)
  {
    // decrement numFrozenColumns if hiding a frozen column
    var idx = this.colDefMap[colDefId];
    for ( var i = 0; i < this.colOrderMap.length; i++)
    {
      if (this.colOrderMap[i] == idx)
      {
        if (i < this.numFrozenColumns && this.numFrozenColumns > 1)
        {
          this.numFrozenColumns--;
          this.gradebookService.updateNumFrozenColumns(this.numFrozenColumns);
        }
        break;
      }
    }
    this.gradebookService.hideColumn(colDefId);
  },

  setColumnStudentVisibility : function(colDefId, visible)
  {
    this.gradebookService.setColumnStudentVisibility((this.setColumnStudentVisibilityCallback).bind(this), colDefId, visible);
  },

  showGradeDetails : function(userId, colDefId)
  {
    this.gradebookService.showGradeDetails(userId, colDefId);
  },

  onAddComment : function(userId, colDefId)
  {
    this.gradebookService.loadComments(userId, colDefId, "studentComments", "instructorComments");
  },

  exemptGrade : function(userId, colDefId)
  {
    this.gradebookService.setExemption((this.updateGradeCallback).bind(this), this.version, userId, colDefId, true);
  },

  clearExemption : function(userId, colDefId)
  {
    this.gradebookService.setExemption((this.updateGradeCallback).bind(this), this.version, userId, colDefId, false);
  },

  setComments : function(userId, colDefId, studentComments, instructorComments)
  {
    this.gradebookService.setComments(userId, colDefId, studentComments, instructorComments);
  },

  getRowByUserId : function(userId)
  {
    var rowIndex = this.rowUserIdMap[userId];
    if ( rowIndex === undefined || this.rows[rowIndex][0].uid != userId)
    {
      return null;
    }
    return this.rows[rowIndex];
  },

  _getGradesForItemId : function(itemId, includeUnavailable)
  {
    var grades = [];
    var colIndex = this.colDefMap[itemId];
    if (!colIndex)
    {
      GradebookUtil.error('GridModel _getGradesForItemId contains data for invalid column id: ' + itemId);
      return grades;
    }
    var rows = (includeUnavailable) ? this.rows : this.visibleRows;
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      var data = rows[i][colIndex];
      if (!data.metaData)
      {
        data.metaData = rows[i][0];
      }
      if (includeUnavailable || data.metaData.isAvailable)
      {
        if (!data.colDef)
        {
          data.colDef = this.colDefs[colIndex];
        }
        grades.push(new Gradebook.GridCell(data));
      }
    }
    return grades;
  },

  getColDefById : function(itemId)
  {
    return this.colDefs[this.colDefMap[itemId]];
  },

  setColumnStudentVisibilityCallback : function(retData)
  {
    if (!retData)
    {
      GradebookUtil.error('GridModel error updating column visibility');
      return;
    }
    this.getColDefById(retData.columnId).vis = retData.vis;
    this.fireModelChanged();
  },

  updateGradeCallback : function(retData)
  {
    if (!retData || retData.length === 0)
    {
      GradebookUtil.error('GridModel error updating grade');
      return;
    }
    var lastSavedDate = null;
    for ( var i = 0, len = retData.length; i < len; i++)
    {
      var data = retData[i];
      var colDefId = data.itemId;
      var userId = data.courseUserId;
      var score = data.score;
      var textInput = data.textInput;
      var row = this.getRowByUserId(userId);
      var colIndex = this.colDefMap[colDefId];
      if (!colIndex)
      {
        // ignore
        continue;
      }
      var gridCell = row[colIndex];
      gridCell.tv = textInput;
      if (textInput.length === 0 && score === 0)
      {
        gridCell.v = '-';
      }
      else
      {
        gridCell.v = score;
      }
      gridCell.or = (data.override) ? "y" : null;
      gridCell.x = (data.exempt) ? "y" : null;
      gridCell.ng = (data.needsGrading) ? "y" : null;
      gridCell.ip = (data.inProgress) ? "y" : null;
      gridCell.notExcluded = !data.excluded;
      gridCell.mp = data.points;
      gridCell.attemptsInfo = null;
      gridCell.numAtt = data.numOfAttempts; 
      if ( lastSavedDate === null )
      {
        lastSavedDate = data.lastSavedDate;
      }
      gridCell.orBefAtt = data.overrideBeforeAttempt;
    }
    this.lastLogEntryTS = lastSavedDate;
    this.fireModelChanged();
  },

  setResizingWindow : function(f)
  {
    this.resizingWindow = f;
  },

  getResizingWindow : function()
  {
    return this.resizingWindow;
  },

  setMinimumRows : function(minRows)
  {
    if (minRows < 5)
    {
      minRows = 5;
    }
    if (minRows > 50)
    {
      minRows = 50;
    }
    this.minimumRows = minRows;
  },

  getMinimumRows : function()
  {
    return this.minimumRows;
  },
  
  getColorScheme: function( gradeCell )
  {
    if ( gradeCell.needsGrading() )
    {     
      return "cs_ng";
    }
    if ( gradeCell.attemptInProgress() )
    {
      return "cs_ip";
    }
    if ( gradeCell.isExempt() )
    {
      return "cs_ex";
    }
    var colorScheme = this.gridColorScheme;
    if ( colorScheme && gradeCell.isGraded() )
    {
      var normalizedPoints =  gradeCell.getNormalizedGrade();
      if ( normalizedPoints !== null )
      {
        normalizedPoints = normalizedPoints * 100;
        for ( var i = 0; i < colorScheme.length; ++i )
        {
          var range = colorScheme[ i ];
          if ( range.u && normalizedPoints > range.u )
          {
            continue;
          }
          if ( range.l && normalizedPoints < range.l )
          {
            continue;
          }
          return range.cid;
        }
      }
    }
    return "";
  },
  

  _reportException : function(e)
  {
    this.fireModelError(e, e.message);
  },

  _reportError : function(t)
  {
    this.fireModelError('error getting data from server', t.statusText);
  },

  getVisibleColDefIndex : function(id)
  {
    var colnum = this.colDefMap[id];
    if (colnum === undefined || this.colDefs[colnum] === undefined || !this.colDefs[colnum].gbvis)
    {
      return -1;
    }
    for ( var i = 0, len = this.colOrderMap.length; i < len; i++)
    {
      if (this.colOrderMap[i] == colnum)
      {
        return i;
      }
    }
    return -1;
  },

  updateUserVisibility : function(userId, visible)
  {
    this.gradebookService.updateUserVisibility(userId, visible);
  },

  _hasNewUsers : function(jsonBook)
  {
    if (!jsonBook || !jsonBook.rows)
    {
      return false;
    }
    for ( var i = 0; i < jsonBook.rows.length; i++)
    {
      if (!this.getRowByUserId(jsonBook.rows[i][0].uid))
      {
        return true;
      }
    }
    return false;
  },

  _containsUser : function(rows, userId)
  {
    for ( var i = 0; i < rows.length; i++)
    {
      if (rows[i][0].uid == userId)
      {
        return true;
      }
    }
    return false;
  },

  // called by view to get a window of row data
  // returns iterators to get row data in correct order while skipping hidden columns
  getRowIterators : function(startRow, numRows, startCol)
  {
    var rows = this.visibleRows;
    if (!startRow)
    {
      startRow = 0;
    }
    if (!startCol)
    {
      startCol = 0;
    }
    if (!numRows)
    {
      numRows = rows.length;
    }
    var endRow = startRow + numRows;

    if (startRow < 0 || startRow >= rows.length)
    {
      GradebookUtil.error('getRowIterators startRow out of range. Max is: ' + rows.length - 1 + ' startRow is: ' + startRow);
      return null;
    }
    if (numRows < 0 || numRows > rows.length)
    {
      GradebookUtil.error('getRowIterators numRows out of range. Max is: ' + rows.length + ' numRows is: ' + numRows);
      return null;
    }
    if (startCol < 0 || startCol >= this.colOrderMap.length)
    {
      GradebookUtil.error('getRowIterators startCol out of range. Max is: ' + this.colOrderMap.length + ' startCol is: ' + startCol);
      return null;
    }
    if (endRow > rows.length)
    {
      endRow = rows.length;
      GradebookUtil.error('Error: GridModel getRowIterators input args requesting too much data. startRow = ' + startRow + ' numRows = ' + numRows + 
          ' rows.length = ' + rows.length);
      return null;
    }

    var results =
      [];
    var index = 0;
    for ( var i = startRow; i < endRow; i++)
    {
      results[index++] = new Gradebook.GridRowIterator(rows[i], this.colOrderMap, startCol, this.colDefs);
    }
    return results;
  },

  // called by view to get the column definitions
  // returns iterator to get definitions in correct order while skipping hidden columns
  getColDefIterator : function(startCol)
  {
    if (!startCol)
    {
      startCol = 0;
    }
    if (startCol < 0 || startCol >= this.colOrderMap.length)
    {
      GradebookUtil.error('getColDefIterator startCol out of range. Max is: ' + this.colOrderMap.length + ' startCol is: ' + startCol);
      return null;
    }
    return new Gradebook.ColDefIterator(this.colDefs, this.colOrderMap, startCol);
  },

  // called by view to determine how much vertical scroll is needed
  getNumRows : function()
  {
    if (this.visibleRows)
    {
      return this.visibleRows.length;
    }
    else
    {
      return 0;
    }
  },

  // called by view to determine how much horizontal scroll is needed
  getNumColDefs : function()
  {
    return this.colOrderMap.length;
  },

  // called by view to determine how many columns to freeze
  getNumFrozenColumns : function()
  {
    return this.numFrozenColumns;
  },

  getSortDir : function()
  {
    return this.sortDir;
  },

  // columnId is optional, it will return the sort index currently used if not speficied
  getSortIndex : function( columnId  )
  {
    var sortColumnId = columnId?columnId:this.sortColumnId;
    if ( !sortColumnId )
    {
      return -1;
    }
    var colnum = this.colDefMap[ sortColumnId ];
    if (colnum === undefined)
    {
      return -1;
    }
    else
    {
      var sortColumn = this.colOrderMap[ colnum ];
      if (sortColumn === undefined || this.colDefs[ sortColumn ] === undefined || this.colDefs[ sortColumn ].deleted == "Y")
      {
        return -1;
      }
      else
      {
        return colnum;
      }
    }
  },

  reSort : function()
  {
    if (this.sortColumnId === undefined || this.sortDir === undefined)
    {
      return;
    }
    var colnum = this.getSortIndex();
    if (colnum == -1)
    {
      return;
    }
    this.sort(colnum, this.sortDir);
  },

  setDefaultView : function(view)
  {
    this.defView = view;
    this.gradebookService.setDefaultView(view);
  },

  getDefaultView : function()
  {
    if (!this._isValidView(this.defView))
    {
      this.defView = 'fullGC';
    }
    return this.defView;
  },

  setCategoryFilter : function(category)
  {
    this.categoryFilter = category; // override category for current view
  },
  
  setStatusFilter : function(status)
  {
    if (status.startsWith("stat_"))
    {
      status = status.substr(5, status.length - 5);
    }
    this.statusFilter = status; // override status for current view
  },
  
  setInitialCurrentView : function(view)
  {
    this.initialView = view;
  },
  
  // set the current view to a fullGC, custom view, or grading period
  //   view param is:
  //     'fullGC' for full
  //     'cv_123' for custom views
  //     '456' for grading periods
  // if specified view is invalid, use default, if default is invalid, use full
  //
  setCurrentView : function(view)
  {

    this.categoryFilter = null; // clear category override
    this.statusFilter = null; // clear status override
    this.currentCustomView = null;
    this.currentGradingPeriodId = null;

    if (!this._isValidView(view))
    {
      view = this.defView;
    }
    if (!this._isValidView(view))
    {
      view = 'fullGC';
    }

    if (view == 'fullGC')
    {
      // use a custom view for full grade center to allow category/status overrides
      this.currentCustomView = Gradebook.CustomView.getFullGC(this);
    }
    else if (view.startsWith('cv_'))
    {
      var idx = this.customViewMap[view.substring(3)];
      this.currentCustomView = this.customViews[idx];
    }
    else if (view.startsWith('gp_'))
    {
      this.currentGradingPeriodId = view.substring(3);
    }
    this.currentView = view;
  },

  _isValidView : function(view)
  {
    if (!view)
    {
      return false;
    }
    if (view == 'fullGC')
    {
      return true;
    }
    if (view.startsWith('cv_') && this.customViewMap[ view.substring( 3 ) ] !== 'undefined' && this.customViewMap[ view.substring( 3 ) ] !== null )
    {
      var idx = this.customViewMap[view.substring(3)];
      return this.customViews[idx].evaluate();
    }
    if (view.startsWith('gp_'))
    {
      return this.gradingPeriodMap && this.gradingPeriodMap[view.substring(3)];
    }
    return false;
  },

  _applyCustomView : function()
  {
    var row;
    if (this.isolatedStudentId)
    {
      this.visibleRows =
        [];
      row = this.getRowByUserId(this.isolatedStudentId);
      this.visibleRows.push(row);
    }
    if (!this.currentCustomView)
    {
      return;
    }
    this.currentCustomView.evaluate(this);
    var userIds = this.currentCustomView.getUserIds();
    if (this.isolatedStudentId)
    {
      return;
    }
    this.visibleRows =
      [];
    // loop through custom view users and add to visibleRows
    for ( var i = 0, len = userIds.length; i < len; i++)
    {
      row = this.getRowByUserId(userIds[i]);
      if (row)
      {
        this.visibleRows.push(row);
      }
    }
  },
  
  getCustomView : function(cvId)
  {
    var idx = this.customViewMap[cvId];
    if (!idx)
    {
      return null;
    }
    else
    {
      return this.customViews[idx];
    }
  },
  
  getCurrentCustomView : function()
  {
    return this.currentCustomView;
  },
  
  getCurrentStatus : function()
  {
    if (!this.isStatusView())
    {
      return 'stat_ALL';
    }
    else if (this.statusFilter)
    {
      return this.statusFilter;
    }
    else
    {
      return this.currentCustomView.display.items;
    }
  },
  
  getCurrentCategory : function()
  {
    if (!this.isStatusView())
    {
      return 'c_all';
    }
    else if (this.categoryFilter)
    {
      return this.categoryFilter;
    }
    else if (this.currentCustomView.category == 'c_all')
    {
      return 'c_all';
    }
    else
    {
      return 'c_' + this.currentCustomView.aliasMap[this.currentCustomView.category];
    }
  },
  
  isStatusView : function()
  {
    return this.currentCustomView && this.currentCustomView.searchType == 'status';
  },
  
  getCurrentViewName : function()
  {
    if (this.currentCustomView)
    {
      return this.currentCustomView.name;
    }
    else if (this.currentGradingPeriodId)
    {
      return this.gradingPeriodMap[this.currentGradingPeriodId].name;
    }
    else
    {
      return "";
    }
  },
  
  getCurrentViewStatus : function()
  {
    var s = this.getCurrentStatus();
    if (s.startsWith("stat_"))
    {
      s = s.substr(5, status.length - 5);
    }
    if (s == "ALL")
    {
      return this.getMessage('all_statusesMsg');
    }
    else if (s == "NA")
    {
      return this.getMessage('not_attemptedMsg');
    }
    else if (s == "C")
    {
      return this.getMessage('completedMsg');
    }
    else if (s == "NG")
    {
      return this.getMessage('needs_gradingMsg');
    }
    else if (s == "IP")
    {
      return this.getMessage('in_progressMsg');
    }
    else if (s == "EM")
    {
      return this.getMessage('edited_manuallyMsg');
    }
  },
  
  sortColumns : function(sortBy)
  {
    if (!this.sortColAscending)
    {
      this.sortColAscending = true;
    }
  
    if (sortBy)
    {
      if (this.currentSortColumnBy == sortBy)
      {
        this.sortColAscending = !this.sortColAscending;
      }
      else
      {
        this.sortColAscending = true;
        this.currentSortColumnBy = sortBy;
      }
    }
    else if (!this.currentSortColumnBy)
    {
      this.currentSortColumnBy = 'pos';
    }
    var sortFunc = null;
    sortBy = this.currentSortColumnBy;
    if (sortBy == 'pos')
    {
      sortFunc = this._sortColByPosFunc.bind(this);
    }
    else if (sortBy == 'categories')
    {
      sortFunc = this._sortColByCategoriesFunc.bind(this);
    }
    else if (sortBy == 'dueDate')
    {
      sortFunc = this._sortColByDueDateFunc.bind(this);
    }
    else if (sortBy == 'creationdate')
    {
      sortFunc = this._sortColByCreationDateFunc.bind(this);
    }
    else if (sortBy == 'points')
    {
      sortFunc = this._sortColByPointsFunc.bind(this);
    }
    else if (sortBy == 'name')
    {
      sortFunc = this._sortColByNameFunc.bind(this);
    }
  
    var tempColDefs =
      [];
  
    var i, cd, len, idx;
  
    if (this.currentCustomView)
    {
      var colIds = this.currentCustomView.getDisplayItemIds();
      tempColDefs = this._getVisibleToAll(this.currentCustomView.includeHiddenItems, colIds);
      for (i = 0, len = colIds.length; i < len; i++)
      {
        cd = this.colDefs[this.colDefMap[colIds[i]]];
        tempColDefs.push(cd);
      }
    }
    else
    {
      // filter out colDefs that are: deleted, hidden, not in all grading periods
    // or not in current grading period
    for (i = 0, len = this.colDefs.length; i < len; i++)
    {
      cd = this.colDefs[i];
      if (cd.deleted || !cd.gbvis)
      {
        continue;
      }
      var cgp = this.currentGradingPeriodId;
      var ingp = (!cgp || cgp == cd.gpid || cgp == 'all' || (cgp == 'none' && !cd.gpid));
      if (cd.visAll || !cd.isGrade() || ingp)
      {
        tempColDefs.push(cd);
      }
    }
  }
  tempColDefs.sort(sortFunc);
  
  // compute colOrderMap based on the sorted columns
    this.colOrderMap =
      [];
    for (i = 0, len = tempColDefs.length, idx = 0; i < len; i++)
    {
      this.colOrderMap[idx++] = this.colDefMap[tempColDefs[i].id];
    }
  },
  
  _getVisibleToAll : function(includeHidden, excludeIds)
  {
    var tempColDefs =
      [];
    for ( var i = 0, len = this.colDefs.length; i < len; i++)
    {
      var cd = this.colDefs[i];
      if (excludeIds.indexOf(cd.id) != -1)
      {
        continue;
      }
      var visAll = cd.visAll || !cd.isGrade();
      if (cd.deleted || !visAll || (!includeHidden && !cd.gbvis))
      {
        continue;
      }
      tempColDefs.push(cd);
    }
    return tempColDefs;
  },
  
  // if both a & b are NOT visible to all, returns null
  // if both a & b are visible to all, sorts by position
  // if a is visible to all, returns -1 so visible to all columns come first
  // if b is visible to all, returns 1 so visible to all columns come first
  _sortVisibleToAll : function(a, b)
  {
    var aVisAll = a.visAll || !a.isGrade();
    var bVisAll = b.visAll || !b.isGrade();
    if (!aVisAll && !bVisAll)
    {
      return null;
    }
    else if (aVisAll && bVisAll)
    {
      return a.pos - b.pos;
    }
    else if (aVisAll)
    {
      return -1;
    }
    else if (bVisAll)
    {
      return 1;
    }
  },

  _sortColDir : function(result)
  {
    return (this.sortColAscending) ? result : result * -1;
  },

  _sortColByPosFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var gpPosA = (a.gpid.blank()) ? -1 : this.gradingPeriodMap[a.gpid].pos;
    var gpPosB = (b.gpid.blank()) ? -1 : this.gradingPeriodMap[b.gpid].pos;
    var res;
    if (gpPosA == gpPosB)
    {
      res = a.pos - b.pos;
    }
    else if (gpPosA >= 0 && gpPosB >= 0)
    {
      res = gpPosA - gpPosB;
    }
    else if (gpPosB == -1)
    {
      res = -1;
    }
    else
    {
      res = 1;
    }
    return this._sortColDir(res);
  },

  _sortColByPointsFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var aa = a.points;
    var bb = b.points;
    var res;
    if (aa == bb)
    {
      res = a.cdate - b.cdate;
    }
    else if (aa < bb)
    {
      res = -1;
    }
    else
    {
      res = 1;
    }
    return this._sortColDir(res);
  },

  _sortColByNameFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var aa = a.name.toLocaleLowerCase();
    var bb = b.name.toLocaleLowerCase();
    var res;
    if (aa == bb)
    {
      res = a.cdate - b.cdate;
    }
    else if (aa < bb)
    {
      res = -1;
    }
    else
    {
      res = 1;
    }
    return this._sortColDir(res);
  },

  _sortColByDueDateFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var aa = a.due;
    var bb = b.due;
    var res;
    if (aa == bb)
    {
      res = a.cdate - b.cdate;
    }
    else if (aa === 0)
    {
      res = 1; // items with no due date, appear after items with due date
    }
    else if (bb === 0)
    {
      res = -1; // items with no due date, appear after items with due date
    }
    else if (aa < bb)
    {
      res = -1;
    }
    else
    {
      res = 1;
    }
    return this._sortColDir(res);
  },
  
  _sortColByCreationDateFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var res = a.cdate - b.cdate;
    return this._sortColDir(res);
  },
  
  _sortColByCategoriesFunc : function(a, b)
  {
    var sf = this._sortVisibleToAll(a, b);
    if (sf)
    {
      return sf;
    }
    var aa = a.getCategory();
    var bb = b.getCategory();
    var res;
    if (aa == bb)
    {
      res = a.cdate - b.cdate;
    }
    else if (aa < bb)
    {
      res = -1;
    }
    else
    {
      res = 1;
    }
    return this._sortColDir(res);
  },
  
  sort : function(colnum, sortdir, secondaryColumnId )
  {
    if (colnum < -1 || colnum >= this.colOrderMap.length)
    {
      GradebookUtil.error('sort colnum out of range. Max is: ' + this.colOrderMap.length + ' colnum is: ' + colnum);
      return;
    }
    this.sortDir = sortdir;
    var sortFunc;
    if (colnum == -1)
    {
      this.sortColumnId = null;
      if (sortdir == 'ASC')
      {
        sortFunc = this._sortCheckedASC.bind(this);
      }
      else
      {
        sortFunc = this._sortCheckedDESC.bind(this);
      }
    }
    else
    {
      var sortColumn = this.colOrderMap[colnum];
      var colDef = this.colDefs[sortColumn];
      this.sortColumnId = colDef.id;
      if(!secondaryColumnId) 
      {
        if ( this.sortColumnId == "LN" )
        {
          secondaryColumnId = this.colDefMap[ "FN" ];
        }
        else if ( this.sortColumnId == "FN" )
        {
          secondaryColumnId = this.colDefMap[ "LN" ];
        }
      }
      sortFunc = colDef.getSortFunction( sortdir, secondaryColumnId?this.colDefs[ secondaryColumnId ]:null );
    }
    this.visibleRows.sort(sortFunc);
  },
  
  _sortCheckedASC : function(a, b)
  {
    var aa = a[0].isRowChecked ? 1 : 0;
    var bb = b[0].isRowChecked ? 1 : 0;
    if (aa == bb)
    {
      return 0;
    }
    if (aa < bb)
    {
      return -1;
    }
    return 1;
  },
  
  _sortCheckedDESC : function(a, b)
  {
    var aa = a[0].isRowChecked ? 1 : 0;
    var bb = b[0].isRowChecked ? 1 : 0;
    if (aa == bb)
    {
      return 0;
    }
    if (bb < aa)
    {
      return -1;
    }
    return 1;
  },
  
  // called by cumultive item authoring
  // returns gradable items
  getColDefs : function(gradableOnly, includeHidden)
  {
    var colDefs = this.colDefs;
    var retColDefs =
      [];
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      var c = colDefs[i];
      if (!c.deleted && (!gradableOnly || c.isGrade()) && (includeHidden || !c.isHidden()))
      {
        retColDefs.push(c);
      }
    }
    return retColDefs;
  },

  // called by grade detail page and report page
  getCurrentColDefs : function(includeCalculated)
  {
    var colDefs = this.colDefs;
    var retColDefs =
      [];
    for ( var i = 0, len = this.colOrderMap.length; i < len; i++)
    {
      var c = colDefs[this.colOrderMap[i]];
      if (c.isGrade() && (includeCalculated || !c.isCalculated()))
      {
        retColDefs.push(c);
      }
    }
    return retColDefs;
  },

  // called by grade detail page
  getNextColDefId : function(colDefs, colDefId)
  {
    for ( var i = 0; i < colDefs.length - 1; i++)
    {
      if (colDefs[i].getID() == colDefId)
      {
        return colDefs[i + 1].getID();
      }
    }
    return null;
  },

  // called by grade detail page
  getPrevColDefId : function(colDefs, colDefId)
  {
    for ( var i = 1; i < colDefs.length; i++)
    {
      if (colDefs[i].getID() == colDefId)
      {
        return colDefs[i - 1].getID();
      }
    }
    return null;
  },

  // called by grade detail page
  getStudents : function(includeHidden)
  {
    var rows = (includeHidden) ? this.rows : this.visibleRows;
    var students = [];
    var LAST_NAME_COL_IDX = 0;
    var FIRST_NAME_COL_IDX = 1;
    var USER_NAME_COL_IDX = 2;
    if (rows)
    {
      //TODO:Lance doesn't like this because we should really be working on the students
    	// in the order they are already sorted in the UI.  This is the new GC pattern (i.e.
    	// see needs-grading where the next/prev grading follows the sort you started with).
    	// We should remove this sort so that the same thing applies to all grading from the gc grid
      rows.sort(function(a, b)
      {
        var aa = a[LAST_NAME_COL_IDX].v;
        var bb = b[LAST_NAME_COL_IDX].v;
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
      for ( var i = 0; i < rows.length; i++)
      {
        var s = {};
        var row = rows[i];
        s.last = row[LAST_NAME_COL_IDX].v;
        s.sortval = row[LAST_NAME_COL_IDX].sortval;
        s.first = row[FIRST_NAME_COL_IDX].v;
        s.user = row[USER_NAME_COL_IDX].v;
        s.id = row[0].uid;
        s.hidden = row[0].isHidden;
        s.available = row[0].isAvailable;
        students.push(s);
      }
    }
    return students;
  },

  // called by cumulative item page
  getGradingPeriods : function()
  {
    return this.gradingPeriods;
  },

  // called by cumulative item page
  getCategories : function()
  {
    return this.categories;
  },

  // called by grade detail page
  getNextUserId : function(userId)
  {
    for ( var i = 0; i < this.visibleRows.length - 1; i++)
    {
      if (this.visibleRows[i][0].uid == userId)
      {
        return this.visibleRows[i + 1][0].uid;
      }
    }
    return null;
  },

  // called by grade detail page
  getPrevUserId : function(userId)
  {
    for ( var i = 1; i < this.visibleRows.length; i++)
    {
      if (this.visibleRows[i][0].uid == userId)
      {
        return this.visibleRows[i - 1][0].uid;
      }
    }
    return null;
  },

  // called by grade detail page; returns null if invalid colId
  getRawValue : function(colId, displayValue)
  {
    var colIndex = this.colDefMap[colId];
    if (colIndex === undefined)
    {
      return null;
    }
    var colDef = this.colDefs[colIndex];
    return colDef.getRawValue(displayValue);
  },

  // called by grade detail page; returns null if invalid colId
  getDisplayValue : function(colId, rawValue)
  {
    var colIndex = this.colDefMap[colId];
    if (colIndex === undefined)
    {
      return null;
    }
    var colDef = this.colDefs[colIndex];
    return colDef.getDisplayValue(rawValue);
  },

  // called by grade detail page; returns null if invalid colId
  getDisplayType : function(colId)
  {
    var colIndex = this.colDefMap[colId];
    if (colIndex === undefined)
    {
      return null;
    }
    var colDef = this.colDefs[colIndex];
    return colDef.getDisplayType();
  },

  // called by grade detail page; returns validate error or null if no error
  validate : function(colId, newValue)
  {
    var colIndex = this.colDefMap[colId];
    if (colIndex === undefined)
    {
      return null;
    }
    var colDef = this.colDefs[colIndex];
    return colDef.validate(newValue);
  },

  getCheckedStudentIds : function()
  {
    var rows = this.visibleRows;
    var students =
      [];
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      if (rows[i][0].isRowChecked)
      {
        students.push(rows[i][0].uid);
      }
    }
    return students;
  },

  checkedAllStudents : function()
  {
    var rows = this.visibleRows;
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      rows[i][0].isRowChecked = true;
    }
    this.fireModelChanged();

  },

  checkedNoStudents : function()
  {
    var rows = this.visibleRows;
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      rows[i][0].isRowChecked = false;
    }
    this.fireModelChanged();
  },

  invertCheckedStudents : function()
  {
    var rows = this.visibleRows;
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      rows[i][0].isRowChecked = !rows[i][0].isRowChecked;
    }
    this.fireModelChanged();
  },

  checkedRangeOfStudents : function(uid1, uid2)
  {
    var startId;
    var rows = this.visibleRows;
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      var uid = rows[i][0].uid;
      if (!startId && (uid != uid1 && uid != uid2))
      {
        continue;
      }
      else if (!startId && uid == uid1)
      {
        startId = uid;
      }
      else if (!startId && uid == uid2)
      {
        startId = uid;
      }
      else if (uid == uid1 || uid == uid2)
      {
        break;
      }
      else
      {
        rows[i][0].isRowChecked = true;
      }
    }
    this.fireModelChanged();
  },

  clearAttempts : function(colId, clearOption, startDate, endDate)
  {
    this.gradebookService.clearAttempts(colId, clearOption, startDate, endDate);
  },

  updateGroups : function()
  {
    var crsId = this.courseId;
    if (crsId.indexOf("_") >= 0)
    {
      crsId = crsId.split("_")[1];
    }
    var gradeCenterContentFrame = window.frames.gradecenterframe; // Grade Center Frame in SSL mode
    if (!gradeCenterContentFrame)
    {
      gradeCenterContentFrame = window.frames.content; // regular course content frame
    }
    if (!gradeCenterContentFrame.GradebookDWRFacade)
    {
      gradeCenterContentFrame = window.frames.content.frames.main;
    }
    gradeCenterContentFrame.GradebookDWRFacade.getGroups(crsId, Gradebook.GridModel.prototype.updateGroupsCallback);
  },
  
  updateGroupsCallback : function(retData)
  {
    var groupsMap = [];
    var groups = [];
    var h = $H(retData);
    h.each(function(pair)
    {
      var g = {};
      g.id = pair.key;
      g.uids = pair.value;
      groupsMap[g.id] = groups.length;
      groups.push(g);
    });
    var model = Gradebook.getModel();
    model.groupsMap = groupsMap;
    model.groups = groups;
  },
  
  // used by reporting
  getReportData : function(reportDef)
  {
    var LAST_NAME_COL_IDX = 0;
    // get rows for students to include in report
    var userIds = null;
    if (reportDef.students == 'BYGROUPS')
    {
      if (!reportDef.groupIds)
      {
        GradebookUtil.error('GridModel error getReportData: no reportDef.groupIds');
        return null;
      }
      userIds = this._getUserIdsByGroupIds(reportDef.groupIds);
    }
    else if (reportDef.students == 'BYSTUDENT')
    {
      if (!reportDef.studentIds)
      {
        GradebookUtil.error('GridModel error getReportData: no reportDef.studentIds');
        return null;
      }
      userIds = reportDef.studentIds;
    }
    var rows = this._getRowsByUserIds(userIds);
    if (!reportDef.includeHiddenStudents)
    {
      rows = this._removeHiddenStudents(rows);
    }
    // get columns to include in report
    var colDefs = this.getCurrentColDefs(true);;
    if (reportDef.columns == 'BYITEM')
    {
      colDefs = this._getColDefsById(reportDef.itemIds);
    }
    else if (reportDef.columns == 'BYGP')
    {
      colDefs = this._getColDefsByGradingPeriodId(reportDef.gradingPeriodIds);
    }
    else if (reportDef.columns == 'BYCAT')
    {
      colDefs = this._getColDefsByCategoryId(reportDef.categoryIds);
    }
    if (!reportDef.includeHiddenColumns)
    {
      colDefs = this._removeHiddenColumns(colDefs);
    }
  
    //before printing the report, sort on student's last name,according to PM's requirement
    rows.sort(function(a, b)
    {
      var aa = a[LAST_NAME_COL_IDX].sortval;
      var bb = b[LAST_NAME_COL_IDX].sortval;
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
    // create return data structure
    var reportData =
    {};
    reportData.columnInfoMap = [];
    reportData.studentGradeInfo = [];
  
    var i, len, len0;
  
    // add column data
    for (i = 0, len = colDefs.length; i < len; i++)
    {
      var cdef = colDefs[i];
      var cdata = {};
      reportData.columnInfoMap[cdef.id] = cdata;
      cdata.name = cdef.getName();
      if (reportDef.columnInfoDescription)
      {
        cdata.description = 'tbd'; // server will provide desc map
      }
      if (reportDef.columnInfoDueDate)
      {
        cdata.dueDate = cdef.getDueDate();
      }
      if (reportDef.columnInfoStatsMedian || reportDef.columnInfoStatsAverage)
      {
        var stats = cdef.getStats(true); // include unavailable students
        cdata.statsMedian = stats.median;
        cdata.statsAverage = stats.avg;
      }
    }
  
    // add student data
    for (i = 0, len0 = rows.length; i < len0; i++)
    {
      var row = rows[i];
      var rd =
      {};
      reportData.studentGradeInfo.push(rd);
  
      if (reportDef.firstName)
      {
        rd.firstName = this._getStudentAttribute(row, 'FN');
      }
      if (reportDef.lastName)
      {
        rd.lastName = this._getStudentAttribute(row, 'LN');
      }
      if (reportDef.studentId)
      {
        rd.studentId = this._getStudentAttribute(row, 'SI');
      }
      if (reportDef.userName)
      {
        rd.userName = this._getStudentAttribute(row, 'UN');
      }
      if (reportDef.lastAccessed)
      {
        rd.lastAccessed = this._getStudentAttribute(row, 'LA');
        if (rd.lastAccessed && rd.lastAccessed > 0)
        {
          var date = new Date();
          date.setTime(rd.lastAccessed);
          rd.lastAccessed = formatDate(date, 'MMM d, y');
        }
      }
      rd.grades = [];
      for ( var c = 0, len1 = colDefs.length; c < len1; c++)
      {
        var g =
        {};
        g.cid = colDefs[c].id;
        var gridCell = this._getGrade(row, colDefs[c]);
        if (gridCell.attemptInProgress() && !gridCell.isOverride())
        {
          g.grade = this.getMessage('inProgressMsg');
        }
        else if (gridCell.needsGrading() && !gridCell.isOverride())
        {
          g.grade = this.getMessage('needsGradingMsg');
        }
        else
        {
          g.grade = gridCell.getCellValue();
        }
        rd.grades.push(g);
      }
    }
    return reportData;
  },
  
  _getGrade : function(row, colDef)
  {
    var colIndex = this.colDefMap[colDef.id];
    if (!colIndex)
    {
      GradebookUtil.error('GridModel _getGrade invalid column id: ' + colDef.id);
      return null;
    }
    var data = row[colIndex];
    if (!data.metaData)
    {
      data.metaData = row[0];
    }
    if (!data.colDef)
    {
      data.colDef = colDef;
    }
    return new Gradebook.GridCell(data);
  },
  
  _getStudentAttribute : function(row, colDefId)
  {
    var colIndex = this.colDefMap[colDefId];
    if ( Object.isUndefined( colIndex ) )
    {
      GradebookUtil.error('GridModel _getStudentAttribute invalid column id: ' + colDefId);
      return null;
    }
    return row[colIndex].v;
  },
  
  _removeHiddenStudents : function(students)
  {
    var retStudents = [];
    for ( var i = 0, len = students.length; i < len; i++)
    {
      if (!students[i][0].isHidden)
      {
        retStudents.push(students[i]);
      }
    }
    return retStudents;
  },
  
  _removeHiddenColumns : function(colDefs)
  {
    var retColDefs =  [];
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      if (!colDefs[i].isHidden())
      {
        retColDefs.push(colDefs[i]);
      }
    }
    return retColDefs;
  },
  
  _getColDefsById : function(itemIds)
  {
    var colDefs = [];
    for ( var i = 0, len = this.colDefs.length; i < len; i++)
    {
      if (itemIds.indexOf(this.colDefs[i].id) != -1)
      {
        colDefs.push(this.colDefs[i]);
      }
    }
    return colDefs;
  },
  
  _getColDefsByCategoryId : function(categoryIds)
  {
    var colDefs =
      [];
    for ( var i = 0, len = this.colDefs.length; i < len; i++)
    {
      if (categoryIds.indexOf(this.colDefs[i].catid) != -1)
      {
        colDefs.push(this.colDefs[i]);
      }
    }
    return colDefs;
  },
  
  _getColDefsByGradingPeriodId : function(gradingPeriodIds)
  {
    var colDefs =
      [];
    for ( var i = 0, len = this.colDefs.length; i < len; i++)
    {
      if (gradingPeriodIds.indexOf(this.colDefs[i].gpid) != -1)
      {
        colDefs.push(this.colDefs[i]);
      }
    }
    return colDefs;
  },
  
  _getRowsByUserIds : function(userIds)
  {
    var rows = this.rows;
    if (!userIds)
    {
      return rows;
    }
    var retRows =
      [];
    for ( var i = 0, len = rows.length; i < len; i++)
    {
      if (userIds.indexOf(rows[i][0].uid) != -1)
      {
        retRows.push(rows[i]);
      }
    }
    return retRows;
  },
  
  _getUserIdsByGroupIds : function(groupIds)
  {
    if (!this.groupsMap || !this.groups)
    {
      GradebookUtil.error('GridModel error getUserIdsByGroupIds: no groups');
      return null;
    }
    var userIds = [];
    for ( var i = 0; i < groupIds.length; i++)
    {
      var index = this.groupsMap[Number(groupIds[i])];
      if ( undefined === index )
      {
        GradebookUtil.error('GridModel error getUserIdsByGroupIds: no group for id: ' + groupIds[i]);
        continue;
      }
      var group = this.groups[index];
      for ( var g = 0; g < group.uids.length; g++)
      {
        if (userIds.indexOf(group.uids[g]) == -1)
        {
          userIds.push(String(group.uids[g]));
        }
      }
    }
    return userIds;
  },
  
  // called by student stats page
  getStudentStats : function(userId, currentViewOnly)
  {
    var studentStats =
    {};
    studentStats.catStats =
      [];
    var catMap =
      [];
    var i, catStat;

    // get columns, either all or current view
  var colDefs =
    [];
  var len = currentViewOnly ? this.colOrderMap.length : this.colDefs.length;
  for (i = 0; i < len; i++)
  {
    var idx = currentViewOnly ? this.colOrderMap[i] : i;
    var c = this.colDefs[idx];
    if (!c.deleted && c.isGrade() && !c.isCalculated())
    {
      colDefs.push(c);
    }
  }

  var row = this.getRowByUserId(userId);

  for (i = 0; i < colDefs.length; i++)
  {
    var colDef = colDefs[i];
    var catId = colDef.getCategoryID();
    catStat = catMap[catId];
    if (!catStat)
    {
      catStat =
      {};
      catStat.name = colDef.getCategory();
      catStat.qtyGraded = 0;
      catStat.qtyInProgress = 0;
      catStat.qtyNeedsGrading = 0;
      catStat.qtyExempt = 0;
      catStat.sum = 0;
      catStat.avg = 0;
      catMap[catId] = catStat;
      studentStats.catStats.push(catStat);
    }
    var grade = this._getGrade(row, colDef);
    var val = grade.getValue();
    var isNull = (val == '-');
    var isIP = grade.attemptInProgress();
    var isNG = grade.needsGrading();
    var isExempt = grade.isExempt();
    var isVal = (!isNull && !isIP && !isNG && !isExempt);
    if (isIP)
    {
      catStat.qtyInProgress++;
    }
    else if (isNG)
    {
      catStat.qtyNeedsGrading++;
    }
    else if (isExempt)
    {
      catStat.qtyExempt++;
    }

    if (isVal)
    {
      catStat.qtyGraded++;
      if (colDef.isCalculated())
      {
        val = parseFloat(val) / parseFloat(grade.getPointsPossible()) * 100.0;
      }
      catStat.sum += parseFloat(val);
    }
  }
  studentStats.numItemsCompleted = 0;
  var totNumExempt = 0;
  for (i = 0; i < studentStats.catStats.length; i++)
  {
    catStat = studentStats.catStats[i];
    if (catStat.sum > 0)
    {
      catStat.avg = catStat.sum / parseFloat(catStat.qtyGraded);
      catStat.avg = NumberFormatter.getDisplayFloat(catStat.avg.toFixed(2));
    }
    totNumExempt += catStat.qtyExempt;
    studentStats.numItemsCompleted += (catStat.qtyNeedsGrading + catStat.qtyGraded);
  }
  studentStats.numItems = colDefs.length - totNumExempt;
  return studentStats;
},

getAccessibleMode : function()
{
  return this.accessibleMode;
},

setAccessibleMode : function(accessibleMode)
{
  this.accessibleMode = accessibleMode;
},

setMessages : function(messages)
{
  this.messages = messages;
},

getMessage : function(key)
{
  if (this.messages)
  {
    return this.messages[key];
  }
  else
  {
    return key;
  }
}

};

////////////////////////////Utility //////////////////////////////////////

Gradebook.GridRowIterator = Class.create();

Gradebook.GridRowIterator.prototype =
{
  initialize : function(dataArray, orderMap, startIndex, colDefs)
  {
    this.dataArray = dataArray;
    this.orderMap = orderMap;
    this.currentIndex = startIndex;
    this.colDefs = colDefs;
  },
  
  hasNext : function()
  {
    return this.currentIndex < this.orderMap.length;
  },
  
  next : function()
  {
    if (this.currentIndex >= this.orderMap.length)
    {
      GradebookUtil.error('GridRowIterator out of data. length = ' + this.orderMap.length);
      return null;
    }
    var idx = this.orderMap[this.currentIndex++];
    var data = this.dataArray[idx];
    // add colDef & metedata reference to cell data, if not already there
    if (!data.colDef)
    {
      data.colDef = this.colDefs[idx];
    }
    if (!data.metaData)
    {
      data.metaData = this.dataArray[0]; // first cell is extended with metadata
    }
    return data;
  }
};

Gradebook.ColDefIterator = Class.create();

Gradebook.ColDefIterator.prototype =
{
  initialize : function(dataArray, orderMap, startIndex)
  {
    this.dataArray = dataArray;
    this.orderMap = orderMap;
    this.currentIndex = startIndex;
  },
  hasNext : function()
  {
    return this.currentIndex < this.orderMap.length;
  },
  next : function()
  {
    if (this.currentIndex >= this.orderMap.length)
    {
      GradebookUtil.error('ColDefIterator out of data. length = ' + this.orderMap.length);
      return null;
    }
    return this.dataArray[this.orderMap[this.currentIndex++]];
  }
};

Gradebook.numberComparator = function(a, b)
{
  return a - b;
};

var NumberFormatter =
{
 
  // usually called from frameset scope and re-set when the locale format is set on the model
  needToConvert : false,

  toStringMin2Digits: function( num, maxPrecision )
  {
    var roundBase = 100;
    for ( var i = 2; i < maxPrecision; ++ i )
    {
      var floatRound =  Math.round( num * roundBase ) / roundBase;
      roundBase *= 10;
      var floatRound2 = Math.round( num * roundBase ) / roundBase;
      if ( floatRound == floatRound2 )
      {
        return NumberFormatter.getDisplayFloat( num.toFixed( i ) );
      }
    }
    return NumberFormatter.getDisplayFloat( num.toFixed( maxPrecision ) );
  },
  
  getDisplayFloat : function(f)
  {
    if (!NumberFormatter.needToConvert)
    {
      return f;
    }
    f = '' + f;
    return f.replace('.', ',');
  },

  getDotFloat : function(f)
  {
    if (!NumberFormatter.needToConvert)
    {
      return f;
    }
    f = '' + f;
    return f.replace(',', '.');
  },
  
  parseLocaleFloat: function ( num )
  {
    if ( !num )
    {
      return NaN;
    }
    var dotFloat = NumberFormatter.getDotFloat( num );
    return parseFloat( dotFloat );
  }
};
