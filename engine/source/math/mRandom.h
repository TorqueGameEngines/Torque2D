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

#ifndef _MRANDOM_H_
#define _MRANDOM_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#include "algorithm/pcg_basic.h"

//-----------------------------------------------------------------------------

class RandomGeneratorBase
{
protected:
    RandomGeneratorBase() {}
    S32  mSeed;

public:
    void resetSeed( void );
    inline S32 getSeed( void ) const { return mSeed; }

    inline F32 randRangeF( const F32 from, const F32 to ) { AssertFatal( from <= to, "RandomGeneratorBase::randF() - Inverted range." ); return (from + (to - from) * randF()); }
    inline S32 randRangeI( const S32 from, const S32 to ) { AssertFatal( from <= to, "RandomGeneratorBase::randI() - Inverted range." ); return (S32)(from + (randI() % (to - from + 1)) ); }

    virtual F32 randF( void ) { return F32(randI()) * F32(1.0/2147483647.0); }
    virtual U32 randI( void ) = 0;
    virtual void setSeed( const S32 seed ) = 0;
};

//-----------------------------------------------------------------------------
/// Linear Congruential Method, the "minimal standard generator"
///
/// Fast, fairly good random numbers (better than using rand)
///
/// @author Park & Miller, 1988, Comm of the ACM, 31(10), pp. 1192-1201
//-----------------------------------------------------------------------------
class RandomLCG : public RandomGeneratorBase
{
protected:
   static const S32 msQuotient;
   static const S32 msRemainder;

public:
    RandomLCG();
    RandomLCG( const S32 seed );

    static void setGlobalRandSeed( const U32 seed );

    void setSeed( const S32 seed );
    U32 randI( void );
};


//-----------------------------------------------------------------------------
/// Fast, very good random numbers
///
/// Period = 2^249
///
/// Kirkpatrick, S., and E. Stoll, 1981; A Very Fast Shift-Register
///       Sequence Random Number Generator, Journal of Computational Physics,
/// V. 40.
///
/// Maier, W.L., 1991; A Fast Pseudo Random Number Generator,
/// Dr. Dobb's Journal, May, pp. 152 - 157
//-----------------------------------------------------------------------------
class RandomR250 : public RandomGeneratorBase
{
private:
   U32 mBuffer[250];
   S32 mIndex;

public:
   RandomR250();
   RandomR250( const S32 seed );

   void setSeed( const S32 seed );
   U32 randI( void );
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/// PCG Random Number Generator
/// 
/// Fast and statistically excellent random numbers
///
/// Period = 2^64
///
/// Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
///
/// Licensed under the Apache License, Version 2.0
//-----------------------------------------------------------------------------
class RandomPCG : public RandomGeneratorBase
{
private:
    pcg32_random_t rng;

public:
    RandomPCG();
    RandomPCG(const S32 seed);
    RandomPCG(const S32 seed, const S32 stream);

    void setSeed(const S32 seed);
    void setSeed(const S32 seed, const S32 stream);
    U32 randI(void) { return static_cast<U32>(pcg32_random_r(&rng)); }
    U32 randI(U32 bound) { return static_cast<U32>(pcg32_boundedrand_r(&rng, static_cast<uint32_t>(bound))); }
};

//-----------------------------------------------------------------------------

extern RandomLCG gRandGen;

#endif //_MRANDOM_H_
