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

ConsoleMethodGroupBeginWithDocs(GuiProgressCtrl, GuiControl)

/*! Sets the current progress. The smoothly animate to the given progress over the optional time or previously used time.
	@param progress The updated progress of the progress bar as a decimal value between 0 and 1.
	@param time The optional time in milliseconds it will take to animate from the progress bar's current position to the updated one.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, setProgress, ConsoleVoid, 3, 4, "( F32 progress, [S32 time] )")
{
	if (argc == 3)
	{
		object->setCurrentProgress(dAtof(argv[2]));
	}
	else if (argc == 4)
	{
		object->setCurrentProgress(dAtof(argv[2]), dAtoi(argv[3]));
	}
	else
	{
		Con::warnf("GuiProgressCtrl::setProgress() - Wrong number of parameters!");
	}
}

/*! Returns the current progress.
	@return The progress as a decimal value between 0 and 1.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, getProgress, ConsoleFloat, 2, 2, "()")
{
	return object->getCurrentProgress();
}

/*! Returns the currently displayed progress. This may be differnt if the animation to the most recently set progress is still running.
	@return The displayed progress as a decimal value between 0 and 1.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, getDisplayProgress, ConsoleFloat, 2, 2, "()")
{
	return object->getDisplayedProgress();
}

/*! Sets the progress bar back to the start.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, resetProgress, ConsoleVoid, 2, 2, "()")
{
	return object->resetProgress();
}

/*! Sets the amount of time in milliseconds that it will take to animate each time the progress is updated.
	@param time The animation time in milliseconds.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, setAnimationTime, ConsoleVoid, 3, 3, "( S32 time )")
{
	if (argc == 3)
	{
		object->setAnimationLength(dAtoi(argv[2]));
	}
	else
	{
		Con::warnf("GuiProgressCtrl::setAnimationTime() - Wrong number of parameters!");
	}
}

/*! Gets the time it takes in milliseconds for the progress bar to animate to its set progress.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiProgressCtrl, getAnimationTime, ConsoleInt, 2, 2, "()")
{
	return object->getAnimationLength();
}

ConsoleMethodGroupEndWithDocs(GuiProgressCtrl)