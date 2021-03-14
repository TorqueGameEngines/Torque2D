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

#ifndef _GUIPOPUPCTRL_H_
#define _GUIPOPUPCTRL_H_

#ifndef _GUITEXTCTRL_H_
#include "gui/guiTextCtrl.h"
#endif
#ifndef _GUITEXTLISTCTRL_H_
#include "gui/guiTextListCtrl.h"
#endif
#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif
class GuiPopUpMenuCtrl;
class GuiPopupTextListCtrl;

class GuiPopUpBackgroundCtrl : public GuiControl
{
protected:
   GuiPopUpMenuCtrl *mPopUpCtrl;
   GuiPopupTextListCtrl *mTextList; 
public:
   GuiPopUpBackgroundCtrl(GuiPopUpMenuCtrl *ctrl, GuiPopupTextListCtrl* textList);
   void onMouseDown(const GuiEvent &event);
};

class GuiPopupTextListCtrl : public GuiTextListCtrl
{
private:
   typedef GuiTextListCtrl Parent;

protected:
   GuiPopUpMenuCtrl *mPopUpCtrl;

public:
   GuiPopupTextListCtrl(); // for inheritance
   GuiPopupTextListCtrl(GuiPopUpMenuCtrl *ctrl);

   // GuiArrayCtrl overload:
   void onCellSelected(Point2I cell);

   // GuiControl overloads:
   bool onKeyDown(const GuiEvent &event);
   void onMouseDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   void onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver);
};

class GuiPopUpMenuCtrl : public GuiControl
{
   typedef GuiControl Parent;

public:
   struct Entry
   {
      char buf[256];
      S32 id;
      U16 ascii;
      U16 scheme;
      bool usesColorBox;	// DAW: Added
      ColorI colorbox;		// DAW: Added
   };

   struct Scheme
   {
      U32      id;
      ColorI   fontColor;
      ColorI   fontColorHL;
      ColorI   fontColorSEL;
   };

   bool mBackgroundCancel; // DAW: Added

protected:
   GuiPopupTextListCtrl *mTl;
   GuiScrollCtrl *mSc;
   GuiPopUpBackgroundCtrl *mBackground;
   Vector<Entry> mEntries;
   Vector<Scheme> mSchemes;
   S32 mSelIndex;
   S32 mMaxPopupHeight;
   F32 mIncValue;
   F32 mScrollCount;
   S32 mLastYvalue;
   GuiEvent mEventSave;
   S32 mRevNum;
   bool mInAction;
   bool mReplaceText;
   bool mMouseOver; // DAW: Added
   bool mRenderScrollInNA; // DAW: Added
   bool mReverseTextList;	// DAW: Added - Should we reverse the text list if we display up?
   StringTableEntry mBitmapName; // DAW: Added
   Point2I mBitmapBounds; // DAW: Added
   TextureHandle mTextureNormal; // DAW: Added
   TextureHandle mTextureDepressed; // DAW: Added

   virtual void addChildren();
   virtual void repositionPopup();

public:
   GuiPopUpMenuCtrl(void);
   ~GuiPopUpMenuCtrl();   
   GuiScrollCtrl::Region mScrollDir;
   bool onWake(); // DAW: Added
   bool onAdd();
   void onSleep();
   void setBitmap(const char *name); // DAW: Added
   void sort();
   void sortID(); // DAW: Added
   void addEntry(const char *buf, S32 id, U32 scheme = 0);
   void addScheme(U32 id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL);
   void onRender(Point2I offset, const RectI &updateRect);
   void onAction();
   virtual void closePopUp();
   void clear();
   void onMouseDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   void onMouseEnter(const GuiEvent &event); // DAW: Added
   void onMouseLeave(const GuiEvent &); // DAW: Added
   void setupAutoScroll(const GuiEvent &event);
   void autoScroll();
   bool onKeyDown(const GuiEvent &event);
   void reverseTextList();
   bool getFontColor(ColorI &fontColor, S32 id, bool selected, bool mouseOver);
   bool getColoredBox(ColorI &boxColor, S32 id); // DAW: Added

   S32 getSelected();
   void setSelected(S32 id, bool bNotifyScript = true);
   void setFirstSelected(); // DAW: Added
   void setNoneSelected();	// DAW: Added
   const char *getScriptValue();
   const char *getTextById(S32 id);
   S32 findText( const char* text );
   S32 getNumEntries()   { return( mEntries.size() ); }
   void replaceText(S32);

   DECLARE_CONOBJECT(GuiPopUpMenuCtrl);
   static void initPersistFields(void);

};

#endif //_GUI_POPUPMENU_CTRL_H
