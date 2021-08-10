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

function AquariumToy::create( %this )
{
    exec("./scripts/aquarium.cs");
    exec("./scripts/fish.cs");

    // Configure settings.
    AquariumToy.maxFish = 10;
    AquariumToy.currentFish = 0;
    AquariumToy.selectedAnimation = "TropicalAssets:angelfish1Anim";
    
    // Set the fish scene-layer to sort in "batch" mode
    // so that all the fish will be sorted into a batchable order to reduce draw calls.
    SandboxScene.setLayerSortMode( 15, batch );
    
    addNumericOption("Max Fish", 1, 50, 1, "setMaxFish", %this.maxFish, true, "Sets the maximum number of fish to be created.");
    addSelectionOption(getFishAnimationList(), "Fish Animation", 5, "setSelectedAnimation", false, "Selects the fish animation that can be spawned manually.");
    addButtonOption("Spawn fish", "spawnOneFish", false, "Spawns the selected fish animation." );  

    // Reset the toy initially.
    AquariumToy.reset();
}

//-----------------------------------------------------------------------------

function AquariumToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function AquariumToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();

    // Set the gravity.
    SandboxScene.setGravity(0, 0);

    buildAquarium(SandboxScene);
    createAquariumEffects(SandboxScene);

    // Reset the ball count.
    %this.currentFish = 0;

    // Start the timer.
    AquariumToy.startTimer( "spawnFish", 100 );
}

//-----------------------------------------------------------------------------

function AquariumToy::spawnFish(%this)
{
    %position = getRandom(-55, 55) SPC getRandom(-20, 20);
    %index = getRandom(0, 5);
    %anim = getUnit(getFishAnimationList(), %index, ",");

    %fishSize = getFishSize(%anim);

    %fish = new Sprite()
    {
        Animation = %anim;
        class = "FishClass";
        position = %position;
        size = %fishSize;
        SceneLayer = "15";
        SceneGroup = "14";
        minSpeed = "5";
        maxSpeed = "15";
        CollisionCallback = true;
    };

    // aquarium boundary triggers are in group 15.  See TropicalAssets/scripts/aquarium.cs
    %fish.setCollisionGroups( 15 );
    %fish.createPolygonBoxCollisionShape(%fishSize);
    %fish.setDefaultDensity( 1 );
    SandboxScene.add( %fish );

    %this.currentFish++;

    // Have we reached the maximum number of fish?
    if ( %this.currentFish >= %this.maxFish)
    {
        // Yes, so stop the timer.
        AquariumToy.stopTimer();
    }
}

//-----------------------------------------------------------------------------

function AquariumToy::setMaxFish(%this, %value)
{
    %this.maxFish = %value;
}
//-----------------------------------------------------------------------------

function AquariumToy::setSelectedAnimation(%this, %value)
{
    %this.selectedAnimation = %value;
}

//-----------------------------------------------------------------------------

function AquariumToy::spawnOneFish(%this)
{
    %position = getRandom(-55, 55) SPC getRandom(-20, 20);

    %fishSize = getFishSize(%this.selectedAnimation);

    %fish = new Sprite()
    {
        Animation = %this.selectedAnimation;
        class = "FishClass";
        position = %position;
        size = %fishSize;
        SceneLayer = "2";
        SceneGroup = "14";
        minSpeed = "5";
        maxSpeed = "15";
        CollisionCallback = true;
    };

    %fish.createPolygonBoxCollisionShape( 15, 15);
    %fish.setCollisionGroups( 15 );
    %fish.setDefaultDensity( 1 );
    %fish.setDefaultFriction( 1.0 );
    SandboxScene.add( %fish );
}


