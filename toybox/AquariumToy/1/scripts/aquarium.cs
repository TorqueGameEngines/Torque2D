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

function getFishAnimationList()
{
   %list = "TropicalAssets:angelfish1Anim" @ "," @ "TropicalAssets:angelfish2Anim" @ "," @ "TropicalAssets:butterflyfishAnim";
   %list = %list @ "," @ "TropicalAssets:pufferfishAnim" @ "," @ "TropicalAssets:rockfishAnim" @ "," @ "TropicalAssets:seahorseAnim";
   %list = %list @ "," @ "TropicalAssets:triggerfish1Anim" @ "," @ "TropicalAssets:eelAnim";
}

//-----------------------------------------------------------------------------

function getFishSize(%anim)
{
    switch$(%anim)
    {
        case "TropicalAssets:angelfish1Anim":
        %fishInfo = "15 15";

        case "TropicalAssets:angelfish2Anim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:butterflyfishAnim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:pufferfishAnim":
        %fishInfo = "15 15";
        
        case "TropicalAssets:rockfishAnim":
        %fishInfo = "15 7.5";
        
        case "TropicalAssets:seahorseAnim":
        %fishInfo = "7.5 15";
        
        case "TropicalAssets:triggerfish1Anim":
        %fishInfo = "15 15";

        case "TropicalAssets:eelAnim":
        %fishInfo = "7.5 3.75";
    }

    return %fishInfo;
}

//-----------------------------------------------------------------------------

function buildAquarium(%scene)
{
    // A pre-built aquarium of size 100x75, with blue water, some haze, and some nice rocks.
    // Triggers will be provide around the edges to let the developer know when objects in the
    // aquarium have reached the edges.

    // Background
    %background = new Sprite();
    %background.setBodyType( "static" );
    %background.setImage( "TropicalAssets:background" );
    %background.setSize( 100, 75 );
    %background.setCollisionSuppress();
    %background.setAwake( false );
    %background.setActive( false );
    %background.setSceneLayer(30);
    %scene.add( %background );
    
    // Far rocks
    %farRocks = new Sprite();
    %farRocks.setBodyType( "static" );
    %farRocks.setPosition( 0, -7.5 );
    %farRocks.setImage( "TropicalAssets:rocksfar" );
    %farRocks.setSize( 100, 75 );
    %farRocks.setCollisionSuppress();
    %farRocks.setAwake( false );
    %farRocks.setActive( false );
    %farRocks.setSceneLayer(20);
    %scene.add( %farRocks );
    
    // Near rocks
    %nearRocks = new Sprite();
    %nearRocks.setBodyType( "static" );
    %nearRocks.setPosition( 0, -8.5 );
    %nearRocks.setImage( "TropicalAssets:rocksnear" );
    %nearRocks.setSize( 100, 75 );
    %nearRocks.setCollisionSuppress();
    %nearRocks.setAwake( false );
    %nearRocks.setActive( false );
    %nearRocks.setSceneLayer(10);
    %scene.add( %nearRocks );
    
    addAquariumBoundaries( %scene, 100, 75 );
}

//-----------------------------------------------------------------------------

function addAquariumBoundaries(%scene, %width, %height)
{
    // add boundaries on all sides of the aquarium a bit outside of the border of the tank.
    // The triggers allow for onCollision to be sent to any fish or other object that touches the edges.
    // The triggers are far enough outside the tank so that objects will most likely be just out of view
    // before they are sent the onCollision callback.  This way will they can adjust "off stage".

    // Calculate a width and height to use for the bounds.
    // They should be bigger than the aquarium itself.
    %wrapWidth = %width * 1.5;
    %wrapHeight = %height * 1.5;

    %scene.add( createOneAquariumBoundary( "left",   -%wrapWidth/2 SPC 0,  5 SPC %wrapHeight) );
    %scene.add( createOneAquariumBoundary( "right",  %wrapWidth/2 SPC 0,   5 SPC %wrapHeight) );
    %scene.add( createOneAquariumBoundary( "top",    0 SPC -%wrapHeight/2, %wrapWidth SPC 5 ) );
    %scene.add( createOneAquariumBoundary( "bottom", 0 SPC %wrapHeight/2,  %wrapWidth SPC 5 ) );
}

//-----------------------------------------------------------------------------

function createOneAquariumBoundary(%side, %position, %size)
{
    %boundary = new SceneObject() { class = "AquariumBoundary"; };
    
    %boundary.setSize( %size );
    %boundary.side = %side;
    %boundary.setPosition( %position );
    %boundary.setSceneLayer( 1 );
    %boundary.createPolygonBoxCollisionShape( %size );
    // the objects that collide with us should handle any callbacks.
    // remember to set those scene objects to collide with scene group 15 (which is our group)!
    %boundary.setSceneGroup( 15 );
    %boundary.setCollisionCallback(false);
    %boundary.setBodyType( "static" );
    %boundary.setCollisionShapeIsSensor(0, true);
    return %boundary;
}

//-----------------------------------------------------------------------------

function createAquariumEffects(%scene)
{
    %obj = new Scroller();
    %obj.setBodyType( "static" );
    %obj.setImage( "TropicalAssets:wave" );
    %obj.setPosition( 0, 0 );
    %obj.setScrollX(2);
    %obj.setSize( 100, 75 );
    %obj.setRepeatX( 0.2 );   
    %obj.setSceneLayer( 0 );
    %obj.setSceneGroup( 0 );
    %obj.setCollisionSuppress();
    %obj.setAwake( false );
    %obj.setActive( false );
    %scene.add( %obj );
    
    // Add the caustics particle.
    %caustics = new ParticlePlayer();
    %caustics.Particle = "TropicalAssets:Caustics";
    %scene.add( %caustics ); 
}

//-----------------------------------------------------------------------------
