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

ConsoleMethodGroupBeginWithDocs(GuiExpandCtrl, GuiControl)

/*! Gets if the extent has changed or is changing to fit all available children in the right or down directions.
	@return True if the extent is dynamically expanded.
*/
ConsoleMethodWithDocs(GuiExpandCtrl, getExpanded, ConsoleBool, 2, 2, ())
{
	return object->getExpanded();
}

/*! Sets if the control should collapse or expand.
	@param expanded True if the Expand should expand to fit children.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiExpandCtrl, setExpanded, ConsoleVoid, 3, 3, (bool expanded))
{
	object->setExpanded(dAtob(argv[2]));
}

/*! Sets the easing that should be used to expand or collapse. Time in milliseconds can optionally be provided.
	@param EaseFunction Name of the selected easing function to use. Defaults to Linear (none).
	@param EaseTime Optional length of time in milliseconds that it takes to expand or collapse.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiExpandCtrl, setExpandEase, ConsoleVoid, 3, 4, (EaseFunction, [EaseTime]))
{
	if (argc >= 4)
	{
		object->setAnimationLength(dAtoi(argv[3]));
	}
	object->setEasingFunction(argv[2]);
}

/*! Gets the name of the current easing function used for expanding and collapsing.
	@return Name of the current easing function.
*/
ConsoleMethodWithDocs(GuiExpandCtrl, getExpandEaseFunction, ConsoleString, 2, 2, ())
{
	return object->getEasingFunctionDescription(object->getEasingFunction());
}

/*! Gets the amount of time in milliseconds that it takes to expand or collapse.
	@return Time in milliseconds.
*/
ConsoleMethodWithDocs(GuiExpandCtrl, getExpandEaseTime, ConsoleInt, 2, 2, ())
{
	return object->getAnimationLength();
}

ConsoleMethodGroupEndWithDocs(GuiExpandCtrl)