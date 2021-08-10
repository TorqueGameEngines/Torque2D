#ifndef _GUIGRIDCTRL_H_
#define _GUIGRIDCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiGridCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;
	Point2I mCalcCellExt;
	Point2I mCalcCellSpace;
	U16 mCalcChainLength;
	U32 mRunningChainHeight;
	U32 mCurrentChainHeight;
	U8 mChainNumber;

public:
	enum CellMode
	{
		Absolute,
		Variable,
		Percent
	};
	CellMode mCellModeX, mCellModeY;
	F32 mCellSizeX, mCellSizeY;
	F32 mCellSpacingX, mCellSpacingY;
	S32 mMaxColCount, mMaxRowCount;

	//LRTB means Left to Right, Top to Bottom (the normal way we read english)
	enum OrderMode
	{
		LRTB,
		RLTB,
		TBLR,
		TBRL,
		LRBT,
		RLBT,
		BTLR,
		BTRL
	};
	OrderMode mOrderMode;
	bool mIsExtentDynamic;

private:
	void AdjustGrid(const Point2I& innerExtent);
	Point2F GetGridItemWidth(const S32 totalArea, const S32 maxChainLength, const F32 itemSize, const F32 spaceSize, const CellMode cellMode);
	Point2F GetGridItemHeight(const S32 totalArea, const S32 maxChainLength, const F32 itemSize, const F32 spaceSize, const CellMode cellMode);
	Point2I getCellPosition(U16 num, const Point2I &innerExtent, GuiControl *ctrl);
	Point2I getCellExtent(GuiControl *ctrl);
	inline bool IsVertical() { return mOrderMode == LRTB || mOrderMode == RLTB || mOrderMode == LRBT || mOrderMode == RLBT; }
	inline bool HasVariableChainHeight() { return (mCellModeX == Variable && !IsVertical()) || (mCellModeY == Variable && IsVertical()); }

public:
	GuiGridCtrl();

	void resize(const Point2I &newPosition, const Point2I &newExtent);
	//void childResized(GuiControl *child);
	void inspectPostApply();
	bool onWake();
	void onSleep();
	void onChildAdded(GuiControl *child);
	void onChildRemoved(SimObject *child);

	void setCellSize(F32 width, F32 height);
	inline Vector2 getCellSize(void) const { return Vector2(mCellSizeX, mCellSizeY); }
	void setCellSpacing(F32 x, F32 y);
	inline Vector2 getCellSpacing(void) const { return Vector2(mCellSpacingX, mCellSpacingY); }

	void setCellModeX(const CellMode mode);
	void setCellModeY(const CellMode mode);
	inline CellMode getCellModeX(void) const { return mCellModeX; }
	inline CellMode getCellModeY(void) const { return mCellModeY; }
	static CellMode getCellModeEnum(const char* label);
	static const char* getCellModeDescription(const CellMode mode);
	inline void setMaxColCount(S32 max) { mMaxColCount = max; resize(getPosition(), getExtent()); }
	inline void setMaxRowCount(S32 max) { mMaxRowCount = max; resize(getPosition(), getExtent()); }
	inline S32 getMaxColCount(void) const { return mMaxColCount; }
	inline S32 getMaxRowCount(void) const { return mMaxRowCount; }
	inline OrderMode getOrderMode(void) const { return mOrderMode; }
	static const char* getOrderModeDescription(const OrderMode mode);
	inline bool getIsExtentDynamic(void) { return mIsExtentDynamic; }

	static void initPersistFields();
	DECLARE_CONOBJECT(GuiGridCtrl);
};

#endif // _GUIGRIDCTRL_H_