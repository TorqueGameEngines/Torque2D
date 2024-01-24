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

#include "gui/guiColorPopupCtrl.h"
#include "gui/guiCanvas.h"
#include "console/consoleTypes.h"

#include "guiColorPopupCtrl_ScriptBinding.h"

ColorI blankColor(0,0,0,0);

#pragma region GuiColorPopupBGCtrl
GuiColorPopupBGCtrl::GuiColorPopupBGCtrl(GuiColorPopupCtrl* ctrl)
{
	mColorPopupCtrl = ctrl;
	mBounds.point.set(0, 0);
	setField("profile", "GuiDefaultProfile");
}

void GuiColorPopupBGCtrl::onTouchUp(const GuiEvent& event)
{
	mColorPopupCtrl->closeColorPopup();
}
#pragma endregion

#pragma region GuiColorPopupContentCtrl
GuiColorPopupContentCtrl::GuiColorPopupContentCtrl()
{
	setField("profile", "GuiDefaultProfile");
}

void GuiColorPopupContentCtrl::onTouchUp(const GuiEvent& event) { }
#pragma endregion

#pragma region GuiColorPopupBlendCtrl
GuiColorPopupBlendCtrl::GuiColorPopupBlendCtrl(GuiColorPopupCtrl* ctrl, GuiColorPopupAlphaCtrl* alpha)
{
	mColorPopupCtrl = ctrl;
	mAlphaCtrl = alpha;
	setField("profile", "GuiDefaultProfile");
}

void GuiColorPopupBlendCtrl::updatePickColor(const Point2I& offset, const RectI& contentRect)
{
	if(mPositionChanged)
	{
		GuiColorPickerCtrl::updatePickColor(offset, contentRect);
		mColorPopupCtrl->setColor(mPickColor);
		mAlphaCtrl->setValue(mPickColor);
	}
}
#pragma endregion

#pragma region GuiColorPopupHueCtrl
GuiColorPopupHueCtrl::GuiColorPopupHueCtrl(GuiColorPopupBlendCtrl* ctrl)
{
	mBlendCtrl = ctrl;
	setField("profile", "GuiDefaultProfile");
}

void GuiColorPopupHueCtrl::updatePickColor(const Point2I& offset, const RectI& contentRect)
{
	if (mPositionChanged)
	{
		GuiColorPickerCtrl::updatePickColor(offset, contentRect);
		mBlendCtrl->setValue(mPickColor);
		mBlendCtrl->updateColor();
	}
}
#pragma endregion

#pragma region GuiColorPopupAlphaCtrl
GuiColorPopupAlphaCtrl::GuiColorPopupAlphaCtrl(GuiColorPopupCtrl* ctrl)
{
	mColorPopupCtrl = ctrl;
	setField("profile", "GuiDefaultProfile");
}

void GuiColorPopupAlphaCtrl::updatePickColor(const Point2I& offset, const RectI& contentRect)
{
	if (mPositionChanged)
	{
		GuiColorPickerCtrl::updatePickColor(offset, contentRect);
		mColorPopupCtrl->setAlpha(mPickColor.alpha);
	}
}
#pragma endregion

IMPLEMENT_CONOBJECT(GuiColorPopupCtrl);

GuiColorPopupCtrl::GuiColorPopupCtrl()
{
	mBounds.extent.set(140, 24);
	mIsOpen = false;
	mActive = true;
	mIsContainer = false;
	mBaseColor = ColorF(0.5f, 0.5f, 0.5f);
	mPopupSize = Point2I(240, 208);
	mBarHeight = 20;
	mShowAlphaBar = true;
	mBounds.extent.set(40, 40);

	setField("profile", "GuiColorPopupProfile");

	mBackground = new GuiColorPopupBGCtrl(this);
	AssertFatal(mBackground, "GuiColorPopupCtrl: Failed to initialize GuiColorPopupBGCtrl!");
	mBackgroundProfile = mBackground->mProfile;
	mBackgroundProfile->incRefCount();

	mContent = new GuiColorPopupContentCtrl();
	AssertFatal(mContent, "GuiColorPopupCtrl: Failed to initialize GuiControl!");
	mContent->setField("profile", "GuiPanelProfile");
	mPopupProfile = mContent->mProfile;
	mPopupProfile->incRefCount();
	mContent->setExtent(mPopupSize);
	RectI contentRect = mContent->getInnerRect();

	mColorAlphaPicker = new GuiColorPopupAlphaCtrl(this);
	AssertFatal(mColorAlphaPicker, "GuiColorPopupCtrl: Failed to initialize GuiColorPopupAlphaCtrl!");
	mColorAlphaPicker->setField("profile", "GuiColorPickerProfile");
	mColorAlphaPicker->setField("displayMode", "horizAlpha");
	mColorAlphaPicker->setExtent(Point2I(contentRect.extent.x, mBarHeight));
	mColorAlphaPicker->showSelector();
	mPickerProfile = mColorAlphaPicker->mProfile;
	mPickerProfile->incRefCount();

	mColorAlphaPicker->setField("selectorProfile", "GuiColorSelectorProfile");
	mSelectorProfile = mColorAlphaPicker->mSelectorProfile;
	mSelectorProfile->incRefCount();

	mColorBlendPicker = new GuiColorPopupBlendCtrl(this, mColorAlphaPicker);
	AssertFatal(mColorBlendPicker, "GuiColorPopupCtrl: Failed to initialize GuiColorPopupBlendCtrl!");
	mColorBlendPicker->setField("profile", "GuiColorPickerProfile");
	mColorBlendPicker->setField("displayMode", "blendColor");
	mColorBlendPicker->setExtent(Point2I(contentRect.extent.x, 100));
	mColorBlendPicker->showSelector();
	mPickerProfile = mColorBlendPicker->mProfile;
	mPickerProfile->incRefCount();

	mColorBlendPicker->setField("selectorProfile", "GuiColorSelectorProfile");
	mSelectorProfile = mColorBlendPicker->mSelectorProfile;
	mSelectorProfile->incRefCount();

	mColorHuePicker = new GuiColorPopupHueCtrl(mColorBlendPicker);
	AssertFatal(mColorHuePicker, "GuiColorPopupCtrl: Failed to initialize GuiColorPopupHueCtrl!");
	mColorHuePicker->setField("profile", "GuiColorPickerProfile");
	mColorHuePicker->setField("displayMode", "horizColor");
	mColorHuePicker->setExtent(Point2I(contentRect.extent.x, mBarHeight));
	mColorHuePicker->showSelector();
	mPickerProfile = mColorHuePicker->mProfile;
	mPickerProfile->incRefCount();

	mColorHuePicker->setField("selectorProfile", "GuiColorSelectorProfile");
	mSelectorProfile = mColorHuePicker->mSelectorProfile;
	mSelectorProfile->incRefCount();

	mContent->addObject(mColorBlendPicker);
	mContent->addObject(mColorHuePicker);
	mContent->addObject(mColorAlphaPicker);
	mBackground->addObject(mContent);
}

void GuiColorPopupCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("Color Popup");
	addField("backgroundProfile", TypeGuiProfile, Offset(mBackgroundProfile, GuiColorPopupCtrl));
	addField("popupProfile", TypeGuiProfile, Offset(mPopupProfile, GuiColorPopupCtrl));
	addField("pickerProfile", TypeGuiProfile, Offset(mPickerProfile, GuiColorPopupCtrl));
	addField("selectorProfile", TypeGuiProfile, Offset(mSelectorProfile, GuiColorPopupCtrl));
	addField("baseColor", TypeColorF, Offset(mBaseColor, GuiColorPopupCtrl));
	addField("popupSize", TypePoint2I, Offset(mPopupSize, GuiColorPopupCtrl));
	addField("barHeight", TypeS32, Offset(mBarHeight, GuiColorPopupCtrl));
	addField("showAlphaBar", TypeBool, Offset(mShowAlphaBar, GuiColorPopupCtrl));
	endGroup("Color Popup");
}

void GuiColorPopupCtrl::onTouchUp(const GuiEvent& event)
{
	if (!mActive)
		return;

	Parent::onTouchUp(event);

	mouseUnlock();

	if (!mIsOpen)
	{
		openColorPopup();
	}
	else
	{
		closeColorPopup();
	}
}

GuiControlState GuiColorPopupCtrl::getCurrentState()
{
	if (!mActive)
		return GuiControlState::DisabledState;
	else if (mDepressed || mIsOpen)
		return GuiControlState::SelectedState;
	else if (mMouseOver)
		return GuiControlState::HighlightState;
	else
		return GuiControlState::NormalState;
}

void GuiColorPopupCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	GuiControlState currentState = getCurrentState();
	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	renderUniversalRect(ctrlRect, mProfile, currentState, blankColor, true);

	//Get the content area
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);

	if (mBaseColor.alpha < 1.0f)
	{
		if (mBounds.extent.x < 50 || mBounds.extent.y < 50)
		{
			dglRenderCheckers(contentRect, 8);
		}
		else 
		{
			dglRenderCheckers(contentRect);
		}
	}
	dglDrawRectFill(contentRect, mBaseColor);
}

bool GuiColorPopupCtrl::onKeyDown(const GuiEvent& event)
{
	//if the control is a dead end, don't process the input:
	if (!mVisible || !mActive || !mAwake)
		return false;

	//see if the key down is a <return> or not
	if (event.keyCode == KEY_RETURN && event.modifier == 0)
	{
		if (!mIsOpen)
		{
			openColorPopup();
		}
		else
		{
			closeColorPopup();
		}
		return true;
	}

	return false;
}

void GuiColorPopupCtrl::onAction() //called when the button is clicked.
{
	if (!mActive)
		return;

	setUpdate();
}

void GuiColorPopupCtrl::openColorPopup()
{
	if (mIsOpen)
		return;

	GuiCanvas* root = getRoot();
	AssertFatal(root, "GuiColorPopupCtrl::openColorPopup: Unable to optain the Canvas!");
	mBackground->mBounds.extent = root->mBounds.extent;

	//Update all pass through values
	mBackground->setControlProfile(mBackgroundProfile);
	mContent->setControlProfile(mPopupProfile);
	mColorBlendPicker->setControlProfile(mPickerProfile);
	mColorBlendPicker->setControlSelectorProfile(mSelectorProfile);
	mColorHuePicker->setControlProfile(mPickerProfile);
	mColorHuePicker->setControlSelectorProfile(mSelectorProfile);
	mColorAlphaPicker->setControlProfile(mPickerProfile);
	mColorAlphaPicker->setControlSelectorProfile(mSelectorProfile);

	mContent->setExtent(mPopupSize);
	RectI contentRect = mContent->getInnerRect();
	mColorBlendPicker->setWidth(contentRect.extent.x);

	if (mShowAlphaBar)
	{
		mColorAlphaPicker->setActive(true);
		mColorAlphaPicker->setVisible(true);
		mColorAlphaPicker->resize(Point2I(0, contentRect.extent.y - mBarHeight), Point2I(contentRect.extent.x, mBarHeight));
		mColorHuePicker->resize(Point2I(0, contentRect.extent.y - (2 * mBarHeight)), Point2I(contentRect.extent.x, mBarHeight));
		mColorBlendPicker->setHeight(contentRect.extent.y - (2 * mBarHeight));
	}
	else
	{
		mColorAlphaPicker->setActive(false);
		mColorAlphaPicker->setVisible(false);
		mColorHuePicker->resize(Point2I(0, contentRect.extent.y - mBarHeight), Point2I(contentRect.extent.x, mBarHeight));
		mColorBlendPicker->setHeight(contentRect.extent.y - mBarHeight);
	}

	Point2I huePos = mColorHuePicker->getSelectorPositionForColor(mBaseColor);
	mColorHuePicker->setSelectorPos(huePos);
	ColorF hueColor = mBaseColor.getHueColor();
	mColorBlendPicker->setValue(hueColor);
	Point2I selPos = mColorBlendPicker->getSelectorPositionForColor(mBaseColor);
	mColorBlendPicker->setSelectorPos(selPos);
	Point2I alphaPos = mColorAlphaPicker->getSelectorPositionForColor(mBaseColor);
	mColorAlphaPicker->setSelectorPos(alphaPos);


	S32 width = mPopupSize.x;
	S32 height = mPopupSize.y;
	Point2I pos = localToGlobalCoord(Point2I(0, 0));

	//Is there enough space below?
	if ((height + pos.y + mBounds.extent.y) <= root->mBounds.extent.y)
	{
		pos.y += mBounds.extent.y;
	}
	else if (height <= pos.y) //Is there enough space above?
	{
		pos.y -= height;
	}
	else if (pos.y < (root->mBounds.extent.y - (pos.y + mBounds.extent.y))) //Is there more space below?
	{
		pos.y += mBounds.extent.y;
		height = root->mBounds.extent.y - pos.y;
	}
	else //There must be more space above
	{
		height = pos.y;
		pos.y = 0;
	}

	mContent->resize(pos, Point2I(width, height));

	root->pushDialogControl(mBackground, 99);

	mIsOpen = true;

	setFirstResponder();

	if (isMethod("onOpen"))
		Con::executef(this, 1, "onOpen");
}

void GuiColorPopupCtrl::closeColorPopup()
{
	if (!mIsOpen)
		return;

	GuiCanvas* root = mBackground->getRoot();
	if (!root)
	{
		return;
	}
	root->popDialogControl(mBackground);

	mIsOpen = false;

	if (isMethod("onClose"))
		Con::executef(this, 1, "onClose");

	if (mConsoleCommand[0])
		Con::evaluate(mConsoleCommand, false);
}

bool GuiColorPopupCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->incRefCount();

	if (mPopupProfile != NULL)
		mPopupProfile->incRefCount();

	if (mPickerProfile != NULL)
		mPickerProfile->incRefCount();

	if (mSelectorProfile != NULL)
		mSelectorProfile->incRefCount();

	return true;
}

void GuiColorPopupCtrl::onSleep()
{
	Parent::onSleep();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->decRefCount();

	if (mPopupProfile != NULL)
		mPopupProfile->decRefCount();

	if (mPickerProfile != NULL)
		mPickerProfile->decRefCount();

	if (mSelectorProfile != NULL)
		mSelectorProfile->decRefCount();
}

void GuiColorPopupCtrl::setControlBackgroundProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiColorPopupCtrl::setControlBackgroundProfile: invalid background profile");
	if (prof == mBackgroundProfile)
		return;
	if (mAwake)
		mBackgroundProfile->decRefCount();
	mBackgroundProfile = prof;
	if (mAwake)
		mBackgroundProfile->incRefCount();
}

void GuiColorPopupCtrl::setControlPopupProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiColorPopupCtrl::setControlPopupProfile: invalid popup profile");
	if (prof == mPopupProfile)
		return;
	if (mAwake)
		mPopupProfile->decRefCount();
	mPopupProfile = prof;
	if (mAwake)
		mPopupProfile->incRefCount();
}

void GuiColorPopupCtrl::setControlPickerProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiColorPopupCtrl::setControlPickerProfile: invalid picker profile");
	if (prof == mPickerProfile)
		return;
	if (mAwake)
		mPickerProfile->decRefCount();
	mPickerProfile = prof;
	if (mAwake)
		mPickerProfile->incRefCount();
}

void GuiColorPopupCtrl::setControlSelectorProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiColorPopupCtrl::setControlSelectorProfile: invalid selector profile");
	if (prof == mSelectorProfile)
		return;
	if (mAwake)
		mSelectorProfile->decRefCount();
	mSelectorProfile = prof;
	if (mAwake)
		mSelectorProfile->incRefCount();
}

void GuiColorPopupCtrl::setColor(const ColorF& theColor)
{
	mBaseColor.red = theColor.red;
	mBaseColor.green = theColor.green;
	mBaseColor.blue = theColor.blue;
	mBaseColor.alpha = theColor.alpha;
}

const char* GuiColorPopupCtrl::getScriptValue()
{
	static char temp[256];
	ColorF color = getValue();
	dSprintf(temp, 256, "%g %g %g %g", color.red, color.green, color.blue, color.alpha);
	return temp;
}
  
void GuiColorPopupCtrl::setScriptValue(const char* value)
{
	ColorF newValue;
	dSscanf(value, "%g %g %g %g", &newValue.red, &newValue.green, &newValue.blue, &newValue.alpha);
	setValue(newValue);
}