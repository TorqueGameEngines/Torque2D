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

function createSandboxWindow()
{
    // Sanity!
    if ( !isObject(SandboxWindow) )
    {
        // Create the scene window.
        new SceneWindow(SandboxWindow);

        // Set profile.
        SandboxWindow.Profile = SandboxWindowProfile;

        // Push the window.
        Canvas.setContent( SandboxWindow );
    }

    // Set camera to a canonical state.
    %allBits = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";
    SandboxWindow.stopCameraMove();
    SandboxWindow.dismount();
    SandboxWindow.setViewLimitOff();
    SandboxWindow.setRenderGroups( %allBits );
    SandboxWindow.setRenderLayers( %allBits );
    SandboxWindow.setObjectInputEventGroupFilter( %allBits );
    SandboxWindow.setObjectInputEventLayerFilter( %allBits );
    SandboxWindow.setLockMouse( true );
    SandboxWindow.setCameraPosition( 0, 0 );
    SandboxWindow.setCameraSize( 100, 75 );
    SandboxWindow.setCameraZoom( 1 );
    SandboxWindow.setCameraAngle( 0 );
}

//-----------------------------------------------------------------------------

function destroySandboxWindow()
{
    // Finish if no window available.
    if ( !isObject(SandboxWindow) )
        return;

    // Delete the window.
    SandboxWindow.delete();
}

//-----------------------------------------------------------------------------

function createSandboxScene()
{
    // Destroy the scene if it already exists.
    if ( isObject(SandboxScene) )
        destroySandboxScene();

    // Create the scene.
    new Scene(SandboxScene);

    // Sanity!
    if ( !isObject(SandboxWindow) )
    {
        error( "Sandbox: Created scene but no window available." );
        return;
    }

    // Set window to scene.
    setSceneToWindow();
}

//-----------------------------------------------------------------------------

function destroySandboxScene()
{
    // Finish if no scene available.
    if ( !isObject(SandboxScene) )
        return;

    // Delete the scene.
    SandboxScene.delete();
}

//-----------------------------------------------------------------------------

function setSceneToWindow()
{
    // Sanity!
    if ( !isObject(SandboxScene) )
    {
        error( "Cannot set Sandbox Scene to Window as the Scene is invalid." );
        return;
    }

     // Set scene to window.
    SandboxWindow.setScene( SandboxScene );

    // Set camera to a canonical state.
    %allBits = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31";
    SandboxWindow.stopCameraMove();
    SandboxWindow.dismount();
    SandboxWindow.setViewLimitOff();
    SandboxWindow.setRenderGroups( %allBits );
    SandboxWindow.setRenderLayers( %allBits );
    SandboxWindow.setObjectInputEventGroupFilter( %allBits );
    SandboxWindow.setObjectInputEventLayerFilter( %allBits );
    SandboxWindow.setLockMouse( true );
    SandboxWindow.setCameraPosition( 0, 0 );
    SandboxWindow.setCameraSize( 100, 75 );
    SandboxWindow.setCameraZoom( 1 );
    SandboxWindow.setCameraAngle( 0 );

    // Update the toolbox options.
    updateToolboxOptions();

    // reset the sandbox manipulation modes.
    Sandbox.resetManipulationModes();
}

//-----------------------------------------------------------------------------

function setCustomScene( %scene )
{
    // Sanity!
    if ( !isObject(%scene) )
    {
        error( "Cannot set Sandbox to use an invalid Scene." );
        return;
    }

    // Destroy the existing scene.
    destroySandboxScene();

    // The Sandbox needs the scene to be named this.
    %scene.setName( "SandboxScene" );

    // Set the scene to the window.
    setSceneToWindow();
}

function SceneWindow::onExtentChange(%this)
{
    %extent = Canvas.extent;
    %aspect = %extent.x / %extent.y;
    %cam = SandboxWindow.getCameraSize();
    %cam.x = %cam.y * %aspect;
    %this.setCameraSize(%cam);
}
