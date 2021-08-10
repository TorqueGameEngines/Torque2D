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

function ChainToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );   

    // Set the scene gravity.
    SandboxScene.setGravity(0, -29.8);
    
    // Configure the toy.
    ChainToy.GroundWidth = 80;
    ChainToy.ChainLinks = 25;
    ChainToy.ChainCount = 2;
    ChainToy.ChainLimit = true;
    ChainToy.BallDensity = 4;

    // Add configuration option.
    addNumericOption( "Chain Links", 1, 46, 1, "setChainLinks", ChainToy.ChainLinks, true, "Sets the number of links in each chain." );
    addNumericOption( "Chain Count", 1, 20, 1, "setChainCount", ChainToy.ChainCount, true, "Sets the number of chains to create." );
    addFlagOption("Chain Limit", "setChainLimit", ChainToy.ChainLimit, true, "Whether to restrict the length of the chain with a rope joint or not." );
    addNumericOption( "Ball Density", 1, 10, 1, "setBallDensity", ChainToy.BallDensity, true, "Sets the ball density." );
    
    // Reset the toy initially.
    ChainToy.reset();
}

//-----------------------------------------------------------------------------

function ChainToy::destroy( %this )
{   
       
}

//-----------------------------------------------------------------------------

function ChainToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );
    
    // Create a background.
    %this.createBackground();
               
    // Create the chains.
    %chainSpacing = 2;
    %chainOffset = (ChainToy.ChainCount-1) * -(%chainSpacing*0.5);
    for( %n = 0; %n < ChainToy.ChainCount; %n++ )
    {        
        %this.createChain(%chainOffset, 15);
        %chainOffset += %chainSpacing;
    }

    // Create the ground.
    %this.createGround();          
}

//-----------------------------------------------------------------------------

function ChainToy::createBackground( %this )
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
    %object.Image = "ToyAssets:jungleSky";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function ChainToy::createGround( %this )
{
    // Create the ground.
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "ToyAssets:dirtGround";
    %ground.setPosition(0, -12);
    %ground.setSize(ChainToy.GroundWidth, 6);
    %ground.setRepeatX(ChainToy.GroundWidth / 60);   
    %ground.createEdgeCollisionShape(ChainToy.GroundWidth/-2, 3, ChainToy.GroundWidth/2, 3);
    
    // Add to the scene.
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -8.5);
    %grass.setSize(ChainToy.GroundWidth, 2); 
    
    // Add to the scene.
    SandboxScene.add(%grass);       
}

//-----------------------------------------------------------------------------

function ChainToy::createChain(%this, %posX, %posY)
{
    // Set-up some initial dimensions.
    %linkWidth = 0.25;
    %linkHeight = %linkWidth * 2;
    %halfLinkHeight = %linkHeight * 0.5;
    %weightSize = 1.5;
    %weightHalfSize = %weightSize * 0.5;
    %pivotDistance = %linkHeight * %this.ChainLinks;  

    // Create a fixed pivot object.
    %fixedObject = new Sprite();
    %fixedObject.BodyType = static;
    %fixedObject.Image = "ToyAssets:chain";
    %fixedObject.setPosition( %posX, %posY );
    %fixedObject.setSize( %linkWidth, %linkHeight );
    SandboxScene.add( %fixedObject );

    // Set-up the last linked object as the fixed pivot.
    %lastLinkObj = %fixedObject;

    // Now add the rest of the links.
    for ( %n = 1; %n <= %this.ChainLinks; %n++ )
    {
        // Create the link object.
        %obj = new Sprite();
        %obj.setImage( "ToyAssets:chain" );
        %obj.setPosition( %posX, %posY - (%n*%linkHeight) );
        %obj.setSize( %linkWidth, %linkHeight );
        %obj.setDefaultDensity( 20 );
        %obj.setDefaultFriction( 0.2 );
        %obj.createPolygonBoxCollisionShape( %linkWidth, %linkHeight );
        %obj.setAngularDamping( 0.1 );
        %obj.setLinearDamping( 0.1 );
        SandboxScene.add( %obj );   

        // Create a revolute joint from the last link to this one.
        SandboxScene.createRevoluteJoint( %lastLinkObj, %obj, 0, -%halfLinkHeight, 0, %halfLinkHeight, false );

        // Reference this link as the last link.
        %lastLinkObj = %obj;
    }    

    // Create the weight.
    %weight = new Sprite();
    %weight.setImage( "ToyAssets:whitesphere" );
    %weight.BlendColor = DarkGreen;
    %weight.setSize( %weightSize );
    %weight.setPosition( %posX, %posY - %pivotDistance - %weightHalfSize );
    %weight.setDefaultFriction( 0.2 );
    %weight.setDefaultDensity( 1 );
    %weight.createCircleCollisionShape( %weightHalfSize );
    SandboxScene.add( %weight );

    // Create a revolute joint from the last link to the weight.
    SandboxScene.createRevoluteJoint( %lastLinkObj, %weight, 0, -%halfLinkHeight, 0, %halfLinkHeight, false );
    
    // If the chain limit is on then create a rope join from the fixed pivot to the weight.
    if ( %this.ChainLimit )
        SandboxScene.createRopeJoint(%fixedObject, %weight, 0, 0, 0, %weightHalfSize, %pivotDistance, false);
}

//-----------------------------------------------------------------------------

function ChainToy::setChainLinks(%this, %value)
{
    %this.ChainLinks = %value;
}

//-----------------------------------------------------------------------------

function ChainToy::setChainCount(%this, %value)
{
    %this.ChainCount = %value;
}

//-----------------------------------------------------------------------------

function ChainToy::setChainLimit(%this, %value)
{
    %this.ChainLimit = %value;
}

//-----------------------------------------------------------------------------

function ChainToy::setBallDensity(%this, %value)
{
    %this.BallDensity = %value;
}
