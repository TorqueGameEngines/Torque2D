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

ConsoleMethodGroupBeginWithDocs(RandomNumberGenerator, ScriptObject)

/*! Sets the seed and stream for the random number generater. Random values will always be the same for a seed/stream combination.
@param seed An integer seed value.
@param stream An optional integer value for the stream. If no stream is set a random stream value will be picked.
@return No return value.
*/
ConsoleMethodWithDocs(RandomNumberGenerator, setSeed, ConsoleVoid, 3, 4, (int seed, [int stream]))
{
	if (argc == 3)
	{
		object->setSeed(dAtoi(argv[2]));
	}
	else if (argc == 4)
	{
		object->setSeed(dAtoi(argv[2]), dAtoi(argv[3]));
	}
}

//------------------------------------------------------------------------------

/*! Returns a random number integer value between 0 and an optional upper bound.
* @param bound The optional value that the random number will be less than.
@return A 32 bit number or a value where 0 <= number < bound.
*/
ConsoleMethodWithDocs(RandomNumberGenerator, getRandom, ConsoleFloat, 2, 3, ([int bound]))
{
	if (argc == 3)
	{
		return object->getRandom(dAtoi(argv[2]));
	}
	else
	{
		return object->getRandom();
	}
}

ConsoleMethodGroupEndWithDocs(RandomNumberGenerator)