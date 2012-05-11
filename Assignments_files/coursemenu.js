var courseMenu = {};

courseMenu.Util = {};

courseMenu.Util.getTocItemTemplate = function( type, isHidden, isEmpty, menuGeneratorUrl )
{
    var template = '';
    var url = ( menuGeneratorUrl ) ? menuGeneratorUrl : '/webapps/blackboard/execute/getCourseMenuContextMenu';
    var closeStr = page.bundle.getString("closeStr");
    var moreOptionsStr = page.bundle.getString("moreOptionsStr");
    var contextMenuTemplate = 
      "<span class='contextMenuContainer' bb:menuGeneratorURL='" + url + "' bb:contextParameters='course_id=<&= courseId &>&toc_id=<&= id &>'>" +  
      "<a class='cmimg editmode' href='#contextMenu' title='" + moreOptionsStr + "'><img src='/images/ci/icons/cm_arrow.gif' alt='" + moreOptionsStr + "'></a> " +
      "<div id='menudiv' class='cmdiv' style='display:none;'> "+
       "<ul><li class='contextmenubar_top'><a title='" + closeStr + "' href='#close'><img alt='" + closeStr + "' src='/images/ci/ng/close_mini.gif'></a></li></ul></div></span>";

    if ( type == 'DIVIDER' )
    {
      template = "<li class='divider' id='paletteItem:<&= id &>'><span class='reorder editmode'>&nbsp;</span><hr>";
    }
    else if ( type == 'SUBHEADER' )
    {
      template = "<li class='subhead' id='paletteItem:<&= id &>'><span class='reorder editmode'>&nbsp;</span><h3><span><&= name &></span></h3>";
    }
    else
    {
      var liClass = (isHidden || isEmpty) ? 'clearfix invisible editmode' : 'clearfix'; 
      template = "<li id='paletteItem:<&= id &>' class='"+ liClass +"'><span class='reorder editmode'>&nbsp;</span>";
       if (type == 'URL')
       {
         template += "<a onClick=\"this.href='<&= href &>';\" href='<&= url &>' target='<&= target &>' id='label:<&= id &>'><span title='<&= name &>'><&= name &></span>";
       }
       else
       {
         template += "<a href='<&= href &>' target='<&= target &>' id='label:<&= id &>'><span title='<&= name &>'><&= name &></span>";   
       }
      
      if ( isHidden )
      {
        var hiddenStr = page.bundle.getString("hiddenStr");
        template += "<span class='cmLink-hidden' title='" + hiddenStr + "'><img src='/images/spacer.gif' alt='" + hiddenStr + "'/></span>";
      }
      if ( isEmpty )
      {
        var emptyStr = page.bundle.getString("emptyStr");
        template += "<span class='cmLink-empty' title='" + emptyStr + "'><img src='/images/spacer.gif' alt='" + emptyStr + "'/></span>";
      }
      template += "</a>";
    }
    return template + contextMenuTemplate;
};

/**
 * Controls the dynamic course menu behaviour.
 */
courseMenu.CourseMenu = Class.create();
courseMenu.CourseMenu.prototype = 
{
  initialize: function( menuActionUrl, menuGeneratorUrl )
  {
    this.menuActionUrl = '/webapps/blackboard/execute/doCourseMenuAction';
    if( menuActionUrl )
    {
      this.menuActionUrl= menuActionUrl;
    }
    
    this.menuGeneratorUrl = '/webapps/blackboard/execute/getCourseMenuContextMenu';
    if( menuGeneratorUrl ) 
    {
      this.menuGeneratorUrl = menuGeneratorUrl;
    }
    var addButton = $('addCmItem'); // open the flyout form relative to this element
    this.editMode = false;
    if ( addButton )
    {
        this.editMode = true;
      var addNewMenuItemFunc = this._addNewMenuItem.bind( this );
      var afterModifyItemFunc = this._afterModifyItem.bind( this );
      
      if( $('pickerLink') )
      {
        Event.observe( 'pickerLink', 'click', this._showCourseLinkPicker.bindAsEventListener( this ) );
      }

      if( $('modifyCourseLinkPickerLink') )
      {
        Event.observe( 'modifyCourseLinkPickerLink', 'click', this._showModifyCourseLinkPicker.bindAsEventListener( this ) );
      }
      
        var addButtonLink = $(addButton.getElementsByTagName('a')[0]);       
        
    // Initialize flyout forms
    new flyoutform.FlyoutForm({  linkId:'addContentAreaButton',
                  formDivId:'addContentAreaForm',
                  openRelativeItem:addButton,
                  customPostSubmitHandler:addNewMenuItemFunc,
                  onCloseFocusItem:addButtonLink });
    
    // Initialize flyout forms
    if ( $s('addBlankPageButton') )
    {
      new flyoutform.FlyoutForm({  linkId:'addBlankPageButton',
                    formDivId:'addBlankPageForm',
                    openRelativeItem:addButton,
                    customPostSubmitHandler:addNewMenuItemFunc,
                    onCloseFocusItem:addButtonLink });
    }
                  
    new flyoutform.FlyoutForm({  linkId:'addToolLinkButton',
                  formDivId:'addToolLinkForm',
                  openRelativeItem:addButton,
                  customPostSubmitHandler:addNewMenuItemFunc,
                  onCloseFocusItem:addButtonLink });
                  
    if( $('addToolLinkButton') )
    {
      Event.observe( 'addToolLinkButton', 'click', function()
      {
        setTimeout( function()
        {
          var toolSelect = $('toolSelect');
          if ( toolSelect )
          {
            // dynamically load the tool select if not already done
              if ( toolSelect.length === 0)
            {
                CourseMenuDWRFacade.getCourseTools( window.course_id, function( tools )
              {  
                dwr.util.addOptions( "toolSelect", tools);
              });
            }
            toolSelect.selectedIndex = 0;
          }
        }, 100); // wait for the flyout to open before loading
      });
    }

    if ( $s('addCourseLinkButton' ) )
    {  
      new flyoutform.FlyoutForm({  linkId:'addCourseLinkButton',
                    formDivId:'addCourseLinkForm',
                    openRelativeItem:addButton,
                    customPostSubmitHandler:addNewMenuItemFunc,
                    onCloseFocusItem:addButtonLink });
    }
    if ( $s('addExternalLinkButton' ) )
    {  
      new flyoutform.FlyoutForm({  linkId:'addExternalLinkButton',
                    formDivId:'addExternalLinkForm',
                    openRelativeItem:addButton,
                    customPostSubmitHandler:addNewMenuItemFunc,
                    onCloseFocusItem:addButtonLink });
    }
    if ( $s('addModulePageButton' ) )
    {
      new flyoutform.FlyoutForm({  linkId:'addModulePageButton',
                    formDivId:'addModulePageForm',
                    openRelativeItem:addButton,
                    customPostSubmitHandler:addNewMenuItemFunc,
                    onCloseFocusItem:addButtonLink });
    }
  
    new flyoutform.FlyoutForm({  linkId:'addSubHeaderButton',
                  formDivId:'addSubheaderForm',
                  openRelativeItem:addButton,
                  customPostSubmitHandler:addNewMenuItemFunc,
                  onCloseFocusItem:addButtonLink });
  
    new flyoutform.FlyoutForm({  formDivId:'addDividerForm',
                  customPostSubmitHandler:addNewMenuItemFunc,
                  onCloseFocusItem:addButtonLink });

    // when adding dividers, no need show form, just submit it 
    if( $('addDividerButton') )
    {
      Event.observe( 'addDividerButton', 'click', function()
        {
          flyoutform.flyoutForms.addDividerForm.submit();
        });
      }
      
    // opening of modify... flyout forms is handled via context menu rather than button
    new flyoutform.FlyoutForm({  formDivId:'modifyCourseLinkForm',
                  resetFormOnOpen:false,
                  customPostSubmitHandler:null});
    
    new flyoutform.FlyoutForm({  formDivId:'modifyExternalLinkForm',
                  resetFormOnOpen:false,
                  customPostSubmitHandler:this._afterModifyExternalLinkItem.bind( this ) });
                  
    }
        
  // Hack for Safari for links in a new window
    if ( window.inNewWindow )
    {
      this.inNewWindow = true;
      $A(document.getElementsByTagName('a')).each( function( item )
      {
        if ( item.target == 'content' )
        {
          Event.observe(item, "click", this._targetParentWindow.bindAsEventListener( this, item ));
        }
      }.bind(this));
    }
    else
    {
      this.inNewWindow = false;
    }  
      
  this.courseMapLink = $('courseMapButton');
  if ( this.courseMapLink )
  {
    Event.observe( this.courseMapLink, 'click', this._showCourseMenuInWindow.bindAsEventListener( this ));
  }

    this.quickViewLink = $('quickViewLink');
    this.quickViewContainer = $('courseMenuPalette_contents');
    if ( this.quickViewLink )
    {
      Event.observe( this.quickViewLink, 'click', this._showQuickView.bindAsEventListener( this ) );
    }   
  
  this.expandAllLink = $('expandAllLink');
    if ( this.expandAllLink )
    {
      Event.observe( this.expandAllLink, 'click', this._detailViewExpandAll.bindAsEventListener( this ) );
    }
    
    this.collapseAllLink = $('collapseAllLink');
    if ( this.collapseAllLink )
    {
      Event.observe( this.collapseAllLink, 'click', this._detailViewCollapseAll.bindAsEventListener( this ) );
    }

  this.detailViewLink = $('detailViewLink');
  this.detailViewLink2 = $('detailViewLink2');
  if ( this.detailViewLink )
  {
    Event.observe( this.detailViewLink, 'click', this._showDetailView.bindAsEventListener( this ) );
    var courseMenuDivs = $('courseMenuPalette').getElementsByTagName('div');
    var courseMenuContents = null;
    for ( var i = 0; i < courseMenuDivs.length; i++ )
    {
      if ( page.util.hasClassName( courseMenuDivs[i], 'navPaletteContent') )
      {
        courseMenuContents = $(courseMenuDivs[i]);
      }
    }
    this.detailViewContainer = new Element("div", { id: 'courseMenu_folderView',style: 'display: none;' }).addClassName('treeContainer');
    courseMenuContents.appendChild(this.detailViewContainer);
      this.tree = null;
      $(this.detailViewLink2.getElementsByTagName('a')[0]).addClassName('options');
      this.detailViewLink2.hide();
      if ( this.detailViewLink.hasClassName('active') )
      {
        // show the folder view initially if it's the active one
        this._showDetailView();
      }
  }
  
  this.refreshLink = $('refreshMenuLink');
  if ( this.refreshLink )
  {
    Event.observe( this.refreshLink, 'click', this._refreshMenu.bindAsEventListener(this) );
  }
  
  this.quickEnrollLink = $('quickEnrollLink');
  if ( this.quickEnrollLink)
  {
    Event.observe( this.quickEnrollLink, 'click', this._quickEnrollToggle.bindAsEventListener(this) );
  }
  var renameSubHeaderInputBox = $("renameSubHeaderInputBox");
  if ( renameSubHeaderInputBox )
  {
    Event.observe( renameSubHeaderInputBox, "keydown", this.onKeyDown.bindAsEventListener( this ) );
  }
  
  var renameSubHeaderForm = $('renameSubHeaderForm');
  if ( renameSubHeaderForm )
  {
      renameSubHeaderForm.remove();
      document.body.appendChild( renameSubHeaderForm );
    }
  
  this.initPaletteState();
  this._updateSubheaders();
  },
  
  /**
   * Update the appearance of subheader names in the accessible drag and drop window
   * for course menu toc reordering to "Subheader: [subheader_name]".
   * And, of course, this applies in course menu edit mode only.
   */
  _updateSubheaders: function( )
  {
    if( this.editMode )
    {
      var firstTocItem = $( 'courseMenuPalette_contents' ).down( 'li' );
      var hideReorderBtn = !firstTocItem.readAttribute( 'id' );// prototype bug with IE7
      // create a map of option values to options
      var optionMap = {};
      dragdrop.ListReordering.addDivs();
  
      var sel = $('courseMenuPalette_pageListReorderControlSelect');
      $A(sel.options).each( function( o )
      { 
        optionMap[o.value] = o;
      });
      dragdrop.ListReordering.removeDivs();
      var lst = $('courseMenuPalette_contents');
      $A(lst.getElementsByTagName("li")).each( function( listItem ) 
      { 
        if ( $(listItem).hasClassName('subhead') )
        {
          // isolate id (after last colon)
          var id = listItem.id;
          id = id.substr( id.lastIndexOf(':') + 1 );
          var option = optionMap[id];
          // prepend "Subheader: " to option label
          $(option).update( page.bundle.getString("subheaderColonStr", option.innerHTML) ); 
        }
      });
      if( hideReorderBtn )
      {
        $( 'courseMenuPalette_reorderControlLink' ).up( 'li' ).hide();
      }
    }
  },

  _targetParentWindow: function( event, item )
  {
    Event.stop( event );
    window.opener.top.content.location = item.href;
  },

/**
 * Deletes a course menu item for the given item (toc) id
 *   Prompts the user for confirmation before deleting
 *  Calls the server to remove the Toc
 *  The current doc location is sent as a retUrl param to allow reloading the current
 *    page after deleting the item.
 */
  deleteItem: function( toc_id )
  {
    var appendQueryStringFunc = this.appendQueryString.bind( this );
    new Ajax.Request( this.appendMenuActionUrl( 'method=getTocRemoveMessage&course_id='+course_id+'&toc_id='+toc_id ), {
    onSuccess: function(transport, json) {            
      var result = transport.responseText.evalJSON( true );
      var retUrl = escape(document.location.href);
      if ( result.success == "true" )
      {
        if (confirm(result.removalMessage))
        {
          var removeTocUrl = appendQueryStringFunc( result.actionUrl, "method=removeToc&course_id=" + course_id +
                                                    "&toc_id=" + toc_id + '&retUrl=' + retUrl );
          new Ajax.Request(removeTocUrl, {
            onSuccess: function(transport, json) {            
              var result = transport.responseText.evalJSON( true );      
              if ( result.success == "true" )
              {
                top.content.location = result.refreshUrl;
              }
              else
              {
                new page.InlineConfirmation("error", result.errorMessage, false ); 
              }
            }
          } );
        }
      }
     else
     {
       new page.InlineConfirmation("error", result.errorMessage, false ); 
     }       
    }
  });    
  },

  toggleItemAvailability: function( toc_id, isEntryPoint )
  {
    var changeEntryPont = true;
    if ( isEntryPoint ) 
    {
      var entryPointChangeConfirmStr = page.bundle.getString("entryPointChangeConfirmStr");
      changeEntryPont = confirm ( entryPointChangeConfirmStr );
    }
    if(  changeEntryPont ) 
    {
      var url = this.appendMenuActionUrl( "method=toggleTocAvailability&course_id="+course_id+"&toc_id="+toc_id+"&retUrl="+escape(document.location.href) );
      window.location.href = url;    
    }
  },
  
  modifyCourseLink: function( toc_id )
  {
    var courseLinkId = 'paletteItem:' + toc_id;
    var url = this.appendMenuActionUrl( 'method=getCourseLinkTitle&course_id='+window.course_id+'&toc_id='+toc_id );
    new Ajax.Request( url, {
    onSuccess: function(transport, json) {            
      var result = transport.responseText.evalJSON( true );      
      if ( result.success == "true" )
      {
        flyoutform.flyoutForms.modifyCourseLinkForm.open( $(courseLinkId).down('a') );
        $s('modifiedLinkLocation').value = result.linkLocation;
        $s('modifyCourseLinkFormTocId').value = toc_id;
        $s('modifiedLinkId').value = result.linkedItemId;
        $s('modifiedLinkType').value = result.linkedItemType;
      }
     else
     {
       new page.InlineConfirmation("error", result.errorMessage, false ); 
     }       
    }
  });     
  },
  
  modifyExternalLink: function( toc_id )
  {
    var externalLinkId = 'paletteItem:' + toc_id;
    var url = this.appendMenuActionUrl( 'method=getExternalLinkUrl&course_id='+window.course_id+'&toc_id='+toc_id );
    new Ajax.Request( url, {
    onSuccess: function(transport, json) {            
      var result = transport.responseText.evalJSON( true );      
      if ( result.success == "true" )
      {
        flyoutform.flyoutForms.modifyExternalLinkForm.open( $(externalLinkId).down('a') );
        $s('modifyExternalLinkFormTocId').value = toc_id;
        $s('externalLinkUrlInputId').value = result.linkUrl;
      }
     else
     {
       new page.InlineConfirmation("error", result.errorMessage, false ); 
     }       
    }
  });     
  },
  
  toggleItemLaunchInd: function( toc_id )
  {    
    var tocId = 'paletteItem:' + toc_id; 
    var cmItem = $('toggleTocLaunchIndex:' + toc_id);
    var url = this.appendMenuActionUrl( 'method=toggleTocLaunchIndex&course_id='+window.course_id+'&toc_id='+toc_id );
    new Ajax.Request( url, {
    onSuccess: function(transport, json) {            
      var result = transport.responseText.evalJSON( true );
      if ( result.success == "true" )
      {  
        var tocLink = $(tocId).down('a');
        tocLink.target = result.windowType;
        tocLink.href = result.href;
        cmItem.innerHTML = result.menuItem;        
      }
     else
     {
       new page.InlineConfirmation("error", result.errorMessage, false ); 
     }           
    }
  });
  },  
    
  toggleItemGuestAccess: function( toc_id )
  { 
    var url = this.appendMenuActionUrl( "method=modifyGuestAccessibility&course_id="+window.course_id+"&toc_id="+toc_id+"&retUrl="+escape(document.location.href) );
    window.location.href = url;
  },
   
  toggleItemObserverAccess: function( toc_id )
  {
    var url = this.appendMenuActionUrl( "method=modifyObserverAccessibility&course_id="+window.course_id+"&toc_id="+toc_id+"&retUrl="+escape(document.location.href) );
    window.location.href = url;
  },
    
  renameItem: function( toc_id, targetType )
  {
   var subHeaderId = 'paletteItem:' + toc_id; 
   var val;
   this.renameTocId = toc_id;
   if(targetType)
   {
     this.renameTargetType = targetType;
     val = $(subHeaderId).down('h3').down('span').innerHTML;
     $('renameSubHeaderInputBox').value = val;
     Position.clone($(subHeaderId).down('h3').down('span'),$("renameSubHeaderForm"),{ setLeft: true, setTop: true, setWidth: false, setHeight: false } );   
   }
   else
   {
     this.renameTargetType = '';
     val = $(subHeaderId).down('a',0).down('span',0).innerHTML;
     $('renameSubHeaderInputBox').value = val;
     Position.clone($(subHeaderId).down('a',0).down('span',0),$("renameSubHeaderForm"),{ setLeft: true, setTop: true, setWidth: false, setHeight: false } );   
   }   
   $("renameSubHeaderForm").setStyle({display : 'block'});
   if (!this.modalOverlay)
   {
     this.modalOverlay = new ModalOverlay( $("renameSubHeaderForm") );
   }
   this.modalOverlay.setDisplay( true );
   $("renameSubHeaderInputBox").focus(); 
   $("renameSubHeaderInputBox").select();    
  }, 
  
  onKeyDown: function( event )
  {
  if (event.keyCode == Event.KEY_ESC)
  {
      Event.stop( event );
      this.cancelRename();
  }

  else if(event.keyCode == Event.KEY_RETURN){
    Event.stop( event );
    this.saveRenamedSubHeaderItem();      
  }
  },
  
  cancelRename: function()
  {
    this.modalOverlay.setDisplay( false );
    $("renameSubHeaderForm").setStyle({display : 'none'}); 
  },
    
  saveRenamedSubHeaderItem: function()
  {
    var toc_id = this.renameTocId;
    var new_name = $("renameSubHeaderInputBox").value;
    
    if(!new_name.blank())
    {    
      var subHeaderId = 'paletteItem:' + toc_id;
      var url = this.appendMenuActionUrl( 'method=renameSubHeader&course_id='+window.course_id+'&toc_id='+toc_id+'&new_name='+encodeURIComponent(new_name) );
      new Ajax.Request( url, {
        onSuccess: function(transport, json) {            
        var result = transport.responseText.evalJSON( true );
        if ( result.success == "true" )
        {
          var newValue = $('renameSubHeaderInputBox').value;
          $('renameSubHeaderInputBox').value = '';
          $("renameSubHeaderForm").setStyle({display : 'none'}); 
          var dispElem;
          if(this.renameTargetType == "subheader")
          {
            dispElem = $(subHeaderId).down('h3').down('span');
            dispElem.title = newValue;
            dispElem.innerHTML = newValue;
          }
          else
          {
            var link = $(subHeaderId).down('a',0);
            dispElem = link.down('span',0);
            dispElem.title = newValue;
            dispElem.innerHTML = newValue;
            if ( link.href.include('courseTocLabel=') )
            {
              // Also rewrite the url of the link to insert the new name in it
              link.href = link.href.sub( /courseTocLabel=([^&]+)/, 'courseTocLabel=' + encodeURIComponent( newValue ) );
            }
          }
        }
       else
       {
         $("renameSubHeaderForm").setStyle({display : 'none'}); 
         new page.InlineConfirmation("error", result.errorMessage, false ); 
       }           
         this.modalOverlay.setDisplay( false );
      }.bind(this)
    });  
    }
    else
    {
      this.cancelRename();
    }  
  },
  
  _afterModifyItem: function ( result)
  {
    $('observerEnabledId').removeAttribute("checked");
  $('guestEnabledId').removeAttribute("checked");
  },
  
  _afterModifyExternalLinkItem: function ( result)
  {
    var tocId = 'paletteItem:' + result.tocId; 
    $(tocId).down('a').setAttribute("href",result.linkUrl);    
  },

  _addNewMenuItem: function( result )
  {
    var newItem = result.returnData;
    
    var inWizard = page.util.hasClassName( document.body, 'wizardBody' );
    
    // If it's a CONTENT_ITEM type and it has a URL, and we're not in the course creation
    // wizard, redirect to that view URL.
    if( newItem.type == 'CONTENT_ITEM' && !inWizard && newItem.href && newItem.href != '#' )
    {
      document.location.href = newItem.href;
      return;
    }
    // If it's in the course creation wizard, disable the link.
    else if ( inWizard )
    {
      newItem.href = "#";
      newItem.target = "";
    }
    
    var template = courseMenu.Util.getTocItemTemplate( newItem.type, newItem.enabled == 'false', newItem.empty == 'true', this.menuGeneratorUrl );
    // create the appropriate html for the menu item using a template and the returned data, 
    // then add it to the bottom of the coursemenu
    var syntax = /(^|.|\r|\n)(<\&=\s*(\w+)\s*\&>)/; //matches symbols like '<&= field &>'
    var t = new Template( template, syntax );
    var d = t.evaluate( newItem );
    $('courseMenuPalette_contents').insert({bottom: d});
  
    // add a select option for the item to the accessibleToolSelect
    dragdrop.ListReordering.addDivs();
    var sel = $('courseMenuPalette_pageListReorderControlSelect');
    var itemName = newItem.name;
    if ( newItem.type == 'SUBHEADER' )
    {
      itemName = page.bundle.getString("subheaderColonStr", itemName);
    }

    sel.options[sel.length] = new Option( itemName, newItem.id );
    dragdrop.ListReordering.removeDivs();
  
    // re-init DND by disabling and enabling it so new item is dragable
    var dnd = dragdrop.controllers.find( function( dndController )
    {
      return ( dndController.itemContainer && dndController.itemContainer.id == 'courseMenuPalette_contents');    
    }.bind(this));
    dnd.disableDragAndDrop();
    dnd.enableDragAndDrop();
    dnd.calculateItemOrder();
    //show the course menu reorder button
    $( 'courseMenuPalette_reorderControlLink' ).up( 'li' ).show();
    // add behavior to context menu of new item 
    var items = $('courseMenuPalette_contents').childNodes;
    var lastItem = items[items.length-1];
    var contextMenu = $(lastItem).down( ".contextMenuContainer" );
    if ( contextMenu )
    {
      new page.ContextMenu( contextMenu );
    }
    var id = lastItem.id;
    id = id.replace("paletteItem","label");

    // Focus on the new menu item after the DOM is ready
    (function()
    {
    $(id).focus();
    }.defer());
  },

  _showCourseMenuInWindow: function( event )
  {
    if ( event )
    {
      Event.stop( event );
    }
    var lpix = screen.width - 800;
    window.remote = window.open('/webapps/blackboard/content/courseMenu.jsp?course_id='+window.course_id+'&newWindow=true&openInParentWindow=true', 'newwin', 'width=220,height=440,resizable=yes,scrollbars=yes,status=no,top=20,left='+lpix);
    if ( window.remote )
    {
      window.remote.focus();
      if ( !window.remote.opener )
    { 
        window.remote.opener = self;
      }
      window.top.name = 'bbWin';
      //hack for IE
      if(top.attachEvent)
      {
         top.attachEvent( "onunload",function(){ if ( window.remote ) { window.remote.close(); } } );
      }
      else
      {
         Event.observe( top, "unload", function(){ if ( window.remote ) { window.remote.close(); } } );
      }   
    } 
  },

  _showCourseLinkPicker: function( )
  {
    var lpix = screen.width - 800;
    window.remote = window.open('/webapps/blackboard/execute/course/courseMapPicker?displayMode=courseLinkPicker&course_id='+window.course_id, 'picker_browse', 'width=250,height=350,resizable=yes,scrollbars=yes,status=yes,top=20,left='+lpix);
    if ( window.remote )
    {
      window.remote.focus();
      if ( !window.remote.opener )
    { 
        window.remote.opener = self;
      }
      window.remote.opener.inputItemPathToSet = $('linkLocation');
      window.remote.opener.inputItemPKToSet = $('linkId');
      window.remote.opener.inputItemTypeToSet = $('linkType');
      window.remote.opener.callBack = this._linkPickerCallback.bind( this );
      window.top.name = 'bbWin'; 
    } 
  },
  
  _showModifyCourseLinkPicker: function( )
  {
    var lpix = screen.width - 800;
    window.remote = window.open('/webapps/blackboard/execute/course/courseMapPicker?displayMode=courseLinkPicker&course_id='+window.course_id, 'picker_browse', 'width=250,height=350,resizable=yes,scrollbars=yes,status=yes,top=20,left='+lpix);
    if ( window.remote )
    {
      window.remote.focus();
      if ( !window.remote.opener )
    { 
        window.remote.opener = self;
      }
      window.remote.opener.inputItemPathToSet = $('modifiedLinkLocation');
      window.remote.opener.inputItemPKToSet = $('modifiedLinkId');
      window.remote.opener.inputItemTypeToSet = $('modifiedLinkType');
      window.remote.opener.callBack = this._ModifyLinkPickerCallback.bind( this );
      window.top.name = 'bbWin'; 
    } 
  },
  
  _ModifyLinkPickerCallback: function( )
  {
    flyoutform.flyoutForms.modifyCourseLinkForm.updateSubmitButtonEnable();
  },  

  _linkPickerCallback: function( )
  {
    var link_name = $("linkLocation").value.split('/').pop();
    var linkNameField = $("addCourseLinkName");
    var origName = linkNameField.value;
    if (origName.trim() == '')
    {
      linkNameField.value = link_name;
    }
    flyoutform.flyoutForms.addCourseLinkForm.updateSubmitButtonEnable();
  },
  
  _showDetailView: function( event )
  {
    if ( event )
    {
      Event.stop( event );
    }
    
    if ( !this.detailViewLink.hasClassName('active') )
    {
      CourseMenuDWRFacade.setMenuDisplayMode( window.course_id, true);
    }
    
    if ( this.quickViewLink )
    {
      this.quickViewLink.removeClassName('active');
    }
    this.detailViewLink.addClassName('active');
    this.detailViewLink.hide();
    this.detailViewLink2.addClassName('active');
    this.detailViewLink2.show();

    // set the curently active palette contents container so expand/collapse on palette will work
    page.PaletteController.setActivePaletteContentsContainer( "courseMenuPalette", this.detailViewContainer );

    var keyboardDndLink = $('courseMenuPalette_reorderControlLink');
    if ( keyboardDndLink )
    {
      keyboardDndLink.up().hide();
    }
    var addItemLink = $('addCmItem');
    if ( addItemLink )
    {
      addItemLink.hide();
    }
    this.quickViewContainer.hide();
    this.detailViewContainer.show();
    
    if ( !this.tree || this.editMode )
    {
      var displayMode = "courseMenu";
      if ( this.inNewWindow )
      {
        displayMode = "courseMenu_newWindow";
      }
      this.tree = new dynamictree.Tree( this.detailViewContainer, null, '/webapps/blackboard/execute/course/menuFolderViewGenerator', 'course_id='+window.course_id+'&displayMode='+displayMode+'&editMode='+this.editMode+'&openInParentWindow=true', true );
    }
    
  },
  
  _detailViewExpandAll: function( event )
  {
    Event.stop( event );
    if ( this.tree )
    {
      this.tree.expandAll();
    }
  },
  
  _detailViewCollapseAll: function( event )
  {
    Event.stop( event );
    if ( this.tree )
    {
      this.tree.collapseAll();
    }
  },
  
  _showQuickView: function( event )
  {
    if ( event )
    {
      Event.stop( event );
    }
    
    this.quickViewLink.addClassName('active');

    // set the curently active palette contents container so expand/collapse on palette will work
    page.PaletteController.setActivePaletteContentsContainer( "courseMenuPalette", this.quickViewContainer );

    if ( this.detailViewLink )
    {
      this.detailViewLink.removeClassName('active');
      this.detailViewLink.show();
      this.detailViewLink2.removeClassName('active');
      this.detailViewLink2.hide();
    }
    var keyboardDndLink = $('courseMenuPalette_reorderControlLink');
    if ( keyboardDndLink )
    {
      keyboardDndLink.up().show();
    }
    var addItemLink = $('addCmItem');
    if ( addItemLink )
    {
      addItemLink.show();
    }
    this.quickViewContainer.show();
    this.detailViewContainer.hide();
    
    CourseMenuDWRFacade.setMenuDisplayMode( window.course_id, false);
  },
  
  _refreshMenu: function( event )
  {
    Event.stop( event );
    var loc = window.location + '';
    var hashLoc = loc.indexOf("#");
    if ( hashLoc >= 0 )
    {
      loc = loc.substring(0, hashLoc);
    }
    if ( loc.indexOf('refreshCourseMenu') < 0 )
    {
      loc = this.appendQueryString( loc, 'refreshCourseMenu=true' );
    }
    window.location = loc;
  },
  
  _quickEnrollToggle: function( event )
  {
    Event.stop( event );
    var nonceIdValue = nonceUtil.getNonceIdValue(/* no formId since there is not form for the quick enroll drop down*/);
     
    if ( this.quickEnrollLink.hasClassName("quickEnrolled") )
    {
      if ( confirm( window.confirmQuickUnenrollMsg ) )
      {
       // using a get request to modify the role is not the best practice, if we refactor make this a new Ajax.request post, since this modifying the data 
        var returnUrl = document.location.href;
        window.location = this.appendMenuActionUrl( 'method=quickEnrollToggle&course_id='+window.course_id+'&retUrl='+escape(returnUrl) + '&blackboard.platform.security.NonceUtil.nonce=' + nonceIdValue );
      }
    }
    else
    {
      var roleId = event.target.parentNode.id;
      if ( roleId.indexOf("paletteItem") > -1 )
      {
        roleId = roleId.substring(12);
        var name = ( typeof event.target.parentNode.childNodes[0].value == "undefined" )? event.target.parentNode.childNodes[1].value : event.target.parentNode.childNodes[0].value;
        if ( confirm( page.bundle.getString( "confirmQuickEnrollStr", name ) ) )
        {  
         // using a get request to modify the role is not the best practice, if we refactor make this a new Ajax.request post, since this modifying the data 
          window.location = this.appendMenuActionUrl( 'method=quickEnrollToggle&course_id='+window.course_id+'&targetRole='+roleId+'&retUrl='+escape(document.location.href) + '&blackboard.platform.security.NonceUtil.nonce=' + nonceIdValue );
        }
      }
      else
      {
        if ( confirm( window.confirmQuickEnrollMsg ) )
        {
          // using a get request to modify the role is not the best practice, if we refactor make this a new Ajax.request post, since this modifying the data 
          
          window.location = this.appendMenuActionUrl( 'method=quickEnrollToggle&course_id='+window.course_id+'&targetRole=P'+'&retUrl='+escape(document.location.href) + '&blackboard.platform.security.NonceUtil.nonce=' + nonceIdValue );
        }
      }
    }
  },
  
  appendMenuActionUrl : function( paramsToAppend )
  {
    return this.appendQueryString( this.menuActionUrl, paramsToAppend );
  },

  appendQueryString : function( baseUrl, paramsToAppend )
  {
    var paramSeparator = baseUrl.indexOf( '?' ) != -1 ? '&' : '?';
    return baseUrl + paramSeparator + paramsToAppend;
  },

  initPaletteState: function()
  {
    var activeCourseMenuContentsContainer = page.PaletteController.getPaletteControllerObjById( "courseMenuPalette" ).getActiveContentsContainer();
    UserDataDWRFacade.getStringTempScope( activeCourseMenuContentsContainer.id + window.course_id, this.getCourseMenuPaletteStateResponse.bind( this ) );
    UserDataDWRFacade.getStringTempScope( 'myGroups_contents' + window.course_id, this.getMyGroupsPaletteStateResponse.bind( this ) );
    UserDataDWRFacade.getStringTempScope( 'controlPanelPalette_contents' + window.course_id, this.getControlPanelPaletteStateResponse.bind( this ) );
    UserDataDWRFacade.getStringTempScope( 'pickerPalette_contents' + window.course_id, this.getPickerPaletteStateResponse.bind( this ) );
  }, 
  
  /**
   * By default, palettes are expanded.  However, the "Files" palette's default
   * behavior is to be collapsed.
   * 
   * This function ensures when the course menu loads, the "Files" palette default collapsed behavior
   * is enforced.
   */
  getPickerPaletteStateResponse : function ( paletteState  ) 
  {
    if ($('pickerPalette') === null)
    {
      // No File Manager palette found.  
      return;
    }
    
    if (paletteState !== 'block') 
    { 
      // File palette was closed on the prior page, ensure Files palette is closed when this page is loaded.
      page.PaletteController.collapsePalette('pickerPalette', true);
    }
  },

  getCourseMenuPaletteStateResponse : function ( paletteState  ) 
  {
    this.getPaletteStateResponse( paletteState, 'courseMenuPalette' );
  },
  
  getMyGroupsPaletteStateResponse : function ( paletteState  ) 
  {
    this.getPaletteStateResponse( paletteState, 'myGroups' );
  },
  
  getControlPanelPaletteStateResponse : function ( paletteState  ) 
  {
    this.getPaletteStateResponse( paletteState, 'controlPanelPalette' );
  },
  
  getPaletteStateResponse : function ( paletteState, id )
  { 
    if ( !$(id) ) //If the palette doesn't exist on the page, don't do anything
    {
      return;
    }
  
    var paletteItem = $(page.PaletteController.getDefaultContentsContainerId(id));
    
    // if the course menu palette, get the currently active contents container
    if( id == 'courseMenuPalette' )
    {
      // get the active palette content container element (List VS Folder view)
      paletteItem = this.quickViewContainer;
      if( this.detailViewLink && this.detailViewLink.hasClassName('active') )
      {
        paletteItem = this.detailViewContainer;    
    }
    }

    var originalPaletteState = paletteItem.style.display;
    if( originalPaletteState != 'none' && originalPaletteState != 'block' )
    {
      originalPaletteState = 'block';
    }
    var cachedPaletteState = 'block';
    if ( paletteState.length > 0 ) 
    {
      if ( paletteState == 'none' || paletteState == 'block' )
      {
        cachedPaletteState = paletteState; 
      }
       else 
      {
         cachedPaletteState = 'block';
    } 
    }
    
    if ( originalPaletteState != cachedPaletteState ) 
    {
      //because we want the menu to be in the cached state, 
      //we pass in the opposite so that expandCollapse changes the menu state.
      // pass true for 2nd param to supress persisting state on init
      page.PaletteController.toggleExpandCollapsePalette(id, true);
    } 
  }  
     
};

courseMenu.searchWidget = {
  goOnClick : function()
  {
    if ( $('toolsSearchBox').value )
    {
      window.location = $('searchGo').href + "&" + $('toolsSearchBox').name + "=" + $('toolsSearchBox').value;
    }
    else
    {
      alert( page.bundle.getString("enterSearchKeyStr") );
    }
    return false;
  },
  
  onKeydown : function( event )
  {
    var e = event || window.event;
    var key = e.keyCode || e.which;
    if ( key == Event.KEY_RETURN )
    {
      this.goOnClick();
    }
  }
};

function collapsePalettesForGroupSpace(groupId)
{
  // Expand the group in the my groups palette that was clicked
  // the my groups palette may not exist on the doc yet if current user isn't part of a group
  var myGroupsPalette = $('myGroups_contents');
  if( myGroupsPalette )
  {
    // NOTE - do NOT explicitly close all other palettes - while it may have been a requirement
    // at one point in time it appears as though it has come back several times as a bug.
    page.PaletteController.expandPalette('myGroups');

    var groupmenulink = ('mygroups.' + groupId + '_groupExpanderLink');
    var itemExpanderObj = page.ItemExpander.itemExpanderMap[groupmenulink];
    if( itemExpanderObj )
    {
      itemExpanderObj.expandCollapse(false);
    }
  }  
}

