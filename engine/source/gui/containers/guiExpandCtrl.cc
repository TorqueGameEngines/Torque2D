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

#include "gui/containers/guiExpandCtrl.h"

#include "guiExpandCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiExpandCtrl);

GuiExpandCtrl::GuiExpandCtrl()
{
   mActive = true;
   mExpanded = false;
   mIsContainer = true;
   mCollapsedExtent.set(64,64);
   mAnimationProgress = 1;
   mExpandedExtent.set(64, 64);
   mEasingFunction = EasingFunction::Linear;
   mAnimationLength = 500;
}

void GuiExpandCtrl::initPersistFields()
{
  Parent::initPersistFields();

  addField("easeExpand", TypeEnum, Offset(mEasingFunction, GuiExpandCtrl), 1, &gEasingTable);
  addField("easeTimeExpand", TypeS32, Offset(mAnimationLength, GuiExpandCtrl));
}

void GuiExpandCtrl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
	Point2I newPosition = getPosition();
	Point2I newExtent = getExtent();

	S32 deltaX = newParentExtent.x - oldParentExtent.x;
	S32 deltaY = newParentExtent.y - oldParentExtent.y;

	if (mHorizSizing == horizResizeCenter)
		newPosition.x = (newParentExtent.x - mBounds.extent.x) >> 1;
	else if (mHorizSizing == horizResizeWidth)
	{
		newExtent.x += deltaX;
		mCollapsedExtent.x += deltaX;
	}
	else if (mHorizSizing == horizResizeLeft)
		newPosition.x += deltaX;
	else if (mHorizSizing == horizResizeRelative && oldParentExtent.x != 0)
	{
		S32 newLeft = (newPosition.x * newParentExtent.x) / oldParentExtent.x;
		S32 newRight = ((newPosition.x + newExtent.x) * newParentExtent.x) / oldParentExtent.x;

		newPosition.x = newLeft;
		newExtent.x = newRight - newLeft;

		S32 collapsedRight = ((newPosition.x + mCollapsedExtent.x) * newParentExtent.x) / oldParentExtent.x;
		mCollapsedExtent.x = collapsedRight - newLeft;
	}

	if (mVertSizing == vertResizeCenter)
		newPosition.y = (newParentExtent.y - mBounds.extent.y) >> 1;
	else if (mVertSizing == vertResizeHeight)
	{
		newExtent.y += deltaY;
		mCollapsedExtent.y += deltaY;
	}
	else if (mVertSizing == vertResizeTop)
		newPosition.y += deltaY;
	else if (mVertSizing == vertResizeRelative && oldParentExtent.y != 0)
	{
		S32 newTop = (newPosition.y * newParentExtent.y) / oldParentExtent.y;
		S32 newBottom = ((newPosition.y + newExtent.y) * newParentExtent.y) / oldParentExtent.y;

		newPosition.y = newTop;
		newExtent.y = newBottom - newTop;

		S32 collapsedBottom = ((newPosition.y + mCollapsedExtent.y) * newParentExtent.y) / oldParentExtent.y;
		mCollapsedExtent.y = collapsedBottom - newTop;
	}

	if (mAnimationProgress == 1 && !mExpanded)
	{
		setCollapsedExtent(newExtent);
	}

	resize(newPosition, newExtent);

	calcExpandedExtent();

	if (mExpanded)
	{
		mBounds.extent = mExpandedExtent;
	}
	else
	{
		mBounds.extent = mCollapsedExtent;
	}
	setUpdate();
}

void GuiExpandCtrl::setCollapsedExtent(const Point2I &extent)
{
	mCollapsedExtent = extent;

	mExpandedExtent.set(getMax(extent.x, mExpandedExtent.x), getMax(extent.y, mExpandedExtent.y));
}

bool GuiExpandCtrl::calcExpandedExtent()
{
	if (!size())
		return false;

	mExpandedExtent = Point2I(0, 0);
	for (iterator itr = begin(); itr != end(); ++itr)
	{
		GuiControl* child = dynamic_cast<GuiControl*>(*itr);
		mExpandedExtent.setMax(child->getExtent() + child->getPosition());
	}

	mExpandedExtent = getOuterExtent(mExpandedExtent, GuiControlState::NormalState, mProfile);
	mExpandedExtent.set(getMax(mCollapsedExtent.x, mExpandedExtent.x), getMax(mCollapsedExtent.y, mExpandedExtent.y));

	return true;
}

void GuiExpandCtrl::setExpanded(bool isExpanded)
{
	if ((mExpanded == isExpanded) || (isExpanded && !calcExpandedExtent()))
	{
		return;
	}
	
	mAnimationProgress = 1 - mAnimationProgress;
	mExpanded = isExpanded;
	setProcessTicks(true);
}

bool GuiExpandCtrl::processExpansion()
{
	F32 progress = getProgress(32.0f);

	setUpdate();
	if (mExpanded)
	{
		//We are growing
		mBounds.extent.x = processValue(progress, mCollapsedExtent.x, mExpandedExtent.x);
		mBounds.extent.y = processValue(progress, mCollapsedExtent.y, mExpandedExtent.y);
	}
	else
	{
		//We are shrinking
		mBounds.extent.x = processValue(progress, mExpandedExtent.x, mCollapsedExtent.x);
		mBounds.extent.y = processValue(progress, mExpandedExtent.y, mCollapsedExtent.y);
	}
	setUpdate();

	GuiControl *parent = getParent();
	if (parent)
		parent->childResized(this);


	if (mAnimationProgress >= 1.0f)
	{
		mAnimationProgress = 1.0f;
		return false;
	}
	return true;
}

void GuiExpandCtrl::processTick() 
{
	bool shouldWeContinue = false;

	//Expanding
	shouldWeContinue |= processExpansion();

	if (!shouldWeContinue)
	{
		setProcessTicks(false);
	}
}