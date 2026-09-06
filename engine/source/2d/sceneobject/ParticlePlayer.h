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

#ifndef _PARTICLE_PLAYER_H_
#define _PARTICLE_PLAYER_H_

#ifndef _PARTICLE_ASSET_H_
#include "2d/assets/ParticleAsset.h"
#endif

#ifndef _PARTICLE_SYSTEM_H_
#include "2d/core/ParticleSystem.h"
#endif

//-----------------------------------------------------------------------------

#define PARTICLE_PLAYER_EMISSION_RATE_SCALE     "$pref::T2D::ParticlePlayerEmissionRateScale"
#define PARTICLE_PLAYER_SIZE_SCALE              "$pref::T2D::ParticlePlayerSizeScale"
#define PARTICLE_PLAYER_FORCE_SCALE             "$pref::T2D::ParticlePlayerForceScale"
#define PARTICLE_PLAYER_TIME_SCALE              "$pref::T2D::ParticlePlayerTimeScale"

//-----------------------------------------------------------------------------

class ParticlePlayer : public SceneObject, protected AssetPtrCallback
{
private:
    typedef SceneObject Parent;

    /// Emitter node.
    class EmitterNode
    {
    private:
        ParticlePlayer*                 mOwner;
        ParticleAssetEmitter*           mpAssetEmitter;
        ParticleSystem::ParticleNode    mParticleNodeHead;
        F32                             mTimeSinceLastGeneration;
        bool                            mPaused;
        bool                            mVisible;

    public:
        EmitterNode( ParticlePlayer* pParticlePlayer, ParticleAssetEmitter* pParticleAssetEmitter )
        {
            // Sanity!
            AssertFatal( pParticlePlayer != NULL, "EmitterNode() - Cannot have a NULL owner." );
            AssertFatal( pParticleAssetEmitter != NULL, "EmitterNode() - Cannot have a NULL particle asset emitter." );

            // Set owner.
            mOwner = pParticlePlayer;

            // Set asset emitter.
            mpAssetEmitter = pParticleAssetEmitter;

            // Set emitter not paused.
            mPaused = false;

            // Set emitter visible.
            mVisible = true;

            // Reset time since last generation.
            mTimeSinceLastGeneration = 0.0f;

            // Reset the node head.
            mParticleNodeHead.mNextNode = mParticleNodeHead.mPreviousNode = &mParticleNodeHead;
        }

        ~EmitterNode()
        {
            freeAllParticles();
        }

        inline ParticlePlayer* getOwner( void ) const { return mOwner; }
        inline ParticleAssetEmitter* getAssetEmitter( void ) const { return mpAssetEmitter; }

        inline bool getActiveParticles( void ) const { return mParticleNodeHead.mNextNode != &mParticleNodeHead; }

        inline ParticleSystem::ParticleNode* getFirstParticle( void ) const { return mParticleNodeHead.mNextNode; }
        inline ParticleSystem::ParticleNode* getLastParticle( void ) const { return mParticleNodeHead.mPreviousNode; }
        inline ParticleSystem::ParticleNode* getParticleNodeHead( void ) { return &mParticleNodeHead; }

        inline void setTimeSinceLastGeneration( const F32 timeSinceLastGeneration ) { mTimeSinceLastGeneration = timeSinceLastGeneration; }
        inline F32 getTimeSinceLastGeneration( void ) const { return mTimeSinceLastGeneration; }

        inline void setPaused( const bool paused ) { mPaused = paused; }
        inline bool getPaused( void ) const { return mPaused; }

        inline void setVisible( const bool visible ) { mVisible = visible; }
        inline bool getVisible( void ) const { return mVisible; }

        ParticleSystem::ParticleNode* createParticle( void );
        void freeParticle( ParticleSystem::ParticleNode* pParticleNode );
        void freeAllParticles( void );        
    };

    typedef Vector<EmitterNode*> typeEmitterVector;

    AssetPtr<ParticleAsset>     mParticleAsset;
    typeEmitterVector           mEmitters;

    bool                        mCameraIdle;
    F32                         mCameraIdleDistance;

    bool						mParticleInterpolation;

    bool                        mPlaying;
    bool                        mPaused;
    F32                         mAge;
    F32                         mEmissionRateScale;
    F32                         mSizeScale;
    F32                         mForceScale;
    F32                         mTimeScale;

    bool                        mWaitingForParticles;
    bool                        mWaitingForDelete;

public:
    ParticlePlayer();
    virtual ~ParticlePlayer();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);
    virtual void safeDelete( void );

    virtual void preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    void integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats );
    void interpolateObject( const F32 timeDelta );

    virtual bool validRender( void ) const { return mParticleAsset.notNull() && mParticleAsset->isAssetValid(); }
    virtual bool shouldRender( void ) const { return true; }
    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );
    virtual void sceneRenderOverlay( const SceneRenderState* sceneRenderState );

    void setParticle( const char* pAssetId );
    const char* getParticle( void ) const { return mParticleAsset->getAssetId(); }

    inline void setCameraIdleDistance( const F32 idleDistance ) { mCameraIdleDistance = idleDistance; mCameraIdle = false; }
    inline F32 getCameraIdleDistance( void ) const { return mCameraIdleDistance; }

    inline void setParticleInterpolation( const bool interpolation ) { mParticleInterpolation = interpolation; }
    inline bool getParticleInterpolation( void ) const { return mParticleInterpolation; }

    inline void setEmissionRateScale( const F32 scale ) { mEmissionRateScale = scale; }
    inline F32 getEmissionRateScale( void  ) const { return mEmissionRateScale; }

    inline void setSizeScale( const F32 scale ) { mSizeScale = scale; }
    inline F32 getSizeScale( void  ) const { return mSizeScale; }

    inline void setForceScale( const F32 scale ) { mForceScale = scale; }
    inline F32 getForceScale( void  ) const { return mForceScale; }

    inline void setTimeScale( const F32 scale ) { mTimeScale = scale; }
    inline F32 getTimeScale( void  ) const { return mTimeScale; }

    inline const U32 getEmitterCount( void ) const { return (U32)mEmitters.size(); }

    void setEmitterPaused( const bool paused, const U32 emitterIndex );
    bool getEmitterPaused( const U32 emitterIndex );
    void setEmitterVisible( const bool visible, const U32 emitterIndex );
    bool getEmitterVisible( const U32 emitterIndex );

    bool play( const bool resetParticles );
    void stop( const bool waitForParticles, const bool killEffect );
    inline bool getIsPlaying( void ) const { return mPlaying; };
    inline void setPaused( const bool paused ) { mPaused = paused; }
    inline bool getPaused( void ) const { return mPaused; }

    /// Combine an emitter's authored emission angle with the player's own rotation.
    /// The authored angle is in DEGREES (that is how the field is edited, and how the
    /// rest of the emission math carries it), while a SceneObject's angle is in
    /// RADIANS, so the player angle has to be converted before the two are added.
    static inline F32 calculateEmissionAngle( const F32 emissionAngleDegrees, const F32 playerAngleRadians, const bool linkEmissionRotation )
    {
        return linkEmissionRotation ? emissionAngleDegrees + mRadToDeg( playerAngleRadians ) : emissionAngleDegrees;
    }

    /// Declare Console Object.
    DECLARE_CONOBJECT(ParticlePlayer);

protected:
    virtual void OnRegisterScene( Scene* pScene );
    virtual void OnUnregisterScene( Scene* pScene );

    virtual void onAssetRefreshed( AssetPtrBase* pAssetPtrBase );

    /// Particle Creation/Integration.
    void configureParticle( EmitterNode* pEmitterNode, ParticleSystem::ParticleNode* pParticleNode );
    void integrateParticle( EmitterNode* pEmitterNode, ParticleSystem::ParticleNode* pParticleNode, const F32 particleAge, const F32 elapsedTime );

    /// Persistence.
    virtual void onTamlAddParent( SimObject* pParentObject );

    static bool     setParticle(void* obj, const char* data)                                { static_cast<ParticlePlayer*>( obj )->setParticle(data); return false; };
    static bool     writeCameraIdleDistance( void* obj, StringTableEntry pFieldName )       { return static_cast<ParticlePlayer*>( obj )->getCameraIdleDistance() > 0.0f; }
    static bool     writeParticleInterpolation( void* obj, StringTableEntry pFieldName )    { return static_cast<ParticlePlayer*>( obj )->getParticleInterpolation(); }
    static bool     writeEmissionRateScale( void* obj, StringTableEntry pFieldName )        { return !mIsOne( static_cast<ParticlePlayer*>( obj )->getEmissionRateScale() ); }
    static bool     writeSizeScale( void* obj, StringTableEntry pFieldName )                { return !mIsOne( static_cast<ParticlePlayer*>( obj )->getSizeScale() ); }
    static bool     writeForceScale( void* obj, StringTableEntry pFieldName )               { return !mIsOne( static_cast<ParticlePlayer*>( obj )->getForceScale() ); }
    static bool     writeTimeScale( void* obj, StringTableEntry pFieldName )                { return !mIsOne( static_cast<ParticlePlayer*>( obj )->getTimeScale() ); }

private:
    void initializeParticleAsset( void );
    void destroyParticleAsset( void );
};

#endif // _PARTICLE_PLAYER_H_
