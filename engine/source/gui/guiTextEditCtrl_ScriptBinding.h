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

ConsoleMethodGroupBeginWithDocs(GuiTextEditCtrl, GuiControl)


/*! Get the contents of the textedit control.
	@return The current text in the textedit box.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getText, ConsoleString, 2, 2, "()")
{
	if (!object->hasText())
		return StringTable->EmptyString;

	char *retBuffer = Con::getReturnBuffer(GuiTextEditCtrl::MAX_STRING_LENGTH);
	object->getText(retBuffer);

	return retBuffer;
}

/*! Gets the current position of the text cursor in the control.
	@return The current position of the text cursor in the control, where 0 is at the beginning of the line, 1 is after the first letter, and so on.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getCursorPos, ConsoleInt, 2, 2, "()")
{
	return(object->getCursorPos());
}

/*! Sets the current position of the text cursor in the control.
	@param newPos The new position to place the cursor at, where 0 is at the beginning of the line, 1 is after the first letter, and so on.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setCursorPos, ConsoleVoid, 3, 3, "( newPos )")
{
	object->reallySetCursorPos(dAtoi(argv[2]));
}

/*! Selects all the text in the control.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, selectAllText, ConsoleVoid, 2, 2, "()")
{
	object->selectAllText();
}

/*! Calls the validation function for the control. This is depreciated. Please use validate() instead.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, forceValidateText, ConsoleBool, 2, 2, "()")
{
	Con::warnf("GuiTextEditCtrl::forceValidateText() is depreciated. Please use GuiTextEditCtrl::validate() instead.");
	return object->validate();
}

/*! Calls the validation function for the control.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, validate, ConsoleBool, 2, 2, "()")
{
	return object->validate();
}

/*! Sets the returnCausesTab flag. If true, the pressing enter will attempt to tab to the next control. False will keep the focus in this control.
	@param setting True to turn on the flag. False otherwise.
	@return No return value
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setReturnCausesTab, ConsoleVoid, 3, 3, (setting))
{
	object->setReturnCausesTab(dAtob(argv[2]));
}

/*! Returns if the returnCausesTab flag is on.
	@return Returns the state of the returnCausesTab flag.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getReturnCausesTab, ConsoleBool, 2, 2, ())
{
	return object->getReturnCausesTab();
}

/*! Sets the sinkAllKeyEvents flag.
	@param setting True to turn on the flag. False otherwise.
	@return No return value
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setSinkAllKeyEvents, ConsoleVoid, 3, 3, (setting))
{
	object->setSinkAllKeyEvents(dAtob(argv[2]));
}

/*! Returns if the sinkAllKeyEvents flag is on.
	@return Returns the state of the sinkAllKeyEvents flag.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getSinkAllKeyEvents, ConsoleBool, 2, 2, ())
{
	return object->getSinkAllKeyEvents();
}

/*! Sets the password flag.
	@param setting True to turn on the flag. False otherwise.
	@return No return value
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setIsPassword, ConsoleVoid, 3, 3, (setting))
{
	object->setIsPassword(dAtob(argv[2]));
}

/*! Returns if the password flag is on.
	@return Returns the state of the password flag.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getIsPassword, ConsoleBool, 2, 2, ())
{
	return object->getIsPassword();
}

/*! Sets the maxLength which is the max number of characters that can be entered in the text edit box.
	@param max An integer value between 1 and 1024.
	@return No return value
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setMaxLength, ConsoleVoid, 3, 3, (max))
{
	object->setMaxLength(dAtoi(argv[2]));
}

/*! Returns if the current maxLength.
	@return Returns the maxLength.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getMaxLength, ConsoleBool, 2, 2, ())
{
	return object->getMaxLength();
}

/*! Sets the InputMode. Possible values include AllText, Decimal, Number, Alpha, and AlphaNumeric.
	@param mode The InputMode to use.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, setInputMode, ConsoleVoid, 3, 3, (mode))
{
	// Fetch body type.
	const GuiTextEditCtrl::InputMode mode = GuiTextEditCtrl::getInputModeEnum(argv[2]);

	// Set body type.
	object->setInputMode(mode);
}

/*! Gets the text edit InputMode.
	@return The InputMode.
*/
ConsoleMethodWithDocs(GuiTextEditCtrl, getInputMode, ConsoleString, 2, 2, ())
{
	return GuiTextEditCtrl::getInputModeDescription(object->getInputMode());
}

ConsoleMethodGroupEndWithDocs(GuiTextEditCtrl)