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
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/guiDefaultControlRender.h"

#include "guiButtonCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiButtonCtrl);

GuiButtonCtrl::GuiButtonCtrl()
{
	mDepressed = false;
	mMouseOver = false;
	mActive = true;
	mBounds.extent.set(140, 30);
	mText = StringTable->insert("Button");
	mTextID = StringTable->EmptyString;

	//fill color
	mEaseFillColorHL = EasingFunction::Linear;
	mEaseFillColorSL = EasingFunction::Linear;
	mEaseTimeFillColorHL = 500;
	mEaseTimeFillColorSL = 0;

	//control state
	mPreviousState = GuiControlState::DisabledState;
	mCurrentState = GuiControlState::DisabledState;
}

void GuiButtonCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("easeFillColorHL", TypeEnum, Offset(mEaseFillColorHL, GuiButtonCtrl), 1, &gEasingTable);
	addField("easeFillColorSL", TypeEnum, Offset(mEaseFillColorSL, GuiButtonCtrl), 1, &gEasingTable);
	addField("easeTimeFillColorHL", TypeS32, Offset(mEaseTimeFillColorHL, GuiButtonCtrl));
	addField("easeTimeFillColorSL", TypeS32, Offset(mEaseTimeFillColorSL, GuiButtonCtrl));
}

void GuiButtonCtrl::acceleratorKeyPress(U32)
{
	if (!mActive)
		return;

	//set the bool
	mDepressed = true;

	if (mProfile->mTabable)
		setFirstResponder();

	//update
	setUpdate();
}

void GuiButtonCtrl::acceleratorKeyRelease(U32)
{
	if (!mActive)
		return;

	if (mDepressed)
	{
		//set the bool
		mDepressed = false;
		//perform the action
		onAction();
	}

	//update
	setUpdate();
}

void GuiButtonCtrl::onTouchEnter(const GuiEvent &event)
{
	if (!mActive)
		return;

	mMouseOver = true;

	if (isMouseLocked())
	{
		mDepressed = true;
	}
	else if (mActive && mProfile->mSoundButtonOver)
	{
		AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonOver);
		alxPlay(handle);
	}

	Con::executef(this, 1, "onTouchEnter");

	//update
	setUpdate();
}

void GuiButtonCtrl::onTouchLeave(const GuiEvent &)
{
	if (!mActive)
		return;

	if (isMouseLocked())
		mDepressed = false;

	mouseUnlock();
	mMouseOver = false;

	Con::executef(this, 1, "onTouchLeave");

	//update
	setUpdate();
}

void GuiButtonCtrl::onTouchDown(const GuiEvent &event)
{
	if (!mActive)
		return;

	mDepressed = true;

	if (mProfile->mCanKeyFocus)
		setFirstResponder();

	if (mProfile->mSoundButtonDown)
	{
		AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
		alxPlay(handle);
	}

	//lock the mouse
	mouseLock();

	// Execute callback
	char buf[3][32];
	dSprintf(buf[0], 32, "%d", event.modifier);
	dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
	dSprintf(buf[2], 32, "%d", event.mouseClickCount);
	Con::executef(this, 4, "onTouchDown", buf[0], buf[1], buf[2]);

	//update
	setUpdate();
}

void GuiButtonCtrl::onTouchUp(const GuiEvent &event)
{
	if (!mActive)
		return;

	mouseUnlock();

	//if we released the mouse within this control, perform the action
	if (mDepressed)
		onAction();

	// Execute callback
	char buf[3][32];
	dSprintf(buf[0], 32, "%d", event.modifier);
	dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
	dSprintf(buf[2], 32, "%d", event.mouseClickCount);
	Con::executef(this, 4, "onTouchUp", buf[0], buf[1], buf[2]);

	mDepressed = false;

	//update
	setUpdate();
}

void GuiButtonCtrl::onRightMouseUp(const GuiEvent &event)
{
	if (!mActive)
		return;

	Con::executef(this, 1, "onRightClick");

	Parent::onRightMouseUp(event);
}

bool GuiButtonCtrl::onKeyDown(const GuiEvent &event)
{
	if (!mActive)
		return true;

	//see if the key down is a return or space or not
	if ((event.keyCode == KEY_RETURN || event.keyCode == KEY_SPACE)
		&& event.modifier == 0)
	{
		if (mProfile->mSoundButtonDown)
		{
			AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
			alxPlay(handle);
		}
		return true;
	}
	//otherwise, pass the event to it's parent
	return Parent::onKeyDown(event);
}

bool GuiButtonCtrl::onKeyUp(const GuiEvent &event)
{
	if (!mActive)
		return true;

	//see if the key down is a return or space or not
	if (mDepressed &&
		(event.keyCode == KEY_RETURN || event.keyCode == KEY_SPACE) &&
		event.modifier == 0)
	{
		onAction();
		return true;
	}

	//otherwise, pass the event to it's parent
	return Parent::onKeyUp(event);
}

void GuiButtonCtrl::onAction()
{
	if (!mActive)
		return;

	setUpdate();

	if (isMethod("onClick"))
		Con::executef(this, 2, "onClick");

	Parent::onAction();
}

GuiControlState GuiButtonCtrl::getCurrentState()
{
	if (!mActive)
		return GuiControlState::DisabledState;
	else if (mDepressed)
		return GuiControlState::SelectedState;
	else if (mMouseOver)
		return GuiControlState::HighlightState;
	else
	return GuiControlState::NormalState;
}

S32 GuiButtonCtrl::getBitmapIndex(const GuiControlState state)
{
	if (state == GuiControlState::HighlightState)
		return 2;
	else if (state == GuiControlState::SelectedState)
		return 3;
	else if (state == GuiControlState::DisabledState)
		return 4;
	else 
		return 1;
}

void GuiButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	GuiControlState currentState = getCurrentState();
	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	if(mProfile->mBitmapName != NULL && mProfile->constructBitmapArray() >= 36)
	{
		renderSizableBitmapBordersFilled(ctrlRect, getBitmapIndex(currentState), mProfile );
	}
	else
	{
		renderBorderedRect(ctrlRect, mProfile, currentState, getFillColor(currentState));
	}

	//Render Text
	dglSetBitmapModulation(mProfile->getFontColor(currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);
	renderText(contentRect.point, contentRect.extent, mText, mProfile);

	//Render the childen
	renderChildControls(offset, contentRect, updateRect);
}

void GuiButtonCtrl::setScriptValue(const char *value)
{
	// Update the console variable:
	if (mConsoleVariable[0])
		Con::setVariable(mConsoleVariable, value);

	setUpdate();
}

const char *GuiButtonCtrl::getScriptValue()
{
	return StringTable->EmptyString;
}

void GuiButtonCtrl::onMessage(GuiControl *sender, S32 msg)
{
	Parent::onMessage(sender, msg);
}

const ColorI& GuiButtonCtrl::getFillColor(const GuiControlState state)
{
	if (state != mCurrentState)
	{
		//We have just switched states!
		mPreviousState = mCurrentState;
		mCurrentState = state;
		if (mCurrentState == GuiControlState::DisabledState || mPreviousState == GuiControlState::DisabledState)
		{
			mFluidFillColor.stopFluidAnimation();
			mFluidFillColor.set(mProfile->getFillColor(state));
		}
		else if (mCurrentState == GuiControlState::SelectedState || mPreviousState == GuiControlState::SelectedState)
		{
			mFluidFillColor.setEasingFunction(mEaseFillColorSL);
			mFluidFillColor.setAnimationLength(mEaseTimeFillColorSL);
			mFluidFillColor.startFluidAnimation(mProfile->getFillColor(state));
		}
		else if (mCurrentState == GuiControlState::HighlightState || mPreviousState == GuiControlState::HighlightState)
		{
			mFluidFillColor.setEasingFunction(mEaseFillColorHL);
			mFluidFillColor.setAnimationLength(mEaseTimeFillColorHL);
			mFluidFillColor.startFluidAnimation(mProfile->getFillColor(state));
		}
		else
		{
			//we should never get here...
			mFluidFillColor.stopFluidAnimation();
			mFluidFillColor.set(mProfile->getFillColor(state));
		}
	}

	if (mFluidFillColor.isAnimating() && !isProcessingTicks())
	{
		setProcessTicks(true);
	}

	if (!mFluidFillColor.isAnimating())
	{
		mFluidFillColor.set(mProfile->getFillColor(state));
	}

	return mFluidFillColor;
}

void GuiButtonCtrl::processTick()
{
	bool shouldWeContinue = false;

	shouldWeContinue |= mFluidFillColor.processTick();

	if (!shouldWeContinue)
	{
		setProcessTicks(false);
	}
}

void GuiButtonCtrl::setControlProfile(GuiControlProfile *prof)
{
	Parent::setControlProfile(prof);
	mCurrentState = mCurrentState == DisabledState ? NormalState : DisabledState;
}