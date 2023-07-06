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

#include "gui/guiTreeViewCtrl.h"
#include "graphics/dgl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiCanvas.h"
#include "gui/editor/guiEditCtrl.h"

#include "guiTreeViewCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiTreeViewCtrl);

GuiTreeViewCtrl::GuiTreeViewCtrl()
{
	mActive = true;
	mIsContainer = false;
	mIndentSize = 10;
	mMultipleSelections = true;
	mTouchPoint = Point2I::Zero;
	mDragActive = false;
	mDragIndex = 0;
	mIsDragLegal = false;
	mIsBoundToGuiEditor = false;
	mFocusControl = nullptr;
}

GuiTreeViewCtrl::~GuiTreeViewCtrl()
{
}

GuiTreeViewCtrl::TreeItem* GuiTreeViewCtrl::grabItemPtr(S32 index)
{
	// Range Check
	if (index >= mItems.size() || index < 0)
	{
		Con::warnf("GuiTreeViewCtrl::grabItemPtr - index out of range!");
		return nullptr;
	}

	// Grab our item
	TreeItem* treeItem = dynamic_cast<GuiTreeViewCtrl::TreeItem*>(mItems[index]);
	return treeItem;
}

void GuiTreeViewCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addField("BindToGuiEditor", TypeBool, Offset(mIsBoundToGuiEditor, GuiTreeViewCtrl));
}

void GuiTreeViewCtrl::onTouchDown(const GuiEvent& event)
{
	mTouchPoint == event.mousePoint;
	S32 hitIndex = getHitIndex(event);
	if (mIsBoundToGuiEditor && smDesignTime && hitIndex == 0)
	{
		if (!(event.modifier & SI_CTRL) && !(event.modifier & SI_SHIFT))
		{
			clearSelection();
			GuiEditCtrl* edit = GuiControl::smEditorHandle;
			if (edit)
			{
				GuiControl* root = static_cast<GuiControl*>(mItems[0]->itemData);
				edit->setCurrentAddSet(root, true);
			}
		}
	}
	else 
	{
		Parent::onTouchDown(event);
	}
}

void GuiTreeViewCtrl::onTouchDragged(const GuiEvent& event)
{
	mDragActive = false;
	if (!mActive || !mVisible)
		return;

	S32 hitIndex = getHitIndex(event);
	if (hitIndex >= mItems.size() || hitIndex == -1)
		return;

	LBItem* hitItem = mItems[hitIndex];
	if (hitItem == NULL || !hitItem->isActive)
		return;

	if (mAbs(mTouchPoint.x - event.mousePoint.x) > 2 || mAbs(mTouchPoint.y - event.mousePoint.y) > 2)
	{
		mDragActive = true;
	}
}
void GuiTreeViewCtrl::onTouchUp(const GuiEvent& event)
{
	if (mDragActive && mIsDragLegal)
	{
		TreeItem* dragItem = grabItemPtr(mDragIndex);
		if (mReorderMethod == ReorderMethod::Below && dragItem->isOpen)
		{
			mReorderMethod = ReorderMethod::Insert;
		}
		SimGroup* target = static_cast<SimGroup*>(mReorderMethod == ReorderMethod::Insert ? dragItem->itemData : dragItem->trunk->itemData);
		
		if (!target)
		{
			Con::warnf("GuiTreeViewCtrl::onTouchUp - attempted to drag selection into an object that is not a SimGroup");
			return;
		}
		vector<SimObject*> objectAboveTargetList = vector<SimObject*>();
		if (mReorderMethod != ReorderMethod::Insert)
		{
			S32 index = mReorderMethod == ReorderMethod::Below ? mDragIndex : mDragIndex - 1;
			TreeItem* checkItem = grabItemPtr(index);
			while (checkItem->level == dragItem->level)
			{
				if(!checkItem->isSelected)
				{
					SimObject* obj = static_cast<SimObject*>(checkItem->itemData);
					objectAboveTargetList.push_back(obj);
				}
				index = index - 1;
				checkItem = grabItemPtr(index);
			}
		}
		else
		{
			dragItem->isOpen = true;
		}

		for (S32 i = mItems.size() - 1; i >= 0; i--)
		{
			TreeItem* treeItem = dynamic_cast<TreeItem*>(mItems[i]);
			if(treeItem && treeItem->isSelected)
			{
				SimObject* obj = static_cast<SimObject*>(treeItem->itemData);
				if(obj)
				{
					target->addObject(obj);
					target->bringObjectToFront(obj);
				}
			}
		}
		for (auto obj : objectAboveTargetList)
		{
			SimGroup* group = obj->getGroup();
			group->bringObjectToFront(obj);
		}
		GuiControl* control = static_cast<GuiControl*>(mReorderMethod != ReorderMethod::Insert ? dragItem->trunk->itemData : dragItem->itemData);
		if (control)
		{
			control->childrenReordered();
		}
		refreshTree();
	}
	mDragActive = false;
	Parent::onTouchUp(event);
}

void GuiTreeViewCtrl::onPreRender()
{
	if (mIsBoundToGuiEditor && smDesignTime && smEditorHandle)
	{
		GuiEditCtrl* edit = GuiControl::smEditorHandle;
		if (edit)
		{
			const GuiControl* oldFocus = mFocusControl;
			mFocusControl = edit->getCurrentAddSet();
			if(oldFocus != mFocusControl)
			{
				for (S32 i = 0; i < mItems.size(); i++)
				{
					LBItem* item = mItems[i];
					TreeItem* treeItem = dynamic_cast<TreeItem*>(item);
					SimObject* obj = static_cast<SimObject*>(item->itemData);
					if (obj && obj->getId() == mFocusControl->getId())
					{
						treeItem->isSelected = false;
						if(!treeItem->isOpen)
						{
							treeItem->isOpen = true;
							refreshTree();
						}
					}
				}
			}
		}
	}
}

void GuiTreeViewCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	mFocusLevel = -1;
	RectI clip = dglGetClipRect();
	if (mFitParentWidth && (mBounds.extent.x != clip.extent.x || mItemSize.x != clip.extent.x))
	{
		mBounds.extent.x = clip.extent.x;
		mItemSize.x = clip.extent.x;
	}

	RectI dragRect;
	for (S32 i = 0, j = 0; i < mItems.size(); i++)
	{
		// Only render visible items
		if ((j + 1) * mItemSize.y + offset.y < updateRect.point.y)
			continue;

		// Break out once we're no longer in visible item range
		if (j * mItemSize.y + offset.y >= updateRect.point.y + updateRect.extent.y)
			break;

		RectI itemRect = RectI(offset.x, offset.y + (j * mItemSize.y), mItemSize.x, mItemSize.y);

		TreeItem* treeItem = dynamic_cast<TreeItem*>(mItems[i]);

		if(!treeItem || treeItem->isVisible)
		{
			if (mFocusLevel >= 0 && mFocusLevel >= treeItem->level)
			{
				mFocusLevel = -1;
			}

			// Render our item
			onRenderItem(itemRect, mItems[i]);

			if (mItems[i]->ID == mFocusControl->getId())
			{
				mFocusLevel = treeItem->level;
			}

			if (mDragActive && j == mDragIndex)
			{
				dragRect = RectI(itemRect);
			}
			j++;
		}
	}
	if(mDragActive && mIsDragLegal)
	{
		onRenderDragLine(dragRect);
	}
}

void GuiTreeViewCtrl::onRenderItem(RectI& itemRect, LBItem* item)
{
	TreeItem* treeItem = dynamic_cast<TreeItem*>(item);
	SimObject* obj = static_cast<SimObject*>(item->itemData);
	if (!treeItem)
	{
		Parent::onRenderItem(itemRect, item);
		return;
	}

	Point2I cursorPt = Point2I(0, 0);
	GuiCanvas* root = getRoot();
	if (root)
	{
		cursorPt = root->getCursorPos();
	}
	bool isFocus = obj == mFocusControl;
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
	dglSetBitmapModulation(getFontColor(mProfile, currentState));
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, currentState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, currentState, mProfile);

	//indent to the focus level
	if(mFocusLevel >= 0)
	{
		contentRect.point.x += (mFocusLevel * contentRect.extent.y);
		contentRect.extent.x -= (mFocusLevel * contentRect.extent.y);

		//convert this space to a line by crushing down the sides
		S32 crush = mRound((contentRect.extent.y - 2) / 2);
		RectI line = RectI(contentRect.point.x + crush, contentRect.point.y, 2, contentRect.extent.y);
		ColorI lineColor = currentState == SelectedState ? mProfile->getFillColor(NormalState) : mProfile->getFillColor(SelectedState);
		dglDrawRectFill(line, lineColor);

		//Remove indent
		contentRect.point.x -= (mFocusLevel * contentRect.extent.y);
		contentRect.extent.x += (mFocusLevel * contentRect.extent.y);
	}

	// Indent by level
	contentRect.point.x += (treeItem->level * contentRect.extent.y);
	contentRect.extent.x -= (treeItem->level * contentRect.extent.y);

	// Render open/close triangle
	if(obj)
	{
		SimGroup* setObj = dynamic_cast<SimGroup*>(obj);
		GuiControl* guiCtrl = dynamic_cast<GuiControl*>(obj);
		bool showTriangle = (guiCtrl && guiCtrl->mIsContainer) || (!guiCtrl && setObj && setObj->size() > 0);
		if (showTriangle)
		{
			RectI drawArea = RectI(contentRect.point.x, contentRect.point.y, contentRect.extent.y, contentRect.extent.y);
			treeItem->triangleArea.set(drawArea.point, drawArea.extent);
			ColorI color = mProfile->getFontColor(currentState);
			if (isFocus)
			{
				color = mProfile->getFillColor(SelectedState);
			}
			renderTriangleIcon(drawArea, color, treeItem->isOpen ? GuiDirection::Down : GuiDirection::Right, 8);
		}
	}
	contentRect.point.x += contentRect.extent.y;
	contentRect.extent.x -= contentRect.extent.y;

	renderText(contentRect.point, contentRect.extent, item->itemText, mProfile);
}

void GuiTreeViewCtrl::onRenderDragLine(RectI& itemRect)
{
	ColorI colorW = ColorI(255,255,255,150);
	ColorI colorB = ColorI(0, 0, 0, 150);
	RectI rect = RectI(itemRect);
	rect.inset(4, 2);
	if (mReorderMethod == ReorderMethod::Insert)
	{
		dglDrawRect(itemRect, colorW);
		itemRect.inset(-1, -1);
		dglDrawRect(itemRect, colorB);
	}
	else if (mReorderMethod == ReorderMethod::Above)
	{
		itemRect.extent.y = 4;
		dglDrawRect(itemRect, colorW);
		itemRect.inset(-1, -1);
		dglDrawRect(itemRect, colorB);
	}
	else if(mReorderMethod == ReorderMethod::Below)
	{
		itemRect.point.y += mItemSize.y;
		itemRect.extent.y = 4;
		dglDrawRect(itemRect, colorW);
		itemRect.inset(-1, -1);
		dglDrawRect(itemRect, colorB);
	}
}

S32 GuiTreeViewCtrl::getHitIndex(const GuiEvent& event)
{
	Point2I localPoint = globalToLocalCoord(event.mousePoint);
	if (localPoint.y < 0)
	{
		return -1;
	}
	S32 slot = (S32)mFloor((F32)localPoint.y / (F32)mItemSize.y);
	for (S32 i = 0, j = 0; i < mItems.size(); i++)
	{
		TreeItem* treeItem = dynamic_cast<TreeItem*>(mItems[i]);
		if (treeItem && treeItem->isVisible)
		{
			if (j == slot)
			{
				S32 roundSlot = (S32)mRound((F32)localPoint.y / (F32)mItemSize.y);
				mReorderMethod = roundSlot == slot ? ReorderMethod::Above : ReorderMethod::Below;

				SimObject* obj = static_cast<SimObject*>(treeItem->itemData);
				bool showTriangle = false;
				if(obj)
				{
					SimGroup* setObj = dynamic_cast<SimGroup*>(obj);
					GuiControl* guiCtrl = dynamic_cast<GuiControl*>(obj);
					showTriangle = (guiCtrl && guiCtrl->mIsContainer) || (!guiCtrl && setObj && setObj->size() > 0);
					
					if (((slot * mItemSize.y) + 5) < localPoint.y && mReorderMethod == ReorderMethod::Above && showTriangle)
					{
						mReorderMethod = ReorderMethod::Insert;
					}
				}
				if (j == 0)
				{
					mReorderMethod = ReorderMethod::Insert; 
				}

				mIsDragLegal = true;
				for(TreeItem* trunk = treeItem; trunk != nullptr; trunk = trunk->trunk)
				{
					if (trunk->isSelected)
					{
						mIsDragLegal = false;
						break;
					}
				}

				mDragIndex = j;
				return i;
			}
			j++;
		}
	}
	return -1;
}

void GuiTreeViewCtrl::handleItemClick(LBItem* hitItem, S32 hitIndex, const GuiEvent& event)
{
	TreeItem* treeItem = dynamic_cast<TreeItem*>(hitItem);
	if (treeItem)
	{
		if (treeItem->triangleArea.pointInRect(event.mousePoint))
		{
			treeItem->isOpen = !treeItem->isOpen;
			setBranchesVisible(treeItem, treeItem->isOpen);
			return;
		}
	}

	Parent::handleItemClick(hitItem, hitIndex, event);
}

void GuiTreeViewCtrl::handleItemClick_ClickCallbacks(LBItem* hitItem, S32 hitIndex, const GuiEvent& event)
{
	TreeItem* treeItem = dynamic_cast<TreeItem*>(hitItem);
	if (!treeItem)
	{
		Parent::handleItemClick_ClickCallbacks(hitItem, hitIndex, event);
		return;
	}
		
	SimObject* obj = static_cast<SimObject*>(treeItem->itemData);
	if (hitItem == mLastClickItem && event.mouseClickCount == 2)
	{
		if (caller->isMethod("onDoubleClick"))
			Con::executef(caller, 2, "onDoubleClick", Con::getIntArg(obj->getId()));
	}
	else if (caller->isMethod("onClick"))
	{
		Con::executef(caller, 2, "onClick", Con::getIntArg(obj->getId()));
	}
}

void GuiTreeViewCtrl::inspectObject(SimObject* obj)
{
	clearItems();
	mRootObject = obj;

	StringTableEntry text = getObjectText(obj);
	S32 id = addItemWithID(text, obj->getId(), obj);
	TreeItem* treeItem = grabItemPtr(id);
	treeItem->level = 0;
	addBranches(treeItem, obj, 1);
}

void GuiTreeViewCtrl::uninspectObject()
{
	clearItems();
	mRootObject = NULL;
}

void GuiTreeViewCtrl::addBranches(TreeItem* treeItem, SimObject* obj, U16 level)
{
	SimGroup* setObj = dynamic_cast<SimGroup*>(obj);
	if(setObj)
	{
		for(auto sub : *setObj)
		{
			StringTableEntry text = getObjectText(sub);
			S32 index = addItemWithID(text, sub->getId(), sub);
			TreeItem* branch = grabItemPtr(index);
			branch->level = level;
			branch->trunk = treeItem;
			treeItem->branchList.push_back(branch);

			addBranches(branch, sub, level + 1);
		}
	}
}

GuiListBoxCtrl::LBItem* GuiTreeViewCtrl::createItem()
{
	LBItem* newItem = new TreeItem;
	if (!newItem)
	{
		return nullptr;
	}
	return newItem;
}

void GuiTreeViewCtrl::refreshTree()
{
	if (mRootObject)
	{
		Vector<U32> selectedList;
		Vector<U32> openList;
		for (auto item : mItems)
		{
			TreeItem* treeItem = dynamic_cast<TreeItem*>(item);
			if (item->isSelected)
			{
				selectedList.push_back(treeItem->ID);
			}
			if (treeItem && treeItem->isOpen)
			{
				openList.push_back(treeItem->ID);
			}
		}

		Point2I pos = Point2I::Zero;
		auto scroller = dynamic_cast<GuiScrollCtrl*>(getParent());
		if (scroller)
		{
			pos = scroller->mScrollOffset;
		}
		
		inspectObject(mRootObject);


		for (auto item : mItems)
		{
			TreeItem* treeItem = dynamic_cast<TreeItem*>(item);
			if (selectedList.contains(treeItem->ID))
			{
				item->isSelected = true;
				mSelectedItems.push_front(item);
			}
			if (treeItem && openList.contains(treeItem->ID))
			{
				treeItem->isOpen = true;
			}
			else if(treeItem)
			{
				treeItem->isOpen = false;
				setBranchesVisible(treeItem, treeItem->isOpen);
			}
		}

		if (scroller)
		{
			scroller->scrollTo(pos.x, pos.y);
		}
	}
}

StringTableEntry GuiTreeViewCtrl::getObjectText(SimObject* obj)
{
	char buffer[1024];
	if (obj)
	{
		if (isMethod("onGetObjectText"))
		{
			const char* text = Con::executef(this, 2, "onGetObjectText", Con::getIntArg(obj->getId()));
			StringTableEntry name = StringTable->insert(text, true);
			if (name != StringTable->EmptyString)
			{
				return name;
			}
		}
		const char* pObjName = obj->getName();
		const char* pInternalName = obj->getInternalName();
		if (pObjName != NULL)
			dSprintf(buffer, sizeof(buffer), "%d: %s - %s", obj->getId(), obj->getClassName(), pObjName);
		else if (pInternalName != NULL)
			dSprintf(buffer, sizeof(buffer), "%d: %s [%s]", obj->getId(), obj->getClassName(), pInternalName);
		else
			dSprintf(buffer, sizeof(buffer), "%d: %s", obj->getId(), obj->getClassName());
	}

	return StringTable->insert(buffer, true);
}

void GuiTreeViewCtrl::calculateHeaderExtent()
{
	if(mProfile)
	{
		GuiBorderProfile* topProfile = mProfile->getTopBorder();
		GuiBorderProfile* bottomProfile = mProfile->getBottomBorder();

		S32 topSize = (topProfile) ? topProfile->getMargin(NormalState) + topProfile->getBorder(NormalState) + topProfile->getPadding(NormalState) : 0;
		S32 bottomSize = (bottomProfile) ? bottomProfile->getMargin(NormalState) + bottomProfile->getBorder(NormalState) + bottomProfile->getPadding(NormalState) : 0;

		GFont* font = mProfile->getFont();
		S32 fontSize = (font) ? font->getHeight() : 0;

		S32 height = topSize + bottomSize + fontSize;
		S32 width = mBounds.extent.x;

	}
}

void GuiTreeViewCtrl::setBranchesVisible(TreeItem* treeItem, bool isVisible)
{
	for (auto branch : treeItem->branchList)
	{
		if(!isVisible || branch->isOpen)
		{
			setBranchesVisible(branch, isVisible);
		}
		branch->isVisible = isVisible;
	}
}

void GuiTreeViewCtrl::setItemOpen(S32 index, bool isOpen)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiTreeViewCtrl::setItemOpen - invalid index");
		return;
	}

	TreeItem* item = dynamic_cast<TreeItem*>(mItems[index]);
	item->isOpen = isOpen;
}

bool GuiTreeViewCtrl::getItemOpen(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf("GuiTreeViewCtrl::getItemOpen - invalid index");
		return true;
	}

	TreeItem* item = dynamic_cast<TreeItem*>(mItems[index]);
	return item->isOpen;
}

S32 GuiTreeViewCtrl::getItemTrunk(S32 index)
{
	if ((index >= mItems.size()) || index < 0)
	{
		Con::warnf(" GuiTreeViewCtrl::getItemTrunk - invalid index");
		return -1;
	}

	TreeItem* item = dynamic_cast<TreeItem*>(mItems[index]);
	
	return item->level == 0 ? -1 : getItemIndex(item->trunk);
}