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

function TruckToy::create( %this )
{        
    TruckToy.ObstacleFriction = 1.5;
    TruckToy.CameraWidth = 20;
    TruckToy.CameraHeight = 15;
    TruckToy.WorldWidth = TruckToy.CameraWidth * 10;
    TruckToy.WorldLeft = TruckToy.WorldWidth * -0.5;
    TruckToy.WorldRight = TruckToy.WorldWidth * 0.5;
    TruckToy.FloorLevel = -4.5;
    TruckToy.BackdropDomain = 31;
    TruckToy.BackgroundDomain = 25;
    TruckToy.TruckDomain = 20;
    TruckToy.GroundDomain = 18;
    TruckToy.ObstacleDomain = 15;
    TruckToy.ProjectileDomain = 16;
    TruckToy.ForegroundDomain = 10;    

    TruckToy.WheelSpeed = 400;
    TruckToy.WheelFriction = 1;
    TruckToy.FrontWheelDensity = 6;
    TruckToy.RearWheelDensity = 3;
    TruckToy.FrontWheelDrive = true;
    TruckToy.RearWheelDrive = true;
    TruckToy.ProjectileRate = 3000;
    TruckToy.ExplosionScale = 1;
    
    TruckToy.RotateCamera = true;
    
    // Add the custom controls.
    addNumericOption( "Wheel Speed", 100, 1000, 50, "setWheelSpeed", TruckToy.WheelSpeed, false, "Sets the rotational speed of the wheel when it is put into drive." );
    addNumericOption( "Wheel Friction", 0, 10, 1, "setWheelFriction", TruckToy.WheelFriction, true, "Sets the friction for the surface of each wheel." );
    addNumericOption( "Front Wheel Density", 1, 20, 1, "setFrontWheelDensity", TruckToy.FrontWheelDensity, true, "Sets the density of the front wheel." );
    addNumericOption( "Rear Wheel Density", 1, 20, 1, "setFrontWheelDensity", TruckToy.RearWheelDensity, true, "Sets the density of the rear wheel." );
    addNumericOption( "Projectile Rate (ms)", 100, 60000, 100, "setProjectileRate", TruckToy.ProjectileRate, false, "Sets the time interval in-between projectiles appearing." );
    addNumericOption( "Explosion Scale", 1, 11, 1, "setExplosionScale", TruckToy.ExplosionScale, false, "Sets the size scale of the explosions caused by a projectile landing." );
    addFlagOption("Front Wheel Drive", "setFrontWheelDrive", TruckToy.FrontWheelDrive, false, "Whether the motor on the front wheel is active or not." );
    addFlagOption("Rear Wheel Drive", "setRearWheelDrive", TruckToy.RearWheelDrive, false, "Whether the motor on the rear wheel is active or not." );
    addFlagOption("Rotate Camera", "setRotateCamera", TruckToy.RotateCamera, true, "Whether the rotate the camera that is mounted to the truck or not." );

	Canvas.setCursor(DefaultCursor);

    // Reset the toy.
    %this.reset();
	
}

//-----------------------------------------------------------------------------

function TruckToy::destroy( %this )
{
    // Deactivate the package.
    deactivatePackage( TruckToyPackage );
}

//-----------------------------------------------------------------------------

function TruckToy::reset( %this )
{   
    // Clear the scene.
    SandboxScene.clear();    
    
    // Set a typical Earth gravity.
    SandboxScene.setGravity( 0, -9.8 );  

    // Camera Configuration
    SandboxWindow.setCameraPosition( TruckToy.WorldLeft + (TruckToy.CameraWidth/2) - 10, 0 );
    SandboxWindow.setCameraAngle( 0 );
    SandboxWindow.setCameraSize( TruckToy.CameraWidth, TruckToy.CameraHeight );
    SandboxWindow.setViewLimitOn( TruckToy.WorldLeft, TruckToy.CameraHeight/-2, TruckToy.WorldRight, TruckToy.CameraHeight/2 );

    // Create the scene contents in a roughly left to right order.      

    // Background.
    %this.createBackground();

    // Floor.
    %this.createFloor();
     
    // Wrecked cars at start.
    %this.createWreckedCar( 1, -90, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createWreckedCar( 2, -85, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createWreckedCar( 3, -82, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createWreckedCar( 1, -87.123, -2.478, 2.537, true );
    %this.createBrick( 3, -87.5, TruckToy.FloorLevel + 0.25, true );     
    %this.createBrick( 4, -87.5, TruckToy.FloorLevel + 0.75, true );     
    %this.createBrick( 2, -79, TruckToy.FloorLevel + 0.25, true );     
    %this.createBonfire( -91.5, TruckToy.FloorLevel + 0.5, 1, TruckToy.BackgroundDomain-1 );

    // Building with chains.   
    %this.createForegroundWall( 2, -99, -5 );   
    %this.createForegroundWall( 1, -75.5, -6.5 );  
    %this.createBrokenCementWall( -78, -1.5 );
    %this.createWreckedBuilding( -71.5, -1 );
    %this.createWoodPile( -65, -2.5 );
    %this.createBrickPile( -67, TruckToy.FloorLevel + 0.45 );
    %this.createForegroundBrickWall( 1, -61, -6 );
    %this.createBonfire( -82, TruckToy.FloorLevel + 0.5, 1.5, TruckToy.ObstacleDomain+1 );

    // Start of bridge.   
    %this.createPlank( 1, -53, TruckToy.FloorLevel + 0.5, 0, true );
    %this.createPlank( 1, -50.1522, -2.3, 21.267, true );
    %this.createWreckedCar( 2, -47, TruckToy.FloorLevel + 1.9, -100, true );
    %this.createWreckedCar( 3, -45.5, TruckToy.FloorLevel + 1.9, 100, true );
    %this.createPlank( 2, -44, TruckToy.FloorLevel + 2, -90, true );
    %this.createPlank( 1, -43, TruckToy.FloorLevel + 2, -90, true );
    %this.createPlank( 2, -42, TruckToy.FloorLevel + 2, -90, true );
    %this.createPlank( 1, -41, TruckToy.FloorLevel + 2, -90, true );  
    %this.createForegroundWall( 2, -42, -4.5 );  
    %this.createBridge( -41, TruckToy.FloorLevel + 4, 40 );
    for ( %n = 0; %n < 10; %n++ )
    {
      %brick = %this.createBrickStack( getRandom(1,5), -39 + getRandomF(0,16), TruckToy.FloorLevel + 5, false );     
      %brick.setAwake(true);
    }   
    %this.createPlank( 1, -20.5, TruckToy.FloorLevel + 1.5, -90, true );
    %this.createPlank( 3, -19, TruckToy.FloorLevel + 4, 0, true );
    %this.createPlank( 1, -16.5, TruckToy.FloorLevel + 1.5, -90, true );
    %this.createForegroundBrickWall( 2, -19, -6 );
    %this.createBonfire( -46.5, TruckToy.FloorLevel, 3, TruckToy.BackgroundDomain-1 );
    %this.createBonfire( -18.7, TruckToy.FloorLevel + 1, 2, TruckToy.BackgroundDomain-1 );

    // More wrecked cars.
    %this.createWreckedCar( 1, -12, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createWreckedCar( 2, -7, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createWreckedCar( 3, -4, TruckToy.FloorLevel + 0.75, 0, true );
    %this.createBonfire( -5, TruckToy.FloorLevel + 0.5, 1, TruckToy.BackgroundDomain-1 );
     
     
    // ************************************************************************   
    // Start of pyramid.
    // ************************************************************************   
    %this.createPyramid( 2, TruckToy.FloorLevel + 0.25, 19, true );   
    %this.createForegroundWall( 1, 9, -6 );
    %this.createPyramid( 2+21, TruckToy.FloorLevel + 0.25, 13, true );
    %this.createForegroundBrickWall( 1, 9, -7 );
    %this.createBonfire( 21, TruckToy.FloorLevel, 3, TruckToy.BackgroundDomain-1 );


    // ************************************************************************   
    // Start of brick stacks.
    // ************************************************************************      
    %this.createBrickStack( 45, TruckToy.FloorLevel + 0.25, 10, false );
    %this.createBrickStack( 47, TruckToy.FloorLevel + 0.25, 1, true );
    %this.createBrickStack( 49, TruckToy.FloorLevel + 0.25, 10, false );
     
    %this.createBrickStack( 72, TruckToy.FloorLevel + 0.25, 1, true );
    %this.createBrickStack( 74, TruckToy.FloorLevel + 0.25, 10, false );
    %this.createBrickStack( 76, TruckToy.FloorLevel + 0.25, 1, true );
    %this.createBrickStack( 78, TruckToy.FloorLevel + 0.25, 10, false );

    %this.createBonfire( 71, TruckToy.FloorLevel, 1, TruckToy.BackgroundDomain-1 );
    %this.createBonfire( 85, TruckToy.FloorLevel, 1.5, TruckToy.BackgroundDomain-1 );

    // Truck.
    %truckStartX = TruckToy.WorldLeft + (TruckToy.CameraWidth/6);
    %truckStartY = 3;   
    %this.createTruck( %truckStartX, %truckStartY );    
    
    // Start the timer.
    TruckToy.startTimer( "createProjectile", TruckToy.ProjectileRate );
}

// -----------------------------------------------------------------------------

function TruckToy::createBackground(%this)
{  
    // Atmosphere
    %obj = new Sprite();
    %obj.setBodyType( "static" );
    %obj.setImage( "ToyAssets:highlightBackground" );
    %obj.BlendColor = DarkGray;
    %obj.setSize( TruckToy.WorldWidth * (TruckToy.CameraWidth*2), 75 );
    %obj.setSceneLayer( TruckToy.BackdropDomain );
    %obj.setSceneGroup( TruckToy.BackdropDomain );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );  


    // Industrial Background
    %obj = new Scroller();
    %obj.setBodyType( "static" );
    %obj.setImage( "TruckToy:industrial_02" );
    %obj.setPosition( 0, -1 );
    %obj.setSize( TruckToy.WorldWidth, 8 );
    %obj.setRepeatX( TruckToy.WorldWidth / 8 );   
    %obj.setSceneLayer( TruckToy.BackgroundDomain);
    %obj.setSceneGroup( TruckToy.BackgroundDomain);
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );    
}

// -----------------------------------------------------------------------------

function TruckToy::createFloor(%this)
{
    // Ground  
    %obj = new Scroller();
    %obj.setBodyType( "static" );
    %obj.setImage( "ToyAssets:woodGround" );
    %obj.setSize( TruckToy.WorldWidth, 3 );
    %obj.setPosition( 0, TruckToy.FloorLevel - (%obj.getSizeY()/2) );
    %obj.setRepeatX( TruckToy.WorldWidth / 12 );   
    %obj.setSceneLayer( TruckToy.ObstacleDomain );
    %obj.setSceneGroup( TruckToy.GroundDomain );
    %obj.setDefaultFriction( TruckToy.ObstacleFriction );
    %obj.setCollisionGroups( none );
    %obj.createEdgeCollisionShape( TruckToy.WorldWidth/-2, 1.5, TruckToy.WorldWidth/2, 1.5 );
    %obj.createEdgeCollisionShape( TruckToy.WorldWidth/-2, 3, TruckToy.WorldWidth/-2, 50 );
    %obj.createEdgeCollisionShape( TruckToy.WorldWidth/2, 3, TruckToy.WorldWidth/2, 50 );
    %obj.CollisionCallback = true;
    %obj.setAwake( false );
    SandboxScene.add( %obj );   
}

// -----------------------------------------------------------------------------

function TruckToy::createBrokenCementWall( %this, %posX, %posY )
{
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
    %obj.setImage( "TruckToy:brokenCementWall" );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 6, 6 );
    %obj.setSceneLayer( TruckToy.BackgroundDomain-2 );
    %obj.setSceneGroup( TruckToy.BackgroundDomain);
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );

    return %obj;   
}

// -----------------------------------------------------------------------------

function TruckToy::createWoodPile( %this, %posX, %posY )
{
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
    %obj.setImage( "TruckToy:woodPile" );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 8, 5 );
    %obj.setSceneLayer( TruckToy.BackgroundDomain-2 );
    %obj.setSceneGroup( TruckToy.BackgroundDomain);
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );

    return %obj;   
}

// -----------------------------------------------------------------------------

function TruckToy::createBrickStack( %this, %posX, %posY, %brickCount, %static )
{
    for ( %n = 0; %n < %brickCount; %n++ )
    {
        %this.createBrick( getRandom(1,5), %posX, %posY + (%n*0.5), %static );     
    }      
}

// -----------------------------------------------------------------------------

function TruckToy::createPyramid( %this, %posX, %posY, %brickBaseCount, %static )
{
    if ( %brickBaseCount < 2 )
    {
        echo( "Invalid pyramid brick base count of" SPC %brickBaseCount );
        return;
    }

    for( %stack = 0; %stack < %brickBaseCount; %stack++ )
    {
        %stackIndexCount = %brickBaseCount - (%stack*2);
        %stackX = %posX + ( %stack * 1.0 ) + getRandomF(-0.3, 0.3 );
        %stackY = %posY + ( %stack * 0.5 );
        for ( %stackIndex = 0; %stackIndex < %stackIndexCount; %stackIndex++ )
        {
            %this.createBrick( getRandom(1, 5), %stackX + %stackIndex, %stackY, %static );
        }
    }
}

// -----------------------------------------------------------------------------

function TruckToy::createBridge( %this, %posX, %posY, %linkCount )
{
   
   %linkWidth = 0.5;
   %linkHeight = %linkWidth * 0.5;
   %halfLinkWidth = %linkWidth * 0.5;
   
   %rootObj = new Sprite();
   %rootObj.setBodyType( "static" );
   %rootObj.setImage( "ToyAssets:cable" );
   %rootObj.setPosition( %posX, %posY );
   %rootObj.setSize( %linkWidth, %linkHeight );
   %rootObj.setSceneLayer( TruckToy.BackgroundDomain-3 );
   %rootObj.setSceneGroup( TruckToy.ObstacleDomain );
   %rootObj.setCollisionSuppress();
   SandboxScene.add( %rootObj );

   %lastLinkObj = %rootObj;
   
   for ( %n = 1; %n <= %linkCount; %n++ )
   {
      %obj = new Sprite();
         
      %obj.setImage( "ToyAssets:cable" );
      %obj.setPosition( %posX + (%n*%linkWidth), %posY );
      %obj.setSize( %linkWidth, %linkHeight );
      %obj.setSceneLayer( TruckToy.BackgroundDomain-3 );
      %obj.setSceneGroup( TruckToy.ObstacleDomain );
      
      if ( %n == %linkCount )
      {
         %obj.setBodyType( "static" );
         %obj.setCollisionSuppress();
      }
      else
      {      
         %obj.setCollisionGroups( none );
         %obj.setDefaultDensity( 1 );
         %obj.setDefaultFriction( TruckToy.ObstacleFriction );
         %obj.createPolygonBoxCollisionShape( %linkWidth, %linkHeight );
         %obj.setAngularDamping( 1.0 );
         %obj.setLinearDamping( 1.0 );
      }
      //%obj.setDebugOn( 5 );
      SandboxScene.add( %obj );   
      
      SandboxScene.createRevoluteJoint( %lastLinkObj, %obj, %halfLinkWidth, 0, -%halfLinkWidth, 0 );
      %joint = SandboxScene.createMotorJoint( %lastLinkObj, %obj );
      SandboxScene.setMotorJointMaxForce( %joint, 1000 );
      %obj.setAwake( false );
      %lastLinkObj.setAwake( false );
      //%obj.setDebugOn( 5 );
      
      %lastLinkObj = %obj;
   }
   
   return %lastLinkObj;        
}

// -----------------------------------------------------------------------------

function TruckToy::createChain( %this, %posX, %posY, %linkCount )
{
    %linkWidth = 0.25;
    %linkHeight = %linkWidth * 2;
    %halfLinkHeight = %linkHeight * 0.5;

    %rootObj = new Sprite();
    %rootObj.setBodyType( "static" );
    %rootObj.setImage( "ToyAssets:chain" );
    %rootObj.setPosition( %posX, %posY );
    %rootObj.setSize( %linkWidth, %linkHeight );
    %rootObj.setSceneLayer( TruckToy.BackgroundDomain-1 );
    %rootObj.setSceneGroup( TruckToy.ObstacleDomain );
    %rootObj.setCollisionSuppress();
    SandboxScene.add( %rootObj );

    %lastLinkObj = %rootObj;

    for ( %n = 1; %n <= %linkCount; %n++ )
    {
        if ( %n < %linkCount )
        {
            %obj = new Sprite();
            %obj.setImage( "ToyAssets:chain" );
            %obj.setPosition( %posX, %posY - (%n*%linkHeight) );        
            %obj.setSize( %linkWidth, %linkHeight );
            SandboxScene.add( %obj );   
        }
        else
        {
            %obj = %this.createBonfire( %posX, %posY - (%n*%linkHeight), 0.5, TruckToy.BackgroundDomain-1 );                    
            %obj.BodyType = dynamic;
        }
        
        %obj.setSceneLayer( TruckToy.BackgroundDomain-1 );
        %obj.setSceneGroup( TruckToy.ObstacleDomain );
        %obj.setCollisionGroups( none );
        %obj.setDefaultDensity( 1 );
        %obj.setDefaultFriction( 0.2 );
        %obj.createPolygonBoxCollisionShape( %linkWidth, %linkHeight );
        %obj.setAngularDamping( 1.0 );
        %obj.setLinearDamping( 0.5 );

        SandboxScene.createRevoluteJoint( %lastLinkObj, %obj, 0, -%halfLinkHeight, 0, %halfLinkHeight, false );

        %lastLinkObj = %obj;
    }

    %lastLinkObj.setAwake(false);

    return %lastLinkObj;        
}

// -----------------------------------------------------------------------------

function TruckToy::createWreckedBuilding( %this, %posX, %posY )
{
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
    %obj.setImage( "TruckToy:wreckedBuilding" );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 9, 8 );
    %obj.setSceneLayer( TruckToy.BackgroundDomain-1 );
    %obj.setSceneGroup( TruckToy.BackgroundDomain);
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );

    %this.createChain( %posX - 3, %posY + 3.4, 10 );   
    %this.createChain( %posX - 1, %posY + 3.2, 10 );   
    %this.createChain( %posX + 1, %posY + 3.0, 10 );   
    %this.createChain( %posX + 3, %posY + 2.8, 10 );   
}

// -----------------------------------------------------------------------------

function TruckToy::createForegroundBrickWall( %this, %wallNumber, %posX, %posY )
{
    if ( %wallNumber < 1 || %wallNumber > 2 )
    {
        echo( "Invalid foreground wall no of" SPC %wallNumber );
        return;
    }

    %image = "TruckToy:brickWall_0" @ %wallNumber;
      
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
      
    %obj.setImage( %image );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 10, 5 );
    %obj.setSceneLayer( TruckToy.ForegroundDomain-1 );
    %obj.setSceneGroup( TruckToy.ForegroundDomain );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );
      

    return %obj;   
}

// -----------------------------------------------------------------------------

function TruckToy::createForegroundWall( %this, %wallNumber, %posX, %posY )
{
    if ( %wallNumber < 1 || %wallNumber > 2 )
    {
        echo( "Invalid foreground wall no of" SPC %wallNumber );
        return;
    }

    %image = "TruckToy:foregroundWall_0" @ %wallNumber;
      
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
      
    %obj.setImage( %image );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 6, 6 );
    %obj.setSceneLayer( TruckToy.ForegroundDomain-1 );
    %obj.setSceneGroup( TruckToy.ForegroundDomain );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );

    return %obj;   
}

// -----------------------------------------------------------------------------

function TruckToy::createBrick( %this, %brickNumber, %posX, %posY, %static )
{
    if ( %brickNumber < 1 || %brickNumber > 5 )
    {
        echo( "Invalid brick no of" SPC %brickNumber );
        return;
    }

    %image = "ToyAssets:brick_0" @ %brickNumber;

    %obj = new Sprite();   
    if ( %static ) %obj.setBodyType( "static" );
    %obj.setImage( %image );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 1, 0.5 );
    %obj.setSceneLayer( TruckToy.ObstacleDomain );
    %obj.setSceneGroup( TruckToy.ObstacleDomain );
    %obj.setCollisionGroups( TruckToy.GroundDomain, TruckToy.ObstacleDomain );
    %obj.setDefaultFriction( TruckToy.ObstacleFriction );
    %obj.createPolygonBoxCollisionShape( 1, 0.5 );
    %obj.setAwake( false );
    SandboxScene.add( %obj );

    return %obj;
}

// -----------------------------------------------------------------------------

function TruckToy::createBrickPile( %this, %posX, %posY )
{
    %obj = new Sprite();   
    %obj.setBodyType( "static" );
    %obj.setImage( "TruckToy:brickPile" );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 4, 1 );
    %obj.setSceneLayer( TruckToy.BackgroundDomain-3 );
    %obj.setSceneGroup( TruckToy.BackgroundDomain);
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    SandboxScene.add( %obj );   
}

// -----------------------------------------------------------------------------

function TruckToy::createPlank( %this, %plankNumber, %posX, %posY, %angle, %static )
{
    if ( %plankNumber < 1 || %plankNumber > 3 )
    {
        echo( "Invalid plank no of" SPC %plankNumber );
        return;
    }

    %image = "TruckToy:plank_0" @ %plankNumber;

    %obj = new Sprite();   
    if ( %static ) %obj.setBodyType( "static" );
    %obj.setImage( %image );
    %obj.setAngle( %angle );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 5, 1 );   
    %obj.setSceneLayer( TruckToy.ObstacleDomain );
    %obj.setSceneGroup( TruckToy.ObstacleDomain );
    %obj.setDefaultFriction( TruckToy.ObstacleFriction );
    %obj.setCollisionGroups( TruckToy.GroundDomain, TruckToy.ObstacleDomain );
    %obj.setAwake( false );
    %obj.setDefaultFriction( 1.0 );

    switch$( %plankNumber )
    {  
        case 1: 
            %obj.createPolygonCollisionShape( "-2.5 -0.5 2.5 -0.5 2.5 -0.2 2.0 0.5 -2.5 -0.2" );
        case 2:
            %obj.createPolygonCollisionShape( "-2.5 -0.4 2.4 -0.5 2.4 0.5 0 0.5 -2.1 0.1 -2.5 -0.2" );
        case 3:
            %obj.createPolygonCollisionShape( "-2.5 -0.4 2.5 -0.5 1.9 0.5 -1.8 0.5 -2.5 0" );
    }

    SandboxScene.add( %obj );

    return %obj;   
}

// -----------------------------------------------------------------------------

function TruckToy::createWreckedCar( %this, %carNumber, %posX, %posY, %angle, %static )
{
    if ( %carNumber < 1 || %carNumber > 3 )
    {
        echo( "Invalid brick no of" SPC %brickNumber );
        return;
    }

    %image = "TruckToy:wreckedCar_0" @ %carNumber;

    %obj = new Sprite();   
    if ( %static ) %obj.setBodyType( "static" );
    %obj.setImage( %image );
    %obj.setAngle( %angle );
    %obj.setPosition( %posX, %posY );
    %obj.setSize( 4, 1.5 );   
    %obj.setSceneLayer( TruckToy.ObstacleDomain );
    %obj.setSceneGroup( TruckToy.ObstacleDomain );
    %obj.setCollisionGroups( TruckToy.GroundDomain, TruckToy.ObstacleDomain );
    %obj.setAwake( false );
    %obj.setDefaultFriction( TruckToy.ObstacleFriction );

    switch$( %carNumber )
    {  
        case 1: 
            %obj.createPolygonCollisionShape( "-2 -0.65 0.5 -0.75 2 -0.45 1.9 0.2 0.5 0.65 -0.5 0.6 -2 -0.3" );
        case 2:
            %obj.createPolygonCollisionShape( "-2 -0.75 2 -0.75 2 -0.2 0.4 0.65 -0.9 0.7 -2 0.0" );
        case 3:
            %obj.createPolygonCollisionShape( "-2 -0.65 0 -0.75 2 -0.55 1.8 0.3 0.5 0.75 -0.5 0.75 -2 0.1" );
    }

    SandboxScene.add( %obj );

    return %obj;
}

// -----------------------------------------------------------------------------

function TruckToy::createBonfire(%this, %x, %y, %scale, %layer)
{
    // Create an impact explosion at the projectiles position.
    %particlePlayer = new ParticlePlayer();
    %particlePlayer.BodyType = static;
    %particlePlayer.SetPosition( %x, %y );
    %particlePlayer.SceneLayer = %layer;
    %particlePlayer.ParticleInterpolation = true;
    %particlePlayer.Particle = "ToyAssets:bonfire";
    %particlePlayer.SizeScale = %scale;
    %particlePlayer.CameraIdleDistance = TruckToy.CameraWidth * 0.8;
    SandboxScene.add( %particlePlayer ); 
    return %particlePlayer;
    
}

// -----------------------------------------------------------------------------

function TruckToy::createProjectile(%this)
{
    // Fetch the truck position.
    %truckPositionX = TruckToy.TruckBody.Position.x;
    
    %projectile = new Sprite() { class = "TruckProjectile"; };
    %projectile.Animation = "ToyAssets:Projectile_FireballAnim";
    %projectile.setPosition( getRandom( %truckPositionX - (TruckToy.CameraWidth * 0.2), %truckPositionX + (TruckToy.CameraWidth * 0.5) ), 12 );
    %projectile.setSceneLayer( TruckToy.BackgroundDomain-2 );
    %projectile.setSceneGroup( TruckToy.ProjectileDomain );
    %projectile.FlipY = true;
    %projectile.Size = getRandom(0.5, 2);
    %projectile.Lifetime = 2.5;
    %projectile.createCircleCollisionShape( 0.2 ); 
    %projectile.setCollisionGroups( TruckToy.GroundDomain );
    %projectile.CollisionCallback = true;
    SandboxScene.add( %projectile ); 
}

// -----------------------------------------------------------------------------

function TruckProjectile::onCollision(%this, %object, %collisionDetails)
{   
    // Create an impact explosion at the projectiles position.
    %particlePlayer = new ParticlePlayer();
    %particlePlayer.BodyType = Static;
    %particlePlayer.Position = %this.Position;    
    %particlePlayer.Size = 10;
    %particlePlayer.SceneLayer = TruckToy.BackgroundDomain-1;
    %particlePlayer.ParticleInterpolation = true;
    %particlePlayer.Particle = "ToyAssets:ImpactExplosion";
    %particlePlayer.SizeScale = getRandom(TruckToy.ExplosionScale, TruckToy.ExplosionScale * 1.5);
    SandboxScene.add( %particlePlayer ); 
    
    // Start the camera shaking.
    SandboxWindow.startCameraShake( 10 + (3 * TruckToy.ExplosionScale), 1 );
    
    // Delete the projectile.
    %this.safeDelete();   
}

// -----------------------------------------------------------------------------

function TruckToy::createTruck( %this, %posX, %posY )
{
    // Truck Body.
    %exhaustParticles = new ParticlePlayer();
    %exhaustParticles.setPosition( %posX-3, %posY );
    %exhaustParticles.setSceneLayer( TruckToy.TruckDomain );
    %exhaustParticles.Particle = "TruckToy:exhaust";
    %exhaustParticles.SizeScale = 0.1;
    %exhaustParticles.ForceScale = 0.4;
    %exhaustParticles.EmissionRateScale = 4;
    SandboxScene.add( %exhaustParticles );
    %exhaustParticles.play();
    TruckToy.TruckExhaust = %exhaustParticles;

    TruckToy.TruckBody = new Sprite();
    TruckToy.TruckBody.setPosition( %posX, %posY );
    TruckToy.TruckBody.setImage( "TruckToy:truckBody" );
    TruckToy.TruckBody.setSize( 5, 2.5 );
    TruckToy.TruckBody.setSceneLayer( TruckToy.TruckDomain );
    TruckToy.TruckBody.setSceneGroup( TruckToy.ObstacleDomain);
    TruckToy.TruckBody.setCollisionGroups( TruckToy.ObstacleDomain, TruckToy.ObstacleDomain-1, TruckToy.GroundDomain );
    TruckToy.TruckBody.createPolygonCollisionShape( "-2 0.2 -2 -0.5 0 -.95 2 -0.5 2 0.0 0 0.7 -1.5 0.7" ); 
    //TruckToy.TruckBody.setDebugOn( 5 );
    SandboxScene.add( TruckToy.TruckBody );

    // Attach the exhaust output to the truck body.   
    %joint = SandboxScene.createRevoluteJoint( TruckToy.TruckBody, TruckToy.TruckExhaust, "-2.3 -0.6", "0 0" );
    SandboxScene.setRevoluteJointLimit( %joint, 0, 0 );


    // Mount camera to truck body.
    SandboxWindow.mount( TruckToy.TruckBody, "0 0", 3, true, TruckToy.RotateCamera );

    // Tires.
    // Suspension = -1.0 : -1.5   

    // Rear tire.   
    %tireRear = new Sprite();
    %tireRear.setPosition( %posX-1.4, %posY-1.0 );
    %tireRear.setImage( "ToyAssets:tires" );
    %tireRear.setSize( 1.7, 1.7 );
    %tireRear.setSceneLayer( TruckToy.TruckDomain-1 );
    %tireRear.setSceneGroup( TruckToy.ObstacleDomain );
    %tireRear.setCollisionGroups( TruckToy.ObstacleDomain, TruckToy.GroundDomain );
    %tireRear.setDefaultFriction( TruckToy.WheelFriction );
    %tireRear.setDefaultDensity( TruckToy.RearWheelDensity );
    %tireRear.createCircleCollisionShape( 0.8 ); 
    SandboxScene.add( %tireRear );
    TruckToy.RearWheel = %tireRear;
    
    // Front tire.
    %tireFront = new Sprite();
    %tireFront.setPosition( %posX+1.7, %posY-1.0 );
    %tireFront.setImage( "ToyAssets:tires" );
    %tireFront.setSize( 1.7, 1.7 );
    %tireFront.setSceneLayer( TruckToy.TruckDomain-1 );
    %tireFront.setSceneGroup( TruckToy.ObstacleDomain );
    %tireFront.setCollisionGroups( TruckToy.ObstacleDomain, TruckToy.GroundDomain );
    %tireFront.setDefaultFriction( TruckToy.WheelFriction );
    %tireFront.setDefaultDensity( TruckToy.FrontWheelDensity );
    %tireFront.createCircleCollisionShape( 0.8 ); 
    SandboxScene.add( %tireFront );   
    TruckToy.FrontWheel = %tireFront;

    // Suspension joints.
    TruckToy.RearMotorJoint = SandboxScene.createWheelJoint( TruckToy.TruckBody, %tireRear, "-1.4 -1.25", "0 0", "0 1" );
    TruckToy.FrontMotorJoint = SandboxScene.createWheelJoint( TruckToy.TruckBody, %tireFront, "1.7 -1.25", "0 0", "0 1" );     
}

// -----------------------------------------------------------------------------

function truckForward(%val)
{
    if(%val)
    { 
        if ( !TruckToy.TruckMoving )
        {
            %driveActive = false;
            if ( TruckToy.FrontWheelDrive )
            {
                SandboxScene.setWheelJointMotor( TruckToy.FrontMotorJoint, true, -TruckToy.WheelSpeed, 10000 );
                %driveActive = true;
            }
            else
            {
                SandboxScene.setWheelJointMotor( TruckToy.FrontMotorJoint, false );
            }
            
            if ( TruckToy.RearWheelDrive )
            {
                SandboxScene.setWheelJointMotor( TruckToy.RearMotorJoint, true, -TruckToy.WheelSpeed, 10000 );
                %driveActive = true;
            }
            else
            {
                SandboxScene.setWheelJointMotor( TruckToy.RearMotorJoint, false );
            }            
            
            if ( %driveActive )
            {
                TruckToy.TruckExhaust.SizeScale *= 4;
                TruckToy.TruckExhaust.ForceScale /= 2;
            }
        }
              
        TruckToy.TruckMoving = true;
    }
    else
    {
        truckStop();
    }
}

// -----------------------------------------------------------------------------

function truckReverse(%val)
{
    if(%val)
    {
        if ( !TruckToy.TruckMoving )
        {
            %driveActive = false;
            if ( TruckToy.FrontWheelDrive )
            {
                SandboxScene.setWheelJointMotor( TruckToy.FrontMotorJoint, true, TruckToy.WheelSpeed, 10000 );
                %driveActive = true;
            }
            else
            {
                SandboxScene.setWheelJointMotor( TruckToy.FrontMotorJoint, false );
            }
            if ( TruckToy.RearWheelDrive )
            {
                SandboxScene.setWheelJointMotor( TruckToy.RearMotorJoint, true, TruckToy.WheelSpeed, 10000 );
                %driveActive = true;
            }
            else
            {
                SandboxScene.setWheelJointMotor( TruckToy.RearMotorJoint, false );
            }
            
            if ( %driveActive )
            {
                TruckToy.TruckExhaust.SizeScale *= 4;
                TruckToy.TruckExhaust.ForceScale /= 2;
            }            
        }
              
        TruckToy.TruckMoving = true;
    }
    else
    {
        truckStop();
    }
}

//-----------------------------------------------------------------------------

function TruckToy::truckStop(%this)
{
    // Finish if truck is not moving.
    if ( !TruckToy.TruckMoving )
        return;

    // Stop truck moving.
    SandboxScene.setWheelJointMotor( TruckToy.RearMotorJoint, true, 0, 10000 );
    SandboxScene.setWheelJointMotor( TruckToy.FrontMotorJoint, true, 0, 10000 );
    TruckToy.TruckExhaust.SizeScale /= 4;
    TruckToy.TruckExhaust.ForceScale *= 2;

    // Flag truck as not moving.    
    TruckToy.TruckMoving = false;
}

//-----------------------------------------------------------------------------

function TruckToy::setWheelSpeed( %this, %value )
{
    %this.WheelSpeed = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setWheelFriction( %this, %value )
{
    %this.WheelFriction = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setFrontWheelDensity( %this, %value )
{
    %this.FrontWheelDensity = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setRearWheelDensity( %this, %value )
{
    %this.RearWheelDensity = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setFrontWheelDrive( %this, %value )
{
    %this.FrontWheelDrive = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setRearWheelDrive( %this, %value )
{
    %this.RearWheelDrive = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setProjectileRate( %this, %value )
{
    %this.ProjectileRate = %value;
    
    // Start the timer.
    TruckToy.startTimer( "createProjectile", TruckToy.ProjectileRate );
}

//-----------------------------------------------------------------------------

function TruckToy::setExplosionScale( %this, %value )
{
    %this.ExplosionScale = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::setRotateCamera( %this, %value )
{
    %this.RotateCamera = %value;
}

//-----------------------------------------------------------------------------

function TruckToy::onTouchDown(%this, %touchID, %worldPosition)
{
    // Finish if truck is already moving.
    if ( TruckToy.TruckMoving )
        return;

    // If we touch in-front of the truck then move forward else reverse.
    if ( %worldPosition.x >= TruckToy.TruckBody.Position.x )
    {
        truckForward( true );
    }
    else
    {
        truckReverse( true );
    }
}

//-----------------------------------------------------------------------------

function TruckToy::onTouchUp(%this, %touchID, %worldPosition)
{
    // Stop the truck.
    TruckToy.truckStop();
}
    
