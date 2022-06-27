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
#define _NOISE_GENERATOR_H_

#ifndef PERLINNOISE_H
#include "algorithm/Perlin.h"
#endif

#ifndef _SCRIPT_OBJECT_H_
#include "sim/scriptObject.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

//-----------------------------------------------------------------------------

class NoiseGenerator : public ScriptObject
{
	typedef ScriptObject			Parent;

private:
	PerlinNoise					mPerlin;
	U32							mSeed;

public:
	NoiseGenerator();
	virtual ~NoiseGenerator();

	void setSeed(const U32 seed);
	F64 getNoise(F64 x, F64 y);
	F64 getComplexNoise(F64 x, F64 y, S32 octaves, F64 persistence);

	/// Declare Console Object.
	DECLARE_CONOBJECT(NoiseGenerator);

private:

protected:
};

#endif // _NOISE_GENERATOR_H_
