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

function BridgeToy::create(%this)
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );   
    
    // Set the scene gravity.
    SandboxScene.setGravity(0, -9.8);   
    
    // Configure the toy.
    BridgeToy.GroundWidth = 40;
    BridgeToy.maxDebris = 3;
    
    // Add configuration option.
    addNumericOption("Amount of Debris", 0, 30, 1, "setMaxDebris", BridgeToy.maxDebris, true, "Sets the amount of debris created.");
    
    // Reset the toy initially.
    BridgeToy.reset();
}

//-----------------------------------------------------------------------------

function BridgeToy::destroy(%this)
{   
       
}

//-----------------------------------------------------------------------------

function BridgeToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Set the camera size.
    SandboxWindow.setCameraSize( 40, 30 );
       
    // Create a background.
    %this.createBackground();
                
    // Create the links for the bridge
    %this.createBridge(-12.5, -1, 52);
    
    // Add some debris
    %this.createDebris();
    
    // Create the left side of the bridge
    %this.createBase(-13, 0, 0);
    
    // Create the right side of the bridge
    %this.createBase(13, 0, 0);
        
    // Create the ground.
    %this.createGround();   
}

//-----------------------------------------------------------------------------

function BridgeToy::createBackground( %this )
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

function BridgeToy::createGround( %this )
{
    // Create the ground
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "ToyAssets:dirtGround";
    %ground.setPosition(0, -12);
    %ground.setSize(BridgeToy.GroundWidth, 6);
    %ground.setRepeatX(BridgeToy.GroundWidth / 60);   
    %ground.createEdgeCollisionShape(BridgeToy.GroundWidth/-2, 3, BridgeToy.GroundWidth/2, 3);
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -8.5);
    %grass.setSize(BridgeToy.GroundWidth, 2); 
    SandboxScene.add(%grass);       
}

//-----------------------------------------------------------------------------

function BridgeToy::createBase(%this, %posX, %posY, %angle)
{
    %obj = new Sprite();   
    %obj.setBodyType("static");
    %obj.setImage("ToyAssets:jungleTree");
    %obj.setSize(10, 18);   
    %obj.setPosition(%posX, %posY);
    %obj.setAngle(%angle);    
    
    SandboxScene.add(%obj);
}

//-----------------------------------------------------------------------------

function BridgeToy::createBridge(%this, %posX, %posY, %linkCount)
{
    %linkWidth = 0.5;
    %linkHeight = %linkWidth * 0.5;
    %halfLinkWidth = %linkWidth * 0.5;

    %rootObj = new Sprite();
    %rootObj.setBodyType("static");
    %rootObj.setImage("ToyAssets:cable");
    %rootObj.setPosition(%posX, %posY);
    %rootObj.setSize(%linkWidth, %linkHeight);
    %rootObj.setCollisionSuppress();
    SandboxScene.add(%rootObj);

    %lastLinkObj = %rootObj;

    for (%n = 1; %n <= %linkCount; %n++)
    {
        %obj = new Sprite();

        %obj.setImage("ToyAssets:cable");
        %obj.setPosition(%posX + (%n*%linkWidth), %posY);
        %obj.setSize(%linkWidth, %linkHeight);
        
        if (%n == %linkCount)
        {
            %obj.setBodyType("static");
            %obj.setCollisionSuppress();
        }
        else
        {      
            %obj.setDefaultDensity(1);
            %obj.setDefaultFriction(1.0);
            %obj.createPolygonBoxCollisionShape(%linkWidth, %linkHeight);
            %obj.setAngularDamping(1.0);
            %obj.setLinearDamping(1.0);
        }
        
        SandboxScene.add(%obj);   

        SandboxScene.createRevoluteJoint(%lastLinkObj, %obj, %halfLinkWidth, 0, -%halfLinkWidth, 0);
        %joint = SandboxScene.createMotorJoint(%lastLinkObj, %obj);
        SandboxScene.setMotorJointMaxForce(%joint, 100);
        %obj.setAwake(false);
        %lastLinkObj.setAwake(false);
        
        %lastLinkObj = %obj;
    }
}

//-----------------------------------------------------------------------------

function BridgeToy::createDebris(%this)
{
    for (%image = 0; %i < %this.maxDebris; %i++)
    {
        %randomPosition = getRandom(-10, 10) SPC getRandom(2, 8);
        
        %obj = new Sprite();   
        
        %obj.setImage("ToyAssets:crate");
        %obj.setPosition(%randomPosition);
        %obj.setSize(1.5, 1.5);
        %obj.setDefaultFriction(1.0);
        %obj.setDefaultDensity(0.1);
        %obj.createPolygonBoxCollisionShape(1.4, 1.4);
        %obj.setBullet( true );
        
        SandboxScene.add(%obj);
    }
}

//-----------------------------------------------------------------------------

function BridgeToy::setMaxDebris(%this, %value)
{
    %this.maxDebris = %value;
}
