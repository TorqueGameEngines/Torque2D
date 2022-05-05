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
#include "memory/frameAllocator.h"
#include "gui/guiTreeViewCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiTypes.h"
#include "platform/event.h"

IMPLEMENT_CONOBJECT(GuiTreeViewCtrl);

//-----------------------------------------------------------------------------
// TreeView Item 
//-----------------------------------------------------------------------------
GuiTreeViewCtrl::Item::Item( GuiControlProfile *pProfile )
{
   AssertFatal( pProfile != NULL , "Cannot create a tree item without a valid tree and control profile!");

   mState               = 0;
   mId                  = -1;
   mTabLevel            = 0;
   mIcon                = 0;
   mDataRenderWidth     = 0;
   mScriptInfo.mText    = NULL;
   mScriptInfo.mValue   = NULL;
   mInspectorInfo.mObject = NULL;
   mParent              = NULL;
   mChild               = NULL;
   mNext                = NULL;
   mPrevious            = NULL;
   mProfile             = pProfile;
}

GuiTreeViewCtrl::Item::~Item()
{
   if( ! mState.test(InspectorData) )
   {
      if ( getText() )
      {
         delete [] getText();
         setText(NULL);
      }

      if ( getValue() )
      {
         delete [] getValue();
         setValue( NULL );
      }
   }
}

void GuiTreeViewCtrl::Item::setNormalImage(S8 id)
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to set normal image %d for item %d, which is InspectorData!", id, mId);
      return;
   }

   mScriptInfo.mNormalImage = id;
}

void GuiTreeViewCtrl::Item::setExpandedImage(S8 id)
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to set expanded image %d for item %d, which is InspectorData!", id, mId);
      return;
   }

   mScriptInfo.mExpandedImage = id;
}

void GuiTreeViewCtrl::Item::setText(char *txt)
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to set text for item %d, which is InspectorData!", mId);
      return;
   }

   mScriptInfo.mText = txt;


   // Update Render Data
   if( !mProfile.isNull() )
      mDataRenderWidth = getDisplayTextWidth( mProfile->getFont() );

}

void GuiTreeViewCtrl::Item::setValue(const char *val)
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to set value for item %d, which is InspectorData!", mId);
      return;
   }

   mScriptInfo.mValue = const_cast<char*>(val); // mValue really ought to be a StringTableEntry

   // Update Render Data
   if( !mProfile.isNull() )
      mDataRenderWidth = getDisplayTextWidth( mProfile->getFont());

}

const S8 GuiTreeViewCtrl::Item::getNormalImage() const
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to get the normal image for item %d, which is InspectorData!", mId);
      return 0; // fail safe for width determinations
   }

   return mScriptInfo.mNormalImage;
}

const S8 GuiTreeViewCtrl::Item::getExpandedImage() const
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to get the expanded image for item %d, which is InspectorData!", mId);
      return 0; // fail safe for width determinations
   }

   return mScriptInfo.mExpandedImage;
}

char *GuiTreeViewCtrl::Item::getText()
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to get the text for item %d, which is InspectorData!", mId);
      return NULL;
   }

   return mScriptInfo.mText;
}

char *GuiTreeViewCtrl::Item::getValue()
{
   if(mState.test(InspectorData))
   {
      Con::errorf("Tried to get the value for item %d, which is InspectorData!", mId);
      return NULL;
   }

   return mScriptInfo.mValue;
}

void GuiTreeViewCtrl::Item::setObject(SimObject *obj)
{
   if(!mState.test(InspectorData))
   {
      Con::errorf("Tried to set the object for item %d, which is not InspectorData!", mId);
      return;
   }

   mInspectorInfo.mObject = obj;

   // Update Render Data
   if( !mProfile.isNull() )
      mDataRenderWidth = getDisplayTextWidth( mProfile->getFont());

}

SimObject *GuiTreeViewCtrl::Item::getObject()
{
   if(!mState.test(InspectorData))
   {
      Con::errorf("Tried to get the object for item %d, which is not InspectorData!", mId);
      return NULL;
   }

   return mInspectorInfo.mObject;
}


const U32 GuiTreeViewCtrl::Item::getDisplayTextLength()
{
   if(mState.test(Item::InspectorData))
   {
      SimObject *obj = getObject();

      if(!obj)
         return 0;

      // For the results buffer, it's prefix along with a bunch of other stuff.
      // So we'll be mostly accurate and add a bit of fudge.
      return (16
         + (obj->getName() ? dStrlen(obj->getName()) : 0) + dStrlen(obj->getClassName())
         + dStrlen(obj->getIdString()) + 20
         );
   }

   char *pText = getText();
   if( pText == NULL )
      return 0;

   return dStrlen( pText );
}

void GuiTreeViewCtrl::Item::getDisplayText(U32 bufLen, char *buf)
{
   FrameAllocatorMarker txtAlloc;

   if(mState.test(Item::InspectorData))
   {
      // Inspector data!
      SimObject *pObject = getObject();
      if(pObject)
      {
         const char* pObjName = pObject->getName();
         const char* pInternalName = pObject->getInternalName();
         if( pObjName != NULL )
            dSprintf(buf, bufLen, "%d: %s - %s", pObject->getId(), pObject->getClassName(), pObjName );
         else if ( pInternalName != NULL )
            dSprintf(buf, bufLen, "%d: %s [%s]", pObject->getId(), pObject->getClassName(), pInternalName);
         else
            dSprintf(buf, bufLen, "%d: %s", pObject->getId(), pObject->getClassName());

      }
   }
   else
   {
      // Script data! (copy it in)
      dStrncpy(buf, getText(), bufLen);
   }
}

const S32 GuiTreeViewCtrl::Item::getDisplayTextWidth(GFont *font)
{
   if( !font )
      return 0;

   FrameAllocatorMarker txtAlloc;
   U32 bufLen = getDisplayTextLength();
   if( bufLen == 0 )
      return 0;

   char *buf = (char*)txtAlloc.alloc(bufLen);
   getDisplayText(bufLen, buf);

   return font->getStrWidth(buf);
}

const bool GuiTreeViewCtrl::Item::isParent() const
{
   if(mState.test(VirtualParent))
   {
      if( !isInspectorData() )
         return true;

      // Does our object have any children?
      if(mInspectorInfo.mObject)
      {
         SimSet *pSimSet = dynamic_cast<SimSet*>( (SimObject*)mInspectorInfo.mObject);
         if ( pSimSet != NULL && pSimSet->size() > 0)
            return pSimSet->size();
      }
   }

   // Otherwise, just return whether the child list is populated.
   return mChild;
}

const bool GuiTreeViewCtrl::Item::isExpanded() const
{
   if(mState.test(InspectorData))
      return mInspectorInfo.mObject ? mInspectorInfo.mObject->isExpanded() : false;
   else
      return mState.test(Expanded);
}

void GuiTreeViewCtrl::Item::setExpanded(bool f)
{
   if(mState.test(InspectorData) && !mInspectorInfo.mObject.isNull() )
      mInspectorInfo.mObject->setExpanded(f);
   else
      mState.set(Expanded, f);
}

void GuiTreeViewCtrl::Item::setVirtualParent( bool value )
{
   mState.set(VirtualParent, value);
}

GuiTreeViewCtrl::Item *GuiTreeViewCtrl::Item::findChildByValue(const SimObject *obj)
{
   // Iterate over our children and try to find the given
   // SimObject

   Item *pResultObj = mChild;

   while(pResultObj)
   {
      // Skip non-inspector data stuff.
      if(pResultObj->mState.test(InspectorData))
      {
         if(pResultObj->getObject() == obj)
            return pResultObj;
      }
      pResultObj = pResultObj->mNext;
   }

   // If the loop terminated we are NULL, otherwise we have the result in res.
   return NULL;
}

GuiTreeViewCtrl::Item *GuiTreeViewCtrl::Item::findChildByValue( StringTableEntry Value )
{
   // Iterate over our children and try to find the given Value
   // Note : This is a case-insensitive search
   Item *pResultObj = mChild;

   while(pResultObj)
   {

      // check the script value of the item against the specified value
      if( pResultObj->mScriptInfo.mValue != NULL && dStricmp( pResultObj->mScriptInfo.mValue, Value ) == 0 )
         return pResultObj;
      pResultObj = pResultObj->mNext;
   }
   // If the loop terminated we didn't find an item with the specified script value
   return NULL;
}


//------------------------------------------------------------------------------

GuiTreeViewCtrl::GuiTreeViewCtrl()
{
   VECTOR_SET_ASSOCIATION(mItems);
   VECTOR_SET_ASSOCIATION(mVisibleItems);
   VECTOR_SET_ASSOCIATION(mSelectedItems);
   VECTOR_SET_ASSOCIATION(mSelected);

   mItemFreeList  =  NULL;
   mRoot          =  NULL;
   mInstantGroup  =  0;
   mItemCount     =  0;
   mSelectedItem  =  0;
   mStart         =  0;
   mTicksPassed   =  0;
   mTreeRefreshInterval   =  30;

   mDraggedToItem =  0;
   mOldDragY      = 0;
   mCurrentDragCell = 0;
   mPreviousDragCell = 0;
   mDragMidPoint = NomDragMidPoint;
   mMouseDragged = false;
   mDebug = false;

   // persist info..
   mTabSize = 16;
   mTextOffset = 2;
   mFullRowSelect = false;
   mItemHeight = 20;

   //
   setSize(Point2I(1, 0));

   // Set up default state
   mFlags.set(ShowTreeLines);
   mFlags.set(IsEditable, false);

   mDestroyOnSleep = true;
   mSupportMouseDragging = true;
   mMultipleSelections = true;
   mDeleteObjectAllowed = true;
   mDragToItemAllowed = true;

   mBitmapBase       = StringTable->EmptyString;
   mTexRollover      = NULL;
   mTexSelected      = NULL;

   
}

GuiTreeViewCtrl::~GuiTreeViewCtrl()
{
   destroyTree();
}


//------------------------------------------------------------------------------
void GuiTreeViewCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("TreeView");
   addField("tabSize",              TypeS32,    Offset(mTabSize,              GuiTreeViewCtrl));
   addField("textOffset",           TypeS32,    Offset(mTextOffset,           GuiTreeViewCtrl));
   addField("fullRowSelect",        TypeBool,   Offset(mFullRowSelect,        GuiTreeViewCtrl));
   addField("itemHeight",           TypeS32,    Offset(mItemHeight,           GuiTreeViewCtrl));
   addField("destroyTreeOnSleep",   TypeBool,   Offset(mDestroyOnSleep,       GuiTreeViewCtrl));
   addField("MouseDragging",        TypeBool,   Offset(mSupportMouseDragging, GuiTreeViewCtrl));
   addField("MultipleSelections",   TypeBool,   Offset(mMultipleSelections,   GuiTreeViewCtrl));
   addField("DeleteObjectAllowed",  TypeBool,   Offset(mDeleteObjectAllowed,  GuiTreeViewCtrl));
   addField("DragToItemAllowed",    TypeBool,   Offset(mDragToItemAllowed,    GuiTreeViewCtrl));
   endGroup("TreeView");
}

//------------------------------------------------------------------------------

GuiTreeViewCtrl::Item * GuiTreeViewCtrl::getItem(S32 itemId)
{
   if((itemId > 0) && (itemId <= mItems.size()))
   {
      if (mItems[itemId-1] != NULL)
         return(mItems[itemId-1]);
   }

   return(0);
}

//------------------------------------------------------------------------------

GuiTreeViewCtrl::Item * GuiTreeViewCtrl::createItem(S32 icon)
{
   Item * pNewItem = NULL;

   // grab from the free list?
   if( mItemFreeList )
   {
      pNewItem = mItemFreeList;
      mItemFreeList = pNewItem->mNext;

      // re-add to vector
      mItems[ pNewItem->mId - 1 ] = pNewItem;
   }
   else
   {
      pNewItem = new Item( mProfile );

      AssertFatal( pNewItem != NULL, "Fatal : unable to allocate tree item!");

      mItems.push_back( pNewItem );

      // set the id
      pNewItem->mId = mItems.size();
   }

   // reset
   if (icon)
      pNewItem->mIcon = icon;
   else
      pNewItem->mIcon = Default; //default icon to stick next to an item

   pNewItem->mState.clear();
   pNewItem->mState = 0;
   pNewItem->mTabLevel = 0;

   // Null out item pointers
   pNewItem->mNext = 0;
   pNewItem->mPrevious = 0;
   pNewItem->mChild = 0;
   pNewItem->mParent = 0;

   mItemCount++;

   return pNewItem;
}

//------------------------------------------------------------------------------
void GuiTreeViewCtrl::destroyChildren(Item * item, Item * parent)
{
   if ( !item  || item == parent )
      return;

   if ( item->isParent() && item->mChild )
      destroyChildren(item->mChild, parent);
   else if( item->mNext )
      destroyChildren(item->mNext, parent);
   else
   {
      // destroy the item and back up
      Item * pPrevItem = item->mPrevious;
      destroyItem( item );
      destroyChildren( pPrevItem, parent );
   }
}
void GuiTreeViewCtrl::destroyItem(Item * item)
{
   if(!item)
      return;

   if(item->isInspectorData())
   {
      // make sure the SimObjectPtr is clean!
      //Con::executef(this,2, "onDeleteObject",Con::getIntArg(item->mInspectorInfo.mObject->getIdString()));
      SimObject *pObject = item->getObject();
      if( pObject && pObject->isProperlyAdded() )
         pObject->deleteObject();

      item->setObject( NULL );
   }
   else
   {
      char *pCleanup = item->getText();
      // Clean up the memory...
      if ( pCleanup != NULL )
      {
         delete []pCleanup;
         item->setText( NULL );
      }

      pCleanup = item->getValue();
      if ( pCleanup != NULL )
      {
         delete []pCleanup;
         item->setValue( NULL );
      }
   }

   // unlink
   if( item->mPrevious )
      item->mPrevious->mNext = item->mNext;
   if( item->mNext )
      item->mNext->mPrevious = item->mPrevious;
   if( item->mParent && ( item->mParent->mChild == item ) )
      item->mParent->mChild = item->mNext;

   // remove from vector
   mItems[item->mId-1] = 0;

   // set as root free item
   item->mNext = mItemFreeList;
   mItemFreeList = item;
   mItemCount--;
}

//------------------------------------------------------------------------------
void GuiTreeViewCtrl::deleteItem(Item *item)
{
   removeItem(item->mId);
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::destroyTree()
{
   // clear the item list
   for(U32 i = 0; i < (U32)mItems.size(); i++)
   {
      Item *pFreeItem = mItems[ i ];
      if( pFreeItem != NULL )
         delete pFreeItem;
   }

   mItems.clear();

   // clear the free list
   while(mItemFreeList)
   {
      Item *next = mItemFreeList->mNext;
      delete mItemFreeList;
      mItemFreeList = next;
   }

   mVisibleItems.clear();
   mSelectedItems.clear();

   //
   mRoot          = NULL;
   mItemFreeList  = NULL;
   mItemCount     = 0;
   mSelectedItem  = 0;
   mDraggedToItem = 0;
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::buildItem( Item* item, U32 tabLevel, bool bForceFullUpdate )
{
   if (!item || !mActive || !isVisible() || !mProfile  )
      return;

   // If it's inspector data, make sure we still have it, if not, kill it.
   if(item->isInspectorData() && !item->getObject() )
   {
      removeItem(item->mId);
      return;
   }

   // If it's a virtual parent, give a chance to update itself...
   if(item->mState.test( Item::VirtualParent) )
   {
      // If it returns false the item has been removed.
      if(!onVirtualParentBuild(item, bForceFullUpdate))
         return;
   }

   item->mTabLevel = tabLevel;
   mVisibleItems.push_back( item );

   if ( mProfile != NULL && mProfile->getFont(mFontSizeAdjust) )
   {
      S32 width = ( tabLevel + 1 ) * mTabSize + item->getDisplayTextWidth(mProfile->getFont(mFontSizeAdjust));
      if ( mProfile->mBitmapArrayRects.size() > 0 )
         width += mProfile->mBitmapArrayRects[0].extent.x;
      
      width += (item->mTabLevel+1) * mItemHeight; // using mItemHeight for icon width, close enough
                                                  // this will only fail if somebody starts using super wide icons.

      if ( width > mMaxWidth )
         mMaxWidth = width;
   }

   // if expanded, then add all the children items as well
  if ( item->isExpanded() || bForceFullUpdate)
   {
      Item * child = item->mChild;
      while ( child )
      {
         // Bit of a hack so we can safely remove items as we
         // traverse.
         Item *pChildTemp = child;
         child = child->mNext;

         buildItem( pChildTemp, tabLevel + 1, bForceFullUpdate );
      }
   }
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::buildVisibleTree(bool bForceFullUpdate)
{
   // Recursion Prevention.
   if( mFlags.test( BuildingVisTree ) )
      return;
   mFlags.set( BuildingVisTree, true );

   mMaxWidth = 0;
   mVisibleItems.clear();

   // Update the flags.
   mFlags.clear(RebuildVisible);

   // build the root items
   Item *traverse = mRoot;
   while(traverse)
   {
      buildItem(traverse, 0, bForceFullUpdate);
      traverse = traverse->mNext;
   }

   // adjust the GuiArrayCtrl
   mCellSize.set(mMaxWidth+1, mItemHeight);
   setSize(Point2I(1, mVisibleItems.size()));
   syncSelection();

   // Done Recursing.
   mFlags.clear( BuildingVisTree );
}

//------------------------------------------------------------------------------

bool GuiTreeViewCtrl::scrollVisible( S32 itemId )
{
   Item* item = getItem(itemId);
   if(item)
      return scrollVisible(item);

   return false;
}
bool GuiTreeViewCtrl::scrollVisible( Item *item )
{
   // Now, make sure it's visible (ie, all parents expanded)
   Item *parent = item->mParent;

   if( !item->isInspectorData() && item->mState.test(Item::VirtualParent) )
      onVirtualParentExpand(item);

   while(parent)
   {
      parent->setExpanded(true);

      if( !parent->isInspectorData() && parent->mState.test(Item::VirtualParent) )
         onVirtualParentExpand(parent);

      parent = parent->mParent;
   }

   // Get our scroll-pappy, if any.
   GuiScrollCtrl *pScrollParent = dynamic_cast<GuiScrollCtrl*>( getParent() );

   if ( !pScrollParent )
   {
      Con::warnf("GuiTreeViewCtrl::scrollVisible - parent control is not a GuiScrollCtrl!");
      return false;
   }

   // And now, build the visible tree so we know where we have to scroll.
   buildVisibleTree();

   // All done, let's figure out where we have to scroll...
   for(S32 i=0; i<mVisibleItems.size(); i++)
   {
      if(mVisibleItems[i] == item)
      {
         pScrollParent->scrollRectVisible(RectI(0, i * mItemHeight, mMaxWidth, mItemHeight));
         return true;
      }
   }

   // If we got here, it's probably bad...
   Con::errorf("GuiTreeViewCtrl::scrollVisible - was unable to find specified item in visible list!");
   return false;
}

//------------------------------------------------------------------------------

S32 GuiTreeViewCtrl::insertItem(S32 parentId, const char * text, const char * value, const char * iconString, S16 normalImage, S16 expandedImage)
{
   if( ( parentId < 0 ) || ( parentId > mItems.size() ) )
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::insertItem: invalid parent id!");
      return 0;
   }

   if((parentId != 0) && (mItems[parentId-1] == 0))
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::insertItem: parent item invalid!");
      return 0;
   }


   const char * pItemText  = ( text != NULL ) ? dStrdup( text ) : "";
   const char * pItemValue = ( value != NULL ) ? dStrdup( value ) : "";

   S32 icon = getIcon(iconString);

   // create an item (assigns id)
   Item * pNewItem = createItem(icon);
   if( pNewItem == NULL )
      return 0;

//   // Ugh.  This code bothers me. - JDD
//   pNewItem->setText( new char[dStrlen( pItemText ) + 1] );
//   dStrcpy( pNewItem->getText(), pItemText );
//   pNewItem->setValue( new char[dStrlen( pItemValue ) + 1] );
//   dStrcpy( pNewItem->getValue(), pItemValue );
   // paxorr fix:
   char *tmp = new char[dStrlen( pItemText ) + 1];
   dStrcpy( tmp, pItemText );
   pNewItem->setText( tmp );
   tmp = new char[dStrlen( pItemValue ) + 1];
   dStrcpy( tmp, pItemValue );
   pNewItem->setValue( tmp );
   
   pNewItem->setNormalImage( (S8)normalImage );
   pNewItem->setExpandedImage( (S8)expandedImage );

   // root level?
   if(parentId == 0)
   {
      // insert back
      if( mRoot != NULL )
      {
         Item * pTreeTraverse = mRoot;
         while( pTreeTraverse != NULL && pTreeTraverse->mNext != NULL )
            pTreeTraverse = pTreeTraverse->mNext;

         pTreeTraverse->mNext = pNewItem;
         pNewItem->mPrevious = pTreeTraverse;
      }
      else
         mRoot = pNewItem;

      mFlags.set(RebuildVisible);
   }
   else if( mItems.size() >= ( parentId - 1 ) )
   {
      Item * pParentItem = mItems[parentId-1];

      // insert back
      if( pParentItem != NULL && pParentItem->mChild)
      {
         Item * pTreeTraverse = pParentItem->mChild;
         while( pTreeTraverse != NULL && pTreeTraverse->mNext != NULL )
            pTreeTraverse = pTreeTraverse->mNext;

         pTreeTraverse->mNext = pNewItem;
         pNewItem->mPrevious = pTreeTraverse;
      }
      else
         pParentItem->mChild = pNewItem;

      pNewItem->mParent = pParentItem;

      if( pParentItem->isExpanded() )
         mFlags.set(RebuildVisible);
   }

   //
   if(mFlags.test(RebuildVisible))
      buildVisibleTree();

   return pNewItem->mId;
}

//------------------------------------------------------------------------------

bool GuiTreeViewCtrl::removeItem(S32 itemId)
{
   // tree?
   if(itemId == 0)
   {
      destroyTree();
      return(true);
   }

   Item * item = getItem(itemId);
   if(!item)
   {
      //Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::removeItem: invalid item id!");
      return false;
   }

   // root?
   if(item == mRoot)
      mRoot = item->mNext;

   // Dispose of any children...
   if (item->mChild)
      destroyChildren(item->mChild, item);

   // Kill the item...
   destroyItem(item);

   // Update the rendered tree...
   buildVisibleTree();

   return true;
}


void GuiTreeViewCtrl::removeAllChildren(S32 itemId)
{
   Item * item = getItem(itemId);
   if(item)
   {
      destroyChildren(item->mChild, item);
   }
}
//------------------------------------------------------------------------------

const S32 GuiTreeViewCtrl::getFirstRootItem() const
{
   return (mRoot ? mRoot->mId : 0);
}

//------------------------------------------------------------------------------

S32 GuiTreeViewCtrl::getChildItem(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getChild: invalid item id!");
      return(0);
   }

   return(item->mChild ? item->mChild->mId : 0);
}

S32 GuiTreeViewCtrl::getParentItem(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getParent: invalid item id!");
      return(0);
   }

   return(item->mParent ? item->mParent->mId : 0);
}

S32 GuiTreeViewCtrl::getNextSiblingItem(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getNextSibling: invalid item id!");
      return(0);
   }

   return(item->mNext ? item->mNext->mId : 0);
}

S32 GuiTreeViewCtrl::getPrevSiblingItem(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getPrevSibling: invalid item id!");
      return(0);
   }

   return(item->mPrevious ? item->mPrevious->mId : 0);
}

//------------------------------------------------------------------------------

S32 GuiTreeViewCtrl::getItemCount()
{
   return(mItemCount);
}

S32 GuiTreeViewCtrl::getSelectedItem()
{
   return mSelectedItem;
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::moveItemUp( S32 itemId )
{
   GuiTreeViewCtrl::Item* pItem = getItem( itemId );
   if ( !pItem )
   {
      Con::errorf( ConsoleLogEntry::General, "GuiTreeViewCtrl::moveItemUp: invalid item id!");
      return;
   }

   Item * pParent   = pItem->mParent;
   Item * pPrevItem = pItem->mPrevious;
   if ( pPrevItem == NULL || pParent == NULL )
   {
      Con::errorf( ConsoleLogEntry::General, "GuiTreeViewCtrl::moveItemUp: Unable to move item up, bad data!");
      return;
   }

   //  Diddle the linked list!
   if ( pPrevItem->mPrevious )
      pPrevItem->mPrevious->mNext = pItem;
   else if ( pItem->mParent )
      pItem->mParent->mChild = pItem;

   if ( pItem->mNext )
      pItem->mNext->mPrevious = pPrevItem;

   pItem->mPrevious = pPrevItem->mPrevious;
   pPrevItem->mNext = pItem->mNext;
   pItem->mNext = pPrevItem;
   pPrevItem->mPrevious = pItem;

   // Update SimObjects if Appropriate.
   SimObject * pSimObject = NULL;
   SimSet    * pParentSet = NULL;

   // Fetch Current Add Set
   if( pParent->isInspectorData() )
      pParentSet = dynamic_cast<SimSet*>( pParent->getObject() );
   else
   {
      // parent is probably script data so we search up the tree for a
      // set to put our object in
      Item * pTraverse = pItem->mParent;
      while ( pTraverse != NULL && !pTraverse->isInspectorData() )
         pTraverse = pTraverse->mParent;

      // found an ancestor who is an inspectorData?
      pParentSet = pTraverse->isInspectorData() ? dynamic_cast<SimSet*>( pTraverse->getObject() ) : NULL;
   }

   // Reorder the item and make sure that the children of the item get updated
   // correctly prev item may be script... so find a prevItem if there is.
   // We only need to reorder if there you move it above an inspector item.
   if ( pSimObject != NULL && pParentSet != NULL )
   {
      Item * pTraverse = pItem->mNext;

      while(pTraverse)
      {
         if (pTraverse->isInspectorData())
            break;
         pTraverse = pTraverse->mNext;
      }

      if (pTraverse && pItem->getObject() &&  pTraverse->getObject())
         pParentSet->reOrder(pItem->getObject(), pTraverse->getObject());
   }

   buildVisibleTree();
}
void GuiTreeViewCtrl::moveItemDown( S32 itemId )
{
   GuiTreeViewCtrl::Item* item = getItem( itemId );
   if ( !item )
   {
      Con::errorf( ConsoleLogEntry::General, "GuiTreeViewCtrl::moveItemDown: invalid item id!");
      return;
   }

   Item* nextItem = item->mNext;
   if ( !nextItem )
   {
      Con::errorf( ConsoleLogEntry::General, "GuiTreeViewCtrl::moveItemDown: no next sibling?");
      return;
   }

   //  Diddle the linked list!
   if ( nextItem->mNext )
      nextItem->mNext->mPrevious = item;

   if ( item->mPrevious )
      item->mPrevious->mNext = nextItem;
   else if ( item->mParent )
      item->mParent->mChild = nextItem;

   item->mNext = nextItem->mNext;
   nextItem->mPrevious = item->mPrevious;
   item->mPrevious = nextItem;
   nextItem->mNext = item;

   // And update the simobjects if apppropriate...
   SimObject * simobj = NULL;
   if (item->isInspectorData())
      simobj = item->getObject();

   SimSet *parentSet = NULL;

   // grab the current parentSet if there is any...
   if(item->mParent->isInspectorData())
      parentSet = dynamic_cast<SimSet*>(item->mParent->getObject());
   else
   {
      // parent is probably script data so we search up the tree for a
      // set to put our object in
      Item * temp = item->mParent;
      while (!temp->isInspectorData())
         temp = temp->mParent;

      // found an ancestor who is an inspectorData?
      parentSet = temp->isInspectorData() ? dynamic_cast<SimSet*>(temp->getObject()) : NULL;
   }

   // Reorder the item and make sure that the children of the item get updated
   // correctly prev item may be script... so find a prevItem if there is.
   // We only need to reorder if there you move it above an inspector item.
   if (simobj && parentSet)
   {
      Item * temp = item->mPrevious;

      while(temp)
      {
         if (temp->isInspectorData())
            break;
         temp = temp->mPrevious;
      }

      if (temp && item->getObject() && temp->getObject())
         parentSet->reOrder(temp->getObject(), item->getObject());
   }

     buildVisibleTree();
}



//------------------------------------------------------------------------------

bool GuiTreeViewCtrl::onWake()
{
   if(!Parent::onWake() || !mProfile->constructBitmapArray())
      return false;

   // If destroy on sleep, then we have to give things a chance to rebuild.
   if(mDestroyOnSleep)
   {
      destroyTree();
      Con::executef(this, 1, "onWake");

      // (Re)build our icon table.
      const char * res = Con::executef(this, 1, "onDefineIcons");

      // If no icons were defined in script then use defaults.
      if(!(dAtob(res)))
      {
         buildIconTable(NULL);
      }
   }

   // Update the row height, if appropriate.
   // DEPRECIATED
   /*
   if(mProfile->mAutoSizeHeight)
   {
      // make sure it's big enough for both bitmap AND font...
      mItemHeight = getMax((S32)mFont->getHeight(), (S32)mProfile->mBitmapArrayRects[0].extent.y);
   }
   */

   return true;
}

void GuiTreeViewCtrl::onSleep()
{
   Parent::onSleep();

   // If appropriate, blast the tree. (We probably rebuild it on wake.)
   if( mDestroyOnSleep )
      destroyTree();
}

bool GuiTreeViewCtrl::buildIconTable(const char * icons)
{
   // Icons should be designated by the bitmap/png file names (minus the file extensions)
   // and separated by colons (:). This list should be synchronized with the Icons enum.

   // This is an abominal piece of code. -- BJG
   if (!icons)
   {
      icons =  "^Sandbox/gui/images/default:"
               "^Sandbox/gui/images/simgroup:"
               "^Sandbox/gui/images/simgroupClosed:"
               "^Sandbox/gui/images/simgroupSelected:"
               "^Sandbox/gui/images/simgroupSelectedClosed:"
               "^Sandbox/gui/images/Camera:"
               "^Sandbox/gui/images/iconVisible:"
               "^Sandbox/gui/images/iconLocked:";
   }

   // Figure the size of the buffer we need...
   const char* temp = dStrchr( icons, '\t' );
   U32 textLen = temp ? (U32)( temp - icons ) : dStrlen( icons );

   // Allocate temporary space.
   FrameAllocatorMarker txtBuff;
   char* drawText = (char*)txtBuff.alloc(sizeof(char) * (textLen + 4));
   dStrncpy( drawText, icons, textLen );
   drawText[textLen] = '\0';

   U32 numIcons = 0;
   char *buf = (char*)txtBuff.alloc(sizeof(char) * 256);
   char* token = dStrtok( drawText, ":" );

   // Count the number of icons and store them.
   while (token && numIcons < MaxIcons)
   {
      dSprintf( buf, sizeof( char ) * 256, "%s", token );
      mIconTable[numIcons] = TextureHandle( buf, TextureHandle::BitmapKeepTexture );
      token = dStrtok( NULL, ":" );
      numIcons++;
   }

   return true;
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::onPreRender()
{
   Parent::onPreRender();

   S32 nRootItemId = getFirstRootItem();
   if( nRootItemId == 0 )
      return;

   Item *pRootItem = getItem( nRootItemId );
   if( pRootItem == NULL )
      return;

   mTicksPassed++;

   if( mTicksPassed > mTreeRefreshInterval ) 
   {
      // Update every render in case new objects are added
      buildVisibleTree();

      mTicksPassed = 0;
   }

}

//------------------------------------------------------------------------------

bool GuiTreeViewCtrl::hitTest(const Point2I & pnt, Item* & item, BitSet32 & flags)
{

   // Initialize some things.
   const Point2I pos = globalToLocalCoord(pnt);
   flags.clear();
   item = 0;

   // get the hit cell
   Point2I cell((pos.x < 0 ? -1 : pos.x / mCellSize.x),
                (pos.y < 0 ? -1 : pos.y / mCellSize.y));

   // valid?
   if((cell.x < 0 || cell.x >= mSize.x) ||
      (cell.y < 0 || cell.y >= mSize.y))
      return false;

   flags.set(OnRow);

   // Grab the cell.
   if (cell.y >= mVisibleItems.size())
      return false; //Invalid cell, so don't do anything

   item = mVisibleItems[cell.y];

   S32 min = mTabSize * item->mTabLevel;

   // left of icon/text?
   if(pos.x < min)
   {
      flags.set(OnIndent);
      return true;
   }

   // check image
   S32 image = BmpChild;

   if(item->isInspectorData())
      image = item->isExpanded() ? BmpExp : BmpCon;
   else
      image = item->isExpanded() ? item->getExpandedImage() : item->getNormalImage();

   if((image >= 0) && (image < mProfile->mBitmapArrayRects.size()))
      min += mProfile->mBitmapArrayRects[image].extent.x;

   // Is it on the image?
   if(pos.x < min)
   {
      flags.set(OnImage);
      return(true);
   }

   // Bump over to the start of the text.
   min += mTextOffset;

   // Check against the text.
   FrameAllocatorMarker txtAlloc;
   U32 bufLen = item->getDisplayTextLength();
   char *buf = (char*)txtAlloc.alloc(bufLen);
   item->getDisplayText(bufLen, buf);

   min += mProfile->getFont(mFontSizeAdjust)->getStrWidth(buf);
   if(pos.x < min)
      flags.set(OnText);

   return true;
}

void GuiTreeViewCtrl::setInstantGroup(SimObject * obj)
{
   // make sure we're talking about a group.
   SimGroup * grp = dynamic_cast<SimGroup*>(obj);

   // Set the instant group variable.
   if(grp)
   {
      Con::setVariable("instantGroup", grp->getIdString());

    // Notify Script 
      Con::executef(this,2,"onInstantGroupSelected",Con::getIntArg(grp->getId()));
   }
}

void GuiTreeViewCtrl::syncSelection()
{
   // for each visible item check to see if it is on the mSelected list.
   // if it is then make sure that it is on the mSelectedItems list as well.
   for (S32 i = 0; i < mVisibleItems.size(); i++) 
   {
      for (S32 j = 0; j < mSelected.size(); j++) 
      {
         if (mVisibleItems[i]->mId == mSelected[j]) 
         {
            // check to see if it is on the visible items list.
            bool addToSelectedItems = true;
            for (S32 k = 0; k < mSelectedItems.size(); k++) 
            {
               if (mSelected[j] == mSelectedItems[k]->mId) 
               {
                  // don't add it
                  addToSelectedItems = false;
               }
            }
            if (addToSelectedItems) 
            {
               mVisibleItems[i]->mState.set(Item::Selected, true);
               mSelectedItems.push_front(mVisibleItems[i]);
               break;
            }
         } 
         else if (mVisibleItems[i]->isInspectorData()) 
         {
            if (mVisibleItems[i]->getObject() && mVisibleItems[i]->getObject()->getId() == mSelected[j]) 
            {
               // check to see if it is on the visible items list.
               bool addToSelectedItems = true;
               for (S32 k = 0; k < mSelectedItems.size(); k++) 
               {
                  if (mSelectedItems[k]->isInspectorData()) 
                  {
                     if (mSelected[j] == mSelectedItems[k]->getObject()->getId()) 
                     {
                        // don't add it
                        addToSelectedItems = false;
                     }
                  } 
                  else 
                  {
                     if (mSelected[j] == mSelectedItems[k]->mId) 
                     {
                        // don't add it
                        addToSelectedItems = false;
                     }
                  }
               }
               if (addToSelectedItems) 
               {
                  mVisibleItems[i]->mState.set(Item::Selected, true);
                  mSelectedItems.push_front(mVisibleItems[i]);
                  break;
               }
            }
         }

      }

   }
}

void GuiTreeViewCtrl::removeSelection(S32 itemId)
{
   if (mDebug)
      Con::printf("removeSelection called");
   Item * item = getItem(itemId);

   // the item may have been selected at one point but was never created/visible in the tree
   // so remove it.
   for (S32 j = 0; j <mSelected.size(); j++) 
   {
      if (item) 
      {
         if (item->isInspectorData()) 
         {
            if (item->getObject() && item->getObject()->getId() == mSelected[j]) 
            {
               mSelected.erase(j);
               break;
            }
         }
      }

      if (mSelected[j] == itemId) 
      {
         mSelected.erase(j);
         break;
      }
   }

   if(!item)
   {
      // maybe what we were passed wasn't an item id but an object id.
      for (S32 i = 0; i <mItems.size(); i++) 
      {
         if (mItems[i] != NULL) 
         {
            if (mItems[i]->isInspectorData()) 
            {
               if (mItems[i]->getObject()->getId() == itemId) 
               {
                  item = mItems[i];
                  break;
               }
            }
         }
      }

      if (!item) 
      {
         //Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::removeSelection: invalid item id! Perhaps it isn't visible yet");
         return;
      }
   }

   item->mState.set(Item::Selected, false);
   
   for (S32 i = 0; i < mSelectedItems.size(); i++) 
   {
      if (mSelectedItems[i] == item) 
      {
         mSelectedItems.erase(i);
         break;
      }
   }

   // Callback - onRemoveSelection( %itemID )
   if (item->getObject())
      Con::executef(this, 2, "onRemoveSelection", Con::getIntArg(item->getObject()->getId()));
}

void GuiTreeViewCtrl::addSelection(S32 itemId)
{
   if (mDebug)
      Con::printf("addSelection called");

   Item * item = getItem(itemId);

   S32 itr; // used to loop thru items
   bool foundMatch = false;

   if(!item)
   {
      // maybe what we were passed wasn't an item id but an object id.
      for (itr = 0; itr <mItems.size(); itr++)
      {
         if (mItems[itr] != NULL)
         {
            if (mItems[itr]->isInspectorData())
            {
               if ( mItems[itr]->getObject() && mItems[itr]->getObject()->getId() == itemId)
               {
                  item = mItems[itr];
                  //looks like it is. check to see if it is on the list
                  bool alreadySelected = false;
                  Vector<Item *>::iterator i;
                  for(i = mSelectedItems.begin(); i != mSelectedItems.end(); i++)
                  {
                     if (*(i) == item)
                     {
                        //already a selected item which means this call should be ignored
                        alreadySelected = true;
                        return;
                     }
                  }
                  break;
               }
            }
         }
      }

      if (!item)
      {
         // Do we want to allow more than one selected item?
         if( !mMultipleSelections )
            clearSelection();

         // rdbnote: this isn't going to be very fast, but we need to make sure we're not adding things to this list twice!!
         // in a perfect world, code would be cleanly written enough to not have this problem..
         foundMatch = false;
         for (itr = 0; itr < mSelected.size(); itr++)
         {
            if (mSelected[itr] == itemId)
               foundMatch = true;
         }

         //Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::addSelection: invalid item id! Perhaps it isn't visible yet.");
         if (!foundMatch)
            mSelected.push_front(itemId);

         return;
      }
   }
   else
   {
      // Do we want to allow more than one selected item?
      if( !mMultipleSelections )
         clearSelection();

      // rdbnote: this isn't going to be very fast, but we need to make sure we're not adding things to this list twice!!
      // in a perfect world, code would be cleanly written enough to not have this problem..
      foundMatch = false;
      for (itr = 0; itr < mSelected.size(); itr++)
      {
         if (mSelected[itr] == itemId)
            foundMatch = true;
      }

      // regardless of whether we found an item, we keep track of the Id that was passed
      // as the item may simply not have been created/visible yet.
      if (!foundMatch)
         mSelected.push_front(itemId);
   }

   item->mState.set(Item::Selected, true);

   // Also make it so we can see it if we didn't already.
   scrollVisible(item);

   // Do we want to allow more than one selected item?
   //if( !mMultipleSelections )
   //   clearSelection();

   // rdbnote: this isn't going to be very fast, but we need to make sure we're not adding things to this list twice!!
   // in a perfect world, code would be cleanly written enough to not have this problem..
   foundMatch = false;
   for (itr = 0; itr < mSelectedItems.size(); itr++)
   {
      if (mSelectedItems[itr]->mId == item->mId)
         foundMatch = true;
   }

   if (!foundMatch)
      mSelectedItems.push_front(item);

   // Callback - onAddSelection( %itemID )
   if (item->getObject())
      Con::executef(this, 2, "onAddSelection", Con::getIntArg(item->getObject()->getId()));
}


void GuiTreeViewCtrl::onItemSelected( Item *item )
{
   char buf[16];
   dSprintf(buf, 16, "%d", item->mId);
   if (item->isInspectorData())
   {
       if(item->getObject())
      Con::executef(this, 2, "onSelect", Con::getIntArg(item->getObject()->getId()));
      if (!(item->isParent()) && item->getObject())
         Con::executef(this, 2, "onInspect", Con::getIntArg(item->getObject()->getId()));
   }
   else
   {
      Con::executef(this, 2, "onSelect", buf);
      if (!(item->isParent()))
         Con::executef(this, 2, "onInspect", buf);
   }
   mSelectedItem = item->getID();
}

bool GuiTreeViewCtrl::setItemSelected(S32 itemId, bool select)
{
   Item * item = getItem(itemId);

   if (select)
   {
      if (mDebug) Con::printf("setItemSelected called true");

      // rdbnote: this isn't going to be very fast, but we need to make sure we're not adding things to this list twice!!
      // in a perfect world, code would be cleanly written enough to not have this problem..
      bool foundMatch = false;
      for (S32 itr = 0; itr < mSelected.size(); itr++)
      {
         if (mSelected[itr] == itemId)
            foundMatch = true;
      }

      if (!foundMatch)
         mSelected.push_front(itemId);
   }
   else
   {
      if (mDebug) Con::printf("setItemSelected called false");

      // remove it from the mSelected list
      for (S32 j = 0; j <mSelected.size(); j++)
      {
         if (item)
         {
            if (item->isInspectorData())
            {
               if (item->getObject())
               {
                  if(item->getObject()->getId() == mSelected[j])
                  {
                     mSelected.erase(j);
                     break;
                  }
               }
               else
               {
                  // Zombie, kill it!
                  mSelected.erase(j);
                  j--;
               }
            }
         }

         if (mSelected[j] == itemId)
         {
            mSelected.erase(j);
            break;
         }
      }
   }

   if(!item)
   {
      // maybe what we were passed wasn't an item id but an object id.
      for (S32 i = 0; i <mItems.size(); i++)
      {
         if (mItems[i] != NULL)
         {
            if (mItems[i]->isInspectorData())
            {
               if (mItems[i]->getObject())
               {
                  if(mItems[i]->getObject()->getId() == itemId)
                  {
                     item = mItems[i];
                     break;
                  }
               }
               else
               {
                  // It's a zombie, blast it.
                  mItems.erase(i);
                  i--;
               }
            }
         }
      }

      if (!item)
      {
         //Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::setItemSelected: invalid item id! Perhaps it isn't visible yet.");
         return(false);
      }
   }

   if(select)
   {
      addSelection( item->mId );
      onItemSelected( item );
   }
   else
   {
      // unselect the item, if it's present.
      item->mState.set(Item::Selected, false);

      if (item->isInspectorData() && item->getObject())
         Con::executef(this, 2, "onUnSelect", Con::getIntArg(item->getObject()->getId()));
      else
         Con::executef(this, 2, "onUnSelect", Con::getIntArg(item->mId));

      // remove it from the selected items list
      for (S32 i = 0; i < mSelectedItems.size(); i++)
      {
         if (mSelectedItems[i] == item)
         {
            mSelectedItems.erase(i);
            break;
         }
      }
   }

   setUpdate();
   return(true);
}

// Given an item's index in the selection list, return its itemId
S32 GuiTreeViewCtrl::getSelectedItem(S32 index)
{
   if(index >= 0 && index < getSelectedItemsCount())
   {
      return mSelectedItems[index]->mId;
   }

   return -1;
}
bool GuiTreeViewCtrl::setItemExpanded(S32 itemId, bool expand)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::setItemExpanded: invalid item id!");
      return(false);
   }

   if(item->isExpanded() == expand)
      return(true);

   // expand parents
   if(expand)
   {
      while(item)
      {
         if(item->mState.test(Item::VirtualParent))
            onVirtualParentExpand(item);

         item->setExpanded(true);
         item = item->mParent;
      }
   }
   else
   {
      if(item->mState.test(Item::VirtualParent))
         onVirtualParentCollapse(item);

      item->setExpanded(false);
   }
   return(true);
}


bool GuiTreeViewCtrl::setItemValue(S32 itemId, StringTableEntry Value)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::setItemValue: invalid item id!");
      return(false);
   }

   item->setValue( ( Value ) ? Value : "" );

   return(true);
}

const char * GuiTreeViewCtrl::getItemText(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getItemText: invalid item id!");
      return("");
   }

   return(item->getText() ? item->getText() : "");
}

const char * GuiTreeViewCtrl::getItemValue(S32 itemId)
{
   Item * item = getItem(itemId);
   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getItemValue: invalid item id!");
      return("");
   }

   if(item->mState.test(Item::InspectorData))
   {
      // If it's InspectorData, we let people use this call to get an object reference.
      return item->mInspectorInfo.mObject->getIdString();
   }
   else
   {
      // Just return the script value...
      return(item->getValue() ? item->getValue() : "");
   }
}

bool GuiTreeViewCtrl::editItem( S32 itemId, const char* newText, const char* newValue )
{
   Item* item = getItem( itemId );
   if ( !item )
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::editItem: invalid item id!");
      return false;
   }

   if ( item->mState.test(Item::InspectorData) )
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::editItem: item %d is inspector data and may not be modified!", itemId);
      return false;
   }

   delete [] item->getText();
   item->setText (new char[dStrlen( newText ) + 1]);
   dStrcpy( item->getText(), newText );

   delete [] item->getValue();
   item->setValue( new char[dStrlen( newValue ) + 1] );
   dStrcpy( item->getValue(), newValue );

   // Update the widths and such:
   buildVisibleTree();
   return true;
}

void GuiTreeViewCtrl::deleteSelection()
{
   Con::executef(this, 1, "onDeleteSelection");

   if (mSelectedItems.empty())
   {
      for (S32 i = 0; i < mSelected.size(); i++)
      {
         S32 objectId = mSelected[i];

         // find the object
         SimObject* obj = Sim::findObject(objectId);
         obj->deleteObject();
      }
   }
   else
   {
      Vector<Item*> delSelection;
      delSelection = mSelectedItems;
      mSelectedItems.clear();
      while (!delSelection.empty())
      {
         Item * item = delSelection.front();
         setItemSelected(item->mId,false);
         if ( item->mParent )
            deleteItem( item );
         
         delSelection.pop_front();      
      }
   }

   mSelected.clear();
   mSelectedItems.clear();
   Con::executef( this, 1, "onObjectDeleteCompleted");   
}

//------------------------------------------------------------------------------
// keyboard movement of items is restricted to just one item at a time
// if more than one item is selected then movement operations are not performed
bool GuiTreeViewCtrl::onKeyDown( const GuiEvent& event )
{
   if ( !mVisible || !mActive || !mAwake )
      return true;

   // All the keyboard functionality requires a selected item, so if none exists...

   // Deal with enter and delete
   if ( event.modifier == 0 )
   {
      if ( event.keyCode == KEY_RETURN )
      {
         if ( mAltConsoleCommand[0] )
            Con::evaluate( mAltConsoleCommand );
         return true;
      }

      if ( event.keyCode == KEY_DELETE && mDeleteObjectAllowed )
      {
         // Don't delete the root!
         if (mSelectedItems.empty())
            return true;

         //this may be fighting with the world editor delete
         deleteSelection();
         return true;
      }
 
      //call a generic bit of script that will let the subclass know that a key was pressed
      Con::executef(this, 3, "onKeyDown", Con::getIntArg(event.modifier), Con::getIntArg(event.keyCode));
   }

   // only do operations if only one item is selected
   if ( mSelectedItems.empty() || (mSelectedItems.size() > 1))
      return true;

   Item* item = mSelectedItems.first();

   if ( !item )
      return true;

   // The Alt key lets you move items around!
   if ( mFlags.test(IsEditable) && event.modifier & SI_ALT )
   {
      switch ( event.keyCode )
      {
      case KEY_UP:
         // Move us up.
         if ( item->mPrevious )
         {
            moveItemUp( item->mId );
            scrollVisible(item);
         }
         return true;

      case KEY_DOWN:
         // Move the item under us up.
         if ( item->mNext )
         {
            moveItemUp( item->mNext->mId );
            scrollVisible(item);
         }
         return true;

      case KEY_LEFT:
         if ( item->mParent )
         {
            if ( item->mParent->mParent )
            {
               // Ok, we have both an immediate parent, and a grandparent.

               // The goal of left-arrow alt is to become the child of our
               // grandparent, ie, to become a sibling of our parent.

               // First, unlink item from its siblings.
               if ( item->mPrevious )
                  item->mPrevious->mNext = item->mNext;
               else
                  item->mParent->mChild = item->mNext;

               if ( item->mNext )
                  item->mNext->mPrevious = item->mPrevious;

               // Now, relink as the next sibling of our parent.
               item->mPrevious = item->mParent;
               item->mNext = item->mParent->mNext;

               // If there was already a next sibling, deal with that case.
               if ( item->mNext )
                  item->mNext->mPrevious = item;
               item->mParent->mNext = item;

               // Snag the current parent set if any...
               SimSet *parentSet = NULL;

               if(item->mParent->isInspectorData())
                  parentSet = dynamic_cast<SimSet*>(item->mParent->getObject());
               else
               {
                  // parent is probably script data so we search up the tree for a
                  // set to put our object in
                  Item * temp = item->mParent;
                  while (!temp->isInspectorData())
                     temp = temp->mParent;
                  // found a ancestor who is an inspectorData
                  if (temp->isInspectorData())
                     parentSet = dynamic_cast<SimSet*>(temp->getObject());
                  else parentSet = NULL;
               }

               // Get our active SimObject if any
               SimObject *simObj = NULL;
               if(item->isInspectorData())
                  simObj = item->getObject();

               // Remove from the old parentset...
               if(simObj && parentSet) {
                  if (parentSet->size()>0)
                  {
                     SimObject *lastObject = parentSet->last();
                     parentSet->removeObject(simObj);
                     parentSet->reOrder(lastObject);
                  } else
                     parentSet->removeObject(simObj);
               }

               // And finally, update our item
               item->mParent = item->mParent->mParent;

               // Snag the newparent set if any...
               SimSet *newParentSet = NULL;

               if(item->mParent->isInspectorData())
                  newParentSet = dynamic_cast<SimSet*>(item->mParent->getObject());
               else
               {
                  // parent is probably script data so we search up the tree for a
                  // set to put our object in
                  Item * temp = item->mParent;
                  while (!temp->isInspectorData())
                     temp = temp->mParent;
                  // found a ancestor who is an inspectorData
                  if (temp->isInspectorData())
                     newParentSet = dynamic_cast<SimSet*>(temp->getObject());
                  else newParentSet = NULL;
               }
               if(simObj && newParentSet)
               {

                  newParentSet->addObject(simObj);
                  Item * temp = item->mNext;
                  // item->mNext may be script, so find an inspector item to reorder with if any

                  if (temp) {
                     do {
                        if (temp->isInspectorData())
                           break;
                        temp = temp->mNext;
                     } while (temp);
                     if (temp && item->getObject() && temp->getObject()) //do we still have a item->mNext? If not then don't bother reordering
                        newParentSet->reOrder(item->getObject(), temp->getObject());
                  }

               } else if (!simObj&&newParentSet) {
                  // our current item is script data. but it may have children who
                  // is inspector data who need an updated set
                  if (item->mChild)
                     inspectorSearch(item->mChild, item, parentSet, newParentSet);

               }

               // And update everything hurrah.
               buildVisibleTree();
               scrollVisible(item);
            }
         }
         return true;

      case KEY_RIGHT:
         if ( item->mPrevious )
         {
            // Make the item the last child of its previous sibling.

            // First, unlink from the current position in the list
            item->mPrevious->mNext = item->mNext;

            if ( item->mNext )
               item->mNext->mPrevious = item->mPrevious;

            // Get the object we're poking with.
            SimObject *simObj = NULL;
            SimSet *parentSet = NULL;
            if(item->isInspectorData())
               simObj = item->getObject();
            if(item->mParent->isInspectorData())
               parentSet = dynamic_cast<SimSet*>(item->mParent->getObject());
            else {
               // parent is probably script data so we search up the tree for a
               // set to put our object in
               Item * temp = item->mParent;
               while (!temp->isInspectorData())
                  temp = temp->mParent;
               // found an ancestor who is an inspectorData
               if (temp->isInspectorData())
                  parentSet = dynamic_cast<SimSet*>(temp->getObject());
            }

            // If appropriate, remove from the current SimSet.
            if(parentSet && simObj) {
               if (parentSet->size()>0)
               {
                  SimObject *lastObject = parentSet->last();
                  parentSet->removeObject(simObj);
                  parentSet->reOrder(lastObject);
               } else
                  parentSet->removeObject(simObj);
            }


            // Now, make our previous sibling our parent...
            item->mParent = item->mPrevious;
            item->mNext = NULL;

            // And sink us down to the end of its siblings, if appropriate.
            if ( item->mParent->mChild )
            {
               Item* temp = item->mParent->mChild;
               while ( temp->mNext )
                  temp = temp->mNext;

               temp->mNext = item;
               item->mPrevious = temp;
            }
            else
            {
               // only child...<sniff>
               item->mParent->mChild = item;
               item->mPrevious = NULL;
            }

            // Make sure the new parent is expanded:
            if ( !item->mParent->mState.test( Item::Expanded ) )
               setItemExpanded( item->mParent->mId, true );

            // Snag the new parent simset if any.
            SimSet *newParentSet = NULL;

            // new parent might be script. so figure out what set we need to add it to.
            if(item->mParent->isInspectorData())
               newParentSet = dynamic_cast<SimSet*>(item->mParent->getObject());
            else
            {
               // parent is probably script data so we search up the tree for a
               // set to put our object in
               if (mDebug) Con::printf("oh nos my parent is script!");
               Item * temp = item->mParent;
               while (!temp->isInspectorData())
                  temp = temp->mParent;
               // found a ancestor who is an inspectorData
               if (temp->isInspectorData())
                  newParentSet = dynamic_cast<SimSet*>(temp->getObject());
               else newParentSet = NULL;
            }
            // Add the item's SimObject to the new parent simset, at the end.
            if(newParentSet && simObj)
               newParentSet->addObject(simObj);
            else if (!simObj&&newParentSet&&parentSet) {
               // our current item is script data. but it may have children who
               // is inspector data who need an updated set

               if (item->mChild) {
                  inspectorSearch(item->mChild, item, parentSet, newParentSet);
               }

            }
            scrollVisible(item);
         }
         return true;
      }
   }

   // Explorer-esque navigation...
   switch( event.keyCode )
   {
   case KEY_UP:
      // Select previous visible item:
      if ( item->mPrevious )
      {
         item = item->mPrevious;
         while ( item->isParent() && item->isExpanded() )
         {
            item = item->mChild;
            while ( item->mNext )
               item = item->mNext;
         }
         setItemSelected(mSelectedItems.first()->mId,false);
         setItemSelected( item->mId, true );
         scrollVisible(item);
         return true;
      }

      // or select parent:
      if ( item->mParent )
      {
         setItemSelected(mSelectedItems.first()->mId,false);
         setItemSelected( item->mParent->mId, true );
         scrollVisible(item->mParent);
         return true;
      }

      return false;
      break;

   case KEY_DOWN:
      // Selected child if it is visible:
      if ( item->isParent() && item->isExpanded() )
      {
         setItemSelected(mSelectedItems.first()->mId,false);
         setItemSelected( item->mChild->mId, true );
         scrollVisible(item->mChild);
         return true;
      }
      // or select next sibling (recursively):
      do
      {
         if ( item->mNext )
         {
            setItemSelected(mSelectedItems.first()->mId,false);
            setItemSelected(item->mNext->mId, true );
            scrollVisible(item->mNext);
            return true;
         }

         item = item->mParent;
      } while ( item );

      return false;
      break;

   case KEY_LEFT:
      // Contract current menu:
      if ( item->isExpanded() )
      {
         setItemExpanded( item->mId, false );
         scrollVisible(item);
         return true;
      }
      // or select parent:
      if ( item->mParent )
      {
         setItemSelected(mSelectedItems.first()->mId,false);
         setItemSelected( item->mParent->mId, true );
         scrollVisible(item->mParent);
         return true;
      }

      return false;
      break;

   case KEY_RIGHT:
      // Expand selected item:
      if ( item->isParent() )
      {
         if ( !item->isExpanded() )
         {
            setItemExpanded( item->mId, true );
            scrollVisible(item);
            return true;
         }

         // or select child:
         setItemSelected(mSelectedItems.first()->mId,false);
         setItemSelected( item->mChild->mId, true );
         scrollVisible(item->mChild);
         return true;
      }

      return false;
      break;
   }

   // Not processed, so pass the event on:
   return Parent::onKeyDown( event );
}



//------------------------------------------------------------------------------
// on mouse up look at the current item and check to see if it is valid
// to move the selected item(s) to it.
void GuiTreeViewCtrl::onTouchUp(const GuiEvent &event)
{
   if( !mActive || !mAwake || !mVisible )
      return;

   mouseUnlock();

   if ( mSelectedItems.empty())
   {
      mDragMidPoint = NomDragMidPoint;
      return;
   }
   
   if (!mMouseDragged) 
      return;
   else
      mMouseDragged = false;

   Item* newItem = NULL;
   Item* newItem2 = NULL;

   if (mFlags.test(IsEditable))
   {
      Parent::onTouchMove( event );
      if (mOldDragY != mMouseOverCell.y)
      {

         mOldDragY = mMouseOverCell.y;
         BitSet32 hitFlags = 0;
         if ( !hitTest( event.mousePoint, newItem2, hitFlags ) )
         {
            mDragMidPoint = NomDragMidPoint;
            return;
         }

         newItem2->mState.clear(Item::MouseOverBmp | Item::MouseOverText );
         
         // if the newItem isn't in the mSelectedItemList then continue.

         Vector<Item *>::iterator k;
         for(k = mSelectedItems.begin(); k != mSelectedItems.end(); k++) 
         {
            newItem = newItem2;
            
            if (*(k) == newItem) 
            {
               mDragMidPoint = NomDragMidPoint;
               return;
            }

            Item * temp = *(k);
            Item * grandpaTemp = newItem->mParent;
            
            // grandpa check, kick out if an item would be its own ancestor
            while (grandpaTemp)
            {
               if (temp == grandpaTemp)
               {
                  if (mDebug)
                  {
                     Con::printf("grandpa check");

                     if (temp->isInspectorData())
                        Con::printf("temp's name: %s",temp->getObject()->getName());

                     if (grandpaTemp->isInspectorData())
                        Con::printf("grandpa's name: %s",grandpaTemp->getObject()->getName());
                  }

                  mDragMidPoint = NomDragMidPoint;
                  return;
               }

               grandpaTemp = grandpaTemp->mParent;
            }
         }


         for (S32 i = 0; i <mSelectedItems.size();i++) 
         {
            newItem = newItem2;
            Item * item = mSelectedItems[i];

            if (mDebug) Con::printf("----------------------------");
         
            // clear old highlighting of the item
            item->mState.clear(Item::MouseOverBmp | Item::MouseOverText );

            // move the selected item to the newItem
            Item* oldParent = item->mParent;
            // Snag the current parent set if any for future reference
            SimSet *parentSet = NULL;

            if(oldParent->isInspectorData())
               parentSet = dynamic_cast<SimSet*>(oldParent->getObject());
            else 
            {
               // parent is probably script data so we search up the tree for a
               // set to put our object in
               Item * temp = oldParent;
               while (temp) 
               {
                  if (temp->isInspectorData())
                     break;
                  temp = temp->mParent;
               }
               // found an ancestor who is an inspectorData
               if (temp) 
               {
                  if (temp->isInspectorData())
                     parentSet = dynamic_cast<SimSet*>(temp->getObject());
               }
            }

            // unlink from the current position in the list
            unlinkItem(item);

            // update the parent's children

            // check if we an only child
            if (item->mParent->mChild == item)
            {
               if (item->mNext)
                  item->mParent->mChild = item->mNext;
               else
                  item->mParent->mChild = NULL;
            }

            if (mDragMidPoint != NomDragMidPoint)
            {

               //if it is below an expanded tree, place as last item in the tree
               //if it is below a parent who isn't expanded put below it

               // position the item above or below another item
               if (mDragMidPoint == AbovemDragMidPoint)
               {
                  // easier to treat everything as "Below the mDragMidPoint" so make some adjustments
                  if (mDebug) Con::printf("adding item above mDragMidPoint");

                  // above the mid point of an item, so grab either the parent
                  // or the previous sibling

                  // does the item have a previous sibling?
                  if (newItem->mPrevious)
                  {
                     newItem = newItem->mPrevious;

                     if (mDebug) Con::printf("treating as if below an item that isn't expanded");

                     // otherwise add below that item as a sibling
                     item->mParent = newItem->mParent;
                     item->mPrevious = newItem;
                     item->mNext = newItem->mNext;
                     if (newItem->mNext)
                        newItem->mNext->mPrevious = item;
                     newItem->mNext = item;
                  } 
                  else
                  {
                     if (mDebug) Con::printf("treating as if adding below the parent of the item");

                     // instead we add as the first item below the newItem's parent
                     item->mParent = newItem->mParent;
                     item->mNext = newItem;
                     item->mPrevious = NULL;
                     newItem->mPrevious = item;
                     item->mParent->mChild = item;

                  }
               }
               else if (mDragMidPoint == BelowmDragMidPoint)
               {
                  if ((newItem->isParent())&&(newItem->isExpanded()))
                  {
                     if (mDebug) Con::printf("adding item to an expanded parent below the mDragMidPoint");

                     item->mParent = newItem;

                     // then add the new item as a child
                     item->mNext = newItem->mChild;
                     if (newItem->mChild)
                        newItem->mChild->mPrevious = item;
                     item->mParent->mChild = item;
                     item->mPrevious = NULL;
                  }
                  else if ((!newItem->mNext)&&(newItem->mParent)&&(newItem->mParent->mParent)) 
                  {
                     // add below it's parent.
                     if (mDebug) Con::printf("adding below a tree");

                     item->mParent = newItem->mParent->mParent;
                     item->mNext = newItem->mParent->mNext;
                     item->mPrevious = newItem->mParent;

                     if (newItem->mParent->mNext)
                        newItem->mParent->mNext->mPrevious = item;

                     newItem->mParent->mNext = item;
                  }
                  else 
                  {
                     // adding below item not as a child
                     if (mDebug) Con::printf("adding item below the mDragMidPoint of an item");

                     item->mParent = newItem->mParent;
                     // otherwise the item is a sibling
                     if (newItem->mNext)
                        newItem->mNext->mPrevious = item;
                     item->mNext = newItem->mNext;
                     item->mPrevious = newItem;
                     newItem->mNext = item;
                  }
               }
            }
            // if we're not allowed to add to items, then try to add to the parent of the hit item.
            // if we are, just add to the item we hit.
            else 
            {
               if (mDebug) 
               {
                  if (item->isInspectorData() && item->getObject())
                     Con::printf("Item: %i",item->getObject()->getId());
                  if (newItem->isInspectorData() && newItem->getObject())
                     Con::printf("Parent: %i",newItem->getObject()->getId());
                  Con::printf("dragged onto an item");
               }
               
               // if the hit item is not already a group, and we're not allowed to drag to items, 
               // then try to add to the parent.
               if(!mDragToItemAllowed && !newItem->isParent())
               {
                   // add to the item's parent.
                  if(!newItem->mParent || !newItem->mParent->isParent())
                  {
                     if(mDebug)
                        Con::printf("could not find the parent of that item. dragging to an item is not allowed, kicking out.");
                     mDragMidPoint = NomDragMidPoint;
                     return;
                  }
                  newItem = newItem->mParent;
               }
               
               // new parent is the item in the current cell
               item->mParent = newItem;
               
               // adjust children if any
               if (newItem->mChild)
               {
                  if (mDebug) Con::printf("not the first child");
                  
                  // put it at the top of the list (easier to find if there are many children)
                  if (newItem->mChild)
                     newItem->mChild->mPrevious = item;
                  item->mNext = newItem->mChild;
                  newItem->mChild = item;
                  item->mPrevious = NULL;
               }
               else 
               {
                  if (mDebug) Con::printf("first child");
                  
                  // only child
                  newItem->mChild = item;
                  item->mNext = NULL;
                  item->mPrevious = NULL;
               }
            }

            // expand the item we added to, if it isn't expanded already
            if ( !item->mParent->mState.test( Item::Expanded ) )
               setItemExpanded( item->mParent->mId, true );

            //----------------------------------------------------------------
            // handle objects

            // Get our active SimObject if any
            SimObject *simObj = NULL;
            if(item->isInspectorData()) 
            {
               simObj = item->getObject();
            }

            // Remove from the old parentset
            if((simObj && parentSet)&&(oldParent != item->mParent))
            {
               if (mDebug) Con::printf("removing item from old parentset");
            
               // hack to get around the way removeObject takes the last item of the set
               // and moves it into the place of the object we removed
               if (parentSet->size()>0)
               {
                  SimObject *lastObject = parentSet->last();
                  parentSet->removeObject(simObj);
                  parentSet->reOrder(lastObject);
               }
               else
               {
                  parentSet->removeObject(simObj);
               }
            }

            // Snag the newparent set if any...
            SimSet *newParentSet = NULL;

            if(item->mParent->isInspectorData()) 
            {
               if (mDebug) Con::printf("getting a new parent set");

               SimObject * tmpObj = item->mParent->getObject();
               newParentSet = dynamic_cast<SimSet*>(tmpObj);
            }
            else
            {
               // parent is probably script data so we search up the tree for a
               // set to put our object in
               if (mDebug) Con::printf("oh nos my parent is script!");

               Item * temp = item->mParent;
               while (temp) 
               {
                  if (temp->isInspectorData())
                     break;
                  temp = temp->mParent;
               }
               
               // found a ancestor who is an inspectorData
               if (temp) 
               {
                  if (temp->isInspectorData())
                     newParentSet = dynamic_cast<SimSet*>(temp->getObject());
               } 
               else 
               {
                  newParentSet = NULL;
               }
            }

            if(simObj && newParentSet)
            {
               if (mDebug) Con::printf("simobj and new ParentSet");

               if (oldParent != item->mParent)
                  newParentSet->addObject(simObj);

                             //order the objects in the simset according to their
                             //order in the tree view control
                             if(!item->mNext)
                             {
                                    if(!item->mPrevious) break;
                                    //bring to the end of the set
                                    SimObject *prevObject = item->mPrevious->getObject();
                                    if (prevObject && item->getObject()) 
                                    {
                                        newParentSet->reOrder(item->getObject(), prevObject);
                                    }
                                }
                                else
                                {
                                    //reorder within the set
                                    SimObject *nextObject = item->mNext->getObject();
                                    if(nextObject && item->getObject())
                                    {
                                        newParentSet->reOrder(item->getObject(), nextObject);
                                    }
                                }
            } 
            else if (!simObj&&newParentSet) 
            {
               // our current item is script data. but it may have children who
               // is inspector data who need an updated set
               if (mDebug) Con::printf("no simobj but new parentSet");
               if (item->mChild)
                  inspectorSearch(item->mChild, item, parentSet, newParentSet);

            }
            else if (simObj&&!newParentSet) 
            {
               if (mDebug) Con::printf("simobject and no new parent set");
            }
            else
               if (mDebug) Con::printf("no simobject and no new parent set");
         }

         // And update everything.
         scrollVisible(newItem);
      }
   }

   mDragMidPoint = NomDragMidPoint;
}

//------------------------------------------------------------------------------
void GuiTreeViewCtrl::onTouchDragged(const GuiEvent &event)
{
    if(!mSupportMouseDragging) return;
   if( !mActive || !mAwake || !mVisible )
      return;

   if (mSelectedItems.size() == 0)
      return;
   Point2I pt = globalToLocalCoord(event.mousePoint);
   Parent::onTouchMove(event);
   mouseLock();
   mMouseDragged = true;
   // whats our mDragMidPoint?
   mCurrentDragCell = mMouseOverCell.y;
   S32 midpCell = mCurrentDragCell * mItemHeight + (mItemHeight/2);
   S32 currentY = pt.y;
   S32 yDiff = currentY-midpCell;
   S32 variance = (mItemHeight/5);
   if (mPreviousDragCell >= 0)
      mVisibleItems[mPreviousDragCell]->mState.clear( Item::MouseOverBmp | Item::MouseOverText );

   if (mAbs(yDiff) > variance)
   {
      //above or below an item?
      if (yDiff < 0)
         mDragMidPoint = AbovemDragMidPoint;
      else
         mDragMidPoint = BelowmDragMidPoint;
   } else
   {
      mDragMidPoint = NomDragMidPoint;
      // highlight the current item
      // hittest to detect whether we are on an item
      // ganked from onMouseMouse

      // used for tracking what our last cell was so we can clear it.
      mPreviousDragCell = mCurrentDragCell;
      if (mCurrentDragCell >= 0)
      {

         Item* item = NULL;
         BitSet32 hitFlags = 0;
         if ( !hitTest( event.mousePoint, item, hitFlags ) )
            return;

         if ( hitFlags.test( OnImage ) )
            item->mState.set( Item::MouseOverBmp );

         if ( hitFlags.test( OnText ))
            item->mState.set( Item::MouseOverText );

         // Always redraw the entire mouse over item, since we are distinguishing
         // between the bitmap and the text:
         setUpdateRegion( Point2I( mMouseOverCell.x * mCellSize.x, mMouseOverCell.y * mCellSize.y ), mCellSize );
      }
   }
}

void GuiTreeViewCtrl::onMiddleMouseDown(const GuiEvent & event)
{
   //for debugging items
   if (mDebug) {
      Item* item;
      BitSet32 hitFlags = 0;
      hitTest( event.mousePoint, item, hitFlags );
      Con::printf("debugging %d", item->mId);
      Point2I pt = globalToLocalCoord(event.mousePoint);
      if (item->isInspectorData() && item->getObject()) {
         Con::printf("object data:");
         Con::printf("name:%s",item->getObject()->getName());
         Con::printf("className:%s",item->getObject()->getClassName());
      }
      Con::printf("contents of mSelectedItems:");
      for(S32 i = 0; i < mSelectedItems.size(); i++) {
         if (mSelectedItems[i]->isInspectorData()) {
            Con::printf("%d",mSelectedItems[i]->getObject()->getId());
         } else
            Con::printf("wtf %d", mSelectedItems[i]);
      }
      Con::printf("contents of mSelected");
      for (S32 j = 0; j < mSelected.size(); j++) {
         Con::printf("%d", mSelected[j]);
      }
      S32 mCurrentDragCell = mMouseOverCell.y;
      S32 midpCell = (mCurrentDragCell) * mItemHeight + (mItemHeight/2);
      S32 currentY = pt.y;
      S32 yDiff = currentY-midpCell;
      Con::printf("cell info: (%d,%d) mCurrentDragCell=%d est=(%d,%d,%d) ydiff=%d",pt.x,pt.y,mCurrentDragCell,mCurrentDragCell*mItemHeight, midpCell, (mCurrentDragCell+1)*mItemHeight,yDiff);
   }
}


void GuiTreeViewCtrl::onTouchDown(const GuiEvent & event)
{
   if( !mActive || !mAwake || !mVisible )
   {
      Parent::onTouchDown(event);
      return;
   }
   if ( mProfile->mCanKeyFocus )
      setFirstResponder();

   Item * item = 0;
   BitSet32 hitFlags;
   mOldDragY = 0;
   mDragMidPoint = NomDragMidPoint;

   //
   if(!hitTest(event.mousePoint, item, hitFlags))
      return;

   //
   if(event.modifier & SI_CTRL)
   {
      bool selectFlag = item->mState.test(Item::Selected);
      if (selectFlag == true)
      {
         // already selected, so unselect it and remove it
         removeSelection(item->mId);
         if (item->isInspectorData() && item->getObject())
            Con::executef(this,2,"onRemoveSelection",Con::getIntArg(item->getObject()->getId()));
      } else
      {
         // otherwise select it and add it to the list
         // check if it is already on the list.
         /*bool newSelection = true;
         for (S32 i = 0; i < mSelectedItems.size(); i++) {
         if (mSelectedItems[i] == item) {
         newSelection = false;
         }
         }*/
         //if (newSelection) {
         addSelection(item->mId);
         if (item->isInspectorData() && item->getObject())
            Con::executef(this,2,"onAddSelection",Con::getIntArg(item->getObject()->getId()));
         //}



      }
   }
   else if (event.modifier & SI_SHIFT)
   {
      // is something already selected?
      S32 firstSelectedIndex = 0;
      Item * firstItem = NULL;
      if (!mSelectedItems.empty())
      {
         firstItem = mSelectedItems.front();
         for (S32 i = 0; i < mVisibleItems.size();i++)
         {
            if (mVisibleItems[i] == mSelectedItems.front())
            {
               firstSelectedIndex = i;
               break;
            }
         }
         S32 mCurrentDragCell = mMouseOverCell.y;
         if (mVisibleItems[firstSelectedIndex] != firstItem )
         {
            /*
            Con::printf("something isn't right...");
            if (mVisibleItems[firstSelectedIndex]->isInspectorData())
            Con::printf("visibleItem %s",mVisibleItems[firstSelectedIndex]->getObject()->getName());
            if (firstItem->isInspectorData())
            Con::printf("firstItem %s",firstItem->getObject()->getName());
            */
         }
         else
         {
            // select the cells

            if ((mCurrentDragCell) < firstSelectedIndex)
            {
               //select up
               for (S32 j = (mCurrentDragCell); j < firstSelectedIndex; j++) {
                  //if the item isn't already selected, then select it
                  bool newSelection = true;
                  Vector<Item *>::iterator k;
                  for(k = mSelectedItems.begin(); k != mSelectedItems.end(); k++)
                  {
                     if (mVisibleItems[j] == *(k)){
                        newSelection = false;
                        break;
                     }
                  }
                  if (newSelection)
                  {
                     addSelection(mVisibleItems[j]->mId);
                     if (mVisibleItems[j]->isInspectorData())
                        Con::executef(this,2,"onAddSelection",Con::getIntArg(mVisibleItems[j]->getObject()->getId()));
                  }
               }
            }
            else
            {
               // select down
               for (S32 j = firstSelectedIndex+1; j < (mCurrentDragCell+1); j++) {
                  bool newSelection = true;
                  Vector<Item *>::iterator k;
                  for(k = mSelectedItems.begin(); k != mSelectedItems.end(); k++) {
                     if (mVisibleItems[j] == *(k)){
                        newSelection = false;
                        break;
                     }
                  }
                  if (newSelection)
                  {
                     addSelection(mVisibleItems[j]->mId);
                     if (mVisibleItems[j]->isInspectorData())
                        Con::executef(this,2,"onAddSelection",Con::getIntArg(mVisibleItems[j]->getObject()->getId()));
                  }
               }
            }
         }
      }
   }
   else if (event.modifier & SI_ALT)
   {
      if (item->isInspectorData() && item->getObject())
      {
         setInstantGroup(item->getObject());
         mInstantGroup = item->mId;
      }
   } else if (!hitFlags.test(OnImage))
   {
      // first check to see if the item is already selected
      bool newSelection = true;
      Vector<Item *>::iterator k;
      for(k = mSelectedItems.begin(); k != mSelectedItems.end(); k++) {
         if(*(k) == item)
         {
            newSelection = false;
            break;
         }
      }


      // if the item is not already selected then we have a
      //newly selected item, so clear our list of selected items
      if (newSelection)
      {
         clearSelection();
         //Con::executef(this, 1, "onClearSelection");
         //mSelectedItems.clear();
         setItemSelected(item->mId,true);
      }
   }
   if ( hitFlags.test( OnText ) && ( event.mouseClickCount > 1 ) && mAltConsoleCommand[0] )
      Con::evaluate( mAltConsoleCommand );

   if(!item->isParent())
      return;

   //
   if ( mFullRowSelect || hitFlags.test( OnImage ) )
   {
      item->setExpanded(!item->isExpanded());
      if( !item->isInspectorData() && item->mState.test(Item::VirtualParent) )
         onVirtualParentExpand(item);
      scrollVisible(item);
   }
}


//------------------------------------------------------------------------------
void GuiTreeViewCtrl::onTouchMove( const GuiEvent &event )
{
   if ( mMouseOverCell.y >= 0 && mVisibleItems.size() > mMouseOverCell.y)
      mVisibleItems[mMouseOverCell.y]->mState.clear( Item::MouseOverBmp | Item::MouseOverText );

   Parent::onTouchMove( event );

   if ( mMouseOverCell.y >= 0 )
   {
      Item* item = NULL;
      BitSet32 hitFlags = 0;
      if ( !hitTest( event.mousePoint, item, hitFlags ) )
         return;

      if ( hitFlags.test( OnImage ) )
         item->mState.set( Item::MouseOverBmp );

      if ( hitFlags.test( OnText ))
         item->mState.set( Item::MouseOverText );

      // Always redraw the entire mouse over item, since we are distinguishing
      // between the bitmap and the text:
      setUpdateRegion( Point2I( mMouseOverCell.x * mCellSize.x, mMouseOverCell.y * mCellSize.y ), mCellSize );
   }
}


//------------------------------------------------------------------------------
void GuiTreeViewCtrl::onTouchEnter( const GuiEvent &event )
{
   Parent::onTouchEnter( event );
   onTouchMove( event );
}


//------------------------------------------------------------------------------
void GuiTreeViewCtrl::onTouchLeave( const GuiEvent &event )
{
   if ( mMouseOverCell.y >= 0 && mVisibleItems.size() > mMouseOverCell.y)
      mVisibleItems[mMouseOverCell.y]->mState.clear( Item::MouseOverBmp | Item::MouseOverText );

   Parent::onTouchLeave( event );
}


//------------------------------------------------------------------------------
void GuiTreeViewCtrl::onRightMouseDown(const GuiEvent & event)
{
   if(!mActive)
   {
      Parent::onRightMouseDown(event);
      return;
   }

   Item * item = NULL;
   BitSet32 hitFlags;

   //
   if(!hitTest(event.mousePoint, item, hitFlags))
      return;

   //
   char bufs[2][32];
   dSprintf(bufs[0], 32, "%d", item->mId);
   dSprintf(bufs[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);

   if (item->isInspectorData() && item->getObject())
      Con::executef(this,4, "onRightMouseDown", bufs[0],bufs[1],Con::getIntArg(item->getObject()->getId()));
   else
      Con::executef(this, 3, "onRightMouseDown", bufs[0], bufs[1]);
}

//------------------------------------------------------------------------------

void GuiTreeViewCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // Get all our contents drawn!
   Parent::onRender(offset,updateRect);

   // Deal with drawing the drag & drop line, if any...
   dglSetClipRect(updateRect);

   // only do it if we have a mDragMidPoint
   if (mDragMidPoint == NomDragMidPoint || !mSupportMouseDragging )
      return;

   ColorF greyLine(0.5,0.5,0.5,1);
   Point2F squarePt;

   glLineWidth(2.f);
   // draw mDragMidPoint lines with a diamond
   if (mDragMidPoint == AbovemDragMidPoint)
   {
      S32 tempY = mItemHeight*mCurrentDragCell+offset.y ;
      squarePt.y = (F32)tempY;
      squarePt.x = (F32)(125.0f+offset.x);
      dglDrawLine(0+offset.x, tempY, 250+offset.x, tempY,greyLine);
      dglDraw2DSquare(squarePt, 6, 90 );

   }
   if (mDragMidPoint == BelowmDragMidPoint)
   {
      S32 tempY2 = mItemHeight*(mCurrentDragCell+1) +offset.y;
      squarePt.y = (F32)tempY2;
      squarePt.x = (F32)(125.0f+offset.x);
      dglDrawLine(0+offset.x, tempY2, 250+offset.x, tempY2,greyLine);
      dglDraw2DSquare(squarePt,6, 90 );

   }

   glLineWidth(1.f);
}

void GuiTreeViewCtrl::onRenderCell(Point2I offset, Point2I cell, bool, bool )
{
   if( !mVisibleItems.size() )
      return;

   // Do some sanity checking and data retrieval.
   AssertFatal(cell.y < mVisibleItems.size(), "GuiTreeViewCtrl::onRenderCell: invalid cell");
   Item * item = mVisibleItems[cell.y];

   // If there's no object, deal with it.
   if(item->isInspectorData())
      if(!item->getObject())
         return;

   RectI drawRect( offset, mCellSize );
   dglClearBitmapModulation();

   FrameAllocatorMarker txtBuff;

   // Ok, we have the item. There are a few possibilities at this point:
   //    - We need to draw inheritance lines and a treeview-chosen icon
   //       OR
   //    - We have to draw an item-dependent icon
   //    - If we're mouseover, we have to highlight it.
   //
   //    - We have to draw the text for the item
   //       - Taking into account various mouseover states
   //       - Taking into account the value (set or not)
   //       - If it's an inspector data, we have to do some custom rendering

   // Ok, first draw the tab and icon.

   // Do we draw the tree lines?
   if(mFlags.test(ShowTreeLines))
   {
      drawRect.point.x += ( mTabSize * item->mTabLevel );
      Item* parent = item->mParent;
      for ( S32 i = item->mTabLevel; ( parent && i > 0 ); i-- )
      {
         drawRect.point.x -= mTabSize;
         if ( parent->mNext )
            dglDrawBitmapSR( mProfile->mTextureHandle, drawRect.point, mProfile->mBitmapArrayRects[BmpLine] );

         parent = parent->mParent;
      }
   }

   // Now, the icon...
   drawRect.point.x = offset.x + mTabSize * item->mTabLevel;

   // First, draw the rollover glow, if it's an inner node.
   if ( item->isParent() && item->mState.test( Item::MouseOverBmp ) )
      dglDrawBitmapSR( mProfile->mTextureHandle, drawRect.point, mProfile->mBitmapArrayRects[BmpGlow] );

   // Now, do we draw a treeview-selected item or an item dependent one?
   S32 newOffset = 0; // This is stored so we can render glow, then update render pos.

   if(item->isInspectorData())
   {
      S32 bitmap = 0;

      // Ok, draw the treeview lines as appropriate.
      if ( !item->isParent() )
      {
         bitmap = item->mNext ? BmpChild : BmpLastChild;
      }
      else
      {
         bitmap = item->isExpanded() ? BmpExp : BmpCon;

         if ( item->mParent || item->mPrevious )
            bitmap += ( item->mNext ? 3 : 2 );
         else
            bitmap += ( item->mNext ? 1 : 0 );
      }

      if ( ( bitmap >= 0 ) && ( bitmap < mProfile->mBitmapArrayRects.size() ) )
      {
         dglDrawBitmapSR( mProfile->mTextureHandle, drawRect.point, mProfile->mBitmapArrayRects[bitmap] );
         newOffset = mProfile->mBitmapArrayRects[bitmap].extent.x;
      }

      // draw lock icon if need be
      S32 icon = Lock1;
      S32 icon2 = Hidden;

      if (item->getObject() && item->getObject()->isLocked())
      {
         if (mIconTable[icon])
         {
            //drawRect.point.x = offset.x + mTabSize * item->mTabLevel + mIconTable[icon].getWidth();
            drawRect.point.x += mIconTable[icon].getWidth();
            dglDrawBitmap( mIconTable[icon], drawRect.point, 0 );
         }
      }

      if (item->getObject() && item->getObject()->isHidden())
      {
         if (mIconTable[icon2])
         {
            //drawRect.point.x = offset.x + mTabSize * item->mTabLevel + mIconTable[icon].getWidth();
            drawRect.point.x += mIconTable[icon2].getWidth();
            dglDrawBitmap( mIconTable[icon2], drawRect.point, 0 );
         }
      }


      SimObject * pObject = item->getObject();
      SimGroup  * pGroup  = ( pObject == NULL ) ? NULL : dynamic_cast<SimGroup*>( pObject );

      // draw the icon associated with the item
      if (item->isParent())
      {
         if ( pGroup != NULL)
         {
            if (item->isExpanded())
               item->mIcon = SimGroup1;
            else
               item->mIcon = SimGroup2;
         }
         else
            item->mIcon = SimGroup2;
      }
      
      if (mInstantGroup == item->mId)
      {
         if (item->isInspectorData())
         {
            if ( pGroup != NULL )
            {
               if (item->isExpanded())
                  item->mIcon = SimGroup3;
               else
                  item->mIcon = SimGroup4;
            }
         }
      }


      if (item->mIcon)
      {
         if (mIconTable[item->mIcon])
         {
            S32 iconHeight = (mItemHeight - mIconTable[item->mIcon].getHeight()) / 2;
            S32 oldHeight = drawRect.point.y;
            if(iconHeight > 0)
               drawRect.point.y += iconHeight;
            drawRect.point.x += mIconTable[item->mIcon].getWidth();
            dglDrawBitmap( mIconTable[item->mIcon], drawRect.point, 0 );
            drawRect.point.y = oldHeight;
         }
      }
   }
   else
   {
      S32 bitmap = 0;

      // Ok, draw the treeview lines as appropriate.
      if ( !item->isParent() )
         bitmap = item->mNext ? BmpChild : BmpLastChild;
      else
      {
         bitmap = item->isExpanded() ? BmpExp : BmpCon;

         if ( item->mParent || item->mPrevious )
            bitmap += ( item->mNext ? 3 : 2 );
         else
            bitmap += ( item->mNext ? 1 : 0 );
      }

      if ( ( bitmap >= 0 ) && ( bitmap < mProfile->mBitmapArrayRects.size() ) )
      {
         dglDrawBitmapSR( mProfile->mTextureHandle, drawRect.point, mProfile->mBitmapArrayRects[bitmap] );
         newOffset = mProfile->mBitmapArrayRects[bitmap].extent.x;
      }

      S32 icon = item->isExpanded() ? item->mScriptInfo.mExpandedImage : item->mScriptInfo.mNormalImage;
      if ( icon )
      {
         if (mIconTable[icon])
         {
            S32 iconHeight = (mItemHeight - mIconTable[icon].getHeight()) / 2;
            S32 oldHeight = drawRect.point.y;
            if(iconHeight > 0)
               drawRect.point.y += iconHeight;
            drawRect.point.x += mIconTable[icon].getWidth();
            dglDrawBitmap( mIconTable[icon], drawRect.point, 0 );
            drawRect.point.y = oldHeight;
         }
      }
   }

   // Ok, update offset so we can render some text!
   drawRect.point.x += newOffset;

   // Ok, now we're off to rendering the actual data for the treeview item.

   U32 bufLen = item->mDataRenderWidth + 1;
   char *displayText = (char *)txtBuff.alloc(bufLen);
   displayText[bufLen-1] = 0;
   item->getDisplayText(bufLen, displayText);

   // Draw the rollover/selected bitmap, if one was specified.
   drawRect.extent.x = mProfile->getFont(mFontSizeAdjust)->getStrWidth( displayText ) + ( 2 * mTextOffset );
   if ( item->mState.test( Item::Selected ) && mTexSelected )
      dglDrawBitmapStretch( mTexSelected, drawRect );
   else if ( item->mState.test( Item::MouseOverText ) && mTexRollover )
      dglDrawBitmapStretch( mTexRollover, drawRect );

   // Offset a bit so as to space text properly.
   drawRect.point.x += mTextOffset;

   // Determine what color the font should be.
   ColorI fontColor;

   fontColor = item->mState.test( Item::Selected ) ? mProfile->mFontColorSL :
             ( item->mState.test( Item::MouseOverText ) ? mProfile->mFontColorHL : mProfile->mFontColor );

   if (item->mState.test(Item::Selected))
   {
      dglDrawRectFill(drawRect, mProfile->mFillColorHL);
   }
   else if (item->mState.test(Item::MouseOverText))
   {
      dglDrawRectFill(drawRect, mProfile->mFontColorNA);
   }
   
   if( mInstantGroup == item->mId)
   {
        fontColor	=	mProfile->mFontColorHL;
   }
   
   dglSetBitmapModulation( fontColor );

   // Center the text horizontally.
   S32 height = (mItemHeight - mProfile->getFont(mFontSizeAdjust)->getHeight()) / 2;

   if(height > 0)
      drawRect.point.y += height;

   // JDD - offset by two pixels or so to keep the text from rendering RIGHT ONTOP of the outline
   drawRect.point.x += 2;

   dglDrawText( mProfile->getFont(mFontSizeAdjust), drawRect.point, displayText, mProfile->mFontColors );

}

//------------------------------------------------------------------------------
void GuiTreeViewCtrl::clearSelection()
{
   while (!mSelectedItems.empty())
   {
      if(!setItemSelected(mSelectedItems.last()->mId, false))
         mSelectedItems.pop_back();
   }

   mSelectedItems.clear();
   mSelected.clear();
   
   Con::executef(this, 1, "onClearSelection");
}

void GuiTreeViewCtrl::lockSelection(bool lock)
{
   for(U32 i = 0; i < (U32)mSelectedItems.size(); i++)
   {
      if(mSelectedItems[i]->isInspectorData())
         mSelectedItems[i]->getObject()->setLocked(lock);
   }
}
void GuiTreeViewCtrl::hideSelection(bool hide)
{
   for (U32 i = 0; i < (U32)mSelectedItems.size(); i++)
   {
      if (mSelectedItems[i]->isInspectorData())
         mSelectedItems[i]->getObject()->setHidden(hide);
   }

}

//------------------------------------------------------------------------------

// handles icon assignments
S32 GuiTreeViewCtrl::getIcon(const char * iconString)
{
   if( iconString == NULL ) 
      return Default;

   S32 icon = Default;

   if (!dStrcmp(iconString, "SimGroup"))
      icon = SimGroup1;

   return icon;
}

void GuiTreeViewCtrl::addInspectorDataItem(Item *parent, SimObject *obj)
{
   S32 icon = getIcon(obj->getClassName());
   Item *item = createItem(icon);
   item->mState.set(Item::InspectorData);

   // Deal with child objects...
   if(dynamic_cast<SimSet*>(obj))
      item->mState.set(Item::VirtualParent);

   // Actually store the data!
   item->setObject(obj);

   // Now add us to the data structure...
   if(parent)
   {
      // Add as child of parent.
      if(parent->mChild)
      {
         Item * traverse = parent->mChild;
         while(traverse->mNext)
            traverse = traverse->mNext;

         traverse->mNext = item;
         item->mPrevious = traverse;
      }
      else
         parent->mChild = item;

      item->mParent = parent;
   }
   else
   {
      // If no parent, add to root.
      item->mNext = mRoot;
      mRoot = item;
      item->mParent = NULL;
   }

   if(!parent || parent->isExpanded())
      mFlags.set(RebuildVisible);

   buildVisibleTree();

}

void GuiTreeViewCtrl::unlinkItem(Item * item)
{
   if (item->mPrevious)
      item->mPrevious->mNext = item->mNext;

   if (item->mNext)
      item->mNext->mPrevious = item->mPrevious;
}

bool GuiTreeViewCtrl::childSearch(Item * item, SimObject *obj, bool yourBaby)
{
   Item * temp = item->mChild;
   while (temp)
   {
      //do you have my baby?
      if (temp->isInspectorData())
      {
         if (temp->getObject() == obj)
            yourBaby = false; //probably a child of an inner script
      }
      yourBaby = childSearch(temp,obj, yourBaby);
      temp = temp->mNext;
   }
   return yourBaby;
}

void GuiTreeViewCtrl::inspectorSearch(Item * item, Item * parent, SimSet * parentSet, SimSet * newParentSet)
{
   if (!parentSet||!newParentSet)
      return;

   if (item == parent->mNext)
      return;

   if (item)
   {
      if (item->isInspectorData())
      {
         // remove the object from the parentSet and add it to the newParentSet
         SimObject* simObj = item->getObject();

         if (parentSet->size())
         {
            SimObject *lastObject = parentSet->last();
            parentSet->removeObject(simObj);
            parentSet->reOrder(lastObject);
         }
         else
            parentSet->removeObject(simObj);

         newParentSet->addObject(simObj);

         if (item->mNext)
         {
            inspectorSearch(item->mNext, parent, parentSet, newParentSet);
            return;
         }
         else
         {
            // end of children so backing up
            if (item->mParent == parent)
               return;
            else
            {
               inspectorSearch(item->mParent->mNext, parent, parentSet, newParentSet);
               return;
            }
         }
      }

      if (item->mChild)
      {
         inspectorSearch(item->mChild, parent, parentSet, newParentSet);
         return;
      }

      if (item->mNext)
      {
         inspectorSearch(item->mNext, parent, parentSet, newParentSet);
         return;
      }
   }
}
bool GuiTreeViewCtrl::onVirtualParentBuild(Item *item, bool bForceFullUpdate)
{
   if(!item->mState.test(Item::InspectorData))
      return true;

   // Blast an item if it doesn't have a corresponding SimObject...
   if(item->mInspectorInfo.mObject == NULL)
   {
      removeItem(item->mId);
      return false;
   }

   // Skip the next stuff unless we're expanded...
     if(!item->isExpanded() && !bForceFullUpdate)
      return true;

   // Verify that we have all the kids we should in here...
   SimSet *srcObj = dynamic_cast<SimSet*>(&(*item->mInspectorInfo.mObject));

   // If it's not a SimSet... WTF are we doing here?
   if(!srcObj)
      return true;

   SimSet::iterator i;

   // This is slow but probably ok.
   for(i = srcObj->begin(); i != srcObj->end(); i++)
   {
      SimObject *obj = *i;

      // If we can't find it, add it.
      // unless it has a parent that is a child that is a script
      Item *res = item->findChildByValue(obj);

      bool foundChild = true;

      // search the children. if any of them are the parent of the object then don't add it.
      foundChild = childSearch(item,obj,foundChild);

      if(!res && foundChild)
      {
         if (mDebug) Con::printf("adding something");
         addInspectorDataItem(item, obj);
      }
   }

   return true;
}

bool GuiTreeViewCtrl::onVirtualParentExpand(Item *item)
{
   // Do nothing...
   return true;
}

bool GuiTreeViewCtrl::onVirtualParentCollapse(Item *item)
{
   // Do nothing...
   return true;
}

void GuiTreeViewCtrl::inspectObject(SimObject *obj, bool okToEdit)
{
   destroyTree();
   mFlags.set(IsEditable, okToEdit);

   //build our icon table
   const char * res  = Con::executef(this, 1, "onDefineIcons");

   if(!(dAtob(res)))
   {
      // if no icons were defined in script then use defaults.
      buildIconTable(NULL);
   }

   addInspectorDataItem(NULL, obj);
}

S32 GuiTreeViewCtrl::findItemByName(const char *name)
{
   for (S32 i = 0; i < mItems.size(); i++) 
   {
      if (mItems[i] != NULL)
      {
         if (dStrcmp(mItems[i]->getText(),name) == 0) 
            return mItems[i]->mId;
      }
   }

   return 0;
}

StringTableEntry GuiTreeViewCtrl::getTextToRoot( S32 itemId, const char * delimiter )
{
   Item * item = getItem(itemId);

   if(!item)
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getTextToRoot: invalid start item id!");
      return StringTable->EmptyString;
   }

   if(item->isInspectorData())
   {
      Con::errorf(ConsoleLogEntry::General, "GuiTreeViewCtrl::getTextToRoot: cannot get text to root of inspector data items");
      return StringTable->EmptyString;
   }

   char bufferOne[1024];
   char bufferTwo[1024];
   char bufferNodeText[128];

   dMemset( bufferOne, 0, sizeof(bufferOne) );
   dMemset( bufferTwo, 0, sizeof(bufferTwo) );

   dStrcpy( bufferOne, item->getText() );

   Item *prevNode = item->mParent;
   while ( prevNode )
   {
      dMemset( bufferNodeText, 0, sizeof(bufferNodeText) );
      dStrcpy( bufferNodeText, prevNode->getText() );
      dSprintf( bufferTwo, 1024, "%s%s%s",bufferNodeText, delimiter, bufferOne );
      dStrcpy( bufferOne, bufferTwo );
      dMemset( bufferTwo, 0, sizeof(bufferTwo) );
      prevNode = prevNode->mParent;
   }

   // Return the result, StringTable-ized.
   return StringTable->insert( bufferOne );
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, findItemByName, S32, 3, 3, "(name) Find item by name\n"
              "@param name The name of the desired object.\n"
              "@return Returns the ID of the object, or -1 on failure (not found).")
{
   return(object->findItemByName(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, insertItem, S32, 4, 8, "(TreeItemId parent, name, value, icon, normalImage=0, expandedImage=0) Adds item to tree control.\n"
              "@param parent The new item's parent.\n"
              "@param name The name of the new item.\n"
              "@param value The new item's value.\n"
              "@param icon The new item's icon\n"
              "@return Returns the new item's ID.")
{
   S32 norm=0, expand=0;

   if (argc > 6) 
   {
      norm = dAtoi(argv[6]);
      if(argc > 7)
         expand = dAtoi(argv[7]);
   }

   return(object->insertItem(dAtoi(argv[2]),  argv[3], argv[4], argv[5], norm, expand));
}

ConsoleMethod(GuiTreeViewCtrl, lockSelection, void, 2, 3, "([bool lock]) Set whether the selection is to be locked."
              "@param lock Boolean flag for whether or not the current selected object should be locked\n"
              "@return No return value.")
{
   bool lock = true;
   if(argc == 3)
      lock = dAtob(argv[2]);
   object->lockSelection(lock);
}

ConsoleMethod(GuiTreeViewCtrl, clearSelection, void, 2, 2, "() Clear selection\n"
              "@return No return value.")
{
   object->clearSelection();
}

ConsoleMethod(GuiTreeViewCtrl, deleteSelection, void, 2, 2, "() Delete all selected items.\n"
              "@return No return value.\n")
{
   object->deleteSelection();
}

ConsoleMethod(GuiTreeViewCtrl, addSelection, void, 3, 3, "(string ID) Select an item"
              "@param ID The ID of the item to select.\n"
              "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->addSelection(id);
}

ConsoleMethod(GuiTreeViewCtrl, addChildSelectionByValue, void, 4, 4, "(TreeItemId parent, value)")
{
   S32 id = dAtoi(argv[2]);
   GuiTreeViewCtrl::Item* parentItem = object->getItem(id);
   if (parentItem)
   {
      GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(argv[3]);
      if (child)
         object->addSelection(child->getID());
   }
}

ConsoleMethod(GuiTreeViewCtrl, removeSelection, void, 3, 3, "(string ID) Deselects given item.\n"
              "@param ID The ID of the item to deselect.\n"
              "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->removeSelection(id);
}

ConsoleMethod(GuiTreeViewCtrl, removeChildSelectionByValue, void, 4, 4, "removeChildSelectionByValue(TreeItemId parent, value)")
{
   S32 id = dAtoi(argv[2]);
   GuiTreeViewCtrl::Item* parentItem = object->getItem(id);
   if(parentItem)
   {
      GuiTreeViewCtrl::Item* child = parentItem->findChildByValue(argv[3]);
      if(child)
         object->removeSelection(child->getID());
   }
}

ConsoleMethod(GuiTreeViewCtrl, selectItem, bool, 3, 4, "(TreeItemId item, [bool select=true]) Selects item.")
{
   S32 id = dAtoi(argv[2]);
   bool select = true;
   if(argc == 4)
      select = dAtob(argv[3]);

   return(object->setItemSelected(id, select));
}

ConsoleMethod(GuiTreeViewCtrl, expandItem, bool, 3, 4, "(TreeItemId item, [bool expand=true]) Deselects item")
{
   S32 id = dAtoi(argv[2]);
   bool expand = true;
   if(argc == 4)
      expand = dAtob(argv[3]);
   return(object->setItemExpanded(id, expand));
}

// Make the given item visible.
ConsoleMethod(GuiTreeViewCtrl, scrollVisible, void, 3, 3, "(TreeItemId item) Make the given item visible.\n"
              "@param ID of the desired item.\n"
              "@return No return value.")
{
   object->scrollVisible(dAtoi(argv[2]));
}


ConsoleMethod(GuiTreeViewCtrl, buildIconTable, bool, 3,3, "(string icons) Icons should be designated by the bitmap/png file names (minus the file extensions) "
              "and separated by colons (:). This list should be synchronized with the Icons enum.\n"
              "@param icons The list of icons to add sepated by colons.\n"
              "@return Returns true on success, false otherwise.")
{   
   const char * icons = argv[2];
   return object->buildIconTable(icons);
}

ConsoleMethod( GuiTreeViewCtrl, open, void, 3, 4, "(SimSet obj, bool okToEdit=true) Set the root of the tree view to the specified object, or to the root set.")
{
   SimSet *treeRoot = NULL;
   SimObject* target = Sim::findObject(argv[2]);

   bool okToEdit = true;

   if (argc == 4)
      okToEdit = dAtob(argv[3]);

   if (target)
      treeRoot = dynamic_cast<SimSet*>(target);

   if (! treeRoot)
      Sim::findObject(RootGroupId, treeRoot);

   object->inspectObject(treeRoot,okToEdit);
}

ConsoleMethod(GuiTreeViewCtrl, getItemText, const char *, 3, 3, "(TreeItemId item)")
{
   return(object->getItemText(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getItemValue, const char *, 3, 3, "(TreeItemId item)")
{
   return(object->getItemValue(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, editItem, bool, 5, 5, "(TreeItemId item, string newText, string newValue)")
{
   return(object->editItem(dAtoi(argv[2]), argv[3], argv[4]));
}

ConsoleMethod(GuiTreeViewCtrl, removeItem, bool, 3, 3, "(TreeItemId item)")
{
   return(object->removeItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, removeAllChildren, void, 3, 3, "removeAllChildren(TreeItemId parent)")
{
   object->removeAllChildren(dAtoi(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, clear, void, 2, 2, "() - empty tree")
{
   object->removeItem(0);
}

ConsoleMethod(GuiTreeViewCtrl, getFirstRootItem, S32, 2, 2, "Get id for root item.")
{
   return(object->getFirstRootItem());
}

ConsoleMethod(GuiTreeViewCtrl, getChild, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getChildItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, buildVisibleTree, void, 3, 3, "Build the visible tree")
{
   object->buildVisibleTree(dAtob(argv[2]));
}

ConsoleMethod(GuiTreeViewCtrl, getParent, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getParentItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getNextSibling, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getNextSiblingItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getPrevSibling, S32, 3, 3, "(TreeItemId item)")
{
   return(object->getPrevSiblingItem(dAtoi(argv[2])));
}

ConsoleMethod(GuiTreeViewCtrl, getItemCount, S32, 2, 2, "() @return Returns the number of items in control")
{
   return(object->getItemCount());
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedItem, S32, 2, 2, "() @return Returns the ID of the selected item.")
{
   return ( object->getSelectedItem() );
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedObject, S32, 2, 2, "() @return Returns the currently selected simObject in inspector mode or -1")
{
   GuiTreeViewCtrl::Item *item = object->getItem( object->getSelectedItem() );
   if( item != NULL && item->isInspectorData() )
   {
      SimObject *obj = item->getObject();
      if( obj != NULL )
         return object->getId();
   }

   return -1;
}

ConsoleMethod(GuiTreeViewCtrl, moveItemUp, void, 3, 3, "(TreeItemId item)")
{
   object->moveItemUp( dAtoi( argv[2] ) );
}

ConsoleMethod(GuiTreeViewCtrl, getSelectedItemsCount, S32, 2, 2, "")
{
   return ( object->getSelectedItemsCount() );
}

ConsoleMethod(GuiTreeViewCtrl, moveItemDown, void, 3, 3, "(TreeItemId item)")
{
   object->moveItemDown( dAtoi( argv[2] ) );
}

//-----------------------------------------------------------------------------

ConsoleMethod(GuiTreeViewCtrl, getTextToRoot, const char*,4,4,"(TreeItemId item,Delimiter=none) gets the text from the current node to the root, concatenating at each branch upward, with a specified delimiter optionally")
{
   if ( argc < 4 )
   {
      Con::warnf("GuiTreeViewCtrl::getTextToRoot - Invalid number of arguments!");
      return ("");
   }
   S32 itemId = dAtoi( argv[2] );
   StringTableEntry delimiter = argv[3];

   return object->getTextToRoot( itemId, delimiter );
}

ConsoleMethod(GuiTreeViewCtrl,getSelectedItemList,const char*, 2,2,"returns a space seperated list of mulitple item ids")
{
    char* buff = Con::getReturnBuffer(1024);
    dSprintf(buff,1024,"");

    for(int i = 0; i < object->mSelected.size(); i++)
    {
        S32 id  = object->mSelected[i];
        //get the current length of the buffer
        U32	len = dStrlen(buff);
        //the start of the buffer where we want to write
        char* buffPart = buff+len;
        //the size of the remaining buffer (-1 cause dStrlen doesn't count the \0)
        S32 size	=	1024-len-1;
        //write it:
        if(size < 1)
        {
            Con::errorf("GuiTreeViewCtrl::getSelectedItemList - Not enough room to return our object list");
            return buff;
        }

        dSprintf(buffPart,size,"%d ", id);
    }
//mSelected

    return buff;
}

//------------------------------------------------------------------------------
S32 GuiTreeViewCtrl::findItemByObjectId(S32 iObjId)
{  
   for (S32 i = 0; i < mItems.size(); i++)
   {
      if (mItems[i] != NULL)
      {
         SimObject* pObj = mItems[i]->getObject();
         if(pObj && pObj->getId() == iObjId)
            return mItems[i]->mId;
      }
   }

   return -1;
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, findItemByObjectId, S32, 3, 3, "(find item by object id and returns the mId)")
{
   return(object->findItemByObjectId(dAtoi(argv[2])));
}

//------------------------------------------------------------------------------
bool GuiTreeViewCtrl::scrollVisibleByObjectId(S32 objID)
{
   S32 itemID = findItemByObjectId(objID);

   if(itemID == -1)
   {
      // we did not find the item in our current items
      // we should try to find and show the parent of the item.
      SimObject *obj = Sim::findObject(objID);

      if(!obj || !obj->getGroup())
         return false;
    
      // if we can't show the parent, we fail.
      if(! scrollVisibleByObjectId(obj->getGroup()->getId()) )
         return false;
      
      // get the parent. expand the parent. rebuild the tree. this ensures that
      // we'll be able to find the child item we're targeting.
      S32 parentID = findItemByObjectId(obj->getGroup()->getId());
      AssertFatal(parentID != -1, "We were able to show the parent, but could not then find the parent. This should not happen.");
      Item *parentItem = getItem(parentID);
      parentItem->setExpanded(true);
      buildVisibleTree();
      
      // NOW we should be able to find the object. if not... something's wrong.
      itemID = findItemByObjectId(objID);
      AssertWarn(itemID != -1,"GuiTreeViewCtrl::scrollVisibleByObjectId() found the parent, but can't find it's immediate child. This should not happen.");
      if(itemID == -1)
         return false;
   }
   
   // ok, item found. scroll to it.
   scrollVisible(itemID);
   
   return true;
}

//------------------------------------------------------------------------------
ConsoleMethod(GuiTreeViewCtrl, scrollVisibleByObjectId, S32, 3, 3, "(show item by object id. returns true if sucessful.)")
{
   return(object->scrollVisibleByObjectId(dAtoi(argv[2])));
}


