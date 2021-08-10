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

function TumblerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Initialize the toys settings.
    TumblerToy.maxBalls = 100;
    TumblerToy.currentBalls = 0;
    TumblerToy.MotorSpeed = 10;

    // Add the custom controls.
    addNumericOption("Number of balls", 10, 200, 10, "setMaxBalls", TumblerToy.maxBalls, true, "Sets the maximum number of balls to create." );
    addNumericOption("Motor Speed", 0, 360, 10, "setMotorSpeed", TumblerToy.MotorSpeed, false, "Sets the motor angular motor speed of the tumbler." );

    // Reset the toy initially.
    TumblerToy.reset();
}


//-----------------------------------------------------------------------------

function TumblerToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function TumblerToy::reset(%this)
{   
    // Clear the scene.
    SandboxScene.clear();
    
    // Prefer the collision option off as it severely affects the performance.
    setCollisionOption( false );
    
    // Set the scene gravity.
    SandboxScene.setGravity( 0, -39.8 );
    
    // Create the tumbler.
    %this.createTumbler();

    // Reset the ball count.    
    %this.currentBalls = 0;
            
    // Start the timer.
    TumblerToy.startTimer( "createBall", 100 );
}

//-----------------------------------------------------------------------------

function TumblerToy::setMaxBalls(%this, %value)
{
    %this.maxBalls = %value;
}

//-----------------------------------------------------------------------------

function TumblerToy::setMotorSpeed(%this, %value)
{
    TumblerToy.MotorSpeed = %value;
    
    SandboxScene.setRevoluteJointMotor( TumblerToy.MotorJoint, true, TumblerToy.MotorSpeed, 1000000 );
}

//-----------------------------------------------------------------------------

function TumblerToy::createTumbler(%this)
{
    // Create the tumbler.
    %tumbler = new Sprite();
    %tumbler.Image = "ToyAssets:checkered";
    %tumbler.BlendColor = BlueViolet;
    %tumbler.Size = 120.5;
    %tumbler.setDefaultDensity( 0.1 );
    %tumbler.createPolygonBoxCollisionShape( 1, 50, 25, 0, 0 );
    %tumbler.createPolygonBoxCollisionShape( 1, 50, -25, 0, 0 );
    %tumbler.createPolygonBoxCollisionShape( 50, 1, 0, 25, 0 );
    %tumbler.createPolygonBoxCollisionShape( 50, 1, 0, -25, 0 );    
    SandboxScene.add( %tumbler );

    // Create the motor joint.    
    TumblerToy.MotorJoint = SandboxScene.createRevoluteJoint( %tumbler, "", "0 0" );
    SandboxScene.setRevoluteJointMotor( TumblerToy.MotorJoint, true, TumblerToy.MotorSpeed, 1000000 );    
}

//-----------------------------------------------------------------------------

function TumblerToy::createBall(%this)
{
    // Fetch the stock color count.
    %stockColorCount = getStockColorCount();
    
    // Create some balls.
    for ( %n = 0; %n < 5; %n++ )
    {      
        // Create the ball.
        %ball = new Sprite();
        %ball.Position = getRandom(-10,10) SPC "0";
        %ball.Size = "2";
        %ball.Image = "ToyAssets:football";        
        %ball.setDefaultRestitution( 0.6 );
        %ball.createCircleCollisionShape( 1 );
        SandboxScene.add( %ball );

        // Increase ball count.
        %this.currentBalls++;
        
        // Stop the timer if exceeded the required number of balls.
        if ( %this.currentBalls >= %this.maxBalls)
        {
            // Cancel the timer.
            TumblerToy.stopTimer();

            return;
        }
    }
}


