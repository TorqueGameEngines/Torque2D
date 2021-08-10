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

function MultiWindowToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
        
    // Configure the toy.
    MultiWindowToy.MaxGems = 10;
    MultiWindowToy.MountSubWindow = true;
    MultiWindowToy.SubWindowBackground = true;
    
    // Add configuration option.
    addNumericOption("Max Balls", 1, 100, 10, "setMaxGems", MultiWindowToy.MaxGems, true, "Sets the number of gems created.");
    addFlagOption("Mount Sub-Window", "setMountSubWindow", MultiWindowToy.MountSubWindow, true, "Whether mount the sub-window to a random gem or not." );
    addFlagOption("Sub-Window Background", "setSubWindowBackground", MultiWindowToy.SubWindowBackground, true, "Whether render the sub-window background or not." );
    
    // Create a scene window.
    new SceneWindow(SandboxWindow2)
    {
        Profile = SandboxWindowProfile;
        Position = "64 64";
        Extent = "320 240";
    };
            
    // Set it to the scene.
    SandboxWindow2.setScene( SandboxScene );
        
    // Add it as a child window.    
    SandboxWindow.add( SandboxWindow2 );
    
    // Add window to the toy so it is destroyed.
    MultiWindowToy.add( SandboxWindow2 );
    
    // Reset the toy.
    MultiWindowToy.reset();
}

//-----------------------------------------------------------------------------

function MultiWindowToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function MultiWindowToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
       
    // Create background.
    %this.createBackground();
          
    // Create gems.
    %this.createGems();
    
    // Reset the camera position if not mounted.    
    if ( !MultiWindowToy.MountSubWindow )
    {
        SandboxWindow2.setCameraPosition( "0 0" );
        SandboxWindow2.setCameraAngle( 0 );
    }
        
    // Set the scene layers to render.
    if ( MultiWindowToy.SubWindowBackground )        
    {
        // Render the layers #20 & #31 i.e. the gems and backgound.
        SandboxWindow2.setRenderLayers( "20 31" );    
    }
    else
    {
        // Render the layer #20 only i.e. the gems.
        SandboxWindow2.setRenderLayers( "20" );    
    }
}

//-----------------------------------------------------------------------------

function MultiWindowToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the sprite as "static" so it is not affected by gravity.
    %object.BodyType = static;
       
    // Always try to configure a scene-object prior to adding it to a scene for best performance.

    // Set the position.
    %object.Position = "0 0";

    // Set the size.        
    %object.Size = "400 300";
    
    // Set to the furthest background layer.
    %object.SceneLayer = 31;
    
    // Set the scroller to use an animation!
    %object.Image = "ToyAssets:jungleSky";
        
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 37.5 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, 37.5, 50, 37.5 );
    %object.createEdgeCollisionShape( -50, -34.5, 50, -34.5 );   
            
    // Add the sprite to the scene.
    SandboxScene.add( %object );        
}

//-----------------------------------------------------------------------------

function MultiWindowToy::createGems( %this )
{    
    // Create balls.
    for( %n = 0; %n < MultiWindowToy.MaxGems; %n++ )
    {
        // Create the sprite.
        %object = new Sprite();
        
        // Set the position.
        %object.Position = getRandom(-40,40) SPC getRandom(-30,30);
            
        // If the size is to be square then we can simply pass a single value.
        // This applies to any 'Vector2' engine type.
        %object.Size = 5;
        
        // Set the layer.
        %object.SceneLayer = 20;
        
        // Create a circle collision shape.
        %object.setDefaultRestitution( 1 );
        %object.setDefaultFriction( 0.2 );
        %object.createCircleCollisionShape( 2 );
        
        // Set velocities.
        %object.SetLinearVelocity( getRandom(-40,40) SPC getRandom(-30,30) );
        %object.SetAngularVelocity( getRandom(-15,15) );    
            
        // Add the sprite to the scene.
        SandboxScene.add( %object );

        // Mount to sub-window to the first object if selected.        
        if ( MultiWindowToy.MountSubWindow == true && %n == 0 )
        {
            // Set the sprite to use an image.
            %object.Image = "ToyAssets:CrossHair2";
            %object.BlendColor = LimeGreen;
            
            // Mount to the object.
            SandboxWindow2.mount( %object, "0 0", 0, true, false );
        }
        else
        {
            // Set the sprite to use an image.
            %object.Image = "ToyAssets:Gems";
            %object.Frame = getRandom(0,63);
        }
    }
}

//-----------------------------------------------------------------------------

function MultiWindowToy::setMaxGems(%this, %value)
{
    %this.MaxGems = %value;
}

//-----------------------------------------------------------------------------

function MultiWindowToy::setMountSubWindow(%this, %value)
{
    %this.MountSubWindow = %value;
}

//-----------------------------------------------------------------------------

function MultiWindowToy::setSubWindowBackground(%this, %value)
{
    %this.SubWindowBackground = %value;
}