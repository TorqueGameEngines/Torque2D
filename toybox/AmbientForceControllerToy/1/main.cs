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

function AmbientForceControllerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Configure settings.
    AmbientForceControllerToy.ForceAngle = 45;
    AmbientForceControllerToy.ForceMagnitude = 50;
    AmbientForceControllerToy.DebrisCount = 100;

    // Add options.    
    addNumericOption("Force Angle", -359, 359, 1, "setForceAngle", AmbientForceControllerToy.ForceAngle, false, "The angle of the ambient force.");   
    addNumericOption("Force Magnitude", 0, 1000, 10, "setForceMagnitude", AmbientForceControllerToy.ForceMagnitude, false, "The magnitude of the ambient force.");   
    addNumericOption("Debris Count", 10, 1000, 10, "setDebrisCount", AmbientForceControllerToy.DebrisCount, true, "The amount of debris affected by the ambient force controller.");
    
    // Reset the toy.
    AmbientForceControllerToy.reset();
}


//-----------------------------------------------------------------------------

function AmbientForceControllerToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
    
    // Create ambient force controller.
    %this.createAmbientForceController();
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::createBackground( %this )
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
    
    // Set the scroller to use an animation!
    %object.Image = "ToyAssets:highlightBackground";
    
    // Set the blend color.
    %object.BlendColor = DarkGray;
    
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 37.5 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, 37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, -34.5, 50, -34.5 );
    
           
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::createSprite( %this, %asset, %position, %size, %angle, %blendColor )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the position.
    %object.Position = %position;

    // Set the size.        
    %object.Size = %size;
    
    // Set the angle.
    %object.Angle = %angle;
       
    // Set the scroller to use an animation!
    %object.Image = %asset;
    
    // Set the blend color.
    %object.BlendColor = %blendColor $= "" ? White : %blendColor;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
    
    return %object;
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::createAmbientForceController( %this )
{
    // Create a new controller.
    %controller = new AmbientForceController();
    
    // Set scene controller.
    AmbientForceControllerToy.SceneController = %controller;

    // Update the ambient force controller.
    %this.updateAmbientForceController();
    
    // Add the controller.
    SandboxScene.Controllers.add( %controller );
   
    // Create some sprites used by the controller.
    for( %n = 0; %n < AmbientForceControllerToy.DebrisCount; %n++ )
    {    
        %sizeX = getRandom(1,4);
        %sizeY = getRandom(1,4);
        %size = %sizeX SPC %sizeY;
        
        // Create some sprites.
        %sprite = %this.createSprite( "ToyAssets:tiles", getRandom(-40,40) SPC getRandom(-20,20), %size, getRandom(0,360), White );
        %sprite.Frame = getRandom(0,15);
        %sprite.createPolygonBoxCollisionShape( %sizeX, %sizeY );
        %sprite.setAngularVelocity(getRandom(-180,180));
        
        // Add to the controller.
        %controller.add( %sprite );
    }   
    
    // Create some sprites NOT used by the controller.
    for( %n = 0; %n < 10; %n++ )
    {    
        %size = getRandom(3,4);
        
        // Create some sprites.
        %sprite = %this.createSprite( "ToyAssets:football", getRandom(-40,40) SPC getRandom(-20,20), %size, getRandom(0,360), White );
        %sprite.createCircleCollisionShape( %size * 0.5 );
        %sprite.setAngularVelocity(getRandom(-180,180));
    }
    
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::updateAmbientForceController( %this )
{
    // Calculate the force.
    AmbientForceControllerToy.SceneController.Force = Vector2Direction( %this.ForceAngle, %this.ForceMagnitude );
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::setForceAngle(%this, %value)
{
    %this.ForceAngle = %value;
    
    // Update the ambient force.
    %this.updateAmbientForceController();   
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::setForceMagnitude(%this, %value)
{
    %this.ForceMagnitude = %value;
    
    // Update the ambient force.
    %this.updateAmbientForceController();    
}

//-----------------------------------------------------------------------------

function AmbientForceControllerToy::setDebrisCount(%this, %value)
{
    %this.DebrisCount = %value;
}
