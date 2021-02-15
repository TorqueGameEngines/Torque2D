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

#include "math/mMathFn.h"
#include "math/mPlane.h"
#include "math/mMatrix.h"


void mTransformPlane(const MatrixF& mat,
                     const Point3F& scale,
                     const PlaneF&  plane,
                     PlaneF*        result)
{
   m_matF_x_scale_x_planeF(mat, &scale.x, &plane.x, &result->x);
}

//--------------------------------------

U32 getNextPow2(U32 io_num)
{
   S32 oneCount   = 0;
   S32 shiftCount = -1;
   while (io_num) {
      if(io_num & 1)
         oneCount++;
      shiftCount++;
      io_num >>= 1;
   }
   if(oneCount > 1)
      shiftCount++;

   return U32(1 << shiftCount);
}

//--------------------------------------

U32 getBinLog2(U32 io_num)
{
   AssertFatal(io_num != 0 && isPow2(io_num) == true,
               "Error, this only works on powers of 2 > 0");

   S32 shiftCount = 0;
   while (io_num) {
      shiftCount++;
      io_num >>= 1;
   }

   return U32(shiftCount - 1);
}

F32 mEase(const EasingFunction &ease, const F32 &progress)
{
	//Animation progress should be between 0 and 1. Result may go past 0 or 1 depending on the animation.
	F32 r = mClampF(progress, 0.0f, 1.0f);
	switch (ease)
	{
		case EasingFunction::EaseIn:
			r = 1 - mCos((r * M_PI_F) / 2);
			break;
		case EasingFunction::EaseOut:
			r = mSin((r * M_PI_F) / 2);
			break;
		case EasingFunction::EaseInOut:
			r = -(mCos(M_PI_F * r) - 1) / 2;
			break;
		case EasingFunction::EaseInBack:
			r = r * r * ((2.70158 * r) - 1.70158);
			break;
		case EasingFunction::EaseOutBack:
			r = 1 + (--r) * r * ((2.70158 * r) + 1.70158);
			break;
		case EasingFunction::EaseInOutBack:
			if (r < 0.5f)
			{
				r = r * r * ((7 * r) - 2.5) * 2;
			}
			else
			{
				r = 1 + (--r) * r * 2 * ((7 * r) + 2.5);
			}
			break;
		case EasingFunction::EaseInElastic:
			if (r < 1 || r > 0)
			{
				const F32 c = (2 * M_PI_F) / 3;
				r = -mPow(2.0f, (10 * r) - 10) * mSin(((r * 10) - 10.75) * c);
			}
			break;
		case EasingFunction::EaseOutElastic:
			if (r < 1 || r > 0)
			{
				const F32 c = (2 * M_PI_F) / 3;
				r = (mPow(2.0f, -10 * r) * mSin(((r * 10) - 0.75) * c)) + 1;
			}
			break;
		case EasingFunction::EaseInOutElastic:
			if (r < 1 || r > 0)
			{
				const F32 c = (2 * M_PI_F) / 4.5;
				if (r < 0.5f)
				{
					r = -(mPow(2.0f, (20 * r) - 10) * mSin(((r * 20) - 11.125) * c)) / 2;
				}
				else
				{
					r = ((mPow(2.0f, (-20 * r) + 10) * mSin(((r * 20) - 11.125) * c)) / 2) + 1;
				}
			}
			break;
		case EasingFunction::EaseInBounce:
			r = mPow(2, 6 * (r - 1)) * mFabs(mSin(r * M_PI_F * 3.5f));
			break;
		case EasingFunction::EaseOutBounce:
			r = 1 - mPow(2, -6 * r) * mFabs(mCos(r * M_PI_F * 3.5f));
			break;
		case EasingFunction::EaseInOutBounce:
			if (r < 0.5f)
			{
				r = 8 * mPow(2, 8 * (r - 1)) * mFabs(mSin(r * M_PI_F * 7.0f));
			}
			else
			{
				r = 1 - (8 * mPow(2, -8 * r) * mFabs(mSin(r * M_PI_F * 7.0f)));
			}
			break;
	}
	return r;
}

