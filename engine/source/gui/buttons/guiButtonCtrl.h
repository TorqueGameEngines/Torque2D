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

#ifndef _GUIBUTTONCTRL_H_
#define _GUIBUTTONCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

class GuiButtonCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:
	bool mDepressed;
	bool mMouseOver;
	FluidColorI mFluidFillColor; //The actual fill color as it moves fluidly from one color to another.
	GuiControlState mPreviousState;
	GuiControlState mCurrentState;
	GuiControlState getCurrentState();
	S32 getBitmapIndex(const GuiControlState state);

public:
	GuiButtonCtrl();
	static void initPersistFields();

	DECLARE_CONOBJECT(GuiButtonCtrl);

	EasingFunction mEaseFillColorHL; //Transitioning to or from HL (if SL is not involved)
	EasingFunction mEaseFillColorSL; //Transitioning to or from SL (over HL)

	S32 mEaseTimeFillColorHL;
	S32 mEaseTimeFillColorSL;

	void acceleratorKeyPress(U32 index);
	void acceleratorKeyRelease(U32 index);

	virtual void onTouchDown(const GuiEvent &);
	virtual void onTouchUp(const GuiEvent &);
	virtual void onRightMouseUp(const GuiEvent &);

	virtual void onTouchEnter(const GuiEvent &);
	virtual void onTouchLeave(const GuiEvent &);

	virtual bool onKeyDown(const GuiEvent &event);
	virtual bool onKeyUp(const GuiEvent &event);

	virtual void setScriptValue(const char *value);
	virtual const char *getScriptValue();

	virtual void onMessage(GuiControl *, S32 msg);
	virtual void onAction();
   
	virtual void onRender(Point2I offset, const RectI &updateRect);
	virtual void setControlProfile(GuiControlProfile *prof);

	const ColorI& getFillColor(const GuiControlState state); //Returns the fill color based on the state.
	virtual void processTick();

	inline const char* getEasingFunctionDescription(const EasingFunction ease) { return mFluidFillColor.getEasingFunctionDescription(ease); };
};

#endif //_GUI_BUTTON_CTRL_H
