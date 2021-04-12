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
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "gui/guiDefaultControlRender.h"

#include "gui/guiProgressCtrl.h"

#include "gui/guiProgressCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiProgressCtrl);

GuiProgressCtrl::GuiProgressCtrl()
{
	mBounds.extent.set(230, 24);
   mCurrent = 0.0f;
   mStart = 0.0f;
   mEnd = 0.0f;

   setAnimationLength(250);
   setEasingFunction(EaseInOut);

   setField("profile", "GuiProgressProfile");
}

void GuiProgressCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("animationTime", TypeS32, Offset(mAnimationLength, GuiProgressCtrl));
}

const char* GuiProgressCtrl::getScriptValue()
{
   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%g", mCurrent);
   return ret;
}

void GuiProgressCtrl::setScriptValue(const char *value)
{
   //set the value
   F32 target = 0.0f;
   if (value)
	   target = dAtof(value);

	setCurrentProgress(target);
}

void GuiProgressCtrl::setCurrentProgress(F32 target)
{
	setCurrentProgress(target, mAnimationLength);
}

void GuiProgressCtrl::setCurrentProgress(F32 target, S32 time)
{
	mAnimationLength = time;

   //validate the value
   target = mClampF(target, 0.f, 1.f);

   if (target != mCurrent)
   {
		mStart = mCurrent;
		mEnd = target;
		startFluidAnimation();
		setProcessTicks(true);
   }

   setUpdate();
}

void GuiProgressCtrl::resetProgress()
{
	setCurrentProgress(0.0f);
}

void GuiProgressCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	//Render the background box
	RectI backRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);
	renderUniversalRect(backRect, mProfile, NormalState);

	if (mCurrent == 0.0f)
	{
		return;
	}

	GuiControlState currentState = SelectedState;
	if(mCurrent < 1.0f)
	{
		currentState = HighlightState;
	}

	//Render the progress bar
	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);
	ctrlRect.extent.x = ctrlRect.extent.x * mCurrent;
	renderUniversalRect(ctrlRect, mProfile, currentState);

	//Render Text
	dglSetBitmapModulation(mProfile->getFontColor(currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);

	if (contentRect.isValidRect())
	{
		renderText(contentRect.point, contentRect.extent, mText, mProfile);
	}
}

void GuiProgressCtrl::processTick()
{
	bool shouldWeContinue = false;

	//Expanding
	shouldWeContinue |= animateProgressBar();

	if (!shouldWeContinue)
	{
		setProcessTicks(false);
	}
}

bool GuiProgressCtrl::animateProgressBar()
{
	F32 progress = getProgress(32.0f);

	setUpdate();
	mCurrent = processValue(progress, mStart, mEnd);

	if (mAnimationProgress >= 1.0f)
	{
		mAnimationProgress = 1.0f;
		return false;
	}
	return true;
}
	

