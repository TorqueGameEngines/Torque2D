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

function DeathBallToy::create( %this )
{
    // Execute behavior scripts
    exec("./scripts/dealsDamageBehavior.cs");
    exec("./scripts/takesDamageBehavior.cs");
    exec("./scripts/faceObjectBehavior.cs");
    exec("./scripts/moveTowardBehavior.cs");
    exec("./scripts/spawnAreaBehavior.cs");

    // Initialize the toys settings.
    DeathBallToy.WorldTop = 35;
    DeathBallToy.WorldBottom = -110;
    DeathBallToy.WorldLeft = -50;
    DeathBallToy.WorldRight = 140;

    DeathBallToy.rotateSpeed = 360;
    DeathBallToy.maxBallSpeed = 10;
    DeathBallToy.ballSpeed = 5;
    DeathBallToy.soldierSpeed = 1;
    DeathBallToy.maxSoldierSpeed = 10;
    DeathballToy.spawnAmount = 80;

    // Add the custom controls.
    addNumericOption("Deathball move speed", 1, 10, 1, "setBallSpeed", DeathBallToy.ballSpeed, false, "Sets the deathball movement speed.");
    addNumericOption("Soldier count", 40, 100, 10, "setSpawnAmount", DeathBallToy.spawnAmount, true, "Sets the solider count.");
    addNumericOption("Soldier speed", 1, 10, 1, "setSoldierSpeed", DeathBallToy.soldierSpeed, false, "Sets the soldier speed.");

    // Create the BehaviorTemplate definitions. They are automatically added to the
    // DeathBallToy scopeset in the "create" functions
    %this.createDealsDamageBehavior();
    %this.createTakesDamageBehavior();
    %this.createFaceObjectBehavior();
    %this.createMoveTowardBehavior();
    %this.createSpawnAreaBehavior();

    // Reset the toy initially.
    DeathBallToy.reset();
}

//-----------------------------------------------------------------------------

function DeathBallToy::destroy( %this )
{
    // Cancel any pending events.
    DeathBallToy::cancelPendingEvents();
}

//-----------------------------------------------------------------------------

function DeathBallToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();

    // Set the gravity.
    SandboxScene.setGravity(0, 0);

    // Limit the camera to the four sections of the desert
    SandboxWindow.setViewLimitOn( DeathBallToy.WorldLeft, DeathBallToy.WorldBottom, DeathBallToy.WorldRight, DeathBallToy.WorldTop );
    
    // Add backgrounds
    %this.createDesertBackgrounds();

    // Create invisible walls for the deathball to bounce off
    %this.createBarriers();

    // Add deathball
    %this.spawnDeathball("50 -40");

    // Create the soldier enemy we will be spawning
    %this.generateSoldierTemplate();

    // Setup the spawners
    %this.setupSpawnPoints();
}

//-----------------------------------------------------------------------------

function DeathBallToy::createDesertBackgrounds(%this)
{
    // Create the composite sprite.
    %composite = new CompositeSprite();

    %composite.setSceneLayer(3);

    // Set the batch layout mode.  We must do this before we add any sprites.
    %composite.SetBatchLayout( "off" );

    // Add upper left desert
    %composite.addSprite();
    %composite.setSpriteLocalPosition(0, 0);
    %composite.setSpriteSize(100, 75);
    %composite.setSpriteImage( "DeathBallToy:upperLeftDesert");

    // Add upper right desert
    %composite.addSprite();
    %composite.setSpriteLocalPosition(100, 0);
    %composite.setSpriteSize(100, 75);
    %composite.setSpriteImage( "DeathBallToy:upperRightDesert");

    // Add lower left desert
    %composite.addSprite();
    %composite.setSpriteLocalPosition(0, -75);
    %composite.setSpriteSize(100, 75);
    %composite.setSpriteImage( "DeathBallToy:lowerLeftDesert");

    // Add lower right desert
    %composite.addSprite();
    %composite.setSpriteLocalPosition(100, -75);
    %composite.setSpriteSize(100, 75);
    %composite.setSpriteImage( "DeathBallToy:lowerRightDesert");

    // Add to the scene.
    SandboxScene.add( %composite );
}

function DeathBallToy::createBarriers(%this)
{
    %leftWall = new SceneObject()
    {
        Size = "10 500";
        Position = "-55 0";
    };

    %leftWall.createPolygonBoxCollisionShape( 10, 500);
    %leftWall.setBodyType( "static" );

    %rightWall = new SceneObject()
    {
        Size = "10 500";
        Position = "155 0";
    };

    %rightWall.createPolygonBoxCollisionShape( 10, 500);
    %rightWall.setBodyType( "static" );

    %topWall = new SceneObject()
    {
        Size = "500 10";
        Position = "0 42";
    };

    %topWall.createPolygonBoxCollisionShape( 500, 10);
    %topWall.setBodyType( "static" );

    %bottomWall = new SceneObject()
    {
        Size = "500 10";
        Position = "0 -118";
    };

    %bottomWall.createPolygonBoxCollisionShape( 500, 10);
    %bottomWall.setBodyType( "static" );

    SandboxScene.add(%leftWall);
    SandboxScene.add(%rightWall);
    SandboxScene.add(%topWall);
    SandboxScene.add(%bottomWall);

}

//-----------------------------------------------------------------------------

function DeathBallToy::spawnDeathball(%this, %position)
{
    %db = new Sprite(Deathball)
    {
        Animation = "DeathBallToy:dbForwardAnim";
        position = %position;
        size = "20 20";
        SceneLayer = "1";
        SceneGroup = "14";
        minSpeed = "5";
        maxSpeed = "15";
        CollisionCallback = true;
    };
    
    %db.createCircleCollisionShape(8);
    %dealsDamageBehavior = DealsDamageBehavior.createInstance();
    %dealsDamageBehavior.initialize(100, false);
    Deathball.addBehavior(%dealsDamageBehavior);
    
    //%db.pauseAnimation(1);

    Deathball.rollSchedule = Deathball.schedule(100, "updateRollAnimation");

    SandboxScene.add(%db);

    SandboxWindow.mount( Deathball, "0 0", 0, true, false );
}

//-----------------------------------------------------------------------------

function Deathball::updateRollAnimation(%this)
{
    %this.rollSchedule = "";

    %velocity = %this.getLinearVelocity();    
    %scaledVelocity = VectorLen(%velocity); // (mAbs(getWord(%velocity, 0))) + mAbs(getWord(%velocity, 1)) / 50;
    %flooredVelocity = mFloatLength(%scaledVelocity, 1);

    %this.setAnimationTimeScale(%flooredVelocity / 20);

    %this.rollSchedule = %this.schedule(100, updateRollAnimation);
}

//-----------------------------------------------------------------------------

function DeathBallToy::generateSoldierTemplate(%this)
{
    // Create the soldier sprite
    %soldier = new Sprite()
    {
        Animation = "DeathBallToy:soldierAnim";
        size = "10 10";
        SceneLayer = "2";
        SceneGroup = "14";
        CollisionCallback = true;
        Class = "DeathBallSoldier";
    };

    %soldier.createPolygonBoxCollisionShape(5, 8);
    
    // Return it to the toy
    %this.soldierTemplate = %soldier;
    
    SandboxScene.add(%this.soldierTemplate);

    // Disable it
    %this.soldierTemplate.setEnabled(0);

    // Add the behaviors
    %takesDamageBehavior = TakesDamageBehavior.createInstance();
    %takesDamageBehavior.initialize(1, "", "", "DeathBallToy:soldierDeathAnim", false);
    
    %moveTowardBehavior = MoveTowardBehavior.createInstance();
    
    %adjustedSpeed = DeathBallToy.soldierSpeed / DeathBallToy.maxSoldierSpeed;
    %moveTowardBehavior.initialize(Deathball, %adjustedSpeed);
    
    %faceObjectBehavior = FaceObjectBehavior.createInstance();
    %faceObjectBehavior.initialize(Deathball, 0, 0);
    
    %soldier.addBehavior(%takesDamageBehavior);
    %soldier.addBehavior(%moveTowardBehavior);
    %soldier.addBehavior(%faceObjectBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::setupSpawnPoints(%this)
{
    %amount = %this.spawnAmount / 6;

    // Creating four in the corners of the desert
    %this.createSpawnPoint("-30 10", %amount);
    %this.createSpawnPoint("50 10", %amount);
    %this.createSpawnPoint("120 10", %amount);
    %this.createSpawnPoint("-30 -90", %amount);
    %this.createSpawnPoint("50 -90", %amount);
    %this.createSpawnPoint("120 -90", %amount);

}

//-----------------------------------------------------------------------------

function DeathBallToy::createSpawnPoint(%this, %position, %amount)
{
    %spawnPoint = new sceneObject()
    {
        size = "40 40";
        position = %position;
    };

    %spawnPointBehavior = %this.spawnAreaBehavior.createInstance();
    %spawnPointBehavior.initialize(%this.soldierTemplate, %amount, 0.5, 0, true, "Area");
    %spawnPoint.addBehavior(%spawnPointBehavior);

    SandboxScene.add(%spawnPoint);

}

//-----------------------------------------------------------------------------

function DeathBallToy::createDealsDamageBehavior(%this)
{
    // Create the named template and retain it as a custom field on this toy
    %this.dealsDamageBehavior = new BehaviorTemplate(DealsDamageBehavior);

    // Fill in the details of the behavior
    %this.dealsDamageBehavior.friendlyName = "Deals Damage";
    %this.dealsDamageBehavior.behaviorType = "Game";
    %this.dealsDamageBehavior.description  = "Set the object to deal damage to TakesDamage objects it collides with";

    // Add the custom behavior fields
    %this.dealsDamageBehavior.addBehaviorField(strength, "The amount of damage the object deals", int, 10);
    %this.dealsDamageBehavior.addBehaviorField(deleteOnHit, "Delete the object when it collides", bool, 1);

    // Add the BehaviorTemplate to the scope set so it is destroyed when the module is unloaded
    DeathBallToy.add(%this.dealsDamageBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::createTakesDamageBehavior(%this)
{
    // Create the named template and retain it as a custom field on this toy
    %this.takesDamageBehavior = new BehaviorTemplate(TakesDamageBehavior);

    // Fill in the details of the behavior
    %this.takesDamageBehavior.friendlyName = "Takes Damage";
    %this.takesDamageBehavior.behaviorType = "Game";
    %this.takesDamageBehavior.description  = "Set the object to take damage from DealsDamage objects that collide with it";

    // Add the custom behavior fields
    %this.takesDamageBehavior.addBehaviorField(health, "The amount of health the object has", int, 100);
    %this.takesDamageBehavior.addBehaviorField(explodeEffect, "The particle effect to play on death", asset, "", ParticleAsset);
    %this.takesDamageBehavior.addBehaviorField(spawnEffect, "The particle effect to play on spawn", asset, "", ParticleAsset);
    %this.takesDamageBehavior.addBehaviorField(deathAnim, "The object's death animation, alternative to explodeEffect", asset, "", AnimationAsset);
    %this.takesDamageBehavior.addBehaviorField(deleteOnDeath, "Delete the owner upon dieing", bool, false);

    // Add the BehaviorTemplate to the scope set so it is destroyed when the module is unloaded
    DeathBallToy.add(%this.takesDamageBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::createFaceObjectBehavior(%this)
{
    // Create the named template and retain it as a custom field on this toy
    %this.faceObjectBehavior = new BehaviorTemplate(FaceObjectBehavior);

    // Fill in the details of the behavior
    %this.faceObjectBehavior.friendlyName = "Face Object";
    %this.faceObjectBehavior.behaviorType = "AI";
    %this.faceObjectBehavior.description  = "Set the object to face another object";

    // Add the custom behavior fields
    %this.faceObjectBehavior.addBehaviorField(target, "The object to face", object, "", sceneObject);
    %this.faceObjectBehavior.addBehaviorField(turnSpeed, "The speed to rotate at (degrees per second). Use 0 to snap", float, 0.0);
    %this.faceObjectBehavior.addBehaviorField(rotationOffset, "The rotation offset (degrees)", float, 0.0);

    // Add the BehaviorTemplate to the scope set so it is destroyed when the module is unloaded
    DeathBallToy.add(%this.faceObjectBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::createMoveTowardBehavior(%this)
{
    // Create the named template and retain it as a custom field on this toy
    %this.moveTowardBehavior = new BehaviorTemplate(MoveTowardBehavior);

    // Fill in the details of the behavior
    %this.moveTowardBehavior.friendlyName = "Move Toward";
    %this.moveTowardBehavior.behaviorType = "AI";
    %this.moveTowardBehavior.description  = "Set the object to move toward another object";

    // Add the custom behavior fields
    %this.moveTowardBehavior.addBehaviorField(target, "The object to move toward", object, "", sceneObject);
    %this.moveTowardBehavior.addBehaviorField(speed, "The speed to move toward the object at (world units per second)", float, 2.0);

    // Add the BehaviorTemplate to the scope set so it is destroyed when the module is unloaded
    DeathBallToy.add(%this.moveTowardBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::createSpawnAreaBehavior(%this)
{
    // Create the named template and retain it as a custom field on this toy
    %this.spawnAreaBehavior = new BehaviorTemplate(SpawnAreaBehavior);

    // Fill in the details of the behavior
    %this.spawnAreaBehavior.friendlyName = "Spawn Area";
    %this.spawnAreaBehavior.behaviorType = "AI";
    %this.spawnAreaBehavior.description  = "Spawns objects inside the area of this object";

    // Add the custom behavior fields
    %this.spawnAreaBehavior.addBehaviorField(object, "The object to clone", object, "", sceneObject);
    %this.spawnAreaBehavior.addBehaviorField(count, "The number of objects to clone (-1 for infinite)", int, 50);
    %this.spawnAreaBehavior.addBehaviorField(spawnTime, "The time between spawns (seconds)", float, 2.0);
    %this.spawnAreaBehavior.addBehaviorField(spawnVariance, "The variance in the spawn time (seconds)", float, 1.0);
    %this.spawnAreaBehavior.addBehaviorField(autoSpawn, "Automatically start/stop spawning", bool, true);

    %spawnLocations = "Area" TAB "Edges" TAB "Center" TAB "Top" TAB "Bottom" TAB "Left" TAB "Right";
    %this.spawnAreaBehavior.addBehaviorField(spawnLocation, "The are in which objects can be spawned", enum, "Area", %spawnLocations);

    // Add the BehaviorTemplate to the scope set so it is destroyed when the module is unloaded
    DeathBallToy.add(%this.spawnAreaBehavior);
}

//-----------------------------------------------------------------------------

function DeathBallToy::setBallSpeed(%this, %value)
{
    %this.ballSpeed = %value;
}

//-----------------------------------------------------------------------------

function DeathBallToy::setSpawnAmount(%this, %value)
{
    %this.spawnAmount = %value;
}

//-----------------------------------------------------------------------------

function DeathBallToy::setSoldierSpeed(%this, %value)
{
    %this.soldierSpeed = %value;
}

//-----------------------------------------------------------------------------

function DeathBallToy::cancelPendingEvents()
{
    // Finish if there are not pending events.
    if ( !isEventPending(DeathBall.rollSchedule) )
        return;

    cancel(DeathBall.rollSchedule);
    DeathBall.rollSchedule = "";
}

//-----------------------------------------------------------------------------

function DeathBallToy::onTouchDown(%this, %touchID, %worldPosition)
{
    %origin = Deathball.getPosition();
    %angle = mAtan( Vector2Sub( %worldPosition, %origin ) ) - 90;

    Deathball.RotateTo( %angle, DeathBallToy.rotateSpeed );

    %adjustedSpeed = DeathBallToy.ballSpeed / DeathBallToy.maxBallSpeed;

    %relativePosition = VectorSub(%worldPosition, Deathball.getPosition());

    %scaledVelocity = VectorScale(%relativePosition, %adjustedSpeed);

    Deathball.setLinearVelocity( getWord(%scaledVelocity, 0), getWord(%scaledVelocity, 1) );
}

//-----------------------------------------------------------------------------

function DeathBallToy::onTouchUp(%this, %touchID, %worldPosition)
{
    %origin = Deathball.getPosition();
    %angle = mAtan( Vector2Sub( %worldPosition, %origin ) );
    
    // Since the speed is used instead of time, we can use the current velocity to set it's speed.
    %adjustedSpeed = VectorLen(DeathBall.getLinearVelocity());// (DeathBallToy.ballSpeed / DeathBallToy.maxBallSpeed) * 3000;
   
    Deathball.MoveTo( %worldPosition, %adjustedSpeed, true, false );
}

//-----------------------------------------------------------------------------

function DeathBallToy::onTouchDragged(%this, %touchID, %worldPosition)
{    
    %origin = Deathball.getPosition();
    %angle = mAtan( Vector2Sub( %worldPosition, %origin ) ) - 90;

    Deathball.RotateTo( %angle, DeathBallToy.rotateSpeed );

    %adjustedSpeed = DeathBallToy.ballSpeed / DeathBallToy.maxBallSpeed;

    %relativePosition = VectorSub(%worldPosition, Deathball.getPosition());

    %scaledVelocity = VectorScale(%relativePosition, %adjustedSpeed);

    Deathball.setLinearVelocity( getWord(%scaledVelocity, 0), getWord(%scaledVelocity, 1) );
}
