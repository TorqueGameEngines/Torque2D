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

#ifndef _MFLUID_H_
#define _MFLUID_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _MMATHFN_H_
#include "math/mMathFn.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

DefineConsoleType(TypeFluidColorI)

class Fluid
{
protected:
	F32 mAnimationProgress; //a time value between 0 and 1 
	S32 mAnimationLength; //Length of the currently playing animation in milliseconds.
	EasingFunction mEasingFunction;

public:
	Fluid();
	~Fluid();

	inline S32 getAnimationLength() { return mAnimationLength; }
	inline void setAnimationLength(S32 time) { mAnimationLength = time; }

	inline EasingFunction getEasingFunction() { return mEasingFunction; }
	inline void setEasingFunction(EasingFunction ease) { mEasingFunction = ease; }
	void setEasingFunction(const char* label);
	const char* getEasingFunctionDescription(const EasingFunction ease);

	virtual void startFluidAnimation(); //starts an animation.
	inline void stopFluidAnimation() { mAnimationProgress = 1.0f; }
	inline bool isAnimating() { return mAnimationProgress < 1.0f; }
	F32 getProgress(const S32 time); //Returns the progress to the target with 0 as starting and 1 as the target.

	inline U8 processValue(const F32 progress, const U8 start, const U8 target) { return start + (U8)mRound((target - start) * progress); };
	inline S32 processValue(const F32 progress, const S32 start, const S32 target) { return start + (S32)mRound((target - start) * progress); };
	inline F32 processValue(const F32 progress, const F32 start, const F32 target) { return start + ((target - start) * progress); };
};

class FluidColorI : public ColorI, public Fluid
{
protected:
	U8 redStart;
	U8 greenStart;
	U8 blueStart;
	U8 alphaStart;

	U8 redTarget;
	U8 greenTarget;
	U8 blueTarget;
	U8 alphaTarget;

public:
	FluidColorI() { }
	FluidColorI(ColorI &color);
	FluidColorI(const U8 in_r,
		const U8 in_g,
		const U8 in_b,
		const U8 in_a = U8(255)) : ColorI(in_r, in_g, in_b, in_a) { };

	virtual void startFluidAnimation(const ColorI &color);
	virtual bool processTick(); //Returns true if we should continue to processTicks
	void set(const ColorI& in_rCopy);
	void set(U8 red, U8 green, U8 blue, U8 alpha = 255);
};

#endif // _MFLUID_H_
