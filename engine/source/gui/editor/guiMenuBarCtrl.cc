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
#include "gui/guiCanvas.h"
#include "input/actionMap.h"

#include "gui/editor/guiMenuBarCtrl.h"

#include "gui/editor/guiMenuBarCtrl_ScriptBinding.h"

#pragma region GuiMenuBarCtrl
IMPLEMENT_CONOBJECT(GuiMenuBarCtrl);

GuiMenuBarCtrl::GuiMenuBarCtrl()
{
	mBounds.point.set(0, 0);
	mBounds.extent.set(400, 26);
	mActive = true;
	mUseKeyMode = false;
	mAcceleratorKey = "alt";
	setField("profile", "GuiMenuBarProfile");

	mMenuProfile = NULL;
	setField("MenuProfile", "GuiMenuProfile");

	mMenuContentProfile = NULL;
	setField("MenuContentProfile", "GuiMenuContentProfile");

	mMenuItemProfile = NULL;
	setField("MenuItemProfile", "GuiMenuItemProfile");

	mThumbProfile = NULL;
	setField("ThumbProfile", "GuiScrollThumbProfile");

	mArrowProfile = NULL;
	setField("ArrowProfile", "GuiScrollArrowProfile");

	mTrackProfile = NULL;
	setField("TrackProfile", "GuiScrollTrackProfile");

	mHoverTarget = NULL;
	mOpenMenu = NULL;

	mUseConstantHeightThumb = false;
	mScrollBarThickness = 12;
	mShowArrowButtons = false;

	mBackground = new GuiMenuBGCtrl(this);
	AssertFatal(mBackground, "GuiDropDownCtrl: Failed to initialize GuiMenuBGCtrl!");
	mBackgroundProfile = mBackground->mProfile;
	mBackgroundProfile->incRefCount();
}

void GuiMenuBarCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("MenuProfile", TypeGuiProfile, Offset(mMenuProfile, GuiMenuBarCtrl));
	addField("MenuContentProfile", TypeGuiProfile, Offset(mMenuContentProfile, GuiMenuBarCtrl));
	addField("MenuItemProfile", TypeGuiProfile, Offset(mMenuItemProfile, GuiMenuBarCtrl));
	addField("backgroundProfile", TypeGuiProfile, Offset(mBackgroundProfile, GuiMenuBarCtrl));
	addField("constantThumbHeight", TypeBool, Offset(mUseConstantHeightThumb, GuiMenuBarCtrl));
	addField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiMenuBarCtrl));
	addField("showArrowButtons", TypeBool, Offset(mShowArrowButtons, GuiMenuBarCtrl));
	addField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiMenuBarCtrl));
	addField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiMenuBarCtrl));
	addField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiMenuBarCtrl));
}

void GuiMenuBarCtrl::inspectPostApply()
{
	ApplyMenuSettings();
	calculateMenus();
	Parent::inspectPostApply();
}

void GuiMenuBarCtrl::childResized(GuiControl *child)
{
	calculateMenus();
}

void GuiMenuBarCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	Parent::resize(Point2I(0, 0), newExtent);
}

void GuiMenuBarCtrl::onChildAdded(GuiControl *child)
{
	GuiMenuItemCtrl *menu = dynamic_cast<GuiMenuItemCtrl*>(child);
	if (!menu)
	{
		Con::warnf("GuiMenuBarCtrl::onChildAdded - Only a GuiMenuItemCtrl can be added to a GuiMenuBarCtrl! Child will not be added.");
		SimObject *simObj = reinterpret_cast<SimObject*>(child);
		removeObject(simObj);
		return;
	}
	menu->mMenuBar = this;
	menu->setControlProfile(mMenuProfile);
	calculateMenus();

	if (size() > 1)
	{
		iterator i = end() - 2;
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);
		menu->mPrevItem = ctrl;
		ctrl->mNextItem = menu;
	}
}

void GuiMenuBarCtrl::onChildRemoved(SimObject *child)
{
	calculateMenus();
}

void GuiMenuBarCtrl::calculateMenus()
{
	RectI innerRect = getInnerRect(Point2I(mBounds.point.Zero), Point2I(getExtent()), NormalState, mProfile);
	iterator i;
	S32 length = 0;
	for (i = begin(); i != end(); i++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		if (ctrl->isVisible())
		{
			S32 width = ctrl->mProfile->mFont->getStrWidth((const UTF8*)ctrl->getText());
			Point2I outerExtent = getOuterExtent(Point2I(width, 0), NormalState, ctrl->mProfile);
			ctrl->mBounds.set(Point2I(length, 0), Point2I(outerExtent.x, innerRect.extent.y));
			length += ctrl->getExtent().x;
		}
	}
}

GuiControl* GuiMenuBarCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
	return this;
}

GuiMenuItemCtrl* GuiMenuBarCtrl::findHitMenu(const Point2I &pt)
{
	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);
		if (ctrl && ctrl->isVisible() && ctrl->isActive())
		{
			if (ctrl->mBounds.pointInRect(pt - ctrl->mRenderInsetLT))
			{
				return ctrl;
			}
		}
	}
	return NULL;
}

void GuiMenuBarCtrl::processHover(const GuiEvent &event)
{
	GuiMenuItemCtrl *ctrl = findHitMenu(globalToLocalCoord(event.mousePoint));
	mUseKeyMode = false;
	setHoverTarget(ctrl);
}

void GuiMenuBarCtrl::setHoverTarget(GuiMenuItemCtrl *ctrl)
{
	if (mHoverTarget != ctrl)
	{
		if (mHoverTarget != NULL)
		{
			mHoverTarget->mIsHover = false;
		}
		if(ctrl != NULL)
		{
			ctrl->mIsHover = true;
		}
		mHoverTarget = ctrl;
	}
}

void GuiMenuBarCtrl::onTouchMove(const GuiEvent &event)
{
	processHover(event);
}

void GuiMenuBarCtrl::onTouchEnter(const GuiEvent &event)
{
	processHover(event);
}

void GuiMenuBarCtrl::onTouchLeave(const GuiEvent &event)
{
	if (mHoverTarget != NULL) {
		mHoverTarget->mIsHover = false;
	}
	mHoverTarget = NULL;
	clearFirstResponder();
}

void GuiMenuBarCtrl::onTouchDown(const GuiEvent &event)
{
	GuiMenuItemCtrl *ctrl = findHitMenu(globalToLocalCoord(event.mousePoint));
	if (mOpenMenu != ctrl)
	{
		if (mOpenMenu != NULL)
		{
			mOpenMenu->closeMenu();
		}
		if (ctrl != NULL)
		{
			ctrl->mIsOpen = true;
		}
		mOpenMenu = ctrl;
	}
	openMenu();
}

void GuiMenuBarCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI clipRect = dglGetClipRect();
	if (clipRect.extent.x != mBounds.extent.x || mBounds.point.x != 0 || mBounds.point.y != 0)
	{
		resize(Point2I(0, 0), Point2I(clipRect.extent.x, mBounds.extent.y));
	}
	RectI ctrlRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, NormalState);

	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

	if (contentRect.isValidRect())
	{
		//Render the childen
		renderChildControls(offset, contentRect, updateRect);
	}
}

void GuiMenuBarCtrl::openMenu()
{
	if (mOpenMenu == NULL)
	{
		return;
	}

	GuiCanvas *root = getRoot();
	AssertFatal(root, "GuiMenuBarCtrl::openMenu: Unable to optain the Canvas!");
	mBackground->mBounds.extent = root->mBounds.extent;
	mBackground->setControlProfile(mBackgroundProfile);

	ApplyMenuSettings();

	mBackground->openMenu();

	root->pushDialogControl(mBackground, 99);

	setFirstResponder();

	if (isMethod("onOpen"))
		Con::executef(this, 1, "onOpen");
}

void GuiMenuBarCtrl::closeMenu()
{
	if (!mOpenMenu)
		return;

	mOpenMenu->closeMenu();

	mBackground->getRoot()->popDialogControl(mBackground);

	mOpenMenu->mIsOpen = false;
	mOpenMenu->mIsHover = false;
	mOpenMenu = NULL;

	if (isMethod("onClose"))
		Con::executef(this, 1, "onClose");
}

void GuiMenuBarCtrl::ApplyMenuSettings()
{
	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);
		if (ctrl->isVisible() && ctrl->isActive() && ctrl->mHasGoodChildren)
		{
			ctrl->ApplyMenuSettings();
		}
		ctrl->setControlProfile(mMenuProfile);
	}
	setUpdate();
}

void GuiMenuBarCtrl::acceleratorKeyPress(U32 index)
{
	if (mUseKeyMode && mOpenMenu != NULL)
	{
		mUseKeyMode = true;
		GuiMenuItemCtrl *ctrl = mOpenMenu;
		closeMenu();
		mHoverTarget = ctrl;
		mHoverTarget->mIsHover = true;
	}
	else if (mUseKeyMode)
	{
		mUseKeyMode = false;
		if (mHoverTarget != NULL)
		{
			mHoverTarget->mIsHover = false;
		}
		mHoverTarget = NULL;
		clearFirstResponder();
	}
	else
	{
		mUseKeyMode = true;
		GuiMenuItemCtrl *newTarget = dynamic_cast<GuiMenuItemCtrl *>(first());
		setHoverTarget(newTarget);
		setFirstResponder();
	}
}

bool GuiMenuBarCtrl::onKeyDown(const GuiEvent &event)
{
	if (!mVisible || !mActive || !mAwake)
		return false;

	if (mHoverTarget == NULL && mOpenMenu == NULL)
	{
		clearFirstResponder();
		return false;
	}

	if ((event.keyCode == KEY_ALT || event.keyCode == KEY_LALT || event.keyCode == KEY_RALT) && event.modifier == 0)
	{
		acceleratorKeyPress(0);
		return true;
	}
	else if (mOpenMenu != NULL)
	{
		mOpenMenu->onKeyDown(event);
	}
	else if (event.keyCode == KEY_RIGHT && event.modifier == 0)
	{
		mUseKeyMode = true;
		if (mHoverTarget->mNextItem != NULL)
		{
			setHoverTarget(mHoverTarget->mNextItem);
			if (!mHoverTarget->isVisible() || !mHoverTarget->isActive() || !mHoverTarget->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		else
		{
			GuiMenuItemCtrl *newTarget = dynamic_cast<GuiMenuItemCtrl *>(first());
			setHoverTarget(newTarget);
			if (!mHoverTarget->isVisible() || !mHoverTarget->isActive() || !mHoverTarget->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		return true;
	}
	else if (event.keyCode == KEY_LEFT && event.modifier == 0)
	{
		mUseKeyMode = true;
		if (mHoverTarget->mPrevItem != NULL)
		{
			setHoverTarget(mHoverTarget->mPrevItem);
			if (!mHoverTarget->isVisible() || !mHoverTarget->isActive() || !mHoverTarget->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		else
		{
			GuiMenuItemCtrl *newTarget = dynamic_cast<GuiMenuItemCtrl *>(last());
			setHoverTarget(newTarget);
			if (!mHoverTarget->isVisible() || !mHoverTarget->isActive() || !mHoverTarget->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		return true;
	}
	else if (event.keyCode == KEY_DOWN && event.modifier == 0)
	{
		mUseKeyMode = true;
		if (mOpenMenu == NULL && mHoverTarget->mDisplayType == GuiMenuItemCtrl::DisplayType::Menu && mHoverTarget->isVisible() && mHoverTarget->isActive() && mHoverTarget->mHasGoodChildren)
		{
			mOpenMenu = mHoverTarget;
			mOpenMenu->mIsHover = false;
			mOpenMenu->mIsOpen = true;
			openMenu();
			return true;
		}
	}

	return false;
}

bool GuiMenuBarCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mMenuProfile != NULL)
		mMenuProfile->incRefCount();

	if (mMenuItemProfile != NULL)
		mMenuItemProfile->incRefCount();

	if (mMenuContentProfile != NULL)
		mMenuContentProfile->incRefCount();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->incRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

	return true;
}

void GuiMenuBarCtrl::onSleep()
{
	Parent::onSleep();

	if (mMenuProfile != NULL)
		mMenuProfile->decRefCount();

	if (mMenuItemProfile != NULL)
		mMenuItemProfile->decRefCount();

	if (mMenuContentProfile != NULL)
		mMenuContentProfile->decRefCount();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->decRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->decRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->decRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->decRefCount();
}

void GuiMenuBarCtrl::setControlBackgroundProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlBackgroundProfile: invalid background profile");
	if (prof == mBackgroundProfile)
		return;
	if (mAwake)
		mBackgroundProfile->decRefCount();
	mBackgroundProfile = prof;
	if (mAwake)
		mBackgroundProfile->incRefCount();

	ApplyMenuSettings();
}

void GuiMenuBarCtrl::setControlMenuProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlMenuProfile: invalid profile");
	if (prof == mMenuProfile)
		return;
	if (mAwake)
		mMenuProfile->decRefCount();
	mMenuProfile = prof;
	if (mAwake)
		mMenuProfile->incRefCount();

	ApplyMenuSettings();
	setUpdate();
}

void GuiMenuBarCtrl::setControlMenuItemProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlMenuItemProfile: invalid profile");
	if (prof == mMenuItemProfile)
		return;
	if (mAwake)
		mMenuItemProfile->decRefCount();
	mMenuItemProfile = prof;
	if (mAwake)
		mMenuItemProfile->incRefCount();

	ApplyMenuSettings();
	setUpdate();
}

void GuiMenuBarCtrl::setControlMenuContentProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlMenuContentProfile: invalid profile");
	if (prof == mMenuContentProfile)
		return;
	if (mAwake)
		mMenuContentProfile->decRefCount();
	mMenuContentProfile = prof;
	if (mAwake)
		mMenuContentProfile->incRefCount();

	ApplyMenuSettings();
}

void GuiMenuBarCtrl::setControlThumbProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlThumbProfile: invalid thumb profile");
	if (prof == mThumbProfile)
		return;
	if (mAwake)
		mThumbProfile->decRefCount();
	mThumbProfile = prof;
	if (mAwake)
		mThumbProfile->incRefCount();

	ApplyMenuSettings();
}

void GuiMenuBarCtrl::setControlTrackProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlTrackProfile: invalid track profile");
	if (prof == mTrackProfile)
		return;
	if (mAwake)
		mTrackProfile->decRefCount();
	mTrackProfile = prof;
	if (mAwake)
		mTrackProfile->incRefCount();

	ApplyMenuSettings();
}

void GuiMenuBarCtrl::setControlArrowProfile(GuiControlProfile* prof)
{
	AssertFatal(prof, "GuiMenuBarCtrl::setControlArrowProfile: invalid arrow profile");
	if (prof == mArrowProfile)
		return;
	if (mAwake)
		mArrowProfile->decRefCount();
	mArrowProfile = prof;
	if (mAwake)
		mArrowProfile->incRefCount();

	ApplyMenuSettings();
}
#pragma endregion

#pragma region GuiMenuItemCtrl
IMPLEMENT_CONOBJECT(GuiMenuItemCtrl);

GuiMenuItemCtrl::GuiMenuItemCtrl()
{
	mText = "Menu";
	mDisplayType = TextCommand;
	mActive = true;
	mIsOpen = false;
	mIsHover = false;
	mIsOn = false;
	mToggle = false;
	mRadio = false;
	mHasGoodChildren = true;

	mPrevItem = NULL;
	mNextItem = NULL;
	mOpenSubMenu = NULL;

	mScroll = NULL;
	mList = NULL;
}

void GuiMenuItemCtrl::initPersistFields()
{
	SimObject::initPersistFields();

	addGroup("MenuItem");
	addField("Variable", TypeString, Offset(mConsoleVariable, GuiMenuItemCtrl));
	addField("Command", TypeString, Offset(mConsoleCommand, GuiMenuItemCtrl));
	addField("AltCommand", TypeString, Offset(mAltConsoleCommand, GuiMenuItemCtrl));
	addField("Accelerator", TypeString, Offset(mAcceleratorKey, GuiMenuItemCtrl));
	addField("Active", TypeBool, Offset(mActive, GuiMenuItemCtrl));
	addField("Visible", TypeBool, Offset(mVisible, GuiMenuItemCtrl));
	addProtectedField("IsOn", TypeBool, Offset(mIsOn, GuiMenuItemCtrl), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeIsOn, "");
	addProtectedField("Toggle", TypeBool, Offset(mToggle, GuiMenuItemCtrl), &setToggle, &defaultProtectedGetFn, &writeToggle, "");
	addProtectedField("Radio", TypeS32, Offset(mRadio, GuiMenuItemCtrl), &setRadio, &defaultProtectedGetFn, &writeRadio, "");
	endGroup("MenuItem");

	addGroup("Localization");
	addField("langTableMod", TypeString, Offset(mLangTableName, GuiMenuItemCtrl));
	endGroup("Localization");

	addGroup("Text");
	addProtectedField("text", TypeCaseString, Offset(mText, GuiMenuItemCtrl), setTextProperty, getTextProperty, "");
	addField("textID", TypeString, Offset(mTextID, GuiMenuItemCtrl));
	endGroup("Text");
}

void GuiMenuItemCtrl::onAction()
{
	if (!mActive || !mVisible)
		return;

	if (mDisplayType == TextCommand)
	{
		Parent::onAction();
	}
	else if (mDisplayType == Toggle || mDisplayType == Radio)
	{
		toggleControl();
	}
}

void GuiMenuItemCtrl::inspectPostApply()
{
	Parent::inspectPostApply();

	if(mDisplayType == Menu)
	{
		checkForGoodChildren();
	}

	GuiMenuItemCtrl *menu = dynamic_cast<GuiMenuItemCtrl *>(getParent());
	if (menu != NULL)
	{
		menu->checkForGoodChildren();
	}
}

void GuiMenuItemCtrl::onChildAdded(GuiControl *child)
{
	GuiMenuItemCtrl *subMenu = dynamic_cast<GuiMenuItemCtrl*>(child);
	if (!subMenu)
	{
		Con::warnf("GuiMenuItemCtrl::onChildAdded - Only a GuiMenuItemCtrl can be added to a GuiMenuItemCtrl! Child will not be added.");
		SimObject *simObj = reinterpret_cast<SimObject*>(child);
		removeObject(simObj);
		return;
	}
	subMenu->mMenuBar = this->mMenuBar;
	subMenu->setControlProfile(mMenuBar->mMenuItemProfile);
	if (dStrcmp(subMenu->getText(), "-") == 0)
	{
		subMenu->mDisplayType = Spacer;
		subMenu->setText(StringTable->EmptyString);
	}
	else if (subMenu->mToggle)
	{
		subMenu->mDisplayType = Toggle;
	}
	else if (subMenu->mRadio)
	{
		subMenu->mDisplayType = Radio;
	}

	if (size() > 1)
	{
		iterator i = end() - 2;
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);
		subMenu->mPrevItem = ctrl;
		ctrl->mNextItem = subMenu;
	}

	if (mDisplayType != Menu)
	{
		mDisplayType = Menu;
		if(mScroll == NULL)
		{
			mScroll = new GuiScrollCtrl();
			AssertFatal(mScroll, "GuiMenuItemCtrl::onChildAdded Failed to initialize GuiScrollCtrl!");
		}

		if(mList == NULL)
		{
			mList = new GuiMenuListCtrl(this);
			AssertFatal(mList, "GuiMenuItemCtrl::onChildAdded Failed to initialize GuiMenuListCtrl!");
			mScroll->addObject(mList);
		}
	}

	if (mDisplayType == Menu)
	{
		mList->updateSize();
	}
	checkForGoodChildren();
}

void GuiMenuItemCtrl::onChildRemoved(SimObject *child)
{
	if (size() <= 0)
	{
		if (mDisplayType == Menu)
		{
			mDisplayType = TextCommand;
			mList->deleteObject();
			mList = NULL;
			mScroll->deleteObject();
			mScroll = NULL;
		}
	}

	checkForGoodChildren();
}

void GuiMenuItemCtrl::checkForGoodChildren()
{
	if (mDisplayType != Menu)
	{
		mHasGoodChildren = true;
		return;
	}

	iterator iter;
	for (iter = begin(); iter != end(); iter++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*iter);
		if (ctrl->isVisible() && ctrl->isActive() && ctrl->mDisplayType != Spacer)
		{
			mHasGoodChildren = true;
			return;
		}
	}
	mHasGoodChildren = false;
}

void GuiMenuItemCtrl::closeMenu()
{
	if (mOpenSubMenu != NULL)
	{
		mOpenSubMenu->closeMenu();
	}
	mOpenSubMenu = NULL;
	mList->mHoveredItem = NULL;
	mMenuBar->mBackground->removeObject(mScroll);
	mIsOpen = false;
}

void GuiMenuItemCtrl::ApplyMenuSettings()
{
	if (mDisplayType == Menu)
	{
		mScroll->setControlProfile(mMenuBar->mMenuContentProfile);
		mScroll->setControlThumbProfile(mMenuBar->mThumbProfile);
		mScroll->setControlArrowProfile(mMenuBar->mArrowProfile);
		mScroll->setControlTrackProfile(mMenuBar->mTrackProfile);
		mScroll->setField("hScrollBar", "AlwaysOff");
		mScroll->setField("vScrollBar", "Dynamic");
		mScroll->mUseConstantHeightThumb = mMenuBar->mUseConstantHeightThumb;
		mScroll->mScrollBarThickness = mMenuBar->mScrollBarThickness;
		mScroll->mShowArrowButtons = mMenuBar->mShowArrowButtons;
	}

	iterator i;
	for (i = begin(); i != end(); i++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);
		if (ctrl->isVisible() && ctrl->isActive() && ctrl->mHasGoodChildren)
		{
			ctrl->ApplyMenuSettings();
		}
	}
	setControlProfile(mMenuBar->mMenuItemProfile);
}

void GuiMenuItemCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	GuiControlState currentState = GuiControlState::NormalState;
	if (!mActive || !mHasGoodChildren)
		currentState = GuiControlState::DisabledState;
	else if (mIsOpen)
		currentState = GuiControlState::SelectedState;
	else if (mIsHover)
		currentState = GuiControlState::HighlightState;

	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	renderUniversalRect(ctrlRect, mProfile, currentState);

	//Render Text
	dglSetBitmapModulation(mProfile->getFontColor(currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);
	renderText(contentRect.point, contentRect.extent, mText, mProfile);
}

void GuiMenuItemCtrl::toggleControl()
{
	if(mDisplayType == Toggle && !mIsOn)
	{
		mIsOn = true;
		Parent::onAction();
	}
	else if (mDisplayType == Toggle && mIsOn)
	{
		mIsOn = false;
		execAltConsoleCallback();
	}
	else if(mDisplayType == Radio && !mIsOn)
	{
		mIsOn = true;
		Parent::onAction();
		//Turn off other radio controls in this block
		turnOffPrevRadio();
		turnOffNextRadio();
	}
}

void GuiMenuItemCtrl::turnOffPrevRadio()
{
	if (mPrevItem != NULL && mPrevItem->mDisplayType == Radio)
	{
		if(mPrevItem->mIsOn)
		{
			mPrevItem->mIsOn = false;
			mPrevItem->execAltConsoleCallback();
		}
		mPrevItem->turnOffPrevRadio();
	}
}

void GuiMenuItemCtrl::turnOffNextRadio()
{
	if (mNextItem != NULL && mNextItem->mDisplayType == Radio)
	{
		if (mNextItem->mIsOn)
		{
			mNextItem->mIsOn = false;
			mNextItem->execAltConsoleCallback();
		}
		mNextItem->turnOffNextRadio();
	}
}

const char* GuiMenuItemCtrl::getHotKeyText()
{
	if(mAcceleratorKey == StringTable->EmptyString || mDisplayType == Menu)
		return StringTable->EmptyString;

	char key[128];
	dSprintf(key, 128, "%s", mAcceleratorKey);

	char buffer[128];
	buffer[0] = '\0';
	char *tok;
	tok = dStrtok((char*)key, " -");
	while (tok != NULL)
	{
		if(buffer[0] != '\0')
			dStrcat(buffer, "+");
		dStrcat(buffer, ActionMap::swapCtrlForCmd(tok));

		tok = dStrtok(NULL, " -");
	}

	return StringTable->insert(buffer);
}

bool GuiMenuItemCtrl::onKeyDown(const GuiEvent &event)
{
	if (mOpenSubMenu != NULL && (mOpenSubMenu->mList->mHoveredItem != NULL || mOpenSubMenu->mOpenSubMenu != NULL))
	{
		mOpenSubMenu->onKeyDown(event);
	}
	else if (event.keyCode == KEY_DOWN && event.modifier == 0)
	{
		mMenuBar->mUseKeyMode = true;
		if (mList->mHoveredItem == NULL || mList->mHoveredItem->mNextItem == NULL)
		{
			mList->setHoveredItem(dynamic_cast<GuiMenuItemCtrl *>(first()));
			if (!mList->mHoveredItem->isVisible() || !mList->mHoveredItem->isActive() || !mList->mHoveredItem->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		else if(mList->mHoveredItem->mNextItem->mDisplayType == DisplayType::Spacer || !mList->mHoveredItem->mNextItem->isVisible() || !mList->mHoveredItem->mNextItem->isActive() || !mList->mHoveredItem->mNextItem->mHasGoodChildren)
		{
			//To skip the spacer and still do all the needed checks, we'll move the hovered item to the next item and simulate a second down button push
			mList->setHoveredItem(mList->mHoveredItem->mNextItem);
			onKeyDown(event);
		}
		else
		{
			mList->setHoveredItem(mList->mHoveredItem->mNextItem);
		}
		return true;
	}
	else if (event.keyCode == KEY_UP && event.modifier == 0)
	{
		mMenuBar->mUseKeyMode = true;
		if (mList->mHoveredItem == NULL || mList->mHoveredItem->mPrevItem == NULL)
		{
			mList->setHoveredItem(dynamic_cast<GuiMenuItemCtrl *>(last()));
			if (!mList->mHoveredItem->isVisible() || !mList->mHoveredItem->isActive() || !mList->mHoveredItem->mHasGoodChildren)
			{
				onKeyDown(event);
			}
		}
		else if (mList->mHoveredItem->mPrevItem->mDisplayType == DisplayType::Spacer || !mList->mHoveredItem->mPrevItem->isVisible() || !mList->mHoveredItem->mPrevItem->isActive() || !mList->mHoveredItem->mPrevItem->mHasGoodChildren)
		{
			//To skip the spacer and still do all the needed checks, we'll move the hovered item to the prev item and simulate a second up button push
			mList->setHoveredItem(mList->mHoveredItem->mPrevItem);
			onKeyDown(event);
		}
		else
		{
			mList->setHoveredItem(mList->mHoveredItem->mPrevItem);
		}
		return true;
	}
	else if (event.keyCode == KEY_RIGHT && event.modifier == 0)
	{
		mMenuBar->mUseKeyMode = true;
		if (mList->mHoveredItem == NULL)
		{
			//loop to find the first valid menu item
			GuiMenuItemCtrl *foundCtrl = NULL;
			iterator iter;
			for (iter = begin(); iter != end(); iter++)
			{
				GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*iter);
				if (ctrl->isVisible() && ctrl->isActive() && ctrl->mDisplayType != Spacer && ctrl->mHasGoodChildren )
				{
					foundCtrl = ctrl;
					break;
				}
			}
			if (foundCtrl != NULL)
			{
				mList->setHoveredItem(foundCtrl);
			}
		}
		else if (mList->mHoveredItem == mOpenSubMenu)
		{
			mOpenSubMenu->onKeyDown(event);
			mList->mHoveredItem = NULL;
		}
		else if(mList->mHoveredItem->mDisplayType == DisplayType::Menu && mList->mHoveredItem->isVisible() && mList->mHoveredItem->isActive() && mList->mHoveredItem->mHasGoodChildren)
		{
			mList->processMenuItem(mList->mHoveredItem);
			mOpenSubMenu->onKeyDown(event);
			mList->mHoveredItem = NULL;
		}
		return true;
	}
	else if (event.keyCode == KEY_LEFT && event.modifier == 0)
	{
		mMenuBar->mUseKeyMode = true;

		//Is this the open sub menu?
		GuiMenuItemCtrl *menu = dynamic_cast<GuiMenuItemCtrl *>(getParent());
		if (menu != NULL && menu->mOpenSubMenu == this)
		{
			menu->mList->mHoveredItem = menu->mOpenSubMenu;
			closeMenu();
			menu->mOpenSubMenu = NULL;
		}
		return true;
	}
	else if (event.keyCode == KEY_RETURN && event.modifier == 0)
	{
		mMenuBar->mUseKeyMode = true;
		if (mList->mHoveredItem == NULL)
		{
			mList->setHoveredItem(dynamic_cast<GuiMenuItemCtrl *>(first()));
		}
		else
		{
			mList->processMenuItem(mList->mHoveredItem);
		}
		return true;
	}
}
#pragma endregion

#pragma region GuiMenuBGCtrl
GuiMenuBGCtrl::GuiMenuBGCtrl(GuiMenuBarCtrl *ctrl)
{
	mMenuBarCtrl = ctrl;
	mBounds.point.set(0, 0);
	setField("profile", "GuiDefaultProfile");
}

void GuiMenuBGCtrl::openMenu()
{
	GuiMenuItemCtrl *menu = mMenuBarCtrl->mOpenMenu;
	if (menu == NULL)
	{
		return;
	}

	positionMenu(menu, Point2I(menu->mBounds.point.x, menu->mBounds.point.y + menu->mBounds.extent.y));

	addObject(menu->mScroll);
}

void GuiMenuBGCtrl::closeMenu()
{
	mMenuBarCtrl->closeMenu();
}

void GuiMenuBGCtrl::openSubMenu(GuiMenuItemCtrl *subMenu)
{
	if (subMenu == NULL)
	{
		return;
	}

	GuiMenuItemCtrl *menu = dynamic_cast<GuiMenuItemCtrl *>(subMenu->getParent());
	if (subMenu == menu->mOpenSubMenu)
	{
		return;
	}

	if(menu->mOpenSubMenu != NULL)
	{
		menu->mOpenSubMenu->closeMenu();
	}
	menu->mOpenSubMenu = subMenu;
	subMenu->mIsOpen = true;

	positionMenu(subMenu, Point2I(subMenu->mBounds.point.x + subMenu->mBounds.extent.x - 4, subMenu->mBounds.point.y - 4));

	addObject(subMenu->mScroll);

	menu->mList->mArmSubMenu = false;
}

void GuiMenuBGCtrl::onTouchMove(const GuiEvent &event)
{
	processHover(event);
}

void GuiMenuBGCtrl::onTouchDragged(const GuiEvent &event)
{
	processHover(event);
}

void GuiMenuBGCtrl::onTouchDown(const GuiEvent &event)
{
	mMenuBarCtrl->closeMenu();
}

void GuiMenuBGCtrl::processHover(const GuiEvent &event)
{
	if (mMenuBarCtrl->pointInControl(event.mousePoint))
	{
		GuiMenuItemCtrl *menu = mMenuBarCtrl->findHitMenu(globalToLocalCoord(event.mousePoint));
		if (menu != NULL && mMenuBarCtrl->mOpenMenu != menu)
		{
			mMenuBarCtrl->mOpenMenu->closeMenu();
			mMenuBarCtrl->mOpenMenu = menu;
			mMenuBarCtrl->mOpenMenu->mIsOpen = true;
			openMenu();
			setUpdate();
		}
	}
	mMenuBarCtrl->mUseKeyMode = false;
}

void GuiMenuBGCtrl::positionMenu(const GuiMenuItemCtrl *menu, const Point2I &topLeft)
{
	menu->mList->updateSize();
	Point2I menuSize = menu->mList->mBounds.extent;
	RectI box = RectI(topLeft, menuSize);
	box.extent = getOuterExtent(box.extent, NormalState, menu->mScroll->mProfile);

	//Is it too tall?
	if ((box.point.y + box.extent.y) > mBounds.extent.y)
	{
		//can we move it up?
		if ((mMenuBarCtrl->mBounds.point.y + mMenuBarCtrl->mBounds.extent.y) < box.point.y)
		{
			box.point.y = getMax(mMenuBarCtrl->mBounds.point.y + mMenuBarCtrl->mBounds.extent.y, mBounds.extent.y - box.extent.y);
		}

		//Is it still too tall?
		if ((box.point.y + box.extent.y) > mBounds.extent.y)
		{
			box.extent.y = mBounds.extent.y - box.point.y;
			box.extent.x += menu->mScroll->mScrollBarThickness;
		}
	}

	//Is it too far to the right?
	if ((box.point.x + box.extent.x) > mBounds.extent.x)
	{
		box.point.x = mBounds.extent.x - box.extent.x;
	}

	menu->mScroll->mBounds.extent = box.extent;
	menu->mScroll->mBounds.point = box.point;
}

#pragma endregion

#pragma region GuiMenuListCtrl
GuiMenuListCtrl::GuiMenuListCtrl(GuiMenuItemCtrl *ctrl)
{
	mMenu = ctrl;
	mBounds.point.set(0,0);
	mHoveredItem = NULL;
	mEnterItemTime = 0;
	mSubMenuStallTime = 500;
}

void GuiMenuListCtrl::onTouchMove(const GuiEvent &event)
{
	Parent::onTouchMove(event);
	mMenu->mMenuBar->mUseKeyMode = false;
}

GuiMenuItemCtrl* GuiMenuListCtrl::GetHitItem(const Point2I &pt)
{
	iterator iter;
	for (iter = mMenu->begin(); iter != mMenu->end(); iter++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*iter);
		if (!ctrl->mVisible) continue;

		if (ctrl->pointInControl(pt))
		{
			return ctrl;
		}
	}
	return nullptr;
}

void GuiMenuListCtrl::onPreRender()
{
	U32 currentTime = Platform::getVirtualMilliseconds();
	if (mArmSubMenu && mHoveredItem != NULL && mHoveredItem != mMenu->mOpenSubMenu && (currentTime - mEnterItemTime) > mSubMenuStallTime)
	{
		//Close an open sub menu
		if (mMenu->mOpenSubMenu != NULL)
		{
			mMenu->mOpenSubMenu->closeMenu();
			mMenu->mOpenSubMenu = NULL;
		}

		//Open a sub menu
		if (mHoveredItem->mDisplayType == GuiMenuItemCtrl::DisplayType::Menu)
		{
			processMenuItem(mHoveredItem);
		}
	}
}

void GuiMenuListCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	bool foundHoveredItem = false;
	iterator iter;
	S32 h = 0;
	for (iter = mMenu->begin(); iter != mMenu->end(); iter++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*iter);

		if (!ctrl->mVisible) continue;

		S32 top = h;
		S32 bottom = h;
		if (ctrl->mDisplayType == GuiMenuItemCtrl::DisplayType::Spacer)
		{
			 bottom += mSpacerSize.y;
		}
		else
		{
			bottom += mItemSize.y;
		}

		// Only render visible items
		if (bottom + offset.y < updateRect.point.y)
			continue;

		// Break out once we're no longer in visible item range
		if (top + offset.y >= updateRect.point.y + updateRect.extent.y)
			break;

		RectI itemRect;
		if (ctrl->mDisplayType == GuiMenuItemCtrl::DisplayType::Spacer)
		{
			itemRect = RectI(offset.x, offset.y + top, mSpacerSize.x, mSpacerSize.y);
		}
		else
		{
			itemRect = RectI(offset.x, offset.y + top, mItemSize.x, mItemSize.y);
		}

		// Render our item
		foundHoveredItem |= onRenderItem(itemRect, ctrl);

		h = bottom;
	}

	if (!foundHoveredItem)
	{
		mHoveredItem = NULL;
	}
}

bool GuiMenuListCtrl::onRenderItem(RectI &itemRect, GuiMenuItemCtrl *item)
{
	GuiControlProfile *profile = item->mProfile;
	if (!profile)
		return false;

	if (item->mBounds != itemRect)
	{
		item->mBounds = itemRect;
	}

	Point2I cursorPt = Point2I(0, 0);
	GuiCanvas *root = getRoot();
	if (root)
	{
		cursorPt = root->getCursorPos();
	}

	//is this item being hovered?
	bool foundHoveredItem = false;
	if (mMenu->mMenuBar->mUseKeyMode && mHoveredItem == item)
	{
		foundHoveredItem = true;
	}
	else if (!mMenu->mMenuBar->mUseKeyMode && itemRect.pointInRect(cursorPt) && (mMenu->mOpenSubMenu == NULL || !mMenu->mOpenSubMenu->mScroll->mBounds.pointInRect(cursorPt)))
	{
		if (mHoveredItem != item)
		{
			setHoveredItem(item);
		}
		foundHoveredItem = true;
	}

	//Get the current state
	GuiControlState currentState = GuiControlState::NormalState;
	if (!item->mActive || !item->mHasGoodChildren)
		currentState = GuiControlState::DisabledState;
	else if (item->mDisplayType == GuiMenuItemCtrl::DisplayType::Spacer)
		currentState = GuiControlState::SelectedState;
	else if (mHoveredItem == item ||
		(item->mIsOpen && (mMenu->mMenuBar->mUseKeyMode || !mMenu->mScroll->mBounds.pointInRect(cursorPt) || mMenu->mOpenSubMenu->mScroll->mBounds.pointInRect(cursorPt))))
		currentState = GuiControlState::HighlightState;
	RectI ctrlRect = applyMargins(itemRect.point, itemRect.extent, currentState, profile);

	if (!ctrlRect.isValidRect())
	{
		return foundHoveredItem;
	}

	renderUniversalRect(ctrlRect, profile, currentState);

	//Render Text
	dglSetBitmapModulation(profile->getFontColor(currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, profile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, profile);

	if(contentRect.isValidRect())
	{
		//Left Icon
		RectI leftIconRect = RectI(itemRect.point, Point2I(itemRect.extent.y, itemRect.extent.y));
		if(item->mDisplayType == GuiMenuItemCtrl::DisplayType::Toggle)
		{
			ColorI itemColor = item->mIsOn ? profile->getFillColor(HighlightState) : profile->getFillColor(NormalState);
			S32 size = profile->mFont->getHeight();
			renderColorBullet(leftIconRect, itemColor, getMin(size, 16));
		}
		else if (item->mDisplayType == GuiMenuItemCtrl::DisplayType::Radio)
		{
			ColorI itemColor = item->mIsOn ? profile->getFillColor(HighlightState) : profile->getFillColor(NormalState);
			S32 size = profile->mFont->getHeight();
			renderColorBullet(leftIconRect, itemColor, getMin(size, 16), true);
		}

		//Right Icon
		RectI rightIconRect = RectI(Point2I(itemRect.point.x + itemRect.extent.x - itemRect.extent.y, itemRect.point.y), leftIconRect.extent);
		if (item->mDisplayType == GuiMenuItemCtrl::DisplayType::Menu)
		{
			S32 size = (profile->mFont->getHeight() / 2);
			rightIconRect.inset(2, 0);
			renderTriangleIcon(rightIconRect, ColorI(profile->getFontColor(currentState)), GuiDirection::Right, size);
		}

		//Text Space
		RectI textRect = RectI(itemRect.point.x + itemRect.extent.y, itemRect.point.y, itemRect.extent.x - (2 * itemRect.extent.y), itemRect.extent.y);

		//Command Description
		profile->mAlignment = GuiControlProfile::AlignmentType::LeftAlign;
		renderText(textRect.point, textRect.extent, item->getText(), profile);

		//Hot Keys!!
		profile->mAlignment = GuiControlProfile::AlignmentType::RightAlign;
		renderText(textRect.point, textRect.extent, item->getHotKeyText(), profile);
	}

	return foundHoveredItem;
}

void GuiMenuListCtrl::updateSize()
{
	if(mMenu->size() == 0)
	{
		resize(mBounds.extent, Point2I(100, 20));
		return;
	}

	GuiMenuItemCtrl *child = dynamic_cast<GuiMenuItemCtrl *>(mMenu->first());
	if (!child)
		return;

	GuiControlProfile *profile = child->mProfile;
	if (!profile)
		return;

	GFont *font = profile->mFont;
	Point2I contentSize = Point2I(10, font->getHeight() + 2);
	Point2I spacerSize = Point2I(10, 0);

	// Find the maximum width cell:
	mItemCount = 0;
	mSpacerCount = 0;
	S32 maxWidth = 1;
	iterator i;
	for (i = mMenu->begin(); i != mMenu->end(); i++)
	{
		GuiMenuItemCtrl *ctrl = static_cast<GuiMenuItemCtrl *>(*i);

		if(!ctrl->mVisible) continue;

		S32 width = font->getStrWidth(ctrl->getText());
		if(ctrl->mDisplayType != GuiMenuItemCtrl::DisplayType::Menu && ctrl->mDisplayType != GuiMenuItemCtrl::DisplayType::Spacer)
		{
			StringTableEntry hotKey = ctrl->getHotKeyText();
			if(hotKey != StringTable->EmptyString)
			{
				width += font->getStrWidth(hotKey) + 30;
			}
		}

		if (width > maxWidth)
			maxWidth = width;

		if (ctrl->mDisplayType == GuiMenuItemCtrl::DisplayType::Spacer)
		{
			mSpacerCount++;
		}
		else
		{
			mItemCount++;
		}
	}
	contentSize.x = maxWidth;
	spacerSize.x = contentSize.x;

	mItemSize = this->getOuterExtent(contentSize, NormalState, profile);
	mSpacerSize = this->getOuterExtent(spacerSize, SelectedState, profile);

	//Add a square of space at both ends for icons
	mItemSize.x += (2 * mItemSize.y);
	mSpacerSize.x += (2 * mItemSize.y);

	Point2I newExtent = Point2I(mItemSize.x, (mItemSize.y * mItemCount) + (mSpacerSize.y * mSpacerCount));

	resize(mBounds.point, newExtent);
}

void GuiMenuListCtrl::onTouchUp(const GuiEvent &event)
{
	GuiMenuItemCtrl *ctrl = GetHitItem(event.mousePoint);
	if (ctrl != nullptr )
	{
		processMenuItem(ctrl);
	}
}

void GuiMenuListCtrl::processMenuItem(GuiMenuItemCtrl *ctrl)
{
	if(!ctrl->mActive || ctrl->mDisplayType == GuiMenuItemCtrl::DisplayType::Spacer || !ctrl->mHasGoodChildren)
		return;

	ctrl->onAction();

	GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
	if (parent)
	{
		GuiMenuBGCtrl* grandParent = dynamic_cast<GuiMenuBGCtrl *>(parent->getParent());
		if (grandParent)
		{
			if (ctrl->mDisplayType != GuiMenuItemCtrl::DisplayType::Menu)
			{
				grandParent->closeMenu();
			}
			else
			{
				grandParent->openSubMenu(ctrl);

			}
		}
	}
}

#pragma endregion
