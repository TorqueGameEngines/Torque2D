//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Revision History:
// December 31, 2003	David Wyand		Changed a bunch of stuff.  Search for DAW below
//										and make better notes here and in the changes doc.
// May 19, 2004			David Wyand		Made changes to allow for a colored rectangle to be
//										displayed to the left of the text in the popup.
// May 27, 2004			David Wyand		Added a check for mReverseTextList to see if we should
//										reverse the text list if we must render it above
//										the control.  NOTE: there are some issues with setting
//										the selected ID using the 'setSelected' command externally
//										and the list is rendered in reverse.  It seems that the
//										entry ID's also get reversed.
// November 16, 2005	David Wyand		Added the method setNoneSelected() to set none of the
//										items as selected.  Use this over setSelected(-1); when
//										there are negative IDs in the item list.  This also
//										includes the setNoneSelected console method.
//

#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/guiPopUpCtrl.h"
#include "console/consoleTypes.h"
#include "gui/guiDefaultControlRender.h"

static ColorI colorWhite(255,255,255); // DAW: Added

// Function to return the number of columns in 'string' given delimeters in 'set'
static U32 getColumnCount(const char *string, const char *set)
{
   U32 count = 0;
   U8 last = 0;
   while(*string)
   {
      last = *string++;

      for(U32 i =0; set[i]; i++)
      {
         if(last == set[i])
         {
            count++;
            last = 0;
            break;
         }   
      }
   }
   if(last)
      count++;
   return count;
}   

// Function to return the 'index' column from 'string' given delimeters in 'set'
static const char *getColumn(const char *string, char* returnbuff, U32 index, const char *set)
{
   U32 sz;
   while(index--)
   {
      if(!*string)
         return "";
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return "";
      string += (sz + 1);    
   }
   sz = dStrcspn(string, set);
   if (sz == 0)
      return "";
   char *ret = returnbuff;
   dStrncpy(ret, string, sz);
   ret[sz] = '\0';
   return ret;
}   

GuiPopUpBackgroundCtrl::GuiPopUpBackgroundCtrl(GuiPopUpMenuCtrl *ctrl, GuiPopupTextListCtrl *textList)
{
   mPopUpCtrl = ctrl;
   mTextList = textList;
}

void GuiPopUpBackgroundCtrl::onMouseDown(const GuiEvent &event)
{
   //   mTextList->setSelectedCell(Point2I(-1,-1)); // DAW: Removed
   mPopUpCtrl->mBackgroundCancel = true; // DAW: Set that the user didn't click within the text list.  Replaces the line above.
   mPopUpCtrl->closePopUp();
}

//------------------------------------------------------------------------------
GuiPopupTextListCtrl::GuiPopupTextListCtrl()
{
   mPopUpCtrl = NULL;
}


//------------------------------------------------------------------------------
GuiPopupTextListCtrl::GuiPopupTextListCtrl(GuiPopUpMenuCtrl *ctrl)
{
   mPopUpCtrl = ctrl;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//void GuiPopUpTextListCtrl::onCellSelected( Point2I /*cell*/ )
//{
//   // Do nothing, the parent control will take care of everything...
//}
void GuiPopupTextListCtrl::onCellSelected( Point2I cell )
{
   // DAW: The old function is above.  This new one will only call the the select
   //      functions if we were not cancelled by a background click.

   // DAW: Check if we were cancelled by the user clicking on the Background ie: anywhere
   //      other than within the text list.
   if(mPopUpCtrl->mBackgroundCancel)
      return;

   if( isMethod( "onSelect" ) )
      Con::executef(this, 3, "onSelect", Con::getFloatArg(cell.x), Con::getFloatArg(cell.y));

   //call the console function
   if (mConsoleCommand[0])
      Con::evaluate(mConsoleCommand, false);

}

//------------------------------------------------------------------------------
bool GuiPopupTextListCtrl::onKeyDown(const GuiEvent &event)
{
   //if the control is a dead end, don't process the input:
   if ( !mVisible || !mActive || !mAwake )
      return false;

   //see if the key down is a <return> or not
   if ( event.modifier == 0 )
   {
      if ( event.keyCode == KEY_RETURN )
      {
         mPopUpCtrl->closePopUp();
         return true;
      }
      else if ( event.keyCode == KEY_ESCAPE )
      {
         mSelectedCell.set( -1, -1 );
         mPopUpCtrl->closePopUp();
         return true;
      }
   }

   //otherwise, pass the event to it's parent
   return Parent::onKeyDown(event);
}

void GuiPopupTextListCtrl::onMouseDown(const GuiEvent &event)
{
   // Moved to onMouseUp in order to capture the mouse for the entirety of the click. ADL.
   // This also has the side effect of allowing the mouse to be held down and a selection made.
   //Parent::onMouseDown(event);
   //mPopUpCtrl->closePopUp();
}

void GuiPopupTextListCtrl::onMouseUp(const GuiEvent &event)
{
   Parent::onTouchDown(event);
   mPopUpCtrl->closePopUp();
   Parent::onTouchUp(event);
}

//------------------------------------------------------------------------------
void GuiPopupTextListCtrl::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
   // DAW: Get the size of the cells
   Point2I size;
   getCellSize(size);
   if(mouseOver)
   {
      // DAW: Render a background color for the cell
      RectI cellR(offset.x, offset.y, size.x, size.y);
      ColorI color(0,0,0);
      dglDrawRectFill(cellR, color);

   } else if(selected)
   {
      // DAW: Render a background color for the cell
      RectI cellR(offset.x, offset.y, size.x, size.y);
      ColorI color(128,128,128);
      dglDrawRectFill(cellR, color);
   }

   // DAW: Define the default x offset for the text
   U32 textXOffset = offset.x + mProfile->mTextOffset.x;

   // DAW: Do we also draw a colored box beside the text?
   ColorI boxColor;
   bool drawbox = mPopUpCtrl->getColoredBox( boxColor, mList[cell.y].id);
   if(drawbox)
   {
      Point2I coloredboxsize(15,10);
      RectI r(offset.x + mProfile->mTextOffset.x, offset.y+2, coloredboxsize.x, coloredboxsize.y);
      dglDrawRectFill( r, boxColor);
      dglDrawRect( r, ColorI(0,0,0));

      textXOffset += coloredboxsize.x + mProfile->mTextOffset.x;
   }

   ColorI fontColor;
   mPopUpCtrl->getFontColor( fontColor, mList[cell.y].id, selected, mouseOver );

   dglSetBitmapModulation( fontColor );
   //dglDrawText( mFont, Point2I( offset.x + 4, offset.y ), mList[cell.y].text );

   // DAW: Get the number of columns in the cell
   S32 colcount = getColumnCount(mList[cell.y].text, "\t");

   // DAW: Are there two or more columns?
   if(colcount >= 2)
   {
      char buff[256];

      // Draw the first column
      getColumn(mList[cell.y].text, buff, 0, "\t");
      dglDrawText( mFont, Point2I( textXOffset, offset.y ), buff ); // DAW: Used mTextOffset as a margin for the text list rather than the hard coded value of '4'.

      // Draw the second column to the right
      getColumn(mList[cell.y].text, buff, 1, "\t");
      S32 txt_w = mFont->getStrWidth(buff);

      dglDrawText( mFont, Point2I( offset.x+size.x-mProfile->mTextOffset.x-txt_w, offset.y ), buff ); // DAW: Used mTextOffset as a margin for the text list rather than the hard coded value of '4'.

   } else
   {
      dglDrawText( mFont, Point2I( textXOffset, offset.y ), mList[cell.y].text ); // DAW: Used mTextOffset as a margin for the text list rather than the hard coded value of '4'.
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiPopUpMenuCtrl);

GuiPopUpMenuCtrl::GuiPopUpMenuCtrl(void)
{
   VECTOR_SET_ASSOCIATION(mEntries);
   VECTOR_SET_ASSOCIATION(mSchemes);

   mSelIndex = -1;
   mActive = true;
   mMaxPopupHeight = 200;
   mScrollDir = GuiScrollCtrl::None;
   mScrollCount = 0;
   mLastYvalue = 0;
   mIncValue = 0;
   mRevNum = 0;
   mInAction = false;
   mMouseOver = false; // DAW: Added
   mRenderScrollInNA = false; // DAW: Added
   mBackgroundCancel = false; // DAW: Added
   mReverseTextList = false; // DAW: Added - Don't reverse text list if displaying up
   mBitmapName = StringTable->EmptyString; // DAW: Added
   mBitmapBounds.set(16, 16); // DAW: Added
}

//------------------------------------------------------------------------------
GuiPopUpMenuCtrl::~GuiPopUpMenuCtrl()
{

}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::initPersistFields(void)
{
   Parent::initPersistFields();

   addField("maxPopupHeight",           TypeS32,          Offset(mMaxPopupHeight, GuiPopUpMenuCtrl));
   addField("sbUsesNAColor",            TypeBool,         Offset(mRenderScrollInNA, GuiPopUpMenuCtrl));
   addField("reverseTextList",          TypeBool,         Offset(mReverseTextList, GuiPopUpMenuCtrl));
   addField("bitmap",                   TypeFilename,     Offset(mBitmapName, GuiPopUpMenuCtrl));
   addField("bitmapBounds",             TypePoint2I,      Offset(mBitmapBounds, GuiPopUpMenuCtrl));
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrl, add, void, 4, 5, "( entryText , entryID [ , entryScheme ] ) Use the add method to add a new entry with text entryText, ID entryID, and using the scheme entryScheme.\n"
                                                                "@param entryText Text to display in menu entry.\n"
                                                                "@param entryID ID to assign to entry. This value may be 1 or greater.\n"
                                                                "@param entryScheme An integer value representing the ID of an optional color scheme to be used for this entry.\n"
                                                                "@return No return value.\n"
                                                                "@sa addScheme, clear")
{
   if ( argc > 4 )
      object->addEntry(argv[2],dAtoi(argv[3]),dAtoi(argv[4]));
   else
      object->addEntry(argv[2],dAtoi(argv[3]),0);
}

ConsoleMethod( GuiPopUpMenuCtrl, addScheme, void, 6, 6, "( entryScheme , fontColor , fontColorHL , fontColorSEL ) Use the addScheme method to create a new color scheme or to modify an existing one.\n"
                                                                "An integer color vector contains three integer values, each between 0 and 255 and is organized in this order: 'R G B'.\n"
                                                                "@param entryScheme An integer value representing the ID of the scheme, between 1 and inf.\n"
                                                                "@param fontColor A vector containing an integer representation of the menu entry's normal color.\n"
                                                                "@param fontColorHL A vector containing an integer representation of the menu entry's highlighted color.\n"
                                                                "@param fontColorSEL A vector containing an integer representation of the menu entry's selected color.\n"
                                                                "@return No return value.\n"
                                                                "@sa add")
{
   ColorI fontColor, fontColorHL, fontColorSEL;
   U32 r, g, b;
   char buf[64];

   dStrcpy( buf, argv[3] );
   char* temp = dStrtok( buf, " \0" );
   r = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   g = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   b = temp ? dAtoi( temp ) : 0;
   fontColor.set( r, g, b );

   dStrcpy( buf, argv[4] );
   temp = dStrtok( buf, " \0" );
   r = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   g = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   b = temp ? dAtoi( temp ) : 0;
   fontColorHL.set( r, g, b );

   dStrcpy( buf, argv[5] );
   temp = dStrtok( buf, " \0" );
   r = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   g = temp ? dAtoi( temp ) : 0;
   temp = dStrtok( NULL, " \0" );
   b = temp ? dAtoi( temp ) : 0;
   fontColorSEL.set( r, g, b );

   object->addScheme( dAtoi( argv[2] ), fontColor, fontColorHL, fontColorSEL );
}

ConsoleMethod( GuiPopUpMenuCtrl, setText, void, 3, 3, "( text ) Use the setText method to change the text displayed in the menu bar.\n"
                                                                "Pass the NULL string to clear the menu bar text.\n"
                                                                "@param text New text to display in the menu bar.\n"
                                                                "@return No return value.\n"
                                                                "@sa getText, replaceText")
{
   object->setText(argv[2]);
}

ConsoleMethod( GuiPopUpMenuCtrl, getText, const char*, 2, 2, "() Use the getText method to get the text currently displayed in the menu bar.\n"
                                                                "@return Returns the text in the menu bar or NULL if no text is present.\n"
                                                                "@sa getSelected, setText")
{
   return object->getText();
}

ConsoleMethod( GuiPopUpMenuCtrl, clear, void, 2, 2, "() Use the clear method to remove all entries and schemes from the menu.\n"
                                                                "@return No return value.\n"
                                                                "@sa add, addScheme")
{
   object->clear();
}

ConsoleMethod(GuiPopUpMenuCtrl, sort, void, 2, 2, "() Use the sort method to sort the menu in ascending order.\n"
                                                                "This is a lexicographic sort, so number (1,2,3,...) will come before letters (a,b,c,...)\n"
                                                                "@return No return value.")
{
   object->sort();
}

// DAW: Added to sort the entries by ID
ConsoleMethod(GuiPopUpMenuCtrl, sortID, void, 2, 2, "() Sort the list by ID.\n"
                                                    "@return No return value.")
{
   object->sortID();
}

ConsoleMethod( GuiPopUpMenuCtrl, forceOnAction, void, 2, 2, "() Use the forceOnAction method to force the onAction callback to be triggered.\n"
                                                                "@return No return value.\n"
                                                                "@sa forceClose, onAction (GUIControl callback)")
{
   object->onAction();
}

ConsoleMethod( GuiPopUpMenuCtrl, forceClose, void, 2, 2, "() Use the forceClose method to force the menu to close.\n"
                                                                "This is useful for making menus that fold up after a short delay when the mouse leaves the menu area.\n"
                                                                "@return No return value.\n"
                                                                "@sa forceOnAction")
{
   object->closePopUp();
}

ConsoleMethod( GuiPopUpMenuCtrl, getSelected, S32, 2, 2, "() Use the getSelected method to get the ID of the last selected entry.\n"
                                                                "@return Returns the ID of the currently selected entry, or 0 meaning no menu was selected after the last menu open.")
{
   return object->getSelected();
}

ConsoleMethod( GuiPopUpMenuCtrl, setSelected, void, 3, 4, "(int id, [scriptCallback=true]) Set the object status as selected\n"
              "@param id The object's ID.\n"
              "@param scriptCallback Flag whether to notify\n"
              "@return No return value.")
{
   if( argc > 3 )
      object->setSelected( dAtoi( argv[2] ), dAtob( argv[3] ) );
   else
      object->setSelected( dAtoi( argv[2] ) );
}

ConsoleMethod( GuiPopUpMenuCtrl, setFirstSelected, void, 2, 2, "() \n"
              "@return No return value.")
{
   object->setFirstSelected();
}

ConsoleMethod( GuiPopUpMenuCtrl, setNoneSelected, void, 2, 2, "() Deselects all popup menu controls\n"
                                                            "@return No return value.")
{
   object->setNoneSelected();
}

ConsoleMethod( GuiPopUpMenuCtrl, getTextById, const char*, 3, 3,  "( ID ) Use the getTextById method to get the text value for the menu item represented by ID.\n"
                                                                "@param ID An integer value representing the ID of a menu item.\n"
                                                                "@return Returns a string containing the menu item corresponding to ID, or a NULL string if no menu item has the specified ID.\n"
                                                                "@sa add, getText")
{
   return(object->getTextById(dAtoi(argv[2])));
}

ConsoleMethod( GuiPopUpMenuCtrl, setEnumContent, void, 4, 4, "(string class, string enum)"
              "This fills the popup with a classrep's field enumeration type info.\n\n"
              "More of a helper function than anything.   If console access to the field list is added, "
              "at least for the enumerated types, then this should go away.."
              "@param className The class name associated with this enum content.\n"
              "@param enumName The name of the enumerated entry to add to the menu. This value must match an enum string as exposed by the engine for the class. The menu item will have the same text as the enum string name, and the ID will be equal to the enumerated entries value.\n"
              "@return No return value")
{
   AbstractClassRep * classRep = AbstractClassRep::getClassList();

   // walk the class list to get our class
   while(classRep)
   {
      if(!dStricmp(classRep->getClassName(), argv[2]))
         break;
      classRep = classRep->getNextClass();
   }

   // get it?
   if(!classRep)
   {
      Con::warnf(ConsoleLogEntry::General, "failed to locate class rep for '%s'", argv[2]);
      return;
   }

   // walk the fields to check for this one (findField checks StringTableEntry ptrs...)
   U32 i;
   for(i = 0; i < (U32)classRep->mFieldList.size(); i++)
      if(!dStricmp(classRep->mFieldList[i].pFieldname, argv[3]))
         break;

   // found it?   
   if(i == classRep->mFieldList.size())
   {   
      Con::warnf(ConsoleLogEntry::General, "failed to locate field '%s' for class '%s'", argv[3], argv[2]);
      return;
   }

   const AbstractClassRep::Field & field = classRep->mFieldList[i];

   // check the type
   if(field.type != TypeEnum)
   {
      Con::warnf(ConsoleLogEntry::General, "field '%s' is not an enumeration for class '%s'", argv[3], argv[2]);
      return;
   }

   AssertFatal(field.table, avar("enumeration '%s' for class '%s' with NULL ", argv[3], argv[2]));

   // fill it
   for(i = 0; i < (U32)field.table->size; i++)
      object->addEntry(field.table->table[i].label, field.table->table[i].index);
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrl, findText, S32, 3, 3, "( text ) Use the findText method to locate the string text in the list of menu items. It will return the ID of the first entry found.\n"
                                                                "@param text A string containing the text to search for.\n"
                                                                "@return Returns an integer value representing the ID of the menu item, or -1 if the text was not found.")
{
   return( object->findText( argv[2] ) );   
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrl, size, S32, 2, 2, "() Get the size of the menu, ie the number of entries in it.\n"
              "@return The numbers of entries as an integer.")
{
   return( object->getNumEntries() ); 
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrl, replaceText, void, 3, 3, "( enable ) Use the replaceText method to enable the updating of the menu bar text when a menu item is selected.\n"
                                                                "This does not prevent changing the menu bar text with setText.\n"
                                                                "@param enable A boolean value enabling or disabling the automatic updating of the menu bar text when a selection is made.\n"
                                                                "@return No return value.\n"
                                                                "@sa getText, setText")
{
   object->replaceText(dAtoi(argv[2]));  
}

//------------------------------------------------------------------------------
// DAW: Added
bool GuiPopUpMenuCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   // Set the bitmap for the popup.
   setBitmap(mBitmapName);

   // Now update the Form Control's bitmap array, and possibly the child's too
   mProfile->constructBitmapArray();
   if(mProfile->mProfileForChildren)
      mProfile->mProfileForChildren->constructBitmapArray();

   return true;
}

//------------------------------------------------------------------------------
bool GuiPopUpMenuCtrl::onAdd()
{
   if (!Parent::onAdd())
      return false;
   mSelIndex = -1;
   mReplaceText = true;
   return true;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::onSleep()
{
   mTextureNormal = NULL; // DAW: Added
   mTextureDepressed = NULL; // DAW: Added
   Parent::onSleep();
   closePopUp();  // Tests in function.
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::clear()
{
   mEntries.setSize(0);
   setText("");
   mSelIndex = -1;
   mRevNum = 0;
}

//------------------------------------------------------------------------------
static S32 QSORT_CALLBACK textCompare(const void *a,const void *b)
{
   GuiPopUpMenuCtrl::Entry *ea = (GuiPopUpMenuCtrl::Entry *) (a);
   GuiPopUpMenuCtrl::Entry *eb = (GuiPopUpMenuCtrl::Entry *) (b);
   return (dStricmp(ea->buf, eb->buf));
} 

// DAW: Added to sort by entry ID
//------------------------------------------------------------------------------
static S32 QSORT_CALLBACK idCompare(const void *a,const void *b)
{
   GuiPopUpMenuCtrl::Entry *ea = (GuiPopUpMenuCtrl::Entry *) (a);
   GuiPopUpMenuCtrl::Entry *eb = (GuiPopUpMenuCtrl::Entry *) (b);
   return ( (ea->id < eb->id) ? -1 : ((ea->id > eb->id) ? 1 : 0) );
} 

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrl::setBitmap(const char *name)
{
   mBitmapName = StringTable->insert(name);
   if(!isAwake())
      return;

   if (*mBitmapName)
   {
      char buffer[1024];
      char *p;
      dStrcpy(buffer, name);
      p = buffer + dStrlen(buffer);

      dStrcpy(p, "_n");
      mTextureNormal = TextureHandle(buffer, TextureHandle::BitmapTexture, true);

      dStrcpy(p, "_d");
      mTextureDepressed = TextureHandle(buffer, TextureHandle::BitmapTexture, true);
      if (!mTextureDepressed)
         mTextureDepressed = mTextureNormal;
   }
   else
   {
      mTextureNormal = NULL;
      mTextureDepressed = NULL;
   }
   setUpdate();
}   

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::sort()
{
    dQsort((void *)mEntries.address(), mEntries.size(), sizeof(Entry), textCompare);
}

// DAW: Added to sort by entry ID
//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::sortID()
{
    dQsort((void *)mEntries.address(), mEntries.size(), sizeof(Entry), idCompare);
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::addEntry(const char *buf, S32 id, U32 scheme)
{
   if( !buf )
   {
      //Con::printf( "GuiPopupMenuCtrlEx::addEntry - Invalid buffer!" );
      return;
   }

   Entry e;
   dStrcpy(e.buf, buf);
   e.id = id;
   e.scheme = scheme;

   // see if there is a shortcut key
   char * cp = dStrchr(e.buf, '~');
   e.ascii = cp ? cp[1] : 0;

   // DAW: See if there is a color box defined with the text
   char* cb = dStrchr(e.buf, '|');
   if(cb)
   {
      e.usesColorBox = true;
      cb[0] = '\0';

      char* red = &cb[1];
      cb = dStrchr(red, '|');
      cb[0] = '\0';
      char* green = &cb[1];
      cb = dStrchr(green, '|');
      cb[0] = '\0';
      char* blue = &cb[1];

      U32 r = dAtoi(red);
      U32 g = dAtoi(green);
      U32 b = dAtoi(blue);

      e.colorbox = ColorI(r,g,b);

   } else
   {
      e.usesColorBox = false;
   }

   mEntries.push_back(e);

   if ( mInAction && mTl )
   {
      // Add the new entry:
      mTl->addEntry( e.id, e.buf );
      repositionPopup();
   }
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::addScheme( U32 id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL )
{
   if ( !id )
      return;

   Scheme newScheme;
   newScheme.id = id;
   newScheme.fontColor = fontColor;
   newScheme.fontColorHL = fontColorHL;
   newScheme.fontColorSEL = fontColorSEL;

   mSchemes.push_back( newScheme );
}

//------------------------------------------------------------------------------
S32 GuiPopUpMenuCtrl::getSelected()
{
   if (mSelIndex == -1)
      return 0;
   return mEntries[mSelIndex].id;
}

//------------------------------------------------------------------------------
const char* GuiPopUpMenuCtrl::getTextById(S32 id)
{
   for ( U32 i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( mEntries[i].id == id )
         return( mEntries[i].buf );
   }

   return( "" );
}

//------------------------------------------------------------------------------
S32 GuiPopUpMenuCtrl::findText( const char* text )
{
   for ( U32 i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( dStrcmp( text, mEntries[i].buf ) == 0 )
         return( mEntries[i].id );        
   }
   return( -1 );
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::setSelected(S32 id, bool bNotifyScript )
{
   S32 i;
   for (i = 0; U32(i) < (U32)mEntries.size(); i++)
      if (id == mEntries[i].id)
      {
         i = (mRevNum > i) ? mRevNum - i : i;
         mSelIndex = i;
         if(mReplaceText) // DAW: Only change the displayed text if appropriate.
         {
            setText(mEntries[i].buf);
         }

         // Now perform the popup action:
         char idval[24];
         dSprintf( idval, sizeof(idval), "%d", mEntries[mSelIndex].id );
         if( isMethod( "onSelect" ) && bNotifyScript)
            Con::executef( this, 3, "onSelect", idval, mEntries[mSelIndex].buf );
         return;
      }

      if(mReplaceText) // DAW: Only change the displayed text if appropriate.
      {
         setText("");
      }
      mSelIndex = -1;

      if( isMethod( "onCancel" ) && bNotifyScript)
         Con::executef( this, 1, "onCancel" );

      if( id == -1 )
         return;

      // Execute the popup console command:
      if ( mConsoleCommand[0]  && bNotifyScript )
         Con::evaluate( mConsoleCommand, false );
}

//------------------------------------------------------------------------------
// DAW: Added to set the first item as selected.
void GuiPopUpMenuCtrl::setFirstSelected()
{
   if (mEntries.size() > 0)
   {
      mSelIndex = 0;
      if(mReplaceText) // DAW: Only change the displayed text if appropriate.
      {
         setText(mEntries[0].buf);
      }

      // Now perform the popup action:
      char idval[24];
      dSprintf( idval, sizeof(idval), "%d", mEntries[mSelIndex].id );
      if(isMethod("onSelect"))
         Con::executef( this, 3, "onSelect", idval, mEntries[mSelIndex].buf );
      return;
   }

   if(mReplaceText) // DAW: Only change the displayed text if appropriate.
   {
      setText("");
   }
   mSelIndex = -1;

   Con::executef( this, 1, "onCancel" );

   // Execute the popup console command:
   if ( mConsoleCommand[0] )
      Con::evaluate( mConsoleCommand, false );
}

//------------------------------------------------------------------------------
// DAW: Added to set no items as selected.
void GuiPopUpMenuCtrl::setNoneSelected()
{
   if(mReplaceText) // DAW: Only change the displayed text if appropriate.
   {
      setText("");
   }
   mSelIndex = -1;
}

//------------------------------------------------------------------------------
const char *GuiPopUpMenuCtrl::getScriptValue()
{
   return getText();
}

//------------------------------------------------------------------------------
//-Mat untested
void GuiPopUpMenuCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   Point2I localStart;

   if(mScrollDir != GuiScrollCtrl::None)
      autoScroll();

   S32 renderedBitmapIndex = 1;

   RectI r(offset, mBounds.extent);
    if(mProfile->mBitmapArrayRects.size() >= 4)
    {
        // if size >= 4 then the inactive state images should be present
        S32 index = 3;
        if(!mActive)
        {
            index = 4; // inactive state images are indexes 4 and 5.
            //S32 l = r.point.x, r2 = r.point.x + r.extent.x - 1;
            //S32 t = r.point.y, b = r.point.y + r.extent.y - 1;
            renderFixedBitmapBordersStretchYFilled(r, index, mProfile);
            return;
        }
    }
   if(mInAction)
   {
      S32 l = r.point.x, r2 = r.point.x + r.extent.x - 1;
      S32 t = r.point.y, b = r.point.y + r.extent.y - 1;

      // Do we render a bitmap border or lines?
      if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
      {
         // Render the fixed, filled in border
         renderFixedBitmapBordersStretchYFilled(r, 3, mProfile);
         renderedBitmapIndex = 3;
      } 
      else
      {
         //renderSlightlyLoweredBox(r, mProfile);
         dglDrawRectFill( r, mProfile->mFillColor);
      }

      // DAW: Draw a bitmap over the background?
      if(mTextureDepressed)
      {
         RectI rect(offset, mBitmapBounds);
         dglClearBitmapModulation();
         dglDrawBitmapStretch(mTextureDepressed, rect);
      } else if(mTextureNormal)
      {
         RectI rect(offset, mBitmapBounds);
         dglClearBitmapModulation();
         dglDrawBitmapStretch(mTextureNormal, rect);
      }

      // Do we render a bitmap border or lines?
      if(!(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size()))
      {
         dglDrawLine(l, t, l, b, colorWhite);
         dglDrawLine(l, t, r2, t, colorWhite);
         //dglDrawLine(l + 1, b, r2, b, mProfile->mBorderColor);
         //dglDrawLine(r2, t + 1, r2, b - 1, mProfile->mBorderColor);
      }

   }
   else
      // TODO: Implement
      if(mMouseOver) // TODO: Add onMouseEnter() and onMouseLeave() and a definition of mMouseOver (see guiButtonBaseCtrl) for this to work.
      {
         S32 l = r.point.x, r2 = r.point.x + r.extent.x - 1;
         S32 t = r.point.y, b = r.point.y + r.extent.y - 1;

         // Do we render a bitmap border or lines?
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
         {
            // Render the fixed, filled in border
            renderFixedBitmapBordersStretchYFilled(r, 2, mProfile);
            renderedBitmapIndex = 2;

         } else
         {
            dglDrawRectFill( r, mProfile->mFillColorHL);
         }

         // DAW: Draw a bitmap over the background?
         if(mTextureNormal)
         {
            RectI rect(offset, mBitmapBounds);
            dglClearBitmapModulation();
            dglDrawBitmapStretch(mTextureNormal, rect);
         }

         // Do we render a bitmap border or lines?
         if(!(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size()))
         {
            dglDrawLine(l, t, l, b, colorWhite);
            dglDrawLine(l, t, r2, t, colorWhite);
            //dglDrawLine(l + 1, b, r2, b, mProfile->mBorderColor);
            //dglDrawLine(r2, t + 1, r2, b - 1, mProfile->mBorderColor);
         }
      }
      else
      {

         // Do we render a bitmap border or lines?
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
         {
            // Render the fixed, filled in border
            renderFixedBitmapBordersStretchYFilled(r, 1, mProfile);
            renderedBitmapIndex = 1;

         } else
         {
            dglDrawRectFill( r, mProfile->mFillColorNA);
         }

         // DAW: Draw a bitmap over the background?
         if(mTextureNormal)
         {
            RectI rect(offset, mBitmapBounds);
            dglClearBitmapModulation();
            dglDrawBitmapStretch(mTextureNormal, rect);
         }

         // Do we render a bitmap border or lines?
         if(!(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size()))
         {
            //dglDrawRect(r, mProfile->mBorderColorNA);
         }
      }
      //      renderSlightlyRaisedBox(r, mProfile); // DAW: Used to be the only 'else' condition to mInAction above.

      S32 txt_w = mProfile->mFont->getStrWidth(mText);
      localStart.x = 0;
      localStart.y = (mBounds.extent.y - (mProfile->mFont->getHeight())) / 2;

        // DAW: Indices into the bitmap array
        const S32 NumBitmaps = 3;
        const S32 BorderLeft =     NumBitmaps * renderedBitmapIndex - NumBitmaps;
        const S32 BorderRight =       2 + BorderLeft;

      // align the horizontal
      switch (mProfile->mAlignment)
      {
      case GuiControlProfile::RightAlign:
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
         {
            // We're making use of a bitmap border, so take into account the
            // right cap of the border.
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
            localStart.x = mBounds.extent.x - mBitmapBounds[BorderRight].extent.x - txt_w;

         } else
         {
            localStart.x = mBounds.extent.x - txt_w;  
         }
         break;
      case GuiControlProfile::CenterAlign:

         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
         {
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();

             // GuiControlProfile::LeftAlign
             if(txt_w > (mBounds.extent.x - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x) )
             {
                // We're making use of a bitmap border, so take into account the
                // right cap of the border.
                localStart.x = mBounds.extent.x - mBitmapBounds[BorderRight].extent.x - txt_w;
             }
             else
             {
                // We're making use of a bitmap border, so take into account the
                // right cap of the border.
                localStart.x = mBitmapBounds[BorderLeft].extent.x + ((mBounds.extent.x - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x - txt_w) / 2);
             }

         } else
         {
            localStart.x = (mBounds.extent.x - txt_w) / 2;
         }
         break;
      default:
        // DAW: The width of the text is greater than the width of the control.
        // In this case we will right justify the text and leave some space
        // for the down arrow.
        if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
        {
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();

             // GuiControlProfile::LeftAlign
             if(txt_w > (mBounds.extent.x - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x) )
             {
                // We're making use of a bitmap border, so take into account the
                // right cap of the border.
                RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
                localStart.x = mBounds.extent.x - mBitmapBounds[BorderRight].extent.x - txt_w;
             }
             else
             {
                 localStart.x = mBitmapBounds[BorderLeft].extent.x;
             }

        } else
        {
            localStart.x = mBounds.extent.x - txt_w - 12;
        }
         break;
      }

      // DAW: Do we first draw a colored box beside the text?
      ColorI boxColor;
      bool drawbox = getColoredBox( boxColor, mSelIndex);
      if(drawbox)
      {
         Point2I coloredboxsize(15,10);
         RectI r(offset.x + mProfile->mTextOffset.x, offset.y + ((mBounds.extent.y - coloredboxsize.y) / 2), coloredboxsize.x, coloredboxsize.y);
         dglDrawRectFill( r, boxColor);
         dglDrawRect( r, ColorI(0,0,0));

         localStart.x += coloredboxsize.x + mProfile->mTextOffset.x;
      }

      // Draw the text
      Point2I globalStart = localToGlobalCoord(localStart);
      ColorI fontColor   = mActive ? (mInAction ? mProfile->mFontColorNA : mProfile->mFontColor) : mProfile->mFontColorNA;

      dglSetBitmapModulation(fontColor); // DAW: was: (mProfile->mFontColor);


        // Save the clip rectangle.
        const RectI previousClipRect = dglGetClipRect();
      
        // Set a text rendering clip rectangle when we're using bitmap edges.
        if( mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size() )
        {
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
            dglSetClipRect( RectI( r.point.x + mBitmapBounds[BorderLeft].extent.x, r.point.y, r.extent.x - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x, r.extent.y ) );
        }

      // DAW: Get the number of columns in the text
      S32 colcount = getColumnCount(mText, "\t");

      // DAW: Are there two or more columns?
      if(colcount >= 2)
      {
         char buff[256];

         // Draw the first column
         getColumn(mText, buff, 0, "\t");
         dglDrawText(mProfile->mFont, globalStart, buff, mProfile->mFontColors);

         // Draw the second column to the right
         getColumn(mText, buff, 1, "\t");
         S32 txt_w = mProfile->mFont->getStrWidth(buff);
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
         {
            // We're making use of a bitmap border, so take into account the
            // right cap of the border.
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
            Point2I textpos = localToGlobalCoord(Point2I(mBounds.extent.x - txt_w - mBitmapBounds[2].extent.x,localStart.y));
            dglDrawText(mProfile->mFont, textpos, buff, mProfile->mFontColors);

         } else
         {
            Point2I textpos = localToGlobalCoord(Point2I(mBounds.extent.x - txt_w - 12,localStart.y));
            dglDrawText(mProfile->mFont, textpos, buff, mProfile->mFontColors);
         }

      } else
      {
         dglDrawText(mProfile->mFont, globalStart, mText, mProfile->mFontColors);
      }

      // Restore the clip rectangle.
      dglSetClipRect( previousClipRect );

      // If we're rendering a bitmap border, then it will take care of the arrow.
      if(!(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size()))
      {
         // DAW: Draw a triangle (down arrow)
         F32 left = (F32)(r.point.x + r.extent.x - 12);
         F32 right = (F32)(left + 8);
         F32 middle = (F32)(left + 4);
         F32 top = (F32)(r.extent.y / 2 + r.point.y - 4);
         F32 bottom = (F32)(top + 8);

         glBegin(GL_TRIANGLES);
         glColor3i(mProfile->mFontColor.red,mProfile->mFontColor.green,mProfile->mFontColor.blue);
         glVertex2fv( Point3F(left,top,0) );
         glVertex2fv( Point3F(right,top,0) );
         glVertex2fv( Point3F(middle,bottom,0) );
         glEnd();
      }
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::closePopUp()
{
   if ( !mInAction )
      return;

   // Get the selection from the text list:
   mSelIndex = mTl->getSelectedCell().y;
   mSelIndex = (mRevNum >= mSelIndex && mSelIndex != -1) ? mRevNum - mSelIndex : mSelIndex;
   if ( mSelIndex != -1 )
   {
      if(mReplaceText)
         setText( mEntries[mSelIndex].buf );
      setIntVariable( mEntries[mSelIndex].id );
   }

   // Release the mouse:
   mInAction = false;
   mTl->mouseUnlock();

   // DAW: Commented out below and moved to the end of the function.  See the
   // note below for the reason why.
   /*
   // Pop the background:
   getRoot()->popDialogControl(mBackground);

   // Kill the popup:
   mBackground->removeObject( mSc );
   mTl->deleteObject();
   mSc->deleteObject();
   mBackground->deleteObject();

   // Set this as the first responder:
   setFocus();
   */

   // Now perform the popup action:
   if ( mSelIndex != -1 )
   {
      char idval[24];
      dSprintf( idval, sizeof(idval), "%d", mEntries[mSelIndex].id );
      if(isMethod("onSelect"))
         Con::executef( this, 3, "onSelect", idval, mEntries[mSelIndex].buf );
   }
   else if(isMethod("onSelect"))
      Con::executef( this, 1, "onCancel" );

   // Execute the popup console command:
   if ( mConsoleCommand[0] )
      Con::evaluate( mConsoleCommand, false );

   // DAW: Reordered this pop dialog to be after the script select callback.  When the
   // background was popped it was causing all sorts of focus issues when
   // suddenly the first text edit control took the focus before it came back
   // to this popup.

   // Pop the background:
   getRoot()->popDialogControl(mBackground);

   // Kill the popup:
   mBackground->removeObject( mSc );
   mTl->deleteObject();
   mSc->deleteObject();
   mBackground->deleteObject();

   // Set this as the first responder:
   setFirstResponder();
}

//------------------------------------------------------------------------------
bool GuiPopUpMenuCtrl::onKeyDown(const GuiEvent &event)
{
   //if the control is a dead end, don't process the input:
   if ( !mVisible || !mActive || !mAwake )
      return false;

   //see if the key down is a <return> or not
   if ( event.keyCode == KEY_RETURN && event.modifier == 0 )
   {
      onAction();
      return true;
   }

   //otherwise, pass the event to its parent
   return Parent::onKeyDown( event );
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::onAction()
{
    if ( !mVisible || !mActive || !mAwake )
      return;

   GuiControl *canCtrl = getParent();

   addChildren();

   GuiCanvas *root = getRoot();
   Point2I windowExt = root->mBounds.extent;

   mBackground->mBounds.point.set(0,0);
   mBackground->mBounds.extent = root->mBounds.extent;

   S32 textWidth = 0, width = mBounds.extent.x;
   //const S32 menuSpace = 5; // DAW: Removed as no longer used.
   const S32 textSpace = 2;
   bool setScroll = false;

   for( U32 i=0; i < (U32)mEntries.size(); ++i )
      if(S32(mProfile->mFont->getStrWidth(mEntries[i].buf)) > textWidth)
         textWidth = mProfile->mFont->getStrWidth(mEntries[i].buf);

   //if(textWidth > mBounds.extent.x)
   S32 sbWidth = 0;//mSc->mProfile->mBorderSize * 2 + mSc->scrollBarThickness(); // DAW: Calculate the scroll bar width
   if(textWidth > (mBounds.extent.x - sbWidth-mProfile->mTextOffset.x)) // DAW: The text draw area to test against is the width of the drop-down minus the scroll bar width, the text margin and the scroll bar child margins.
   {
      //textWidth +=10;
      textWidth +=sbWidth + mProfile->mTextOffset.x; // DAW: The new width is the width of the text plus the scroll bar width plus the text margin size plus the scroll bar child margins.
      width = textWidth;

      // DAW: If a child margin is not defined for the scroll control, let's add
      //      some space between the text and scroll control for readability
      //if(mSc->getChildMargin().x == 0)
      //   width += 2;
   }

   //mTl->setCellSize(Point2I(width, mFont->getHeight()+3));
   mTl->setCellSize(Point2I(width, mProfile->mFont->getHeight() + textSpace)); // DAW: Modified the above line to use textSpace rather than the '3' as this is what is used below.

   for( U32 j = 0; j < (U32)mEntries.size(); ++j )
      mTl->addEntry(mEntries[j].id, mEntries[j].buf);

   Point2I pointInGC = canCtrl->localToGlobalCoord(mBounds.point);
   Point2I scrollPoint(pointInGC.x, pointInGC.y + mBounds.extent.y); 

   //Calc max Y distance, so Scroll Ctrl will fit on window 
   //S32 maxYdis = windowExt.y - pointInGC.y - mBounds.extent.y - menuSpace; 
   S32 sbBorder = 0;//TODO: mSc->mProfile->mBorderSize * 2 + mSc->getChildMargin().y * 2; // DAW: Added to take into account the border thickness and the margin of the child controls of the scroll control when figuring out the size of the contained text list control
   S32 maxYdis = windowExt.y - pointInGC.y - mBounds.extent.y - sbBorder; // - menuSpace; // DAW: Need to remove the border thickness from the contained control maximum extent and got rid of the 'menuspace' variable

   //If scroll bars need to be added
   mRevNum = 0; // DAW: Added here rather than within the following 'if' statements.
   if(maxYdis < mTl->mBounds.extent.y + sbBorder) // DAW: Instead of adding sbBorder, it was: 'textSpace'
   {
      //Should we pop menu list above the button
      if(maxYdis < pointInGC.y ) // DAW: removed: '- menuSpace)' from check to see if there is more space above the control than below.
      {
         if(mReverseTextList) // DAW: Added this check if we should reverse the text list.
            reverseTextList();

         maxYdis = pointInGC.y; // DAW: Was at the end: '- menuSpace;'
         //Does the menu need a scroll bar 
         if(maxYdis < mTl->mBounds.extent.y + sbBorder) // DAW: Instead of adding sbBorder, it was: 'textSpace'
         {
            // DAW: Removed width recalculation for scroll bar as the scroll bar is already being taken into account.
            //Calc for the width of the scroll bar 
            //            if(textWidth >=  width)
            //               width += 20;
            //            mTl->setCellSize(Point2I(width,mFont->getHeight() + textSpace));

            //Pop menu list above the button
            //            scrollPoint.set(pointInGC.x, menuSpace - 1); // DAW: Removed as calculated outside the 'if', and was wrong to begin with
            setScroll = true;
         }
         //No scroll bar needed
         else
         {
            maxYdis = mTl->mBounds.extent.y + sbBorder; // DAW: Instead of adding sbBorder, it was: 'textSpace'
            //            scrollPoint.set(pointInGC.x, pointInGC.y - maxYdis -1); // DAW: Removed as calculated outside the 'if' and the '-1' at the end is wrong
         }

         // DAW: Added the next two lines
         scrollPoint.set(pointInGC.x, pointInGC.y - maxYdis); // DAW: Used to have the following on the end: '-1);'
      } 
      //Scroll bar needed but Don't pop above button
      else
      {
         //mRevNum = 0; // DAW: Commented out as it has been added at the beginning of this function
         if ( mSelIndex >= 0 )
            mTl->setSelectedCell( Point2I( 0, mSelIndex ) ); // DAW: Added as we were not setting the selected cell if the list is displayed down.

         // DAW: Removed width recalculation for scroll bar as the scroll bar is already being taken into account.
         //Calc for the width of the scroll bar 
         //         if(textWidth >=  width)
         //            width += 20;
         //         mTl->setCellSize(Point2I(width,mFont->getHeight() + textSpace));
         setScroll = true;
      }
   }
   //No scroll bar needed
   else
   {
      if ( mSelIndex >= 0 )
         mTl->setSelectedCell( Point2I( 0, mSelIndex ) ); // DAW: Added as we were not setting the selected cell if the list is displayed down.

      //maxYdis = mTl->mBounds.extent.y + textSpace;
      maxYdis = mTl->mBounds.extent.y + sbBorder; // DAW: Added in the border thickness of the scroll control and removed the addition of textSpace
   }

   //offset it from the background so it lines up properly
   mSc->mBounds.point = mBackground->globalToLocalCoord(scrollPoint);

   if(mSc->mBounds.point.x + width > mBackground->mBounds.extent.x)
      if(width - mBounds.extent.x > 0)
         mSc->mBounds.point.x -= width - mBounds.extent.x;

   //mSc->mBounds.extent.set(width-1, maxYdis);
   mSc->mBounds.extent.set(width, maxYdis); // DAW: Not sure why the '-1' above.

   mSc->registerObject();
   mTl->registerObject();
   mBackground->registerObject();

   mSc->addObject( mTl );
   mBackground->addObject( mSc );

   mBackgroundCancel = false; // DAW: Setup check if user clicked on the background instead of the text list (ie: didn't want to change their current selection).

   root->pushDialogControl(mBackground, 99);

   if ( setScroll )
   {
      if ( mSelIndex )
         mTl->scrollCellVisible( Point2I(0, mSelIndex));
      else
         mTl->scrollCellVisible( Point2I( 0, 0 ) );
   }

   mTl->setFirstResponder();

   mInAction = true;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::addChildren()
{
   mTl = new GuiPopupTextListCtrl(this);
   AssertFatal(mTl, "Failed to create the GuiPopUpTextListCtrl for the PopUpMenu");
   mTl->mProfile = mProfile->mProfileForChildren ? mProfile->mProfileForChildren : mProfile; // Use the children's profile rather than the parent's profile, if it exists.
   mTl->setField("noDuplicates", "false");

   mSc = new GuiScrollCtrl;
   AssertFatal(mSc, "Failed to create the GuiScrollCtrl for the PopUpMenu");
   mSc->mProfile = mProfile->mProfileForChildren ? mProfile->mProfileForChildren : mProfile; // Use the children's profile rather than the parent's profile, if it exists.
   mSc->setField("hScrollBar","AlwaysOff");
   mSc->setField("vScrollBar","dynamic");
   //if(mRenderScrollInNA) // DAW: Force the scroll control to render using fillColorNA rather than fillColor
   // mSc->mUseNABackground = true;

   mBackground = new GuiPopUpBackgroundCtrl(this, mTl);
   AssertFatal(mBackground, "Failed to create the GuiBackgroundCtrl for the PopUpMenu");
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::repositionPopup()
{
   if ( !mInAction || !mSc || !mTl )
      return;

   // I'm not concerned with this right now...
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::reverseTextList()
{
   mTl->clear();
   for(S32 i=mEntries.size()-1; i >= 0; --i)
      mTl->addEntry(mEntries[i].id, mEntries[i].buf);

   // Don't lose the selected cell:
   if ( mSelIndex >= 0 )
      mTl->setSelectedCell( Point2I( 0, mEntries.size() - mSelIndex - 1 ) ); 

   mRevNum = mEntries.size() - 1;
}

//------------------------------------------------------------------------------
bool GuiPopUpMenuCtrl::getFontColor( ColorI &fontColor, S32 id, bool selected, bool mouseOver )
{
   U32 i;
   Entry* entry = NULL;
   for ( i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( mEntries[i].id == id )
      {
         entry = &mEntries[i];
         break;
      }
   }

   if ( !entry )
      return( false );

   if ( entry->scheme != 0 )
   {
      // Find the entry's color scheme:
      for ( i = 0; i < (U32)mSchemes.size(); i++ )
      {
         if ( mSchemes[i].id == entry->scheme )
         {
            fontColor = selected ? mSchemes[i].fontColorSEL : mouseOver ? mSchemes[i].fontColorHL : mSchemes[i].fontColor;
            return( true );
         }
      }
   }

   // Default color scheme...
   fontColor = selected ? mProfile->mFontColorSL : mouseOver ? mProfile->mFontColorHL : mProfile->mFontColorNA; // DAW: Modified the final color choice from mProfile->mFontColor to mProfile->mFontColorNA

   return( true );
}

//------------------------------------------------------------------------------
// DAW: Added
bool GuiPopUpMenuCtrl::getColoredBox( ColorI &fontColor, S32 id )
{
   U32 i;
   Entry* entry = NULL;
   for ( i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( mEntries[i].id == id )
      {
         entry = &mEntries[i];
         break;
      }
   }

   if ( !entry )
      return( false );

   if( entry->usesColorBox == false)
      return false;

   fontColor = entry->colorbox;

   return true;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::onMouseDown(const GuiEvent &event)
{
    if ( !mVisible || !mActive || !mAwake )
      return;

   onAction();
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::onMouseUp(const GuiEvent &event)
{
    if ( !mVisible || !mActive || !mAwake )
      return;
}

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrl::onMouseEnter(const GuiEvent &event)
{
    if ( !mVisible || !mActive || !mAwake )
      return;
   mMouseOver = true;
}

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrl::onMouseLeave(const GuiEvent &)
{
    if ( !mVisible || !mActive || !mAwake )
      return;
   mMouseOver = false;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::setupAutoScroll(const GuiEvent &event)
{
   GuiControl *parent = getParent();
   if (! parent) return;

   Point2I mousePt = mSc->globalToLocalCoord(event.mousePoint);

   mEventSave = event;      

   if(mLastYvalue != mousePt.y)
   {
      mScrollDir = GuiScrollCtrl::None;
      if(mousePt.y > mSc->mBounds.extent.y || mousePt.y < 0)
      {
         S32 topOrBottom = (mousePt.y > mSc->mBounds.extent.y) ? 1 : 0;
         mSc->scrollTo(0, topOrBottom);
         return;
      }   

      F32 percent = (F32)mousePt.y / (F32)mSc->mBounds.extent.y;
      if(percent > 0.7f && mousePt.y > mLastYvalue)
      {
         mIncValue = percent - 0.5f;
         mScrollDir = GuiScrollCtrl::DownArrow;
      }
      else if(percent < 0.3f && mousePt.y < mLastYvalue)
      {
         mIncValue = 0.5f - percent;         
         mScrollDir = GuiScrollCtrl::UpArrow;
      }
      mLastYvalue = mousePt.y;
   }
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::autoScroll()
{
   mScrollCount += mIncValue;

   while(mScrollCount > 1)
   {
      mSc->scrollByRegion(mScrollDir);
      mScrollCount -= 1;
   }
   mTl->onTouchMove(mEventSave);
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrl::replaceText(S32 boolVal)
{
   mReplaceText = boolVal;
}
