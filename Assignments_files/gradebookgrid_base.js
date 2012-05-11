// Gradebook is grade center namespace 
var Gradebook = 
{
  getModel: function() 
  {
    try 
    {
      if (window.gbModel) 
      {
        return window.gbModel; // in case scope is GC/Course Frameset
      }
      if (parent.gbModel) 
      {
        return parent.gbModel; 
      }
      return parent.parent.gbModel;
    } 
    catch (ignore) 
    {
        return null;
    } 
  },

  clearModel: function() 
  {
    parent.gbModel = null; 
  }
};

var GradebookUtil =
{

  parseLocaleFloat : function( num )
  {
    // substitute for later calls to not have to Gradebook.getModel().getNumberFormatter()
    GradebookUtil.parseLocaleFloat = Gradebook.getModel().getNumberFormatter().parseLocaleFloat;
    return GradebookUtil.parseLocaleFloat( num );
  },
  
  toLocaleFloat : function( num )
  {
    GradebookUtil.toLocaleFloat = Gradebook.getModel().getNumberFormatter().getDisplayFloat;
    return GradebookUtil.toLocaleFloat( num );
  },
  
  round: function( num )
  {
    return Math.round( num * 100) / 100;    
  },

  error : function( errorMsg )
  {
    // firebug/IE console
    if ( console && console.error )
    {
      console.error( errorMsg );
    }     
  },

  log : function( logMsg )
  {
    // firebug/IE console
    if ( console && console.log )
    {
      console.log( logMsg );
    }     
  },
  
  isIE: function () 
  {
    return navigator.userAgent.toLowerCase().indexOf("msie") >= 0;
  },
 
  isFFonMac: function() 
  {
    return GradebookUtil.isMac() && GradebookUtil.isFirefox();
  },
 
  isFirefox: function() 
  {
    return (navigator.userAgent.toLowerCase().indexOf("firefox") != -1);
  },
 
  isMac: function() 
  {
    return (navigator.userAgent.toLowerCase().indexOf("mac") != -1);
  },
  
  getFloatLocaleFormatFromWindow: function()
  {
    var localeFloatFormat = { separator:'.', format:'^[-]?[0-9]*(\\.[0-9]+)?$' };
    if ( window.LOCALE_SETTINGS )
    {
      localeFloatFormat.separator = LOCALE_SETTINGS.getString('number_format.decimal_point');
      localeFloatFormat.format = LOCALE_SETTINGS.getString( 'float.allow.negative.format' );
    }
    else
    {
      localeFloatFormat.separator = page.bundle.getString('number_format.decimal_point');
    }
    return localeFloatFormat;
  },
  
  isValidFloat: function ( n ) 
  {
    n = '' + n;
    var trimmedVal = n.strip();
    var floatLocaleFormat = null;
    var model = Gradebook.getModel();
    if ( model && model.getFloatLocaleFormat()  )
    {
      floatLocaleFormat = model.getFloatLocaleFormat();
    }
    else
    {
      // those settings would be the settings of the page where the javascript code
      // is executed, which might not be in the same locale as the course itself
      floatLocaleFormat = this.getFloatLocaleFormatFromWindow();
    }
    if (trimmedVal.endsWith( floatLocaleFormat.separator )) 
    {
      trimmedVal += '0';
    }
    var re = new RegExp( floatLocaleFormat.format );  
    var isValidNum = trimmedVal.search( re ) === 0;
    return isValidNum;
  },
  
  isGradeValueTooBig: function ( inputValue ) 
  {
    return inputValue >= 10000000000;
  },
  
  formatStudentName: function ( student ) 
  {
    var nameTemplate = new Template(GradebookUtil.getMessage('userNameTemplate'));
    var nameData = {first:student.first, last:student.last, user:student.user};
    return nameTemplate.evaluate(nameData);
  },

  trimId: function( primaryKey )
  {
    if ( primaryKey.charAt(0) != '_' ) 
    {
      return primaryKey;
    }
    return primaryKey.slice(1, primaryKey.lastIndexOf('_') );
  },

  getMessage: function (key) {
    if ( Gradebook.getModel() ){
      return Gradebook.getModel().getMessage(key);
    } else {
      return key;
    }
  },

  getElementsComputedStyle: function ( htmlElement, cssProperty, mozillaEquivalentCSS)
  {
    if ( arguments.length == 2 )
    {
      mozillaEquivalentCSS = cssProperty;
    }

    var el = $(htmlElement);
    if ( el.currentStyle )
    {
      return el.currentStyle[cssProperty];
    }
    else
    {
      return document.defaultView.getComputedStyle(el, null).getPropertyValue(mozillaEquivalentCSS);
    }
  },

  toViewportPosition: function(element) 
  {
    return this._toAbsolute(element,true);
  },

  /**
   *  Compute the elements position in terms of the window viewport
   *  so that it can be compared to the position of the mouse (dnd)
   *  This is additions of all the offsetTop,offsetLeft values up the
   *  offsetParent hierarchy, ...taking into account any scrollTop,
   *  scrollLeft values along the way...
   *
   *  Note: initially there was 2 implementations, one for IE, one for others.
   *  Mozilla one seems to fit all though (tested XP: FF2,IE7, OSX: FF2, SAFARI)
   **/
  _toAbsolute: function(element,accountForDocScroll, topParent ) 
  {
    return this._toAbsoluteMozilla(element,accountForDocScroll,topParent);
  },

  /**
   *  Mozilla did not report all of the parents up the hierarchy via the
   *  offsetParent property that IE did.  So for the calculation of the
   *  offsets we use the offsetParent property, but for the calculation of
   *  the scrollTop/scrollLeft adjustments we navigate up via the parentNode
   *  property instead so as to get the scroll offsets...
   *
   **/
  _toAbsoluteMozilla: function(element,accountForDocScroll, topParent) 
  {
    var x = 0;
    var y = 0;
    var parent = element;
    while ( parent && ( !topParent || parent!=topParent ) ) 
    {
      x += parent.offsetLeft;
      y += parent.offsetTop;
      parent = parent.offsetParent;
    }

    parent = element;
    while ( parent &&
        parent != document.body &&
        parent != document.documentElement &&
        ( !topParent || parent!=topParent ) ) 
    {
      if ( parent.scrollLeft  )
      {
        x -= parent.scrollLeft;
      }
      if ( parent.scrollTop )
      {
        y -= parent.scrollTop;
      }
      parent = parent.parentNode;
    }

    if ( accountForDocScroll ) 
    {
      x -= this.docScrollLeft();
      y -= this.docScrollTop();
    }

    return { x:x, y:y };
  },

  docScrollLeft: function() {
    if ( window.pageXOffset )
    {
      return window.pageXOffset;
    }
    else if ( document.documentElement && document.documentElement.scrollLeft )
    {
      return document.documentElement.scrollLeft;
    }
    else if ( document.body )
    {
      return document.body.scrollLeft;
    }
    else
    {
      return 0;
    }
  },

  docScrollTop: function() 
  {
    if ( window.pageYOffset )
    {
      return window.pageYOffset;
    }
    else if ( document.documentElement && document.documentElement.scrollTop )
    {
      return document.documentElement.scrollTop;
    }
    else if ( document.body )
    {
      return document.body.scrollTop;
    }
    else
    {
      return 0;
    }
  },

  getChildElementByClassName: function(parent, childTag, childClassName)
  {
    var children = parent.getElementsByTagName(childTag);
    if (!children || children.length === 0) 
    {
      return null;
    }
    for (var i = 0; i < children.length; i++)
    {
      if (children[i].className.indexOf(childClassName) >= 0)
      {
        return children[i];
      }
    }
    return null;
  },
  
  // returns true if the text area length is less than maxLength. 
  // text area length is greater than maxLength, alerts user, sets focus to text area and returns false
  validateMaxLength : function( textArea, label, maxlength ) 
  {
    var textLength = textArea.value.length;
    if ( maxlength < textLength )
    { 
      if ( (textLength - maxlength) > 1 )
      {
        alert(JS_RESOURCES.getFormattedString('validation.maximum_length.plural', [label, maxlength, textLength - maxlength] ));
      }
      else
      {
        alert(JS_RESOURCES.getFormattedString('validation.maximum_length.singular', [label, maxlength] ));
      }
      textArea.focus();
      return false;
    }
    else
    {
      return true;
    }
  }

};


