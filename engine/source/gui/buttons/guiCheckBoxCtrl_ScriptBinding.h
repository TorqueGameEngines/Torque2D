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

ConsoleMethodGroupBeginWithDocs(GuiCheckBoxCtrl, GuiControl)

/*! Sets the state to on if true or off if false.
	@param isStateOn True if the control is on, false if off.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, setStateOn, ConsoleVoid, 3, 3, (isStateOn))
{
	object->setStateOn(dAtob(argv[2]));
}

/*! Gets the current state of the control.
	@return Returns True if on and false if off.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, getStateOn, ConsoleBool, 2, 2, ())
{
	return object->getStateOn();
}

/*! Sets the amount the button is offset from the top left corner of the control.
	@param offset The amount to offset the button.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, setBoxOffset, ConsoleVoid, 3, 3, (offset))
{
	Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
	object->setBoxOffset(kPoint);
}

/*! Sets the size of the button within the control proper.
	@param extent The size to render the button.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, setBoxExtent, ConsoleVoid, 3, 3, (extent))
{
	Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
	object->setBoxExtent(kPoint);
}

/*! Sets the amount the text is offset from the top left corner of the control.
	@param offset The amount to offset the text.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, setTextOffset, ConsoleVoid, 3, 3, (offset))
{
	Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
	object->setTextOffset(kPoint);
}

/*! Sets the size of the area the text will be rendered in within the control proper.
	@param extent The area available to render the text in.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, setTextExtent, ConsoleVoid, 3, 3, (extent))
{
	Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
	object->setTextExtent(kPoint);
}

/*! Get the offset that the button will be rendered from the top left corner of the control.
	@return The offset as a string with space-separated integers.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, getBoxOffset, ConsoleString, 2, 2, (...))
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &ext = object->getBoxOffset();
	dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
	return retBuffer;
}

/*! Get the area that the button will be rendered to within the control.
	@return The width and height of the button as a string with space-separated integers.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, getBoxExtent, ConsoleString, 2, 2, (...))
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &ext = object->getBoxExtent();
	dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
	return retBuffer;
}

/*! Get the offset that the text area will be from the top left corner of the control.
	@return The offset of the text area as a string with space-separated integers.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, getTextOffset, ConsoleString, 2, 2, (...))
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &ext = object->getTextOffset();
	dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
	return retBuffer;
}

/*! Get the area that the text will be rendered to within the control.
	@return The width and height of the text area as a string with space-separated integers.
*/
ConsoleMethodWithDocs(GuiCheckBoxCtrl, getTextExtent, ConsoleString, 2, 2, (...))
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &ext = object->getTextExtent();
	dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
	return retBuffer;
}
