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

ConsoleMethodGroupBeginWithDocs(GuiTabBookCtrl, GuiControl)

/*! Selects the active tab by index.
	@param pageIndex The zero-based index of the tab to make active.
*/
ConsoleMethodWithDocs(GuiTabBookCtrl, selectPage, ConsoleVoid, 3, 3, "(pageIndex)")
{
	S32 pageIndex = dAtoi(argv[2]);

	object->selectPage(pageIndex);
}

/*! Selects the active tab by name.
	@param pageName The name that appears on the tab to make active.
	@return No return value
*/
ConsoleMethodWithDocs(GuiTabBookCtrl, selectPageName, ConsoleVoid, 3, 3, "(pageName)")
{
	object->selectPage(argv[2]);
}

/*! Returns the currently selected page index.
	@return The index of the currently selected page.
*/
ConsoleMethodWithDocs(GuiTabBookCtrl, getSelectedPage, ConsoleInt, 2, 2, "()")
{
	return object->getSelectedPage();
}

/*! Sets the currently used TabProfile for the GuiControl
	@param p The tabprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(GuiTabBookCtrl, setTabProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlTabProfile(profile);
}