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
#include "gui/containers/guiSceneScrollCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiCanvas.h"

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

GuiSceneScrollCtrl::GuiSceneScrollCtrl(SceneWindow* window) : GuiScrollCtrl()
{
   mWindow = window;
}

void GuiSceneScrollCtrl::initPersistFields()
{
	//Not a con object...
}

bool GuiSceneScrollCtrl::onWake()
{
	if (mAwake)
		return false;

	if (mProfile != NULL)
		mProfile->incRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

	mAwake = true;

	return true;
}

void GuiSceneScrollCtrl::onSleep()
{
	if (!mAwake)
		return;

	if (mProfile != NULL)
		mProfile->decRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->decRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->decRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->decRefCount();

	mAwake = false;
}

#pragma region CalculationFunctions
void GuiSceneScrollCtrl::calcContentExtents()
{
	Vector2 v = mWindow->getCameraSize();
	mContentExt = Point2I(mCeil(v.x), mCeil(v.y));

	F32 zoom = getMax(mWindow->getCameraZoom(), 0.0000001f);
	mContentExt.x /= zoom;
	mContentExt.y /= zoom;

	//This will be removed later so we are countering it now.
	if (mHasVScrollBar)
		mContentExt.x += mScrollBarThickness;
	if (mHasHScrollBar)
		mContentExt.y += mScrollBarThickness;
}

bool GuiSceneScrollCtrl::calcChildExtents()
{
	if (!mWindow->isViewLimitOn())
		return false;

	Vector2 v = mWindow->getViewLimitMax() - mWindow->getViewLimitMin();
	mChildExt = Point2I(mFloor(v.x), mFloor(v.y));
	return true;
}

void GuiSceneScrollCtrl::calcScrollOffset()
{
	//Set the offset based on the position of the visible area of the window
	RectF cameraArea = mWindow->getCameraArea();
	F32 zoom = getMax(mWindow->getCameraZoom(), 0.0000001f);
	Point2F actualExtent = Point2F(cameraArea.extent);
	cameraArea.extent.x /= zoom;
	cameraArea.extent.y /= zoom;
	cameraArea.point.x += ((actualExtent.x - cameraArea.extent.x) / 2);
	cameraArea.point.y += ((actualExtent.y - cameraArea.extent.y) / 2);

	Vector2 min = mWindow->getViewLimitMin();
	Vector2 max = mWindow->getViewLimitMax();
	mScrollOffset.x = mRound(cameraArea.point.x - min.x);
	mScrollOffset.y = mRound(max.y - (cameraArea.point.y + cameraArea.extent.y));
}

void GuiSceneScrollCtrl::calcScrollRects(RectI &fillRect)
{
	RectI rect = RectI(Point2I(0,0), mWindow->mBounds.extent);
	Parent::calcScrollRects(rect);
}
#pragma endregion

#pragma region ScrollingFunctions
void GuiSceneScrollCtrl::scrollDelta(S32 deltaX, S32 deltaY)
{
	Vector2 pos = mWindow->getCameraPosition();
	Vector2 size = mWindow->getCameraSize() / mWindow->getCameraZoom();
	pos.x += deltaX;
	pos.y -= deltaY;

	Vector2 min = mWindow->getViewLimitMin();
	Vector2 max = mWindow->getViewLimitMax();

	pos.x = getMax(pos.x, min.x + (size.x / 2));
	pos.y = getMax(pos.y, min.y + (size.y / 2));

	pos.x = getMin(pos.x, max.x - (size.x / 2));
	pos.y = getMin(pos.y, max.y - (size.y / 2));

	mWindow->setTargetCameraPosition(pos);
	mWindow->startCameraMove(0.2);
}

void GuiSceneScrollCtrl::scrollTo(S32 x, S32 y)
{
	Vector2 pos = mWindow->getCameraPosition();
	Vector2 size = mWindow->getCameraSize() / mWindow->getCameraZoom();

	Vector2 min = mWindow->getViewLimitMin();
	Vector2 max = mWindow->getViewLimitMax();

	if (x != mScrollOffset.x)
	{
		F32 halfW = size.x / 2;
		pos.x = getMin(getMax(x + halfW + min.x, min.x + halfW), max.x - halfW);
	}

	if (y != mScrollOffset.y)
	{
		F32 halfH = size.y / 2;
		pos.y = getMin(getMax(max.y - (y + halfH), min.y + halfH), max.y - halfH);
	}

	mWindow->setCameraPosition(pos);
}

void GuiSceneScrollCtrl::scrollByRegion(Region reg)
{
	Vector2 v = mWindow->getCameraSize() / mWindow->getCameraZoom();
	U32 pageWidth = mCeil(v.x * 0.8);
	U32 pageHeight = mCeil(v.y * 0.8);
	U32 columnWidth = mCeil(v.x * 0.1);
	U32 rowHeight = mCeil(v.y * 0.1);

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
			//Con::errorf("Unhandled case in GuiSceneScrollCtrl::scrollByRegion");
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
			//Con::errorf("Unhandled case in GuiSceneScrollCtrl::scrollByRegion");
			break;
		}
	}
}

void GuiSceneScrollCtrl::scrollRectVisible(RectI rect)
{
	//We don't do this...
}
#pragma endregion

#pragma region Event_Processing
Point2I GuiSceneScrollCtrl::localToGlobalCoord(const Point2I &src)
{
	return mWindow->localToGlobalCoord(src);
}

Point2I GuiSceneScrollCtrl::globalToLocalCoord(const Point2I &src)
{
	return mWindow->globalToLocalCoord(src);
}

void GuiSceneScrollCtrl::onTouchDown(const GuiEvent &event)
{
	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	mDepressed = true;
	mouseLock();

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

void GuiSceneScrollCtrl::onTouchDragged(const GuiEvent &event)
{
	Point2I curMousePos = globalToLocalCoord(event.mousePoint);
	mWindow->setUpdate();

	if ((curHitRegion != VertThumb) && (curHitRegion != HorizThumb))
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
			if (newVThumbPos != mVThumbPos)
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
			if (newHThumbPos != mHThumbPos)
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

bool GuiSceneScrollCtrl::isMouseLocked()
{
	GuiCanvas *root = mWindow->getRoot();
	return root ? root->getMouseLockedControl() == this : false;
}

void GuiSceneScrollCtrl::mouseLock(GuiControl *lockingControl)
{
	GuiCanvas *root = mWindow->getRoot();
	if (root)
		root->mouseLock(lockingControl);
}

void GuiSceneScrollCtrl::mouseLock()
{
	GuiCanvas *root = mWindow->getRoot();
	if (root)
		root->mouseLock(this);
}

void GuiSceneScrollCtrl::mouseUnlock()
{
	GuiCanvas *root = mWindow->getRoot();
	if (root)
		root->mouseUnlock(this);
}
#pragma endregion

#pragma region rendering
void GuiSceneScrollCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI contentRect = applyScrollBarSpacing(offset, mBounds.extent);
	mChildArea.set(contentRect.point, contentRect.extent);

	renderVScrollBar(offset);
	renderHScrollBar(offset);
}
#pragma endregion

