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

function SoftbodyToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Configure the toy.
    SoftbodyToy.GroundWidth = 100;
    SoftbodyToy.NodeSize = 5;
    SoftbodyToy.NodeX = 5;
    SoftbodyToy.NodeY = 5;
    SoftbodyToy.Frequency = 3;
    SoftbodyToy.DampingRatio = 0;
    SoftbodyToy.Density = 0.1;
    SoftbodyToy.Restitution = 0.5;
    
    // Add configuration option.
    addNumericOption( "Node Size", 1, 5, 1, "setNodeSize", SoftbodyToy.NodeSize, true, "Sets the size of each node sprite." );
    addNumericOption( "Node Count X", 1, 100, 1, "setNodeX", SoftbodyToy.NodeX, true, "Sets the number of nodes in the X axis." );
    addNumericOption( "Node Count Y", 1, 100, 1, "setNodeY", SoftbodyToy.NodeY, true, "Sets the number of nodes in the Y axis." );
    addNumericOption( "Node Density", 0, 10, 0.1, "setDensity", SoftbodyToy.Density, true, "Sets the density of each node." );
    addNumericOption( "Node Restitution", 0, 5, 0.1, "setRestitution", SoftbodyToy.Restitution, true, "Sets the restitution of each node." );
    addNumericOption( "Joint Frequency", 0, 120, 1, "setFrequency", SoftbodyToy.Frequency, true, "Sets the frequency of the distance joints." );
    addNumericOption( "Joint Damping Ratio", 0, 10, 0.1, "setDampingRatio", SoftbodyToy.DampingRatio , true, "Sets the damping ratio of the distance joints." );
    
        
    // Reset the toy initially.
    SoftbodyToy.reset();        
}

//-----------------------------------------------------------------------------

function SoftbodyToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function SoftbodyToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Set the scene gravity.
    SandboxScene.setGravity( 0, -9.8 );
            
    // Create the background.
    %this.createBackground();
    
    // Create the surround.
    %this.createSurround();
    
    // Create a cube soft body.
    %this.createCubeSoftbody();
}

//-----------------------------------------------------------------------------

function SoftbodyToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Stop the background from being affected by gravity.
    %object.setBodyType( "static" );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:skyBackground";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function SoftbodyToy::createSurround( %this )
{
    // Create the bottom
    %obj = new Scroller();
    %obj.setBodyType("static");
    %obj.Image = "ToyAssets:dirtGround";
    %obj.setPosition(0, -35);
    %obj.setSize(SoftbodyToy.GroundWidth, 6);
    %obj.setRepeatX(SoftbodyToy.GroundWidth / 60);   
    %obj.createEdgeCollisionShape(SoftbodyToy.GroundWidth/-2, 3, SoftbodyToy.GroundWidth/2, 3);
    %obj.createEdgeCollisionShape(SoftbodyToy.GroundWidth/-2, 72, SoftbodyToy.GroundWidth/2, 72);
    %obj.createEdgeCollisionShape(SoftbodyToy.GroundWidth/-2, 3, SoftbodyToy.GroundWidth/-2, 72);
    %obj.createEdgeCollisionShape(SoftbodyToy.GroundWidth/2, 3, SoftbodyToy.GroundWidth/2, 72);
    SandboxScene.add(%obj);  
        
    // Create the grass.
    %obj = new Sprite();
    %obj.setBodyType("static");
    %obj.Image = "ToyAssets:grassForeground";
    %obj.setPosition(0, -31.5);
    %obj.setSize(SoftbodyToy.GroundWidth, 2); 
    SandboxScene.add(%obj);                
}

//-----------------------------------------------------------------------------

function SoftbodyToy::createCubeSoftbody( %this )
{
    // Set cube start.
    %positionX = 0;
    %positionY = 0;
    
    // Calculate cube offset.
    %halfNodeSize = SoftbodyToy.NodeSize * 0.5;
    %nodeStride = SoftbodyToy.NodeSize * 0.8;
    %offsetX = %positionX - (( %nodeStride * (SoftbodyToy.NodeX-1)) * 0.5);
    %offsetY = %positionY - (( %nodeStride * (SoftbodyToy.NodeY-1)) * 0.5);
        
    // Create the sprites.
    for ( %x = 0; %x < SoftbodyToy.NodeX; %x++ )
    {
        for ( %y = 0; %y < SoftbodyToy.NodeY; %y++ )
        {            
            // Create the sprite.
            %object = new Sprite();
            
            // Set the position.
            %object.setPosition( %offsetX + (%x * %nodeStride), %offsetY + (%y * %nodeStride) );
            
            // Set the static image.
            //%object.Image = "ToyAssets:WhiteSphere";
            %object.Animation = "ToyAssets:PoisonCloudWobble";
            
            // Set a useful size.
            %object.Size = SoftbodyToy.NodeSize;

            // Set the object at a fixed angle so it cannot rotate.            
            %object.setFixedAngle( true );

            // Set default density.
            %object.setDefaultDensity( SoftbodyToy.Density );            

            // Set default restitution.
            %object.setDefaultRestitution( SoftbodyToy.Restitution );            
            
            // Create a collision shape.
            %object.createCircleCollisionShape( %halfNodeSize * 0.7 );
            
            // Add to the scene.
            SandboxScene.add( %object );
            
            // Store object.
            SoftbodyToy.body[%x,%y] = %object;
        }
    }
      
    // Create distance joints.
    for ( %x = 0; %x < SoftbodyToy.NodeX; %x++ )
    {
        for ( %y = 0; %y < (SoftbodyToy.NodeY-1); %y++ )
        {
            // Fetch objects.
            %objectA = SoftbodyToy.body[%x,%y];
            %objectB = SoftbodyToy.body[%x,%y+1];
            
            // Create a distance joint between them.
            %jointId = SandboxScene.createDistanceJoint( %objectA, %objectB );
            
            // Configure the joint.
            SandboxScene.setDistanceJointFrequency( %jointId, SoftbodyToy.Frequency );
            SandboxScene.setDistanceJointDampingRatio( %jointId, SoftbodyToy.DampingRatio );
        }
    }    
    
    // Create distance joints.
    for ( %y = 0; %y < SoftbodyToy.NodeY; %y++ )
    {
        for ( %x = 0; %x < (SoftbodyToy.NodeX-1); %x++ )
        {
            // Fetch objects.
            %objectA = SoftbodyToy.body[%x,%y];
            %objectB = SoftbodyToy.body[%x+1,%y];
            
            // Create a distance joint between them.
            %jointId = SandboxScene.createDistanceJoint( %objectA, %objectB );
            
            // Configure the joint.
            SandboxScene.setDistanceJointFrequency( %jointId, SoftbodyToy.Frequency );
            SandboxScene.setDistanceJointDampingRatio( %jointId, SoftbodyToy.DampingRatio );
        }
    }
    
    // Create distance joints.
    for ( %y = 0; %y < (SoftbodyToy.NodeY-1); %y++ )
    {
        for ( %x = 0; %x < (SoftbodyToy.NodeX-1); %x++ )
        {
            // Fetch objects.
            %objectA = SoftbodyToy.body[%x,%y];
            %objectB = SoftbodyToy.body[%x+1,%y];
            %objectC = SoftbodyToy.body[%x+1,%y+1];
            %objectD = SoftbodyToy.body[%x,%y+1];
            
            // Create a distance joint between them.
            %jointId1 = SandboxScene.createDistanceJoint( %objectA, %objectC );
            %jointId2 = SandboxScene.createDistanceJoint( %objectB, %objectD );
            
            // Configure the joints.
            SandboxScene.setDistanceJointFrequency( %jointId1, SoftbodyToy.Frequency );
            SandboxScene.setDistanceJointFrequency( %jointId2, SoftbodyToy.Frequency );
            SandboxScene.setDistanceJointDampingRatio( %jointId1, SoftbodyToy.DampingRatio );
            SandboxScene.setDistanceJointDampingRatio( %jointId2, SoftbodyToy.DampingRatio );
        }
    }          
    
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setNodeSize( %this, %value )
{
    %this.NodeSize = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setNodeX(%this, %value)
{
    %this.NodeX = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setNodeY(%this, %value)
{
    %this.NodeY = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setDensity(%this, %value)
{
    %this.Density = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setRestitution(%this, %value)
{
    %this.Restitution = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setFrequency(%this, %value)
{
    %this.Frequency = %value;
}

//-----------------------------------------------------------------------------

function SoftbodyToy::setDampingRatio(%this, %value)
{
    %this.DampingRatio = %value;
}
