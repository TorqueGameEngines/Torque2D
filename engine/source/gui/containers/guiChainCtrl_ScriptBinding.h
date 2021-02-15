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

ConsoleMethodGroupBeginWithDocs(GuiChainCtrl, GuiControl)

/*! Sets the spacing used between each child control. Note that if child controls use margin, there will appear to be additional spacing.
	@param spacing The spacing to be used.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiChainCtrl, setChildSpacing, ConsoleVoid, 3, 3, (int spacing))
{
	F32 spacing;

	if (argc > 2)
	{
		spacing = dAtoi(argv[2]);
	}
	else
	{
		Con::warnf("GuiChainCtrl::setChildSpacing() - Invalid number of parameters!");
		return;
	}

	// Set Position.
	object->setChildSpacing(spacing);
}

/*! Gets the spacing that will be between each child control.
	@return (int spacing) The spacing between each child.
*/
ConsoleMethodWithDocs(GuiChainCtrl, getChildSpacing, ConsoleInt, 2, 2, ())
{
	return object->getChildSpacing();
}

/*! Returns true if the chain of children are arranged vertically, false if they are arranged horizontally.
	@return The direction of the chain.
*/
ConsoleMethodWithDocs(GuiChainCtrl, getIsVertical, ConsoleBool, 2, 2, ())
{
	return object->getIsVertical();
}

ConsoleMethodGroupEndWithDocs(GuiChainCtrl)