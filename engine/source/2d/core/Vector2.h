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

#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _CORE_MATH_H_
#include "2d/core/CoreMath.h"
#endif

#ifndef BOX2D_H
#include "Box2D/Box2D.h"
#endif

///-----------------------------------------------------------------------------

DefineConsoleType( TypeVector2 )

///-----------------------------------------------------------------------------

struct Vector2 : b2Vec2
{
    /// Constructors.
    inline Vector2( void ) {}
    inline Vector2( const Vector2& copy ) : b2Vec2( copy.x, copy.y ) {}
    inline Vector2( const F32 x, const F32 y ) : b2Vec2(x, y) {}
    inline Vector2( const b2Vec2& vec2 ) : b2Vec2( vec2 ) {}
    inline Vector2( const Point2I& point ) : b2Vec2( F32(point.x), F32(point.y) ) {}
    inline Vector2( const Point2F& point ) : b2Vec2( point.x, point.y ) {}
    inline Vector2( const Point2D& point ) : b2Vec2( F32(point.x), F32(point.y) ) {}
    inline Vector2( const char* pString ) { setString( pString ); }

    /// Operators.
    inline Vector2& operator /= (const F32 s)                           { x /= s; y /= s; return *this; }
    inline Vector2& operator += (const Vector2& v)                      { x += v.x; y += v.y;   return *this; }
    inline Vector2& operator -= (const Vector2& v)                      { x -= v.x; y -= v.y;   return *this; }
    inline Vector2 operator / (F32 s) const                             { return Vector2(x/s, y/s); }
    inline Vector2 operator + (const Vector2 &v) const                  { return Vector2(x+v.x, y+v.y); }
    inline Vector2 operator - (const Vector2 &v) const                  { return Vector2(x-v.x, y-v.y); }
    friend Vector2 operator * (F32 s, const Vector2& v)                 { return Vector2(v.x*s, v.y*s); }
    friend Vector2 operator * (const Vector2& v, F32 s)                 { return Vector2(v.x*s, v.y*s); }
    friend Vector2 operator * (const Vector2& v1, Vector2& v2)          { return Vector2(v1.x*v2.x, v1.y*v2.y); }
    friend Vector2 operator * (const Vector2& v1, const Vector2& v2)    { return Vector2(v1.x*v2.x, v1.y*v2.y); }
    inline Vector2 operator - (void) const                              { return Vector2(-x, -y); }
    inline bool operator == (const Vector2 &v) const                    { return (v.x == x && v.y == y); }
    inline bool operator != (const Vector2 &v) const                    { return (v.x != x || v.y != y); }

    /// Operator 'Point2F' Support (Assignment/Conversion).
    inline Vector2 operator = (const Point2F &p)                        { x = p.x; y = p.y; return *this; }
    inline Vector2 operator = (const Point2I &p)                        { x = F32(p.x); y = F32(p.y); return *this; }
    inline operator Point2F ()                                          { return Point2F(x, y); }
    inline Point2F ToPoint2F( void ) const                              { return Point2F(x, y); }

    /// Operator 'b2Vec2' Support (Assignment/Conversions).
    inline Vector2 operator = (const b2Vec2 &p)                         { x = p.x; y = p.y; return *this; }

    /// Utility.
    inline void setAngle(const F32 radians)                             { x = mCos(radians); y = mSin(radians); }
    inline void setPolar(const F32 radians,F32 length)                  { x = mCos(radians)*length; y = mSin(radians)*length; }
    inline Vector2 getVecFromAng(Vector2 curPos, F32 radians, F32 length) { F32 o = mSin(radians)*length; F32 a = mCos(radians)*length; return curPos + Vector2(a, o); }
    inline void setString(const char* pString )
    {
        const U32 elementCount = Utility::mGetStringElementCount(pString);

        if ( elementCount == 0 )
        {
            SetZero();
            return;
        }

        if ( elementCount == 1 )
        {
            x = y = dAtof(Utility::mGetStringElement(pString,0));
            return;
        }

        x = dAtof(Utility::mGetStringElement(pString,0));
        y = dAtof(Utility::mGetStringElement(pString,1));
    }
    inline const Vector2& setZero()                                     { (*this) = getZero(); return *this; }
    inline const Vector2& setOne()                                      { (*this) = getOne(); return *this; }
    inline static const Vector2& getZero()                              { static const Vector2 v(0.0f, 0.0f); return v; }
    inline static const Vector2& getOne()                               { static const Vector2 v(1.0f, 1.0f); return v; }
    inline F32 getAngle(void) const                                     { return mAtan(x, y); }
    inline F32 getMinorAxis(void) const                                 { return mLessThan(x,y) ? x : y; }
    inline F32 getMajorAxis(void) const                                 { return mGreaterThan(x,y) ? x : y; }
    inline Vector2 getUnitDirection(void) const                         { Vector2 temp(*this); temp.Normalize(); return temp; }
    inline bool isNAN(void) const                                       { return IsValid(); }
    inline bool isEqualRange(const Vector2& v, const F32 epsilon) const { return mIsEqualRange(x,v.x,epsilon) && mIsEqualRange(y,v.y,epsilon); }
    inline bool isEqual(const Vector2& v) const                         { return mIsEqual(x,v.x) && mIsEqual(y,v.y); }
    inline bool notEqual(const Vector2& v) const                        { return !isEqual(v); }
    inline bool isXZero(void) const                                     { return mIsZero(x); }
    inline bool isYZero(void) const                                     { return mIsZero(y); }
    inline bool isZero(void) const                                      { return mIsZero(LengthSquared()); }
    inline bool notZero(void) const                                     { return !isZero(); }
    inline F32 Normalize(void)                                          { return b2Vec2::Normalize(); }
    inline F32 Normalize(const F32 s)                                   { const F32 length = Length(); if ( length > 0.0f ) m_point2F_normalize_f((F32*)this, s); return length; }
    inline Vector2& absolute(void)                                      { if (x < 0.0f) x = -x; if (y < 0.0f) y = -y; return *this; }
    inline Vector2& receiprocate(void)                                  { x = 1.0f/x; y = 1.0f/y; return *this; }
    inline Vector2 getReceiprocate(void) const                          { Vector2 temp = *this; temp.receiprocate();return temp; }
    inline Vector2& add(const Vector2& v)                               { x += v.x; y += v.y; return *this; }
    inline Vector2& sub(const Vector2& v)                               { x -= v.x; y -= v.y; return *this; }
    inline Vector2& mult(const Vector2& v)                              { x *= v.x; y *= v.y; return *this; }
    inline Vector2& div(const Vector2& v)                               { x /= v.x; y /= v.y; return *this; }
    inline Vector2& scale(F32 scale)                                    { x *= scale; y *= scale; return *this; }
    inline Vector2& scale(const Vector2& v)                             { x *= v.x; y *= v.y; return *this; }
    inline Vector2& rotate(F32 angle)                                   { F32 tempX = x; x = x * mCos(angle) - y * mSin(angle); y = tempX * mSin(angle) + y * mCos(angle); return *this; }
    inline Vector2& rotate(const Vector2& center, F32 angle)            { Vector2 temp = *this - center; temp.rotate(angle); *this = center + temp; return *this; }
    inline Vector2& perp(void)                                          { const F32 temp = x; x = -y; y = temp; return *this; }
    inline Vector2 getPerp(void) const                                  { return Vector2(*this).perp(); }
    inline F32 dot( const Vector2&v )                                   { return (x * v.x) + (y * v.y); }
    inline void lerp(const Vector2& v, const F32 time, Vector2& out)    { out.Set( x + (v.x-x)*time, y + (v.y-y)*time); }
    inline void swap( Vector2& v)                                       { mSwap( v.x, x ); mSwap( v.y, y ); }
    inline Vector2& clamp(const Vector2& min, const Vector2& max)       { x = (x < min.x)? min.x : (x > max.x)? max.x : x; y = (y < min.y)? min.y : (y > max.y)? max.y : y; return *this; }
    inline Vector2& clampZero(void)                                     { if (isXZero()) x = 0.0f; if (isYZero()) y = 0.0f; return *this; }
    inline Vector2& clampMin(const Vector2& min)                        { if (x < min.x) x = min.x; if (y < min.y) y = min.y;  return *this; }
    inline Vector2& clampMax(const Vector2& max)                        { if (x > max.x) x = max.x; if (y > max.y) y = max.y;  return *this; }
    inline void rand(const Vector2& min, const Vector2& max)            { x = CoreMath::mGetRandomF(min.x, max.x), y = CoreMath::mGetRandomF(min.y,max.y); }
    inline void round(const F32 epsilon = FLT_EPSILON)                  { x = mRound(x, epsilon); y = mRound(y, epsilon); }

    inline StringTableEntry stringThis(void) const                      { char buffer[32]; dSprintf(buffer, 32, "%g %g", x, y ); return StringTable->insert(buffer); }
    inline const char* scriptThis(void) const                           { char* pBuffer = Con::getReturnBuffer(32); dSprintf(pBuffer, 32, "%.5g %.5g", x, y ); return pBuffer; }
};

#endif // _VECTOR2_H_
