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

function CompoundObjectsToy::create( %this )
{
    // Configure the toy.
    CompoundObjectsToy.BlockSize = 1.5;
    CompoundObjectsToy.BlockCount = 15;
    CompoundObjectsToy.GroundWidth = 40;    
    
    // Set the camera.
    SandboxWindow.setCameraSize( 40, 30 );
    
    // Se the gravity.
    SandboxScene.setGravity( 0, -9.8 );
   
    // Reset the toy.
    CompoundObjectsToy.reset();
}

//-----------------------------------------------------------------------------

function CompoundObjectsToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function CompoundObjectsToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
        
    // Create a background.
    %this.createBackground();
       
    // Create the ground.
    %this.createGround();    
}

//-----------------------------------------------------------------------------

function CompoundObjectsToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the size.        
    %object.Size = CompoundObjectsToy.GroundWidth SPC (CompoundObjectsToy.GroundWidth * 0.75);
    
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

function CompoundObjectsToy::createGround( %this )
{
    // Create the ground
    %ground = SandboxScene.create( Scroller );
    %ground.BodyType = static;
    %ground.Image = "ToyAssets:dirtGround";
    %ground.SceneGroup = 10;
    %ground.setPosition(0, -12);
    %ground.setSize(CompoundObjectsToy.GroundWidth, 6);
    %ground.setRepeatX(CompoundObjectsToy.GroundWidth / 60);   
    %ground.createEdgeCollisionShape(CompoundObjectsToy.GroundWidth/-2, 3, CompoundObjectsToy.GroundWidth/2, 3);
    %ground.createEdgeCollisionShape(CompoundObjectsToy.GroundWidth/-2, 3, CompoundObjectsToy.GroundWidth/-2, 40 );
    %ground.createEdgeCollisionShape(CompoundObjectsToy.GroundWidth/2, 3, CompoundObjectsToy.GroundWidth/2, 40 );
    
    // Create the grass.
    %grass = SandboxScene.create( Sprite );
    %grass.BodyType = static;
    %grass.Image = "ToyAssets:grassForeground";
    %grass.SetPosition(0, -8.5);
    %grass.setSize(CompoundObjectsToy.GroundWidth, 2); 

}

//-----------------------------------------------------------------------------

function CompoundObjectsToy::createCompoundObject( %this, %worldPosition )
{
    // Configure the compound object.
    %radius = 2;
    %angleStride = 15;
    %blockSize = 1;    
      
    // Create the composite.
    %composite = SandboxScene.create( CompositeSprite );
    
    // Turn-off batch culling to save memory as this is a small composite.
    %composite.BatchCulling = false;
    
    // Turn-off batch layout as these sprites will be positioned explicitly.
    %composite.BatchLayout = "off";
    
    // Render everything together, don't sort the sprites with the rest of the scene layer.
    %composite.BatchIsolated = true;
    
    // Set the position.    
    %composite.Position = %worldPosition;
    
    // Set the scene layer (behind the grass).
    %composite.SceneLayer = 1;
    
    // Create compound ring.    
    for( %angle = 0; %angle < 360; %angle += %angleStride )
    {
        %spriteX = mCos( %angle ) * %radius;
        %spriteY = mSin( %angle ) * %radius;
        
        %composite.addSprite();
        %composite.setSpriteLocalPosition( %spriteX, %spriteY );
        %composite.setSpriteSize( %blockSize );
        %composite.setSpriteAngle( %angle );
        %composite.setSpriteImage( "ToyAssets:Blocks" );
        %composite.setSpriteImageFrame( getRandom(0,55) );
    }

    // Add center sprite.
    %composite.addSprite();
    %composite.setSpriteSize( %radius * 2 );
    %composite.setSpriteAnimation( "ToyAssets:TD_Barbarian_WalkSouth" );

    // Set the collision shape defaults.
    %composite.setDefaultFriction( 0.25 );
    %composite.setDefaultRestitution( 0.75 );
    
    // Create a collision shape.
    %composite.createCircleCollisionShape( %radius + (%blockSize * 0.5 ) ); 
}

//-----------------------------------------------------------------------------

function CompoundObjectsToy::onTouchDown(%this, %touchID, %worldPosition)
{
    %this.createCompoundObject( %worldPosition );
}
