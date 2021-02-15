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

ConsoleMethodGroupBeginWithDocs(GuiButtonCtrl, GuiControl)

/*! Sets the easing that should be used to change the fill color to the highlight color. Time in milliseconds can optionally be provided.
	@param EaseFunction Name of the selected easing function to use. Defaults to Linear (none).
	@param EaseTime Optional length of time in milliseconds that it takes to change.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, setFillColorHLEase, ConsoleVoid, 3, 4, (EaseFunction, [EaseTime]))
{
	if (argc >= 4)
	{
		object->mEaseTimeFillColorHL = dAtoi(argv[3]);
	}
	EasingFunction f = Linear;
	for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(easingEnums[i].label, argv[2]) == 0)
			f = (EasingFunction)easingEnums[i].index;
	}
	object->mEaseFillColorHL = f;
}

/*! Sets the easing that should be used to change the fill color to the selected color. Time in milliseconds can optionally be provided.
	@param EaseFunction Name of the selected easing function to use. Defaults to Linear (none).
	@param EaseTime Optional length of time in milliseconds that it takes to change.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, setFillColorSLEase, ConsoleVoid, 3, 4, (EaseFunction, [EaseTime]))
{
	if (argc >= 4)
	{
		object->mEaseTimeFillColorSL = dAtoi(argv[3]);
	}
	EasingFunction f = Linear;
	for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(easingEnums[i].label, argv[2]) == 0)
			f = (EasingFunction)easingEnums[i].index;
	}
	object->mEaseFillColorSL = f;
}

/*! Gets the name of the current easing function used to change the fill color to the highlight color.
	@return Name of the current easing function.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, getFillColorHLEaseFunction, ConsoleString, 2, 2, ())
{
	return object->getEasingFunctionDescription(object->mEaseFillColorHL);
}

/*! Gets the name of the current easing function used to change the fill color to the select color.
	@return Name of the current easing function.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, getFillColorSLEaseFunction, ConsoleString, 2, 2, ())
{
	return object->getEasingFunctionDescription(object->mEaseFillColorSL);
}

/*! Gets the amount of time in milliseconds that it takes to change fill color to the highlight color.
	@return Time in milliseconds.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, getFillColorHLEaseTime, ConsoleInt, 2, 2, ())
{
	return object->mEaseTimeFillColorHL;
}

/*! Gets the amount of time in milliseconds that it takes to change fill color to the selected color.
	@return Time in milliseconds.
*/
ConsoleMethodWithDocs(GuiButtonCtrl, getFillColorSLEaseTime, ConsoleInt, 2, 2, ())
{
	return object->mEaseTimeFillColorSL;
}

ConsoleMethodGroupEndWithDocs(GuiButtonCtrl)