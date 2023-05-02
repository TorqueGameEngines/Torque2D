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
#define _RANDOM_NUMBER_GENERATOR_H_

#ifndef _MRANDOM_H_
#include "math/mRandom.h"
#endif

#ifndef _SCRIPT_OBJECT_H_
#include "sim/scriptObject.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

//-----------------------------------------------------------------------------

class RandomNumberGenerator : public ScriptObject
{
	typedef ScriptObject			Parent;

private:
	RandomPCG					mRNG;

public:
	RandomNumberGenerator();
	virtual ~RandomNumberGenerator();

	void setSeed(const U32 seed);
	void setSeed(const U32 seed, const U32 stream);
	U32 getSeed();
	U32 getRandom();
	U32 getRandom(const U32 bound);

	/// Declare Console Object.
	DECLARE_CONOBJECT(RandomNumberGenerator);

private:

protected:
};

#endif // _NOISE_GENERATOR_H_
