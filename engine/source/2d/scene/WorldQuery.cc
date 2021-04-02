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

#include "2d/scene/WorldQuery.h"

#ifndef _SCENE_H_
#include "Scene.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

WorldQuery::WorldQuery( Scene* pScene ) :
        mpScene(pScene),
        mIsRaycastQueryResult(false),
        mMasterQueryKey(0),
        mCheckPoint(false),
        mCheckAABB(false),
        mCheckOOBB(false),
        mCheckCircle(false)
{
    // Set debug associations.
    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
        VECTOR_SET_ASSOCIATION( mLayeredQueryResults[n] );
    }
    VECTOR_SET_ASSOCIATION( mQueryResults );

    // Clear the query.
    clearQuery();
}

//-----------------------------------------------------------------------------

S32 WorldQuery::add( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Add);

    return CreateProxy( pSceneObject->getAABB(), static_cast<PhysicsProxy*>(pSceneObject) );
}

//-----------------------------------------------------------------------------

void WorldQuery::remove( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Remove);

    DestroyProxy( pSceneObject->getWorldProxy() );
}

//-----------------------------------------------------------------------------

bool WorldQuery::update( SceneObject* pSceneObject, const b2AABB& aabb, const b2Vec2& displacement )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_Update);

    return MoveProxy( pSceneObject->getWorldProxy(), aabb, displacement );
}

//-----------------------------------------------------------------------------

void WorldQuery::addAlwaysInScope( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AddAlwaysInScope);

    // Sanity!
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        AssertFatal( (*itr) != pSceneObject, "Object attempted to be in Always-in-Scope more than once." );
    }

    // Add to always-in-scope.
    mAlwaysInScopeSet.push_back( pSceneObject );

    // Set always in scope.
    pSceneObject->mAlwaysInScope = true;
}

//-----------------------------------------------------------------------------

void WorldQuery::removeAlwaysInScope( SceneObject* pSceneObject )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RemoveAlwaysInScope);

    // Remove from always-in-scope.
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        // Skip if not object.
        if ( (*itr) != pSceneObject )
            continue;

        mAlwaysInScopeSet.erase_fast( itr );

        // Reset always in scope.
        pSceneObject->mAlwaysInScope = false;

        return;
    }

    AssertFatal( false, "Object attempted to be removed from Always-in-Scope but wasn't present." );
}

//-----------------------------------------------------------------------------

U32 WorldQuery::collisionQueryAABB( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_collisionQueryAABB);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2Vec2 verts[4];
    verts[0].Set( aabb.lowerBound.x, aabb.lowerBound.y );
    verts[1].Set( aabb.upperBound.x, aabb.lowerBound.y );
    verts[2].Set( aabb.upperBound.x, aabb.upperBound.y );
    verts[3].Set( aabb.lowerBound.x, aabb.upperBound.y );
    mComparePolygonShape.Set( verts, 4 );
    mCompareTransform.SetIdentity();
    mCheckAABB = true;
    mpScene->getWorld()->QueryAABB( this, aabb );
    mCheckAABB = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::collisionQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_CollisionQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    mpScene->getWorld()->RayCast( this, point1, point2 );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::collisionQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_CollisionQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    aabb.lowerBound = point;
    aabb.upperBound = point;
    mCheckPoint = true;
    mComparePoint = point;
    mpScene->getWorld()->QueryAABB( this, aabb );
    mCheckPoint = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::collisionQueryCircle( const Vector2& centroid, const F32 radius )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_CollisionQueryCircle);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    mCompareTransform.SetIdentity();
    mCompareCircleShape.m_p = centroid;
    mCompareCircleShape.m_radius = radius;
    mCompareCircleShape.ComputeAABB( &aabb, mCompareTransform, 0 );
    mCheckCircle = true;
    mpScene->getWorld()->QueryAABB( this, aabb );
    mCheckCircle = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::aabbQueryAABB( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_aabbQueryAABB);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    Query( this, aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::aabbQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AABBQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    mCompareRay.p1 = point1;
    mCompareRay.p2 = point2;
    mCompareRay.maxFraction = 1.0f;
    mCompareTransform.SetIdentity();
    RayCast( this, mCompareRay );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::aabbQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AABBQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    aabb.lowerBound = point;
    aabb.upperBound = point;
    Query( this, aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::aabbQueryCircle( const Vector2& centroid, const F32 radius )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AABBQueryCircle);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    mCompareTransform.SetIdentity();
    mCompareCircleShape.m_p = centroid;
    mCompareCircleShape.m_radius = radius;
    mCompareCircleShape.ComputeAABB( &aabb, mCompareTransform, 0 );
    mCheckCircle = true;
    Query( this, aabb );
    mCheckCircle = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::oobbQueryAABB( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_aabbQueryAABB);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2Vec2 verts[4];
    verts[0].Set( aabb.lowerBound.x, aabb.lowerBound.y );
    verts[1].Set( aabb.upperBound.x, aabb.lowerBound.y );
    verts[2].Set( aabb.upperBound.x, aabb.upperBound.y );
    verts[3].Set( aabb.lowerBound.x, aabb.upperBound.y );
    mComparePolygonShape.Set( verts, 4 );
    mCompareTransform.SetIdentity();
    mCheckOOBB = true;
    mCheckAABB = true;
    Query( this, aabb );
    mCheckAABB = false;
    mCheckOOBB = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::oobbQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AABBQueryRay);

    mMasterQueryKey++;

    // Flag as a ray-cast query result.
    mIsRaycastQueryResult = true;

    // Query.
    mCompareRay.p1 = point1;
    mCompareRay.p2 = point2;
    mCompareRay.maxFraction = 1.0f;
    mCompareTransform.SetIdentity();
    mCheckOOBB = true;
    RayCast( this, mCompareRay );
    mCheckOOBB = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::oobbQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AABBQueryPoint);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    aabb.lowerBound = point;
    aabb.upperBound = point;
    mComparePoint = point;
    mCompareTransform.SetIdentity();
    mCheckOOBB = true;
    mCheckPoint = true;
    Query( this, aabb );
    mCheckPoint = false;
    mCheckOOBB = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::oobbQueryCircle( const Vector2& centroid, const F32 radius )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_OOBBQueryCircle);

    mMasterQueryKey++;

    // Flag as not a ray-cast query result.
    mIsRaycastQueryResult = false;

    // Query.
    b2AABB aabb;
    mCompareTransform.SetIdentity();
    mCompareCircleShape.m_p = centroid;
    mCompareCircleShape.m_radius = radius;
    mCompareCircleShape.ComputeAABB( &aabb, mCompareTransform, 0 );
    mCheckOOBB = true;
    mCheckCircle = true;
    Query( this, aabb );
    mCheckCircle = false;
    mCheckOOBB = false;

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryAABB( const b2AABB& aabb )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_anyQueryAABBAABB);

    // Query.
    oobbQueryAABB( aabb );
    mMasterQueryKey--;
    collisionQueryAABB( aabb );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

U32 WorldQuery::anyQueryArea(const Vector2& lower, const Vector2& upper)
{
   b2AABB aabb;
   aabb.lowerBound.Set(getMin(lower.x, lower.x), getMin(lower.x, upper.x));
   aabb.upperBound.Set(getMax(lower.x, upper.x), getMax(lower.x, upper.x));

   return anyQueryAABB(aabb);
}


//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryRay( const Vector2& point1, const Vector2& point2 )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryRay);

    // Query.
    oobbQueryRay( point1, point2 );
    mMasterQueryKey--;
    collisionQueryRay( point1, point2 );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryPoint( const Vector2& point )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryPoint);

    // Query.
    oobbQueryPoint( point );
    mMasterQueryKey--;
    collisionQueryPoint( point );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

U32 WorldQuery::anyQueryCircle( const Vector2& centroid, const F32 radius )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_AnyQueryCircle);

    // Query.
    oobbQueryCircle( centroid, radius );
    mMasterQueryKey--;
    collisionQueryCircle( centroid, radius );

    // Inject always-in-scope.
    injectAlwaysInScope();

    return getQueryResultsCount();
}

//-----------------------------------------------------------------------------

void WorldQuery::clearQuery( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ClearQuery);

    for ( U32 n = 0; n < MAX_LAYERS_SUPPORTED; n++ )
    {
        mLayeredQueryResults[n].clear();
    }

    mQueryResults.clear();
}

//-----------------------------------------------------------------------------

typeWorldQueryResultVector& WorldQuery::getLayeredQueryResults( const U32 layer ) 
{
    // Sanity!
    AssertFatal( layer < MAX_LAYERS_SUPPORTED, "WorldQuery::getResults() - Layer out of range." );

    return mLayeredQueryResults[ layer ];
}

//-----------------------------------------------------------------------------

void WorldQuery::sortRaycastQueryResult( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_SortRayCastQueryResult);

    // Ignore if not a ray-cast query result or there are not results to sort.
    if ( !getIsRaycastQueryResult() || getQueryResultsCount() == 0 )
        return;

    // Sort query results.
    dQsort( mQueryResults.address(), mQueryResults.size(), sizeof(WorldQueryResult), rayCastFractionSort );

    for ( U32 layer = 0; layer < MAX_LAYERS_SUPPORTED; ++layer )
    {
        // Fetch layer query results.
        typeWorldQueryResultVector& layerQueryResults = mLayeredQueryResults[layer];

        // Skip if nothing in the layer query results.
        if ( layerQueryResults.size() == 0 )
            continue;

        // Sort query results.
        dQsort( layerQueryResults.address(), layerQueryResults.size(), sizeof(WorldQueryResult), rayCastFractionSort );
    }
}

//-----------------------------------------------------------------------------

bool WorldQuery::ReportFixture( b2Fixture* fixture )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ReportFixture);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(fixture->GetBody()->GetUserData());
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return true;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return true;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return true;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return true;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getPickingAllowed() )
        return true;

    // Check collision point.
    if ( mCheckPoint && !fixture->TestPoint( mComparePoint ) )
        return true;

    // Check collision AABB.
    if ( mCheckAABB )
        if ( !b2TestOverlap( &mComparePolygonShape, 0, fixture->GetShape(), 0, mCompareTransform, fixture->GetBody()->GetTransform() ) )
            return true;

    // Check collision circle.
    if ( mCheckCircle )
        if ( !b2TestOverlap( &mCompareCircleShape, 0, fixture->GetShape(), 0, mCompareTransform, fixture->GetBody()->GetTransform() ) )
            return true;

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );

        // Tag with world query key.
        pSceneObject->setWorldQueryKey( mMasterQueryKey );
    }

    return true;
}

//-----------------------------------------------------------------------------

F32 WorldQuery::ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, F32 fraction )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_ReportFixtureRay);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(fixture->GetBody()->GetUserData());
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return 1.0f;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return 1.0f;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return 1.0f;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return 1.0f;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getPickingAllowed() )
        return 1.0f;

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Fetch collision shape index.
    const S32 shapeIndex = pSceneObject->getCollisionShapeIndex( fixture );

    // Sanity!
    AssertFatal( shapeIndex >= 0, "WorldQuery::ReportFixture() - Cannot find shape index reported on physics proxy of a fixture." );

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject, point, normal, fraction, (U32)shapeIndex );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );

        // Tag with world query key.
        pSceneObject->setWorldQueryKey( mMasterQueryKey );
    }

    return 1.0f;
}

//-----------------------------------------------------------------------------

bool WorldQuery::QueryCallback( S32 proxyId )
{    
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_QueryCallback);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(GetUserData( proxyId ));
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return true;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return true;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return true;

    // Visible filter.  If an object has a size x or y value of zero then they are treated here as invisible.
    if (mQueryFilter.mVisibleFilter && (!pSceneObject->getVisible() || pSceneObject->getSize().isXZero() || pSceneObject->getSize().isYZero()))
        return true;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getPickingAllowed() )
        return true;

    // Check OOBB.
    if ( mCheckOOBB )
    {
        // Fetch the shapes render OOBB.
        b2PolygonShape oobb;
        oobb.Set( pSceneObject->getRenderOOBB(), 4);

        // Check point.
        if ( mCheckPoint )
        {
            if ( !oobb.TestPoint( mCompareTransform, mComparePoint ) )
                return true;
        }
        // Check AABB.
        else if ( mCheckAABB )
        {
            if ( !b2TestOverlap( &mComparePolygonShape, 0, &oobb, 0, mCompareTransform, mCompareTransform ) )
                return true;
        }
        // Check circle.
        else if ( mCheckCircle )
        {
            if ( !b2TestOverlap( &mCompareCircleShape, 0, &oobb, 0, mCompareTransform, mCompareTransform ) )
                return true;
        }
    }
    // Check circle.
    else if ( mCheckCircle )
    {
        // Fetch the shapes AABB.
        b2AABB aabb = pSceneObject->getAABB();
        b2Vec2 verts[4];
        verts[0].Set( aabb.lowerBound.x, aabb.lowerBound.y );
        verts[1].Set( aabb.upperBound.x, aabb.lowerBound.y );
        verts[2].Set( aabb.upperBound.x, aabb.upperBound.y );
        verts[3].Set( aabb.lowerBound.x, aabb.upperBound.y );
        b2PolygonShape shapeAABB;
        shapeAABB.Set( verts, 4);
        if ( !b2TestOverlap( &mCompareCircleShape, 0, &shapeAABB, 0, mCompareTransform, mCompareTransform ) )
            return true;
    }


    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );

        // Tag with world query key.
        pSceneObject->setWorldQueryKey( mMasterQueryKey );
    }

    return true;
}

//-----------------------------------------------------------------------------

F32 WorldQuery::RayCastCallback( const b2RayCastInput& input, S32 proxyId )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RayCastCallback);

    // If not the correct proxy then ignore.
    PhysicsProxy* pPhysicsProxy = static_cast<PhysicsProxy*>(GetUserData( proxyId ));
    if ( pPhysicsProxy->getPhysicsProxyType() != PhysicsProxy::PHYSIC_PROXY_SCENEOBJECT )
        return 1.0f;

    // Fetch scene object.
    SceneObject* pSceneObject = static_cast<SceneObject*>(pPhysicsProxy);

    // Ignore if already tagged with the world query key.
    if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
        return 1.0f;

    // Enabled filter.
    if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
        return 1.0f;

    // Visible filter.
    if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
        return 1.0f;

    // Picking allowed filter.
    if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getPickingAllowed() )
        return 1.0f;

    // Check OOBB.
    if ( mCheckOOBB )
    {
        // Fetch the shapes render OOBB.
        b2PolygonShape oobb;
        oobb.Set( pSceneObject->getRenderOOBB(), 4);
        b2RayCastOutput rayOutput;
        if ( !oobb.RayCast( &rayOutput, mCompareRay, mCompareTransform, 0 ) )
            return true;
    }

    // Fetch layer and group masks.
    const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
    const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

    // Compare masks and report.
    if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
    {
        WorldQueryResult queryResult( pSceneObject );
        mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
        mQueryResults.push_back( queryResult );

        // Tag with world query key.
        pSceneObject->setWorldQueryKey( mMasterQueryKey );
    }

    return 1.0f;
}

//-----------------------------------------------------------------------------

void WorldQuery::injectAlwaysInScope( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_InjectAlwaysInScope);

    // Finish if filtering always-in-scope.
    if ( mQueryFilter.mAlwaysInScopeFilter )
        return;

    // Iterate always-in-scope.
    for( typeSceneObjectVector::iterator itr = mAlwaysInScopeSet.begin(); itr != mAlwaysInScopeSet.end(); ++itr )
    {
        // Fetch scene object.
        SceneObject* pSceneObject = (*itr);

        // Ignore if already tagged with the world query key.
        if ( pSceneObject->getWorldQueryKey() == mMasterQueryKey )
            continue;

        // Enabled filter.
        if ( mQueryFilter.mEnabledFilter && !pSceneObject->isEnabled() )
            continue;

        // Visible filter.
        if ( mQueryFilter.mVisibleFilter && !pSceneObject->getVisible() )
            continue;

        // Picking allowed filter.
        if ( mQueryFilter.mPickingAllowedFilter && !pSceneObject->getPickingAllowed() )
            continue;

        // Fetch layer and group masks.
        const U32 sceneLayerMask = pSceneObject->getSceneLayerMask();
        const U32 sceneGroupMask = pSceneObject->getSceneGroupMask();

        // Compare masks and report.
        if ( (mQueryFilter.mSceneLayerMask & sceneLayerMask) != 0 && (mQueryFilter.mSceneGroupMask & sceneGroupMask) != 0 )
        {
            WorldQueryResult queryResult( pSceneObject );
            mLayeredQueryResults[pSceneObject->getSceneLayer()].push_back( queryResult );
            mQueryResults.push_back( queryResult );

            // Tag with world query key.
            pSceneObject->setWorldQueryKey( mMasterQueryKey );
        }
    }
}

//-----------------------------------------------------------------------------

S32 QSORT_CALLBACK WorldQuery::rayCastFractionSort(const void* a, const void* b)
{
    // Debug Profiling.
    PROFILE_SCOPE(WorldQuery_RayCastFractionSort);

    // Fetch scene objects.
    WorldQueryResult* pQueryResultA  = (WorldQueryResult*)a;
    WorldQueryResult* pQueryResultB  = (WorldQueryResult*)b;

    // Fetch fractions.
    const F32 queryFractionA = pQueryResultA->mFraction;
    const F32 queryFractionB = pQueryResultB->mFraction;

    if ( queryFractionA < queryFractionB )
        return -1;

    if ( queryFractionA > queryFractionB )
        return 1;

    return 0;
}

