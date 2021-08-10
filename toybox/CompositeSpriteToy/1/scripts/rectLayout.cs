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

function CompositeSpriteToy::createRectLayout( %this )
{
    // Set the layer #0 sort mode to be depth.
    SandboxScene.setLayerSortMode( 0, "z" );
    
    // Create the composite sprite.
	%composite = new CompositeSprite();

    // Set the default sprite stride.
    // This is used in rectilinear layout mode to scale the specified logical position arguments.	
    %composite.setDefaultSpriteStride( 8, 8 );
    
    // Set the default sprite size used to a little less than the stride so we get a "gap"
    // in between the sprites.
    %composite.setDefaultSpriteSize( 6, 6 );

	// Set the batch layout mode.  We must do this before we add any sprites.
    %composite.SetBatchLayout( "rect" );

    // Set the batch sort mode for when we're render isolated.
    %composite.SetBatchSortMode( "z" );

    // Set the batch render isolation.
    %composite.SetBatchIsolated( CompositeSpriteToy.RenderIsolated );

	// Calculate a range.
	%range = mSqrt( CompositeSpriteToy.SpriteCount ) * 0.5;
	if ( %range < 1 ) %range = 1;
	
	// Add some sprites.
    for ( %y = -%range; %y <= %range; %y++ )
	{
	    for ( %x = -%range; %x <= %range; %x++ )
        {                
            // Add a sprite with the specified logical position.
	        // In rectilinear layout this two-part position is scaled by the default sprite-stride.
            %composite.addSprite( %x SPC %y );
            
            // The sprite is automatically selected when it is added so
            // we can perform operations on it immediately.        
                        
            // Set the sprite image with a random frame.
            // We could also use an animation here.         
            %composite.setSpriteImage( "ToyAssets:Blocks", getRandom(0,55) );            
            
            // Set an interesting angle.
            %composite.setSpriteAngle( (%x+%y) * 10 );
                                    
            // Set the sprite spinning to make it more interesting.
            %composite.setAngularVelocity( CompositeSpriteToy.AngularVelocity );           
            
            // Set a random depth.
            %composite.SetSpriteDepth( getRandom( -10.0, 10 ) );                   
        }
	}
	
	// Add to the scene.
	SandboxScene.add( %composite );
	
	// Set the composite sprite toy.
	CompositeSpriteToy.CompositeSprite = %composite;	
}
