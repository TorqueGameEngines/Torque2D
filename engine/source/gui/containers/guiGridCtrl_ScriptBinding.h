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

ConsoleMethodGroupBeginWithDocs(GuiGridCtrl, GuiControl)

/*! Sets the size assigned to each child control.
	@param width The width of the child.
	@param height The height of the child.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setCellSize, ConsoleVoid, 3, 4, (float width, float height))
{
	F32 width, height;

	const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

	// ("width height")
	if ((elementCount == 2) && (argc == 3))
	{
		width = dAtof(Utility::mGetStringElement(argv[2], 0));
		height = dAtof(Utility::mGetStringElement(argv[2], 1));
	}

	// (width, [height])
	else if (elementCount == 1)
	{
		width = dAtof(argv[2]);

		if (argc > 3)
			height = dAtof(argv[3]);
		else
			height = width;
	}

	// Invalid
	else
	{
		Con::warnf("GuiGridCtrl::setCellSize() - Invalid number of parameters!");
		return;
	}

	// Set Position.
	object->setCellSize(width, height);
}

/*! Gets the size assigned to child controls. Results should be interpreted based on CellMode.
	@return (float width/float height) The width and height of the cells.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getCellSize, ConsoleString, 2, 2, ())
{
	return object->getCellSize().stringThis();
}

/*! Sets the spacing used between each child control. Note that if child controls use margin, there will appear to be additional spacing.
	@param x The horizontal spacing.
	@param y The vertical spacing.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setCellSpacing, ConsoleVoid, 3, 4, (float x, float y))
{
	F32 width, height;

	const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

	// ("width height")
	if ((elementCount == 2) && (argc == 3))
	{
		width = dAtof(Utility::mGetStringElement(argv[2], 0));
		height = dAtof(Utility::mGetStringElement(argv[2], 1));
	}

	// (width, [height])
	else if (elementCount == 1)
	{
		width = dAtof(argv[2]);

		if (argc > 3)
			height = dAtof(argv[3]);
		else
			height = width;
	}

	// Invalid
	else
	{
		Con::warnf("GuiGridCtrl::setCellSpacing() - Invalid number of parameters!");
		return;
	}

	// Set Position.
	object->setCellSpacing(width, height);
}

/*! Gets the spacing that will be between each child control. Results should be interpreted based on CellMode.
	@return (float x/float y) The horizontal and vertical spacing between the cells.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getCellSpacing, ConsoleString, 2, 2, ())
{
	return object->getCellSpacing().stringThis();
}

/*! Sets the cell mode in the horizontal direction.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setCellModeX, ConsoleVoid, 3, 3, (cellMode mode))
{
	// Fetch cell mode.
	const GuiGridCtrl::CellMode mode = GuiGridCtrl::getCellModeEnum(argv[2]);

	// Check for error.
	if (mode != GuiGridCtrl::Percent && mode != GuiGridCtrl::Variable && mode != GuiGridCtrl::Absolute)
		return;

	// Set body type.
	object->setCellModeX(mode);
}

/*! Sets the cell mode in the vertical direction.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setCellModeY, ConsoleVoid, 3, 3, (cellMode mode))
{
	// Fetch cell mode.
	const GuiGridCtrl::CellMode mode = GuiGridCtrl::getCellModeEnum(argv[2]);

	// Check for error.
	if (mode != GuiGridCtrl::Percent && mode != GuiGridCtrl::Variable && mode != GuiGridCtrl::Absolute)
		return;

	// Set body type.
	object->setCellModeY(mode);
}

/*! Gets the horizontal cell mode.
	@return The horizontal cell mode.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getCellModeX, ConsoleString, 2, 2, ())
{
	return GuiGridCtrl::getCellModeDescription(object->getCellModeX());
}


/*! Gets the vertical cell mode.
	@return The vertical cell mode.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getCellModeY, ConsoleString, 2, 2, ())
{
	return GuiGridCtrl::getCellModeDescription(object->getCellModeY());
}

/*! Sets the max number of columns. This will be ignored by Order Modes that end in L or R.
	@param max The max number of columns. Use zero for auto.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setMaxColCount, ConsoleVoid, 3, 3, (int max))
{
	object->setMaxColCount(dAtoi(argv[2]));
}

/*! Sets the max number of rows. This will be ignored by Order Modes that end in T or B.
	@param max The max number of rows. Use zero for auto.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiGridCtrl, setMaxRowCount, ConsoleVoid, 3, 3, (int max))
{
	object->setMaxRowCount(dAtoi(argv[2]));
}

/*! Gets the max column count.
	@return The max column count.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getMaxColCount, ConsoleInt, 2, 2, ())
{
	return object->getMaxColCount();
}

/*! Gets the max row count.
	@return The max row count.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getMaxRowCount, ConsoleInt, 2, 2, ())
{
	return object->getMaxRowCount();
}

/*! Gets the order mode.
	@return The order mode.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getOrderMode, ConsoleString, 2, 2, ())
{
	return GuiGridCtrl::getOrderModeDescription(object->getOrderMode());
}

/*! Gets if the extent will change based on the size of the contents. Expansion will happen in the direction of the final letter of the order mode.
	@return The dynamic extent.
*/
ConsoleMethodWithDocs(GuiGridCtrl, getIsExtentDynamic, ConsoleBool, 2, 2, ())
{
	return object->getIsExtentDynamic();
}

ConsoleMethodGroupEndWithDocs(GuiGridCtrl)