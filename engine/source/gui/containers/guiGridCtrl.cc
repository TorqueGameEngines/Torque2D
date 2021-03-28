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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/containers/guiGridCtrl.h"

#include "guiGridCtrl_ScriptBinding.h"

static EnumTable::Enums cellModeEnums[] = 
{
	{ GuiGridCtrl::Absolute, "absolute" },
	{ GuiGridCtrl::Variable, "variable" },
	{ GuiGridCtrl::Percent, "percent" }
};
static EnumTable gCellModeTable(3, &cellModeEnums[0]);


static EnumTable::Enums orderModeEnums[] =
{
	{ GuiGridCtrl::LRTB, "lrtb" },
	{ GuiGridCtrl::RLTB, "rltb" },
	{ GuiGridCtrl::TBLR, "tblr" },
	{ GuiGridCtrl::TBRL, "tbrl" },
	{ GuiGridCtrl::LRBT, "lrbt" },
	{ GuiGridCtrl::RLBT, "rlbt" },
	{ GuiGridCtrl::BTLR, "btlr" },
	{ GuiGridCtrl::BTRL, "btrl" }
};
static EnumTable gOrderModeTable(8, &orderModeEnums[0]);

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiGridCtrl);

//------------------------------------------------------------------------------

GuiGridCtrl::GuiGridCtrl()
{
	mIsContainer = true;
	mCellModeX = CellMode::Absolute;
	mCellModeY = CellMode::Absolute;
	mCellSizeX = 20;
	mCellSizeY = 20;
	mCellSpacingX = 0;
	mCellSpacingY = 0;
	mMaxColCount = 0;
	mMaxRowCount = 0;
	mOrderMode = OrderMode::LRTB;
	mIsExtentDynamic = true;
   setField("profile", "GuiDefaultProfile");
}

//------------------------------------------------------------------------------

void GuiGridCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addField("CellModeX", TypeEnum, Offset(mCellModeX, GuiGridCtrl), 1, &gCellModeTable);
	addField("CellModeY", TypeEnum, Offset(mCellModeY, GuiGridCtrl), 1, &gCellModeTable);
	addField("CellSizeX", TypeF32, Offset(mCellSizeX, GuiGridCtrl));
	addField("CellSizeY", TypeF32, Offset(mCellSizeY, GuiGridCtrl));
	addField("CellSpacingX", TypeF32, Offset(mCellSpacingX, GuiGridCtrl));
	addField("CellSpacingY", TypeF32, Offset(mCellSpacingY, GuiGridCtrl));
	addField("MaxColCount", TypeS32, Offset(mMaxColCount, GuiGridCtrl));
	addField("MaxRowCount", TypeS32, Offset(mMaxRowCount, GuiGridCtrl));
	addField("OrderMode", TypeEnum, Offset(mOrderMode, GuiGridCtrl), 1, &gOrderModeTable);
	addField("IsExtentDynamic", TypeBool, Offset(mIsExtentDynamic, GuiGridCtrl));
}

//------------------------------------------------------------------------------

bool GuiGridCtrl::onWake()
{
	if (!Parent::onWake())
        return false;

	return true;
}

//------------------------------------------------------------------------------

void GuiGridCtrl::onSleep()
{
	Parent::onSleep();
}

//------------------------------------------------------------------------------

void GuiGridCtrl::inspectPostApply()
{
    resize(getPosition(), getExtent());
	Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void GuiGridCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	Point2I actualNewExtent = Point2I(getMax(mMinExtent.x, newExtent.x),
		getMax(mMinExtent.y, newExtent.y));

	//call set update both before and after
	setUpdate();

	RectI innerRect = getInnerRect(mBounds.point.Zero, actualNewExtent, NormalState, mProfile);
	if (!innerRect.isValidRect() && !mIsExtentDynamic)
	{
		return;
	}
		
	AdjustGrid(innerRect.extent);

	iterator i;
	U16 num = 0;
	for (i = begin(); i != end(); i++, num++)
	{
		GuiControl *ctrl = static_cast<GuiControl *>(*i);
		ctrl->resize(getCellPosition(num, innerRect.extent), mCalcCellExt);
	}

	Point2I actualNewPosition = Point2I(newPosition);
	if(mIsExtentDynamic)
	{
		F32 h = mCeil((F32)size() / (F32)mCalcChainLength);
		if (mOrderMode == LRTB || mOrderMode == RLTB)
		{
			h = (h * mCalcCellExt.y) + ((h-1) * mCalcCellSpace.y);
			innerRect.extent.y = h;
		}
		else if (mOrderMode == TBLR || mOrderMode == BTLR)
		{
			h = (h * mCalcCellExt.x) + ((h - 1) * mCalcCellSpace.x);
			innerRect.extent.x = h;
		}
		else if (mOrderMode == LRBT || mOrderMode == RLBT)
		{
			h = (h * mCalcCellExt.y) + ((h - 1) * mCalcCellSpace.y);
			actualNewPosition.y += innerRect.extent.y - h;
			innerRect.extent.y = h;
		}
		else if (mOrderMode == TBRL || mOrderMode == BTRL)
		{
			h = (h * mCalcCellExt.x) + ((h - 1) * mCalcCellSpace.x);
			actualNewPosition.x += innerRect.extent.x - h;
			innerRect.extent.x = h;
		}
		actualNewExtent = getOuterExtent(innerRect.extent, NormalState, mProfile);
	}

	mBounds.set(actualNewPosition, actualNewExtent);

	GuiControl *parent = getParent();
	if (parent)
		parent->childResized(this);
	setUpdate();
}

//------------------------------------------------------------------------------

Point2I GuiGridCtrl::getCellPosition(const U16 num, const Point2I &innerExtent)
{
	Point2I result(0,0);
	U16 y = (U16)mFloor(num / mCalcChainLength);
	U16 x = (U16)(num % mCalcChainLength);
	F32 ChainCount = mCeil((F32)size() / (F32)mCalcChainLength);

	if (mOrderMode == LRTB)
	{
		result.set(x * (mCalcCellExt.x + mCalcCellSpace.x), y * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == RLTB)
	{
		x = (mCalcChainLength - 1) - x;
		S32 delta = innerExtent.x - ((mCalcChainLength * mCalcCellExt.x) + ((mCalcChainLength - 1) * mCalcCellSpace.x));
		result.set((x * (mCalcCellExt.x + mCalcCellSpace.x)) + delta, y * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == TBLR)
	{
		result.set(y * (mCalcCellExt.x + mCalcCellSpace.x), x * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == BTLR)
	{
		x = (mCalcChainLength - 1) - x;
		S32 delta = innerExtent.y - ((mCalcChainLength * mCalcCellExt.y) + ((mCalcChainLength - 1) * mCalcCellSpace.y));
		result.set(y * (mCalcCellExt.x + mCalcCellSpace.x), (x * (mCalcCellExt.y + mCalcCellSpace.y)) + delta);
	}
	else if (mOrderMode == LRBT)
	{
		y = (ChainCount - 1) - y;
		result.set(x * (mCalcCellExt.x + mCalcCellSpace.x), y * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == RLBT)
	{
		y = (ChainCount - 1) - y;
		x = (mCalcChainLength - 1) - x;
		S32 delta = innerExtent.x - ((mCalcChainLength * mCalcCellExt.x) + ((mCalcChainLength - 1) * mCalcCellSpace.x));
		result.set((x * (mCalcCellExt.x + mCalcCellSpace.x)) + delta, y * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == TBRL)
	{
		y = (ChainCount - 1) - y;
		result.set(y * (mCalcCellExt.x + mCalcCellSpace.x), x * (mCalcCellExt.y + mCalcCellSpace.y));
	}
	else if (mOrderMode == BTRL)
	{
		y = (ChainCount - 1) - y;
		x = (mCalcChainLength - 1) - x;
		S32 delta = innerExtent.y - ((mCalcChainLength * mCalcCellExt.y) + ((mCalcChainLength - 1) * mCalcCellSpace.y));
		result.set(y * (mCalcCellExt.x + mCalcCellSpace.x), (x * (mCalcCellExt.y + mCalcCellSpace.y)) + delta);
	}

	return result;
}

//------------------------------------------------------------------------------

void GuiGridCtrl::AdjustGrid(const Point2I& innerExtent)
{
	Point2F cellExtX, cellExtY;
	if (mOrderMode == LRTB || mOrderMode == LRBT || mOrderMode == RLTB || mOrderMode == RLBT)
	{
		cellExtX = GetGridItemWidth(innerExtent.x, mMaxColCount, mCellSizeX, mCellSpacingX, mCellModeX);
		cellExtY = GetGridItemHeight(innerExtent.y, mMaxRowCount, mCellSizeY, mCellSpacingY, mCellModeY);
	}
	else
	{
		cellExtY = GetGridItemWidth(innerExtent.y, mMaxRowCount, mCellSizeY, mCellSpacingY, mCellModeY);
		cellExtX = GetGridItemHeight(innerExtent.x, mMaxColCount, mCellSizeX, mCellSpacingX, mCellModeX);
	}
	mCalcCellExt.set(cellExtX.x, cellExtY.x);
	mCalcCellSpace.set(cellExtX.y, cellExtY.y);
}

Point2F GuiGridCtrl::GetGridItemWidth(const S32 totalArea, const S32 maxChainLength, const F32 itemSize, const F32 spaceSize, const CellMode cellMode)
{
	//The two values returned are the extent and spacing held in the x and y respectively
	if (cellMode == GuiGridCtrl::Percent)
	{
		mCalcChainLength = (U16)getMax((100 + spaceSize) / (itemSize + spaceSize), 1.f);
	}
	else
	{
		mCalcChainLength = (U16)getMax((totalArea + spaceSize) / (itemSize + spaceSize), 1.f);
	}

	if (maxChainLength > 0)
	{
		mCalcChainLength = (U16)getMin((S32)mCalcChainLength, maxChainLength);
	}

	if (cellMode == GuiGridCtrl::Absolute)
	{
		return Point2F(mFloor(itemSize), mFloor(spaceSize));
	}
	else if (cellMode == GuiGridCtrl::Variable)
	{
		F32 remainder = totalArea - ((mCalcChainLength * itemSize) + ((mCalcChainLength - 1) * spaceSize));
		return Point2F(mFloor(itemSize + (remainder / mCalcChainLength)), mFloor(spaceSize));
	}
	else if (cellMode == GuiGridCtrl::Percent)
	{
		F32 calcCellSize = mFloor(getMin((F32)(totalArea * (itemSize / 100)), (F32)(totalArea)));
		F32 calcSpaceSize = mFloor(getMin((F32)(totalArea * (spaceSize / 100)), (F32)(totalArea)));
		return Point2F(calcCellSize, calcSpaceSize);
	}
	return Point2F(1,1);
}

Point2F GuiGridCtrl::GetGridItemHeight(const S32 totalArea, const S32 maxChainLength, const F32 itemSize, const F32 spaceSize, const CellMode cellMode)
{
	if (mIsExtentDynamic || cellMode == CellMode::Absolute)
	{
		return Point2F(mFloor(itemSize), mFloor(spaceSize));
	}
	else if (cellMode == CellMode::Variable)
	{
		U16 length = (U16)mClamp((totalArea + spaceSize) / (itemSize + spaceSize), 1, maxChainLength);
		F32 remainder = totalArea - ((length * itemSize) + ((length - 1) * spaceSize));
		return Point2F(mFloor(itemSize + (remainder / mCalcChainLength)), mFloor(spaceSize));
	}
	else if (cellMode == CellMode::Percent)
	{
		F32 calcCellSize = mFloor(getMin((F32)(totalArea * (itemSize / 100)), (F32)(totalArea)));
		F32 calcSpaceSize = mFloor(getMin((F32)(totalArea * (spaceSize / 100)), (F32)(totalArea)));
		return Point2F(calcCellSize, calcSpaceSize);
	}
	return Point2F(1, 1);
}

void GuiGridCtrl::onChildAdded(GuiControl *child)
{
	resize(getPosition(), getExtent());
}

void GuiGridCtrl::onChildRemoved(SimObject *child)
{
	resize(getPosition(), getExtent());
}

void GuiGridCtrl::setCellSize(F32 width, F32 height)
{
	mCellSizeX = mAbs(width);
	mCellSizeY = mAbs(height);
	resize(getPosition(), getExtent());
}

void GuiGridCtrl::setCellSpacing(F32 x, F32 y)
{
	mCellSpacingX = mAbs(x);
	mCellSpacingY = mAbs(y);
	resize(getPosition(), getExtent());
}

void GuiGridCtrl::setCellModeX(const CellMode mode)
{
	// Sanity!
	AssertFatal(mode == Percent || mode == Variable || mode == Absolute, "Invalid cell mode.");

	mCellModeX = mode;
	resize(getPosition(), getExtent());
}

void GuiGridCtrl::setCellModeY(const CellMode mode)
{
	// Sanity!
	AssertFatal(mode == Percent || mode == Variable || mode == Absolute, "Invalid cell mode.");

	mCellModeY = mode;
	resize(getPosition(), getExtent());
}

GuiGridCtrl::CellMode GuiGridCtrl::getCellModeEnum(const char* label)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(cellModeEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(cellModeEnums[i].label, label) == 0)
			return (CellMode)cellModeEnums[i].index;
	}

	// Warn.
	Con::warnf("GuiGridCtrl::getCellModeEnum() - Invalid cell mode of '%s'", label);

	return (CellMode)-1;
}

const char* GuiGridCtrl::getCellModeDescription(const GuiGridCtrl::CellMode mode)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(cellModeEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (cellModeEnums[i].index == mode)
			return cellModeEnums[i].label;
	}

	// Warn.
	Con::warnf("GuiGridCtrl::getCellModeDescription() - Invalid cell mode.");

	return StringTable->EmptyString;
}

const char* GuiGridCtrl::getOrderModeDescription(const GuiGridCtrl::OrderMode mode)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(orderModeEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (orderModeEnums[i].index == mode)
			return orderModeEnums[i].label;
	}

	// Warn.
	Con::warnf("GuiGridCtrl::getOrderModeDescription() - Invalid order mode.");

	return StringTable->EmptyString;
}