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

#include "guiChainCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiChainCtrl);

//------------------------------------------------------------------------------

GuiChainCtrl::GuiChainCtrl()
{
	mIsContainer = true;
	mChildSpacing = 0;
	mIsVertical = true;
}

//------------------------------------------------------------------------------

void GuiChainCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("ChildSpacing", TypeS32, Offset(mChildSpacing, GuiChainCtrl));
	addField("IsVertical", TypeBool, Offset(mIsVertical, GuiChainCtrl));
}

//------------------------------------------------------------------------------

void GuiChainCtrl::inspectPostApply()
{
	calculateExtent();
	Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void GuiChainCtrl::childResized(GuiControl *child)
{
	calculateExtent();
}

void GuiChainCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
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
}

void GuiChainCtrl::onChildAdded(GuiControl *child)
{
	Parent::onChildAdded(child);
	calculateExtent();
}

void GuiChainCtrl::onChildRemoved(SimObject *child)
{
	calculateExtent();
}

void GuiChainCtrl::calculateExtent()
{
	RectI innerRect = getInnerRect(Point2I(mBounds.point.Zero), Point2I(getExtent()), NormalState, mProfile);

	S32 length = positionChildren(innerRect);

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
		if (ctrl->isVisible())
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
			ctrl->setPosition(childPos);
			length += mIsVertical ? ctrl->getExtent().y : ctrl->getExtent().x;
		}
	}

	return length;
}