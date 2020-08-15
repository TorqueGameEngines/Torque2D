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

function SpriteStressToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pan );
    
    // Turn-off the full metrics.
    setMetricsOption( false );
    
    // Turn-on the FPS metrics only.
    setFPSMetricsOption( true );
    
    // Configure the toy.
    SpriteStressToy.SpriteSceneLayer = 10;
    SpriteStressToy.MaxSprite = 100;
    SpriteStressToy.SpriteCreateRate = 50;
    SpriteStressToy.SpriteCreatePeriod = 100;
    SpriteStressToy.SpriteSize = 8;
    SpriteStressToy.RandomAngle = false;
    SpriteStressToy.RenderMode = "Static";
    SpriteStressToy.RenderSortMode = "off";
    
    // Add the configuration options.
    addNumericOption("Sprite Maximum", 1, 100000, 100, "setSpriteMaximum", SpriteStressToy.MaxSprite, true, "Sets the maximum number of sprites to create." );
    addNumericOption("Sprite Size", 1, 75, 1, "setSpriteSize", SpriteStressToy.SpriteSize, true, "Sets the size of the sprites to create." );
    addFlagOption("Random Angle", "setSpriteRandomAngle", SpriteStressToy.RandomAngle, true, "Whether to place the sprites at a random angle or not." );    
    addNumericOption("Sprite Create Period (ms)", 10, 1000, 10, "setSpriteCreatePeriod", SpriteStressToy.SpriteCreatePeriod, true, "Sets the time interval for creating bursts of sprites.  Bigger values create the maximum sprites quicker." );
    addSelectionOption( "Static,Static (Composite),Animated,Animated (Composite)", "Render Mode", 4, "setRenderMode", true, "Sets the type of object used in the stress test." );
    addSelectionOption( "Off,New,Old,X,Y,Z,-X,-Y,-Z", "Render Sort Mode", 9, "setRenderSortMode", false, "Sets the render sorting mode controlling the order of rendering." );
    
    // Reset the toy.
    SpriteStressToy.reset();
}

//-----------------------------------------------------------------------------

function SpriteStressToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function SpriteStressToy::reset( %this )
{
    // Reset sprite count.
    SpriteStressToy.SpriteCount = 0;

    // Calculate sprite bounds.
    SpriteStressToy.HalfSize = SpriteStressToy.SpriteSize * 0.5;    
    SpriteStressToy.MinX = -50 + SpriteStressToy.HalfSize;
    SpriteStressToy.MaxX = 50 - SpriteStressToy.HalfSize;    
    SpriteStressToy.MinY = -37.5 + SpriteStressToy.HalfSize;
    SpriteStressToy.MaxY = 37.5 - SpriteStressToy.HalfSize;    
                
    // Clear the scene.
    SandboxScene.clear();

    // Update the render sort mode.
    %this.updateRenderSortMode();
                  
    // Create background.
    %this.createBackground();
            
    // Create sprite count overlay.
    %this.createSpriteCountOverlay();
    
    // Handle static sprites.
    if ( SpriteStressToy.RenderMode $= "Static" )
    {
        // Create the static sprites.
        SpriteStressToy.startTimer( "createStaticSprites", SpriteStressToy.SpriteCreatePeriod );
        
        return;
    }
    
    // Handle static composite sprites.
    if ( SpriteStressToy.RenderMode $= "Static (Composite)" )
    {
        // Create the static composite sprites.
        SpriteStressToy.startTimer( "createStaticCompositeSprites", SpriteStressToy.SpriteCreatePeriod );
    
        return;
    }
    
    // Handle animated sprites.
    if ( SpriteStressToy.RenderMode $= "Animated" )
    {
        // Create the animated sprites.
        SpriteStressToy.startTimer( "createAnimatedSprites", SpriteStressToy.SpriteCreatePeriod );
    
        return;
    }    
    
    // Handle animated composite sprites.
    if ( SpriteStressToy.RenderMode $= "Animated (Composite)" )
    {
        // Create the animated composite sprites.
        SpriteStressToy.startTimer( "createAnimatedCompositeSprites", SpriteStressToy.SpriteCreatePeriod );
    
        return;
    }    
    
    // Error.
    error( "SpriteStressToy::reset() - Unknown render mode." );
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createBackground( %this )
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
    %object.ScrollX = 4;
    %object.ScrollY = 3;
    %object.RepeatX = 4;
    %object.RepeatY = 3;
    
    // Set the blend color.
    %object.BlendColor = LightSteelBlue;
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createSpriteCountOverlay( %this )
{    
    // Create the image font.
    %object = new TextSprite();

    // Set the overlay font object.    
    SpriteStressToy.OverlayFontObject = %object;
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.setBodyType( static );
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.
	%object.Size = "24 24";
	
    // Set the position.
    %object.Position = "-35 -40";

    // Set the size.        
    %object.FontSize = 5;

    // Set the text alignment.
    %object.TextAlignment = Left;
        
    // Set to the nearest layer.
    %object.SceneLayer = 0;
    
    // Set a font image.
    %object.Font = "ToyAssets:OratorBoldFont";
    
    // Set the blend color.
    %object.BlendColor = White;
                
    // Add the sprite to the scene.
    SandboxScene.add( %object );  
    
    // Update the overlay.
    %this.updateSpriteCountOverlay();  
}

//-----------------------------------------------------------------------------

function SpriteStressToy::updateSpriteCountOverlay( %this )
{    
    // Update sprite count overlay.
    SpriteStressToy.OverlayFontObject.Text = SpriteStressToy.SpriteCount;    
}

//-----------------------------------------------------------------------------

function SpriteStressToy::updateRenderSortMode( %this )
{
    // Set the sprite layer sort mode.
    SandboxScene.setLayerSortMode( SpriteStressToy.SpriteSceneLayer, SpriteStressToy.RenderSortMode );  
    
    // Finish if no composite-sprite object.
    if ( !isObject(SpriteStressToy.CompositeSpriteObject) )
        return;
        
    // Set the batch sort mode.
    SpriteStressToy.CompositeSpriteObject.SetBatchSortMode( SpriteStressToy.RenderSortMode );        
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createStaticSprites( %this )
{
    // Finish if max sprites reached.
    if ( SpriteStressToy.SpriteCount >= SpriteStressToy.MaxSprite )
    {
        // Stop the timer.
        SpriteStressToy.stopTimer();
        
        return;
    }
        
    // Create the sprites at the specified rate.
    for( %n = 0; %n < SpriteStressToy.SpriteCreateRate; %n++ )
    {        
        // Create the sprite.
        %object = new Sprite();
                
        // Always try to configure a scene-object prior to adding it to a scene for best performance.

        // The sprite is static i.e. it won't move.
        %object.BodyType = static;

        // Set the position.
        %object.Position = getRandom(SpriteStressToy.MinX, SpriteStressToy.MaxX) SPC getRandom(SpriteStressToy.MinY, SpriteStressToy.MaxY);

        // Set to just in-front of the background layer.
        %object.SceneLayer = SpriteStressToy.SpriteSceneLayer;
        
        // Set a random scene-layer depth.
        %object.SceneLayerDepth = getRandom(-100,100);

        // Set the size of the sprite.            
        %object.Size = SpriteStressToy.SpriteSize;
        
        // Set a random angle if selected.
        if ( SpriteStressToy.RandomAngle )
            %object.Angle = getRandom(0,360);
               
        // Set the sprite to use an image.  This is known as "static" image mode.
        %object.Image = "ToyAssets:TD_Knight_CompSprite";
        
        // We don't really need to do this as the frame is set to zero by default.
        %object.Frame = getRandom(0,63);
            
        // Add the sprite to the scene.
        SandboxScene.add( %object );
        
        // Increase sprite count.
        SpriteStressToy.SpriteCount++;
        
        // Finish if max sprites reached.
        if ( SpriteStressToy.SpriteCount == SpriteStressToy.MaxSprite )
        {
            // Update the overlay.
            %this.updateSpriteCountOverlay();
            
            // Stop the timer.
            SpriteStressToy.stopTimer();         
            
            return;
        }
    }
    
    // Update the overlay.
    %this.updateSpriteCountOverlay();  
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createAnimatedSprites( %this )
{
    // Finish if max sprites reached.
    if ( SpriteStressToy.SpriteCount >= SpriteStressToy.MaxSprite )
    {
        // Stop the timer.
        SpriteStressToy.stopTimer();
        
        return;
    }

    // Create the sprites at the specified rate.
    for( %n = 0; %n < SpriteStressToy.SpriteCreateRate; %n++ )
    {           
        // Create the sprite.
        %object = new Sprite();
        
        // Always try to configure a scene-object prior to adding it to a scene for best performance.

        // The sprite is static i.e. it won't move.
        %object.BodyType = static;

        // Set the position.
        %object.Position = getRandom(SpriteStressToy.MinX, SpriteStressToy.MaxX) SPC getRandom(SpriteStressToy.MinY, SpriteStressToy.MaxY);
            
        // Set to just in-front of the background layer.
        %object.SceneLayer = SpriteStressToy.SpriteSceneLayer;
            
        // Set a random scene-layer depth.
        %object.SceneLayerDepth = getRandom(-100,100);
            
        // Set the size of the sprite.            
        %object.Size = SpriteStressToy.SpriteSize;

        // Set a random angle if selected.
        if ( SpriteStressToy.RandomAngle )
            %object.Angle = getRandom(0,360);
        
        // Set the sprite to use an animation.  This is known as "animated" image mode.
        %object.Animation = "ToyAssets:TD_Knight_MoveSouth";
           
        // Add the sprite to the scene.
        SandboxScene.add( %object );
                
        // Increase sprite count.
        SpriteStressToy.SpriteCount++;
        
        // Finish if max sprites reached.
        if ( SpriteStressToy.SpriteCount == SpriteStressToy.MaxSprite )
        {
            // Update the overlay.
            %this.updateSpriteCountOverlay();              
            
            // Stop the timer.
            SpriteStressToy.stopTimer();         
            
            return;
        }
    }
    
    // Update the overlay.
    %this.updateSpriteCountOverlay();  
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createStaticCompositeSprites( %this )
{
    // Finish if max sprites reached.
    if ( SpriteStressToy.SpriteCount >= SpriteStressToy.MaxSprite )
    {
        // Stop the timer.
        SpriteStressToy.stopTimer();
        
        return;
    }

    // Do we have a composite sprite yet?
    if ( !isObject(SpriteStressToy.CompositeSpriteObject) )
    {
        // No, so create it.
        %composite = new CompositeSprite();
        
        // Set the composite object.
        SpriteStressToy.CompositeSpriteObject = %composite;
        
        // The sprite is static i.e. it won't move.
        %composite.BodyType = static;
           
        // Set the batch layout mode.  We must do this before we add any sprites.
        %composite.SetBatchLayout( "off" );
                
        // Set the batch render isolation.
        %composite.SetBatchIsolated( SpriteStressToy.RenderSortMode );

        // Set to just in-front of the background layer.
        %composite.SceneLayer = SpriteStressToy.SpriteSceneLayer;
                
        // Set the batch to not cull because the sprites are always on the screen
        // and it's faster and takes less memory.
        %composite.setBatchCulling( false );
       
        // Add to the scene.
        SandboxScene.add( %composite );        
    }

    // Fetch the composite object.
    %composite = SpriteStressToy.CompositeSpriteObject;

    // Create the sprites at the specified rate.
    for( %n = 0; %n < SpriteStressToy.SpriteCreateRate; %n++ )
    { 
        // Add a sprite with no logical position.
        %composite.addSprite();
        
        // Set the sprites location position to a random location.
        %composite.setSpriteLocalPosition( getRandom(SpriteStressToy.MinX, SpriteStressToy.MaxX), getRandom(SpriteStressToy.MinY, SpriteStressToy.MaxY) );
                
        // Set a random size.
        %composite.setSpriteSize( SpriteStressToy.SpriteSize );
        
        // Set a random angle.
        if ( SpriteStressToy.RandomAngle )
            %composite.setSpriteAngle( getRandom(0,360) );

        // Set the sprite to use an image.
        %composite.setSpriteImage( "ToyAssets:TD_Knight_CompSprite", getRandom(0,63) );          
        
        // Set a random depth.
        %composite.SetSpriteDepth( getRandom( -100, 100 ) ); 
                        
        // Increase sprite count.
        SpriteStressToy.SpriteCount++;
        
        // Finish if max sprites reached.
        if ( SpriteStressToy.SpriteCount == SpriteStressToy.MaxSprite )
        {
            // Update the overlay.
            %this.updateSpriteCountOverlay();              
            
            // Stop the timer.
            SpriteStressToy.stopTimer();         
            
            return;
        }
    }
    
    // Update the overlay.
    %this.updateSpriteCountOverlay();  
}

//-----------------------------------------------------------------------------

function SpriteStressToy::createAnimatedCompositeSprites( %this )
{
    // Finish if max sprites reached.
    if ( SpriteStressToy.SpriteCount >= SpriteStressToy.MaxSprite )
    {
        // Stop the timer.
        SpriteStressToy.stopTimer();
        
        return;
    }

    // Do we have a composite sprite yet?
    if ( !isObject(SpriteStressToy.CompositeSpriteObject) )
    {
        // No, so create it.
        %composite = new CompositeSprite();
        
        // Set the composite object.
        SpriteStressToy.CompositeSpriteObject = %composite;
        
        // The sprite is static i.e. it won't move.
        %composite.BodyType = static;
           
        // Set the batch layout mode.  We must do this before we add any sprites.
        %composite.SetBatchLayout( "off" );
                
        // Set the batch render isolation.
        %composite.SetBatchIsolated( SpriteStressToy.RenderSortMode );

        // Set to just in-front of the background layer.
        %composite.SceneLayer = SpriteStressToy.SpriteSceneLayer;
                
        // Set the batch to not cull because the sprites are always on the screen
        // and it's faster and takes less memory.
        %composite.setBatchCulling( false );
       
        // Add to the scene.
        SandboxScene.add( %composite );        
    }

    // Fetch the composite object.
    %composite = SpriteStressToy.CompositeSpriteObject;

    // Create the sprites at the specified rate.
    for( %n = 0; %n < SpriteStressToy.SpriteCreateRate; %n++ )
    { 
        // Add a sprite with no logical position.
        %composite.addSprite();
        
        // Set the sprites location position to a random location.
        %composite.setSpriteLocalPosition( getRandom(SpriteStressToy.MinX, SpriteStressToy.MaxX), getRandom(SpriteStressToy.MinY, SpriteStressToy.MaxY) );
                
        // Set a random size.
        %composite.setSpriteSize( SpriteStressToy.SpriteSize );
        
        // Set a random angle.
        if ( SpriteStressToy.RandomAngle )
            %composite.setSpriteAngle( getRandom(0,360) );

        // Set the sprite to use an animation.
        %composite.setSpriteAnimation( "ToyAssets:TD_Knight_MoveSouth" );
        
        // Set a random depth.
        %composite.SetSpriteDepth( getRandom( -100, 100 ) ); 
                        
        // Increase sprite count.
        SpriteStressToy.SpriteCount++;
        
        // Finish if max sprites reached.
        if ( SpriteStressToy.SpriteCount == SpriteStressToy.MaxSprite )
        {
            // Update the overlay.
            %this.updateSpriteCountOverlay();              

            // Stop the timer.
            SpriteStressToy.stopTimer();         
            
            return;
        }
    }
    
    // Update the overlay.
    %this.updateSpriteCountOverlay();  
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setSpriteMaximum( %this, %value )
{
    SpriteStressToy.MaxSprite = %value;
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setSpriteSize( %this, %value )
{
    SpriteStressToy.SpriteSize = %value;
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setSpriteRandomAngle( %this, %value )
{
    SpriteStressToy.RandomAngle = %value;
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setSpriteCreatePeriod( %this, %value )
{
    SpriteStressToy.SpriteCreatePeriod = %value;
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setRenderMode( %this, %value )
{
    SpriteStressToy.RenderMode = %value;
}

//-----------------------------------------------------------------------------

function SpriteStressToy::setRenderSortMode( %this, %value )
{
    SpriteStressToy.RenderSortMode = %value;
    
    // Update the render sort mode.
    %this.updateRenderSortMode();    
}
