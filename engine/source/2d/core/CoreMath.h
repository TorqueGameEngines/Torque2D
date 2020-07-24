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

#ifndef _CORE_MATH_H_
#define _CORE_MATH_H_

#ifndef _MRANDOM_H_
#include "math/mRandom.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef B2_COLLISION_H
#include "Box2D/Collision/b2Collision.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMGL_H_
#include "platform/platformGL.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

//-----------------------------------------------------------------------------

struct Vector2;

namespace CoreMath
{
extern RandomLCG gRandomGenerator;

/// Random Float Range.
inline F32 mGetRandomF( F32 from, F32 to ) { return gRandomGenerator.randRangeF( from, to ); }

/// Random Float.
inline F32 mGetRandomF( void ) { return gRandomGenerator.randF(); }

/// Random Integer Range.
inline S32 mGetRandomI( const S32 from, const S32 to ) { return gRandomGenerator.randRangeI( from, to ); }

/// Random Integer.
inline S32 mGetRandomI( void ) { return gRandomGenerator.randI(); }

// Calculate an AABB.
inline void mCalculateAABB( const b2Vec2* const pAABBVertices, const b2Transform& xf, b2AABB* pAABB )
{
    b2Vec2 lower = b2Mul(xf, pAABBVertices[0]);
    b2Vec2 upper = lower;

    for ( S32 i = 1; i < 4; ++i)
    {
        b2Vec2 v = b2Mul( xf, pAABBVertices[i] );
        lower = b2Min(lower, v);
        upper = b2Max(upper, v);
    }
    
    pAABB->lowerBound = lower;
    pAABB->upperBound = upper;
}

/// Calculate an OOBB.
inline void mCalculateOOBB( const b2Vec2* const pAABBVertices, const b2Transform& xf, b2Vec2* pOOBBVertices )
{
    pOOBBVertices[0] = b2Mul( xf, pAABBVertices[0] );
    pOOBBVertices[1] = b2Mul( xf, pAABBVertices[1] );
    pOOBBVertices[2] = b2Mul( xf, pAABBVertices[2] );
    pOOBBVertices[3] = b2Mul( xf, pAABBVertices[3] );
}

/// Calculate an OOBB.
inline void mCalculateInverseOOBB( const b2Vec2* const pAABBVertices, const b2Transform& xf, b2Vec2* pOOBBVertices )
{
    pOOBBVertices[0] = b2MulT( xf, pAABBVertices[0] );
    pOOBBVertices[1] = b2MulT( xf, pAABBVertices[1] );
    pOOBBVertices[2] = b2MulT( xf, pAABBVertices[2] );
    pOOBBVertices[3] = b2MulT( xf, pAABBVertices[3] );
}

/// Convert RectF to AABB.
inline b2AABB mRectFtoAABB( const RectF& rect )
{
    b2AABB aabb;
    b2Vec2 lower(rect.point.x, rect.point.y);
    b2Vec2 upper(lower.x + rect.len_x(), lower.y + rect.len_y() );
    aabb.lowerBound = lower;
    aabb.upperBound = upper;
    return aabb;
}

/// Convert AABB to OOBB.
inline void mAABBtoOOBB( const b2AABB& aabb, b2Vec2* pOOBBVertices )
{
    pOOBBVertices[0].Set( aabb.lowerBound.x, aabb.lowerBound.y );
    pOOBBVertices[1].Set( aabb.upperBound.x, aabb.lowerBound.y );
    pOOBBVertices[2].Set( aabb.upperBound.x, aabb.upperBound.y );
    pOOBBVertices[3].Set( aabb.lowerBound.x, aabb.upperBound.y );
}

/// Convert OOBB to AABB.
inline void mOOBBtoAABB( b2Vec2* pOOBBVertices, b2AABB& aabb )
{
    // Calculate AABB.
    b2Vec2 lower = pOOBBVertices[0];
    b2Vec2 upper = lower;
    for ( U32 index = 1; index < 4; ++index )
    {
        const b2Vec2 v = pOOBBVertices[index];
        lower = b2Min(lower, v);
        upper = b2Max(upper, v);
    }
    aabb.lowerBound.Set( lower.x, lower.y );
    aabb.upperBound.Set( upper.x, upper.y );
}

/// Rotate an AABB.
inline void mRotateAABB( const b2AABB& aabb, const F32& angle, b2AABB& transformedAABB )
{
    // Fetch the AABB center.
    const b2Vec2 centerPosition = aabb.GetCenter();

    // Convert to an OOBB with the specified offset.
    b2Vec2 localOOBB[4];
    CoreMath::mAABBtoOOBB( aabb, localOOBB );
    localOOBB[0] -= centerPosition;
    localOOBB[1] -= centerPosition;
    localOOBB[2] -= centerPosition;
    localOOBB[3] -= centerPosition;

    // Rotate the OOBB.
    CoreMath::mCalculateOOBB( localOOBB, b2Transform( b2Vec2(0.0f, 0.0f), b2Rot(angle) ), localOOBB );

    // Convert back to an AABB with the inverse specified offset.
    CoreMath::mOOBBtoAABB( localOOBB, transformedAABB );
    transformedAABB.lowerBound += centerPosition;
    transformedAABB.upperBound += centerPosition;
}

/// Returns a point on the given line AB that is closest to 'point'.
Vector2 mGetClosestPointOnLine( Vector2 &a, Vector2 &b, Vector2 &point);

/// Calculate point in rectangle.
bool mPointInRectangle( const Vector2& point, const Vector2& rectMin, const Vector2& rectMax );

/// Calculate line/rectangle intersection.
bool mLineRectangleIntersect( const Vector2& startPoint, const Vector2& endPoint, const Vector2& rectMin, const Vector2& rectMax, F32* pTime = NULL );

/// Transform the incoming point 'v' by rotating it about the point (and by the amount) specified in the transform 'T'.
/// NOTE: b2Math has a method called b2Mul() that is similar, but doesn't take into account the offset in T. So I made this.
inline b2Vec2 mRotateAboutArbitraryPoint(const b2Transform& T, const b2Vec2& v)
{
	float32 x = (T.q.c * (v.x - T.p.x) - T.q.s * (v.y - T.p.y)) + T.p.x;
	float32 y = (T.q.s * (v.x - T.p.x) + T.q.c * (v.y - T.p.y)) + T.p.y;

	return b2Vec2(x, y);
}

} // Namespace CoreMath.

#endif // _CORE_UTILITY_H_
