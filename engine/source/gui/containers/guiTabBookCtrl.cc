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
#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "sim/simBase.h"
#include "gui/guiCanvas.h"
#include "gui/containers/guiTabBookCtrl.h"
#include "platform/event.h"
#include "io/fileStream.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/editor/guiEditCtrl.h"
#include "gui/guiDefaultControlRender.h"

#include "guiTabBookCtrl_ScriptBinding.h"

// So we can set tab alignment via gui editor
static EnumTable::Enums tabAlignEnums[] =
{
   { GuiTabBookCtrl::AlignTop,   "Top"    },
   { GuiTabBookCtrl::AlignLeft,  "Left"   },
   { GuiTabBookCtrl::AlignBottom,"Bottom" },
   { GuiTabBookCtrl::AlignRight,	"Right"  }
};
static EnumTable gTabAlignEnums(4,&tabAlignEnums[0]);


IMPLEMENT_CONOBJECT(GuiTabBookCtrl);

GuiTabBookCtrl::GuiTabBookCtrl()
{
   VECTOR_SET_ASSOCIATION(mPages);
   mFontHeight = 0;
   mTabPosition = GuiTabBookCtrl::AlignTop;
   mLastTabPosition = mTabPosition;
   mActivePage = NULL;
   mHoverTab = NULL;
   mHasTexture = false;
   mBitmapBounds = NULL;
   mBounds.extent.set( 400, 300 );
   mPageRect = RectI(0,0,0,0);
   mTabRect = RectI(0,0,0,0);

   mPages.reserve(12);
   mMinTabWidth = 64;
   mTabWidth = 64;
   mIsContainer = true;

   mTabProfile = NULL;

   setField("TabProfile", "GuiTabProfile");
}

void GuiTabBookCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("TabPosition",		TypeEnum,		Offset(mTabPosition,GuiTabBookCtrl), 1, &gTabAlignEnums );
   addField("MinTabWidth", TypeS32,    Offset(mMinTabWidth,GuiTabBookCtrl));
   addField("TabProfile", TypeGuiProfile, Offset(mTabProfile, GuiTabBookCtrl));
}

// Empty for now, will implement for handling design time context menu for manipulating pages
ConsoleMethod( GuiTabBookCtrl, addPage, void, 2, 2, "() Empty")
{
   object->addNewPage();
}

//ConsoleMethod( GuiTabBookCtrl, removePage, void, 2, 2, "()")
//{
//}


bool GuiTabBookCtrl::onAdd()
{
   Parent::onAdd();

   return true;
}


void GuiTabBookCtrl::onRemove()
{
   Parent::onRemove();
}

void GuiTabBookCtrl::onChildRemoved( GuiControl* child )
{
   for (S32 i = 0; i < mPages.size(); i++ )
   {
      GuiTabPageCtrl* tab = mPages[i].Page;
      if( tab == child )
      {
         if( tab == mActivePage )
            mActivePage = NULL;
         mPages.erase( i );
         break;
      }
   }

   if( mPages.empty() )
      mActivePage = NULL;
   else if (mActivePage == NULL )
      mActivePage = static_cast<GuiTabPageCtrl*>(mPages[0].Page);

}

void GuiTabBookCtrl::onChildAdded( GuiControl *child )
{
   GuiTabPageCtrl *page = dynamic_cast<GuiTabPageCtrl*>(child);
   if( !page )
   {
      Con::warnf("GuiTabBookCtrl::onChildAdded - attempting to add NON GuiTabPageCtrl as child page");
      SimObject *simObj = reinterpret_cast<SimObject*>(child);
      removeObject( simObj );
      if( mActivePage )
      {
         mActivePage->addObject( simObj );
      }
      else
      {
         Con::warnf("GuiTabBookCtrl::onChildAdded - unable to find active page to reassign ownership of new child control to, placing on parent");
         GuiControl *rent = getParent();
         if( rent )
            rent->addObject( simObj );
      }
      return;
   }


   TabHeaderInfo newPage;

   newPage.Page      = page;
   newPage.TabRow    = -1;
   newPage.TabColumn = -1;

   mPages.push_back( newPage );

   // Calculate Page Information
   calculatePageTabs();

   child->resize( Point2I(0, 0), mPageRect.extent );
}


bool GuiTabBookCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   mHasTexture = mProfile->constructBitmapArray();
   if( mHasTexture )
      mBitmapBounds = mProfile->mBitmapArrayRects.address();

   //increment the tab profile
   mTabProfile->incRefCount();

   return true;
}

void GuiTabBookCtrl::onSleep()
{
   Parent::onSleep();

   //decrement the tab profile referrence
   if (mTabProfile != NULL)
       mTabProfile->decRefCount();
}

void GuiTabBookCtrl::setControlTabProfile(GuiControlProfile* prof)
{
    AssertFatal(prof, "GuiTabBookCtrl::setControlTabProfile: invalid tab profile");
    if (prof == mTabProfile)
        return;
    if (mAwake)
        mTabProfile->decRefCount();
    mTabProfile = prof;
    if (mAwake)
        mTabProfile->incRefCount();

	calculatePageTabs();
}

void GuiTabBookCtrl::addNewPage()
{
   char textbuf[1024];

   GuiTabPageCtrl * page = new GuiTabPageCtrl();

   page->setField("profile", "GuiTabPageProfile");

   dSprintf(textbuf, sizeof(textbuf), "TabBookPage%d_%d", getId(), page->getId());
   page->registerObject(textbuf);

   this->addObject( page );
}

void GuiTabBookCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize( newPosition, newExtent );

   calculatePageTabs();

   // Resize Children
   SimSet::iterator i;
   for(i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      ctrl->resize( Point2I(0, 0), mPageRect.extent );
   }
}

void GuiTabBookCtrl::childResized(GuiControl *child)
{
   child->resize( Point2I(0,0), mPageRect.extent );
}

Point2I GuiTabBookCtrl::getTabLocalCoord(const Point2I &src)
{
	//Get the border profiles
	GuiBorderProfile *leftProfile = mProfile->getLeftBorder();
	GuiBorderProfile *topProfile = mProfile->getTopBorder();

	S32 leftSize = (leftProfile) ? leftProfile->getMargin(NormalState) + leftProfile->getBorder(NormalState) + leftProfile->getPadding(NormalState) : 0;
	S32 topSize = (topProfile) ? topProfile->getMargin(NormalState) + topProfile->getBorder(NormalState) + topProfile->getPadding(NormalState) : 0;

	Point2I ret = Point2I(src.x - leftSize, src.y - topSize);
	ret.x -= mTabRect.point.x;
	ret.y -= mTabRect.point.y;

	return ret;
}

void GuiTabBookCtrl::onTouchDown(const GuiEvent &event)
{
    Point2I localMouse = globalToLocalCoord( event.mousePoint );
    if( mTabRect.pointInRect( localMouse ) )
    {
		Point2I tabLocalMouse = getTabLocalCoord(localMouse);
        GuiTabPageCtrl *tab = findHitTab(tabLocalMouse);
        if( tab != NULL && tab->isActive() )
            selectPage( tab );
    }
}

void GuiTabBookCtrl::onTouchMove(const GuiEvent &event)
{
   Point2I localMouse = globalToLocalCoord( event.mousePoint );
   if( mTabRect.pointInRect( localMouse ) )
   {
	   Point2I tabLocalMouse = getTabLocalCoord(localMouse);
      GuiTabPageCtrl *tab = findHitTab(tabLocalMouse);
      if( tab != NULL && mHoverTab != tab )
         mHoverTab = tab;
      else if ( !tab )
         mHoverTab = NULL;
   }
   else
   {
	   mHoverTab = NULL;
   }
   Parent::onTouchMove( event );
}

void GuiTabBookCtrl::onTouchLeave( const GuiEvent &event )
{
   mHoverTab = NULL;
}

bool GuiTabBookCtrl::onMouseDownEditor(const GuiEvent &event, Point2I offset)
{
   bool handled = false;
   Point2I localMouse = globalToLocalCoord( event.mousePoint );

   if( mTabRect.pointInRect( localMouse ) )
   {
      GuiTabPageCtrl *tab = findHitTab( localMouse );
      if( tab != NULL )
      {
         selectPage( tab );
         handled = true;
      }
   }

   // This shouldn't be called if it's not design time, but check just incase
   if ( GuiControl::smDesignTime )
   {
      // If we clicked in the editor and our addset is the tab book
      // ctrl, select the child ctrl so we can edit it's properties
      GuiEditCtrl* edit = GuiControl::smEditorHandle;
      if( edit  && ( edit->getAddSet() == this ) && mActivePage != NULL )
         edit->select( mActivePage );
   }

   // Return whether we handled this or not.
   return handled;

}

void GuiTabBookCtrl::onPreRender()
{
   // sometimes we need to resize because of a changed persistent field
   // that's what this does
   solveDirty();
}

void GuiTabBookCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   Point2I totalOffset = offset + mTabRect.point;
	RectI ctrlRect = applyMargins(totalOffset, mTabRect.extent, NormalState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, NormalState);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);
	if (contentRect.isValidRect())
	{
		renderTabs(contentRect.point);
	}

	if(mPageRect.isValidRect())
	{
		// Render Children
		renderChildControls(offset, RectI(offset + mPageRect.point, mPageRect.extent), updateRect);
	}
}

void GuiTabBookCtrl::renderTabs( const Point2I &offset )
{
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty())
      return;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      RectI tabBounds = mPages[i].TabRect;
      tabBounds.point += offset;
      GuiTabPageCtrl *tab = mPages[i].Page;
      if( tab != NULL )
         renderTab( tabBounds, tab );
   }
}

void GuiTabBookCtrl::renderTab( RectI tabRect, GuiTabPageCtrl *tab )
{
   StringTableEntry text = tab->getText();

   GuiControlState currentState = GuiControlState::NormalState;
   if (mActivePage == tab)
   {
	   currentState = SelectedState;
   }
   else if (mHoverTab == tab)
   {
	   currentState = HighlightState;
   }

   RectI ctrlRect = applyMargins(tabRect.point, tabRect.extent, currentState, mTabProfile);
   if (!ctrlRect.isValidRect())
   {
	   return;
   }

   renderUniversalRect(ctrlRect, mTabProfile, currentState);

   //Render Text
   dglSetBitmapModulation(mTabProfile->getFontColor(currentState));
   RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mTabProfile);
   RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mTabProfile);

   TextRotationOptions rot = tRotateNone;
   if (mTabPosition == AlignLeft)
   {
		rot = tRotateLeft;
   }
   else if(mTabPosition == AlignRight)
   {
		rot = tRotateRight;
   }

   renderText(contentRect.point, contentRect.extent, text, mTabProfile, rot);

   /*
   // Is this a skinned control?
   if( mHasTexture && mProfile->mBitmapArrayRects.size() >= 9 )
   {
      S32 indexMultiplier = 1;
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         
         if ( mActivePage == tab )
            indexMultiplier += TabSelected;
         else if( mHoverTab == tab )
            indexMultiplier += TabHover;
         else
            indexMultiplier += TabNormal;
         
         //dglDrawBitmapStretchSR(mProfile->mTextureHandle,tabRect,stretchRect, ( mTabPosition == AlignBottom ) ? GFlip_Y : 0 );
         break;
      case AlignLeft:
      case AlignRight:
         if ( mActivePage == tab )
            indexMultiplier += TabSelectedVertical;
         else if( mHoverTab == tab )
            indexMultiplier += TabHoverVertical;
         else
            indexMultiplier += TabNormalVertical;

         //dglDrawBitmapStretchSR(mProfile->mTextureHandle,tabRect,stretchRect, ( mTabPosition == AlignRight ) ? GFlip_X : 0 );
         break;
      } 

      renderFixedBitmapBordersFilled( tabRect, indexMultiplier, mProfile );
   }
   else
   {
      // If this isn't a skinned control or the bitmap is simply missing, handle it WELL
      if ( mActivePage == tab )
         dglDrawRectFill(tabRect, mProfile->mFillColor);
      else if( mHoverTab == tab )
         dglDrawRectFill(tabRect, mProfile->mFillColorHL);
      else
         dglDrawRectFill(tabRect, mProfile->mFillColorNA);

   }


   dglSetBitmapModulation(mProfile->mFontColor);

   switch( mTabPosition )
   {
   case AlignTop:
   case AlignBottom:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, 0);
   break;
   case AlignLeft:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, -90 );
      break;
   case AlignRight:
      renderJustifiedTextRot( tabRect.point, tabRect.extent, text, -90 );
      break;
   }
   */
}

// This is nothing but a clever hack to allow the tab page children
// to cast this to a GuiControl* so that the file doesn't need to have circular
// includes.  generic method overriding for the win!
void GuiTabBookCtrl::setUpdate()
{
   Parent::setUpdate();

   setUpdateRegion(Point2I(0,0), mBounds.extent);

   calculatePageTabs();
}

void GuiTabBookCtrl::solveDirty()
{
   bool dirty = false;
   if( mTabPosition != mLastTabPosition )
   {
      mLastTabPosition = mTabPosition;
      dirty = true;
   }
   else if( mTabProfile != NULL && mTabProfile->mFont != NULL && mTabProfile->mFont->getHeight() != mFontHeight )
   {
      dirty = true;
   }
   else if(mPages.size() > 0 && mTabProfile != NULL && mTabProfile->mFont != NULL)
   {
	   S32 tabWidth = calculatePageTabWidth(mPages[0].Page);
	   tabWidth = getMax(tabWidth, mMinTabWidth);
	   if(mTabWidth != tabWidth)
	   {
		  dirty = true;
	   }
   }

   if( dirty )
   {
      resize( mBounds.point, mBounds.extent );
   }

}

S32 GuiTabBookCtrl::calculatePageTabWidth( GuiTabPageCtrl *page )
{
   if( !page )
      return mTabWidth;

   StringTableEntry text = page->getText();

   if( !text || dStrlen(text) == 0 || !mTabProfile || !mTabProfile->mFont || mTabProfile->mFont == '\0' )
      return mTabWidth;

	S32 textLength = mTabProfile->mFont->getStrNWidth(text, dStrlen(text));

   Point2I innerExtent = Point2I(textLength, textLength);
	Point2I outerExtent = getOuterExtent(innerExtent, NormalState, mTabProfile);

	if (mTabPosition == AlignTop || mTabPosition == AlignBottom)
	{
		return outerExtent.x;
	}
	else
	{
		return outerExtent.y;
	}
}

void GuiTabBookCtrl::calculatePageTabs()
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty())
      return;

   S32 currRow    = 0;
   S32 currColumn = 0;
   S32 currX      = 0;
   S32 currY      = 0;
   S32 tabHeight  = 0;
   RectI innerRect = getInnerRect(mBounds.point, mBounds.extent, NormalState, mProfile);
   Point2I innerExtent = Point2I(mTabProfile->mFont->getHeight(), mTabProfile->mFont->getHeight());
   Point2I fontBasedBounds = getOuterExtent(innerExtent, NormalState, mTabProfile);
   mFontHeight = mTabProfile->mFont->getHeight();

   if (mTabPosition == AlignTop || mTabPosition == AlignBottom)
   {
	   tabHeight = fontBasedBounds.y;
   }
   else
   {
	   tabHeight = fontBasedBounds.x;
   }

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      // Fetch Tab Width
      S32 tabWidth = calculatePageTabWidth( mPages[i].Page );
      tabWidth = getMax( tabWidth, mMinTabWidth );

	  if (i == 0)
	  {
		  mTabWidth = tabWidth;
	  }

      TabHeaderInfo &info = mPages[i];
      switch( mTabPosition )
      {
      case AlignTop:
      case AlignBottom:
         // If we're going to go outside our bounds
         // with this tab move it down a row
         if( currX + tabWidth > innerRect.extent.x )
         {
            // Calculate and Advance State.
            balanceRow( currRow, currX );
            info.TabRow = ++currRow;
            // Reset Necessaries
            info.TabColumn = currColumn = currX = 0;
         }
         else
         {
            info.TabRow = currRow;
            info.TabColumn = currColumn++;
         }

         // Calculate Tabs Bounding Rect
         info.TabRect.point.x  = currX;
		 info.TabRect.point.y = (info.TabRow * tabHeight);
         info.TabRect.extent.x = tabWidth;
         info.TabRect.extent.y = tabHeight;

         currX += tabWidth;
         break;
      case AlignLeft:
      case AlignRight:
         // If we're going to go outside our bounds
         // with this tab move it down a row
         if( currY + tabWidth > innerRect.extent.y )
         {
            // Balance Tab Column.
            balanceColumn( currColumn, currY );

            // Calculate and Advance State.
            info.TabColumn = ++currColumn;
            info.TabRow = currRow = currY = 0;
         }
         else
         {
            info.TabColumn = currColumn;
            info.TabRow = currRow++;
         }

         // Calculate Tabs Bounding Rect
		 info.TabRect.point.x = (info.TabColumn * tabHeight);
         info.TabRect.point.y  = currY;
		 info.TabRect.extent.x = tabHeight;
         info.TabRect.extent.y = tabWidth;

         currY += tabWidth;
         break;
      };
   }

   currRow++;
   currColumn++;

   Point2I colExtent = Point2I(currColumn * tabHeight, currRow * tabHeight);
   Point2I outerExtent = getOuterExtent(colExtent, NormalState, mProfile);

   // Calculate 
   switch( mTabPosition )
   {
   case AlignTop:
   	  mTabRect.point.x = 0;
	  mTabRect.point.y = 0;
      mTabRect.extent.x = mBounds.extent.x;
      mTabRect.extent.y = outerExtent.y;

      mPageRect.point.x = 0;
      mPageRect.point.y = mTabRect.extent.y;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = mBounds.extent.y - mTabRect.extent.y;

      break;
   case AlignBottom:
      mTabRect.point.x = 0;
	  mTabRect.point.y = mBounds.extent.y - mTabRect.extent.y;
      mTabRect.extent.x = mBounds.extent.x;
      mTabRect.extent.y = outerExtent.y;

      mPageRect.point.x = 0;
      mPageRect.point.y = 0;
      mPageRect.extent.x = mTabRect.extent.x;
      mPageRect.extent.y = mBounds.extent.y - mTabRect.extent.y;

      break;
   case AlignLeft:
	  mTabRect.point.x = 0;
      mTabRect.point.y = 0;
	  mTabRect.extent.x = outerExtent.x;
      mTabRect.extent.y = mBounds.extent.y;

	  mPageRect.point.x = mTabRect.extent.x;
      mPageRect.point.y = 0;
	  mPageRect.extent.x = mBounds.extent.x - mTabRect.extent.x;
      mPageRect.extent.y = mBounds.extent.y;

      break;
   case AlignRight:
	  mTabRect.point.x = mBounds.extent.x - mTabRect.extent.x;
      mTabRect.point.y = 0;
	  mTabRect.extent.x = outerExtent.x;
      mTabRect.extent.y = mBounds.extent.y;

	  mPageRect.point.x = 0;
      mPageRect.point.y = 0;
	  mPageRect.extent.x = mBounds.extent.x - mTabRect.extent.x;
      mPageRect.extent.y = mTabRect.extent.y;

      break;
   };


}

void GuiTabBookCtrl::balanceColumn( S32 column , S32 totalTabWidth )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty())
      return;

   Vector<TabHeaderInfo*> rowTemp;
   rowTemp.clear();

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      TabHeaderInfo &info = mPages[i];

      if(info.TabColumn == column )
         rowTemp.push_back( &mPages[i] );
   }

   if( rowTemp.empty() )
      return;

   // Balance the tabs across the remaining space
   RectI innerRect = getInnerRect(mBounds.point, mBounds.extent, NormalState, mProfile);
   S32 spaceToDivide = innerRect.extent.y - totalTabWidth;
   S32 pointDelta    = 0;
   for( S32 i = 0; i < rowTemp.size(); i++ )
   {
      TabHeaderInfo &info = *rowTemp[i];
      S32 extraSpace = (S32)( spaceToDivide / rowTemp.size() );
      info.TabRect.extent.y += extraSpace;
      info.TabRect.point.y += pointDelta;
      pointDelta += extraSpace;
   }

}
void GuiTabBookCtrl::balanceRow( S32 row, S32 totalTabWidth )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty())
      return;

   Vector<TabHeaderInfo*> rowTemp;
   rowTemp.clear();

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      TabHeaderInfo &info = mPages[i];

      if(info.TabRow == row )
         rowTemp.push_back( &mPages[i] );
   }

   if( rowTemp.empty() )
      return;

   // Balance the tabs across the remaining space
   RectI innerRect = getInnerRect(mBounds.point, mBounds.extent, NormalState, mProfile);
   S32 spaceToDivide = innerRect.extent.x - totalTabWidth;
   S32 pointDelta    = 0;
   for( S32 i = 0; i < rowTemp.size(); i++ )
   {
      TabHeaderInfo &info = *rowTemp[i];
      S32 extraSpace = (S32)spaceToDivide / ( rowTemp.size() );
      info.TabRect.extent.x += extraSpace;
      info.TabRect.point.x += pointDelta;
      pointDelta += extraSpace;
   }
}


GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( const GuiEvent &event )
{
   return findHitTab( event.mousePoint );
}

GuiTabPageCtrl *GuiTabBookCtrl::findHitTab( Point2I hitPoint )
{
   // Short Circuit.
   //
   // If the tab size is zero, don't render tabs,
   //  and assume it's a tab-less tab-book - JDD
   if( mPages.empty())
      return NULL;

   for( S32 i = 0; i < mPages.size(); i++ )
   {
      if( mPages[i].TabRect.pointInRect( hitPoint ) )
         return mPages[i].Page;
   }
   return NULL;
}

U32 GuiTabBookCtrl::getSelectedPage()
{
	U32 index = 0;

	for (U32 i = 0; i < mPages.size(); i++)
	{
		if (mActivePage == mPages[i].Page)
		{
			index = i;
			break;
		}
	}

	return index;
}

void GuiTabBookCtrl::selectPage( S32 index )
{
   if( index < 0 || index >= mPages.size())
      return;

   // Select the page
   selectPage( mPages[ index ].Page );
}


void GuiTabBookCtrl::selectPage( GuiTabPageCtrl *page )
{
   Vector<TabHeaderInfo>::iterator i = mPages.begin();
   for( ; i != mPages.end() ; i++ )
   {
      GuiTabPageCtrl *tab = reinterpret_cast<GuiTabPageCtrl*>((*i).Page);
      if( page == tab )
      {
         mActivePage = tab;
         tab->setVisible( true );

         // Notify User
         char *retBuffer = Con::getReturnBuffer( 512 );
         dStrcpy( retBuffer, tab->getText() );
         Con::executef( this, 2, "onTabSelected",  retBuffer );

      }
      else
         tab->setVisible( false );
   }
}

void GuiTabBookCtrl::selectPage( const char* pageName )
{
   Vector<TabHeaderInfo>::iterator i = mPages.begin();
   for( ; i != mPages.end() ; i++ )
   {
      GuiTabPageCtrl *tab = reinterpret_cast<GuiTabPageCtrl*>((*i).Page);
      if( dStricmp( pageName, tab->getText() ) == 0 )
      {
         mActivePage = tab;
         tab->setVisible( true );

         // Notify User
         char *retBuffer = Con::getReturnBuffer( 512 );
         dStrcpy( retBuffer, tab->getText() );
         Con::executef( this, 2, "onTabSelected",  retBuffer );

      }
      else
         tab->setVisible( false );
   }
}


bool GuiTabBookCtrl::onKeyDown(const GuiEvent &event)
{
   // Tab      = Next Page
   // Ctrl-Tab = Previous Page
   if( 0 && event.keyCode == KEY_TAB )
   {
      if( event.modifier & SI_CTRL )
         selectPrevPage();
      else 
         selectNextPage();

      return true;
   }

   return Parent::onKeyDown( event );
}

void GuiTabBookCtrl::selectNextPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == ( mPages.size() - 1 ) )
            selectPage( 0 );
         else if ( nI + 1 <= ( mPages.size() - 1 ) ) 
            selectPage( nI + 1 );
         else
            selectPage( 0 );

         // Notify User
         if( isMethod( "onTabSelected" ) )
         {
            char *retBuffer = Con::getReturnBuffer( 512 );
            dStrcpy( retBuffer, tab->getText() );
            Con::executef( this, 2, "onTabSelected",  retBuffer );
         }

         return;
      }
   }
}

void GuiTabBookCtrl::selectPrevPage()
{
   if( mPages.empty() )
      return;

   if( mActivePage == NULL )
      mActivePage = mPages[0].Page;

   S32 nI = 0;
   for( ; nI < mPages.size(); nI++ )
   {
      GuiTabPageCtrl *tab = mPages[ nI ].Page;
      if( tab == mActivePage )
      {
         if( nI == 0 )
            selectPage( mPages.size() - 1 );
         else
            selectPage( nI - 1 );

         // Notify User
         if( isMethod( "onTabSelected" ) )
         {
            char *retBuffer = Con::getReturnBuffer( 512 );
            dStrcpy( retBuffer, tab->getText() );
            Con::executef( this, 2, "onTabSelected",  retBuffer );
         }

         return;
      }
   }

}
