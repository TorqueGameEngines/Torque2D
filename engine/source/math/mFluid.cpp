//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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
#include "math/mFluid.h"

Fluid::Fluid()
{
	mAnimationLength = 1000;
	mAnimationProgress = 1.0f;
	mEasingFunction = EasingFunction::Linear;
}

Fluid::~Fluid() { }

void Fluid::startFluidAnimation()
{
	mAnimationProgress = 0.0f;
}

F32 Fluid::getProgress(const S32 time)
{
	mAnimationProgress += (F32)((F32)time / (F32)mAnimationLength);

	F32 progress = 1.0f;
	if (mAnimationProgress < 1.0f)
	{
		progress = mEase(mEasingFunction, mAnimationProgress);
	}
	return progress;
}

void Fluid::setEasingFunction(const char* label)
{
	EasingFunction f = Linear;
	for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(easingEnums[i].label, label) == 0)
			f = (EasingFunction)easingEnums[i].index;
	}

	mEasingFunction = f;
}

const char* Fluid::getEasingFunctionDescription(const EasingFunction ease)
{
	for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (easingEnums[i].index == ease)
			return easingEnums[i].label;
	}

	return StringTable->EmptyString;
}

//------------------------------------------------------------------------

void FluidColorI::startFluidAnimation(const ColorI &color)
{
	mAnimationProgress = 0.0f;

	redStart = red;
	greenStart = green;
	blueStart = blue;
	alphaStart = alpha;

	redTarget = color.red;
	greenTarget = color.green;
	blueTarget = color.blue;
	alphaTarget = color.alpha;
}

//Returns true if we should continue to processTicks 
bool FluidColorI::processTick()
{
	F32 progress = getProgress(32.0f);

	red = processValue(progress, redStart, redTarget);
	green = processValue(progress, greenStart, greenTarget);
	blue = processValue(progress, blueStart, blueTarget);
	alpha = processValue(progress, alphaStart, alphaTarget);

	clamp();

	if (mAnimationProgress >= 1.0f)
	{
		mAnimationProgress = 1.0f;
		return false;
	}
	return true;
}

void FluidColorI::set(const ColorI& in_rCopy)
{
	red = in_rCopy.red;
	green = in_rCopy.green;
	blue = in_rCopy.blue;
	alpha = in_rCopy.alpha;
}