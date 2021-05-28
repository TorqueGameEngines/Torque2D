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

#ifndef _GUIDROPDOWNCTRL_H_
#define _GUIDROPDOWNCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif
#ifndef _GUILISTBOXCTRL_H_
#include "gui/guiListBoxCtrl.h"
#endif
class GuiDropDownCtrl;

class GuiDropDownBGCtrl : public GuiControl
{
protected:
	GuiDropDownCtrl *mDropDownCtrl;
public:
	GuiDropDownBGCtrl(GuiDropDownCtrl *ctrl);
	void onTouchUp(const GuiEvent &event);
};

class GuiDropDownListBoxCtrl : public GuiListBoxCtrl
{
private:
	typedef GuiListBoxCtrl Parent;

protected:
	GuiDropDownCtrl *mDropDownCtrl;
public:
	GuiDropDownListBoxCtrl(GuiDropDownCtrl *ctrl);
	void addSelection(LBItem *item, S32 index);
	void setCurSel(S32 index);
};

class GuiDropDownCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

   S32 mMaxHeight;
   bool mIsOpen;

   GuiDropDownBGCtrl *mBackground;
   GuiDropDownListBoxCtrl *mListBox;
   GuiScrollCtrl *mScroll;

   GuiControlProfile *mBackgroundProfile; //Used to render the background when the drop down is open
   GuiControlProfile *mListBoxProfile; //Used to render the list box items
   GuiControlProfile *mScrollProfile; //Used to render the scroll content area
   GuiControlProfile *mThumbProfile; //Used to render the thumb and arrow buttons
   GuiControlProfile *mTrackProfile; //Used to render the tracks
   GuiControlProfile *mArrowProfile; //Used to render the arrow buttons

   S32 mScrollBarThickness;
   bool mShowArrowButtons;
   bool mUseConstantHeightThumb;

protected:

public:
   GuiDropDownCtrl();
   static void initPersistFields();

   virtual void onTouchUp(const GuiEvent &event);
   GuiControlState getCurrentState();
   void onRender(Point2I offset, const RectI &updateRect);

   bool onKeyDown(const GuiEvent &event);
   virtual void onAction();
   void itemSelected();
   void openDropDown();
   void closeDropDown();

   bool onWake();
   void onSleep();
   void setControlBackgroundProfile(GuiControlProfile* prof);
   void setControlListBoxProfile(GuiControlProfile* prof);
   void setControlScrollProfile(GuiControlProfile* prof);
   void setControlThumbProfile(GuiControlProfile* prof);
   void setControlTrackProfile(GuiControlProfile* prof);
   void setControlArrowProfile(GuiControlProfile* prof);
   void setConstantThumbHeight(bool isConstant) { mUseConstantHeightThumb = isConstant; }
   void setShowArrowButtons(bool show) { mShowArrowButtons = show; }
   void setScrollBarThickness(S32 thickness) { mScrollBarThickness = thickness; }

   inline GuiListBoxCtrl* getList() { return mListBox; }

   DECLARE_CONOBJECT(GuiDropDownCtrl);
};

#endif