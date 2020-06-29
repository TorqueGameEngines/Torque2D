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

ConsoleMethodGroupBeginWithDocs(ParticleAssetEmitter, SimObject)

//-----------------------------------------------------------------------------
/// Particle emitter accessors.
//-----------------------------------------------------------------------------

/*! Gets the asset owner of the emitter.
    @return The asset owner of the emitter or nothing if no owner assigned.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getOwner, ConsoleString, 2, 2, ())
{
    // Fetch the owner.
    ParticleAsset* pParticleAsset = object->getOwner();

    return pParticleAsset == NULL ? StringTable->EmptyString : pParticleAsset->getIdString();
}

//-----------------------------------------------------------------------------

/*! Sets the name of the emitter.
    @param emitterName The name to set the emitter to.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setEmitterName, ConsoleVoid, 3, 3, (emitterName))
{
    object->setEmitterName( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the name of the emitter.
    @return The name of the emitter.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getEmitterName, ConsoleString, 2, 2, ())
{
    return object->getEmitterName();
}

//-----------------------------------------------------------------------------

/*! Sets the type of the emitter.
    @param emitterType The type to set the emitter.  Either 'POINT', 'LINE', 'BOX' or 'DISK', 'ELLIPSE' or 'TORUS'.
    An emitter-type of 'POINT' creates the particles at the position of the particle asset.
    An emitter-type of 'LINE' creates the particles along a line defined by the particle width.
    An emitter-type of 'BOX' creates the particles within the dimensions defined by the particle size.
    An emitter-type of 'DISK' creates the particles within a disk with radii defined by the particle size.
    An emitter-type of 'ELLIPSE' creates the particles on an ellipse with the radii defined by the particle size.
    An emitter-type of 'TORUS' creates the particles within a torus with a maximum and minimum radii defined by the particle width and height respectively.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setEmitterType, ConsoleVoid, 3, 3, (emitterType))
{
    object->setEmitterType( ParticleAssetEmitter::getEmitterTypeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the type of the emitter.
    @return The type of the emitter.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getEmitterType, ConsoleString, 2, 2, ())
{
    return ParticleAssetEmitter::getEmitterTypeDescription( object->getEmitterType() );
}

//-----------------------------------------------------------------------------

/*! Offsets the position of the emitter relative to the effect or player position.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setEmitterOffset, ConsoleVoid, 3, 4, (float X / float Y))
{
    // Grab the element count.
    U32 elementCount =Utility::mGetStringElementCount(argv[2]);
    
    // ("positionX positionY")
    if ( (elementCount == 2) && (argc < 4) )
    {
        object->setEmitterOffset( Vector2( argv[2] ) );
        return;
    }
    
    // (positionX, positionY)
    if ( (elementCount == 1) && (argc > 3) )
    {
        object->setEmitterOffset( Vector2( dAtof(argv[2]), dAtof(argv[3]) ) );
        return;
    }
    
    // Warn.
    Con::warnf( "ParticleAssetEmitter::setEmitterOffset() - Invalid number of parameters!" );
}

//------------------------------------------------------------------------------

/*! Gets the emitter offset position.
    @return (float x/float y) The offset of the emitter relative to the effect or player position.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getEmitterOffset, ConsoleString, 2, 2, ())
{
    return object->getEmitterOffset().scriptThis();
}

/*! Sets if the emitter targets a point.
    @param target The point that the emitter's particles will be drawn to.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setIsTargeting, ConsoleVoid, 3, 3, (target))
{
   object->setIsTargeting(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets whether the emitter targets a point.
    @return True if the emitter uses a target for particles.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getIsTargeting, ConsoleBool, 2, 2, ())
{
   return object->getIsTargeting();
}

/* Sets the world point that the emitter will target. Targeting must be turned on with setIsTargeting().
    @param Position The world point that will be the target.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setTargetPosition, ConsoleVoid, 3, 4, (float X / float Y))
{
   // Grab the element count.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("positionX positionY")
   if ((elementCount == 2) && (argc < 4))
   {
      object->setTargetPosition(Vector2(argv[2]));
      return;
   }

   // (positionX, positionY)
   if ((elementCount == 1) && (argc > 3))
   {
      object->setTargetPosition(Vector2(dAtof(argv[2]), dAtof(argv[3])));
      return;
   }

   // Warn.
   Con::warnf("ParticleAssetEmitter::setTargetPosition() - Invalid number of parameters!");
}

//------------------------------------------------------------------------------

/*! Gets the emitter target position.
    @return (float x/float y) The position of the target of the emitter.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getTargetPosition, ConsoleString, 2, 2, ())
{
   return object->getTargetPosition().scriptThis();
}


//------------------------------------------------------------------------------

/*! Sets the emitter size.
    @param width The width of the emitter.
    @param height The height of the emitter.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setEmitterSize, ConsoleVoid, 3, 4, (width / height))
{
    F32 width, height;
    
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);
    
    // ("width height")
    if ((elementCount == 2) && (argc == 3))
    {
        width = dAtof(Utility::mGetStringElement(argv[2], 0));
        height = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    
    // (width, [height])
    else if (elementCount == 1)
    {
        width = dAtof(argv[2]);
        
        if (argc > 3)
            height = dAtof(argv[3]);
        else
            height = width;
    }
    
    // Invalid
    else
    {
        Con::warnf("ParticleAssetEmitter::setEmitterSize() - Invalid number of parameters!");
        return;
    }
    
    // Set Size.
    object->setEmitterSize(Vector2(width, height));
}

//------------------------------------------------------------------------------

/*! Gets the emitter size.
    @return (float width/float height) The width and height of the emitter.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getEmitterSize, ConsoleString, 2, 2, ())
{
    return object->getEmitterSize().scriptThis();
}

//------------------------------------------------------------------------------

/*! Sets the emitter angle.
    @param angle The angle of the emitter.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setEmitterAngle, ConsoleVoid, 3, 3, (angle))
{
    // Set Rotation.
    object->setEmitterAngle( mDegToRad( dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets the emitter angle.
    @return (float angle) The emitter's current angle.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getEmitterAngle, ConsoleFloat, 2, 2, ())
{
    // Return angle.
    return mRadToDeg( object->getEmitterAngle());
}

//-----------------------------------------------------------------------------

/*! Sets the emitter to used a fixed-aspect for particles.
    @param fixedAspect Whether to use a fixed-aspect or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setFixedAspect, ConsoleVoid, 3, 3, (fixedAspect))
{
    object->setFixedAspect( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the emitter uses a fixed-aspect for particles or not.
    @return Whether the emitter uses a fixed-aspect for particles or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getFixedAspect, ConsoleBool, 2, 2, ())
{
    return object->getFixedAspect();
}

//-----------------------------------------------------------------------------

/*! Sets the emitter fixed-force angle for particles.
    @param fixedForceAngle The fixed-force angle for particles.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setFixedForceAngle, ConsoleVoid, 3, 3, (fixedForceAngle))
{
    object->setFixedForceAngle( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the emitter fixed-force angle for particles.
    @return The fixed-force angle for particles.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getFixedForceAngle, ConsoleFloat, 2, 2, ())
{
    return object->getFixedForceAngle();
}

//-----------------------------------------------------------------------------

/*! Sets the orientation-type of the emitter.
    @param orientationType The orientation-type to set the emitter to.  Either 'FIXED', 'ALIGNED' or 'RANDOM'.
    An orientation-type of 'FIXED' causes the particles to be orientation at a fixed angle.
    An orientation-type of 'ALIGNED' causes the particles to be orientation at the current emission angle.
    An orientation-type of 'RANDOM' causes the particles to be orientation at a fixed angle.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setOrientationType, ConsoleVoid, 3, 3, (orientationType))
{
    object->setOrientationType( ParticleAssetEmitter::getOrientationTypeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the orientation-type of the emitter.
    @return The orientation-type of the emitter.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getOrientationType, ConsoleString, 2, 2, ())
{
    return ParticleAssetEmitter::getOrientationTypeDescription( object->getOrientationType() );
}

//------------------------------------------------------------------------------

/*! Sets whether to keep emitted particles aligned or not.
    @keepAligned Whether to keep emitted particles aligned or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setKeepAligned, ConsoleVoid, 3, 3, (keepAligned))
{
    object->setKeepAligned( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets whether to keep emitted particles aligned or not.
    @return Whether to keep emitted particles aligned or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getKeepAligned, ConsoleBool, 2, 2, ())
{
    return object->getKeepAligned();
}

//------------------------------------------------------------------------------

/*! Sets the aligned angle offset.
    @param alignAngleOffset The aligned angle offset.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setAlignedAngleOffset, ConsoleVoid, 3, 3, (alignAngleOffset))
{
    object->setAlignedAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets the aligned angle offset.
    @return The aligned angle offset.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getAlignedAngleOffset, ConsoleFloat, 2, 2, ())
{
    return object->getAlignedAngleOffset();
}

//------------------------------------------------------------------------------

/*! Set Random-Orientation Angle-Offset.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setRandomAngleOffset, ConsoleVoid, 3, 3, (randomAngle))
{
    object->setRandomAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Random-Orientation Angle-Offset.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getRandomAngleOffset, ConsoleFloat, 2, 2, ())
{
    return object->getRandomAngleOffset();
}

//------------------------------------------------------------------------------

/*! Set Random-Orientation Arc.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setRandomArc, ConsoleVoid, 3, 3, (randomArc))
{
    object->setRandomArc( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Random-Orientation Arc.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getRandomArc, ConsoleFloat, 2, 2, ())
{
    return object->getRandomArc();
}

//------------------------------------------------------------------------------

/*! Set Fixed-Orientation Angle-Offset.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setFixedAngleOffset, ConsoleVoid, 3, 3, (randomAngle))
{
    object->setFixedAngleOffset( dAtof(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Fixed-Orientation Angle-Offset.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getFixedAngleOffset, ConsoleFloat, 2, 2, ())
{
    return object->getFixedAngleOffset();
}

//------------------------------------------------------------------------------

/*! Set the Pivot-Point.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setPivotPoint, ConsoleVoid, 3, 4, (pivotX / pivotY))
{
    // Grab the element count.
    U32 elementCount =Utility::mGetStringElementCount(argv[2]);

    // ("pivotX pivotY")
    if ( (elementCount == 2) && (argc < 4) )
    {
        object->setPivotPoint( Vector2( argv[2] ) );
        return;
    }

    // (pivotX, pivotY)
    if ( (elementCount == 1) && (argc > 3) )
    {
        object->setPivotPoint( Vector2( dAtof(argv[2]), dAtof(argv[3]) ) );
        return;
    }

    // Warn.
    Con::warnf( "ParticleAssetEmitter::setPivotPoint() - Invalid number of parameters!" );
}

//------------------------------------------------------------------------------

/*! Get Pivot-Point.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getPivotPoint, ConsoleString, 2, 2, ())
{
    return object->getPivotPoint().scriptThis();
}

//------------------------------------------------------------------------------

/*! Set Link-Emission-Rotation Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setLinkEmissionRotation, ConsoleVoid, 3, 3, (linkEmissionRotation))
{
    object->setLinkEmissionRotation( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Link-Emission-Rotation Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getLinkEmissionRotation, ConsoleBool, 2, 2, ())
{
    return object->getLinkEmissionRotation();
}

//------------------------------------------------------------------------------

/*! Set Intense-Particles Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setIntenseParticles, ConsoleVoid, 3, 3, (intenseParticles))
{
    object->setIntenseParticles( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Intense-Particles Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getIntenseParticles, ConsoleBool, 2, 2, ())
{
    return object->getIntenseParticles();
}

//------------------------------------------------------------------------------

/*! Set Single-Particle Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setSingleParticle, ConsoleVoid, 3, 3, (singleParticle))
{
    // Set Single Particle.
    object->setSingleParticle( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Single-Particle Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getSingleParticle, ConsoleBool, 2, 2, ())
{
    return object->getSingleParticle();
}

//------------------------------------------------------------------------------

/*! Set Attach-Position-To-Emitter Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setAttachPositionToEmitter, ConsoleVoid, 3, 3, (attachPositionToEmitter))
{
    object->setAttachPositionToEmitter( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Attach-Position-To-Emitter Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getAttachPositionToEmitter, ConsoleBool, 2, 2, ())
{
    return object->getAttachPositionToEmitter();
}

//------------------------------------------------------------------------------

/*! Set Attach-Rotation-To-Emitter Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setAttachRotationToEmitter, ConsoleVoid, 3, 3, (attachRotationToEmitter))
{
    object->setAttachRotationToEmitter( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Get Attach-Rotation-To-Emitter Flag.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getAttachRotationToEmitter, ConsoleBool, 2, 2, ())
{
    return object->getAttachRotationToEmitter();
}

//------------------------------------------------------------------------------

/*! Sets whether to render particles as oldest on front or not.
    @param oldestInFront Whether to render particles as oldest on front or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setOldestInFront, ConsoleVoid, 3, 3, (oldestInFront))
{
    object->setOldestInFront( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets whether to render particles as oldest on front or not.
    @return Whether to render particles as oldest on front or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getOldestInFront, ConsoleBool, 2, 2, ())
{
    return object->getOldestInFront();
}

//------------------------------------------------------------------------------

/*! Sets the emitter to use the specified image asset Id and optional frame.
    @param imageAssetId The image asset Id to use.
    @param frame The frame of the image asset Id to use.  Optional.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setImage, ConsoleBool, 3, 4, (imageAssetId, [frame]))
{
    // Was a frame specified?
    if (argc >= 4)
    {
        // Was it a number or a string?
        if (!dIsalpha(*argv[3]))
        {
            // Fetch the numerical frame and set the image
            const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;
            return object->setImage(argv[2], frame);
        }
        else
        {
            // Set the image and pass the named frame string
            return object->setImage(argv[2], argv[3]);
        }
    }
    else
    {
        // Frame was not specified, use default 0 and set the image
        const U32 frame = 0;
        return object->setImage( argv[2], frame);
    }
}

//------------------------------------------------------------------------------

/*! Gets the asset Id of the image asset assigned to the emitter.
    @return The asset Id of the image asset assigned to the emitter or nothing if no image is assigned.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getImage, ConsoleString, 2, 2, ())
{
    return object->getImage();
}

//------------------------------------------------------------------------------

/*! Sets the emitter to use the specified numerical image frame.
    @param frame The frame index of the image to use..
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setImageFrame, ConsoleBool, 3, 3, (frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::setImageFrame() - Method invalid, not in static mode." );
        return false;
    }

    // Are we using named frames?
    if ( object->isUsingNamedImageFrame() )
    {
        // Yes, so warn.
        Con::warnf( "ParticleAssetEmitter::setImageFrame() - Method invalid, using named frames." );
        return false;
    }

    return object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets the frame index of the image asset assigned to the emitter.
    @return The frame index of the image asset assigned to the emitter or nothing if no image is assigned.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::getImageFrame() - Method invalid, not in static mode." );
        return -1;
    }

    // Are we using named frames?
    if ( object->isUsingNamedImageFrame() )
    {
        // Yes, so warn.
        Con::warnf( "ParticleAssetEmitter::getImageFrame() - Method invalid, using named frames." );
        return -1;
    }

    return object->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Disables the Frame and NamedFrame fields and uses a random frame from the specified ImageAsset.
    @param randomImageFrame Whether to use a random image frame or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setRandomImageFrame, ConsoleVoid, 3, 3, (randomImageFrame))
{
    object->setRandomImageFrame( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets whether a random frame from the specified ImageAsset is being used or not.
    @return Whether to use a random image frame or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getRandomImageFrame, ConsoleBool, 2, 2, ())
{
    return object->getRandomImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the emitter to use the specified image frame by name.
    @param frame String containing the name of the frame in the image to use.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setNamedImageFrame, ConsoleBool, 3, 3,  (frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::setNamedImageFrame() - Method invalid, not in static mode." );
        return false;
    }

    // Are we using named frames?
    if ( !object->isUsingNamedImageFrame() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::setNamedImageFrame() - Method invalid, not using named frames." );
        return false;
    }

    return object->setNamedImageFrame( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the asset Id of the image asset assigned to the emitter.
    @return The asset Id of the image asset assigned to the emitter or nothing if no image is assigned.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getNamedImageFrame, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::getNamedImageFrame() - Method invalid, not in static mode." );
        return NULL;
    }

    // Are we using named frames?
    if ( !object->isUsingNamedImageFrame() )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetEmitter::getNamedImageFrame() - Method invalid, not using named frames." );
        return NULL;
    }

    return object->getNamedImageFrame();
}

//------------------------------------------------------------------------------

/*! Gets whether the emitter is using a numerical or named image frame.
    @return Returns true when using a named frame, false when using a numerical index.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, isUsingNamedImageFrame, ConsoleBool, 2, 2, ())
{
    return object->isUsingNamedImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the emitter to use the specified animation asset Id.
    @param animationAssetId The animation asset Id to use.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setAnimation, ConsoleBool, 3, 3, (animationAssetId))
{
    return object->setAnimation( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the asset Id of the animation asset assigned to the emitter.
    @return The asset Id of the animation asset assigned to the emitter or nothing if no animation is assigned.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getAnimation, ConsoleString, 2, 2, ())
{
    return object->getAnimation();
}

//------------------------------------------------------------------------------

/*! Sets whether to use render blending or not.
    @param blendMode Whether to use render blending or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setBlendMode, ConsoleVoid, 3, 3, (blendMode))
{
    object->setBlendMode( dAtob(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets whether to use render blending or not.
    @return Whether to use render blending or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getBlendMode, ConsoleBool, 2, 2, ())
{
    return object->getBlendMode();
}

//-----------------------------------------------------------------------------

/*! Sets the source blend factory.
    @param srcBlend The source blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setSrcBlendFactor, ConsoleVoid, 3, 3, (srcBlend))
{
    // Fetch source blend factor.
    const  GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the source render blend factor.
    @return (srcBlend) The source render blend factor.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getSrcBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getSrcBlendFactorDescription(object->getSrcBlendFactor());
}

//-----------------------------------------------------------------------------

/*! Sets the destination render blend factor.
    @param dstBlend The destination render blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setDstBlendFactor, ConsoleVoid, 3, 3, (dstBlend))
{
    // Fetch destination blend factor.
    const GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the destination render blend factor.
    @return (dstBlend) The destination render blend factor.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getDstBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getDstBlendFactorDescription(object->getDstBlendFactor());
}

//-----------------------------------------------------------------------------

/*! Set the render alpha test threshold.
    @param alpha The alpha test threshold in the range of 0.0 to 1.0.  Less than zero to disable alpha testing.
    @return No return value.

*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setAlphaTest, ConsoleVoid, 3, 3, (float alpha))
{
    object->setAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the render alpha test threshold.
    @return The render alpha test threshold in the range of 0.0f to 1.0.  Less than zero represents disabled alpha testing.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getAlphaTest, ConsoleFloat, 2, 2, ())
{
    return object->getAlphaTest();
}


//-----------------------------------------------------------------------------
/// Particle emitter fields.
//-----------------------------------------------------------------------------

/*! Gets the number of available selectable fields.
    @return The number of available selectable fields.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getSelectableFieldCount, ConsoleInt, 2, 2, ())
{
    return object->getParticleFields().getFields().size();
}

//-----------------------------------------------------------------------------

/*! Gets the selectable field at the specified index.
    @return The selectable field name at the specified index.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getSelectableFieldName, ConsoleString, 3, 3, (fieldIndex))
{
    // Fetch the field hash.
    const ParticleAssetFieldCollection::typeFieldHash& fieldHash = object->getParticleFields().getFields();

    // Fetch the index.
    S32 fieldIndex = dAtoi( argv[2] );

    // Is the field index valid?
    if ( fieldIndex >= 0 && fieldIndex < (S32)fieldHash.size() )
    {
        // Yes, but because the fields are in a hash-table, we'll have to iterate and get O(index).
        for( ParticleAssetFieldCollection::typeFieldHash::const_iterator fieldItr = fieldHash.begin(); fieldItr != fieldHash.end(); ++fieldItr, --fieldIndex )
        {
            // Skip if this is not the field index we're looking for?
            if ( fieldIndex != 0 )
                continue;

            // Found it so return the field name.
            return fieldItr->value->getFieldName();
        }
    }


    // Warn.
    Con::warnf( "ParticleAssetEmitter::getSelectableFieldName() - Index '%d' is out of range.", fieldIndex );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

/*! Select the specified field by its name.
    @param fieldName The field name to use for the selection.  Use an empty name to deselect to stop accidental changes.
    @return Whether the field was successfully selected or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, selectField, ConsoleBool, 3, 3, (fieldName))
{
    return object->getParticleFields().selectField( argv[2] ) != NULL;
}

//-----------------------------------------------------------------------------

/*! Deselect any selected field.  If no field is selected then nothing happens.
    @return No return value.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, deselectField, ConsoleVoid, 2, 2, ())
{
    object->getParticleFields().deselectField();
}

//-----------------------------------------------------------------------------

/*! Gets the selected field name or nothing if no field is selected.
    @return The selected field name or nothing if no fields is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getSelectedField, ConsoleBool, 2, 2, ())
{
    // Get the selected field.
    const ParticleAssetField* pParticleAssetField = object->getParticleFields().getSelectedField();

    return pParticleAssetField == NULL ? StringTable->EmptyString : pParticleAssetField->getFieldName();
}

//-----------------------------------------------------------------------------

/*! Sets a single data-key at time-zero with the specified value.  All existing keys are cleared.
    @param value The value to set the key to.
    @return Returns the index of the new data-key (always zero) or -1 on failure.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setSingleDataKey, ConsoleInt, 3, 3, (value))
{
    return object->getParticleFields().setSingleDataKey( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Add Data-Key to Graph.
    @param time The key time.
    @param value The value at specified time
    @return Returns the index of the new data-key or -1 on failure.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, addDataKey, ConsoleInt, 4, 4, (time, value))
{
    return object->getParticleFields().addDataKey( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Remove the data-key from the field.
    @param keyIndex The index of the data-key you want to remove.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, removeDataKey, ConsoleBool, 3, 3, (keyIndex))
{
   return object->getParticleFields().removeDataKey( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Clears all data-key(s) from the field.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, clearDataKeys, ConsoleBool, 2, 2, ())
{
   return object->getParticleFields().clearDataKeys();
}

//-----------------------------------------------------------------------------

/*! Set data-key value for the field.
    @param keyIndex The index of the key to be modified.
    @param value The value to change the key to.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setDataKeyValue, ConsoleBool, 4, 4, (keyIndex, value))
{
   // Set Data Key.
   return object->getParticleFields().setDataKey( dAtoi(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets the data-key count.
    @return The number of data-keys in the currently selected field or -1 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getDataKeyCount, ConsoleInt, 2, 2, ())
{
   // Get Data Key Count.
   return object->getParticleFields().getDataKeyCount();
}

//-----------------------------------------------------------------------------

/*! Gets the data-key at the specified index from the field.
    @param keyIndex The index of the data-key to be retrieved.
    @return The data-key comprising both the time and value or nothing if the key is invalid.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getDataKey, ConsoleString, 3, 3, (keyIndex))
{
   // Fetch the key index.
   const S32 keyIndex = dAtoi(argv[2]);

   // Fetch the data-key.
   const ParticleAssetField::DataKey dataKey = object->getParticleFields().getDataKey( keyIndex );

   // Finish if the data-key is bad.
   if ( dataKey == ParticleAssetField::BadDataKey )
       return StringTable->EmptyString;

   // Create Returnable Buffer.
   char* pBuffer = Con::getReturnBuffer(32);

   // Format Buffer.
   dSprintf(pBuffer, 32, "%f %f", dataKey.mTime, dataKey.mValue );

   // Return buffer.
   return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Get the minimum value for the field.
    @return The minimum value for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getMinValue, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMinValue();
}

//-----------------------------------------------------------------------------

/*! Get the maximum value for the field.
    @return The maximum value for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getMaxValue, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMaxValue();
}

//-----------------------------------------------------------------------------

/*! Get the minimum time for the field.
    @return The minimum time for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getMinTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMinTime();
}

//-----------------------------------------------------------------------------

/*! Get the maximum time for the field.
    @return The maximum time for the field or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getMaxTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getMaxTime();
}

//-----------------------------------------------------------------------------

/*! Get the fields' value at the specified time.
    @param time The time to sample the field value at.
    @return The fields' value at the specified time or always 0.0 if no field is selected.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getFieldValue, ConsoleFloat, 3, 3, (time))
{
   return object->getParticleFields().getFieldValue( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Sets the time period to repeat (cycle) the fields' values at.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setRepeatTime, ConsoleBool, 3, 3, (repeatTime))
{
   return object->getParticleFields().setRepeatTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the time period that the fields' value repeat (cycle) at.
    @return The time period that the fields' value repeat (cycle) at.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getRepeatTime, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getRepeatTime();
}

//-----------------------------------------------------------------------------

/*! Set the scaling of field values retrieved from the field.  This does not alter the actual data-key values.
    @param valueScale The scale for field values retrieved from the field.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, setValueScale, ConsoleBool, 3, 3, (valueScale))
{
   return object->getParticleFields().setValueScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the scaling of field values' retrieved from the field.
    @return The scaling of field values' retrieved from the field.
*/
ConsoleMethodWithDocs(ParticleAssetEmitter, getValueScale, ConsoleFloat, 2, 2, ())
{
   return object->getParticleFields().getValueScale();
}

ConsoleMethodGroupEndWithDocs(ParticleAssetEmitter)
