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

// Revision History:
// January 3, 2004		David Wyand		Added onMouseEnter() and onMouseLeave() methods to
//										GuiPopUpMenuCtrl class.  Also added the bool mMouseOver
//										to the same class.
// January 3, 2004		David Wyand		Added the bool mBackgroundCancel to the GuiPopUpMenuCtrl
//										class.
// May 19, 2004			David Wyand		Added the bool usesColorBox and ColorI colorbox to the
//										Entry structure of the GuiPopUpMenuCtrl class.  These
//										are used to draw a colored rectangle beside the text in
//										the list.
// November 16, 2005	David Wyand		Added the method setNoneSelected() to set none of the
//										items as selected.  Use this over setSelected(-1); when
//										there are negative IDs in the item list.
// January 11, 2006		David Wyand		Added the method setFirstSelected() to set the first
//										item to be the selected one.  Handy when you don't know
//										the ID of the first item.
//

#ifndef _GUIPOPUPCTRLEX_H_
#define _GUIPOPUPCTRLEX_H_

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
class GuiPopUpMenuCtrlEx;
class GuiPopupTextListCtrlEx;

class GuiPopUpBackgroundCtrlEx : public GuiControl
{
protected:
   GuiPopUpMenuCtrlEx *mPopUpCtrl;
   GuiPopupTextListCtrlEx *mTextList; 
public:
   GuiPopUpBackgroundCtrlEx(GuiPopUpMenuCtrlEx *ctrl, GuiPopupTextListCtrlEx* textList);
   void onTouchDown(const GuiEvent &event);
};

class GuiPopupTextListCtrlEx : public GuiTextListCtrl
{
   private:
      typedef GuiTextListCtrl Parent;

      bool hasCategories();

   protected:
      GuiPopUpMenuCtrlEx *mPopUpCtrl;

   public:
      GuiPopupTextListCtrlEx(); // for inheritance
      GuiPopupTextListCtrlEx(GuiPopUpMenuCtrlEx *ctrl);

      // GuiArrayCtrl overload:
      void onCellSelected(Point2I cell);

      // GuiControl overloads:
      bool onKeyDown(const GuiEvent &event);
      void onTouchUp(const GuiEvent &event);
      void onTouchMove(const GuiEvent &event);
      void onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver);
};

class GuiPopUpMenuCtrlEx : public GuiControl
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
   GuiPopupTextListCtrlEx *mTl;
   GuiScrollCtrl *mSc;
   GuiPopUpBackgroundCtrlEx *mBackground;
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
   bool mHotTrackItems;
   StringTableEntry mBitmapName; // DAW: Added
   Point2I mBitmapBounds; // DAW: Added
   TextureHandle mTextureNormal; // DAW: Added
   TextureHandle mTextureDepressed; // DAW: Added

   virtual void addChildren();
   virtual void repositionPopup();

  public:
   GuiPopUpMenuCtrlEx(void);
   ~GuiPopUpMenuCtrlEx();   
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
   void onTouchDown(const GuiEvent &event);
   void onTouchUp(const GuiEvent &event);
   void onTouchEnter(const GuiEvent &event); // DAW: Added
   void onTouchLeave(const GuiEvent &); // DAW: Added
   void setupAutoScroll(const GuiEvent &event);
   void autoScroll();
   bool onKeyDown(const GuiEvent &event);
   void reverseTextList();
   bool getFontColor(ColorI &fontColor, S32 id, bool selected, bool mouseOver);
   bool getColoredBox(ColorI &boxColor, S32 id); // DAW: Added

   S32 getSelected();
   void setSelected(S32 id);
   void setFirstSelected(); // DAW: Added
   void setNoneSelected();	// DAW: Added
   const char *getScriptValue();
   const char *getTextById(S32 id);
   S32 findText( const char* text );
   S32 getNumEntries()   { return( mEntries.size() ); }
   void replaceText(S32);
   
   DECLARE_CONOBJECT(GuiPopUpMenuCtrlEx);
   static void initPersistFields(void);

};

#endif //_GUIIMPROVEDPOPUPCTRL_H_
