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

ConsoleMethodGroupBeginWithDocs(Sprite, SpriteBase)

/*! Sets the sprite texture flipping for each axis.
    @param flipX Whether or not to flip the texture along the x (horizontal) axis.
    @param flipY Whether or not to flip the texture along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(Sprite, setFlip, ConsoleVoid, 4, 4, (bool flipX, bool flipY))
{
    // Set Flip.
    object->setFlip( dAtob(argv[2]), dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets the flip for each axis.
    @return (bool flipX/bool flipY) Whether or not the texture is flipped along the x and y axis.
*/
ConsoleMethodWithDocs(Sprite, getFlip, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped horizontally.
    @param flipX Whether or not to flip the texture along the x (horizontal) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(Sprite, setFlipX, ConsoleVoid, 3, 3, (bool flipX))
{
    // Set Flip.
    object->setFlipX( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped vertically.
    @param flipY Whether or not to flip the texture along the y (vertical) axis.
    @return No return value.
*/
ConsoleMethodWithDocs(Sprite, setFlipY, ConsoleVoid, 3, 3, (bool flipY))
{
    // Set Flip.
    object->setFlipY( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped horizontally.
    @return (bool flipX) Whether or not the texture is flipped along the x axis.
*/
ConsoleMethodWithDocs(Sprite, getFlipX, ConsoleBool, 2, 2, ())
{
   return object->getFlipX();
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped vertically.
    @return (bool flipY) Whether or not the texture is flipped along the y axis.
*/
ConsoleMethodWithDocs(Sprite, getFlipY, ConsoleBool, 2, 2, ())
{
   return object->getFlipY();
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprite uses complex colors or not.
	@complexColor Whether complex color is on or not.
	@return No return Value.
*/
ConsoleMethodWithDocs(Sprite, setUseComplexColor, ConsoleVoid, 3, 3, (bool complexColor))
{
	// Fetch blend mode.
	const bool complex = dAtob(argv[2]);

	object->setUseComplexColor(complex);
}

//-----------------------------------------------------------------------------

/*! Gets whether the sprite uses complex colors or not.
	@return (bool complexColor) Whether complex color is on or not.
*/
ConsoleMethodWithDocs(Sprite, getUseComplexColor, ConsoleBool, 2, 2, ())
{
	return object->getUseComplexColor();
}

//-----------------------------------------------------------------------------

/*! Sets the complex color.
	@param topLeft The topLeft color space separated.
	@param topLeft The topRight color space separated.
	@param topLeft The bottomRight color space separated.
	@param topLeft The bottomLeft color space separated.
	@return No return Value.
*/
ConsoleMethodWithDocs(Sprite, setComplexColor, ConsoleVoid, 6, 6, (topLeft, topRight, bottomRight, bottomLeft))
{
	// The corners.
	ColorF corner0;
	ColorF corner1;
	ColorF corner2;
	ColorF corner3;

	// Space separated.
	if (argc == 6)
	{
		// Grab the element count.
		const U32 elementCount0 = Utility::mGetStringElementCount(argv[2]);
		if (elementCount0 == 3)
		{
			corner0 = ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)), dAtof(Utility::mGetStringElement(argv[2], 1)), dAtof(Utility::mGetStringElement(argv[2], 2)));
		}
		else if (elementCount0 == 4)
		{
			corner0 = ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)), dAtof(Utility::mGetStringElement(argv[2], 1)), dAtof(Utility::mGetStringElement(argv[2], 2)), dAtof(Utility::mGetStringElement(argv[2], 3)));
		}
		else
		{
			Con::warnf("Sprite::setComplexColor() - Top Left Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount1 = Utility::mGetStringElementCount(argv[3]);
		if (elementCount1 == 3)
		{
			corner1 = ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)), dAtof(Utility::mGetStringElement(argv[3], 2)));
		}
		else if (elementCount1 == 4)
		{
			corner1 = ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)), dAtof(Utility::mGetStringElement(argv[3], 2)), dAtof(Utility::mGetStringElement(argv[3], 3)));
		}
		else
		{
			Con::warnf("Sprite::setComplexColor() - Top Right Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount2 = Utility::mGetStringElementCount(argv[4]);
		if (elementCount2 == 3)
		{
			corner2 = ColorF(dAtof(Utility::mGetStringElement(argv[4], 0)), dAtof(Utility::mGetStringElement(argv[4], 1)), dAtof(Utility::mGetStringElement(argv[4], 2)));
		}
		else if (elementCount2 == 4)
		{
			corner2 = ColorF(dAtof(Utility::mGetStringElement(argv[4], 0)), dAtof(Utility::mGetStringElement(argv[4], 1)), dAtof(Utility::mGetStringElement(argv[4], 2)), dAtof(Utility::mGetStringElement(argv[4], 3)));
		}
		else
		{
			Con::warnf("Sprite::setComplexColor() - Bottom Right Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount3 = Utility::mGetStringElementCount(argv[5]);
		if (elementCount3 == 3)
		{
			corner3 = ColorF(dAtof(Utility::mGetStringElement(argv[5], 0)), dAtof(Utility::mGetStringElement(argv[5], 1)), dAtof(Utility::mGetStringElement(argv[5], 2)));
		}
		else if (elementCount3 == 4)
		{
			corner3 = ColorF(dAtof(Utility::mGetStringElement(argv[5], 0)), dAtof(Utility::mGetStringElement(argv[5], 1)), dAtof(Utility::mGetStringElement(argv[5], 2)), dAtof(Utility::mGetStringElement(argv[5], 3)));
		}
		else
		{
			Con::warnf("Sprite::setComplexColor() - Bottom Left Corner has Invalid Number of arguments!");
			return;
		}

		// Set blend color.
		object->setComplexColor(corner0, corner1, corner2, corner3);
	}

	// Invalid.
	else
	{
		Con::warnf("Sprite::setComplexColor() - Invalid Number of parameters!");
		return;
	}
}

//-----------------------------------------------------------------------------

/*! Gets the complex color
	@param cornerID The cornerID whose color will be returned (1 - 4).
	@return (float red / float green / float blue / float alpha) The corner blend color.
*/
ConsoleMethodWithDocs(Sprite, getComplexColor, ConsoleString, 3, 3, (cornerID))
{
	S8 cornerID = dAtoi(argv[2]);

	// Get Blend color.
	ColorF blendColor = object->getComplexColor(cornerID);

	// No, so fetch the raw color values.
	return blendColor.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Fades one corner of the sprite to the target color.
	The current color of the object will continue to change until it arrives at the target color or the fade is cancelled.
	The change will continue even if the blendColor is set directly.
	@param cornerID The cornerID whose color will be returned (1 - 4).
	@param (red / green / blue / alpha) The target color to fade the corner of the sprite to.
	@param rate The rate per second to change each color value. Must be a number greater than zero.
	@return Whether the fade started or not.
*/
ConsoleMethodWithDocs(Sprite, fadeToComplex, ConsoleBool, 5, 5, (cornerID, targetColor red / green / blue / alpha, rate))
{
	if (argc < 5)
	{
		Con::warnf("Sprite::fadeToComplex() - Invalid number of parameters!");
		return false;
	}

	const S8 cornerID = dAtoi(argv[2]);

	const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
	if (colorCount != 4)
	{
		Con::warnf("Sprite::fadeToComplex() - Invalid color! Colors require four values (red / green / blue / alpha)!");
		return false;
	}

	F32 rate = dAtof(argv[4]);
	if (rate <= 0.0f)
	{
		Con::warnf("Sprite::fadeToComplex() - Rate must be greater than zero!");
		return false;
	}

	return object->fadeToComplex(cornerID, ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)),
		dAtof(Utility::mGetStringElement(argv[3], 1)),
		dAtof(Utility::mGetStringElement(argv[3], 2)),
		dAtof(Utility::mGetStringElement(argv[3], 3))),
		rate, rate, rate, rate);
}

//-----------------------------------------------------------------------------

/*! Fades one corner of the sprite to the target color over a period of time.
	The current color of the object will continue to change until it arrives at the target color or the fade is cancelled.
	The change will continue even if the complexColors is set directly which will change the amount of time it takes.
	Unhindered, each of the color values will arrive at the target in approximately the target time.
	@param cornerID The cornerID whose color will be returned (1 - 4).
	@param (red / green / blue / alpha) The target color to fade the corner of the sprite to.
	@param time The amount of time in milliseconds that each color value will take to reach the target. Must be a number greater than zero.
	@return Whether the fade started or not.
*/
ConsoleMethodWithDocs(Sprite, fadeToTimeComplex, ConsoleBool, 5, 5, (cornerID, targetColor red / green / blue / alpha, time))
{
	if (argc < 5)
	{
		Con::warnf("Sprite::fadeToTimeComplex() - Invalid number of parameters!");
		return false;
	}

	const S8 cornerID = dAtoi(argv[2]);

	const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
	if (colorCount != 4)
	{
		Con::warnf("Sprite::fadeToTimeComplex() - Invalid color! Colors require four values (red / green / blue / alpha)!");
		return false;
	}

	F32 time = dAtof(argv[4]);
	if (time <= 0.0f)
	{
		Con::warnf("Sprite::fadeToTimeComplex() - Time must be greater than zero!");
		return false;
	}

	// Get the target color values.
	const F32 tRed = dAtof(Utility::mGetStringElement(argv[3], 0));
	const F32 tGreen = dAtof(Utility::mGetStringElement(argv[3], 1));
	const F32 tBlue = dAtof(Utility::mGetStringElement(argv[3], 2));
	const F32 tAlpha = dAtof(Utility::mGetStringElement(argv[3], 3));

	// Get the rate to change each value. The rate will be change per second.
	const ColorF currentColor = object->getComplexColor(cornerID);
	F32 rRed = (1000.0f * fabs(tRed - currentColor.red)) / time;
	F32 rGreen = (1000.0f * fabs(tGreen - currentColor.green)) / time;
	F32 rBlue = (1000.0f * fabs(tBlue - currentColor.blue)) / time;
	F32 rAlpha = (1000.0f * fabs(tAlpha - currentColor.alpha)) / time;

	return object->fadeToComplex(cornerID, ColorF(tRed, tGreen, tBlue, tAlpha), rRed, rGreen, rBlue, rAlpha);
}

ConsoleMethodGroupEndWithDocs(Sprite)
