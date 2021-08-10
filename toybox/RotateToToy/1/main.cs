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

function RotateToToy::create( %this )
{        
    // Initialize the toys settings.
    RotateToToy.rotateSpeed = 360;
    RotateToToy.trackMouse = true;

    // Add the custom controls.
    addNumericOption("Rotate Speed", 1, 720, 1, "setRotateSpeed", RotateToToy.rotateSpeed, false, "Sets the angular speed to use to rotate to the target angle.");
    addFlagOption("Track Mouse", "setTrackMouse", RotateToToy.trackMouse, false, "Whether to track the angle to the mouse or not." );
    
    // Reset the toy initially.
    RotateToToy.reset();      
}


//-----------------------------------------------------------------------------

function RotateToToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function RotateToToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Create background.
    %this.createBackground();
    
    // Create the target.
    %this.createTarget();        
}

//-----------------------------------------------------------------------------

function RotateToToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:highlightBackground";
    
    // Set the blend color.
    %object.BlendColor = SlateGray;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function RotateToToy::createTarget( %this )
{
    // Create the sprite.
    %object = new Sprite();
    
    // Set the target.
    RotateToToy.TargetObject = %object;
    
    // Set the static image.
    %object.Image = "ToyAssets:hollowArrow";
    
    // Set a useful size.
    %object.Size = 30;
    
    // Add to the scene.
    SandboxScene.add( %object );
    
}

//-----------------------------------------------------------------------------

function RotateToToy::setRotateSpeed( %this, %value )
{
    %this.rotateSpeed = %value;
}

//-----------------------------------------------------------------------------

function RotateToToy::setTrackMouse( %this, %value )
{
    %this.trackMouse = %value;
}

//-----------------------------------------------------------------------------

function RotateToToy::onTouchDown(%this, %touchID, %worldPosition)
{
    %this.rotateTargetObject( %worldPosition );
}

//-----------------------------------------------------------------------------

function RotateToToy::onTouchDragged(%this, %touchID, %worldPosition)
{
    // Finish if not tracking the mouse.
    if ( !RotateToToy.trackMouse )
        return;
        
    %this.rotateTargetObject( %worldPosition );
}

//-----------------------------------------------------------------------------

function RotateToToy::rotateTargetObject(%this, %worldPosition)
{
    // Finish if not tracking the mouse.
    if ( !RotateToToy.trackMouse )
        return;
        
    // Calculate the angle to the mouse.
    %origin = RotateToToy.TargetObject.getPosition();
    %angle = mAtan( Vector2Sub( %worldPosition, %origin ) );
    
    // The target object points up (the 90-degree point in polar coordinates).
    // We can simply subtract 90 from the calculate angle to make the
    // object point to the mouse.
    %angle -= 90;
    
    //Rotate to the touched angle.
    RotateToToy.TargetObject.RotateTo( %angle, RotateToToy.rotateSpeed );        
}
