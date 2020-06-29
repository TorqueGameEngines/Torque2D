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

#ifndef _PARTICLE_ASSET_EMITTER_H_
#define _PARTICLE_ASSET_EMITTER_H_

#ifndef _PARTICLE_ASSET_FIELD_H_
#include "2d/assets/ParticleAssetFieldCollection.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _ANIMATION_ASSET_H_
#include "2d/assets/AnimationAsset.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

class ParticleAsset;

//-----------------------------------------------------------------------------

class ParticleAssetEmitter : public SimObject, protected AssetPtrCallback
{
    friend class ParticleAsset;

public:
    enum ParticleOrientationType
    {
        INVALID_ORIENTATION,

        FIXED_ORIENTATION,
        ALIGNED_ORIENTATION,
        RANDOM_ORIENTATION
    };

    enum EmitterType
    {
        INVALID_EMITTER_TYPE,

        POINT_EMITTER,
        LINE_EMITTER,
        BOX_EMITTER,
        DISK_EMITTER,
        ELLIPSE_EMITTER,
        TORUS_EMITTER,
    };

    enum PhysicsParticleType
    {
       INVALID_PHYSICS_PARTICLE_TYPE,

       b2_barrierParticle,
       b2_colorMixingParticle,
       b2_destructionListenerParticle,
       b2_elasticParticle,
       b2_powderParticle,
       b2_reactiveParticle,
       b2_repulsiveParticle,
       b2_springParticle,
       b2_staticPressureParticle,
       b2_tensileParticle,
       b2_viscousParticle,
       b2_wallParticle,
       b2_waterParticle,
       b2_zombieParticle
    };

private:
    typedef SimObject Parent;

    StringTableEntry                        mEmitterName;
    ParticleAsset*                          mOwner;

    EmitterType                             mEmitterType;
    Vector2                                 mEmitterOffset;
    F32                                     mEmitterAngle;
    Vector2                                 mEmitterSize;
    bool                                    mFixedAspect;
    F32                                     mFixedForceAngle;
    Vector2                                 mFixedForceDirection;
    ParticleOrientationType                 mOrientationType;
    F32                                     mAlignedAngleOffset;
    bool                                    mKeepAligned;
    F32                                     mRandomAngleOffset;
    F32                                     mRandomArc;
    F32                                     mFixedAngleOffset;
    Vector2                                 mPivotPoint;
    //Physics Particles
    PhysicsParticleType                     mPhysicsParticleType;
    bool                                    mPhysicsParticles;
    //Physics Particles end---
    //Particle Target
    bool                                    mTargetParticle;
    Vector2                                 mTargetPosition;
    //Particle Target end---
    bool                                    mLinkEmissionRotation;
    bool                                    mIntenseParticles;
    bool                                    mSingleParticle;
    bool                                    mAttachPositionToEmitter;
    bool                                    mAttachRotationToEmitter;
    bool                                    mOldestInFront;

    bool                                    mBlendMode;
    S32                                     mSrcBlendFactor;
    S32                                     mDstBlendFactor;
    F32                                     mAlphaTest;

    bool                                    mStaticMode;
    AssetPtr<ImageAsset>                    mImageAsset;
    U32                                     mImageFrame;
    StringTableEntry                        mNamedImageFrame;
    bool                                    mRandomImageFrame;
    AssetPtr<AnimationAsset>                mAnimationAsset;
    bool                                    mUsingNamedFrame;

    /// Particle fields.
    ParticleAssetFieldCollection            mParticleFields;
    ParticleAssetFieldBaseVariation         mParticleLife;
    ParticleAssetFieldBaseVariation         mQuantity;
    ParticleAssetFieldBaseVariationLife     mSizeX;
    ParticleAssetFieldBaseVariationLife     mSizeY;
    ParticleAssetFieldBaseVariationLife     mSpeed;
    ParticleAssetFieldBaseVariationLife     mSpin;
    ParticleAssetFieldBaseVariationLife     mFixedForce;
    ParticleAssetFieldBaseVariationLife     mRandomMotion;
    ParticleAssetFieldBaseVariation         mEmissionForce;
    ParticleAssetFieldBaseVariation         mEmissionAngle;
    ParticleAssetFieldBaseVariation         mEmissionArc;
    ParticleAssetFieldLife                  mRedChannel;
    ParticleAssetFieldLife                  mGreenChannel;
    ParticleAssetFieldLife                  mBlueChannel;
    ParticleAssetFieldLife                  mAlphaChannel;

    Vector2                                 mLocalPivotAABB[4];

public:
    ParticleAssetEmitter();
    virtual ~ParticleAssetEmitter();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    inline ParticleAsset* getOwner( void ) const { return mOwner; }
    void setEmitterName( const char* pEmitterName );
    inline StringTableEntry getEmitterName( void ) const { return mEmitterName; }
    inline void setEmitterType( const EmitterType emitterType ) { mEmitterType = emitterType; refreshAsset(); }
    inline EmitterType getEmitterType( void ) { return mEmitterType; }
    inline void setEmitterOffset( const Vector2& offset ) { mEmitterOffset = offset; refreshAsset(); }
    inline const Vector2& getEmitterOffset( void ) const { return mEmitterOffset; }
    inline void setEmitterAngle( const F32 angle ) { mEmitterAngle = angle; refreshAsset(); }
    inline F32 getEmitterAngle( void ) const { return mEmitterAngle; }
    inline void setEmitterSize( const Vector2& size ) { mEmitterSize = size; refreshAsset(); }
    inline const Vector2& getEmitterSize( void ) const { return mEmitterSize; }
    inline void setFixedAspect( const bool fixedAspect ) { mFixedAspect = fixedAspect; refreshAsset(); }
    inline bool getFixedAspect( void ) const { return mFixedAspect; }
    void setFixedForceAngle( const F32 fixedForceAngle );
    inline F32 getFixedForceAngle( void ) const { return mFixedForceAngle; }
    inline const Vector2& getFixedForceDirection( void ) const { return mFixedForceDirection; }
    inline void setOrientationType( const ParticleOrientationType particleOrientationType ) { mOrientationType = particleOrientationType; refreshAsset(); }
    inline ParticleOrientationType getOrientationType( void ) const { return mOrientationType; }
    //Physics particles
    inline void setPhysicsParticleType(const PhysicsParticleType physicsParticleType) { mPhysicsParticleType = physicsParticleType; refreshAsset(); }
    inline PhysicsParticleType getPhysicsParticleType(void) const { return mPhysicsParticleType; }
    inline void setPhysicsParticles(const bool physicsPaticle) { mPhysicsParticles = physicsPaticle; refreshAsset(); }
    inline bool getPhysicsParticles(void) const { return mPhysicsParticles; }
    //Phyiscs particles end---
    //Particle Target
    inline void setTargetPosition(const Vector2& targetPos) { mTargetPosition = targetPos; }
    inline const Vector2& getTargetPosition(void) const { return mTargetPosition; }
    inline void setIsTargeting(const bool targetParticle) { mTargetParticle = targetParticle; refreshAsset(); }
    inline bool getIsTargeting(void) const { return mTargetParticle; }
    //Particle Target end---
    inline void setKeepAligned( const bool keepAligned ) { mKeepAligned = keepAligned; refreshAsset(); }
    inline bool getKeepAligned( void ) const { return mKeepAligned; }
    inline void setAlignedAngleOffset( const F32 angleOffset ) { mAlignedAngleOffset = angleOffset; refreshAsset(); }
    inline F32 getAlignedAngleOffset( void ) const { return mAlignedAngleOffset; }
    inline void setRandomAngleOffset( const F32 angleOffset ) { mRandomAngleOffset = angleOffset; refreshAsset(); };
    inline F32 getRandomAngleOffset( void ) const { return mRandomAngleOffset; }
    inline void setRandomArc( const F32 arc ) { mRandomArc = arc; refreshAsset(); }
    inline F32 getRandomArc( void ) const { return mRandomArc; }
    inline void setFixedAngleOffset( const F32 angleOffset ) { mFixedAngleOffset = angleOffset; refreshAsset(); }
    inline F32 getFixedAngleOffset( void ) const { return mFixedAngleOffset; }
    void setPivotPoint( const Vector2& pivotPoint );
    inline const Vector2& getPivotPoint( void ) const { return mPivotPoint; }

    inline void setLinkEmissionRotation( const bool linkEmissionRotation ) { mLinkEmissionRotation = linkEmissionRotation; refreshAsset(); }
    inline bool getLinkEmissionRotation( void ) const { return mLinkEmissionRotation; }
    inline void setIntenseParticles( const bool intenseParticles ) { mIntenseParticles = intenseParticles; refreshAsset(); }
    inline bool getIntenseParticles( void ) const { return mIntenseParticles; }
    inline void setSingleParticle( const bool singleParticle ) { mSingleParticle = singleParticle; refreshAsset(); }
    inline bool getSingleParticle( void ) const { return mSingleParticle; }
    inline void setAttachPositionToEmitter( const bool attachPositionToEmitter ) { mAttachPositionToEmitter = attachPositionToEmitter; refreshAsset(); }
    inline bool getAttachPositionToEmitter( void ) const { return mAttachPositionToEmitter; }
    inline void setAttachRotationToEmitter( const bool attachRotationToEmitter ) { mAttachRotationToEmitter = attachRotationToEmitter; refreshAsset(); }
    inline bool getAttachRotationToEmitter( void ) const { return mAttachRotationToEmitter; }
    inline void setOldestInFront( const bool oldestInFront ) { mOldestInFront = oldestInFront; refreshAsset();  }
    inline bool getOldestInFront( void ) const { return mOldestInFront; }
   
    inline bool isStaticFrameProvider( void ) const { return mStaticMode; }
    inline bool isUsingNamedImageFrame( void ) const { return mUsingNamedFrame; }
    bool setImage( const char* pAssetId, const U32 frame = 0 );
    bool setImage( const char* pAssetId, const char* frameName);
    inline StringTableEntry getImage( void ) const { return mImageAsset.getAssetId(); }
    bool setImageFrame( const U32 frame );
    inline U32 getImageFrame( void ) const { return mImageFrame; }
    bool setNamedImageFrame( const char* frameName);
    inline const char* getNamedImageFrame( void ) const { return mNamedImageFrame; }
    inline void setRandomImageFrame( const bool randomImageFrame ) { mRandomImageFrame = randomImageFrame; }
    inline bool getRandomImageFrame( void ) const { return mRandomImageFrame; }
    bool setAnimation( const char* animationName );
    inline StringTableEntry getAnimation( void ) const { return mAnimationAsset.getAssetId(); }
    inline AssetPtr<ImageAsset>& getImageAsset( void ) { return mImageAsset; }
    inline AssetPtr<AnimationAsset>& getAnimationAsset( void ) { return mAnimationAsset; }

    inline void setBlendMode( bool blendMode ) { mBlendMode = blendMode; refreshAsset(); }
    inline bool getBlendMode( void ) const { return mBlendMode; };
    inline void setSrcBlendFactor( const S32 blendFactor ) { mSrcBlendFactor = blendFactor; refreshAsset(); }
    inline S32 getSrcBlendFactor( void ) const { return mSrcBlendFactor; };
    inline void setDstBlendFactor( const S32 blendFactor ) { mDstBlendFactor = blendFactor; refreshAsset(); }
    inline S32 getDstBlendFactor( void ) const { return mDstBlendFactor; };
    inline void setAlphaTest( const F32 alphaTest ) { mAlphaTest = alphaTest; refreshAsset(); }
    inline F32 getAlphaTest( void ) const { return mAlphaTest; }

    inline ParticleAssetFieldCollection& getParticleFields( void ) { return mParticleFields; }

    inline ParticleAssetField& getParticleLifeBaseField( void ) { return mParticleLife.getBase(); }
    inline ParticleAssetField& getParticleLifeVariationField( void ) { return mParticleLife.getVariation(); }
    inline ParticleAssetField& getQuantityBaseField( void ) { return mQuantity.getBase(); }
    inline ParticleAssetField& getQuantityVariationField( void ) { return mQuantity.getVariation(); }
    inline ParticleAssetField& getSizeXBaseField( void ) { return mSizeX.getBase(); }
    inline ParticleAssetField& getSizeXVariationField( void ) { return mSizeX.getVariation(); }
    inline ParticleAssetField& getSizeXLifeField( void ) { return mSizeX.getLife(); }
    inline ParticleAssetField& getSizeYBaseField( void ) { return mSizeY.getBase(); }
    inline ParticleAssetField& getSizeYVariationField( void ) { return mSizeY.getVariation(); }
    inline ParticleAssetField& getSizeYLifeField( void ) { return mSizeY.getLife(); }
    inline ParticleAssetField& getSpeedBaseField( void ) { return mSpeed.getBase(); }
    inline ParticleAssetField& getSpeedVariationField( void ) { return mSpeed.getVariation(); }
    inline ParticleAssetField& getSpeedLifeField( void ) { return mSpeed.getLife(); }
    inline ParticleAssetField& getSpinBaseField( void ) { return mSpin.getBase(); }
    inline ParticleAssetField& getSpinVariationField( void ) { return mSpin.getVariation(); }
    inline ParticleAssetField& getSpinLifeField( void ) { return mSpin.getLife(); }
    inline ParticleAssetField& getFixedForceBaseField( void ) { return mFixedForce.getBase(); }
    inline ParticleAssetField& getFixedForceVariationField( void ) { return mFixedForce.getVariation(); }
    inline ParticleAssetField& getFixedForceLifeField( void ) { return mFixedForce.getLife(); }
    inline ParticleAssetField& getRandomMotionBaseField( void ) { return mRandomMotion.getBase(); }
    inline ParticleAssetField& getRandomMotionVariationField( void ) { return mRandomMotion.getVariation(); }
    inline ParticleAssetField& getRandomMotionLifeField( void ) { return mRandomMotion.getLife(); }
    inline ParticleAssetField& getEmissionForceForceBaseField( void ) { return mEmissionForce.getBase(); }
    inline ParticleAssetField& getEmissionForceVariationField( void ) { return mEmissionForce.getVariation(); }
    inline ParticleAssetField& getEmissionAngleBaseField( void ) { return mEmissionAngle.getBase(); }
    inline ParticleAssetField& getEmissionAngleVariationField( void ) { return mEmissionAngle.getVariation(); }
    inline ParticleAssetField& getEmissionArcBaseField( void ) { return mEmissionArc.getBase(); }
    inline ParticleAssetField& getEmissionArcVariationField( void ) { return mEmissionArc.getVariation(); }
    inline ParticleAssetField& getRedChannelLifeField( void ) { return mRedChannel.getLife(); }
    inline ParticleAssetField& getGreenChannelLifeField( void ) { return mGreenChannel.getLife(); }
    inline ParticleAssetField& getBlueChannelLifeField( void ) { return mBlueChannel.getLife(); }
    inline ParticleAssetField& getAlphaChannelLifeField( void ) { return mAlphaChannel.getLife(); }

    inline const Vector2& getLocalPivotAABB0( void ) { return mLocalPivotAABB[0]; }
    inline const Vector2& getLocalPivotAABB1( void ) { return mLocalPivotAABB[1]; }
    inline const Vector2& getLocalPivotAABB2( void ) { return mLocalPivotAABB[2]; }
    inline const Vector2& getLocalPivotAABB3( void ) { return mLocalPivotAABB[3]; }

    static EmitterType getEmitterTypeEnum(const char* label);
    static const char* getEmitterTypeDescription( const EmitterType emitterType );
    //phyiscs particles
    static PhysicsParticleType getPhysicsParticleTypeEnum(const char * label);
    static const char* getPhysicsParticleTypeDescription(const PhysicsParticleType particleType);
    //physics particles end----
    static ParticleOrientationType getOrientationTypeEnum(const char* label);
    static const char* getOrientationTypeDescription( const ParticleOrientationType orientationType );

    /// Declare Console Object.
    DECLARE_CONOBJECT(ParticleAssetEmitter);

private:
    void setOwner( ParticleAsset* pParticleAsset );
    void refreshAsset( void );

protected:
    virtual void onAssetRefreshed( AssetPtrBase* pAssetPtrBase );
    void onTamlCustomWrite( TamlCustomNodes& customNodes );
    void onTamlCustomRead( const TamlCustomNodes& customNodes );

    static bool     setEmitterName(void* obj, const char* data)                         { static_cast<ParticleAssetEmitter*>(obj)->setEmitterName( data ); return false; }
    static bool     setEmitterType(void* obj, const char* data)                         { static_cast<ParticleAssetEmitter*>(obj)->setEmitterType( getEmitterTypeEnum(data) ); return false; }
    static bool     writeEmitterType( void* obj, StringTableEntry pFieldName )          { return static_cast<ParticleAssetEmitter*>(obj)->getEmitterType() != POINT_EMITTER; }
    static bool     setEmitterOffset(void* obj, const char* data)                       { static_cast<ParticleAssetEmitter*>(obj)->setEmitterOffset(Vector2(data)); return false; }
    static bool     writeEmitterOffset( void* obj, StringTableEntry pFieldName )        { return static_cast<ParticleAssetEmitter*>(obj)->getEmitterOffset().notZero(); }

    static bool     setIsTargeting(void* obj, const char* data)                      { static_cast<ParticleAssetEmitter*>(obj)->setIsTargeting(dAtob(data)); return false; }
    static bool     writeTargetParticle(void* obj, StringTableEntry pFieldName)         { return static_cast<ParticleAssetEmitter*>(obj)->getIsTargeting() == false; }
    static bool     setTargetPosition(void* obj, const char* data)                      { static_cast<ParticleAssetEmitter*>(obj)->setTargetPosition(Vector2(data)); return false; }
    static bool     writeTargetPosition(void* obj, StringTableEntry pFieldName)         { return static_cast<ParticleAssetEmitter*>(obj)->getTargetPosition().notZero(); }

    static bool     setEmitterAngle(void* obj, const char* data)                        { static_cast<ParticleAssetEmitter*>(obj)->setEmitterAngle(dAtof(data)); return false; }
    static bool     writeEmitterAngle( void* obj, StringTableEntry pFieldName )         { return mNotZero(static_cast<ParticleAssetEmitter*>(obj)->getEmitterAngle()); }
    static bool     setEmitterSize(void* obj, const char* data)                         { static_cast<ParticleAssetEmitter*>(obj)->setEmitterSize(Vector2(data)); return false; }
    static bool     writeEmitterSize( void* obj, StringTableEntry pFieldName )          { return static_cast<ParticleAssetEmitter*>(obj)->getEmitterSize().notEqual(Vector2(10.0f, 10.0f)); }
    static bool     setFixedAspect(void* obj, const char* data)                         { static_cast<ParticleAssetEmitter*>(obj)->setFixedAspect(dAtob(data)); return false; }
    static bool     writeFixedAspect( void* obj, StringTableEntry pFieldName )          { return static_cast<ParticleAssetEmitter*>(obj)->getFixedAspect() == false; }
    static bool     setFixedForceAngle(void* obj, const char* data)                     { static_cast<ParticleAssetEmitter*>(obj)->setFixedForceAngle(dAtof(data)); return false; }
    static bool     writeFixedForceAngle( void* obj, StringTableEntry pFieldName )      { return mNotZero(static_cast<ParticleAssetEmitter*>(obj)->getFixedForceAngle() ); }
    //Physics Particles
    static bool     setPhysicsParticleType(void* obj, const char* data)                { static_cast<ParticleAssetEmitter*>(obj)->setPhysicsParticleType(getPhysicsParticleTypeEnum(data)); return false; }
    static bool     writePhysicsParticleType(void* obj, StringTableEntry pFieldName)   { return static_cast<ParticleAssetEmitter*>(obj)->getPhysicsParticleType() != b2_zombieParticle; }
    static bool     setPhysicsParticles(void* obj, const char* data)                    { static_cast<ParticleAssetEmitter*>(obj)->setPhysicsParticles(dAtob(data)); return false; }
    static bool     writePhysicsParticles(void* obj, StringTableEntry pFieldName)       { return static_cast<ParticleAssetEmitter*>(obj)->getPhysicsParticles() == false; }
    //Physics Particles end---

    static bool     setOrientationType(void* obj, const char* data)                     { static_cast<ParticleAssetEmitter*>(obj)->setOrientationType( getOrientationTypeEnum(data) ); return false; }
    static bool     writeOrientationType( void* obj, StringTableEntry pFieldName )      { return static_cast<ParticleAssetEmitter*>(obj)->getOrientationType() != FIXED_ORIENTATION; }
    static bool     setKeepAligned(void* obj, const char* data)                         { static_cast<ParticleAssetEmitter*>(obj)->setKeepAligned(dAtob(data)); return false; }
    static bool     writeKeepAligned( void* obj, StringTableEntry pFieldName )          { return static_cast<ParticleAssetEmitter*>(obj)->getKeepAligned() == true; }
    static bool     setAlignedAngleOffset(void* obj, const char* data)                  { static_cast<ParticleAssetEmitter*>(obj)->setAlignedAngleOffset(dAtof(data)); return false; }
    static bool     writeAlignedAngleOffset( void* obj, StringTableEntry pFieldName )   { return mNotZero(static_cast<ParticleAssetEmitter*>(obj)->getAlignedAngleOffset() ); }
    static bool     setRandomAngleOffset(void* obj, const char* data)                   { static_cast<ParticleAssetEmitter*>(obj)->setRandomAngleOffset(dAtof(data)); return false; }
    static bool     writeRandomAngleOffset( void* obj, StringTableEntry pFieldName )    { return mNotZero(static_cast<ParticleAssetEmitter*>(obj)->getRandomAngleOffset() ); }
    static bool     setRandomArc(void* obj, const char* data)                           { static_cast<ParticleAssetEmitter*>(obj)->setRandomArc(dAtof(data)); return false; }
    static bool     writeRandomArc( void* obj, StringTableEntry pFieldName )            { return mNotEqual(static_cast<ParticleAssetEmitter*>(obj)->getRandomArc(), 360.0f ); }
    static bool     setFixedAngleOffset(void* obj, const char* data)                    { static_cast<ParticleAssetEmitter*>(obj)->setFixedAngleOffset(dAtof(data)); return false; }
    static bool     writeFixedAngleOffset( void* obj, StringTableEntry pFieldName )     { return mNotZero(static_cast<ParticleAssetEmitter*>(obj)->getFixedAngleOffset() ); }
    static bool     setPivotPoint(void* obj, const char* data)                          { static_cast<ParticleAssetEmitter*>(obj)->setPivotPoint(Vector2(data)); return false; }
    static bool     writePivotPoint( void* obj, StringTableEntry pFieldName )           { return static_cast<ParticleAssetEmitter*>(obj)->getPivotPoint().notZero(); }
    static bool     setLinkEmissionRotation(void* obj, const char* data)                { static_cast<ParticleAssetEmitter*>(obj)->setLinkEmissionRotation(dAtob(data)); return false; }
    static bool     writeLinkEmissionRotation( void* obj, StringTableEntry pFieldName ) { return static_cast<ParticleAssetEmitter*>(obj)->getLinkEmissionRotation() == false; }
    static bool     setIntenseParticles(void* obj, const char* data)                    { static_cast<ParticleAssetEmitter*>(obj)->setIntenseParticles(dAtob(data)); return false; }
    static bool     writeIntenseParticles( void* obj, StringTableEntry pFieldName )     { return static_cast<ParticleAssetEmitter*>(obj)->getIntenseParticles() == true; }
    static bool     setSingleParticle(void* obj, const char* data)                      { static_cast<ParticleAssetEmitter*>(obj)->setSingleParticle(dAtob(data)); return false; }
    static bool     writeSingleParticle( void* obj, StringTableEntry pFieldName )       { return static_cast<ParticleAssetEmitter*>(obj)->getSingleParticle() == true; }
    static bool     setAttachPositionToEmitter(void* obj, const char* data)             { static_cast<ParticleAssetEmitter*>(obj)->setAttachPositionToEmitter(dAtob(data)); return false; }
    static bool     writeAttachPositionToEmitter( void* obj, StringTableEntry pFieldName ) { return static_cast<ParticleAssetEmitter*>(obj)->getAttachPositionToEmitter() == true; }
    static bool     setAttachRotationToEmitter(void* obj, const char* data)             { static_cast<ParticleAssetEmitter*>(obj)->setAttachRotationToEmitter(dAtob(data)); return false; }
    static bool     writeAttachRotationToEmitter( void* obj, StringTableEntry pFieldName ) { return static_cast<ParticleAssetEmitter*>(obj)->getAttachRotationToEmitter() == true; }
    static bool     setOldestInFront(void* obj, const char* data)                       { static_cast<ParticleAssetEmitter*>(obj)->setOldestInFront(dAtob(data)); return false; }
    static bool     writeOldestInFront( void* obj, StringTableEntry pFieldName )        { return static_cast<ParticleAssetEmitter*>(obj)->getOldestInFront() == true; }

    static bool     setImage(void* obj, const char* data)                               { static_cast<ParticleAssetEmitter*>(obj)->setImage(data); return false; };
    static const char* getImage(void* obj, const char* data)                            { return static_cast<ParticleAssetEmitter*>(obj)->getImage(); }
    static bool     writeImage( void* obj, StringTableEntry pFieldName )                { ParticleAssetEmitter* pCastObject = static_cast<ParticleAssetEmitter*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAsset.notNull(); }
    static bool     setImageFrame(void* obj, const char* data)                          { static_cast<ParticleAssetEmitter*>(obj)->setImageFrame(dAtoi(data)); return false; };
    static bool     writeImageFrame( void* obj, StringTableEntry pFieldName )           { ParticleAssetEmitter* pCastObject = static_cast<ParticleAssetEmitter*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAsset.notNull() && !pCastObject->getRandomImageFrame(); }
    static bool     setNamedImageFrame(void* obj, const char* data)                     { static_cast<ParticleAssetEmitter*>(obj)->setNamedImageFrame(data); return false; };
    static bool     writeNamedImageFrame( void* obj, StringTableEntry pFieldName )      { ParticleAssetEmitter* pCastObject = static_cast<ParticleAssetEmitter*>(obj); if ( !pCastObject->isStaticFrameProvider() || !pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->mImageAsset.notNull() && !pCastObject->getRandomImageFrame(); }
    static bool     setRandomImageFrame(void* obj, const char* data)                    { static_cast<ParticleAssetEmitter*>(obj)->setRandomImageFrame(dAtob(data)); return false; };
    static bool     writeRandomImageFrame( void* obj, StringTableEntry pFieldName )     { ParticleAssetEmitter* pCastObject = static_cast<ParticleAssetEmitter*>(obj); if ( !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->getRandomImageFrame(); }
    static bool     setAnimation(void* obj, const char* data)                           { static_cast<ParticleAssetEmitter*>(obj)->setAnimation(data); return false; };
    static const char* getAnimation(void* obj, const char* data)                        { return static_cast<ParticleAssetEmitter*>(obj)->getAnimation(); }
    static bool     writeAnimation( void* obj, StringTableEntry pFieldName )            { ParticleAssetEmitter* pCastObject = static_cast<ParticleAssetEmitter*>(obj); if ( pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mAnimationAsset.notNull(); }

    static bool     setBlendMode(void* obj, const char* data)                           { static_cast<ParticleAssetEmitter*>(obj)->setBlendMode(dAtob(data)); return false; }
    static bool     writeBlendMode( void* obj, StringTableEntry pFieldName )            { return static_cast<ParticleAssetEmitter*>(obj)->getBlendMode() == false; }
    static bool     setSrcBlendFactor(void* obj, const char* data)                      { static_cast<ParticleAssetEmitter*>(obj)->setSrcBlendFactor(SceneObject::getSrcBlendFactorEnum(data)); return false; }
    static bool     writeSrcBlendFactor( void* obj, StringTableEntry pFieldName )       { return static_cast<ParticleAssetEmitter*>(obj)->getSrcBlendFactor() != GL_SRC_ALPHA; }
    static bool     setDstBlendFactor(void* obj, const char* data)                      { static_cast<ParticleAssetEmitter*>(obj)->setDstBlendFactor(SceneObject::getDstBlendFactorEnum(data)); return false; }
    static bool     writeDstBlendFactor( void* obj, StringTableEntry pFieldName )       { return static_cast<ParticleAssetEmitter*>(obj)->getDstBlendFactor() != GL_ONE_MINUS_SRC_ALPHA; }
    static bool     setAlphaTest(void* obj, const char* data)                           { static_cast<ParticleAssetEmitter*>(obj)->setAlphaTest(dAtof(data)); return false; }
    static bool     writeAlphaTest( void* obj, StringTableEntry pFieldName )            { return static_cast<ParticleAssetEmitter*>(obj)->getAlphaTest() >= 0.0f; }
};

#endif // _PARTICLE_ASSET_EMITTER_H_
