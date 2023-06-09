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

#include "guiTreeViewCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiTreeViewCtrl);

GuiTreeViewCtrl::GuiTreeViewCtrl()
{
	mActive = true;
	mIsContainer = false;
	mIndentSize = 10;
}

GuiTreeViewCtrl::~GuiTreeViewCtrl()
{
}

GuiTreeViewCtrl::TreeItem* GuiTreeViewCtrl::grabItemPtr(S32 index)
{
	// Range Check
	if (index >= mItems.size() || index < 0)
	{
		Con::warnf("GuiListBoxCtrl::grabItemPtr - index out of range!");
		return nullptr;
	}

	// Grab our item
	return dynamic_cast<GuiTreeViewCtrl::TreeItem*>(mItems[index]);
}

void GuiTreeViewCtrl::initPersistFields()
{
	Parent::initPersistFields();
}

void GuiTreeViewCtrl::onPreRender()
{
	
}

void GuiTreeViewCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	RectI clip = dglGetClipRect();
	if (mFitParentWidth && (mBounds.extent.x != clip.extent.x || mItemSize.x != clip.extent.x))
	{
		mBounds.extent.x = clip.extent.x;
		mItemSize.x = clip.extent.x;
	}

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
			// Render our item
			onRenderItem(itemRect, mItems[i]);
			j++;
		}
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

	// Indent by level
	contentRect.point.x += (treeItem->level * contentRect.extent.y);
	contentRect.extent.x -= (treeItem->level * contentRect.extent.y);

	// Render open/close triangle
	if(obj)
	{
		SimSet* setObj = dynamic_cast<SimSet*>(obj);
		GuiControl* guiCtrl = dynamic_cast<GuiControl*>(obj);
		bool showTriangle = (guiCtrl && guiCtrl->mIsContainer) || (!guiCtrl && setObj && setObj->size() > 0);
		if (showTriangle)
		{
			RectI drawArea = RectI(contentRect.point.x, contentRect.point.y, contentRect.extent.y, contentRect.extent.y);
			treeItem->triangleArea.set(drawArea.point, drawArea.extent);
			ColorI color = mProfile->getFontColor(currentState);
			renderTriangleIcon(drawArea, color, treeItem->isOpen ? GuiDirection::Down : GuiDirection::Right, 8);
		}
	}
	contentRect.point.x += contentRect.extent.y;
	contentRect.extent.x -= contentRect.extent.y;

	renderText(contentRect.point, contentRect.extent, item->itemText, mProfile);
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
				return i;
			}
			j++;
		}
	}
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

void GuiTreeViewCtrl::addBranches(TreeItem* treeItem, SimObject* obj, U16 level)
{
	SimSet* setObj = dynamic_cast<SimSet*>(obj);
	if(setObj)
	{
		for(auto sub : *setObj)
		{
			StringTableEntry text = getObjectText(sub);
			S32 id = addItemWithID(text, sub->getId(), sub);
			TreeItem* branch = grabItemPtr(id);
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
		inspectObject(mRootObject);
	}
}

StringTableEntry GuiTreeViewCtrl::getObjectText(SimObject* obj)
{
	char buffer[1024];
	if (obj)
	{
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