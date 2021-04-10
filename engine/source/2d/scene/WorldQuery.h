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

#ifndef _WORLD_QUERY_H_
#define _WORLD_QUERY_H_

#ifndef _WORLD_QUERY_FILTER_H_
#include "2d/scene/WorldQueryFilter.h"
#endif

#ifndef _WORLD_QUERY_RESULT_H_
#include "2d/scene/WorldQueryResult.h"
#endif

///-----------------------------------------------------------------------------

class Scene;

///-----------------------------------------------------------------------------

class WorldQuery :
    protected b2DynamicTree,
    public b2QueryCallback,
    public b2RayCastCallback,
    public SimObject
{
public:
    WorldQuery( Scene* pScene );
    virtual         ~WorldQuery() {}

    /// Standard scope.
    S32             add( SceneObject* pSceneObject );
    void            remove( SceneObject* pSceneObject );
    bool            update( SceneObject* pSceneObject, const b2AABB& aabb, const b2Vec2& displacement );

    /// Always in scope.
    void            addAlwaysInScope( SceneObject* pSceneObject );
    void            removeAlwaysInScope( SceneObject* pSceneObject );

    /// World collision-shape queries.
    U32             collisionQueryAABB( const b2AABB& aabb );
    U32             collisionQueryRay( const Vector2& point1, const Vector2& point2 );
    U32             collisionQueryPoint( const Vector2& point );
    U32             collisionQueryCircle( const Vector2& centroid, const F32 radius );

    /// AABB queries.
    U32             aabbQueryAABB( const b2AABB& aabb );
    U32             aabbQueryRay( const Vector2& point1, const Vector2& point2 );
    U32             aabbQueryPoint( const Vector2& point );
    U32             aabbQueryCircle( const Vector2& centroid, const F32 radius );

    /// OOBB queries.
    U32             oobbQueryAABB( const b2AABB& aabb );
    U32             oobbQueryRay( const Vector2& point1, const Vector2& point2 );
    U32             oobbQueryPoint( const Vector2& point );
    U32             oobbQueryCircle( const Vector2& centroid, const F32 radius );

    /// Any queries.
    U32             anyQueryAABB( const b2AABB& aabb );
    U32             anyQueryArea(const Vector2 & lower, const Vector2 & upper);
    U32             anyQueryRay( const Vector2& point1, const Vector2& point2 );
    U32             anyQueryPoint( const Vector2& point );
    U32             anyQueryCircle( const Vector2& centroid, const F32 radius );

    /// Filtering.
    inline void     setQueryFilter( const WorldQueryFilter& queryFilter ) { mQueryFilter = queryFilter; }
   
    /// Results.
    void            clearQuery( void );
    typeWorldQueryResultVector& getLayeredQueryResults( const U32 layer );
    typeWorldQueryResultVector& getQueryResults( void ) { return mQueryResults; }
    inline U32      getQueryResultsCount( void ) const { return mQueryResults.size(); }
    inline bool     getIsRaycastQueryResult( void ) const { return mIsRaycastQueryResult; }
    void            sortRaycastQueryResult( void );

    /// Callbacks.
    virtual bool    ReportFixture( b2Fixture* fixture );
    virtual F32     ReportFixture( b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, F32 fraction );
    bool            QueryCallback( S32 proxyId );
    F32             RayCastCallback( const b2RayCastInput& input, S32 proxyId );

private:
    void            injectAlwaysInScope( void );
    static S32      QSORT_CALLBACK rayCastFractionSort(const void* a, const void* b);

private:
    Scene*                      mpScene;
    WorldQueryFilter            mQueryFilter;
    b2PolygonShape              mComparePolygonShape;
    b2CircleShape               mCompareCircleShape;
    b2RayCastInput              mCompareRay;
    b2Vec2                      mComparePoint;
    b2Transform                 mCompareTransform;
    bool                        mCheckPoint;
    bool                        mCheckAABB;
    bool                        mCheckOOBB;
    bool                        mCheckCircle;
    typeWorldQueryResultVector  mLayeredQueryResults[MAX_LAYERS_SUPPORTED];
    typeWorldQueryResultVector  mQueryResults;
    bool                        mIsRaycastQueryResult;
    typeSceneObjectVector       mAlwaysInScopeSet;
    U32                         mMasterQueryKey;
};

#endif // _WORLD_QUERY_H_