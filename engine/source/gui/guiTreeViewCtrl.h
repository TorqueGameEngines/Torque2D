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

#ifndef _GUI_TREEVIEWCTRL_H
#define _GUI_TREEVIEWCTRL_H

#include "collection/bitSet.h"
#include "math/mRect.h"
#include "gui/guiControl.h"
#include "gui/guiArrayCtrl.h"

//------------------------------------------------------------------------------

class GuiTreeViewCtrl : public GuiArrayCtrl
{
   private:
      typedef GuiArrayCtrl Parent;

   public:
      /// @section GuiTreeViewCtrl_Intro Introduction
      /// @nosubgrouping

      ///
      struct Item
      {

         enum ItemState
         {
            Selected       = BIT(0),
            Expanded       = BIT(1),
            Focus          = BIT(2),
            MouseOverBmp   = BIT(3),
            MouseOverText  = BIT(4),
            InspectorData  = BIT(5), ///< Set if we're representing some inspector
                                     /// info (ie, use mInspectorInfo, not mScriptInfo)
            VirtualParent  = BIT(6), ///< This indicates that we should be rendered as
                                     ///  a parent even though we don't have any children.
                                     ///  This is useful for preventing scenarios where
                                     ///  we might want to create thousands of
                                     ///  Items that might never be shown (for instance
                                     ///  if we're browsing the object hierarchy in
                                     ///  Torque, which might have thousands of objects).
         };

         BitSet32                mState;
         SimObjectPtr<GuiControlProfile> mProfile;
         S16                     mId;
         U16                     mTabLevel;
         Item *                  mParent;
         Item *                  mChild;
         Item *                  mNext;
         Item *                  mPrevious;
         S32                     mIcon; //stores the icon that will represent the item in the tree
         S32                     mDataRenderWidth; /// this stores the pixel width needed
                                                   /// to render the item's data in the 
                                                   /// onRenderCell function to optimize
                                                   /// for speed.


         Item( GuiControlProfile *pProfile );
         ~Item();

         struct ScriptTag
         {
            S8                      mNormalImage;
            S8                      mExpandedImage;
            char*                   mText;
            char*                   mValue;
         } mScriptInfo;
         struct InspectorTag
         {
            SimObjectPtr<SimObject> mObject;
         } mInspectorInfo;

         /// @name Get Methods
         /// @{

         ///
         const S8 getNormalImage() const;
         const S8 getExpandedImage() const;
         char *getText();
         char *getValue();
         inline const S16 getID() const { return mId; };
         SimObject *getObject();
         const U32 getDisplayTextLength();
         const S32 getDisplayTextWidth(GFont *font);
         void getDisplayText(U32 bufLen, char *buf);
         /// @}


         /// @name Set Methods
         /// @{

         /// Set whether an item is expanded or not (showing children or having them hidden)
         void setExpanded(const bool f);
         /// Set the image to display when an item IS expanded
         void setExpandedImage(const S8 id);
         /// Set the image to display when an item is NOT expanded
         void setNormalImage(const S8 id);
         /// Assign a SimObject pointer to an inspector data item
         void setObject(SimObject *obj);
         /// Set the items displayable text (caption)
         void setText(char *txt);
         /// Set the items script value (data)
         void setValue(const char *val);
         /// Set the items virtual parent flag
         void setVirtualParent( bool value );
         /// @}


         /// @name State Retrieval
         /// @{

         /// Returns true if this item is expanded. For
         /// inspector objects, the expansion is stored
         /// on the SimObject, for other things we use our
         /// bit vector.
         const bool isExpanded() const;

         /// Returns true if an item is inspector data
         /// or false if it's just an item.
         inline const bool isInspectorData() const { return mState.test(InspectorData); };

         /// Returns true if we should show the expand art
         /// and make the item interact with the mouse as if
         /// it were a parent.
         const bool isParent() const;
         /// @}

         /// @name Searching Methods
         /// @{

         /// Find an inspector data item by it's SimObject pointer
         Item *findChildByValue(const SimObject *obj);

         /// Find a regular data item by it's script value
         Item *findChildByValue(StringTableEntry Value);
         /// @}

      };

      /// @name Enums
      /// @{

      ///
      enum TreeState
      {
         RebuildVisible    = BIT(0), ///< Temporary flag, we have to rebuild the tree.
         IsInspector       = BIT(1), ///< We are mapping a SimObject hierarchy.
         IsEditable        = BIT(2), ///< We allow items to be moved around.
         ShowTreeLines     = BIT(3), ///< Should we render tree lines or just icons?
         BuildingVisTree   = BIT(4), ///< We are currently building the visible tree (prevent recursion)
      };

protected:
  		enum
		{
         MaxIcons = 32,
		};

      enum Icons
      {
         Default1 = 0,
         SimGroup1,
         SimGroup2,
         SimGroup3,
         SimGroup4,
         Audio,
         Camera,
         Hidden,
         Lock1,
         Lock2,
         Default,
         Icon31,
         Icon32
      };

      enum mDragMidPointFlags
      {
            NomDragMidPoint,
            AbovemDragMidPoint,
            BelowmDragMidPoint
      };

      ///
      enum HitFlags
      {
         OnIndent       = BIT(0),
         OnImage        = BIT(1),
         OnText         = BIT(2),
         OnRow          = BIT(3),
      };

      ///
      enum BmpIndices
      {
         BmpDunno,
         BmpLastChild,
         BmpChild,
         BmpExp,
         BmpExpN,
         BmpExpP,
         BmpExpPN,
         BmpCon,
         BmpConN,
         BmpConP,
         BmpConPN,
         BmpLine,
         BmpGlow,
      };


      /// @}
public:

      ///
      Vector<Item*>           mItems;
      Vector<Item*>           mVisibleItems;
      Vector<Item*>           mSelectedItems;
      Vector<S32>             mSelected;     ///< Used for tracking stuff that was
                                             ///  selected, but may not have been
                                             ///  created at time of selection
      S32                     mItemCount;
      Item *                  mItemFreeList; ///< We do our own free list, as we
                                             ///  we want to be able to recycle
                                             ///  item ids and do some other clever
                                             ///  things.
      Item *                  mRoot;
      S32                     mInstantGroup;
      S32                     mMaxWidth;
      S32                     mSelectedItem;
      S32                     mDraggedToItem;
      S32                     mTempItem;
      S32                     mStart;
      BitSet32                mFlags;

protected:
      TextureHandle mIconTable[MaxIcons];

      // for debugging
      bool mDebug;

      S32               mTabSize;
      S32               mTextOffset;
      bool              mFullRowSelect;
      S32               mItemHeight;
      bool              mDestroyOnSleep;
      bool              mSupportMouseDragging;
      bool              mMultipleSelections;
      bool              mDeleteObjectAllowed;
      bool              mDragToItemAllowed;

      S32               mOldDragY;
      S32               mCurrentDragCell;
      S32               mPreviousDragCell;
      S32               mDragMidPoint;
      bool              mMouseDragged;

      StringTableEntry  mBitmapBase;
      TextureHandle	   mTexRollover;
      TextureHandle	   mTexSelected;

      // Hack to get passed always recursively building tree EVERY TICK!
      S32               mTicksPassed;
      S32               mTreeRefreshInterval;

      ColorI   mAltFontColor;
      ColorI   mAltFontColorHL;
      ColorI   mAltFontColorSE;

      SimObjectPtr<SimObject> mRootObject;

      void destroyChildren(Item * item, Item * parent);
      void destroyItem(Item * item);
      void destroyTree();

      void deleteItem(Item *item);

      void buildItem(Item * item, U32 tabLevel, bool bForceFullUpdate = false);

      bool hitTest(const Point2I & pnt, Item* & item, BitSet32 & flags);

      virtual bool onVirtualParentBuild(Item *item, bool bForceFullUpdate = false);
      virtual bool onVirtualParentExpand(Item *item);
      virtual bool onVirtualParentCollapse(Item *item);
      virtual void onItemSelected( Item *item );

      void addInspectorDataItem(Item *parent, SimObject *obj);

   public:
      GuiTreeViewCtrl();
      virtual ~GuiTreeViewCtrl();

      /// Used for syncing the mSelected and mSelectedItems lists.
      void syncSelection();

      void lockSelection(bool lock);
      void hideSelection(bool hide);
      void addSelection(S32 itemId);

      /// Should use addSelection and removeSelection when calling from script
      /// instead of setItemSelected. Use setItemSelected when you want to select
      /// something in the treeview as it has script call backs.
      void removeSelection(S32 itemId);

      /// Sets the flag of the item with the matching itemId.
      bool setItemSelected(S32 itemId, bool select);
      bool setItemExpanded(S32 itemId, bool expand);
      bool setItemValue(S32 itemId, StringTableEntry Value);

      const char * getItemText(S32 itemId);
      const char * getItemValue(S32 itemId);
      StringTableEntry getTextToRoot(S32 itemId, const char *delimiter = "");

      Item * getItem(S32 itemId);
      Item * createItem(S32 icon);
      bool editItem( S32 itemId, const char* newText, const char* newValue );

      // insertion/removal
      void unlinkItem(Item * item);
      S32 insertItem(S32 parentId, const char * text, const char * value = "", const char * iconString = "", S16 normalImage = 0, S16 expandedImage = 1);
      bool removeItem(S32 itemId);
      void removeAllChildren(S32 itemId); // Remove all children of the given item

      bool buildIconTable(const char * icons);

      void setInstantGroup(SimObject * obj);

      S32 getIcon(const char * iconString);

      // tree items
      const S32 getFirstRootItem() const;
      S32 getChildItem(S32 itemId);
      S32 getParentItem(S32 itemId);
      S32 getNextSiblingItem(S32 itemId);
      S32 getPrevSiblingItem(S32 itemId);
      S32 getItemCount();
      S32 getSelectedItem();
      S32 getSelectedItem(S32 index); // Given an item's index in the selection list, return its itemId
      S32 getSelectedItemsCount() {return mSelectedItems.size();} // Returns the number of selected items
      void moveItemUp( S32 itemId );
      void moveItemDown( S32 itemId );

      // misc.
      bool scrollVisible( Item *item );
      bool scrollVisible( S32 itemId );
      bool scrollVisibleByObjectId( S32 objID );
      
      void deleteSelection();
      void clearSelection();

      S32 findItemByName(const char *name);
      S32 findItemByObjectId(S32 iObjId);

      // GuiControl
      bool onWake();
      void onSleep();
      void onPreRender();
      bool onKeyDown( const GuiEvent &event );
		void onTouchDown(const GuiEvent &event);
      void onMiddleMouseDown(const GuiEvent &event);
      void onTouchMove(const GuiEvent &event);
      void onTouchEnter(const GuiEvent &event);
      void onTouchLeave(const GuiEvent &event);
      void onRightMouseDown(const GuiEvent &event);
      void onTouchDragged(const GuiEvent &event);
      void onTouchUp(const GuiEvent &event);

      /// Returns false if the object is a child of one of the inner items.
      bool childSearch(Item * item, SimObject *obj, bool yourBaby);

      /// Find immediately available inspector items (eg ones that aren't children of other inspector items)
      /// and then update their sets
      void inspectorSearch(Item * item, Item * parent, SimSet * parentSet, SimSet * newParentSet);

      // GuiArrayCtrl
      void onRenderCell(Point2I offset, Point2I cell, bool, bool);
      void onRender(Point2I offset, const RectI &updateRect);

      static void initPersistFields();

      void inspectObject(SimObject * obj, bool okToEdit);
      void buildVisibleTree(bool bForceFullUpdate = false);

      DECLARE_CONOBJECT(GuiTreeViewCtrl);
};

#endif
