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

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _COLOR_H_
#include "graphics/color.h"
#endif

#ifndef _BITSTREAM_H_
#include "io/bitStream.h"
#endif

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _BEHAVIORTEMPLATE_H_
#include "component/behaviors/behaviorTemplate.h"
#endif

#ifndef _SCENE_OBJECT_ROTATE_TO_EVENT_H_
#include "2d/sceneobject/SceneObjectRotateToEvent.h"
#endif

#ifndef _RENDER_PROXY_H_
#include "2d/core/RenderProxy.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

// Script bindings.
#include "SceneObject_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

// Scene-Object counter.
static U32 sGlobalSceneObjectCount = 0;
static U32 sSceneObjectMasterSerialId = 0;

// Collision shapes custom node names.
static StringTableEntry shapeCustomNodeName     = StringTable->insert( "CollisionShapes" );

static StringTableEntry shapeDensityName        = StringTable->insert( "Density" );
static StringTableEntry shapeFrictionName       = StringTable->insert( "Friction" );
static StringTableEntry shapeRestitutionName    = StringTable->insert( "Restitution" );
static StringTableEntry shapeSensorName         = StringTable->insert( "Sensor" );
static StringTableEntry shapePointName          = StringTable->insert( "Point" );
static StringTableEntry shapePrevPointName      = StringTable->insert( "PreviousPoint" );
static StringTableEntry shapeNextPointName      = StringTable->insert( "NextPoint" );
static StringTableEntry circleTypeName          = StringTable->insert( "Circle" );
static StringTableEntry circleRadiusName        = StringTable->insert( "Radius" );
static StringTableEntry circleOffsetName        = StringTable->insert( "Offset" );
static StringTableEntry polygonTypeName         = StringTable->insert( "Polygon" );
static StringTableEntry chainTypeName           = StringTable->insert( "Chain" );
static StringTableEntry edgeTypeName            = StringTable->insert( "Edge" );

//------------------------------------------------------------------------------

// Important: If these defaults are changed then modify the associated "write" field protected methods to ensure
// that the associated field is persisted if not the default.
SceneObject::SceneObject() :
    /// Scene.
    mpScene(NULL),
    mpTargetScene(NULL),

    /// Lifetime.
    mLifetime(0.0f),
    mLifetimeActive(false),

    /// Layers.
    mSceneLayer(0),
    mSceneLayerMask(BIT(mSceneLayer)),
    mSceneLayerDepth(0.0f),

    /// Scene groups.
    mSceneGroup(0),
    mSceneGroupMask(BIT(mSceneGroup)),

    /// Area.
	mWorldProxyId(-1),

	// Growing.
	mGrowActive(false),

    /// Position / Angle.
    mPreTickPosition( 0.0f, 0.0f ),
    mPreTickAngle( 0.0f ),
    mRenderPosition( 0.0f, 0.0f ),
    mRenderAngle( 0.0f ),
    mSpatialDirty( true ),
    mTargetPosition( 0.0f, 0.0f ),
    mLastCheckedPosition( 0.0f, 0.0f ),
    mTargetPositionActive( false ),
    mDistanceToTarget( 0.0f ),
    mTargetPositionMargin( 0.1f ),
    mTargetPositionFound( false ),
    mSnapToTargetPosition( true ),
    mStopAtTargetPosition( true ),

    /// Body.
    mpBody(NULL),
    mWorldQueryKey(0),

    /// Collision control.
    mCollisionLayerMask(MASK_ALL),
    mCollisionGroupMask(MASK_ALL),
    mCollisionSuppress(false),
    mCollisionOneWay(false),
    mGatherContacts(false),
    mpCurrentContacts(NULL),

    /// Render visibility.                                        
    mVisible(true),

    /// Render blending.
    mBlendMode(true),
    mSrcBlendFactor(GL_SRC_ALPHA),
    mDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA),
    mBlendColor(ColorF(1.0f,1.0f,1.0f,1.0f)),
    mAlphaTest(-1.0f),

	// Fading.
	mFadeActive(false),
	mTargetColor(ColorF(1.0f, 1.0f, 1.0f, 1.0f)),
	mDeltaRed(1.0f),
	mDeltaGreen(1.0f),
	mDeltaBlue(1.0f),
	mDeltaAlpha(1.0f),

    /// Render sorting.
    mSortPoint(0.0f,0.0f),

    /// Input events.
    mUseInputEvents(false),

    /// Script callbacks.
    mUpdateCallback(false),
    mCollisionCallback(false),
    mSleepingCallback(false),

    /// Debug mode.
    mDebugMask(0X00000000),

    /// Camera mounting.
    mpAttachedCamera(NULL),

    /// Safe deletion.
    mBeingSafeDeleted(false),
    mSafeDeleteReady(true),

    /// Miscellaneous.
    mBatchIsolated(false),
    mSerialiseKey(0),
    mEditorTickAllowed(true),
    mPickingAllowed(true),
    mAlwaysInScope(false),
    mRotateToEventId(0),
    mSerialId(0),
    mRenderGroup( StringTable->EmptyString )
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mDestroyNotifyList );
    VECTOR_SET_ASSOCIATION( mCollisionFixtureDefs );
    VECTOR_SET_ASSOCIATION( mCollisionFixtures );
    VECTOR_SET_ASSOCIATION( mAttachedCtrls );
    VECTOR_SET_ASSOCIATION( mAudioHandles );
    VECTOR_SET_ASSOCIATION( mHandleDeletionList );

    // Assign scene-object index.
    mSerialId = ++sSceneObjectMasterSerialId;
    sGlobalSceneObjectCount++;

    // Initialize the body definition.
    // Important: If these defaults are changed then modify the associated "write" field protected methods to ensure
    // that the associated field is persisted if not the default.
    mBodyDefinition.userData        = static_cast<PhysicsProxy*>(this);
    mBodyDefinition.position.Set(0.0f, 0.0f);
    mBodyDefinition.angle           = 0.0f;
    mBodyDefinition.linearVelocity.Set(0.0f, 0.0f);
    mBodyDefinition.angularVelocity = 0.0f;
    mBodyDefinition.linearDamping   = 0.0f;
    mBodyDefinition.angularDamping  = 0.0f;
    mBodyDefinition.allowSleep      = true;
    mBodyDefinition.awake           = true;
    mBodyDefinition.fixedRotation   = false;
    mBodyDefinition.bullet          = false;
    mBodyDefinition.type            = b2_dynamicBody;
    mBodyDefinition.active          = true;
    mBodyDefinition.gravityScale    = 1.0f;

    // Initialize the default fixture definition.
    // Important: If these defaults are changed then modify the associated "write" field protected methods to ensure
    // that the associated field is persisted if not the default.
    mDefaultFixture.userData = static_cast<PhysicsProxy*>(this);
    mDefaultFixture.density     = 1.0f;
    mDefaultFixture.friction    = 0.2f;
    mDefaultFixture.restitution = 0.0f;
    mDefaultFixture.isSensor    = false;
    mDefaultFixture.shape       = NULL;

    // Set last awake state.
    mLastAwakeState = !mBodyDefinition.allowSleep || mBodyDefinition.awake;

    // Turn-off auto-sizing.
    mAutoSizing = false;

    // Set size.
    setSize( Vector2::getOne() );
}

//-----------------------------------------------------------------------------

SceneObject::~SceneObject()
{
    // Are we in a Scene?
    if ( mpScene )
    {
        // Yes, so remove from Scene.
        mpScene->removeFromScene( this );
    }

    if (mAudioHandles.size())
    {
       for (typeAudioHandleVector::iterator itr = mAudioHandles.begin(); itr != mAudioHandles.end(); ++itr)
       {
       U32 handle = *itr;
       alxStop(handle);
       }
       mAudioHandles.clear();
    }

    // Decrease scene-object count.
    --sGlobalSceneObjectCount;
}

//-----------------------------------------------------------------------------

void SceneObject::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    /// Lifetime.
    addProtectedField("Lifetime", TypeF32, Offset(mLifetime, SceneObject), &setLifetime, &defaultProtectedGetFn, &writeLifetime, "");

    /// Scene Layers.
    addProtectedField("SceneLayer", TypeS32, Offset(mSceneLayer, SceneObject), &setSceneLayer, &defaultProtectedGetFn, &writeSceneLayer, "");
    addProtectedField("SceneLayerDepth", TypeF32, Offset(mSceneLayerDepth, SceneObject), &setSceneLayerDepth, &defaultProtectedGetFn, &writeSceneLayerDepth, "" );
    
    // Scene Groups.
    addProtectedField("SceneGroup", TypeS32, Offset(mSceneGroup, SceneObject), &setSceneGroup, &defaultProtectedGetFn, &writeSceneGroup, "");

    /// Area.
    addProtectedField("Size", TypeVector2, Offset( mSize, SceneObject), &setSize, &defaultProtectedGetFn, &writeSize, "");

    /// Position / Angle.
    addProtectedField("Position", TypeVector2, 0, &setPosition, &getPosition, &writePosition, "");
    addProtectedField("Angle", TypeF32, 0, &setAngle, &getAngle, &writeAngle, "");
    addProtectedField("FixedAngle", TypeBool, 0, &setFixedAngle, &getFixedAngle, &writeFixedAngle, "");

    /// Body.
    addProtectedField("BodyType", TypeEnum, 0, &setBodyType, &getBodyType, &writeBodyType, 1, &bodyTypeTable, "" );
    addProtectedField("Active", TypeBool, 0, &setActive, &getActive, &writeActive, "" );
    addProtectedField("Awake", TypeBool, 0, &setAwake, &getAwake, &writeAwake, "" );
    addProtectedField("Bullet", TypeBool, 0, &setBullet, &getBullet, &writeBullet, "" );
    addProtectedField("SleepingAllowed", TypeBool, 0, &setSleepingAllowed, &getSleepingAllowed, &writeSleepingAllowed, "" );

    /// Collision control.
    addProtectedField("CollisionGroups", TypeS32, Offset(mCollisionGroupMask, SceneObject), &setCollisionGroups, &getCollisionGroups, &writeCollisionGroups, "");
    addProtectedField("CollisionLayers", TypeS32, Offset(mCollisionLayerMask, SceneObject), &setCollisionLayers, &getCollisionLayers, &writeCollisionLayers, "");
    addField("CollisionSuppress", TypeBool, Offset(mCollisionSuppress, SceneObject), &writeCollisionSuppress, "");
    addField("CollisionOneWay", TypeBool, Offset(mCollisionOneWay, SceneObject), &writeCollisionOneWay, "");
    addProtectedField("GatherContacts", TypeBool, 0, &setGatherContacts, &defaultProtectedGetFn, &writeGatherContacts, "");
    addProtectedField("DefaultDensity", TypeF32, Offset( mDefaultFixture.density, SceneObject), &setDefaultDensity, &defaultProtectedGetFn, &writeDefaultDensity, "");
    addProtectedField("DefaultFriction", TypeF32, Offset( mDefaultFixture.friction, SceneObject), &setDefaultFriction, &defaultProtectedGetFn, &writeDefaultFriction, "");
    addProtectedField("DefaultRestitution", TypeF32, Offset( mDefaultFixture.restitution, SceneObject), &setDefaultRestitution, &defaultProtectedGetFn, &writeDefaultRestitution, "");

    /// Velocities.
    addProtectedField("LinearVelocity", TypeVector2, 0, &setLinearVelocity, &getLinearVelocity, &writeLinearVelocity, "");
    addProtectedField("AngularVelocity", TypeF32, 0, &setAngularVelocity, &getAngularVelocity, &writeAngularVelocity, "");
    addProtectedField("LinearDamping", TypeF32, 0, &setLinearDamping, &getLinearDamping, &writeLinearDamping, "");
    addProtectedField("AngularDamping", TypeF32, 0, &setAngularDamping, &getAngularDamping, &writeAngularDamping, "");

    /// Gravity scaling.
    addProtectedField("GravityScale", TypeF32, 0, &setGravityScale, &getGravityScale, &writeGravityScale, "");

    /// Render visibility.
    addField("Visible", TypeBool, Offset(mVisible, SceneObject), &writeVisible, "");

    /// Render blending.
    addField("BlendMode", TypeBool, Offset(mBlendMode, SceneObject), &writeBlendMode, "");
    addField("SrcBlendFactor", TypeEnum, Offset(mSrcBlendFactor, SceneObject), &writeSrcBlendFactor, 1, &srcBlendFactorTable);
    addField("DstBlendFactor", TypeEnum, Offset(mDstBlendFactor, SceneObject), &writeDstBlendFactor, 1, &dstBlendFactorTable);
    addField("BlendColor", TypeColorF, Offset(mBlendColor, SceneObject), &writeBlendColor, "");
    addField("AlphaTest", TypeF32, Offset(mAlphaTest, SceneObject), &writeAlphaTest, "");

    /// Render sorting.
    addField("SortPoint", TypeVector2, Offset(mSortPoint, SceneObject), &writeSortPoint, "");
    addField("RenderGroup", TypeString, Offset(mRenderGroup, SceneObject), &writeRenderGroup, "");

    /// Input events.
    addField("UseInputEvents", TypeBool, Offset(mUseInputEvents, SceneObject), &writeUseInputEvents, "");

    addField("PickingAllowed", TypeBool, Offset(mPickingAllowed, SceneObject), &writePickingAllowed, "");

    // Script callbacks.
    addField("UpdateCallback", TypeBool, Offset(mUpdateCallback, SceneObject), &writeUpdateCallback, "");
    addField("CollisionCallback", TypeBool, Offset(mCollisionCallback, SceneObject), &writeCollisionCallback, "");
    addField("SleepingCallback", TypeBool, Offset(mSleepingCallback, SceneObject), &writeSleepingCallback, "");

    /// Scene.
    addProtectedField("scene", TypeSimObjectPtr, Offset(mpScene, SceneObject), &setScene, &defaultProtectedGetFn, &writeScene, "");
}

//-----------------------------------------------------------------------------

bool SceneObject::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Add to any target scene.
    if ( mpTargetScene )
    {
        // Add to the target scene.
        mpTargetScene->addToScene(this);

        mpTargetScene = NULL;
    }
   
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void SceneObject::onRemove()
{
    // Detach all GUI Control.
    detachAllGuiControls();

    // Remove from Scene.
    if ( getScene() )
        getScene()->removeFromScene( this );

    // Call Parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------

void SceneObject::onDestroyNotify( SceneObject* pSceneObject )
{
}

//-----------------------------------------------------------------------------

void SceneObject::OnRegisterScene( Scene* pScene )
{
    // Sanity!
    AssertFatal( mpScene == NULL, "Cannot register to a scene if already registered." );
    AssertFatal( mpBody == NULL, "Cannot create a physics body if one already exists." );

    // Initialize contact gathering.
    initializeContactGathering();

    // Set scene.
    mpScene = pScene;

    // Create the physics body.
    mpBody = pScene->getWorld()->CreateBody( &mBodyDefinition );

    // Set active status.
    if ( !isEnabled() ) mpBody->SetActive( false );

    // Create fixtures.
    for( typeCollisionFixtureDefVector::iterator itr = mCollisionFixtureDefs.begin(); itr != mCollisionFixtureDefs.end(); itr++ )
    {
        // Fetch fixture definition.
        b2FixtureDef* pFixtureDef = (*itr);

        // Create fixture.
        b2Fixture* pFixture = mpBody->CreateFixture( pFixtureDef );

        // Push fixture.
        mCollisionFixtures.push_back( pFixture );

        // Destroy fixture shape.
        delete pFixtureDef->shape;

        // Destroy fixture definition.
        delete pFixtureDef;          
    }

    // Clear offline fixture definitions.
    mCollisionFixtureDefs.clear();

    // Calculate current AABB.
    CoreMath::mCalculateAABB( getLocalSizedOOBB(), mpBody->GetTransform(), &mCurrentAABB );

    // Create world proxy Id.
    mWorldProxyId = pScene->getWorldQuery()->add( this );

    // Reset the spatials.
    resetTickSpatials();

    // Notify components.
    notifyComponentsAddToScene();
}

//-----------------------------------------------------------------------------

void SceneObject::OnUnregisterScene( Scene* pScene )
{
    // Sanity!
    AssertFatal( mpScene == pScene, "Cannot unregister from a scene that is not registered." );
    AssertFatal( mpBody != NULL, "Cannot unregister physics body as it does not exist." );

    // Notify components.
    notifyComponentsRemoveFromScene();

    // Copy fixtures to fixture definitions.
    for( typeCollisionFixtureVector::iterator itr = mCollisionFixtures.begin(); itr != mCollisionFixtures.end(); itr++ )
    {
        // Fetch fixture.
        b2Fixture* pFixture = (*itr);

        // Create fixture definition.
        b2FixtureDef* pFixtureDef = new b2FixtureDef();
        pFixtureDef->density      = pFixture->GetDensity();
        pFixtureDef->friction     = pFixture->GetFriction();
        pFixtureDef->restitution  = pFixture->GetRestitution();
        pFixtureDef->isSensor     = pFixture->IsSensor();        
        pFixtureDef->userData     = this;
        pFixtureDef->shape        = pFixture->GetShape()->Clone( pScene->getBlockAllocator() );

        // Push fixture definition.
        mCollisionFixtureDefs.push_back( pFixtureDef );
    }

    // Clear our fixture references.
    // The actual fixtures will get destroyed when the body is destroyed so no need to destroy them here.
    mCollisionFixtures.clear();

    // Transfer physics body configuration back to definition.
    mBodyDefinition.type            = getBodyType();
    mBodyDefinition.position        = getPosition();
    mBodyDefinition.angle           = getAngle();
    mBodyDefinition.linearVelocity  = getLinearVelocity();
    mBodyDefinition.angularVelocity = getAngularVelocity();
    mBodyDefinition.linearDamping   = getLinearDamping();
    mBodyDefinition.angularDamping  = getAngularDamping();
    mBodyDefinition.gravityScale    = getGravityScale();
    mBodyDefinition.allowSleep      = getSleepingAllowed();
    mBodyDefinition.awake           = getAwake();
    mBodyDefinition.fixedRotation   = getFixedAngle();
    mBodyDefinition.bullet          = getBullet();
    mBodyDefinition.active          = getActive();

    // Destroy current contacts.
    delete mpCurrentContacts;
    mpCurrentContacts = NULL;

    // Destroy the physics body.
    mpScene->getWorld()->DestroyBody( mpBody );
    mpBody = NULL;

    // Destroy world proxy Id.
    if ( mWorldProxyId != -1 )
    {

        mpScene->getWorldQuery()->remove( this );
        mWorldProxyId = -1;
    }

    // Reset scene.
    mpScene = NULL;
}

//-----------------------------------------------------------------------------

void SceneObject::resetTickSpatials( const bool resize )
{
    // Set coincident pre-tick, current & render.
    mPreTickPosition = mRenderPosition = getPosition();
    mPreTickAngle = mRenderAngle = getAngle();

    // Fetch body transform.
    b2Transform bodyXform = getTransform();

    // Calculate current AABB.
    CoreMath::mCalculateAABB( getLocalSizedOOBB(), bodyXform, &mCurrentAABB );

    // Set coincident AABBs.
    mPreTickAABB = mCurrentAABB;

    // Calculate render OOBB.
    CoreMath::mCalculateOOBB( getLocalSizedOOBB(), bodyXform, mRenderOOBB );

    // Update world proxy (if in scene).
    if ( mpScene )
    {
        // Fetch world query.
        WorldQuery* pWorldQuery = mpScene->getWorldQuery();

        // Are we resizing the object?
        if ( resize )
        {
            // Yes, so we need to recreate a proxy.
            pWorldQuery->remove( this );
            mWorldProxyId = pWorldQuery->add( this );

        }
        else
        {
            // No, so we can simply update the proxy.
            pWorldQuery->update( this, mCurrentAABB, b2Vec2( 0.0f, 0.0f ) );
        }
    }

    // Flag spatial changed.
    mSpatialDirty = true;
}

//-----------------------------------------------------------------------------

void SceneObject::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_PreIntegrate);

	// Update the Size.
	if (mGrowActive)
	{
		updateSize(elapsedTime);
	}

    if (mAudioHandles.size())
        refreshsources();

   // Finish if nothing is dirty.
    if ( !mSpatialDirty )
        return;
    
    // Reset spatial changed.
    mSpatialDirty = false;

    mPreTickPosition = mRenderPosition = getPosition();
    mPreTickAngle    = mRenderAngle = getAngle();
    mPreTickAABB     = mCurrentAABB;

    // Calculate render OOBB.
    CoreMath::mCalculateOOBB( getLocalSizedOOBB(), getTransform(), mRenderOOBB );
}

//-----------------------------------------------------------------------------

void SceneObject::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_IntegrateObject);

    // Fetch position.
    const b2Vec2 position = getPosition();

    // Has the angle or position changed?
    if (    mPreTickAngle != getAngle() ||
            mPreTickPosition.x != position.x ||
            mPreTickPosition.y != position.y )
    {
        // Yes, so flag spatial dirty.
        mSpatialDirty = true;

        // Calculate current AABB.
        CoreMath::mCalculateAABB( getLocalSizedOOBB(), getTransform(), &mCurrentAABB );

        // Calculate tick AABB.
        b2AABB tickAABB;
        tickAABB.Combine( mPreTickAABB, mCurrentAABB );

        // Calculate tick displacement.
        b2Vec2 tickDisplacement = position - mPreTickPosition;
            
        // Update world proxy.
        mpScene->getWorldQuery()->update( this, tickAABB, tickDisplacement );

        //have we arrived at the target position?
        if (mTargetPositionActive)
        {
           updateTargetPosition();
        }
    }

    // Update Lifetime.
    if ( mLifetimeActive && !getScene()->getIsEditorScene() )
    {
        updateLifetime( elapsedTime );
    }

    // Update Any Attached GUI.
    if ( mAttachedCtrls.size() )
    {
        updateAttachedGui();
    }

    // Are we attached to a camera?
    if ( mpAttachedCamera )
    {
        // Yes, so calculate camera mount.
        mpAttachedCamera->calculateCameraMount( elapsedTime );
    }

	// Update the BlendColor.
	if ( mFadeActive )
	{
		updateBlendColor( elapsedTime );
	}

    if (mAudioHandles.size())
    {
        for (typeAudioHandleVector::iterator itr = mAudioHandles.begin(); itr != mAudioHandles.end(); ++itr)
        {
            U32 handle = *itr;
            Point2F vel = getLinearVelocity();
            alxSource3f(handle, AL_POSITION, position.x, position.y, 0.f);
            alxSource3f(handle, AL_VELOCITY, vel.x, vel.y, 0.f);
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::postIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats *pDebugStats)
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_PostIntegrate);

    // Finish if no scene.
    if( !getScene() )
        return;

    // Notify components.
    notifyComponentsUpdate();

    // Script "onUpdate".
    if ( mUpdateCallback )
    {
        PROFILE_SCOPE(SceneObject_onUpdateCallback);
        Con::executef(this, 1, "onUpdate");
    }

    // Check to see if we're done moving.
    if (mTargetPositionActive && mTargetPositionFound)
    {
       mTargetPositionActive = false;

       PROFILE_SCOPE(SceneObject_onMoveToComplete);
       Con::executef(this, 1, "onMoveToComplete");
    }

	// Check to see if we're done fading.
	if ( mFadeActive && mBlendColor == mTargetColor )
	{
		mFadeActive = false;

		PROFILE_SCOPE(SceneObject_onFadeToComplete);
		Con::executef(this, 1, "onFadeToComplete");
	}

	//Check to see if we're done growing.
	if (mGrowActive && mSize == mTargetSize)
	{
		mGrowActive = false;

		PROFILE_SCOPE(SceneObject_onGrowToComplete);
		Con::executef(this, 1, "onGrowToComplete");
	}

    // Are we using the sleeping callback?
    if ( mSleepingCallback )
    {
        PROFILE_SCOPE(SceneObject_onWakeSleepCallback);

        // Yes, so fetch the current awake state.
        const bool currentAwakeState = getAwake();

        // Did we change awake state?
        if ( currentAwakeState != mLastAwakeState )
        {
            // Yes, so update last awake state.
            mLastAwakeState = currentAwakeState;

            // Perform the appropriate callback.
            if ( currentAwakeState )
                Con::executef(this, 1, "onWake");
            else
                Con::executef(this, 1, "onSleep");
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::interpolateObject( const F32 timeDelta )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_InterpolateObject);

    if ( mSpatialDirty )
    {
        // Debug Profiling.
        PROFILE_SCOPE(SceneObject_InterpolatePositionAngle);

        if ( timeDelta < 1.0f )
        {
            // Calculate render position.
            const b2Vec2 position = getPosition();
            b2Vec2 positionDelta = position - mPreTickPosition;
            positionDelta *= timeDelta;
            mRenderPosition = position - positionDelta;

            // Calculate render angle.
            const F32 angle = getAngle();

            F32 relativeAngle = angle - mPreTickAngle;
            if ( relativeAngle > b2_pi )
                relativeAngle -= b2_pi2;
            else if ( relativeAngle < -b2_pi )
                relativeAngle += b2_pi2;
            mRenderAngle = angle - (relativeAngle * timeDelta);
        }
        else
        {
            mRenderPosition = mPreTickPosition;
            mRenderAngle    = mPreTickAngle;
        }
     
        // Calculate render transform.
        b2Transform renderXF( mRenderPosition, b2Rot(mRenderAngle) );

        // Calculate render OOBB.
        CoreMath::mCalculateOOBB( getLocalSizedOOBB(), renderXF, mRenderOOBB );
    }

    // Update Any Attached GUI.
    if ( mAttachedCtrls.size() )
    {
        updateAttachedGui();
    }

    // Are we attached to a camera?
    if ( mpAttachedCamera )
    {
        // Yes, so interpolate camera mount.
        mpAttachedCamera->interpolateCameraMount( timeDelta );
    }
};

//-----------------------------------------------------------------------------

void SceneObject::sceneRenderFallback( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SceneRenderFallback);

    // Fetch the 'cannot render' proxy.
    RenderProxy* pNoImageRenderProxy = Sim::findObject<RenderProxy>( CANNOT_RENDER_PROXY_NAME );

    // Finish if no render proxy available or it can't render.
    if ( pNoImageRenderProxy == NULL || !pNoImageRenderProxy->validRender() )
        return;

    // Fetch render AABB.
    const b2Vec2* pRenderOOBB = getRenderOOBB();

    // Render using render proxy.
    pNoImageRenderProxy->render(
        false, false,
        pRenderOOBB[0], pRenderOOBB[1], pRenderOOBB[2], pRenderOOBB[3],
        pBatchRenderer );
}

//-----------------------------------------------------------------------------

void SceneObject::sceneRenderOverlay( const SceneRenderState* sceneRenderState )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SceneRenderOverlay);

    // Get Scene.
    Scene* pScene = getScene();

    // Cannot do anything without scene!
    if ( !pScene )
        return;

    // Don't draw debug if not enabled.
    if ( !isEnabled() )
        return;

    // Get merged Local/Scene Debug Mask.
    U32 debugMask = getDebugMask() | pScene->getDebugMask();

    // Finish here if we're not rendering any debug info or only have scene-rendered debug options.
    if ( !debugMask || (debugMask & ~(Scene::SCENE_DEBUG_METRICS | Scene::SCENE_DEBUG_JOINTS)) == 0 )
        return;

    // Clear blending.
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );

    // AABB debug draw.
    if ( debugMask & Scene::SCENE_DEBUG_AABB )
    {
        pScene->mDebugDraw.DrawAABB( mCurrentAABB, ColorF(0.7f, 0.7f, 0.9f) );
    }

    // OOBB debug draw.
    if ( debugMask & Scene::SCENE_DEBUG_OOBB )
    {
        pScene->mDebugDraw.DrawOOBB( mRenderOOBB, ColorF(0.9f, 0.9f, 1.0f) );
    }

    // Asleep debug draw.
    if ( !getAwake() && debugMask & Scene::SCENE_DEBUG_SLEEP )
    {
        pScene->mDebugDraw.DrawAsleep( mRenderOOBB, ColorF( 0.0f, 1.0f, 0.0f ) );
    }

    // Collision Shapes.
    if ( debugMask & Scene::SCENE_DEBUG_COLLISION_SHAPES )
    {
        pScene->mDebugDraw.DrawCollisionShapes( getRenderTransform(), getBody() );
    }

    // Position and local center of mass.
    if ( debugMask & Scene::SCENE_DEBUG_POSITION_AND_COM )
    {
        const b2Vec2 renderPosition = getRenderPosition();

        pScene->mDebugDraw.DrawPoint( renderPosition + getLocalCenter(), 6, ColorF( 0.0f, 1.0f, 0.4f ) );
        pScene->mDebugDraw.DrawPoint( renderPosition, 4, ColorF( 0.0f, 0.4f, 1.0f ) );
    }

    // Sort Points.
    if ( debugMask & Scene::SCENE_DEBUG_SORT_POINTS )
    {
        pScene->mDebugDraw.DrawSortPoint( getRenderPosition(), getSize(), mSortPoint );
    }

    if (debugMask & Scene::SCENE_DEBUG_AUDIO_SOURCES)
    {
        if (mAudioHandles.size())
        {
            for (typeAudioHandleVector::iterator itr = mAudioHandles.begin(); itr != mAudioHandles.end(); ++itr)
            {
                U32 handle = *itr;
                ALfloat MaxDistance = 0.f;
                ALfloat RefDistance = 0.f;
                alxGetSourcef(handle, AL_MAX_DISTANCE, &MaxDistance);
                alxGetSourcef(handle, AL_REFERENCE_DISTANCE, &RefDistance);
                pScene->mDebugDraw.DrawCircle(getRenderPosition(), MaxDistance, ColorF(1.f, 0.2f, 0.2f));
                pScene->mDebugDraw.DrawCircle(getRenderPosition(), RefDistance, ColorF(1.f, 0.0f, 0.0f));
            }
        }
        
    }
}

//-----------------------------------------------------------------------------

U32 SceneObject::packUpdate(NetConnection * conn, U32 mask, BitStream *stream)
{
    return 0;
}

//-----------------------------------------------------------------------------

void SceneObject::unpackUpdate(NetConnection * conn, BitStream *stream)
{
}

//-----------------------------------------------------------------------------

void SceneObject::setEnabled( const bool enabled )
{
    // Call parent.
    Parent::setEnabled( enabled );

    // If we have a scene, modify active.
    if ( mpScene )
    {
        mpBody->SetActive( enabled );
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setLifetime( const F32 lifetime )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_setLifetime);

    // Usage Flag.
    mLifetimeActive = mGreaterThanZero( lifetime );

    // Is life active?
    if ( mLifetimeActive )
    {
        // Yes, so set to incoming lifetime.
        mLifetime = lifetime;
    }
    else
    {
        // No, so reset it to be safe.
        mLifetime = 0.0f;
    }
}

//-----------------------------------------------------------------------------

void SceneObject::updateLifetime( const F32 elapsedTime )
{
    // Update Lifetime.
    if ( mLifetimeActive )
    {
        // Reduce Lifetime.
        setLifetime( getLifetime() - elapsedTime );

        // Are we now dead?
        if ( mLessThanOrEqual( getLifetime(), 0.0f) )
        {
            // Yes, so reset lifetime.
            setLifetime( 0.0f );

            // Initiate Death!
            safeDelete();
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setSceneLayer( const U32 sceneLayer )
{
    // Check Layer.
    if ( sceneLayer > (MAX_LAYERS_SUPPORTED-1) )
    {
        Con::warnf("SceneObject::setSceneLayer() - Invalid scene layer '%d' (0-31).", sceneLayer);
        return;
    }

    // Set Layer.
    mSceneLayer = sceneLayer;

    // Set Layer Mask.
    mSceneLayerMask = BIT( mSceneLayer );
}

//-----------------------------------------------------------------------------

bool SceneObject::setSceneLayerDepthFront( void )
{
    // Fetch the scene.
    Scene* pScene = getScene();

    // Finish if no scene or only a single object.
    if ( pScene == NULL || pScene->getSceneObjectCount() == 1 )
        return false;

    // Fetch scene objects.
    typeSceneObjectVector layerList;
    pScene->getSceneObjects( layerList, getSceneLayer() );

    // Fetch layer object count.
    const U32 layerObjectCount = (U32)layerList.size();

    // Sort the layer.
    dQsort(layerList.address(), layerObjectCount, sizeof(SceneObject*), sceneObjectLayerDepthSort);

    // Reset object index.
    U32 objectIndex = 0;

    // Find object index.
    for ( ; objectIndex < layerObjectCount; ++objectIndex )
    {
        // Stop if this is the current object index.
        if ( layerList[objectIndex] == this )
            break;
    }

    // Finish if already at front of layer.
    if ( objectIndex == layerObjectCount-1 )
        return false;

    // Fetch furthest front depth.
    const F32 frontDepth = layerList.last()->getSceneLayerDepth();

    // Adjust depth to be in front.
    setSceneLayerDepth( frontDepth - 1.0f );

    return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::setSceneLayerDepthBack( void )
{
    // Fetch the scene.
    Scene* pScene = getScene();

    // Finish if no scene or only a single object.
    if ( pScene == NULL || pScene->getSceneObjectCount() == 1 )
        return false;

    // Fetch scene objects.
    typeSceneObjectVector layerList;
    pScene->getSceneObjects( layerList, getSceneLayer() );

    // Fetch layer object count.
    const U32 layerObjectCount = (U32)layerList.size();

    // Sort the layer.
    dQsort(layerList.address(), layerObjectCount, sizeof(SceneObject*), sceneObjectLayerDepthSort);

    // Reset object index.
    U32 objectIndex = 0;

    // Find object index.
    for ( ; objectIndex < layerObjectCount; ++objectIndex )
    {
        // Stop if this is the current object index.
        if ( layerList[objectIndex] == this )
            break;
    }

    // Finish if already at back of layer.
    if ( objectIndex == 0 )
        return false;

    // Fetch furthest back depth.
    const F32 backDepth = layerList.first()->getSceneLayerDepth();

    // Adjust depth to be in back.
    setSceneLayerDepth( backDepth + 1.0f );

    return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::setSceneLayerDepthForward( void )
{
    // Fetch the scene.
    Scene* pScene = getScene();

    // Finish if no scene or only a single object.
    if ( pScene == NULL || pScene->getSceneObjectCount() == 1 )
        return false;

    // Fetch scene objects.
    typeSceneObjectVector layerList;
    pScene->getSceneObjects( layerList, getSceneLayer() );

    // Fetch layer object count.
    const U32 layerObjectCount = (U32)layerList.size();

    // Sort the layer.
    dQsort(layerList.address(), layerObjectCount, sizeof(SceneObject*), sceneObjectLayerDepthSort);

    // Reset object index.
    U32 objectIndex = 0;

    // Find object index.
    for ( ; objectIndex < layerObjectCount; ++objectIndex )
    {
        // Stop if this is the current object index.
        if ( layerList[objectIndex] == this )
            break;
    }

    // Finish if already at the front of the layer.
    if ( objectIndex == layerObjectCount-1 )
        return false;

    // Fetch forwards depth.
    const F32 forwardDepth = layerList[objectIndex+1]->getSceneLayerDepth();

    // Adjust depth to be forward.
    setSceneLayerDepth( forwardDepth - 0.5f );

    // Finish if we were almost at the front anyway.
    if ( objectIndex == layerObjectCount-2 )
        return true;

    // Adjust next objects forwards.
    for( U32 forwardIndex = objectIndex+2; forwardIndex < layerObjectCount; ++forwardIndex )
    {
        // Fetch scene object.
        SceneObject* forwardSceneObject = layerList[forwardIndex];

        // Adjust depth to be forwards.
        forwardSceneObject->setSceneLayerDepth( forwardSceneObject->getSceneLayerDepth() - 1.0f );
    }

    return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::setSceneLayerDepthBackward( void )
{
    // Fetch the scene.
    Scene* pScene = getScene();

    // Finish if no scene or only a single object.
    if ( pScene == NULL || pScene->getSceneObjectCount() == 1 )
        return false;

    // Fetch scene objects.
    typeSceneObjectVector layerList;
    pScene->getSceneObjects( layerList, getSceneLayer() );

    // Fetch layer object count.
    const U32 layerObjectCount = (U32)layerList.size();

    // Sort the layer.
    dQsort(layerList.address(), layerObjectCount, sizeof(SceneObject*), sceneObjectLayerDepthSort);

    // Reset object index.
    U32 objectIndex = 0;

    // Find object index.
    for ( ; objectIndex < layerObjectCount; ++objectIndex )
    {
        // Stop if this is the current object index.
        if ( layerList[objectIndex] == this )
            break;
    }

    // Finish if already at the back of the layer.
    if ( objectIndex == 0 )
        return false;

    // Fetch backwards depth.
    const F32 backDepth = layerList[objectIndex-1]->getSceneLayerDepth();

    // Adjust depth to be backwards.
    setSceneLayerDepth( backDepth + 0.5f );

    // Finish if we were almost at the back anyway.
    if ( objectIndex == 1 )
        return true;

    // Adjust previous objects backwards.
    for( U32 backIndex = 0; backIndex < (objectIndex-1); ++backIndex )
    {
        // Fetch scene object.
        SceneObject* backSceneObject = layerList[backIndex];

        // Adjust depth to be backwards.
        backSceneObject->setSceneLayerDepth( backSceneObject->getSceneLayerDepth() + 1.0f );
    }

    return true;
}

//-----------------------------------------------------------------------------

void SceneObject::setSceneGroup( const U32 sceneGroup )
{
    // Check Group.
    if ( sceneGroup > 31 )
    {
        Con::warnf("SceneObject::setSceneGroup() - Invalid scene group '%d' (0-31).", sceneGroup);
        return;
    }

    // Set Group.
    mSceneGroup = sceneGroup;

    // Set Group Mask.
    mSceneGroupMask = BIT( mSceneGroup );
}

//-----------------------------------------------------------------------------

void SceneObject::setArea( const Vector2& corner1, const Vector2& corner2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SetArea);

   // Calculate Normalized region.
   const Vector2 topLeft((corner1.x <= corner2.x) ? corner1.x : corner2.x, (corner1.y <= corner2.y) ? corner1.y : corner2.y);
   const Vector2 bottomRight((corner1.x > corner2.x) ? corner1.x : corner2.x, (corner1.y > corner2.y) ? corner1.y : corner2.y);

   // Calculate New Size.
   const Vector2 size = bottomRight - topLeft;

   // Calculate New Position.
   const Vector2 position = topLeft + (size * 0.5f);

   // Reset angle.
   setAngle( 0.0f );

   // Set Position/Size.
   setPosition( position );
   setSize( size );
}

//-----------------------------------------------------------------------------

void SceneObject::setSize( const Vector2& size )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SetSize);

    mSize = size;

    // Calculate half size.
    const F32 halfWidth = size.x * 0.5f;
    const F32 halfHeight = size.y * 0.5f;

    // Set local size OOBB.
    mLocalSizeOOBB[0].Set( -halfWidth, -halfHeight );
    mLocalSizeOOBB[1].Set( +halfWidth, -halfHeight );
    mLocalSizeOOBB[2].Set( +halfWidth, +halfHeight );
    mLocalSizeOOBB[3].Set( -halfWidth, +halfHeight );

    if ( mpScene )
    {
        // Reset tick spatials.
        resetTickSpatials( true );
    }
}

//-----------------------------------------------------------------------------
  
void SceneObject::setPosition( const Vector2& position )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SetPosition);

    if ( mpScene )
    {
        mpBody->SetTransform( position, mpBody->GetAngle() );

        // Reset tick spatials.
        resetTickSpatials();
    }
    else
    {
        mBodyDefinition.position = position;
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setAngle( const F32 radians )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_SetAngle);

    if ( mpScene )
    {
        mpBody->SetTransform( mpBody->GetPosition(), radians );

        // Reset tick spatials.
        resetTickSpatials();
    }
    else
    {
        mBodyDefinition.angle = radians;
    }
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getLocalPoint( const Vector2 &worldPoint )
{
    if ( mpScene )
    {
        return mpBody->GetLocalPoint( worldPoint );
    }

    // Calculate body definition transform.
    const b2Transform transform( mBodyDefinition.position, b2Rot(mBodyDefinition.angle) );

    return b2MulT(transform, worldPoint);
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getWorldPoint( const Vector2 &localPoint )
{
    if ( mpScene )
    {
        return mpBody->GetWorldPoint( localPoint );
    }

    // Calculate body definition transform.
    const b2Transform transform( mBodyDefinition.position, b2Rot(mBodyDefinition.angle) );

    return b2Mul(transform, localPoint);
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getLocalVector( const Vector2& worldVector )
{
    if ( mpScene )
    {
        return mpBody->GetLocalVector( worldVector );
    }

    // Calculate body definition transform.
    const b2Transform transform( mBodyDefinition.position, b2Rot(mBodyDefinition.angle) );

    return b2MulT(transform.q, worldVector);
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getWorldVector( const Vector2& localVector )
{
    if ( mpScene )
    {
        return mpBody->GetWorldVector( localVector );
    }

    // Calculate body definition transform.
    const b2Transform transform( mBodyDefinition.position, b2Rot(mBodyDefinition.angle) );

    return b2Mul(transform.q, localVector);
}

//-----------------------------------------------------------------------------

bool SceneObject::getIsPointInOOBB( const Vector2& worldPoint )
{
    // Fetch local point.
    Vector2 localPoint = getLocalPoint( worldPoint );

    // Turn the local point into an absolute component distance.
    localPoint.absolute();

    const Vector2 halfSize = getHalfSize();

    // Check distance.
    return !( localPoint.x > halfSize.x || localPoint.y > halfSize.y );
}

//-----------------------------------------------------------------------------

bool SceneObject::getIsPointInCollisionShape( const U32 shapeIndex, const Vector2& worldPoint )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getIsPointInCollisionShape() - Invalid shape index." );

    // Sanity!
    if ( !mpScene )
    {
        Con::warnf( "SceneObject::getIsPointInCollisionShape() - Cannot test for point, object not in scene." );
        return false;
    }

    // Fetch fixture.
    b2Fixture* pFixture = mCollisionFixtures[shapeIndex];

    // Fetch local point.
    const Vector2 localPoint = getLocalPoint( worldPoint );

    // Test point.
    return pFixture->TestPoint( localPoint );
}

//-----------------------------------------------------------------------------

void SceneObject::setBodyType( const b2BodyType type )
{
    // Sanity!
    AssertFatal( type == b2_staticBody || type == b2_kinematicBody || type == b2_dynamicBody, "Invalid body type." );

    if ( mpScene )
    {
        mpBody->SetType( type );
        return;
    }
    else
    {
        mBodyDefinition.type = type;
    }
}


//-----------------------------------------------------------------------------

void SceneObject::applyForce( const Vector2& worldForce, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    applyForce( worldForce, getPosition() + getLocalCenter(), wake );
}

//-----------------------------------------------------------------------------

void SceneObject::applyForce( const Vector2& worldForce, const Vector2& worldPoint, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    getBody()->ApplyForce( worldForce, worldPoint, wake );
}

//-----------------------------------------------------------------------------

void SceneObject::applyTorque( const F32 torque, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    getBody()->ApplyTorque( torque, wake );
}

//-----------------------------------------------------------------------------

void SceneObject::applyLinearImpulse( const Vector2& worldImpulse, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    applyLinearImpulse( worldImpulse, getPosition() + getLocalCenter(), wake );
}

//-----------------------------------------------------------------------------

void SceneObject::applyLinearImpulse( const Vector2& worldImpulse, const Vector2& worldPoint, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    getBody()->ApplyLinearImpulse( worldImpulse, worldPoint, wake );
}

//-----------------------------------------------------------------------------

void SceneObject::applyAngularImpulse( const F32 impulse, const bool wake )
{
    // Ignore if not in scene.
    if ( !mpScene )
        return;

    getBody()->ApplyAngularImpulse( impulse, wake );
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionAgainst( const SceneObject* pSceneObject, const bool clearMasks )
{
    // Do we need to clear existing masks?
    if ( clearMasks )
    {
        // Yes, so just set the masks to the referenced-objects' masks.
        setCollisionGroupMask( pSceneObject->getSceneGroupMask() );
        setCollisionLayerMask( pSceneObject->getSceneLayerMask() ); 
    }
    else
    {
        // No, so merge with existing masks.
        setCollisionGroupMask( getCollisionGroupMask() | pSceneObject->getSceneGroupMask() );
        setCollisionLayerMask( getCollisionLayerMask() | pSceneObject->getSceneLayerMask() ); 
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setDefaultDensity( const F32 density, const bool updateShapes )
{
    mDefaultFixture.density = density;

    // Early-out if not updating shapes.
    if ( !updateShapes )
        return;

    if ( mpScene )
    {
        // Update live fixtures.
        for( U32 index = 0; index < (U32)mCollisionFixtures.size(); ++index )
        {
            mCollisionFixtures[index]->SetDensity( density );
        }

        // Update the body mass data.
        mpBody->ResetMassData();

        return;
    }

    // Update offline fixture definitions.
    for( U32 index = 0; index < (U32)mCollisionFixtureDefs.size(); ++index )
    {
        mCollisionFixtureDefs[index]->density = density;
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setDefaultFriction( const F32 friction, const bool updateShapes )
{
    mDefaultFixture.friction = friction;

    // Early-out if not updating shapes.
    if ( !updateShapes )
        return;

    if ( mpScene )
    {
        // Update live fixtures.
        for( U32 index = 0; index < (U32)mCollisionFixtures.size(); ++index )
        {
            mCollisionFixtures[index]->SetFriction( friction );
        }
        return;
    }

    // Update offline fixture definitions.
    for( U32 index = 0; index < (U32)mCollisionFixtureDefs.size(); ++index )
    {
        mCollisionFixtureDefs[index]->friction = friction;
    }
}

//-----------------------------------------------------------------------------

void SceneObject::setDefaultRestitution( const F32 restitution, const bool updateShapes )
{
    mDefaultFixture.restitution = restitution;

    // Early-out if not updating shapes.
    if ( !updateShapes )
        return;

    if ( mpScene )
    {
        // Update live fixtures.
        for( U32 index = 0; index < (U32)mCollisionFixtures.size(); ++index )
        {
            mCollisionFixtures[index]->SetRestitution( restitution );
        }
        return;
    }

    // Update offline fixture definitions.
    for( U32 index = 0; index < (U32)mCollisionFixtureDefs.size(); ++index )
    {
        mCollisionFixtureDefs[index]->restitution = restitution;
    }
}

//-----------------------------------------------------------------------------

void SceneObject::initializeContactGathering( void )
{
    // Are we gathering contacts?
    if ( !mGatherContacts )
    {
        // No, so do we have any current contacts?
        if ( mpCurrentContacts )
        {
            // Yes, so destroy them.
            delete mpCurrentContacts;
            mpCurrentContacts = NULL;
        }
        return;
    }

    // Clear current contacts if already present.
    if ( mpCurrentContacts != NULL )
    {
        mpCurrentContacts->clear();
        return;
    }

    // Generate current contacts.
    mpCurrentContacts = new Scene::typeContactVector();
}

//-----------------------------------------------------------------------------

void SceneObject::onBeginCollision( const TickContact& tickContact )
{
    // Finish if we're not gathering contacts.
    if ( !mGatherContacts )
        return;

    // Sanity!
    AssertFatal( mpCurrentContacts != NULL, "SceneObject::onBeginCollision() - Contacts not initialized correctly." );
    AssertFatal( tickContact.mpSceneObjectA == this || tickContact.mpSceneObjectB == this, "SceneObject::onBeginCollision() - Contact does not involve this scene object." );

    // Keep contact.
    mpCurrentContacts->push_back( tickContact );
}

//-----------------------------------------------------------------------------

void SceneObject::onEndCollision( const TickContact& tickContact )
{
    // Finish if we're not gathering contacts.
    if ( !mGatherContacts )
        return;

    // Sanity!
    AssertFatal( mpCurrentContacts != NULL, "SceneObject::onBeginCollision() - Contacts not initialized correctly." );
    AssertFatal( tickContact.mpSceneObjectA == this || tickContact.mpSceneObjectB == this, "SceneObject::onEndCollision() - Contact does not involve this scene object." );

    // Remove contact.
    for( Scene::typeContactVector::iterator contactItr = mpCurrentContacts->begin(); contactItr != mpCurrentContacts->end(); ++contactItr )
    {
        // Is this is the correct contact?
        if ( contactItr->mpContact == tickContact.mpContact )
        {
            // Yes, so remove it.
            mpCurrentContacts->erase_fast( contactItr );
            return;
        }
    }
}

//-----------------------------------------------------------------------------

bool SceneObject::moveTo( const Vector2& targetWorldPoint, const F32 speed, const bool autoStop, const bool snapToTarget, const F32 margin )
{
    // Check in a scene.
    if ( !getScene() )
    {
        Con::warnf("SceneObject::moveTo() - Cannot move object (%d) to a point as it is not in a scene.", getId() );
        return false;
    }

    // Check not a static body.
    if ( getBodyType() == b2_staticBody )
    {
        Con::warnf("SceneObject::moveTo() - Cannot move object (%d) to a point as it is a static body.", getId() );
        return false;
    }

    // Check speed.
    if ( speed <= 0.0f )
    {
        Con::warnf("SceneObject::moveTo() - Speed '%f' is invalid.", speed );
        return false;
    }

    // Set the target position
    mLastCheckedPosition = getPosition();
    mTargetPosition = targetWorldPoint;
    mTargetPositionMargin = margin;
    mTargetPositionActive = true;
    mTargetPositionFound = false;
    mSnapToTargetPosition = snapToTarget;
    mStopAtTargetPosition = autoStop;
    mDistanceToTarget = (mLastCheckedPosition - mTargetPosition).Length();

    // Calculate the linear velocity for the specified speed.
    Vector2 linearVelocity = targetWorldPoint - getPosition();
    linearVelocity.Normalize(speed);

    // Set the linear velocity.
    setLinearVelocity( linearVelocity );

    return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::rotateTo( const F32 targetAngle, const F32 speed, const bool autoStop, const bool warpToTarget )
{
    // Check in a scene.
    if ( !getScene() )
    {
        Con::warnf("SceneObject::rotateTo() - Cannot rotate object (%d) to an angle as it is not in a scene.", getId() );
        return false;
    }

    // Check not a static body.
    if ( getBodyType() == b2_staticBody )
    {
        Con::warnf("SceneObject::rotateTo() - Cannot move object (%d) to an angle as it is a static body.", getId() );
        return false;
    }

    // Check speed.
    if ( speed <= 0.0f )
    {
        Con::warnf("SceneObject::rotateTo() - Speed '%f' is invalid.", speed );
        return false;
    }

    // Cancel any previous event.
    if ( mRotateToEventId != 0 )
    {
        Sim::cancelEvent( mRotateToEventId );
        mRotateToEventId = 0;
    }

    // Calculate relative angle.
    const F32 relativeAngle = targetAngle - getAngle();

    // Calculate delta angle.
    const F32 deltaAngle = mAtan( mCos( relativeAngle ), mSin( relativeAngle ) );

    // Set angular velocity.
    setAngularVelocity( deltaAngle > 0.0f ? speed : -speed );

    // Calculate the time it will take to reach the angle.
    const U32 time = (U32)(mFabs(deltaAngle / speed) * 1000.0f);

    // Create and post event.
    SceneObjectRotateToEvent* pEvent = new SceneObjectRotateToEvent( targetAngle, autoStop, warpToTarget );
    mRotateToEventId = Sim::postEvent(this, pEvent, Sim::getCurrentTime() + time );

    return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::fadeTo(const ColorF& targetColor, const F32 deltaRed, const F32 deltaGreen, const F32 deltaBlue, const F32 deltaAlpha)
{
	// Check in a scene.
	if (!getScene())
	{
		Con::warnf("SceneObject::fadeTo() - Cannot fade object (%d) to a color as it is not in a scene.", getId());
		return false;
	}

	// Check targetColor.
	if (!targetColor.isValidColor())
	{
		Con::warnf("SceneObject::fadeTo() - Cannot fade object (%d) because the color is invalid.", getId());
		return false;
	}

	// Only set fading active if the target color is not the blending color.
	if (targetColor != mBlendColor)
	{
		mFadeActive = true;
		mTargetColor = targetColor;
		mDeltaRed = deltaRed;
		mDeltaGreen = deltaGreen;
		mDeltaBlue = deltaBlue;
		mDeltaAlpha = deltaAlpha;
	}

	return true;
}

//-----------------------------------------------------------------------------

bool SceneObject::growTo(const Vector2& targetSize, const Vector2& deltaSize)
{
	// Check in a scene.
	if (!getScene())
	{
		Con::warnf("SceneObject::fadeTo() - Cannot fade object (%d) to a color as it is not in a scene.", getId());
		return false;
	}

	//only set growing active if the target size is not the current size
	if (targetSize != mSize)
	{
		mGrowActive = true;
		mTargetSize = targetSize;
		mDeltaSize = deltaSize;
	}

	return true;
}

//-----------------------------------------------------------------------------

void SceneObject::cancelMoveTo( const bool autoStop )
{
    // Only cancel an active moveTo event
    if ( mTargetPositionActive )
    {
       mTargetPositionActive = false;

        // Should we auto stop?
        if ( autoStop )
        {
            // Yes, so remove linear velocity
            setLinearVelocity( Vector2::getZero() );
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::cancelRotateTo( const bool autoStop )
{
    // Only cancel an active rotateTo event
    if ( mRotateToEventId != 0 )
    {
        Sim::cancelEvent( mRotateToEventId );
        mRotateToEventId = 0;

        // Should we auto stop?
        if ( autoStop )
        {
            // Yes, so remove angular velocity
            setAngularVelocity( 0.0f );
        }
    }
}

//-----------------------------------------------------------------------------

b2Shape::Type SceneObject::getCollisionShapeType( U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeType() - Invalid shape index." );

    if ( mpScene )
    {
        return mCollisionFixtures[shapeIndex]->GetType();
    }

    return mCollisionFixtureDefs[shapeIndex]->shape->GetType();
}

//-----------------------------------------------------------------------------

S32 SceneObject::getCollisionShapeIndex( const b2Fixture* pFixture ) const
{
    // Iterate collision shapes.
    S32 collisionShapeIndex = 0;
    for( typeCollisionFixtureVector::const_iterator collisionShapeItr = mCollisionFixtures.begin(); collisionShapeItr != mCollisionFixtures.end(); ++collisionShapeItr, ++collisionShapeIndex )
    {
        // Return index if this is the collision shape we are searching for.
        if ( pFixture == *collisionShapeItr )
            return collisionShapeIndex;
    }

    // Not found.
    return -1;
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionShapeDefinition( const U32 shapeIndex, const b2FixtureDef& fixtureDef )
{
    // We only set specific features of a fixture definition here.
    setCollisionShapeDensity( shapeIndex, fixtureDef.density );
    setCollisionShapeFriction( shapeIndex, fixtureDef.friction );
    setCollisionShapeRestitution( shapeIndex, fixtureDef.restitution );
    setCollisionShapeIsSensor( shapeIndex, fixtureDef.isSensor );
}

//-----------------------------------------------------------------------------

b2FixtureDef SceneObject::getCollisionShapeDefinition( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeDefinition() - Invalid shape index." );

    if ( mpScene )
    {       
        // Fetch fixture.
        const b2Fixture* pFixture = mCollisionFixtures[shapeIndex];

        b2FixtureDef fixtureDef;
        fixtureDef.density     = pFixture->GetDensity();
        fixtureDef.friction    = pFixture->GetFriction();
        fixtureDef.restitution = pFixture->GetRestitution();
        fixtureDef.isSensor    = pFixture->IsSensor();
        fixtureDef.filter      = pFixture->GetFilterData();
        fixtureDef.shape       = pFixture->GetShape();

        return fixtureDef;
    }

    return *mCollisionFixtureDefs[shapeIndex];
}

//-----------------------------------------------------------------------------

const b2CircleShape* SceneObject::getCollisionCircleShape( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionCircleShape() - Invalid shape index." );

    // Fetch shape definition.
    const b2FixtureDef fixtureDef = getCollisionShapeDefinition( shapeIndex );

    // Sanity!
    AssertFatal( fixtureDef.shape->GetType() == b2Shape::e_circle, "SceneObject::getCollisionCircleShape() - Shape is not a circle shape." );

    // Fetch circle shape.
    const b2CircleShape* pShape = dynamic_cast<const b2CircleShape*>( fixtureDef.shape );

    // Sanity!
    AssertFatal( pShape != NULL, "SceneObject::getCollisionCircleShape() - Invalid circle shape." );

    return pShape;
}

//-----------------------------------------------------------------------------

const b2PolygonShape* SceneObject::getCollisionPolygonShape( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionPolygonShape() - Invalid shape index." );

    // Fetch shape definition.
    const b2FixtureDef fixtureDef = getCollisionShapeDefinition( shapeIndex );

    // Sanity!
    AssertFatal( fixtureDef.shape->GetType() == b2Shape::e_polygon, "SceneObject::getCollisionPolygonShape() - Shape is not a polygon shape." );

    // Fetch shape.
    const b2PolygonShape* pShape = dynamic_cast<const b2PolygonShape*>( fixtureDef.shape );

    // Sanity!
    AssertFatal( pShape != NULL, "SceneObject::getCollisionPolygonShape() - Invalid polygon shape." );

    return pShape;
}

//-----------------------------------------------------------------------------

const b2ChainShape* SceneObject::getCollisionChainShape( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionChainShape() - Invalid shape index." );

    // Fetch shape definition.
    const b2FixtureDef fixtureDef = getCollisionShapeDefinition( shapeIndex );

    // Sanity!
    AssertFatal( fixtureDef.shape->GetType() == b2Shape::e_chain, "SceneObject::getCollisionChainShape() - Shape is not a chain shape." );

    // Fetch shape.
    const b2ChainShape* pShape = dynamic_cast<const b2ChainShape*>( fixtureDef.shape );

    // Sanity!
    AssertFatal( pShape != NULL, "SceneObject::getCollisionChainShape() - Invalid chain shape." );

    return pShape;
}

//-----------------------------------------------------------------------------

const b2EdgeShape* SceneObject::getCollisionEdgeShape( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionEdgeShape() - Invalid shape index." );

    // Fetch shape definition.
    const b2FixtureDef fixtureDef = getCollisionShapeDefinition( shapeIndex );

    // Sanity!
    AssertFatal( fixtureDef.shape->GetType() == b2Shape::e_edge, "SceneObject::getCollisionEdgeShape() - Shape is not a edge shape." );

    // Fetch shape.
    const b2EdgeShape* pShape = dynamic_cast<const b2EdgeShape*>( fixtureDef.shape );

    // Sanity!
    AssertFatal( pShape != NULL, "SceneObject::getCollisionEdgeShape() - Invalid edge shape." );

    return pShape;
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionShapeDensity( const U32 shapeIndex, const F32 density )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::setCollisionShapeDensity() - Invalid shape index." );

    if ( mpScene )
    {
        // Update live fixture.
        mCollisionFixtures[shapeIndex]->SetDensity( density );

        // Update the body mass data.
        mpBody->ResetMassData();

        return;
    }

    // Update offline fixture definition.
    mCollisionFixtureDefs[shapeIndex]->density = density;
}

//-----------------------------------------------------------------------------

F32 SceneObject::getCollisionShapeDensity( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeDensity() - Invalid shape index." );

    if ( mpScene )
    {
        // Use live fixture.
        return mCollisionFixtures[shapeIndex]->GetDensity();
    }

    // Use offline fixture definition.
    return mCollisionFixtureDefs[shapeIndex]->density;
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionShapeFriction( const U32 shapeIndex, const F32 friction )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::setCollisionShapeFriction() - Invalid shape index." );

    if ( mpScene )
    {
        // Fetch the fixture.
        b2Fixture* pFixture = mCollisionFixtures[shapeIndex];

        // Update live fixture.
        pFixture->SetFriction( friction );

        // Re-filter fixture.
        pFixture->Refilter();

        return;
    }

    // Update offline fixture definition.
    mCollisionFixtureDefs[shapeIndex]->friction = friction;
}

//-----------------------------------------------------------------------------

F32 SceneObject::getCollisionShapeFriction( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeFriction() - Invalid shape index." );

    if ( mpScene )
    {
        // Use live fixture.
        return mCollisionFixtures[shapeIndex]->GetFriction();
    }

    // Use offline fixture definition.
    return mCollisionFixtureDefs[shapeIndex]->friction;
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionShapeRestitution( const U32 shapeIndex, const F32 restitution )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::setCollisionShapeRestitution() - Invalid shape index." );

    if ( mpScene )
    {
        // Fetch the fixture.
        b2Fixture* pFixture = mCollisionFixtures[shapeIndex];

        // Update live fixture.
        pFixture->SetRestitution( restitution );

        // Re-filter fixture.
        pFixture->Refilter();

        return;
    }

    // Update offline fixture definition.
    mCollisionFixtureDefs[shapeIndex]->restitution = restitution;
}

//-----------------------------------------------------------------------------

F32 SceneObject::getCollisionShapeRestitution( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeRestitution() - Invalid shape index." );

    if ( mpScene )
    {
        // Use live fixture.
        return mCollisionFixtures[shapeIndex]->GetRestitution();
    }

    // Use offline fixture definition.
    return mCollisionFixtureDefs[shapeIndex]->restitution;
}

//-----------------------------------------------------------------------------

void SceneObject::setCollisionShapeIsSensor( const U32 shapeIndex, const bool isSensor )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::setCollisionShapeIsSensor() - Invalid shape index." );

    if ( mpScene )
    {
        // Fetch the fixture.
        b2Fixture* pFixture = mCollisionFixtures[shapeIndex];

        // Update live fixture.
        pFixture->SetSensor( isSensor );

        // Re-filter fixture.
        pFixture->Refilter();

        return;
    }

    // Update offline fixture definition.
    mCollisionFixtureDefs[shapeIndex]->isSensor = isSensor;
}

//-----------------------------------------------------------------------------

bool SceneObject::getCollisionShapeIsSensor( const U32 shapeIndex ) const
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::getCollisionShapeIsSensor() - Invalid shape index." );

    if ( mpScene )
    {
        // Use live fixture.
        return mCollisionFixtures[shapeIndex]->IsSensor();
    }

    // Use offline fixture definition.
    return mCollisionFixtureDefs[shapeIndex]->isSensor;
}

//-----------------------------------------------------------------------------

void SceneObject::deleteCollisionShape( const U32 shapeIndex )
{
    // Sanity!
    AssertFatal( shapeIndex < getCollisionShapeCount(), "SceneObject::deleteCollisionShape() - Invalid shape index." );

    if ( mpScene )
    {
        mpBody->DestroyFixture( mCollisionFixtures[ shapeIndex ] );
        mCollisionFixtures.erase_fast( shapeIndex );
        return;
    }

    mCollisionFixtureDefs.erase_fast( shapeIndex );
}

//-----------------------------------------------------------------------------

void SceneObject::clearCollisionShapes( void )
{
    // Delete all collision shapes.
    while ( getCollisionShapeCount() > 0 )
    {
        deleteCollisionShape( 0 );
    }
}

//-----------------------------------------------------------------------------

S32 SceneObject::createCircleCollisionShape( const F32 radius )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreateCircleCollisionShape);

    return createCircleCollisionShape( radius, b2Vec2(0.0f, 0.0f) );
}

//-----------------------------------------------------------------------------

S32 SceneObject::createCircleCollisionShape( const F32 radius, const b2Vec2& localPosition )
{
    // Sanity!
    if ( radius <= 0.0f )
    {
        Con::errorf("SceneObject::createCircleCollisionShape() - Invalid radius of %g.", radius);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2CircleShape* pShape = new b2CircleShape();
    pShape->m_p = localPosition;
    pShape->m_radius = radius;
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

F32 SceneObject::getCircleCollisionShapeRadius( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2CircleShape* pShape = getCollisionCircleShape( shapeIndex );

    return pShape->m_radius;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getCircleCollisionShapeLocalPosition( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2CircleShape* pShape = getCollisionCircleShape( shapeIndex );

    return pShape->m_p;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createPolygonCollisionShape( const U32 pointCount, const b2Vec2* localPoints )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreatePolygonCollisionShape);

    // Sanity!
    if ( pointCount < 3 || pointCount > b2_maxPolygonVertices )
    {
        Con::errorf("SceneObject::createPolygonCollisionShape() - Invalid point count of %d.", pointCount);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2PolygonShape* pShape    = new b2PolygonShape();
    pShape->Set( localPoints, pointCount );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createPolygonBoxCollisionShape( const F32 width, const F32 height )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreatePolygonBoxCollisionShapeWidthHeight);

    // Sanity!
    if ( width <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid width of %g.", width);
        return -1;
    }
    if ( height <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid height of %g.", height);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2PolygonShape* pShape    = new b2PolygonShape();
    pShape->SetAsBox( width * 0.5f, height * 0.5f );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createPolygonBoxCollisionShape( const F32 width, const F32 height, const b2Vec2& localCentroid )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreatePolygonBoxCollisionShapeWidthHeightCentroid);

    // Sanity!
    if ( width <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid width of %g.", width);
        return -1;
    }
    if ( height <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid height of %g.", height);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2PolygonShape* pShape    = new b2PolygonShape();
    pShape->SetAsBox( width * 0.5f, height * 0.5f, localCentroid, 0.0f );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createPolygonBoxCollisionShape( const F32 width, const F32 height, const b2Vec2& localCentroid, const F32 rotation )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreatePolygonBoxCollisionShapeWidthHeightCentroidRotation);

    // Sanity!
    if ( width <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid width of %g.", width);
        return -1;
    }
    if ( height <= 0.0f )
    {
        Con::errorf("SceneObject::createPolygonBoxCollisionShape() - Invalid height of %g.", height);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2PolygonShape* pShape    = new b2PolygonShape();
    pShape->SetAsBox( width * 0.5f, height * 0.5f, localCentroid, rotation );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

U32 SceneObject::getPolygonCollisionShapePointCount( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2PolygonShape* pShape = getCollisionPolygonShape( shapeIndex );

    return pShape->GetVertexCount();
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getPolygonCollisionShapeLocalPoint( const U32 shapeIndex, const U32 pointIndex ) const
{
    // Fetch shape.
    const b2PolygonShape* pShape = getCollisionPolygonShape( shapeIndex );

    // Sanity!
    AssertFatal( pointIndex < (U32)pShape->GetVertexCount(), "SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid local point index." );

    return pShape->GetVertex( pointIndex );
}

//-----------------------------------------------------------------------------

S32 SceneObject::createChainCollisionShape( const U32 pointCount, const b2Vec2* localPoints )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreateChainCollisionShape);

    // Sanity!
    if ( pointCount < 2 )
    {
        Con::errorf("SceneObject::createChainCollisionShape() - Invalid point count of %d.", pointCount);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2ChainShape* pShape      = new b2ChainShape();
    pShape->CreateChain( localPoints, pointCount );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createChainCollisionShape(  const U32 pointCount, const b2Vec2* localPoints,
                                                const bool hasAdjacentLocalPositionStart, const bool hasAdjacentLocalPositionEnd,
                                                const b2Vec2& adjacentLocalPositionStart, const b2Vec2& adjacentLocalPositionEnd )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreateChainCollisionShapeAdjacents);

    // Sanity!
    if ( pointCount < 2 )
    {
        Con::errorf("SceneObject::createChainCollisionShape() - Invalid point count of %d.", pointCount);
        return -1;
    }

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2ChainShape* pShape      = new b2ChainShape();
    pShape->CreateChain( localPoints, pointCount );
    
    if ( hasAdjacentLocalPositionStart )
        pShape->SetPrevVertex( adjacentLocalPositionStart );

    if ( hasAdjacentLocalPositionEnd )
        pShape->SetNextVertex( adjacentLocalPositionEnd );

    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

U32 SceneObject::getChainCollisionShapePointCount( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    return pShape->m_count;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getChainCollisionShapeLocalPoint( const U32 shapeIndex, const U32 pointIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    // Sanity!
    AssertFatal( pointIndex < (U32)pShape->m_count, "SceneObject::getChainCollisionShapeLocalPoint() - Invalid local point index." );

    return pShape->m_vertices[ pointIndex ];
}

//-----------------------------------------------------------------------------

bool SceneObject::getChainCollisionShapeHasAdjacentStart( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    return pShape->m_hasPrevVertex;
}

//-----------------------------------------------------------------------------

bool SceneObject::getChainCollisionShapeHasAdjacentEnd( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    return pShape->m_hasNextVertex;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getChainCollisionShapeAdjacentStart( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    return pShape->m_prevVertex;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getChainCollisionShapeAdjacentEnd( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = getCollisionChainShape( shapeIndex );

    return pShape->m_nextVertex;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createEdgeCollisionShape( const b2Vec2& localPositionStart, const b2Vec2& localPositionEnd )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreateEdgeCollisionShape);

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2EdgeShape* pShape       = new b2EdgeShape();
    pShape->Set( localPositionStart, localPositionEnd );
    pFixtureDef->shape = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

S32 SceneObject::createEdgeCollisionShape(   const b2Vec2& localPositionStart, const b2Vec2& localPositionEnd,
                                                const bool hasAdjacentLocalPositionStart, const bool hasAdjacentLocalPositionEnd,
                                                const b2Vec2& adjacentLocalPositionStart, const b2Vec2& adjacentLocalPositionEnd )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CreateEdgeCollisionShapeAdjacents);

    // Configure fixture definition.
    b2FixtureDef* pFixtureDef = new b2FixtureDef( mDefaultFixture );
    b2EdgeShape* pShape       = new b2EdgeShape();
    pShape->Set( localPositionStart, localPositionEnd );
    pShape->m_hasVertex0      = hasAdjacentLocalPositionStart;
    pShape->m_hasVertex3      = hasAdjacentLocalPositionEnd;
    pShape->m_vertex0         = adjacentLocalPositionStart;
    pShape->m_vertex3         = adjacentLocalPositionEnd;
    pFixtureDef->shape        = pShape;

    if ( mpScene )
    {
        // Create and push fixture.
        mCollisionFixtures.push_back( mpBody->CreateFixture( pFixtureDef ) );

        // Destroy shape and fixture.
        delete pShape;
        delete pFixtureDef;

        return mCollisionFixtures.size()-1;
    }

    // Push fixture definition.
    mCollisionFixtureDefs.push_back( pFixtureDef );

    return mCollisionFixtureDefs.size()-1;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getEdgeCollisionShapeLocalPositionStart( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_vertex1;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getEdgeCollisionShapeLocalPositionEnd( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_vertex2;
}

//-----------------------------------------------------------------------------

bool SceneObject::getEdgeCollisionShapeHasAdjacentStart( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_hasVertex0;
}

//-----------------------------------------------------------------------------

bool SceneObject::getEdgeCollisionShapeHasAdjacentEnd( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_hasVertex3;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getEdgeCollisionShapeAdjacentStart( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_vertex0;
}

//-----------------------------------------------------------------------------

Vector2 SceneObject::getEdgeCollisionShapeAdjacentEnd( const U32 shapeIndex ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = getCollisionEdgeShape( shapeIndex );

    return pShape->m_vertex3;
}

//-----------------------------------------------------------------------------

void SceneObject::setBlendOptions( void )
{
    // Set Blend Status.
    if ( mBlendMode )
    {
        // Enable Blending.
        glEnable( GL_BLEND );
        // Set Blend Function.
        glBlendFunc( mSrcBlendFactor, mDstBlendFactor );

        // Set color.
        glColor4f(mBlendColor.red,mBlendColor.green,mBlendColor.blue,mBlendColor.alpha );
    }
    else
    {
        // Disable Blending.
        glDisable( GL_BLEND );
        // Reset color.
        glColor4f(1,1,1,1);
    }

    // Set Alpha Test.
    if ( mAlphaTest >= 0.0f )
    {
        // Enable Test.
        glEnable( GL_ALPHA_TEST );
        glAlphaFunc( GL_GREATER, mAlphaTest );
    }
    else
    {
        // Disable Test.
        glDisable( GL_ALPHA_TEST );
    }
}

//-----------------------------------------------------------------------------

void SceneObject::resetBlendOptions( void )
{
    // Disable Blending.
    glDisable( GL_BLEND );

    glDisable( GL_ALPHA_TEST);

    // Reset color.
    glColor4f(1,1,1,1);
}

//---------------------------------------------------------------------------------------------

void SceneObject::onInputEvent( StringTableEntry name, const GuiEvent& event, const Vector2& worldMousePosition )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_OnInputEvent);

    // Argument Buffers.
    char argBuffer[3][32];

    // ID
    dSprintf(argBuffer[0], 32, "%d", event.eventID);
    
    // Format Mouse-Position Buffer.
    dSprintf(argBuffer[1], 32, "%g %g", worldMousePosition.x, worldMousePosition.y);

    // Optional double click
    dSprintf(argBuffer[2], 32, "%d", event.mouseClickCount);

    // Call Scripts.
    Con::executef(this, 4, name, argBuffer[0], argBuffer[1], argBuffer[2]);
}

//-----------------------------------------------------------------------------

void SceneObject::attachGui(GuiControl* pGuiControl, SceneWindow* pSceneWindow, const bool sizeControl, const Vector2 offset)
{
    // Attach GUI control.
    SceneObjectAttachedGUI attachedGui;
    attachedGui.mpAttachedCtrl = pGuiControl;
    attachedGui.mpAttachedSceneWindow = pSceneWindow;
    attachedGui.mAutoSize = sizeControl;
    attachedGui.mAttachedOffset = offset;

    // Register GUI control & window references.
    attachedGui.mpAttachedCtrl->registerReference((SimObject**)&attachedGui.mpAttachedCtrl);
    attachedGui.mpAttachedSceneWindow->registerReference((SimObject**)&attachedGui.mpAttachedSceneWindow);

    // Check & adjust GUI parentage.
    if (attachedGui.mpAttachedCtrl->getParent() != attachedGui.mpAttachedSceneWindow)
    {
        // Warn user & remove the GuiControl from the existing parent (if it has one).
        if (attachedGui.mpAttachedCtrl->getParent())
        {
            Con::warnf("Warning: SceneObject::attachGui - GuiControl already has a parent GuiWindow!");
            attachedGui.mpAttachedCtrl->getParent()->removeObject(attachedGui.mpAttachedCtrl);
        }

        // Add the GuiControl to the scene window.
        attachedGui.mpAttachedSceneWindow->addObject(attachedGui.mpAttachedCtrl);
    }

    mAttachedCtrls.push_back(attachedGui);
}
//-----------------------------------------------------------------------------

void SceneObject::detachGui(void)
{
    // Remove the last attached GUI.
    if (mAttachedCtrls.size() > 0)
    {
        mAttachedCtrls.last().mpAttachedCtrl->unregisterReference((SimObject**)&mAttachedCtrls.last().mpAttachedCtrl);
        mAttachedCtrls.last().mpAttachedSceneWindow->unregisterReference((SimObject**)&mAttachedCtrls.last().mpAttachedSceneWindow);

        mAttachedCtrls.pop_back();
    }
}

void SceneObject::detachGui(GuiControl* pGuiControl)
{
    Vector<SceneObjectAttachedGUI>::iterator i;
    for (i = mAttachedCtrls.begin(); i != mAttachedCtrls.end(); i++)
    {
        if (i->mpAttachedCtrl == pGuiControl)
        {
            // Remove the attached GuiControl.
            i->mpAttachedCtrl->unregisterReference((SimObject**)&i->mpAttachedCtrl);
            i->mpAttachedSceneWindow->unregisterReference((SimObject**)&i->mpAttachedSceneWindow);

            mAttachedCtrls.pop_back();

            return;
        }
    }

    // Warn user that no GuiControls were found.
    Con::warnf("Warning: SceneObject::detachGui() - The GuiControl was not found!");
}

//-----------------------------------------------------------------------------

void SceneObject::detachAllGuiControls(void)
{
    Vector<SceneObjectAttachedGUI>::iterator i;
    for (i = mAttachedCtrls.begin(); i != mAttachedCtrls.end(); i++)
    {
        // Remove the attached GuiControl.
        i->mpAttachedCtrl->unregisterReference((SimObject**)&i->mpAttachedCtrl);
        i->mpAttachedSceneWindow->unregisterReference((SimObject**)&i->mpAttachedSceneWindow);
    }

    // Clear all references to the attached GuiControls.
    mAttachedCtrls.clear();
}

//-----------------------------------------------------------------------------
void SceneObject::updateAttachedGui(void)
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_updateAttachedGui);

    // Early-out if no GUIs are attached.
    if (mAttachedCtrls.size() == 0)
        return;

    Vector<SceneObjectAttachedGUI>::iterator i;
    for (i = mAttachedCtrls.begin(); i != mAttachedCtrls.end(); i++)
    {
        // Ignore if we're not in the scene that the GUI is attached to.
        if (getScene() != i->mpAttachedSceneWindow->getScene())
        {
            // Warn.
            Con::warnf("Warning: SceneObject::updateAttachedGui() - SceneWindow is not attached to the Scene!");

            // Detach the control.
            detachGui(i->mpAttachedCtrl);

            return;
        }

        // Calculate the GUI Controls' dimensions.
        Point2I topLeftI, extentI;

        // Size Control?
        if (i->mAutoSize)
        {
            // Yes, so fetch Clip Rectangle; this forms the area we want to fix the Gui-Control to.
            const RectF objAABB = getAABBRectangle();
            // Fetch Top-Left.
            Vector2 upperLeft = Vector2(objAABB.point.x, objAABB.point.y + objAABB.extent.y);
            Vector2 lowerRight = Vector2(objAABB.point.x + objAABB.extent.x, objAABB.point.y);

            // Convert Scene to Window Coordinates.
            i->mpAttachedSceneWindow->sceneToWindowPoint(upperLeft, upperLeft);
            i->mpAttachedSceneWindow->sceneToWindowPoint(lowerRight, lowerRight);

            // Convert Control Dimensions.
            topLeftI.set(S32(upperLeft.x), S32(upperLeft.y));
            extentI.set(S32(lowerRight.x - upperLeft.x), S32(lowerRight.y - upperLeft.y));

            // Add offset
            topLeftI.x += static_cast<S32>(i->mAttachedOffset.x);
            topLeftI.y += static_cast<S32>(i->mAttachedOffset.y);
        }
        else
        {
            // No, so center GUI-Control on objects position but don't resize it.

            // Calculate Position from World Clip.
            const RectF clipRectangle = getAABBRectangle();
            // Calculate center position.
            const Vector2 centerPosition = clipRectangle.point + Vector2(clipRectangle.len_x()*0.5f, clipRectangle.len_y()*0.5f);

            // Convert Scene to Window Coordinates.
            Vector2 positionI;
            i->mpAttachedSceneWindow->sceneToWindowPoint(centerPosition, positionI);

            // Fetch Control Extents (which don't change here).
            extentI = i->mpAttachedCtrl->getExtent();

            // Calculate new top-left.
            topLeftI.set(S32(positionI.x - extentI.x / 2), S32(positionI.y - extentI.y / 2));

            // Add offset
            topLeftI.x += static_cast<S32>(i->mAttachedOffset.x);
            topLeftI.y += static_cast<S32>(i->mAttachedOffset.y);
        }

        // Set Control Dimensions.
        i->mpAttachedCtrl->resize(topLeftI, extentI);
    }
}

//-----------------------------------------------------------------------------

void SceneObject::copyFrom( SceneObject* pSceneObject, const bool copyDynamicFields )
{
    // Copy the specified object.
    pSceneObject->copyTo( this );

    // Copy over dynamic fields if requested.
    if ( copyDynamicFields )
        pSceneObject->assignDynamicFieldsFrom( this );
}

//-----------------------------------------------------------------------------

void SceneObject::copyTo( SimObject* obj )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_CopyTo);

    // Call parent.
    Parent::copyTo(obj);

    // Fetch object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>(obj);

    // Sanity!
    AssertFatal(pSceneObject != NULL, "SceneObject::copyTo() - Object is not the correct type.");

    /// Lifetime.
    pSceneObject->setLifetime( getLifetime() );

    /// Scene Layers.
    pSceneObject->setSceneLayer( getSceneLayer() );

    /// Scene groups.
    pSceneObject->setSceneGroup( getSceneGroup() );

    /// Area.
    pSceneObject->setSize( getSize() );

    /// Position / Angle.
    pSceneObject->setPosition( getPosition() );
    pSceneObject->setAngle( getAngle() );
    pSceneObject->setFixedAngle( getFixedAngle() );

    /// Body.
    pSceneObject->setBodyType( getBodyType() );
    pSceneObject->setActive( getActive() );
    pSceneObject->setAwake( getAwake() );
    pSceneObject->setBullet( getBullet() );
    pSceneObject->setSleepingAllowed( getSleepingAllowed() );

    /// Collision control.
    pSceneObject->setCollisionGroupMask( getCollisionGroupMask() );
    pSceneObject->setCollisionLayerMask( getCollisionLayerMask() );
    pSceneObject->setCollisionSuppress( getCollisionSuppress() );
    pSceneObject->setCollisionOneWay( getCollisionOneWay() );
    pSceneObject->setGatherContacts( getGatherContacts() );
    pSceneObject->setDefaultDensity( getDefaultDensity() );
    pSceneObject->setDefaultFriction( getDefaultFriction() );
    pSceneObject->setDefaultRestitution( getDefaultRestitution() );

    /// Velocities.
    pSceneObject->setLinearVelocity( getLinearVelocity() );
    pSceneObject->setAngularVelocity( getAngularVelocity() );
    pSceneObject->setLinearDamping( getLinearDamping() );
    pSceneObject->setAngularDamping( getAngularDamping() );

    /// Gravity scaling.
    pSceneObject->setGravityScale( getGravityScale() );

    /// Collision shapes.
    copyCollisionShapes( pSceneObject, true );

    /// Render visibility.
    pSceneObject->setVisible( getVisible() );

    /// Render blending.
    pSceneObject->setBlendMode( getBlendMode() );
    pSceneObject->setSrcBlendFactor( getSrcBlendFactor() );
    pSceneObject->setDstBlendFactor( getDstBlendFactor() );
    pSceneObject->setBlendColor( getBlendColor() );
    pSceneObject->setAlphaTest( getAlphaTest() );

    /// Render sorting.
    pSceneObject->setSortPoint( getSortPoint() );

    /// Input events.
    pSceneObject->setUseInputEvents( getUseInputEvents() );

    // Script callbacks.
    pSceneObject->setUpdateCallback( getUpdateCallback() );   
    pSceneObject->setCollisionCallback( getCollisionCallback() );
    pSceneObject->setSleepingCallback( getSleepingCallback() );

    /// Misc.
    pSceneObject->setBatchIsolated( getBatchIsolated() );
   
    /// Debug mode.
    setDebugOn( getDebugMask() );
}

//-----------------------------------------------------------------------------

S32 SceneObject::copyCollisionShapes( SceneObject* pSceneObject, const bool clearTargetShapes, const S32 shapeIndex )
{
    // Sanity!
    AssertFatal( pSceneObject != NULL, "SceneObject::copyCollisionShapes() - Cannot copy to a NULL scene object." );

    // Clear the collision shapes.
    if ( clearTargetShapes )
        pSceneObject->clearCollisionShapes();

    // Fetch collision shape count.
    const U32 collisionShapeCount = getCollisionShapeCount();

    // If a shape index is specified, is it valid?
    if ( shapeIndex != INVALID_COLLISION_SHAPE_INDEX && shapeIndex >= (S32)collisionShapeCount )
    {
        // No, so warn.
        Con::warnf( "SceneObject::copyCollisionShapes() - Invalid shape index '%d'.", shapeIndex );
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Finish if there are no collision shapes.
    if ( collisionShapeCount == 0 )
        return INVALID_COLLISION_SHAPE_INDEX;

    // Calculate shape range.
    const U32 startShapeIndex = shapeIndex >= 0 ? shapeIndex : 0;
    const U32 endShapeIndex = shapeIndex >= 0 ? shapeIndex : collisionShapeCount -1;

    // Iterate collision shapes.
    for ( U32 index = startShapeIndex; index <= endShapeIndex; ++index )
    {
        b2FixtureDef fixtureDef;

        if ( mpScene )
        {
            // Fetch fixture.
            b2Fixture* pFixture = mCollisionFixtures[index];

            // Fetch common details.
            fixtureDef.density     = pFixture->GetDensity();
            fixtureDef.friction    = pFixture->GetFriction();
            fixtureDef.restitution = pFixture->GetRestitution();
            fixtureDef.isSensor    = pFixture->IsSensor();
            fixtureDef.shape       = pFixture->GetShape();
        }
        else
        {
            // Fetch fixture def.
            b2FixtureDef* pFixtureDef = mCollisionFixtureDefs[index];

            // Fetch common details.
            fixtureDef = *pFixtureDef;
        }

        S32 newShapeIndex;

        // Fetch shape type.
        const b2Shape::Type shapeType = fixtureDef.shape->GetType();

        // Copy appropriate shape type.
        switch( shapeType )
        {
            case b2Shape::e_circle:
                newShapeIndex = copyCircleCollisionShapeTo( pSceneObject, fixtureDef );

                // Return the new shape if we're copying a specific index.
                if ( shapeIndex >= 0 )
                    return newShapeIndex;

                continue;

            case b2Shape::e_polygon:
                newShapeIndex = copyPolygonCollisionShapeTo( pSceneObject, fixtureDef );

                // Return the new shape if we're copying a specific index.
                if ( shapeIndex >= 0 )
                    return newShapeIndex;

                continue;

            case b2Shape::e_chain:
                newShapeIndex = copyChainCollisionShapeTo( pSceneObject, fixtureDef );

                // Return the new shape if we're copying a specific index.
                if ( shapeIndex >= 0 )
                    return newShapeIndex;

                continue;

            case b2Shape::e_edge:
                newShapeIndex = copyEdgeCollisionShapeTo( pSceneObject, fixtureDef );

                // Return the new shape if we're copying a specific index.
                if ( shapeIndex >= 0 )
                    return newShapeIndex;

                continue;

            default:
                AssertFatal( false, "SceneObject::copyCollisionShapes() - Unsupported collision shape type encountered." );
        }
    }

    // Return the first index if we're copying all the shapes.
    if ( shapeIndex < 0 )
        return 0;

    return INVALID_COLLISION_SHAPE_INDEX;
}

//-----------------------------------------------------------------------------

S32 SceneObject::copyCircleCollisionShapeTo( SceneObject* pSceneObject, const b2FixtureDef& fixtureDef ) const
{
    // Fetch shape.
    const b2CircleShape* pShape = dynamic_cast<const b2CircleShape*>( fixtureDef.shape );

    // Check shape.
    if ( !pShape )
    {
        Con::errorf("SceneObject::copyCircleCollisionShapeTo() - Invalid shape.");
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch shape details.
    const F32 radius           = pShape->m_radius;
    const b2Vec2 localPosition = pShape->m_p;

    // Copy shape.
    const S32 shapeIndex = pSceneObject->createCircleCollisionShape( radius, localPosition );

    // Was shape created.
    if ( shapeIndex != -1 )
    {
        // Yes, so configure shape.
        pSceneObject->setCollisionShapeDefinition( shapeIndex, fixtureDef );
    }

    return shapeIndex;
}

//-----------------------------------------------------------------------------

S32 SceneObject::copyPolygonCollisionShapeTo( SceneObject* pSceneObject, const b2FixtureDef& fixtureDef ) const
{
    // Fetch shape.
    const b2PolygonShape* pShape = dynamic_cast<const b2PolygonShape*>( fixtureDef.shape );

    // Check shape.
    if ( !pShape )
    {
        Con::errorf("SceneObject::copyPolygonCollisionShapeTo() - Invalid shape.");
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch point count.
    const U32 pointCount = pShape->GetVertexCount();

    // Fetch local points.
    const b2Vec2* plocalPoints = pShape->m_vertices;

    // Copy shape.
    const S32 shapeIndex = pSceneObject->createPolygonCollisionShape( pointCount, plocalPoints );

    // Was shape created.
    if ( shapeIndex != -1 )
    {
        // Yes, so configure shape.
        pSceneObject->setCollisionShapeDefinition( shapeIndex, fixtureDef );
    }

    return shapeIndex;
}

//-----------------------------------------------------------------------------

S32 SceneObject::copyChainCollisionShapeTo( SceneObject* pSceneObject, const b2FixtureDef& fixtureDef ) const
{
    // Fetch shape.
    const b2ChainShape* pShape = dynamic_cast<const b2ChainShape*>( fixtureDef.shape );

    // Check shape.
    if ( !pShape )
    {
        Con::errorf("SceneObject::copyChainCollisionShapeTo() - Invalid shape.");
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch point count.
    const U32 pointCount = pShape->m_count;

    // Fetch local points.
    b2Vec2* localPoints = pShape->m_vertices;

    // Fetch adjacent positions.
    const bool hasAdjacentLocalPositionStart = pShape->m_hasPrevVertex;
    const bool hasAdjacentLocalPositionEnd   = pShape->m_hasNextVertex;
    const b2Vec2 adjacentLocalPositionStart  = pShape->m_prevVertex;
    const b2Vec2 adjacentLocalPositionEnd    = pShape->m_nextVertex;

    // Create shape.
    const S32 shapeIndex = pSceneObject->createChainCollisionShape(
        pointCount, localPoints,
        hasAdjacentLocalPositionStart, hasAdjacentLocalPositionEnd,
        adjacentLocalPositionStart, adjacentLocalPositionEnd);

    // Was shape created.
    if ( shapeIndex != -1 )
    {
        // Yes, so configure shape.
        pSceneObject->setCollisionShapeDefinition( shapeIndex, fixtureDef );
    }

    return shapeIndex;
}

//-----------------------------------------------------------------------------

S32 SceneObject::copyEdgeCollisionShapeTo( SceneObject* pSceneObject, const b2FixtureDef& fixtureDef ) const
{
    // Fetch shape.
    const b2EdgeShape* pShape = dynamic_cast<const b2EdgeShape*>( fixtureDef.shape );

    // Check shape.
    if ( !pShape )
    {
        Con::errorf("SceneObject::copyEdgeCollisionShapeTo() - Invalid shape.");
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch positions.
    const b2Vec2 localPosition1          = pShape->m_vertex1;
    const b2Vec2 localPosition2          = pShape->m_vertex2;
    const bool hasAdjacentLocalPosition1 = pShape->m_hasVertex0;
    const bool hasAdjacentLocalPosition2 = pShape->m_hasVertex3;
    const b2Vec2 adjacentLocalPosition1  = pShape->m_vertex0;
    const b2Vec2 adjacentLocalPosition2  = pShape->m_vertex3;

    // Create shape.
    const S32 shapeIndex = pSceneObject->createEdgeCollisionShape(
        localPosition1, localPosition2,
        hasAdjacentLocalPosition1, hasAdjacentLocalPosition2,
        adjacentLocalPosition1, adjacentLocalPosition2 );

    // Was shape created.
    if ( shapeIndex != -1 )
    {
        // Yes, so configure shape.
        pSceneObject->setCollisionShapeDefinition( shapeIndex, fixtureDef );
    }

    return shapeIndex;
}

//-----------------------------------------------------------------------------

void SceneObject::safeDelete( void )
{
    // Are we in a scene?
    if ( getScene() )
    {
        // Yes, so add a delete-request to the scene.
        getScene()->addDeleteRequest( this );
    }
    else
    {
        // No, so use standard SimObject helper.
        deleteObject();
    }
}

//-----------------------------------------------------------------------------

void SceneObject::addDestroyNotification( SceneObject* pSceneObject )
{
    // Search list to see if we're already in it (finish if we are).
    for ( U32 n = 0; n < (U32)mDestroyNotifyList.size(); n++ )
    {
        // In the list already?
        if ( mDestroyNotifyList[n].mpSceneObject == pSceneObject )
        {
            // Yes, so just bump-up the reference count.
            mDestroyNotifyList[n].mRefCount++;

            // Finish here.
            return;
        }
    }

    // Add Destroy Notification.
    tDestroyNotification notification;
    notification.mpSceneObject = pSceneObject;
    notification.mRefCount = 1;

    // Add Notification.
    mDestroyNotifyList.push_back( notification );
}

//-----------------------------------------------------------------------------

void SceneObject::removeDestroyNotification( SceneObject* pSceneObject )
{
    // Find object in notification list.
    for ( U32 n = 0; n < (U32)mDestroyNotifyList.size(); n++ )
    {
        // Our object?
        if ( mDestroyNotifyList[n].mpSceneObject == pSceneObject )
        {
            // Yes, so reduce reference count.
            mDestroyNotifyList[n].mRefCount--;
            // Finish Here.
            return;
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::processDestroyNotifications( void )
{
    // Find object in notification list.
    while( mDestroyNotifyList.size() )
    {
        // Fetch Notification Item.
        tDestroyNotification notification = mDestroyNotifyList.first();
        // Only action if we've got a reference active.
        if ( notification.mRefCount > 0 )
            // Call Destroy Notification.
            notification.mpSceneObject->onDestroyNotify( this );

        // Remove it.
        mDestroyNotifyList.pop_front();
    }

    // Sanity!
    AssertFatal( mDestroyNotifyList.size() == 0, "SceneObject::processDestroyNotifications() - Notifications still pending!" );
}

//-----------------------------------------------------------------------------

void SceneObject::notifyComponentsAddToScene( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_NotifyComponentsAddToScene);

    // Notify components.
    VectorPtr<SimComponent*>& componentList = lockComponentList();
    for( SimComponentIterator itr = componentList.begin(); itr != componentList.end(); ++itr )
    {
        SimComponent *pComponent = *itr;
        if( pComponent != NULL )
            pComponent->onAddToScene();
    }
    unlockComponentList();
}

//-----------------------------------------------------------------------------

void SceneObject::notifyComponentsRemoveFromScene( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_NotifyComponentsRemoveFromScene);

    // Notify components.
    VectorPtr<SimComponent*>& componentList = lockComponentList();
    for( SimComponentIterator itr = componentList.begin(); itr != componentList.end(); ++itr )
    {
        SimComponent *pComponent = *itr;
        if( pComponent != NULL )
            pComponent->onRemoveFromScene();
    }
    unlockComponentList();
}

//-----------------------------------------------------------------------------

void SceneObject::notifyComponentsUpdate( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_NotifyComponentsUpdate);

    // Notify components.
    VectorPtr<SimComponent*>& componentList = lockComponentList();
    for( SimComponentIterator itr = componentList.begin(); itr != componentList.end(); ++itr )
    {
        SimComponent *pComponent = *itr;
        if( pComponent != NULL )
            pComponent->onUpdate();
    }
    unlockComponentList();
}

//-----------------------------------------------------------------------------

U32 SceneObject::getGlobalSceneObjectCount( void )
{
    return sGlobalSceneObjectCount;
}

//-----------------------------------------------------------------------------

void SceneObject::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_OnTamlCustomWrite);

    // Call parent.
    Parent::onTamlCustomWrite( customNodes );

    // Fetch collision shape count.
    const U32 collisionShapeCount = getCollisionShapeCount();

    // Finish if no collision shapes.
    if ( collisionShapeCount == 0 )
        return;

    // Add collision shape node.
    TamlCustomNode* pCustomCollisionShapes = customNodes.addNode( shapeCustomNodeName );

    // Iterate collision shapes.
    for ( U32 shapeIndex = 0; shapeIndex < collisionShapeCount; ++shapeIndex )
    {
        // Fetch collision shape definition.
        b2FixtureDef fixtureDef = getCollisionShapeDefinition( shapeIndex );

        // Add collision shape node.
        // NOTE:    The name of the node will get updated shortly.
        TamlCustomNode* pCollisionShapeNode = pCustomCollisionShapes->addNode( StringTable->EmptyString );

        // Add common collision shape fields.
        if ( mNotEqual( getDefaultDensity(), fixtureDef.density ) )
            pCollisionShapeNode->addField( shapeDensityName, fixtureDef.density );

        if ( mNotEqual( getDefaultFriction(), fixtureDef.friction ) )
            pCollisionShapeNode->addField( shapeFrictionName, fixtureDef.friction );

        if ( mNotEqual( getDefaultRestitution(), fixtureDef.restitution ) )
            pCollisionShapeNode->addField( shapeRestitutionName, fixtureDef.restitution );

        if ( fixtureDef.isSensor == true )
            pCollisionShapeNode->addField( shapeSensorName, fixtureDef.isSensor );

        // Populate collision shape appropriately.
        switch( fixtureDef.shape->GetType() )
        {
        case b2Shape::e_circle:
            {
                // Set node name.
                pCollisionShapeNode->setNodeName( circleTypeName );

                // Fetch shape.
                const b2CircleShape* pShape = dynamic_cast<const b2CircleShape*>( fixtureDef.shape );

                // Sanity!
                AssertFatal( pShape != NULL, "SceneObject::onTamlCustomWrite() - Invalid circle shape type returned." );

                // Add radius property.
                pCollisionShapeNode->addField( circleRadiusName, pShape->m_radius );

                // Add offset property (if not zero).
                if ( !Vector2(pShape->m_p).isZero() )
                    pCollisionShapeNode->addField( circleOffsetName, pShape->m_p );
            }
            break;

        case b2Shape::e_polygon:
            {
                // Set node name.
                pCollisionShapeNode->setNodeName( polygonTypeName );

                // Fetch shape.
                const b2PolygonShape* pShape = dynamic_cast<const b2PolygonShape*>( fixtureDef.shape );

                // Sanity!
                AssertFatal( pShape != NULL, "SceneObject::onTamlCustomWrite() - Invalid polygon shape type returned." );

                // Fetch point count.
                const U32 pointCount = pShape->GetVertexCount();

                // Add shape properties.
                for ( U32 pointIndex = 0; pointIndex < pointCount; ++pointIndex )
                {
                    // Add point node.
                    TamlCustomNode* pPointNode = pCollisionShapeNode->addNode( shapePointName );

                    // Fetch point.
                    const b2Vec2& point = pShape->GetVertex( pointIndex );

                    // Add point fields.
                    pPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, point );
                }
            }
            break;

        case b2Shape::e_chain:
            {
                // Set node name.
                pCollisionShapeNode->setNodeName( chainTypeName );

                // Fetch shape.
                const b2ChainShape* pShape = dynamic_cast<const b2ChainShape*>( fixtureDef.shape );

                // Sanity!
                AssertFatal( pShape != NULL, "SceneObject::onTamlCustomWrite() - Invalid chain shape type returned." );

                // Fetch point count.
                const U32 pointCount = pShape->m_count;

                // Add shape properties.
                for ( U32 pointIndex = 0; pointIndex < pointCount; ++pointIndex )
                {
                    // Add point node.
                    TamlCustomNode* pPointNode = pCollisionShapeNode->addNode( shapePointName );

                    // Add point fields.
                    pPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_vertices[pointIndex] );
                }

                // Add adjacent start point (if specified).
                if ( pShape->m_hasPrevVertex )
                {
                    TamlCustomNode* pPrevPointNode = pCollisionShapeNode->addNode( shapePrevPointName );
                    pPrevPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_prevVertex );
                }

                // Add adjacent end point (if specified).
                if ( pShape->m_hasNextVertex )
                {
                    TamlCustomNode* pNextPointNode = pCollisionShapeNode->addNode( shapeNextPointName );
                    pNextPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_nextVertex );
                }
            }
            break;

        case b2Shape::e_edge:
            {
                // Set node name.
                pCollisionShapeNode->setNodeName( edgeTypeName );

                // Fetch shape.
                const b2EdgeShape* pShape = dynamic_cast<const b2EdgeShape*>( fixtureDef.shape );

                // Sanity!
                AssertFatal( pShape != NULL, "SceneObject::onTamlCustomWrite() - Invalid edge shape type returned." );

                // Add start point.
                TamlCustomNode* pStartPointNode = pCollisionShapeNode->addNode( shapePointName );
                pStartPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_vertex1 );

                // Add end point.
                TamlCustomNode* pEndPointNode = pCollisionShapeNode->addNode( shapePointName );
                pEndPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_vertex2 );

                // Add adjacent start point (if specified).
                if ( pShape->m_hasVertex0 )
                {
                    TamlCustomNode* pPrevPointNode = pCollisionShapeNode->addNode( shapePrevPointName );
                    pPrevPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_vertex0 );
                }

                // Add adjacent end point (if specified).
                if ( pShape->m_hasVertex3 )
                {
                    TamlCustomNode* pNextPointNode = pCollisionShapeNode->addNode( shapeNextPointName );
                    pNextPointNode->getNodeTextField().setFieldValue( StringTable->EmptyString, pShape->m_vertex3 );
                }
            }
            break;

        default:
            // Sanity!
            AssertFatal( false, "SceneObject::onTamlCustomWrite() - Unknown shape type detected." );
        }
    }
}

//-----------------------------------------------------------------------------

void SceneObject::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SceneObject_OnTamlCustomRead);

    // Call parent.
    Parent::onTamlCustomRead( customNodes );

    // Find collision shape custom node.
    const TamlCustomNode* pCustomCollisionShapes = customNodes.findNode( shapeCustomNodeName );

    // Finish if we don't have collision shapes.
    if ( pCustomCollisionShapes == NULL )
        return;

    // Fetch children shapes.
    const TamlCustomNodeVector& collisionShapeChildren = pCustomCollisionShapes->getChildren();

    // Iterate collision shapes.
    for( TamlCustomNodeVector::const_iterator shapeNodeItr = collisionShapeChildren.begin(); shapeNodeItr != collisionShapeChildren.end(); ++shapeNodeItr )
    {
        // Fetch shape node.
        TamlCustomNode* pShapeNode = *shapeNodeItr;

        // Fetch shape name.
        StringTableEntry shapeName = pShapeNode->getNodeName();

        // Ready common fields.
        F32 shapeDensity     = getDefaultDensity();
        F32 shapeFriction    = getDefaultFriction();
        F32 shapeRestitution = getDefaultRestitution();
        bool shapeSensor     = false;

        S32 shapeIndex;

        // Is this a circle shape?
        if ( shapeName == circleTypeName )
        {
            // Yes, so ready fields.
            F32 radius = 0.0f;
            b2Vec2 offset( 0.0f, 0.0f );

            // Fetch shape children.
            const TamlCustomFieldVector& shapeFields = pShapeNode->getFields();

            // Iterate property fields.
            for ( TamlCustomFieldVector::const_iterator shapeFieldItr = shapeFields.begin(); shapeFieldItr != shapeFields.end(); ++shapeFieldItr )
            {
                // Fetch field.
                const TamlCustomField* pField = *shapeFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pField->getFieldName();

                // Check common fields.
                if ( fieldName == shapeDensityName )
                {
                    pField->getFieldValue( shapeDensity );
                }
                else if ( fieldName == shapeFrictionName )
                {
                    pField->getFieldValue( shapeFriction );
                }
                else if ( fieldName == shapeRestitutionName )
                {
                    pField->getFieldValue( shapeRestitution );
                }
                else if ( fieldName == shapeSensorName )
                {
                    pField->getFieldValue( shapeSensor );
                }
                // Check circle fields.
                else if ( fieldName == circleRadiusName )
                {
                    pField->getFieldValue( radius );
                }
                else if ( fieldName == circleOffsetName )
                {
                    pField->getFieldValue( offset );
                }                   
            }

            // Is radius valid?
            if ( radius <= 0.0f )
            {
                // No, so warn.
                Con::warnf( "SceneObject::onTamlCustomRead() - Invalid radius on circle collision shape '%g'.  Using default.", radius );

                // Set default.
                radius = 1.0f;
            }

            // Create shape.
            shapeIndex = createCircleCollisionShape( radius, offset );
        }
        // Is this a polygon shape?
        else if ( shapeName == polygonTypeName )
        {
            // Yes, so fetch shape fields.
            const TamlCustomFieldVector& shapeFields = pShapeNode->getFields();

            // Iterate property fields.
            for ( TamlCustomFieldVector::const_iterator shapeFieldItr = shapeFields.begin(); shapeFieldItr != shapeFields.end(); ++shapeFieldItr )
            {
                // Fetch field.
                const TamlCustomField* pField = *shapeFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pField->getFieldName();

                // Check common fields.
                if ( fieldName == shapeDensityName )
                {
                    pField->getFieldValue( shapeDensity );
                }
                else if ( fieldName == shapeFrictionName )
                {
                    pField->getFieldValue( shapeFriction );
                }
                else if ( fieldName == shapeRestitutionName )
                {
                    pField->getFieldValue( shapeRestitution );
                }
                else if ( fieldName == shapeSensorName )
                {
                    pField->getFieldValue( shapeSensor );
                }
            }

            // Fetch shape children.
            const TamlCustomNodeVector& shapeChildren = pShapeNode->getChildren();

            // Fetch shape children count.
            const U32 shapeChildrenCount = (U32)shapeChildren.size();

            // Reset points.
            b2Vec2 points[b2_maxPolygonVertices];
            U32 pointCount = 0;

            // Do we have any shape children.
            if ( shapeChildrenCount > 0 )
            {
                // Yes, so iterate them.
                for( TamlCustomNodeVector::const_iterator childItr = shapeChildren.begin(); childItr != shapeChildren.end(); ++childItr )
                {
                    TamlCustomNode* pChildNode = *childItr;

                    // Skip if it's not a point.
                    if ( pChildNode->getNodeName() != shapePointName )
                        continue;
                    
                    // Skip if it's empty.
                    if ( pChildNode->getNodeTextField().isValueEmpty() )
                        continue;

                    // Read point.
                    b2Vec2 point;
                    pChildNode->getNodeTextField().getFieldValue( point );
                    points[pointCount++] = point;
                }
            }

            // Is point count valid?
            if ( pointCount == 0 )
            {
                // No, so warn.
                Con::warnf( "SceneObject::onTamlCustomRead() - No points on polygon collision shape." );

                continue;
            }

            // Create shape.
            shapeIndex = createPolygonCollisionShape( pointCount, points );
        }
        // Is this a chain shape?
        else if ( shapeName == chainTypeName )
        {
            // Yes, so ready fields.
            Vector<b2Vec2> points;
            bool hasAdjacentStartPoint = false;
            bool hasAdjacentEndPoint = false;
            b2Vec2 adjacentStartPoint;
            b2Vec2 adjacentEndPoint;

            // Fetch shape children.
            const TamlCustomFieldVector& shapeFields = pShapeNode->getFields();

            // Iterate property fields.
            for ( TamlCustomFieldVector::const_iterator shapeFieldItr = shapeFields.begin(); shapeFieldItr != shapeFields.end(); ++shapeFieldItr )
            {
                // Fetch field.
                const TamlCustomField* pField = *shapeFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pField->getFieldName();

                // Check common fields.
                if ( fieldName == shapeDensityName )
                {
                    pField->getFieldValue( shapeDensity );
                }
                else if ( fieldName == shapeFrictionName )
                {
                    pField->getFieldValue( shapeFriction );
                }
                else if ( fieldName == shapeRestitutionName )
                {
                    pField->getFieldValue( shapeRestitution );
                }
                else if ( fieldName == shapeSensorName )
                {
                    pField->getFieldValue( shapeSensor );
                }
            }

            // Fetch shape children.
            const TamlCustomNodeVector& shapeChildren = pShapeNode->getChildren();

            // Fetch shape children count.
            const U32 shapeChildrenCount = (U32)shapeChildren.size();

            // Do we have any shape children.
            // NOTE: Only do this if the old methods has not been used.
            if ( points.size() == 0 && shapeChildrenCount > 0 )
            {
                // Yes, so iterate them.
                for( TamlCustomNodeVector::const_iterator childItr = shapeChildren.begin(); childItr != shapeChildren.end(); ++childItr )
                {
                    TamlCustomNode* pChildNode = *childItr;

                    // Fetch the node name.
                    StringTableEntry nodeName = pChildNode->getNodeName();

                    // Skip if it's not a point.
                    if ( !(nodeName == shapePointName || nodeName == shapePrevPointName || nodeName == shapeNextPointName) )
                        continue;
                    
                    // Skip if it's empty.
                    if ( pChildNode->getNodeTextField().isValueEmpty() )
                        continue;

                    if ( nodeName == shapePointName )
                    {
                        // Read point.
                        b2Vec2 point;
                        pChildNode->getNodeTextField().getFieldValue( point );
                        points.push_back( point );
                    }
                    else if ( nodeName == shapePrevPointName )
                    {
                        // Read adjacent point.
                        pChildNode->getNodeTextField().getFieldValue( adjacentStartPoint );
                        hasAdjacentStartPoint = true;
                    }
                    else if ( nodeName == shapeNextPointName )
                    {
                        // Read adjacent point.
                        pChildNode->getNodeTextField().getFieldValue( adjacentEndPoint );
                        hasAdjacentEndPoint = true;
                    }
                }
            }

            // Is point count valid?
            if ( points.size() == 0 || points.size() < 2 )
            {
                // No, so warn.
                Con::warnf( "SceneObject::onTamlCustomRead() - No points (or less than two) on chain collision shape." );

                continue;
            }

            // Create shape.
            shapeIndex = createChainCollisionShape( points.size(), points.address(), hasAdjacentStartPoint, hasAdjacentEndPoint, adjacentStartPoint, adjacentEndPoint );
        }
        // Is this an edge shape?
        else if ( shapeName == edgeTypeName )
        {
            // Yes, so ready fields.
            b2Vec2 point0;
            b2Vec2 point1;
            U32 pointCount = 0;
            bool hasAdjacentStartPoint = false;
            bool hasAdjacentEndPoint = false;
            b2Vec2 adjacentStartPoint;
            b2Vec2 adjacentEndPoint;

            // Fetch shape children.
            const TamlCustomFieldVector& shapeFields = pShapeNode->getFields();

            // Iterate property fields.
            for ( TamlCustomFieldVector::const_iterator shapeFieldItr = shapeFields.begin(); shapeFieldItr != shapeFields.end(); ++shapeFieldItr )
            {
                // Fetch field.
                const TamlCustomField* pField = *shapeFieldItr;

                // Fetch property field name.
                StringTableEntry fieldName = pField->getFieldName();

                // Check common fields.
                if ( fieldName == shapeDensityName )
                {
                    pField->getFieldValue( shapeDensity );
                }
                else if ( fieldName == shapeFrictionName )
                {
                    pField->getFieldValue( shapeFriction );
                }
                else if ( fieldName == shapeRestitutionName )
                {
                    pField->getFieldValue( shapeRestitution );
                }
                else if ( fieldName == shapeSensorName )
                {
                    pField->getFieldValue( shapeSensor );
                }
            }

            // Fetch shape children.
            const TamlCustomNodeVector& shapeChildren = pShapeNode->getChildren();

            // Fetch shape children count.
            const U32 shapeChildrenCount = (U32)shapeChildren.size();

            // Do we have any shape children.
            if ( shapeChildrenCount > 0 )
            {
                // Yes, so iterate them.
                for( TamlCustomNodeVector::const_iterator childItr = shapeChildren.begin(); childItr != shapeChildren.end(); ++childItr )
                {
                    TamlCustomNode* pChildNode = *childItr;

                    // Fetch the node name.
                    StringTableEntry nodeName = pChildNode->getNodeName();

                    // Skip if it's not a point.
                    if ( !(nodeName == shapePointName || nodeName == shapePrevPointName || nodeName == shapeNextPointName) )
                        continue;
                    
                    // Skip if it's empty.
                    if ( pChildNode->getNodeTextField().isValueEmpty() )
                        continue;

                    if ( nodeName == shapePointName )
                    {
                        // Ignore if too many points.
                        if ( pointCount >= 2 )
                            continue;

                        // Read point.               
                        if ( pointCount == 0 )
                            pChildNode->getNodeTextField().getFieldValue( point0 );
                        else
                            pChildNode->getNodeTextField().getFieldValue( point1 );

                        pointCount++;
                    }
                    else if ( nodeName == shapePrevPointName )
                    {
                        // Read adjacent point.
                        pChildNode->getNodeTextField().getFieldValue( adjacentStartPoint );
                        hasAdjacentStartPoint = true;
                    }
                    else if ( nodeName == shapeNextPointName )
                    {
                        // Read adjacent point.
                        pChildNode->getNodeTextField().getFieldValue( adjacentEndPoint );
                        hasAdjacentEndPoint = true;
                    }
                }
            }

            // Is point count valid?
            if ( pointCount == 0 || pointCount != 2 )
            {
                // No, so warn.
                Con::warnf( "SceneObject::onTamlCustomRead() - No points (or not two points) on edge collision shape." );

                continue;
            }

            // Create shape.
            shapeIndex = createEdgeCollisionShape( point0, point1, hasAdjacentStartPoint, hasAdjacentEndPoint, adjacentStartPoint, adjacentEndPoint );
        }
        // Unknown shape type!
        else
        {
            // Warn.
            Con::warnf( "Unknown shape type of '%s' encountered.", shapeName );

            // Sanity!
            AssertFatal( false, "SceneObject::onTamlCustomRead() - Unknown shape type detected." );

            continue;
        }

        // Set common properties.
        setCollisionShapeDensity( shapeIndex, shapeDensity );
        setCollisionShapeFriction( shapeIndex, shapeFriction );
        setCollisionShapeRestitution( shapeIndex, shapeRestitution );
        setCollisionShapeIsSensor( shapeIndex, shapeSensor );
    }        
}

//-----------------------------------------------------------------------------

bool SceneObject::writeField(StringTableEntry fieldname, const char* value)
{
   if (!Parent::writeField(fieldname, value))
      return false;

   // Never save the scene field.
   if (dStricmp(fieldname, "scene") == 0)
      return false;

   return true;
}

void SceneObject::addAudioHandle(AUDIOHANDLE handle)
{
   mAudioHandles.push_back_unique(handle);
   Con::printf("New Vector size : %i", mAudioHandles.size());
}

S32 SceneObject::getSoundsCount(void)
{
    return mAudioHandles.size();
}

U32 SceneObject::getSound(S32 index)
{
    if (mAudioHandles.size() - 1 < index)
        return NULL_AUDIOHANDLE;
    U32 handle = mAudioHandles[index];
    return handle;
}

void SceneObject::refreshsources()
{
if (mAudioHandles.size())
{
    S32 index = 0;
    for (typeAudioHandleVector::iterator itr = mAudioHandles.begin(); itr != mAudioHandles.end(); ++itr)
    {
        U32 handle = *itr;

        if (handle)
        {
            if (!alxIsValidHandle(handle))
            mHandleDeletionList.push_back(index);

            index++;
        }
    }
        
    if (mHandleDeletionList.size())
    {

        for (Vector<S32>::iterator delitr = mHandleDeletionList.begin(); delitr != mHandleDeletionList.end(); ++delitr)
        {
            mAudioHandles.erase(*delitr);
        }
            mHandleDeletionList.clear();
    }
}
}


//------------------------------------------------------------------------------

S32 QSORT_CALLBACK SceneObject::sceneObjectLayerDepthSort(const void* a, const void* b)
{
    // Fetch scene objects.
    SceneObject* pSceneObjectA  = *((SceneObject**)a);
    SceneObject* pSceneObjectB  = *((SceneObject**)b);

    // Fetch layers.
    const U32 layerA = pSceneObjectA->getSceneLayer();
    const U32 layerB = pSceneObjectB->getSceneLayer();

    if ( layerA < layerB )
        return -1;

    if ( layerA > layerB )
        return 1;

    // Fetch layer depths.
    const F32 depthA = pSceneObjectA->getSceneLayerDepth();
    const F32 depthB = pSceneObjectB->getSceneLayerDepth();

    return depthA < depthB ? 1 : depthA > depthB ? -1 : pSceneObjectA->getSerialId() - pSceneObjectB->getSerialId();
}

//-----------------------------------------------------------------------------

static EnumTable::Enums bodyTypeLookup[] =
                {
                { b2_staticBody,    "Static"    },
                { b2_kinematicBody, "Kinematic" },
                { b2_dynamicBody,   "Dynamic"   },
                };

EnumTable bodyTypeTable(sizeof(bodyTypeLookup) / sizeof(EnumTable::Enums), &bodyTypeLookup[0]);

//-----------------------------------------------------------------------------

static EnumTable::Enums collisionShapeTypeLookup[] =
                {
                { b2Shape::e_circle,             "Circle"   },
                { b2Shape::e_edge,               "Edge"     },
                { b2Shape::e_polygon,            "Polygon"  },
                { b2Shape::e_chain,              "Chain"    },
                };

EnumTable collisionShapeTypeTable(sizeof(collisionShapeTypeLookup) / sizeof(EnumTable::Enums), &collisionShapeTypeLookup[0]);

//-----------------------------------------------------------------------------

static EnumTable::Enums srcBlendFactorLookup[] =
                {
                { GL_ZERO,                  "ZERO"                  },
                { GL_ONE,                   "ONE"                   },
                { GL_DST_COLOR,             "DST_COLOR"             },
                { GL_ONE_MINUS_DST_COLOR,   "ONE_MINUS_DST_COLOR"   },
                { GL_SRC_ALPHA,             "SRC_ALPHA"             },
                { GL_ONE_MINUS_SRC_ALPHA,   "ONE_MINUS_SRC_ALPHA"   },
                { GL_DST_ALPHA,             "DST_ALPHA"             },
                { GL_ONE_MINUS_DST_ALPHA,   "ONE_MINUS_DST_ALPHA"   },
                { GL_SRC_ALPHA_SATURATE,    "SRC_ALPHA_SATURATE"    },
                };

EnumTable srcBlendFactorTable(sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums), &srcBlendFactorLookup[0]);

//-----------------------------------------------------------------------------

static EnumTable::Enums dstBlendFactorLookup[] =
                {
                { GL_ZERO,                  "ZERO" },
                { GL_ONE,                   "ONE" },
                { GL_SRC_COLOR,             "SRC_COLOR" },
                { GL_ONE_MINUS_SRC_COLOR,   "ONE_MINUS_SRC_COLOR" },
                { GL_SRC_ALPHA,             "SRC_ALPHA" },
                { GL_ONE_MINUS_SRC_ALPHA,   "ONE_MINUS_SRC_ALPHA" },
                { GL_DST_ALPHA,             "DST_ALPHA" },
                { GL_ONE_MINUS_DST_ALPHA,   "ONE_MINUS_DST_ALPHA" },
                };

EnumTable dstBlendFactorTable(sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums), &dstBlendFactorLookup[0]);

//-----------------------------------------------------------------------------

b2BodyType SceneObject::getBodyTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(bodyTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(bodyTypeLookup[i].label, label) == 0)
            return (b2BodyType)bodyTypeLookup[i].index;
    }

    // Warn.
    Con::warnf("SceneObject::getBodyTypeEnum() - Invalid body type of '%s'", label );

    return (b2BodyType)-1;
}

//-----------------------------------------------------------------------------

const char* SceneObject::getBodyTypeDescription(const b2BodyType bodyType)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(bodyTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( bodyTypeLookup[i].index == bodyType )
            return bodyTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "SceneObject::getBodyTypeDescription() - Invalid body type." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

b2Shape::Type SceneObject::getCollisionShapeTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(collisionShapeTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(collisionShapeTypeLookup[i].label, label) == 0)
            return (b2Shape::Type)collisionShapeTypeLookup[i].index;
    }

    // Warn!
    Con::warnf("SceneObject::getCollisionShapeTypeEnum() - Invalid collision shape type of '%s'", label );

    return b2Shape::e_typeCount;
}

//-----------------------------------------------------------------------------

const char* SceneObject::getCollisionShapeTypeDescription(const b2Shape::Type collisionShapeType)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(collisionShapeTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( collisionShapeTypeLookup[i].index == collisionShapeType )
            return collisionShapeTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "SceneObject::getCollisionShapeTypeDescription() - Invalid collision shape type." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

S32 SceneObject::getSrcBlendFactorEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(srcBlendFactorLookup[i].label, label) == 0)
            return(srcBlendFactorLookup[i].index);
    }

    // Warn.
    Con::warnf("SceneObject::getSrcBlendFactorEnum() - Invalid source blend factor of '%s'", label );

    return GL_INVALID_BLEND_FACTOR;
}

//-----------------------------------------------------------------------------

const char* SceneObject::getSrcBlendFactorDescription(const GLenum factor)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( srcBlendFactorLookup[i].index == (S32)factor )
            return srcBlendFactorLookup[i].label;
    }

    // Warn.
    Con::warnf( "SceneObject::getSrcBlendFactorDescription() - Invalid source blend factor." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

S32 SceneObject::getDstBlendFactorEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(dstBlendFactorLookup[i].label, label) == 0)
            return(dstBlendFactorLookup[i].index);
    }

    // Warn.
    Con::warnf("SceneObject::getSrcBlendFactorEnum() - Invalid destination blend factor of '%s'", label );

    return GL_INVALID_BLEND_FACTOR;
}

//-----------------------------------------------------------------------------

const char* SceneObject::getDstBlendFactorDescription(const GLenum factor)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dstBlendFactorLookup[i].index == (S32)factor )
            return dstBlendFactorLookup[i].label;
    }

    // Warn.
    Con::warnf( "SceneObject::getDstBlendFactorDescription() - Invalid destination blend factor." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

void SceneObject::updateBlendColor(const F32 elapsedTime)
{
	// Apply the color deltas to the blendColor to move it toward the targetColor.
	mBlendColor.red = processEffect(mBlendColor.red, mTargetColor.red, mDeltaRed * elapsedTime);
	mBlendColor.green = processEffect(mBlendColor.green, mTargetColor.green, mDeltaGreen * elapsedTime);
	mBlendColor.blue = processEffect(mBlendColor.blue, mTargetColor.blue, mDeltaBlue * elapsedTime);
	mBlendColor.alpha = processEffect(mBlendColor.alpha, mTargetColor.alpha, mDeltaAlpha * elapsedTime);
}

//-----------------------------------------------------------------------------

void SceneObject::updateSize(const F32 elapsedTime)
{
   // Apply the size deltas to the area to move it toward the targetSize.
   mSize.x = processEffect(mSize.x, mTargetSize.x, mDeltaSize.x * elapsedTime);
   mSize.y = processEffect(mSize.y, mTargetSize.y, mDeltaSize.y * elapsedTime);

   setSize(mSize);
}

//-----------------------------------------------------------------------------

void SceneObject::updateTargetPosition()
{
   F32 distance = (getPosition() - mTargetPosition).Length();
   bool hasArrived = false;

   // Is the current position within the target?
   if ( distance <= mTargetPositionMargin )
   {
      hasArrived = true;
   }
   else // Did we pass through the target?
   {
      // Moving vertically
      if (mLastCheckedPosition.x == getPosition().x)
      {
         if (((mLastCheckedPosition.y < mTargetPosition.y && mTargetPosition.y < getPosition().y) || (mLastCheckedPosition.y > mTargetPosition.y && mTargetPosition.y > getPosition().y)) &&
         mFabs(getPosition().x - mTargetPosition.x) <= mTargetPositionMargin)
         {
            hasArrived = true;
         }
      }// Or moving horizontally
      else if (mLastCheckedPosition.y == getPosition().y)
      {
         if (((mLastCheckedPosition.x < mTargetPosition.x && mTargetPosition.x < getPosition().x) || (mLastCheckedPosition.x > mTargetPosition.x && mTargetPosition.x > getPosition().x)) &&
            mFabs(getPosition().y - mTargetPosition.y) <= mTargetPositionMargin)
         {
            hasArrived = true;
         }
      }// Or moving diagonally
      else
      {
         //slopes of the two lines
         Vector2 slope = (mLastCheckedPosition - getPosition());
         Vector2 perpSlope = slope.getPerp();
         F32 m1 = slope.y / slope.x;
         F32 m2 = perpSlope.y / perpSlope.x;

         //y-intercepts
         F32 b1 = mLastCheckedPosition.y - (m1 * mLastCheckedPosition.x);
         F32 b2 = mTargetPosition.y - (m2 * mTargetPosition.x);

         //point of interception
         F32 x = (b1 - b2) / (m2 - m1);
         F32 y = (m1 * x) + b1;
         Vector2 intercept = Vector2(x, y);

         //Is the intercept point between the other two points and is the distance less than the margin?
         if (((mLastCheckedPosition.x < intercept.x && intercept.x < getPosition().x) || (mLastCheckedPosition.x > intercept.x && intercept.x > getPosition().x)) &&
            (intercept - mTargetPosition).Length() <= mTargetPositionMargin)
         {
            hasArrived = true;
         }
      }
   }

   if (hasArrived)
   {
      if (mSnapToTargetPosition)
      {
         setPosition(mTargetPosition);
      }
      if (mStopAtTargetPosition)
      {
         setLinearVelocity(Vector2::getZero());
      }
      mTargetPositionFound = true;
   }
   // Are we moving away from the target?
   else if (distance > mDistanceToTarget)
   {
      // Then turn off the target. No need to keep checking.
      mTargetPositionActive = false;
   }
   mLastCheckedPosition = getPosition();
   mDistanceToTarget = distance;
}

//-----------------------------------------------------------------------------

F32 SceneObject::processEffect(const F32 current, const F32 target, const F32 rate)
{
	if (mFabs(current - target) < rate)
	{
		return target;
	}
	else if (current < target)
	{
		return current + rate;
	}
	else if (current > target)
	{
		return current - rate;
	}
	else
	{
		return target;
	}
}

//-----------------------------------------------------------------------------

static void WriteCircleCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SceneObject::WriteCircleCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SceneObject::WriteCircleCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create circle element.
    TiXmlElement* pCircleElement = new TiXmlElement( "xs:element" );
    pCircleElement->SetAttribute( "name", circleTypeName );
    pCircleElement->SetAttribute( "minOccurs", 0 );
    pCircleElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pCircleElement );

    // Create complex type Element.
    TiXmlElement* pCircleComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pCircleElement->LinkEndChild( pCircleComplexTypeElement );

    // Create "Radius" attribute.
    TiXmlElement* pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", circleRadiusName );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );
    TiXmlElement* pCircleElementB = new TiXmlElement( "xs:simpleType" );
    pCircleElementA->LinkEndChild( pCircleElementB );
    TiXmlElement* pCircleElementC = new TiXmlElement( "xs:restriction" );
    pCircleElementC->SetAttribute( "base", "xs:float" );
    pCircleElementB->LinkEndChild( pCircleElementC );
    TiXmlElement* pCircleElementD = new TiXmlElement( "xs:minExclusive" );
    pCircleElementD->SetAttribute( "value", "0" );
    pCircleElementC->LinkEndChild( pCircleElementD );

    // Create "Offset" attribute.
    pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", circleOffsetName );
    pCircleElementA->SetAttribute( "type", "Vector2_ConsoleType" );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );

    // Create "IsSensor" attribute.
    pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", shapeSensorName );
    pCircleElementA->SetAttribute( "type", "xs:boolean" );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );

    // Create "Density" attribute.
    pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", shapeDensityName );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );
    pCircleElementB = new TiXmlElement( "xs:simpleType" );
    pCircleElementA->LinkEndChild( pCircleElementB );
    pCircleElementC = new TiXmlElement( "xs:restriction" );
    pCircleElementC->SetAttribute( "base", "xs:float" );
    pCircleElementB->LinkEndChild( pCircleElementC );
    pCircleElementD = new TiXmlElement( "xs:minInclusive" );
    pCircleElementD->SetAttribute( "value", "0" );
    pCircleElementC->LinkEndChild( pCircleElementD );

    // Create "Friction" attribute.
    pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", shapeFrictionName );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );
    pCircleElementB = new TiXmlElement( "xs:simpleType" );
    pCircleElementA->LinkEndChild( pCircleElementB );
    pCircleElementC = new TiXmlElement( "xs:restriction" );
    pCircleElementC->SetAttribute( "base", "xs:float" );
    pCircleElementB->LinkEndChild( pCircleElementC );
    pCircleElementD = new TiXmlElement( "xs:minInclusive" );
    pCircleElementD->SetAttribute( "value", "0" );
    pCircleElementC->LinkEndChild( pCircleElementD );

    // Create "Restitution" attribute.
    pCircleElementA = new TiXmlElement( "xs:attribute" );
    pCircleElementA->SetAttribute( "name", shapeRestitutionName );
    pCircleComplexTypeElement->LinkEndChild( pCircleElementA );
    pCircleElementB = new TiXmlElement( "xs:simpleType" );
    pCircleElementA->LinkEndChild( pCircleElementB );
    pCircleElementC = new TiXmlElement( "xs:restriction" );
    pCircleElementC->SetAttribute( "base", "xs:float" );
    pCircleElementB->LinkEndChild( pCircleElementC );
    pCircleElementD = new TiXmlElement( "xs:minInclusive" );
    pCircleElementD->SetAttribute( "value", "0" );
    pCircleElementC->LinkEndChild( pCircleElementD );
}

//-----------------------------------------------------------------------------

static void WritePolygonCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SceneObject::WritePolygonCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SceneObject::WritePolygonCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create polygon element.
    TiXmlElement* pPolygonElement = new TiXmlElement( "xs:element" );
    pPolygonElement->SetAttribute( "name", polygonTypeName );
    pPolygonElement->SetAttribute( "minOccurs", 0 );
    pPolygonElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pPolygonElement );

    // Create complex type Element.
    TiXmlElement* pPolygonComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pPolygonElement->LinkEndChild( pPolygonComplexTypeElement );

    // Create "polygon" child.
    TiXmlElement* pPolygonElementA = new TiXmlElement( "xs:choice" );
    pPolygonElementA->SetAttribute( "minOccurs", 0 );
    pPolygonElementA->SetAttribute( "maxOccurs", "unbounded" );
    pPolygonComplexTypeElement->LinkEndChild( pPolygonElementA );
    TiXmlElement* pPolygonElementB = new TiXmlElement( "xs:element" );
    pPolygonElementB->SetAttribute( "name", shapePointName );
    pPolygonElementB->SetAttribute( "type", "Vector2_ConsoleType" );
    pPolygonElementA->LinkEndChild( pPolygonElementB );

    // Create "IsSensor" attribute.
    pPolygonElementA = new TiXmlElement( "xs:attribute" );
    pPolygonElementA->SetAttribute( "name", shapeSensorName );
    pPolygonElementA->SetAttribute( "type", "xs:boolean" );
    pPolygonComplexTypeElement->LinkEndChild( pPolygonElementA );

    // Create "Density" attribute.
    pPolygonElementA = new TiXmlElement( "xs:attribute" );
    pPolygonElementA->SetAttribute( "name", shapeDensityName );
    pPolygonComplexTypeElement->LinkEndChild( pPolygonElementA );
    pPolygonElementB = new TiXmlElement( "xs:simpleType" );
    pPolygonElementA->LinkEndChild( pPolygonElementB );
    TiXmlElement* pPolygonElementC = new TiXmlElement( "xs:restriction" );
    pPolygonElementC->SetAttribute( "base", "xs:float" );
    pPolygonElementB->LinkEndChild( pPolygonElementC );
    TiXmlElement* pPolygonElementD = new TiXmlElement( "xs:minInclusive" );
    pPolygonElementD->SetAttribute( "value", "0" );
    pPolygonElementC->LinkEndChild( pPolygonElementD );

    // Create "Friction" attribute.
    pPolygonElementA = new TiXmlElement( "xs:attribute" );
    pPolygonElementA->SetAttribute( "name", shapeFrictionName );
    pPolygonComplexTypeElement->LinkEndChild( pPolygonElementA );
    pPolygonElementB = new TiXmlElement( "xs:simpleType" );
    pPolygonElementA->LinkEndChild( pPolygonElementB );
    pPolygonElementC = new TiXmlElement( "xs:restriction" );
    pPolygonElementC->SetAttribute( "base", "xs:float" );
    pPolygonElementB->LinkEndChild( pPolygonElementC );
    pPolygonElementD = new TiXmlElement( "xs:minInclusive" );
    pPolygonElementD->SetAttribute( "value", "0" );
    pPolygonElementC->LinkEndChild( pPolygonElementD );

    // Create "Restitution" attribute.
    pPolygonElementA = new TiXmlElement( "xs:attribute" );
    pPolygonElementA->SetAttribute( "name", shapeRestitutionName );
    pPolygonComplexTypeElement->LinkEndChild( pPolygonElementA );
    pPolygonElementB = new TiXmlElement( "xs:simpleType" );
    pPolygonElementA->LinkEndChild( pPolygonElementB );
    pPolygonElementC = new TiXmlElement( "xs:restriction" );
    pPolygonElementC->SetAttribute( "base", "xs:float" );
    pPolygonElementB->LinkEndChild( pPolygonElementC );
    pPolygonElementD = new TiXmlElement( "xs:minInclusive" );
    pPolygonElementD->SetAttribute( "value", "0" );
    pPolygonElementC->LinkEndChild( pPolygonElementD );
}

//-----------------------------------------------------------------------------

static void WriteChainCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SceneObject::WriteChainCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SceneObject::WriteChainCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create chain element.
    TiXmlElement* pChainElement = new TiXmlElement( "xs:element" );
    pChainElement->SetAttribute( "name", chainTypeName );
    pChainElement->SetAttribute( "minOccurs", 0 );
    pChainElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pChainElement );

    // Create complex type Element.
    TiXmlElement* pChainComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pChainElement->LinkEndChild( pChainComplexTypeElement );

    // Create "Chain" child.
    TiXmlElement* pChainElementA = new TiXmlElement( "xs:sequence" );
    pChainComplexTypeElement->LinkEndChild( pChainElementA );
    TiXmlElement* pChainElementB = new TiXmlElement( "xs:choice" );
    pChainElementB->SetAttribute( "minOccurs", 0 );
    pChainElementB->SetAttribute( "maxOccurs", "unbounded" );
    pChainElementA->LinkEndChild( pChainElementB );
    TiXmlElement* pChainElementC = new TiXmlElement( "xs:element" );
    pChainElementC->SetAttribute( "name", shapePointName );
    pChainElementC->SetAttribute( "type", "Vector2_ConsoleType" );
    pChainElementB->LinkEndChild( pChainElementC );
    TiXmlElement* pChainElementD = new TiXmlElement( "xs:element" );
    pChainElementD->SetAttribute( "name", shapePrevPointName );
    pChainElementD->SetAttribute( "type", "Vector2_ConsoleType" );
    pChainElementD->SetAttribute( "minOccurs", 0 );
    pChainElementD->SetAttribute( "maxOccurs", 1 );
    pChainElementA->LinkEndChild( pChainElementD );
    TiXmlElement* pChainElementE = new TiXmlElement( "xs:element" );
    pChainElementE->SetAttribute( "name", shapeNextPointName );
    pChainElementE->SetAttribute( "type", "Vector2_ConsoleType" );
    pChainElementE->SetAttribute( "minOccurs", 0 );
    pChainElementE->SetAttribute( "maxOccurs", 1 );
    pChainElementA->LinkEndChild( pChainElementE );

    // Create "IsSensor" attribute.
    pChainElementA = new TiXmlElement( "xs:attribute" );
    pChainElementA->SetAttribute( "name", shapeSensorName );
    pChainElementA->SetAttribute( "type", "xs:boolean" );
    pChainComplexTypeElement->LinkEndChild( pChainElementA );

    // Create "Density" attribute.
    pChainElementA = new TiXmlElement( "xs:attribute" );
    pChainElementA->SetAttribute( "name", shapeDensityName );
    pChainComplexTypeElement->LinkEndChild( pChainElementA );
    pChainElementB = new TiXmlElement( "xs:simpleType" );
    pChainElementA->LinkEndChild( pChainElementB );
    pChainElementC = new TiXmlElement( "xs:restriction" );
    pChainElementC->SetAttribute( "base", "xs:float" );
    pChainElementB->LinkEndChild( pChainElementC );
    pChainElementD = new TiXmlElement( "xs:minInclusive" );
    pChainElementD->SetAttribute( "value", "0" );
    pChainElementC->LinkEndChild( pChainElementD );

    // Create "Friction" attribute.
    pChainElementA = new TiXmlElement( "xs:attribute" );
    pChainElementA->SetAttribute( "name", shapeFrictionName );
    pChainComplexTypeElement->LinkEndChild( pChainElementA );
    pChainElementB = new TiXmlElement( "xs:simpleType" );
    pChainElementA->LinkEndChild( pChainElementB );
    pChainElementC = new TiXmlElement( "xs:restriction" );
    pChainElementC->SetAttribute( "base", "xs:float" );
    pChainElementB->LinkEndChild( pChainElementC );
    pChainElementD = new TiXmlElement( "xs:minInclusive" );
    pChainElementD->SetAttribute( "value", "0" );
    pChainElementC->LinkEndChild( pChainElementD );

    // Create "Restitution" attribute.
    pChainElementA = new TiXmlElement( "xs:attribute" );
    pChainElementA->SetAttribute( "name", shapeRestitutionName );
    pChainComplexTypeElement->LinkEndChild( pChainElementA );
    pChainElementB = new TiXmlElement( "xs:simpleType" );
    pChainElementA->LinkEndChild( pChainElementB );
    pChainElementC = new TiXmlElement( "xs:restriction" );
    pChainElementC->SetAttribute( "base", "xs:float" );
    pChainElementB->LinkEndChild( pChainElementC );
    pChainElementD = new TiXmlElement( "xs:minInclusive" );
    pChainElementD->SetAttribute( "value", "0" );
    pChainElementC->LinkEndChild( pChainElementD );
}

//-----------------------------------------------------------------------------

static void WriteEdgeCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SceneObject::WriteEdgeCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SceneObject::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create edge element.
    TiXmlElement* pEdgeElement = new TiXmlElement( "xs:element" );
    pEdgeElement->SetAttribute( "name", edgeTypeName );
    pEdgeElement->SetAttribute( "minOccurs", 0 );
    pEdgeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pEdgeElement );

    // Create complex type Element.
    TiXmlElement* pEdgeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pEdgeElement->LinkEndChild( pEdgeComplexTypeElement );

    // Create "Edge" child.
    TiXmlElement* pEdgeElementA = new TiXmlElement( "xs:sequence" );
    pEdgeComplexTypeElement->LinkEndChild( pEdgeElementA );
    TiXmlElement* pEdgeElementB = new TiXmlElement( "xs:element" );
    pEdgeElementB->SetAttribute( "name", shapePointName );
    pEdgeElementB->SetAttribute( "type", "Vector2_ConsoleType" );
    pEdgeElementB->SetAttribute( "minOccurs", 0 );
    pEdgeElementB->SetAttribute( "maxOccurs", 2 );
    pEdgeElementA->LinkEndChild( pEdgeElementB );
    TiXmlElement* pEdgeElementC = new TiXmlElement( "xs:element" );
    pEdgeElementC->SetAttribute( "name", shapePrevPointName );
    pEdgeElementC->SetAttribute( "type", "Vector2_ConsoleType" );
    pEdgeElementC->SetAttribute( "minOccurs", 0 );
    pEdgeElementC->SetAttribute( "maxOccurs", 1 );
    pEdgeElementA->LinkEndChild( pEdgeElementC );
    TiXmlElement* pEdgeElementD = new TiXmlElement( "xs:element" );
    pEdgeElementD->SetAttribute( "name", shapeNextPointName );
    pEdgeElementD->SetAttribute( "type", "Vector2_ConsoleType" );
    pEdgeElementD->SetAttribute( "minOccurs", 0 );
    pEdgeElementD->SetAttribute( "maxOccurs", 1 );
    pEdgeElementA->LinkEndChild( pEdgeElementD );

    // Create "IsSensor" attribute.
    pEdgeElementA = new TiXmlElement( "xs:attribute" );
    pEdgeElementA->SetAttribute( "name", shapeSensorName );
    pEdgeElementA->SetAttribute( "type", "xs:boolean" );
    pEdgeComplexTypeElement->LinkEndChild( pEdgeElementA );

    // Create "Density" attribute.
    pEdgeElementA = new TiXmlElement( "xs:attribute" );
    pEdgeElementA->SetAttribute( "name", shapeDensityName );
    pEdgeComplexTypeElement->LinkEndChild( pEdgeElementA );
    pEdgeElementB = new TiXmlElement( "xs:simpleType" );
    pEdgeElementA->LinkEndChild( pEdgeElementB );
    pEdgeElementC = new TiXmlElement( "xs:restriction" );
    pEdgeElementC->SetAttribute( "base", "xs:float" );
    pEdgeElementB->LinkEndChild( pEdgeElementC );
    pEdgeElementD = new TiXmlElement( "xs:minInclusive" );
    pEdgeElementD->SetAttribute( "value", "0" );
    pEdgeElementC->LinkEndChild( pEdgeElementD );

    // Create "Friction" attribute.
    pEdgeElementA = new TiXmlElement( "xs:attribute" );
    pEdgeElementA->SetAttribute( "name", shapeFrictionName );
    pEdgeComplexTypeElement->LinkEndChild( pEdgeElementA );
    pEdgeElementB = new TiXmlElement( "xs:simpleType" );
    pEdgeElementA->LinkEndChild( pEdgeElementB );
    pEdgeElementC = new TiXmlElement( "xs:restriction" );
    pEdgeElementC->SetAttribute( "base", "xs:float" );
    pEdgeElementB->LinkEndChild( pEdgeElementC );
    pEdgeElementD = new TiXmlElement( "xs:minInclusive" );
    pEdgeElementD->SetAttribute( "value", "0" );
    pEdgeElementC->LinkEndChild( pEdgeElementD );

    // Create "Restitution" attribute.
    pEdgeElementA = new TiXmlElement( "xs:attribute" );
    pEdgeElementA->SetAttribute( "name", shapeRestitutionName );
    pEdgeComplexTypeElement->LinkEndChild( pEdgeElementA );
    pEdgeElementB = new TiXmlElement( "xs:simpleType" );
    pEdgeElementA->LinkEndChild( pEdgeElementB );
    pEdgeElementC = new TiXmlElement( "xs:restriction" );
    pEdgeElementC->SetAttribute( "base", "xs:float" );
    pEdgeElementB->LinkEndChild( pEdgeElementC );
    pEdgeElementD = new TiXmlElement( "xs:minInclusive" );
    pEdgeElementD->SetAttribute( "value", "0" );
    pEdgeElementC->LinkEndChild( pEdgeElementD );
}

//-----------------------------------------------------------------------------

static void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SceneObject::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SceneObject::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create shapes node element.
    TiXmlElement* pShapesNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), shapeCustomNodeName );
    pShapesNodeElement->SetAttribute( "name", buffer );
    pShapesNodeElement->SetAttribute( "minOccurs", 0 );
    pShapesNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pShapesNodeElement );
    
    // Create complex type.
    TiXmlElement* pShapesNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pShapesNodeElement->LinkEndChild( pShapesNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pShapesNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pShapesNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pShapesNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pShapesNodeComplexTypeElement->LinkEndChild( pShapesNodeChoiceElement );

    // Write collision shapes.
    WriteCircleCustomTamlSchema( pClassRep, pShapesNodeChoiceElement );
    WritePolygonCustomTamlSchema( pClassRep, pShapesNodeChoiceElement );
    WriteChainCustomTamlSchema( pClassRep, pShapesNodeChoiceElement );
    WriteEdgeCustomTamlSchema( pClassRep, pShapesNodeChoiceElement );
}

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_SCHEMA(SceneObject, WriteCustomTamlSchema);
