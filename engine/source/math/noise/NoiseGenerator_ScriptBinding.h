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

ConsoleMethodGroupBeginWithDocs(NoiseGenerator, ScriptObject)

/*! Sets the seed for the noise generater.
@param seed An integer seed value.
@return No return value.
*/
ConsoleMethodWithDocs(NoiseGenerator, setSeed, ConsoleVoid, 3, 3, (int seed))
{
	object->setSeed(dAtoi(argv[2]));
}

//------------------------------------------------------------------------------

/*! Returns the noise for a given x and y value.
* @param x A floating point value.
* @param y A floating point value.
@return A decimal value between 0 and 1.
*/
ConsoleMethodWithDocs(NoiseGenerator, getNoise, ConsoleFloat, 3, 4, (float x, float y))
{
	// Elements in the first argument.
	U32 elementCount = Utility::mGetStringElementCount(argv[2]);

	if (elementCount == 1 && argc == 4)
	{
		return object->getNoise(dAtof(argv[2]), dAtof(argv[3]));
	}
	else if (elementCount == 2 && argc == 3)
	{
		return object->getNoise(dAtof(Utility::mGetStringElement(argv[2], 0)), dAtof(Utility::mGetStringElement(argv[2], 1)));
	}
	else
	{
		Con::warnf("NoiseGenerator::getNoise() - Invalid number of parameters!");
		return 0.0f;
	}
}

//------------------------------------------------------------------------------

/*! Returns the noise for a given x and y value with multiple octaves applied.
* @param x A floating point value.
* @param y A floating point value.
* @param octaves An integer value between 1 and 8. Each octave is a layer of progressively smaller noise that is applied to the final result.
* @param persistence A decimal value between 0.05 and 0.95. Larger values will cause each octave to have a larger impact on the final result.
@return A decimal value between 0 and 1.
*/
ConsoleMethodWithDocs(NoiseGenerator, getComplexNoise, ConsoleFloat, 6, 6, (float x, float y, int octaves, float persistence))
{
	if (argc < 6)
	{
		Con::warnf("NoiseGenerator::getComplexNoise() - Invalid number of parameters!");
		return 0.0f;
	}
	else 
	{
		return object->getComplexNoise(dAtof(argv[2]), dAtof(argv[3]), dAtoi(argv[4]), dAtof(argv[5]));
	}
}

ConsoleMethodGroupEndWithDocs(NoiseGenerator)