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

#include "gui/buttons/guiDropDownCtrl.h"
#include "gui/guiCanvas.h"

#include "guiDropDownCtrl_ScriptBinding.h"

#pragma region GuiDropDownBGCtrl
GuiDropDownBGCtrl::GuiDropDownBGCtrl(GuiDropDownCtrl *ctrl)
{
	mDropDownCtrl = ctrl;
	mBounds.point.set(0, 0);
	setField("profile", "GuiDefaultProfile");
}

void GuiDropDownBGCtrl::onTouchUp(const GuiEvent &event)
{
	mDropDownCtrl->closeDropDown();
}
#pragma endregion

#pragma region GuiDropDownListBoxCtrl
GuiDropDownListBoxCtrl::GuiDropDownListBoxCtrl(GuiDropDownCtrl *ctrl)
{
	mDropDownCtrl = ctrl;
	mBounds.point.set(0, 0);
	caller = ctrl;
}

void GuiDropDownListBoxCtrl::addSelection(LBItem *item, S32 index)
{
	Parent::addSelection(item, index);
	mDropDownCtrl->itemSelected();
}

void GuiDropDownListBoxCtrl::setCurSel(S32 index)
{
	Parent::setCurSel(index);
}

void GuiDropDownListBoxCtrl::onTouchUp(const GuiEvent &event)
{
	Parent::onTouchUp(event);
	mDropDownCtrl->closeDropDown();
}
#pragma endregion

IMPLEMENT_CONOBJECT(GuiDropDownCtrl);

GuiDropDownCtrl::GuiDropDownCtrl()
{
	mMaxHeight = 300;
	mBounds.extent.set(140, 24);
	mIsOpen = false;
	mActive = true;
	mText = StringTable->insert("none");

	setField("profile", "GuiDropDownProfile");

	mBackground = new GuiDropDownBGCtrl(this);
	AssertFatal(mBackground, "GuiDropDownCtrl: Failed to initialize GuiDropDownBGCtrl!");
	mBackgroundProfile = mBackground->mProfile;
	mBackgroundProfile->incRefCount();

	mListBox = new GuiDropDownListBoxCtrl(this);
	AssertFatal(mListBox, "GuiDropDownCtrl: Failed to initialize GuiDropDownListBoxCtrl!");
	mListBox->setField("profile", "GuiListBoxProfile");
	mListBoxProfile = mListBox->mProfile;
	mListBoxProfile->incRefCount();
	mListBox->setField("FitParentWidth", "1");
	mListBox->setField("AllowMultipleSelections", "0");

	mScroll = new GuiScrollCtrl();
	AssertFatal(mScroll, "GuiDropDownCtrl: Failed to initialize GuiScrollCtrl!");
	mScroll->setField("profile", "GuiScrollProfile");
	mScrollProfile = mScroll->mProfile;
	mScrollProfile->incRefCount();

	mScroll->setField("thumbProfile", "GuiScrollThumbProfile");
	mThumbProfile = mScroll->mThumbProfile;
	mThumbProfile->incRefCount();

	mScroll->setField("arrowProfile", "GuiScrollArrowProfile");
	mArrowProfile = mScroll->mArrowProfile;
	mArrowProfile->incRefCount();

	mScroll->setField("trackProfile", "GuiScrollTrackProfile");
	mTrackProfile = mScroll->mTrackProfile;
	mTrackProfile->incRefCount();

	mScroll->setField("hScrollBar", "AlwaysOff");
	mScroll->setField("vScrollBar", "Dynamic");
	mScroll->setField("constantThumbHeight", "0");
	mUseConstantHeightThumb = false;
	mScroll->setField("scrollBarThickness", "12");
	mScrollBarThickness = 12;
	mScroll->setField("showArrowButtons", "0");
	mShowArrowButtons = false;
	
	mScroll->addObject(mListBox);
	mBackground->addObject(mScroll);
}

void GuiDropDownCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("maxHeight", TypeBool, Offset(mMaxHeight, GuiDropDownCtrl));
   addField("scrollProfile", TypeGuiProfile, Offset(mScrollProfile, GuiDropDownCtrl));
   addField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiDropDownCtrl));
   addField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiDropDownCtrl));
   addField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiDropDownCtrl));
   addField("listBoxProfile", TypeGuiProfile, Offset(mListBoxProfile, GuiDropDownCtrl));
   addField("backgroundProfile", TypeGuiProfile, Offset(mBackgroundProfile, GuiDropDownCtrl));
   addField("constantThumbHeight", TypeBool, Offset(mUseConstantHeightThumb, GuiDropDownCtrl));
   addField("showArrowButtons", TypeBool, Offset(mShowArrowButtons, GuiDropDownCtrl));
   addField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiDropDownCtrl));
}

void GuiDropDownCtrl::onTouchUp(const GuiEvent &event)
{
	if (!mActive)
		return;

	Parent::onTouchUp(event);

	mouseUnlock();

	if (!mIsOpen)
	{
		openDropDown();
	}
	else
	{
		closeDropDown();
	}
}

GuiControlState GuiDropDownCtrl::getCurrentState()
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

void GuiDropDownCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	GuiControlState currentState = getCurrentState();
	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	renderUniversalRect(ctrlRect, mProfile, currentState, getFillColor(currentState), true);

	//Get the content area
	dglSetBitmapModulation(mProfile->getFontColor(currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);

	//Render the triangle
	if(mProfile->usesDefaultRendering(currentState))
	{
		RectI drawArea = RectI(contentRect.point.x + contentRect.extent.x - contentRect.extent.y, contentRect.point.y, contentRect.extent.y, contentRect.extent.y);
		ColorI color = ColorI(mProfile->getFontColor(currentState));
		renderTriangleIcon(drawArea, color, GuiDirection::Down, 8);
		contentRect.extent.x -= contentRect.extent.y;
	}

	//Render the text
	S32 index = mListBox->getSelectedItem();
	if (index == -1)
	{
		renderText(contentRect.point, contentRect.extent, mText, mProfile);
	}
	else
	{
		if (mListBox->getItemHasColor(index))
		{
			//Draw the bullet
			RectI drawArea = RectI(contentRect.point.x, contentRect.point.y, contentRect.extent.y, contentRect.extent.y);
			ColorI color = ColorI(mListBox->getItemColor(index));
			renderColorBullet(drawArea, color, 5);

			contentRect.point.x += contentRect.extent.y;
			contentRect.extent.x -= contentRect.extent.y;
		}
		renderText(contentRect.point, contentRect.extent, mListBox->getItemText(index), mProfile);
	}
}

bool GuiDropDownCtrl::onKeyDown(const GuiEvent &event)
{
	//if the control is a dead end, don't process the input:
	if (!mVisible || !mActive || !mAwake)
		return false;

	//see if the key down is a <return> or not
	if (event.keyCode == KEY_RETURN && event.modifier == 0)
	{
		if(!mIsOpen)
		{
			openDropDown();
		}
		else
		{
			closeDropDown();
		}
		return true;
	}
	else if (mIsOpen)
	{
		return mListBox->onKeyDown(event);
	}

	return false;
}

void GuiDropDownCtrl::onAction() //called when the button is clicked.
{
	if (!mActive)
		return;

	setUpdate();
}

void GuiDropDownCtrl::itemSelected()
{
	if (mConsoleCommand[0])
		Con::evaluate(mConsoleCommand, false);
}

void GuiDropDownCtrl::openDropDown()
{
	if (mIsOpen)
		return;

	GuiCanvas *root = getRoot();
	AssertFatal(root, "GuiDropDownCtrl::openDropDown: Unable to optain the Canvas!");
	mBackground->mBounds.extent = root->mBounds.extent;

	//Update all pass through values
	mBackground->setControlProfile(mBackgroundProfile);
	mListBox->setControlProfile(mListBoxProfile);
	mScroll->setControlProfile(mScrollProfile);
	mScroll->setControlThumbProfile(mThumbProfile);
	mScroll->setControlArrowProfile(mArrowProfile);
	mScroll->setControlTrackProfile(mTrackProfile);
	mScroll->mUseConstantHeightThumb = mUseConstantHeightThumb;
	mScroll->mScrollBarThickness = mScrollBarThickness;
	mScroll->mShowArrowButtons = mShowArrowButtons;

	//Set the size of the scroll control.
	mListBox->updateSize();
	S32 width = mClamp(mListBox->mBounds.extent.x, mBounds.extent.x, mBounds.extent.x * 2);
	S32 height = mClamp(mListBox->mBounds.extent.y, 10, mMaxHeight);

	Point2I pos = localToGlobalCoord(Point2I(0,0));

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

	mScroll->resize(pos, Point2I(width, height));

	root->pushDialogControl(mBackground, 99);

	mListBox->ScrollToIndex(mListBox->getSelectedItem());

	mIsOpen = true;

	setFirstResponder();

	if (isMethod("onOpen"))
		Con::executef(this, 1, "onOpen");
}

void GuiDropDownCtrl::closeDropDown()
{
	if(!mIsOpen)
		return;

	GuiCanvas *root = mBackground->getRoot();
	if (!root)
	{
		return;
	}
	root->popDialogControl(mBackground);

	mIsOpen = false;

	if (isMethod("onClose"))
		Con::executef(this, 1, "onClose");
}

bool GuiDropDownCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->incRefCount();

	if (mListBoxProfile != NULL)
		mListBoxProfile->incRefCount();

	if (mScrollProfile != NULL)
		mScrollProfile->incRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

	return true;
}

void GuiDropDownCtrl::onSleep()
{
	Parent::onSleep();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->decRefCount();

	if (mListBoxProfile != NULL)
		mListBoxProfile->decRefCount();

	if (mScrollProfile != NULL)
		mScrollProfile->decRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->decRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->decRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->decRefCount();
}

void GuiDropDownCtrl::setControlBackgroundProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlBackgroundProfile: invalid background profile");
	if (prof == mBackgroundProfile)
		return;
	if (mAwake)
		mBackgroundProfile->decRefCount();
	mBackgroundProfile = prof;
	if (mAwake)
		mBackgroundProfile->incRefCount();
}

void GuiDropDownCtrl::setControlListBoxProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlListBoxProfile: invalid list box profile");
	if (prof == mListBoxProfile)
		return;
	if (mAwake)
		mListBoxProfile->decRefCount();
	mListBoxProfile = prof;
	if (mAwake)
		mListBoxProfile->incRefCount();
}

void GuiDropDownCtrl::setControlScrollProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlScrollProfile: invalid scroll profile");
	if (prof == mScrollProfile)
		return;
	if (mAwake)
		mScrollProfile->decRefCount();
	mScrollProfile = prof;
	if (mAwake)
		mScrollProfile->incRefCount();
}

void GuiDropDownCtrl::setControlThumbProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlThumbProfile: invalid thumb profile");
	if (prof == mThumbProfile)
		return;
	if (mAwake)
		mThumbProfile->decRefCount();
	mThumbProfile = prof;
	if (mAwake)
		mThumbProfile->incRefCount();
}

void GuiDropDownCtrl::setControlTrackProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlTrackProfile: invalid track profile");
	if (prof == mTrackProfile)
		return;
	if (mAwake)
		mTrackProfile->decRefCount();
	mTrackProfile = prof;
	if (mAwake)
		mTrackProfile->incRefCount();
}

void GuiDropDownCtrl::setControlArrowProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiDropDownCtrl::setControlArrowProfile: invalid arrow profile");
	if (prof == mArrowProfile)
		return;
	if (mAwake)
		mArrowProfile->decRefCount();
	mArrowProfile = prof;
	if (mAwake)
		mArrowProfile->incRefCount();
}