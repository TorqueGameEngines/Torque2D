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
#include "gui/containers/guiFrameSetCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiCanvas.h"
#include "gui/containers/guiWindowCtrl.h"
#include "graphics/dgl.h"
#include "gui/containers/guiTabBookCtrl.h"
#include "gui/containers/guiTabPageCtrl.h"

#include "guiFrameSetCtrl_ScriptBinding.h"

void GuiFrameSetCtrl::Frame::deleteChildren()
{
	if (child1)
	{
		child1->deleteChildren();
		delete child1;
	}
	if (child2)
	{
		child2->deleteChildren();
		delete child2;
	}
}

void GuiFrameSetCtrl::Frame::resize(const Point2I& newPosition, const Point2I& newExtent)
{
	const S32 minSize = owner->minSize;
	extent.set(newExtent.x, newExtent.y);
	localPosition.set(newPosition.x, newPosition.y);
	sizeInsertButtons(newPosition, newExtent);
	if (control)
	{
		if (control->mMinExtent.x > minSize || control->mMinExtent.y > minSize)
		{
			control->mMinExtent.set(minSize, minSize);
		}
		control->resize(newPosition, newExtent);
	}
	else if (child1 && child2)
	{
		S32 spaceX = isVertical ? newExtent.x : newExtent.x - owner->mDividerThickness;
		S32 spaceY = !isVertical ? newExtent.y : newExtent.y - owner->mDividerThickness;

		S32 x1 = newExtent.x;
		S32 x2 = newExtent.x;
		if(!isVertical)
		{	
			if (child1->isAnchored)
			{
				x1 = getMin(child1->extent.x, spaceX - minSize);
				x2 = spaceX - x1;
			}
			else
			{
				x2 = getMin(child2->extent.x, spaceX - minSize);
				x1 = spaceX - x2;
			}
		}

		S32 y1 = newExtent.y;
		S32 y2 = newExtent.y;
		if (isVertical)
		{
			if (child1->isAnchored)
			{
				y1 = getMin(child1->extent.y, spaceY - minSize);
				y2 = spaceY - y1;
			}
			else
			{
				y2 = getMin(child2->extent.y, spaceY - minSize);
				y1 = spaceY - y2;
			}
		}

		Point2I ext1 = Point2I(x1, y1);
		Point2I ext2 = Point2I(x2, y2);
		Point2I pos2 = isVertical ? Point2I(newPosition.x, newPosition.y + y1 + owner->mDividerThickness) : Point2I(newPosition.x + x1 + owner->mDividerThickness, newPosition.y);

		if(isVertical)
		{
			dividerRect.set(newPosition.x, newPosition.y + y1, x1, owner->mDividerThickness);
		}
		else
		{
			dividerRect.set(newPosition.x + x1, newPosition.y, owner->mDividerThickness, y1);
		}

		child1->resize(newPosition, ext1);
		child2->resize(pos2, ext2);
	}
}

void GuiFrameSetCtrl::Frame::sizeInsertButtons(const Point2I& newPosition, const Point2I& newExtent)
{
	const U32 size = 40;
	const U32 gutter = 10;

	hasLeftRightButtons = false;
	hasTopBottomButtons = false;

	if (newExtent.x > (4 * (size + gutter)))
	{
		hasLeftRightButtons = true;
		mLeftButtonRect = RectI(newPosition.x + gutter, newPosition.y + ((newExtent.y - size) / 2), size, size);
		mRightButtonRect = RectI(newPosition.x + newExtent.x - (gutter + size), newPosition.y + ((newExtent.y - size) / 2), size, size);
	}

	if (newExtent.y > (4 * (size + gutter)))
	{
		hasTopBottomButtons = true;
		mTopButtonRect = RectI(newPosition.x + ((newExtent.x - size) / 2), newPosition.y + gutter, size, size);
		mBottomButtonRect = RectI(newPosition.x + ((newExtent.x - size) / 2), newPosition.y + newExtent.y - (gutter + size), size, size);
	}

	mCenterButtonRect = RectI(newPosition.x + ((newExtent.x - size) / 2), newPosition.y + ((newExtent.y - size) / 2), size, size);
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findFrame(const S32 frameID)
{
	if (id == frameID)
	{
		return this;
	}

	if (child1 && child2)
	{
		Frame* attempt = child1->findFrame(frameID);
		return attempt ? attempt : child2->findFrame(frameID);
	}
	return nullptr;
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findEmptyFrame()
{
	if (!control && !child1 && !child2)
	{
		return this;
	}
	else if (child1 && child2)
	{
		Frame* attempt = child1->findEmptyFrame();
		return attempt ? attempt : child2->findEmptyFrame();
	}
	return nullptr;
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::twin()
{
	if (parent)
	{
		if (parent->child1 == this)
		{
			return parent->child2;
		}
		else
		{
			return parent->child1;
		}
	}
	return this;
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findHitDivider(const Point2I& position)
{
	S32 x = position.x;
	S32 y = position.y;

	if (child1 && child2)
	{
		if (x >= dividerRect.point.x && x <= (dividerRect.point.x + dividerRect.extent.x) && 
			y >= dividerRect.point.y && y <= (dividerRect.point.y + dividerRect.extent.y))
		{
			return this;
		}
		Frame* attempt = child1->findHitDivider(position);
		return attempt ? attempt : child2->findHitDivider(position);
	}
	return nullptr;
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findFrameWithCtrl(GuiControl* ctrl)
{
	if (ctrl == control)
	{
		return this;
	}
	else if(child1 && child2)
	{
		Frame* attempt = child1->findFrameWithCtrl(ctrl);
		return attempt ? attempt : child2->findFrameWithCtrl(ctrl);
	}
	return nullptr;
}

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findFrameWithPoint(const Point2I& point)
{
	//Point is local to the frame.
	if (control)
	{
		return this;
	}
	if (child1 && child2)
	{
		S32 x = point.x;
		S32 y = point.y;

		Point2I pos2 = isVertical ? Point2I(x, child1->extent.y + owner->mDividerThickness) : Point2I(child1->extent.x + owner->mDividerThickness, y);


		if ((isVertical && y < child1->extent.y) || (!isVertical && x < child1->extent.x))
		{
			return child1->findFrameWithPoint(point);
		}
		else if ((isVertical && y > pos2.y && y < (pos2.y + child2->extent.y)) || (!isVertical && x > pos2.x && x < (pos2.x + child2->extent.x)))
		{
			Point2I pt = Point2I(x - pos2.x, y - pos2.y);
			return child2->findFrameWithPoint(pt);
		}
	}
	return nullptr;//This will happen if the mouse is over a divider.
}


//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiFrameSetCtrl);

//------------------------------------------------------------------------------

GuiFrameSetCtrl::GuiFrameSetCtrl()
{
	mIsContainer = true;
	setField("profile", "GuiDefaultProfile");

	mRootFrame = Frame(this, nullptr);
	mHitDivider = nullptr;
	mDividerThickness = 8;
	mNextFrameID = 1;
	mResizeGuard = false;
	mDepressed = false;
	mActive = true;
	mFrameDragAnchor = 0;

	mEaseFillColorHL = EasingFunction::EaseInOut;
	mEaseFillColorSL = EasingFunction::EaseOut;
	mEaseTimeFillColorHL = 500;
	mEaseTimeFillColorSL = 1000;

	mDropButtonProfile = NULL;
	setField("dropButtonProfile", "GuiButtonProfile");

	mTabBookProfile = NULL;
	mTabProfile = NULL;
	mTabPageProfile = NULL;
	setField("tabBookProfile", "GuiDefaultProfile");
	setField("tabProfile", "GuiTabProfile");
	setField("tabPageProfile", "GuiDefaultProfile");

	mLeftRightCursor = NULL;
	mUpDownCursor = NULL;
}

GuiFrameSetCtrl::~GuiFrameSetCtrl()
{
	mRootFrame.deleteChildren();
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("DividerThickness", TypeS32, Offset(mDividerThickness, GuiFrameSetCtrl));
	addField("dropButtonProfile", TypeGuiProfile, Offset(mDropButtonProfile, GuiFrameSetCtrl));
	addField("leftRightCursor", TypeGuiCursor, Offset(mLeftRightCursor, GuiFrameSetCtrl));
	addField("upDownCursor", TypeGuiCursor, Offset(mUpDownCursor, GuiFrameSetCtrl));
	addProtectedField("TabBookProfile", TypeGuiProfile, Offset(mTabBookProfile, GuiFrameSetCtrl), &setBookProfileFn, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
	addProtectedField("TabProfile", TypeGuiProfile, Offset(mTabProfile, GuiFrameSetCtrl), &setTabProfileFn, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
	addProtectedField("TabPageProfile", TypeGuiProfile, Offset(mTabPageProfile, GuiFrameSetCtrl), &setPageProfileFn, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
}

//------------------------------------------------------------------------------

bool GuiFrameSetCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mDropButtonProfile != NULL)
		mDropButtonProfile->incRefCount();

	if (mTabBookProfile != NULL)
		mTabBookProfile->incRefCount();

	if (mTabProfile != NULL)
		mTabProfile->incRefCount();

	if (mTabPageProfile != NULL)
		mTabPageProfile->incRefCount();

	return true;
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::onSleep()
{
	Parent::onSleep();

	if (mDropButtonProfile != NULL)
		mDropButtonProfile->decRefCount();

	if (mTabBookProfile != NULL)
		mTabBookProfile->decRefCount();

	if (mTabProfile != NULL)
		mTabProfile->decRefCount();

	if (mTabPageProfile != NULL)
		mTabPageProfile->decRefCount();
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::inspectPostApply()
{
	resize(getPosition(), getExtent());
	Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::resize(const Point2I& newPosition, const Point2I& newExtent)
{
	if(!mResizeGuard)//Prevent circular resizing
	{
		mResizeGuard = true;
		Point2I actualNewExtent = Point2I(getMax(mMinExtent.x, newExtent.x),
			getMax(mMinExtent.y, newExtent.y));

		//call set update both before and after
		setUpdate();
	
		Point2I origin = Point2I::Zero;
		mRootFrame.resize(origin, actualNewExtent);
		mBounds.set(newPosition, actualNewExtent);

		GuiControl* parent = getParent();
		if (parent)
			parent->childResized(this);
		setUpdate();
		mResizeGuard = false;
	}
}

bool GuiFrameSetCtrl::onAdd()
{
	// Let Parent Do Work.
	if (!Parent::onAdd())
		return false;

	// Always expand to fill the parent (we just ignore the points passed in).
	parentResized(Point2I(), Point2I());

	// Return Success.
	return true;
}

void GuiFrameSetCtrl::parentResized(const Point2I& oldParentExtent, const Point2I& newParentExtent)
{
	//In the case of centering, we want to make doubly sure we are using the inner rect.
	GuiControl* parent = getParent();
	if(parent)
	{
		Point2I origin = Point2I(0, 0);
		Point2I parentInnerExt = getInnerRect(origin, parent->mBounds.extent, NormalState, parent->mProfile).extent;

		resize(origin, parentInnerExt);
	}
}

void GuiFrameSetCtrl::onChildAdded(GuiControl* child)
{
	//Ensure the child isn't positioned to the center
	if (child->getHorizSizing() == horizResizeCenter)
	{
		child->setHorizSizing(horizResizeLeft);
	}
	if (child->getVertSizing() == vertResizeCenter)
	{
		child->setVertSizing(vertResizeTop);
	}
	resize(getPosition(), getExtent());

	Parent::onChildAdded(child);

	Frame* frame = mRootFrame.findFrameWithCtrl(child);
	if(!frame)
	{
		Frame* emptyFrame = mRootFrame.findEmptyFrame();
		if(emptyFrame)
		{
			emptyFrame->control = child;
		}
	}
	resize(getPosition(), getExtent());
}

void GuiFrameSetCtrl::onChildRemoved(GuiControl* child)
{
	Frame* frame = mRootFrame.findFrameWithCtrl(child);

	if (frame && frame == &mRootFrame)
	{
		mRootFrame.control = nullptr;
	}
	else if(frame)
	{
		//Remove the frame from the heirarchy
		Frame* frameParent = frame->parent;
		Frame* frameTwin = frame->twin();

		frameParent->control = frameTwin->control;
		frameParent->child1 = frameTwin->child1;
		frameParent->child2 = frameTwin->child2;
		if (frameParent->child1)
		{
			frameParent->child1->parent = frameParent;
		}
		if (frameParent->child2)
		{
			frameParent->child2->parent = frameParent;
		}
		frameParent->isVertical = frameTwin->isVertical;

		delete frame;
		delete frameTwin;
	}

	resize(getPosition(), getExtent());
}

void GuiFrameSetCtrl::childResized(GuiControl* child)
{
	Parent::childResized(child);
	resize(getPosition(), getExtent());
}

void GuiFrameSetCtrl::childMoved(GuiControl* child)
{
	Parent::childMoved(child);
	resize(getPosition(), getExtent());
}

void GuiFrameSetCtrl::childrenReordered()
{
	resize(getPosition(), getExtent());
	Parent::childrenReordered();
}

Point2I GuiFrameSetCtrl::splitFrame(S32 frameID, bool isVertical)
{
	Frame* frame = mRootFrame.findFrame(frameID);

	if(frame)
	{
		splitFrame(frame, isVertical ? GuiDirection::Up : GuiDirection::Left);
		return Point2I(frame->child1 ? static_cast<S32>(frame->child1->id) :  0, frame->child2 ? static_cast<S32>(frame->child2->id) : 0);
	}
	return Point2I::Zero;
}

void GuiFrameSetCtrl::splitFrame(GuiFrameSetCtrl::Frame* frame, GuiDirection direction)
{
	//The existing control, if any, is moved to the new frame in the direction and anchored. The frame oposite is left empty.
	if (!frame->child1 && !frame->child2)
	{
		GuiControl* ctrl = frame->control;
		frame->child1 = new GuiFrameSetCtrl::Frame(this, frame);
		frame->child2 = new GuiFrameSetCtrl::Frame(this, frame);

		frame->control = nullptr;
		frame->child1->control = direction == GuiDirection::Left || direction == GuiDirection::Up ? ctrl : nullptr;
		frame->child2->control = direction == GuiDirection::Right || direction == GuiDirection::Down ? ctrl : nullptr;
		frame->child1->id = ++mNextFrameID;
		frame->child2->id = ++mNextFrameID;
		frame->child1->isAnchored = direction == GuiDirection::Left || direction == GuiDirection::Up;
		frame->child2->isAnchored = direction == GuiDirection::Right || direction == GuiDirection::Down;
	}

	frame->isVertical = direction == GuiDirection::Up || direction == GuiDirection::Down;
}

void GuiFrameSetCtrl::anchorFrame(S32 frameID)
{
	Frame* frame = mRootFrame.findFrame(frameID);
	anchorFrame(frame);
}

void GuiFrameSetCtrl::anchorFrame(GuiFrameSetCtrl::Frame* frame)
{
	if (frame && frame != &mRootFrame)//The root frame has no twin and can't be anchored.
	{
		frame->isAnchored = true;
		frame->twin()->isAnchored = false;
	}
}

void GuiFrameSetCtrl::setFrameSize(S32 frameID, S32 size)
{
	Frame* frame = mRootFrame.findFrame(frameID);

	if (frame && frame != &mRootFrame)//The root frame must be the size of the control.
	{
		if (frame->parent->isVertical)
		{
			frame->extent.y = size;
			frame->twin()->extent.y = getMax(frame->parent->extent.y - (size + mDividerThickness), minSize);
		}
		else
		{
			frame->extent.x = size;
			frame->twin()->extent.x = getMax(frame->parent->extent.x - (size + mDividerThickness), minSize);
		}
		//Pass this down to the other frames and then to the child controls
		resize(getPosition(), getExtent());
	}
}

void GuiFrameSetCtrl::onPreRender()
{
	SimSet::iterator i;
	for(i = begin(); i < end(); i++)
	{
		GuiTabBookCtrl* book = static_cast<GuiTabBookCtrl*>(*i);
		if (book->mIsFrameSetGenerated && book->size() == 0)
		{
			book->deleteObject();
		}
	}
}

void GuiFrameSetCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	Parent::onRender(offset, updateRect);

	if (mHitDivider)
	{
		GuiControlState currentState = mDepressed ? SelectedState : HighlightState;

		mOldHitDivider = mHitDivider;
		RectI contentRect = RectI(localToGlobalCoord(mHitDivider->dividerRect.point), mHitDivider->dividerRect.extent);
		renderUniversalRect(contentRect, mProfile, currentState, getFillColor(currentState), true);
	}
	else if (mOldHitDivider && (mCurrentState == HighlightState || mFluidFillColor.isAnimating()))
	{
		RectI contentRect = RectI(localToGlobalCoord(mOldHitDivider->dividerRect.point), mOldHitDivider->dividerRect.extent);
		renderUniversalRect(contentRect, mProfile, NormalState, getFillColor(NormalState), true);
	}
	else if (mOldHitDivider)
	{
		mOldHitDivider = nullptr;
	}
}

void GuiFrameSetCtrl::onTouchMove(const GuiEvent& event)
{
	if (!mVisible || !mAwake)
		return;

	if(!mDepressed)
	{
		Point2I localPoint = globalToLocalCoord(event.mousePoint);
		mHitDivider = mRootFrame.findHitDivider(localPoint);
	}

	if (!mHitDivider)
	{
		Parent::onTouchMove(event);
	}
}

void GuiFrameSetCtrl::onTouchDragged(const GuiEvent& event)
{
	if (mDepressed && mHitDivider && mHitDivider->child1 && mHitDivider->child2)
	{
		S32 offset = (mHitDivider->isVertical ? event.mousePoint.y : event.mousePoint.x) - mFrameDragAnchor;
		if(offset != 0)
		{
			if (mHitDivider->isVertical)
			{
				mHitDivider->child1->extent.y = getMax(minSize, mHitDivider->child1->extent.y + offset);
				mHitDivider->child2->extent.y = getMax(minSize, mHitDivider->child2->extent.y - offset);
			}
			else
			{
				mHitDivider->child1->extent.x = getMax(minSize, mHitDivider->child1->extent.x + offset);
				mHitDivider->child2->extent.x = getMax(minSize, mHitDivider->child2->extent.x - offset);
			}
			resize(getPosition(), getExtent());
			mFrameDragAnchor = mHitDivider->isVertical ? event.mousePoint.y : event.mousePoint.x;
		}
	}
}

void GuiFrameSetCtrl::onTouchDown(const GuiEvent& event)
{
 	if (!mActive)
		return;

	if(mHitDivider)
	{
		mDepressed = true;
		mFrameDragAnchor = mHitDivider->isVertical ? event.mousePoint.y : event.mousePoint.x;

		//lock the mouse
		mouseLock();

		//update
		setUpdate();
	}
}

void GuiFrameSetCtrl::onTouchUp(const GuiEvent& event)
{
	if (!mActive)
		return;

	if(mHitDivider)
	{
		mouseUnlock();

		mDepressed = false;

		//update
		setUpdate();
	}
}

void GuiFrameSetCtrl::getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent)
{
	GuiControl* parent = getParent();
	if (!parent)
	{
		return;
	}
	if (mHitDivider && mHitDivider->isVertical)
	{
		if (mUpDownCursor == NULL)
		{
			SimObject* obj;
			obj = Sim::findObject("UpDownCursor");
			mUpDownCursor = dynamic_cast<GuiCursor*>(obj);
		}
		if (mUpDownCursor != NULL)
		{
			cursor = mUpDownCursor;
		}
	}
	else if (mHitDivider && !mHitDivider->isVertical)
	{
		if (mLeftRightCursor == NULL)
		{
			SimObject* obj;
			obj = Sim::findObject("LeftRightCursor");
			mLeftRightCursor = dynamic_cast<GuiCursor*>(obj);
		}
		if (mLeftRightCursor != NULL)
		{
			cursor = mLeftRightCursor;
		}
	}
}

void GuiFrameSetCtrl::renderDropOptions(GuiWindowCtrl* window)
{
	Point2I cursorPt = Point2I(0, 0);
	GuiCanvas* root = getRoot();
	if (root)
	{
		cursorPt = globalToLocalCoord(root->getCursorPos());
		cursorPt.x = getMin(getMax(0, cursorPt.x), mRootFrame.extent.x);
		cursorPt.y = getMin(getMax(0, cursorPt.y), mRootFrame.extent.y);
	}

	Frame* frame = mRootFrame.findFrameWithPoint(cursorPt);
	if (frame)
	{
		ColorI fillColor = mProfile->getFillColor(SelectedState);
		fillColor.alpha = 50;
		const U32 width = getMax(minSize, getMin(frame->extent.x / 2, window->mBounds.extent.x));
		const U32 height = getMax(minSize, getMin(frame->extent.y / 2, window->mBounds.extent.y));

		if (frame->hasLeftRightButtons)
		{
			Point2I fillExt = Point2I(width, frame->extent.y);
			renderDropButton(frame, frame->mLeftButtonRect, cursorPt, frame->localPosition, fillExt, GuiDirection::Left);
			renderDropButton(frame, frame->mRightButtonRect, cursorPt, Point2I(frame->localPosition.x + frame->extent.x - width, frame->localPosition.y), fillExt, GuiDirection::Right);
		}

		if (frame->hasTopBottomButtons)
		{
			Point2I fillExt = Point2I(frame->extent.x, height);
			renderDropButton(frame, frame->mTopButtonRect, cursorPt, frame->localPosition, fillExt, GuiDirection::Up);
			renderDropButton(frame, frame->mBottomButtonRect, cursorPt, Point2I(frame->localPosition.x, frame->localPosition.y + frame->extent.y - height), fillExt, GuiDirection::Down);
		}

		if (hasCenterButton(frame))
		{
			renderDropButton(frame, frame->mCenterButtonRect, cursorPt, frame->localPosition, frame->extent, GuiDirection::Center);
		}
	}
}

bool GuiFrameSetCtrl::hasCenterButton(GuiFrameSetCtrl::Frame* frame)
{
	return (frame != &mRootFrame && (frame->isAnchored || (!frame->control && !frame->child1 && !frame->child2)));
}

void GuiFrameSetCtrl::renderDropButton(const GuiFrameSetCtrl::Frame* frame, const RectI& buttonRect, const Point2I& cursorPt, const Point2I& fillPos, const Point2I& fillExt, GuiDirection direction)
{
	GuiControlState state = NormalState;
	if (buttonRect.pointInRect(cursorPt))
	{
		state = HighlightState;
		RectI rect = RectI(fillPos, fillExt);
		rect.point = localToGlobalCoord(rect.point);
		setUpdateRegion(rect.point, rect.extent);
		renderUniversalRect(rect, mDropButtonProfile, SelectedState);
	}
	RectI globalButtonRect = RectI(localToGlobalCoord(buttonRect.point), buttonRect.extent);
	renderUniversalRect(globalButtonRect, mDropButtonProfile, state);

	ColorI triColor = getFontColor(mDropButtonProfile, state);
	if(direction != GuiDirection::Center)
	{
		renderTriangleIcon(globalButtonRect, triColor, direction, 10);
	}
	else
	{
		RectI iconRect = RectI(buttonRect);
		iconRect.inset(15, 15);
		RectI lineH = RectI(localToGlobalCoord(Point2I(iconRect.point.x, iconRect.point.y + 4)), Point2I(iconRect.extent.x, 2));
		RectI lineV = RectI(localToGlobalCoord(Point2I(iconRect.point.x + 4, iconRect.point.y)), Point2I(2, iconRect.extent.y));
		dglDrawRectFill(lineH, triColor);
		dglDrawRectFill(lineV, triColor);
	}
}

void GuiFrameSetCtrl::handleDropButtons(GuiWindowCtrl* window)
{
	Point2I cursorPt = Point2I(0, 0);
	GuiCanvas* root = getRoot();
	if (root)
	{
		cursorPt = globalToLocalCoord(root->getCursorPos());
		cursorPt.x = getMin(getMax(0, cursorPt.x), mRootFrame.extent.x);
		cursorPt.y = getMin(getMax(0, cursorPt.y), mRootFrame.extent.y);
	}
	Frame* frame = mRootFrame.findFrameWithPoint(cursorPt);

	if (frame)
	{
		const U32 width = getMax(minSize, getMin(frame->extent.x / 2, window->mBounds.extent.x));
		const U32 height = getMax(minSize, getMin(frame->extent.y / 2, window->mBounds.extent.y));

		bool hitButton = false;
		if (frame->hasLeftRightButtons && frame->mLeftButtonRect.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Right);//This existing control goes right, the new window will go left.
			anchorFrame(frame->child1);
			frame->child1->control = window;
			frame->child1->extent = Point2I(width, frame->extent.y);
			hitButton = true;
		}
		else if (frame->hasLeftRightButtons && frame->mRightButtonRect.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Left);
			anchorFrame(frame->child2);
			frame->child2->control = window;
			frame->child2->extent = Point2I(width, frame->extent.y);
			hitButton = true;
		}
		else if (frame->hasTopBottomButtons && frame->mTopButtonRect.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Down);
			anchorFrame(frame->child1);
			frame->child1->control = window;
			frame->child1->extent = Point2I(frame->extent.x, height);
			hitButton = true;
		}
		else if (frame->hasTopBottomButtons && frame->mBottomButtonRect.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Up);
			anchorFrame(frame->child2);
			frame->child2->control = window;
			frame->child2->extent = Point2I(frame->extent.x, height);
			hitButton = true;
		}
		else if (hasCenterButton(frame) && frame->mCenterButtonRect.pointInRect(cursorPt))
		{
			if (!frame->control)
			{
				frame->control = window;
				hitButton = true;
			}
			else
			{
				GuiTabBookCtrl* book = dynamic_cast<GuiTabBookCtrl*>(frame->control);
				GuiTabPageCtrl* page = nullptr;
				if (!book)
				{
					book = new GuiTabBookCtrl();
					book->setControlProfile(mTabBookProfile);
					book->setControlTabProfile(mTabProfile);
					book->mBounds.set(frame->localPosition, frame->extent);
					book->mIsFrameSetGenerated = true;
					book->registerObject();
					book->addNewPage();	
					page = dynamic_cast<GuiTabPageCtrl*>((*book)[0]);
					page->setControlProfile(mTabPageProfile);
					GuiWindowCtrl* windowChild = dynamic_cast<GuiWindowCtrl*>(frame->control);
					GuiControl* child = dynamic_cast<GuiControl*>(frame->control);
					if (windowChild)
					{
						windowChild->dockToPage();
					}
					page->setText(child->getText());
					frame->control = book;
					page->addObject(child);
					addObject(book);
				}
				window->dockToPage();
				book->addNewPage();
				page = dynamic_cast<GuiTabPageCtrl*>((*book)[book->size() - 1]);
				page->setControlProfile(mTabPageProfile);
				page->setText(window->getText());
				page->addObject(window);
				book->selectPage(window->getText());
				window->resize(window->getPosition(), window->getExtent());
			}
		}

		if (hitButton)
		{
			addObject(window);
		}
	}
}

void GuiFrameSetCtrl::undockWindowFromBook(GuiWindowCtrl* window, GuiTabBookCtrl* book, GuiTabPageCtrl* page)
{
	GuiControl* parent = getParent();
	GuiCanvas* root = getRoot();
	if (parent && window && book && page && window->mPageDocked && root)
	{
		parent->addObject(window);
		parent->pushObjectToBack(window); 
		Point2I cursorPt = globalToLocalCoord(root->getCursorPos());
		
		window->undockFromPage();
		window->restore();
		Point2I newPosition = Point2I(cursorPt.x - (window->getExtent().x / 2), cursorPt.y - (window->getTitleHeight() / 2));
		Point2I pos = parent->localToGlobalCoord(newPosition);
		root->addUpdateRegion(pos, window->getExtent());
		window->resize(newPosition, window->getExtent());
		GuiEvent event = GuiEvent();
		event.mousePoint = Point2I(root->getCursorPos());
		event.mouseClickCount = 1;
		root->rootScreenTouchUp(event);
		root->rootScreenTouchDown(event);

		book->removeObject(page);
		page->deleteObject();

		if (book->size() > 0)
		{
			book->calculatePageTabs();
			book->selectPage(0);
		}

		if (book->mIsFrameSetGenerated && book->size() == 1)
		{
			GuiTabPageCtrl* lastPage = dynamic_cast<GuiTabPageCtrl*>((*book)[0]);
			GuiWindowCtrl* lastWindow = dynamic_cast<GuiWindowCtrl*>((*lastPage)[0]);
			GuiFrameSetCtrl::Frame* frame = mRootFrame.findFrameWithCtrl(book);
			frame->control = lastWindow;
			addObject(lastWindow);
			lastPage->deleteObject();
			lastWindow->undockFromPage();
		}
	}
}

void GuiFrameSetCtrl::setDropButtonProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiFrameSetCtrl::setDropButtonProfile: invalid content profile");
	if (prof == mDropButtonProfile)
		return;
	if (mAwake)
		mDropButtonProfile->decRefCount();
	mDropButtonProfile = prof;
	if (mAwake)
		mDropButtonProfile->incRefCount();
}

void GuiFrameSetCtrl::setTabBookProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiFrameSetCtrl::setTabBookProfile: invalid content profile");
	if (prof == mTabBookProfile)
		return;
	if (mAwake)
		mTabBookProfile->decRefCount();
	mTabBookProfile = prof;
	if (mAwake)
		mTabBookProfile->incRefCount();

	//Cycle through all children and replace the profiles of frameSetGenerated books.
	SimSet::iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiTabBookCtrl* book = dynamic_cast<GuiTabBookCtrl*>(*i);
		if (book && book->mIsFrameSetGenerated)
		{
			book->setControlProfile(prof);
		}
	}
}

void GuiFrameSetCtrl::setTabProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiFrameSetCtrl::setTabProfile: invalid content profile");
	if (prof == mTabProfile)
		return;
	if (mAwake)
		mTabProfile->decRefCount();
	mTabProfile = prof;
	if (mAwake)
		mTabProfile->incRefCount();

	SimSet::iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiTabBookCtrl* book = dynamic_cast<GuiTabBookCtrl*>(*i);
		if (book && book->mIsFrameSetGenerated)
		{
			book->setControlTabProfile(prof);
		}
	}
}

void GuiFrameSetCtrl::setTabPageProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiFrameSetCtrl::setTabPageProfile: invalid content profile");
	if (prof == mTabPageProfile)
		return;
	if (mAwake)
		mTabPageProfile->decRefCount();
	mTabPageProfile = prof;
	if (mAwake)
		mTabPageProfile->incRefCount();

	SimSet::iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiTabBookCtrl* book = dynamic_cast<GuiTabBookCtrl*>(*i);
		if (book && book->mIsFrameSetGenerated)
		{
			SimSet::iterator j;
			for (j = begin(); j != end(); j++)
			{
				GuiTabPageCtrl* page = dynamic_cast<GuiTabPageCtrl*>(*i);
				if (page)
				{
					page->setControlProfile(prof);
				}
			}
		}
	}
}

void GuiFrameSetCtrl::setControlLeftRightCursor(GuiCursor* cursor)
{
	AssertFatal(cursor, "GuiFrameSetCtrl::setControlLeftRightCursor: invalid cursor");
	if (cursor == mLeftRightCursor)
		return;
	mLeftRightCursor = cursor;
}

void GuiFrameSetCtrl::setControlUpDownCursor(GuiCursor* cursor)
{
	AssertFatal(cursor, "GuiFrameSetCtrl::setControlUpDownCursor: invalid cursor");
	if (cursor == mUpDownCursor)
		return;
	mUpDownCursor = cursor;
}