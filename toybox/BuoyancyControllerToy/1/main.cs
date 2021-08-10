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

function BuoyancyControllerToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );
    
    // Set the manipulation mode.
    Sandbox.useManipulation( pull );
    
    // Set gravity.
    SandboxScene.setGravity( 0, -29.8 );
    
    // Configure settings.
    BuoyancyControllerToy.FluidArea = "-30 -34 30 0";
    BuoyancyControllerToy.FluidDensity = 2.5;
    BuoyancyControllerToy.LinearDrag = 3;
    BuoyancyControllerToy.AngularDrag = 3;
    BuoyancyControllerToy.FluidGravity = "0 -29.8";    
    BuoyancyControllerToy.UseShapeDensity = false;

    BuoyancyControllerToy.FlowMagnitude = 13;
    
    BuoyancyControllerToy.MaxDebrisCount = 50;
    BuoyancyControllerToy.MinDebrisDensity = 1;
    BuoyancyControllerToy.MaxDebrisDensity = 1;
    
    BuoyancyControllerToy.GroundWidth = 140;

    // Add options.    
    addNumericOption("Fluid Density", 0, 10, 0.1, "setFluidDensity", BuoyancyControllerToy.FluidDensity, false, "The fluid density.");   
    addNumericOption("Fluid Flow Magnitude", 0, 1000, 10, "setFluidFlowMagnitude", BuoyancyControllerToy.FlowMagnitude, true, "The magnitude of the constant force.");      
    addNumericOption("Linear Drag", 0, 10, 0.1, "setLinearDrag", BuoyancyControllerToy.LinearDrag, false, "The linear drag co-efficient for the fluid.");
    addNumericOption("Angular Drag", 0, 10, 0.1, "setAngularDrag", BuoyancyControllerToy.AngularDrag, false, "The angular drag co-efficient for the fluid.");
    addNumericOption("Maximum Debris Count", 1, 500, 1, "setMaxDebrisCount", BuoyancyControllerToy.MaxDebrisCount, true, "The maximum number of debris to create.");
    addNumericOption("Minimum Debris Density", 0, 10, 0.1, "setMinDebrisDensity", BuoyancyControllerToy.MinDebrisDensity, true, "The minimum density to randomly choose for the debris.");
    addNumericOption("Maximum Debris Density", 0, 10, 0.1, "setMaxDebrisDensity", BuoyancyControllerToy.MaxDebrisDensity, true, "The maximum density to randomly choose for the debris.");   
    addFlagOption("Use Collision Shape Densities", "setUseShapeDensity", BuoyancyControllerToy.UseShapeDensity, false, "Whether to use the collision shape densities or assume a uniform density." );
    
    // Reset the toy.
    BuoyancyControllerToy.reset();
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Calculate fluid bounds to help layout only.
    BuoyancyControllerToy.FluidLeft = BuoyancyControllerToy.FluidArea._0;
    BuoyancyControllerToy.FluidBottom = BuoyancyControllerToy.FluidArea._1;
    BuoyancyControllerToy.FluidRight = BuoyancyControllerToy.FluidArea._2;
    BuoyancyControllerToy.FluidTop = BuoyancyControllerToy.FluidArea._3;
    
    // Create a background.
    %this.createBackground();
   
    // Create fluid.
    %this.createFluid( "-50 20 30 30", "1 0", Navy );
    %this.createFluid( "-10 2 50 12", "-1 0", Cyan );
    %this.createFluid("-40 -16 30 -6", "1 0", LightBlue );
    %this.createFluid( "-50 -34 50 -24", "-1 0", Green );
    %this.createFluid( "-50 -34 -40 30", "0 0", Blue );

    // Update the buoyancy controllers.
    %this.updateBuoyancyControllers();     
    
    // Create debris.    
    %this.createDebris();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createBackground( %this )
{    
    // Create the sprite.
    %object = new Sprite();
    %object.BodyType = static;
    %object.Position = "0 0";
    %object.Size = "100 75";
    %object.SceneLayer = 31;
    %object.Image = "ToyAssets:highlightBackground";
    %object.BlendColor = SlateGray;
    SandboxScene.add( %object );

    %object.setDefaultFriction( 0 );    
    
    // Create border collisions.
    %object.createEdgeCollisionShape( -50, -37.5, -50, 100 );
    %object.createEdgeCollisionShape( 50, -37.5, 50, 100 );
    %object.createEdgeCollisionShape( -50, -34, 50, -34 );
    
    // Vertical fluid edge.
    %object.createEdgeCollisionShape( -39.8, -16.2, -39.8, 20 );    
    %object.createEdgeCollisionShape( -39.8, -16.2, -20, -16.2 );    
                
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createSprite( %this, %asset, %position, %size, %angle )
{    
    // Create the sprite.
    %object = new Sprite();
    
    // Set the position.
    %object.Position = %position;

    // Set the size.        
    %object.Size = %size;
    
    // Set the angle.
    %object.Angle = %angle;
       
    // Set the scroller to use an animation!
    %object.Image = %asset;
    
    // Set the scene layer.
    %object.SceneLayer = 10;
    
    // Add the sprite to the scene.
    SandboxScene.add( %object );    
    
    return %object;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createDebris( %this )
{  
    // Fetch the debris count.
    %debrisCount = BuoyancyControllerToy.MaxDebrisCount;

    for( %debris = 0; %debris < %debrisCount; %debris++ )
    {
        // Choose a random debris (polygon or circle shapes).
        if ( getRandom(0,100) < 10 )
        {    
            %size = 2;
            
            // Create some sprites.
            %sprite = %this.createSprite( "ToyAssets:football", getRandom(-45,20) SPC getRandom(40,50), %size, getRandom(0,360) );
            %sprite.setDefaultFriction( 0.5 );
            %sprite.setDefaultDensity( getRandom(%this.MinDebrisDensity, %this.MaxDebrisDensity) );
            %sprite.createCircleCollisionShape( %size * 0.5 );
            %sprite.setAngularVelocity(getRandom(-180,180));
        }
        else
        {    
            %sizeX = getRandom(1,5);
            %sizeY = getRandom(1,2);
            %size = %sizeX SPC %sizeY;
            
            // Create some sprites.
            %sprite = %this.createSprite( "ToyAssets:Blocks", getRandom(-45,20) SPC getRandom(40,50), %size, getRandom(0,360) );
            %sprite.Frame = getRandom( 0, 55 );
            %sprite.setDefaultFriction( 0.5 );
            %sprite.setDefaultDensity( getRandom(%this.MinDebrisDensity, %this.MaxDebrisDensity) );
            %sprite.createPolygonBoxCollisionShape( %sizeX, %sizeY );
            %sprite.setAngularVelocity(getRandom(-180,180));
        }
    }    
}


//-----------------------------------------------------------------------------

function BuoyancyControllerToy::createFluid( %this, %area, %flowVelocity, %color )
{
    %areaLeft = %area._0;
    %areaBottom = %area._1;
    %areaRight = %area._2;
    %areaTop = %area._3;

    %crestY = %areaTop - 3;
    %width = %areaRight - %areaLeft;
    
    // Create a new controller.
    %controller = new BuoyancyController();
    %controller.FluidArea = %area;
    %controller.FlowVelocity = %flowVelocity;
    SandboxScene.Controllers.add( %controller );     
    
    // Add the water.
    %water = new Sprite();
    %water.BodyType = static;
    %water.setArea( %areaLeft SPC %areaBottom SPC %areaRight SPC %crestY);
    %water.Image = "ToyAssets:Blank";
    %water.BlendColor = %color;    
    %water.SetBlendAlpha( 0.5 );
    SandboxScene.add( %water );

    // Create some wave crests.
    %crests1 = new Scroller();   
    %crests1.setArea( %areaLeft SPC %crestY SPC %areaRight SPC %areaTop );
    %crests1.Image = "BuoyancyControllerToy:WaveCrests";
    %crests1.BlendColor = %color;    
    %crests1.SetBlendAlpha( 0.3 );
    %crests1.RepeatX = %width / 50;
    %crests1.ScrollX = getRandom(5,10);
    %crests1.ScrollPositionX = getRandom(0,50);
    SandboxScene.add( %crests1 );
    
    // Create some wave crests.
    %crests2 = new Scroller();   
    %crests2.setArea( %areaLeft SPC %crestY SPC %areaRight SPC %areaTop );
    %crests2.Image = "BuoyancyControllerToy:WaveCrests";
    %crests2.BlendColor = %color;    
    %crests2.SetBlendAlpha( 0.3 );
    %crests2.RepeatX = %width / 50;
    %crests2.ScrollX = getRandom(-5,-10);
    %crests2.ScrollPositionX = getRandom(0,50);
    SandboxScene.add( %crests2 );         
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidFlowMagnitude(%this, %value)
{
    %this.FlowMagnitude = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidDensity(%this, %value)
{
    %this.FluidDensity = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setLinearDrag(%this, %value)
{
    %this.LinearDrag = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setAngularDrag(%this, %value)
{
    %this.AngularDrag = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setFluidGravity(%this, %value)
{
    %this.FluidGravity = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setUseShapeDensity(%this, %value)
{
    %this.UseShapeDensity = %value;
    
    // Update the controllers.
    %this.updateBuoyancyControllers();   
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMaxDebrisCount(%this, %value)
{
    %this.MaxDebrisCount = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMinDebrisDensity(%this, %value)
{
    %this.MinDebrisDensity = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::setMaxDebrisDensity(%this, %value)
{
    %this.MaxDebrisDensity = %value;
}

//-----------------------------------------------------------------------------

function BuoyancyControllerToy::updateBuoyancyControllers( %this )
{
    // Fetch the controller set.
    %controllerSet = SandboxScene.Controllers;
    
    // Fetch controller count.
    %controllerCount = %controllerSet.getCount();
    
    // Iterate controllers.
    for( %i = 0; %i < %controllerCount; %i++ )
    {
        %controller = %controllerSet.getObject(%i);
        if ( %controller.getClassName() !$= "BuoyancyController" )
            continue;

        // Update the controllers.
        %controller.FluidDensity = BuoyancyControllerToy.FluidDensity;
        %controller.FlowVelocity = Vector2Direction( Vector2AngleToPoint("0 0",%controller.FlowVelocity), %this.FlowMagnitude );
        %controller.LinearDrag = BuoyancyControllerToy.LinearDrag;
        %controller.AngularDrag = BuoyancyControllerToy.AngularDrag;
        %controller.FluidGravity = BuoyancyControllerToy.FluidGravity;
        %controller.UseShapeDensity = BuoyancyControllerToy.UseShapeDensity;        
    }
}
