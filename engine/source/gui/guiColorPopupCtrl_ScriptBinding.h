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

/*! Gets the selected color as floating point values.
	@return The selected color as four floating point values between 0 and 1.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, getColorF, ConsoleString, 2, 2, ())
{
	ColorF color = object->getValue();

	// Format stock color.
	char* returnBuffer = Con::getReturnBuffer(256);
	dSprintf(returnBuffer, 256, "%g %g %g %g", color.red, color.green, color.blue, color.alpha);
	return(returnBuffer);
}

/*! Gets the selected color as integer values.
	@return The selected color as four integer values between 0 and 255.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, getColorI, ConsoleString, 2, 2, ())
{
	const ColorI& color = object->getValue();

	// Format stock color.
	char* returnBuffer = Con::getReturnBuffer(256);
	dSprintf(returnBuffer, 256, "%d %d %d %d", color.red, color.green, color.blue, color.alpha);
	return(returnBuffer);
}

/*! Sets the base color displayed using floating point values.
	@param color The color to display as values between 0 and 1.0.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setColorF, ConsoleVoid, 3, 3, "(color red / green / blue / [alpha])")
{
	if (argc == 3)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiColorPopupCtrl::setColorF() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			return;
		}

		F32 red, green, blue, alpha;

		red = dAtof(Utility::mGetStringElement(argv[2], 0));
		green = dAtof(Utility::mGetStringElement(argv[2], 1));
		blue = dAtof(Utility::mGetStringElement(argv[2], 2));
		alpha = colorCount > 3 ? dAtof(Utility::mGetStringElement(argv[2], 3)) : 1.0;

		ColorF color = ColorF(red, green, blue, alpha);
		object->setColor(color);
	}
	else
	{
		Con::warnf("GuiColorPopupCtrl::setColorF() - Invalid number of parameters!");
	}
}

/*! Sets the base color displayed using integer values.
	@param color The color to display as values between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPopupCtrl, setColorI, ConsoleVoid, 3, 3, "(color red / green / blue / [alpha])")
{
	if (argc == 3)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiColorPopupCtrl::setColorI() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			return;
		}

		S32 red, green, blue, alpha;

		red = dAtoi(Utility::mGetStringElement(argv[2], 0));
		green = dAtoi(Utility::mGetStringElement(argv[2], 1));
		blue = dAtoi(Utility::mGetStringElement(argv[2], 2));
		alpha = colorCount > 3 ? dAtoi(Utility::mGetStringElement(argv[2], 3)) : 255;

		ColorF color = ColorI(red, green, blue, alpha);
		object->setColor(color);
	}
	else
	{
		Con::warnf("GuiColorPopupCtrl::setColorI() - Invalid number of parameters!");
	}
}

ConsoleMethodGroupEndWithDocs(GuiColorPopupCtrl)