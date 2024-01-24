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

#ifndef _GUICOLORPOPUPCTRL_H_
#define _GUICOLORPOPUPCTRL_H_

#include "gui/guiControl.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/guiColorPickerCtrl.h"
#include "graphics/dgl.h"
#include "gui/guiDefaultControlRender.h"

class GuiColorPopupCtrl;

class GuiColorPopupBGCtrl : public GuiControl
{
protected:
	GuiColorPopupCtrl* mColorPopupCtrl;
public:
	GuiColorPopupBGCtrl(GuiColorPopupCtrl* ctrl);
	void onTouchUp(const GuiEvent& event);
};

class GuiColorPopupContentCtrl : public GuiControl
{
public:
	GuiColorPopupContentCtrl();
	void onTouchUp(const GuiEvent& event);
};

class GuiColorPopupAlphaCtrl : public GuiColorPickerCtrl
{
protected:
	GuiColorPopupCtrl* mColorPopupCtrl;
public:
	GuiColorPopupAlphaCtrl(GuiColorPopupCtrl* ctrl);
	void updatePickColor(const Point2I& offset, const RectI& contentRect);
};

class GuiColorPopupBlendCtrl : public GuiColorPickerCtrl
{
protected:
	GuiColorPopupCtrl* mColorPopupCtrl;
	GuiColorPopupAlphaCtrl* mAlphaCtrl;
public:
	GuiColorPopupBlendCtrl(GuiColorPopupCtrl* ctrl, GuiColorPopupAlphaCtrl* alpha);
	void updatePickColor(const Point2I& offset, const RectI& contentRect);
};

class GuiColorPopupHueCtrl : public GuiColorPickerCtrl
{
protected:
	GuiColorPopupBlendCtrl* mBlendCtrl;
public:
	GuiColorPopupHueCtrl(GuiColorPopupBlendCtrl* ctrl);
	void updatePickColor(const Point2I& offset, const RectI& contentRect);
};

class GuiColorPopupCtrl : public GuiButtonCtrl
{
private:
	typedef GuiButtonCtrl Parent;
	ColorF mBaseColor;
	bool mIsOpen;
	Point2I mPopupSize;
	S32 mBarHeight;
	bool mShowAlphaBar;

	GuiColorPopupBGCtrl* mBackground;
	GuiControl* mContent;
	GuiColorPopupBlendCtrl* mColorBlendPicker;
	GuiColorPopupHueCtrl* mColorHuePicker;
	GuiColorPopupAlphaCtrl* mColorAlphaPicker;

	GuiControlProfile* mBackgroundProfile; //Used to render the background when the drop down is open
	GuiControlProfile* mPopupProfile; //Used for the content box of the popup
	GuiControlProfile* mPickerProfile; //Used for the three color pickers used in the popup
	GuiControlProfile* mSelectorProfile; //Used for the selectors in the popup

protected:

public:
	GuiColorPopupCtrl();
	static void initPersistFields();

	virtual void onTouchUp(const GuiEvent& event);
	GuiControlState getCurrentState();
	void onRender(Point2I offset, const RectI& updateRect);
	void setPopupSize(const Point2I& size) { mPopupSize.set(size.x, size.y); }

	bool onKeyDown(const GuiEvent& event);
	virtual void onAction();
	void openColorPopup();
	void closeColorPopup();

	bool onWake();
	void onSleep();
	void setControlBackgroundProfile(GuiControlProfile* prof);
	void setControlPopupProfile(GuiControlProfile* prof);
	void setControlPickerProfile(GuiControlProfile* prof);
	void setControlSelectorProfile(GuiControlProfile* prof);

	void setColor(const ColorF& theColor);
	void setAlpha(const F32 alpha) { mBaseColor.alpha = alpha; }
	void setValue(ColorF& value) { mBaseColor = value; }
	ColorF getValue() { return mBaseColor; }
	const char* getScriptValue();
	void setScriptValue(const char* value);

	DECLARE_CONOBJECT(GuiColorPopupCtrl);
};

#endif