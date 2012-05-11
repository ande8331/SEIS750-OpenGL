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

Gradebook.NumericSchema = Class.create();
Gradebook.NumericSchema.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
  },
  
  getGradeDistribution : function(grades, points, stats)
  {
    return Gradebook.PercentageSchema.prototype.getGradeDistribution(grades, points, stats);
  },
  
  // called by ColDef.getCellValue to get value for spreadsheet
  getCellValue : function(gridCell)
  {
    // for grades, we show a precision up to 5, otherwise show the historic 2 digits precision
    var maxPrecision = ( gridCell.colDef.isCalculated()?2:5 ); 
    return this.getDisplayValue( gridCell.getValue(), gridCell.getPointsPossible(), maxPrecision );
  },

  // this is the value that appears in the input box when editing
  getEditValue : function(gridCell)
  {
    return this.getCellValue(gridCell);
  },

  getSortValue : function(gridCell)
  {
    return gridCell.getValue();
  },

  // called by: this.getCellValue to get value for spreadsheet or
  // by colDef.getDisplayValue when external pages need to convert a rawValue
  getDisplayValue : function(rawValue, points, maxPrecision )
  {
    if (rawValue == '-' || rawValue.length === 0)
    {
      return rawValue;
    }
    if ( 2 == maxPrecision )
    {
      return NumberFormatter.getDisplayFloat( parseFloat(rawValue).toFixed(2) );
    }
    else
    {
      if ( !maxPrecision )
      {
        maxPrecision = 5;
      }
      return NumberFormatter.toStringMin2Digits( parseFloat( rawValue ), maxPrecision );
    }
  },

  getRawValue : function(displayValue, colDef)
  {
    return NumberFormatter.getDotFloat(displayValue);
  },

  validate : function(newValue, matchPartial)
  {
    if (!newValue || newValue == "0" || newValue == "-")
    {
      return null;
    }
    if (!GradebookUtil.isValidFloat( newValue ))
    {
      return GradebookUtil.getMessage('invalidNumberErrorMsg');
    }
    if ( GradebookUtil.isGradeValueTooBig( newValue ) )
    {
      return GradebookUtil.getMessage('gradeValueTooBigErrorMsg');
    }
    var val = '' +newValue;
    var idx = val.indexOf('.');
    if (idx > -1 && (val.length - idx - 1) > 4)
    {
      return GradebookUtil.getMessage('tooManyDecimalPlacesErrorMsg');
    }
    else
    {
      return null;
    }
  }
};

Gradebook.TextSchema = Class.create();
Gradebook.TextSchema.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
},

getGradeDistribution : function(grades, points, stats)
{
  return null;
},

// this is the value that appears in the input box when editing
  getEditValue : function(gridCell)
  {
    return this.getCellValue(gridCell);
  },

  // called by ColDef.getCellValue to get value for spreadsheet
  getCellValue : function(gridCell)
  {
    return this.getDisplayValue(gridCell.getTextValue(), gridCell.getPointsPossible());
  },

  getSortValue : function(gridCell)
  {
    return gridCell.getTextValue().toUpperCase();
  },
  
  // called by: this.getCellValue to get value for spreadsheet or
  // by colDef.getDisplayValue when external pages need to convert a rawValue
  getDisplayValue : function(rawValue, points)
  {
    return rawValue;
  },

  getRawValue : function(displayValue, colDef)
  {
    return displayValue;
  },

  validate : function(newValue, matchPartial)
  {
    // is any value bad?
  return null;
}

};

Gradebook.PercentageSchema = Class.create();
Gradebook.PercentageSchema.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
  },
  
  // called by ColDef.getStats
  getGradeDistribution : function(grades, points, stats)
  {
    var i, len;
    var dist = [];
    var range = [];
    range.count = 0;
    range.text = 'less than 0';
    dist.push(range);
    for (i = 0; i < 10; i++)
    {
      range = [];
      range.count = 0;
      range.low = (i * 10);
      range.high = (i * 10) + ((i < 9) ? 9 : 10);
      range.text = range.low + ' - ' + range.high;
      dist.push(range);
    }
    range = [];
    range.count = 0;
    range.text = 'greater than 100';
    dist.push(range);
    for (i = 0, len = grades.length; i < len; i++)
    {
      var percent = (points) ? (parseFloat(grades[i]) / parseFloat(points) * 100.0) : parseFloat(grades[i]);
      if (percent == 100)
      {
        percent -= 0.1; // 100 should fall into 90-100 bin
      }
      var index = parseInt(percent / 10.0, 10) + 1;
      if (percent < 0)
      {
        index = 0;
      }
      if (percent > 100)
      {
        index = 11;
      }
      dist[index].count++;
    }
    dist.reverse();
    return dist;
  },

  // called by ColDef.getCellValue to get value for spreadsheet
  getCellValue : function(gridCell)
  {
    return this.getDisplayValue(gridCell.getValue(), gridCell.getPointsPossible());
  },

  // this is the value that appears in the input box when editing
  getEditValue : function(gridCell)
  {
    return this.getCellValue(gridCell);
  },

  getSortValue : function(gridCell)
  {
    return gridCell.getNormalizedValue();
  },

  // called by: this.getCellValue to get value for spreadsheet or
  // by colDef.getDisplayValue when external pages need to convert a rawValue
  getDisplayValue : function(rawValue, points)
  {
    if (parseFloat(points) === 0.0 || rawValue == '-' || rawValue.length === 0)
    {
      return rawValue;
    }
    var percent = parseFloat(rawValue) / parseFloat(points) * 100.0;
    return NumberFormatter.getDisplayFloat(parseFloat(percent).toFixed(2)) + '%';
  },

  getRawValue : function(displayValue, colDef)
  {
    var points = (colDef.points) ? colDef.points : 100;
    displayValue = displayValue.replace('%', '');
    displayValue = NumberFormatter.getDotFloat(displayValue);
    return parseFloat(displayValue) / 100.0 * parseFloat(points);
  },

  validate : function(newValue, matchPartial)
  {
    newValue = newValue.replace('%', '');
    if (!newValue || newValue == "0" || newValue == "-")
    {
      return null;
    }
    if (!GradebookUtil.isValidFloat( newValue ))
    {
      return GradebookUtil.getMessage('invalidNumberErrorMsg');
    }
    if ( GradebookUtil.isGradeValueTooBig( newValue ) )
    {
      return GradebookUtil.getMessage('gradeValueTooBigErrorMsg');
    }
    var val = '' +newValue;
    var idx = val.indexOf('.');
    if (idx > -1 && (val.length - idx - 1) > 4)
    {
      return GradebookUtil.getMessage('tooManyDecimalPlacesErrorMsg');
    }
    else
    {
      return null;
    }
  }

};

Gradebook.CompleteIncompleteSchema = Class.create();
Gradebook.CompleteIncompleteSchema.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
},

// called by ColDef.getStats
  getGradeDistribution : function(grades, points, stats)
  {
    var dist =
      [];
    var range =
      [];
    range.count = stats.qtyNull;
    range.text = 'Incomplete';
    dist.push(range);
    range =
      [];
    range.count = grades.length;
    range.text = 'Complete';
    dist.push(range);
    dist.reverse();
    return dist;
  },

  // called by ColDef.getCellValue to get value for spreadsheet
  getCellValue : function(gridCell)
  {
    return this.getDisplayValue(gridCell.getTextValue(), gridCell.getPointsPossible());
  },

  // this is the value that appears in the input box when editing
  getEditValue : function(gridCell)
  {
    return gridCell.getValue();
  },

  getSortValue : function(gridCell)
  {
    var tv = gridCell.getTextValue().toUpperCase();
    if (tv == '-')
    {
      return '-';
    }
    else
    {
      return gridCell.getValue();
    }
  },

  // called by: this.getCellValue to get value for spreadsheet or
  // by colDef.getDisplayValue when external pages need to convert a rawValue
  getDisplayValue : function(rawValue, points)
  {
    if (rawValue != '-' && rawValue.length > 0)
    {
      return '<img border="0" width="16" height="16" src="/images/ci/icons/checkmark_ia.gif" alt="' + GradebookUtil.getMessage('completedMsg') + '">';
    }
    else
    {
      return '-';
    }
  },

  getRawValue : function(displayValue, colDef)
  {
    return displayValue;
  },

  validate : function(newValue, matchPartial)
  {
    if (!newValue || newValue == "0" || newValue == "-")
    {
      return null;
    }
    // todo: determine what is allowed. I.E. is "-" allowed?
    // allow empty string or number
    // return (newValue.length == 0 || parseFloat(newValue));
    if (!GradebookUtil.isValidFloat(newValue))
    {
      return GradebookUtil.getMessage('invalidNumberErrorMsg');
    }
    else
    {
      return null;
    }
  }
};

Gradebook.LetterSchema = Class.create();
Gradebook.LetterSchema.prototype =
{
  initialize : function(jsonObj, model)
  {
    this.model = model;
    Object.extend(this, jsonObj); // assign json properties to this object
},

// called by ColDef.getStats
  getGradeDistribution : function(grades, points, stats)
  {
    var dist = [];
    var symMap = [];
    this.symbols.each(function(s)
    {
      var range =
        [];
      range.count = 0;
      range.text = s.sym;
      symMap[s.sym] = dist.length;
      dist.push(range);
    });
    for ( var i = 0, len = grades.length; i < len; i++)
    {
      var val = this.getDisplayValue(grades[i], points);
      var index = symMap[val];
      if (index)
      {
        dist[index].count++;
      }
    }
    return dist;
  },

  // called by ColDef.getCellValue to get value for spreadsheet
  getCellValue : function(gridCell)
  {
    return this.getDisplayValue(gridCell.getValue(), gridCell.getPointsPossible());
  },

  // this is the value that appears in the input box when editing
  getEditValue : function(gridCell)
  {
    return this.getCellValue(gridCell);
  },

  getSortValue : function(gridCell)
  {
    return gridCell.getNormalizedValue();
  },

  // called by: this.getCellValue to get value for spreadsheet or
  // by colDef.getDisplayValue when external pages need to convert a rawValue
  getDisplayValue : function(rawValue, points)
  {

    if (parseFloat(points) === 0.0 || rawValue == '-' || rawValue.length === 0)
    {
      return rawValue;
    }
    if ( isNaN( rawValue ) )
    {
      // see if raw value is one of the symbols
      var matchingSymbol;
      rawValue = rawValue.toUpperCase();
      this.symbols.each(function(s)
      {
        if (rawValue == s.sym.toUpperCase())
        {
          matchingSymbol = s.sym;
          throw $break; // needed to get out of each loop
        }
      });
      if (matchingSymbol)
      {
        return matchingSymbol;
      }
      return rawValue;
    }
    var retVal = rawValue;
    var percent = GradebookUtil.round( parseFloat(rawValue) / parseFloat(points) * 100.0 );
    this.symbols.each(function(s)
    {
      if (percent >= s.lb && percent <= s.ub)
      {
        retVal = s.sym;
        throw $break; // needed to get out of each loop
      }
    });
    return retVal;
  },

  getRawValue : function(displayValue, colDef)
  {

    // What it SHOULD be doing is:
    // Column created with Letter as primary display and secondary display of % -
    // worth 10 points
    // Enter A - go to schema and determine that A = 95% use 95% to determine
    // score of 9.5 - store 9.5 and display A
    // Enter 9 - determine the 9 is 90% (item is out of 10) 90% is an A - store 9
    // and display A
  
    var points = (colDef.points) ? colDef.points : 100;
    displayValue = '' + displayValue;
    displayValue = displayValue.replace('%', '');
    var score = displayValue.toUpperCase();
    this.symbols.each(function(s)
    {
      if (score == s.sym.toUpperCase())
      {
        score = (parseFloat(s.abs) / 100.0) * points;
        throw $break; // needed to get out of each loop
      }
    });
    return score;
  },

  validate : function(newValue, matchPartial)
  {
    if (!newValue || newValue == "0" || newValue == "-")
    {
      return null;
    }
    // allow numeric value for letter schemas too
    if (GradebookUtil.isValidFloat(newValue))
    {
      return null;
    }
    var retVal = GradebookUtil.getMessage('invalidLetterErrorMsg');
    newValue = newValue.toUpperCase();
    this.symbols.each(function(s)
    {
      if (newValue == s.sym.toUpperCase() || (matchPartial && s.sym.toUpperCase().startsWith(newValue)))
      {
        retVal = null;
        throw $break; // needed to get out of each loop
      }
    });
    return retVal;
  }
};
