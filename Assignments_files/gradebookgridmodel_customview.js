/**
 * Gradebook data grid
 * 
 * PORTIONS OF THIS FILE ARE BASED ON RICO LIVEGRID 1.1.2
 * 
 * Copyright 2005 Sabre Airline Solutions
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 * 
 * @author "Bill Richard"
 * @version
 * 
 * 
 */

// Gradebook.GridModel -----------------------------------------------------
Gradebook.CustomView = Class.create();
Gradebook.CustomView.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
  },

  // evaluate this custom view; determine userIds & itemIds for view
  // returns false if the formula cannot be evaluated, else true
  evaluate : function()
  {
    try
    {
      if (this.definition)
      {
        var ext = eval('({' + this.definition + '})');
        Object.extend(this, ext);
        this.definition = null;
      }
      this.userIds =
        [];
      this.colIds =
        [];
      this.aliasMap =
        [];
      for ( var i = 0, len = this.aliases.length; i < len; i++)
      {
        this.aliasMap[this.aliases[i].key] = this.aliases[i].val;
      }
      if (this.formula)
      {
        this._evaluateAdvanced();
        this._computeDisplayItems();
      }
      else if (this.searchType == 'status')
      {
        this._evaluateStatus();
      }
      else
      {
        this._evaluateBasic();
        this._computeDisplayItems();
      }
      return true;
    }
    catch (e)
    {
      alert(this.model.getMessage('custViewRenderErrorMsg'));
      return false;
    }
  },

  usesGroups : function()
  {
    for ( var i = 0, len = this.aliases.length; i < len; i++)
    {
      if (this.aliases[i].key.startsWith('gr'))
      {
        return true;
      }
    }
    return false;
  },

  getUserIds : function()
  {
    return this.userIds;
  },

  getDisplayItemIds : function()
  {
    return this.colIds;
  },

  _computeDisplayItems : function()
  {
    // map aliased ids to real ids
    if (this.display.ids)
    {
      this.display.unAliasedIds =
        [];
      for ( var i = 0, len = this.display.ids.length; i < len; i++)
      {
        var id = this.aliasMap[this.display.ids[i]];
        if (!id)
        {
          throw 'missing alias';
        }
        this.display.unAliasedIds.push(id);
      }
    }
    var colDefs = this.model.getColDefs(false, this.display.showhidden);
    var dispType = this.display.items.toUpperCase();
    if (dispType == "BYITEM")
    {
      this.colIds = this._getItemsById();
    }
    else if (dispType == "INCRI")
    { // in criteria
      this.colIds = this._getItemsInCriteria();
    }
    else if (dispType == "BYCAT")
    { // by category
      this.colIds = this._getItemsByCategoryId(colDefs);
    }
    else if (dispType == "BYGP")
    { // by grading period
      this.colIds = this._getItemsByGradingPeriodId(colDefs);
    }
    else if (dispType == "ALLITEM")
    {
      this.colIds = this._getAllItems(colDefs);
    }
    else if (dispType == "IVS")
    {
      this.colIds = this._getItemsByVisibilityToStudents(colDefs, true);
    }
    else if (dispType == "INVS")
    {
      this.colIds = this._getItemsByVisibilityToStudents(colDefs, false);
    }
    else if (dispType == "NOITEM")
    {
      this.colIds =
        [];
    }
  },
  
  _getItemsById : function()
  {
    return this.display.unAliasedIds;
  },
  
  _getItemsInCriteria : function()
  {
    var itemIds =
      [];
    // get items that are used in criteria; which are in aliases
    for ( var i = 0, len = this.aliases.length; i < len; i++)
    {
      if (this.aliases[i].key.startsWith('I_'))
      {
        itemIds.push(this.aliases[i].val);
      }
    }
    return itemIds;
  },
  
  _getItemsByCategoryId : function(colDefs)
  {
    var itemIds =
      [];
    // get items that have category id in display.ids
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      if (this.display.unAliasedIds.indexOf(colDefs[i].catid) != -1)
      {
        itemIds.push(colDefs[i].id);
      }
    }
    return itemIds;
  },
  
  _getItemsByGradingPeriodId : function(colDefs)
  {
    var itemIds =
      [];
    // get items that have grading period id in display.ids
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      if (this.display.unAliasedIds.indexOf(colDefs[i].gpid) != -1)
      {
        itemIds.push(colDefs[i].id);
      }
    }
    return itemIds;
  },
  
  _getItemsByVisibilityToStudents : function(colDefs, vis)
  {
    var itemIds =
      [];
    // get items that have grading period id in display.ids
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      if (colDefs[i].vis == vis)
      {
        itemIds.push(colDefs[i].id);
      }
    }
    return itemIds;
  },
  
  _getAllItems : function(colDefs)
  {
    var itemIds =
      [];
    for ( var i = 0, len = colDefs.length; i < len; i++)
    {
      itemIds.push(colDefs[i].id);
    }
    return itemIds;
  },
  
  _evaluateStatus : function()
  {
    var i, len, id;
    if (this.students.userIds && this.students.userIds[0] == "all")
    {
      var showstuhidden = this.students.showstuhidden;
      var modelStudents = this.model.getStudents(showstuhidden);
      for (i = 0, len = modelStudents.length; i < len; i++)
      {
        this.userIds.push(modelStudents[i].id);
      }
    }
    else if (this.students.userIds)
    {
      var uids = this.students.userIds;
      for (i = 0, len = uids.length; i < len; i++)
      {
        id = this.aliasMap[uids[i]];
        if (!id)
        {
          throw 'missing alias';
        }
        this.userIds.push(id);
      }
    }
    else if (this.students.groupIds)
    {
      var groupIds =
        [];
      for (i = 0, len = this.students.groupIds.length; i < len; i++)
      {
        id = this.aliasMap[this.students.groupIds[i]];
        if (!id)
        {
          throw 'missing alias';
        }
        groupIds.push(id);
      }
      var getUserIdsByGroupIdsFunc = this.model._getUserIdsByGroupIds.bind(this.model);
      this.userIds = getUserIdsByGroupIdsFunc(groupIds);
    }
    else
    {
      throw 'no userIds or groupIds in smart view';
    }
    var colDefs = this.model.getColDefs(false, this.display.showhidden);
    this.colIds =
      [];
    var catid;
    if (this.model.categoryFilter)
    {
      var cf = this.model.categoryFilter;
      catid = cf.startsWith('c_') ? cf.substr(2, cf.length - 2) : cf;
    }
    else if (this.category == 'c_all')
    {
      catid = 'all';
    }
    else
    {
      catid = this.aliasMap[this.category];
    }
    for (i = 0, len = colDefs.length; i < len; i++)
    {
      if (colDefs[i].catid == catid || catid == "all" && colDefs[i].isGrade())
      {
        this.colIds.push(colDefs[i].id);
      }
    }
    if (this.colIds.length === 0)
    {
      this.userIds =
        [];
      return;
    }
  
    var filterType;
    if (this.model.statusFilter)
    {
      var sf = this.model.statusFilter;
      filterType = sf.startsWith('stat_') ? sf.substr(5, sf.length - 5) : sf;
    }
    else
    {
      filterType = this.display.items;
    }
    if (filterType == "ALL")
    {
      return; // no filtering needed
    }
    
    var rowFlags =
      [];
    var colFlags =
      [];
    var temp_userIds =
      [];
    var temp_colIds =
      [];
    var r, c, rlen, clen;
    
    for (r = 0, len = this.userIds.length; r < len; r++)
    {
      rowFlags.push(false);
    }
    for (c = 0, len = this.colIds.length; c < len; c++)
    {
      colFlags.push(false);
    }
    
    // evaluate filter to determine which user/col to include.
    for (r = 0, rlen = this.userIds.length; r < rlen; r++)
    {
  
      var row = this.model.getRowByUserId(this.userIds[r]);
      for (c = 0, clen = this.colIds.length; c < clen; c++)
      {
  
        var colIndex = this.model.colDefMap[this.colIds[c]];
        var colDef = this.model.colDefs[colIndex];
        var grade = this._getGrade(row, colDef);
  
        if (grade.passesFilter(filterType))
        {
          if (!rowFlags[r])
          {
            rowFlags[r] = true;
            temp_userIds.push(this.userIds[r]);
          }
          if (!colFlags[c])
          {
            colFlags[c] = true;
            temp_colIds.push(this.colIds[c]);
          }
        }
      }
    }
    this.userIds = temp_userIds;
    this.colIds = temp_colIds;
  },
  
  _getGrade : function(row, colDef)
  {
    var colIndex = this.model.colDefMap[colDef.id];
    var data = row[colIndex];
    if (!data.metaData)
    {
      data.metaData = row[0];
    }
    if (!data.colDef)
    {
      data.colDef = colDef;
    }
    if (!this.gridCell)
    {
      this.gridCell = new Gradebook.GridCell();
    }
    this.gridCell.setData(data);
    return this.gridCell;
  },
  
  _evaluateBasic : function()
  {
    var i, len;
    if (this.students.userIds && this.students.userIds[0] != "all")
    {
      var uids = this.students.userIds;
      for (i = 0, len = uids.length; i < len; i++)
      {
        var id = this.aliasMap[uids[i]];
        if (!id)
        {
          throw 'missing alias';
        }
        this.userIds.push(id);
      }
    }
    else
    { // all students
      var showstuhidden = this.students.showstuhidden;
      var modelStudents = this.model.getStudents(showstuhidden);
      for (i = 0, len = modelStudents.length; i < len; i++)
      {
        this.userIds.push(modelStudents[i].id);
      }
    }
  },
  
  _evaluateAdvanced : function()
  {
    var i, len;
    // lazily compute postfix formula & criteriaMap
    if (!this.postFixFormula)
    {
      this.postFixFormula = this.infix2postfix(this.formula);
    }
    if (!this.criteriaMap)
    {
      this.criteriaMap =
        [];
      for (i = 0, len = this.criteria.length; i < len; i++)
      {
        this.criteriaMap[this.criteria[i].fid] = i;
      }
    }
    // test each row and add to userIds if it passes formula
    var rows = this.model.rows;
    for (i = 0, len = rows.length; i < len; i++)
    {
      if (this._evaluateFormulaForRow(rows[i]))
      {
        this.userIds.push(rows[i][0].uid);
      }
    }
  },
  
  _evaluateFormulaForRow : function(row)
  {
    // only one criteria in formula
    if (this.postFixFormula.length == 1)
    {
      return this._evalCriteria(this.postFixFormula[0], row);
    }
    // evaluate postfix formula:
    // * push non-operators on stack
    // * when operators are encountered:
    // pop two operands off stack
    // evaluate operands (criteria)
    // apply operator to the two evaluated operands
    // store result on stack
    // * pop & return final result
    var stack =
      [];
    for ( var i = 0, len = this.postFixFormula.length; i < len; i++)
    {
      var tok = this.postFixFormula[i];
      switch (tok)
      {
        case "AND":
        case "OR":
          if (stack.length < 2)
          {
            throw (this.model.getMessage('custViewStackEmptyMsg') + tok);
          }
          var op2 = stack.pop();
          var op1 = stack.pop();
          var firstValue = op1;
          if (typeof (op1) == 'string')
          {
            firstValue = this._evalCriteria(op1, row);
          }
          var secondValue = op2;
          if (typeof (op2) == 'string')
          {
            secondValue = this._evalCriteria(op2, row);
          }
          if (tok == "AND")
          {
            stack.push((firstValue && secondValue));
          }
          else if (tok == "OR")
          {
            stack.push((firstValue || secondValue));
          }
          break;
        default:
          stack.push(tok);
          break;
      }
    }
    if (stack.length != 1)
    {
      throw this.model.getMessage('custViewUnableToEvaluateMsg');
    }
    else
    {
      return stack.pop();
    }
  },
  
  _getAliasOrId : function(id)
  {
    if (id.startsWith('I_') || id.startsWith('c_') || id.startsWith('gp_') || id.startsWith('gr_') || id.startsWith('st_'))
    {
      return this.aliasMap[id];
    }
    else
    {
      return id;
    }
  },
  
  _evalCriteria : function(fid, row)
  {
    // look up criteria by fid
    var crit = this.criteria[this.criteriaMap[fid]];
    var colId = this._getAliasOrId(crit.cid);
    if (!colId)
    {
      throw 'missing alias';
    }
    var colDefMap = this.model.colDefMap;
    var colIdx = colDefMap[colId];
    if (colId == 'SV' || colId == 'GM')
    {
      colIdx = 0;
    }
    if (colIdx === undefined || colIdx === null)
    {
      throw 'missing alias';
    }
    var colDef = this.model.colDefs[colIdx];
    var gridCell = this._getGrade(row, colDef);
    var evalFunc = this._getEvalCriteriaFunc(crit);
    return evalFunc(crit, gridCell);
  },
  
  _evalAvailableCriteria : function(crit, gridCell)
  {
    var avail = (gridCell.isAvailable()) ? "A" : "U";
    return crit.value == avail;
  },
  
  _evalStatusCriteria : function(crit, gridCell)
  {
    return gridCell.passesFilter(crit.value);
  },
  
  _evalStudentVisibleCriteria : function(crit, gridCell)
  {
    var avail = (gridCell.isHidden()) ? "H" : "V";
    return crit.value == avail;
  },
  
  _evalGroupMembershipCriteria : function(crit, gridCell)
  {
    // There may be 1 or more values passed. We allow multiple selection of Groups
    var result = (crit.cond == "eq") ? false : true;
    var groupNames = crit.value.split(",");
    for ( var i = 0, len = groupNames.length; i < len; i++)
    {
      var groupId = this.aliasMap[groupNames[i]];
      if (!groupId)
      {
        throw 'missing alias';
      }
      var userId = gridCell.getUserId();
      var inGroup = this._userIsInGroup(userId, groupId);
      result = ((crit.cond == "eq") ? result || inGroup : result && !inGroup);
    }
    return result;
  },
  
  _evalLastAccessedCriteria : function(crit, gridCell)
  {
    var cellVal = gridCell.getValue();
    if (crit.cond == "eq")
    {
      var numMSecPerDay = 1000 * 60 * 60 * 24;
      var v1 = parseInt(cellVal / numMSecPerDay, 10);
      var v2 = parseInt(crit.value / numMSecPerDay, 10);
      return (v1 == v2);
    }
    else if (crit.cond == "be")
    {
      return (cellVal < crit.value);
    }
    else if (crit.cond == "af")
    {
      return (cellVal > crit.value);
    }
  },
  
  _defaultEvalCriteria : function(crit, gridCell)
  {
    var cellVal = gridCell.getValue();
    var critVal;
    if (gridCell.attemptInProgress() || gridCell.needsGrading() || (cellVal == '-') || gridCell.isExempt())
    {
      return false;
    }
    var operator = crit.cond;
    // '-' will end up NaN  
    if ( crit.value != '-')
    {
      critVal = gridCell.colDef.getRawValue( crit.value );
    }
    else
    {
      critVal = '-';
    }
    if (this._isNumber(cellVal) && this._isNumber(critVal))
    {
      var dblCellVal = this._toNumber(cellVal);
      var dblCritVal = this._toNumber(critVal);
      var dblCritVal2 = crit.value2 ? this._toNumber(gridCell.colDef.getRawValue(crit.value2)) : 0;
      if (operator == "eq")
      {
        return (dblCellVal == dblCritVal);
      }
      else if (operator == "neq")
      {
        return (dblCellVal != dblCritVal);
      }
      else if (operator == "gt")
      {
        return (dblCellVal > dblCritVal);
      }
      else if (operator == "lt")
      {
        return (dblCellVal < dblCritVal);
      }
      else if (operator == "le")
      {
        return (dblCellVal <= dblCritVal);
      }
      else if (operator == "ge")
      {
        return (dblCellVal >= dblCritVal);
      }
      else if (operator == "bet")
      {
        return ((dblCritVal <= dblCellVal) && (dblCellVal <= dblCritVal2));
      }
    }
    else if ( typeof (critVal) == "string" )
    {
      var cellTextValue = gridCell.getTextValue();
      //if data.tv is not empty
      if ( cellTextValue != '-' && cellTextValue !== undefined && cellTextValue !== null && 
          typeof ( cellTextValue ) == "string")
      {
        // replace gridCell.getValue() with gridCell.getTextValue()
        cellVal = cellTextValue.toUpperCase();
      }     
      critVal = critVal.toUpperCase();
      if (operator == "eq")
      {
        return (cellVal == critVal);
      }
      else if (operator == "neq")
      {
        return (cellVal != critVal);
      }
      else if (operator == "bw")
      {
        return (cellVal.startsWith(critVal));
      }
      else if (operator == "con")
      {
        return (cellVal.indexOf(critVal) != -1);
      }
    }
    else
    {
      throw (this.model.getMessage('custViewDataTypeMismatchMsg') + ' ' + crit.fid);
    }
  },
  
  _getEvalCriteriaFunc : function(crit)
  {
    if (!this.evalCriteriaFuncMap)
    {
      this.evalCriteriaFuncMap =
        [];
      this.evalCriteriaFuncMap.AV = this._evalAvailableCriteria.bind(this);
      this.evalCriteriaFuncMap.SV = this._evalStudentVisibleCriteria.bind(this);
      this.evalCriteriaFuncMap.LA = this._evalLastAccessedCriteria.bind(this);
      this.evalCriteriaFuncMap.GM = this._evalGroupMembershipCriteria.bind(this);
    }
    var func = this.evalCriteriaFuncMap[crit.cid];
    if (!func)
    {
      if (crit.cond == 'se')
      {
        func = this._evalStatusCriteria.bind(this);
      }
      else
      {
        func = this._defaultEvalCriteria.bind(this);
      }
    }
    return func;
  },
  
  _userIsInGroup : function(userId, groupId)
  {
    userId = Number(userId);
    var groups = this.model.groups;
    for ( var i = 0, len = groups.length; i < len; i++)
    {
      if (groups[i].id == groupId)
      {
        return (groups[i].uids.indexOf(userId) != -1);
      }
    }
    return false;
  },
  
  getValidationError : function(f, criteriaLst)
  {
    try
    {
      var postFix = this.infix2postfix(f, criteriaLst);
      return null;
    }
    catch (e)
    {
      return e;
    }
  },
  
  infix2postfix : function(formula, criteriaLst)
  {
    var f = formula;
    f = f.gsub(/\(/, ' ( '); // add spaces around parens
    f = f.gsub(/\)/, ' ) '); // add spaces around parens
    var a = $w(f); // split into array
    var stack =
      [];
    var out =
      [];
    var tok;
    for ( var i = 0, len = a.length; i < len; i++)
    {
      tok = a[i].toUpperCase();
      switch (tok)
      {
        case "AND":
        case "OR":
          while (this._isOperator(stack[stack.length - 1]))
          {
            out.push(stack.pop());
          }
          stack.push(tok.toUpperCase());
          break;
        case "(":
          stack.push(tok);
          break;
        case ")":
          var foundStart = false;
          while (stack.length > 0)
          {
            tok = stack.pop();
            if (tok == "(")
            {
              foundStart = true;
              break;
            }
            else
            {
              out.push(tok);
            }
          }
          if (stack.length === 0 && !foundStart)
          {
            throw (this.model.getMessage('custViewMismatchedParensMsg') + ' ' + this.name);
          }
          break;
        default:
          if (criteriaLst && criteriaLst.indexOf(tok) == -1)
          {
            throw this.model.getMessage('criteriaNotFoundMsg');
          }
          out.push(tok);
          break;
      }
    }
    while (stack.length > 0)
    {
      tok = stack.pop();
      if (tok == '(')
      {
        throw (this.model.getMessage('custViewMismatchedParensMsg') + ' ' + this.name);
      }
      out.push(tok);
    }
    return out;
  },
  
  _isOperator : function(s)
  {
    return (s == 'OR' || s == 'AND');
  },
  _isNumber : function(s)
  {
    return (isNaN(parseFloat(s)) ? false : true);
  },
  
  _toNumber : function(s)
  {
    if (typeof (s) == "number")
    {
      return s;
    }
    else
    {
      var n = parseFloat(s);
      return n.valueOf();
    }
  }

};

Gradebook.CustomView.getFullGC = function(model)
{
  var json =
  {};
  json.name = model.getMessage('fullGradeCenterMsg');
  json.id = 'fullGC';
  json.definition = "\"searchType\":\"status\",\"category\":\"c_all\",\"students\":{\"userIds\":[\"all\"],\"showstuhidden\":false}, \"display\":{ \"items\":\"ALL\",\"showhidden\":false}";
  json.aliases =
    [];
  return new Gradebook.CustomView(json, model);
};
