/*

  GridCell class wraps and provides functionality to a data cell in the grade center.
  
  Each HTML cell controller will contain a GridCell to allow manipulating the data cell 
  that is currently assigned to it.
  When data cells are retrieved for processing they are wrapped in a GridCell.
 
  brichard
*/


Gradebook.GridCell = Class.create();

Gradebook.GridCell.prototype =
{
  initialize : function(data)
  {
    if (data)
    {
      this.setData(data);
    }
  },

  setData : function(data)
  {
    this.data = data;
    this.colDef = data.colDef;
    this.metaData = data.metaData;
    if (this.colDef.id == 'UN')
    {
      this.metaData.userNameDataCell = data;
    }
  },

  passesFilter : function(f)
  {
    var ng = this.needsGrading();
    var ip = this.attemptInProgress();
    var or = this.isOverride();
    var x = this.isExempt();
    var sv = this.getValue(this);
    var svn = sv == '-';
    var svnn = sv != '-';
    var na = svn && !ip && !ng && !this.colDef.isCalculated() && !this.isExcluded();
    var c = !ip && !ng && !x && svnn;
    var nn = ip || ng || or || svnn;
    if (f == 'IP')
    {
      return ip;
    }
    else if (f == 'NG')
    {
      return ng && !or;
    }
    else if (f == 'EM')
    {
      return or;
    }
    else if (f == 'X')
    {
      return x;
    }
    else if (f == "NA")
    {
      return na; // notAttempted
    }
    else if (f == "NN")
    {
      return nn; // not null
    }
    else if (f == "C")
    {
      return c || or; // completed/graded
    }
    else
    {
      return true; // all
    }
  },

  getUserId : function()
  {
    return this.metaData.uid;
  },

  getKey : function()
  {
    return this.colDef.id + '_' + this.metaData.uid;
  },

  getUserName : function()
  {
    return this.metaData.userNameDataCell.v;
  },

  isHidden : function()
  {
    return this.metaData.isHidden;
  },

  setHidden : function(h)
  {
    this.metaData.isHidden = h;
  },

  isRowChecked : function()
  {
    return this.metaData.isRowChecked;
  },

  canAddComment : function()
  {
    // Can add comments for: overridden, exempted, or graded cells
    return this.isOverride() || this.isExempt() || 
                        ( this.isPersisted() && !this.needsGrading() &&
                          !this.attemptInProgress() && this.isGraded() && !this.colDef.isAttemptAverage() );
  },

  isActivity : function()
  {
    return this.colDef.src && !this.colDef.getScoreProvider().attemptBased;
  },

  hasGradableAttempts : function()
  {
    return (!this.isActivity() && (this.colDef.src || this.colDef.extAttemptHandler) && 
        (this.hasAttempts() || this.data.ax /* has exempted attempt */));
  },

  isExcluded : function()
  {
    return this.data.excluded || (this.colDef.limitedAttendance && !this.isPersisted());
  },

  isPersisted : function()
  {
    return ("v" in this.data);
  },

  setRowChecked : function(c)
  {
    this.metaData.isRowChecked = c;
  },

  isAvailable : function()
  {
    return this.metaData.isAvailable;
  },

  isGrade : function()
  {
    return (this.colDef.isGrade());
  },

  isOverride : function()
  {
    return (this.data.or && this.data.or == "y" && !this.colDef.isCalculated());
  },

  isOverridingAttempts: function()
  {
    return this.isOverride( ) && this.hasAttempts( );
  },
  
  /*
   * Did the override occurred before the attempt creation? If so
   * we will show the needs grading icon.
   */
  isOverrideBeforeNeedsGrading: function()
  {
    return ( this.data.orBefAtt && this.data.orBefAtt == "y" );
  },
  
  needsGrading : function()
  {
    return (this.data.ng && this.data.ng && this.data.ng == "y");
  },

  attemptInProgress : function()
  {
    return (this.data.ip && this.data.ip && this.data.ip == "y");
  },

  isGraded : function()
  {
    var tv = this.getTextValue();
    return (tv != '-' && tv.length > 0);
  },

  isComplete : function()
  {
    if (this.colDef.primarySchema instanceof Gradebook.CompleteIncompleteSchema)
    {
      return this.isGraded();
    }
    else
    {
      return false;
    }
  },

  isExempt : function()
  {
    return (this.data.x && this.data.x == "y");
  },

  hasMultipleAttempts : function()
  {
    return (this.data.numAtt && this.data.numAtt == "M");
  },

  hasOneAttempt : function()
  {
    return (!this.data.numAtt || this.data.numAtt == "1");
  },

  hasAttempts : function()
  {
    return this.hasOneAttempt() || this.hasMultipleAttempts();
  },

  validate : function(newValue, matchPartial)
  {
    return this.colDef.validate(newValue, matchPartial);
  },
  
  getColumnDefinition: function( )
  {
    return this.colDef;
  },
  
  update : function(newValue)
  {
    this.colDef.updateGrade(newValue, this.getUserId());
  },

  clearAll : function(isDelete)
  {
    this.colDef.model.clearAll(isDelete, this.getUserId(), this.colDef.id);
  },

  clearSelected : function(attemptIds, isDelete)
  {
    this.colDef.model.clearSelected(attemptIds, isDelete, this.getUserId(), this.colDef.id);
  },

  // called by CellController.renderHTML to get value for spreadsheet
  getCellValue : function()
  {
    return this.colDef.getCellValue(this);
  },

  // called by GridCell.getAltValue to get alt (mouse over) value for rendering
  // in spreadsheet
  getAltValue : function()
  {
    if (this.isGrade() && !this.isGraded())
    {
      return GradebookUtil.getMessage('noGradeMsg');
    }
    return this.colDef.getAltValue(this);
  },

  // called by CellController.startEdit to get input value for editing
  getEditValue : function()
  {
    if ( !this.isGraded() )
    {
      return "";
    }
    return this.colDef.getEditValue(this);
  },

  getSortValue : function()
  {
    return this.colDef.getSortValue(this);
  },
  
  getNormalizedValue: function()
  {
    if ( this.data.v !== undefined && this.data.v !== null  && this.getPointsPossible() )
    {
      return this.data.v / this.getPointsPossible();
    }
    return NaN;
  },

  getPointsPossible : function()
  {
    if (this.data.mp)
    {
      return this.data.mp;
    }
    else if (this.colDef.points)
    {
      return this.colDef.points;
    }
    else
    {
      return 0;
    }
  },

  getTextValue : function()
  {
    if (this.data.tv)
    {
      return this.data.tv;
    }
    else
    {
      return '-';
    }
  },

  getValue : function()
  {
    // do not use if ( this.data.v ) since it will prevent 0 to display properly 
    if ( this.data.v !== undefined && this.data.v !== null )
    {
      return this.data.v;
    }
    else
    {
      return '-';
    }
  },

  getNormalizedGrade: function() 
  {
    if (this.data.v !== undefined && this.data.v !== null)
    {
      var pointsPossible = this.getPointsPossible();
      if ( pointsPossible > 0 )
      {
        return this.data.v / pointsPossible;
      }
    }
    return null;
  },
    
  canEdit : function()
  {
    return (this.isGrade() && !this.isExcluded() && !this.colDef.isCalculated() && !this.colDef.isHideAttemptScore());
  },

  loadAttemptsInfo : function(callbackFunction)
  {
    var currentCell = this;
    this.colDef.model.gradebookService.loadAttemptsInfo(this.getUserId(), this.colDef.id, function(attempts)
    {
      currentCell.loadAttemptsInfoCallback.call(currentCell, attempts, callbackFunction);
    });
  },

  loadAttemptsInfoCallback : function(attempts, callbackFunction)
  {
    this.data.attemptsInfo =
      [];
    for ( var i = 0; i < attempts.length; ++i)
    {
      this.data.attemptsInfo.push(new Gradebook.AttemptInfo(this, attempts[i]));
    }
    callbackFunction(this);
  },

  getMenuDynItems : function()
  {
    var dynItems = [];
    var gradeCell = this;
    for ( var i = 0; i < this.data.attemptsInfo.length; ++i)
    {
      var attemptId = gradeCell.data.attemptsInfo[i].id;
      var groupAttemptId = gradeCell.data.attemptsInfo[i].groupAttemptId;
      // note that we cannot create a function as a direct closure here
      // since it would rely on this function scope which actually changes
      // as we iterate i.e. all functions will point to the same scope which
      // ends
      // up being the scope as at the last iteration.
      // To 'freeze' the scope we create a new local scope calling another
      // function using current parameters.
      //Don't display the 'not_attempted' attempts
      var attemptInfo = this.data.attemptsInfo[ i ];
      if ( attemptInfo.status != "na" )
      {
        dynItems.push(
        {              
          id : "attemptDynItem",
          name : attemptInfo.getText(),
          onclick : this.getGotoAttemptFunction( attemptId, groupAttemptId )
        } );
      }
    }
    
    return dynItems;
  },

  getGotoAttemptFunction : function(attemptId, groupAttemptId)
  {
    var gradeCell = this;
    var currentAttemptId = attemptId;
    var currentGroupAttemptId = groupAttemptId;
    return function()
    {
      gradeCell.gotoAttempt.call(gradeCell, attemptId, groupAttemptId);
    };
  },

  gotoAttempt : function(attemptId, groupAttemptId)
  {
    if (this.colDef.groupActivity && !groupAttemptId)
    {
      this.showGradeDetails();
      return;
    }
    
    this.colDef.gradeAttempt( this.getUserId(), attemptId, false, groupAttemptId );
  },

  gotoActivity : function()
  {
    this.gotoAttempt();
  },

  hasContextMenuInfo : function(cellController)
  {
    if (this.isGrade())
    {
      return !this.isExcluded() && !this.colDef.isCalculated();
    }
    else
    {
      return true;
    }
  },
  
  hideUser : function()
  {
    this.colDef.model.updateUserVisibility( this.getUserId(), false );
  }
  
};

Gradebook.AttemptInfo = Class.create();

Object.extend(Gradebook.AttemptInfo.prototype,
{

  initialize : function(gradeCel, attemptData)
  {
    this.gradeCel = gradeCel;
    this.id = attemptData.id;
    this.date = attemptData.date;
    this.score = attemptData.score;
    this.status = attemptData.status;
    this.exempt = attemptData.exempt;
    if (attemptData.groupAttemptId)
    {
      this.groupAttemptId = attemptData.groupAttemptId;
      this.groupName = attemptData.groupName;
      this.override = attemptData.override;
      this.groupScore = attemptData.groupScore;
      this.groupStatus = attemptData.groupStatus;
    }
  },

  getScoreDisplayValue : function()
  {
    if (this.status)
    {
      if (this.status == "ip")
      {
        return this.gradeCel.colDef.model.gridImages.attemptInProgress;
      }
      return this.gradeCel.colDef.model.gridImages.needsGrading;
    }
    var primaryValue = this.gradeCel.colDef.getDisplayValue(this.score);
    var secondaryValue = this.gradeCel.colDef.getSecondaryDisplayValue(this.score);
    if (secondaryValue)
    {
      primaryValue += " (" + secondaryValue + ")";
    }
    return primaryValue;    
  },

  getGroupScoreDisplayValue : function()
  {
    if (this.groupStatus)
    {
      if (this.groupStatus == "ip")
      {
        return this.gradeCel.colDef.model.gridImages.attemptInProgress;
      }
      return this.gradeCel.colDef.model.gridImages.needsGrading;
    }
    var primaryValue = this.gradeCel.colDef.getDisplayValue(this.groupScore);
    var secondaryValue = this.gradeCel.colDef.getSecondaryDisplayValue(this.groupScore);
    if (secondaryValue)
    {
      primaryValue += " (" + secondaryValue + ")";
    }
    return primaryValue;
  },

  getText : function()
  {
    var exemptIcon = "";
    if (this.exempt)
    {
      var altText = this.gradeCel.colDef.model.getMessage('exemptAttemptMsg');
      exemptIcon = "<img src='/images/ci/gradebook/exempt.gif' alt='" + altText + "' title='" + altText + "'>";
    }
    if (!this.groupAttemptId)
    {
      if (!Gradebook.GridCell.attemptTemplate)
      {
        Gradebook.GridCell.attemptTemplate = new Template(this.gradeCel.colDef.model.getMessage('attemptInfoMsg'));
      }
      return Gradebook.GridCell.attemptTemplate.evaluate(
      {
        date : this.date,
        score : this.getScoreDisplayValue(),
        exempt : exemptIcon
      });
    }
    if (!this.override)
    {
      if (!Gradebook.GridCell.groupAttemptTemplate)
      {
        Gradebook.GridCell.groupAttemptTemplate = new Template(this.gradeCel.colDef.model.getMessage('groupAttemptInfoMsg'));
      }
      return Gradebook.GridCell.groupAttemptTemplate.evaluate(
      {
        date : this.date,
        score : this.getScoreDisplayValue(),
        groupName : this.groupName,
        exempt : exemptIcon
      });
    }
    if (!Gradebook.GridCell.groupAttemptOverrideTemplate)
    {
      Gradebook.GridCell.groupAttemptOverrideTemplate = new Template(this.gradeCel.colDef.model.getMessage('groupAttemptInfoWithOverrideMsg'));
    }
    return Gradebook.GridCell.groupAttemptOverrideTemplate.evaluate(
    {
      date : this.date,
      score : this.getScoreDisplayValue(),
      groupName : this.groupName,
      groupScore : this.getGroupScoreDisplayValue(),
      exempt : exemptIcon
    });
  }

});
