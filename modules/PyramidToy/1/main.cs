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

function PyramidToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Configure the toy.
    PyramidToy.BlockSize = 1.5;
    PyramidToy.BlockCount = 15;
    PyramidToy.GroundWidth = 140;
    
    // Set the camera.
    SandboxWindow.setCameraSize( 40, 30 );
    
    // Se the gravity.
    SandboxScene.setGravity( 0, -9.8 );

    // Add configuration option.
    addNumericOption( "Block Count", 2, 30, 1, "setBlockCount", PyramidToy.BlockCount, true, "Sets the number of blocks used to create the pyramid." );
    addNumericOption( "Block Size", 1, 4, 0.5, "setBlockSize", PyramidToy.BlockSize, true, "Sets the size of the blocks used to create the pyramid." );
    
    // Reset the toy.
    PyramidToy.reset();
}

//-----------------------------------------------------------------------------

function PyramidToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function PyramidToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
        
    // Create a background.
    %this.createBackground();
    
    // Create the pyramid.
    %this.createPyramid();    
    
    // Create the ground.
    %this.createGround();    
}

//-----------------------------------------------------------------------------

function PyramidToy::setBlockCount(%this, %value)
{
    %this.BlockCount = %value;
}

//-----------------------------------------------------------------------------

function PyramidToy::setBlockSize(%this, %value)
{
    %this.BlockSize = %value;
}

//-----------------------------------------------------------------------------

function PyramidToy::createBackground( %this )
{ 

    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the size.        
    %object.Size = PyramidToy.GroundWidth SPC (PyramidToy.GroundWidth * 0.75);
    
    // Set the position.
    %object.setPositionY( (%object.getSizeY() * 0.5) - 15 );
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:jungleSky";
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function PyramidToy::createGround( %this )
{
    // Create the ground
    %ground = new Scroller();
    %ground.setBodyType("static");
    %ground.Image = "ToyAssets:dirtGround";
    %ground.setPosition(0, -12);
    %ground.setSize(PyramidToy.GroundWidth, 6);
    %ground.setRepeatX(PyramidToy.GroundWidth / 60);   
    %ground.createEdgeCollisionShape(PyramidToy.GroundWidth/-2, 3, PyramidToy.GroundWidth/2, 3);
	//%ground.SceneLayer = 10;
    SandboxScene.add(%ground);  
    
    // Create the grass.
    %grass = new Sprite();
    %grass.setBodyType("static");
    %grass.Image = "ToyAssets:grassForeground";
    %grass.setPosition(0, -8.5);
    %grass.setSize(PyramidToy.GroundWidth, 2); 
	%grass.SceneLayer = 10;
    SandboxScene.add(%grass);       
}

//-----------------------------------------------------------------------------

function PyramidToy::createPyramid( %this )
{   
    // Fetch the block count.
    %blockCount = PyramidToy.BlockCount;
    
    // Sanity!
    if ( %blockCount < 2 )
    {
        echo( "Cannot have a pyramid block count less than two." );
        return;
    }

    // Set the block size.
    %blockSize = PyramidToy.BlockSize;
    
    // Calculate a block building position.
    %posx = %blockCount * -0.5 * %blockSize;
    %posy = -8.8 + (%blockSize * 0.5);

    // Build the stack of blocks.
    for( %stack = 0; %stack < %blockCount; %stack++ )
    {
        // Calculate the stack position.
        %stackIndexCount = %blockCount - (%stack*2);
        %stackX = %posX + ( %stack * %blockSize );
        %stackY = %posY + ( %stack * %blockSize );
        
        // Build the stack.
        for ( %stackIndex = 0; %stackIndex < %stackIndexCount; %stackIndex++ )
        {
            // Calculate the block position.
            %blockX = %stackX + (%stackIndex*%blockSize);
            %blockY = %stackY;

            // Create the sprite.
            %obj = new Sprite();
            %obj.setPosition( %blockX, %blockY );
            %obj.setSize( %blockSize );
            %obj.setImage( "ToyAssets:blocks" );
            %obj.setImageFrame( getRandom(0,55) );
            %obj.setDefaultFriction( 1.0 );
            %obj.createPolygonBoxCollisionShape( %blockSize, %blockSize );
			%obj.SceneLayer = 15;
            
            // Add to the scene.
            SandboxScene.add( %obj );          
        }
    }
	
	%map = new LightObject();
	%map.setSize("15 15");
	%map.SceneLayer = 17;
	%map.setBlendColor(0.5,0.5,1.0,1.0);
	SandboxScene.add(%map);
}
