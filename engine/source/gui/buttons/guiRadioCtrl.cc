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

#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/buttons/guiRadioCtrl.h"
#include "console/consoleTypes.h"
#include "gui/guiDefaultControlRender.h"

//---------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiRadioCtrl);

GuiRadioCtrl::GuiRadioCtrl()
{
}


void GuiRadioCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("groupNum", TypeS32, Offset(mRadioGroup, GuiRadioCtrl));
}

void GuiRadioCtrl::renderInnerControl(RectI &boxRect, const GuiControlState currentState)
{
	U8 stateIndex = currentState;

	if (mProfile->usesAssetRendering(currentState) || mProfile->usesBitmapRendering(currentState))
	{
		renderUniversalRect(boxRect, mProfile, currentState);
	}
	else 
	{
		S32 radius = boxRect.extent.x;
		if (boxRect.extent.y < radius)
		{
			radius = boxRect.extent.y;
		}
		radius = (S32)round(radius / 2);
		Point2I center = Point2I(boxRect.point.x + (boxRect.extent.x / 2), boxRect.point.y + (boxRect.extent.y / 2));
		renderBorderedCircle(center, radius, mProfile, currentState);
	}
}

void GuiRadioCtrl::onAction()
{
	if (!mActive)
		return;

	mStateOn = false;

	Parent::onAction();

	messageSiblings(mRadioGroup);
}

void GuiRadioCtrl::setStateOn(bool bStateOn)
{
	if (!mActive)
		return;

	mStateOn = true;
	messageSiblings(mRadioGroup);

	setUpdate();
}

void GuiRadioCtrl::onMessage(GuiControl *sender, S32 msg)
{
	Parent::onMessage(sender, msg);
	if (mRadioGroup == msg)
	{
		mStateOn = (sender == this);
		setUpdate();
	}
}