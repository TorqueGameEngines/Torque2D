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

#include "2d/sceneobject/ParticlePlayer.h"

// Script bindings.
#include "2d/sceneobject/ParticlePlayer_ScriptBinding.h"


//------------------------------------------------------------------------------

ParticleSystem::ParticleNode* ParticlePlayer::EmitterNode::createParticle( void )
{
    // Sanity!
    AssertFatal( mOwner != NULL, "ParticlePlayer::EmitterNode::createParticle() - Cannot create a particle with a NULL owner." );
  
    // Fetch a free node,
    ParticleSystem::ParticleNode* pFreeParticleNode = ParticleSystem::Instance->createParticle();

    // Insert node into emitter chain.
    pFreeParticleNode->mNextNode        = mParticleNodeHead.mNextNode;
    pFreeParticleNode->mPreviousNode    = &mParticleNodeHead;
    mParticleNodeHead.mNextNode         = pFreeParticleNode;
    pFreeParticleNode->mNextNode->mPreviousNode = pFreeParticleNode;

    // Configure the node.
    mOwner->configureParticle( this, pFreeParticleNode );

    return pFreeParticleNode;
}

//------------------------------------------------------------------------------

void ParticlePlayer::EmitterNode::freeParticle( ParticleSystem::ParticleNode* pParticleNode )
{
    // Sanity!
    AssertFatal( mOwner != NULL, "ParticlePlayer::EmitterNode::freeParticle() - Cannot free a particle with a NULL owner." );

    // Deallocate the assets.
    pParticleNode->mFrameProvider.deallocateAssets();

    // Remove the node from the emitter chain.
    pParticleNode->mPreviousNode->mNextNode = pParticleNode->mNextNode;
    pParticleNode->mNextNode->mPreviousNode = pParticleNode->mPreviousNode;
   
    // Free the node.
    ParticleSystem::Instance->freeParticle( pParticleNode );
}

//------------------------------------------------------------------------------

void ParticlePlayer::EmitterNode::freeAllParticles( void )
{
    // Sanity!
    AssertFatal( mOwner != NULL, "ParticlePlayer::EmitterNode::freeAllParticles() - Cannot free all particles with a NULL owner." );

    // Free all the nodes,
    while( mParticleNodeHead.mNextNode != &mParticleNodeHead )
    {
        freeParticle( mParticleNodeHead.mNextNode );
    }
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(ParticlePlayer);

//------------------------------------------------------------------------------

ParticlePlayer::ParticlePlayer() :
                    mPlaying( false ),
                    mPaused( false ),
                    mAge( 0.0f ),
                    mParticleInterpolation( false ),
                    mCameraIdleDistance( 0.0f ),
                    mCameraIdle( false ),
                    mWaitingForParticles( false ),
                    mWaitingForDelete( false )
{
    // Fetch the particle player scales.
    mEmissionRateScale = Con::getFloatVariable( PARTICLE_PLAYER_EMISSION_RATE_SCALE, 1.0f );
    mSizeScale         = Con::getFloatVariable( PARTICLE_PLAYER_SIZE_SCALE, 1.0f );
    mForceScale        = Con::getFloatVariable( PARTICLE_PLAYER_FORCE_SCALE, 1.0f );
    mTimeScale         = Con::getFloatVariable( PARTICLE_PLAYER_TIME_SCALE, 1.0f );
     
    // Register for refresh notifications.
    mParticleAsset.registerRefreshNotify( this );
}

//------------------------------------------------------------------------------

ParticlePlayer::~ParticlePlayer()
{
    // Destroy the particle asset.
    destroyParticleAsset();
}

//------------------------------------------------------------------------------

void ParticlePlayer::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    addProtectedField( "Particle", TypeParticleAssetPtr, Offset(mParticleAsset, ParticlePlayer), &setParticle, &defaultProtectedGetFn, defaultProtectedWriteFn, "" );
    addProtectedField( "CameraIdleDistance", TypeF32, Offset(mCameraIdleDistance, ParticlePlayer),&defaultProtectedSetFn, &defaultProtectedGetFn, &writeCameraIdleDistance,"" );
    addProtectedField( "ParticleInterpolation", TypeBool, Offset(mParticleInterpolation, ParticlePlayer), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeParticleInterpolation,"" );
    addProtectedField( "EmissionRateScale", TypeF32, Offset(mEmissionRateScale, ParticlePlayer), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeEmissionRateScale, "" );
    addProtectedField( "SizeScale", TypeF32, Offset(mSizeScale, ParticlePlayer), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeSizeScale, "" );
    addProtectedField( "ForceScale", TypeF32, Offset(mForceScale, ParticlePlayer), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeForceScale, "" );
    addProtectedField( "TimeScale", TypeF32, Offset(mTimeScale, ParticlePlayer), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeTimeScale, "" );
}

//------------------------------------------------------------------------------

void ParticlePlayer::copyTo(SimObject* object)
{
    // Fetch particle asset object.
   ParticlePlayer* pParticlePlayer = static_cast<ParticlePlayer*>( object );

   // Sanity!
   AssertFatal( pParticlePlayer != NULL, "ParticlePlayer::copyTo() - Object is not the correct type.");

   // Copy parent.
   Parent::copyTo( object );

   // Copy the fields.
   pParticlePlayer->setParticle( getParticle() );
   pParticlePlayer->setCameraIdleDistance( getCameraIdleDistance() );
   pParticlePlayer->setParticleInterpolation( getParticleInterpolation() );
   pParticlePlayer->setEmissionRateScale( getEmissionRateScale() );
   pParticlePlayer->setSizeScale( getSizeScale() );
   pParticlePlayer->setForceScale( getForceScale() );
   pParticlePlayer->setTimeScale( getTimeScale() );
}

//------------------------------------------------------------------------------

void ParticlePlayer::onAssetRefreshed( AssetPtrBase* pAssetPtrBase )
{
    // Initialize the particle asset.
    initializeParticleAsset();
}

//-----------------------------------------------------------------------------

void ParticlePlayer::safeDelete( void )
{
    // Finish if we already waiting for delete.
    if ( mWaitingForDelete )
        return;

    // Is the player plating?
    if ( mPlaying )
    {
        // Yes, so stop playing and allow it to kill itself.
        stop(true, true);
        return;
    }

    // Call parent which will deal with the deletion.
    Parent::safeDelete();
}

//-----------------------------------------------------------------------------

void ParticlePlayer::OnRegisterScene( Scene* pScene )
{
    // Call parent.
    Parent::OnRegisterScene( pScene );

    // Add always in scope.
    pScene->getWorldQuery()->addAlwaysInScope( this );

    // Play the the particles if appropriate.
    if ( mParticleAsset.notNull() && mParticleAsset->getEmitterCount() > 0 )
        play( true );
}

//-----------------------------------------------------------------------------

void ParticlePlayer::OnUnregisterScene( Scene* pScene )
{
    // Stop the player.
    stop( false, false );

    // Remove always in scope.
    pScene->getWorldQuery()->removeAlwaysInScope( this );

    // Call parent.
    Parent::OnUnregisterScene( pScene );
}

//-----------------------------------------------------------------------------

void ParticlePlayer::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::preIntegrate( totalTime, elapsedTime, pDebugStats );

    // Finish if the camera idle distance is zero.
    if ( mIsZero(mCameraIdleDistance) || !validRender() )
        return;

    // Fetch current position.
    const Vector2 position = getPosition();

    // Calculate camera idle distance squared.
    const F32 cameraIdleDistanceSqr = mCameraIdleDistance * mCameraIdleDistance;

    // Fetch scene windows.
    SimSet& sceneWindows = getScene()->getAttachedSceneWindows();

    // Find a scene window that stops the pause.
    for( SimSet::iterator itr = sceneWindows.begin(); itr != sceneWindows.end(); itr++ )
    {
        // Fetch the scene window.
        SceneWindow* pSceneWindow = static_cast<SceneWindow*>(*itr);

        // Are we within the camera distance?
        if ( (pSceneWindow->getCameraPosition() - position).LengthSquared() < cameraIdleDistanceSqr )
        {
            // Yes, so play.
            if ( !getIsPlaying() )
                play( true );

            // Flag as not camera idle.
            mCameraIdle = false;

            return;
        }
    }

    // If playing then stop.
    if ( getIsPlaying() )
        stop( false, false );

    // Flag as camera idle.
    mCameraIdle = true;
}

//------------------------------------------------------------------------------

void ParticlePlayer::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );

    // Finish if no need to integrate.
    if (    !mPlaying ||
            mPaused ||
            mEmitters.size() == 0 )
        return;

    // Calculate scaled time.
    const F32 scaledTime = elapsedTime * mTimeScale;

    // Fetch particle asset.
    ParticleAsset* pParticleAsset = mParticleAsset;

    // Finish if no particle asset assigned.
    if ( pParticleAsset == NULL )
        return;

    // Reset active particle count.
    U32 activeParticleCount = 0;

    // Is the camera idle?
    if ( !mCameraIdle )
    {
        // No, so update the particle player age.
        mAge += scaledTime;

        // Iterate the emitters.
        for( typeEmitterVector::iterator emitterItr = mEmitters.begin(); emitterItr != mEmitters.end(); ++emitterItr )
        {
            // Fetch the emitter node.
            EmitterNode* pEmitterNode = *emitterItr;

            // Fetch the asset emitter.
            ParticleAssetEmitter* pParticleAssetEmitter = pEmitterNode->getAssetEmitter();

            // Fetch the first particle node.
            ParticleSystem::ParticleNode* pParticleNode = pEmitterNode->getFirstParticle();

            // Fetch the particle node head.
            ParticleSystem::ParticleNode* pParticleNodeHead = pEmitterNode->getParticleNodeHead();

            // Process All particle nodes.
            while ( pParticleNode != pParticleNodeHead )
            {
                // Update the particle age.
                pParticleNode->mParticleAge += scaledTime;

                // Has the particle expired?
                // NOTE:-   If we're in single-particle mode then the particle lives as long as the particle player does.
                if (    ( !pParticleAssetEmitter->getSingleParticle() && pParticleNode->mParticleAge > pParticleNode->mParticleLifetime ) ||
                        ( mIsZero(pParticleNode->mParticleLifetime) ) )
                {
                    // Yes, so fetch next particle before we kill it.
                    pParticleNode = pParticleNode->mNextNode;

                    // Kill the particle.
                    // NOTE:-   Because we move to the next particle the particle to kill is now the previous!
                    pEmitterNode->freeParticle( pParticleNode->mPreviousNode );
                }
                else
                {
                    // No, so integrate the particle.
                    integrateParticle( pEmitterNode, pParticleNode, pParticleNode->mParticleAge / pParticleNode->mParticleLifetime, scaledTime );

                    // Move to the next particle node.
                    pParticleNode = pParticleNode->mNextNode;

                    // Only count particles when not in single-particle mode.
                    activeParticleCount++;
                }
            };

            // Skip generating new particles if the emitter is paused.
            if ( pEmitterNode->getPaused() )
                continue;

            // Are we in single-particle mode?
            if ( pParticleAssetEmitter->getSingleParticle() )
            {
                // Yes, so do we have a single particle yet?
                if ( pParticleNodeHead->mNextNode == pParticleNodeHead )
                {
                    // No, so generate a single particle.
                    pEmitterNode->createParticle();
                }
            }
            else
            {
                // Accumulate the last generation time as we need to handle very small time-integrations correctly.
                //
                // NOTE:    We need to do this if there's an emission target but the time-integration is so small
                //          that rounding results in no emission.  Downside to good FPS!
                pEmitterNode->setTimeSinceLastGeneration( pEmitterNode->getTimeSinceLastGeneration() + scaledTime );

                // Fetch the particle player age.
                const F32 particlePlayerAge = mAge;

                // Fetch the quantity base and variation fields.
                const ParticleAssetField& quantityBaseField = pParticleAssetEmitter->getQuantityBaseField();
                const ParticleAssetField& quantityVaritationField = pParticleAssetEmitter->getQuantityBaseField();

                // Fetch the emissions.
                const F32 baseEmission = quantityBaseField.getFieldValue( particlePlayerAge );
                const F32 varEmission = quantityVaritationField.getFieldValue( particlePlayerAge ) * 0.5f;

                // Fetch the emission scale.
                const F32 effectEmission = pParticleAsset->getQuantityScaleField().getFieldValue( particlePlayerAge ) * getEmissionRateScale();

                // Calculate the local emission.
                const F32 localEmission = mClampF(  (baseEmission + CoreMath::mGetRandomF(-varEmission, varEmission)) * effectEmission,
                                                    quantityBaseField.getMinValue(),
                                                    quantityBaseField.getMaxValue() );

                // Calculate the final time-independent emission count.
                const U32 emissionCount = U32(mFloor( localEmission * pEmitterNode->getTimeSinceLastGeneration() ));

                // Do we have an emission?
                if ( emissionCount > 0 )
                {
                    // Yes, so remove this emission from accumulated time.
                    pEmitterNode->setTimeSinceLastGeneration( getMax(0.0f, pEmitterNode->getTimeSinceLastGeneration() - (emissionCount / localEmission) ) );

                    // Suppress Precision Errors.
                    if ( mIsZero( pEmitterNode->getTimeSinceLastGeneration() ) )
                        pEmitterNode->setTimeSinceLastGeneration( 0.0f );

                    // Generate the required emission.
                    for ( U32 n = 0; n < emissionCount; n++ )
                        pEmitterNode->createParticle();
                }
            }
        }
    }

    // Fetch the particle life-mode.
    const ParticleAsset::LifeMode lifeMode = pParticleAsset->getLifeMode();

    // Are we waiting for particles and there are non left?
    if ( mWaitingForParticles )
    {
        // Yes, so are there any particles left?
        if ( activeParticleCount == 0 )
        {
            // No, so stop the player immediately.
            stop( false, mWaitingForDelete );
        }

        return;
    }

    // Finish if the particle player is in "infinite" mode.
    if ( lifeMode == ParticleAsset::INFINITE )
        return;

    // Fetch the particle lifetime.
    const F32 lifetime = pParticleAsset->getLifetime();

    // Cycle life-mode?
    if ( lifeMode == ParticleAsset::CYCLE )
    {
        // Has the age expired?
        if ( mAge >= lifetime )
        {
            // Yes, so restart the particle.
            play( false );
        }
        return;
    }

    // Stop life-mode?
    if ( lifeMode == ParticleAsset::STOP )
    {
        // Has the age expired?
        if ( mAge >= lifetime )
        {
            // Yes, so stop the particle.
            stop( true, false );
        }
        return;
    }

    // kill life-mode?
    if ( lifeMode == ParticleAsset::KILL )
    {
        // Has the age expired?
        if ( mAge >= lifetime )
        {
            // Yes, so kill the particle.
            stop( true, true );
        }
        return;
    }
}

//------------------------------------------------------------------------------

void ParticlePlayer::interpolateObject( const F32 timeDelta )
{    
    // Call parent.
    Parent::interpolateObject( timeDelta );

    // Finish if no need to interpolate.
    if ( !mParticleInterpolation || !mPlaying || mCameraIdle || mPaused )
        return;

    // Iterate the emitters.
    for( typeEmitterVector::iterator emitterItr = mEmitters.begin(); emitterItr != mEmitters.end(); ++emitterItr )
    {
        // Fetch the emitter node.
        EmitterNode* pEmitterNode = *emitterItr;

        // Fetch First Particle Node.
        ParticleSystem::ParticleNode* pParticleNode = pEmitterNode->getFirstParticle();

        // Fetch the particle node head.
        ParticleSystem::ParticleNode* pParticleNodeHead = pEmitterNode->getParticleNodeHead();

        // Fetch the asset emitter.
        ParticleAssetEmitter* pParticleAssetEmitter = pEmitterNode->getAssetEmitter();

        // Fetch the local AABB..
        const Vector2& localAABB0 = pParticleAssetEmitter->getLocalPivotAABB0();
        const Vector2& localAABB1 = pParticleAssetEmitter->getLocalPivotAABB1();
        const Vector2& localAABB2 = pParticleAssetEmitter->getLocalPivotAABB2();
        const Vector2& localAABB3 = pParticleAssetEmitter->getLocalPivotAABB3();

        // Process All particle nodes.
        while ( pParticleNode != pParticleNodeHead )
        {
            // Interpolate the position.
            pParticleNode->mRenderTickPosition = (timeDelta * pParticleNode->mPreTickPosition) + ((1.0f-timeDelta) * pParticleNode->mPostTickPosition);

            // Set the transform.
            pParticleNode->mTransform.p = pParticleNode->mRenderTickPosition;

            // Fetch the render size.
            const Vector2& renderSize = pParticleNode->mRenderSize;

            // Calculate the scaled AABB.
            Vector2 scaledAABB[4];
            scaledAABB[0] = localAABB0 * renderSize;
            scaledAABB[1] = localAABB1 * renderSize;
            scaledAABB[2] = localAABB2 * renderSize;
            scaledAABB[3] = localAABB3 * renderSize;

            // Calculate the world OOBB..
            CoreMath::mCalculateOOBB( scaledAABB, pParticleNode->mTransform, pParticleNode->mRenderOOBB );

            // Move to the next particle.
            pParticleNode = pParticleNode->mNextNode;
        }
    }
}

//-----------------------------------------------------------------------------

void ParticlePlayer::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Finish if we can't render.
    if ( !mPlaying || mCameraIdle )
        return;

    // Flush.
    pBatchRenderer->flush( getScene()->getDebugStats().batchIsolatedFlush );

    // Fetch emitter count.
    const U32 emitterCount = mEmitters.size();

    // Render all the emitters.
    for ( U32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex )
    {
        // Fetch the emitter node.
        EmitterNode* pEmitterNode = mEmitters[emitterIndex];

        // Fetch the particle emitter.
        ParticleAssetEmitter* pParticleAssetEmitter = pEmitterNode->getAssetEmitter();

        // Skip if the emitter is not visible.
        if ( !pEmitterNode->getVisible() )
            continue;

        // Skip if there are no active particles.
        if ( !pEmitterNode->getActiveParticles() )
            continue;       

        // Fetch both image and animation assets.
        const AssetPtr<ImageAsset>& imageAsset = pParticleAssetEmitter->getImageAsset();
        const AssetPtr<AnimationAsset>& animationAsset = pParticleAssetEmitter->getAnimationAsset();

        // Fetch static mode.
        const bool isStaticFrameProvider = pParticleAssetEmitter->isStaticFrameProvider();

        // Are we in static mode?
        if ( isStaticFrameProvider )
        {
            // Yes, so skip if no image available.
            if ( imageAsset.isNull() )
                continue;
        }
        else
        {
            // No, so skip if no animation available.
            if ( animationAsset.isNull() )
                continue;
        }

        // Flush.
        pBatchRenderer->flush( getScene()->getDebugStats().batchIsolatedFlush );

        // Intense particles?
        if ( pParticleAssetEmitter->getIntenseParticles() )
        {
            // Yes, so set additive blending.
            pBatchRenderer->setBlendMode( GL_SRC_ALPHA, GL_ONE );
        }
        else
        {
            // No, so set standard blend options.
            if ( mBlendMode )
            {
                pBatchRenderer->setBlendMode( mSrcBlendFactor, mDstBlendFactor );
            }
            else
            {
                pBatchRenderer->setBlendOff();
            }
        }

        // Set alpha-testing.
        pBatchRenderer->setAlphaTestMode( pParticleAssetEmitter->getAlphaTest() );

        // Save the transformation.
        glPushMatrix();

        // Is the Position attached to the emitter?
        if ( pParticleAssetEmitter->getAttachPositionToEmitter() )
        {
            // Yes, so get player position.
            const Vector2 renderPosition = getRenderPosition();

            // Move into emitter-space.
            glTranslatef( renderPosition.x, renderPosition.y, 0.0f );

            // Is the rotation attached to the emitter?
            if ( pParticleAssetEmitter->getAttachRotationToEmitter() )
            {
                // Yes, so rotate into emitter-space.
                // NOTE:- We need clockwise rotation here.
                glRotatef( mRadToDeg(getRenderAngle()), 0.0f, 0.0f, 1.0f );
            }
        }

        // Fetch the oldest-in-front flag.
        const bool oldestInFront = pParticleAssetEmitter->getOldestInFront();

        // Fetch the starting particle (using appropriate particle order).
        ParticleSystem::ParticleNode* pParticleNode = oldestInFront ? pEmitterNode->getFirstParticle() : pEmitterNode->getLastParticle();

        // Fetch the particle node head.
        ParticleSystem::ParticleNode* pParticleNodeHead = pEmitterNode->getParticleNodeHead();

        // Process all particle nodes.
        while ( pParticleNode != pParticleNodeHead )
        {
            // Fetch the frame provider.
            const ImageFrameProviderCore& frameProvider = pParticleNode->mFrameProvider;

            // Fetch the frame area.
            const ImageAsset::FrameArea::TexelArea& texelFrameArea = frameProvider.getProviderImageFrameArea().mTexelArea;

            // Frame texture.
            TextureHandle& frameTexture = frameProvider.getProviderTexture();

            // Fetch the particle render OOBB.
            Vector2* renderOOBB = pParticleNode->mRenderOOBB;

            // Fetch lower/upper texture coordinates.
            const Vector2& texLower = texelFrameArea.mTexelLower;
            const Vector2& texUpper = texelFrameArea.mTexelUpper;

            // Submit batched quad.
            pBatchRenderer->SubmitQuad(
                renderOOBB[0],
                renderOOBB[1],
                renderOOBB[2],
                renderOOBB[3],
                Vector2( texLower.x, texUpper.y ),
                Vector2( texUpper.x, texUpper.y ),
                Vector2( texUpper.x, texLower.y ),
                Vector2( texLower.x, texLower.y ),
                frameTexture,
                pParticleNode->mColor );

            // Move to next Particle ( using appropriate sort-order ).
            pParticleNode = oldestInFront ? pParticleNode->mNextNode : pParticleNode->mPreviousNode;
        };

        // Flush.
        pBatchRenderer->flush( getScene()->getDebugStats().batchIsolatedFlush );

        // Restore the transformation.
        glPopMatrix();
    }
}

//-----------------------------------------------------------------------------

void ParticlePlayer::sceneRenderOverlay( const SceneRenderState* sceneRenderState )
{
    // Call parent.
    Parent::sceneRenderOverlay( sceneRenderState );

    // Get Scene.
    Scene* pScene = getScene();

    // Finish if no scene.
    if ( !pScene )
        return;

    // Finish if we shouldn't be drawing the debug overlay.
    if ( !pScene->getIsEditorScene() || mLessThanOrEqual( mCameraIdleDistance, 0.0f ) || !isEnabled() || !getVisible() )
        return;

    // Draw camera pause distance.
    pScene->mDebugDraw.DrawCircle( getRenderPosition(), mCameraIdleDistance, ColorF(1.0f, 1.0f, 0.0f ) );
}

//-----------------------------------------------------------------------------

void ParticlePlayer::setParticle( const char* pAssetId )
{
    // Sanity!
    AssertFatal( pAssetId != NULL, "ParticlePlayer::setParticle() - Cannot use a NULL asset Id." );

    // Set asset Id.
    mParticleAsset = pAssetId;

    // Initialize the particle.
    initializeParticleAsset();
}

//-----------------------------------------------------------------------------

void ParticlePlayer::setEmitterPaused( const bool paused, const U32 emitterIndex )
{
    // Is the emitter index valid?
    if ( emitterIndex >= getEmitterCount() )
    {
        // No, so warn.
        Con::warnf( "ParticlePlayer::setEmitterPaused() - Emitter index is out of bounds." );
        return;
    }

    mEmitters[emitterIndex]->setPaused( paused );
}

//-----------------------------------------------------------------------------

bool ParticlePlayer::getEmitterPaused( const U32 emitterIndex )
{
    // Is the emitter index valid?
    if ( emitterIndex >= getEmitterCount() )
    {
        // No, so warn.
        Con::warnf( "ParticlePlayer::getEmitterPaused() - Emitter index is out of bounds." );
        return false;
    }

    return mEmitters[emitterIndex]->getPaused();
}

//-----------------------------------------------------------------------------

void ParticlePlayer::setEmitterVisible( const bool visible, const U32 emitterIndex )
{
    // Is the emitter index valid?
    if ( emitterIndex >= getEmitterCount() )
    {
        // No, so warn.
        Con::warnf( "ParticlePlayer::setEmitterVisible() - Emitter index is out of bounds." );
        return;
    }

    mEmitters[emitterIndex]->setVisible( visible );
}

//-----------------------------------------------------------------------------

bool ParticlePlayer::getEmitterVisible( const U32 emitterIndex )
{
    // Is the emitter index valid?
    if ( emitterIndex >= getEmitterCount() )
    {
        // No, so warn.
        Con::warnf( "ParticlePlayer::getEmitterVisible() - Emitter index is out of bounds." );
        return false;
    }

    return mEmitters[emitterIndex]->getVisible();
}

//-----------------------------------------------------------------------------

bool ParticlePlayer::play( const bool resetParticles )
{
    // Cannot do anything if we've not got any emitters!
    if ( mParticleAsset.isNull() || mParticleAsset->getEmitterCount() == 0 )
    {
        // Warn.
        Con::warnf("ParticlePlayer::play() - Cannot play; no emitters!");
        return false;
    }

    // Are we in a scene?
    if ( getScene() == NULL )
    {
        // No, so warn.
        Con::warnf("ParticlePlayer::play() - Cannot play when not in a scene!");
        return false;
    }

    // Reset the age.
    mAge = 0.0f;

    // Iterate the emitters.
    for( typeEmitterVector::iterator emitterItr = mEmitters.begin(); emitterItr != mEmitters.end(); ++emitterItr )
    {
        // Fetch the emitter node.
        EmitterNode* pEmitterNode = *emitterItr;
        pEmitterNode->setPaused(false);
        
        // Reset the time since last generation.
        pEmitterNode->setTimeSinceLastGeneration( 0.0f );
    }

    // Reset Waiting for Particles.
    mWaitingForParticles = false;

    // Reset Waiting for delete.
    mWaitingForDelete = false;

    // Flag as playing.
    mPlaying = true;

    // Turn-off paused.
    mPaused = false;

    // Set unsafe delete status.
    setSafeDelete(false);

    return true;
}

//-----------------------------------------------------------------------------

void ParticlePlayer::stop( const bool waitForParticles, const bool killEffect )
{
    // Finish if we're not playing and there's no kill command.
    if ( !mPlaying && !killEffect )
        return;

    // Fetch emitter count.
    const U32 emitterCount = mEmitters.size();

    // Are we waiting for particles to end?
    if ( waitForParticles )
    {
        // Yes, so pause all the emitters.
        for ( U32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex )
        {
            // Fetch the emitter.
            mEmitters[emitterIndex]->setPaused( true );
        }

        // Set waiting for particles.
        mWaitingForParticles = true;

        // Flag as waiting for deletion if killing effect.
        if ( killEffect )
            mWaitingForDelete = true;

        return;
    }

    // No, so free all particles.
    for ( U32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex )
    {
        mEmitters[emitterIndex]->freeAllParticles();
    }

    // Reset the age.
    mAge = 0.0f;

    // Flag as stopped and not waiting.
    mPlaying = mWaitingForParticles = mWaitingForDelete = false;

    // Turn off paused.
    mPaused = false;

    // Set safe deletion.
    setSafeDelete(true);

    // Perform the callback.
    if( isMethod( "onStopParticlePlayer" ) )
        Con::executef( this, 1, "onStopParticlePlayer" );

    // Flag for immediate deletion if killing.
    if ( killEffect )
        safeDelete();
}

//------------------------------------------------------------------------------

void ParticlePlayer::configureParticle( EmitterNode* pEmitterNode, ParticleSystem::ParticleNode* pParticleNode )
{
    // Fetch the particle player age.
    const F32 particlePlayerAge = mAge;

    // Fetch the particle player position.
    const Vector2& particlePlayerPosition = getPosition();

    // Default to not suppressing movement.
    pParticleNode->mSuppressMovement = false;

    // Fetch particle asset.
    ParticleAsset* pParticleAsset = mParticleAsset;

    // Fetch the asset emitter.
    ParticleAssetEmitter* pParticleAssetEmitter = pEmitterNode->getAssetEmitter();


    // **********************************************************************************************************************
    // Calculate Particle Position.
    // **********************************************************************************************************************

    // Fetch attachment options.
    const bool attachPositionToEmitter = pParticleAssetEmitter->getAttachPositionToEmitter();

    // Fetch the emitter offset, angle and size
    const Vector2& emitterOffset = pParticleAssetEmitter->getEmitterOffset() * getSizeScale();
    const Vector2& emitterSize = pParticleAssetEmitter->getEmitterSize() * getSizeScale();
    const F32 emitterAngle = mDegToRad(pParticleAssetEmitter->getEmitterAngle());

    // Are we using Single Particle?
    if ( pParticleAssetEmitter->getSingleParticle() )
    {
        // Determine whether to use world-space or emitter-space.
        if ( attachPositionToEmitter )
        {
            pParticleNode->mPosition = emitterOffset;
        }
        else
        {
            pParticleNode->mPosition = particlePlayerPosition + emitterOffset;
        }
    }
    else
    {
        // No, so select Emitter-Type.
        switch( pParticleAssetEmitter->getEmitterType() )
        {
            // Emit at a point defined by the emitters position.
            case ParticleAssetEmitter::POINT_EMITTER:
            {
                // Are we attaching the position to the emitter?
                if ( attachPositionToEmitter )
                {
                    // Yes, so transform the particle into emitter-space only.
                    pParticleNode->mPosition = emitterOffset;
                }
                else
                {
                    // No, so transform the particle into world-space here.
                    pParticleNode->mPosition = emitterOffset + particlePlayerPosition;
                }

            } break;

            // Emit along a line defined by the emitters width.
            case ParticleAssetEmitter::LINE_EMITTER:
            {
                // Calculate half-width.
                const F32 halfWidth = emitterSize.x * 0.5f;

                // Calculate emitter position.
                Vector2 emissionPosition( CoreMath::mGetRandomF( -halfWidth, halfWidth ), 0.0f );

                // Transform particle position in emitter-space.
                pParticleNode->mPosition = b2Mul( b2Rot(emitterAngle), emissionPosition ) + emitterOffset;

                // Are we attaching the position to the emitter?
                if ( !attachPositionToEmitter )
                {
                    // No, so transform the particle into world-space here.
                    b2Transform xform( particlePlayerPosition, b2Rot( getAngle()) );
                    pParticleNode->mPosition = b2Mul( xform, pParticleNode->mPosition );
                }

            } break;

            // Emit inside a box defined by the emitters size.
            case ParticleAssetEmitter::BOX_EMITTER:
            {
                // Calculate half-width/height.
                const F32 halfWidth = emitterSize.x * 0.5f;
                const F32 halfHeight = emitterSize.y * 0.5f;

                // Calculate emitter position.
                Vector2 emissionPosition( CoreMath::mGetRandomF( -halfWidth, halfWidth ), CoreMath::mGetRandomF( -halfHeight, halfHeight ) );

                // Transform particle position in emitter-space.
                pParticleNode->mPosition = b2Mul( b2Rot(emitterAngle), emissionPosition ) + emitterOffset;

                // Are we attaching the position to the emitter?
                if ( !attachPositionToEmitter )
                {
                    // No, so transform the particle into world-space here.
                    b2Transform xform( particlePlayerPosition, b2Rot( getAngle()) );
                    pParticleNode->mPosition = b2Mul( xform, pParticleNode->mPosition );
                }

            } break;

            // Emit from an ellipse with a radii defined by the emitters size.
            case ParticleAssetEmitter::DISK_EMITTER:
            {
                // Calculate the random angle.
                const F32 angle = CoreMath::mGetRandomF( 0.0f, b2_pi2 );
#if 1
                // Calculate the uniform distribution scale.
                const F32 distributionScale = mSqrt( CoreMath::mGetRandomF(0.0f, 1.0f) );

                // Calculate the radii.
                const F32 radiusX = emitterSize.x * 0.5f * distributionScale;
                const F32 radiusY = emitterSize.y * 0.5f * distributionScale;
#else
                // Calculate the radii.
                const F32 radiusX = emitterSize.x * 0.5f;
                const F32 radiusY = emitterSize.y * 0.5f;
#endif
                // Calculate emitter position using a uniform distribution.
                Vector2 emissionPosition( radiusX * mCos(angle), radiusY * mSin(angle) );

                // Transform particle position in emitter-space.
                pParticleNode->mPosition = b2Mul( b2Rot(emitterAngle), emissionPosition ) + emitterOffset;

                // Are we attaching the position to the emitter?
                if ( !attachPositionToEmitter )
                {
                    // No, so transform the particle into world-space here.
                    b2Transform xform( particlePlayerPosition, b2Rot( getAngle()) );
                    pParticleNode->mPosition = b2Mul( xform, pParticleNode->mPosition );
                }

            } break;

            // Emit from an ellipse with a radii defined by the emitters size.
            case ParticleAssetEmitter::ELLIPSE_EMITTER:
            {
                // Calculate the random angle.
                const F32 angle = CoreMath::mGetRandomF( 0.0f, b2_pi2 );

                // Calculate emitter position using a uniform distribution.
                Vector2 emissionPosition( emitterSize.x * 0.5f * mCos(angle), emitterSize.y * 0.5f * mSin(angle) );

                // Transform particle position in emitter-space.
                pParticleNode->mPosition = b2Mul( b2Rot(emitterAngle), emissionPosition ) + emitterOffset;

                // Are we attaching the position to the emitter?
                if ( !attachPositionToEmitter )
                {
                    // No, so transform the particle into world-space here.
                    b2Transform xform( particlePlayerPosition, b2Rot( getAngle()) );
                    pParticleNode->mPosition = b2Mul( xform, pParticleNode->mPosition );
                }

            } break;

            // Emit inside a torus with an outer-diameter defined by the emitters major size axis and an inner-diameter defined by the emitters minor size axis..
            case ParticleAssetEmitter::TORUS_EMITTER:
            {
                // Calculate the random angle.
                const F32 angle = CoreMath::mGetRandomF( 0.0f, b2_pi2 );

                // Calculate the inner and outer radii.
                const F32 outerRadii = emitterSize.getMajorAxis() * 0.5f;
                const F32 innerRadii = emitterSize.getMinorAxis() * 0.5f;
#if 1
                // Calculate the radius as a uniform distribution.
                const F32 radius = innerRadii + ( mSqrt( CoreMath::mGetRandomF(0.0f, 1.0f) ) * (outerRadii-innerRadii) );
#else
                // Calculate the radius as a non-uniform distribution.
                const F32 radius = CoreMath::mGetRandomF( innerRadii, outerRadii );
#endif
                // Calculate emitter position using a uniform distribution.
                Vector2 emissionPosition( radius * mCos(angle), radius * mSin(angle) );

                // Are we attaching the position to the emitter?
                if ( attachPositionToEmitter )
                {
                    // Yes, so transform the particle into emitter-space only.
                    pParticleNode->mPosition = emissionPosition + emitterOffset;
                }
                else
                {
                    // No, so transform the particle into world-space here.
                    pParticleNode->mPosition = emissionPosition + emitterOffset + particlePlayerPosition;
                }

            } break;
                
            default:
                break;
        }
    }

    // **********************************************************************************************************************
    // Calculate Particle Lifetime.
    // **********************************************************************************************************************

    pParticleNode->mParticleAge = 0.0f;
    pParticleNode->mParticleLifetime = ParticleAssetField::calculateFieldBVE(   pParticleAssetEmitter->getParticleLifeBaseField(),
                                                                                pParticleAssetEmitter->getParticleLifeVariationField(),
                                                                                pParticleAsset->getParticleLifeScaleField(),
                                                                                particlePlayerAge );


    // **********************************************************************************************************************
    // Calculate Particle Size-X.
    // **********************************************************************************************************************

    pParticleNode->mSize.x = ParticleAssetField::calculateFieldBVE( pParticleAssetEmitter->getSizeXBaseField(),
                                                                    pParticleAssetEmitter->getSizeXVariationField(),
                                                                    pParticleAsset->getSizeXScaleField(),
                                                                    particlePlayerAge ) * getSizeScale();

    // Is the particle using a fixed aspect?
    if ( pParticleAssetEmitter->getFixedAspect() )
    {
        // Yes, so simply copy Size-X.
        pParticleNode->mSize.y = pParticleNode->mSize.x;
    }
    else
    {
        // No, so calculate the particle Size-Y.
        pParticleNode->mSize.y = ParticleAssetField::calculateFieldBVE( pParticleAssetEmitter->getSizeYBaseField(),
                                                                        pParticleAssetEmitter->getSizeYVariationField(),
                                                                        pParticleAsset->getSizeYScaleField(),
                                                                        particlePlayerAge ) * getSizeScale();
    }

    // Reset the render size.
    pParticleNode->mRenderSize.Set(-1.0f, -1.0f);


    // **********************************************************************************************************************
    // Calculate Speed, Random Motion and Emission Angle.
    // **********************************************************************************************************************

    // We reset the emission angle/arc in-case we're using single-particle mode as this is the default.
    F32 emissionForce = 0;
    F32 emissionAngle = 0;
    F32 emissionArc = 0;

    // Ignore if we're using a single-particle.
    if ( !pParticleAssetEmitter->getSingleParticle() )
    {
        pParticleNode->mSpeed = ParticleAssetField::calculateFieldBVE(  pParticleAssetEmitter->getSpeedBaseField(),
                                                                        pParticleAssetEmitter->getSpeedVariationField(),
                                                                        pParticleAsset->getSpeedScaleField(),
                                                                        particlePlayerAge ) * getForceScale();

        pParticleNode->mRandomMotion = ParticleAssetField::calculateFieldBVE(   pParticleAssetEmitter->getRandomMotionBaseField(),
                                                                                pParticleAssetEmitter->getRandomMotionVariationField(),
                                                                                pParticleAsset->getRandomMotionScaleField(),
                                                                                particlePlayerAge ) * getForceScale();


        //  Calculate the emission force.
        emissionForce = ParticleAssetField::calculateFieldBV(   pParticleAssetEmitter->getEmissionForceForceBaseField(),
                                                                pParticleAssetEmitter->getEmissionForceVariationField(),
                                                                particlePlayerAge) * getForceScale();

        if (pParticleAssetEmitter->getIsTargeting())
        {
           Vector2 tPos = pParticleAssetEmitter->getTargetPosition();
           Vector2 pPos = pParticleNode->mPosition;
           Vector2 subVec = tPos - pPos;
           F32 vecN = mAtan(subVec.x, subVec.y);
           F32 vecDeg = mRadToDeg(vecN);
           emissionAngle = vecDeg;
        }
        else
        {

           // Calculate Emission Angle.
           emissionAngle = ParticleAssetField::calculateFieldBV(  pParticleAssetEmitter->getEmissionAngleBaseField(),
                                                                  pParticleAssetEmitter->getEmissionAngleVariationField(),
                                                                  particlePlayerAge);
        }
        // Calculate Emission Arc.
        // NOTE:-   We're actually interested in half the emission arc!
        emissionArc = ParticleAssetField::calculateFieldBV( pParticleAssetEmitter->getEmissionArcBaseField(),
                                                            pParticleAssetEmitter->getEmissionArcVariationField(),
                                                            particlePlayerAge ) * 0.5f;

        // Is the emission rotation linked?
        if ( pParticleAssetEmitter->getLinkEmissionRotation() )
        {
            // Yes, so add the particle player angle.
            emissionAngle += getAngle();
        }

        // Calculate the final emission angle choosing random Arc.
        emissionAngle = mFmod( CoreMath::mGetRandomF( emissionAngle-emissionArc, emissionAngle+emissionArc ), 360.0f );

        // Calculate the particle velocity.
        const F32 emissionAngleRadians = mDegToRad( emissionAngle );
        pParticleNode->mVelocity.Set( emissionForce * mCos( emissionAngleRadians ), emissionForce * mSin( emissionAngleRadians ) );
    }


    // **********************************************************************************************************************
    // Calculate Spin.
    // **********************************************************************************************************************

    pParticleNode->mSpin = ParticleAssetField::calculateFieldBVE(   pParticleAssetEmitter->getSpinBaseField(),
                                                                    pParticleAssetEmitter->getSpinVariationField(),
                                                                    pParticleAsset->getSpinScaleField(),
                                                                    particlePlayerAge );


    // **********************************************************************************************************************
    // Calculate Fixed-Force.
    // **********************************************************************************************************************

    pParticleNode->mFixedForce = ParticleAssetField::calculateFieldBVE( pParticleAssetEmitter->getFixedForceBaseField(),
                                                                        pParticleAssetEmitter->getFixedForceVariationField(),
                                                                        pParticleAsset->getFixedForceScaleField(),
                                                                        particlePlayerAge ) * getForceScale();


    // **********************************************************************************************************************
    // Calculate Orientation Angle.
    // **********************************************************************************************************************

    // Configure particle orientation.
    switch( pParticleAssetEmitter->getOrientationType() )
    {
        // Aligned to initial emission.
        case ParticleAssetEmitter::ALIGNED_ORIENTATION:
        {
            // Use the emission angle with fixed offset.
            pParticleNode->mOrientationAngle = mFmod( emissionAngle - pParticleAssetEmitter->getAlignedAngleOffset(), 360.0f );

        } break;

        // Fixed orientation.
        case ParticleAssetEmitter::FIXED_ORIENTATION:
        {
            // Use a fixed angle.
            pParticleNode->mOrientationAngle = mFmod( pParticleAssetEmitter->getFixedAngleOffset(), 360.0f );

        } break;

        // Random with constraints.
        case ParticleAssetEmitter::RANDOM_ORIENTATION:
        {
            // Used a random angle/arc.
            const F32 randomArc = pParticleAssetEmitter->getRandomArc() * 0.5f;
            pParticleNode->mOrientationAngle = mFmod( CoreMath::mGetRandomF( pParticleAssetEmitter->getRandomAngleOffset() - randomArc, pParticleAssetEmitter->getRandomAngleOffset() + randomArc ), 360.0f );

        } break;
        
        default:
            break;
    }

    // **********************************************************************************************************************
    // Calculate RGBA Components.
    // **********************************************************************************************************************

    // Fetch the channels.
    const ParticleAssetField& redChannel = pParticleAssetEmitter->getRedChannelLifeField();
    const ParticleAssetField& greenChannel = pParticleAssetEmitter->getGreenChannelLifeField();
    const ParticleAssetField& blueChannel = pParticleAssetEmitter->getBlueChannelLifeField();
    const ParticleAssetField& alphaChannel = pParticleAssetEmitter->getAlphaChannelLifeField();
    const ParticleAssetField& alphaChannelScale = pParticleAsset->getAlphaChannelScaleField();

    // Calculate the color.
    pParticleNode->mColor.set(  mClampF( redChannel.getFieldValue( 0.0f ), redChannel.getMinValue(), redChannel.getMaxValue() ),
                                mClampF( greenChannel.getFieldValue( 0.0f ),greenChannel.getMinValue(), greenChannel.getMaxValue() ),
                                mClampF( blueChannel.getFieldValue( 0.0f ), blueChannel.getMinValue(),blueChannel.getMaxValue() ),
                                mClampF( alphaChannel.getFieldValue( 0.0f ) * alphaChannelScale.getFieldValue( 0.0f ), alphaChannel.getMinValue(), alphaChannel.getMaxValue() ) );


    // **********************************************************************************************************************
    // Image, Frame and Animation Controller.
    // **********************************************************************************************************************

    // Fetch the image frame provider.
    ImageFrameProviderCore& frameProvider = pParticleNode->mFrameProvider;

    // Allocate assets to the particle.
    frameProvider.allocateAssets( &(pParticleAssetEmitter->getImageAsset()), &(pParticleAssetEmitter->getAnimationAsset()) );

    // Is the emitter in static mode?
    if ( pParticleAssetEmitter->isStaticFrameProvider() )
    {
        // Yes, so is random image frame active?
        if ( pParticleAssetEmitter->getRandomImageFrame() )
        {
            // Yes, so fetch the frame count for the image asset.
            const U32 frameCount = pParticleAssetEmitter->getImageAsset()->getFrameCount();

            // Choose a random frame.
            frameProvider.setImageFrame( (U32)CoreMath::mGetRandomI( 0, frameCount-1 ) );
        }
        else
        {
            // No, so set the emitter image frame.
            if (pParticleAssetEmitter->isUsingNamedImageFrame())
                frameProvider.setNamedImageFrame( pParticleAssetEmitter->getNamedImageFrame() );
            else
                frameProvider.setImageFrame( pParticleAssetEmitter->getImageFrame() );
        }
    }
    else
    {
        // No, so fetch the animation asset.
        const AssetPtr<AnimationAsset>& animationAsset = pParticleAssetEmitter->getAnimationAsset();

        // Play it.
        frameProvider.playAnimation( animationAsset );
    }


    // **********************************************************************************************************************
    // Reset Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mPreTickPosition = pParticleNode->mPostTickPosition = pParticleNode->mRenderTickPosition = pParticleNode->mPosition;


    // **********************************************************************************************************************
    // Do a Single Particle Integration to get things going.
    // **********************************************************************************************************************
    integrateParticle( pEmitterNode, pParticleNode, 0.0f, 0.0f );
}

//------------------------------------------------------------------------------

void ParticlePlayer::integrateParticle( EmitterNode* pEmitterNode, ParticleSystem::ParticleNode* pParticleNode, F32 particleAge, F32 elapsedTime )
{
    // Fetch particle asset.
    ParticleAsset* pParticleAsset = mParticleAsset;

    // Fetch the asset emitter.
    ParticleAssetEmitter* pParticleAssetEmitter = pEmitterNode->getAssetEmitter();


    // **********************************************************************************************************************
    // Copy Old Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mRenderTickPosition = pParticleNode->mPreTickPosition = pParticleNode->mPostTickPosition;


    // **********************************************************************************************************************
    // Scale Size.
    // **********************************************************************************************************************

    // Scale Size-X.
    pParticleNode->mRenderSize.x = mClampF( pParticleNode->mSize.x * pParticleAssetEmitter->getSizeXLifeField().getFieldValue( particleAge ),
                                            pParticleAssetEmitter->getSizeXBaseField().getMinValue(),
                                            pParticleAssetEmitter->getSizeXBaseField().getMaxValue());

    // Is the particle using a fixed aspect?
    if ( pParticleAssetEmitter->getFixedAspect() )
    {
        // Yes, so simply copy Size-X.
        pParticleNode->mRenderSize.y = pParticleNode->mRenderSize.x;
    }
    else
    {
        // No, so Scale Size-Y.
        pParticleNode->mRenderSize.y = mClampF( pParticleNode->mSize.y * pParticleAssetEmitter->getSizeYLifeField().getFieldValue( particleAge ),
                                                pParticleAssetEmitter->getSizeYBaseField().getMinValue(),
                                                pParticleAssetEmitter->getSizeYBaseField().getMaxValue() );
    }


    // **********************************************************************************************************************
    // Scale Speed.
    // **********************************************************************************************************************
    pParticleNode->mRenderSpeed = mClampF(  pParticleNode->mSpeed * pParticleAssetEmitter->getSpeedLifeField().getFieldValue( particleAge ),
                                            pParticleAssetEmitter->getSpeedBaseField().getMinValue(),
                                            pParticleAssetEmitter->getSpeedBaseField().getMaxValue() );


    // **********************************************************************************************************************
    // Scale Fixed-Force.
    // **********************************************************************************************************************
    pParticleNode->mRenderFixedForce = mClampF( pParticleNode->mFixedForce * pParticleAssetEmitter->getFixedForceLifeField().getFieldValue( particleAge ),
                                                pParticleAssetEmitter->getFixedForceBaseField().getMinValue(),
                                                pParticleAssetEmitter->getFixedForceBaseField().getMaxValue() );


    // **********************************************************************************************************************
    // Scale Random-Motion.
    // **********************************************************************************************************************
    pParticleNode->mRenderRandomMotion = mClampF(   pParticleNode->mRandomMotion * pParticleAssetEmitter->getRandomMotionLifeField().getFieldValue( particleAge ),
                                                    pParticleAssetEmitter->getRandomMotionBaseField().getMinValue(),
                                                    pParticleAssetEmitter->getRandomMotionBaseField().getMaxValue() );


    // **********************************************************************************************************************
    // Calculate RGBA Components.
    // **********************************************************************************************************************

    // Fetch the channels.
    const ParticleAssetField& redChannel = pParticleAssetEmitter->getRedChannelLifeField();
    const ParticleAssetField& greenChannel = pParticleAssetEmitter->getGreenChannelLifeField();
    const ParticleAssetField& blueChannel = pParticleAssetEmitter->getBlueChannelLifeField();
    const ParticleAssetField& alphaChannel = pParticleAssetEmitter->getAlphaChannelLifeField();
    const ParticleAssetField& alphaChannelScale = pParticleAsset->getAlphaChannelScaleField();

    // Calculate the color.
    pParticleNode->mColor.set(  mClampF( redChannel.getFieldValue( particleAge ), redChannel.getMinValue(), redChannel.getMaxValue() ),
                                mClampF( greenChannel.getFieldValue( particleAge ),greenChannel.getMinValue(), greenChannel.getMaxValue() ),
                                mClampF( blueChannel.getFieldValue( particleAge ), blueChannel.getMinValue(),blueChannel.getMaxValue() ),
                                mClampF( alphaChannel.getFieldValue( particleAge ) * alphaChannelScale.getFieldValue( 0.0f ), alphaChannel.getMinValue(), alphaChannel.getMaxValue() ) );


    // **********************************************************************************************************************
    // Integrate Particle.
    // **********************************************************************************************************************


    // Is the emitter in static mode?
    if ( !pParticleAssetEmitter->isStaticFrameProvider() )
    {
        // No, so update animation.
        pParticleNode->mFrameProvider.updateAnimation( elapsedTime );
    }


    // **********************************************************************************************************************
    // Calculate New Velocity...
    // **********************************************************************************************************************

    // Calculate the velocity if not a single particle.
    if ( !pParticleAssetEmitter->getSingleParticle() )
    {
        // Calculate random motion (if we've got any).
        if ( mNotZero( pParticleNode->mRenderRandomMotion ) )
        {
            // Fetch random motion.
            const F32 randomMotion = pParticleNode->mRenderRandomMotion * 0.5f;

            // Add time-integrated random motion into velocity.
            pParticleNode->mVelocity += Vector2( CoreMath::mGetRandomF(-randomMotion, randomMotion) * elapsedTime, CoreMath::mGetRandomF(-randomMotion, randomMotion) * elapsedTime );
        }

        // Do we have any fixed force?
        if ( mNotZero( pParticleNode->mRenderFixedForce ) )
        {
            // Yes, so time-integrate a fixed force to the velocity.
            pParticleNode->mVelocity += (pParticleAssetEmitter->getFixedForceDirection() * (pParticleNode->mRenderFixedForce * getForceScale()) * elapsedTime);
        }

        // Are we suppressing movement?
        if ( !pParticleNode->mSuppressMovement )
        {
            // No, so adjust particle position.
            pParticleNode->mPosition += (pParticleNode->mVelocity * pParticleNode->mRenderSpeed * elapsedTime);
        }
    }


    // **********************************************************************************************************************
    // Are we Aligning to motion?
    // **********************************************************************************************************************
    if ( pParticleAssetEmitter->getKeepAligned() && pParticleAssetEmitter->getOrientationType() == ParticleAssetEmitter::ALIGNED_ORIENTATION )
    {
        // Yes, so calculate last movement direction.
        F32 movementAngle = mRadToDeg( mAtan( pParticleNode->mVelocity.x, pParticleNode->mVelocity.y ) );

        // Adjust for negative ArcTan quadrants.
        if ( movementAngle < 0.0f )
            movementAngle += 360.0f;

        // Set new Orientation Angle.
        pParticleNode->mOrientationAngle = movementAngle - pParticleAssetEmitter->getAlignedAngleOffset();

    }
    else
    {
        // No, so calculate the render spin.
        pParticleNode->mRenderSpin = pParticleNode->mSpin * pParticleAssetEmitter->getSpinLifeField().getFieldValue( particleAge );

        // Have we got some Spin?
        if ( mNotZero(pParticleNode->mRenderSpin) )
        {
            // Yes, so add into Orientation.
            pParticleNode->mOrientationAngle += pParticleNode->mRenderSpin * elapsedTime;

            // Clamp the orientation angle.
            pParticleNode->mOrientationAngle = mFmod( pParticleNode->mOrientationAngle, 360.0f );
        }
    }

    // Calculate the transform.
    pParticleNode->mTransform.Set( pParticleNode->mPosition, mDegToRad(pParticleNode->mOrientationAngle) );

    // Fetch the local AABB..
    const Vector2& localAABB0 = pParticleAssetEmitter->getLocalPivotAABB0();
    const Vector2& localAABB1 = pParticleAssetEmitter->getLocalPivotAABB1();
    const Vector2& localAABB2 = pParticleAssetEmitter->getLocalPivotAABB2();
    const Vector2& localAABB3 = pParticleAssetEmitter->getLocalPivotAABB3();

    // Fetch the render size.
    const Vector2& renderSize = pParticleNode->mRenderSize;

    // Calculate the scaled AABB.
    Vector2 scaledAABB[4];
    scaledAABB[0] = localAABB0 * renderSize;
    scaledAABB[1] = localAABB1 * renderSize;
    scaledAABB[2] = localAABB2 * renderSize;
    scaledAABB[3] = localAABB3 * renderSize;

    // Calculate the world OOBB..
    CoreMath::mCalculateOOBB( scaledAABB, pParticleNode->mTransform, pParticleNode->mRenderOOBB );


    // **********************************************************************************************************************
    // Set Post Tick Position.
    // **********************************************************************************************************************
    pParticleNode->mPostTickPosition = pParticleNode->mPosition;
}

//-----------------------------------------------------------------------------

void ParticlePlayer::onTamlAddParent( SimObject* pParentObject )
{
    // Call parent.
    Parent::onTamlAddParent( pParentObject );

    // Play  automatically when added to a parent.
    play( true );
}

//-----------------------------------------------------------------------------

void ParticlePlayer::initializeParticleAsset( void )
{
    // Note if we were playing.
    const bool wasPlaying = getIsPlaying();

    // Destroy any existing particle asset.
    destroyParticleAsset();

    // Finish if no particle asset.
    if ( mParticleAsset.isNull() )
        return;

    // Fetch the particle asset.
    ParticleAsset* pParticleAsset = mParticleAsset;

    // Fetch the emitter count.
    const U32 emitterCount = pParticleAsset->getEmitterCount();

    // Finish if no emitters found.
    if ( emitterCount == 0 )
        return;

    // Add each emitter reference.
    for( U32 emitterIndex = 0; emitterIndex < emitterCount; ++emitterIndex )
    {
        // Fetch the asset emitter.
        ParticleAssetEmitter* pParticleAssetEmitter = pParticleAsset->getEmitter( emitterIndex );

        // Fetch both image and animation assets.
        const AssetPtr<ImageAsset>& imageAsset = pParticleAssetEmitter->getImageAsset();
        const AssetPtr<AnimationAsset>& animationAsset = pParticleAssetEmitter->getAnimationAsset();

        // Skip if the emitter does not have a valid assigned asset to render.
        if (( pParticleAssetEmitter->isStaticFrameProvider() && (imageAsset.isNull() || imageAsset->getFrameCount() == 0 ) ) ||
            ( !pParticleAssetEmitter->isStaticFrameProvider() && (animationAsset.isNull() || (animationAsset->getValidatedAnimationFrames().size() == 0 && animationAsset->getValidatedNamedAnimationFrames().size() == 0)) ) )
            continue;

        // Create a new emitter node.
        EmitterNode* pEmitterNode = new EmitterNode( this, pParticleAssetEmitter );

        // Store new emitter node.
        mEmitters.push_back( pEmitterNode );
    }

    // Start playing if we were playing before the update.
    if ( wasPlaying )
        play( false );
}

//-----------------------------------------------------------------------------

void ParticlePlayer::destroyParticleAsset( void )
{
    // Stop playing.
    stop( false, false );

    // Destroy all emitters.
    while( mEmitters.size() > 0 )
    {
        delete mEmitters[mEmitters.size()-1];
        mEmitters.pop_back();
    }
    mEmitters.clear();
}
