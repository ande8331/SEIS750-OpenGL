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

Gradebook.ColDef = Class.create();
Gradebook.ColDef.prototype =
{
  initialize : function(jsonObj, model, schemaMap)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
  if (this.sid)
  {
    this.primarySchema = schemaMap[this.sid];
  }
  if (this.ssid)
  {
    this.secondarySchema = schemaMap[this.ssid];
  }
},

/**
 * In the case of headers, the colum definition is treated as a cell, so little trick here
 * to have a uniform api: getGridCell().getColumnDefinition() in all cases
 */
getColumnDefinition: function( )
{
  return this;
},

getSortFunction : function( sortdir, secondarySortColumn )
{
  this.secondarySortColumn = secondarySortColumn; // can be null, in that case no second sorting
  if (sortdir == 'ASC')
  {
    return this._sortASC.bind(this);
  } else
  {
    return this._sortDESC.bind(this);
  }
},

validate : function(newValue, matchPartial)
{
  if (!this.primarySchema)
  {
    return null;
  } else
  {
    return this.primarySchema.validate(newValue, matchPartial);
  }
},
_sortASC : function(a, b, isSecondary )
{
  isSecondary = isSecondary || ( !this.secondarySortColumn );
  var sortColumnIndex = this.model.colDefMap[ this.id ];
  var aa = a[ sortColumnIndex ].sortval !== undefined?a[ sortColumnIndex ].sortval:a[ sortColumnIndex ].v;
  var bb = b[ sortColumnIndex ].sortval !== undefined?b[ sortColumnIndex ].sortval:b[ sortColumnIndex ].v;
  if (!aa && !bb)
  {
    return isSecondary?0:this.secondarySortColumn._sortASC( a, b, true);
  }
  if (!aa)
  {
    return -1;
  }
  if (!bb)
  {
    return 1;
  }
  if ( isNaN( aa ) || isNaN( bb ) )
  {
    aa = ( "" + aa ).toLocaleUpperCase();
    bb = ( "" + bb ).toLocaleUpperCase();
    var stringCompare = aa.localeCompare( bb );
    if ( stringCompare === 0 )
    {
      return isSecondary?0:this.secondarySortColumn._sortASC( a, b, true);
    }
    return stringCompare;
  }
  if (aa == bb)
  {
    return isSecondary?0:this.secondarySortColumn._sortASC( a, b, true);
  }
  if (aa < bb)
  {
    return -1;
  }
  return 1;
},

_sortDESC : function(a, b)
{
  return this._sortASC( b, a);
},

getEditValue : function(gridCell)
{
  if (!this.primarySchema)
  {
    return gridCell.getValue();
  }
  return this.primarySchema.getEditValue(gridCell);
},

// called by GridCell.getCellValue to get value for rendering in spreadsheet
  // uses primary (and optional secondary) schema to convert value to proper
  // display format
  getCellValue : function(gridCell)
  {
    if (!this.primarySchema)
    {
      return gridCell.getValue();
    }
    var cellVal = this.primarySchema.getCellValue(gridCell);
    if (this.secondarySchema)
    {
      var cellVal2 = this.secondarySchema.getCellValue(gridCell);
      cellVal += ' <span>(' + cellVal2 + ')</span>';
    }

    return cellVal;
  },

  // called by GridCell.getAltValue to get alt (mouse over) value for rendering
  // in spreadsheet
  // same as getCellValue unless there is a secondary schema
  getAltValue : function(gridCell)
  {
    if (gridCell.isExempt())
    {
      return this.model.getMessage('cmExemptGrade');
    }

    if (!this.secondarySchema)
    {
      return this.getCellValue(gridCell);
    }
    var cellVal = this.primarySchema.getCellValue(gridCell);
    if (this.secondarySchema)
    {
      var cellVal2 = this.secondarySchema.getCellValue(gridCell);
      cellVal += ' (' + cellVal2 + ')';
    }
    return cellVal;
  },

  getSortValue : function(gridCell)
  {
    if ( gridCell.data && gridCell.data.sortval !== undefined )
    {
      return gridCell.sortval;
    }
    return gridCell.getValue();
  },

  getName : function()
  {
    return this.name;
  },

  getID : function()
  {
    return this.id;
  },

  getPoints : function()
  {
    return this.points;
  },

  getPointsForDisplay : function()
  {
    var formattedPoints = NumberFormatter.getDisplayFloat(this.points);
    if (this.isCalculated())
    {
      var msgTemplate = new Template(GradebookUtil.getMessage('variesPerStudentMsg'));
      return msgTemplate.evaluate(
      {
        points : formattedPoints
      });
    }
    return formattedPoints;
  },

  getAliasID : function()
  {
    return this.id;
  },

  getCategoryID : function()
  {
    return this.catid;
  },

  getCategory : function()
  {
    if (!this.catid)
    {
      return "";
    }
    if (!this.model.catNameMap)
    {
      return "";
    }
    var name = this.model.catNameMap[Number(this.catid)];
    if (name)
    {
      return name;
    }
    return "";
  },

  getCategoryAliasID : function()
  {
    return this.catid;
  },

  isHidden : function()
  {
    return !this.gbvis;
  },

  isScorable : function()
  {
    return this.scrble;
  },

  isPublic : function()
  {
    return (this.id == this.model.pubColID);
  },

  isVisibleToStudents : function()
  {
    return this.vis;
  },

  hideColumn : function()
  {
    this.gbvis = false;
    this.model.hideColumn( this.id );
  },

  canHide : function()
  {
    return true;
  },

  toggleColumnStudentVisibility : function()
  {
    this.model.setColumnStudentVisibility( this.id, !this.vis );
  },

  getDisplayType : function()
  {
    return this.primarySchema.type;
  },

  hasError : function()
  {
    return this.comput_err;
  },

  // called by model.getDisplayValue when external pages need to convert a
  // rawValue
  // This function passes this.points to schema.getDisplayValue.
  // This method should not be called for this colDef if this colDef is a
  // calculated
  // column, because we do not have access to the gridCell to get its max
  // points.
  // todo: determine how to handle error condition if this column is a calulated
  // col
  getDisplayValue : function(rawValue)
  {
    if (this.primarySchema)
    {
      return this.primarySchema.getDisplayValue(rawValue, this.points);
    }
    return rawValue;
  },

  getSecondaryDisplayValue : function(rawValue)
  {
    if (this.secondarySchema)
    {
      return this.secondarySchema.getDisplayValue(rawValue, this.points);
    }
    return;
  }

};

Gradebook.GradeColDef = Class.create();
Object.extend(Gradebook.GradeColDef.prototype, Gradebook.ColDef.prototype);
Object.extend(Gradebook.GradeColDef.prototype,
{
  initialize : function(jsonObj, model, schemaMap)
  {
    this.linkrefid = "";
    Gradebook.ColDef.prototype.initialize.call(this, jsonObj, model, schemaMap);
  },

  getRawValue : function(newValue)
  {
    var score = newValue;
    // compute score based on primary schema
    if (this.primarySchema)
    {
      var rawValue = this.primarySchema.getRawValue(newValue, this);
      score = parseFloat(rawValue);
      if (!GradebookUtil.isValidFloat(rawValue))
      {
        if (typeof (rawValue) == "string")
        {
          return rawValue;
        }
        score = 0;
      }
    }
    return score;
  },

  getSortValue : function(gridCell)
  {
    if ( this.primarySchema )
    {
      return this.primarySchema.getSortValue( gridCell );
    }
    else
    {
      return gridCell.getValue();
    }
  },

  updateGrade : function(newValue, userId)
  {
    var score = this.getRawValue(newValue);
    var textValue = newValue;
    this.model.updateGrade(score, textValue, userId, this.id);
  },

  // get the grade for this column in the given row, use shared instance of
  // gridcell A
  // use for sort comparisons only... does not support multiple simultaneous
  // instances
  _getGradeA : function(row)
  {
    if (!this.colIndex)
    {
      this.colIndex = this.model.colDefMap[this.id];
    }
    var data = row[this.colIndex];
    if (!data.metaData)
    {
      data.metaData = row[0];
    }
    if (!data.colDef)
    {
      data.colDef = this;
    }
    var gc = Gradebook.GradeColDef.gridCellA;
    if (!gc)
    {
      Gradebook.GradeColDef.gridCellA = new Gradebook.GridCell();
      gc = Gradebook.GradeColDef.gridCellA;
    }
    gc.setData(data);
    return gc;
  },

  // get the grade for this column in the given row, use shared instance of
  // gridcell B
  // use for sort comparisons only... does not support multiple simultaneous
  // instances
  _getGradeB : function(row)
  {
    if (!this.colIndex)
    {
      this.colIndex = this.model.colDefMap[this.id];
    }
    var data = row[this.colIndex];
    if (!data.metaData)
    {
      data.metaData = row[0];
    }
    if (!data.colDef)
    {
      data.colDef = this;
    }
    var gc = Gradebook.GradeColDef.gridCellB;
    if (!gc)
    {
      Gradebook.GradeColDef.gridCellB = new Gradebook.GridCell();
      gc = Gradebook.GradeColDef.gridCellB;
    }
    gc.setData(data);
    return gc;
  },

  _sortASC : function(a, b, isSecondary )
  {
    // if secondary sort is null, we rely on the JS engine stable sort to derive sub-ordering
    isSecondary = isSecondary || ( !this.secondarySortColumn );
    var gradeA = this._getGradeA(a);
    var gradeB = this._getGradeB(b);
    var aa = gradeA.getSortValue();
    var bb = gradeB.getSortValue();
    if (gradeA.colDef.primarySchema instanceof Gradebook.TextSchema)
    {
      var stringComparaison = aa.localeCompare( bb );
      if ( stringComparaison === 0 )
      {
        return isSecondary?0:this.secondarySortColumn._sortASC(a, b, true);
      }
      return stringComparaison;
    }
    var aaa = parseFloat(aa);
    var bbb = parseFloat(bb);
    var aNull = (aa == '-');
    var bNull = (bb == '-');
    var ax = gradeA.isExempt();
    var bx = gradeB.isExempt();
    var aIP = gradeA.attemptInProgress();
    var bIP = gradeB.attemptInProgress();
    var aNG = gradeA.needsGrading();
    var bNG = gradeB.needsGrading();
    var aOr = gradeA.isOverride();
    var bOr = gradeB.isOverride();
    var aNoScore = (aNull || isNaN(aaa) || ax || ( !aOr && ( aIP || aNG ) ) );
    var bNoScore = (bNull || isNaN(bbb) || bx || ( !bOr && ( bIP || bNG ) ) );
    var aVal = (ax) ? 1 : (aIP) ? 2 : (aNG) ? 3 : (aNull) ? 0 : aa;
    var bVal = (bx) ? 1 : (bIP) ? 2 : (bNG) ? 3 : (bNull) ? 0 : bb;
    if (aNoScore || bNoScore)
    {
      if (aNoScore && bNoScore)
      {
        if (aVal == bVal)
        {
          return isSecondary?0:this.secondarySortColumn._sortASC(a, b, true);
        }
        else
        {
          return aVal - bVal;
        }
      }
      if (aNoScore)
      {
        return -1;
      }
      else
      {
        return 1;
      }
    }
    else
    {
      if (aaa == bbb)
      {
        return isSecondary?0:this.secondarySortColumn._sortASC(a, b, true);
      }
      else
      {
        return aaa - bbb;
      }
    }
  },

  _sortDESC : function(a, b)
  {
    return this._sortASC( b, a );
  },
  
  isAllowAttemptGrading: function()
  {
    var scoreProvider = this.getScoreProvider();
    // default is true unless specified otherwise in the score provider
    return scoreProvider?scoreProvider.allowAttempGrading:true;
  },
  
  /**
   * Used to determine if an attempt is just a grade holder or if it
   * can be expected to actually contain data behind it. That is determined
   * by the score provider being attempt based or not. If no score provider
   * then it is assumed the attempt might contain payload.
   */
  isAttemptWithPayload: function( )
  {
    if ( this.isManual( ) )
    {
      return false;
    }
    var scoreProvider = this.getScoreProvider();
    return scoreProvider?scoreProvider.attemptBased:true;
  },

  isGrade : function()
  {
    return true;
  },

  isCalculated : function()
  {
    return this.type != "N";
  },

  isTotal : function()
  {
    return this.type == "T";
  },

  isWeighted : function()
  {
    return this.type == "W";
  },

  getType : function()
  {
    switch (this.type)
    {
      case "T":
        return 'total';
      case "W":
        return 'weighted';
      case "A":
        return 'average';
      case "M":
        return 'minMax';
    }
    return "grade";
  },

  isManual : function()
  {
    return this.manual;
  },

  isUserCreated : function()
  {
    return this.userCreated;
  },

  isAlignable : function()
  {
    return this.align && this.align == 'y';
  },
  
  isAttemptAverage : function() 
  {
    return this.avg && this.avg == 'y';
  },
  
  isHideAttemptScore : function()
  {
    return this.hideAtt;
  },

  isTextSchema : function(schemaId)
  {
    var schema = this.model.schemaMap[schemaId];
    if (schema && (schema.type == "X"))
    {
      return true;
    }
    return false;
  },

  isAssessment : function()
  {
    return (this.src && this.src == 'resource/x-bb-assessment');
  },

  isAssignment : function()
  {
    return (this.src && this.src == 'resource/x-bb-assignment');
  },
  
  hasRubricAssociations : function()
  {
    return (this.hasRubrics && this.hasRubrics == "y");
  },

  getRubricIds : function()
  {
    return this.rubricIds;
  },

  getScoreProvider : function()
  {
    if (!this.src)
    {
      return "";
    }
    return this.model.scoreProvidersMap[this.src];
  },

  isAllowMulti : function()
  {
    return (this.am && this.am == "y");
  },

  clearAttemptsByDate : function(startDate, endDate)
  {
    this.model.clearAttempts(this.id, 'BYDATE', startDate, endDate);
  },

  clearAttempts : function(option)
  {
    this.model.clearAttempts(this.id, option);
  },

  getFirstUserWithCurrentViewAttempt : function(anonymousMode)
  {
    var grades = this.model._getGradesForItemId(this.id, false /* includeUnavailable */);
    if (anonymousMode)
    {
      grades.sort(function()
      {
        return (Math.round(Math.random()) - 0.5);
      });
    }
    var filterType = this.model.getCurrentStatus().toUpperCase();
    if (!filterType)
    {
      filterType = "STAT_ALL";
    }
    if (filterType.startsWith("STAT_"))
    {
      filterType = filterType.substr(5, filterType.length - 5);
    }
    if (filterType == "ALL")
    {
      filterType = "NN"; // we can't grade null grades
    }

    // find first user that has a grade which passes the current filter
    for ( var i = 0; i < grades.length; i++)
    {
      if (grades[i].passesFilter(filterType))
      {
        if (grades[i].isOverride() && !grades[i].hasAttempts())
        {
          continue;
        } else
        {
          return grades[i].getUserId();
        }
      }
    }
    return null;
  },

  _gradeAttempts: function ( anonymousMode ) {
    var userId = this.getFirstUserWithCurrentViewAttempt( anonymousMode );
    if (userId === null){
      alert(this.model.getMessage('noUsersFoundAlertMsg'));
      return;
    }
    // get attempts for user
    var s = this.model.getCurrentStatus();
    if (s.startsWith("stat_"))
    {
      s = s.substr( 5, status.length - 5 );
    }
    var url = "/webapps/gradebook/do/instructor/getJSONAttemptData?itemId="+this.id+"&course_id="+this.model.courseId+"&userId="+userId+'&status='+s;
    this.model.gradebookService.makeAjaxRequest(url, function ( resp ){
      var attempts = resp.responseJSON;
      if (attempts === null || attempts.length === 0){
        alert(this.model.getMessage('noAttemptsFoundAlertMsg'));
        return;
      }
      var groupAttemptId = ( attempts[0].groupAttemptId !== 0 ) ? attempts[0].groupAttemptId : null;
      this.gradeAttempt( userId, attempts[0].aid, anonymousMode, groupAttemptId );
    }.bind(this));
  },

   gradeAttempt: function ( userId, attemptId, anonymousMode, groupAttemptId, stat, returnUrl, mode, source ) {
     
      var url = '/webapps/gradebook/do/instructor/performGrading';
      
      var status = stat ? stat : this.model.statusFilter;
      if (!status)
      {
        status = "stat_ALL";
      }
      if (status.startsWith("stat_"))
      {
        status = status.substr(5,status.length-5);  // remove starting "stat_"
      }
      var cancelGradeUrl = returnUrl ? returnUrl : '/webapps/gradebook/do/instructor/enterGradeCenter?course_id='+this.model.courseId;
      
      url = url.concat(
            "?course_id=", this.model.courseId,
            "&status=", status,
            "&viewInfo=", this.model.getCurrentViewName(),
            "&itemId=", this.id,
            "&courseMembershipId=", userId,
            "&category=", this.getCategory(),
            "&itemName=", escape( this.getName() ),
            "&source=", source ? source : "cp_gradebook",
            "&mode=", mode ? mode : "invokeFromGradeCenter",
            "&anonymousMode=", anonymousMode ? anonymousMode : "false",
            "&cancelGradeUrl=", encodeURIComponent(cancelGradeUrl) );
    if ( groupAttemptId )
    {
      url += "&groupAttemptId=" + groupAttemptId;
    }
    if ( attemptId )
    {
      url += "&attemptId=" + attemptId;
    }
          
    this.postGradingForm( url );
   },

   postGradingForm: function ( url ) {
      var gcFrame = (top.content.gradecenterframe) ? top.content.gradecenterframe : top.content;
      var gradingForm = new gcFrame.Element('form',{'method':'post','action':url});
      gcFrame.document.body.insert({ bottom:gradingForm });
      var vo;
      var students = this.model.getStudents();
      var retStudents = [];
      for (var i = 0; i < students.length; i++)
      {
        vo = {};
        vo.name = GradebookUtil.formatStudentName( students[i] );
        vo.id = students[i].id;
        retStudents.push( vo );
      }
      gradingForm.insert({bottom:new gcFrame.Element('input',{'type':'hidden','name':'students','value':'{"students":'+Object.toJSON(retStudents)+'}'})});
      var items = this.model.getCurrentColDefs();
      var retItems = [];
      for (var j = 0; j < items.length; j++)
      {
          if (!items[j].isAssignment() && !items[j].isAssessment())
          {
            continue;
          }
          var txt = items[j].getName() + ' ('+items[j].getCategory() +')';
        vo = {};
        vo.name = txt;
        vo.id = items[j].getID();
        retItems.push( vo );
      }
      gradingForm.insert({bottom:new gcFrame.Element('input',{'type':'hidden','name':'items','value':'{"items":'+Object.toJSON(retItems)+'}'})});
      gradingForm.submit();
   },

  hasContextMenuInfo : function()
  {
    return true;
  },
  getDueDate : function()
  {
    var dueDate = GradebookUtil.getMessage('noneMsg');
    //ldue is the date localized in server
    if ( this.ldue && this.ldue != 0 )
    {
      dueDate = this.ldue;
    }
    return dueDate;
  },

  // called by item stats page
  getStats : function(includeUnavailableStudents)
  {

    var grades = this.model._getGradesForItemId(this.id, includeUnavailableStudents);
    if (this.primarySchema instanceof Gradebook.TextSchema)
    {
      grades = [];
    }

    var values = [];
    var sum = 0;
    var stats = {};
    stats.count = 0;
    stats.minVal = null;
    stats.maxVal = null;
    stats.qtyNull = 0;
    stats.qtyInProgress = 0;
    stats.qtyNeedsGrading = 0;
    stats.qtyExempt = 0;

    for ( var i = 0; i < grades.length; i++)
    {
      var grade = grades[i];
      if (grade.isExcluded())
      {
        continue;
      }
      var val = grade.getValue();
      var isNull = (val == '-' || val === '' || null === val );
      var isIP = grade.attemptInProgress();
      var isNG = grade.needsGrading();
      var isExempt = grade.isExempt();
      var isVal = (!isNull && !isIP && !isNG && !isExempt);
      if (isIP)
      {
        stats.qtyInProgress++;
      }
      else if (isNG)
      {
        stats.qtyNeedsGrading++;
      }
      else if (isExempt)
      {
        stats.qtyExempt++;
      }
      else if (isNull)
      {
        stats.qtyNull++;
      }

      if (isVal)
      {
        if (this.isCalculated())
        {
          val = (parseFloat(val) / parseFloat( grade.getPointsPossible() ) * 100.0);
        }
        values.push(val);
        sum += parseFloat(val);
        stats.minVal = ( null === stats.minVal ) ? val : Math.min(val, stats.minVal);
        stats.maxVal = ( null === stats.maxVal ) ? val : Math.max(val, stats.maxVal);
      }
    }
    stats.count = values.length;

    if (values.length === 0 || this.isHideAttemptScore())
    {
      stats.avg = '';
      stats.range = '';
      stats.minVal = '';
      stats.maxVal = '';
      stats.median = '';
      stats.variance = '';
      stats.stdDev = '';
    }
    else
    {
      stats.avg = sum / values.length;
      stats.range = stats.maxVal - stats.minVal;

      values.sort(Gradebook.numberComparator);
      if (values.length == 1)
      {
        stats.median = values[0];
      }
      else if (values.length % 2)
      {
        // number of values is odd, the median is the middle value
        stats.median = values[parseInt(values.length / 2, 10)];
      }
      else
      {
        // number of values is even, the median is the average of the two middle
        // values
        stats.median = (values[values.length / 2 - 1] + values[values.length / 2]) / 2;
      }
      stats.variance = this._computeVariance(values, stats.avg);
      stats.stdDev = Math.sqrt(stats.variance);

      stats.maxVal = this._formatFloat(stats.maxVal);
      stats.minVal = this._formatFloat(stats.minVal);
      stats.avg = this._formatFloat(stats.avg);
      stats.range = this._formatFloat(stats.range);
      stats.median = this._formatFloat(stats.median);
      stats.variance = this._formatFloat(stats.variance);
      stats.stdDev = this._formatFloat(stats.stdDev);
    }

    stats.gradeDistribution = this.primarySchema.getGradeDistribution(values, this.isCalculated() ? 100 : this.points, stats);
    return stats;
  },

  _formatFloat : function(f)
  {
    try
    {
      if (f)
      {
        return NumberFormatter.getDisplayFloat(f.toFixed(2));
      }
    }
    catch (e)
    {
      // ignore and return the current value
    }
    return f;
  },

  _computeVariance : function(values, average)
  {
    var sumXMeanSquare = 0;
    for ( var i = 0; i < values.length; i++)
    {
      var xMean = values[i] - average;
      sumXMeanSquare += (xMean * xMean);
    }
    return sumXMeanSquare / values.length;
  },

  getInfo : function()
  {
    var publicLabel;
    if (this.isPublic())
    {
      publicLabel = GradebookUtil.getMessage('isMsg');
    }
    else
    {
      publicLabel = GradebookUtil.getMessage('isNotMsg');
    }
    var includedInCalculationsLabel;
    if (this.isScorable())
    {
      includedInCalculationsLabel = GradebookUtil.getMessage('yesMsg');
    }
    else
    {
      includedInCalculationsLabel = GradebookUtil.getMessage('noMsg');
    }
    var points = this.getPointsForDisplay();
    var info = {};
    info.itemInfoId = this.getID();
    info.itemInfoName = this.name;
    info.itemInfoCategory = this.getCategory();
    info.itemInfoSchema = this.primarySchema.name;
    info.itemInfoPoints = (points === 0 ? "-" : points);
    info.itemInfoPublic = publicLabel;
    info.itemInfoIncludedInCalculations = includedInCalculationsLabel;
    info.itemInfoDueDate = this.getDueDate();
    return info;
  }
});

Gradebook.StudentAttributeColDef = Class.create();

Object.extend(Gradebook.StudentAttributeColDef.prototype, Gradebook.ColDef.prototype);

Object.extend(Gradebook.StudentAttributeColDef.prototype,
{
  initialize : function(jsonObj, model, schemaMap)
  {
    Gradebook.ColDef.prototype.initialize.call(this, jsonObj, model, schemaMap);
    this.vis = true;
  },

  isGrade : function()
  {
    return false;
  },

  isCalculated : function()
  {
    return false;
  },
  isTotal : function()
  {
    return false;
  },

  isWeighted : function()
  {
    return false;
  },

  getType : function()
  {
    return "student";
  },

  getCellValue : function(gridCell)
  {
    return gridCell.getValue();
  },

  getRawValue : function(newValue)
  {
    return newValue;
  },

  canHide : function()
  {
    return (this.model.colOrderMap[0] != this.model.colDefMap[this.id]);
  },

  hasContextMenuInfo : function()
  {
    return true;
  }
});
