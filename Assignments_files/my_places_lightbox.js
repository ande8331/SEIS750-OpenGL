/*
Created By: Chris Campbell
Website: http://particletree.com
Date: 2/1/2006

Inspired by the lightbox implementation found at http://www.huddletogether.com/projects/lightbox/
*/

/*-------------------------------GLOBAL VARIABLES------------------------------------*/

var detect = navigator.userAgent.toLowerCase();
var OS,browser,version,total,thestring,place;

/*-----------------------------------------------------------------------------------------------*/

//Browser detect script origionally created by Peter Paul Koch at http://www.quirksmode.org/
function checkIt(string) {
  place = detect.indexOf(string) + 1;
  thestring = string;
  return place;
}


function getBrowserInfo() {
  if (checkIt('konqueror')) {
    browser = "Konqueror";
    OS = "Linux";
  }
  else if (checkIt('safari')) { browser   = "Safari"; }
  else if (checkIt('omniweb')) { browser   = "OmniWeb"; }
  else if (checkIt('opera')) { browser     = "Opera"; }
  else if (checkIt('firefox')) { browser   = "Firefox"; }
  else if (checkIt('webtv')) { browser     = "WebTV"; }
  else if (checkIt('icab')) { browser     = "iCab"; }
  else if (checkIt('msie')) { browser     = "Internet Explorer"; }
  else if (!checkIt('compatible')) {
    browser = "Netscape Navigator";
    version = detect.charAt(8);
  }
  else { browser = "An unknown browser"; }

  if (!version) 
  {
    version = detect.charAt(place + thestring.length);
  }

  if (!OS) {
    if (checkIt('linux'))  { OS     = "Linux"; }
    else if (checkIt('x11')) { OS   = "Unix"; }
    else if (checkIt('mac')) { OS   = "Mac"; }
    else if (checkIt('win')) { OS   = "Windows"; }
    else  { OS                 = "an unknown operating system"; }
  }
}

/*-----------------------------------------------------------------------------------------------*/

var MpLightbox = Class.create();

MpLightbox.prototype = {

  yPos : 0,
  xPos : 0,
    
  initialize: function(ctrl) {
    this.myplacesLink = ctrl;
    this.content = ctrl.href;
    this.eventListnerMethod = this.activate.bindAsEventListener(this);
    Event.observe(this.myplacesLink, 'click', this.eventListnerMethod, false);
    ctrl.onclick = function(){return false;};
  },
  
  // Turn everything on - mainly the IE fixes
  activate: function(){
    
    try
    {            
      this.bigFrame = parent.content;
        this.smallFrame = null;
        if (this.bigFrame.main) // course frameset
        {
          this.bigFrame = this.bigFrame.main;
        }
        else if (this.bigFrame.WFS_Files) // content system frameset
        {
            this.smallFrame = this.bigFrame.WFS_Navigation;  
          this.bigFrame = this.bigFrame.WFS_Files;  
        }  
        
        if(this.bigFrame.document.getElementById('overlay'))
        {
          return;
        }      
      
      var head = this.bigFrame.document.getElementsByTagName('head')[0];      
            
      var doctype = this.bigFrame.document.doctype;      
      
      if((doctype && doctype.name != 'HTML' && browser == 'Firefox') || ( !doctype && browser != 'Internet Explorer'))
      {
        this.newwindow = window.open('myPlaces?newWindow=true','name','resizable=0,scrollbars=0,height=600,width=600');
        this.newwindow.focus();
        return;
      }
      else
      {  
        var addPrototype = true;      
        var scripts = $A(this.bigFrame.document.getElementsByTagName('script')).each( function (link )
            {
              if(link.src.endsWith('prototype.js'))
          {
          addPrototype = false;
          } 
            });
      
        if( addPrototype )
        {
          var proScript              = this.bigFrame.document.createElement('script');
          proScript.type     = 'text/javascript';
          proScript.src       = '/javascript/prototype.js';
          head.appendChild(proScript);    
        }
        
        var bod         = this.bigFrame.document.getElementsByTagName('body')[0];
        var html                = this.bigFrame.document.getElementsByTagName('html')[0];        
        
        var overlay       = this.bigFrame.document.createElement('div');
        overlay.id    = 'overlay';
        
        var lb          = this.bigFrame.document.createElement('div');
        lb.id        = 'lightboxWrapper';
        lb.className   = 'loading';        
        
        bod.appendChild(overlay);
        bod.appendChild(lb);

      
        if(this.smallFrame)
        {
          bod         = this.smallFrame.document.getElementsByTagName('body')[0];
          overlay       = this.smallFrame.document.createElement('div');
          overlay.id    = 'overlay';
            bod.appendChild(overlay);
        }
        
        this.getScroll();
        this.setScroll(0,0);
        if (browser == 'Internet Explorer'){
          this.hideSelects('hidden');
        }
        this.displayLightbox("block");
      }
    }
    catch(err)
    {      
      this.newwindow = window.open('myPlaces?newWindow=true','name','resizable=0,scrollbars=0,height=600,width=600');      
      this.newwindow.focus();
      return;
    }  

  },
  
  // In IE, select elements hover on top of the lightbox
  hideSelects: function(visibility){
    var selects = this.bigFrame.document.getElementsByTagName('select');
    for( var i = 0; i < selects.length; i++) {
      selects[i].style.visibility = visibility;
    }
  },
  
  // Taken from lightbox implementation found at http://www.huddletogether.com/projects/lightbox/
  getScroll: function()
  {
    if (this.bigFrame.pageYOffset) {
      this.yPos = this.bigFrame.document.pageYOffset;
    } else if (this.bigFrame.document.documentElement && parent.content.document.documentElement.scrollTop){
      this.yPos = this.bigFrame.document.documentElement.scrollTop; 
    } else if (this.bigFrame.document.body) {
      this.yPos = this.bigFrame.document.body.scrollTop;
    }
  },
  
  setScroll: function(x, y){
    this.bigFrame.scrollTo(x, y); 
  },
  
  displayLightbox: function(display){
    this.bigFrame.document.getElementById('overlay').style.display = display;
    this.bigFrame.document.getElementById('lightboxWrapper').style.display = display;    
    if(this.smallFrame)
    {
      this.smallFrame.document.getElementById('overlay').style.display = display;  
    }
    if(display != 'none') 
    {
      this.loadInfo();
    }
  },
  
  // Begin Ajax request based off of the href of the clicked linked
  loadInfo: function() {
    new Ajax.Request('/webapps/portal/execute/myPlaces', {
      onSuccess: function(transport, json) {            
      var result = transport.responseText.evalJSON( true );      
      if ( result.success == "true" )
      {        
      this.bigFrame.document.getElementById('lightboxWrapper').innerHTML = result.myPlacesContent;
      var myPlacesContentDiv = this.bigFrame.document.getElementById('myPlacesContent');
      this.firstLink = myPlacesContentDiv.getElementsByTagName('a')[0];
	  this.firstLink.focus();      
      //set all tab groups to be inactive
      var active = parent.nav.document.getElementsByClassName('active');
      for( var i = 0; i < active.length; i++) 
      {
        this.activeTab = active[0];
        active[i].removeClassName('active');
      }         
      this.actions();
      }
     else
     {
       new page.InlineConfirmation("error", result.errorMessage, false ); 
     }       
    }.bind(this)
  });
    
   },   
  
  // Display Ajax response
  processInfo: function(response){
    var info = "<div id='lbContent'><div class='lbContainer'>" + response.responseText + "</div></div>";
    $('lbLoadMessage').insert({before: info});
    $('lightbox').className = "done";  
    this.actions();      
  },
  
  // Search through new links within the lightbox, and attach click event
  actions: function(){
	  var lbAction = this.lastLink = this.bigFrame.document.getElementById('lbAction');
    Event.observe(lbAction, 'click', this.deactivate.bindAsEventListener(this), false);
    lbAction.onclick = function(){return false;};
    this.bigFrame.document.getElementById('lightboxWrapper').observe( 'keydown', this.onKeyPress.bindAsEventListener( this ) );
  },
  
  // Example of creating your own functionality once lightbox is initiated
  insert: function(e){
     var link = Event.element(e).parentNode;
     Element.remove($('lbContent'));
   
     var myAjax = new Ajax.Request(
        link.href,
        {method: 'post', parameters: "", onComplete: this.processInfo.bindAsEventListener(this)}
     );
   
  },
  
  onKeyPress: function( event ) 
  {
	var key = event.keyCode || event.which;
	var elem = event.element();
	// Close on ESC type
	if ( key == Event.KEY_ESC )
	{
	  this.deactivate();
	  event.stop();
	}
	// Set up the tab loop (don't tab/shift-tab out of the lb)
	else if ( key == Event.KEY_TAB && !event.shiftKey && elem == this.lastLink )
	{
	  this.firstLink.focus();
	  event.stop();
	} 
	else if ( key == Event.KEY_TAB && event.shiftKey && elem == this.firstLink )
	{
	  this.lastLink.focus();
	  event.stop();
	}	     
  },
  
  // Example of creating your own functionality once lightbox is initiated
  deactivate: function(){
    if(this.smallFrame)
    {
      this.smallFrame.document.getElementById('overlay').style.display='none';
    }
    
    this.bigFrame.$('lightboxWrapper').remove();
    this.bigFrame.$('overlay').remove();        
    this.setScroll(0,this.yPos);
    if (browser == "Internet Explorer"){
      this.hideSelects("visible");
    }
    
    if(this.activeTab) 
    {
      this.activeTab.addClassName('active');
    }
  }
};

/*-----------------------------------------------------------------------------------------------*/

// Onload, make all links that need to trigger a lightbox active
function initialize(){
  var lbox = document.getElementsByClassName('myPlaces');
  for( var i = 0; i < lbox.length; i++) {
    new MpLightbox(lbox[i]);
  }
}

Event.observe(window, 'load', initialize, false);
Event.observe(window, 'load', getBrowserInfo, false);