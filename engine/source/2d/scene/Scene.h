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
#define _SCENE_H_

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

#ifndef _NETOBJECT_H_
#include "network/netObject.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _PHYSICS_PROXY_H_
#include "2d/scene/PhysicsProxy.h"
#endif

#ifndef _WORLD_QUERY_H_
#include "2d/scene/WorldQuery.h"
#endif

#ifndef _DEBUG_DRAW_H_
#include "2d/scene/DebugDraw.h"
#endif

#ifndef _HASHTABLE_H
#include "collection/hashTable.h"
#endif

#ifndef _BATCH_RENDER_H_
#include "2d/core/BatchRender.h"
#endif

#ifndef _SCENE_RENDER_QUEUE_H_
#include "2d/scene/SceneRenderQueue.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/scene/SceneRenderObject.h"
#endif

#ifndef _BEHAVIOR_COMPONENT_H_
#include "component/behaviors/behaviorComponent.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

//-----------------------------------------------------------------------------

extern EnumTable jointTypeTable;

///-----------------------------------------------------------------------------

class SceneObject;
class SceneWindow;

///-----------------------------------------------------------------------------

struct tDeleteRequest
{
    SimObjectId     mObjectId;
    SceneObject*    mpSceneObject;
    bool            mSafeDeleteReady;
};

///-----------------------------------------------------------------------------

struct TickContact
{
    TickContact()
    {
        initialize( NULL, NULL, NULL, NULL, NULL );
    }

    void initialize(
        b2Contact*      pContact,
        SceneObject*    pSceneObjectA,
        SceneObject*    pSceneObjectB,
        b2Fixture*      pFixtureA,
        b2Fixture*      pFixtureB )
    {
        mpContact      = pContact;
        mpSceneObjectA = pSceneObjectA;
        mpSceneObjectB = pSceneObjectB;
        mpFixtureA     = pFixtureA;
        mpFixtureB     = pFixtureB;

        // Get world manifold. 
        if ( mpContact != NULL )
        {
            mPointCount = pContact->GetManifold()->pointCount;
            mpContact->GetWorldManifold( &mWorldManifold );
        }
        else
        {
            mPointCount = 0;
        }

        // Reset impulses.
        for (U32 i = 0; i < b2_maxManifoldPoints; i++)
        {
            mNormalImpulses[i] = 0;
            mTangentImpulses[i] = 0;
        }
    }

    inline SceneObject* getCollideWith( SceneObject* pMe ) const
    {
        return pMe == mpSceneObjectA ? mpSceneObjectB : mpSceneObjectA;
    }

    inline b2Fixture* getCollideWithFixture( b2Fixture* pMe ) const
    {
        return pMe == mpFixtureA ? mpFixtureB : mpFixtureA;
    }

    b2Contact*      mpContact;
    SceneObject*    mpSceneObjectA;
    SceneObject*    mpSceneObjectB;
    b2Fixture*      mpFixtureA;
    b2Fixture*      mpFixtureB;
    U32             mPointCount;
    b2WorldManifold mWorldManifold;
    F32             mNormalImpulses[b2_maxManifoldPoints];
    F32             mTangentImpulses[b2_maxManifoldPoints];
};

///-----------------------------------------------------------------------------

class Scene :
    public BehaviorComponent,
    public TamlChildren,
    public PhysicsProxy,
    public b2ContactListener,
    public b2DestructionListener,
    public virtual Tickable
{
public:
    typedef HashMap<S32, b2Joint*>              typeJointHash;
    typedef HashMap<b2Joint*, S32>              typeReverseJointHash;
    typedef Vector<tDeleteRequest>              typeDeleteVector;
    typedef Vector<TickContact>                 typeContactVector;
    typedef HashMap<b2Contact*, TickContact>    typeContactHash;
    typedef Vector<AssetPtr<AssetBase>*>        typeAssetPtrVector;

    /// Scene Debug Options.
    enum DebugOption
    {
        SCENE_DEBUG_INVALID,
        ///
        SCENE_DEBUG_METRICS            = BIT(0),
        SCENE_DEBUG_FPS_METRICS        = BIT(1),
        SCENE_DEBUG_CONTROLLERS        = BIT(2), 
        SCENE_DEBUG_JOINTS             = BIT(3),
        SCENE_DEBUG_WIREFRAME_RENDER   = BIT(4),
        SCENE_DEBUG_AUDIO_SOURCES      = BIT(5),
        ///
        SCENE_DEBUG_AABB               = BIT(16),
        SCENE_DEBUG_OOBB               = BIT(17),
        SCENE_DEBUG_SLEEP              = BIT(18),
        SCENE_DEBUG_COLLISION_SHAPES   = BIT(19),
        SCENE_DEBUG_POSITION_AND_COM   = BIT(20),
        SCENE_DEBUG_SORT_POINTS        = BIT(21),
    };

    /// Pick mode.
    enum PickMode
    {
        PICK_INVALID,
        ///---
        PICK_ANY,
        PICK_AABB,
        PICK_OOBB,
        PICK_COLLISION,
    };

    /// Debug drawing.
    DebugDraw                   mDebugDraw;

private:
    typedef BehaviorComponent   Parent;
    typedef SceneObject         Children;

    /// World.
    b2World*                    mpWorld;
    WorldQuery*                 mpWorldQuery;
    b2Vec2                      mWorldGravity;
    S32                         mVelocityIterations;
    S32                         mPositionIterations;
    b2BlockAllocator            mBlockAllocator;
    b2Body*                     mpGroundBody;

    /// Scene occupancy.
    typeSceneObjectVector       mSceneObjects;
    typeSceneObjectVector       mTickedSceneObjects;

    /// Joint access.
    typeJointHash               mJoints;
    typeReverseJointHash        mReverseJoints;
    S32                         mJointMasterId;

    /// Scene controllers.
    SimObjectPtr<SimSet>	    mControllers;

    /// Asset pre-loads.
    typeAssetPtrVector          mAssetPreloads;

    /// Scene time.
    F32                         mSceneTime;
    bool                        mScenePause;

    /// Debug and metrics.
    DebugStats                  mDebugStats;
    U32                         mDebugMask;
    SceneObject*                mpDebugSceneObject;

    /// Layer sorting and draw order.
    SceneRenderQueue::RenderSort mLayerSortModes[MAX_LAYERS_SUPPORTED];

    /// Batch rendering.
    BatchRender                 mBatchRenderer;

    /// Window rendering.
    SceneWindow*                mpCurrentRenderWindow;

    /// Window attachments.
    SimSet                      mAttachedSceneWindows;

    /// Delete requests.
    typeDeleteVector            mDeleteRequests;
    typeDeleteVector            mDeleteRequestsTemp;
  
    /// Miscellaneous.
    S32                         mIsEditorScene;
    bool                        mUpdateCallback;
    bool                        mRenderCallback;
    typeContactHash             mBeginContacts;
    typeContactVector           mEndContacts;
    U32                         mSceneIndex;

private:   
    /// Contacts.
    void                        forwardContacts( void );
    void                        dispatchBeginContactCallbacks( void );
    void                        dispatchEndContactCallbacks( void );

    /// Joint definition.
    struct CommonJointDefinition
    {
        CommonJointDefinition()
        {
            jointType        = e_unknownJoint;
            collideConnected = false;
            pSceneObjectA    = NULL;
            pSceneObjectB    = NULL;
            localAnchorA     = b2Vec2_zero;
            localAnchorB     = b2Vec2_zero;
        }

        b2JointType         jointType;
        bool                collideConnected;
        SceneObject*        pSceneObjectA;
        SceneObject*        pSceneObjectB;
        b2Vec2              localAnchorA;
        b2Vec2              localAnchorB;
    };

protected:
    /// Taml callbacks.
    virtual void            onTamlPreRead( void );
    virtual void            onTamlPostRead( const TamlCustomNodes& customNodes );
    virtual void            onTamlCustomWrite( TamlCustomNodes& customNodes );
    virtual void            onTamlCustomRead( const TamlCustomNodes& customNodes );

public:
    Scene();
    virtual ~Scene();

    /// Engine.
    virtual bool            onAdd();
    virtual void            onRemove();
    virtual void            onDeleteNotify( SimObject* object );
    static void             initPersistFields();

    /// Contact processing.
    virtual void            PreSolve( b2Contact* pContact, const b2Manifold* pOldManifold ) {}
    virtual void            PostSolve( b2Contact* pContact, const b2ContactImpulse* pImpulse );
    virtual void            BeginContact( b2Contact* pContact );
    virtual void            EndContact( b2Contact* pContact );
    const typeContactHash&  getBeginContacts( void ) const              { return mBeginContacts; }
    const typeContactVector& getEndContacts( void ) const               { return mEndContacts; }

    /// Integration.
    virtual void            processTick();
    virtual void            interpolateTick( F32 delta );
    virtual void            advanceTime( F32 timeDelta ) {};

    /// Render output.
    void                    sceneRender( const SceneRenderState* pSceneRenderState );

    /// World.
    inline b2World*         getWorld( void ) const                      { return mpWorld; }
    inline WorldQuery*      getWorldQuery( const bool clearQuery = false ) { if ( clearQuery ) mpWorldQuery->clearQuery(); return mpWorldQuery; }
    b2BlockAllocator*       getBlockAllocator( void )                   { return &mBlockAllocator; }
    inline b2Body*          getGroundBody( void ) const                 { return mpGroundBody; }
    virtual ePhysicsProxyType getPhysicsProxyType( void ) const         { return PhysicsProxy::PHYSIC_PROXY_GROUNDBODY; }
    void                    setGravity( const b2Vec2& gravity )         { mWorldGravity = gravity; if (mpWorld) mpWorld->SetGravity( gravity ); }
    inline b2Vec2           getGravity( void )                          { if (mpWorld) mWorldGravity = mpWorld->GetGravity(); return mWorldGravity; }
    inline void             setVelocityIterations( const S32 iterations ) { mVelocityIterations = iterations; }
    inline S32              getVelocityIterations( void ) const         { return mVelocityIterations; }
    inline void             setPositionIterations( const S32 iterations ) { mPositionIterations = iterations; }
    inline S32              getPositionIterations( void ) const         { return mPositionIterations; }

    /// Scene occupancy.
    void                    clearScene( bool deleteObjects = true );
    void                    addToScene( SceneObject* pSceneObject );
    void                    removeFromScene( SceneObject* pSceneObject );

    inline typeSceneObjectVectorConstRef getSceneObjects( void ) const  { return mSceneObjects; }
    inline U32              getSceneObjectCount( void ) const           { return mSceneObjects.size(); }
    SceneObject*            getSceneObject( const U32 objectIndex ) const;
    U32                     getSceneObjects( typeSceneObjectVector& objects ) const;
    U32                     getSceneObjects( typeSceneObjectVector& objects, const U32 sceneLayer ) const;

    void                    mergeScene( const Scene* pScene );

    inline SimSet*			getControllers( void )						{ return mControllers; }

    inline S32              getAssetPreloadCount( void ) const          { return mAssetPreloads.size(); }
    const AssetPtr<AssetBase>* getAssetPreload( const S32 index ) const;
    void                    addAssetPreload( const char* pAssetId );
    void                    removeAssetPreload( const char* pAssetId );
    void                    clearAssetPreloads( void );

    /// Scene time.
    inline F32              getSceneTime( void ) const                  { return mSceneTime; };
    inline void             setScenePause( bool status )                { mScenePause = status; }
    inline bool             getScenePause( void ) const                 { return mScenePause; };

    /// Joint access.
    inline U32              getJointCount( void ) const                 { return mJoints.size(); }
    b2JointType             getJointType( const S32 jointId );
    b2Joint*                findJoint( const S32 jointId );
    S32                     findJointId( b2Joint* pJoint );
    S32                     createJoint( b2JointDef* pJointDef );
    bool                    deleteJoint( const U32 jointId );
    bool                    hasJoints( SceneObject* pSceneObject );

    /// Distance joint.
    S32                     createDistanceJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA = b2Vec2_zero, const b2Vec2& localAnchorB = b2Vec2_zero,
                                const F32 length = -1.0f,
                                const F32 frequency = 0.0f,
                                const F32 dampingRatio = 0.0f,
                                const bool collideConnected = false );

    void                    setDistanceJointLength(
                                const U32 jointId,
                                const F32 length );

    F32                     getDistanceJointLength( const U32 jointId );

    void                    setDistanceJointFrequency(
                                const U32 jointId,
                                const F32 frequency );

    F32                     getDistanceJointFrequency( const U32 jointId );

    void                    setDistanceJointDampingRatio(
                                const U32 jointId,
                                const F32 dampingRatio );

    F32                     getDistanceJointDampingRatio( const U32 jointId );

    /// Rope joint.
    S32                     createRopeJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA = b2Vec2_zero, const b2Vec2& localAnchorB = b2Vec2_zero,
                                const F32 maxLength = -1.0f,
                                const bool collideConnected = false );

    void                    setRopeJointMaxLength(
                                const U32 jointId,
                                const F32 maxLength );

    F32                     getRopeJointMaxLength( const U32 jointId );

    /// Revolute joint.
    S32                     createRevoluteJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA = b2Vec2_zero, const b2Vec2& localAnchorB = b2Vec2_zero,
                                const bool collideConnected = false );

    void                    setRevoluteJointLimit(
                                const U32 jointId,
                                const bool enableLimit,
                                const F32 lowerAngle, const F32 upperAngle );

    bool                    getRevoluteJointLimit(
                                const U32 jointId,
                                bool& enableLimit,
                                F32& lowerAngle, F32& upperAngle );

    void                    setRevoluteJointMotor(
                                const U32 jointId,
                                const bool enableMotor,
                                const F32 motorSpeed = b2_pi,
                                const F32 maxMotorTorque = 0.0f );

    bool                    getRevoluteJointMotor(
                                const U32 jointId,
                                bool& enableMotor,
                                F32& motorSpeed,
                                F32& maxMotorTorque );

	F32                     getRevoluteJointAngle( const U32 jointId );
	F32						getRevoluteJointSpeed( const U32 jointId );

    /// Weld joint.
    S32                     createWeldJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA = b2Vec2_zero, const b2Vec2& localAnchorB = b2Vec2_zero,
                                const F32 frequency = 0.0f,
                                const F32 dampingRatio = 0.0f,
                                const bool collideConnected = false );

    void                    setWeldJointFrequency(
                                const U32 jointId,
                                const F32 frequency );

    F32                     getWeldJointFrequency( const U32 jointId );

    void                    setWeldJointDampingRatio(
                                const U32 jointId,
                                const F32 dampingRatio );

    F32                     getWeldJointDampingRatio( const U32 jointId );

    /// Wheel joint.
    S32                     createWheelJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
                                const b2Vec2& worldAxis,
                                const bool collideConnected = false );

    void                    setWheelJointMotor(
                                const U32 jointId,
                                const bool enableMotor,
                                const F32 motorSpeed = b2_pi,
                                const F32 maxMotorTorque = 0.0f );

    bool                    getWheelJointMotor(
                                const U32 jointId,
                                bool& enableMotor,
                                F32& motorSpeed,
                                F32& maxMotorTorque );

    void                    setWheelJointFrequency(
                                const U32 jointId,
                                const F32 frequency );

    F32                     getWheelJointFrequency( const U32 jointId );

    void                    setWheelJointDampingRatio(
                                const U32 jointId,
                                const F32 dampingRatio );

    F32                     getWheelJointDampingRatio( const U32 jointId );

    /// Friction joint.
    S32                     createFrictionJoint(
                                const SceneObject* pSceneObjectA,const  SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA = b2Vec2_zero, const b2Vec2& localAnchorB = b2Vec2_zero,
                                const F32 maxForce = 0.0f,
                                const F32 maxTorque = 0.0f,
                                const bool collideConnected = false );

    void                    setFrictionJointMaxForce(
                                const U32 jointId,
                                const F32 maxForce );

    F32                     getFrictionJointMaxForce( const U32 jointId );

    void                    setFrictionJointMaxTorque(
                                const U32 jointId,
                                const F32 maxTorque );

    F32                     getFrictionJointMaxTorque( const U32 jointId );

    /// Prismatic joint.
    S32                     createPrismaticJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
                                const b2Vec2& worldAxis,
                                const bool collideConnected = false );

    void                    setPrismaticJointLimit(
                                const U32 jointId,
                                const bool enableLimit,
                                const F32 lowerTranslation, const F32 upperTranslation );

    bool                    getPrismaticJointLimit(
                                const U32 jointId,
                                bool& enableLimit,
                                F32& lowerTranslation, F32& upperTranslation );

    void                    setPrismaticJointMotor(
                                const U32 jointId,
                                const bool enableMotor,
                                const F32 motorSpeed = b2_pi,
                                const F32 maxMotorForce = 0.0f );

    bool                    getPrismaticJointMotor(
                                const U32 jointId,
                                bool& enableMotor,
                                F32& motorSpeed,
                                F32& maxMotorTorque );

    /// Pulley joint.
    S32                     createPulleyJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2& localAnchorA, const b2Vec2& localAnchorB,
                                const b2Vec2& worldGroundAnchorA, const b2Vec2& worldGroundAnchorB,
                                const F32 ratio,
                                const F32 lengthA = -1.0f, const F32 lengthB = -1.0f,
                                const bool collideConnected = false );

    /// Target (a.k.a Mouse) joint.
    S32                     createTargetJoint(
                                const SceneObject* pSceneObject,
                                const b2Vec2& worldTarget,
                                const F32 maxForce,
                                const bool useCenterOfMass = false,
                                const F32 frequency = 5.0f,
                                const F32 dampingRatio = 0.7f,
                                const bool collideConnected = false );

    void                    setTargetJointTarget(
                                const U32 jointId,
                                const b2Vec2& worldTarget );

    b2Vec2                  getTargetJointTarget( const U32 jointId );

    void                    setTargetJointMaxForce(
                                const U32 jointId,
                                const F32 maxForce );

    F32                     getTargetJointMaxForce( const U32 jointId );

    void                    setTargetJointFrequency(
                                const U32 jointId,
                                const F32 frequency );

    F32                     getTargetJointFrequency( const U32 jointId );

    void                    setTargetJointDampingRatio(
                                const U32 jointId,
                                const F32 dampingRatio );

    F32                     getTargetJointDampingRatio( const U32 jointId );

    /// Motor Joint.
    S32                     createMotorJoint(
                                const SceneObject* pSceneObjectA, const SceneObject* pSceneObjectB,
                                const b2Vec2 linearOffset = b2Vec2_zero,
                                const F32 angularOffset = 0.0f,
                                const F32 maxForce = 1.0f,
                                const F32 maxTorque = 1.0f,
                                const F32 correctionFactor = 0.3f,
                                const bool collideConnected = false );

    void                    setMotorJointLinearOffset(
                                const U32 jointId,
                                const b2Vec2& linearOffset );

    b2Vec2                  getMotorJointLinearOffset( const U32 jointId );

    void                    setMotorJointAngularOffset(
                                const U32 jointId,
                                const F32 angularOffset );

    F32                     getMotorJointAngularOffset( const U32 jointId );

    void                    setMotorJointMaxForce(
                                const U32 jointId,
                                const F32 maxForce );

    F32                     getMotorJointMaxForce( const U32 jointId );

    void                    setMotorJointMaxTorque(
                                const U32 jointId,
                                const F32 maxTorque );

    F32                     getMotorJointMaxTorque( const U32 jointId );

    /// Debug and metrics.
    inline void             setDebugOn( const U32 debugMask )           { mDebugMask |= debugMask; }
    inline void             setDebugOff( const U32 debugMask )          { mDebugMask &= ~debugMask; }
    inline U32              getDebugMask( void ) const                  { return mDebugMask; }
    DebugStats&             getDebugStats( void )                       { return mDebugStats; }
    inline void             resetDebugStats( void )                     { mDebugStats.reset(); }
    void                    setDebugSceneObject( SceneObject* pSceneObject );
    inline SceneObject*     getDebugSceneObject( void ) const           { return mpDebugSceneObject; }

    /// Layer sorting.
    void setLayerSortMode( const U32 layer, const SceneRenderQueue::RenderSort sortMode );
    SceneRenderQueue::RenderSort getLayerSortMode( const U32 layer );

    /// Window attachments.
    void                    attachSceneWindow( SceneWindow* pSceneWindow2D );
    void                    detachSceneWindow( SceneWindow* pSceneWindow2D );
    void                    detachAllSceneWindows( void );
    bool                    isSceneWindowAttached( SceneWindow* pSceneWindow2D );
    inline SimSet&          getAttachedSceneWindows( void )             { return mAttachedSceneWindows; }

    /// Delete requests.
    void                    addDeleteRequest( SceneObject* pSceneObject );
    void                    processDeleteRequests( const bool forceImmediate );

    /// Destruction listeners.
    virtual                 void SayGoodbye( b2Joint* pJoint );
    virtual                 void SayGoodbye( b2Fixture* pFixture )      {}

    virtual SceneObject*    create( const char* pType );

    /// Miscellaneous.
    inline void             setBatchingEnabled( const bool enabled )    { mBatchRenderer.setBatchEnabled( enabled ); }
    inline bool             getBatchingEnabled( void ) const            { return mBatchRenderer.getBatchEnabled(); }
    inline bool             getIsEditorScene( void ) const              { return ((mIsEditorScene > 0) ? true : false); }
    inline void             setIsEditorScene( bool status )             { mIsEditorScene += (status ? 1 : -1); }
    static U32              getGlobalSceneCount( void );
    inline U32              getSceneIndex( void ) const                 { return mSceneIndex; }
    inline void             setUpdateCallback( const bool callback )    { mUpdateCallback = callback; }
    inline bool             getUpdateCallback( void ) const             { return mUpdateCallback; }
    inline void             setRenderCallback( const bool callback )    { mRenderCallback = callback; }
    inline bool             getRenderCallback( void ) const             { return mRenderCallback; }
    static SceneRenderRequest* createDefaultRenderRequest( SceneRenderQueue* pSceneRenderQueue, SceneObject* pSceneObject  );

    /// Taml children.
    virtual U32 getTamlChildCount( void ) const                         { return (U32)mSceneObjects.size(); }
    virtual SimObject* getTamlChild( const U32 childIndex ) const;
    virtual void addTamlChild( SimObject* pSimObject );

    static b2JointType getJointTypeEnum(const char* label);
    static const char* getJointTypeDescription( b2JointType jointType );
    static PickMode getPickModeEnum(const char* label);
    static const char* getPickModeDescription( PickMode pickMode );
    static DebugOption getDebugOptionEnum(const char* label);
    static const char* getDebugOptionDescription( DebugOption debugOption );
    b2ParticleSystem*			mParticleSystem;
    /// Declare Console Object.
    DECLARE_CONOBJECT(Scene);

protected:
    /// Physics.
    static bool setGravity( void* obj, const char* data )                           { static_cast<Scene*>(obj)->setGravity( Vector2( data ) ); return false; }
    static const char* getGravity(void* obj, const char* data)                      { return Vector2(static_cast<Scene*>(obj)->getGravity()).scriptThis(); }
    static bool writeGravity( void* obj, StringTableEntry pFieldName )              { return Vector2(static_cast<Scene*>(obj)->getGravity()).notEqual( Vector2::getZero() ); }
    static bool writeVelocityIterations( void* obj, StringTableEntry pFieldName )   { return static_cast<Scene*>(obj)->getVelocityIterations() != 8; }
    static bool writePositionIterations( void* obj, StringTableEntry pFieldName )   { return static_cast<Scene*>(obj)->getPositionIterations() != 3; }

    static bool writeLayerSortMode( void* obj, StringTableEntry pFieldName )
    {
        // Find the layer index portion of the layer sort mode field.
        const char* pLayerNumber = pFieldName;
        while( true )
        {
            // Fetch character.
            char value = *pLayerNumber;

            // Finish if end of the field or is numeric.
            if ( value == 0 || ( value >= '0' && value <= '9' ) )
                break;

            // Move to next value.
            pLayerNumber++;
        };

        // Sanity!
        AssertFatal( *pLayerNumber != 0, "Scene::writeLayerSortMode() - Could not find the layer index portion of the layer sort mode field." );

        // Fetch layer number.
        const U32 layer = dAtoi(pLayerNumber);

        // Just allow the write if an bad parse.
        if ( layer > MAX_LAYERS_SUPPORTED )
            return true;

        return static_cast<Scene*>(obj)->getLayerSortMode( layer ) != SceneRenderQueue::RENDER_SORT_NEWEST;
    }

    // Callbacks.
    static bool writeUpdateCallback( void* obj, StringTableEntry pFieldName )       { return static_cast<Scene*>(obj)->getUpdateCallback(); }
    static bool writeRenderCallback( void* obj, StringTableEntry pFieldName )       { return static_cast<Scene*>(obj)->getRenderCallback(); }

public:
    static SimObjectPtr<Scene> LoadingScene;
};

//-----------------------------------------------------------------------------

extern void findObjectsCallback(SceneObject* pSceneObject, void* storage);
extern void findLayeredObjectsCallback(SceneObject* pSceneObject, void* storage);

#endif // _SCENE_H_
