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

function AngleToy::create( %this )
{        
    // Reset the toy initially.
    AngleToy.reset();
}

//-----------------------------------------------------------------------------

function AngleToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function AngleToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Create background.
    %this.createBackground();
    
    // Create the target.
    %this.createTargets();
    
    // Create Mathematical Objects/Labels
    %this.createMathematicalObjects();
    
    AngleToy.repointTarget = "0 0";
}

//-----------------------------------------------------------------------------

function AngleToy::createBackground( %this )
{    
    // Create the Polar Coordinate Background
    %axisColor = "0.4 0.4 0.4";
    %circleColor = "0.2 0.2 0.2";
    %textColor = "1 1 0";
    
    // X-Axis
    %object = new ShapeVector();
    %object.setBodyType( static );
    %object.Position = "0 0";
    %object.Size = "100 0";
    %object.SceneLayer = 31;
    %object.LineColor = %axisColor;
    %object.FillMode = false;
    %object.setPolyCustom( 2, "-1 0 1 0" );
    SandboxScene.add( %object );    
  
    // Y-Axis
    %object = new ShapeVector();
    %object.setBodyType( static );
    %object.Position = "0 0";
    %object.Size = "0 25";
    %object.SceneLayer = 31;
    %object.LineColor = %axisColor;
    %object.FillMode = false;
    %object.setPolyCustom( 2, "0 -1 0 1" );
    SandboxScene.add( %object );
    
    // Radius Circles
    for( %i = 1; %i <= 2; %i++ )
    {
        %object = new ShapeVector();
        %object.setBodyType( static );
        %object.Position = "0 0";
        %object.Size = "20 20";
        %object.SceneLayer = 31;
        %object.LineColor = %circleColor;
        %object.FillMode = false;
        %object.IsCircle = true;
        %object.CircleRadius = %i * 10;
        SandboxScene.add( %object );
    }
    
    // Angle Labels
    for( %i = -165; %i <= 180; %i += 15 )
    {
        %object = new TextSprite();
        %object.Font = "ToyAssets:ArialFont";
		%object.Size = "1.5 1.5";
        %object.Position = Vector2Direction( %i, 30 ); // Polar ( 30, %i° )
        %object.Angle = %i - 90;
        %object.FontSize = 1.5;
        %object.TextAlignment = "Center";
        %object.BlendColor = %textColor;
        %object.Text = %i;
        SandboxScene.add( %object );    
    }
}

//-----------------------------------------------------------------------------

function AngleToy::createTargets( %this )
{
    // Create the sprite.
    %object = new Sprite() { class = "MagicTarget"; };
    AngleToy.TargetObject = %object;
    %object.Image = "ToyAssets:hollowArrow";
    %object.Size = 5;
    %object.setBodyType( dynamic );
    %object.startTimer( repoint, 100 );
    SandboxScene.add( %object );

    %effect = new ParticleAsset();
    %effect.AssetName = "DirectedParticles";
    
    %emitter = %effect.createEmitter();
    AngleToy.EmitterParameters = %emitter;
	%emitter.setEmitterType("ELLIPSE");
	//%emitter.setAttachPositionToEmitter(false);
	%emitter.setEmitterSize("40 40");
	%emitter.IsTargeting = true;
	%emitter.setTargetPosition("0.0 0.0");
    %emitter.EmitterName = "AngledParticles";
    %emitter.setKeepAligned( true );
    %emitter.setOrientationType( ALIGNED );
    %emitter.Image = "ToyAssets:Crosshair3";
    %emitter.selectField( "Lifetime" );
    %emitter.addDataKey( 0, 1 );
    %emitter.selectField( "Quantity" );
    %emitter.addDataKey( 0, 250 );
    %emitter.selectField( "Speed" );
    %emitter.addDataKey( 0, 4 );
    %emitter.selectField( "EmissionArc" );
    %emitter.addDataKey( 0, 0 );
    %emitter.selectField( "EmissionForceVariation" );
    %emitter.addDataKey( 0, 0 );
    %emitter.deselectField();
	%emitter.dump();
    
    %assetId = AssetDatabase.addPrivateAsset( %effect );
    
    %object = new ParticlePlayer();
    AngleToy.TargetParticles = %object;
    %object.BodyType = static;
    %object.Particle = %assetId;
    
    SandboxScene.add( %object );      
}

//-----------------------------------------------------------------------------

function AngleToy::createMathematicalObjects( %this )
{
    %lineSegmentColor = "0.25 0.25 0.75";
  
    %object = new Sprite() { class = "LineSegment"; };
    AngleToy.SinLineSegment = %object;
    %object.Image = "ToyAssets:Blank";
    %object.setBodyType( static );
    %object.BlendColor = %lineSegmentColor;
    SandboxScene.add( %object );
    
    %object = new Sprite() { class = "LineSegment"; };
    AngleToy.CosLineSegment = %object;
    %object.Image = "ToyAssets:Blank";
    %object.setBodyType( static );
    %object.BlendColor = %lineSegmentColor;
    SandboxScene.add( %object );    
    
    %object = new Sprite() { class = "LineSegment"; };
    AngleToy.TanLineSegment = %object;
    %object.Image = "ToyAssets:Blank";
    %object.setBodyType( static );
    %object.BlendColor = %lineSegmentColor;
    SandboxScene.add( %object );
    
    %object = new TextSprite();
    AngleToy.SinLabel = %object;
    %object.Font = "ToyAssets:OratorBoldFont";
    %object.FontSize = 2;
	%object.Size = "1.5 1.5";
    %object.TextAlignment = Left;
	%object.OverflowModeX = "visible";
    %object.BlendColor = %lineSegmentColor;
	%object.Visible = 0;
	%object.setBodyType( static );
    SandboxScene.add( %object );
    
    %object = new TextSprite();
    AngleToy.CosLabel = %object;
    %object.Font = "ToyAssets:OratorBoldFont";
    %object.FontSize = 2;
	%object.Size = "1.5 1.5";
    %object.TextAlignment = Left;
	%object.OverflowModeX = "visible";
    %object.BlendColor = %lineSegmentColor;
	%object.Visible = 0;
	%object.setBodyType( static );
    SandboxScene.add( %object );
    
    %object = new TextSprite();
    AngleToy.TanLabel = %object;
    %object.Font = "ToyAssets:OratorBoldFont";
    %object.FontSize = 2;
	%object.Size = "1.5 1.5";
    %object.TextAlignment = Left;
	%object.OverflowModeX = "visible";
    %object.BlendColor = %lineSegmentColor;
	%object.Visible = 0;
	%object.setBodyType( static );
    SandboxScene.add( %object );
}

//-----------------------------------------------------------------------------

function AngleToy::onTouchDown(%this, %touchID, %worldPosition)
{
    // Used to let the repointing target kno  
    AngleToy.repointTarget = %worldPosition;

    // Calculate the angle to the mouse.
    %angle = mAtan( %worldPosition );
    
    // "Point" particles towards cursor
    AngleToy.EmitterParameters.setTargetPosition(%worldPosition);
    
    // Show Sin, Cos, Tan
    %sin = mSin( %angle );
    %cos = mCos( %angle );
    %tan = mTan( %angle );

    // Find the vector that's 20/21 units from the center in the direction of
    // %worldPosition.  Here are a couple of ways to do that:
    %worldPositionAtRadius20 = Vector2Direction( %angle, 20 );
    %worldPositionAtRadius21 = Vector2Scale( Vector2Normalize( %worldPosition ), 21 );

    // Draw the Sine    
    %onYAxis = setWord( %worldPositionAtRadius20, 0, 0 ); // Set the X-component to 0
    AngleToy.SinLineSegment.draw( %worldPositionAtRadius20, %onYAxis );
    AngleToy.SinLabel.setPosition( Vector2Add( %onYAxis, "0 -1" ) );
    AngleToy.SinLabel.setText( mFloatLength( %sin, 4 ) );
    AngleToy.SinLabel.Visible = 1;
	
    // Draw the Cosine
    %onXAxis = setWord( %worldPositionAtRadius20, 1, 0 ); // Set the Y-component to 0
    AngleToy.CosLineSegment.draw( %worldPositionAtRadius20, %onXAxis );
    AngleToy.CosLabel.setPosition( Vector2Add( %onXAxis, "-1 0" ) );
    AngleToy.CosLabel.setAngle( 90 );
    AngleToy.CosLabel.setText( mFloatLength( %cos, 4 ) );
    AngleToy.CosLabel.Visible = 1;
	
    // Draw the Tangent
    AngleToy.TanLineSegment.drawTangent( %worldPositionAtRadius20, %tan, %angle );
    AngleToy.TanLabel.setPosition( %worldPositionAtRadius21 );
    AngleToy.TanLabel.setAngle( %angle - 90 );
    AngleToy.TanLabel.setText( mFloatLength( %tan, 4 ) );
	AngleToy.TanLabel.Visible = 1;
}

//-----------------------------------------------------------------------------

function AngleToy::onTouchDragged(%this, %touchID, %worldPosition)
{
    // Used to let the repointing target kno  
    AngleToy.repointTarget = %worldPosition;

    // Calculate the angle to the mouse.
    %angle = mAtan( %worldPosition );
    
    // "Point" particles towards cursor
    //AngleToy.EmitterParameters.selectField( "EmissionAngle" );
    //AngleToy.EmitterParameters.addDataKey( 0, %angle );
    //AngleToy.EmitterParameters.deselectField();
	AngleToy.EmitterParameters.setTargetPosition(%worldPosition);
    
    // Show Sin, Cos, Tan
    %sin = mSin( %angle );
    %cos = mCos( %angle );
    %tan = mTan( %angle );

    // Find the vector that's 20/21 units from the center in the direction of
    // %worldPosition.  Here are a couple of ways to do that:
    %worldPositionAtRadius20 = Vector2Direction( %angle, 20 );
    %worldPositionAtRadius21 = Vector2Scale( Vector2Normalize( %worldPosition ), 21 );

    // Draw the Sine    
    %onYAxis = setWord( %worldPositionAtRadius20, 0, 0 ); // Set the X-component to 0
    AngleToy.SinLineSegment.draw( %worldPositionAtRadius20, %onYAxis );
    AngleToy.SinLabel.setPosition( Vector2Add( %onYAxis, "0 -1" ) );
    AngleToy.SinLabel.setText( mFloatLength( %sin, 4 ) );
    
    // Draw the Cosine
    %onXAxis = setWord( %worldPositionAtRadius20, 1, 0 ); // Set the Y-component to 0
    AngleToy.CosLineSegment.draw( %worldPositionAtRadius20, %onXAxis );
    AngleToy.CosLabel.setPosition( Vector2Add( %onXAxis, "-1 0" ) );
    AngleToy.CosLabel.setAngle( 90 );
    AngleToy.CosLabel.setText( mFloatLength( %cos, 4 ) );
    
    // Draw the Tangent
    AngleToy.TanLineSegment.drawTangent( %worldPositionAtRadius20, %tan, %angle );
    AngleToy.TanLabel.setPosition( %worldPositionAtRadius21 );
    AngleToy.TanLabel.setAngle( %angle - 90 );
    AngleToy.TanLabel.setText( mFloatLength( %tan, 4 ) );
}

//-----------------------------------------------------------------------------

function MagicTarget::repoint( %this )
{
    %myPosition = %this.getPosition();
    %angle = Vector2AngleToPoint( %myPosition, AngleToy.repointTarget );
    %this.rotateTo( %angle - 90, 360 ); // Image points at 90 degrees, so we need to subtract that off.
    %this.setLinearVelocityPolar( %angle,
        Vector2Length( Vector2Sub( AngleToy.repointTarget, %myPosition  ) ) );
}

//-----------------------------------------------------------------------------

function LineSegment::draw( %this, %from, %to )
{
    %length = Vector2Distance( %from, %to );
    %width = 0.25;
    %mid = Vector2Scale( Vector2Add( %from, %to ), 0.5 ); // Mid-Point
    %angle = Vector2AngleToPoint( %from, %to );
    
    %this.setPosition( %mid );
    %this.setSize( %length, %width );
    %this.setAngle( %angle );
}

function LineSegment::drawTangent( %this, %from, %tan, %angleOnCircle )
{
    // One of the many, many definitions of tangent is that the line that
    // is tangent to the circle will intersect the X-axis with a length of
    // %tangent.  It's fun, so we'll use it for this example.

    // The tangent to a circle at a given angle is equal to +/- 90 degrees.
    %tangentAngle = %angleOnCircle - 90;
    
    // Sine, cosine, and tangent assume a unit circle.  Let's scale to our 20 units.
    %length = %tan * 20; 
    
    %tangentEnd = Vector2Add( %from, Vector2Direction( %tangentAngle, %length ) );
    
    %this.draw( %from, %tangentEnd );
}
