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
#include "graphics/gBitmap.h"
#include "graphics/TextureManager.h"
#include "io/resource/resourceManager.h"
#include "platform/event.h"
#include "graphics/dgl.h"
#include "gui/guiArrayCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/guiDefaultControlRender.h"

#include "guiScrollCtrl_ScriptBinding.h"


static EnumTable::Enums scrollBarEnums[] =
{
   { GuiScrollCtrl::ScrollBarAlwaysOn,     "alwaysOn"     },
   { GuiScrollCtrl::ScrollBarAlwaysOff,    "alwaysOff"    },
   { GuiScrollCtrl::ScrollBarDynamic,      "dynamic"      },
};
static EnumTable gScrollBarTable(3, &scrollBarEnums[0]);

IMPLEMENT_CONOBJECT(GuiScrollCtrl);

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiScrollCtrl::GuiScrollCtrl()
{
   mBounds.extent.set(200,200);
   mScrollBarThickness = 16;
   mScrollBarDragTolerance = 130;
   mDepressed = false;
   curHitRegion = None;
   mActive = true;
   mShowArrowButtons = true;
   mBaseThumbSize = (mScrollBarThickness * 2);

   mUseConstantHeightThumb = false;
   mIsContainer = true;

   mForceVScrollBar = ScrollBarAlwaysOn;
   mForceHScrollBar = ScrollBarAlwaysOn;

   mThumbProfile = NULL;
   mTrackProfile = NULL;
   mArrowProfile = NULL;

   mScrollOffset.set(0, 0);
   mContentExt.set(200,200);

   setField("thumbProfile", "GuiScrollThumbProfile");
   setField("arrowProfile", "GuiScrollArrowProfile");
   setField("trackProfile", "GuiScrollTrackProfile");
   setField("profile", "GuiScrollProfile");
}

void GuiScrollCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("hScrollBar",           TypeEnum,    Offset(mForceHScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("vScrollBar",           TypeEnum,    Offset(mForceVScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("constantThumbHeight",  TypeBool,    Offset(mUseConstantHeightThumb, GuiScrollCtrl));
   addField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiScrollCtrl));
   addField("showArrowButtons",		TypeBool,	 Offset(mShowArrowButtons, GuiScrollCtrl));
   addField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiScrollCtrl));
   addField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiScrollCtrl));
   addField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiScrollCtrl));

}

void GuiScrollCtrl::resize(const Point2I &newPos, const Point2I &newExt)
{
   Parent::resize(newPos, newExt);
   computeSizes();
}

void GuiScrollCtrl::childResized(GuiControl *child)
{
   Parent::childResized(child);
   computeSizes();
}

void GuiScrollCtrl::addObject(SimObject* object)
{
	Parent::addObject(object);
	computeSizes();
}

bool GuiScrollCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

   return true;
}

void GuiScrollCtrl::onSleep()
{
   Parent::onSleep();

   if (mThumbProfile != NULL)
	   mThumbProfile->decRefCount();

   if (mTrackProfile != NULL)
	   mTrackProfile->decRefCount();

   if (mArrowProfile != NULL)
	   mArrowProfile->decRefCount();
}

void GuiScrollCtrl::setControlThumbProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiScrollCtrl::setControlThumbProfile: invalid thumb profile");
	if (prof == mThumbProfile)
		return;
	if (mAwake)
		mThumbProfile->decRefCount();
	mThumbProfile = prof;
	if (mAwake)
		mThumbProfile->incRefCount();

}

void GuiScrollCtrl::setControlTrackProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiScrollCtrl::setControlTrackProfile: invalid track profile");
	if (prof == mTrackProfile)
		return;
	if (mAwake)
		mTrackProfile->decRefCount();
	mTrackProfile = prof;
	if (mAwake)
		mTrackProfile->incRefCount();

}

void GuiScrollCtrl::setControlArrowProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiScrollCtrl::setControlArrowProfile: invalid Arrow profile");
	if (prof == mArrowProfile)
		return;
	if (mAwake)
		mArrowProfile->decRefCount();
	mArrowProfile = prof;
	if (mAwake)
		mArrowProfile->incRefCount();

}

GuiControl* GuiScrollCtrl::findHitControl(const Point2I& pt, S32 initialLayer)
{
	Point2I localPt = localToGlobalCoord(pt);
	if (mChildArea.pointInRect(localPt))
	{
		iterator i = end(); // find in z order (last to first)
		while (i != begin())
		{
			i--;
			GuiControl* ctrl = static_cast<GuiControl*>(*i);
			if (initialLayer >= 0 && ctrl->mLayer > initialLayer)
			{
				continue;
			}
			else if (ctrl->mVisible && ctrl->pointInControl(pt - ctrl->mRenderInsetLT))
			{
				Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
				GuiControl* hitCtrl = ctrl->findHitControl(ptemp);

				if (hitCtrl->mProfile->mUseInput)
					return hitCtrl;
			}
		}
	}
	return this;
}

GuiScrollCtrl::Region GuiScrollCtrl::findHitRegion(const Point2I& pt)
{
	if (mVBarEnabled && mHasVScrollBar)
	{
		if (mShowArrowButtons && mUpArrowRect.pointInRect(pt))
			return UpArrow;
		else if (mShowArrowButtons && mDownArrowRect.pointInRect(pt))
			return DownArrow;
		else if (mVTrackRect.pointInRect(pt))
		{
			S32 y = pt.y - mVTrackRect.point.y;
			if (y < mVThumbPos)
				return UpPage;
			else if (y < mVThumbPos + mVThumbSize)
				return VertThumb;
			else
				return DownPage;
		}
	}
	if (mHBarEnabled && mHasHScrollBar)
	{
		if (mShowArrowButtons && mLeftArrowRect.pointInRect(pt))
			return LeftArrow;
		else if (mShowArrowButtons && mRightArrowRect.pointInRect(pt))
			return RightArrow;
		else if (mHTrackRect.pointInRect(pt))
		{
			S32 x = pt.x - mHTrackRect.point.x;
			if (x < mHThumbPos)
				return LeftPage;
			else if (x < mHThumbPos + mHThumbSize)
				return HorizThumb;
			else
				return RightPage;
		}
	}
	return None;
}

#pragma region CalculationFunctions
void GuiScrollCtrl::computeSizes()
{
	calcContentExtents();

	mHBarEnabled = false;
	mVBarEnabled = false;
	mHasVScrollBar = (mForceVScrollBar == ScrollBarAlwaysOn);
	mHasHScrollBar = (mForceHScrollBar == ScrollBarAlwaysOn);

	setUpdate();

	if (calcChildExtents())
	{
		if (mChildExt.x > mContentExt.x && (mForceHScrollBar == ScrollBarDynamic))
		{
			mHasHScrollBar = true;
		}
		if (mChildExt.y > mContentExt.y && (mForceVScrollBar == ScrollBarDynamic))
		{
			mHasVScrollBar = true;

			// If Extent X Changed, check Horiz Scrollbar.
			if (mChildExt.x > mContentExt.x && !mHasHScrollBar && (mForceHScrollBar == ScrollBarDynamic))
			{
				mHasHScrollBar = true;
			}
		}

		if (mHasVScrollBar)
			mContentExt.x -= mScrollBarThickness;
		if (mHasHScrollBar)
			mContentExt.y -= mScrollBarThickness;

		// enable needed scroll bars
		if (mChildExt.x > mContentExt.x)
			mHBarEnabled = true;
		if (mChildExt.y > mContentExt.y)
			mVBarEnabled = true;

		//Are we now over-scrolled?
		if ((mScrollOffset.x + mContentExt.x) > mChildExt.x)
			mScrollOffset.x = getMax(mChildExt.x - mContentExt.x, 0);
		if ((mScrollOffset.y + mContentExt.y) > mChildExt.y)
			mScrollOffset.y = getMax(mChildExt.y - mContentExt.y, 0);
	}
	// build all the rectangles and such...
	calcScrollRects();
	calcThumbs();
}

void GuiScrollCtrl::calcContentExtents()
{
	RectI ctrlRect = applyMargins(mBounds.point, mBounds.extent, NormalState, mProfile);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);
	mContentExt = contentRect.extent;
}

bool GuiScrollCtrl::calcChildExtents()
{
	if (!size())
		return false;

	mChildExt = Point2I(0,0);
	for (iterator itr = begin(); itr != end(); ++itr)
	{
		GuiControl* child = dynamic_cast<GuiControl*>(*itr);
		mChildExt.setMax(child->getExtent() + child->getPosition());
	}
	return true;
}


void GuiScrollCtrl::calcScrollRects(void)
{
	RectI ctrlRect = applyMargins(mBounds.point.Zero, mBounds.extent, NormalState, mProfile);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);

	if (mHasVScrollBar)
	{
		RectI vScrollRect = RectI(fillRect.point.x + fillRect.extent.x - mScrollBarThickness, fillRect.point.y, mScrollBarThickness, fillRect.extent.y);
		if (mHasHScrollBar)
		{
			vScrollRect.extent.y -= mScrollBarThickness;
		}
		mVTrackRect = RectI(vScrollRect);
		if (mShowArrowButtons)
		{
			mUpArrowRect = RectI(vScrollRect.point.x, vScrollRect.point.y, vScrollRect.extent.x, mScrollBarThickness);
			mDownArrowRect = RectI(vScrollRect.point.x, vScrollRect.point.y + vScrollRect.extent.y - mScrollBarThickness, vScrollRect.extent.x, mScrollBarThickness);
			mVTrackRect = RectI(vScrollRect.point.x, vScrollRect.point.y + mScrollBarThickness, vScrollRect.extent.x, vScrollRect.extent.y - (2 * mScrollBarThickness));
		}
	}

	if (mHasHScrollBar)
	{
		RectI hScrollRect = RectI(fillRect.point.x, fillRect.point.y + fillRect.extent.y - mScrollBarThickness, fillRect.extent.x, mScrollBarThickness);
		if (mHasVScrollBar)
		{
			hScrollRect.extent.x -= mScrollBarThickness;
		}
		mHTrackRect = RectI(hScrollRect);
		if (mShowArrowButtons)
		{
			mLeftArrowRect = RectI(hScrollRect.point.x, hScrollRect.point.y, mScrollBarThickness, hScrollRect.extent.y);
			mRightArrowRect = RectI(hScrollRect.point.x + hScrollRect.extent.x - mScrollBarThickness, hScrollRect.point.y, mScrollBarThickness, hScrollRect.extent.y);
			mHTrackRect = RectI(hScrollRect.point.x + mScrollBarThickness, hScrollRect.point.y, hScrollRect.extent.x - (2 * mScrollBarThickness), hScrollRect.extent.y);
		}
	}
}

void GuiScrollCtrl::calcThumbs()
{
   if (mHBarEnabled)
   {
	   S32 totalArea = mChildExt.x - mContentExt.x;
	   if (totalArea <= 0)
	   {
		   mHBarEnabled = false;
		   mHThumbSize = mBaseThumbSize;
		   mHThumbPos = 0;
	   }
	   else
	   {
			U32 trackSize = mHTrackRect.len_x();

			if (mUseConstantHeightThumb)
				mHThumbSize = mBaseThumbSize;
			else
				mHThumbSize = getMax(mBaseThumbSize, S32((mContentExt.x * trackSize) / mChildExt.x));

			F32 fraction = (F32)mScrollOffset.x / (F32)totalArea;
			mHThumbPos = roundf((trackSize - mHThumbSize) * fraction);
		}
   }
   if (mVBarEnabled)
   {
		S32 totalArea = mChildExt.y - mContentExt.y;
		if (totalArea <= 0)
		{
			mVBarEnabled = false;
			mVThumbSize = mBaseThumbSize;
			mVThumbPos = 0;
		}
		else
		{
			U32 trackSize = mVTrackRect.len_y();

			if (mUseConstantHeightThumb)
				mVThumbSize = mBaseThumbSize;
			else
				mVThumbSize = getMax(mBaseThumbSize, S32((mContentExt.y * trackSize) / mChildExt.y));

			F32 fraction = (F32)mScrollOffset.y / (F32)totalArea;
			mVThumbPos = roundf((trackSize - mVThumbSize) * fraction);
		}
      
   }
}
#pragma endregion

#pragma region ScrollingFunctions
void GuiScrollCtrl::scrollDelta(S32 deltaX, S32 deltaY)
{
	mScrollOffset.x += deltaX;
	mScrollOffset.y += deltaY;

	mScrollOffset.setMin(mChildExt - mContentExt);
	mScrollOffset.setMax(mScrollOffset.Zero);

	calcThumbs();
}

void GuiScrollCtrl::scrollTo(S32 x, S32 y)
{
   if(!size())
      return;

    // keep scroll start state
    Point2I startPoint = Point2I(0,0);

   setUpdate();
   if (x > mChildExt.x - mContentExt.x)
      x = mChildExt.x - mContentExt.x;
   if (x < 0)
      x = 0;

   if (y > mChildExt.y - mContentExt.y)
      y = mChildExt.y - mContentExt.y;
   if (y < 0)
      y = 0;

   mScrollOffset.set(x, y);
   calcThumbs();
}

void GuiScrollCtrl::scrollByRegion(Region reg)
{
	setUpdate();
	if (!size())
		return;
	GuiControl* content = (GuiControl*)front();
	U32 rowHeight, columnWidth;
	U32 pageHeight, pageWidth;

	content->getScrollLineSizes(&rowHeight, &columnWidth);

	if (rowHeight >= (U32)mContentExt.y)
		pageHeight = 1;
	else
		pageHeight = mContentExt.y - rowHeight;

	if (columnWidth >= (U32)mContentExt.x)
		pageWidth = 1;
	else
		pageWidth = mContentExt.x - columnWidth;

	if (mVBarEnabled)
	{
		switch (reg)
		{
		case UpPage:
			scrollDelta(0, -(S32)pageHeight);
			break;
		case DownPage:
			scrollDelta(0, pageHeight);
			break;
		case UpArrow:
			scrollDelta(0, -(S32)rowHeight);
			break;
		case DownArrow:
			scrollDelta(0, rowHeight);
			break;
		case LeftArrow:
		case RightArrow:
		case LeftPage:
		case RightPage:
		case VertThumb:
		case HorizThumb:
		case None:
			//Con::errorf("Unhandled case in GuiScrollCtrl::scrollByRegion");
			break;
		}
	}

	if (mHBarEnabled)
	{
		switch (reg)
		{
		case LeftPage:
			scrollDelta(-(S32)pageWidth, 0);
			break;
		case RightPage:
			scrollDelta(pageWidth, 0);
			break;
		case LeftArrow:
			scrollDelta(-(S32)columnWidth, 0);
			break;
		case RightArrow:
			scrollDelta(columnWidth, 0);
			break;
		case UpArrow:
		case DownArrow:
		case UpPage:
		case DownPage:
		case VertThumb:
		case HorizThumb:
		case None:
			//Con::errorf("Unhandled case in GuiScrollCtrl::scrollByRegion");
			break;
		}
	}
}

void GuiScrollCtrl::scrollRectVisible(RectI rect)
{
	// rect is passed in virtual client space
	if (rect.extent.x > mContentExt.x)
		rect.extent.x = mContentExt.x;
	if (rect.extent.y > mContentExt.y)
		rect.extent.y = mContentExt.y;

	// Determine the points bounding the requested rectangle
	Point2I rectUpperLeft = rect.point;
	Point2I rectLowerRight = rect.point + rect.extent;

	// Determine the points bounding the actual visible area...
	Point2I visUpperLeft = mScrollOffset;
	Point2I visLowerRight = mContentExt;
	Point2I delta(0, 0);

	// We basically try to make sure that first the top left of the given
	// rect is visible, and if it is, then that the bottom right is visible.

	// Make sure the rectangle is visible along the X axis...
	if (rectUpperLeft.x < visUpperLeft.x)
		delta.x = rectUpperLeft.x - visUpperLeft.x;
	else if (rectLowerRight.x > visLowerRight.x)
		delta.x = rectLowerRight.x - visLowerRight.x;

	// Make sure the rectangle is visible along the Y axis...
	if (rectUpperLeft.y < visUpperLeft.y)
		delta.y = rectUpperLeft.y - visUpperLeft.y;
	else if (rectLowerRight.y > visLowerRight.y)
		delta.y = rectLowerRight.y - visLowerRight.y;

	// If we had any changes, scroll, otherwise don't.
	if (delta.x || delta.y)
		scrollDelta(delta.x, delta.y);
}
#pragma endregion

#pragma region Event_Processing

void GuiScrollCtrl::onTouchMove(const GuiEvent& event)
{
	curHitRegion = findHitRegion(globalToLocalCoord(event.mousePoint));
}

void GuiScrollCtrl::onTouchLeave(const GuiEvent &event)
{
	if (!mDepressed)
	{
		curHitRegion = None;
	}
}

bool GuiScrollCtrl::onKeyDown(const GuiEvent &event)
{
      switch (event.keyCode)
      {
         case KEY_RIGHT:
            scrollByRegion(RightArrow);
            return true;

         case KEY_LEFT:
            scrollByRegion(LeftArrow);
            return true;

         case KEY_DOWN:
            scrollByRegion(DownArrow);
            return true;

         case KEY_UP:
            scrollByRegion(UpArrow);
            return true;

         case KEY_PAGE_UP:
            scrollByRegion(UpPage);
            return true;

         case KEY_PAGE_DOWN:
            scrollByRegion(DownPage);
            return true;
      }
   return Parent::onKeyDown(event);
}

void GuiScrollCtrl::onTouchDown(const GuiEvent &event)
{
   mouseLock();

   setUpdate();

   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   curHitRegion = findHitRegion(curMousePos);
   mDepressed = true;

   // Set a 0.5 second delay before we start scrolling
   mLastUpdated = Platform::getVirtualMilliseconds() + 500;

   scrollByRegion(curHitRegion);

   if (curHitRegion == VertThumb)
   {
	   mScrollOffsetAnchor = mScrollOffset;
      mThumbMouseDelta = curMousePos.y - mVThumbPos;
   }
   else if (curHitRegion == HorizThumb)
   {
	   mScrollOffsetAnchor = mScrollOffset;
      mThumbMouseDelta = curMousePos.x - mHThumbPos;
   }
}

void GuiScrollCtrl::onTouchUp(const GuiEvent &)
{
   mouseUnlock();

   setUpdate();

   curHitRegion = None;
   mDepressed = false;
}

void GuiScrollCtrl::onTouchDragged(const GuiEvent &event)
{
   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   setUpdate();

   if ( (curHitRegion != VertThumb) && (curHitRegion != HorizThumb) )
   {
      Region hit = findHitRegion(curMousePos);
      if (hit != curHitRegion)
         mDepressed = false;
      else
         mDepressed = true;
      return;
   }

   // ok... if the mouse is 'near' the scroll bar, scroll with it
   // otherwise, snap back to the previous position.

   if (curHitRegion == VertThumb)
   {
      if (curMousePos.x >= mVTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mVTrackRect.point.x + mVTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mVTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mVTrackRect.point.y + mVTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newVThumbPos = curMousePos.y - mThumbMouseDelta;
         if(newVThumbPos != mVThumbPos)
         {
            S32 newVPos = (newVThumbPos) *
                          (mChildExt.y - mContentExt.y) /
                          (mVTrackRect.extent.y - mVThumbSize);

            scrollTo(mScrollOffset.x, newVPos);
         }
      }
      else
         scrollTo(mScrollOffset.x, mScrollOffsetAnchor.y);
   }
   else if (curHitRegion == HorizThumb)
   {
      if (curMousePos.x >= mHTrackRect.point.x - mScrollBarDragTolerance &&
         curMousePos.x <= mHTrackRect.point.x + mHTrackRect.extent.x - 1 + mScrollBarDragTolerance &&
         curMousePos.y >= mHTrackRect.point.y - mScrollBarDragTolerance &&
         curMousePos.y <= mHTrackRect.point.y + mHTrackRect.extent.y - 1 + mScrollBarDragTolerance)
      {
         S32 newHThumbPos = curMousePos.x - mThumbMouseDelta;
         if(newHThumbPos != mHThumbPos)
         {
            S32 newHPos = (newHThumbPos) *
                          (mChildExt.x - mContentExt.x) /
                          (mHTrackRect.extent.x - mHThumbSize);

            scrollTo(newHPos, mScrollOffset.y);
         }
      }
      else
         scrollTo(mScrollOffsetAnchor.x, mScrollOffset.y);
   }
}

bool GuiScrollCtrl::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mScrollOffset;
   scrollByRegion((event.modifier & SI_CTRL) ? UpPage : UpArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl*>( *itr );
      grandKid->onTouchMove( event );
   }

   // If no scrolling happened (already at the top), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mScrollOffset))
      return parent->onMouseWheelUp(event);

   return true;
}

bool GuiScrollCtrl::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   Point2I previousPos = mScrollOffset;
   scrollByRegion((event.modifier & SI_CTRL) ? DownPage : DownArrow);

   // Tell the kids that the mouse moved (relatively):
   iterator itr;
   for ( itr = begin(); itr != end(); itr++ )
   {
      GuiControl* grandKid = static_cast<GuiControl *>( *itr );
      grandKid->onTouchMove( event );
   }

   // If no scrolling happened (already at the bottom), pass it on to the parent.
   GuiControl* parent = getParent();
   if (parent && (previousPos == mScrollOffset))
      return parent->onMouseWheelDown(event);

   return true;
}
#pragma endregion

#pragma region rendering
void GuiScrollCtrl::onPreRender()
{
   Parent::onPreRender();

   // Short circuit if not depressed to save cycles
   if( mDepressed != true )
      return;

   //default to one second, though it shouldn't be necessary
   U32 timeThreshold = 1000;

   // We don't want to scroll by pages at an interval the same as when we're scrolling
   // using the arrow buttons, so adjust accordingly.
   switch( curHitRegion )
   {
   case UpPage:
   case DownPage:
   case LeftPage:
   case RightPage:
      timeThreshold = 200;
      break;
   case UpArrow:
   case DownArrow:
   case LeftArrow:
   case RightArrow:
      timeThreshold = 20;
      break;
   default:
      // Neither a button or a page, don't scroll (shouldn't get here)
      return;
      break;
   };

   S32 timeElapsed = Platform::getVirtualMilliseconds() - mLastUpdated;

   if ( ( timeElapsed > 0 ) && ( timeElapsed > (S32)timeThreshold ) )
   {

      mLastUpdated = Platform::getVirtualMilliseconds();
      scrollByRegion(curHitRegion);
   }

}

void GuiScrollCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI ctrlRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, NormalState);

	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyScrollBarSpacing(fillRect.point, fillRect.extent);
	mChildArea.set(contentRect.point, contentRect.extent);

	renderVScrollBar(offset);
	renderHScrollBar(offset);

	if (contentRect.isValidRect())
	{
		renderChildControls(offset, contentRect, updateRect);
	}
}

RectI GuiScrollCtrl::applyScrollBarSpacing(Point2I offset, Point2I extent)
{
	RectI contentRect = RectI(offset, extent);

	if (mHasVScrollBar && mHasHScrollBar)
	{
		contentRect.extent.x -= mScrollBarThickness;
		contentRect.extent.y -= mScrollBarThickness;
	}
	else if (mHasVScrollBar)
	{
		contentRect.extent.x -= mScrollBarThickness;
	}
	else if (mHasHScrollBar)
	{
		contentRect.extent.y -= mScrollBarThickness;
	}

	return contentRect;
}

GuiControlState GuiScrollCtrl::getRegionCurrentState(GuiScrollCtrl::Region region)
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

void GuiScrollCtrl::renderBorderedRectWithArrow(RectI& bounds, GuiControlProfile* profile, GuiControlState state, GuiDirection direction)
{
	if (!profile)
	{
		return;
	}

	renderUniversalRect(bounds, profile, state);

	RectI ctrlRect = applyMargins(bounds.point, bounds.extent, state, profile);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, state, profile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, state, profile);

	if (contentRect.isValidRect())
	{
		Point2I p1, p2, p3;
		switch (direction)
		{
			case GuiDirection::Up:
				p1 = Point2I(contentRect.point.x + (contentRect.extent.x / 2), contentRect.point.y);
				p2 = Point2I(contentRect.point.x, contentRect.point.y + contentRect.extent.y);
				p3 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + contentRect.extent.y);
				break;
			case GuiDirection::Down:
				p1 = Point2I(contentRect.point.x, contentRect.point.y);
				p2 = Point2I(contentRect.point.x + (contentRect.extent.x / 2), contentRect.point.y + contentRect.extent.y);
				p3 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y);
				break;
			case GuiDirection::Left:
				p1 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y);
				p2 = Point2I(contentRect.point.x, contentRect.point.y + (contentRect.extent.y/2));
				p3 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + contentRect.extent.y);
				break;
			case GuiDirection::Right:
				p1 = Point2I(contentRect.point.x, contentRect.point.y);
				p2 = Point2I(contentRect.point.x, contentRect.point.y + contentRect.extent.y);
				p3 = Point2I(contentRect.point.x + contentRect.extent.x, contentRect.point.y + (contentRect.extent.y/2));
				break;
		}
		
		dglDrawTriangleFill(p1, p2, p3, profile->getFontColor(state));
	}
}

void GuiScrollCtrl::renderVScrollBar(const Point2I& offset)
{
	if(mHasVScrollBar && mTrackProfile && mThumbProfile)
	{ 
		if(mVBarEnabled)
		{
			if (mShowArrowButtons && mArrowProfile)
			{
				renderBorderedRectWithArrow(RectI(mUpArrowRect.point + offset, mUpArrowRect.extent), mArrowProfile, getRegionCurrentState(Region::UpArrow), GuiDirection::Up);
				renderBorderedRectWithArrow(RectI(mDownArrowRect.point + offset, mDownArrowRect.extent), mArrowProfile, getRegionCurrentState(Region::DownArrow), GuiDirection::Down);
			}
			renderUniversalRect(RectI(mVTrackRect.point + offset, mVTrackRect.extent), mTrackProfile, GuiControlState::NormalState);

			//The Thumb
			GuiControlState thumbState = getRegionCurrentState(Region::VertThumb);
			RectI vThumb = RectI(mVTrackRect.point.x + offset.x, mVTrackRect.point.y + mVThumbPos + offset.y, mScrollBarThickness, mVThumbSize);
			RectI vThumbWithMargins = applyMargins(vThumb.point, vThumb.extent, thumbState, mThumbProfile);
			renderUniversalRect(vThumbWithMargins, mThumbProfile, thumbState);
		}
		else
		{
			if (mShowArrowButtons && mArrowProfile)
			{
				renderBorderedRectWithArrow(RectI(mUpArrowRect.point + offset, mUpArrowRect.extent), mArrowProfile, GuiControlState::DisabledState, GuiDirection::Up);
				renderBorderedRectWithArrow(RectI(mDownArrowRect.point + offset, mDownArrowRect.extent), mArrowProfile, GuiControlState::DisabledState, GuiDirection::Down);
			}
			renderUniversalRect(RectI(mVTrackRect.point + offset, mVTrackRect.extent), mTrackProfile, GuiControlState::DisabledState);
		}
	}
}

void GuiScrollCtrl::renderHScrollBar(const Point2I& offset)
{
	if(mHasHScrollBar && mTrackProfile && mThumbProfile)
	{
		if (mHBarEnabled)
		{
			if (mShowArrowButtons && mArrowProfile)
			{
				renderBorderedRectWithArrow(RectI(mLeftArrowRect.point + offset, mLeftArrowRect.extent), mArrowProfile, getRegionCurrentState(Region::LeftArrow), GuiDirection::Left);
				renderBorderedRectWithArrow(RectI(mRightArrowRect.point + offset, mRightArrowRect.extent), mArrowProfile, getRegionCurrentState(Region::RightArrow), GuiDirection::Right);
			}
			renderUniversalRect(RectI(mHTrackRect.point + offset, mHTrackRect.extent), mTrackProfile, GuiControlState::NormalState);

			//The Thumb
			GuiControlState thumbState = getRegionCurrentState(Region::HorizThumb);
			RectI hThumb = RectI(mHTrackRect.point.x + mHThumbPos + offset.x, mHTrackRect.point.y + offset.y, mHThumbSize, mScrollBarThickness);
			RectI hThumbWithMargins = applyMargins(hThumb.point, hThumb.extent, thumbState, mThumbProfile);
			renderUniversalRect(hThumbWithMargins, mThumbProfile, thumbState);
		}
		else
		{
			if (mShowArrowButtons && mArrowProfile)
			{
				renderBorderedRectWithArrow(RectI(mLeftArrowRect.point + offset, mLeftArrowRect.extent), mArrowProfile, GuiControlState::DisabledState, GuiDirection::Left);
				renderBorderedRectWithArrow(RectI(mRightArrowRect.point + offset, mRightArrowRect.extent), mArrowProfile, GuiControlState::DisabledState, GuiDirection::Right);
			}
			renderUniversalRect(RectI(mHTrackRect.point + offset, mHTrackRect.extent), mTrackProfile, GuiControlState::DisabledState);
		}
	}
}

void GuiScrollCtrl::renderChildControls(Point2I offset, RectI content, const RectI& updateRect)
{
	// offset is the upper-left corner of this control in screen coordinates. It should almost always be the same offset passed into the onRender method.
	// updateRect is the area that this control was allowed to draw in. It should almost always be the same as the value in onRender.
	// content is the area that child controls are allowed to draw in.
	RectI clipRect = content;
	if (clipRect.intersect(dglGetClipRect()))
	{
		S32 size = objectList.size();
		S32 size_cpy = size;

		//Get the border profiles - padding is actually applied here...
		GuiBorderProfile* leftProfile = mProfile->getLeftBorder();
		GuiBorderProfile* topProfile = mProfile->getTopBorder();
		S32 leftSize = (leftProfile) ? leftProfile->getPadding(NormalState) : 0;
		S32 topSize = (topProfile) ? topProfile->getPadding(NormalState) : 0;
		Point2I ltPadding = Point2I(leftSize, topSize);

		//-Mat look through our vector all normal-like, trying to use an iterator sometimes gives us
	   //bad cast on good objects
		for (S32 count = 0; count < objectList.size(); count++)
		{
			GuiControl* ctrl = (GuiControl*)objectList[count];
			if (ctrl == NULL) {
				Con::errorf("GuiControl::renderChildControls() object %i is NULL", count);
				continue;
			}
			if (ctrl->mVisible)
			{
				ctrl->mRenderInsetLT = (ltPadding + content.point - offset) - mScrollOffset;
				ctrl->mRenderInsetRB = mBounds.extent - (ctrl->mRenderInsetLT + content.extent);
				Point2I childPosition = ltPadding + content.point + ctrl->getPosition() - mScrollOffset;
				RectI childClip(childPosition, ctrl->getExtent());

				if (childClip.intersect(clipRect))
				{
					RectI old = dglGetClipRect();
					dglSetClipRect(clipRect);
					glDisable(GL_CULL_FACE);
					ctrl->onRender(childPosition, RectI(childPosition, ctrl->getExtent()));
					dglSetClipRect(old);
				}
			}
			size_cpy = objectList.size(); //	CHRIS: i know its wierd but the size of the list changes sometimes during execution of this loop
			if (size != size_cpy)
			{
				size = size_cpy;
				count--;	//	CHRIS: just to make sure one wasnt skipped.
			}
		}
	}
}
#pragma endregion

