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
#include "gui/containers/guiChainCtrl.h"
#include "gui/editor/guiEditCtrl.h"

#include "guiChainCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiChainCtrl);

//------------------------------------------------------------------------------

GuiChainCtrl::GuiChainCtrl()
{
	mIsContainer = true;
	mChildSpacing = 0;
	mIsVertical = true;
	mBounds.extent.set(140, mEditOpenSpace);
	mResizeGuard = false;
}

//------------------------------------------------------------------------------

void GuiChainCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("Gui Chain Settings");
	addField("ChildSpacing", TypeS32, Offset(mChildSpacing, GuiChainCtrl));
	addField("IsVertical", TypeBool, Offset(mIsVertical, GuiChainCtrl));
	endGroup("Gui Chain Settings");
}

//------------------------------------------------------------------------------
void GuiChainCtrl::inspectPreApply()
{
	mPrevIsVertical = mIsVertical;
	Parent::inspectPreApply();
}

void GuiChainCtrl::inspectPostApply()
{
	if (mPrevIsVertical != mIsVertical && isEditMode())
	{
		//Since we're in the editor, we'll swap x and y, then calculateExtent will fix one of them.
		iterator i;
		for (i = begin(); i != end(); i++)
		{
			GuiControl* ctrl = static_cast<GuiControl*>(*i);
			if (ctrl->isVisible())
			{
				S32 temp = ctrl->mBounds.point.x;
				ctrl->mBounds.point.x = ctrl->mBounds.point.y;
				ctrl->mBounds.point.y = temp;
			}
		}
	}
	calculateExtent();
	Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void GuiChainCtrl::childResized(GuiControl *child)
{
	calculateExtent(isEditMode());
	Parent::childResized(child);
}

void GuiChainCtrl::childMoved(GuiControl* child)
{
	calculateExtent(isEditMode());
	Parent::childMoved(child);
}

void GuiChainCtrl::childrenReordered()
{
	calculateExtent(isEditMode());
	Parent::childrenReordered();
}

void GuiChainCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	if (!mResizeGuard)
	{
		mResizeGuard = true;
		Point2I actualNewExtent = newExtent;
		if (mIsVertical)
		{
			actualNewExtent.y = getExtent().y;
		}
		else
		{
			actualNewExtent.x = getExtent().x;
		}
		if(newPosition != getPosition() || actualNewExtent != getExtent())
		{
			Parent::resize(newPosition, actualNewExtent);
		}
		mResizeGuard = false;
	}
}

void GuiChainCtrl::onChildAdded(GuiControl *child)
{
	//Ensure the child isn't positioned to the center
	if (child->getHorizSizing() == horizResizeCenter && !mIsVertical)
	{
		child->setHorizSizing(horizResizeLeft);
	}
	if (child->getVertSizing() == vertResizeCenter && mIsVertical)
	{
		child->setVertSizing(vertResizeTop);
	}

	if (isEditMode())
	{
		child->mBounds.point = Point2I::Zero;
	}

	Parent::onChildAdded(child);
	calculateExtent();
}

void GuiChainCtrl::onChildRemoved(SimObject *child)
{
	calculateExtent();
}

void GuiChainCtrl::calculateExtent(bool holdLength)
{
	Point2I offset = Point2I(mBounds.point.Zero);
	Point2I extent = Point2I(getExtent());
	RectI innerRect = getInnerRect(offset, extent, NormalState, mProfile);

	S32 length = positionChildren(innerRect);
	S32 oldLength = mIsVertical ? innerRect.extent.y : innerRect.extent.x;

	if (isEditMode())
	{
		length += mEditOpenSpace;//This gives some space to add new controls.
	}

	if (holdLength && oldLength > length)
	{
		length = oldLength;
	}

	if (!mIsVertical)
	{
		innerRect.extent.x = length;
	}
	else
	{
		innerRect.extent.y = length;
	}

	//call set update both before and after
	setUpdate();

	Point2I oldExtent = getExtent();
	mBounds.extent = getOuterExtent(innerRect.extent, NormalState, mProfile);
	GuiControl *parent = getParent();

	if(oldExtent != getExtent() && parent)
	{
		parent->childResized(this);
	}
	setUpdate();
}

S32 GuiChainCtrl::positionChildren(RectI &innerRect)
{
	iterator i;
	S32 length = 0;
	for (i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		if (ctrl->isVisible() || smDesignTime)
		{
			if (length != 0)
			{
				length += mChildSpacing;
			}
			Point2I childPos = ctrl->getPosition();
			if (!mIsVertical)
			{
				childPos.x = length;
			}
			else
			{
				childPos.y = length;
			}
			ctrl->mBounds.point = childPos;
			length += mIsVertical ? ctrl->getExtent().y : ctrl->getExtent().x;
		}
	}

	return length;
}

void GuiChainCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	Parent::onRender(offset, updateRect);

	if (isEditMode())
	{
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		const GuiControl* addSet = edit->getAddSet();
		SimSet& selectSet = edit->getSelectedSet();
		if(this == addSet || selectSet.isMember(this))
		{
			ColorI fill = edit->getEditorColor();
			fill.alpha = 100;

			RectI rect;
			if (mIsVertical)
			{
				rect = RectI(0, mBounds.extent.y - mEditOpenSpace, mBounds.extent.x, mEditOpenSpace);
			}
			else 
			{
				rect = RectI(mBounds.extent.x - mEditOpenSpace, 0, mEditOpenSpace, mBounds.extent.y);
			}
			rect.point = localToGlobalCoord(rect.point);
			dglDrawRectFill(rect, fill);
			dglSetBitmapModulation(getFontColor(edit->mProfile, NormalState));
			F32 tempAdjust = mFontSizeAdjust;
			mFontSizeAdjust = 1.5f;
			renderText(rect.point, rect.extent, "+", edit->mProfile);
			mFontSizeAdjust = tempAdjust;
		}
	}
}