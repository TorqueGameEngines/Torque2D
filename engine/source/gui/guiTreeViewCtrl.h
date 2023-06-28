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

#include "gui/guiListBoxCtrl.h"
#include <vector>

//------------------------------------------------------------------------------

class GuiTreeViewCtrl : public GuiListBoxCtrl
{
private:
	typedef GuiListBoxCtrl Parent;
	class TreeItem;

	enum class ReorderMethod { Above, Below, Insert };

protected:
	SimObjectPtr<SimObject> mRootObject;
	S32 mIndentSize;
	Point2I mTouchPoint;
	bool mDragActive;
	S32 mDragIndex;
	bool mIsDragLegal;
	ReorderMethod mReorderMethod;

public:
	GuiTreeViewCtrl();
	virtual ~GuiTreeViewCtrl();
	static void initPersistFields();

	class TreeItem : public GuiListBoxCtrl::LBItem
	{
	public:
		TreeItem() : isOpen(1), level(0), triangleArea(RectI()), isVisible(1), branchList(vector<TreeItem*>()), trunk(nullptr) { }
		virtual ~TreeItem() { }

		bool				isOpen;
		U16					level;
		RectI               triangleArea;
		bool				isVisible;
		vector<TreeItem*>	branchList;
		TreeItem*			trunk;
	};

private:
	TreeItem* grabItemPtr(S32 index);

public:
	// GuiControl
	//bool onWake();
	//void onSleep();
	//void onPreRender();
	//bool onKeyDown(const GuiEvent& event);
	void onTouchDown(const GuiEvent& event);
	//void onMiddleMouseDown(const GuiEvent& event);
	//void onTouchMove(const GuiEvent& event);
	//void onTouchEnter(const GuiEvent& event);
	//void onTouchLeave(const GuiEvent& event);
	//void onRightMouseDown(const GuiEvent& event);
	void onTouchDragged(const GuiEvent& event);
	void onTouchUp(const GuiEvent& event);

	//bool onAdd();
	void onPreRender();
	void onRender(Point2I offset, const RectI& updateRect);
	//void setControlProfile(GuiControlProfile* prof);
	//void resize(const Point2I& newPosition, const Point2I& newExtent);
	virtual void onRenderItem(RectI& itemRect, LBItem* item);
	virtual void onRenderDragLine(RectI& itemRect);

	S32 getHitIndex(const GuiEvent& event);
	virtual void handleItemClick(LBItem* hitItem, S32 hitIndex, const GuiEvent& event);
	virtual void handleItemClick_ClickCallbacks(LBItem* hitItem, S32 hitIndex, const GuiEvent& event);

	void inspectObject(SimObject* obj);
	void uninspectObject();
	void addBranches(TreeItem* treeItem, SimObject* obj, U16 level);
	void refreshTree();
	StringTableEntry getObjectText(SimObject* obj);
	void calculateHeaderExtent();
	virtual GuiListBoxCtrl::LBItem* createItem();
	void setBranchesVisible(TreeItem* treeItem, bool isVisible);
	void setItemOpen(S32 index, bool isOpen);
	bool getItemOpen(S32 index);
	S32 getItemTrunk(S32 index);

	DECLARE_CONOBJECT(GuiTreeViewCtrl);
};

#endif
