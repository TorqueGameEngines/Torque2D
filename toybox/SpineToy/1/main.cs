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

function SpineToy::create(%this)
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );

    // Set the manipulation mode.
    Sandbox.useManipulation( pan );

    %this.asset = "SpineToy:goblins";
    %this.skin = "goblin";
    %this.animation = "walk";
    
    addSelectionOption( "goblin,goblingirl", "Select Skin", 4, "setSkin", false, "Sets the skin for the skeleton object." );
    
    // Reset the toy.
    SpineToy.reset();
}

//-----------------------------------------------------------------------------

function SpineToy::destroy(%this)
{
}

//-----------------------------------------------------------------------------

function SpineToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();

    if (%this.resetSchedule !$= "")
        cancel(%this.resetSchedule);
        
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );

    %this.createBackground();
    %this.createGoblin();
}

//-----------------------------------------------------------------------------

function SpineToy::setSkeleton(%this, %value)
{
    %this.asset = %value;
    
    if (%value $= "SpineToy:goblins")
        %this.setSkin("goblin");
    else
        %this.setSkin("default");
}

//-----------------------------------------------------------------------------

function SpineToy::setSkin(%this, %value)
{
    %this.skin = %value;
    %this.walker.Skin = %this.skin;
}

//-----------------------------------------------------------------------------

function SpineToy::createGoblin(%this)
{
    // Create the skeleton object
    %goblin = new SkeletonObject();
    
    // Assign it an asset
    %goblin.Asset = %this.asset;
    %goblin.Skin = %this.skin;
        
    // Set the animation name
    %goblin.setAnimationName(%this.animation, true);
    
    %goblin.RootBoneScale = 0.025;
    %goblin.setRootBoneOffset(0, -5);
    
    %goblin.position = "-25 -8";
    %goblin.SceneLayer = 29;
    %goblin.setLinearVelocity(7.5, 0);
        
    %this.walker = %goblin;
    
    %this.resetSchedule = %this.schedule(8000, resetWalker);

    // Add it to the scene
    SandboxScene.add(%goblin);
}

//-----------------------------------------------------------------------------

function SpineToy::createSpineBoy(%this)
{
    // Create the skeleton object
    %object = new SkeletonObject()
    {
        class = "SpineBoy";
    };

    %object.Asset = "SpineToy:TestSkeleton";
    %object.setAnimationName("walk", true);
    %object.RootBoneScale = 0.025;

    %object.SceneLayer = 29;
    %object.Position = 0;
    %object.setMix("walk", "jump", 0.2);
    %object.setMix("jump", "walk", 0.4);

    SandboxScene.add(%object);

    %object.schedule(4000, "doJump");
}

//-----------------------------------------------------------------------------

function SpineBoy::onAnimationFinished(%this, %animationName)
{
    if (%animationName $= "jump")
    {
        %this.setAnimationName("walk", true);
        %this.schedule(4000, "doJump");
    }
}

//-----------------------------------------------------------------------------

function SpineBoy::doJump(%this)
{
    %this.setAnimationName("jump", false);
}

//-----------------------------------------------------------------------------

function SpineToy::resetWalker(%this)
{
    %this.walker.setPosition("-25 -8");
    %this.resetSchedule = %this.schedule(8000, resetWalker);
}

//-----------------------------------------------------------------------------

function SpineToy::createBackground(%this)
{
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "40 30";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "SpineToy:background";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );
    
    %this.createPowerup(-14, 6);
    %this.createPowerup(14, 6);
    
    // Create the skeleton object
    %animatedMenu = new SkeletonObject();
    
    // Assign it an asset
    %animatedMenu.Asset = "SpineToy:spinosaurus";

    // Set properties    
    %animatedMenu.setAnimationName("Animation", false);
    %duration = %animatedMenu.getAnimationDuration();
    %animatedMenu.schedule(%duration*1000, "setAnimationName", "loop", true);
    %animatedMenu.position = "0 0";
    %animatedMenu.SceneLayer = 30;
    %animatedMenu.RootBoneScale = 0.025;
    %animatedMenu.setRootBoneOffset(0, -10);
    
    // Add it to the scene
    SandboxScene.add(%animatedMenu);
}

//-----------------------------------------------------------------------------

function SpineToy::createPowerup(%this, %xPos, %yPos)
{
    // Create the skeleton object
    %powerup = new SkeletonObject();
    
    // Assign it an asset
    %powerup.Asset = "SpineToy:powerup";

    // Set properties    
    %powerup.setAnimationName("Animation", true);    
    %powerup.position = %xPos SPC %yPos;
    %powerup.SceneLayer = 30;
    
    %powerup.RootBoneScale = 0.025;
    %powerup.setRootBoneOffset(0, -5);
    
    // Add it to the scene
    SandboxScene.add(%powerup);
}

//-----------------------------------------------------------------------------
