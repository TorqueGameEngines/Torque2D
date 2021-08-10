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
#include "gui/buttons/guiCheckBoxCtrl.h"
#include "console/consoleTypes.h"
#include "gui/guiDefaultControlRender.h"

#include "guiCheckBoxCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiCheckBoxCtrl);

//---------------------------------------------------------------------------
GuiCheckBoxCtrl::GuiCheckBoxCtrl()
{
   mBounds.extent.set(140, 30);
   mStateOn = false;
   mBoxOffset.set(0,7);
   mBoxExtent.set(16, 16);
   mTextOffset.set(22, 7);
   mTextExtent.set(mBounds.extent.x - mTextOffset.x, mBounds.extent.y - mTextOffset.y);
}

//---------------------------------------------------------------------------

void GuiCheckBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("stateOn", TypeBool, Offset(mStateOn, GuiCheckBoxCtrl));
   addField("boxOffset", TypePoint2I, Offset(mBoxOffset, GuiCheckBoxCtrl));
   addField("boxExtent", TypePoint2I, Offset(mBoxExtent, GuiCheckBoxCtrl));
   addField("textOffset", TypePoint2I, Offset(mTextOffset, GuiCheckBoxCtrl));
   addField("textExtent", TypePoint2I, Offset(mTextExtent, GuiCheckBoxCtrl));
}

bool GuiCheckBoxCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   return true;
}

void GuiCheckBoxCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	GuiControlState currentState = mStateOn ? GuiControlState::NormalStateOn : GuiControlState::NormalState;
	if (!mActive)
	{
		currentState = mStateOn ? GuiControlState::DisabledStateOn : GuiControlState::DisabledState;
	}
	else if (mDepressed)
	{
		currentState = mStateOn ? GuiControlState::SelectedStateOn : GuiControlState::SelectedState;
	}
	else if (mMouseOver)
	{
		currentState = mStateOn ? GuiControlState::HighlightStateOn : GuiControlState::HighlightState;
	}

	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);
	RectI boxRect(contentRect.point + mBoxOffset, mBoxExtent);

	//Contrain the Box Rect. It must fit in the content Rect.
	if ((boxRect.point.x + boxRect.extent.x) > (contentRect.point.x + contentRect.extent.x))
	{
		boxRect.extent.x = contentRect.point.x + contentRect.extent.x - boxRect.point.x;
	}
	if ((boxRect.point.y + boxRect.extent.y) > (contentRect.point.y + contentRect.extent.y))
	{
		boxRect.extent.y = contentRect.point.y + contentRect.extent.y - boxRect.point.y;
	}
	
	//Draw the checkbox
	renderInnerControl(boxRect, currentState);

	RectI textRect(contentRect.point + mTextOffset, mTextExtent);

	//Contrain the Text Rect. It must fit in the content Rect.
	if ((textRect.point.x + textRect.extent.x) > (contentRect.point.x + contentRect.extent.x))
	{
		textRect.extent.x = contentRect.point.x + contentRect.extent.x - textRect.point.x;
	}
	if ((textRect.point.y + textRect.extent.y) > (contentRect.point.y + contentRect.extent.y))
	{
		textRect.extent.y = contentRect.point.y + contentRect.extent.y - textRect.point.y;
	}

	//Render Text
	dglSetBitmapModulation(mProfile->getFontColor(currentState));
	renderText(textRect.point, textRect.extent, mText, mProfile);

	//Render the childen
	renderChildControls(offset, contentRect, updateRect);
}

void GuiCheckBoxCtrl::renderInnerControl(RectI &boxRect, const GuiControlState currentState)
{
	renderUniversalRect(boxRect, mProfile, currentState);
}

void GuiCheckBoxCtrl::onAction()
{
	if (!mActive)
		return;

	mStateOn = !mStateOn;

	Parent::onAction();
}

void GuiCheckBoxCtrl::setStateOn(bool bStateOn)
{
	if (!mActive)
		return;

	mStateOn = bStateOn;
	
	setUpdate();
}

void GuiCheckBoxCtrl::setScriptValue(const char *value)
{
	mStateOn = dAtob(value);

	setUpdate();
}

const char *GuiCheckBoxCtrl::getScriptValue()
{
	return mStateOn ? "1" : "0";
}

void GuiCheckBoxCtrl::onMessage(GuiControl *sender, S32 msg)
{
	Parent::onMessage(sender, msg);
}