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

ConsoleMethodGroupBeginWithDocs(GuiFrameSetCtrl, GuiControl)

/*! Splits the frame with the frameID given into two frames with one on top and one on bottom. The existing control will move to the first frame.
	@param frameID The frameID that should be split.
	@return (int frameID1/int frameID2) The resulting two frameIDs seperated by a space.
*/
ConsoleMethodWithDocs(GuiFrameSetCtrl, createVerticalSplit, ConsoleString, 3, 3, (int frameID))
{
	Point2I point = object->splitFrame(dAtoi(argv[2]), true);
	char* retBuffer = Con::getReturnBuffer(64);
	dSprintf(retBuffer, 64, "%d %d", point.x, point.y);

	return retBuffer;
}

/*! Splits the frame with the frameID given into two frames with one on the left and one on the right. The existing control will move to the first frame.
	@param frameID The frameID that should be split.
	@return (int frameID1/int frameID2) The resulting two frameIDs seperated by a space.
*/
ConsoleMethodWithDocs(GuiFrameSetCtrl, createHorizontalSplit, ConsoleString, 3, 3, (int frameID))
{
	Point2I point = object->splitFrame(dAtoi(argv[2]), false);
	char* retBuffer = Con::getReturnBuffer(64);
	dSprintf(retBuffer, 64, "%d %d", point.x, point.y);

	return retBuffer;
}

/*! This anchors a frame and unachors it's twin. An anchored frame tries to stay the same size when its parent resizes. It can still be resized by the user.
	@param frameID The frameID that should be anchored.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiFrameSetCtrl, anchorFrame, ConsoleVoid, 3, 3, (int frameID))
{
	object->anchorFrame(dAtoi(argv[2]));
}

/*! Sets the width or height of a frame depending on if its parent is split horizontally or vertically.
	@param frameID The frameID that should be set.
	@param size The size in pixels to set the width or height to.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiFrameSetCtrl, setFrameSize, ConsoleVoid, 4, 4, (int frameID, int size))
{
	object->setFrameSize(dAtoi(argv[2]), dAtoi(argv[3]));
}

ConsoleMethodGroupEndWithDocs(GuiFrameSetCtrl)