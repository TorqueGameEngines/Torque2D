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
   mScrollBarThickness = 14;
   mScrollBarDragTolerance = 130;
   mDepressed = false;
   curHitRegion = Content;
   mActive = true;
   mShowArrowButtons = true;
   mBaseThumbSize = (mScrollBarThickness * 2);

   mUseConstantHeightThumb = false;

   mForceVScrollBar = ScrollBarAlwaysOn;
   mForceHScrollBar = ScrollBarAlwaysOn;

   // What computeSizes decides for the modes above. resize reads these before
   // it has called computeSizes even once.
   mHasHScrollBar = true;
   mHasVScrollBar = true;
   mHBarEnabled = false;
   mVBarEnabled = false;

   mThumbProfile = NULL;
   mTrackProfile = NULL;
   mArrowProfile = NULL;

   mScrollOffset.set(0, 0);
   mContentExt.set(200,200);

   mHorizSizing = horizResizeFill;
   mVertSizing = vertResizeFill;

   setField("thumbProfile", "GuiScrollThumbProfile");
   setField("arrowProfile", "GuiScrollArrowProfile");
   setField("trackProfile", "GuiScrollTrackProfile");
   setField("profile", "GuiScrollProfile");

   mEventBubbled = false;
   mCalcGuard = false;
   mResizeGuard = false;
   mNotifyGuard = false;
}

void GuiScrollCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("GuiScrollCtrl");
   addField("hScrollBar",           TypeEnum,    Offset(mForceHScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("vScrollBar",           TypeEnum,    Offset(mForceVScrollBar, GuiScrollCtrl), 1, &gScrollBarTable);
   addField("constantThumbHeight",  TypeBool,    Offset(mUseConstantHeightThumb, GuiScrollCtrl));
   addField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiScrollCtrl));
   addField("showArrowButtons",		TypeBool,	 Offset(mShowArrowButtons, GuiScrollCtrl));
   addField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiScrollCtrl));
   addField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiScrollCtrl));
   addField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiScrollCtrl));
   endGroup("GuiScrollCtrl");
}

void GuiScrollCtrl::resize(const Point2I &newPos, const Point2I &newExt)
{
	if(!mResizeGuard)
	{
		mResizeGuard = true;
		bool hasH = mHasHScrollBar;
		bool hasV = mHasVScrollBar;
		mCalcGuard = true;
		Parent::resize(newPos, newExt);
		mCalcGuard = false;
		computeSizes();

		// The bar appearing or going away is now announced by computeSizes, which
		// is where it is actually decided -- and which catches the far more
		// common case of a bar arriving because a child grew, nowhere near a
		// resize of this control. All that is left here is to settle the
		// rectangles against the size the children have just been given.
		//
		// What stood here instead adjusted each child's mRenderInsetRB and then
		// called parentResized with the same extent for old AND new, so every
		// sizing mode that works from a delta computed zero and did nothing; the
		// inset it wrote was overwritten by the next renderChild anyway.
		if (hasH != mHasHScrollBar || hasV != mHasVScrollBar)
		{
			mCalcGuard = true;
			Parent::resize(newPos, newExt);
			mCalcGuard = false;
			computeSizes();
		}
		mResizeGuard = false;
	}
}

void GuiScrollCtrl::childResized(GuiControl *child)
{
   Parent::childResized(child);
   computeSizes();
}

RectI GuiScrollCtrl::getInnerRect(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile)
{
	// The margins, borders and padding first, then whatever the bars are using.
	// A child asking this control how big it is has to be told what it can
	// actually see, or it lays its last column out underneath the bar.
	RectI inner = Parent::getInnerRect(offset, extent, currentState, profile);
	inner.extent = subtractScrollBars(inner.extent, mHasHScrollBar, mHasVScrollBar, mScrollBarThickness);

	return inner;
}

void GuiScrollCtrl::preventUnsizedModes(GuiControl *child)
{
	// Fill and center both mean "put me where the parent's size says", and in an
	// axis this control can scroll there is no such size: the content is as long
	// as it wants to be and this is a window onto it. Filling there would clamp
	// the content to what is visible and leave nothing to scroll.
	//
	// An axis whose bar is alwaysOff is a different thing entirely. Nothing
	// scrolls across it, the room is exactly the inner rect less any bar on the
	// other axis, and fill is the honest way for a child to ask for it. Refusing
	// it there is what forced callers to compute widths in script.
	if (canScrollHorizontally())
	{
		child->preventHorizResizeModeFill();
		child->preventHorizResizeModeCenter();
	}

	if (canScrollVertically())
	{
		child->preventVertResizeModeFill();
		child->preventVertResizeModeCenter();
	}
}

void GuiScrollCtrl::addObject(SimObject* object)
{
	GuiControl* child = dynamic_cast<GuiControl*>(object);
	if (child)
	{
		preventUnsizedModes(child);
	}
	Parent::addObject(object);
	computeSizes();

	// A child that fills wants its size now rather than at whatever resize
	// happens to come next -- the same reason GuiControl::applySizing exists.
	if (child)
	{
		child->parentResized(mContentExt, mContentExt);
	}
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

void GuiScrollCtrl::inspectPostApply()
{
	Parent::inspectPostApply();
	computeSizes();
}

void GuiScrollCtrl::onStaticModified(const char* slotName, const char* newValue)
{
	Parent::onStaticModified(slotName, newValue);

	// Which bars are showing is decided by computeSizes and kept, and the room a
	// child is offered is read from what was kept. These are the fields that
	// decision is made from, and writing one used to redo nothing. A new{} block
	// that set Extent before the bar modes had computeSizes run -- through the
	// Extent setter -- while both were still the constructor's alwaysOn, and a
	// child added afterwards measured itself against two bars that did not
	// exist. A scaled child records that proportion once, so it then grew by it
	// for good: 1290x730 in a 1280x720 scroller with 10px bars.
	if (dStricmp(slotName, "hScrollBar") == 0 || dStricmp(slotName, "vScrollBar") == 0 ||
		dStricmp(slotName, "scrollBarThickness") == 0)
	{
		computeSizes();
	}
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
			else if (ctrl->mVisible && ctrl->pointInControl(pt - ctrl->mRenderInsetLT) && ctrl->mUseInput)
			{
				Point2I ptemp = pt - (ctrl->mBounds.point + ctrl->mRenderInsetLT);
				GuiControl* hitCtrl = ctrl->findHitControl(ptemp);

				if (hitCtrl->mUseInput)
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
	return Content;
}

#pragma region CalculationFunctions
Point2I GuiScrollCtrl::subtractScrollBars(const Point2I &extent, const bool hasHBar, const bool hasVBar, const S32 barThickness)
{
	// A vertical bar stands down the side and so costs WIDTH; a horizontal one
	// costs height. Getting that pair the wrong way round is the easiest
	// mistake here, which is most of why this is one function and not four.
	return Point2I(extent.x - (hasVBar ? barThickness : 0),
		extent.y - (hasHBar ? barThickness : 0));
}

void GuiScrollCtrl::calcBarPresence(const S32 forceHBar, const S32 forceVBar, const Point2I &childExtent,
	const Point2I &contentExtent, const S32 barThickness, bool &outHasHBar, bool &outHasVBar)
{
	outHasHBar = (forceHBar == ScrollBarAlwaysOn);
	outHasVBar = (forceVBar == ScrollBarAlwaysOn);

	// Every test below is against the room that actually REMAINS, which is what
	// lets one bar call the other into being. The old code compared both against
	// the un-narrowed extent, so its second look at the horizontal bar asked a
	// question it had already answered and could never say yes.
	Point2I room = subtractScrollBars(contentExtent, outHasHBar, outHasVBar, barThickness);

	if (!outHasHBar && forceHBar == ScrollBarDynamic && childExtent.x > room.x)
	{
		outHasHBar = true;
		room.y -= barThickness;
	}

	if (!outHasVBar && forceVBar == ScrollBarDynamic && childExtent.y > room.y)
	{
		outHasVBar = true;
		room.x -= barThickness;

		// The vertical bar just narrowed the content, and that can be what
		// pushes it wide enough to need a horizontal one after all.
		if (!outHasHBar && forceHBar == ScrollBarDynamic && childExtent.x > room.x)
		{
			outHasHBar = true;
		}
	}
}

void GuiScrollCtrl::computeSizes()
{
	if (!mCalcGuard)//Prevent needless calcuations
	{
		const bool hadHBar = mHasHScrollBar;
		const bool hadVBar = mHasVScrollBar;

		calcContentExtents();

		// What there would be with no bars at all. Kept because the bars are
		// decided from it and then taken off it, and because a child that has to
		// be told the room changed needs both answers.
		const Point2I barFreeExtent = mContentExt;

		mHBarEnabled = false;
		mVBarEnabled = false;
		mHasVScrollBar = (mForceVScrollBar == ScrollBarAlwaysOn);
		mHasHScrollBar = (mForceHScrollBar == ScrollBarAlwaysOn);

		setUpdate();

		const bool hasChildren = calcChildExtents();
		if (hasChildren)
		{
			calcBarPresence(mForceHScrollBar, mForceVScrollBar, mChildExt, barFreeExtent,
				mScrollBarThickness, mHasHScrollBar, mHasVScrollBar);
		}

		// Outside the children test, unlike before: a bar forced alwaysOn takes
		// its space whether or not anything has been put in the control yet, and
		// a scroller that reported its whole width until its first child arrived
		// would hand that width to the child.
		mContentExt = subtractScrollBars(barFreeExtent, mHasHScrollBar, mHasVScrollBar, mScrollBarThickness);

		if (hasChildren)
		{
			// enable needed scroll bars
			if (mChildExt.x > mContentExt.x)
				mHBarEnabled = true;
			if (mChildExt.y > mContentExt.y)
				mVBarEnabled = true;

			//Are we now over-scrolled?
			calcScrollOffset();
		}

		// The bars appearing is a change in how much room the children have, and
		// this is the only place that knows it happened -- most of the time the
		// bar arrives from childResized, nowhere near a resize of this control.
		if (hadHBar != mHasHScrollBar || hadVBar != mHasVScrollBar)
		{
			notifyChildrenOfBarChange(barFreeExtent, hadHBar, hadVBar);
		}

		// build all the rectangles and such...
		Point2I zero = mBounds.point.Zero;
		RectI ctrlRect = applyMargins(zero, mBounds.extent, NormalState, mProfile);
		RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
		calcScrollRects(fillRect);
		calcThumbs();
	}
}

void GuiScrollCtrl::notifyChildrenOfBarChange(const Point2I &barFreeExtent, const bool hadHBar, const bool hadVBar)
{
	// Resizing a child calls back into childResized and so into here again. The
	// re-entered pass does its own arithmetic and settles; what it must not do
	// is announce this same change a second time.
	if (mNotifyGuard)
	{
		return;
	}
	mNotifyGuard = true;

	// Both extents measured from the SAME bar-free rect, so the difference
	// between them is the bars and nothing else. Any change to the control's own
	// size has already been passed down by GuiControl::resize.
	const Point2I before = subtractScrollBars(barFreeExtent, hadHBar, hadVBar, mScrollBarThickness);
	const Point2I after = subtractScrollBars(barFreeExtent, mHasHScrollBar, mHasVScrollBar, mScrollBarThickness);

	for (iterator i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		ctrl->parentResized(before, after);
	}

	mNotifyGuard = false;
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

void GuiScrollCtrl::calcScrollOffset()
{
	if ((mScrollOffset.x + mContentExt.x) > mChildExt.x)
		mScrollOffset.x = getMax(mChildExt.x - mContentExt.x, 0);
	if ((mScrollOffset.y + mContentExt.y) > mChildExt.y)
		mScrollOffset.y = getMax(mChildExt.y - mContentExt.y, 0);
}

void GuiScrollCtrl::calcScrollRects(RectI &fillRect)
{
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
			else if(mChildExt.x > 0)
				mHThumbSize = getMax(mBaseThumbSize, S32((mContentExt.x * trackSize) / mChildExt.x));
			else 
				mHThumbSize = mBaseThumbSize;

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
			else if(mChildExt.y > 0)
				mVThumbSize = getMax(mBaseThumbSize, S32((mContentExt.y * trackSize) / mChildExt.y));
			else 
				mVThumbSize = mBaseThumbSize;

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
		case Content:
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
		case Content:
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
	Point2I visLowerRight = mContentExt + mScrollOffset;
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

	GuiControl* parent = getParent();
	if (parent)
		parent->onTouchMove(event);
}

void GuiScrollCtrl::onTouchLeave(const GuiEvent &event)
{
	if (!mDepressed)
	{
		curHitRegion = Content;
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
   mEventBubbled = false;

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
   else if (curHitRegion == Content)
   {
	   GuiControl* parent = getParent();
	   if (parent)
	   {
		   parent->onTouchDown(event);
		   mEventBubbled = true;
	   }
   }
}

void GuiScrollCtrl::onTouchUp(const GuiEvent &event)
{
   mouseUnlock();

   setUpdate();

   if (mEventBubbled)
   {
	   GuiControl* parent = getParent();
	   if (parent)
	   {
		   parent->onTouchUp(event);
	   }
		mEventBubbled = false;
   }

   curHitRegion = Content;
   mDepressed = false;
}

void GuiScrollCtrl::onTouchDragged(const GuiEvent &event)
{
   Point2I curMousePos = globalToLocalCoord(event.mousePoint);
   setUpdate();

   if (mEventBubbled)
   {
	   GuiControl* parent = getParent();
	   if (parent)
	   {
		   parent->onTouchDragged(event);
		   return;
	   }
   }

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

void GuiScrollCtrl::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return;

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
      parent->onMouseWheelUp(event);
}

void GuiScrollCtrl::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return;

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
      parent->onMouseWheelDown(event);
}

bool GuiScrollCtrl::onMouseDownEditor(const GuiEvent& event, const Point2I& offset)
{
	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	Region hitRegion = findHitRegion(curMousePos);

	if (hitRegion != Region::Content)
	{
		onTouchDown(event);
		return true;
	}
	return false;
}

bool GuiScrollCtrl::onMouseUpEditor(const GuiEvent& event, const Point2I& offset)
{
	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	Region hitRegion = findHitRegion(curMousePos);

	if (hitRegion != Region::Content || mDepressed)
	{
		onTouchUp(event);
		return true;
	}
	return false;
}

bool GuiScrollCtrl::onMouseDraggedEditor(const GuiEvent& event, const Point2I& offset)
{
	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	Region hitRegion = findHitRegion(curMousePos);

	if (hitRegion != Region::Content || mDepressed)
	{
		onTouchDragged(event);
		return true;
	}
	return false;
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

	// The same rect the children were SIZED against -- getInnerRect is now the
	// one definition of it. Two separate subtractions were how the visible area
	// and the laid-out area came to disagree in the first place.
	RectI contentRect = getInnerRect(offset, mBounds.extent, NormalState, mProfile);
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
	// Kept for anything overriding or calling it, but no longer a second copy of
	// the arithmetic: getInnerRect and this both go through subtractScrollBars,
	// so they cannot come to disagree about what a bar costs.
	return RectI(offset, subtractScrollBars(extent, mHasHScrollBar, mHasVScrollBar, mScrollBarThickness));
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
				RectI upArrowExtent = RectI(mUpArrowRect.point + offset, mUpArrowRect.extent);
				renderBorderedRectWithArrow(upArrowExtent, mArrowProfile, getRegionCurrentState(Region::UpArrow), GuiDirection::Up);

				RectI downArrowExtent = RectI(mDownArrowRect.point + offset, mDownArrowRect.extent);
				renderBorderedRectWithArrow(downArrowExtent, mArrowProfile, getRegionCurrentState(Region::DownArrow), GuiDirection::Down);
			}
			RectI mVTrackExtent = RectI(mVTrackRect.point + offset, mVTrackRect.extent);
			renderUniversalRect(mVTrackExtent, mTrackProfile, GuiControlState::NormalState);

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
				RectI upArrowExtent = RectI(mUpArrowRect.point + offset, mUpArrowRect.extent);
				renderBorderedRectWithArrow(upArrowExtent, mArrowProfile, GuiControlState::DisabledState, GuiDirection::Up);
				RectI downArrowExtent = RectI(mDownArrowRect.point + offset, mDownArrowRect.extent);
				renderBorderedRectWithArrow(downArrowExtent, mArrowProfile, GuiControlState::DisabledState, GuiDirection::Down);
			}
			RectI mVTrackExtent = RectI(mVTrackRect.point + offset, mVTrackRect.extent);
			renderUniversalRect(mVTrackExtent, mTrackProfile, GuiControlState::DisabledState);
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
				RectI leftArrowBounds = RectI(mLeftArrowRect.point + offset, mLeftArrowRect.extent);
				renderBorderedRectWithArrow(leftArrowBounds, mArrowProfile, getRegionCurrentState(Region::LeftArrow), GuiDirection::Left);
				RectI rightArrowBounds = RectI(mRightArrowRect.point + offset, mRightArrowRect.extent);
				renderBorderedRectWithArrow(rightArrowBounds, mArrowProfile, getRegionCurrentState(Region::RightArrow), GuiDirection::Right);
			}
			RectI hTrackBounds = RectI(mHTrackRect.point + offset, mHTrackRect.extent);
			renderUniversalRect(hTrackBounds, mTrackProfile, GuiControlState::NormalState);

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
				RectI leftArrowBounds = RectI(mLeftArrowRect.point + offset, mLeftArrowRect.extent);
				renderBorderedRectWithArrow(leftArrowBounds, mArrowProfile, GuiControlState::DisabledState, GuiDirection::Left);
				RectI rightArrowBounds = RectI(mRightArrowRect.point + offset, mRightArrowRect.extent);
				renderBorderedRectWithArrow(rightArrowBounds, mArrowProfile, GuiControlState::DisabledState, GuiDirection::Right);
			}
			RectI hTrackBounds = RectI(mHTrackRect.point + offset, mHTrackRect.extent);
			renderUniversalRect(hTrackBounds, mTrackProfile, GuiControlState::DisabledState);
		}
	}
}

void GuiScrollCtrl::renderChildControls(const Point2I& offset, const RectI& content, const RectI& updateRect)
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

