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

#include "2d/assets/ParticleAssetEmitter.h"

#ifndef _PARTICLE_ASSET_H_
#include "2d/assets/ParticleAsset.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

// Script bindings.
#include "ParticleAssetEmitter_ScriptBinding.h"

//------------------------------------------------------------------------------

static EnumTable::Enums emitterTypeLookup[] =
                {
                { ParticleAssetEmitter::POINT_EMITTER,      "POINT" },
                { ParticleAssetEmitter::LINE_EMITTER,       "LINE" },
                { ParticleAssetEmitter::BOX_EMITTER,        "BOX" },
                { ParticleAssetEmitter::DISK_EMITTER,       "DISK" },
                { ParticleAssetEmitter::ELLIPSE_EMITTER,    "ELLIPSE" },
                { ParticleAssetEmitter::TORUS_EMITTER,      "TORUS" },
                };

//------------------------------------------------------------------------------

static EnumTable EmitterTypeTable(sizeof(emitterTypeLookup) / sizeof(EnumTable::Enums), &emitterTypeLookup[0]);

//------------------------------------------------------------------------------

ParticleAssetEmitter::EmitterType ParticleAssetEmitter::getEmitterTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(emitterTypeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(emitterTypeLookup[i].label, label) == 0)
            return((ParticleAssetEmitter::EmitterType)emitterTypeLookup[i].index);

    // Warn.
    Con::warnf(  "ParticleAssetEmitter::getEmitterTypeEnum() - Invalid emitter-type '%s'.", label );

    return ParticleAssetEmitter::INVALID_EMITTER_TYPE;
}

//-----------------------------------------------------------------------------

const char* ParticleAssetEmitter::getEmitterTypeDescription( const EmitterType emitterType )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(emitterTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( emitterTypeLookup[i].index == (S32)emitterType )
            return emitterTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "ParticleAssetEmitter::getEmitterTypeDescription() - Invalid emitter-type." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

static EnumTable::Enums physicsParticleType[] =
{
   {b2ParticleFlag::b2_barrierParticle,            "BarrierPartice"},
   {b2ParticleFlag::b2_colorMixingParticle,        "ColorMixingParticle"},
   {b2ParticleFlag::b2_destructionListenerParticle,"DesctructionListenerParticle"},
   {b2ParticleFlag::b2_elasticParticle,            "ElasticParticle"},
   {b2ParticleFlag::b2_powderParticle,             "PowderParticle"},
   {b2ParticleFlag::b2_reactiveParticle,           "ReactiveParticle"},
   {b2ParticleFlag::b2_repulsiveParticle,          "RepulsiveParticle"},
   {b2ParticleFlag::b2_springParticle,             "SpringParticle"},
   {b2ParticleFlag::b2_staticPressureParticle,     "StaticPressureParticle"},
   {b2ParticleFlag::b2_tensileParticle,            "TensileParticle"},
   {b2ParticleFlag::b2_viscousParticle,            "ViscousParticle"},
   {b2ParticleFlag::b2_wallParticle,               "WallParticle"},
   {b2ParticleFlag::b2_waterParticle,              "WaterParticle"},
   {b2ParticleFlag::b2_zombieParticle,             "ZombieParticle"}
};


static EnumTable  PhysicsParticleTypeTable(sizeof(physicsParticleType) / sizeof(EnumTable::Enums), &physicsParticleType[0]);

ParticleAssetEmitter::PhysicsParticleType ParticleAssetEmitter::getPhysicsParticleTypeEnum(const char* label)
{
   for (U32 i = 0; i < (sizeof(physicsParticleType) / sizeof(EnumTable::Enums)); i++)
      if (dStricmp(physicsParticleType[i].label, label) == 0)
         return((ParticleAssetEmitter::PhysicsParticleType)physicsParticleType[i].index);

   // Warn.
   Con::warnf("ParticleAssetEmitter::getPhysicsParticleType() - Invalid physics particle type '%s'.", label);

   return ParticleAssetEmitter::INVALID_PHYSICS_PARTICLE_TYPE;
}

const char* ParticleAssetEmitter::getPhysicsParticleTypeDescription(const PhysicsParticleType particleType)
{
   // Search for Mnemonic.
   for (U32 i = 0; i < (sizeof(physicsParticleType) / sizeof(EnumTable::Enums)); i++)
   {
      if (physicsParticleType[i].index == (S32)particleType)
         return physicsParticleType[i].label;
   }

   // Warn.
   Con::warnf("ParticleAssetEmitter::getPhysicsParticleTypeDescription() - Invalid physics particle-type");

   return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

static EnumTable::Enums particleOrientationTypeLookup[] =
                {
                { ParticleAssetEmitter::FIXED_ORIENTATION,    "FIXED" },
                { ParticleAssetEmitter::ALIGNED_ORIENTATION,  "ALIGNED" },
                { ParticleAssetEmitter::RANDOM_ORIENTATION,   "RANDOM" },
                };

//------------------------------------------------------------------------------

static EnumTable OrientationTypeTable(sizeof(particleOrientationTypeLookup) / sizeof(EnumTable::Enums), &particleOrientationTypeLookup[0]);

//------------------------------------------------------------------------------

ParticleAssetEmitter::ParticleOrientationType ParticleAssetEmitter::getOrientationTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(particleOrientationTypeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(particleOrientationTypeLookup[i].label, label) == 0)
            return((ParticleAssetEmitter::ParticleOrientationType)particleOrientationTypeLookup[i].index);

    // Warn.
    Con::warnf( "ParticleAssetEmitter::getOrientationTypeEnum() - Invalid orientation type '%s'.", label );

    return ParticleAssetEmitter::INVALID_ORIENTATION;
}

//------------------------------------------------------------------------------

const char* ParticleAssetEmitter::getOrientationTypeDescription( const ParticleOrientationType orientationType )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(particleOrientationTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( particleOrientationTypeLookup[i].index == (S32)orientationType )
            return particleOrientationTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "ParticleAssetEmitter::getOrientationTypeDescription() - Invalid orientation-type" );

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

ParticleAssetEmitter::ParticleAssetEmitter() :
                            mEmitterName( StringTable->EmptyString ),
                            mOwner( NULL ),
                            mEmitterType( POINT_EMITTER ),
                            mPhysicsParticleType(b2_waterParticle),
                            mPhysicsParticles(false),
                            mEmitterOffset( 0.0f, 0.0f),
                            mTargetParticle(false),
                            mTargetPosition(0.0f, 0.0f),
                            mEmitterAngle( 0.0f ),
                            mEmitterSize( 10.0f, 10.0f ),
                            mFixedAspect( true ),
                            mOrientationType( FIXED_ORIENTATION ),
                            mKeepAligned( false ),
                            mAlignedAngleOffset( 0.0f ),
                            mRandomAngleOffset( 0.0f ),
                            mRandomArc( 360.0f ),
                            mFixedAngleOffset( 0.0f ),
                            mLinkEmissionRotation( true ),
                            mIntenseParticles( false ),
                            mSingleParticle( false ),
                            mAttachPositionToEmitter( false ),
                            mAttachRotationToEmitter( false ),
                            mOldestInFront( false ),
                            mStaticMode( true ),
                            mImageAsset( NULL ),
                            mImageFrame( 0 ),
                            mRandomImageFrame( false ),
                            mAnimationAsset( NULL ),
                            mBlendMode( true ),
                            mSrcBlendFactor( GL_SRC_ALPHA ),
                            mDstBlendFactor( GL_ONE_MINUS_SRC_ALPHA ),
                            mAlphaTest( -1.0f )
{
    // Set the pivot point.
    // NOTE:    This is called to set the local AABB.
    setPivotPoint( Vector2::getZero() );

    // Set fixed force angle.
    // NOTE:    This is called to set the fixed-force-direction.
    setFixedForceAngle( 0.0f );

    // Initialize particle fields.
    mParticleFields.addField( mParticleLife.getBase(), "Lifetime", 1000.0f, 0.0f, 10000.0f, 2.0f );
    mParticleFields.addField( mParticleLife.getVariation(), "LifetimeVariation", 1000.0f, 0.0f, 5000.0f, 0.0f  );
    mParticleFields.addField( mQuantity.getBase(), "Quantity", 1000.0f, 0.0f, 100000.0f, 10.0f );
    mParticleFields.addField( mQuantity.getVariation(), "QuantityVariation", 1000.0f, 0.0f, 100000.0f, 0.0f );
    mParticleFields.addField( mSizeX.getBase(), "SizeX",  1000.0f, 0.0f, 100.0f, 2.0f );
    mParticleFields.addField( mSizeX.getVariation(), "SizeXVariation", 1000.0f, 0.0f, 200.0f, 0.0f );
    mParticleFields.addField( mSizeX.getLife(), "SizeXLife", 1.0f, -100.0f, 100.0f, 1.0f );
    mParticleFields.addField( mSizeY.getBase(), "SizeY",  1000.0f, 0.0f, 100.0f, 2.0f );
    mParticleFields.addField( mSizeY.getVariation(), "SizeYVariation", 1000.0f, 0.0f, 200.0f, 0.0f );
    mParticleFields.addField( mSizeY.getLife(), "SizeYLife", 1.0f, -100.0f, 100.0f, 1.0f );
    mParticleFields.addField( mSpeed.getBase(), "Speed", 1000.0f, 0.0f, 100.0f, 10.0f );
    mParticleFields.addField( mSpeed.getVariation(), "SpeedVariation", 1000.0f, 0.0f, 200.0f, 0.0f );
    mParticleFields.addField( mSpeed.getLife(), "SpeedLife", 1.0f, -100.0f, 100.0f, 1.0f );
    mParticleFields.addField( mSpin.getBase(), "Spin", 1000.0f, -1000.0f, 1000.0f, 0.0f );
    mParticleFields.addField( mSpin.getVariation(), "SpinVariation", 1000.0f, 0.0f, 2000.0f, 0.0f );
    mParticleFields.addField( mSpin.getLife(), "SpinLife", 1.0f, -1000.0f, 1000.0f, 1.0f );
    mParticleFields.addField( mFixedForce.getBase(), "FixedForce", 1000.0f, -1000.0f, 1000.0f, 0.0f );
    mParticleFields.addField( mFixedForce.getVariation(), "FixedForceVariation", 1000.0f, 0.0f, 2000.0f, 0.0f );
    mParticleFields.addField( mFixedForce.getLife(), "FixedForceLife", 1.0f, -1000.0f, 1000.0f, 1.0f );
    mParticleFields.addField( mRandomMotion.getBase(), "RandomMotion", 1000.0f, 0.0f, 1000.0f, 0.0f );
    mParticleFields.addField( mRandomMotion.getVariation(), "RandomMotionVariation", 1000.0f, 0.0f, 2000.0f, 0.0f );
    mParticleFields.addField( mRandomMotion.getLife(), "RandomMotionLife", 1.0f, -100.0f, 100.0f, 1.0f );
    mParticleFields.addField( mEmissionForce.getBase(), "EmissionForce", 1000.0f, -100.0f, 1000.0f, 5.0f );
    mParticleFields.addField( mEmissionForce.getVariation(), "EmissionForceVariation", 1000.0f, -500.0f, 500.0f, 5.0f );
    mParticleFields.addField( mEmissionAngle.getBase(), "EmissionAngle", 1000.0f, -180.0f, 180.0f, 0.0f );
    mParticleFields.addField( mEmissionAngle.getVariation(), "EmissionAngleVariation", 1000.0f, 0.0f, 360.0f, 0.0f );
    mParticleFields.addField( mEmissionArc.getBase(), "EmissionArc", 1000.0f, 0.0f, 360.0f, 360.0f );
    mParticleFields.addField( mEmissionArc.getVariation(), "EmissionArcVariation", 1000.0f, 0.0f, 720.0f, 0.0f );
    mParticleFields.addField( mRedChannel.getLife(), "RedChannel", 1.0f, 0.0f, 1.0f, 1.0f );
    mParticleFields.addField( mGreenChannel.getLife(), "GreenChannel", 1.0f, 0.0f, 1.0f, 1.0f );
    mParticleFields.addField( mBlueChannel.getLife(), "BlueChannel", 1.0f, 0.0f, 1.0f, 1.0f );
    mParticleFields.addField( mAlphaChannel.getLife(), "AlphaChannel", 1.0f, 0.0f, 1.0f, 1.0f );

    // Register for refresh notifications.
    mImageAsset.registerRefreshNotify( this );
    mAnimationAsset.registerRefreshNotify( this );
    
    mNamedImageFrame = "";
    mUsingNamedFrame = false;
}

//------------------------------------------------------------------------------

ParticleAssetEmitter::~ParticleAssetEmitter()
{
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField("EmitterName", TypeString, Offset(mEmitterName, ParticleAssetEmitter), &setEmitterName, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
    addProtectedField("EmitterType", TypeEnum, Offset(mEmitterType, ParticleAssetEmitter), &setEmitterType, &defaultProtectedGetFn, &writeEmitterType, 1, &EmitterTypeTable);
    addProtectedField("EmitterOffset", TypeVector2, Offset(mEmitterOffset, ParticleAssetEmitter), &setEmitterOffset, &defaultProtectedGetFn, &writeEmitterOffset, "");
    addProtectedField("IsTargeting", TypeBool, Offset(mTargetParticle, ParticleAssetEmitter), &setIsTargeting, &defaultProtectedGetFn, &writeTargetParticle, "");
    addProtectedField("TargetPosition", TypeVector2, Offset(mTargetPosition, ParticleAssetEmitter), &setTargetPosition, &defaultProtectedGetFn, &writeTargetPosition, "");

    //Physics Particles
    addProtectedField("PhysicsParticle", TypeBool, Offset(mPhysicsParticles, ParticleAssetEmitter), &setPhysicsParticles, &defaultProtectedGetFn, &writePhysicsParticles, "");
    addProtectedField("PhysicsParticleType", TypeEnum, Offset(mPhysicsParticleType, ParticleAssetEmitter), &setPhysicsParticleType, &defaultProtectedGetFn, &writePhysicsParticleType, 1, &PhysicsParticleTypeTable);
    //Physics Particles end---

    addProtectedField("EmitterAngle", TypeF32, Offset(mEmitterAngle, ParticleAssetEmitter), &setEmitterAngle, &defaultProtectedGetFn, &writeEmitterAngle, "");
    addProtectedField("EmitterSize", TypeVector2, Offset(mEmitterSize, ParticleAssetEmitter), &setEmitterSize, &defaultProtectedGetFn, &writeEmitterSize, "");
    addProtectedField("FixedAspect", TypeBool, Offset(mFixedAspect, ParticleAssetEmitter), &setFixedAspect, &defaultProtectedGetFn, &writeFixedAspect, "");
    addProtectedField("FixedForceAngle", TypeF32, Offset(mFixedForceAngle, ParticleAssetEmitter), &setFixedForceAngle, &defaultProtectedGetFn, &writeFixedForceAngle, "");
    addProtectedField("OrientationType", TypeEnum, Offset(mOrientationType, ParticleAssetEmitter), &setOrientationType, &defaultProtectedGetFn, &writeOrientationType, 1, &OrientationTypeTable);
    addProtectedField("KeepAligned", TypeBool, Offset(mKeepAligned, ParticleAssetEmitter), &setKeepAligned, &defaultProtectedGetFn, &writeKeepAligned, "");
    addProtectedField("AlignedAngleOffset", TypeF32, Offset(mAlignedAngleOffset, ParticleAssetEmitter), &setAlignedAngleOffset, &defaultProtectedGetFn, &writeAlignedAngleOffset, "");
    addProtectedField("RandomAngleOffset", TypeF32, Offset(mRandomAngleOffset, ParticleAssetEmitter), &setRandomAngleOffset, &defaultProtectedGetFn, &writeRandomAngleOffset, "");
    addProtectedField("RandomArc", TypeF32, Offset(mRandomArc, ParticleAssetEmitter), &setRandomArc, &defaultProtectedGetFn, &writeRandomArc, "");
    addProtectedField("FixedAngleOffset", TypeF32, Offset(mFixedAngleOffset, ParticleAssetEmitter), &setFixedAngleOffset, &defaultProtectedGetFn, &writeFixedAngleOffset, "");
    addProtectedField("PivotPoint", TypeVector2, Offset(mPivotPoint, ParticleAssetEmitter), &setPivotPoint, &defaultProtectedGetFn, &writePivotPoint, "");
    addProtectedField("LinkEmissionRotation", TypeBool, Offset(mLinkEmissionRotation, ParticleAssetEmitter), &setLinkEmissionRotation, &defaultProtectedGetFn, &writeLinkEmissionRotation, "");
    addProtectedField("IntenseParticles", TypeBool, Offset(mIntenseParticles, ParticleAssetEmitter), &setIntenseParticles, &defaultProtectedGetFn, &writeIntenseParticles, "");
    addProtectedField("SingleParticle", TypeBool, Offset(mSingleParticle, ParticleAssetEmitter), &setSingleParticle, &defaultProtectedGetFn, &writeSingleParticle, "");
    addProtectedField("AttachPositionToEmitter", TypeBool, Offset(mAttachPositionToEmitter, ParticleAssetEmitter), &setAttachPositionToEmitter, &defaultProtectedGetFn, &writeAttachPositionToEmitter, "");
    addProtectedField("AttachRotationToEmitter", TypeBool, Offset(mAttachRotationToEmitter, ParticleAssetEmitter), &setAttachRotationToEmitter, &defaultProtectedGetFn, &writeAttachRotationToEmitter, "");
    addProtectedField("OldestInFront", TypeBool, Offset(mOldestInFront, ParticleAssetEmitter), &setOldestInFront, &defaultProtectedGetFn, &writeOldestInFront, "");

    addProtectedField("BlendMode", TypeBool, Offset(mBlendMode, ParticleAssetEmitter), &setBlendMode, &defaultProtectedGetFn, &writeBlendMode, "");
    addProtectedField("SrcBlendFactor", TypeEnum, Offset(mSrcBlendFactor, ParticleAssetEmitter), &setSrcBlendFactor, &defaultProtectedGetFn, &writeSrcBlendFactor, 1, &srcBlendFactorTable, "");
    addProtectedField("DstBlendFactor", TypeEnum, Offset(mDstBlendFactor, ParticleAssetEmitter), &setDstBlendFactor, &defaultProtectedGetFn, &writeDstBlendFactor, 1, &dstBlendFactorTable, "");
    addProtectedField("AlphaTest", TypeF32, Offset(mAlphaTest, ParticleAssetEmitter), &setAlphaTest, &defaultProtectedGetFn, &writeAlphaTest, "");

    addProtectedField("Image", TypeImageAssetPtr, Offset(mImageAsset, ParticleAssetEmitter), &setImage, &getImage, &writeImage, "");
    addProtectedField("Frame", TypeS32, Offset(mImageFrame, ParticleAssetEmitter), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "");
    addProtectedField("NamedFrame", TypeString, Offset(mNamedImageFrame, ParticleAssetEmitter), &setNamedImageFrame, &defaultProtectedGetFn, &writeNamedImageFrame, "");
    addProtectedField("RandomImageFrame", TypeBool, Offset(mRandomImageFrame, ParticleAssetEmitter), &setRandomImageFrame, &defaultProtectedGetFn, &writeRandomImageFrame, "");
    addProtectedField("Animation", TypeAnimationAssetPtr, Offset(mAnimationAsset, ParticleAssetEmitter), &setAnimation, &getAnimation, &writeAnimation, "");
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::copyTo(SimObject* object)
{
    // Fetch particle asset emitter object.
   ParticleAssetEmitter* pParticleAssetEmitter = static_cast<ParticleAssetEmitter*>( object );

   // Sanity!
   AssertFatal( pParticleAssetEmitter != NULL, "ParticleAssetEmitter::copyTo() - Object is not the correct type.");

   // Copy parent.
   Parent::copyTo( object );

   // Copy fields.
   pParticleAssetEmitter->setEmitterName( getEmitterName() );
   pParticleAssetEmitter->setEmitterType( getEmitterType() );
   pParticleAssetEmitter->setEmitterOffset( getEmitterOffset() );
   pParticleAssetEmitter->setEmitterSize( getEmitterSize() );
   pParticleAssetEmitter->setEmitterAngle( getEmitterAngle() );
   pParticleAssetEmitter->setFixedAspect( getFixedAspect() );
   pParticleAssetEmitter->setFixedForceAngle( getFixedForceAngle() );
   pParticleAssetEmitter->setOrientationType( getOrientationType() );
   pParticleAssetEmitter->setKeepAligned( getKeepAligned() );
   pParticleAssetEmitter->setAlignedAngleOffset( getAlignedAngleOffset() );
   pParticleAssetEmitter->setRandomAngleOffset( getRandomAngleOffset() );
   pParticleAssetEmitter->setRandomArc( getRandomArc() );
   pParticleAssetEmitter->setFixedAngleOffset( getFixedAngleOffset() );
   pParticleAssetEmitter->setPivotPoint( getPivotPoint() );
   pParticleAssetEmitter->setLinkEmissionRotation( getLinkEmissionRotation() );
   pParticleAssetEmitter->setIntenseParticles( getIntenseParticles() );
   pParticleAssetEmitter->setSingleParticle( getSingleParticle() );
   pParticleAssetEmitter->setAttachPositionToEmitter( getAttachPositionToEmitter() );
   pParticleAssetEmitter->setAttachRotationToEmitter( getAttachRotationToEmitter() );
   pParticleAssetEmitter->setOldestInFront( getOldestInFront() );

   pParticleAssetEmitter->setBlendMode( getBlendMode() );
   pParticleAssetEmitter->setSrcBlendFactor( getSrcBlendFactor() );
   pParticleAssetEmitter->setDstBlendFactor( getDstBlendFactor() );
   pParticleAssetEmitter->setAlphaTest( getAlphaTest() );

   pParticleAssetEmitter->setRandomImageFrame( getRandomImageFrame() );

   // Static provider?
   if ( pParticleAssetEmitter->isStaticFrameProvider() )
   {
        // Named image frame?
        if ( pParticleAssetEmitter->isUsingNamedImageFrame() )
            pParticleAssetEmitter->setImage( getImage(), getNamedImageFrame() );
        else
            pParticleAssetEmitter->setImage( getImage(), getImageFrame() );
   }
   else
   {
       pParticleAssetEmitter->setAnimation( getAnimation() );
   }

   // Copy particle fields.
   mParticleFields.copyTo( pParticleAssetEmitter->mParticleFields );
}

//-----------------------------------------------------------------------------

void ParticleAssetEmitter::setEmitterName( const char* pEmitterName )
{
    // Sanity!
    AssertFatal( mEmitterName != NULL, "ParticleAssetEmitter::setEmitterName() - Cannot set a NULL particle asset emitter name." );

    // Set the emitter name.
    mEmitterName = StringTable->insert( pEmitterName );

    // Refresh the asset.
    refreshAsset();
}

//-----------------------------------------------------------------------------

void ParticleAssetEmitter::setOwner( ParticleAsset* pParticleAsset )
{
    // Sanity!
    AssertFatal( mOwner == NULL, "ParticleAssetEmitter::setOwner() - Cannot set an owner when one is already assigned." );

    // Set owner.
    mOwner = pParticleAsset;
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::setPivotPoint( const Vector2& pivotPoint )
{
    // Set the pivot point.
    mPivotPoint = pivotPoint;

    // Calculate the local pivot AABB.
    mLocalPivotAABB[0].Set( -0.5f + mPivotPoint.x, -0.5f + mPivotPoint.y );
    mLocalPivotAABB[1].Set(  0.5f + mPivotPoint.x, -0.5f + mPivotPoint.y );
    mLocalPivotAABB[2].Set(  0.5f + mPivotPoint.x,  0.5f + mPivotPoint.y );
    mLocalPivotAABB[3].Set( -0.5f + mPivotPoint.x,  0.5f + mPivotPoint.y );

    // Refresh the asset.
    refreshAsset(); }

//------------------------------------------------------------------------------

void ParticleAssetEmitter::setFixedForceAngle( F32 fixedForceAngle )
{
    // Set Fixed-Force Angle.
    mFixedForceAngle = fixedForceAngle;

    // Calculate the angle in radians.
    const F32 fixedForceAngleRadians = mDegToRad(mFixedForceAngle);

    // Set Fixed-Force Direction.
    mFixedForceDirection.Set( mCos(fixedForceAngleRadians), mSin(fixedForceAngleRadians) );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

bool ParticleAssetEmitter::setImage( const char* pAssetId, U32 frame )
{
    // Sanity!
    AssertFatal( pAssetId != NULL, "ParticleAssetEmitter::setImage() - Cannot use a NULL asset Id." );

    // Set static mode.
    mStaticMode = true;

    // Clear animation asset.
    mAnimationAsset.clear();

    // Set asset Id.
    mImageAsset = pAssetId;

    // Is there an asset?
    if ( mImageAsset.notNull() )
    {
        // Yes, so is the frame valid?
        if ( frame >= mImageAsset->getFrameCount() )
        {
            // No, so warn.
            Con::warnf( "ParticleAssetEmitter::setImage() - Invalid frame '%d' for ImageAsset '%s'.", frame, mImageAsset.getAssetId() );
        }
        else
        {
            // Yes, so set the frame.
            mImageFrame = frame;
        }
    }
    else
    {
        // No, so reset the image frame.
        mImageFrame = 0;
    }

    // Using a numerical frame index
    mUsingNamedFrame = false;

    // Refresh the asset.
    refreshAsset();

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ParticleAssetEmitter::setImage( const char* pAssetId, const char* frameName )
{
    // Sanity!
    AssertFatal( pAssetId != NULL, "ParticleAssetEmitter::setImage() - Cannot use a NULL asset Id." );
    
    // Set static mode.
    mStaticMode = true;
    
    // Clear animation asset.
    mAnimationAsset.clear();
    
    // Set asset Id.
    mImageAsset = pAssetId;
    
    // Is there an asset?
    if ( mImageAsset.notNull() )
    {
        // Yes, so is the frame valid?
        if ( !mImageAsset->containsFrame(frameName) )
        {
            // No, so warn.
            Con::warnf( "ParticleAssetEmitter::setImage() - Invalid frame '%s' for ImageAsset '%s'.", frameName, mImageAsset.getAssetId() );
        }
        else
        {
        // Yes, so set the frame.
        mNamedImageFrame = StringTable->insert(frameName);
        }
    }
    else
    {
        // No, so reset the image frame.
        mNamedImageFrame = StringTable->insert(StringTable->EmptyString);
    }

    // Using a named frame index
    mUsingNamedFrame = true;
    
    // Refresh the asset.
    refreshAsset();
    
    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ParticleAssetEmitter::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mImageAsset.isNull() )
    {
        // Warn.
        Con::warnf("ParticleAssetEmitter::setImageFrame() - Cannot set Frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= mImageAsset->getFrameCount() )
    {
        // Warn.
        Con::warnf( "ParticleAssetEmitter::setImageFrame() - Invalid Frame #%d for asset Id '%s'.", frame, mImageAsset.getAssetId() );
        // Return Here.
        return false;
    }

    // Set Frame.
    mImageFrame = frame;

    // Using a numerical frame index.
    mUsingNamedFrame = false;

    // Refresh the asset.
    refreshAsset();

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ParticleAssetEmitter::setNamedImageFrame( const char* frameName )
{
    // Check Existing Image.
    if ( mImageAsset.isNull() )
    {
        // Warn.
        Con::warnf("ParticleAssetEmitter::setNamedImageFrame() - Cannot set Frame without existing asset Id.");
        
        // Return Here.
        return false;
    }
    
    // Check Frame Validity.
    if ( !mImageAsset->containsFrame(frameName) )
    {
        // Warn.
        Con::warnf( "ParticleAssetEmitter::setNamedImageFrame() - Invalid Frame %s for asset Id '%s'.", frameName, mImageAsset.getAssetId() );
        
        // Return Here.
        return false;
    }
    
    // Set frame.
    mNamedImageFrame = StringTable->insert(frameName);

    // Using a named frame index
    mUsingNamedFrame = true;
    
    // Refresh the asset.
    refreshAsset();
    
    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ParticleAssetEmitter::setAnimation( const char* pAnimationAssetId )
{
    // Sanity!
    AssertFatal( pAnimationAssetId != NULL, "ParticleAssetEmitter::setAnimation() - Cannot use NULL asset Id." );

    // Set animated mode.
    mStaticMode = false;

    // Clear static asset.
    mImageAsset.clear();

    // Set animation asset.
    mAnimationAsset = pAnimationAssetId;

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::refreshAsset( void )
{
    // Finish if no owner.
    if ( mOwner == NULL )
        return;

    // Refresh the asset.
    mOwner->refreshAsset();
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::onAssetRefreshed( AssetPtrBase* pAssetPtrBase )
{
    // Either the image or animation asset has been refreshed to just refresh the
    // asset that this emitter may belong to.
    ParticleAssetEmitter::refreshAsset();
}

//------------------------------------------------------------------------------

void ParticleAssetEmitter::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetEmitter_OnTamlCustomWrite);

    // Write the fields.
    mParticleFields.onTamlCustomWrite( customNodes );
}

//-----------------------------------------------------------------------------

void ParticleAssetEmitter::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetEmitter_OnTamlCustomRead);

    // Read the fields.
    mParticleFields.onTamlCustomRead( customNodes );
}


//-----------------------------------------------------------------------------

static void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "ParticleAssetEmitter::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "ParticleAssetEmitter::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    // Write the particle asset emitter fields.
    ParticleAssetEmitter particleAssetEmitter;
    particleAssetEmitter.getParticleFields().WriteCustomTamlSchema( pClassRep, pParentElement );
}

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_SCHEMA(ParticleAssetEmitter, WriteCustomTamlSchema);
