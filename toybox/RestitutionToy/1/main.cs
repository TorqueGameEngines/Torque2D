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

function RestitutionToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Initialize the toys settings.
    RestitutionToy.maxBalls = 20;
    RestitutionToy.GroundWidth = 150;
    
    // Add the custom controls.
    addNumericOption("Ball Count", 1, 100, 1, "setMaxBalls", RestitutionToy.maxBalls, true, "Sets the number of balls to bounce.");

    // Reset the toy initially.
    RestitutionToy.reset();
}

//-----------------------------------------------------------------------------

function RestitutionToy::destroy( %this )
{   
}

//-----------------------------------------------------------------------------

function RestitutionToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Zoom the camera in    
    SandboxWindow.setCameraSize( 50, 37.5 );
    
    // Prefer the collision option off as it severely affects the performance.
    setCollisionOption( false );
    
    // Set the scene gravity.
    SandboxScene.setGravity( 0, -9.8 );
            
    // Create the background.
    %this.createBackground();
    
    // Create the ground.
    %this.createGround();
    
    // Create the balls.
    %this.createBalls();    
}

//-----------------------------------------------------------------------------

function RestitutionToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Stop the background from being affected by gravity.
    %object.setBodyType( "static" );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:skyBackground";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function RestitutionToy::createGround( %this )
{
    // Create the ground
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "ToyAssets:dirtGround";
    %ground.setPosition(0, -16);
    %ground.setSize(RestitutionToy.GroundWidth, 6);
    %ground.setRepeatX(RestitutionToy.GroundWidth / 60);   
    %ground.setSceneGroup( 1 );
    %ground.setCollisionGroups( none );
    %ground.createEdgeCollisionShape(RestitutionToy.GroundWidth/-2, 3, RestitutionToy.GroundWidth/2, 3);
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -12.5);
    %grass.setSize(RestitutionToy.GroundWidth, 2); 
    SandboxScene.add(%grass);       
}

//-----------------------------------------------------------------------------

function RestitutionToy::createBalls(%this)
{      
    // Calculate restitution stride.
    %restitutionStride = 1.0 / RestitutionToy.maxBalls;
    
    // Set the ball size.
    %virtualBallSize = 20 / RestitutionToy.maxBalls;
    %ballSize = 2;
    
    // Calculate ball-stack offset and stride.    
    %ballOffset = ((RestitutionToy.maxBalls-1) * %virtualBallSize * 2) * -0.5;    

    // Add a ball to each stack.
    for( %n = 0; %n < RestitutionToy.maxBalls; %n++ )
    { 
        // Calculate the stack offset.
        %stackOffset = %ballOffset + (%n * %virtualBallSize * 2);
                       
        // Create the ball.
        %ball = new Sprite();
        %ball.SetPosition( %stackOffset, 14 );
        %ball.Size = %ballSize;
        %ball.Image = "ToyAssets:Football";        
        %ball.setDefaultDensity( 0.1 );
        %ball.setDefaultRestitution( 1 - (%restitutionStride * %n) );
        %ball.setSceneGroup( 2 );
        %ball.setCollisionGroups( 1 );        
        %ball.createCircleCollisionShape( %ballSize * 0.5 );
        
        // Add to the scene.
        SandboxScene.add( %ball );
    }
}

//-----------------------------------------------------------------------------

function RestitutionToy::setMaxBalls(%this, %value)
{
    %this.maxBalls = %value;
}

