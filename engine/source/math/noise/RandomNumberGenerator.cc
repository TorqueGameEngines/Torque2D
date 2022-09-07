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

#ifndef _RANDOM_NUMBER_GENERATOR_H_
#include "RandomNumberGenerator.h"
#endif

// Script bindings.
#include "RandomNumberGenerator_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(RandomNumberGenerator);

//------------------------------------------------------------------------------

RandomNumberGenerator::RandomNumberGenerator()
{
    mRNG = RandomPCG();
}

//------------------------------------------------------------------------------

RandomNumberGenerator::~RandomNumberGenerator()
{
}

//------------------------------------------------------------------------------

void RandomNumberGenerator::setSeed(const U32 seed)
{
    mRNG.setSeed(seed);
}

void RandomNumberGenerator::setSeed(const U32 seed, const U32 stream)
{
    mRNG.setSeed(seed, stream);
}

U32 RandomNumberGenerator::getSeed()
{
    return mRNG.getSeed();
}

U32 RandomNumberGenerator::getRandom()
{
    return mRNG.randI();
}

U32 RandomNumberGenerator::getRandom(const U32 bound)
{
    return mRNG.randI(bound);
}