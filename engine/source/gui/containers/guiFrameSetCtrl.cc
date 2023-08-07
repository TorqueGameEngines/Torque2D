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
#include "gui/editor/guiEditCtrl.h"
#include "collection/vector.h"

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
	if (!child1 && !child2 && owner->isEditMode())
	{
		if (extent.x > (3 * owner->minSize))
		{
			spliterRect1.set(localPosition.x + ((extent.x - owner->mDividerThickness) / 2), localPosition.y, owner->mDividerThickness, extent.y);
		}
		else
		{
			spliterRect1.set(0, 0, 0, 0);
		}

		if (extent.y > (3 * owner->minSize))
		{
			spliterRect2.set(localPosition.x, localPosition.y + ((extent.y - owner->mDividerThickness) / 2), extent.x, owner->mDividerThickness);
		}
		else
		{
			spliterRect2.set(0, 0, 0, 0);
		}
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

GuiFrameSetCtrl::Frame* GuiFrameSetCtrl::Frame::findEmptyFrameAtLocation(const RectI& location)
{
	if (!control && !child1 && !child2 && localPosition == location.point && extent == location.extent)
	{
		return this;
	}
	else if (child1 && child2)
	{
		Frame* attempt = child1->findEmptyFrameAtLocation(location);
		return attempt ? attempt : child2->findEmptyFrameAtLocation(location);
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
	if (!child1 && !child2)
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
			Point2I pt = isVertical ? Point2I(x, y - pos2.y) : Point2I(x - pos2.x, y);

			return child2->findFrameWithPoint(pt);
		}
	}
	return nullptr;//This will happen if the mouse is over a divider.
}

void GuiFrameSetCtrl::Frame::editRender(const Point2I& cursorPt, const F32 fade)
{
	//cursorPt is local to owner
	if (!child1 && !child2)
	{
		//Show the frame
		RectI localRect = RectI(localPosition, extent);
		Point2I pos = localPosition;
		RectI rect = RectI(owner->localToGlobalCoord(pos), extent);
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		ColorI border = edit->getEditorColor();
		if (!localRect.pointInRect(cursorPt))
		{
			border.alpha = (150 * fade);
		}
		dglDrawRect(rect, border);
		ColorI fill = ColorI(border);
		fill.alpha = (40 * fade);
		dglDrawRectFill(rect, fill);

		U8 divThickness = owner->mDividerThickness;

		fill.alpha = (10 * fade);
		if(spliterRect1.pointInRect(cursorPt))
		{
			fill.alpha = (200 * fade);
		}
		RectI globalSpliter1 = RectI(owner->localToGlobalCoord(spliterRect1.point), spliterRect1.extent);
		dglDrawRectFill(globalSpliter1, fill);

		fill.alpha = (10 * fade);
		if (spliterRect2.pointInRect(cursorPt) && !spliterRect1.pointInRect(cursorPt))
		{
			fill.alpha = (200 * fade);
		}
		RectI globalSpliter2 = RectI(owner->localToGlobalCoord(spliterRect2.point), spliterRect2.extent);
		dglDrawRectFill(globalSpliter2, fill);
	}

	if (child1)
	{
		child1->editRender(cursorPt, fade);
	}

	if (child2)
	{
		child2->editRender(cursorPt, fade);
	}
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

	addField("DividerThickness", TypeS8, Offset(mDividerThickness, GuiFrameSetCtrl));
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

	Frame* frame = &mRootFrame;
	loadFrame(frame, 1);
	clearDynamicFields();

	//resize one last time for the frames
	resize(getPosition(), getExtent());

	// Return Success.
	return true;
}

void GuiFrameSetCtrl::parentResized(const Point2I& oldParentExtent, const Point2I& newParentExtent)
{
	//In the case of centering, we want to make doubly sure we are using the inner rect.
	GuiControl* parent = getParent();
	if(parent)
	{
		Point2I origin = Point2I::Zero;
		Point2I parentInnerExt = parent->getInnerRect().extent;

		resize(origin, parentInnerExt);
	}
}

void GuiFrameSetCtrl::loadFrame(GuiFrameSetCtrl::Frame* frame, const U32 frameID)
{
	frame->id = frameID;
	U32 child1ID = dAtoi(getDataField("child1ID", frameID));
	U32 child2ID = dAtoi(getDataField("child2ID", frameID));
	frame->isVertical = dAtob(getDataField("isVertical", frameID));
	U32 frameExtentX = dAtoi(getDataField("frameExtentX", frameID));
	U32 frameExtentY = dAtoi(getDataField("frameExtentY", frameID));
	frame->isAnchored = dAtob(getDataField("isAnchored", frameID));

	frame->extent.set(frameExtentX, frameExtentY);
	if (child1ID && child2ID)
	{
		splitFrame(frame, frame->isVertical ? GuiDirection::Up : GuiDirection::Left);

		loadFrame(frame->child1, child1ID);
		loadFrame(frame->child2, child2ID);
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

	Parent::onChildAdded(child);

	assignChildToFrame(child);
}

void GuiFrameSetCtrl::assignChildToFrame(GuiControl* child)
{
	Frame* frame = mRootFrame.findFrameWithCtrl(child);
	if(!frame)
	{
		Frame* emptyFrame = mRootFrame.findEmptyFrameAtLocation(child->getBounds());
		if(emptyFrame)
		{
			emptyFrame->control = child;
		}
		else
		{
			Frame* emptyFrame = mRootFrame.findEmptyFrame();
			if (emptyFrame)
			{
				emptyFrame->control = child;
			}
		}
	}
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
		GuiTabBookCtrl* book = dynamic_cast<GuiTabBookCtrl*>(*i);
		if (book && book->mIsFrameSetGenerated && book->size() == 0)
		{
			book->deleteObject();
		}
	}

	if (isEditMode())
	{
		GuiCanvas* root = getRoot();
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		if (root && edit)
		{
			Point2I cursorPt = globalToLocalCoord(root->getCursorPos());
			if (mBounds.pointInRect(cursorPt))
			{
				mHitDivider = mRootFrame.findHitDivider(cursorPt);
			}
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

	if (isEditMode())
	{
		GuiCanvas* root = getRoot();
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		if(root && edit)
		{
			Point2I cursorPt = globalToLocalCoord(root->getCursorPos());
			RectI visBounds = RectI(mBounds);
			const S32 fadeDist = 140;
			visBounds.inset(-fadeDist, -fadeDist);
			if (visBounds.pointInRect(cursorPt) && isEditSelected())
			{
				visBounds.inset(fadeDist, fadeDist);
				F32 fade = getMin(1.0, getMax(0.0, 1.0 - ((F32)visBounds.distanceToRect(cursorPt) / (F32)fadeDist)));
				mRootFrame.editRender(cursorPt, fade);
			}
		}
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

bool GuiFrameSetCtrl::onMouseDownEditor(const GuiEvent& event, const Point2I& offset)
{
	Point2I cursorPt = globalToLocalCoord(event.mousePoint);
	Frame* frame = mRootFrame.findFrameWithPoint(cursorPt);

	if(frame && isEditSelected())
	{
		if (frame->spliterRect1.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Left);
			setFrameSize(frame->child1->id, (frame->extent.x - mDividerThickness) / 2);
			return true;
		}
		else if (frame->spliterRect2.pointInRect(cursorPt))
		{
			splitFrame(frame, GuiDirection::Up);
			setFrameSize(frame->child1->id, (frame->extent.y - mDividerThickness) / 2);
			return true;
		}
	}
	frame = mRootFrame.findHitDivider(cursorPt);
	if (frame)
	{
		onTouchDown(event);
		return true;
	}

	return Parent::onMouseDownEditor(event, offset);
}

bool GuiFrameSetCtrl::onMouseUpEditor(const GuiEvent& event, const Point2I& offset)
{
	if (mDepressed)
	{
		onTouchUp(event);
		return true;
	}
	return false;
}

bool GuiFrameSetCtrl::onMouseDraggedEditor(const GuiEvent& event, const Point2I& offset)
{
	if (mDepressed)
	{
		onTouchDragged(event);
		return true;
	}
	return false;
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

void GuiFrameSetCtrl::writeFields(Stream& stream, U32 tabStop)
{
	if (mRootFrame.child1 && mRootFrame.child2)
	{
		setModDynamicFields(true);
		setCanSaveDynamicFields(true);

		Frame* frame = &mRootFrame;
		generateFrameFields(frame);
		Parent::writeFields(stream, tabStop);
		clearDynamicFields();

		setModDynamicFields(false);
		setCanSaveDynamicFields(false);
	}
	else
	{
		Parent::writeFields(stream, tabStop);
	}
}

void GuiFrameSetCtrl::generateFrameFields(GuiFrameSetCtrl::Frame* frame)
{
	U32 id = frame->id;
	U32 child1ID = frame->child1 ? frame->child1->id : 0;
	U32 child2ID = frame->child2 ? frame->child2->id : 0;

	setDataField("child1ID", id, child1ID);
	setDataField("child2ID", id, child2ID);
	setDataField("isVertical", id, frame->isVertical);
	setDataField("frameExtentX", id, frame->extent.x);
	setDataField("frameExtentY", id, frame->extent.y);
	setDataField("isAnchored", id, frame->isAnchored);

	if (frame->child1)
	{
		generateFrameFields(frame->child1);
	}

	if (frame->child2)
	{
		generateFrameFields(frame->child2);
	}
}

void GuiFrameSetCtrl::setDataField(const char* tag, const U32 id, const U32 value)
{
	char idBuffer[16];
	char valueBuffer[16];

	StringTableEntry tagFieldName = StringTable->insert(tag);

	dSprintf(idBuffer, 16, "%d", id);
	dSprintf(valueBuffer, 16, "%d", value);
	SimObject::setDataField(tagFieldName, idBuffer, valueBuffer);
}

const char* GuiFrameSetCtrl::getDataField(const char* tag, const U32 id)
{
	char idBuffer[16];
	StringTableEntry tagFieldName = StringTable->insert(tag);

	dSprintf(idBuffer, 16, "%d", id);
	return SimObject::getDataField(tagFieldName, idBuffer);
}

static StringTableEntry frameNodeSectionName		= StringTable->insert("Frames", true);
static StringTableEntry frameNodeName				= StringTable->insert("Frame", true);
static StringTableEntry frameIDName					= StringTable->insert("ID", true);
static StringTableEntry frameChild1Name				= StringTable->insert("Child1ID", true);
static StringTableEntry frameChild2Name				= StringTable->insert("Child2ID", true);
static StringTableEntry frameIsVerticalName			= StringTable->insert("IsVertical", true);
static StringTableEntry frameExtentXName			= StringTable->insert("ExtentX", true);
static StringTableEntry frameExtentYName			= StringTable->insert("ExtentY", true);
static StringTableEntry frameIsAnchoredName			= StringTable->insert("IsAnchored", true);
static StringTableEntry frameChildMapName			= StringTable->insert("ChildMap", true);

void GuiFrameSetCtrl::onTamlCustomWrite(TamlCustomNodes& customNodes)
{
	// Debug Profiling.
	PROFILE_SCOPE(GuiFrameSetCtrl_OnTamlCustomWrite);

	// Call parent.
	Parent::onTamlCustomWrite(customNodes);

	if (mRootFrame.child1 && mRootFrame.child2)
	{
		TamlCustomNode* pCustomFrameSection = customNodes.addNode(frameNodeSectionName);

		Frame* frame = &mRootFrame;
		writeFrameCustomTaml(pCustomFrameSection, frame);
	}
}

void GuiFrameSetCtrl::writeFrameCustomTaml(TamlCustomNode* pParentNode, GuiFrameSetCtrl::Frame* frame)
{
	TamlCustomNode* pFrameNode = pParentNode->addNode(frameNodeName);

	U32 child1ID = frame->child1 ? frame->child1->id : 0;
	U32 child2ID = frame->child2 ? frame->child2->id : 0;

	pFrameNode->addField(frameIDName, frame->id);
	pFrameNode->addField(frameChild1Name, child1ID);
	pFrameNode->addField(frameChild2Name, child2ID);
	pFrameNode->addField(frameIsVerticalName, frame->isVertical);
	pFrameNode->addField(frameExtentXName, frame->extent.x);
	pFrameNode->addField(frameExtentYName, frame->extent.y);
	pFrameNode->addField(frameIsAnchoredName, frame->isAnchored);

	if (frame->control)
	{
		U32 childMap = 0;
		for (SimSet::iterator itr = begin(); itr != end(); itr++, childMap++)
		{
			GuiControl* ctrl = dynamic_cast<GuiControl*>(*itr);
			if (ctrl && ctrl == frame->control)
			{
				break;
			}
		}
		pFrameNode->addField(frameChildMapName, childMap);
	}

	if (frame->child1)
	{
		writeFrameCustomTaml(pFrameNode, frame->child1);
	}

	if (frame->child2)
	{
		writeFrameCustomTaml(pFrameNode, frame->child2);
	}
}

void GuiFrameSetCtrl::onTamlCustomRead(const TamlCustomNodes& customNodes)
{
	// Debug Profiling.
	PROFILE_SCOPE(GuiFrameSetCtrl_OnTamlCustomRead);

	// Call parent.
	Parent::onTamlCustomRead(customNodes);

	// Load the frames
	loadTamlFrames(customNodes);
}

void GuiFrameSetCtrl::loadTamlFrames(const TamlCustomNodes& customNodes)
{
	const TamlCustomNode* pCustomFrameSection = customNodes.findNode(frameNodeSectionName);

	if (pCustomFrameSection != NULL)
	{
		const TamlCustomNodeVector& childList = pCustomFrameSection->getChildren();

		if (childList.size() == 0)
		{
			//not really an error so let's move on.
			return;
		}
		else if (childList.size() != 1)
		{
			Con::warnf("GuiFrameSetCtrl::loadTamlFrames() - Only a single root frame should be defined.");
			return;
		}

		TamlCustomNode* rootNode = *childList.begin();
		Frame* frame = &mRootFrame;
		loadTamlFrame(*rootNode, frame);
		mRootFrame.resize(getPosition(), getExtent());
	}
}

void GuiFrameSetCtrl::loadTamlFrame(const TamlCustomNode& node, GuiFrameSetCtrl::Frame* frame)
{
	StringTableEntry frameName = node.getNodeName();

	if (frameName != frameNodeName)
	{
		Con::warnf("GuiFrameSetCtrl::loadTamlFrame() - Unknown tag name of '%s'. Only '%s' is valid.", frameName, frameNodeName);
		return;
	}

	U32 child1ID = 0;
	U32 child2ID = 0;
	U32 childMap = 0;

	const TamlCustomFieldVector& fields = node.getFields();
	for (TamlCustomFieldVector::const_iterator fieldItr = fields.begin(); fieldItr != fields.end(); ++fieldItr)
	{
		const TamlCustomField* pField = *fieldItr;
		StringTableEntry fieldName = pField->getFieldName();

		if (fieldName == frameIDName)
		{
			pField->getFieldValue(frame->id);
		}
		else if (fieldName == frameChild1Name)
		{
			pField->getFieldValue(child1ID);
		}
		else if (fieldName == frameChild2Name)
		{
			pField->getFieldValue(child2ID);
		}
		else if (fieldName == frameIsVerticalName)
		{
			pField->getFieldValue(frame->isVertical);
		}
		else if (fieldName == frameExtentXName)
		{
			pField->getFieldValue(frame->extent.x);
		}
		else if (fieldName == frameExtentYName)
		{
			pField->getFieldValue(frame->extent.y);
		}
		else if (fieldName == frameIsAnchoredName)
		{
			pField->getFieldValue(frame->isAnchored);
		}
		else if (fieldName == frameChildMapName)
		{
			pField->getFieldValue(childMap);
		}
		else
		{
			Con::warnf("GuiFrameSetCtrl::loadTamlFrame() - Encountered an unknown field name of '%s'.", fieldName);
			continue;
		}
	}

	if (frame->extent.x <= 0 || frame->extent.y <= 0)
	{
		Con::warnf("GuiFrameSetCtrl::loadTamlFrame() - Frame extent of '(%d,%d)' is invalid or was not set.", frame->extent.x, frame->extent.y);
		frame->extent.set(1, 1);
	}

	const TamlCustomNodeVector& childList = node.getChildren();

	if ((child1ID != 0 || child2ID != 0) && childList.size() < 2)
	{
		Con::warnf("GuiFrameSetCtrl::loadTamlFrame() - Frame %d shows child IDs but child nodes don't exist. Frames may have no children or exactly 2.", frame->id);
		return;
	}
	else if (childList.size() != 0 && childList.size() != 2)
	{
		Con::warnf("GuiFrameSetCtrl::loadTamlFrame() - Frame %d has %d child nodes. Frames may have no children or exactly 2.", frame->id, childList.size());
		return;
	}

	if (childList.size() == 2 && child1ID && child2ID)
	{
		splitFrame(frame, frame->isVertical ? GuiDirection::Up : GuiDirection::Left);

		TamlCustomNode* child1Node = *childList.begin();
		loadTamlFrame(*child1Node, frame->child1);

		TamlCustomNode* child2Node = *(childList.begin() + 1);
		loadTamlFrame(*child2Node, frame->child2);
	}
	else
	{
		GuiControl* child = dynamic_cast<GuiControl*>(at(childMap));
		frame->control = child;
	}
}