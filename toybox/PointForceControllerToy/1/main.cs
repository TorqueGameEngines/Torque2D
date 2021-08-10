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

function PointForceControllerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Initialize the toys settings.
    PointForceControllerToy.autoSpawnAsteroids = false;
    PointForceControllerToy.showPlanetoid = true;
    PointForceControllerToy.showExplosions = true;    
    PointForceControllerToy.nonLinearController = true;
    PointForceControllerToy.controllerForce = 35;
    PointForceControllerToy.controllerRadius = 36;
    PointForceControllerToy.controllerLinearDrag = 0.1;
    PointForceControllerToy.controllerAngularDrag = 0;
    PointForceControllerToy.planetoidSize = 26;
    PointForceControllerToy.asteroidSize = 4;
    PointForceControllerToy.asteroidDensity = 0.2;
    PointForceControllerToy.asteroidLifetime = 10;
    PointForceControllerToy.asteroidSpeed = 30;
    
    // Add the custom controls.
    addFlagOption("Auto Spawn Asteroids", "setAutoSpawnAsteroids", PointForceControllerToy.autoSpawnAsteroids, true, "Whether to auto-spawn asteroids or not." );
    addFlagOption("Show Planetoid", "setShowPlanetoid", PointForceControllerToy.showPlanetoid, true, "Whether to show the planetoid or not or not." );
    addFlagOption("Show Explosions", "setShowExplosions", PointForceControllerToy.showExplosions, false, "Whether to show the explosions or not or not." );
    addFlagOption("Controller Non-Linear", "setNonLinearController", PointForceControllerToy.nonLinearController, true, " Whether to apply the controller force non-linearly (using the inverse square law) or linearly" );
    addNumericOption("Controller Force", -1000, 1000, 10, "setControllerForce", PointForceControllerToy.controllerForce, true, "Sets the controller force.");
    addNumericOption("Controller Radius", 1, 30, 1, "setControllerRadius", PointForceControllerToy.controllerRadius, true, "Sets the controller radius.");
    addNumericOption("Controller Linear Drag", 0, 1, 0.1, "setControllerLinearDrag", PointForceControllerToy.controllerLinearDrag, true, "Sets the controller linear drag.");
    addNumericOption("Controller Angular Drag", 0, 1, 0.1, "setControllerAngularDrag", PointForceControllerToy.controllerAngularDrag, true, "Sets the controller angular drag.");
    addNumericOption("Planetoid Size", 1, 30, 1, "setPlanetoidSize", PointForceControllerToy.planetoidSize, true, "Sets the planetoid size.");
    addNumericOption("Asteroid Size", 1, 10, 1, "setAsteroidSize", PointForceControllerToy.asteroidSize, true, "Sets the asteroid size.");
    addNumericOption("Asteroid Density", 0.1, 10, 0.1, "setAsteroidDensity", PointForceControllerToy.asteroidDensity, true, "Sets the asteroid density.");
    addNumericOption("Asteroid Lifetime", 1, 10, 1, "setAsteroidLifetime", PointForceControllerToy.asteroidLifetime, true, "Sets the asteroid lifetime.");
    addNumericOption("Asteroid Speed", 1, 100, 1, "setAsteroidSpeed", PointForceControllerToy.asteroidSpeed, true, "Sets the asteroid speed.");
      
    // Reset the toy.
    PointForceControllerToy.reset();
}


//-----------------------------------------------------------------------------

function PointForceControllerToy::destroy( %this )
{
    // Deactivate the package.
    deactivatePackage( PointForceControllerToyPackage );    
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
    
    // Create the planetoid.
    %this.createPlanetoid();
    
    // Start a timer throwing asteroids.
    if ( PointForceControllerToy.autoSpawnAsteroids )
        %this.startTimer( "createAsteroid", 1000 );
    else
        %this.stopTimer();        
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Scroller();
    %object.BodyType = static;
    %object.Size = "200 150";
    %object.SceneLayer = 31;
    %object.Image = "ToyAssets:SkyBackground";  
    %object.ScrollX = 2;
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::createPlanetoid( %this )
{
    // Choose a position for the planetoid.
    %position = 0;
    
    if ( PointForceControllerToy.showPlanetoid )
    {
        // Create the planetoid.
        %object = new Sprite()
        {
            class = "Planetoid";
        };
        %object.Position = %position;
        %object.Size = PointForceControllerToy.planetoidSize;
        %object.Image = "ToyAssets:Planetoid";
        %object.AngularVelocity = -5;
        %object.setDefaultDensity( 10000 );
        %object.createCircleCollisionShape( PointForceControllerToy.planetoidSize * 0.48 );
        %object.CollisionCallback = true;
        SandboxScene.add( %object );
    }
    
    // Create planetoid bubble.
    %player = new ParticlePlayer();
    %player.BodyType = static;
    %player.Position = %position;
    %player.Particle = "ToyAssets:ForceBubble";
    %player.SceneLayer = 0;
    SandboxScene.add( %player );
        
    // Create a new controller.
    %controller = new PointForceController();
    %controller.setControlLayers( 10 ); // Only affect asteroids.
    %controller.Radius = PointForceControllerToy.controllerRadius;
    %controller.Force = PointForceControllerToy.ControllerForce;
    %controller.NonLinear = PointForceControllerToy.nonLinearController;
    %controller.LinearDrag = PointForceControllerToy.controllerLinearDrag;
    %controller.AngularDrag = PointForceControllerToy.controllerAngularDrag;
    SandboxScene.Controllers.add( %controller );

    if ( isObject(%object) )
        %controller.setTrackedObject( %object );
    else
        %controller.Position = %position;
    
    // This is so we can reference it in the toy, no other reason.
    PointForceControllerToy.Controller = %controller;
}

//-----------------------------------------------------------------------------

function Planetoid::onCollision( %this, %object, %collisionDetails )
{
    // Are we showing explosions?
    if ( PointForceControllerToy.showExplosions )
    {
        // Yes, so calculate position angle.
        %positionDelta = Vector2Sub( %object.Position, %this.Position );
        %angle = mAtan( %positionDelta ) - 90;
        
        // Fetch contact position.
        %contactPosition = %collisionDetails._4 SPC %collisionDetails._5;
        
        // Calculate total impact force.
        %impactForce = mAbs(%collisionDetails._6 / 100) + mAbs(%collisionDetails._7 / 20);
        
        // Create explosion.
        %player = new ParticlePlayer();
        %player.BodyType = static;
        %player.Particle = "ToyAssets:impactExplosion";
        %player.Position = %contactPosition;
        %player.Angle = %angle;
        %player.SizeScale = mClamp( %impactForce, 0.1, 10 );
        %player.SceneLayer = 0;
        SandboxScene.add( %player );            
    }
    
    // Delete the asteroid.
    %object.Trail.LinearVelocity = 0;
    %object.Trail.AngularVelocity = 0;
    %object.Trail.safeDelete();
    %object.safeDelete();  
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::createAsteroid( %this, %position )
{
    // Create an asteroid.
    %object = new Sprite();
    %object.Position = %position !$= "" ? %position : -40 SPC getRandom(-35,35);
    %object.Size = PointForceControllerToy.asteroidSize;
    %object.Image = "ToyAssets:Asteroids";
    %object.ImageFrame = getRandom(0,3);
    %object.SceneLayer = 10;
    %object.setDefaultDensity( PointForceControllerToy.asteroidDensity );
    %object.createCircleCollisionShape( PointForceControllerToy.asteroidSize * 0.4 );
    %object.setLinearVelocity( PointForceControllerToy.asteroidSpeed, 0 );
    %object.setAngularVelocity( getRandom(-90,90) );
    %object.setLifetime( PointForceControllerToy.asteroidLifetime );  
    SandboxScene.add( %object ); 
    
    // Create fire trail.
    %player = new ParticlePlayer();
    %player.Particle = "ToyAssets:bonfire";
    %player.Position = %object.Position;
    %player.EmissionRateScale = 3;
    %player.SizeScale = 2;
    %player.SceneLayer = 11;
    %player.setLifetime( PointForceControllerToy.asteroidLifetime );  
    SandboxScene.add( %player );
    %jointId = SandboxScene.createRevoluteJoint( %object, %player );
    SandboxScene.setRevoluteJointLimit( %jointId, 0, 0 );    

    %object.Trail = %player;
            
    return %object;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setAutoSpawnAsteroids( %this, %value )
{
    %this.autoSpawnAsteroids = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setShowPlanetoid( %this, %value )
{
    %this.showPlanetoid = %value;
}

//-----------------------------------------------------------------------------

function AngryBirdsSpaceToy::setShowExplosions( %this, %value )
{
    %this.showExplosions = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setNonLinearController( %this, %value )
{
    %this.nonLinearController = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setControllerForce( %this, %value )
{
    %this.controllerForce = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setControllerRadius( %this, %value )
{
    %this.controllerRadius = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setControllerLinearDrag( %this, %value )
{
    %this.controllerLinearDrag = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setControllerAngularDrag( %this, %value )
{
    %this.controllerAngularDrag = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setPlanetoidSize( %this, %value )
{
    %this.planetoidSize = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setAsteroidSize( %this, %value )
{
    %this.asteroidSize = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setAsteroidDensity( %this, %value )
{
    %this.asteroidDensity = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setAsteroidLifetime( %this, %value )
{
    %this.asteroidLifetime = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::setAsteroidSpeed( %this, %value )
{
    %this.asteroidSpeed = %value;
}

//-----------------------------------------------------------------------------

function PointForceControllerToy::onTouchDown(%this, %touchID, %worldPosition)
{
    // Create an asteroid.
    %object = PointForceControllerToy.createAsteroid( %worldPosition );
    
    if ( %worldPosition.x < PointForceControllerToy.Controller.Position.x )
        %object.setLinearVelocity( PointForceControllerToy.asteroidSpeed, 0 );
    else
        %object.setLinearVelocity( -PointForceControllerToy.asteroidSpeed, 0 );    
}
