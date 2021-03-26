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

#ifndef _MMATHFN_H_
#define _MMATHFN_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _MCONSTANTS_H_
#include "math/mConstants.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#include <math.h>

// Remove a couple of annoying macros, if they are present (In VC 6, they are.)
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

class MatrixF;
class PlaneF;

extern void MathConsoleInit();

//--------------------------------------
// Installable Library Prototypes
extern S32  (*m_mulDivS32)(S32 a, S32 b, S32 c);
extern U32  (*m_mulDivU32)(S32 a, S32 b, U32 c);

extern F32  (*m_catmullrom)(F32 t, F32 p0, F32 p1, F32 p2, F32 p3);

extern void (*m_point2F_normalize)(F32 *p);
extern void (*m_point2F_normalize_f)(F32 *p, F32 len);
extern void (*m_point2D_normalize)(F64 *p);
extern void (*m_point2D_normalize_f)(F64 *p, F64 len);
extern void (*m_point3F_normalize)(F32 *p);
extern void (*m_point3F_normalize_f)(F32 *p, F32 len);
extern void (*m_point3F_interpolate)(const F32 *from, const F32 *to, F32 factor, F32 *result);

extern void (*m_point3D_normalize)(F64 *p);
extern void (*m_point3D_normalize_f)(F64 *p, F64 len);
extern void (*m_point3D_interpolate)(const F64 *from, const F64 *to, F64 factor, F64 *result);

extern void (*m_point3F_bulk_dot)(const F32* refVector,
                                  const F32* dotPoints,
                                  const U32  numPoints,
                                  const U32  pointStride,
                                  F32*       output);
extern void (*m_point3F_bulk_dot_indexed)(const F32* refVector,
                                          const F32* dotPoints,
                                          const U32  numPoints,
                                          const U32  pointStride,
                                          const U32* pointIndices,
                                          F32*       output);

extern void (*m_quatF_set_matF)( F32 x, F32 y, F32 z, F32 w, F32* m );

extern void (*m_matF_set_euler)(const F32 *e, F32 *result);
extern void (*m_matF_set_euler_point)(const F32 *e, const F32 *p, F32 *result);
extern void (*m_matF_identity)(F32 *m);
extern void (*m_matF_inverse)(F32 *m);
extern void (*m_matF_affineInverse)(F32 *m);
extern void (*m_matF_transpose)(F32 *m);
extern void (*m_matF_scale)(F32 *m,const F32* p);
extern void (*m_matF_normalize)(F32 *m);
extern F32  (*m_matF_determinant)(const F32 *m);
extern void (*m_matF_x_matF)(const F32 *a, const F32 *b, F32 *mresult);
extern void(*m_matF_x_matF_aligned)(const F32 *a, const F32 *b, F32 *mresult);
// extern void (*m_matF_x_point3F)(const F32 *m, const F32 *p, F32 *presult);
// extern void (*m_matF_x_vectorF)(const F32 *m, const F32 *v, F32 *vresult);
extern void (*m_matF_x_point4F)(const F32 *m, const F32 *p, F32 *presult);
extern void (*m_matF_x_scale_x_planeF)(const F32 *m, const F32* s, const F32 *p, F32 *presult);
extern void (*m_matF_x_box3F)(const F32 *m, F32 *min, F32 *max);

// Note that x must point to at least 4 values for quartics, and 3 for cubics
extern U32 (*mSolveQuadratic)(F32 a, F32 b, F32 c, F32* x);
extern U32 (*mSolveCubic)(F32 a, F32 b, F32 c, F32 d, F32* x);
extern U32 (*mSolveQuartic)(F32 a, F32 b, F32 c, F32 d, F32 e, F32* x);

extern S32 mRandI(S32 i1, S32 i2); // random # from i1 to i2 inclusive
extern F32 mRandF(F32 f1, F32 f2); // random # from f1 to f2 inclusive


inline void m_matF_x_point3F(const F32 *m, const F32 *p, F32 *presult)
{
   AssertFatal(p != presult, "Error, aliasing matrix mul pointers not allowed here!");
#if defined(TORQUE_SUPPORTS_GCC_INLINE_X86_ASM)
   // inline assembly version because gcc's math optimization isn't as good
   // JMQ: the profiler shows that with g++ 2.96, the difference
   // between the asm and optimized c versions is minimal.
   int u0, u1, u2;
   __asm__ __volatile__ (
   "flds   0x8(%%eax)\n"
   "fmuls  0x8(%%ecx)\n"
   "flds   0x4(%%eax)\n"
   "fmuls  0x4(%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "flds   (%%eax)\n"
   "fmuls  (%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "fadds  0xc(%%eax)\n"
   "fstps  (%%edx)\n"
   "flds   0x18(%%eax)\n"
   "fmuls  0x8(%%ecx)\n"
   "flds   0x10(%%eax)\n"
   "fmuls  (%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "flds   0x14(%%eax)\n"
   "fmuls  0x4(%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "fadds  0x1c(%%eax)\n"
   "fstps  0x4(%%edx)\n"
   "flds   0x28(%%eax)\n"
   "fmuls  0x8(%%ecx)\n"
   "flds   0x20(%%eax)\n"
   "fmuls  (%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "flds   0x24(%%eax)\n"
   "fmuls  0x4(%%ecx)\n"
   "faddp  %%st,%%st(1)\n"
   "fadds  0x2c(%%eax)\n"
   "fstps  0x8(%%edx)\n"
     : "=&a" (u0), "=&c" (u1), "=&d" (u2)
     : "0"   (m),  "1"   (p),  "2"   (presult)
     : "memory" );
#else
   presult[0] = m[0]*p[0] + m[1]*p[1] + m[2]*p[2]  + m[3];
   presult[1] = m[4]*p[0] + m[5]*p[1] + m[6]*p[2]  + m[7];
   presult[2] = m[8]*p[0] + m[9]*p[1] + m[10]*p[2] + m[11];
#endif
}


//--------------------------------------
inline void m_matF_x_vectorF(const F32 *m, const F32 *v, F32 *vresult)
{
   AssertFatal(v != vresult, "Error, aliasing matrix mul pointers not allowed here!");
#if defined(TORQUE_SUPPORTS_GCC_INLINE_X86_ASM)
   // inline assembly version because gcc's math optimization isn't as good
   // JMQ: the profiler shows that with g++ 2.96, the difference
   // between the asm and optimized c versions is minimal.
   int u0, u1, u2;
   __asm__ __volatile__ (
        "flds   0x8(%%ecx)\n"
        "fmuls  0x8(%%eax)\n"
        "flds   0x4(%%ecx)\n"
        "fmuls  0x4(%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "flds   (%%ecx)\n"
        "fmuls  (%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "fstps  (%%edx)\n"
        "flds   0x18(%%ecx)\n"
        "fmuls  0x8(%%eax)\n"
        "flds   0x10(%%ecx)\n"
        "fmuls  (%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "flds   0x14(%%ecx)\n"
        "fmuls  0x4(%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "fstps  0x4(%%edx)\n"
        "flds   0x28(%%ecx)\n"
        "fmuls  0x8(%%eax)\n"
        "flds   0x20(%%ecx)\n"
        "fmuls  (%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "flds   0x24(%%ecx)\n"
        "fmuls  0x4(%%eax)\n"
        "faddp  %%st,%%st(1)\n"
        "fstps  0x8(%%edx)\n"
     : "=&c" (u0), "=&a" (u1), "=&d" (u2)
     : "0"   (m),  "1"   (v),  "2"   (vresult)
     : "memory" );
#else
   vresult[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
   vresult[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2];
   vresult[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2];
#endif
}

U32 getNextPow2(U32 io_num);

U32 getBinLog2(U32 io_num);


/// Determines if the given U32 is some 2^n
/// @returns true if in_num is a power of two, otherwise false
 inline bool isPow2(const U32 in_num)
{
   return (in_num == getNextPow2(in_num));
}


/// Returns the lesser of the two parameters: a & b.
inline U32 getMin(U32 a, U32 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline U16 getMin(U16 a, U16 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline U8 getMin(U8 a, U8 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline S32 getMin(S32 a, S32 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline S16 getMin(S16 a, S16 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline S8 getMin(S8 a, S8 b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline float getMin(float a, float b)
{
   return a>b ? b : a;
}

/// Returns the lesser of the two parameters: a & b.
inline double getMin(double a, double b)
{
   return a>b ? b : a;
}

/// Returns the greater of the two parameters: a & b.
inline U32 getMax(U32 a, U32 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline U16 getMax(U16 a, U16 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline U8 getMax(U8 a, U8 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline S32 getMax(S32 a, S32 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline S16 getMax(S16 a, S16 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline S8 getMax(S8 a, S8 b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline float getMax(float a, float b)
{
   return a>b ? a : b;
}

/// Returns the greater of the two parameters: a & b.
inline double getMax(double a, double b)
{
   return a>b ? a : b;
}

inline F32 mFloor(const F32 val)
{
   return (F32) floor(val);
}

inline F32 mCeil(const F32 val)
{
   return (F32) ceil(val);
}

inline F32 mFabs(const F32 val)
{
   return (F32) fabs(val);
}

inline F32 mFsign(const F32 val)
{
   return (F32) (val > 0 ? 1 : val < 0 ? -1 : 0);
}

inline F32 mFmod(const F32 val, const F32 mod)
{
   return (F32) fmod(val, mod);
}

inline S32 mAbs(const S32 val)
{
   // Kinda lame, and disallows intrinsic inlining by the compiler.  Maybe fix?
   //  DMM
   if (val < 0)
      return -val;

   return val;
}

inline S32 mClamp(S32 val, S32 low, S32 high)
{
   return getMax(getMin(val, high), low);
}

inline F32 mClampF(F32 val, F32 low, F32 high)
{
   return (F32) getMax(getMin(val, high), low);
}

inline S32 mMulDiv(S32 a, S32 b, S32 c)
{
   return m_mulDivS32(a, b, c);
}

inline U32 mMulDiv(S32 a, S32 b, U32 c)
{
   return m_mulDivU32(a, b, c);
}

/// Template function for doing a linear interpolation between any two
/// types which implement operators for scalar multiply and addition.
template <typename T>
inline T mLerp( const T &v1, const T &v2, F32 factor )
{
   factor = mClampF( factor, 0.0f, 1.0f);
   return ( v1 * ( 1.0f - factor ) ) + ( v2 * factor );
}

template <typename T>
inline T mSmoothStep( const T &v1, const T &v2, F32 factor)
{
   factor = mClampF( factor, 0.0f, 1.0f);
   return mLerp(v1, v2, (factor*factor*(3.0f-2.0f*factor)));
}

inline F32 mSin(const F32 angle)
{
   return (F32) sin(angle);
}

inline F32 mCos(const F32 angle)
{
   return (F32) cos(angle);
}

inline F32 mTan(const F32 angle)
{
   return (F32) tan(angle);
}

inline F32 mAsin(const F32 val)
{
   return (F32) asin(val);
}

inline F32 mAcos(const F32 val)
{
   return (F32) acos(val);
}

inline F32 mAtan(const F32 x, const F32 y)
{
   return (F32) atan2(y, x);
}

inline void mSinCos(const F32 angle, F32 &s, F32 &c)
{
   s = mSin(angle);
   c = mCos(angle);
}

inline F32 mTanh(const F32 angle)
{
   return (F32) tanh(angle);
}

inline F32 mSqrt(const F32 val)
{
   return (F32) sqrt(val);
}

inline F32 mPow(const F32 x, const F32 y)
{
   return (F32) pow(x, y);
}

inline F32 mLog(const F32 val)
{
   return (F32) log(val);
}


inline F64 mSin(const F64 angle)
{
   return (F64) sin(angle);
}

inline F64 mCos(const F64 angle)
{
   return (F64) cos(angle);
}

inline F64 mTan(const F64 angle)
{
   return (F64) tan(angle);
}

inline F64 mAsin(const F64 val)
{
   return (F64) asin(val);
}

inline F64 mAcos(const F64 val)
{
   return (F64) acos(val);
}

inline F64 mAtan(const F64 x, const F64 y)
{
   return (F64) atan2(y, x);
}

inline void mSinCos(const F64 angle, F64 &sin, F64 &cos)
{
   sin = mSin(angle);
   cos = mCos(angle);
}

inline F64 mTanh(const F64 angle)
{
   return (F64) tanh(angle);
}

inline F64 mPow(const F64 x, const F64 y)
{
   return (F64) pow(x, y);
}

inline F64 mLog(const F64 val)
{
   return (F64) log(val);
}


inline F32 mCatmullrom(F32 t, F32 p0, F32 p1, F32 p2, F32 p3)
{
   return m_catmullrom(t, p0, p1, p2, p3);
}


inline F64 mFabsD(const F64 val)
{
   return (F64) fabs(val);
}

inline F64 mFmodD(const F64 val, const F64 mod)
{
   return (F64) fmod(val, mod);
}

inline F64 mSqrtD(const F64 val)
{
   return (F64) sqrt(val);
}

inline F64 mFloorD(const F64 val)
{
   return (F64) floor(val);
}

inline F64 mCeilD(const F64 val)
{
   return (F64) ceil(val);
}

inline bool isEqual(F32 a, F32 b)
{
   return mFabs(a - b) < __EQUAL_CONST_F;
}

inline bool isZero(F32 a)
{
   return mFabs(a) < __EQUAL_CONST_F;
}

//--------------------------------------
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

inline F32 mDot(const Point2F &p1, const Point2F &p2)
{
   return (p1.x*p2.x + p1.y*p2.y);
}

inline F32 mDot(const Point3F &p1, const Point3F &p2)
{
   return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

inline void mCross(const Point3F &a, const Point3F &b, Point3F *res)
{
   res->x = (a.y * b.z) - (a.z * b.y);
   res->y = (a.z * b.x) - (a.x * b.z);
   res->z = (a.x * b.y) - (a.y * b.x);
}

inline F64 mDot(const Point3D &p1, const Point3D &p2)
{
   return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

inline void mCross(const Point3D &a, const Point3D &b, Point3D *res)
{
   res->x = (a.y * b.z) - (a.z * b.y);
   res->y = (a.z * b.x) - (a.x * b.z);
   res->z = (a.x * b.y) - (a.y * b.x);
}

inline Point3F mCross(const Point3F &a, const Point3F &b)
{
   Point3F ret;
   mCross(a,b,&ret);
   return ret;
}

inline void mCross(const F32* a, const F32* b, F32 *res)
{
   res[0] = (a[1] * b[2]) - (a[2] * b[1]);
   res[1] = (a[2] * b[0]) - (a[0] * b[2]);
   res[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

inline void mCross(const F64* a, const F64* b, F64* res)
{
   res[0] = (a[1] * b[2]) - (a[2] * b[1]);
   res[1] = (a[2] * b[0]) - (a[0] * b[2]);
   res[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

void mTransformPlane(const MatrixF& mat, const Point3F& scale, const PlaneF& plane, PlaneF* result);


//--------------------------------------
inline F32 mDegToRad(F32 d)
{
   return F32((d * M_PI) / F32(180));
}

inline F32 mRadToDeg(F32 r)
{
   return F32((r * 180.0) / M_PI);
}

inline F64 mDegToRad(F64 d)
{
   return (d * M_PI) / F64(180);
}

inline F64 mRadToDeg(F64 r)
{
   return (r * 180.0) / M_PI;
}

/// Precision Rounding.
inline F32 mRound(const F32& value, const F32 epsilon = 0.5f) { return value > 0.0f ? mFloor(value + epsilon) : mCeil(value - epsilon); }

/// Is NAN?
inline F32 mIsNAN(const F32& value) { return (value != value); }

/// Tolerate Is Zero?
inline bool mIsZero(const F32& value) { return mFabs(value) < FLT_EPSILON; }

/// Tolerate Not Zero?
inline bool mNotZero(const F32& value) { return !mIsZero(value); }

/// Tolerate Less-Than?
inline bool mLessThan(const F32& a, const F32& b) { return a < b; }

/// Tolerate Greater-Than?
inline bool mGreaterThan(const F32& a, const F32& b) { return a > b; }

/// Safe Less Than Zero?
inline bool mLessThanZero(const F32& value) { return mLessThan(value, 0.0f); }

/// Safe Greater Than Zero?
inline bool mGreaterThanZero(const F32& value) { return mGreaterThan(value, 0.0f); }

/// Safe Is Equal?
inline bool mIsEqual(const F32& a, const F32& b) { return mIsZero(mFabs(a-b)); }

/// Safe Not Equal?
inline bool mNotEqual(const F32& a, const F32& b) { return !mIsEqual(a,b); }

/// Tolerate Is Equal within Range?
inline bool mIsEqualRange(const F32& a, const F32& b, const F32 epsilon = FLT_EPSILON) { return mFabs(a-b) <= epsilon; }

/// Tolerate Is One?
inline bool mIsOne(const F32& value) { return mIsEqual(value, 1.0f); }

/// Tolerate Less-Than or Equal?
inline bool mLessThanOrEqual(const F32& a, const F32& b) { return ( (a < b) || (!(a>b) && mIsEqual(a,b)) ); }

/// Tolerate Greater-Than or Equal?
inline bool mGreaterThanOrEqual(const F32&a, const F32& b) { return ( (a > b) || (!(a < b) && mIsEqual(a,b)) ); }

/// Get Min/Max.
inline void mGetMinMax(const F32& a, const F32& b, F32& min, F32& max) { if ( mGreaterThan(a,b) ) { max = a; min = b; } else { max = b; min = a; } }

/// Swap.
inline void mSwap(F32& a, F32& b) { F32 temp = b; b = a; a = temp; }

//list of possible ways to ease progress
enum EasingFunction
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	EaseInBack,
	EaseOutBack,
	EaseInOutBack,
	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,
	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce
};

static EnumTable::Enums easingEnums[] =
{
   { EasingFunction::Linear, "linear" },
   { EasingFunction::EaseIn, "easeIn" },
   { EasingFunction::EaseOut, "easeOut" },
   { EasingFunction::EaseInOut, "easeInOut" },
   { EasingFunction::EaseInBack, "easeInBack" },
   { EasingFunction::EaseOutBack, "easeOutBack" },
   { EasingFunction::EaseInOutBack, "easeInOutBack" },
   { EasingFunction::EaseInElastic, "easeInElastic" },
   { EasingFunction::EaseOutElastic, "easeOutElastic" },
   { EasingFunction::EaseInOutElastic, "easeInOutElastic" },
   { EasingFunction::EaseInBounce, "easeInBounce" },
   { EasingFunction::EaseOutBounce, "easeOutBounce" },
   { EasingFunction::EaseInOutBounce, "easeInOutBounce" }
};
static EnumTable gEasingTable(13, &easingEnums[0]);

// Given linear time progress between 0 and 1, this returns animation progress, based on 
// a selected easing function, between 0 and 1 (and possibly outside of 0 and 1 in some cases).
F32 mEase(const EasingFunction &ease, const F32 &progress);

#endif //_MMATHFN_H_
