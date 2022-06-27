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

#ifndef _NOISE_GENERATOR_H_
#include "NoiseGenerator.h"
#endif

// Script bindings.
#include "NoiseGenerator_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(NoiseGenerator);

//------------------------------------------------------------------------------

NoiseGenerator::NoiseGenerator() :
	mSeed(0)
{
	
}

//------------------------------------------------------------------------------

NoiseGenerator::~NoiseGenerator()
{
}

//------------------------------------------------------------------------------

void NoiseGenerator::setSeed(const U32 seed)
{
	mSeed = seed;
	mPerlin = PerlinNoise(mSeed);
}

F64 NoiseGenerator::getNoise(F64 x, F64 y)
{
	return mPerlin.noise(x,y,0.2f);
}

F64 NoiseGenerator::getComplexNoise(F64 x, F64 y, S32 octaves, F64 persistence)
{
    octaves = mClamp(octaves, 1, 8);
    persistence = mClampF(persistence, 0.05f, 0.95f);

    F64 total = 0;
    F64 frequency = 1;
    F64 amplitude = 1;
    F64 maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for (S32 i = 0; i < octaves; i++) {
        total += mPerlin.noise(x * frequency, y * frequency, 0.2f * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}