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

// Set log mode.
setLogMode(2);

// Set profiler.
//profilerEnable( true );

// Controls whether the execution or script files or compiled DSOs are echoed to the console or not.
// Being able to turn this off means far less spam in the console during typical development.
setScriptExecEcho( false );

// Controls whether all script execution is traced (echoed) to the console or not.
trace( false );

// Sets whether to ignore compiled TorqueScript files (DSOs) or not.
$Scripts::ignoreDSOs = true;

// The name of the company. Used to form the path to save preferences. Defaults to GarageGames
// if not specified.
// The name of the game. Used to form the path to save preferences. Defaults to C++ engine define TORQUE_GAME_NAME
// if not specified.
// Appending version string to avoid conflicts with existing versions and other versions.
setCompanyAndProduct("GarageGames", "Torque2D" );

// Set module database information echo.
ModuleDatabase.EchoInfo = false;

// Set asset database information echo.
AssetDatabase.EchoInfo = false;

// Set the asset manager to ignore any auto-unload assets.
// This cases assets to stay in memory unless assets are purged.
AssetDatabase.IgnoreAutoUnload = true;

// Scan modules in your game. This defaults to the toybox for now. You should make an empty game folder, move some modules into it from the library, and change this to scan it.
ModuleDatabase.scanModules( "./toybox" );

// You'll need to load a starting module for your game. This will likely be the AppCore.
ModuleDatabase.LoadExplicit( "AppCore" );

// Starts the editor.
exec("./editor/main.cs");

//-----------------------------------------------------------------------------

function onExit()
{
    // Unload the AppCore or EditorCore modules.
    EditorManager.unloadGroup( "EditorGroup" );
    ModuleDatabase.unloadExplicit( "AppCore" );
}

function androidBackButton(%val)
{
	if (%val) {
		//Add code here for other options the back button can do like going back a screen.  the quit should happen at your main menu.

		quit();
	}

}
