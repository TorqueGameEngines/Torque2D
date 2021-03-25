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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/containers/guiWindowCtrl.h"
#include "gui/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiWindowCtrl);

GuiWindowCtrl::GuiWindowCtrl(void)
{
   mResizeWidth = true;
   mResizeHeight = true;
   mCanMove = true;
   mCanClose = true;
   mCanMinimize = true;
   mCanMaximize = true;
   mTitleHeight = 20;
   mResizeRightWidth = 10;
   mResizeBottomHeight = 10;
   mIsContainer = true;

   mCloseCommand = StringTable->EmptyString;

   mMinimized = false;
   mMaximized = false;
   mMouseMovingWin = false;
   mMouseResizeWidth = false;
   mMouseResizeHeight = false;
   mBounds.extent.set(100, 200);
   mMinSize.set(50, 50);
   mMinimizeIndex = -1;
   mTabIndex = -1;

   RectI closeRect(80, 2, 16, 16);
   mCloseButton = closeRect;
   closeRect.point.x -= 18;
   mMaximizeButton = closeRect;
   closeRect.point.x -= 18;
   mMinimizeButton = closeRect;

   //other defaults
   mActive = true;
   mPressClose = false;
   mPressMaximize = false;
   mPressMinimize = false;

}

void GuiWindowCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("resizeWidth",       TypeBool,         Offset(mResizeWidth, GuiWindowCtrl));
   addField("resizeHeight",      TypeBool,         Offset(mResizeHeight, GuiWindowCtrl));
   addField("canMove",           TypeBool,         Offset(mCanMove, GuiWindowCtrl));
   addField("canClose",          TypeBool,         Offset(mCanClose, GuiWindowCtrl));
   addField("canMinimize",       TypeBool,         Offset(mCanMinimize, GuiWindowCtrl));
   addField("canMaximize",       TypeBool,         Offset(mCanMaximize, GuiWindowCtrl));
   addField("minSize",           TypePoint2I,      Offset(mMinSize, GuiWindowCtrl));
   addField("closeCommand",      TypeString,       Offset(mCloseCommand, GuiWindowCtrl));
}

bool GuiWindowCtrl::isMinimized(S32 &index)
{
   index = mMinimizeIndex;
   return mMinimized && mVisible;
}

// helper fn so button positioning shares code...
void GuiWindowCtrl::PositionButtons(void)
{
   if( !mBitmapBounds || !mAwake )
      return;

   S32 buttonWidth = mBitmapBounds[BmpStates * BmpClose].extent.x;
   S32 buttonHeight = mBitmapBounds[BmpStates * BmpClose].extent.y;
   Point2I mainOff = mProfile->mTextOffset;

   int minLeft = mBounds.extent.x - buttonWidth * 3 - mainOff.x;
   int minTop = mainOff.y;
   int minOff = buttonWidth + 2;
   
   RectI minRect(minLeft, minTop, buttonHeight, buttonWidth);
   mMinimizeButton = minRect;

   mMaximizeButton = minRect;
   minRect.point.x += minOff;
   mMaximizeButton = minRect;

   mCloseButton = minRect;
   minRect.point.x += minOff;
   mCloseButton = minRect;
}

bool GuiWindowCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   //get the texture for the close, minimize, and maximize buttons
   
   bool result = mProfile->constructBitmapArray() >= NumBitmaps;
   mTextureHandle = mProfile->mTextureHandle;
   AssertFatal(result, "Failed to create the bitmap array");
   if(!result)
      return false;

   mBitmapBounds = mProfile->mBitmapArrayRects.address();
   S32 buttonHeight = mBitmapBounds[BmpStates * BmpClose].extent.y;

   mTitleHeight = buttonHeight + 4;
   mResizeRightWidth = mTitleHeight / 2;
   mResizeBottomHeight = mTitleHeight / 2;

   //set the button coords
   PositionButtons();

   //set the tab index
   mTabIndex = -1;
   GuiControl *parent = getParent();
   if (parent && mFirstResponder)
   {
      mTabIndex = 0;

      //count the number of windows preceeding this one
      iterator i;
      for (i = parent->begin(); i != parent->end(); i++)
      {
         GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
         if (ctrl)
         {
            if (ctrl == this) break;
            else if (ctrl->mFirstResponder) mTabIndex++;
         }
      }
   }

   return true;
}

void GuiWindowCtrl::onSleep()
{
   mTextureHandle = NULL;
   Parent::onSleep();
}

GuiControl* GuiWindowCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
   if (! mMinimized)
      return Parent::findHitControl(pt, initialLayer);
   else
      return this;
}

void GuiWindowCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize(newPosition, newExtent);

   //set the button coords
   PositionButtons();
}

void GuiWindowCtrl::onMouseDown(const GuiEvent &event)
{
   setUpdate();

   mOrigBounds = mBounds;

   mMouseDownPosition = event.mousePoint;
   Point2I localPoint = globalToLocalCoord(event.mousePoint);

   //select this window - move it to the front, and set the first responder
   selectWindow();

   //if we clicked within the title bar
   if (localPoint.y < mTitleHeight)
   {
      //if we clicked on the close button
      if (mCanClose && mCloseButton.pointInRect(localPoint))
      {
         mPressClose = mCanClose;
      }
      else if (mCanMaximize && mMaximizeButton.pointInRect(localPoint))
      {
         mPressMaximize = mCanMaximize;
      }
      else if (mCanMinimize && mMinimizeButton.pointInRect(localPoint))
      {
         mPressMinimize = mCanMinimize;
      }

      //else we clicked within the title
      else
      {
         mMouseMovingWin = mCanMove;
         mMouseResizeWidth = false;
         mMouseResizeHeight = false;
      }
   }
   else
   {
      mMouseMovingWin = false;

      //see if we clicked on the right edge
      if (mResizeWidth && (localPoint.x > mBounds.extent.x - mResizeRightWidth))
      {
         mMouseResizeWidth = true;
      }

      //see if we clicked on the bottom edge (as well)
      if (mResizeHeight && (localPoint.y > mBounds.extent.y - mResizeBottomHeight))
      {
         mMouseResizeHeight = true;
      }
   }


   if (mMouseMovingWin || mMouseResizeWidth || mMouseResizeHeight ||
         mPressClose || mPressMaximize || mPressMinimize)
   {
      mouseLock();
   }
   else
   {

      GuiControl *ctrl = findHitControl(localPoint);
      if (ctrl && ctrl != this)
         ctrl->onTouchDown(event);

   }
}

void GuiWindowCtrl::onMouseDragged(const GuiEvent &event)
{
   GuiControl *parent = getParent();
   GuiCanvas *root = getRoot();
   if (! root) return;

   Point2I deltaMousePosition = event.mousePoint - mMouseDownPosition;

   Point2I newPosition = mBounds.point;
   Point2I newExtent = mBounds.extent;
   bool update = false;
   if (mMouseMovingWin && parent)
   {
      newPosition.x = getMax(0, getMin(parent->mBounds.extent.x - mBounds.extent.x, mOrigBounds.point.x + deltaMousePosition.x));
      newPosition.y = getMax(0, getMin(parent->mBounds.extent.y - mBounds.extent.y, mOrigBounds.point.y + deltaMousePosition.y));
      update = true;
   }
   else if(mPressClose || mPressMaximize || mPressMinimize)
   {
      setUpdate();
   }
   else
   {
      Point2I minExtent = getMinExtent();
      if (mMouseResizeWidth && parent)
      {
         newExtent.x = getMax(0, getMax(minExtent.x, getMin(parent->getWidth(), mOrigBounds.extent.x + deltaMousePosition.x)));
         update = true;
      }

      if (mMouseResizeHeight && parent)
      {
         newExtent.y = getMax(0, getMax(minExtent.y, getMin(parent->getHeight(), mOrigBounds.extent.y + deltaMousePosition.y)));
         update = true;
      }
   }

   if (update)
   {
      Point2I pos = parent->localToGlobalCoord(getPosition());
      root->addUpdateRegion(pos, getExtent());
      resize(newPosition, newExtent);
   }
}

void GuiWindowCtrl::onMouseUp(const GuiEvent &event)
{
   bool closing = mPressClose;
   bool maximizing = mPressMaximize;
   bool minimizing = mPressMinimize;
   mPressClose = false;
   mPressMaximize = false;
   mPressMinimize = false;

   mouseUnlock();

   mMouseMovingWin = false;
   mMouseResizeWidth = false;
   mMouseResizeHeight = false;

   GuiControl *parent = getParent();
   if (! parent)
      return;

   //see if we take an action
   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   if (closing && mCloseButton.pointInRect(localPoint))
   {
      Con::evaluate(mCloseCommand);
   }
   else if (maximizing && mMaximizeButton.pointInRect(localPoint))
   {
      if (mMaximized)
      {
         //resize to the previous position and extent, bounded by the parent
         resize(Point2I(getMax(0, getMin(parent->mBounds.extent.x - mStandardBounds.extent.x, mStandardBounds.point.x)),
                        getMax(0, getMin(parent->mBounds.extent.y - mStandardBounds.extent.y, mStandardBounds.point.y))),
                        mStandardBounds.extent);
         //set the flag
         mMaximized = false;
      }
      else
      {
         //only save the position if we're not minimized
         if (! mMinimized)
         {
            mStandardBounds = mBounds;
         }
         else
         {
            mMinimized = false;
         }

         //resize to fit the parent
         resize(Point2I(0, 0), parent->mBounds.extent);

         //set the flag
         mMaximized = true;
      }
   }
   else if (minimizing && mMinimizeButton.pointInRect(localPoint))
   {
      if (mMinimized)
      {
         //resize to the previous position and extent, bounded by the parent
         resize(Point2I(getMax(0, getMin(parent->mBounds.extent.x - mStandardBounds.extent.x, mStandardBounds.point.x)),
                        getMax(0, getMin(parent->mBounds.extent.y - mStandardBounds.extent.y, mStandardBounds.point.y))),
                        mStandardBounds.extent);
         //set the flag
         mMinimized = false;
      }
      else
      {
         if (parent->mBounds.extent.x < 100 || parent->mBounds.extent.y < mTitleHeight + 3)
            return;

         //only save the position if we're not maximized
         if (! mMaximized)
         {
            mStandardBounds = mBounds;
         }
         else
         {
            mMaximized = false;
         }

         //first find the lowest unused minimized index up to 32 minimized windows
         U32 indexMask = 0;
         iterator i;
         S32 count = 0;
         for (i = parent->begin(); i != parent->end() && count < 32; i++)
         {
            count++;
            S32 index;
            GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
            if (ctrl && ctrl->isMinimized(index))
            {
               indexMask |= (1 << index);
            }
         }

         //now find the first unused bit
         for (count = 0; count < 32; count++)
         {
            if (! (indexMask & (1 << count))) break;
         }

         //if we have more than 32 minimized windows, use the first position
         count = getMax(0, count);

         //this algorithm assumes all window have the same title height, and will minimize to 98 pix
         Point2I newExtent(98, mTitleHeight);

         //first, how many can fit across
         S32 numAcross = getMax(1, (parent->mBounds.extent.x / newExtent.x + 2));

         //find the new "mini position"
         Point2I newPosition;
         newPosition.x = (count % numAcross) * (newExtent.x + 2) + 2;
         newPosition.y = parent->mBounds.extent.y - (((count / numAcross) + 1) * (newExtent.y + 2)) - 2;

         //find the minimized position and extent
         resize(newPosition, newExtent);

         //set the index so other windows will not try to minimize to the same location
         mMinimizeIndex = count;

         //set the flag
         mMinimized = true;
      }
   }

}

GuiControl *GuiWindowCtrl::findNextTabable(GuiControl *curResponder, bool firstCall)
{
   //set the global if this is the first call (directly from the canvas)
   if (firstCall)
   {
      GuiControl::smCurResponder = NULL;
   }

   //if the window does not already contain the first responder, return false
   //ie.  Can't tab into or out of a window
   if (! ControlIsChild(curResponder))
   {
      return NULL;
   }

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findNextTabable(curResponder, false);
      if (tabCtrl) break;
   }

   //to ensure the tab cycles within the current window...
   if (! tabCtrl)
   {
      tabCtrl = findFirstTabable();
   }

   mFirstResponder = tabCtrl;
   return tabCtrl;
}

GuiControl *GuiWindowCtrl::findPrevTabable(GuiControl *curResponder, bool firstCall)
{
   if (firstCall)
   {
      GuiControl::smPrevResponder = NULL;
   }

   //if the window does not already contain the first responder, return false
   //ie.  Can't tab into or out of a window
   if (! ControlIsChild(curResponder))
   {
      return NULL;
   }

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findPrevTabable(curResponder, false);
      if (tabCtrl) break;
   }

   //to ensure the tab cycles within the current window...
   if (! tabCtrl)
   {
      tabCtrl = findLastTabable();
   }

   mFirstResponder = tabCtrl;
   return tabCtrl;
}

bool GuiWindowCtrl::onKeyDown(const GuiEvent &event)
{
   //if this control is a dead end, kill the event
   if ((! mVisible) || (! mActive) || (! mAwake)) return true;

   if ((event.keyCode == KEY_TAB) && (event.modifier & SI_CTRL))
   {
      //find the next sibling window, and select it
      GuiControl *parent = getParent();
      if (parent)
      {
         GuiWindowCtrl *firstWindow = NULL;
         iterator i;
         for (i = parent->begin(); i != parent->end(); i++)
         {
            GuiWindowCtrl *ctrl = dynamic_cast<GuiWindowCtrl *>(*i);
            if (ctrl && ctrl->getTabIndex() == mTabIndex + 1)
            {
               ctrl->selectWindow();
               return true;
            }
            else if (ctrl && ctrl->getTabIndex() == 0)
            {
               firstWindow = ctrl;
            }
         }
         //recycle from the beginning
         if (firstWindow != this)
         {
            firstWindow->selectWindow();
            return true;
         }
      }
   }

   return Parent::onKeyDown(event);
}

void GuiWindowCtrl::selectWindow(void)
{
   //first make sure this window is the front most of its siblings
   GuiControl *parent = getParent();
   if (parent)
   {
      parent->pushObjectToBack(this);
   }

   //also set the first responder to be the one within this window
   setFirstResponder(mFirstResponder);
}

void GuiWindowCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if( !mProfile || mProfile->mFont.isNull() || mProfile->mBitmapArrayRects.size() < NumBitmaps )
      return Parent::onRender( offset, updateRect );

   //draw the outline
   RectI winRect;
   winRect.point = offset;
   winRect.extent = mBounds.extent;
   GuiCanvas *root = getRoot();
   GuiControl *firstResponder = root ? root->getFirstResponder() : NULL;

   bool isKey = (!firstResponder || ControlIsChild(firstResponder));

   U32 topBase = isKey ? BorderTopLeftKey : BorderTopLeftNoKey;
   winRect.point.x += mBitmapBounds[BorderLeft].extent.x;
   winRect.point.y += mBitmapBounds[topBase + 2].extent.y;

   winRect.extent.x -= mBitmapBounds[BorderLeft].extent.x + mBitmapBounds[BorderRight].extent.x;
   winRect.extent.y -= mBitmapBounds[topBase + 2].extent.y + mBitmapBounds[BorderBottom].extent.y;

   dglDrawRectFill(winRect, mProfile->mFillColor);

   dglClearBitmapModulation();
   dglDrawBitmapSR(mTextureHandle, offset, mBitmapBounds[topBase]);
   dglDrawBitmapSR(mTextureHandle, Point2I(offset.x + mBounds.extent.x - mBitmapBounds[topBase+1].extent.x, offset.y),
                   mBitmapBounds[topBase + 1]);

   RectI destRect;
   destRect.point.x = offset.x + mBitmapBounds[topBase].extent.x;
   destRect.point.y = offset.y;
   destRect.extent.x = mBounds.extent.x - mBitmapBounds[topBase].extent.x - mBitmapBounds[topBase + 1].extent.x;
   destRect.extent.y = mBitmapBounds[topBase + 2].extent.y;
   RectI stretchRect = mBitmapBounds[topBase + 2];
   stretchRect.inset(1,0);
   dglDrawBitmapStretchSR(mTextureHandle, destRect, stretchRect);

   destRect.point.x = offset.x;
   destRect.point.y = offset.y + mBitmapBounds[topBase].extent.y;
   destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
   destRect.extent.y = mBounds.extent.y - mBitmapBounds[topBase].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
   stretchRect = mBitmapBounds[BorderLeft];
   stretchRect.inset(0,1);
   dglDrawBitmapStretchSR(mTextureHandle, destRect, stretchRect);

   destRect.point.x = offset.x + mBounds.extent.x - mBitmapBounds[BorderRight].extent.x;
   destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
   destRect.point.y = offset.y + mBitmapBounds[topBase + 1].extent.y;
   destRect.extent.y = mBounds.extent.y - mBitmapBounds[topBase + 1].extent.y - mBitmapBounds[BorderBottomRight].extent.y;

   stretchRect = mBitmapBounds[BorderRight];
   stretchRect.inset(0,1);
   dglDrawBitmapStretchSR(mTextureHandle, destRect, stretchRect);

   dglDrawBitmapSR(mTextureHandle, offset + Point2I(0, mBounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y), mBitmapBounds[BorderBottomLeft]);
   dglDrawBitmapSR(mTextureHandle, offset + mBounds.extent - mBitmapBounds[BorderBottomRight].extent, mBitmapBounds[BorderBottomRight]);

   destRect.point.x = offset.x + mBitmapBounds[BorderBottomLeft].extent.x;
   destRect.extent.x = mBounds.extent.x - mBitmapBounds[BorderBottomLeft].extent.x - mBitmapBounds[BorderBottomRight].extent.x;

   destRect.point.y = offset.y + mBounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
   destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
   stretchRect = mBitmapBounds[BorderBottom];
   stretchRect.inset(1,0);

   dglDrawBitmapStretchSR(mTextureHandle, destRect, stretchRect);

   //draw the title
   // dhc addition: copied/modded from renderJustifiedText, since we enforce a
   // different color usage here. NOTE: it currently CAN overdraw the controls
   // if mis-positioned or 'scrunched' in a small width.
   dglSetBitmapModulation(mProfile->mFontColor);
   S32 textWidth = mProfile->mFont->getStrWidth((const UTF8 *)mText);
   Point2I start(0,0);
   // align the horizontal
   if ( mProfile->mAlignment == GuiControlProfile::RightAlign )
      start.set( winRect.extent.x - textWidth, 0 );
   else if ( mProfile->mAlignment == GuiControlProfile::CenterAlign )
      start.set( ( winRect.extent.x - textWidth) / 2, 0 );
   else // GuiControlProfile::LeftAlign or garbage... ;)
      start.set( 0, 0 );
   // If the text is longer then the box size, (it'll get clipped) so force Left Justify
   if( textWidth > winRect.extent.x ) start.set( 0, 0 );
   // center the vertical
//   start.y = ( winRect.extent.y - ( font->getHeight() - 2 ) ) / 2;
   dglDrawText(mProfile->mFont, start + offset + mProfile->mTextOffset, mText);

   // deal with rendering the titlebar controls
   AssertFatal(root, "Unable to get the root Canvas.");
   Point2I localPoint = globalToLocalCoord(root->getCursorPos());

   //draw the close button
   Point2I tempUL;
   Point2I tempLR;
   S32 bmp = BmpStates * BmpClose;

   if( mCanClose ) {
      if( mCloseButton.pointInRect( localPoint ) && mPressClose )
         bmp += BmpHilite;

      dglClearBitmapModulation();
      dglDrawBitmapSR(mTextureHandle, offset + mCloseButton.point, mBitmapBounds[bmp]);
   }

   //draw the maximize button
   if( mMaximized )
      bmp = BmpStates * BmpNormal;
   else
      bmp = BmpStates * BmpMaximize;

   if( mCanMaximize ) {
      if( mMaximizeButton.pointInRect( localPoint ) && mPressMaximize )
         bmp += BmpHilite;

      dglClearBitmapModulation();
      dglDrawBitmapSR( mTextureHandle, offset + mMaximizeButton.point, mBitmapBounds[bmp] );
   }

   //draw the minimize button
   if( mMinimized )
      bmp = BmpStates * BmpNormal;
   else
      bmp = BmpStates * BmpMinimize;

   if( mCanMinimize ) {
      if( mMinimizeButton.pointInRect( localPoint ) && mPressMinimize )
         bmp += BmpHilite;

      dglClearBitmapModulation();
      dglDrawBitmapSR( mTextureHandle, offset + mMinimizeButton.point, mBitmapBounds[bmp] );
   }

   if( !mMinimized )
   {
      //render the children
      renderChildControls( offset, mBounds, updateRect );
   }
}



void GuiWindowCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
   Point2I mousePos  = lastGuiEvent.mousePoint;
   RectI winRect   = mBounds;
   RectI rightRect = RectI( ( ( winRect.extent.x + winRect.point.x ) - mResizeRightWidth), winRect.point.y, mResizeRightWidth, winRect.extent.y );
   RectI bottomRect = RectI( winRect.point.x, ( ( winRect.point.y + winRect.extent.y ) - mResizeBottomHeight), winRect.extent.x , mResizeBottomHeight );

   bool resizeRight = rightRect.pointInRect( mousePos );
   bool resizeBottom = bottomRect.pointInRect( mousePos );

   if ( resizeRight && resizeBottom && mResizeHeight && mResizeWidth )
   {
      if(GuiControl::smCursorChanged != CursorManager::curResizeNWSE)
      {
         // We've already changed the cursor, 
         // so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         // Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeNWSE);
         GuiControl::smCursorChanged = CursorManager::curResizeNWSE;
      }

      //cursor = mNWSECursor;
   }
      
   else if ( resizeBottom && mResizeHeight )
   {
      if(GuiControl::smCursorChanged != CursorManager::curResizeHorz)
      {
         // We've already changed the cursor, 
         // so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         // Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeHorz);
         GuiControl::smCursorChanged = CursorManager::curResizeHorz;
      }

      //cursor = mUpDownCursor;
   }
   else if ( resizeRight && mResizeWidth )
   {
      if(GuiControl::smCursorChanged != CursorManager::curResizeVert)
      {
         // We've already changed the cursor, 
         // so set it back before we change it again.
         if(GuiControl::smCursorChanged != -1)
            Input::popCursor();

         // Now change the cursor shape
         Input::pushCursor(CursorManager::curResizeVert);
         GuiControl::smCursorChanged = CursorManager::curResizeVert;
      }

      //cursor = mLeftRightCursor;
   }
   else
   {
      if(GuiControl::smCursorChanged != -1)
      {
         // We've already changed the cursor, 
         // so set it back before we change it again.
         Input::popCursor();

         // We haven't changed it
         GuiControl::smCursorChanged = -1;
      }

      return;
   }
}


