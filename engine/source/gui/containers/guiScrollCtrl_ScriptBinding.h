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

ConsoleMethodGroupBeginWithDocs(GuiScrollCtrl, GuiControl)

/*! Scrolls the content area to the top.
	@return No return value.
	@sa scrollToBottom
	@sa scrollToLeft
	@sa scrollToRight
	@sa scrollToPosition
*/
ConsoleMethodWithDocs(GuiScrollCtrl, scrollToTop, void, 2, 2, "()")
{
	object->scrollTo(object->mScrollOffset.x, 0);
}

/*! Scrolls the content area to the bottom.
	@return No return value.
	@sa scrollToTop
	@sa scrollToLeft
	@sa scrollToRight
	@sa scrollToPosition
*/
ConsoleMethodWithDocs(GuiScrollCtrl, scrollToBottom, void, 2, 2, "()")
{
	object->scrollTo(object->mScrollOffset.x, 0x7FFFFFFF);
}

/*! Scrolls the content area to the left.
	@return No return value.
	@sa scrollToTop
	@sa scrollToBottom
	@sa scrollToRight
	@sa scrollToPosition
*/
ConsoleMethodWithDocs(GuiScrollCtrl, scrollToLeft, void, 2, 2, "()")
{
	object->scrollTo(0, object->mScrollOffset.y);
}

/*! Scrolls the content area to the right.
	@return No return value.
	@sa scrollToTop
	@sa scrollToBottom
	@sa scrollToLeft
	@sa scrollToPosition
*/
ConsoleMethodWithDocs(GuiScrollCtrl, scrollToRight, void, 2, 2, "()")
{
	object->scrollTo(0x7FFFFFFF, object->mScrollOffset.x);
}

/*! Scrolls the content area to the specified position.
	@param x The horizontal target position.
	@param y The vertical target position.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiScrollCtrl, setScrollPosition, void, 4, 4, "(x, y)")
{
	object->scrollTo(dAtoi(argv[2]), dAtoi(argv[3]));
}

/*! Gets the current horizontal scroll position.
	@return The position of the content in the horizontal direction.
*/
ConsoleMethodWithDocs(GuiScrollCtrl, getScrollPositionX, S32, 2, 2, "()")
{
	return object->mScrollOffset.x;
}

/*! Gets the current vertical scroll position.
	@return The position of the content in the vertical direction.
*/
ConsoleMethodWithDocs(GuiScrollCtrl, getScrollPositionY, S32, 2, 2, "()")
{
	return object->mScrollOffset.y;
}

/*! Refreshes all the contents of the scroll container.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiScrollCtrl, computeSizes, void, 2, 2, "()")
{
	object->computeSizes();
}

/*! Sets the currently used ThumbProfile for the GuiControl
	@param p The Thumbprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(GuiScrollCtrl, setThumbProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlThumbProfile(profile);
}

/*! Sets the currently used TrackProfile for the GuiControl
	@param p The Trackprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(GuiScrollCtrl, setTrackProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlTrackProfile(profile);
}

/*! Sets the currently used ArrowProfile for the GuiControl
	@param p The Arrowprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(GuiScrollCtrl, setArrowProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlArrowProfile(profile);
}

ConsoleMethodGroupEndWithDocs(GuiScrollCtrl)