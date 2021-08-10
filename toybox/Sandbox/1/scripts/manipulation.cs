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

// Sandbox manipulation modes are:
// - Off
// - Pan
// - Pull
Sandbox.ManipulationMode = "off";

// Reset the sandbox pull object.
Sandbox.ManipulationPullMaxForce = 1000;

// Reset the touch events.
Sandbox.InputController = new ScriptObject()
{
    class = SandboxInputController;
    TouchEventCount = 0;
    TouchEventActive[0] = false;
    TouchEventActive[1] = false;
    TouchEventActive[2] = false;
    TouchEventActive[3] = false;
    TouchEventActive[4] = false;
    LastTouchId = -1;
    LatestTouchId = -1;
};

//-----------------------------------------------------------------------------

function SandboxInputController::initialize( %this )
{
    // Add sandbox touch gester as an input listener.
    SandboxWindow.addInputListener( %this );
}

//-----------------------------------------------------------------------------

function SandboxInputController::onTouchDown(%this, %touchID, %worldPosition)
{
    // Finish if the drag mode is off.
    if ( Sandbox.ManipulationMode $= "off" )
        return;
        
    // Sanity!
    if ( %this.TouchEventActive[%touchId] == true )
        return;
        
    // Calculate window position.
    %windowPosition = SandboxWindow.getWindowPoint( %worldPosition );

    // Store the new touch position.
    %this.NewTouchPosition[%touchId] = %windowPosition;
        
    // Set the old touch position as new touch position.
    %this.OldTouchPosition[%touchId] = %windowPosition;
    
    // Flag event as active.
    %this.TouchEventActive[%touchId] = true;

    // Insert the new touch Id.
    %this.PreviousTouchId = %this.CurrentTouchId;
    %this.CurrentTouchId = %touchId;

    // Increase event count.
    %this.TouchEventCount++;                               
           
    // Handle "pull" mode.
    if ( Sandbox.ManipulationMode $= "pull" )
    {
        // Reset the pull
        Sandbox.ManipulationPullObject[%touchID] = "";
        Sandbox.ManipulationPullJointId[%touchID] = "";
        
        // Pick an object.
        %picked = SandboxScene.pickPoint( %worldPosition );
        
        // Finish if nothing picked.
        if ( %picked $= "" )
            return;
        
        // Fetch the pick count.
        %pickCount = %picked.Count;
        
        for( %n = 0; %n < %pickCount; %n++ )
        {
            // Fetch the picked object.
            %pickedObject = getWord( %picked, %n );
            
            // Skip if the object is static.
            if ( %pickedObject.getBodyType() $= "static" )
                continue;
                
            // Set the pull object.
            Sandbox.ManipulationPullObject[%touchID] = %pickedObject;
            Sandbox.ManipulationPullJointId[%touchID] = SandboxScene.createTargetJoint( %pickedObject, %worldPosition, Sandbox.ManipulationPullMaxForce );            
            return;
        }
        
        return;
    }    
}

//-----------------------------------------------------------------------------

function SandboxInputController::onTouchUp(%this, %touchID, %worldPosition)
{
    // Finish if the drag mode is off.
    if ( Sandbox.ManipulationMode $= "off" )
        return;
        
    // Sanity!
    if ( %this.TouchEventActive[%touchId] == false )
        return;
        
    // Reset previous touch.
    %this.OldTouchPosition[%touchId] = "";
    
    // Reset current touch.
    %this.NewTouchPosition[%touchId] = "";
    
    // Flag event as inactive.
    %this.TouchEventActive[%touchId] = false;

    // Remove the touch Id.
    if ( %this.PreviousTouchId == %touchId )
    {
         %this.PreviousTouchId = "";
    }
    if ( %this.CurrentTouchId == %touchId )
    {
         %this.CurrentTouchId = %this.PreviousTouchId;
         %this.PreviousTouchId = "";
    }

    // Decrease event count.
    %this.TouchEventCount--;

    // Handle "pull" mode.
    if ( Sandbox.ManipulationMode $= "pull" )
    {       
        // Finish if nothing is being pulled.
        if ( !isObject(Sandbox.ManipulationPullObject[%touchID]) )
            return;
        
        // Reset the pull object.
        Sandbox.ManipulationPullObject[%touchID] = "";
        
        // Remove the pull joint.
        SandboxScene.deleteJoint( Sandbox.ManipulationPullJointId[%touchID] );
        Sandbox.ManipulationPullJointId[%touchID] = "";        
        return;
    }      
}

//-----------------------------------------------------------------------------

function SandboxInputController::onTouchDragged(%this, %touchID, %worldPosition)
{
    // Finish if the drag mode is off.
    if ( Sandbox.ManipulationMode $= "off" )
        return;

    // Sanity!
    if ( %this.TouchEventActive[%touchId] == false )
        return;
        
    // Calculate window position.
    %windowPosition = SandboxWindow.getWindowPoint( %worldPosition );

    // Set the current touch as the previous touch.
    %this.OldTouchPosition[%touchId] = %this.NewTouchPosition[%touchId];
    
    // Store the touch event.
    %this.NewTouchPosition[%touchId] = %windowPosition;
        
    // Handle "pan" mode.
    if ( Sandbox.ManipulationMode $= "pan" )
    {
        // Fetch the touch event count.
        %touchEventCount = Sandbox.InputController.TouchEventCount;
        
        // Do we have a single touch event?
        if ( %touchEventCount == 1 )
        {
            // Yes, so perform pan gesture.
            Sandbox.InputController.performPanGesture();
            
            return;
        }
        
        // Do we have two event counts?
        if ( %touchEventCount == 2 )
        {
            // Yes, so perform zoom gesture.
            Sandbox.InputController.performZoomGesture();

            return;
        }
    }
    
    // Handle "pull" mode.
    if ( Sandbox.ManipulationMode $= "pull" )
    {
        // Finish if nothing is being pulled.
        if ( !isObject(Sandbox.ManipulationPullObject[%touchID]) )
            return;
              
        // Set a new target for the target joint.
        SandboxScene.setTargetJointTarget( Sandbox.ManipulationPullJointId[%touchID], %worldPosition );
        
        return;
    }
}

//-----------------------------------------------------------------------------

function SandboxInputController::onTouchMoved(%this, %touchID, %worldPosition)
{
    // Finish if the drag mode is off.
    if ( Sandbox.ManipulationMode $= "off" )
        return;
}

//-----------------------------------------------------------------------------

function SandboxInputController::onMouseWheelUp(%this, %modifier, %mousePoint, %mouseClickCount)
{
    // Finish if the drag mode is not "pan".
    if ( !Sandbox.ManipulationMode $= "pan" )
        return;
        
    // Increase the zoom.
    SandboxWindow.setCameraZoom( SandboxWindow.getCameraZoom() + $pref::Sandbox::cameraMouseZoomRate );
}

//-----------------------------------------------------------------------------

function SandboxInputController::onMouseWheelDown(%this, %modifier, %mousePoint, %mouseClickCount)
{
    // Finish if the drag mode is not "pan".
    if ( !Sandbox.ManipulationMode $= "pan" )
        return;

    // Increase the zoom.
    SandboxWindow.setCameraZoom( SandboxWindow.getCameraZoom() - $pref::Sandbox::cameraMouseZoomRate );
}

//-----------------------------------------------------------------------------

function SandboxInputController::performPanGesture( %this )
{
    // Finish if we don't have two touch events.
    if ( %this.TouchEventCount != 1 )
        return;

    // Fetch the last touch event Id.
    %touchId = %this.CurrentTouchId;

    // Sanity!
    if ( %touchId $= "" )
        return;

    // Calculate pan offset.
    %panOffset = Vector2Sub( %this.NewTouchPosition[%touchId], %this.OldTouchPosition[%touchId] );

    // Inverse the Y offset.
    %panOffset = Vector2InverseY( %panOffset );

    // Scale the pan offset by the camera world scale.
    %panOffset = Vector2Mult( %panOffset, SandboxWindow.getCameraWorldScale() );

    // Update the camera position.
    SandboxWindow.setCameraPosition( Vector2Sub( SandboxWindow.getCameraPosition(), %panOffset ) );
}

//-----------------------------------------------------------------------------

function SandboxInputController::performZoomGesture( %this )
{
    // Finish if we don't have two touch events.
    if ( %this.TouchEventCount != 2 )
        return;

    // Fetch current and previous touch Ids.
    %currentTouchId = %this.CurrentTouchId;
    %previousTouchId = %this.PreviousTouchId;

    // Finish if we don't have touch Ids active.
    if ( !%this.TouchEventActive[%currentTouchId] || !%this.TouchEventActive[%previousTouchId] )
        return;

    %currentNewPosition = %this.NewTouchPosition[%currentTouchId];
    %currentOldPosition = %this.OldTouchPosition[%currentTouchId];
    %previousNewPosition = %this.NewTouchPosition[%previousTouchId];
    %previousOldPosition = %this.OldTouchPosition[%previousTouchId];

    // Calculate the last and current separations.
    %lastLength = Vector2Length( Vector2Abs( %currentOldPosition, %previousOldPosition ) );
    %currentLength = Vector2Length( Vector2Abs( %currentNewPosition, %previousNewPosition ) );
    
    // Calculate the change in separation length.
    %separationDelta = %currentLength - %lastLength;

    // Finish if no separation change occurred.
    if ( %separationDelta == 0 || %separationDelta $= "" )
        return;

    // Fetch the camera zoom.
    %cameraZoom =  SandboxWindow.getCameraZoom();

    // Calculate new camera zoom.
    %newCameraZoom = %cameraZoom + ( %separationDelta * $pref::Sandbox::cameraTouchZoomRate );

    // Change the zoom.
    SandboxWindow.setCameraZoom( %newCameraZoom ) ;
}

//-----------------------------------------------------------------------------

function Sandbox::resetManipulationModes( %this )
{
    // These control which drag modes are available or not.
    Sandbox.ManipulationModeState["off"] = true;
    Sandbox.ManipulationModeState["pan"] = false;
    Sandbox.ManipulationModeState["pull"] = false;
    
    // Set the sandbox drag mode default.
    Sandbox.useManipulation( "off" ); 
}

//-----------------------------------------------------------------------------

function Sandbox::resetManipulationModes( %this )
{   
    // These control which drag modes are available or not.
    Sandbox.ManipulationModeState["off"] = true;
    Sandbox.ManipulationModeState["pan"] = false;
    Sandbox.ManipulationModeState["pull"] = false;
    
    // Set the sandbox drag mode default.
    Sandbox.useManipulation( "off" ); 
}


//-----------------------------------------------------------------------------

function Sandbox::allowManipulation( %this, %mode )
{
    // Cannot turn-off the "off" manipulation.
    if ( %mode $= "off" )
        return;
        
    Sandbox.ManipulationModeState[%mode] = true;    
}

//-----------------------------------------------------------------------------

function Sandbox::useManipulation( %this, %mode )
{
    // Is the drag mode available?
    if ( %mode !$= "off" && !Sandbox.ManipulationModeState[%mode] )
        return;
    
    // Set the manipulation mode.
    Sandbox.ManipulationMode = %mode;

    // Set the current mode as text on the button.    
    if ( isObject(ManipulationModeButton) )
    {
        // Make the displayed mode more consistent.
        if ( %mode $= "off" )
            %mode = "Off";
        else if ( %mode $= "pan" )
            %mode = "Pan";
        else if ( %mode $= "pull" )
        %mode = "Pull";
        
        // Make the mode consistent when showed.
        ManipulationModeButton.Text = %mode;
    }
    
    // Reset pulled object and joint.
    Sandbox.ManipulationPullObject = "";    
    if ( Sandbox.ManipulationPullJointId !$= "" && SandboxScene.isJoint(Sandbox.ManipulationPullJointId) )
    {
        SandboxScene.deleteJoint( Sandbox.ManipulationPullJointId );
        Sandbox.ManipulationPullJointId = "";
    }        
}

//-----------------------------------------------------------------------------

function cycleManipulation( %make )
{
    // Finish if being released.
    if ( !%make )
        return;

    // "off" to "pan" transition.
    if ( Sandbox.ManipulationMode $= "off" )
    {
        if ( Sandbox.ManipulationModeState["pan"] )
        {
            Sandbox.useManipulation("pan");
            return;
        }
        
        Sandbox.ManipulationMode = "pan";
    }      
    
    // "pan" to "pull" transition.
    if ( Sandbox.ManipulationMode $= "pan" )
    {
        if ( Sandbox.ManipulationModeState["pull"] )
        {
            Sandbox.useManipulation("pull");
            return;
        }
            
        Sandbox.ManipulationMode = "pull";
    }

    // "pull" to "off" transition.
    if ( Sandbox.ManipulationMode $= "pull" )
    {
        Sandbox.useManipulation("off");
    }          
}
