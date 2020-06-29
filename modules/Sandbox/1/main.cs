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

function Sandbox::create( %this )
{    
    // Load the preferences.
    %this.loadPreferences();
    
    // Load Sandbox scripts.
    exec( "./scripts/console.cs" );
    exec( "./scripts/toolbox.cs" );    
    exec( "./scripts/customToolboxGui.cs" );
    exec( "./scripts/manipulation.cs" );
    exec( "./scripts/scene.cs" );
    exec( "./scripts/toys.cs" );
        
    // Load GUI profiles.
    exec("./gui/guiProfiles.cs");

    // Create the sandbox window.
    CreateSandboxWindow();
    
    // Load and configure the console.
    Sandbox.add( TamlRead("./gui/ConsoleDialog.gui.taml") );
    GlobalActionMap.bind( keyboard, "ctrl tilde", toggleConsole );
    
    // Load and configure the toolbox.
    Sandbox.add( TamlRead("./gui/ToolboxDialog.gui.taml") );

    // Load and configure the main overlay.
    Sandbox.add( TamlRead("./gui/MainOverlay.gui.taml") );
    
    // Scan for toys.
    scanForToys();

    // Initialize the toolbox.    
    initializeToolbox();
    
    // Initialize the input controller.
    Sandbox.InputController.initialize();
    
    // Initialize the "cannot render" proxy.
    new RenderProxy(CannotRenderProxy)
    {
        Image = "Sandbox:CannotRender";
    };
    Sandbox.add( CannotRenderProxy );
}

//-----------------------------------------------------------------------------

function Sandbox::destroy( %this )
{
    // Save sandbox preferences.
    %this.savePreferences();    
    
    // Unload the active toy.
    unloadToy();
    
    // Destroy the sandbox window.
    destroySandboxWindow();
    
    // Destroy the sandbox scene.
    destroySandboxScene();
}

//-----------------------------------------------------------------------------

function Sandbox::loadPreferences( %this )
{
    // Load the default preferences.
    exec( "./scripts/sandboxPreferences.cs" );
    
    // Load the last session preferences if available.
    if ( isFile("preferences.cs") )
        exec( "preferences.cs" );   
}

//-----------------------------------------------------------------------------

function Sandbox::savePreferences( %this )
{
    // Export only the sandbox preferences.
    export("$pref::Sandbox::*", "preferences.cs", false );        
    export("$pref::Video::*", "preferences.cs", true );
}
