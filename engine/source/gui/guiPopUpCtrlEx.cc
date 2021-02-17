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
#include "gui/guiPopUpCtrlEx.h"
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

GuiPopUpBackgroundCtrlEx::GuiPopUpBackgroundCtrlEx(GuiPopUpMenuCtrlEx *ctrl, GuiPopupTextListCtrlEx *textList)
{
   mPopUpCtrl = ctrl;
   mTextList = textList;
}

void GuiPopUpBackgroundCtrlEx::onMouseDown(const GuiEvent &event)
{
//   mTextList->setSelectedCell(Point2I(-1,-1)); // DAW: Removed
   mPopUpCtrl->mBackgroundCancel = true; // DAW: Set that the user didn't click within the text list.  Replaces the line above.
   mPopUpCtrl->closePopUp();
}

//------------------------------------------------------------------------------
GuiPopupTextListCtrlEx::GuiPopupTextListCtrlEx()
{
   mPopUpCtrl = NULL;
}


//------------------------------------------------------------------------------
GuiPopupTextListCtrlEx::GuiPopupTextListCtrlEx(GuiPopUpMenuCtrlEx *ctrl)
{
   mPopUpCtrl = ctrl;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//void GuiPopUpTextListCtrl::onCellSelected( Point2I /*cell*/ )
//{
//   // Do nothing, the parent control will take care of everything...
//}
void GuiPopupTextListCtrlEx::onCellSelected( Point2I cell )
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

bool GuiPopupTextListCtrlEx::hasCategories()
{
   for( S32 i = 0; i < mList.size(); i++)
   {
      if( mList[i].id == -1)
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
bool GuiPopupTextListCtrlEx::onKeyDown(const GuiEvent &event)
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

void GuiPopupTextListCtrlEx::onMouseUp(const GuiEvent &event)
{
    if(!mActive)
        return;

   Point2I pt = globalToLocalCoord(event.mousePoint);
   Point2I cell(
      (pt.x < 0 ? -1 : pt.x / mCellSize.x), 
      (pt.y < 0 ? -1 : pt.y / mCellSize.y)
      );

   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
   {
      if (mList[cell.y].id == -1)
         return;
   }

   Parent::onTouchDown(event);
   mPopUpCtrl->closePopUp();
   Parent::onTouchUp(event);
}

void GuiPopupTextListCtrlEx::onMouseMove( const GuiEvent &event )
{
   if( !mPopUpCtrl || !mPopUpCtrl->isMethod("onHotTrackItem") )
      return Parent::onTouchMove( event );

   Point2I pt = globalToLocalCoord(event.mousePoint);
   Point2I cell( (pt.x < 0 ? -1 : pt.x / mCellSize.x), (pt.y < 0 ? -1 : pt.y / mCellSize.y) );

   // Within Bounds?
   if (cell.x >= 0 && cell.x < mSize.x && cell.y >= 0 && cell.y < mSize.y)
      // Hot Track notification
      Con::executef( mPopUpCtrl, 2, "onHotTrackItem", Con::getIntArg(mList[cell.y].id) );
   else 
      // Hot Track -1
      Con::executef( mPopUpCtrl, 2, "onHotTrackItem", Con::getIntArg(-1) );

   // Call Parent
   Parent::onTouchMove(event);
}

//------------------------------------------------------------------------------
void GuiPopupTextListCtrlEx::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
   // DAW: Get the size of the cells
   Point2I size;
   getCellSize(size);
   if(mouseOver && (mList[cell.y].id != -1))
   {
      // DAW: Render a background color for the cell
      RectI cellR(offset.x, offset.y, size.x, size.y);
	  ColorI color(0,0,0);
      dglDrawRectFill(cellR, color);

   } 
   else if(selected)
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

   // Render 'Group' background
   if(mList[cell.y].id == -1)
   {
      RectI cellR(offset.x, offset.y, size.x, size.y);
      dglDrawRectFill(cellR, mProfile->mFillColorHL );
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

   }
   else
   {

      if ((mList[cell.y].id == -1) || (!hasCategories()))
         dglDrawText( mFont, Point2I( textXOffset, offset.y ), mList[cell.y].text ); // DAW: Used mTextOffset as a margin for the text list rather than the hard coded value of '4'.
      else
         dglDrawText( mFont, Point2I( textXOffset + 8, offset.y ), mList[cell.y].text ); // DAW: Used mTextOffset as a margin for the text list rather than the hard coded value of '4'.
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiPopUpMenuCtrlEx);

GuiPopUpMenuCtrlEx::GuiPopUpMenuCtrlEx(void)
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
   mHotTrackItems = false;
}

//------------------------------------------------------------------------------
GuiPopUpMenuCtrlEx::~GuiPopUpMenuCtrlEx()
{

}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::initPersistFields(void)
{
   Parent::initPersistFields();
   
   addField("maxPopupHeight",           TypeS32,          Offset(mMaxPopupHeight, GuiPopUpMenuCtrlEx));
   addField("sbUsesNAColor",            TypeBool,         Offset(mRenderScrollInNA, GuiPopUpMenuCtrlEx));
   addField("reverseTextList",          TypeBool,         Offset(mReverseTextList, GuiPopUpMenuCtrlEx));
   addField("bitmap",                   TypeFilename,     Offset(mBitmapName, GuiPopUpMenuCtrlEx));
   addField("bitmapBounds",             TypePoint2I,      Offset(mBitmapBounds, GuiPopUpMenuCtrlEx));
   addField("hotTrackCallback",         TypeBool,         Offset(mHotTrackItems, GuiPopUpMenuCtrlEx),
      "Whether to provide a 'onHotTrackItem' callback when a list item is hovered over");
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrlEx, add, void, 4, 5, "(string name, int idNum, int scheme=0) Adds a new pop up menu control\n"
			  "@param name The control's name\n"
			  "@param idNum The control's ID\n"
			  "@param scheme The selected scheme (default 0)\n"
			  "@return No Return Value")
{
   if ( argc > 4 )
      object->addEntry(argv[2],dAtoi(argv[3]),dAtoi(argv[4]));
   else
      object->addEntry(argv[2],dAtoi(argv[3]),0);
}

ConsoleMethod( GuiPopUpMenuCtrlEx, addCategory, void, 3, 3, "(string text) Adds the given category to the list\n"
			  "@param text The category to add\n"
			  "@return No return value.")
{
   object->addEntry(argv[2], -1, 0);
}

ConsoleMethod( GuiPopUpMenuCtrlEx, addScheme, void, 6, 6, "(int id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL) Adds the defined scheme\n"
			  "@param id The object id of the scheme.\n"
			  "@param fontcolor The desired color of the font.\n"
			  "@param fontColorHL The desired color on highlight.\n"
			  "@param fontColorSEL The desired color on select.\n"
			  "@return No return value.")
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

ConsoleMethod( GuiPopUpMenuCtrlEx, setText, void, 3, 3, "(string text) Set control text to given string.\n"
			  "@param text The desired text.\n"
			  "@return No return vlaue.")
{
   object->setText(argv[2]);
}

ConsoleMethod( GuiPopUpMenuCtrlEx, getText, const char*, 2, 2, "()\n @return Returns the control's text")
{
   return object->getText();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, clear, void, 2, 2, "() Clear the popup list.\n"
			  "@return No return value.")
{
   object->clear();
}

ConsoleMethod(GuiPopUpMenuCtrlEx, sort, void, 2, 2, "() Sort the list alphabetically.\n"
			  "@return No return value.")
{
   object->sort();
}

// DAW: Added to sort the entries by ID
ConsoleMethod(GuiPopUpMenuCtrlEx, sortID, void, 2, 2, "() Sort the list by ID.\n"
			  "@return No return value.")
{
   object->sortID();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, forceOnAction, void, 2, 2, "")
{
   object->onAction();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, forceClose, void, 2, 2, "")
{
   object->closePopUp();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, getSelected, S32, 2, 2, "")
{
   return object->getSelected();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, setSelected, void, 3, 3, "(int id)")
{
   object->setSelected(dAtoi(argv[2]));
}

ConsoleMethod( GuiPopUpMenuCtrlEx, setFirstSelected, void, 2, 2, "")
{
   object->setFirstSelected();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, setNoneSelected, void, 2, 2, "")
{
   object->setNoneSelected();
}

ConsoleMethod( GuiPopUpMenuCtrlEx, getTextById, const char*, 3, 3,  "(int id)")
{
   return(object->getTextById(dAtoi(argv[2])));
}

ConsoleMethod( GuiPopUpMenuCtrlEx, setEnumContent, void, 4, 4, "(string class, string enum)"
              "This fills the popup with a classrep's field enumeration type info.\n\n"
              "More of a helper function than anything.   If console access to the field list is added, "
              "at least for the enumerated types, then this should go away..")
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
ConsoleMethod( GuiPopUpMenuCtrlEx, findText, S32, 3, 3, "(string text)"
              "Returns the position of the first entry containing the specified text.")
{
   return( object->findText( argv[2] ) );   
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrlEx, size, S32, 2, 2, "Get the size of the menu - the number of entries in it.")
{
   return( object->getNumEntries() ); 
}

//------------------------------------------------------------------------------
ConsoleMethod( GuiPopUpMenuCtrlEx, replaceText, void, 3, 3, "(bool doReplaceText)")
{
   object->replaceText(dAtoi(argv[2]));  
}

//------------------------------------------------------------------------------
// DAW: Added
bool GuiPopUpMenuCtrlEx::onWake()
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
bool GuiPopUpMenuCtrlEx::onAdd()
{
   if (!Parent::onAdd())
      return false;
   mSelIndex = -1;
   mReplaceText = true;
   return true;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::onSleep()
{
   mTextureNormal = NULL; // DAW: Added
   mTextureDepressed = NULL; // DAW: Added
   Parent::onSleep();
   closePopUp();  // Tests in function.
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::clear()
{
   mEntries.setSize(0);
   setText("");
   mSelIndex = -1;
   mRevNum = 0;
}

//------------------------------------------------------------------------------
static S32 QSORT_CALLBACK textCompare(const void *a,const void *b)
{
   GuiPopUpMenuCtrlEx::Entry *ea = (GuiPopUpMenuCtrlEx::Entry *) (a);
   GuiPopUpMenuCtrlEx::Entry *eb = (GuiPopUpMenuCtrlEx::Entry *) (b);
   return (dStricmp(ea->buf, eb->buf));
} 

// DAW: Added to sort by entry ID
//------------------------------------------------------------------------------
static S32 QSORT_CALLBACK idCompare(const void *a,const void *b)
{
   GuiPopUpMenuCtrlEx::Entry *ea = (GuiPopUpMenuCtrlEx::Entry *) (a);
   GuiPopUpMenuCtrlEx::Entry *eb = (GuiPopUpMenuCtrlEx::Entry *) (b);
   return ( (ea->id < eb->id) ? -1 : ((ea->id > eb->id) ? 1 : 0) );
} 

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrlEx::setBitmap(const char *name)
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
void GuiPopUpMenuCtrlEx::sort()
{
   if (!mEntries.empty())
      dQsort((void *)&(mEntries[0]), mEntries.size(), sizeof(Entry), textCompare);
}

// DAW: Added to sort by entry ID
//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::sortID()
{
   if (!mEntries.empty())
      dQsort((void *)&(mEntries[0]), mEntries.size(), sizeof(Entry), idCompare);
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::addEntry(const char *buf, S32 id, U32 scheme)
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
void GuiPopUpMenuCtrlEx::addScheme( U32 id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL )
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
S32 GuiPopUpMenuCtrlEx::getSelected()
{
   if (mSelIndex == -1)
      return 0;
   return mEntries[mSelIndex].id;
}

//------------------------------------------------------------------------------
const char* GuiPopUpMenuCtrlEx::getTextById(S32 id)
{
   for ( U32 i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( mEntries[i].id == id )
         return( mEntries[i].buf );
   }

   return( "" );
}

//------------------------------------------------------------------------------
S32 GuiPopUpMenuCtrlEx::findText( const char* text )
{
   for ( U32 i = 0; i < (U32)mEntries.size(); i++ )
   {
      if ( dStrcmp( text, mEntries[i].buf ) == 0 )
         return( mEntries[i].id );        
   }
   return( -1 );
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::setSelected(S32 id)
{
   S32 i;
   for (i = 0; i < mEntries.size(); i++)
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
         if( isMethod( "onSelect" ) )
            Con::executef( this, 3, "onSelect", idval, mEntries[mSelIndex].buf );
         return;
      }

   if(mReplaceText) // DAW: Only change the displayed text if appropriate.
   {
      setText("");
   }
   mSelIndex = -1;

   Con::executef( this, 1, "onCancel" );
   
   if( id == -1 )
      return;

   // Execute the popup console command:
   if ( mConsoleCommand[0] )
      Con::evaluate( mConsoleCommand, false );
}

//------------------------------------------------------------------------------
// DAW: Added to set the first item as selected.
void GuiPopUpMenuCtrlEx::setFirstSelected()
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
void GuiPopUpMenuCtrlEx::setNoneSelected()
{
   if(mReplaceText) // DAW: Only change the displayed text if appropriate.
   {
      setText("");
   }
   mSelIndex = -1;
}

//------------------------------------------------------------------------------
const char *GuiPopUpMenuCtrlEx::getScriptValue()
{
   return getText();
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::onRender(Point2I offset, const RectI &updateRect)
{
   Point2I localStart;
   
   if(mScrollDir != GuiScrollCtrl::None)
      autoScroll();

   RectI r(offset, mBounds.extent);
   if(mInAction)
   {
      S32 l = r.point.x, r2 = r.point.x + r.extent.x - 1;
      S32 t = r.point.y, b = r.point.y + r.extent.y - 1;

      // Do we render a bitmap border or lines?
      if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
	  {
         // Render the fixed, filled in border
         renderFixedBitmapBordersFilled(r, 3, mProfile);

	  } else
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
         renderFixedBitmapBordersFilled(r, 2, mProfile);

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
         renderFixedBitmapBordersFilled(r, 1, mProfile);

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

   S32 txt_w = mFont->getStrWidth(mText);
   localStart.x = 0;
   localStart.y = (mBounds.extent.y - (mFont->getHeight())) / 2;

   // align the horizontal
   switch (mProfile->mAlignment)
   {
      case GuiControlProfile::RightAlign:
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
		 {
            // We're making use of a bitmap border, so take into account the
            // right cap of the border.
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
            localStart.x = mBounds.extent.x - mBitmapBounds[2].extent.x - txt_w;

		 } else
		 {
            localStart.x = mBounds.extent.x - txt_w;  
         }
         break;
      case GuiControlProfile::CenterAlign:
         if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
		 {
            // We're making use of a bitmap border, so take into account the
            // right cap of the border.
            RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
            localStart.x = (mBounds.extent.x - mBitmapBounds[2].extent.x - txt_w) / 2;

		 } else
		 {
            localStart.x = (mBounds.extent.x - txt_w) / 2;
         }
         break;
      default:
         // GuiControlProfile::LeftAlign
         if(txt_w > mBounds.extent.x)
		 {
            // DAW: The width of the text is greater than the width of the control.
			// In this case we will right justify the text and leave some space
			// for the down arrow.
            if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
            {
               // We're making use of a bitmap border, so take into account the
               // right cap of the border.
               RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
               localStart.x = mBounds.extent.x - mBitmapBounds[2].extent.x - txt_w;

		    } else
		    {
			   localStart.x = mBounds.extent.x - txt_w - 12;
			}
		 } else
		 {
            localStart.x = mProfile->mTextOffset.x; // DAW: Use mProfile->mTextOffset as a controlable margin for the control's text.
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
   ColorI fontColor   = mActive ? (mInAction ? mProfile->mFontColor : mProfile->mFontColorNA) : mProfile->mFontColorNA;
   dglSetBitmapModulation(fontColor); // DAW: was: (mProfile->mFontColor);

   // DAW: Get the number of columns in the text
   S32 colcount = getColumnCount(mText, "\t");

   // DAW: Are there two or more columns?
   if(colcount >= 2)
   {
      char buff[256];

      // Draw the first column
      getColumn(mText, buff, 0, "\t");
      dglDrawText(mFont, globalStart, buff, mProfile->mFontColors);

	  // Draw the second column to the right
      getColumn(mText, buff, 1, "\t");
      S32 txt_w = mFont->getStrWidth(buff);
      if(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size())
      {
         // We're making use of a bitmap border, so take into account the
         // right cap of the border.
         RectI* mBitmapBounds = mProfile->mBitmapArrayRects.address();
         Point2I textpos = localToGlobalCoord(Point2I(mBounds.extent.x - txt_w - mBitmapBounds[2].extent.x,localStart.y));
         dglDrawText(mFont, textpos, buff, mProfile->mFontColors);

      } else
      {
         Point2I textpos = localToGlobalCoord(Point2I(mBounds.extent.x - txt_w - 12,localStart.y));
         dglDrawText(mFont, textpos, buff, mProfile->mFontColors);
	  }

   } else
   {
      dglDrawText(mFont, globalStart, mText, mProfile->mFontColors);
   }

   // If we're rendering a bitmap border, then it will take care of the arrow.
   if(!(mProfile->mProfileForChildren && mProfile->mBitmapArrayRects.size()))
   {
      // DAW: Draw a triangle (down arrow)
      F32 left = (F32)(r.point.x + r.extent.x - 12);
      F32 right = (F32)(left + 8);
      F32 middle = (F32)(left + 4);
      F32 top = (F32)(r.extent.y / 2 + r.point.y - 4);
      F32 bottom = (F32)(top + 8);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat untested
       glColor4ub(mProfile->mFontColor.red,mProfile->mFontColor.green,mProfile->mFontColor.blue, 255);
	   GLfloat verts[] = {
         left, top,
         right, top,
         middle, bottom,
	   };
	   glVertexPointer(2, GL_FLOAT, 0, verts);	   
	   glDrawArrays(GL_TRIANGLES, 0, 4);
#else
      glBegin(GL_TRIANGLES);
         glColor3i(mProfile->mFontColor.red,mProfile->mFontColor.green,mProfile->mFontColor.blue);
         glVertex2fv( Point3F(left,top,0) );
         glVertex2fv( Point3F(right,top,0) );
         glVertex2fv( Point3F(middle,bottom,0) );
      glEnd();
#endif
   }
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::closePopUp()
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
   //mTl->mouseUnlock();

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
bool GuiPopUpMenuCtrlEx::onKeyDown(const GuiEvent &event)
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
void GuiPopUpMenuCtrlEx::onAction()
{
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
      
   for( U32 i = 0; i < (U32)mEntries.size(); ++i )
      if(S32(mFont->getStrWidth(mEntries[i].buf)) > textWidth)
         textWidth = mFont->getStrWidth(mEntries[i].buf);

   //if(textWidth > mBounds.extent.x)
   S32 sbWidth = 0;//TODO: mSc->mProfile->mBorderSize * 2 + mSc->scrollBarThickness(); // DAW: Calculate the scroll bar width
   if(textWidth > (mBounds.extent.x - sbWidth-mProfile->mTextOffset.x)) // DAW: The text draw area to test against is the width of the drop-down minus the scroll bar width, the text margin and the scroll bar child margins.
   {
      //textWidth +=10;
      textWidth +=sbWidth + mProfile->mTextOffset.x; // DAW: The new width is the width of the text plus the scroll bar width plus the text margin size plus the scroll bar child margins.
      width = textWidth;
	  
	  // DAW: If a child margin is not defined for the scroll control, let's add
	  //      some space between the text and scroll control for readability
	  //if(mSc->getChildMargin().x == 0)
		//  width += 2;
   }

   //mTl->setCellSize(Point2I(width, mFont->getHeight()+3));
   mTl->setCellSize(Point2I(width, mFont->getHeight() + textSpace)); // DAW: Modified the above line to use textSpace rather than the '3' as this is what is used below.

   for( U32 j=0; j < (U32)mEntries.size(); ++j )
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
void GuiPopUpMenuCtrlEx::addChildren()
{
   mTl = new GuiPopupTextListCtrlEx(this);
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

   mBackground = new GuiPopUpBackgroundCtrlEx(this, mTl);
   AssertFatal(mBackground, "Failed to create the GuiBackgroundCtrl for the PopUpMenu");
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::repositionPopup()
{
   if ( !mInAction || !mSc || !mTl )
      return;

   // I'm not concerned with this right now...
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::reverseTextList()
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
bool GuiPopUpMenuCtrlEx::getFontColor( ColorI &fontColor, S32 id, bool selected, bool mouseOver )
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

   if(id == -1)
      fontColor = mProfile->mFontColorHL;
   else
   // Default color scheme...
   fontColor = selected ? mProfile->mFontColorSL : mouseOver ? mProfile->mFontColorHL : mProfile->mFontColorNA; // DAW: Modified the final color choice from mProfile->mFontColor to mProfile->mFontColorNA

   return( true );
}

//------------------------------------------------------------------------------
// DAW: Added
bool GuiPopUpMenuCtrlEx::getColoredBox( ColorI &fontColor, S32 id )
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
void GuiPopUpMenuCtrlEx::onMouseDown(const GuiEvent &event)
{
   if(!mActive)
       return;

   onAction();
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::onMouseUp(const GuiEvent &event)
{
}

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrlEx::onMouseEnter(const GuiEvent &event)
{
   mMouseOver = true;
}

//------------------------------------------------------------------------------
// DAW: Added
void GuiPopUpMenuCtrlEx::onMouseLeave(const GuiEvent &)
{
   mMouseOver = false;
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::setupAutoScroll(const GuiEvent &event)
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
void GuiPopUpMenuCtrlEx::autoScroll()
{
   mScrollCount += mIncValue;

   while(mScrollCount > 1)
   {
      mSc->scrollByRegion(mScrollDir);
      mScrollCount -= 1;
   }
   mTl->onMouseMove(mEventSave);
}

//------------------------------------------------------------------------------
void GuiPopUpMenuCtrlEx::replaceText(S32 boolVal)
{
   mReplaceText = boolVal;
}
