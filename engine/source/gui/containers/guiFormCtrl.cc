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

#include "gui/editor/guiMenuBar.h"
#include "gui/containers/guiFormCtrl.h"
#include "gui/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiFormCtrl);

ConsoleMethod(GuiFormCtrl, getMenuID, S32, 2, 2, "Returns the ID of the Form Menu")
{
   return object->getMenuBarID();
}

ConsoleMethod(GuiFormCtrl, setCaption, void, 3, 3, "setCaption(caption) - Sets the title of the Form Menu")
{
   object->setCaption(argv[2]);
}

GuiFormCtrl::GuiFormCtrl()
{
   mMinExtent.set(10, 10);
   mActive        = true;
   mMouseOver     = false;
   mDepressed     = false;
   mCanMove       = false;
   mCaption = StringTable->insert("[none]");
   mUseSmallCaption = false;
   mSmallCaption = StringTable->EmptyString;

   mContentLibrary = StringTable->EmptyString;
   mContent = StringTable->EmptyString;

   mCanSaveFieldDictionary = true;

   mHasMenu = false;
   
   mIsContainer = true;

   // The attached menu bar
   mMenuBar = NULL;
}

GuiFormCtrl::~GuiFormCtrl()
{
   if(mMenuBar)
   {
      removeObject(mMenuBar);
      //mMenuBar->deleteObject();
   }
}

void GuiFormCtrl::initPersistFields()
{

   addField("Caption",       TypeString,  Offset(mCaption,     GuiFormCtrl));
   addField("ContentLibrary",TypeString,  Offset(mContentLibrary, GuiFormCtrl));
   addField("Content",       TypeString,  Offset(mContent, GuiFormCtrl));
   addField("Movable",       TypeBool,  Offset(mCanMove, GuiFormCtrl));
   addField("HasMenu",       TypeBool,  Offset(mHasMenu, GuiFormCtrl));

   Parent::initPersistFields();


}

void GuiFormCtrl::setCaption(const char* caption)
{
   if(caption)
   {
      mCaption = StringTable->insert(caption, true);
   }
}

bool GuiFormCtrl::onWake()
{
   if ( !Parent::onWake() )
      return false;

   mFont = mProfile->mFont;
   AssertFatal(mFont, "GuiFormCtrl::onWake: invalid font in profile" );

   mProfile->constructBitmapArray();

   if(mProfile->mBitmapArrayRects.size())
   {
      mThumbSize.set(   mProfile->mBitmapArrayRects[0].extent.x, mProfile->mBitmapArrayRects[0].extent.y );
      mThumbSize.setMax( mProfile->mBitmapArrayRects[1].extent );

      if(mFont->getHeight() > (U32)mThumbSize.y)
         mThumbSize.y = mFont->getHeight();
   }
   else
   {
      mThumbSize.set(20, 20);
   }

   return true;
}


void GuiFormCtrl::addObject(SimObject *newObj )
{
   if( ( mHasMenu && size() > 1) || (!mHasMenu && size() > 0 ) )
   {
      Con::warnf("GuiFormCtrl::addObject - Forms may only have one *direct* child - Placing on Parent!");
      GuiControl *parent = getParent();
      if( parent  )
         parent->addObject( newObj );

      return;
   }
   GuiControl *newCtrl = dynamic_cast<GuiControl*>( newObj );
   GuiFormCtrl*formCtrl = dynamic_cast<GuiFormCtrl*>( newObj );
   if( newCtrl && formCtrl )
      newCtrl->setCanSave( true );
   else if ( newCtrl )
      newCtrl->setCanSave( false );

   Parent::addObject( newObj );
}

void GuiFormCtrl::onSleep()
{
   Parent::onSleep();
   mFont = NULL;
}

bool GuiFormCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if( !mMenuBar && mHasMenu )
   {
      mMenuBar = new GuiMenuBar();
      AssertFatal(mMenuBar, "GuiFormCtrl::onWake: cannot create form menu" );
      if( mMenuBar )
      {
         mMenuBar->setField("profile","GuiFormMenuBarProfile");
         mMenuBar->setField("horizSizing", "right");
         mMenuBar->setField("vertSizing", "bottom");
         mMenuBar->setField("extent", "16 16");
         mMenuBar->setField("minExtent", "16 16");
         mMenuBar->setField("position", "0 0");
         mMenuBar->setField("class", "FormMenuBarClass"); // Give a generic class to the menu bar so that one set of functions may be used for all of them.

         mMenuBar->registerObject();
         mMenuBar->setProcessTicks(true); // Activate the processing of ticks to track if the mouse pointer has been hovering within the menu
         addObject(mMenuBar); // Add the menu bar to the form
      }
   }

   return true;
}

U32 GuiFormCtrl::getMenuBarID()
{
   if(mMenuBar)
   {
      return mMenuBar->getId();
   }

   return 0;
}

void GuiFormCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize(newPosition, newExtent);

   if( !mAwake || !mProfile->mBitmapArrayRects.size() )
      return;

   // Should the caption be modified because the title bar is too small?
   S32 textWidth = mProfile->mFont->getStrWidth(mCaption);
   S32 newTextArea = mBounds.extent.x - mThumbSize.x - mProfile->mBitmapArrayRects[4].extent.x;
   if(newTextArea < textWidth)
   {
      static char buf[256];

      mUseSmallCaption = true;
      mSmallCaption = StringTable->EmptyString;

      S32 strlen = dStrlen((const char*)mCaption);
      for(S32 i=strlen; i>=0; --i)
      {
         dStrcpy(buf, "");
         dStrncat(buf, (const char*)mCaption, i);
         dStrcat(buf, "...");

         textWidth = mProfile->mFont->getStrWidth(buf);

         if(textWidth < newTextArea)
         {
            mSmallCaption = StringTable->insert(buf, true);
            break;
         }
      }

   } else
   {
      mUseSmallCaption = false;
   }

   Con::executef(this, 1, "onResize");

}

void GuiFormCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // Fill in the control's child area
   RectI boundsRect(offset, mBounds.extent);
   boundsRect.point.y += mThumbSize.y;
   boundsRect.extent.y -= mThumbSize.y;

   // draw the border of the form if specified
   dglDrawRectFill(boundsRect, mProfile->mFillColor);

   //if (mProfile->mBorder)
      //renderBorder(boundsRect, mProfile);

   // If we don't have a child (other than the menu), put some text in the child area
   if(size() <= 1)
   {
      dglSetBitmapModulation(ColorI(0,0,0));
      renderText(boundsRect.point, boundsRect.extent, "[none]", mProfile);
   }

   S32 textWidth = 0;

   // Draw our little bar, too
   if(mProfile->mBitmapArrayRects.size() >= 5)
   {
      dglClearBitmapModulation();

      S32 barStart = 0;//(mHasMenu ? mThumbSize.x : 1 + mProfile->mBorderSize) + offset.x + textWidth;
      S32 barTop   = mThumbSize.y/2 + offset.y - mProfile->mBitmapArrayRects[3].extent.y /2;

      Point2I barOffset(barStart, barTop);

      // Draw the start of the bar...
      dglDrawBitmapStretchSR(mProfile->mTextureHandle,RectI(barOffset, mProfile->mBitmapArrayRects[2].extent), mProfile->mBitmapArrayRects[2] );

      // Now draw the middle...
      barOffset.x += mProfile->mBitmapArrayRects[2].extent.x;

      S32 barMiddleSize = (getExtent().x - (barOffset.x - offset.x)) - mProfile->mBitmapArrayRects[4].extent.x;

      if(barMiddleSize>0)
      {
         // We have to do this inset to prevent nasty stretching artifacts
         RectI foo = mProfile->mBitmapArrayRects[3];
         foo.inset(1,0);

         dglDrawBitmapStretchSR(
            mProfile->mTextureHandle,
            RectI(barOffset, Point2I(barMiddleSize, mProfile->mBitmapArrayRects[3].extent.y)),
            foo
            );
      }

      // And the end
      barOffset.x += barMiddleSize;

      dglDrawBitmapStretchSR( mProfile->mTextureHandle, RectI(barOffset, mProfile->mBitmapArrayRects[4].extent),
         mProfile->mBitmapArrayRects[4]);

      dglSetBitmapModulation((mMouseOver ? mProfile->mFontColorHL : mProfile->mFontColor));
      renderText(Point2I(mThumbSize.x, 0) + offset, Point2I(mBounds.extent.x - mThumbSize.x - mProfile->mBitmapArrayRects[4].extent.x, mThumbSize.y), (mUseSmallCaption ? mSmallCaption : mCaption), mProfile);

   }

   // Render the children
   renderChildControls(offset, mBounds, updateRect);
}

void GuiFormCtrl::onTouchDragged(const GuiEvent &event)
{
   GuiControl *parent = getParent();
   GuiCanvas *root = getRoot();
   if (! root) return;

   Point2I deltaMousePosition = event.mousePoint - mMouseDownPosition;

   Point2I newPosition = mBounds.point;
   Point2I newExtent = mBounds.extent;
   if (mMouseMovingWin && parent)
   {
      newPosition.x = getMax(0, getMin(parent->mBounds.extent.x - mBounds.extent.x, mOrigBounds.point.x + deltaMousePosition.x));
      newPosition.y = getMax(0, getMin(parent->mBounds.extent.y - mBounds.extent.y, mOrigBounds.point.y + deltaMousePosition.y));
      Point2I pos = parent->localToGlobalCoord(mBounds.point);
      root->addUpdateRegion(pos, mBounds.extent);
      resize(newPosition, newExtent);

   }
}


void GuiFormCtrl::onTouchMove(const GuiEvent &event)
{
   Point2I localMove = globalToLocalCoord(event.mousePoint);

   // If we're clicking in the header then resize
   mMouseOver = (localMove.y < mThumbSize.y);
   if(isMouseLocked())
      mDepressed = mMouseOver;

}

void GuiFormCtrl::onTouchEnter(const GuiEvent &event)
{
   setUpdate();
   if(isMouseLocked())
   {
      mDepressed = true;
      mMouseOver = true;
   }
   else
   {
      mMouseOver = true;
   }

}

void GuiFormCtrl::onTouchLeave(const GuiEvent &event)
{
   setUpdate();
   if(isMouseLocked())
      mDepressed = false;
   mMouseOver = false;
}

void GuiFormCtrl::onTouchDown(const GuiEvent &event)
{
   Point2I localClick = globalToLocalCoord(event.mousePoint);

   // If we're clicking in the header then resize
   if(localClick.y < mThumbSize.y)
   {
      mouseLock();
      mDepressed = true;
      mMouseMovingWin = mCanMove;

      //update
      setUpdate();
   }

   mOrigBounds = mBounds;

   mMouseDownPosition = event.mousePoint;

   ////if we clicked within the title bar
   //if (localPoint.y < mTitleHeight)
   //{
   //   //if we clicked on the close button
   //   if (mCanClose && mCloseButton.pointInRect(localPoint))
   //   {
   //      mPressClose = mCanClose;
   //   }
   //   else if (mCanMaximize && mMaximizeButton.pointInRect(localPoint))
   //   {
   //      mPressMaximize = mCanMaximize;
   //   }
   //   else if (mCanMinimize && mMinimizeButton.pointInRect(localPoint))
   //   {
   //      mPressMinimize = mCanMinimize;
   //   }

   //   //else we clicked within the title
   //   else
   //   {
   //      mMouseMovingWin = mCanMove;
   //      mMouseResizeWidth = false;
   //      mMouseResizeHeight = false;
   //   }
   //}
   //else
   //{
   //   mMouseMovingWin = false;

   //   //see if we clicked on the right edge
   //   if (mResizeWidth && (localPoint.x > mBounds.extent.x - mResizeRightWidth))
   //   {
   //      mMouseResizeWidth = true;
   //   }

   //   //see if we clicked on the bottom edge (as well)
   //   if (mResizeHeight && (localPoint.y > mBounds.extent.y - mResizeBottomHeight))
   //   {
   //      mMouseResizeHeight = true;
   //   }
   //}


   if (mMouseMovingWin )//|| mMouseResizeWidth || mMouseResizeHeight ||
      //mPressClose || mPressMaximize || mPressMinimize)
   {
      mouseLock();
   }
   else
   {

      GuiControl *ctrl = findHitControl(localClick);
      if (ctrl && ctrl != this)
         ctrl->onTouchDown(event);

   }

}

void GuiFormCtrl::onTouchUp(const GuiEvent &event)
{
   // Make sure we only get events we ought to be getting...
   if (! mActive)
      return; 

   mouseUnlock();
   setUpdate();

   //mMouseMovingWin = false;
   //mMouseResizeWidth = false;
   //mMouseResizeHeight = false;


   //Point2I localClick = globalToLocalCoord(event.mousePoint);

   // If we're clicking in the header then resize
   //if(localClick.y < mThumbSize.y && mDepressed)
   //   setCollapsed(!mCollapsed);
}

