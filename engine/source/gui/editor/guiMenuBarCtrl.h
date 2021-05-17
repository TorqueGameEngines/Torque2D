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

#ifndef _GUIMENUBARCTRL_H_
#define _GUIMENUBARCTRL_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif
#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif
#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

class GuiMenuItemCtrl;
class GuiMenuBGCtrl;
class GuiMenuListCtrl;

class GuiMenuBarCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiMenuBarCtrl);
   GuiMenuBarCtrl();
   static void initPersistFields();

   virtual void childResized(GuiControl *child);
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
   virtual void inspectPostApply();
   virtual void onChildAdded(GuiControl *child);
   virtual void onChildRemoved(SimObject *child);
   virtual void calculateMenus();
   virtual GuiControl* findHitControl(const Point2I &pt, S32 initialLayer);
   virtual GuiMenuItemCtrl* findHitMenu(const Point2I &pt);
   virtual void onRender(Point2I offset, const RectI &updateRect);

   virtual void processHover(const GuiEvent &event);
   virtual void setHoverTarget(GuiMenuItemCtrl *ctrl);
   virtual void onTouchMove(const GuiEvent &event);
   virtual void onTouchEnter(const GuiEvent &event);
   virtual void onTouchLeave(const GuiEvent &event);
   virtual void onTouchDown(const GuiEvent &event);

   void openMenu();
   void closeMenu();
   void ApplyMenuSettings();

   bool mUseKeyMode;
   virtual void acceleratorKeyPress(U32 index);
   virtual bool onKeyDown(const GuiEvent &event);

   GuiControlProfile *mMenuProfile;
   GuiControlProfile *mMenuContentProfile;
   GuiControlProfile *mMenuItemProfile;
   GuiControlProfile *mBackgroundProfile;
   GuiControlProfile *mThumbProfile;
   GuiControlProfile *mArrowProfile;
   GuiControlProfile *mTrackProfile;

   bool onWake();
   void onSleep();
   void setControlBackgroundProfile(GuiControlProfile* prof);
   void setControlMenuProfile(GuiControlProfile* prof);
   void setControlMenuItemProfile(GuiControlProfile* prof);
   void setControlMenuContentProfile(GuiControlProfile* prof);
   void setControlThumbProfile(GuiControlProfile* prof);
   void setControlArrowProfile(GuiControlProfile* prof);
   void setControlTrackProfile(GuiControlProfile* prof);

   S32 mScrollBarThickness;
   bool mShowArrowButtons;
   bool mUseConstantHeightThumb;

   GuiMenuItemCtrl *mHoverTarget;
   GuiMenuItemCtrl *mOpenMenu;
   GuiMenuBGCtrl *mBackground;
};

class GuiMenuItemCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;

public:
	//creation methods
	DECLARE_CONOBJECT(GuiMenuItemCtrl);
	GuiMenuItemCtrl();
	static void initPersistFields();

	enum DisplayType
	{
		TextCommand = 0,
		Toggle,
		Radio,
		Spacer,
		Menu
	};

	DisplayType			mDisplayType;
	bool				mIsOpen;
	bool				mIsHover;
	bool				mIsOn;
	bool				mToggle;
	bool				mRadio;
	bool				mHasGoodChildren;

	static bool setToggle(void *obj, const char *data) 
	{ 
		GuiMenuItemCtrl* pCastObject = static_cast<GuiMenuItemCtrl *>(obj);
		pCastObject->mToggle = dAtob(data);
		pCastObject->mDisplayType = pCastObject->mToggle ? Toggle : TextCommand; 
		return false; 
	}
	static bool writeToggle(void* obj, StringTableEntry pFieldName) { return static_cast<GuiMenuItemCtrl*>(obj)->mToggle; }

	static bool setRadio(void *obj, const char *data)
	{
		GuiMenuItemCtrl* pCastObject = static_cast<GuiMenuItemCtrl *>(obj);
		pCastObject->mRadio = dAtob(data);
		pCastObject->mDisplayType = pCastObject->mRadio ? Radio : TextCommand;
		return false;
	}
	static bool writeRadio(void* obj, StringTableEntry pFieldName) { return static_cast<GuiMenuItemCtrl*>(obj)->mRadio; }
	static bool writeIsOn(void* obj, StringTableEntry pFieldName) { GuiMenuItemCtrl* pCastObject = static_cast<GuiMenuItemCtrl *>(obj); return pCastObject->mToggle || pCastObject->mRadio; }

	GuiMenuBarCtrl *mMenuBar;
	GuiScrollCtrl *mScroll;
	GuiMenuListCtrl *mList;

	GuiMenuItemCtrl *mPrevItem;
	GuiMenuItemCtrl *mNextItem;
	GuiMenuItemCtrl *mOpenSubMenu;

	virtual void onAction();
	virtual void inspectPostApply();
	virtual void onChildAdded(GuiControl *child);
	virtual void onChildRemoved(SimObject *child);
	void checkForGoodChildren();
	virtual void closeMenu();
	void ApplyMenuSettings();
	virtual void onRender(Point2I offset, const RectI &updateRect);
	virtual const char* getHotKeyText();
	virtual void toggleControl();
	virtual void turnOffPrevRadio();
	virtual void turnOffNextRadio();
	virtual bool onKeyDown(const GuiEvent &event);
};

class GuiMenuBGCtrl : public GuiControl
{
protected:
	typedef GuiControl Parent;
	GuiMenuBarCtrl *mMenuBarCtrl;
public:
	GuiMenuBGCtrl(GuiMenuBarCtrl *ctrl);
	void openMenu();
	void closeMenu();
	void openSubMenu(GuiMenuItemCtrl *subMenu);
	virtual void onTouchMove(const GuiEvent &event);
	virtual void onTouchDragged(const GuiEvent &event);
	virtual void processHover(const GuiEvent &event);
	virtual void onTouchDown(const GuiEvent &event);
private:
	void positionMenu(const GuiMenuItemCtrl *menu, const Point2I &topLeft);
};

class GuiMenuListCtrl : public GuiControl
{
protected:
	typedef GuiControl Parent;
	GuiMenuItemCtrl *mMenu;
public:
	GuiMenuListCtrl(GuiMenuItemCtrl *ctrl);
	virtual void onTouchMove(const GuiEvent &event);
	GuiMenuItemCtrl* GetHitItem(const Point2I &pt);
	virtual void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	bool onRenderItem(RectI &itemRect, GuiMenuItemCtrl *item);
	void updateSize();

	Point2I mItemSize;
	Point2I	mSpacerSize;
	S32 mItemCount;
	S32 mSpacerCount;
	U32 mEnterItemTime;
	U32 mSubMenuStallTime;
	GuiMenuItemCtrl *mHoveredItem;
	bool mArmSubMenu;

	inline void setHoveredItem(GuiMenuItemCtrl *item) { mEnterItemTime = Platform::getVirtualMilliseconds(); mHoveredItem = item; mArmSubMenu = true; }

	virtual void onTouchUp(const GuiEvent &event);
	void processMenuItem(GuiMenuItemCtrl *ctrl);
};

#endif
