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

ConsoleMethodGroupBeginWithDocs(GuiDragAndDropCtrl, GuiControl)

/*! Simulates a cursor down event on the control with an optional offset to be used when tracking the cursor.
	@param x The horizontal offset from the top left corner of the control. Defaults to 0.
	@param y The vertical offset from the top left corner of the control. Defaults to 0.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDragAndDropCtrl, startDragging, ConsoleVoid, 2, 4, "( [S32 x], [S32 y] )")
{
	Point2I offset = Point2I(0, 0);
	if (argc > 2)
	{
		offset.x = dAtoi(argv[2]);
	}
	if (argc > 3)
	{
		offset.y = dAtoi(argv[3]);
	}
	object->startDragging(offset);
}

ConsoleMethodGroupEndWithDocs(GuiDragAndDropCtrl)