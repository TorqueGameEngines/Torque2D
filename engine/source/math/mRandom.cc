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

#include "platform/platform.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "math/mRandom.h"

//-----------------------------------------------------------------------------

RandomLCG gRandGen;
static U32 msSeed = 1376312589;
U32 gRandGenSeed = msSeed;

const S32 RandomLCG::msQuotient  = S32_MAX / 16807L;
const S32 RandomLCG::msRemainder = S32_MAX % 16807L;

//-----------------------------------------------------------------------------

inline U32 generateSeed()
{
    // A very, VERY crude LCG but good enough to generate a nice range of seed values
    msSeed = (msSeed * 0x015a4e35L) + 1;
    msSeed = (msSeed>>16)&0x7fff;
    return msSeed;
}

//-----------------------------------------------------------------------------

void RandomGeneratorBase::resetSeed( void )
{
    setSeed( generateSeed() );
}

//-----------------------------------------------------------------------------

RandomLCG::RandomLCG()
{
    setSeed(generateSeed());
}

//-----------------------------------------------------------------------------

RandomLCG::RandomLCG( const S32 seed )
{
    setSeed( seed );
}

//-----------------------------------------------------------------------------

void RandomLCG::setGlobalRandSeed( const U32 seed )
{
#ifdef	TORQUE_ALLOW_JOURNALING
    if (Game->isJournalReading())
        Game->journalRead(&gRandGenSeed);
    else
    {
        gRandGenSeed = seed;
        if (Game->isJournalWriting())
            Game->journalWrite(gRandGenSeed);
    }
#else
    gRandGenSeed = seed;
#endif	//TORQUE_ALLOW_JOURNALING

    //now actually set the seed
    gRandGen.setSeed( gRandGenSeed );
}

//-----------------------------------------------------------------------------

void RandomLCG::setSeed( const S32 seed )
{
    mSeed = seed;
}

//-----------------------------------------------------------------------------

U32 RandomLCG::randI()
{
    if ( mSeed <= msQuotient )
    {
        mSeed = (mSeed * 16807L) % S32_MAX;
    }
    else
    {
        const S32 high_part = mSeed / msQuotient;
        const S32 low_part  = mSeed % msQuotient;

        const S32 test = (16807L * low_part) - (msRemainder * high_part);

        if ( test > 0 )
            mSeed = test;
        else
            mSeed = test + S32_MAX;
    }

    return mSeed;
}

//-----------------------------------------------------------------------------

RandomR250::RandomR250()
{
    setSeed( generateSeed() );
}

//-----------------------------------------------------------------------------

RandomR250::RandomR250( const S32 seed )
{
    setSeed( seed );
}

//-----------------------------------------------------------------------------

void RandomR250::setSeed( const S32 seed )
{
    mSeed = seed;
    RandomLCG lcg( seed );
    mIndex = 0;

    S32 j;
    for (j = 0; j < 250; j++)        // fill r250 buffer with bit values
        mBuffer[j] = lcg.randI();

    for (j = 0; j < 250; j++)        // set some MSBs to 1
        if ( lcg.randI() > 0x40000000L )
            mBuffer[j] |= 0x80000000L;


    U32 msb  = 0x80000000;           // turn on diagonal bit
    U32 mask = 0xffffffff;           // turn off the leftmost bits

    for (j = 0; j < 32; j++)
    {
        S32 k = 7 * j + 3;            // select a word to operate on
        mBuffer[k] &= mask;           // turn off bits left of the diagonal
        mBuffer[k] |= msb;            // turn on the diagonal bit
        mask >>= 1;
        msb  >>= 1;
    }
}

//-----------------------------------------------------------------------------

U32 RandomR250::randI( void )
{
    S32 j;

    // wrap pointer around
    if ( mIndex >= 147 ) j = mIndex - 147;
    else                 j = mIndex + 103;

    U32 new_rand = mBuffer[ mIndex ] ^ mBuffer[ j ];
    mBuffer[ mIndex ] = new_rand;

    // increment pointer for next time
    if ( mIndex >= 249 ) mIndex = 0;
    else                 mIndex++;

    return new_rand >> 1;
}

RandomPCG::RandomPCG()
{
    pcg32_srandom_r(&rng, generateSeed(), generateSeed());
}

RandomPCG::RandomPCG(const S32 seed)
{
    pcg32_srandom_r(&rng, seed, generateSeed());
}

RandomPCG::RandomPCG(const S32 seed, const S32 stream)
{
    pcg32_srandom_r(&rng, seed, stream);
}

void RandomPCG::setSeed(const S32 seed)
{
    pcg32_srandom_r(&rng, seed, generateSeed());
}

void RandomPCG::setSeed(const S32 seed, const S32 stream)
{
    pcg32_srandom_r(&rng, seed, stream);
}