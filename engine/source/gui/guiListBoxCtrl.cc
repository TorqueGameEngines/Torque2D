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
#include "platform/platform.h"
#include "gui/guiListBoxCtrl.h"
#include "gui/guiCanvas.h"
#include <algorithm>

#include "guiListBoxCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiListBoxCtrl);

GuiListBoxCtrl::GuiListBoxCtrl()
{
   mItems.clear();
   mSelectedItems.clear();
   mMultipleSelections = true;
   mFitParentWidth = true;
   mItemSize = Point2I(10,20);
   mLastClickItem = NULL;
   mIsContainer = false;
   mActive = true;
   caller = this;

   setField("profile", "GuiListBoxProfile");
}

GuiListBoxCtrl::~GuiListBoxCtrl()
{
   clearItems();
}

void GuiListBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField( "AllowMultipleSelections", TypeBool, Offset( mMultipleSelections, GuiListBoxCtrl) );
   addField( "FitParentWidth", TypeBool, Offset( mFitParentWidth, GuiListBoxCtrl) );
}

bool GuiListBoxCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   updateSize();

   return true;
}

void GuiListBoxCtrl::addObject(SimObject *obj)
{
	AssertWarn(0, "GuiListBoxCtrl::addObject: cannot add children to the GuiListBoxCtrl. It is not a container. Child was not added.");
	removeObject(obj);
}

#pragma region ItemAccessors
void GuiListBoxCtrl::clearItems()
{
   // Free item list allocated memory
   while( mItems.size() )
      deleteItem( 0 );

   // Free our vector lists
   mItems.clear();
   mSelectedItems.clear();
}

void GuiListBoxCtrl::clearSelection()
{
   if( !mSelectedItems.size() )
      return;

   VectorPtr<LBItem*>::iterator i = mSelectedItems.begin();
   for( ; i != mSelectedItems.end(); i++ )
      (*i)->isSelected = false;

   mSelectedItems.clear();
}

void GuiListBoxCtrl::removeSelection( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::removeSelection - index out of range!" );
      return;
   }

   removeSelection( mItems[index], index );
}
void GuiListBoxCtrl::removeSelection( LBItem *item, S32 index )
{
   if( !mSelectedItems.size() )
      return;

   if( !item )
      return;

   for( S32 i = 0 ; i < mSelectedItems.size(); i++ )
   {
      if( mSelectedItems[i] == item )
      {
         mSelectedItems.erase( &mSelectedItems[i] );
         item->isSelected = false;
         Con::executef(caller, 3, "onUnSelect", Con::getIntArg( index ), item->itemText, item->ID);
         return;
      }
   }
}

void GuiListBoxCtrl::addSelection( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::addSelection- index out of range!" );
      return;
   }

   addSelection( mItems[index], index );
}
void GuiListBoxCtrl::addSelection( LBItem *item, S32 index )
{
   if( !mMultipleSelections )
   {
      if( !mSelectedItems.empty() )
      {
         LBItem* selItem = mSelectedItems.front();
         if( selItem != item )
            clearSelection();
         else
            return;
      }
   }
   else
   {
      if( !mSelectedItems.empty() )
      {
         for( S32 i = 0; i < mSelectedItems.size(); i++ )
         {
            if( mSelectedItems[ i ] == item )
               return;
         }
      }
   }

   item->isSelected = true;
   mSelectedItems.push_front( item );

   ScrollToIndex(index);

   if(caller->isMethod("onSelect"))
		Con::executef(caller, 3, "onSelect", Con::getIntArg( index ), item->itemText, item->ID);

}

S32 GuiListBoxCtrl::getItemIndex( LBItem *item )
{
   if( mItems.empty() )
      return -1;

   // Lookup the index of an item in our list, by the pointer to the item
   for( S32 i = 0; i < mItems.size(); i++ )
      if( mItems[i] == item )
         return i;

   return -1;
}

S32 GuiListBoxCtrl::getItemCount()
{
   return mItems.size();
}

S32 GuiListBoxCtrl::getSelCount()
{
   return mSelectedItems.size();
}

S32 GuiListBoxCtrl::getSelectedItem()
{
   if( mSelectedItems.empty() || mItems.empty() )
      return -1;

   for( S32 i = 0 ; i < mItems.size(); i++ )
      if( mItems[i]->isSelected )
         return i;

   return -1;
}

void GuiListBoxCtrl::getSelectedItems( Vector<S32> &Items )
{
   // Clear our return vector
   Items.clear();
   
   // If there are no selected items, return an empty vector
   if( mSelectedItems.empty() )
      return;
   
   for( S32 i = 0; i < mItems.size(); i++ )
      if( mItems[i]->isSelected )
         Items.push_back( i );
}

S32 GuiListBoxCtrl::findItemText( StringTableEntry text, bool caseSensitive )
{
   // Check Proper Arguments
   if( !text || !text[0] || text == StringTable->EmptyString )
   {
      Con::warnf("GuiListBoxCtrl::findItemText - No Text Specified!");
      return -1;
   }

   // Check Items Exist.
   if( mItems.empty() )
      return -1;

   // Lookup the index of an item in our list, by the pointer to the item
   for( S32 i = 0; i < mItems.size(); i++ )
   {
      // Case Sensitive Compare?
      if( caseSensitive && ( dStrcmp( mItems[i]->itemText, text ) == 0 ) )
         return i;
      else if (!caseSensitive && ( dStricmp( mItems[i]->itemText, text ) == 0 ))
         return i;
   }

   // Not Found!
   return -1;
}

void GuiListBoxCtrl::setSelectionInternal(StringTableEntry text)
{
	S32 index = findItemText(text);
	if (index != -1)
	{
		mSelectedItems.clear();
		LBItem *item = mItems[index];
		item->isSelected = true;
		mSelectedItems.push_front(item);
	}
}

void GuiListBoxCtrl::setCurSel( S32 index )
{
   // Range Check
   if( index >= mItems.size() )
   {
      Con::warnf("GuiListBoxCtrl::setCurSel - index out of range!" );
      return;
   }

   // If index -1 is specified, we clear the selection
   if( index == -1 )
   {
      mSelectedItems.clear();
      return;
   }

   // Add the selection
   addSelection( mItems[ index ], index );
}

void GuiListBoxCtrl::setCurSelRange( S32 start, S32 stop )
{
   // Verify Selection Range
   if( start < 0 )
      start = 0;
   else if( start > mItems.size() )
      start = mItems.size();

   if( stop < 0 )
      stop = 0;
   else if( stop > mItems.size() )
      stop = mItems.size();

   S32 iterStart = ( start < stop ) ? start : stop;
   S32 iterStop  = ( start < stop ) ? stop : start;

   for( ; iterStart <= iterStop; iterStart++ )
      addSelection( mItems[iterStart], iterStart );
}

S32 GuiListBoxCtrl::addItem( StringTableEntry text, void *itemData )
{
   // This just calls insert item at the end of the list
   return insertItem( mItems.size(), text, itemData );
}

S32 GuiListBoxCtrl::addItemWithColor( StringTableEntry text, ColorF color, void *itemData )
{
   // This just calls insert item at the end of the list
   return insertItemWithColor( mItems.size(), text, color, itemData );
}

S32	GuiListBoxCtrl::addItemWithID(StringTableEntry text, S32 ID, void *itemData)
{
	S32 index = insertItem( mItems.size(), text, itemData);
	setItemID(index, ID);
	return index;
}

void GuiListBoxCtrl::setItemColor( S32 index, ColorF color )
{
   if ((index >= mItems.size()) || index < 0)
   {
      Con::warnf("GuiListBoxCtrl::setItemColor - invalid index");
      return;
   }

   LBItem* item = mItems[index];
   item->hasColor = true;
   item->color = color;
}

void GuiListBoxCtrl::clearItemColor(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::setItemColor - invalid index");
		return;
	}

	LBItem* item = mItems[index];
	item->hasColor = false;
}

void GuiListBoxCtrl::clearAllColors()
{
	if (!mSelectedItems.size())
		return;

	VectorPtr<LBItem*>::iterator i = mSelectedItems.begin();
	for (; i != mSelectedItems.end(); i++)
		(*i)->hasColor = false;
}

ColorF GuiListBoxCtrl::getItemColor(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::getItemColor - invalid index");
		return ColorF(0,0,0,0);
	}

	LBItem* item = mItems[index];
	return item->color;
}

bool GuiListBoxCtrl::getItemHasColor(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::getItemHasColor - invalid index");
		return false;
	}

	LBItem* item = mItems[index];
	return item->hasColor;
}

void GuiListBoxCtrl::setItemID(S32 index, S32 ID)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::setItemID - invalid index");
		return;
	}

	LBItem* item = mItems[index];
	item->ID = ID;
}

S32 GuiListBoxCtrl::getItemID(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::setItemID - invalid index");
		return 0;
	}

	LBItem* item = mItems[index];
	return item->ID;
}

S32 GuiListBoxCtrl::findItemID(S32 ID)
{
	// Check Items Exist.
	if (mItems.empty())
		return -1;

	// Lookup the index of an item in our list, by the pointer to the item
	for (S32 i = 0; i < mItems.size(); i++)
	{
		if (mItems[i]->ID == ID)
		{
			return i;
		}
	}

	// Not Found!
	return -1;
}

void GuiListBoxCtrl::setItemActive(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::setItemActive - invalid index");
		return;
	}

	LBItem* item = mItems[index];
	item->isActive = true;
}

void GuiListBoxCtrl::setItemInactive(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::setItemInactive - invalid index");
		return;
	}

	LBItem* item = mItems[index];
	item->isActive = false;
}

bool GuiListBoxCtrl::getItemActive(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::getItemActive - invalid index");
		return true;
	}

	LBItem* item = mItems[index];
	return item->isActive;
}

S32 GuiListBoxCtrl::insertItem( S32 index, StringTableEntry text, void *itemData )
{
   // If the index is greater than our list size, insert it at the end
   if( index >= mItems.size() )
      index = mItems.size();

   // Sanity checking
   if( !text )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - cannot add NULL string" );
      return -1;
   }

   LBItem *newItem = new LBItem;
   if( !newItem )
   {
      Con::warnf("GuiListBoxCtrl::insertItem - error allocating item memory!" );
      return -1;
   }

   // Assign item data
   newItem->itemText = StringTable->insert(text, true);
   newItem->isSelected = false;
   newItem->isActive = true;
   newItem->ID = 0;
   newItem->itemData = itemData;
   newItem->hasColor = false;

   // Add to list
   mItems.insert(index);
   mItems[index] = newItem;

   // Resize our list to fit our items
   updateSize();

   // Return our index in list (last)
   return index;
}

S32 GuiListBoxCtrl::insertItemWithColor( S32 index, StringTableEntry text, ColorF color, void *itemData )
{
	if( color == ColorF(-1, -1, -1) )
	{
		Con::warnf("GuiListBoxCtrl::insertItem - cannot add NULL color" );
			return -1;
	}

	index = insertItem(index, text, itemData);

	if(index != -1)
	{
		setItemColor(index, color);
	}

	// Return our index in list (last)
	return index;
}

void  GuiListBoxCtrl::deleteItem( S32 index )
{
   // Range Check
   if( index >= mItems.size() || index < 0 )
   {
      Con::warnf("GuiListBoxCtrl::deleteItem - index out of range!" );
      return;
   }

   // Grab our item
   LBItem* item = mItems[ index ];
   if( !item )
   {
      Con::warnf("GuiListBoxCtrl::deleteItem - Bad Item Data!" );
      return;
   }

   // Remove it from the selected list.
   if( item->isSelected )
   {
      for( VectorPtr<LBItem*>::iterator i = mSelectedItems.begin(); i != mSelectedItems.end(); i++ )
      {
         if( item == *i )
         {
            mSelectedItems.erase_fast( i );
            break;
         }
      }
   }

   // Remove it from the list
   mItems.erase( &mItems[ index ] );

   // Free the memory associated with it
   delete item;
}

StringTableEntry GuiListBoxCtrl::getItemText( S32 index )
{
   // Range Checking
   if( index > mItems.size() || index < 0 )
   {
      Con::warnf( "GuiListBoxCtrl::getItemText - index out of range!" );
      return StringTable->EmptyString;
   }
   
   return mItems[ index ]->itemText;   
}

void GuiListBoxCtrl::setItemText( S32 index, StringTableEntry text )
{
   // Sanity Checking
   if( !text )
   {
      Con::warnf("GuiListBoxCtrl::setItemText - Invalid Text Specified!" );
      return;
   }
   // Range Checking
   if( index > mItems.size() || index < 0 )
   {
      Con::warnf( "GuiListBoxCtrl::getItemText - index out of range!" );
      return;
   }

   mItems[ index ]->itemText = StringTable->insert( text );
}
#pragma endregion

#pragma region Sizing
void GuiListBoxCtrl::updateSize()
{
   if( !mProfile || !mProfile->mFont )
      return;

   GFont *font = mProfile->mFont;
   Point2I contentSize = Point2I(10, font->getHeight() + 2);

   if (!mFitParentWidth)
   {
      // Find the maximum width cell:
      S32 maxWidth = 1;
      for ( U32 i = 0; i < (U32)mItems.size(); i++ )
      {
         S32 width = font->getStrWidth( mItems[i]->itemText );
         if( width > maxWidth )
            maxWidth = width;
      }
      contentSize.x = maxWidth + 6;
   }

   mItemSize = this->getOuterExtent(contentSize, NormalState, mProfile);
   Point2I newExtent = Point2I(mItemSize.x, mItemSize.y * mItems.size());

   //Don't update the extent.x if we are matching our parent's size. We will handle it during rendering.
   if (mFitParentWidth)
   {
	   newExtent.x = mBounds.extent.x;
   }

   resize( mBounds.point, newExtent );
}

void GuiListBoxCtrl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   Parent::parentResized( oldParentExtent, newParentExtent );

   updateSize();
}
#pragma endregion

#pragma region Rendering
void GuiListBoxCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	RectI clip = dglGetClipRect();
	if (mFitParentWidth && ( mBounds.extent.x != clip.extent.x || mItemSize.x != clip.extent.x))
	{
		mBounds.extent.x = clip.extent.x;
		mItemSize.x = clip.extent.x;
	}


	for ( S32 i = 0; i < mItems.size(); i++)
	{
		// Only render visible items
		if ((i + 1) * mItemSize.y + offset.y < updateRect.point.y)
		continue;

		// Break out once we're no longer in visible item range
		if( i * mItemSize.y + offset.y >= updateRect.point.y + updateRect.extent.y)
		break;

		RectI itemRect = RectI( offset.x, offset.y + ( i * mItemSize.y ), mItemSize.x, mItemSize.y );

		// Render our item
		onRenderItem( itemRect, mItems[i] );
	}
}

void GuiListBoxCtrl::onRenderItem( RectI &itemRect, LBItem *item )
{
   Point2I cursorPt = Point2I(0,0);
   GuiCanvas *root = getRoot();
   if (root)
   {
	   cursorPt = root->getCursorPos();
   }
   GuiControlState currentState = GuiControlState::NormalState;
   if (!mActive || !item->isActive)
	   currentState = GuiControlState::DisabledState;
   else if (item->isSelected)
	   currentState = GuiControlState::SelectedState;
   else if (itemRect.pointInRect(cursorPt))
	   currentState = GuiControlState::HighlightState;
   RectI ctrlRect = applyMargins(itemRect.point, itemRect.extent, currentState, mProfile);

   if (!ctrlRect.isValidRect())
   {
	   return;
   }

   renderUniversalRect(ctrlRect, mProfile, currentState);

   //Render Text
   dglSetBitmapModulation(mProfile->getFontColor(currentState));
   RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
   RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);

   // Render color bullet if needed
   if (item->hasColor)
   {
	   RectI drawArea = RectI(contentRect.point.x, contentRect.point.y, contentRect.extent.y, contentRect.extent.y);
      ColorI color = item->color;
	   renderColorBullet(drawArea, color, 5);

	   contentRect.point.x += contentRect.extent.y;
	   contentRect.extent.x -= contentRect.extent.y;
   }

   renderText(contentRect.point, contentRect.extent, item->itemText, mProfile);
}

void GuiListBoxCtrl::ScrollToIndex(const S32 targetIndex)
{
	GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
	if (parent)
		parent->scrollRectVisible(RectI(0, mItemSize.y * targetIndex, mItemSize.x, mItemSize.y));
}
#pragma endregion

#pragma region InputEvents
void GuiListBoxCtrl::onTouchDragged(const GuiEvent &event)
{
	if (!mActive)
	{
		return;
	}

   Parent::onTouchDragged(event);

   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   S32 itemHit = (localPoint.y < 0) ? -1 : (S32)mFloor((F32)localPoint.y / (F32)mItemSize.y);

   if (itemHit >= mItems.size() || itemHit == -1)
	   return;

   LBItem *hitItem = mItems[itemHit];
   if (hitItem == NULL || !hitItem->isActive)
	   return;

   if(caller->isMethod("onTouchDragged"))
      Con::executef(caller, 3, "onTouchDragged", Con::getIntArg(itemHit), hitItem->itemText, hitItem->ID);
}

void GuiListBoxCtrl::onTouchDown( const GuiEvent &event )
{
	if (!mActive)
	{
		return;
	}

	setFirstResponder();

   Point2I localPoint = globalToLocalCoord(event.mousePoint);
   S32 itemHit = ( localPoint.y < 0 ) ? -1 : (S32)mFloor( (F32)localPoint.y / (F32)mItemSize.y );

   if ( itemHit >= mItems.size() || itemHit == -1 )
      return;

   LBItem *hitItem = mItems[ itemHit ];
   if ( hitItem == NULL || !hitItem->isActive)
      return;

   // If we're not a multiple selection listbox, we simply select/unselect an item
   if( !mMultipleSelections )
   {
      // No current selection?  Just select the cell and move on
      S32 selItem = getSelectedItem();

      if ( selItem != itemHit && selItem != -1 )
         clearSelection();

      // Set the current selection
      setCurSel( itemHit );

		if( itemHit == selItem && event.mouseClickCount == 2)
		{
			if(caller->isMethod("onDoubleClick") )
				Con::executef(caller, 3, "onDoubleClick", Con::getIntArg(itemHit), hitItem->itemText, hitItem->ID);
		}
		else if (caller->isMethod("onClick"))
		{
			Con::executef(caller, 3, "onClick", Con::getIntArg(itemHit), hitItem->itemText, hitItem->ID);
		}

      // Store the clicked item
      mLastClickItem = hitItem;

      return;
   }
   
   // Deal with multiple selections
   if( event.modifier & SI_CTRL)
   {
      // Ctrl-Click toggles selection
      if( hitItem->isSelected )
      {
         removeSelection( hitItem, itemHit );

         // We return here when we deselect an item because we don't store last clicked when we deselect
         return;
      }
      else
         addSelection( hitItem, itemHit );
   }
   else if( event.modifier & SI_SHIFT )
   {
      if( !mLastClickItem )
         addSelection( hitItem, itemHit );
      else
         setCurSelRange( getItemIndex( mLastClickItem ), itemHit );
   }
   else
   {
      if( getSelCount() != 0 )
      {
         S32 selItem = getSelectedItem();
         if( selItem != -1 && mItems[selItem] != hitItem )
            clearSelection();
      }
      addSelection( hitItem, itemHit );
   }

   if( hitItem == mLastClickItem && event.mouseClickCount == 2)
   {
		if(caller->isMethod("onDoubleClick") )
			Con::executef(caller, 3, "onDoubleClick", Con::getIntArg(itemHit), hitItem->itemText, hitItem->ID);
	}
   else if (caller->isMethod("onClick"))
   {
	   Con::executef(caller, 3, "onClick", Con::getIntArg(itemHit), hitItem->itemText, hitItem->ID);
   }

   mLastClickItem = hitItem;
}

bool GuiListBoxCtrl::onKeyDown(const GuiEvent &event)
{
	//if this control is a dead end, make sure the event stops here
	if (!mVisible || !mActive || !mAwake || mItems.size() == 0)
		return true;

	S32 index = getSelectedItem();
	switch (event.keyCode)
	{
	case KEY_RETURN:
		if (mAltConsoleCommand[0])
			Con::evaluate(mAltConsoleCommand, false);
		break;
	case KEY_LEFT:
	case KEY_UP:
		if (index == -1)
		{
			//Select the bottom item
			addSelection(mItems.size() - 1);
		}
		else if(index != 0)
		{
			addSelection(index - 1);
		}
		break;
	case KEY_DOWN:
	case KEY_RIGHT:
		if (index == -1)
		{
			//Select the top item
			addSelection(0);
		}
		else if (index != (mItems.size() - 1))
		{
			addSelection(index + 1);
		}
		break;
	case KEY_HOME:
		addSelection(0);
		break;
	case KEY_END:
		addSelection(mItems.size() - 1);
		break;
	case KEY_DELETE:
		if (index != -1 && isMethod("onDeleteKey"))
			Con::executef(caller, 3, "onDeleteKey", Con::getIntArg(index), getItemText(index), getItemID(index));
		break;
	default:
		return(Parent::onKeyDown(event));
		break;
	};
}
#pragma endregion

#pragma region sorting
bool GuiListBoxCtrl::LBItem::sIncreasing = true;

void GuiListBoxCtrl::sortByText(bool increasing)
{
	if (mItems.size() < 2)
		return;

	LBItem::sIncreasing = increasing;
	std::sort(mItems.begin(), mItems.end(), LBItem::compByText);
}

void GuiListBoxCtrl::sortByID(bool increasing)
{
	if (mItems.size() < 2)
		return;

	LBItem::sIncreasing = increasing;
	std::sort(mItems.begin(), mItems.end(), LBItem::compByID);
}
#pragma endregion