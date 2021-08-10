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
#include "console/console.h"
#include "math/mMathFn.h"
#include "math/mRandom.h"
#include "string/stringUnit.h"

ConsoleFunctionGroupBegin( GeneralMath, "General math functions. Use these whenever possible, as they'll run much faster than script equivalents.");

/*! @defgroup MathFunctions Math (Non-Vector)
	@ingroup TorqueScriptFunctions
	@{
*/

/*! 
    Solve a quadratic equation of form a*x^2 + b*x + c = 0.
    @returns A triple, contanining: sol x0 x1. sol is the number of
     solutions (being 0, 1, or 2), and x0 and x1 are the solutions, if any.
     Unused x's are undefined.
*/
ConsoleFunctionWithDocs( mSolveQuadratic, ConsoleString, 4, 4, (float a, float b, float c))
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[2];
   U32 sol = mSolveQuadratic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), x);
   dSprintf(retBuffer, 256, "%d %g %g", sol, x[0], x[1]);
   return retBuffer;
}

/*! 
    Solve a cubic equation of form a*x^3 + b*x^2 + c*x + d = 0.
    @returns A 4-tuple, contanining: sol x0 x1 x2. sol is the number of
     solutions (being 0, 1, 2, or 3), and x0, x1, x2 are the solutions, if any.
     Unused x's are undefined.
*/
ConsoleFunctionWithDocs( mSolveCubic, ConsoleString, 5, 5, (float a, float b, float c, float d))
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[3];
   U32 sol = mSolveCubic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), x);
   dSprintf(retBuffer, 256, "%d %g %g %g", sol, x[0], x[1], x[2]);
   return retBuffer;
}

/*! 
    Solve a quartic equation of form a*x^4 + b*x^3 + c*x^2 + d*x + e = 0.
    @returns A 5-tuple, contanining: sol x0 x1 x2 x3. sol is the number of
     solutions (ranging from 0-4), and x0, x1, x2 and x3 are the solutions, if any.
     Unused x's are undefined.
*/
ConsoleFunctionWithDocs( mSolveQuartic, ConsoleString, 6, 6, (float a, float b, float c, float d, float e))
{
   char * retBuffer = Con::getReturnBuffer(256);
   F32 x[4];
   U32 sol = mSolveQuartic(dAtof(argv[1]), dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]), x);
   dSprintf(retBuffer, 256, "%d %g %g %g %g", sol, x[0], x[1], x[2], x[3]);
   return retBuffer;
}

/*! Use the mFloor function to calculate the next lowest integer value from val.
    @param val A floating-point value.
    @return Returns an integer representing the next lowest integer from val.
    @sa mCeil
*/
ConsoleFunctionWithDocs( mFloor, ConsoleInt, 2, 2, ( val ))
{
   return (S32)mFloor(dAtof(argv[1]));
}
/*! Rounds a number. 0.5 is rounded up.
    @param val A floating-point value
    @return Returns the integer value closest to the given float

*/
ConsoleFunctionWithDocs( mRound, ConsoleFloat, 2, 2, (float v))
{
   return mRound( dAtof(argv[1]) );
}

/*! Use the mCeil function to calculate the next highest integer value from val.
    @param val A floating-point value.
    @return Returns an integer representing the next highest integer from val.
    @sa mFloor
*/
ConsoleFunctionWithDocs( mCeil, ConsoleInt, 2, 2, ( val ))
{
   return (S32)mCeil(dAtof(argv[1]));
}


/*! Use the mFloatLength function to limit the number of decimal places in val to numDecimals.
    @param val A floating-point value.
    @param numDecimals An integer between 0 and inf representing the number of decimal places to allow val to have.
    @return Returns a floating-point value equivalent to a truncated version of val, where the new version has numDecimals decimal places
*/
ConsoleFunctionWithDocs( mFloatLength, ConsoleString, 3, 3, ( val , numDecimals ))
{
   char * outBuffer = Con::getReturnBuffer(256);
   char fmtString[8] = "%.0f";
   U32 precision = dAtoi(argv[2]);
   if (precision > 9)
      precision = 9;
   fmtString[2] = '0' + precision;

   dSprintf(outBuffer, 255, fmtString, dAtof(argv[1]));
   return outBuffer;
}

//------------------------------------------------------------------------------
/*! Use the mAbs function to get the magnitude of val.
    @param val An integer or a floating-point value.
    @return Returns the magnitude of val
*/
ConsoleFunctionWithDocs( mAbs, ConsoleFloat, 2, 2, ( val ))
{
   return(mFabs(dAtof(argv[1])));
}

/*! Use the mSqrt function to calculated the square root of val.
    @param val A numeric value.
    @return Returns the the squareroot of val
*/
ConsoleFunctionWithDocs( mSqrt, ConsoleFloat, 2, 2, ( val ))
{
   return(mSqrt(dAtof(argv[1])));
}

/*! Use the mPow function to calculated val raised to the power of power.
    @param val A numeric (integer or floating-point) value to be raised to a power.
    @param power A numeric (integer or floating-point) power to raise val to.
    @return Returns val^power
*/
ConsoleFunctionWithDocs( mPow, ConsoleFloat, 3, 3, ( val , power ))
{
   return(mPow(dAtof(argv[1]), dAtof(argv[2])));
}

/*! Use the mLog function to calculate the natural logarithm of val.
    @param val A numeric value.
    @return Returns the natural logarithm of val
*/
ConsoleFunctionWithDocs( mLog, ConsoleFloat, 2, 2, ( val ))
{
   return(mLog(dAtof(argv[1])));
}

/*! Use the mSin function to get the sine of the angle val.
    @param val A value in degrees.
    @return Returns the sine of val. This value will be in the range [ -1.0 , 1.0 ].
    @sa mAsin
*/
ConsoleFunctionWithDocs( mSin, ConsoleFloat, 2, 2, ( val ))
{
   return(mSin(mDegToRad(dAtof(argv[1]))));
}

/*! Use the mCos function to get the cosine of the angle val.
    @param val A value in degrees.
    @return Returns the cosine of val. This value will be in the range [ -1.0 , 1.0 ].
    @sa mAcos
*/
ConsoleFunctionWithDocs( mCos, ConsoleFloat, 2, 2, ( val ))
{
   return(mCos(mDegToRad(dAtof(argv[1]))));
}

/*! Use the mTan function to get the tangent of the angle val.
    @param val A value in degrees.
    @return Returns the tangent of val. This value will be in the range [ -inf.0 , inf.0 ].
    @sa mAtan
*/
ConsoleFunctionWithDocs( mTan, ConsoleFloat, 2, 2, ( val ))
{
   return(mTan(mDegToRad(dAtof(argv[1]))));
}

/*! Use the mAsin function to get the inverse sine of val in degrees.
    @param val A value between -1.0 and 1.0 equal to the sine of some angle theta.
    @return Returns the inverse sine of val in degrees. This value will be in the range [ -90, 90 ].
    @sa mSin
*/
ConsoleFunctionWithDocs( mAsin, ConsoleFloat, 2, 2, ( val ))
{
   return(mRadToDeg(mAsin(dAtof(argv[1]))));
}

/*! Use the mAcos function to get the inverse cosine of val in degrees.
    @param val A value between -1.0 and 1.0 equal to the cosine of some angle theta.
    @return Returns the inverse cosine of val in radians. This value will be in the range [ 0 , 180 ].
    @sa mCos
*/
ConsoleFunctionWithDocs( mAcos, ConsoleFloat, 2, 2, ( val ))
{
   return(mRadToDeg(mAcos(dAtof(argv[1]))));
}

/*! Use the mAtan function to get the inverse tangent of rise/run in degrees.
    May be called as mAtan( deltaX, deltaY ) or mAtan( "deltaX deltaY" ).
    @param x-axis run Horizontal component of a line.
    @param y-axis rise Vertical component of a line.
    @return Returns the slope in degrees (the arc-tangent) of a line with the given run and rise.
    @sa mTan
*/
ConsoleFunctionWithDocs( mAtan, ConsoleFloat, 2, 3, ( val ))
{
   F32 xRun, yRise;
   if( argc == 3 )
   {
      xRun = dAtof( argv[1] );
      yRise = dAtof( argv[2] );
   }
   else if( StringUnit::getUnitCount( argv[1], " " ) == 2 )
   {
     dSscanf( argv[1], "%g %g", &xRun, &yRise );
   }
   else
   {
     Con::warnf( "mAtan - Invalid parameters." );
     return 0;
   }
   return(mRadToDeg(mAtan(xRun, yRise)));
}

/*! Use the mRadToDeg function to convert radians to degrees.
    @param val A floating-point number representing some number of radians.
    @return Returns the equivalent of the radian value val in degrees.
    @sa mDegToRad
*/
ConsoleFunctionWithDocs( mRadToDeg, ConsoleFloat, 2, 2, ( val ))
{
   return(mRadToDeg(dAtof(argv[1])));
}

/*! Use the mDegToRad function to convert degrees to radians.
    @param val A floating-point number representing some number of degrees.
    @return Returns the equivalent of the degree value val in radians.
    @sa mRadToDeg
*/
ConsoleFunctionWithDocs( mDegToRad, ConsoleFloat, 2, 2, ( val ))
{
   return(mDegToRad(dAtof(argv[1])));
}

/*! Clamp a value between two other values.
    @param number A float value representing the number to clamp
    @param min The lower bound
    @param max The upper bound
    @return A float value the is within the given range
*/
ConsoleFunctionWithDocs( mClamp, ConsoleFloat, 4, 4, (float number, float min, float max))
{
   F32 value = dAtof( argv[1] );
   F32 min = dAtof( argv[2] );
   F32 max = dAtof( argv[3] );
   return mClampF( value, min, max );
}

//-----------------------------------------------------------------------------

/*! Returns the Minimum of two values.
*/
ConsoleFunctionWithDocs( mGetMin, ConsoleFloat, 3, 3, (a, b))
{
   return getMin(dAtof(argv[1]), dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Returns the Maximum of two values.
*/
ConsoleFunctionWithDocs( mGetMax, ConsoleFloat, 3, 3, (a, b))
{
   return getMax(dAtof(argv[1]), dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Converts a list of bit-positions into a value.
*/
ConsoleFunctionWithDocs( bits, ConsoleString, 2, 2, ())
{
    // Calculate Element Count.
    U32 elementCount = StringUnit::getUnitCount( argv[1], " \t\n" );

    // Return nothing if there's no elements.
    if ( elementCount < 1 )
    {
        // Error!
        Con::printf("bits() - Invalid number of parameters!");
        return "0";
    }

    // Reset Bit Value.
    U32 bitValue = 0;

    // Parse Bits.
    for ( U32 n = 0; n < elementCount; n++ )
    {
        // Merge Bit Value.
        bitValue |= (U32)BIT(dAtoi(StringUnit::getUnit(argv[1], n, " \t\n")));
    }

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);
    // Format Output.
    dSprintf( pBuffer, 16, "%u", bitValue );
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Converts a bit-position into a value.
*/
ConsoleFunctionWithDocs( bit, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);

    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(BIT(dAtoi(argv[1]))) );

    // Return Buffer.
    return pBuffer;
}

/*! Returns the ones complement of a bit.
*/
ConsoleFunctionWithDocs( bitInverse, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(16);

    // Format Output.
    dSprintf( pBuffer, 16, "%u", U32(~BIT(dAtoi(argv[1]))) );

    // Return Buffer.
    return pBuffer;
}

/*! Returns the mask with a bit added to it
*/
ConsoleFunctionWithDocs( addBitToMask, ConsoleInt, 3, 3, ( mask, bit ))
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask | bit;
}

/*! Returns the mask with a bit removed from it
*/
ConsoleFunctionWithDocs( removeBitFromMask, ConsoleInt, 3, 3, ( mask, bit ))
{
   U32 mask;
   dSscanf( argv[1], "%u", &mask );
   U32 bit = BIT( dAtoi( argv[2] ) );
   
   return mask & ~bit;
}

ConsoleFunctionGroupEnd(GeneralMath)

/*! Given a linear progress from 0 to 1, the ease function returns an "eased" version of that progress. The 
	returned value might be outside of 0 and 1.
	@param easeFunction The name of the easing function to use. Possible values include EaseIn, EaseOut, EaseInOut and postfix versions of Back, Elastic, and Bounce (i.e. EaseInBounce).
	@param progress Linear progress from 0 to 1.
	@return An eased progress from 0 to 1 (although the value might go outside of 0 and 1).
*/
ConsoleFunctionWithDocs(mEase, ConsoleFloat, 3, 3, (easeFunction, progress))
{
	F32 progress = 0.0;
	if (argc >= 3)
	{
		progress = dAtof(argv[2]);
	}
	EasingFunction f = Linear;
	for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(easingEnums[i].label, argv[1]) == 0)
			f = (EasingFunction)easingEnums[i].index;
	}

	return mEase(f, progress);
}

/*! @} */ // group MathFunctions
