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

#ifndef _TORQUE_TYPES_H_
#define _TORQUE_TYPES_H_

//------------------------------------------------------------------------------
//-------------------------------------- Basic Types...

typedef signed char        S8;      ///< Compiler independent Signed Char
typedef unsigned char      U8;      ///< Compiler independent Unsigned Char

typedef signed short       S16;     ///< Compiler independent Signed 16-bit short
typedef unsigned short     U16;     ///< Compiler independent Unsigned 16-bit short

typedef signed int         S32;     ///< Compiler independent Signed 32-bit integer
typedef unsigned int       U32;     ///< Compiler independent Unsigned 32-bit integer

typedef float              F32;     ///< Compiler independent 32-bit float
typedef double             F64;     ///< Compiler independent 64-bit float


//------------------------------------------------------------------------------
//------------------------------------- String Types

typedef char           UTF8;        ///< Compiler independent 8  bit Unicode encoded character
typedef unsigned short UTF16;       ///< Compiler independent 16 bit Unicode encoded character
typedef unsigned int   UTF32;       ///< Compiler independent 32 bit Unicode encoded character

typedef const char* StringTableEntry;

//------------------------------------------------------------------------------
//-------------------------------------- Type constants...
#define __EQUAL_CONST_F F32(0.000001)                             ///< Constant float epsilon used for F32 comparisons

static const F32 Float_One  = F32(1.0);                           ///< Constant float 1.0
static const F32 Float_Half = F32(0.5);                           ///< Constant float 0.5
static const F32 Float_Zero = F32(0.0);                           ///< Constant float 0.0
static const F32 Float_Pi   = F32(3.14159265358979323846);        ///< Constant float PI
static const F32 Float_2Pi  = F32(2.0 * 3.14159265358979323846);  ///< Constant float 2*PI

static const S8  S8_MIN  = S8(-128);                              ///< Constant Min Limit S8
static const S8  S8_MAX  = S8(127);                               ///< Constant Max Limit S8
static const U8  U8_MAX  = U8(255);                               ///< Constant Max Limit U8

static const S16 S16_MIN = S16(-32768);                           ///< Constant Min Limit S16
static const S16 S16_MAX = S16(32767);                            ///< Constant Max Limit S16
static const U16 U16_MAX = U16(65535);                            ///< Constant Max Limit U16

static const S32 S32_MIN = S32(-2147483647 - 1);                  ///< Constant Min Limit S32
static const S32 S32_MAX = S32(2147483647);                       ///< Constant Max Limit S32
static const U32 U32_MAX = U32(0xffffffff);                       ///< Constant Max Limit U32

static const F32 F32_MIN = F32(1.175494351e-38F);                 ///< Constant Min Limit F32
static const F32 F32_MAX = F32(3.402823466e+38F);                 ///< Constant Max Limit F32

//--------------------------------------
// Identify the compiler being used

// Metrowerks CodeWarrior
#if defined(__MWERKS__)
#  include "platform/types.codewarrior.h"
// Microsoft Visual C++/Visual.NET
#elif defined(_MSC_VER)
#  include "platform/types.visualc.h"
// GNU GCC
#elif defined(__GNUC__)
#  include "platform/types.gcc.h"
#else
#  error "Unknown Compiler"
#endif

/// Integral type matching the host's memory address width.
#ifdef TORQUE_CPU_X64
typedef U64 MEM_ADDRESS;
#else
typedef U32 MEM_ADDRESS;
#endif


//--------------------------------------
// Enable Asserts in all debug builds -- AFTER compiler types include.
#if defined(TORQUE_DEBUG)
#define TORQUE_ENABLE_ASSERTS
#endif

/**
   Convert the byte ordering on the U16 to and from big/little endian format.
   @param in_swap Any U16
   @returns swaped U16.
 */
inline U16 endianSwap(const U16 in_swap)
{
   return U16(((in_swap >> 8) & 0x00ff) |
              ((in_swap << 8) & 0xff00));
}

/**
   Convert the byte ordering on the U32 to and from big/little endian format.
   @param in_swap Any U32
   @returns swaped U32.
 */
inline U32 endianSwap(const U32 in_swap)
{
   return U32(((in_swap >> 24) & 0x000000ff) |
              ((in_swap >>  8) & 0x0000ff00) |
              ((in_swap <<  8) & 0x00ff0000) |
              ((in_swap << 24) & 0xff000000));
}

//-------------------------------------- Use this instead of Win32 FOURCC()
//                                        macro...
//
#define makeFourCCTag(ch0, ch1, ch2, ch3)    \
   (((U32(ch0) & 0xFF) << 0)  |             \
    ((U32(ch1) & 0xFF) << 8)  |             \
    ((U32(ch2) & 0xFF) << 16) |             \
    ((U32(ch3) & 0xFF) << 24) )

#define makeFourCCString(ch0, ch1, ch2, ch3) { ch0, ch1, ch2, ch3 }

#define BIT(x) (1 << (x))                       ///< Returns value with bit x set (2^x)

#ifndef Offset
/// Offset macro:
/// Calculates the location in memory of a given member x of class cls from the
/// start of the class.  Need several definitions to account for various
/// flavors of GCC.

// define all the variants of Offset that we might use
#define _Offset_Normal(x, cls) ((dsize_t)((const char *)&(((cls *)1)->x)-(const char *)1))
#define _Offset_Variant_1(x, cls) ((dsize_t)(&((cls *)1)->x) - 1)
#define _Offset_Variant_2(x, cls) offsetof(cls, x) // also requires #include <stddef.h>

// now, for each compiler type, define the Offset macros that should be used.
// The Engine code usually uses the Offset macro, but OffsetNonConst is needed
// when a variable is used in the indexing of the member field (see
// TSShapeConstructor::initPersistFields for an example)

// compiler is non-GCC, or gcc < 3
#if !defined(TORQUE_COMPILER_GCC) || (__GNUC__ < 3)
#define Offset(x, cls) _Offset_Normal(x, cls)
#define OffsetNonConst(x, cls) _Offset_Normal(x, cls)

// compiler is GCC 3 with minor version less than 4
#elif defined(TORQUE_COMPILER_GCC) && (__GNUC__ == 3) && (__GNUC_MINOR__ < 4)
#define Offset(x, cls) _Offset_Variant_1(x, cls)
#define OffsetNonConst(x, cls) _Offset_Variant_1(x, cls)

// compiler is GCC 3 with minor version greater than 4
#elif defined(TORQUE_COMPILER_GCC) && (__GNUC__ == 3) && (__GNUC_MINOR__ >= 4)
#include <stddef.h>
#define Offset(x, cls) _Offset_Variant_2(x, cls)
#define OffsetNonConst(x, cls) _Offset_Variant_1(x, cls)

// compiler is GCC 4
#elif defined(TORQUE_COMPILER_GCC) && (__GNUC__ == 4)
#include <stddef.h>
#define Offset(x, cls) _Offset_Normal(x, cls)
#define OffsetNonConst(x, cls) _Offset_Variant_1(x, cls)

#elif defined(TORQUE_COMPILER_GCC) && (__GNUC__ >= 5)
#include <stddef.h>
#define Offset(x, cls) offsetof(cls, x)
#define OffsetNonConst(x, cls) _Offset_Normal(x, cls)

#endif
#endif

#endif //_TORQUE_TYPES_H_
