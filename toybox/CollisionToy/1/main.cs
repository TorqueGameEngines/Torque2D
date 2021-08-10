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

function CollisionToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
        
    // Configure the toy.
    CollisionToy.MaxBlockers = 50;
    CollisionToy.MaxBalls = 10;
    
    // Add configuration option.
    addNumericOption("Max Blockers", 1, 100, 10, "setMaxBlockers", CollisionToy.MaxBlockers, true, "Sets the number of blockers created.");
    addNumericOption("Max Balls", 1, 100, 10, "setMaxBalls", CollisionToy.MaxBalls, true, "Sets the number of balls created.");
    
    // Reset the toy.
    CollisionToy.reset();
}


//-----------------------------------------------------------------------------

function CollisionToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function CollisionToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
    
    // Create blockers.
    %this.createBlockers();
       
    // Create balls.
    %this.createBalls();   
}

//-----------------------------------------------------------------------------

function CollisionToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.BodyType = static;
       
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
    %object.BlendColor = SlateGray;
    
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 37.5 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, 37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, -34.5, 50, -34.5 );   
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );        
}

//-----------------------------------------------------------------------------

function CollisionToy::createBlockers( %this )
{
    // Create blockers.
    for( %n = 0; %n < CollisionToy.MaxBlockers; %n++ )
    {
        // Choose a uniform area.
        %sizeX = getRandom(1, 9);
        %sizeY = 10 - %sizeX;
        
        // Create sprite.
        %object = new Sprite();
        %object.BodyType = static;
        %object.Position = getRandom( -40, 40 ) SPC getRandom( -30, 30 );
        %object.Layer = 30;
        %object.Size = %sizeX SPC %sizeY;
        %object.createPolygonBoxCollisionShape();
        %object.Image = "ToyAssets:Blocks";
        %object.Frame = getRandom(0,55);
        SandboxScene.add( %object );
    }
}

//-----------------------------------------------------------------------------

function CollisionToy::createBalls( %this )
{    
    // Create balls.
    for( %n = 0; %n < CollisionToy.MaxBalls; %n++ )
    {
        // Create the sprite.
        %object = new Sprite()
        {
            class = "CollisionToyBall";
        };
        
        // Always try to configure a scene-object prior to adding it to a scene for best performance.

        // Set the position.
        %object.Position = getRandom(-30,30) SPC getRandom(-30,30);
            
        // If the size is to be square then we can simply pass a single value.
        // This applies to any 'Vector2' engine type.
        %object.Size = 3;
        
        // Set the layer.
        %object.Layer = 20;
        
        // Create a circle collision shape.
        %object.setDefaultRestitution( 1 );
        %object.setDefaultFriction( 0.1 );
        %object.createCircleCollisionShape( 1.5 );
        %object.CollisionCallback = true;
        
        // Set the sprite to use an image.  This is known as "static" image mode.
        %object.Image = "ToyAssets:Football";
        
        // We don't really need to do this as the frame is set to zero by default.
        %object.Frame = 0;

        // Set velocities.
        %object.SetLinearVelocity( getRandom(-40,40) SPC getRandom(-30,30) );
        %object.SetAngularVelocity( getRandom(-360,360) );    
            
        // Add the sprite to the scene.
        SandboxScene.add( %object );
    }
}

//-----------------------------------------------------------------------------

function CollisionToyBall::onCollision(%this, %object, %collisionDetails)
{
    // Finish if there are no contact points (this happens with sensors).
    if ( %collisionDetails.count <= 2 )
        return;

    // Fetch the first contact point (there may possibly be two but ignore that here).        
    %contactPosition = %collisionDetails._4 SPC %collisionDetails._5;
    
    // Create a marker sprite with a limited lifetime.
    // Also set this so that it can't be picked so you can't drag it via the Sandbox "pull" feature.
    %object = new Sprite();
    %object.Position = %contactPosition;
    %object.Layer = 10;   
    %object.Size = 3;
    %object.Image = "ToyAssets:Crosshair2";
    %object.BlendColor = LimeGreen;
    %object.AngularVelocity = -180;
    %object.Lifetime = 3;
    %object.PickingAllowed = false;
    SandboxScene.add( %object );
}

//-----------------------------------------------------------------------------

function CollisionToy::setMaxBlockers(%this, %value)
{
    %this.MaxBlockers = %value;
}

//-----------------------------------------------------------------------------

function CollisionToy::setMaxBalls(%this, %value)
{
    %this.MaxBalls = %value;
}