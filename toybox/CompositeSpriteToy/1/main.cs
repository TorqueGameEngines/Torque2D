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

function CompositeSpriteToy::create( %this )
{
    // Load scripts.
    exec( "./scripts/noLayout.cs" );
    exec( "./scripts/rectLayout.cs" );
    exec( "./scripts/isoLayout.cs" );
    exec( "./scripts/customLayout.cs" );
        
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pan );   

    // Configure the toy.
    CompositeSpriteToy.LayoutMode = "None";
    CompositeSpriteToy.AngularVelocity = 15;
    CompositeSpriteToy.SpriteCount = 50;
    CompositeSpriteToy.RenderIsolated = false;

    // Add the configuration options.
    addSelectionOption( "None,Custom,Rectilinear,Isometric", "Layout Mode", 4, "setLayoutMode", true, "Selects the layout mode for the composite sprite." );
    addNumericOption("Maximum Sprite Count", 10, 1000, 10, "setSpriteCount", CompositeSpriteToy.SpriteCount, true, "Sets the maximum number of sprites to create." );
    addNumericOption("Angular Velocity", -180, 180, 20, "setAngularVelocity", CompositeSpriteToy.AngularVelocity, false, "Sets the rate at which the composite sprite spins." );    
    addFlagOption("Render Isolated", "setRenderIsolated", CompositeSpriteToy.RenderIsolated, true , "Whether the composite renders its sprites isolated from the scene layer it occupies or not.");
    
    // Reset the toy.
    %this.reset();     
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Create the background.
    %this.createBackground();
    
    // Create the appropriate layout.
    switch$( CompositeSpriteToy.LayoutMode )
    {
        case "None":
            %this.createNoLayout();
            
        case "Custom":
            %this.createCustomLayout();
            
        case "Rectilinear":
            %this.createRectLayout();
            
        case "Isometric":
            %this.createIsoLayout();
    }
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::setLayoutMode( %this, %value )
{
    CompositeSpriteToy.LayoutMode = %value;
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::setAngularVelocity( %this, %value )
{
    CompositeSpriteToy.AngularVelocity = %value;
    
    // Update any active composite sprite.
	if ( isObject(CompositeSpriteToy.CompositeSprite) && CompositeSpriteToy.LayoutMode !$= "Isometric" )
	{
	    CompositeSpriteToy.CompositeSprite.setAngularVelocity( %value );
	}
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::setSpriteCount( %this, %value )
{
    CompositeSpriteToy.SpriteCount = %value;
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::setRenderIsolated( %this, %value )
{
    CompositeSpriteToy.RenderIsolated = %value;
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::createBackground(%this)
{
    // Create the checkered background.
    %obj = new Scroller();
    %obj.Image = "ToyAssets:checkered";
    %obj.BlendColor = SlateGray;
    %obj.Size = 200;
    %obj.RepeatX = 8;
    %obj.RepeatY = 8;
    %obj.ScrollX = 10;
    %obj.ScrollY = 7;
    
    // Add to the scene.
    SandboxScene.add( %obj );   
}

//-----------------------------------------------------------------------------

function CompositeSpriteToy::onTouchDown(%this, %touchID, %worldPosition)
{
    // Fetch the composite sprite.
    %compositeSprite = CompositeSpriteToy.CompositeSprite;
    
    // Pick sprites.
    %sprites = %compositeSprite.pickPoint( %worldPosition );    

    // Fetch sprite count.    
    %spriteCount = %sprites.count;
    
    // Finish if no sprites picked.
    if ( %spriteCount == 0 )
        return;    
        
    // Iterate sprites.
    for( %i = 0; %i < %spriteCount; %i++ )
    {
        // Fetch sprite Id.
        %spriteId = getWord( %sprites, %i );
        
        // Select the sprite Id.
        %compositeSprite.selectSpriteId( %spriteId );
        
        // Remove the se
        %compositeSprite.removeSprite();
    }
}
