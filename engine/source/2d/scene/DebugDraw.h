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

#ifndef _DEBUG_DRAW_H_
#define _DEBUG_DRAW_H_

#ifndef BOX2D_H
#include "Box2D/Box2D.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

//-----------------------------------------------------------------------------

class DebugDraw
{
public:
    DebugDraw() {}
    virtual ~DebugDraw() {}

    void DrawAABB( const b2AABB& aabb, const ColorF& color );
    void DrawOOBB( const b2Vec2* pOOBB, const ColorF& color );
    void DrawAsleep( const b2Vec2* pOOBB, const ColorF& color );
    void DrawCollisionShapes( const b2Transform& xf, b2Body* pBody );
    void DrawSortPoint( const b2Vec2& worldPosition, const b2Vec2& size, const b2Vec2& localSortPoint );

    void DrawJoints( b2World* pWorld );

    void DrawShape( b2Fixture* fixture, const b2Transform& xf, const ColorF& color );
    void DrawPolygon( const b2Vec2* vertices, int32 vertexCount, const ColorF& color);
    void DrawSolidPolygon( const b2Vec2* vertices, int32 vertexCount, const ColorF& color);
    void DrawCircle( const b2Vec2& center, float32 radius, const ColorF& color);
    void DrawSolidCircle( const b2Vec2& center, float32 radius, const b2Vec2& axis, const ColorF& color);
    void DrawSegment( const b2Vec2& p1, const b2Vec2& p2, const ColorF& color);
    void DrawTransform(const b2Transform& xf);
    void DrawPoint(const b2Vec2& p, float32 size, const ColorF& color);
};

#endif // _DEBUG_DRAW_H_
