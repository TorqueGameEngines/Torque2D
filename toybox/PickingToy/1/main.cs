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

function PickingToy::create( %this )
{
    // Configure the toy.
    PickingToy.PickType = Point;
    PickingToy.PickMode = Any;
    PickingToy.NotPickedAlpha = 0.2;
    PickingToy.PickAreaSize = 10;
    PickingToy.RayStart = "0 30";

    // Add the configuration options.
    addSelectionOption( "Point,Area,Circle,Ray", "Pick Type", 4, "setPickType", true, "Selects the picking type." );
    addSelectionOption( "Any,OOBB,AABB,Collision", "Pick Mode", 4, "setPickMode", false, "Selects the picking mode." );

    // Force-on debug options.
    setAABBOption( true );
    setOOBBOption( true );
    setCollisionOption( true );
    
    // Reset the toy.
    PickingToy.reset();
}


//-----------------------------------------------------------------------------

function PickingToy::destroy( %this )
{
    // Force-off debug options.
    setAABBOption( false );
    setOOBBOption( false );
    setCollisionOption( false );    
}

//-----------------------------------------------------------------------------

function PickingToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
       
    // Create target.
    %this.createTarget();   
    
    // Create pick cursor.
    %this.createPickCursor();
    
    // Create the ray-cast overlay.
    %this.createRaycastOverlay();    
}

//-----------------------------------------------------------------------------

function PickingToy::createBackground( %this )
{    
    // Create the sprite.
    %object = SandboxScene.create( Sprite );
    %object.BodyType = static;
    %object.Position = "0 0";
    %object.Size = "100 75";
    %object.SceneLayer = 31;
    %object.Image = "ToyAssets:highlightBackground";
    %object.BlendColor = SlateGray;
    %object.PickingAllowed = false;
}

//-----------------------------------------------------------------------------

function PickingToy::createTarget( %this )
{    
    // Create the sprite.
    %object = SandboxScene.create( Sprite );
    %object.Size = 40;
    %object.Angle = -30;
    %object.Image = "ToyAssets:Tiles";
    %object.Frame = 0;
    %object.setBlendAlpha( PickingToy.NotPickedAlpha );
    // Create some collision shapes.    
    %object.createCircleCollisionShape( 10, "-20 -20" );
    %object.createPolygonBoxCollisionShape( "20 20", "20 20" );
    
    // Set the target object.
    PickingToy.TargetObject = %object;
}

//-----------------------------------------------------------------------------

function PickingToy::createPickCursor( %this )
{    
    // Create the sprite.
    %object = SandboxScene.create( Sprite );
    %object.Size = PickingToy.PickAreaSize;
    %object.BlendColor = Red;
    %object.PickingAllowed = false;
    
    if ( PickingToy.PickType $= "point" || PickingToy.PickType $= "ray" )
    {
        %object.Image = "ToyAssets:CrossHair1";
    }
    else if ( PickingToy.PickType $= "area" )
    {
        %object.Image = "ToyAssets:Blank";
    }
    else if ( PickingToy.PickType $= "circle" )
    {
        %object.Image = "ToyAssets:BlankCircle";
    }
    
    // Set the cursor.
    PickingToy.CursorObject = %object;
}

//-----------------------------------------------------------------------------

function PickingToy::createRaycastOverlay( %this )
{    
    // Finish if not in ray mode.
    if ( PickingToy.PickType !$= "ray" )
        return;
    
    // Create the sprite.
    %object = SandboxScene.create( ShapeVector );
    %object.Size = "1 1";
    %object.PickingAllowed = false;
    %object.IsCircle = false;
    %object.FillMode = false;
    %object.LineColor = Red;   
    
    // Set the ray-cast overlay object.
    PickingToy.RaycastOverlay = %object;
}

//-----------------------------------------------------------------------------

function PickingToy::onTouchDown(%this, %touchID, %worldPosition)
{
    // Update cursor position.
    PickingToy.CursorObject.Position = %worldPosition;
    
    // Are we in ray mode?
    if ( PickingToy.PickType $= "ray" )
    {
        // Yes, so update the ray geometry.
        PickingToy.RaycastOverlay.PolyList = PickingToy.RayStart SPC %worldPosition;
    }    
    
    // Handle picking mode appropriately.
    switch$( PickingToy.PickType )
    {
        case "point":
            %picked = SandboxScene.pickPoint( %worldPosition, "", "", PickingToy.PickMode );
        
        case "area":
            %halfSize = PickingToy.PickAreaSize * 0.5;
            %lower = (%worldPosition._0 - %halfSize) SPC(%worldPosition._1 - %halfSize);    
            %upper = (%worldPosition._0 + %halfSize) SPC(%worldPosition._1 + %halfSize);    
            %picked = SandboxScene.pickArea( %lower, %upper, "", "", PickingToy.PickMode );
            
        case "ray":
            %picked = SandboxScene.pickRay( PickingToy.RayStart, %worldPosition, "", "", PickingToy.PickMode );
            
        case "circle":
            %halfSize = PickingToy.PickAreaSize * 0.5;
            %picked = SandboxScene.pickCircle( %worldPosition, %halfSize, "", "", PickingToy.PickMode );
    }
        
    // Fetch pick count.
    %pickCount = %picked.Count;
    
    // See if the target object is amongst those picked.
    for( %i = 0; %i < %pickCount; %i++ )
    {
        // If this is the target object then make it opaque.
        if ( getWord( %picked, %i ) == PickingToy.TargetObject )
        {
            PickingToy.TargetObject.setBlendAlpha( 1.0 );
            return;
        }
    }
    
    // Target not picked so make it transparent.
    PickingToy.TargetObject.setBlendAlpha( 0.25 );
}

//-----------------------------------------------------------------------------

function PickingToy::onTouchDragged(%this, %touchID, %worldPosition)
{
    // Update cursor position.
    PickingToy.CursorObject.Position = %worldPosition;
    
    // Are we in ray mode?
    if ( PickingToy.PickType $= "ray" )
    {
        // Yes, so update the ray geometry.
        PickingToy.RaycastOverlay.PolyList = PickingToy.RayStart SPC %worldPosition;
    }    
    
    // Handle picking mode appropriately.
    switch$( PickingToy.PickType )
    {
        case "point":
            %picked = SandboxScene.pickPoint( %worldPosition, "", "", PickingToy.PickMode );
        
        case "area":
            %halfSize = PickingToy.PickAreaSize * 0.5;
            %lower = (%worldPosition._0 - %halfSize) SPC(%worldPosition._1 - %halfSize);    
            %upper = (%worldPosition._0 + %halfSize) SPC(%worldPosition._1 + %halfSize);    
            %picked = SandboxScene.pickArea( %lower, %upper, "", "", PickingToy.PickMode );
            
        case "ray":
            %picked = SandboxScene.pickRay( PickingToy.RayStart, %worldPosition, "", "", PickingToy.PickMode );
            
        case "circle":
            %halfSize = PickingToy.PickAreaSize * 0.5;
            %picked = SandboxScene.pickCircle( %worldPosition, %halfSize, "", "", PickingToy.PickMode );
    }
        
    // Fetch pick count.
    %pickCount = %picked.Count;
    
    // See if the target object is amongst those picked.
    for( %i = 0; %i < %pickCount; %i++ )
    {
        // If this is the target object then make it opaque.
        if ( getWord( %picked, %i ) == PickingToy.TargetObject )
        {
            PickingToy.TargetObject.setBlendAlpha( 1.0 );
            return;
        }
    }
    
    // Target not picked so make it transparent.
    PickingToy.TargetObject.setBlendAlpha( 0.25 );
}

//-----------------------------------------------------------------------------

function PickingToy::setPickMode( %this, %value )
{
    PickingToy.PickMode = %value;
}

//-----------------------------------------------------------------------------

function PickingToy::setPickType( %this, %value )
{
    PickingToy.PickType = %value;
}