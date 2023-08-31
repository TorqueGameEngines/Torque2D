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

ConsoleMethodGroupBeginWithDocs(GuiColorPickerCtrl, GuiControl)

/*! Gets the current position of the selector.
	@return (int x/int y) The position of the selector as a space-separated x, y coordinate.
*/
ConsoleMethodWithDocs(GuiColorPickerCtrl, getSelectorPos, ConsoleString, 2, 2, ())
{
	char* temp = Con::getReturnBuffer(256);
	Point2I pos;
	pos = object->getSelectorPos();
	dSprintf(temp, 256, "%d %d", pos.x, pos.y);
	return temp;
}

/*! Gets the selector position for the specified color. The display mode must be Horiz Color Range, Vert Color Range, or Blend Color Range.
	@param red The red value between 0 and 1.
	@param green The green value between 0 and 1.
	@param blue The blue value between 0 and 1.
	@param alpha The alpha value between 0 and 1. Alpha values control transparency.
	@return (int x/int y) The position of the selector as a space-separated x, y coordinate.
*/
ConsoleMethodWithDocs(GuiColorPickerCtrl, getSelectorPosForColor, ConsoleString, 3, 6, (red | r/g/b/[a], green, blue, [alpha]))
{
	// The colors.
	F32 red;
	F32 green;
	F32 blue;
	F32 alpha = 1.0f;

	// Grab the element count.
	U32 elementCount = Utility::mGetStringElementCount(argv[2]);

	// Space separated.
	if (argc < 4)
	{
		// ("R G B [A]")
		if ((elementCount == 3) || (elementCount == 4))
		{
			// Extract the color.
			red = dAtof(Utility::mGetStringElement(argv[2], 0));
			green = dAtof(Utility::mGetStringElement(argv[2], 1));
			blue = dAtof(Utility::mGetStringElement(argv[2], 2));

			// Grab the alpha if it's there.
			if (elementCount > 3)
				alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
		}

		// Invalid.
		else
		{
			Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
			return StringTable->EmptyString;
		}
	}

	// (R, G, B)
	else if (argc >= 5)
	{
		red = dAtof(argv[2]);
		green = dAtof(argv[3]);
		blue = dAtof(argv[4]);

		// Grab the alpha if it's there.
		if (argc > 5)
			alpha = dAtof(argv[5]);
	}

	// Invalid.
	else
	{
		Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
		return StringTable->EmptyString;
	}

	char* temp = Con::getReturnBuffer(256);
	Point2I pos;
	pos = object->getSelectorPositionForColor(ColorF(red, green, blue, alpha));
	dSprintf(temp, 256, "%d %d", pos.x, pos.y);
	return temp;
}

/*! Set the selector by position.
	@param (x/y) The space-separated coordinate to set the selector at.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPickerCtrl, setSelectorPos, ConsoleVoid, 3, 3, (x/y))
{
	Point2I newPos;
	dSscanf(argv[2], "%d %d", &newPos.x, &newPos.y);
	object->setSelectorPos(newPos);
}

/*! Forces an update of the pick color.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiColorPickerCtrl, updateColor, ConsoleVoid, 2, 2, ())
{
	object->updateColor();
}

ConsoleMethodGroupEndWithDocs(GuiColorPickerCtrl)