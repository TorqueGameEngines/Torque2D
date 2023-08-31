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

ConsoleMethodGroupBeginWithDocs(GuiColorPopupCtrl, GuiControl)

/*! Opens the popup. Does nothing if the popup is already open.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, openColorPopup, ConsoleVoid, 2, 2, "()")
{
	object->openColorPopup();
}

/*! Closes the popup. Does nothing if the popup is already closed.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, closeColorPopup, ConsoleVoid, 2, 2, "()")
{
	object->closeColorPopup();
}

/*! Sets the currently used BackgroundProfile for the GuiControl
	@param p The BackgroundProfile applies to the the entire screen when the list opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setBackgroundProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlBackgroundProfile(profile);
}

/*! Sets the currently used PopupProfile for the GuiControl
	@param p The PopupProfile applies to the the entire screen when the list opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setPopupProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlPopupProfile(profile);
}

/*! Sets the currently used PickerProfile for the GuiControl
	@param p The PickerProfile applies to the the entire screen when the list opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setPickerProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlPickerProfile(profile);
}

/*! Sets the currently used SelectorProfile for the GuiControl
	@param p The SelectorProfile applies to the the entire screen when the list opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setSelectorProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlSelectorProfile(profile);
}

ConsoleMethodGroupEndWithDocs(GuiColorPopupCtrl)