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

#include "guiFrameSetCtrl_ScriptBinding.h"

void GuiFrameSetCtrl::Frame::resize(const Point2I& newPosition, const Point2I& newExtent)
{
	const S32 minSize = owner->minSize;
	extent.set(newExtent.x, newExtent.y);
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
		Point2I pos2 = isVertical ? Point2I(newPosition.x, y1 + owner->mDividerThickness) : Point2I(x1 + owner->mDividerThickness, newPosition.y);

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

	mLeftRightCursor = NULL;
	mUpDownCursor = NULL;
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("DividerThickness", TypeS32, Offset(mDividerThickness, GuiFrameSetCtrl));
	addField("leftRightCursor", TypeGuiCursor, Offset(mLeftRightCursor, GuiFrameSetCtrl));
	addField("upDownCursor", TypeGuiCursor, Offset(mUpDownCursor, GuiFrameSetCtrl));
}

//------------------------------------------------------------------------------

bool GuiFrameSetCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	return true;
}

//------------------------------------------------------------------------------

void GuiFrameSetCtrl::onSleep()
{
	Parent::onSleep();
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

	Frame* frame = mRootFrame.findEmptyFrame();
	if(frame)
	{
		frame->control = child;
	}
}

void GuiFrameSetCtrl::onChildRemoved(SimObject* child)
{
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
		if (!frame->child1 && !frame->child2)
		{
			GuiControl* ctrl = frame->control;
			frame->child1 = new GuiFrameSetCtrl::Frame(this, frame);
			frame->child2 = new GuiFrameSetCtrl::Frame(this, frame);

			frame->control = nullptr;
			frame->child1->control = ctrl;
			frame->child1->id = ++mNextFrameID;
			frame->child2->id = ++mNextFrameID;
			frame->child1->isAnchored = true;
			frame->child2->isAnchored = false;
		}
	
		frame->isVertical = isVertical;
		return Point2I(static_cast<S32>(frame->child1->id), static_cast<S32>(frame->child2->id));
	}
	return Point2I::Zero;
}

void GuiFrameSetCtrl::anchorFrame(S32 frameID)
{
	Frame* frame = mRootFrame.findFrame(frameID);

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