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

#include "guiWindowCtrl_ScriptBinding.h"

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
   mDepressed = false;
   curHitRegion = None;
   mActive = true;

   mMinimized = false;
   mMaximized = false;
   mMouseMovingWin = false;
   mMouseResizeWidth = false;
   mMouseResizeHeight = false;
   mBounds.extent.set(300, 240);
   mMinimizeIndex = -1;
   mTabIndex = -1;

   //other defaults
   mActive = true;
   mPressClose = false;
   mPressMaximize = false;
   mPressMinimize = false;

   mContentProfile = NULL;
   mCloseButtonProfile = NULL;
   mMinButtonProfile = NULL;
   mMaxButtonProfile = NULL;

   setField("contentProfile", "GuiWindowContentProfile");
   setField("closeButtonProfile", "GuiWindowCloseButtonProfile");
   setField("minButtonProfile", "GuiWindowMinButtonProfile");
   setField("maxButtonProfile", "GuiWindowMaxButtonProfile");
   setField("profile", "GuiWindowProfile");

   mLeftRightCursor = NULL;
   mUpDownCursor = NULL;
   mNWSECursor = NULL;
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
   addField("titleHeight",			TypeS32,		Offset(mTitleHeight, GuiWindowCtrl));
   addField("resizeRightWidth",		TypeS32,		Offset(mResizeRightWidth, GuiWindowCtrl));
   addField("resizeBottomHeight",	TypeS32,		Offset(mResizeBottomHeight, GuiWindowCtrl));
   addField("contentProfile", TypeGuiProfile, Offset(mContentProfile, GuiWindowCtrl));
   addField("closeButtonProfile", TypeGuiProfile, Offset(mCloseButtonProfile, GuiWindowCtrl));
   addField("minButtonProfile", TypeGuiProfile, Offset(mMinButtonProfile, GuiWindowCtrl));
   addField("maxButtonProfile", TypeGuiProfile, Offset(mMaxButtonProfile, GuiWindowCtrl));
   addField("leftRightCursor", TypeGuiCursor, Offset(mLeftRightCursor, GuiWindowCtrl));
   addField("upDownCursor", TypeGuiCursor, Offset(mUpDownCursor, GuiWindowCtrl));
   addField("nWSECursor", TypeGuiCursor, Offset(mNWSECursor, GuiWindowCtrl));
}

bool GuiWindowCtrl::isMinimized(S32 &index)
{
   index = mMinimizeIndex;
   return mMinimized && mVisible;
}

bool GuiWindowCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mContentProfile != NULL)
		mContentProfile->incRefCount();

	if (mCloseButtonProfile != NULL)
		mCloseButtonProfile->incRefCount();

	if (mMinButtonProfile != NULL)
		mMinButtonProfile->incRefCount();

	if (mMaxButtonProfile != NULL)
		mMaxButtonProfile->incRefCount();

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
   Parent::onSleep();

   if (mContentProfile != NULL)
	   mContentProfile->decRefCount();

   if (mCloseButtonProfile != NULL)
	   mCloseButtonProfile->decRefCount();

   if (mMinButtonProfile != NULL)
	   mMinButtonProfile->decRefCount();

   if (mMaxButtonProfile != NULL)
	   mMaxButtonProfile->decRefCount();
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
}

void GuiWindowCtrl::onTouchMove(const GuiEvent &event)
{
	curHitRegion = findHitRegion(globalToLocalCoord(event.mousePoint));
}

void GuiWindowCtrl::onTouchLeave(const GuiEvent &event)
{
	curHitRegion = None;
}

void GuiWindowCtrl::onTouchDown(const GuiEvent &event)
{
   setUpdate();

   mOrigBounds = mBounds;

   mMouseDownPosition = event.mousePoint;
   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   curHitRegion = findHitRegion(localPoint);
   mDepressed = true;

   //select this window - move it to the front, and set the first responder
   selectWindow();

   //if we clicked within the title bar
   if (localPoint.y < mTitleHeight)
   {
      //if we clicked on the close button
      if (curHitRegion == CloseButton)
      {
         mPressClose = true;
      }
      else if (curHitRegion == MaxButton)
      {
         mPressMaximize = true;
      }
      else if (curHitRegion == MinButton)
      {
         mPressMinimize = true;
      }
      else if (curHitRegion == TitleBar)
      {
         mMouseMovingWin = mCanMove;
         ResizeComplete();
      }
   }
   else
   {
      MoveComplete();

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

void GuiWindowCtrl::onTouchDragged(const GuiEvent &event)
{
   GuiControl *parent = getParent();
   GuiCanvas *root = getRoot();
   if (! root) return;

   curHitRegion = findHitRegion(globalToLocalCoord(event.mousePoint));

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

void GuiWindowCtrl::onTouchUp(const GuiEvent &event)
{
   bool closing = mPressClose;
   bool maximizing = mPressMaximize;
   bool minimizing = mPressMinimize;
   mPressClose = false;
   mPressMaximize = false;
   mPressMinimize = false;
   mDepressed = false;

   mouseUnlock();
   setUpdate();

   MoveComplete();
   ResizeComplete();

   GuiControl *parent = getParent();
   if (! parent)
      return;

   //see if we take an action
   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   if (closing && mCloseButton.pointInRect(localPoint))
   {
		if(isMethod("onClose"))
		{
			Con::executef(this, 1, "onClose");
		}
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

		 if (isMethod("onRestore"))
		 {
			 Con::executef(this, 1, "onRestore");
		 }
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

		 if (isMethod("onMaximize"))
		 {
			 Con::executef(this, 1, "onMaximize");
		 }
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

		 if (isMethod("onRestore"))
		 {
			 Con::executef(this, 1, "onRestore");
		 }
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

         //this algorithm assumes all window have the same title height, and will minimize to 148 pix
         Point2I newExtent(148, mTitleHeight);

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

		 if (isMethod("onMinimize"))
		 {
			 Con::executef(this, 1, "onMinimize");
		 }
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

GuiWindowCtrl::Region GuiWindowCtrl::findHitRegion(const Point2I& pt)
{
	if (mCanClose && mCloseButton.pointInRect(pt)) 
	{
		return CloseButton;
	}
	else if (mCanMaximize && mMaximizeButton.pointInRect(pt))
	{
		return MaxButton;
	}
	else if (mCanMinimize && mMinimizeButton.pointInRect(pt))
	{
		return MinButton;
	}
	else if (mTitleBar.pointInRect(pt))
	{
		return TitleBar;
	}
	return None;
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

void GuiWindowCtrl::onFocus()
{
	//bubble the focus up
	GuiControl *parent = getParent();
	if (parent)
	{
		parent->pushObjectToBack(this);
		parent->onFocus();
	}

	setFirstResponder(mFirstResponder);
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

void GuiWindowCtrl::ResizeComplete()
{
	if ((mMouseResizeWidth || mMouseResizeHeight) && isMethod("onResize"))
	{
		Con::executef(this, 1, "onResize");
	}

	mMouseResizeWidth = false;
	mMouseResizeHeight = false;
}

void GuiWindowCtrl::MoveComplete()
{
	if (mMouseMovingWin && isMethod("onMove"))
	{
		Con::executef(this, 1, "onMove");
	}

	mMouseMovingWin = false;
}

void GuiWindowCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	//Does this window have focus (does it or a child receive key events)?
	GuiCanvas *root = getRoot();
	GuiControl *firstResponder = root ? root->getFirstResponder() : NULL;
	bool hasFocus = (firstResponder && ControlIsChild(firstResponder));

	GuiControlState currentState = NormalState;
	if (mMinimized)
	{
		currentState = DisabledState;
	}
	else if (hasFocus)
	{
		currentState = SelectedState;
	}
	else if (curHitRegion == TitleBar)
	{
		currentState = HighlightState;
	}

	//Render the title bar
	RectI ctrlRectTitle = applyMargins(offset, Point2I(mBounds.extent.x, mTitleHeight), currentState, mProfile);
	if (!ctrlRectTitle.isValidRect())
	{
		return;
	}
	mTitleBar.set(Point2I(ctrlRectTitle.point.x - offset.x, ctrlRectTitle.point.y - offset.y), ctrlRectTitle.extent);
	renderUniversalRect(ctrlRectTitle, mProfile, currentState);

	//Render Text and buttons
	dglSetBitmapModulation(mProfile->mFontColor);
	RectI fillRectTitle = applyBorders(ctrlRectTitle.point, ctrlRectTitle.extent, currentState, mProfile);
	RectI contentRectTitle = applyPadding(fillRectTitle.point, fillRectTitle.extent, currentState, mProfile);

	if (contentRectTitle.isValidRect())
	{
		RectI textRect = renderButtons(offset, contentRectTitle);
		renderText(textRect.point, textRect.extent, mText, mProfile);
	}

	//Render window contents
	if (!mMinimized)
	{
		currentState = currentState != SelectedState ? NormalState : SelectedState;
		RectI ctrlRectWindow = applyMargins(Point2I(offset.x, offset.y + mTitleHeight), Point2I(mBounds.extent.x, mBounds.extent.y - mTitleHeight), currentState, mContentProfile);
		if (!ctrlRectWindow.isValidRect())
		{
			return;
		}
		renderUniversalRect(ctrlRectWindow, mContentProfile, currentState);

		RectI fillRectWindow = applyBorders(ctrlRectWindow.point, ctrlRectWindow.extent, currentState, mContentProfile);
		RectI contentRectWindow = applyPadding(fillRectWindow.point, fillRectWindow.extent, currentState, mContentProfile);

		if (contentRectWindow.isValidRect())
		{
			//render the children
			renderChildControls(offset, contentRectWindow, updateRect);
		}
	}
}

RectI GuiWindowCtrl::renderButtons(const Point2I &offset, const RectI &contentRect)
{
	S32 distanceFromEdge = 0;

	if (mCanClose)
	{
		GuiControlState state = getRegionCurrentState(Region::CloseButton);
		RectI content = renderButton(contentRect,  distanceFromEdge, state, mCloseButtonProfile, Icon::Close);
		mCloseButton.set(Point2I(content.point.x - offset.x, content.point.y - offset.y), content.extent);
		distanceFromEdge += content.extent.x;

		GuiBorderProfile *leftProfile = mCloseButtonProfile->getLeftBorder();
		S32 leftSize = (leftProfile) ? leftProfile->getMargin(state) : 0;
		distanceFromEdge += leftSize;

		GuiBorderProfile *rightProfile = mCloseButtonProfile->getRightBorder();
		S32 rightSize = (rightProfile) ? rightProfile->getMargin(state) : 0;
		distanceFromEdge += rightSize;
	}
	if (mCanMaximize)
	{
		GuiControlState state = getRegionCurrentState(Region::MaxButton);
		RectI content = renderButton(contentRect, distanceFromEdge, state, mMaxButtonProfile, Icon::Max);
		mMaximizeButton.set(Point2I(content.point.x - offset.x, content.point.y - offset.y), content.extent);
		distanceFromEdge += content.extent.x;

		GuiBorderProfile *leftProfile = mMaxButtonProfile->getLeftBorder();
		S32 leftSize = (leftProfile) ? leftProfile->getMargin(state) : 0;
		distanceFromEdge += leftSize;

		GuiBorderProfile *rightProfile = mMaxButtonProfile->getRightBorder();
		S32 rightSize = (rightProfile) ? rightProfile->getMargin(state) : 0;
		distanceFromEdge += rightSize;
	}
	if (mCanMinimize)
	{
		GuiControlState state = getRegionCurrentState(Region::MinButton);
		RectI content = renderButton(contentRect, distanceFromEdge, state, mMinButtonProfile, Icon::Min);
		mMinimizeButton.set(Point2I(content.point.x - offset.x, content.point.y - offset.y), content.extent);
		distanceFromEdge += content.extent.x;

		GuiBorderProfile *leftProfile = mMinButtonProfile->getLeftBorder();
		S32 leftSize = (leftProfile) ? leftProfile->getMargin(state) : 0;
		distanceFromEdge += leftSize;

		GuiBorderProfile *rightProfile = mMinButtonProfile->getRightBorder();
		S32 rightSize = (rightProfile) ? rightProfile->getMargin(state) : 0;
		distanceFromEdge += rightSize;
	}

	if (mProfile->mAlignment != GuiControlProfile::AlignmentType::RightAlign)
	{
		return RectI(contentRect.point.x, contentRect.point.y, contentRect.extent.x - distanceFromEdge, contentRect.extent.y);
	}
	else
	{
		return RectI(contentRect.point.x + distanceFromEdge, contentRect.point.y, contentRect.extent.x - distanceFromEdge, contentRect.extent.y);
	}
}

RectI GuiWindowCtrl::renderButton(const RectI &contentRect, S32 distanceFromEdge, GuiControlState buttonState, GuiControlProfile *profile, Icon defaultIcon)
{
	RectI buttonContent = applyMargins(Point2I(contentRect.point), Point2I(contentRect.extent), buttonState, profile);
	S32 horizMarginSize = contentRect.extent.x - buttonContent.extent.x;
	RectI finalButtonRect = RectI(contentRect.point, Point2I(buttonContent.extent.y + horizMarginSize, contentRect.extent.y));
	if (mProfile->mAlignment != GuiControlProfile::AlignmentType::RightAlign)
	{
		//get the right margin and add it to the distance from the edge
		GuiBorderProfile *rightProfile = profile->getRightBorder();
		S32 rightSize = (rightProfile) ? rightProfile->getMargin(buttonState) : 0;
		finalButtonRect.point.x = contentRect.point.x + (contentRect.extent.x - finalButtonRect.extent.x) - distanceFromEdge;
		distanceFromEdge += rightSize;
	}
	else
	{
		//get the left margin and add it to the disance from the edge
		GuiBorderProfile *leftProfile = profile->getLeftBorder();
		S32 leftSize = (leftProfile) ? leftProfile->getMargin(buttonState) : 0;
		finalButtonRect.point.x = contentRect.point.x + distanceFromEdge;
		distanceFromEdge += leftSize;
	}
	RectI finalButtonContent = applyMargins(finalButtonRect.point, finalButtonRect.extent, buttonState, profile);
	renderUniversalRect(finalButtonContent, profile, buttonState);

	//now draw an icon if default rendering was used.
	if ((profile->mImageAsset == NULL || !mCloseButtonProfile->mImageAsset->isAssetValid()) &&
		profile->mBitmapName == NULL)
	{
		RectI fillRect = applyBorders(finalButtonContent.point, finalButtonContent.extent, buttonState, profile);
		RectI contentRect = applyPadding(fillRect.point, fillRect.extent, buttonState, profile);

		//draw the icon
		if (defaultIcon == Icon::Close)
		{
			Point2I p1 = Point2I(contentRect.point.x + 1, contentRect.point.y);
			Point2I p2 = Point2I(contentRect.point.x, contentRect.point.y + 1);
			Point2I p3 = Point2I(contentRect.point.x + contentRect.extent.x - 1, contentRect.point.y + contentRect.extent.y);
			Point2I p4 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + contentRect.extent.y - 1);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));

			Point2I p5 = Point2I(contentRect.point.x, contentRect.point.y + contentRect.extent.y - 1);
			Point2I p6 = Point2I(contentRect.point.x + 1, contentRect.point.y + contentRect.extent.y);
			Point2I p7 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + 1);
			Point2I p8 = Point2I(contentRect.point.x + contentRect.extent.x - 1, contentRect.point.y);
			dglDrawQuadFill(p5, p6, p7, p8, profile->getFontColor(buttonState));
		}
		else if (defaultIcon == Icon::Min)
		{
			S32 h = (contentRect.len_y() / 2) - 1;
			Point2I p1 = Point2I(contentRect.point.x, contentRect.point.y + h);
			Point2I p2 = Point2I(contentRect.point.x, contentRect.point.y + h + 2);
			Point2I p3 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + h + 2);
			Point2I p4 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + h);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));
		}
		else if (defaultIcon == Icon::Max)
		{
			//left side
			S32 h = (contentRect.len_y() / 2) - 1;
			Point2I p1 = Point2I(contentRect.point.x, contentRect.point.y);
			Point2I p2 = Point2I(contentRect.point.x, contentRect.point.y + contentRect.extent.y);
			Point2I p3 = Point2I(contentRect.point.x + 2, contentRect.point.y + contentRect.extent.y);
			Point2I p4 = Point2I(contentRect.point.x + 2, contentRect.point.y);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));

			//right side
			p1.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y);
			p2.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y + contentRect.extent.y);
			p3.set(contentRect.point.x + contentRect.extent.x, contentRect.point.y + contentRect.extent.y);
			p4.set(contentRect.point.x + contentRect.extent.x, contentRect.point.y);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));

			//top
			p1.set(contentRect.point.x + 2, contentRect.point.y);
			p2.set(contentRect.point.x + 2, contentRect.point.y + 2);
			p3.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y + 2);
			p4.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));

			//bottom
			p1.set(contentRect.point.x + 2, contentRect.point.y + contentRect.extent.y - 2);
			p2.set(contentRect.point.x + 2, contentRect.point.y + contentRect.extent.y);
			p3.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y + contentRect.extent.y);
			p4.set(contentRect.point.x + contentRect.extent.x - 2, contentRect.point.y + contentRect.extent.y - 2);
			dglDrawQuadFill(p1, p2, p3, p4, profile->getFontColor(buttonState));
		}
	}

	return finalButtonContent;
}

GuiControlState GuiWindowCtrl::getRegionCurrentState(GuiWindowCtrl::Region region)
{
	GuiControlState currentState = GuiControlState::NormalState;
	if (!mActive)
	{
		currentState = GuiControlState::DisabledState;
	}
	else if (curHitRegion == region && mDepressed)
	{
		currentState = GuiControlState::SelectedState;
	}
	else if (curHitRegion == region)
	{
		currentState = GuiControlState::HighlightState;
	}
	return currentState;
}

void GuiWindowCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
	Point2I mousePos  = lastGuiEvent.mousePoint;
	RectI winRect   = mBounds;
	Point2I offset = getParent()->localToGlobalCoord(Point2I(0,0));
	RectI rightRect = RectI( ( ( winRect.extent.x + winRect.point.x ) - mResizeRightWidth + offset.x), winRect.point.y + mTitleHeight + offset.y, mResizeRightWidth, winRect.extent.y );
	RectI bottomRect = RectI( winRect.point.x + offset.x, ( ( winRect.point.y + winRect.extent.y ) - mResizeBottomHeight) + offset.y, winRect.extent.x , mResizeBottomHeight );

	bool resizeRight = rightRect.pointInRect( mousePos );
	bool resizeBottom = bottomRect.pointInRect( mousePos );

	if ( resizeRight && resizeBottom && mResizeHeight && mResizeWidth )
	{
		if (mNWSECursor == NULL)
		{
			SimObject *obj;
			obj = Sim::findObject("NWSECursor");
			mNWSECursor = dynamic_cast<GuiCursor*>(obj);
		}
		if(mNWSECursor != NULL)
		{
			cursor = mNWSECursor;
		}
	}
      
	else if ( resizeBottom && mResizeHeight )
	{
		if(mUpDownCursor == NULL)
		{
			SimObject *obj;
			obj = Sim::findObject("UpDownCursor");
			mUpDownCursor = dynamic_cast<GuiCursor*>(obj);
		}
		if (mUpDownCursor != NULL)
		{
			cursor = mUpDownCursor;
		}
	}
	else if ( resizeRight && mResizeWidth )
	{
		if (mLeftRightCursor == NULL)
		{
			SimObject *obj;
			obj = Sim::findObject("LeftRightCursor");
			mLeftRightCursor = dynamic_cast<GuiCursor*>(obj);
		}
		if (mLeftRightCursor != NULL)
		{
			cursor = mLeftRightCursor;
		}
	}
}

void GuiWindowCtrl::setControlContentProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiWindowCtrl::setControlContentProfile: invalid content profile");
	if (prof == mContentProfile)
		return;
	if (mAwake)
		mContentProfile->decRefCount();
	mContentProfile = prof;
	if (mAwake)
		mContentProfile->incRefCount();
}

void GuiWindowCtrl::setControlCloseButtonProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiWindowCtrl::setControlCloseButtonProfile: invalid close button profile");
	if (prof == mCloseButtonProfile)
		return;
	if (mAwake)
		mCloseButtonProfile->decRefCount();
	mCloseButtonProfile = prof;
	if (mAwake)
		mCloseButtonProfile->incRefCount();
}

void GuiWindowCtrl::setControlMinButtonProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiWindowCtrl::setControlMinButtonProfile: invalid minimize button profile");
	if (prof == mMinButtonProfile)
		return;
	if (mAwake)
		mMinButtonProfile->decRefCount();
	mMinButtonProfile = prof;
	if (mAwake)
		mMinButtonProfile->incRefCount();
}

void GuiWindowCtrl::setControlMaxButtonProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiWindowCtrl::setControlMaxButtonProfile: invalid maximize button profile");
	if (prof == mMaxButtonProfile)
		return;
	if (mAwake)
		mMaxButtonProfile->decRefCount();
	mMaxButtonProfile = prof;
	if (mAwake)
		mMaxButtonProfile->incRefCount();
}

void GuiWindowCtrl::setControlLeftRightCursor(GuiCursor* cursor)
{
	AssertFatal(cursor, "GuiWindowCtrl::setControlLeftRightCursor: invalid cursor");
	if (cursor == mLeftRightCursor)
		return;
	mLeftRightCursor = cursor;
}

void GuiWindowCtrl::setControlUpDownCursor(GuiCursor* cursor)
{
	AssertFatal(cursor, "GuiWindowCtrl::setControlUpDownCursor: invalid cursor");
	if (cursor == mUpDownCursor)
		return;
	mUpDownCursor = cursor;
}

void GuiWindowCtrl::setControlNWSECursor(GuiCursor* cursor)
{
	AssertFatal(cursor, "GuiWindowCtrl::setControlNWSECursor: invalid cursor");
	if (cursor == mNWSECursor)
		return;
	mNWSECursor = cursor;
}
