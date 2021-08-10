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

function AlphaBlendToy::create( %this )
{
    // Configure the toy.    
    AlphaBlendToy.SpriteCount = 0;
    AlphaBlendToy.SpriteSize = 10;    

    // Add the custom controls.
    addNumericOption("SpriteSize", 1, 30, 1, "setSpriteSize", AlphaBlendToy.SpriteSize, true, "Sets the size of the sprite.");
    
    // Reset the toy initially.
    AlphaBlendToy.reset();        
}

//-----------------------------------------------------------------------------

function AlphaBlendToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function AlphaBlendToy::reset( %this )
{
    // Reset the sprite count.
    AlphaBlendToy.SpriteCount = 0;
    
    // Calculate the sprite configuration.
    AlphaBlendToy.SpriteCountX = mFloor(100.0 / AlphaBlendToy.SpriteSize);
    AlphaBlendToy.SpriteCountY = mFloor(75.0 / AlphaBlendToy.SpriteSize);
    AlphaBlendToy.SpriteCountTotal = AlphaBlendToy.SpriteCountX * AlphaBlendToy.SpriteCountY;
    AlphaBlendToy.SpriteAlphaStride = 1.0 / AlphaBlendToy.SpriteCountTotal;
    AlphaBlendToy.CursorX = 0;
    AlphaBlendToy.CursorY = 0;   
    AlphaBlendToy.CursorAlpha = 1.0; 
    
    // Clear the scene.
    SandboxScene.clear();
    
    // Create background.
    %this.createBackground();

    // Start the timer.
    AlphaBlendToy.startTimer( "createBlendedSprites", 50 );
}

//-----------------------------------------------------------------------------

function AlphaBlendToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Scroller();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set an image.
    %object.Image = "ToyAssets:checkered";
    
    // Repeat the image.
    %object.RepeatX = 4;
    %object.RepeatY = 3;
    
    // Scroll the image. 
    %object.ScrollX = 4;
    %object.ScrollY = 3;
    
    // Set the blend color.
    %object.BlendColor = DimGray;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function AlphaBlendToy::createBlendedSprites( %this )
{
    // Finish if count reached.
    if ( AlphaBlendToy.SpriteCount >= AlphaBlendToy.SpriteCountTotal )
    {
        // Stop the timer.
        AlphaBlendToy.stopTimer();
        
        return;
    }
        
    // Create the sprite.
    %object = new Sprite();
    
    // Calculate sprite position.
    %halfSize = AlphaBlendToy.SpriteSize * 0.5;
    %positionX = -50 + (AlphaBlendToy.CursorX * AlphaBlendToy.SpriteSize) + %halfSize;
    %positionY = 37.5-(AlphaBlendToy.CursorY * AlphaBlendToy.SpriteSize) - %halfSize;
    
    // Set the position.
    %object.setPosition( %positionX, %positionY );
    
    // Set a useful size.
    %object.Size = AlphaBlendToy.SpriteSize;
    
    // Set the animation.
    %object.Animation = "ToyAssets:TD_Wizard_WalkSouth";

    // Set the transparency.
    %object.setBlendAlpha( AlphaBlendToy.CursorAlpha );    
        
    // Add to the scene.
    SandboxScene.add( %object );

    // Update the sprite cursor position.
    AlphaBlendToy.CursorX++;
    if ( AlphaBlendToy.CursorX == AlphaBlendToy.SpriteCountX )
    {
        AlphaBlendToy.CursorX = 0;
        AlphaBlendToy.CursorY++;
        if ( AlphaBlendToy.CursorY == AlphaBlendToy.SpriteCountY )
        {
            // Stop the timer.
            AlphaBlendToy.stopTimer();
            
            return;
        }
    }
    
    // Update the sprite cursor alpha.
    AlphaBlendToy.CursorAlpha -= AlphaBlendToy.SpriteAlphaStride ;  
    
    // Increase the sprite count.
    AlphaBlendToy.SpriteCount++; 
    
    // Finish if count reached.
    if ( AlphaBlendToy.SpriteCount == AlphaBlendToy.SpriteCountTotal )
    {
        // Stop the timer.
        AlphaBlendToy.stopTimer();       
    }
}

//-----------------------------------------------------------------------------

function AlphaBlendToy::setSpriteSize( %this, %value )
{
    %this.SpriteSize = %value;
}
