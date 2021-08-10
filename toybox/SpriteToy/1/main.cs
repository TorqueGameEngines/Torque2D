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

function SpriteToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pan );
    
    // Reset the toy.
    SpriteToy.reset();
}

//-----------------------------------------------------------------------------

function SpriteToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function SpriteToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
       
    // Create a "static" sprite.
    %this.createStaticSprite();   
    
    // Create a "animated" sprite.
    %this.createAnimatedSprite();    
}

//-----------------------------------------------------------------------------

function SpriteToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "100 75";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set the scroller to use an animation!
    %object.Image = "ToyAssets:highlightBackground";
    
    // Set the blend color.
    %object.BlendColor = SlateGray;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function SpriteToy::createStaticSprite( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "-25 0";
        
    // If the size is to be square then we can simply pass a single value.
    // This applies to any 'Vector2' engine type.
    %object.Size = 40;
    
    // Set the sprite to use an image.  This is known as "static" image mode.
    %object.Image = "ToyAssets:Tiles";
    
    // We don't really need to do this as the frame is set to zero by default.
    %object.Frame = 0;
        
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function SpriteToy::createAnimatedSprite( %this )
{
    // Create the sprite.
    %object = new Sprite();
    
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "25 0";
        
    // If the size is to be square then we can simply pass a single value.
    // This applies to any 'Vector2' engine type.
    %object.Size = 40;
    
    // Set the sprite to use an animation.  This is known as "animated" image mode.
    %object.Animation = "ToyAssets:TileAnimation";
       
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}
