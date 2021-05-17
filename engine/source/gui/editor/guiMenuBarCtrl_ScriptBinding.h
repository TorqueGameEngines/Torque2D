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

ConsoleMethodGroupBeginWithDocs(GuiMenuBarCtrl, GuiControl)

/*! Sets the currently used BackgroundProfile for the GuiControl
	@param p The BackgroundProfile applies to the the entire screen behind the open menu.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setBackgroundProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlBackgroundProfile(profile);
}

/*! Sets the currently used MenuProfile for the GuiControl
	@param p The MenuProfile is applied to each top level menu item.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setMenuProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlMenuProfile(profile);
}

/*! Sets the currently used MenuItemProfile for the GuiControl
	@param p The MenuItemProfile is applied to each menu item.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setMenuItemProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlMenuItemProfile(profile);
}

/*! Sets the currently used MenuContentProfile for the GuiControl
	@param p The MenuContentProfile is applied the menu box that opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setMenuContentProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlMenuContentProfile(profile);
}

/*! Sets the currently used ThumbProfile for the GuiControl
	@param p The ThumbProfile is applied to the thumb of the scrollbar.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setThumbProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlThumbProfile(profile);
}

/*! Sets the currently used TrackProfile for the GuiControl
	@param p The TrackProfile is applied to the track area used by the scroll thumb.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setTrackProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlTrackProfile(profile);
}

/*! Sets the currently used ArrowProfile for the GuiControl
	@param p The ArrowProfile is applied to the arrow buttons of the scrollbar if you turn them on.
	@return No return value
*/
ConsoleMethodWithDocs(GuiMenuBarCtrl, setArrowProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlArrowProfile(profile);
}

ConsoleMethodGroupEndWithDocs(GuiMenuBarCtrl)