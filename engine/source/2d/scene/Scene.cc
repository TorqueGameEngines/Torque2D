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

#ifndef _SCENE_H_
#include "Scene.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _BITSTREAM_H_
#include "io/bitStream.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _CONTACT_FILTER_H_
#include "ContactFilter.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/SceneRenderObject.h"
#endif

#ifndef _SCENE_CONTROLLER_H_
#include "2d/controllers/core/SceneController.h"
#endif

#ifndef _PARTICLE_SYSTEM_H_
#include "2d/core/ParticleSystem.h"
#endif

// Script bindings.
#include "Scene_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

SimObjectPtr<Scene> Scene::LoadingScene = NULL;

//------------------------------------------------------------------------------

static ContactFilter mContactFilter;

// Scene counter.
static U32 sSceneCount = 0;
static U32 sSceneMasterIndex = 0;

// Joint custom node names.
static StringTableEntry jointCustomNodeName               = StringTable->insert( "Joints" );
static StringTableEntry jointCollideConnectedName         = StringTable->insert( "CollideConnected" );
static StringTableEntry jointLocalAnchorAName             = StringTable->insert( "LocalAnchorA" );
static StringTableEntry jointLocalAnchorBName             = StringTable->insert( "LocalAnchorB" );

static StringTableEntry jointDistanceNodeName             = StringTable->insert( "Distance" );
static StringTableEntry jointDistanceLengthName           = StringTable->insert( "Length" );
static StringTableEntry jointDistanceFrequencyName        = StringTable->insert( "Frequency" );
static StringTableEntry jointDistanceDampingRatioName     = StringTable->insert( "DampingRatio" );

static StringTableEntry jointRopeNodeName                 = StringTable->insert( "Rope" );
static StringTableEntry jointRopeMaxLengthName            = StringTable->insert( "MaxLength" );

static StringTableEntry jointRevoluteNodeName             = StringTable->insert( "Revolute" );
static StringTableEntry jointRevoluteLimitLowerAngleName  = StringTable->insert( "LowerAngle" );
static StringTableEntry jointRevoluteLimitUpperAngleName  = StringTable->insert( "UpperAngle" );
static StringTableEntry jointRevoluteMotorSpeedName       = StringTable->insert( "MotorSpeed" );
static StringTableEntry jointRevoluteMotorMaxTorqueName   = StringTable->insert( "MaxTorque" );

static StringTableEntry jointWeldNodeName                 = StringTable->insert( "Weld" );
static StringTableEntry jointWeldFrequencyName            = jointDistanceFrequencyName;
static StringTableEntry jointWeldDampingRatioName         = jointDistanceDampingRatioName;

static StringTableEntry jointWheelNodeName                = StringTable->insert( "Wheel" );
static StringTableEntry jointWheelWorldAxisName           = StringTable->insert( "WorldAxis" );
static StringTableEntry jointWheelMotorSpeedName          = StringTable->insert( "MotorSpeed" );
static StringTableEntry jointWheelMotorMaxTorqueName      = jointRevoluteMotorMaxTorqueName;
static StringTableEntry jointWheelFrequencyName           = jointDistanceFrequencyName;
static StringTableEntry jointWheelDampingRatioName        = jointDistanceDampingRatioName;

static StringTableEntry jointFrictionNodeName             = StringTable->insert( "Friction" );
static StringTableEntry jointFrictionMaxForceName         = StringTable->insert( "MaxForce" );
static StringTableEntry jointFrictionMaxTorqueName        = jointRevoluteMotorMaxTorqueName;

static StringTableEntry jointPrismaticNodeName            = StringTable->insert( "Prismatic" );
static StringTableEntry jointPrismaticWorldAxisName       = jointWheelWorldAxisName;
static StringTableEntry jointPrismaticLimitLowerTransName = StringTable->insert( "LowerTranslation" );
static StringTableEntry jointPrismaticLimitUpperTransName = StringTable->insert( "UpperTranslation" );
static StringTableEntry jointPrismaticMotorSpeedName      = jointRevoluteMotorSpeedName;
static StringTableEntry jointPrismaticMotorMaxForceName   = jointFrictionMaxForceName;

static StringTableEntry jointPulleyNodeName               = StringTable->insert( "Pulley" );
static StringTableEntry jointPulleyGroundAnchorAName      = StringTable->insert( "GroundAnchorA" );
static StringTableEntry jointPulleyGroundAnchorBName      = StringTable->insert( "GroundAnchorB" );
static StringTableEntry jointPulleyLengthAName            = StringTable->insert( "LengthA" );
static StringTableEntry jointPulleyLengthBName            = StringTable->insert( "LengthB" );
static StringTableEntry jointPulleyRatioName              = StringTable->insert( "Ratio" );

static StringTableEntry jointTargetNodeName               = StringTable->insert( "Target" );
static StringTableEntry jointTargetWorldTargetName        = StringTable->insert( "WorldTarget" );
static StringTableEntry jointTargetMaxForceName           = StringTable->insert( jointFrictionMaxForceName );
static StringTableEntry jointTargetFrequencyName          = jointDistanceFrequencyName;
static StringTableEntry jointTargetDampingRatioName       = jointDistanceDampingRatioName;

static StringTableEntry jointMotorNodeName                = StringTable->insert( "Motor" );
static StringTableEntry jointMotorLinearOffsetName        = StringTable->insert( "LinearOffset" );
static StringTableEntry jointMotorAngularOffsetName       = StringTable->insert( "AngularOffset" );

static StringTableEntry jointMotorMaxForceName            = jointFrictionMaxForceName;
static StringTableEntry jointMotorMaxTorqueName           = jointRevoluteMotorMaxTorqueName;
static StringTableEntry jointMotorCorrectionFactorName    = StringTable->insert( "CorrectionFactor" );

// Controller custom node names.
static StringTableEntry controllerCustomNodeName          = StringTable->insert( "Controllers" );

// Asset preload custom node names.
static StringTableEntry assetPreloadNodeName              = StringTable->insert( "AssetPreloads" );
static StringTableEntry assetNodeName                     = StringTable->insert( "Asset" );

//-----------------------------------------------------------------------------

Scene::Scene() :
    /// World.
    mpWorld(NULL),
    mWorldGravity(0.0f, 0.0f),
    mVelocityIterations(8),
    mPositionIterations(3),

    /// Joint access.
    mJointMasterId(1),

    /// Scene time.
    mSceneTime(0.0f),
    mScenePause(false),

    /// Debug and metrics.
    mDebugMask(0X00000000),
    mpDebugSceneObject(NULL),

    /// Window rendering.
    mpCurrentRenderWindow(NULL),
    
    /// Miscellaneous.
    mIsEditorScene(0),
    mUpdateCallback(false),
    mRenderCallback(false),
    mSceneIndex(0)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mSceneObjects );
    VECTOR_SET_ASSOCIATION( mDeleteRequests );
    VECTOR_SET_ASSOCIATION( mDeleteRequestsTemp );
    VECTOR_SET_ASSOCIATION( mEndContacts );
    VECTOR_SET_ASSOCIATION( mAssetPreloads );
     
    // Initialize layer sort mode.
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; ++n )
       mLayerSortModes[n] = SceneRenderQueue::RENDER_SORT_NEWEST;

    // Set debug stats for batch renderer.
    mBatchRenderer.setDebugStats( &mDebugStats );

    // Register the scene controllers set.
    mControllers = new SimSet();
    mControllers->registerObject();

    // Assign scene index.    
    mSceneIndex = ++sSceneMasterIndex;
    sSceneCount++;
}

//-----------------------------------------------------------------------------

Scene::~Scene()
{
    // Unregister the scene controllers set.
    if ( mControllers.notNull() )
        mControllers->deleteObject();

    // Decrease scene count.
    --sSceneCount;
}

//-----------------------------------------------------------------------------

bool Scene::onAdd()
{
    // Call Parent.
    if(!Parent::onAdd())
        return false;

    // Create physics world.
    mpWorld = new b2World( mWorldGravity );

    // Set contact filter.
    mpWorld->SetContactFilter( &mContactFilter );

    const b2ParticleSystemDef particleSystemDef;
    mParticleSystem = mpWorld->CreateParticleSystem(&particleSystemDef);

    // Set contact listener.
    mpWorld->SetContactListener( this );

    // Set destruction listener.
    mpWorld->SetDestructionListener( this );

    // Create ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.userData = static_cast<PhysicsProxy*>(this);
    mpGroundBody = mpWorld->CreateBody(&groundBodyDef);
    mpGroundBody->SetAwake( false );

    // Create world query.
    mpWorldQuery = new WorldQuery(this);

    // Set loading scene.
    Scene::LoadingScene = this;

    // Turn-on tick processing.
    setProcessTicks( true );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void Scene::onRemove()
{
    // Turn-off tick processing.
    setProcessTicks( false );

    // Clear Scene.
    clearScene();

    // Process Delete Requests.
    processDeleteRequests(true);

    // Delete ground body.
    mpWorld->DestroyBody( mpGroundBody );
    mpGroundBody = NULL;

    // Delete physics world and world query.
    delete mpWorldQuery;
    delete mpWorld;
    mpWorldQuery = NULL;
    mpWorld = NULL;

    // Detach All Scene Windows.
    detachAllSceneWindows();

    // Call Parent. Clear scene handles all the object removal, so we can skip
    // that part and just do the sim-object stuff.
    SimObject::onRemove();
}

//-----------------------------------------------------------------------------

void Scene::onDeleteNotify( SimObject* object )
{
    // Ignore if we're not monitoring a debug banner scene object.
    if ( mpDebugSceneObject == NULL )
        return;

    // Ignore if it's not the one we're monitoring.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( object );
    if ( pSceneObject != mpDebugSceneObject )
        return;

    // Reset the monitored scene object.
    mpDebugSceneObject = NULL;
}

//-----------------------------------------------------------------------------

void Scene::initPersistFields()
{
    // Call Parent.
    Parent::initPersistFields();

    // Physics.
    addProtectedField("Gravity", TypeVector2, Offset(mWorldGravity, Scene), &setGravity, &getGravity, &writeGravity, "" );
    addField("VelocityIterations", TypeS32, Offset(mVelocityIterations, Scene), &writeVelocityIterations, "" );
    addField("PositionIterations", TypeS32, Offset(mPositionIterations, Scene), &writePositionIterations, "" );

    // Layer sort modes.
    char buffer[64];
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
       dSprintf( buffer, 64, "layerSortMode%d", n );
       addField( buffer, TypeEnum, OffsetNonConst(mLayerSortModes[n], Scene), &writeLayerSortMode, 1, &SceneRenderQueue::renderSortTable, "");
    }

    addProtectedField("Controllers", TypeSimObjectPtr, Offset(mControllers, Scene), &defaultProtectedNotSetFn, &defaultProtectedGetFn, &defaultProtectedNotWriteFn, "The scene controllers to use.");
    
    // Callbacks.
    addField("UpdateCallback", TypeBool, Offset(mUpdateCallback, Scene), &writeUpdateCallback, "");
    addField("RenderCallback", TypeBool, Offset(mRenderCallback, Scene), &writeRenderCallback, "");
}

//-----------------------------------------------------------------------------

void Scene::BeginContact( b2Contact* pContact )
{
    // Ignore contact if it's not a touching contact.
    if ( !pContact->IsTouching() )
        return;

    PROFILE_SCOPE(Scene_BeginContact);

    // Fetch fixtures.
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    // Fetch physics proxies.
    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pFixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pFixtureB->GetBody()->GetUserData());

    // Ignore stuff that's not a scene object.
    if (    pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
            pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
    {
            return;
    }

    // Fetch scene objects.
    SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // Initialize the contact.
    TickContact tickContact;
    tickContact.initialize( pContact, pSceneObjectA, pSceneObjectB, pFixtureA, pFixtureB );

    // Add contact.
    mBeginContacts.insert( pContact, tickContact );
}

//-----------------------------------------------------------------------------

void Scene::EndContact( b2Contact* pContact )
{
    PROFILE_SCOPE(Scene_EndContact);

    // Fetch fixtures.
    b2Fixture* pFixtureA = pContact->GetFixtureA();
    b2Fixture* pFixtureB = pContact->GetFixtureB();

    // Fetch physics proxies.
    PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pFixtureA->GetBody()->GetUserData());
    PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pFixtureB->GetBody()->GetUserData());

    // Ignore stuff that's not a scene object.
    if (    pPhysicsProxyA->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ||
            pPhysicsProxyB->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
    {
            return;
    }

    // Fetch scene objects.
    SceneObject* pSceneObjectA = static_cast<SceneObject*>(pPhysicsProxyA);
    SceneObject* pSceneObjectB = static_cast<SceneObject*>(pPhysicsProxyB);

    // Initialize the contact.
    TickContact tickContact;
    tickContact.initialize( pContact, pSceneObjectA, pSceneObjectB, pFixtureA, pFixtureB );

    // Add contact.
    mEndContacts.push_back( tickContact );
}

//-----------------------------------------------------------------------------

void Scene::PostSolve( b2Contact* pContact, const b2ContactImpulse* pImpulse )
{
    // Find contact mapping.
    typeContactHash::iterator contactItr = mBeginContacts.find( pContact );

    // Finish if we didn't find the contact.
    if ( contactItr == mBeginContacts.end() )
        return;

    // Fetch contact.
    TickContact& tickContact = contactItr->value;

    // Add the impulse.
    for ( U32 index = 0; index < b2_maxManifoldPoints; ++index )
    {
        tickContact.mNormalImpulses[index] += pImpulse->normalImpulses[index];
        tickContact.mTangentImpulses[index] += pImpulse->tangentImpulses[index];
    }
}

//-----------------------------------------------------------------------------

void Scene::forwardContacts( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_ForwardContacts);

    // Iterate end contacts.
    for( typeContactVector::iterator contactItr = mEndContacts.begin(); contactItr != mEndContacts.end(); ++contactItr )
    {
        // Fetch tick contact.
        TickContact& tickContact = *contactItr;

        // Inform the scene objects.
        tickContact.mpSceneObjectA->onEndCollision( tickContact );
        tickContact.mpSceneObjectB->onEndCollision( tickContact );
    }

    // Iterate begin contacts.
    for( typeContactHash::iterator contactItr = mBeginContacts.begin(); contactItr != mBeginContacts.end(); ++contactItr )
    {
        // Fetch tick contact.
        TickContact& tickContact = contactItr->value;

        // Inform the scene objects.
        tickContact.mpSceneObjectA->onBeginCollision( tickContact );
        tickContact.mpSceneObjectB->onBeginCollision( tickContact );
    }
}

//-----------------------------------------------------------------------------

void Scene::dispatchBeginContactCallbacks( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_DispatchBeginContactCallbacks);

    // Sanity!
    AssertFatal( b2_maxManifoldPoints == 2, "Scene::dispatchBeginContactCallbacks() - Invalid assumption about max manifold points." );

    // Fetch contact count.
    const U32 contactCount = mBeginContacts.size();

    // Finish if no contacts.
    if ( contactCount == 0 )
        return;

    // Iterate all contacts.
    for ( typeContactHash::iterator contactItr = mBeginContacts.begin(); contactItr != mBeginContacts.end(); ++contactItr )
    {
        // Fetch contact.
        const TickContact& tickContact = contactItr->value;

        // Fetch scene objects.
        SceneObject* pSceneObjectA = tickContact.mpSceneObjectA;
        SceneObject* pSceneObjectB = tickContact.mpSceneObjectB;

        // Skip if either object is being deleted.
        if ( pSceneObjectA->isBeingDeleted() || pSceneObjectB->isBeingDeleted() )
            continue;

        // Skip if both objects don't have collision callback active.
        if ( !pSceneObjectA->getCollisionCallback() && !pSceneObjectB->getCollisionCallback() )
            continue;

        // Fetch normal and contact points.
        const U32& pointCount = tickContact.mPointCount;
        const b2Vec2& normal = tickContact.mWorldManifold.normal;
        const b2Vec2& point1 = tickContact.mWorldManifold.points[0];
        const b2Vec2& point2 = tickContact.mWorldManifold.points[1];
        const S32 shapeIndexA = pSceneObjectA->getCollisionShapeIndex( tickContact.mpFixtureA );
        const S32 shapeIndexB = pSceneObjectB->getCollisionShapeIndex( tickContact.mpFixtureB );

        // Sanity!
        AssertFatal( shapeIndexA >= 0, "Scene::dispatchBeginContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );
        AssertFatal( shapeIndexB >= 0, "Scene::dispatchBeginContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );

        // Fetch collision impulse information
        const F32 normalImpulse1 = tickContact.mNormalImpulses[0];
        const F32 normalImpulse2 = tickContact.mNormalImpulses[1];
        const F32 tangentImpulse1 = tickContact.mTangentImpulses[0];
        const F32 tangentImpulse2 = tickContact.mTangentImpulses[1];

        // Format objects.
        char sceneObjectABuffer[16];
        char sceneObjectBBuffer[16];
        dSprintf( sceneObjectABuffer, sizeof(sceneObjectABuffer), "%d", pSceneObjectA->getId() );
        dSprintf( sceneObjectBBuffer, sizeof(sceneObjectBBuffer), "%d", pSceneObjectB->getId() );

        // Format miscellaneous information twice so object b can see things from his point of view.
        char miscInfoBufferA[128];
        char miscInfoBufferB[128];
        if ( pointCount == 2 )
        {
            dSprintf(miscInfoBufferA, sizeof(miscInfoBufferA),
                "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
                shapeIndexA, shapeIndexB,
                -normal.x, -normal.y,
                point1.x, point1.y,
                normalImpulse1,
                tangentImpulse1,
                point2.x, point2.y,
                normalImpulse2,
                tangentImpulse2 );

            dSprintf(miscInfoBufferB, sizeof(miscInfoBufferB),
               "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
               shapeIndexB, shapeIndexA,
               normal.x, normal.y,
               point1.x, point1.y,
               normalImpulse1,
               tangentImpulse1,
               point2.x, point2.y,
               normalImpulse2,
               tangentImpulse2);
        }
        else if ( pointCount == 1 )
        {
            dSprintf(miscInfoBufferA, sizeof(miscInfoBufferA),
                "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
                shapeIndexA, shapeIndexB,
                -normal.x, -normal.y,
                point1.x, point1.y,
                normalImpulse1,
                tangentImpulse1 );

            dSprintf(miscInfoBufferB, sizeof(miscInfoBufferB),
               "%d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
               shapeIndexB, shapeIndexA,
               normal.x, normal.y,
               point1.x, point1.y,
               normalImpulse1,
               tangentImpulse1);
        }
        else
        {
            dSprintf(miscInfoBufferA, sizeof(miscInfoBufferA),
                "%d %d",
                shapeIndexA, shapeIndexB );

            dSprintf(miscInfoBufferB, sizeof(miscInfoBufferB),
               "%d %d",
               shapeIndexB, shapeIndexA);
        }

        // Does the scene handle the collision callback?
        Namespace* pNamespace = getNamespace();
        if ( pNamespace != NULL && pNamespace->lookup( StringTable->insert( "onSceneCollision" ) ) != NULL )
        {
            // Yes, so perform script callback on the Scene.
            Con::executef( this, 4, "onSceneCollision",
                sceneObjectABuffer,
                sceneObjectBBuffer,
                miscInfoBufferA );
        }
        else
        {
            // No, so call it on its behaviors.
            const char* args[5] = { "onSceneCollision", "", sceneObjectABuffer, sceneObjectBBuffer, miscInfoBufferA };
            callOnBehaviors( 5, args );
        }

        // Is object A allowed to collide with object B?
        if (    (pSceneObjectA->mCollisionGroupMask & pSceneObjectB->mSceneGroupMask) != 0 &&
                (pSceneObjectA->mCollisionLayerMask & pSceneObjectB->mSceneLayerMask) != 0 )
        {
            // Yes, so does it handle the collision callback?
            if ( pSceneObjectA->isMethod("onCollision") )            
            {
                // Yes, so perform the script callback on it.
                Con::executef( pSceneObjectA, 3, "onCollision",
                    sceneObjectBBuffer,
                    miscInfoBufferA );
            }
            else
            {
                // No, so call it on its behaviors.
                const char* args[4] = { "onCollision", "", sceneObjectBBuffer, miscInfoBufferA };
                pSceneObjectA->callOnBehaviors( 4, args );
            }
        }

        // Is object B allowed to collide with object A?
        if (    (pSceneObjectB->mCollisionGroupMask & pSceneObjectA->mSceneGroupMask) != 0 &&
                (pSceneObjectB->mCollisionLayerMask & pSceneObjectA->mSceneLayerMask) != 0 )
        {
            // Yes, so does it handle the collision callback?
            if ( pSceneObjectB->isMethod("onCollision") )            
            {
                // Yes, so perform the script callback on it.
                Con::executef( pSceneObjectB, 3, "onCollision",
                    sceneObjectABuffer,
                    miscInfoBufferB );
            }
            else
            {
                // No, so call it on its behaviors.
                const char* args[4] = { "onCollision", "", sceneObjectABuffer, miscInfoBufferB };
                pSceneObjectB->callOnBehaviors( 4, args );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::dispatchEndContactCallbacks( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_DispatchEndContactCallbacks);

    // Sanity!
    AssertFatal( b2_maxManifoldPoints == 2, "Scene::dispatchEndContactCallbacks() - Invalid assumption about max manifold points." );

    // Fetch contact count.
    const U32 contactCount = mEndContacts.size();

    // Finish if no contacts.
    if ( contactCount == 0 )
        return;

    // Iterate all contacts.
    for ( typeContactVector::iterator contactItr = mEndContacts.begin(); contactItr != mEndContacts.end(); ++contactItr )
    {
        // Fetch contact.
        const TickContact& tickContact = *contactItr;

        // Fetch scene objects.
        SceneObject* pSceneObjectA = tickContact.mpSceneObjectA;
        SceneObject* pSceneObjectB = tickContact.mpSceneObjectB;

        // Skip if either object is being deleted.
        if ( pSceneObjectA->isBeingDeleted() || pSceneObjectB->isBeingDeleted() )
            continue;

        // Skip if both objects don't have collision callback active.
        if ( !pSceneObjectA->getCollisionCallback() && !pSceneObjectB->getCollisionCallback() )
            continue;

        // Fetch shape index.
        const S32 shapeIndexA = pSceneObjectA->getCollisionShapeIndex( tickContact.mpFixtureA );
        const S32 shapeIndexB = pSceneObjectB->getCollisionShapeIndex( tickContact.mpFixtureB );

        // Sanity!
        AssertFatal( shapeIndexA >= 0, "Scene::dispatchEndContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );
        AssertFatal( shapeIndexB >= 0, "Scene::dispatchEndContactCallbacks() - Cannot find shape index reported on physics proxy of a fixture." );

        // Format objects.
        char sceneObjectABuffer[16];
        char sceneObjectBBuffer[16];
        dSprintf( sceneObjectABuffer, sizeof(sceneObjectABuffer), "%d", pSceneObjectA->getId() );
        dSprintf( sceneObjectBBuffer, sizeof(sceneObjectBBuffer), "%d", pSceneObjectB->getId() );

        // Format miscellaneous information.
        char miscInfoBuffer[32];
        dSprintf(miscInfoBuffer, sizeof(miscInfoBuffer), "%d %d", shapeIndexA, shapeIndexB );

        // Does the scene handle the collision callback?
        Namespace* pNamespace = getNamespace();
        if ( pNamespace != NULL && pNamespace->lookup( StringTable->insert( "onSceneEndCollision" ) ) != NULL )
        {
            // Yes, so does the scene handle the collision callback?
            Con::executef( this, 4, "onSceneEndCollision",
                sceneObjectABuffer,
                sceneObjectBBuffer,
                miscInfoBuffer );
        }
        else
        {
            // No, so call it on its behaviors.
            const char* args[5] = { "onSceneEndCollision", "", sceneObjectABuffer, sceneObjectBBuffer, miscInfoBuffer };
            callOnBehaviors( 5, args );
        }

        // Is object A allowed to collide with object B?
        if (    (pSceneObjectA->mCollisionGroupMask & pSceneObjectB->mSceneGroupMask) != 0 &&
                (pSceneObjectA->mCollisionLayerMask & pSceneObjectB->mSceneLayerMask) != 0 )
        {
            // Yes, so does it handle the collision callback?
            if ( pSceneObjectA->isMethod("onEndCollision") )            
            {
                // Yes, so perform the script callback on it.
                Con::executef( pSceneObjectA, 3, "onEndCollision",
                    sceneObjectBBuffer,
                    miscInfoBuffer );
            }
            else
            {
                // No, so call it on its behaviors.
                const char* args[4] = { "onEndCollision", "", sceneObjectBBuffer, miscInfoBuffer };
                pSceneObjectA->callOnBehaviors( 4, args );
            }
        }

        // Is object B allowed to collide with object A?
        if (    (pSceneObjectB->mCollisionGroupMask & pSceneObjectA->mSceneGroupMask) != 0 &&
                (pSceneObjectB->mCollisionLayerMask & pSceneObjectA->mSceneLayerMask) != 0 )
        {
            // Yes, so does it handle the collision callback?
            if ( pSceneObjectB->isMethod("onEndCollision") )            
            {
                // Yes, so perform the script callback on it.
                Con::executef( pSceneObjectB, 3, "onEndCollision",
                    sceneObjectABuffer,
                    miscInfoBuffer );
            }
            else
            {
                // No, so call it on its behaviors.
                const char* args[4] = { "onEndCollision", "", sceneObjectABuffer, miscInfoBuffer };
                pSceneObjectB->callOnBehaviors( 4, args );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::processTick( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_ProcessTick);

    // Finish if the Scene is not added to the simulation.
    if ( !isProperlyAdded() )
        return;

    // Process Delete Requests.
    processDeleteRequests(false);

    // Update debug stats.
    mDebugStats.fps           = Con::getFloatVariable("fps::framePeriod", 0.0f);
    mDebugStats.frameCount    = (U32)Con::getIntVariable("fps::frameCount", 0);
    mDebugStats.bodyCount     = (U32)mpWorld->GetBodyCount();
    mDebugStats.jointCount    = (U32)mpWorld->GetJointCount();
    mDebugStats.contactCount  = (U32)mpWorld->GetContactCount();
    mDebugStats.proxyCount    = (U32)mpWorld->GetProxyCount();
    mDebugStats.objectsCount  = (U32)mSceneObjects.size();
    mDebugStats.worldProfile  = mpWorld->GetProfile();

    // Set particle stats.
    mDebugStats.particlesAlloc = ParticleSystem::Instance->getAllocatedParticleCount();
    mDebugStats.particlesUsed = ParticleSystem::Instance->getActiveParticleCount();
    mDebugStats.particlesFree = mDebugStats.particlesAlloc - mDebugStats.particlesUsed;

    // Finish if scene is paused.
    if ( !getScenePause() )
    {
        // Reset object stats.
        U32 objectsEnabled = 0;
        U32 objectsVisible = 0;
        U32 objectsAwake   = 0;

        // Fetch if a "normal" i.e. non-editor scene.
        const bool isNormalScene = !getIsEditorScene();

        // Update scene time.
        mSceneTime += Tickable::smTickSec;

        // Clear ticked scene objects.
        mTickedSceneObjects.clear();

        // Iterate scene objects.
        for( S32 n = 0; n < mSceneObjects.size(); ++n )
        {
            // Fetch scene object.
            SceneObject* pSceneObject = mSceneObjects[n];

            // Update awake/asleep counts.
            if ( pSceneObject->getAwake() )
                objectsAwake++;

            // Update visible.
            if ( pSceneObject->getVisible() )
                objectsVisible++;

            // Push scene object if it's eligible for ticking.
            if ( pSceneObject->isEnabled() )
            {
                // Update enabled.
                objectsEnabled++;

                // Add to ticked objects if object is not being deleted and this is a "normal" scene or
                // the object is marked as allowing editor ticks.
                if ( !pSceneObject->isBeingDeleted() && (isNormalScene || pSceneObject->getIsEditorTickAllowed() )  )
                    mTickedSceneObjects.push_back( pSceneObject );
            }
        }

        // Update object stats.
        mDebugStats.objectsEnabled = objectsEnabled;
        mDebugStats.objectsVisible = objectsVisible;
        mDebugStats.objectsAwake   = objectsAwake;

        // Debug Status Reference.
        DebugStats* pDebugStats = &mDebugStats;

        // Fetch ticked scene object count.
        const S32 tickedSceneObjectCount = mTickedSceneObjects.size();

        // ****************************************************
        // Pre-integrate objects.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 i = 0; i < tickedSceneObjectCount; ++i )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_PreIntegrate);

            // Pre-integrate.
            mTickedSceneObjects[i]->preIntegrate( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // ****************************************************
        // Integrate controllers.
        // ****************************************************

        // Fetch the controller set.
        SimSet* pControllerSet = getControllers();

        // Do we have any scene controllers?
        if ( pControllerSet != NULL )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_IntegrateSceneControllers);

            // Yes, so fetch scene controller count.
            const S32 sceneControllerCount = (S32)pControllerSet->size();

            // Iterate scene controllers.
            for( S32 i = 0; i < sceneControllerCount; i++ )
            {
                // Fetch the scene controller.
                SceneController* pController = dynamic_cast<SceneController*>((*pControllerSet)[i]);

                // Skip if not a controller.
                if ( pController == NULL )
                    continue;

                // Integrate.
                pController->integrate( this, mSceneTime, Tickable::smTickSec, pDebugStats );
            }
        }

        // Debug Profiling.
        PROFILE_START(Scene_IntegratePhysicsSystem);

        // Reset contacts.
        mBeginContacts.clear();
        mEndContacts.clear();

        // Only step the physics if a "normal" scene.
        if ( isNormalScene )
        {
            // Step the physics.
            mpWorld->Step( Tickable::smTickSec, mVelocityIterations, mPositionIterations );
        }

        // Debug Profiling.
        PROFILE_END();   // Scene_IntegratePhysicsSystem

        // Forward the contacts.
        forwardContacts();

        // ****************************************************
        // Integrate objects.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 i = 0; i < tickedSceneObjectCount; ++i )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_IntegrateObject);

            // Integrate.
            mTickedSceneObjects[i]->integrateObject( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // ****************************************************
        // Post-Integrate Stage.
        // ****************************************************

        // Iterate ticked scene objects.
        for ( S32 i = 0; i < tickedSceneObjectCount; ++i )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_PostIntegrate);

            // Post-integrate.
            mTickedSceneObjects[i]->postIntegrate( mSceneTime, Tickable::smTickSec, pDebugStats );
        }

        // Scene update callback.
        if( mUpdateCallback )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_OnSceneUpdatetCallback);

            Con::executef( this, 1, "onSceneUpdate" );
        }

        // Only dispatch contacts if a "normal" scene.
        if ( isNormalScene )
        {
            // Dispatch contacts callbacks.
            dispatchEndContactCallbacks();
            dispatchBeginContactCallbacks();
        }

        // Clear ticked scene objects.
        mTickedSceneObjects.clear();
    }

    // Update debug stat ranges.
    mDebugStats.updateRanges();
}

//-----------------------------------------------------------------------------

void Scene::interpolateTick( F32 timeDelta )
{
    // Finish if scene is paused.
    if ( getScenePause() ) return;

    // Debug Profiling.
    PROFILE_SCOPE(Scene_InterpolateTick);

    // ****************************************************
    // Interpolate scene objects.
    // ****************************************************

    // Fetch the scene object count.
    const S32 sceneObjectCount = mSceneObjects.size();

    // Iterate scene objects.
    for( S32 n = 0; n < sceneObjectCount; ++n )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mSceneObjects[n];

        // Skip interpolation of scene object if it's not eligible.
        if ( !pSceneObject->isEnabled() || pSceneObject->isBeingDeleted() )
            continue;

        pSceneObject->interpolateObject( timeDelta );
    }
}

//-----------------------------------------------------------------------------

void Scene::sceneRender( const SceneRenderState* pSceneRenderState )
{
    // Debug Profiling.
    PROFILE_SCOPE(Scene_RenderSceneTotal);

    // Fetch debug stats.
    DebugStats* pDebugStats = pSceneRenderState->mpDebugStats;

    // Reset Render Stats.
    pDebugStats->renderPicked                   = 0;
    pDebugStats->renderRequests                 = 0;
    pDebugStats->renderFallbacks                = 0;
    pDebugStats->batchTrianglesSubmitted        = 0;
    pDebugStats->batchDrawCallsStrict           = 0;
    pDebugStats->batchDrawCallsSorted           = 0;
    pDebugStats->batchFlushes                   = 0;
    pDebugStats->batchBlendStateFlush           = 0;
    pDebugStats->batchColorStateFlush           = 0;
    pDebugStats->batchAlphaStateFlush           = 0;
    pDebugStats->batchTextureChangeFlush        = 0;
    pDebugStats->batchBufferFullFlush           = 0;
    pDebugStats->batchIsolatedFlush             = 0;
    pDebugStats->batchLayerFlush                = 0;
    pDebugStats->batchNoBatchFlush              = 0;
    pDebugStats->batchAnonymousFlush            = 0;

    // Set batch renderer wireframe mode.
    mBatchRenderer.setWireframeMode( getDebugMask() & SCENE_DEBUG_WIREFRAME_RENDER );

    // Debug Profiling.
    PROFILE_START(Scene_RenderSceneVisibleQuery);

    // Rotate the render AABB by the camera angle.
    b2AABB cameraAABB;
    CoreMath::mRotateAABB( pSceneRenderState->mRenderAABB, pSceneRenderState->mRenderAngle, cameraAABB );

    // Rotate the world matrix by the camera angle.
    const Vector2& cameraPosition = pSceneRenderState->mRenderPosition;
    glTranslatef( cameraPosition.x, cameraPosition.y, 0.0f );
    glRotatef( mRadToDeg(pSceneRenderState->mRenderAngle), 0.0f, 0.0f, 1.0f );
    glTranslatef( -cameraPosition.x, -cameraPosition.y, 0.0f );

    // Clear world query.
    mpWorldQuery->clearQuery();

    // Set filter.
    WorldQueryFilter queryFilter( pSceneRenderState->mRenderLayerMask, pSceneRenderState->mRenderGroupMask, true, true, false, false );
    mpWorldQuery->setQueryFilter( queryFilter );

    // Query render AABB.
    mpWorldQuery->aabbQueryAABB( cameraAABB );

    // Debug Profiling.
    PROFILE_END();  //Scene_RenderSceneVisibleQuery

    // Are there any query results?
    if ( mpWorldQuery->getQueryResultsCount() > 0 )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_RenderSceneCompileRenderRequests);

        // Fetch the primary scene render queue.
        SceneRenderQueue* pSceneRenderQueue = SceneRenderQueueFactory.createObject();      

        // Yes so step through layers.
        for ( S32 layer = MAX_LAYERS_SUPPORTED-1; layer >= 0 ; layer-- )
        {
            // Fetch layer.
            typeWorldQueryResultVector& layerResults = mpWorldQuery->getLayeredQueryResults( layer );

            // Fetch layer object count.
            const U32 layerObjectCount = layerResults.size();

            // Are there any objects to render in this layer?
            if ( layerObjectCount > 0 )
            {
                // Yes, so increase render picked.
                pDebugStats->renderPicked += layerObjectCount;

                // Iterate query results.
                for( typeWorldQueryResultVector::iterator worldQueryItr = layerResults.begin(); worldQueryItr != layerResults.end(); ++worldQueryItr )
                {
                    // Fetch scene object.
                    SceneObject* pSceneObject = worldQueryItr->mpSceneObject;

                    // Skip if the object should not render.
                    if ( !pSceneObject->shouldRender() )
                        continue;

                    // Can the scene object prepare a render?
                    if ( pSceneObject->canPrepareRender() )
                    {
                        // Yes. so is it batch isolated.
                        if ( pSceneObject->getBatchIsolated() )
                        {
                            // Yes, so create a default render request  on the primary queue.
                            SceneRenderRequest* pIsolatedSceneRenderRequest = Scene::createDefaultRenderRequest( pSceneRenderQueue, pSceneObject );

                            // Create a new isolated render queue.
                            pIsolatedSceneRenderRequest->mpIsolatedRenderQueue = SceneRenderQueueFactory.createObject();

                            // Prepare in the isolated queue.
                            pSceneObject->scenePrepareRender( pSceneRenderState, pIsolatedSceneRenderRequest->mpIsolatedRenderQueue );

                            // Increase render request count.
                            pDebugStats->renderRequests += (U32)pIsolatedSceneRenderRequest->mpIsolatedRenderQueue->getRenderRequests().size();

                            // Adjust for the extra private render request.
                            pDebugStats->renderRequests -= 1;
                        }
                        else
                        {
                            // No, so prepare in primary queue.
                            pSceneObject->scenePrepareRender( pSceneRenderState, pSceneRenderQueue );
                        }
                    }
                    else
                    {
                        // No, so create a default render request for it.
                        Scene::createDefaultRenderRequest( pSceneRenderQueue, pSceneObject );
                    }
                }

                // Fetch render requests.
                SceneRenderQueue::typeRenderRequestVector& sceneRenderRequests = pSceneRenderQueue->getRenderRequests();

                // Fetch render request count.
                const U32 renderRequestCount = (U32)sceneRenderRequests.size();

                // Increase render request count.
                pDebugStats->renderRequests += renderRequestCount;

                // Do we have more than a single render request?
                if ( renderRequestCount > 1 )
                {
                    // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderSceneLayerSorting);

                    // Yes, so fetch layer sort mode.
                    SceneRenderQueue::RenderSort& mode = mLayerSortModes[layer];

                    // Temporarily switch to normal sort if batch sort but batcher disabled.
                    if ( !mBatchRenderer.getBatchEnabled() && mode == SceneRenderQueue::RENDER_SORT_BATCH )
                        mode = SceneRenderQueue::RENDER_SORT_NEWEST;

                    // Set render queue mode.
                    pSceneRenderQueue->setSortMode( mode );

                    // Sort the render requests.
                    pSceneRenderQueue->sort();
                }

                // Iterate render requests.
                for( SceneRenderQueue::typeRenderRequestVector::iterator renderRequestItr = sceneRenderRequests.begin(); renderRequestItr != sceneRenderRequests.end(); ++renderRequestItr )
                {
                     // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderSceneRequests);

                    // Fetch render request.
                    SceneRenderRequest* pSceneRenderRequest = *renderRequestItr;

                    // Fetch scene render object.
                    SceneRenderObject* pSceneRenderObject = pSceneRenderRequest->mpSceneRenderObject;
             
                    // Flush if the object is not render batched and we're in strict order mode.
                    if ( !pSceneRenderObject->isBatchRendered() && mBatchRenderer.getStrictOrderMode() )
                    {
                        mBatchRenderer.flush( pDebugStats->batchNoBatchFlush );
                    }
                    // Flush if the object is batch isolated.
                    else if ( pSceneRenderObject->getBatchIsolated() )
                    {
                        mBatchRenderer.flush( pDebugStats->batchIsolatedFlush );
                    }

                    // Yes, so is the object batch rendered?
                    if ( pSceneRenderObject->isBatchRendered() )
                    {
                        // Yes, so set the blend mode.
                        mBatchRenderer.setBlendMode( pSceneRenderRequest );

                        // Set the alpha test mode.
                        mBatchRenderer.setAlphaTestMode( pSceneRenderRequest );
                    }

                    // Set batch strict order mode.
                    // NOTE:    We keep reasserting this because an object is free to change it during rendering.
                    mBatchRenderer.setStrictOrderMode( pSceneRenderQueue->getStrictOrderMode() );

                    // Is the object batch isolated?
                    if ( pSceneRenderObject->getBatchIsolated() )
                    {
                        // Yes, so fetch isolated render queue.
                        SceneRenderQueue* pIsolatedRenderQueue = pSceneRenderRequest->mpIsolatedRenderQueue;

                        // Sanity!
                        AssertFatal( pIsolatedRenderQueue != NULL, "Cannot render batch isolated with an isolated render queue." );

                        // Sort the isolated render requests.
                        pIsolatedRenderQueue->sort();

                        // Fetch isolated render requests.
                        SceneRenderQueue::typeRenderRequestVector& isolatedRenderRequests = pIsolatedRenderQueue->getRenderRequests();

                        // Can the object render?
                        if ( pSceneRenderObject->validRender() )
                        {
                            // Yes, so iterate isolated render requests.
                            for( SceneRenderQueue::typeRenderRequestVector::iterator isolatedRenderRequestItr = isolatedRenderRequests.begin(); isolatedRenderRequestItr != isolatedRenderRequests.end(); ++isolatedRenderRequestItr )
                            {
                                pSceneRenderObject->sceneRender( pSceneRenderState, *isolatedRenderRequestItr, &mBatchRenderer );
                            }
                        }
                        else
                        {
                            // No, so iterate isolated render requests.
                            for( SceneRenderQueue::typeRenderRequestVector::iterator isolatedRenderRequestItr = isolatedRenderRequests.begin(); isolatedRenderRequestItr != isolatedRenderRequests.end(); ++isolatedRenderRequestItr )
                            {
                                pSceneRenderObject->sceneRenderFallback( pSceneRenderState, *isolatedRenderRequestItr, &mBatchRenderer );
                            }

                            // Increase render fallbacks.
                            pDebugStats->renderFallbacks++;
                        }

                        // Flush isolated batch.
                        mBatchRenderer.flush( pDebugStats->batchIsolatedFlush );
                    }
                    else
                    {
                        // No, so can the object render?
                        if ( pSceneRenderObject->validRender() )
                        {
                            // Yes, so render object.
                            pSceneRenderObject->sceneRender( pSceneRenderState, pSceneRenderRequest, &mBatchRenderer );
                        }
                        else
                        {
                            // No, so render using fallback.
                            pSceneRenderObject->sceneRenderFallback( pSceneRenderState, pSceneRenderRequest, &mBatchRenderer );

                            // Increase render fallbacks.
                            pDebugStats->renderFallbacks++;
                        }
                    }
                }

                // Flush.
                // NOTE:    We cannot batch between layers as we adhere to a strict layer render order.
                mBatchRenderer.flush( pDebugStats->batchLayerFlush );

                // Iterate query results.
                for( typeWorldQueryResultVector::iterator worldQueryItr = layerResults.begin(); worldQueryItr != layerResults.end(); ++worldQueryItr )
                {
                    // Debug Profiling.
                    PROFILE_SCOPE(Scene_RenderObjectOverlays);

                    // Fetch scene object.
                    SceneObject* pSceneObject = worldQueryItr->mpSceneObject;

                    // Render object overlay.
                    pSceneObject->sceneRenderOverlay( pSceneRenderState );
                }
            }

            // Reset render queue.
            pSceneRenderQueue->resetState();
        }

        // Cache render queue..
        SceneRenderQueueFactory.cacheObject( pSceneRenderQueue );
    }

    // Draw controllers.
    if ( getDebugMask() & Scene::SCENE_DEBUG_CONTROLLERS )
    {
        // Fetch the controller set.
        SimSet* pControllerSet = getControllers();

        // Do we have any scene controllers?
        if ( pControllerSet != NULL )
        {
            // Debug Profiling.
            PROFILE_SCOPE(Scene_RenderControllers);

            // Yes, so fetch scene controller count.
            const S32 sceneControllerCount = (S32)pControllerSet->size();

            // Iterate scene controllers.
            for( S32 i = 0; i < sceneControllerCount; i++ )
            {
                // Fetch the scene controller.
                SceneController* pController = dynamic_cast<SceneController*>((*pControllerSet)[i]);

                // Skip if not a controller.
                if ( pController == NULL )
                    continue;

                // Render the overlay.
                pController->renderOverlay( this, pSceneRenderState, &mBatchRenderer );
            }

            // Flush isolated batch.
            mBatchRenderer.flush( pDebugStats->batchIsolatedFlush );
        }
    }

    // Draw Joints.
    if ( getDebugMask() & Scene::SCENE_DEBUG_JOINTS )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_RenderSceneJointOverlays);

        mDebugDraw.DrawJoints( mpWorld );
    }

    // Update debug stat ranges.
    mDebugStats.updateRanges();

    // Are we using the render callback?
    if( mRenderCallback )
    {
        // Debug Profiling.
        PROFILE_SCOPE(Scene_OnSceneRendertCallback);

        // Yes, so perform callback.
        Con::executef( this, 1, "onSceneRender" );
    }
}

//-----------------------------------------------------------------------------

void Scene::clearScene( bool deleteObjects )
{
    while( mSceneObjects.size() > 0 )
    {
        // Fetch first scene object.
        SceneObject* pSceneObject = mSceneObjects[0];

        // Remove Object from Scene.
        removeFromScene( pSceneObject );

        // Queue Object for deletion.
        if ( deleteObjects )
            pSceneObject->safeDelete();
    }

    // Fetch the controller set.
    SimSet* pControllerSet = getControllers();

    // Do we have any scene controllers?
    if ( pControllerSet != NULL )
    {
        // Yes, so delete them all.
        while( pControllerSet->size() > 0 )
            pControllerSet->at(0)->deleteObject();
    }

    // Clear asset preloads.
    clearAssetPreloads();
}

//-----------------------------------------------------------------------------

void Scene::addToScene( SceneObject* pSceneObject )
{
    if ( pSceneObject == NULL )
        return;

    // Fetch current scene.
    Scene* pCurrentScene = pSceneObject->getScene();

    // Ignore if already in the scene.
    if ( pCurrentScene == this )
        return;

#if defined(TORQUE_DEBUG)
    // Sanity!
    for ( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        AssertFatal( mSceneObjects[n] != pSceneObject, "A scene object has become corrupt." );
    }
#endif

    // Check that object is not already in a scene.
    if ( pCurrentScene )
    {
        // Remove from scene.
        pCurrentScene->removeFromScene( pSceneObject );
    }

    // Add scene object.
    mSceneObjects.push_back( pSceneObject );

    // Register with the scene.
    pSceneObject->OnRegisterScene( this );

    // Perform callback only if properly added to the simulation.
    if ( pSceneObject->isProperlyAdded() )
    {
        Con::executef(pSceneObject, 2, "onAddToScene", getIdString());
    }
    else
    {
        // Warning.
        Con::warnf("Scene object added to scene but the object is not registered with the simulation.  No 'onAddToScene' can be performed!  Use Target scene.");
    }
}

//-----------------------------------------------------------------------------

void Scene::removeFromScene( SceneObject* pSceneObject )
{
    if ( pSceneObject == NULL )
        return;

    // Check if object is actually in a scene.
    if ( !pSceneObject->getScene() )
    {
        Con::warnf("Scene::removeFromScene() - Object '%s' is not in a scene!.", pSceneObject->getIdString());
        return;
    }

    // Remove as debug-object if set.
    if ( pSceneObject == getDebugSceneObject() )
        setDebugSceneObject( NULL );

    // Process Destroy Notifications.
    pSceneObject->processDestroyNotifications();

    // Dismount Any Camera.
    pSceneObject->dismountCamera();

    // Remove from the SceneWindow last pickers
    for( U32 i = 0; i < (U32)mAttachedSceneWindows.size(); ++i )
    {
        (dynamic_cast<SceneWindow*>(mAttachedSceneWindows[i]))->removeFromInputEventPick(pSceneObject);
    }

    // Unregister from scene.
    pSceneObject->OnUnregisterScene( this );

    // Find scene object and remove it quickly.
    for ( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        if ( mSceneObjects[n] == pSceneObject )
        {
            mSceneObjects.erase_fast( n );
            break;
        }
    }

    // Perform callback.
    Con::executef( pSceneObject, 2, "onRemoveFromScene", getIdString() );
}

//-----------------------------------------------------------------------------

SceneObject* Scene::getSceneObject( const U32 objectIndex ) const
{
    // Sanity!
    AssertFatal( objectIndex < getSceneObjectCount(), "Scene::getSceneObject() - Invalid object index." );

    return mSceneObjects[objectIndex];
}

//-----------------------------------------------------------------------------

U32 Scene::getSceneObjects( typeSceneObjectVector& objects ) const
{
    // No objects if scene is empty!
    if ( getSceneObjectCount() == 0 )
        return 0;

    // Merge with objects.
    objects.merge( mSceneObjects );

    return getSceneObjectCount();
}

//-----------------------------------------------------------------------------

U32 Scene::getSceneObjects( typeSceneObjectVector& objects, const U32 sceneLayer ) const
{
    // No objects if scene is empty!
    if ( getSceneObjectCount() == 0 )
        return 0;

    // Reset object count.
    U32 count = 0;

    // Iterate scene objects.
    for( S32 n = 0; n < mSceneObjects.size(); ++n )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = mSceneObjects[n];

        // Skip if not the correct layer.
        if ( pSceneObject->getSceneLayer() != sceneLayer )
            continue;

        // Add to objects.
        objects.push_back( pSceneObject );

        // Increase count.
        count++;
    }

    return count;
}

//-----------------------------------------------------------------------------

const AssetPtr<AssetBase>* Scene::getAssetPreload( const S32 index ) const
{
    // Is the index valid?
    if ( index < 0 || index >= mAssetPreloads.size() )
    {
        // Yes, so warn.
        Con::warnf( "Scene::getAssetPreload() - Out of range index '%d'.  There are only '%d' asset preloads.", index, mAssetPreloads.size() );
        return NULL;
    }

    return mAssetPreloads[index];
}

//-----------------------------------------------------------------------------

void Scene::addAssetPreload( const char* pAssetId )
{
    // Sanity!
    AssertFatal( pAssetId != NULL, "Scene::addAssetPreload() - Cannot add a NULL asset preload." );

    // Fetch asset Id.
    StringTableEntry assetId = StringTable->insert( pAssetId );

    // Ignore if asset already added.
    const S32 assetPreloadCount = mAssetPreloads.size();
    for( S32 index = 0; index < assetPreloadCount; ++index )
    {
        if ( mAssetPreloads[index]->getAssetId() == assetId )
            return;
    }

    // Create asset pointer.
    AssetPtr<AssetBase>* pAssetPtr = new AssetPtr<AssetBase>( pAssetId );

    // Was the asset acquired?
    if ( pAssetPtr->isNull() )
    {
        // No, so warn.
        Con::warnf( "Scene::addAssetPreload() - Failed to acquire asset '%s' so not added as a preload.", pAssetId );

        // No, so delete the asset pointer.
        delete pAssetPtr;
        return;
    }

    // Add asset.
    mAssetPreloads.push_back( pAssetPtr );
}

//-----------------------------------------------------------------------------

void Scene::removeAssetPreload( const char* pAssetId )
{
    // Sanity!
    AssertFatal( pAssetId != NULL, "Scene::removeAssetPreload() - Cannot remove a NULL asset preload." );

    // Fetch asset Id.
    StringTableEntry assetId = StringTable->insert( pAssetId );

    // Remove asset Id.
    const S32 assetPreloadCount = mAssetPreloads.size();
    for( S32 index = 0; index < assetPreloadCount; ++index )
    {
        if ( mAssetPreloads[index]->getAssetId() == assetId )
        {
            delete mAssetPreloads[index];
            mAssetPreloads.erase_fast( index );
            return;
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::clearAssetPreloads( void )
{
    // Delete all the asset preloads.
    while( mAssetPreloads.size() > 0 )
    {
        delete mAssetPreloads.back();
        mAssetPreloads.pop_back();
    }
}

//-----------------------------------------------------------------------------

void Scene::mergeScene( const Scene* pScene )
{
    // Fetch the scene object count.
    const U32 count = pScene->getSceneObjectCount();

    // Finish if there are not objects to copy.
    if ( count == 0 )
        return;


    for( U32 index = 0; index < count; ++index )
    {
        // Fetch a clone of the scene object.
        SceneObject* pSceneObject = (SceneObject*)pScene->getSceneObject( index )->clone( true );

        // Add the clone.
        addToScene( pSceneObject );
    }
}

//-----------------------------------------------------------------------------

b2Joint* Scene::findJoint( const S32 jointId )
{
    // Find joint.
    typeJointHash::iterator itr = mJoints.find( jointId );

    return itr == mJoints.end() ? NULL : itr->value;
}

//-----------------------------------------------------------------------------

b2JointType Scene::getJointType( const S32 jointId )
{
    // Sanity!
    if ( jointId >= mJointMasterId )
    {
        Con::warnf("The joint Id of %d is invalid.", jointId);
        return e_unknownJoint;
    }

    return findJoint( jointId )->GetType();
}

//-----------------------------------------------------------------------------

S32 Scene::findJointId( b2Joint* pJoint )
{
    // Sanity!
    AssertFatal( pJoint != NULL, "Joint cannot be NULL." );

    // Find joint.
    typeReverseJointHash::iterator itr = mReverseJoints.find( pJoint );

    if ( itr == mReverseJoints.end() )
    {
        Con::warnf("The joint Id could not be found via a joint reference of %x", pJoint);
        return 0;
    }

    return itr->value;
}

//-----------------------------------------------------------------------------

S32 Scene::createJoint( b2JointDef* pJointDef )
{
    // Sanity!
    AssertFatal( pJointDef != NULL, "Joint definition cannot be NULL." );

    // Create Joint.
    b2Joint* pJoint = mpWorld->CreateJoint( pJointDef );

    // Allocate joint Id.
    const S32 jointId = mJointMasterId++;

    // Insert joint.
    typeJointHash::iterator itr;
    itr = mJoints.insert( jointId, pJoint );

    // Sanity!
    AssertFatal( itr != mJoints.end(), "Joint already in hash table." );

    // Insert reverse joint.
    mReverseJoints.insert( pJoint, jointId );

    return jointId;
}

//-----------------------------------------------------------------------------

bool Scene::deleteJoint( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Finish if no joint.
    if ( pJoint == NULL )
        return false;

    // Destroy joint.
    // This should result in the joint references also being destroyed
    // as the scene is a destruction listener.
    mpWorld->DestroyJoint( pJoint );

    return true;
}

//-----------------------------------------------------------------------------

bool Scene::hasJoints( SceneObject* pSceneObject )
{
    // Sanity!
    AssertFatal( pSceneObject != NULL, "Scene object cannot be NULL!" );
    AssertFatal( pSceneObject->getScene() != this, "Scene object is not in this scene" );

    // Fetch body.
    b2Body* pBody = pSceneObject->getBody();

    // Fetch joint edge.
    b2JointEdge* pJointEdge = pBody->GetJointList();

    if ( pJointEdge == NULL || pJointEdge->joint == NULL )
        return false;

    // Found at least one joint.
    return true;
}

//-----------------------------------------------------------------------------

S32 Scene::createDistanceJoint(
    const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
    const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
    const F32 length,
    const F32 frequency,
    const F32 dampingRatio,
    const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createDistanceJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2DistanceJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.length           = length < 0.0f ? (pBodyB->GetWorldPoint( localAnchorB ) - pBodyA->GetWorldPoint( localAnchorA )).Length() : length;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointLength(
        const U32 jointId,
        const F32 length )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLength( length );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointLength( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetLength();
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setDistanceJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getDistanceJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_distanceJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2DistanceJoint* pRealJoint = static_cast<b2DistanceJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

S32 Scene::createRopeJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 maxLength,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createRopeJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2RopeJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.maxLength        = maxLength < 0.0f ? (pBodyB->GetWorldPoint( localAnchorB ) - pBodyA->GetWorldPoint( localAnchorA )).Length() : maxLength;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setRopeJointMaxLength(
        const U32 jointId,
        const F32 maxLength )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_ropeJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RopeJoint* pRealJoint = static_cast<b2RopeJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxLength( maxLength );
}

//-----------------------------------------------------------------------------

F32 Scene::getRopeJointMaxLength( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_ropeJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2RopeJoint* pRealJoint = static_cast<b2RopeJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxLength();
}

//-----------------------------------------------------------------------------

S32 Scene::createRevoluteJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createRevoluteJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2RevoluteJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setRevoluteJointLimit(
        const U32 jointId,
        const bool enableLimit,
        const F32 lowerAngle, const F32 upperAngle )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLimits( lowerAngle, upperAngle );
    pRealJoint->EnableLimit( enableLimit );
}

//-----------------------------------------------------------------------------

bool Scene::getRevoluteJointLimit(
        const U32 jointId,
        bool& enableLimit,
        F32& lowerAngle, F32& upperAngle )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    enableLimit = pRealJoint->IsLimitEnabled();
    lowerAngle = pRealJoint->GetLowerLimit();
    upperAngle = pRealJoint->GetUpperLimit();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setRevoluteJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorTorque( maxMotorTorque );
    pRealJoint->EnableMotor( enableMotor );    
}

//-----------------------------------------------------------------------------

bool Scene::getRevoluteJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorTorque = pRealJoint->GetMaxMotorTorque();

    return true;
}

//-----------------------------------------------------------------------------

F32 Scene::getRevoluteJointAngle( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return 0.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return 0.0f;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetJointAngle();
}

//-----------------------------------------------------------------------------

F32 Scene::getRevoluteJointSpeed( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return 0.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_revoluteJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return 0.0f;
    }

    // Cast joint.
    b2RevoluteJoint* pRealJoint = static_cast<b2RevoluteJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetJointSpeed();
}

//-----------------------------------------------------------------------------

S32 Scene::createWeldJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 frequency,
        const F32 dampingRatio,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createWeldJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2WeldJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setWeldJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}


//-----------------------------------------------------------------------------

F32 Scene::getWeldJointFrequency( const U32 jointId  )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setWeldJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getWeldJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_weldJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WeldJoint* pRealJoint = static_cast<b2WeldJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

S32 Scene::createWheelJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldAxis,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createWheelJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2WheelJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.localAxisA       = pBodyA->GetLocalVector( worldAxis );
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorTorque( maxMotorTorque );
    pRealJoint->EnableMotor( enableMotor ); 
}

//-----------------------------------------------------------------------------

bool Scene::getWheelJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorTorque = pRealJoint->GetMaxMotorTorque();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetSpringFrequencyHz( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getWheelJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetSpringFrequencyHz();
}

//-----------------------------------------------------------------------------

void Scene::setWheelJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetSpringDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getWheelJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_wheelJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2WheelJoint* pRealJoint = static_cast<b2WheelJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetSpringDampingRatio();
}

//-----------------------------------------------------------------------------

S32 Scene::createFrictionJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const F32 maxForce,
        const F32 maxTorque,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createFrictionJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2FrictionJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.maxForce         = maxForce;
    jointDef.maxTorque        = maxTorque;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setFrictionJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getFrictionJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setFrictionJointMaxTorque(
        const U32 jointId,
        const F32 maxTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxTorque( maxTorque );
}


//-----------------------------------------------------------------------------

F32 Scene::getFrictionJointMaxTorque( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_frictionJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2FrictionJoint* pRealJoint = static_cast<b2FrictionJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxTorque();
}

//-----------------------------------------------------------------------------

S32 Scene::createPrismaticJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldAxis,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createPrismaticJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2PrismaticJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.referenceAngle   = pBodyB->GetAngle() - pBodyA->GetAngle();
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.localAxisA       = pBodyA->GetLocalVector( worldAxis );
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setPrismaticJointLimit(
        const U32 jointId,
        const bool enableLimit,
        const F32 lowerTranslation, const F32 upperTranslation )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLimits( lowerTranslation, upperTranslation );
    pRealJoint->EnableLimit( enableLimit );
}

//-----------------------------------------------------------------------------

bool Scene::getPrismaticJointLimit(
        const U32 jointId,
        bool& enableLimit,
        F32& lowerTranslation, F32& upperTranslation )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    enableLimit = pRealJoint->IsLimitEnabled();
    lowerTranslation = pRealJoint->GetLowerLimit();
    upperTranslation = pRealJoint->GetUpperLimit();

    return true;
}

//-----------------------------------------------------------------------------

void Scene::setPrismaticJointMotor(
        const U32 jointId,
        const bool enableMotor,
        const F32 motorSpeed,
        const F32 maxMotorForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMotorSpeed( motorSpeed );
    pRealJoint->SetMaxMotorForce( maxMotorForce );
    pRealJoint->EnableMotor( enableMotor ); 
}

//-----------------------------------------------------------------------------

bool Scene::getPrismaticJointMotor(
        const U32 jointId,
        bool& enableMotor,
        F32& motorSpeed,
        F32& maxMotorForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return false;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_prismaticJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return false;
    }

    // Cast joint.
    b2PrismaticJoint* pRealJoint = static_cast<b2PrismaticJoint*>( pJoint );

    // Access joint.
    enableMotor = pRealJoint->IsMotorEnabled();
    motorSpeed = pRealJoint->GetMotorSpeed();
    maxMotorForce = pRealJoint->GetMaxMotorForce();

    return true;
}

//-----------------------------------------------------------------------------

S32 Scene::createPulleyJoint(
        const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
        const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
        const b2Vec2& worldGroundAnchorA, const b2Vec2& worldGroundAnchorB,
        const F32 ratio,
        const F32 lengthA, const F32 lengthB,
        const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createPulleyJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2PulleyJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = pBodyA;
    jointDef.bodyB            = pBodyB;
    jointDef.groundAnchorA    = worldGroundAnchorA;
    jointDef.groundAnchorB    = worldGroundAnchorB;
    jointDef.localAnchorA     = localAnchorA;
    jointDef.localAnchorB     = localAnchorB;
    jointDef.lengthA          = lengthA < 0.0f ? (pBodyA->GetWorldPoint( localAnchorA ) - worldGroundAnchorA).Length() : lengthA;
    jointDef.lengthB          = lengthB < 0.0f ? (pBodyA->GetWorldPoint( localAnchorB ) - worldGroundAnchorB).Length() : lengthB;
    jointDef.ratio            = ratio > b2_epsilon ? ratio : b2_epsilon + b2_epsilon;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

S32 Scene::createTargetJoint(
        const SceneObject* pSceneObject,
        const b2Vec2& worldTarget,
        const F32 maxForce,
        const bool useCenterOfMass,
        const F32 frequency,
        const F32 dampingRatio,
        const bool collideConnected )
{
    // Sanity!
    if ( pSceneObject != NULL && pSceneObject->getScene() == NULL )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for invalid object.
    if ( pSceneObject == NULL )
    {
        Con::warnf("Scene::createPulleyJoint() - Cannot create joint without a scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBody = pSceneObject->getBody();
    
    // Populate definition.
    b2MouseJointDef jointDef;
    jointDef.userData         = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected = collideConnected;
    jointDef.bodyA            = getGroundBody();
    jointDef.bodyB            = pBody;
    jointDef.target           = useCenterOfMass ? pBody->GetWorldCenter() : worldTarget;
    jointDef.maxForce         = maxForce;
    jointDef.frequencyHz      = frequency;
    jointDef.dampingRatio     = dampingRatio;

    // Create joint.
    const U32 jointId = createJoint( &jointDef );

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( findJoint( jointId ) );

    // Are we using the center of mass?
    if ( !useCenterOfMass )
    {
        // No, so set the target as the world target.
        // NOTE:-   This is done because initially the target (mouse) joint assumes the target 
        //          coincides with the body anchor.
        pRealJoint->SetTarget( worldTarget );
    }

    return jointId;
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointTarget(
        const U32 jointId,
        const b2Vec2& worldTarget )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetTarget( worldTarget );
}

//-----------------------------------------------------------------------------
b2Vec2 Scene::getTargetJointTarget( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return b2Vec2_zero;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return b2Vec2_zero;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetTarget();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointFrequency(
        const U32 jointId,
        const F32 frequency )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetFrequency( frequency );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointFrequency( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetFrequency();
}

//-----------------------------------------------------------------------------

void Scene::setTargetJointDampingRatio(
        const U32 jointId,
        const F32 dampingRatio )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetDampingRatio( dampingRatio );
}

//-----------------------------------------------------------------------------

F32 Scene::getTargetJointDampingRatio( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_mouseJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MouseJoint* pRealJoint = static_cast<b2MouseJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetDampingRatio();
}

//-----------------------------------------------------------------------------

S32 Scene::createMotorJoint(
            const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
            const b2Vec2 linearOffset,
            const F32 angularOffset,
            const F32 maxForce,
            const F32 maxTorque,
            const F32 correctionFactor,
            const bool collideConnected )
{
    // Sanity!
    if (    (pSceneObjectA != NULL && pSceneObjectA->getScene() == NULL) ||
            (pSceneObjectB != NULL && pSceneObjectB->getScene() == NULL) )
    {
        // Warn.
        Con::printf( "Cannot add a joint to a scene object that is not in a scene." );
        return -1;
    }

    // Check for two invalid objects.
    if ( pSceneObjectA == NULL && pSceneObjectB == NULL )
    {
        Con::warnf("Scene::createMotorJoint() - Cannot create joint without at least a single scene object." );
        return -1;
    }

    // Fetch bodies.
    b2Body* pBodyA = pSceneObjectA != NULL ? pSceneObjectA->getBody() : getGroundBody();
    b2Body* pBodyB = pSceneObjectB != NULL ? pSceneObjectB->getBody() : getGroundBody();
    
    // Populate definition.
    b2MotorJointDef jointDef;
    jointDef.userData           = static_cast<PhysicsProxy*>(this);
    jointDef.collideConnected   = collideConnected;
    jointDef.bodyA              = pBodyA;
    jointDef.bodyB              = pBodyB;
    jointDef.linearOffset       = linearOffset;
    jointDef.angularOffset      = angularOffset;
    jointDef.correctionFactor   = correctionFactor;
    jointDef.maxForce           = maxForce;
    jointDef.maxTorque          = maxTorque;
    
    // Create joint.
    return createJoint( &jointDef );
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointLinearOffset(
        const U32 jointId,
        const b2Vec2& linearOffset )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetLinearOffset( linearOffset );
}

//-----------------------------------------------------------------------------

b2Vec2 Scene::getMotorJointLinearOffset( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return b2Vec2_zero;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return b2Vec2_zero;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetLinearOffset();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointAngularOffset(
        const U32 jointId,
        const F32 angularOffset )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetAngularOffset( angularOffset );
}

//-----------------------------------------------------------------------------

F32 Scene::getMotorJointAngularOffset( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetAngularOffset();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointMaxForce(
        const U32 jointId,
        const F32 maxForce )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxForce( maxForce );
}

//-----------------------------------------------------------------------------

F32 Scene::getMotorJointMaxForce( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxForce();
}

//-----------------------------------------------------------------------------

void Scene::setMotorJointMaxTorque(
        const U32 jointId,
        const F32 maxTorque )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    pRealJoint->SetMaxTorque( maxTorque );
}


//-----------------------------------------------------------------------------

F32 Scene::getMotorJointMaxTorque( const U32 jointId )
{
    // Fetch joint.
    b2Joint* pJoint = findJoint( jointId );

    // Ignore invalid joint.
    if ( !pJoint )
        return -1.0f;

    // Fetch joint type.
    const b2JointType jointType = pJoint->GetType();

    if ( jointType != e_motorJoint )
    {
        Con::warnf( "Invalid joint type of %s.", getJointTypeDescription(jointType) );
        return -1.0f;
    }

    // Cast joint.
    b2MotorJoint* pRealJoint = static_cast<b2MotorJoint*>( pJoint );

    // Access joint.
    return pRealJoint->GetMaxTorque();
}

//-----------------------------------------------------------------------------

void Scene::setDebugSceneObject( SceneObject* pSceneObject )
{
    // Ignore no change.
    if ( mpDebugSceneObject == pSceneObject )
        return;

    // Remove delete notification for existing monitored object.
    if ( mpDebugSceneObject != NULL )
        clearNotify( mpDebugSceneObject );

    // Set monitored scene object.
    mpDebugSceneObject = pSceneObject;

    // Finish if resetting monitored object.
    if ( pSceneObject == NULL )
        return;

    // Add delete notification for new monitored object.
    deleteNotify( pSceneObject );
}

//-----------------------------------------------------------------------------

void Scene::setLayerSortMode( const U32 layer, const SceneRenderQueue::RenderSort sortMode )
{
    // Is the layer valid?
    if ( layer > MAX_LAYERS_SUPPORTED )
    {
        // No, so warn.
        Con::warnf( "Scene::setLayerSortMode() - Layer '%d' is out of range.", layer );

        return;
    }

    // Is the sort mode valid?
    if ( sortMode == SceneRenderQueue::RENDER_SORT_INVALID )
    {
        // No, so warn.
        Con::warnf( "Scene::setLayerSortMode() - Sort mode is invalid for layer '%d'.", layer );

        return;
    }

    mLayerSortModes[layer] = sortMode;
}

//-----------------------------------------------------------------------------

SceneRenderQueue::RenderSort Scene::getLayerSortMode( const U32 layer )
{
    // Is the layer valid?
    if ( layer > MAX_LAYERS_SUPPORTED )
    {
        // No, so warn.
        Con::warnf( "Scene::getLayerSortMode() - Layer '%d' is out of range.", layer );

        return SceneRenderQueue::RENDER_SORT_INVALID;
    }

    return mLayerSortModes[layer];
}

//-----------------------------------------------------------------------------

void Scene::attachSceneWindow( SceneWindow* pSceneWindow2D )
{
    // Ignore if already attached.
    if ( isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.addObject( pSceneWindow2D );
}

//-----------------------------------------------------------------------------

void Scene::detachSceneWindow( SceneWindow* pSceneWindow2D )
{
    // Ignore if not attached.
    if ( !isSceneWindowAttached( pSceneWindow2D ) )
        return;

    // Add to Attached List.
    mAttachedSceneWindows.removeObject( pSceneWindow2D );
}

//-----------------------------------------------------------------------------

void Scene::detachAllSceneWindows( void )
{
    // Detach All Scene Windows.
    while( mAttachedSceneWindows.size() > 0 )
        dynamic_cast<SceneWindow*>(mAttachedSceneWindows[mAttachedSceneWindows.size()-1])->resetScene();
}

//-----------------------------------------------------------------------------

bool Scene::isSceneWindowAttached( SceneWindow* pSceneWindow2D )
{
    for( SimSet::iterator itr = mAttachedSceneWindows.begin(); itr != mAttachedSceneWindows.end(); itr++ )
        if ( pSceneWindow2D == dynamic_cast<SceneWindow*>(*itr) )
            // Found.
            return true;

    // Not Found.
    return false;
}

//-----------------------------------------------------------------------------

void Scene::addDeleteRequest( SceneObject* pSceneObject )
{
    // Ignore if it's already being safe-deleted.
    if ( pSceneObject->isBeingDeleted() )
        return;

    // Populate Delete Request.
    tDeleteRequest deleteRequest = { pSceneObject->getId(), NULL, false };

    // Push Delete Request.
    mDeleteRequests.push_back( deleteRequest );

    // Flag Delete in Progress.
    pSceneObject->mBeingSafeDeleted = true;
}


//-----------------------------------------------------------------------------

void Scene::processDeleteRequests( const bool forceImmediate )
{
    // Ignore if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Validate All Delete Requests.
    U32 safeDeleteReadyCount = 0;
    for ( U32 requestIndex = 0; requestIndex < (U32)mDeleteRequests.size(); )
    {
        // Fetch Reference to Delete Request.
        tDeleteRequest& deleteRequest = mDeleteRequests[requestIndex];

        // Fetch Object.
        // NOTE:- Let's be safer and check that it's definitely a scene-object.
        SceneObject* pSceneObject = dynamic_cast<SceneObject*>( Sim::findObject( deleteRequest.mObjectId ) );

        // Does this object exist?
        if ( pSceneObject )
        {
            // Yes, so write object.
            deleteRequest.mpSceneObject = pSceneObject;

            // Calculate Safe-Ready Flag.
            deleteRequest.mSafeDeleteReady = forceImmediate || pSceneObject->getSafeDelete();

            // Is it ready to safe-delete?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so increase safe-ready count.
                ++safeDeleteReadyCount;
            }         
        }
        else
        {
            // No, so it looks like the object got deleted prematurely; let's just remove
            // the request instead.
            mDeleteRequests.erase_fast( requestIndex );
            
            // Repeat this item.
            continue;
        }

        // Skip to next request index.
        ++requestIndex;
    }

    // Stop if there's no delete requests!
    if ( mDeleteRequests.size() == 0 )
        return;

    // Transfer Delete-Requests to Temporary version.
    // NOTE:-   We do this because we may delete objects which have dependencies.  This would
    //          cause objects to be added to the safe-delete list.  We don't want to work on
    //          the list whilst this is happening so we'll transfer it to a temporary list.
    mDeleteRequestsTemp = mDeleteRequests;

    // Can we process all remaining delete-requests?
    if ( safeDeleteReadyCount == (U32)mDeleteRequestsTemp.size() )
    {
        // Yes, so process ALL safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex < (U32)mDeleteRequestsTemp.size(); ++requestIndex )
        {
            // Yes, so fetch object.
            SceneObject* pSceneObject = mDeleteRequestsTemp[requestIndex].mpSceneObject;

            // Do script callback.
            Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() );

            // Destroy the object.
            pSceneObject->deleteObject();
        }

        // Remove All delete-requests.
        mDeleteRequestsTemp.clear();
    }
    else
    {
        // No, so process only safe-ready delete-requests.
        for ( U32 requestIndex = 0; requestIndex <(U32) mDeleteRequestsTemp.size(); )
        {
            // Fetch Reference to Delete Request.
            tDeleteRequest& deleteRequest = mDeleteRequestsTemp[requestIndex];

            // Is the Object Safe-Ready?
            if ( deleteRequest.mSafeDeleteReady )
            {
                // Yes, so fetch object.
                SceneObject* pSceneObject = deleteRequest.mpSceneObject;

                // Do script callback.
                Con::executef(this, 2, "onSafeDelete", pSceneObject->getIdString() );

                // Destroy the object.
                pSceneObject->deleteObject();

                // Quickly remove delete-request.
                mDeleteRequestsTemp.erase_fast( requestIndex );

                // Repeat this item.
                continue;
            }

            // Skip to next request index.
            ++requestIndex;
        }
    }
}

//-----------------------------------------------------------------------------

void Scene::SayGoodbye( b2Joint* pJoint )
{
    // Find the joint id.
    const U32 jointId = findJointId( pJoint );

    // Ignore a bad joint.
    if ( jointId == 0 )
        return;

    // Remove joint references.
    mJoints.erase( jointId );
    mReverseJoints.erase( pJoint );
}

//-----------------------------------------------------------------------------

SceneObject* Scene::create( const char* pType )
{
    // Sanity!
    AssertFatal( pType != NULL, "Scene::create() - Cannot create a NULL type." );

    // Find the class rep.
    AbstractClassRep* pClassRep = AbstractClassRep::findClassRep( pType ); 

    // Did we find the type?
    if ( pClassRep == NULL )
    {
        // No, so warn.
        Con::warnf( "Scene::create() - Could not find type '%s' to create.", pType );
        return NULL;
    }

    // Find the scene object rep.
    AbstractClassRep* pSceneObjectRep = AbstractClassRep::findClassRep( "SceneObject" ); 

    // Sanity!
    AssertFatal( pSceneObjectRep != NULL,  "Scene::create() - Could not find SceneObject class rep." );

    // Is the type derived from scene object?
    if ( !pClassRep->isClass( pSceneObjectRep ) )
    {
        // No, so warn.
        Con::warnf( "Scene::create() - Type '%s' is not derived from SceneObject.", pType );
        return NULL;
    }
    
    // Create the type.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( pClassRep->create() );

    // Sanity!
    AssertFatal( pSceneObject != NULL, "Scene::create() - Failed to create type via class rep." );

    // Attempt to register the object.
    if ( !pSceneObject->registerObject() )
    {
        // No, so warn.
        Con::warnf( "Scene::create() - Failed to register type '%s'.", pType );
        delete pSceneObject;
        return NULL;
    }

    // Add to the scene.
    addToScene( pSceneObject );

    return pSceneObject;
}

//-----------------------------------------------------------------------------

void Scene::onTamlPreRead( void )
{
    // Call parent.
    Parent::onTamlPreRead();
}

//-----------------------------------------------------------------------------

void Scene::onTamlPostRead( const TamlCustomNodes& customNodes )
{
    // Call parent.
    Parent::onTamlPostRead( customNodes );

    // Reset the loading scene.
    Scene::LoadingScene = NULL;

    // Find joint custom node.
    const TamlCustomNode* pJointNode = customNodes.findNode( jointCustomNodeName );

    // Do we have any joints?
    if ( pJointNode != NULL )
    {
        // Yes, so fetch children joint nodes.
        const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

        // Iterate joints.
        for( TamlCustomNodeVector::const_iterator jointNodeItr = jointChildren.begin(); jointNodeItr != jointChildren.end(); ++jointNodeItr )
        {
            // Fetch joint node,
            TamlCustomNode* pJointNode = *jointNodeItr;

            // Fetch node name.
            StringTableEntry nodeName = pJointNode->getNodeName();

            // Is this a distance joint?
            if ( nodeName == jointDistanceNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                F32 length = -1.0f;
                F32 frequency = 0.0f;
                F32 dampingRatio = 0.0f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointDistanceLengthName )
                    {
                        pField->getFieldValue( length );
                    }
                    else if ( fieldName == jointDistanceFrequencyName )
                    {
                        pField->getFieldValue( frequency );
                    }
                    else if ( fieldName == jointDistanceDampingRatioName )
                    {
                        pField->getFieldValue( dampingRatio );
                    }
                }

                // Create joint.
                createDistanceJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, length, frequency, dampingRatio, collideConnected );
            }
            // is this a rope joint?
            else if ( nodeName == jointRopeNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                F32 maxLength = -1.0f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointRopeMaxLengthName )
                    {
                        pField->getFieldValue( maxLength );
                    }
                }

                // Create joint.
                createRopeJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxLength, collideConnected );
            }
            // Is this a revolute joint?
            else if ( nodeName == jointRevoluteNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                bool enableLimit = false;
                F32 lowerAngle = 0.0f;
                F32 upperAngle = 0.0f;

                bool enableMotor = false;
                F32 motorSpeed = b2_pi;
                F32 maxMotorTorque = 0.0f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointRevoluteLimitLowerAngleName )
                    {
                        pField->getFieldValue( lowerAngle );
                        lowerAngle = mDegToRad( lowerAngle );
                        enableLimit = true;
                    }
                    else if ( fieldName == jointRevoluteLimitUpperAngleName )
                    {
                        pField->getFieldValue( upperAngle );
                        upperAngle = mDegToRad( upperAngle );
                        enableLimit = true;
                    }
                    else if ( fieldName == jointRevoluteMotorSpeedName )
                    {
                        pField->getFieldValue( motorSpeed );
                        motorSpeed = mDegToRad( motorSpeed );
                        enableMotor = true;
                    }
                    else if ( fieldName == jointRevoluteMotorMaxTorqueName )
                    {
                        pField->getFieldValue( maxMotorTorque );
                        enableMotor = true;
                    }
                }

                // Create joint.
                const U32 jointId = createRevoluteJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, collideConnected );
            
                if ( enableLimit )
                    setRevoluteJointLimit( jointId, true, lowerAngle, upperAngle );

                if ( enableMotor )
                    setRevoluteJointMotor( jointId, true, motorSpeed, maxMotorTorque );
            }
            // is this a weld joint?
            else if ( nodeName == jointWeldNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                F32 frequency = 0.0f;
                F32 dampingRatio = 0.0f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointWeldFrequencyName )
                    {
                        pField->getFieldValue( frequency );
                    }
                    else if ( fieldName == jointWeldDampingRatioName )
                    {
                        pField->getFieldValue( dampingRatio );
                    }
                }

                // Create joint.
                createWeldJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, frequency, dampingRatio, collideConnected );
            }
            // Is this a wheel joint?
            else if ( nodeName == jointWheelNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                bool enableMotor = false;
                F32 motorSpeed = b2_pi;
                F32 maxMotorTorque = 0.0f;

                F32 frequency = 0.0f;
                F32 dampingRatio = 0.0f;
                b2Vec2 worldAxis( 0.0f, 1.0f );

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointWheelMotorSpeedName )
                    {
                        pField->getFieldValue( motorSpeed );
                        motorSpeed = mDegToRad( motorSpeed );
                        enableMotor = true;
                    }
                    else if ( fieldName == jointWheelMotorMaxTorqueName )
                    {
                        pField->getFieldValue( maxMotorTorque );
                        enableMotor = true;
                    }
                    else if ( fieldName == jointWheelFrequencyName )
                    {
                        pField->getFieldValue( frequency );
                    }
                    else if ( fieldName == jointWheelDampingRatioName )
                    {
                        pField->getFieldValue( dampingRatio );
                    }
                    else if ( fieldName == jointWheelWorldAxisName )
                    {
                        pField->getFieldValue( worldAxis );                    
                    }
                }

                // Create joint.
                const U32 jointId = createWheelJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );

                if ( enableMotor )
                    setWheelJointMotor( jointId, true, motorSpeed, maxMotorTorque );

                setWheelJointFrequency( jointId, frequency );
                setWheelJointDampingRatio( jointId, dampingRatio );
            }
            // Is this a friction joint?
            else if ( nodeName == jointFrictionNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                F32 maxForce = 0.0f;
                F32 maxTorque = 0.0f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointFrictionMaxForceName )
                    {
                        pField->getFieldValue( maxForce );
                    }
                    else if ( fieldName == jointFrictionMaxTorqueName )
                    {
                        pField->getFieldValue( maxTorque );
                    }
                }

                // Create joint.
                createFrictionJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, maxForce, maxTorque, collideConnected );
            }
            // Is this a prismatic joint?
            else if ( nodeName == jointPrismaticNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                bool enableLimit;
                F32 lowerTransLimit = 0.0f;
                F32 upperTransLimit = 1.0f;

                bool enableMotor = false;
                F32 motorSpeed = b2_pi;
                F32 maxMotorForce = 0.0f;

                b2Vec2 worldAxis( 0.0f, 1.0f );

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointPrismaticLimitLowerTransName )
                    {
                        pField->getFieldValue( lowerTransLimit );
                        enableLimit = true;
                    }
                    else if ( fieldName == jointPrismaticLimitUpperTransName )
                    {
                        pField->getFieldValue( upperTransLimit );
                        enableLimit = true;
                    }
                    else if ( fieldName == jointPrismaticMotorSpeedName )
                    {
                        pField->getFieldValue( motorSpeed );
                        motorSpeed = mDegToRad( motorSpeed );
                        enableMotor = true;
                    }
                    else if ( fieldName == jointPrismaticMotorMaxForceName )
                    {
                        pField->getFieldValue( maxMotorForce );
                        enableMotor = true;
                    }
                    else if ( fieldName == jointPrismaticWorldAxisName )
                    {
                        pField->getFieldValue( worldAxis );
                    }
                }

                // Create joint.
                const U32 jointId = createPrismaticJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldAxis, collideConnected );

                if ( enableLimit )
                    setPrismaticJointLimit( jointId, true, lowerTransLimit, upperTransLimit );

                if ( enableMotor )
                    setPrismaticJointMotor( jointId, true, motorSpeed, maxMotorForce );
            }
            // Is this a pulley joint?
            else if ( nodeName == jointPulleyNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                b2Vec2 localAnchorA = b2Vec2_zero;
                b2Vec2 localAnchorB = b2Vec2_zero;
                bool collideConnected = false;

                F32 lengthA = -1.0f;
                F32 lengthB = -1.0f;
                F32 ratio = 0.5f;
                b2Vec2 worldGroundAnchorA = b2Vec2_zero;
                b2Vec2 worldGroundAnchorB = b2Vec2_zero;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointLocalAnchorAName )
                    {
                        pField->getFieldValue( localAnchorA );
                    }
                    else if ( fieldName == jointLocalAnchorBName )
                    {
                        pField->getFieldValue( localAnchorB );
                    }
                    else if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointPulleyLengthAName )
                    {
                        pField->getFieldValue( lengthA );
                    }
                    else if ( fieldName == jointPulleyLengthBName )
                    {
                        pField->getFieldValue( lengthB );
                    }
                    else if ( fieldName == jointPulleyRatioName )
                    {
                        pField->getFieldValue( ratio );
                    }
                    else if ( fieldName == jointPulleyGroundAnchorAName )
                    {
                        pField->getFieldValue( worldGroundAnchorA );
                    }
                    else if ( fieldName == jointPulleyGroundAnchorBName )
                    {
                        pField->getFieldValue( worldGroundAnchorB );
                    }
                }

                // Create joint.
                createPulleyJoint( pSceneObjectA, pSceneObjectB, localAnchorA, localAnchorB, worldGroundAnchorA, worldGroundAnchorB, ratio, lengthA, lengthB, collideConnected );

            }
            // Is this a target joint?
            else if ( nodeName == jointTargetNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObject = jointChildren.size() == 1 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObject == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has an invalid connected object.", nodeName );
                    continue;
                }

                bool collideConnected = false;
                b2Vec2 worldTarget = b2Vec2_zero;
                F32 maxForce = 1.0f;
                F32 frequency = 5.0f;
                F32 dampingRatio = 0.7f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointTargetWorldTargetName )
                    {
                        pField->getFieldValue( worldTarget );
                    }
                    else if ( fieldName == jointTargetMaxForceName )
                    {
                        pField->getFieldValue( maxForce );
                    }
                    else if ( fieldName == jointTargetFrequencyName )
                    {
                        pField->getFieldValue( frequency );
                    }
                    else if ( fieldName == jointTargetDampingRatioName )
                    {
                        pField->getFieldValue( dampingRatio );
                    }
                }

                // Create joint.
                createTargetJoint( pSceneObject, worldTarget, maxForce, frequency, dampingRatio, collideConnected );
            }
            // Is this a motor joint?
            else if ( nodeName == jointMotorNodeName )
            {
                // Fetch joint children.
                const TamlCustomNodeVector& jointChildren = pJointNode->getChildren();

                // Fetch joint objects.
                SceneObject* pSceneObjectA = jointChildren.size() > 0 ? jointChildren[0]->getProxyObject<SceneObject>(true) : NULL;
                SceneObject* pSceneObjectB = jointChildren.size() == 2 ? jointChildren[1]->getProxyObject<SceneObject>(true) : NULL;

                // Did we get any connected objects?
                if( pSceneObjectA == NULL && pSceneObjectB == NULL )
                {
                    // No, so warn.
                    Con::warnf( "Scene::onTamlPostRead() - Encountered a joint '%s' but it has invalid connected objects.", nodeName );
                    continue;
                }

                bool collideConnected = false;

                b2Vec2 linearOffset = b2Vec2_zero;
                F32 angularOffset = 0.0f;
                F32 maxForce = 1.0f;
                F32 maxTorque = 1.0f;
                F32 correctionFactor = 0.3f;

                // Fetch joint fields.
                const TamlCustomFieldVector& jointFields = pJointNode->getFields();

                // Iterate property fields.
                for ( TamlCustomFieldVector::const_iterator jointFieldItr = jointFields.begin(); jointFieldItr != jointFields.end(); ++jointFieldItr )
                {
                    // Fetch field node.
                    TamlCustomField* pField = *jointFieldItr;

                    // Fetch property field name.
                    StringTableEntry fieldName = pField->getFieldName();

                    // Fetch fields.
                    if ( fieldName == jointCollideConnectedName )
                    {
                        pField->getFieldValue( collideConnected );
                    }
                    else if ( fieldName == jointMotorLinearOffsetName )
                    {
                        pField->getFieldValue( linearOffset );
                    }
                    else if ( fieldName == jointMotorAngularOffsetName )
                    {
                        pField->getFieldValue( angularOffset );
                        angularOffset = mDegToRad( angularOffset );
                    }
                    else if ( fieldName == jointMotorMaxForceName )
                    {
                        pField->getFieldValue( maxForce );
                    }
                    else if ( fieldName == jointMotorMaxTorqueName )
                    {
                        pField->getFieldValue( maxTorque );
                    }
                    else if ( fieldName == jointMotorCorrectionFactorName )
                    {
                        pField->getFieldValue( correctionFactor );
                    }
                }

                // Create joint.
                createMotorJoint( pSceneObjectA, pSceneObjectB, linearOffset, angularOffset, maxForce, maxTorque, correctionFactor, collideConnected );

            }
            // Unknown joint type!
            else
            {
                // Warn.
                Con::warnf( "Unknown joint type of '%s' encountered.", nodeName );

                // Sanity!
                AssertFatal( false, "Scene::onTamlCustomRead() - Unknown joint type detected." );

                continue;
            }
        }
    }

    // Find controller custom node.
    const TamlCustomNode* pControllerCustomNode = customNodes.findNode( controllerCustomNodeName );

    // Do we have any controllers?
    if ( pControllerCustomNode != NULL )
    {
        // Yes, so fetch the scene controllers.
        SimSet* pControllerSet = getControllers();

        // Fetch children controller nodes.
        const TamlCustomNodeVector& controllerChildren = pControllerCustomNode->getChildren();

        // Iterate controllers.
        for( TamlCustomNodeVector::const_iterator controllerNodeItr = controllerChildren.begin(); controllerNodeItr != controllerChildren.end(); ++controllerNodeItr )
        {
            // Fetch controller node.
            TamlCustomNode* pControllerNode = *controllerNodeItr;
            
            // Is the node a proxy object?
            if ( !pControllerNode->isProxyObject() )
            {
                // No, so warn.
                Con::warnf("Scene::onTamlPostRead() - Reading scene controllers but node '%s'is not an object.", pControllerNode->getNodeName() );

                continue;
            }

            // Add the proxy object.
            SimObject* pProxyObject = pControllerNode->getProxyObject<SimObject>(false);

            // Is it a scene controller?
            if ( dynamic_cast<SceneController*>( pProxyObject ) == NULL )
            {
                // No, so warn.
                Con::warnf("Scene::onTamlPostRead() - Reading scene controllers but node '%s'is not a scene controller.", pControllerNode->getNodeName() );

                // Delete the object.
                pProxyObject->deleteObject();

                continue;
            }

            // Add to scene controllers.
            pControllerSet->addObject( pProxyObject );
        }
    }

    // Find asset preload custom node.
    const TamlCustomNode* pAssetPreloadNode = customNodes.findNode( assetPreloadNodeName );

    // Do we have any asset preloads?
    if ( pAssetPreloadNode != NULL )
    {
        // Yes, so clear any existing asset preloads.
        clearAssetPreloads();

        // Yes, so fetch asset Id type prefix.
        StringTableEntry assetIdTypePrefix = ConsoleBaseType::getType( TypeAssetId )->getTypePrefix();

        // Fetch the prefix length.
        const S32 assetIdPrefixLength = dStrlen( assetIdTypePrefix );

        // Fetch the preload children nodes.
        const TamlCustomNodeVector& preloadChildren = pAssetPreloadNode->getChildren();

        // Iterate asset preloads.
        for( TamlCustomNodeVector::const_iterator assetPreloadNodeItr = preloadChildren.begin(); assetPreloadNodeItr != preloadChildren.end(); ++assetPreloadNodeItr )
        {
            // Fetch asset node.
            const TamlCustomNode* pAssetNode = *assetPreloadNodeItr;

            // Ignore non-asset nodes.
            if ( pAssetNode->getNodeName() != assetNodeName )
                continue;

            // Find the asset Id field.
            const TamlCustomField* pAssetIdField = pAssetNode->findField( "Id" );

            // Did we find the field?
            if ( pAssetIdField == NULL )
            {
                // No, so warn.
                Con::warnf("Scene::onTamlPostRead() - Found asset preload but failed to find asset Id field." );
                continue;
            }

            // Fetch field value.
            const char* pFieldValue = pAssetIdField->getFieldValue();

            // Calculate the field value start (skip any type prefix).
            const S32 prefixOffset = dStrnicmp( pFieldValue, assetIdTypePrefix, assetIdPrefixLength ) == 0 ? assetIdPrefixLength : 0;

            // Add asset preload.
            addAssetPreload( pFieldValue + prefixOffset );
        }
    }   
}

//-----------------------------------------------------------------------------

void Scene::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Call parent.
    Parent::onTamlCustomWrite( customNodes );

    // Fetch joint count.
    const U32 jointCount = getJointCount();

    // Do we have any joints?
    if ( jointCount > 0 )
    {
        // Yes, so add joint custom node.
        TamlCustomNode* pJointCustomNode = customNodes.addNode( jointCustomNodeName );

        // Iterate joints.
        for( typeJointHash::iterator jointItr = mJoints.begin(); jointItr != mJoints.end(); ++jointItr )
        {
            // Fetch base joint.
            b2Joint* pBaseJoint = jointItr->value;

            // Add joint node.
            // NOTE:    The name of the node will get updated shortly.
            TamlCustomNode* pJointNode = pJointCustomNode->addNode( StringTable->EmptyString );

            // Fetch common details.
            b2Body* pBodyA = pBaseJoint->GetBodyA();
            b2Body* pBodyB = pBaseJoint->GetBodyB();

            // Fetch physics proxies.
            PhysicsProxy* pPhysicsProxyA = static_cast<PhysicsProxy*>(pBodyA->GetUserData());
            PhysicsProxy* pPhysicsProxyB = static_cast<PhysicsProxy*>(pBodyB->GetUserData());

            // Fetch physics proxy type.
            PhysicsProxy::ePhysicsProxyType proxyTypeA = static_cast<PhysicsProxy*>(pBodyA->GetUserData())->getPhysicsProxyType();
            PhysicsProxy::ePhysicsProxyType proxyTypeB = static_cast<PhysicsProxy*>(pBodyB->GetUserData())->getPhysicsProxyType();

            // Fetch scene objects.
            SceneObject* pSceneObjectA = proxyTypeA == PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ? static_cast<SceneObject*>(pPhysicsProxyA) : NULL;
            SceneObject* pSceneObjectB = proxyTypeB == PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT ? static_cast<SceneObject*>(pPhysicsProxyB) : NULL;

            // Populate joint appropriately.
            switch( pBaseJoint->GetType() )
            {
                case e_distanceJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointDistanceNodeName );

                        // Fetch joint.
                        const b2DistanceJoint* pJoint = dynamic_cast<const b2DistanceJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid distance joint type returned." );

                        // Add length.
                        pJointNode->addField( jointDistanceLengthName, pJoint->GetLength() );

                        // Add frequency.
                        if ( mNotZero( pJoint->GetFrequency() ) )
                            pJointNode->addField( jointDistanceFrequencyName, pJoint->GetFrequency() );

                        // Add damping ratio.
                        if ( mNotZero( pJoint->GetDampingRatio() ) )
                            pJointNode->addField( jointDistanceDampingRatioName, pJoint->GetDampingRatio() );

                        // Add local anchors.
                        if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_ropeJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointRopeNodeName );

                        // Fetch joint.
                        const b2RopeJoint* pJoint = dynamic_cast<const b2RopeJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid rope joint type returned." );

                        // Add max length.
                        if ( mNotZero( pJoint->GetMaxLength() ) )
                            pJointNode->addField( jointRopeMaxLengthName, pJoint->GetMaxLength() );

                        // Add local anchors.
                        if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_revoluteJoint:
                    {
                        // Set join name.
                        pJointNode->setNodeName( jointRevoluteNodeName );

                        // Fetch joint.
                        const b2RevoluteJoint* pJoint = dynamic_cast<const b2RevoluteJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid revolute joint type returned." );

                        // Add limit.
                        if ( pJoint->IsLimitEnabled() )
                        {
                            // Add limits.
                            pJointNode->addField( jointRevoluteLimitLowerAngleName, mRadToDeg(pJoint->GetLowerLimit()) );
                            pJointNode->addField( jointRevoluteLimitUpperAngleName, mRadToDeg(pJoint->GetUpperLimit()) );
                        }

                        // Add motor.
                        if ( pJoint->IsMotorEnabled() )
                        {
                            // Add motor.
                            pJointNode->addField( jointRevoluteMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                            pJointNode->addField( jointRevoluteMotorMaxTorqueName, pJoint->GetMaxMotorTorque() );
                        }

                        // Add local anchors.
                        if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_weldJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointWeldNodeName );

                        // Fetch joint.
                        const b2WeldJoint* pJoint = dynamic_cast<const b2WeldJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid weld joint type returned." );

                        // Add frequency.
                        if ( mNotZero( pJoint->GetFrequency() ) )
                            pJointNode->addField( jointWeldFrequencyName, pJoint->GetFrequency() );

                        // Add damping ratio.
                        if ( mNotZero( pJoint->GetDampingRatio() ) )
                            pJointNode->addField( jointWeldDampingRatioName, pJoint->GetDampingRatio() );

                        // Add local anchors.
                        if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_wheelJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointWheelNodeName );

                        // Fetch joint.
                        b2WheelJoint* pJoint = dynamic_cast<b2WheelJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid wheel joint type returned." );

                        // Add motor.
                        if ( pJoint->IsMotorEnabled() )
                        {
                            // Add motor.
                            pJointNode->addField( jointWheelMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                            pJointNode->addField( jointWheelMotorMaxTorqueName, pJoint->GetMaxMotorTorque() );
                        }

                        // Add frequency.
                        if ( mNotZero( pJoint->GetSpringFrequencyHz() ) )
                            pJointNode->addField( jointWheelFrequencyName, pJoint->GetSpringFrequencyHz() );

                        // Add damping ratio.
                        if ( mNotZero( pJoint->GetSpringDampingRatio() ) )
                            pJointNode->addField( jointWheelDampingRatioName, pJoint->GetSpringDampingRatio() );

                        // Add world axis.
                        pJointNode->addField( jointWheelWorldAxisName, pJoint->GetBodyA()->GetWorldVector( pJoint->GetLocalAxisA() ) );

                        // Add local anchors.
                        pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_frictionJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointFrictionNodeName );

                        // Fetch joint.
                        const b2FrictionJoint* pJoint = dynamic_cast<const b2FrictionJoint*>( pBaseJoint );

                        // Add max force.
                        if ( mNotZero( pJoint->GetMaxForce() ) )
                            pJointNode->addField( jointFrictionMaxForceName, pJoint->GetMaxForce() );

                        // Add max torque.
                        if ( mNotZero( pJoint->GetMaxTorque() ) )
                            pJointNode->addField( jointFrictionMaxTorqueName, pJoint->GetMaxTorque() );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid friction joint type returned." );

                        // Add local anchors.
                        if ( mNotZero( pJoint->GetLocalAnchorA().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        if ( mNotZero( pJoint->GetLocalAnchorB().LengthSquared() ) )
                            pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_prismaticJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointPrismaticNodeName );

                        // Fetch joint.
                        b2PrismaticJoint* pJoint = dynamic_cast<b2PrismaticJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid prismatic joint type returned." );

                        // Add limit.
                        if ( pJoint->IsLimitEnabled() )
                        {
                            // Add limits.
                            pJointNode->addField( jointPrismaticLimitLowerTransName, pJoint->GetLowerLimit() );
                            pJointNode->addField( jointPrismaticLimitUpperTransName, pJoint->GetUpperLimit() );
                        }

                        // Add motor.
                        if ( pJoint->IsMotorEnabled() )
                        {
                            // Add motor.
                            pJointNode->addField( jointPrismaticMotorSpeedName, mRadToDeg(pJoint->GetMotorSpeed()) );
                            pJointNode->addField( jointPrismaticMotorMaxForceName, pJoint->GetMaxMotorForce() );
                        }

                        // Add world axis.
                        pJointNode->addField( jointPrismaticWorldAxisName, pJoint->GetBodyA()->GetWorldVector( pJoint->GetLocalAxisA() ) );

                        // Add local anchors.
                        pJointNode->addField( jointLocalAnchorAName, pJoint->GetLocalAnchorA() );
                        pJointNode->addField( jointLocalAnchorBName, pJoint->GetLocalAnchorB() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_pulleyJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointPulleyNodeName );

                        // Fetch joint.
                        b2PulleyJoint* pJoint = dynamic_cast<b2PulleyJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid pulley joint type returned." );

                        // Add lengths.
                        pJointNode->addField( jointPulleyLengthAName, pJoint->GetLengthA() );
                        pJointNode->addField( jointPulleyLengthBName, pJoint->GetLengthB() );

                        // Add ratio,
                        pJointNode->addField( jointPulleyRatioName, pJoint->GetRatio() );

                        // Add ground anchors.
                        pJointNode->addField( jointPulleyGroundAnchorAName, pJoint->GetGroundAnchorA() );
                        pJointNode->addField( jointPulleyGroundAnchorBName, pJoint->GetGroundAnchorB() );

                        // Add local anchors.
                        pJointNode->addField( jointLocalAnchorAName, pJoint->GetBodyA()->GetLocalPoint( pJoint->GetAnchorA() ) );
                        pJointNode->addField( jointLocalAnchorBName, pJoint->GetBodyB()->GetLocalPoint( pJoint->GetAnchorB() ) );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_mouseJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointTargetNodeName );

                        // Fetch joint.
                        const b2MouseJoint* pJoint = dynamic_cast<const b2MouseJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid target joint type returned." );

                        // Add target.
                        pJointNode->addField( jointTargetWorldTargetName, pJoint->GetTarget() );

                        // Add max force.
                        pJointNode->addField( jointTargetMaxForceName, pJoint->GetMaxForce() );

                        // Add frequency
                        pJointNode->addField( jointTargetFrequencyName, pJoint->GetFrequency() );

                        // Add damping ratio.
                        pJointNode->addField( jointTargetDampingRatioName, pJoint->GetDampingRatio() );

                        // Add body.
                        // NOTE: This joint uses BODYB as the object, BODYA is the ground-body however for easy of use
                        // we'll refer to this as OBJECTA in the persisted format.
                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

                case e_motorJoint:
                    {
                        // Set joint name.
                        pJointNode->setNodeName( jointMotorNodeName );

                        // Fetch joint.
                        const b2MotorJoint* pJoint = dynamic_cast<const b2MotorJoint*>( pBaseJoint );

                        // Sanity!
                        AssertFatal( pJoint != NULL, "Scene::onTamlCustomWrite() - Invalid motor joint type returned." );

                        // Add linear offset.
                        if ( mNotZero( pJoint->GetLinearOffset().LengthSquared() ) )
                            pJointNode->addField( jointMotorLinearOffsetName, pJoint->GetLinearOffset() );

                        // Add angular offset.
                        if ( mNotZero( pJoint->GetAngularOffset() ) )
                            pJointNode->addField( jointMotorAngularOffsetName, mRadToDeg( pJoint->GetAngularOffset() ) );

                        // Add max force.
                        pJointNode->addField( jointMotorMaxForceName, pJoint->GetMaxForce() );

                        // Add max torque.
                        pJointNode->addField( jointMotorMaxTorqueName, pJoint->GetMaxTorque() );

                        // Add correction factor.
                        pJointNode->addField( jointMotorCorrectionFactorName, pJoint->GetCorrectionFactor() );

                        // Add scene object bodies.
                        if ( pSceneObjectA != NULL )
                            pJointNode->addNode( pSceneObjectA );

                        if ( pSceneObjectB != NULL )
                            pJointNode->addNode( pSceneObjectB );
                    }
                    break;

            default:
                // Sanity!
                AssertFatal( false, "Scene::onTamlCustomWrite() - Unknown joint type detected." );
            }

            // Add collide connected flag.
            if ( pBaseJoint->GetCollideConnected() )
                pJointNode->addField( jointCollideConnectedName, pBaseJoint->GetCollideConnected() );
        }
    }

    // Fetch controller count.
    const S32 sceneControllerCount = getControllers() ? getControllers()->size() : 0;
    
    // Do we have any scene controllers?
    if ( sceneControllerCount > 0 )
    {
        // Yes, so add controller node.
        TamlCustomNode* pControllerCustomNode = customNodes.addNode( controllerCustomNodeName );

        // Fetch the scene controllers.
        SimSet* pControllerSet = getControllers();

        // Iterate scene controllers.
        for( S32 i = 0; i < sceneControllerCount; i++ )
        {
            // Fetch the set object.
            SimObject* pSetObject = pControllerSet->at(i);

            // Skip if not a controller.
            if ( !pSetObject->isType<SceneController*>() )
                continue;

            // Add controller node.
            pControllerCustomNode->addNode( pSetObject );
        }
    }

    // Fetch asset preload count.
    const S32 assetPreloadCount = getAssetPreloadCount();

    // Do we have any asset preloads?
    if ( assetPreloadCount > 0 )
    {
        // Yes, so fetch asset Id type prefix.
        StringTableEntry assetIdTypePrefix = ConsoleBaseType::getType( TypeAssetId )->getTypePrefix();

        // Add asset preload node.
        TamlCustomNode* pAssetPreloadCustomNode = customNodes.addNode( assetPreloadNodeName );

        // Iterate asset preloads.
        for( typeAssetPtrVector::const_iterator assetItr = mAssetPreloads.begin(); assetItr != mAssetPreloads.end(); ++assetItr )
        {
            // Add node.
            TamlCustomNode* pAssetNode = pAssetPreloadCustomNode->addNode( assetNodeName );

            char valueBuffer[1024];
            dSprintf( valueBuffer, sizeof(valueBuffer), "%s%s", assetIdTypePrefix, (*assetItr)->getAssetId() );

            // Add asset Id.
            pAssetNode->addField( "Id", valueBuffer );
        }        
    }
}

//-----------------------------------------------------------------------------

void Scene::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Call parent.
    Parent::onTamlCustomRead( customNodes );
}

//-----------------------------------------------------------------------------

U32 Scene::getGlobalSceneCount( void )
{
    return sSceneCount;
}

//-----------------------------------------------------------------------------

SceneRenderRequest* Scene::createDefaultRenderRequest( SceneRenderQueue* pSceneRenderQueue, SceneObject* pSceneObject )
{
    // Create a render request and populate it with the default details.
    SceneRenderRequest* pSceneRenderRequest = pSceneRenderQueue->createRenderRequest()->set(
        pSceneObject,
        pSceneObject->getRenderPosition(),
        pSceneObject->getSceneLayerDepth(),
        pSceneObject->getSortPoint(),
        pSceneObject->getSerialId(),
        pSceneObject->getRenderGroup() );

    // Prepare the blending for it.
    pSceneRenderRequest->mBlendMode = pSceneObject->getBlendMode();
    pSceneRenderRequest->mBlendColor = pSceneObject->getBlendColor();
    pSceneRenderRequest->mSrcBlendFactor = pSceneObject->getSrcBlendFactor();
    pSceneRenderRequest->mDstBlendFactor = pSceneObject->getDstBlendFactor();
    pSceneRenderRequest->mAlphaTest = pSceneObject->getAlphaTest();

    return pSceneRenderRequest;
}

//-----------------------------------------------------------------------------

SimObject* Scene::getTamlChild( const U32 childIndex ) const
{
    // Sanity!
    AssertFatal( childIndex < (U32)mSceneObjects.size(), "Scene::getTamlChild() - Child index is out of range." );

    // For when the assert is not used.
    if ( childIndex >= (U32)mSceneObjects.size() )
        return NULL;

    return mSceneObjects[ childIndex ];
}

//-----------------------------------------------------------------------------

void Scene::addTamlChild( SimObject* pSimObject )
{
    // Sanity!
    AssertFatal( pSimObject != NULL, "Scene::addTamlChild() - Cannot add a NULL child object." );

    // Fetch as a scene object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( pSimObject );

    // Is it a scene object?
    if ( pSceneObject == NULL )
    {
        // No, so warn.
        Con::warnf( "Scene::addTamlChild() - Cannot add a child object that isn't a scene object." );
        return;
    }

    // Add the scene object.
    addToScene( pSceneObject );
}

//-----------------------------------------------------------------------------

static EnumTable::Enums DebugOptionsLookup[] =
                {
                { Scene::SCENE_DEBUG_METRICS,           "metrics" },
                { Scene::SCENE_DEBUG_FPS_METRICS,       "fps" },
                { Scene::SCENE_DEBUG_CONTROLLERS,       "controllers" },
                { Scene::SCENE_DEBUG_JOINTS,            "joints" },
                { Scene::SCENE_DEBUG_WIREFRAME_RENDER,  "wireframe" },
                { Scene::SCENE_DEBUG_AUDIO_SOURCES,     "audio"},
                ///
                { Scene::SCENE_DEBUG_AABB,              "aabb" },
                { Scene::SCENE_DEBUG_OOBB,              "oobb" },
                { Scene::SCENE_DEBUG_SLEEP,             "sleep" },
                { Scene::SCENE_DEBUG_COLLISION_SHAPES,  "collision" },
                { Scene::SCENE_DEBUG_POSITION_AND_COM,  "position" },
                { Scene::SCENE_DEBUG_SORT_POINTS,       "sort" },
                };

//-----------------------------------------------------------------------------

Scene::DebugOption Scene::getDebugOptionEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(DebugOptionsLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(DebugOptionsLookup[i].label, label) == 0)
            return((Scene::DebugOption)DebugOptionsLookup[i].index);

    // Warn.
    Con::warnf( "Scene::getDebugOptionEnum() - Invalid debug option '%s'.", label );

    return Scene::SCENE_DEBUG_INVALID;
}

//-----------------------------------------------------------------------------

const char* Scene::getDebugOptionDescription( Scene::DebugOption debugOption )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(DebugOptionsLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( DebugOptionsLookup[i].index == debugOption )
            return DebugOptionsLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getDebugOptionDescription() - Invalid debug option." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

static EnumTable::Enums jointTypeLookup[] =
                {
                { e_distanceJoint,  "distance"  },
                { e_ropeJoint,      "rope"      },
                { e_revoluteJoint,  "revolute"  },
                { e_weldJoint,      "weld"      },
                { e_wheelJoint,     "wheel"     },
                { e_frictionJoint,  "friction"  },
                { e_prismaticJoint, "prismatic" },
                { e_pulleyJoint,    "pulley"    },
                { e_mouseJoint,     "target"    },
                { e_motorJoint,     "motor"     },
                };

EnumTable jointTypeTable(sizeof(jointTypeLookup) / sizeof(EnumTable::Enums), &jointTypeLookup[0]);

//-----------------------------------------------------------------------------

const char* Scene::getJointTypeDescription( b2JointType jointType )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(jointTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( jointTypeLookup[i].index == jointType )
            return jointTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getJointTypeDescription() - Invalid joint type." );

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

b2JointType Scene::getJointTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(jointTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(jointTypeLookup[i].label, label) == 0)
            return (b2JointType)jointTypeLookup[i].index;
    }

    // Warn.
    Con::warnf( "Scene::getJointTypeEnum() - Invalid joint of '%s'", label );

    return e_unknownJoint;
}

//-----------------------------------------------------------------------------

static EnumTable::Enums pickModeLookup[] =
                {
                { Scene::PICK_ANY,          "Any" },
                { Scene::PICK_AABB,         "AABB" },
                { Scene::PICK_OOBB,         "OOBB" },
                { Scene::PICK_COLLISION,    "Collision" },
                };

//-----------------------------------------------------------------------------

Scene::PickMode Scene::getPickModeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(pickModeLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(pickModeLookup[i].label, label) == 0)
            return((Scene::PickMode)pickModeLookup[i].index);

    // Warn.
    Con::warnf( "Scene::getPickModeEnum() - Invalid pick mode '%s'.", label );

    return Scene::PICK_INVALID;
}

//-----------------------------------------------------------------------------

const char* Scene::getPickModeDescription( Scene::PickMode pickMode )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(pickModeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( pickModeLookup[i].index == pickMode )
            return pickModeLookup[i].label;
    }

    // Warn.
    Con::warnf( "Scene::getPickModeDescription() - Invalid pick mode.");

    return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

static void WriteJointsCustomTamlScehema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "Taml::WriteJointsCustomTamlScehema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "Taml::WriteJointsCustomTamlScehema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create joints node element.
    TiXmlElement* pJointsNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), jointCustomNodeName );
    pJointsNodeElement->SetAttribute( "name", buffer );
    pJointsNodeElement->SetAttribute( "minOccurs", 0 );
    pJointsNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pJointsNodeElement );
    
    // Create complex type.
    TiXmlElement* pJointsNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pJointsNodeElement->LinkEndChild( pJointsNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pJointsNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pJointsNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pJointsNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pJointsNodeComplexTypeElement->LinkEndChild( pJointsNodeChoiceElement );

    // ********************************************************************************
    // Create Distance Joint Element.
    // ********************************************************************************
    TiXmlElement* pDistanceJointElement = new TiXmlElement( "xs:element" );
    pDistanceJointElement->SetAttribute( "name", jointDistanceNodeName );
    pDistanceJointElement->SetAttribute( "minOccurs", 0 );
    pDistanceJointElement->SetAttribute( "maxOccurs", 1 );
    pJointsNodeChoiceElement->LinkEndChild( pDistanceJointElement );

    // Create complex type Element.
    TiXmlElement* pDistanceJointComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pDistanceJointElement->LinkEndChild( pDistanceJointComplexTypeElement );

    // Create "Length" attribute.
    TiXmlElement* pDistanceJointElementA = new TiXmlElement( "xs:attribute" );
    pDistanceJointElementA->SetAttribute( "name", jointDistanceLengthName );
    pDistanceJointComplexTypeElement->LinkEndChild( pDistanceJointElementA );
    TiXmlElement* pDistanceJointElementB = new TiXmlElement( "xs:simpleType" );
    pDistanceJointElementA->LinkEndChild( pDistanceJointElementB );
    TiXmlElement* pDistanceJointElementC = new TiXmlElement( "xs:restriction" );
    pDistanceJointElementC->SetAttribute( "base", "xs:float" );
    pDistanceJointElementB->LinkEndChild( pDistanceJointElementC );
    TiXmlElement* pDistanceJointElementD = new TiXmlElement( "xs:minInclusive" );
    pDistanceJointElementD->SetAttribute( "value", "0" );
    pDistanceJointElementC->LinkEndChild( pDistanceJointElementD );

    // Create "Frequency" attribute.
    pDistanceJointElementA = new TiXmlElement( "xs:attribute" );
    pDistanceJointElementA->SetAttribute( "name", jointDistanceFrequencyName );
    pDistanceJointComplexTypeElement->LinkEndChild( pDistanceJointElementA );
    pDistanceJointElementB = new TiXmlElement( "xs:simpleType" );
    pDistanceJointElementA->LinkEndChild( pDistanceJointElementB );
    pDistanceJointElementC = new TiXmlElement( "xs:restriction" );
    pDistanceJointElementC->SetAttribute( "base", "xs:float" );
    pDistanceJointElementB->LinkEndChild( pDistanceJointElementC );
    pDistanceJointElementD = new TiXmlElement( "xs:minInclusive" );
    pDistanceJointElementD->SetAttribute( "value", "0" );
    pDistanceJointElementC->LinkEndChild( pDistanceJointElementD );

    // Create "Damping Ratio" attribute.
    pDistanceJointElementA = new TiXmlElement( "xs:attribute" );
    pDistanceJointElementA->SetAttribute( "name", jointDistanceDampingRatioName );
    pDistanceJointComplexTypeElement->LinkEndChild( pDistanceJointElementA );
    pDistanceJointElementB = new TiXmlElement( "xs:simpleType" );
    pDistanceJointElementA->LinkEndChild( pDistanceJointElementB );
    pDistanceJointElementC = new TiXmlElement( "xs:restriction" );
    pDistanceJointElementC->SetAttribute( "base", "xs:float" );
    pDistanceJointElementB->LinkEndChild( pDistanceJointElementC );
    pDistanceJointElementD = new TiXmlElement( "xs:minInclusive" );
    pDistanceJointElementD->SetAttribute( "value", "0" );
    pDistanceJointElementC->LinkEndChild( pDistanceJointElementD );
}

//-----------------------------------------------------------------------------

static void WriteControllersCustomTamlScehema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "Taml::WriteControllersCustomTamlScehema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "Taml::WriteControllersCustomTamlScehema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create controllers node element.
    TiXmlElement* pControllersNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), controllerCustomNodeName );
    pControllersNodeElement->SetAttribute( "name", buffer );
    pControllersNodeElement->SetAttribute( "minOccurs", 0 );
    pControllersNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pControllersNodeElement );
    
    // Create complex type.
    TiXmlElement* pControllersNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pControllersNodeElement->LinkEndChild( pControllersNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pControllersNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pControllersNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pControllersNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pControllersNodeComplexTypeElement->LinkEndChild( pControllersNodeChoiceElement );

    // Fetch the scene controller type.
    AbstractClassRep* pPickingSceneControllerType = AbstractClassRep::findClassRep( "PickingSceneController" );
    AbstractClassRep* pGroupedSceneControllerType = AbstractClassRep::findClassRep( "GroupedSceneController" );

    // Sanity!
    AssertFatal( pPickingSceneControllerType != NULL, "Scene::WriteControllersCustomTamlScehema() - Cannot find the PickingSceneController type." );
    AssertFatal( pGroupedSceneControllerType != NULL, "Scene::WriteControllersCustomTamlScehema() - Cannot find the GroupedSceneController type." );

    // Add choice members.
    for ( AbstractClassRep* pChoiceType = AbstractClassRep::getClassList(); pChoiceType != NULL; pChoiceType = pChoiceType->getNextClass() )
    {
        // Skip if not derived from either of the scene controllers.
        if ( !pChoiceType->isClass( pPickingSceneControllerType ) && !pChoiceType->isClass( pGroupedSceneControllerType ) )
            continue;

        // Add choice member.
        TiXmlElement* pChoiceMemberElement = new TiXmlElement( "xs:element" );
        pChoiceMemberElement->SetAttribute( "name", pChoiceType->getClassName() );
        dSprintf( buffer, sizeof(buffer), "%s_Type", pChoiceType->getClassName() );
        pChoiceMemberElement->SetAttribute( "type", buffer );
        pControllersNodeChoiceElement->LinkEndChild( pChoiceMemberElement );
    }
}

//-----------------------------------------------------------------------------

static void WritePreloadsCustomTamlScehema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "Taml::WritePreloadsCustomTamlScehema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "Taml::WritePreloadsCustomTamlScehema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create preloads node element.
    TiXmlElement* pPreloadsNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), assetPreloadNodeName );
    pPreloadsNodeElement->SetAttribute( "name", buffer );
    pPreloadsNodeElement->SetAttribute( "minOccurs", 0 );
    pPreloadsNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pPreloadsNodeElement );
    
    // Create complex type.
    TiXmlElement* pPreloadsNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pPreloadsNodeElement->LinkEndChild( pPreloadsNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pPreloadsNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pPreloadsNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pPreloadsNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pPreloadsNodeComplexTypeElement->LinkEndChild( pPreloadsNodeChoiceElement );

    // Add choice member element.
    TiXmlElement* pChoiceMemberElement = new TiXmlElement( "xs:element" );
    pChoiceMemberElement->SetAttribute( "name", assetNodeName );
    pPreloadsNodeChoiceElement->LinkEndChild( pChoiceMemberElement );

    // Add choice member complex type element.
    TiXmlElement* pChoiceMemberComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pChoiceMemberElement->LinkEndChild( pChoiceMemberComplexTypeElement );

    // Add choice member attribute element.
    TiXmlElement* pChoiceAttributeElement = new TiXmlElement( "xs:attribute" );
    pChoiceAttributeElement->SetAttribute( "name", "Id" );
    dSprintf( buffer, sizeof(buffer), "%s", "AssetId_ConsoleType" );
    pChoiceAttributeElement->SetAttribute( "type", buffer );
    pChoiceMemberComplexTypeElement->LinkEndChild( pChoiceAttributeElement );
}

//-----------------------------------------------------------------------------

static void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "Taml::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "Taml::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    WriteJointsCustomTamlScehema( pClassRep, pParentElement );
    WriteControllersCustomTamlScehema( pClassRep, pParentElement );
    WritePreloadsCustomTamlScehema( pClassRep, pParentElement );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_CHILDREN_SCHEMA(Scene, WriteCustomTamlSchema);
