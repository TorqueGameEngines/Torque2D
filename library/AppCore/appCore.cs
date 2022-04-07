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

function AppCore::create( %this )
{
	// Init variables
	%this.canvasCreated = false;

    // Load system scripts
    exec("./scripts/constants.cs");
    exec("./scripts/defaultPreferences.cs");
	exec("./gui/guiProfiles.cs");
    exec("./scripts/canvas.cs");

    // Initialize the canvas
    %this.initializeCanvas("Torque2D: Rocket Edition");

	// Load other modules
    ModuleDatabase.loadGroup("gameBase");

	// Put a gui in the Canvas
	Canvas.setContent(TamlRead("./gui/defaultGui.gui.taml"));

	// Play Music - If you are using the standard Audio module
	if(isObject(Audio))
	{
		Audio.PlayMusic("planetfall");
	}
}

//-----------------------------------------------------------------------------

function AppCore::destroy( %this )
{

}
