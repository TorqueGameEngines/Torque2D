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

ConsoleMethodGroupBeginWithDocs(GuiWindowCtrl, GuiControl)

/*! Sets the profile that will be used for the content of the window
	@param p The content profile
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setContentProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlContentProfile(profile);
}

/*! Sets the profile that will be used for the close button
	@param p The close button profile
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setCloseButtonProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlCloseButtonProfile(profile);
}

/*! Sets the profile that will be used for the minimize button
	@param p The minimize button profile
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setMinButtonProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlMinButtonProfile(profile);
}

/*! Sets the profile that will be used for the maximize button
	@param p The maximize button profile
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setMaxButtonProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlMaxButtonProfile(profile);
}

/*! Sets the GuiCursor that will be used when resizing the GuiWindow
	@param c The GuiCursor that will be used when resizing
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setLeftRightCursor, ConsoleVoid, 3, 3, (GuiCursor c))
{
	GuiCursor* cursor;

	if (Sim::findObject(argv[2], cursor))
		object->setControlLeftRightCursor(cursor);
}

/*! Sets the GuiCursor that will be used when resizing the GuiWindow
	@param c The GuiCursor that will be used when resizing
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setUpDownCursor, ConsoleVoid, 3, 3, (GuiCursor c))
{
	GuiCursor* cursor;

	if (Sim::findObject(argv[2], cursor))
		object->setControlUpDownCursor(cursor);
}

/*! Sets the GuiCursor that will be used when resizing the GuiWindow
	@param c The GuiCursor that will be used when resizing
	@return No return value
*/
ConsoleMethodWithDocs(GuiWindowCtrl, setNWSECursor, ConsoleVoid, 3, 3, (GuiCursor c))
{
	GuiCursor* cursor;

	if (Sim::findObject(argv[2], cursor))
		object->setControlNWSECursor(cursor);
}

ConsoleMethodGroupEndWithDocs(GuiWindowCtrl)