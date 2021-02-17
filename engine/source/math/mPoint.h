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

#ifndef _MPOINT_H_
#define _MPOINT_H_

#define POINT_EPSILON (1e-4) ///< Epsilon for point types.

//-------------------------------------- Note: because of a circular dependency,
//                                        the mMathFn.h header is below the point
//                                        class declarations... DMM
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

//------------------------------------------------------------------------------
/// 2D integer point
///
/// Uses S32 internally.
class Point2I
{
   //-------------------------------------- Public data
  public:
   S32 x;   ///< X position
   S32 y;   ///< Y position

   //-------------------------------------- Public interface
  public:
   Point2I();                               ///< Create an uninitialized point.
   Point2I(const Point2I&);                 ///< Copy constructor
   Point2I(const S32 in_x, const S32 in_y); ///< Create point from two co-ordinates.

   //-------------------------------------- Non-math mutators and misc functions
   void set(const S32 in_x, const S32 in_y);///< Set (x,y) position
   void setMin(const Point2I&);             ///< Store lesser co-ordinates from parameter in this point.
   void setMax(const Point2I&);             ///< Store greater co-ordinates from parameter in this point.

   //-------------------------------------- Math mutators
   void neg();                              ///< Invert sign of point's co-ordinates.
   void convolve(const Point2I&);           ///< Convolve this point by parameter.

   //-------------------------------------- Queries
   bool isZero() const;                     ///< Is this point at the origin? (No epsilon used)
   F32  len() const;                        ///< Get the length of the point

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point2I&) const;
   bool operator!=(const Point2I&) const;

   // Arithmetic w/ other points
   Point2I  operator+(const Point2I&) const;
   Point2I  operator-(const Point2I&) const;
   Point2I& operator+=(const Point2I&);
   Point2I& operator-=(const Point2I&);

   // Arithmetic w/ scalars
   Point2I  operator*(const S32) const;
   Point2I& operator*=(const S32);
   Point2I  operator/(const S32) const;
   Point2I& operator/=(const S32);

   // Unary operators
   Point2I operator-() const;

   //-------------------------------------- Public static constants
  public:
	const static Point2I One;
	const static Point2I Zero;
	const static Point2I Min;
	const static Point2I Max;
};

//------------------------------------------------------------------------------
/// 3D integer point
///
/// Uses S32 internally.
class Point3I
{
   //-------------------------------------- Public data
  public:
   S32 x;                                                   ///< X co-ordinate
   S32 y;                                                   ///< Y co-ordinate
   S32 z;                                                   ///< Z co-ordinate

   //-------------------------------------- Public interface
  public:
   Point3I();               ///< Create an uninitialized point.
   Point3I(const Point3I&); ///< Copy constructor.
   Point3I(const S32 in_x, const S32 in_y, const S32 in_z); ///< Create a point from co-ordinates.

   //-------------------------------------- Non-math mutators and misc functions
   void set(const S32 in_x, const S32 in_y, const S32 in_z);///< Set co-ordinates.
   void setMin(const Point3I&); ///< Store lesser co-ordinates in this point.
   void setMax(const Point3I&); ///< Store greater co-ordinates in this point.

   //-------------------------------------- Math mutators
   void neg();                      ///< Invert co-ordinate's signs.
   void convolve(const Point3I&);   ///< Convolve by paramater.

   //-------------------------------------- Queries
   bool isZero() const;             ///< Check for point at origin. (No epsilon.)
   F32  len() const;                ///< Get length.

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point3I&) const;
   bool operator!=(const Point3I&) const;

   // Arithmetic w/ other points
   Point3I  operator+(const Point3I&) const;
   Point3I  operator-(const Point3I&) const;
   Point3I& operator+=(const Point3I&);
   Point3I& operator-=(const Point3I&);

   // Arithmetic w/ scalars
   Point3I  operator*(const S32) const;
   Point3I& operator*=(const S32);
   Point3I  operator/(const S32) const;
   Point3I& operator/=(const S32);

   // Unary operators
   Point3I operator-() const;

   //-------------------------------------- Public static constants
public:
   const static Point3I One;
   const static Point3I Zero;
};



//------------------------------------------------------------------------------
/// 2D floating-point point.
class Point2F
{
   //-------------------------------------- Public data
  public:
   F32 x;
   F32 y;

  public:
   Point2F();                           ///< Create uninitialized point.
   Point2F(const Point2F&);             ///< Copy constructor
   Point2F(const F32 _x, const F32 _y); ///< Create point from co-ordinates.

   //-------------------------------------- Non-math mutators and misc functions
  public:
   void set(const F32 _x, const F32 _y);///< Set point's co-ordinates.

   void setMin(const Point2F&);         ///< Store lesser co-ordinates.
   void setMax(const Point2F&);         ///< Store greater co-ordinates.

   /// Interpolate from a to b, based on c.
   ///
   /// @param   a   Starting point.
   /// @param   b   Ending point.
   /// @param   c   Interpolation factor (0.0 .. 1.0).
   void interpolate(const Point2F& a, const Point2F& b, const F32 c);

   operator F32*() { return (&x); }
   operator F32*() const { return (F32*)(&x); }

   //-------------------------------------- Queries
  public:
   bool  isZero() const;        ///< Check for zero coordinates. (No epsilon.)
   F32 len()    const;          ///< Get length.
   F32 lenSquared() const;      ///< Get squared length (one sqrt less than len()).

   //-------------------------------------- Mathematical mutators
  public:
   void neg();                              ///< Invert signs of co-ordinates.
   void normalize();                        ///< Normalize vector.
   void normalize(F32 val);                 ///< Normalize, scaling by val.
   void convolve(const Point2F&);           ///< Convolve by parameter.
   void convolveInverse(const Point2F&);    ///< Inversely convolute by parameter. (ie, divide)

   //Luma: Utility function to round a Point2F
   void round() {x = (F32)((S32)(x + 0.5f)); y = (F32)((S32)(y + 0.5f));}

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point2F&) const;
   bool operator!=(const Point2F&) const;

   // Arithmetic w/ other points
   Point2F  operator+(const Point2F&) const;
   Point2F  operator-(const Point2F&) const;
   Point2F& operator+=(const Point2F&);
   Point2F& operator-=(const Point2F&);

   // Arithmetic w/ scalars
   Point2F  operator*(const F32) const;
   Point2F  operator/(const F32) const;
   Point2F& operator*=(const F32);
   Point2F& operator*=(const Point2F &_vec);
   Point2F& operator/=(const F32);

   // Unary operators
   Point2F operator-() const;

   //-------------------------------------- Public static constants
  public:
	const static Point2F One;
	const static Point2F Zero;
	const static Point2F Min;
	const static Point2F Max;

};


//------------------------------------------------------------------------------
/// 2D high-precision point.
///
/// Uses F64 internally.
class Point2D
{
   //-------------------------------------- Public data
  public:
   F64 x;   ///< X co-ordinate.
   F64 y;   ///< Y co-ordinate.

  public:
   Point2D();                           ///< Create uninitialized point.
   Point2D(const Point2D&);             ///< Copy constructor
   Point2D(const F64 _x, const F64 _y); ///< Create point from coordinates.

   //-------------------------------------- Non-math mutators and misc functions
  public:
   void set(const F64 _x, const F64 _y);///< Set point's coordinates.

   void setMin(const Point2D&);         ///< Store lesser co-ordinates.
   void setMax(const Point2D&);         ///< Store greater co-ordinates.

   /// Interpolate from a to b, based on c.
   ///
   /// @param   a   Starting point.
   /// @param   b   Ending point.
   /// @param   c   Interpolation factor (0.0 .. 1.0).
   void interpolate(const Point2D &a, const Point2D &b, const F64 c);

   operator F64*() { return (&x); }
   operator F64*() const { return (F64*)(&x); }

   //-------------------------------------- Queries
  public:
   bool  isZero() const;
   F64 len()    const;
   F64 lenSquared() const;

   //-------------------------------------- Mathematical mutators
  public:
   void neg();
   void normalize();
   void normalize(F64 val);
   void convolve(const Point2D&);
   void convolveInverse(const Point2D&);

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point2D&) const;
   bool operator!=(const Point2D&) const;

   // Arithmetic w/ other points
   Point2D  operator+(const Point2D&) const;
   Point2D  operator-(const Point2D&) const;
   Point2D& operator+=(const Point2D&);
   Point2D& operator-=(const Point2D&);

   // Arithmetic w/ scalars
   Point2D  operator*(const F64) const;
   Point2D  operator/(const F64) const;
   Point2D& operator*=(const F64);
   Point2D& operator/=(const F64);

   // Unary operators
   Point2D operator-() const;

//-------------------------------------- Public static constants
  public:
    const static Point2D One;
    const static Point2D Zero;
};


//------------------------------------------------------------------------------
class Point3F
{
   //-------------------------------------- Public data
  public:
   F32 x;
   F32 y;
   F32 z;

  public:
   Point3F();
   Point3F(const Point3F&);
   Point3F(const F32 _x, const F32 _y, const F32 _z);

   //-------------------------------------- Non-math mutators and misc functions
  public:
   void set(const F32 _x, const F32 _y, const F32 _z);
   void set(const Point3F&);

   void setMin(const Point3F&);
   void setMax(const Point3F&);

   void interpolate(const Point3F&, const Point3F&, const F32);
   void zero();

   F32& operator [](int i) { /*ASSERT HERE??*/ return( *((&x)+i) ); }
   operator F32*() { return (&x); }
   operator F32*() const { return (F32*)(&x); }

   //-------------------------------------- Queries
  public:
   bool  isZero() const;
   F32   len()    const;
   F32   lenSquared() const;
   F32   magnitudeSafe();
   bool  equal( Point3F &compare );

   //-------------------------------------- Mathematical mutators
  public:
   void neg();
   void normalize();
   void normalizeSafe();
   void normalize(F32 val);
   void convolve(const Point3F&);
   void convolveInverse(const Point3F&);

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point3F&) const;
   bool operator!=(const Point3F&) const;

   // Arithmetic w/ other points
   Point3F  operator+(const Point3F&) const;
   Point3F  operator-(const Point3F&) const;
   Point3F& operator+=(const Point3F&);
   Point3F& operator-=(const Point3F&);

   // Arithmetic w/ scalars
   Point3F  operator*(const F32) const;
   Point3F  operator/(const F32) const;
   Point3F& operator*=(const F32);
   Point3F& operator/=(const F32);

   Point3F  operator*(const Point3F&) const;
   Point3F& operator*=(const Point3F&);

   // Unary operators
   Point3F operator-() const;

   //-------------------------------------- Public static constants
public:
   const static Point3F One;
   const static Point3F Zero;
   const static Point3F Max;
   const static Point3F Min;
   const static Point3F UnitX;
   const static Point3F UnitY;
   const static Point3F UnitZ;
};


typedef Point3F VectorF;
typedef Point3F EulerF;


//------------------------------------------------------------------------------
class Point3D
{
   //-------------------------------------- Public data
  public:
   F64 x;
   F64 y;
   F64 z;

  public:
   Point3D();
   Point3D(const Point3D&);
   Point3D(const F64 _x, const F64 _y, const F64 _z);

   //-------------------------------------- Non-math mutators and misc functions
  public:
   void set(const F64 _x, const F64 _y, const F64 _z);

   void setMin(const Point3D&);
   void setMax(const Point3D&);

   void interpolate(const Point3D&, const Point3D&, const F64);

   operator F64*() { return (&x); }
   operator F64*() const { return (F64*)(&x); }

   //-------------------------------------- Queries
  public:
   bool  isZero() const;
   F64 len()    const;
   F64 lenSquared() const;

   //-------------------------------------- Mathematical mutators
  public:
   void neg();
   void normalize();
   void normalize(F64 val);
   void convolve(const Point3D&);
   void convolveInverse(const Point3D&);

   //-------------------------------------- Overloaded operators
  public:
   // Comparison operators
   bool operator==(const Point3D&) const;
   bool operator!=(const Point3D&) const;

   // Arithmetic w/ other points
   Point3D  operator+(const Point3D&) const;
   Point3D  operator-(const Point3D&) const;
   Point3D& operator+=(const Point3D&);
   Point3D& operator-=(const Point3D&);

   // Arithmetic w/ scalars
   Point3D  operator*(const F64) const;
   Point3D  operator/(const F64) const;
   Point3D& operator*=(const F64);
   Point3D& operator/=(const F64);

   // Unary operators
   Point3D operator-() const;

   //-------------------------------------- Public static constants
public:
   const static Point3D One;
   const static Point3D Zero;
};



//------------------------------------------------------------------------------
/// 4D floating-point point.
///
/// Uses F32 internally.
///
/// Useful for representing quaternions and other 4d beasties.
class Point4F
{
   //-------------------------------------- Public data
  public:
   F32 x;   ///< X co-ordinate.
   F32 y;   ///< Y co-ordinate.
   F32 z;   ///< Z co-ordinate.
   F32 w;   ///< W co-ordinate.

  public:
   Point4F();               ///< Create an unitialized point.
   Point4F(const Point4F&); ///< Copy constructor.
   /// Create point from coordinates.
   Point4F(const F32 _x, const F32 _y, const F32 _z, const F32 _w);

   /// Set point's coordinates.
   void set(const F32 _x, const F32 _y, const F32 _z, const F32 _w);

   /// Interpolate from _pt1 to _pt2, based on _factor.
   ///
   /// @param   _pt1    Starting point.
   /// @param   _pt2    Ending point.
   /// @param   _factor Interpolation factor (0.0 .. 1.0).
   void interpolate(const Point4F& _pt1, const Point4F& _pt2, const F32 _factor);

   operator F32*() { return (&x); }
   operator F32*() const { return (F32*)(&x); }

   	//-------------------------------------- Public static constants
  public:
	const static Point4F One;
	const static Point4F Zero;
};


typedef Point4F Vector4F;   ///< Points can be vectors!


#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

//------------------------------------------------------------------------------
//-------------------------------------- Inline functions inclusions


//------------------------------------------------------------------------------
//-------------------------------------- Point2I
//
inline Point2I::Point2I()
{
   //
}


inline Point2I::Point2I(const Point2I& _copy)
 : x(_copy.x), y(_copy.y)
{
   //
}


inline Point2I::Point2I(const S32 _x, const S32 _y)
 : x(_x), y(_y)
{
   //
}


inline void Point2I::set(const S32 _x, const S32 _y)
{
   x = _x;
   y = _y;
}


inline void Point2I::setMin(const Point2I& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
}


inline void Point2I::setMax(const Point2I& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
}


inline void Point2I::neg()
{
   x = -x;
   y = -y;
}

inline void Point2I::convolve(const Point2I& c)
{
   x *= c.x;
   y *= c.y;
}

inline bool Point2I::isZero() const
{
   return ((x == 0) && (y == 0));
}


inline F32 Point2I::len() const
{
   return mSqrt(F32(x*x + y*y));
}

inline bool Point2I::operator==(const Point2I& _test) const
{
   return ((x == _test.x) && (y == _test.y));
}


inline bool Point2I::operator!=(const Point2I& _test) const
{
   return (operator==(_test) == false);
}


inline Point2I Point2I::operator+(const Point2I& _add) const
{
   return Point2I(x + _add.x, y + _add.y);
}


inline Point2I Point2I::operator-(const Point2I& _rSub) const
{
   return Point2I(x - _rSub.x, y - _rSub.y);
}


inline Point2I& Point2I::operator+=(const Point2I& _add)
{
   x += _add.x;
   y += _add.y;

   return *this;
}


inline Point2I& Point2I::operator-=(const Point2I& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;

   return *this;
}


inline Point2I Point2I::operator-() const
{
   return Point2I(-x, -y);
}


inline Point2I Point2I::operator*(const S32 mul) const
{
   return Point2I(x * mul, y * mul);
}

inline Point2I Point2I::operator/(const S32 div) const
{
   AssertFatal(div != 0, "Error, div by zero attempted");
   return Point2I(x/div, y/div);
}


inline Point2I& Point2I::operator*=(const S32 mul)
{
   x *= mul;
   y *= mul;

   return *this;
}


inline Point2I& Point2I::operator/=(const S32 div)
{
   AssertFatal(div != 0, "Error, div by zero attempted");

   x /= div;
   y /= div;

   return *this;
}




//------------------------------------------------------------------------------
//-------------------------------------- Point3I
//
inline Point3I::Point3I()
{
   //
}


inline Point3I::Point3I(const Point3I& _copy)
 : x(_copy.x), y(_copy.y), z(_copy.z)
{
   //
}


inline Point3I::Point3I(const S32 _x, const S32 _y, const S32 _z)
 : x(_x), y(_y), z(_z)
{
   //
}


inline void Point3I::set(const S32 _x, const S32 _y, const S32 _z)
{
   x = _x;
   y = _y;
   z = _z;
}


inline void Point3I::setMin(const Point3I& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
   z = (_test.z < z) ? _test.z : z;
}


inline void Point3I::setMax(const Point3I& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
   z = (_test.z > z) ? _test.z : z;
}


inline void Point3I::neg()
{
   x = -x;
   y = -y;
   z = -z;
}

inline F32 Point3I::len() const
{
   return mSqrt(F32(x*x + y*y + z*z));
}

inline void Point3I::convolve(const Point3I& c)
{
   x *= c.x;
   y *= c.y;
   z *= c.z;
}

inline bool Point3I::isZero() const
{
   return ((x == 0) && (y == 0) && (z == 0));
}


inline bool Point3I::operator==(const Point3I& _test) const
{
   return ((x == _test.x) && (y == _test.y) && (z == _test.z));
}


inline bool Point3I::operator!=(const Point3I& _test) const
{
   return (operator==(_test) == false);
}


inline Point3I Point3I::operator+(const Point3I& _add) const
{
   return Point3I(x + _add.x, y + _add.y, z + _add.z);
}


inline Point3I Point3I::operator-(const Point3I& _rSub) const
{
   return Point3I(x - _rSub.x, y - _rSub.y, z - _rSub.z);
}


inline Point3I& Point3I::operator+=(const Point3I& _add)
{
   x += _add.x;
   y += _add.y;
   z += _add.z;

   return *this;
}


inline Point3I& Point3I::operator-=(const Point3I& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;
   z -= _rSub.z;

   return *this;
}


inline Point3I Point3I::operator-() const
{
   return Point3I(-x, -y, -z);
}


inline Point3I Point3I::operator*(const S32 mul) const
{
   return Point3I(x * mul, y * mul, z * mul);
}


inline Point3I Point3I::operator/(const S32 div) const
{
   AssertFatal(div != 0, "Error, div by zero attempted");
   return Point3I(x/div, y/div, z/div);
}


inline Point3I& Point3I::operator*=(const S32 mul)
{
   x *= mul;
   y *= mul;
   z *= mul;

   return *this;
}


inline Point3I& Point3I::operator/=(const S32 div)
{
   AssertFatal(div != 0, "Error, div by zero attempted");

   x /= div;
   y /= div;
   z /= div;

   return *this;
}






//------------------------------------------------------------------------------
//-------------------------------------- Point2F
//
inline Point2F::Point2F()
{
   //
}


inline Point2F::Point2F(const Point2F& _copy)
 : x(_copy.x), y(_copy.y)
{
   //
}


inline Point2F::Point2F(const F32 _x, const F32 _y)
 : x(_x), y(_y)
{
}


inline void Point2F::set(const F32 _x, const F32 _y)
{
   x = _x;
   y = _y;
}


inline void Point2F::setMin(const Point2F& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
}


inline void Point2F::setMax(const Point2F& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
}


inline void Point2F::interpolate(const Point2F& _rFrom, const Point2F& _to, const F32 _factor)
{
   AssertFatal(_factor >= 0.0f && _factor <= 1.0f, "Out of bound interpolation factor");
   x = (_rFrom.x * (1.0f - _factor)) + (_to.x * _factor);
   y = (_rFrom.y * (1.0f - _factor)) + (_to.y * _factor);
}


inline bool Point2F::isZero() const
{
   return (x == 0.0f) && (y == 0.0f);
}


inline F32 Point2F::lenSquared() const
{
   return (x * x) + (y * y);
}


inline void Point2F::neg()
{
   x = -x;
   y = -y;
}

inline void Point2F::convolve(const Point2F& c)
{
   x *= c.x;
   y *= c.y;
}


inline void Point2F::convolveInverse(const Point2F& c)
{
   x /= c.x;
   y /= c.y;
}


inline bool Point2F::operator==(const Point2F& _test) const
{
   return (x == _test.x) && (y == _test.y);
}


inline bool Point2F::operator!=(const Point2F& _test) const
{
   return operator==(_test) == false;
}


inline Point2F Point2F::operator+(const Point2F& _add) const
{
   return Point2F(x + _add.x, y + _add.y);
}


inline Point2F Point2F::operator-(const Point2F& _rSub) const
{
   return Point2F(x - _rSub.x, y - _rSub.y);
}


inline Point2F& Point2F::operator+=(const Point2F& _add)
{
   x += _add.x;
   y += _add.y;

   return *this;
}


inline Point2F& Point2F::operator-=(const Point2F& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;

   return *this;
}


inline Point2F Point2F::operator*(const F32 _mul) const
{
   return Point2F(x * _mul, y * _mul);
}


inline Point2F Point2F::operator/(const F32 _div) const
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F32 inv = 1.0f / _div;

   return Point2F(x * inv, y * inv);
}


inline Point2F& Point2F::operator*=(const F32 _mul)
{
   x *= _mul;
   y *= _mul;

   return *this;
}

inline Point2F& Point2F::operator*=(const Point2F &_vec)
{
   x *= _vec.x;
   y *= _vec.y;

   return *this;
}


inline Point2F& Point2F::operator/=(const F32 _div)
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F32 inv = 1.0f / _div;

   x *= inv;
   y *= inv;

   return *this;
}


inline Point2F Point2F::operator-() const
{
   return Point2F(-x, -y);
}

inline F32 Point2F::len() const
{
   return mSqrt(x*x + y*y);
}

inline void Point2F::normalize()
{
   m_point2F_normalize(*this);
}

inline void Point2F::normalize(F32 val)
{
   m_point2F_normalize_f(*this, val);
}


//------------------------------------------------------------------------------
//-------------------------------------- Point2D
//
inline Point2D::Point2D()
{
   //
}


inline Point2D::Point2D(const Point2D& _copy)
 : x(_copy.x), y(_copy.y)
{
   //
}


inline Point2D::Point2D(const F64 _x, const F64 _y)
 : x(_x), y(_y)
{
}


inline void Point2D::set(const F64 _x, const F64 _y)
{
   x = _x;
   y = _y;
}


inline void Point2D::setMin(const Point2D& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
}


inline void Point2D::setMax(const Point2D& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
}


inline void Point2D::interpolate(const Point2D& _rFrom, const Point2D& _to, const F64 _factor)
{
   AssertFatal(_factor >= 0.0f && _factor <= 1.0f, "Out of bound interpolation factor");
   x = (_rFrom.x * (1.0f - _factor)) + (_to.x * _factor);
   y = (_rFrom.y * (1.0f - _factor)) + (_to.y * _factor);
}


inline bool Point2D::isZero() const
{
   return (x == 0.0f) && (y == 0.0f);
}


inline F64 Point2D::lenSquared() const
{
   return (x * x) + (y * y);
}


inline void Point2D::neg()
{
   x = -x;
   y = -y;
}

inline void Point2D::convolve(const Point2D& c)
{
   x *= c.x;
   y *= c.y;
}

inline void Point2D::convolveInverse(const Point2D& c)
{
   x /= c.x;
   y /= c.y;
}

inline bool Point2D::operator==(const Point2D& _test) const
{
   return (x == _test.x) && (y == _test.y);
}


inline bool Point2D::operator!=(const Point2D& _test) const
{
   return operator==(_test) == false;
}


inline Point2D Point2D::operator+(const Point2D& _add) const
{
   return Point2D(x + _add.x, y + _add.y);
}


inline Point2D Point2D::operator-(const Point2D& _rSub) const
{
   return Point2D(x - _rSub.x, y - _rSub.y);
}


inline Point2D& Point2D::operator+=(const Point2D& _add)
{
   x += _add.x;
   y += _add.y;

   return *this;
}


inline Point2D& Point2D::operator-=(const Point2D& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;

   return *this;
}


inline Point2D Point2D::operator*(const F64 _mul) const
{
   return Point2D(x * _mul, y * _mul);
}


inline Point2D Point2D::operator/(const F64 _div) const
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F64 inv = 1.0f / _div;

   return Point2D(x * inv, y * inv);
}


inline Point2D& Point2D::operator*=(const F64 _mul)
{
   x *= _mul;
   y *= _mul;

   return *this;
}


inline Point2D& Point2D::operator/=(const F64 _div)
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F64 inv = 1.0f / _div;

   x *= inv;
   y *= inv;

   return *this;
}


inline Point2D Point2D::operator-() const
{
   return Point2D(-x, -y);
}

inline F64 Point2D::len() const
{
   return mSqrtD(x*x + y*y);
}

inline void Point2D::normalize()
{
   m_point2D_normalize(*this);
}

inline void Point2D::normalize(F64 val)
{
   m_point2D_normalize_f(*this, val);
}


//------------------------------------------------------------------------------
//-------------------------------------- Point3F
//
inline Point3F::Point3F()
#if defined(TORQUE_OS_LINUX)
 : x(0.f), y(0.f), z(0.f)
#endif
{
// Uninitialized points are definitely a problem.
// Enable the following code to see how often they crop up.
#ifdef DEBUG_MATH
   *(U32 *)&x = 0x7FFFFFFA;
   *(U32 *)&y = 0x7FFFFFFB;
   *(U32 *)&z = 0x7FFFFFFC;
#endif
}


inline Point3F::Point3F(const Point3F& _copy)
 : x(_copy.x), y(_copy.y), z(_copy.z)
{
   //
}


inline Point3F::Point3F(const F32 _x, const F32 _y, const F32 _z)
 : x(_x), y(_y), z(_z)
{
   //
}


inline void Point3F::set(const F32 _x, const F32 _y, const F32 _z)
{
   x = _x;
   y = _y;
   z = _z;
}

inline void Point3F::set(const Point3F& copy)
{
   x = copy.x;
   y = copy.y;
   z = copy.z;
}

inline void Point3F::setMin(const Point3F& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
   z = (_test.z < z) ? _test.z : z;
}


inline void Point3F::setMax(const Point3F& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
   z = (_test.z > z) ? _test.z : z;
}


inline void Point3F::interpolate(const Point3F& _from, const Point3F& _to, const F32 _factor)
{
   AssertFatal(_factor >= 0.0f && _factor <= 1.0f, "Out of bound interpolation factor");
   m_point3F_interpolate( _from, _to, _factor, *this);
}

inline void Point3F::zero()
{
   x = y = z = 0.0f;
}

inline bool Point3F::isZero() const
{
   return ((x*x) <= POINT_EPSILON) && ((y*y) <= POINT_EPSILON) && ((z*z) <= POINT_EPSILON );
}

inline bool Point3F::equal( Point3F &compare )
{
   return( ( mFabs( x - compare.x ) < POINT_EPSILON ) &&
           ( mFabs( y - compare.y ) < POINT_EPSILON ) &&
           ( mFabs( z - compare.z ) < POINT_EPSILON ) );
}

inline void Point3F::neg()
{
   x = -x;
   y = -y;
   z = -z;
}

inline void Point3F::convolve(const Point3F& c)
{
   x *= c.x;
   y *= c.y;
   z *= c.z;
}

inline void Point3F::convolveInverse(const Point3F& c)
{
   x /= c.x;
   y /= c.y;
   z /= c.z;
}

inline F32 Point3F::lenSquared() const
{
   return (x * x) + (y * y) + (z * z);
}


inline F32 Point3F::len() const
{
   return mSqrt(x*x + y*y + z*z);
}


inline void Point3F::normalize()
{
   m_point3F_normalize(*this);
}

inline F32 Point3F::magnitudeSafe()
{
   if( isZero() )
   {
      return 0.0f;
   }
   else
   {
      return len();
   }
}

inline void Point3F::normalizeSafe()
{
   F32 vmag = magnitudeSafe();

   if( vmag > POINT_EPSILON )
   {
      *this *= F32(1.0 / vmag);
   }
}


inline void Point3F::normalize(F32 val)
{
   m_point3F_normalize_f(*this, val);
}

inline bool Point3F::operator==(const Point3F& _test) const
{
   return (x == _test.x) && (y == _test.y) && (z == _test.z);
}


inline bool Point3F::operator!=(const Point3F& _test) const
{
   return operator==(_test) == false;
}


inline Point3F Point3F::operator+(const Point3F& _add) const
{
   return Point3F(x + _add.x, y + _add.y,  z + _add.z);
}


inline Point3F Point3F::operator-(const Point3F& _rSub) const
{
   return Point3F(x - _rSub.x, y - _rSub.y, z - _rSub.z);
}


inline Point3F& Point3F::operator+=(const Point3F& _add)
{
   x += _add.x;
   y += _add.y;
   z += _add.z;

   return *this;
}


inline Point3F& Point3F::operator-=(const Point3F& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;
   z -= _rSub.z;

   return *this;
}


inline Point3F Point3F::operator*(const F32 _mul) const
{
   return Point3F(x * _mul, y * _mul, z * _mul);
}

inline Point3F Point3F::operator*(const Point3F &_vec) const
{
   return Point3F(x * _vec.x, y * _vec.y, z * _vec.z);
}

inline Point3F Point3F::operator/(const F32 _div) const
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F32 inv = 1.0f / _div;

   return Point3F(x * inv, y * inv, z * inv);
}


inline Point3F& Point3F::operator*=(const F32 _mul)
{
   x *= _mul;
   y *= _mul;
   z *= _mul;

   return *this;
}


inline Point3F& Point3F::operator*=(const Point3F &_vec)
{
   x *= _vec.x;
   y *= _vec.y;
   z *= _vec.z;
   return *this;
}


inline Point3F& Point3F::operator/=(const F32 _div)
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F32 inv = 1.0f / _div;
   x *= inv;
   y *= inv;
   z *= inv;

   return *this;
}


inline Point3F Point3F::operator-() const
{
   return Point3F(-x, -y, -z);
}


//------------------------------------------------------------------------------
//-------------------------------------- Point3D
//
inline Point3D::Point3D()
{
   //
}


inline Point3D::Point3D(const Point3D& _copy)
 : x(_copy.x), y(_copy.y), z(_copy.z)
{
   //
}


inline Point3D::Point3D(const F64 _x, const F64 _y, const F64 _z)
 : x(_x), y(_y), z(_z)
{
   //
}


inline void Point3D::set(const F64 _x, const F64 _y, const F64 _z)
{
   x = _x;
   y = _y;
   z = _z;
}


inline void Point3D::setMin(const Point3D& _test)
{
   x = (_test.x < x) ? _test.x : x;
   y = (_test.y < y) ? _test.y : y;
   z = (_test.z < z) ? _test.z : z;
}


inline void Point3D::setMax(const Point3D& _test)
{
   x = (_test.x > x) ? _test.x : x;
   y = (_test.y > y) ? _test.y : y;
   z = (_test.z > z) ? _test.z : z;
}


inline void Point3D::interpolate(const Point3D& _from, const Point3D& _to, const F64 _factor)
{
   AssertFatal(_factor >= 0.0f && _factor <= 1.0f, "Out of bound interpolation factor");
   m_point3D_interpolate( _from, _to, _factor, *this);
}


inline bool Point3D::isZero() const
{
   return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
}


inline void Point3D::neg()
{
   x = -x;
   y = -y;
   z = -z;
}

inline void Point3D::convolve(const Point3D& c)
{
   x *= c.x;
   y *= c.y;
   z *= c.z;
}

inline void Point3D::convolveInverse(const Point3D& c)
{
   x /= c.x;
   y /= c.y;
   z /= c.z;
}

inline F64 Point3D::lenSquared() const
{
   return (x * x) + (y * y) + (z * z);
}


inline F64 Point3D::len() const
{
   return mSqrtD(x*x + y*y + z*z);
}


inline void Point3D::normalize()
{
   m_point3D_normalize(*this);
}

inline void Point3D::normalize(F64 val)
{
   m_point3D_normalize_f(*this, val);
}

inline bool Point3D::operator==(const Point3D& _test) const
{
   return (x == _test.x) && (y == _test.y) && (z == _test.z);
}


inline bool Point3D::operator!=(const Point3D& _test) const
{
   return operator==(_test) == false;
}


inline Point3D Point3D::operator+(const Point3D& _add) const
{
   return Point3D(x + _add.x, y + _add.y,  z + _add.z);
}


inline Point3D Point3D::operator-(const Point3D& _rSub) const
{
   return Point3D(x - _rSub.x, y - _rSub.y, z - _rSub.z);
}


inline Point3D& Point3D::operator+=(const Point3D& _add)
{
   x += _add.x;
   y += _add.y;
   z += _add.z;

   return *this;
}


inline Point3D& Point3D::operator-=(const Point3D& _rSub)
{
   x -= _rSub.x;
   y -= _rSub.y;
   z -= _rSub.z;

   return *this;
}


inline Point3D Point3D::operator*(const F64 _mul) const
{
   return Point3D(x * _mul, y * _mul, z * _mul);
}


inline Point3D Point3D::operator/(const F64 _div) const
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F64 inv = 1.0f / _div;

   return Point3D(x * inv, y * inv, z * inv);
}


inline Point3D& Point3D::operator*=(const F64 _mul)
{
   x *= _mul;
   y *= _mul;
   z *= _mul;

   return *this;
}


inline Point3D& Point3D::operator/=(const F64 _div)
{
   AssertFatal(_div != 0.0f, "Error, div by zero attempted");

   F64 inv = 1.0f / _div;
   x *= inv;
   y *= inv;
   z *= inv;

   return *this;
}


inline Point3D Point3D::operator-() const
{
   return Point3D(-x, -y, -z);
}

//------------------------------------------------------------------------------
//-------------------------------------- Point4F
//
inline Point4F::Point4F()
{
   //
}


inline Point4F::Point4F(const Point4F& _copy)
 : x(_copy.x), y(_copy.y), z(_copy.z), w(_copy.w)
{
   //
}


inline Point4F::Point4F(const F32 _x, const F32 _y, const F32 _z, const F32 _w)
 : x(_x), y(_y), z(_z), w(_w)
{
   //
}


inline void Point4F::set(const F32 _x, const F32 _y, const F32 _z, const F32 _w)
{
   x = _x;
   y = _y;
   z = _z;
   w = _w;
}


inline void Point4F::interpolate(const Point4F& _from, const Point4F& _to, const F32 _factor)
{
   x = (_from.x * (1.0f - _factor)) + (_to.x * _factor);
   y = (_from.y * (1.0f - _factor)) + (_to.y * _factor);
   z = (_from.z * (1.0f - _factor)) + (_to.z * _factor);
   w = (_from.w * (1.0f - _factor)) + (_to.w * _factor);
}

//--------------------------------------------------------------------------
//-------------------------------------- NON-MEMBER Operators
//
inline Point2I operator*(const S32 mul, const Point2I& multiplicand)
{
   return multiplicand * mul;
}

inline Point3I operator*(const S32 mul, const Point3I& multiplicand)
{
   return multiplicand * mul;
}

inline Point2F operator*(const F32 mul, const Point2F& multiplicand)
{
   return multiplicand * mul;
}

inline Point3F operator*(const F32 mul, const Point3F& multiplicand)
{
   return multiplicand * mul;
}

inline Point2D operator*(const F64 mul, const Point2D& multiplicand)
{
   return multiplicand * mul;
}

inline Point3D operator*(const F64 mul, const Point3D& multiplicand)
{
   return multiplicand * mul;
}

#endif // _POINT_H_
