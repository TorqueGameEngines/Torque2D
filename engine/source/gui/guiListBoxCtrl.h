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
#ifndef _GUI_LISTBOXCTRL_H_
#define _GUI_LISTBOXCTRL_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/guiDefaultControlRender.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif


class GuiListBoxCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;
public:

   GuiListBoxCtrl();
   ~GuiListBoxCtrl();
   DECLARE_CONOBJECT(GuiListBoxCtrl);

   class LBItem
   {
   public:
      StringTableEntry  itemText;
      bool              isSelected;
	  bool				isActive;
	  int				ID;
      void*             itemData;
      ColorF            color;
      bool              hasColor;

	  static bool sIncreasing;

	  // Compare Functions
	  static bool compByID(const LBItem *a, const LBItem *b)
	  {
		  bool res = a->ID < b->ID;
		  return (sIncreasing ? res : !res);
	  }
	  static bool compByText(const LBItem *a, const LBItem *b)
	  {
		  char buf[512];
		  char bufB[512];

		  dSprintf(buf, 512, "%s", a->itemText);
		  dSprintf(bufB, 512, "%s", b->itemText);

		  S32 res = dStricmp(buf, bufB);
		  bool result = res <= 0;
		  return (sIncreasing ? result : !result);
	  }
   };

   VectorPtr<LBItem*>   mItems;
   VectorPtr<LBItem*>   mSelectedItems;
   bool                 mMultipleSelections;
   Point2I              mItemSize;
   bool                 mFitParentWidth;
   LBItem*              mLastClickItem;

   

   // Persistence
   static void       initPersistFields();   

   // Item Accessors
   S32               getItemCount();
   S32               getSelCount();
   S32               getSelectedItem();
   void              getSelectedItems( Vector<S32> &Items );
   S32               getItemIndex( LBItem *item );
   StringTableEntry  getItemText( S32 index );
   
   void				 setSelectionInternal(StringTableEntry text);
   virtual void      setCurSel( S32 index );
   void              setCurSelRange( S32 start, S32 stop );
   void              setItemText( S32 index, StringTableEntry text );

   S32               addItem( StringTableEntry text, void *itemData = NULL );
   S32               addItemWithColor( StringTableEntry text, ColorF color = ColorF(-1, -1, -1), void *itemData = NULL);
   S32				 addItemWithID(StringTableEntry text, S32 ID = 0, void *itemData = NULL);
   S32               insertItem( S32 index, StringTableEntry text, void *itemData = NULL );
   S32               insertItemWithColor( S32 index, StringTableEntry text, ColorF color = ColorF(-1, -1, -1), void *itemData = NULL);
   S32               findItemText( StringTableEntry text, bool caseSensitive = false );

   void              setItemColor(S32 index, ColorF color);
   void              clearItemColor(S32 index);
   void				 clearAllColors();
   ColorF			 getItemColor(S32 index);
   bool				 getItemHasColor(S32 index);

   void				 setItemID(S32 index, S32 ID);
   S32				 getItemID(S32 index);
   S32				 findItemID(S32 ID);

   void				 setItemActive(S32 index);
   void				 setItemInactive(S32 index);
   bool				 getItemActive(S32 index);

   void              deleteItem( S32 index );
   void              clearItems();
   void              clearSelection();
   void              removeSelection( LBItem *item, S32 index );
   void              removeSelection( S32 index );
   virtual void      addSelection( LBItem *item, S32 index );
   void              addSelection( S32 index );
   inline void       setMultipleSelection(bool allowMultipleSelect = true) { mMultipleSelections = allowMultipleSelect; };
   inline bool       getMultipleSelection() { return mMultipleSelections; };

   // Sizing
   void              updateSize();
   virtual void      parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
   virtual bool      onWake();
   virtual void		 addObject(SimObject *obj);

   // Rendering
   virtual void      onRender( Point2I offset, const RectI &updateRect );
   virtual void      onRenderItem( RectI &itemRect, LBItem *item );
   virtual void		 ScrollToIndex(const S32 targetIndex);

   // Mouse Events
   virtual void      onTouchDown( const GuiEvent &event );
   virtual void      onTouchDragged(const GuiEvent &event);
   virtual bool		 GuiListBoxCtrl::onKeyDown(const GuiEvent &event);

   // Sorting
   virtual void		 sortByText(bool increasing = true);
   virtual void		 sortByID(bool increasing = true);

protected:
	GuiControl		*caller;
};

#endif