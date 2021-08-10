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
#include "platform/types.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "math/mFluid.h"

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

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

FluidColorI::FluidColorI(ColorI &color)
{
	red = color.red;
	green = color.green;
	blue = color.blue;
	alpha = color.alpha;
}

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

void FluidColorI::set(U8 red, U8 green, U8 blue, U8 alpha)
{
	red = red;
	green = green;
	blue = blue;
	alpha = alpha;
}

//-----------------------------------------------------------------------------

ConsoleType(FluidColorI, TypeFluidColorI, sizeof(FluidColorI), "")

//-----------------------------------------------------------------------------

ConsoleGetType(TypeFluidColorI)
{
	// Fetch color.
	FluidColorI* color = (FluidColorI*)dptr;

	// Fetch stock color name.
	StringTableEntry colorName = StockColor::name(*color);

	// Write as color name if was found.
	if (colorName != StringTable->EmptyString)
		return colorName;

	// Format as color components.
	char* returnBuffer = Con::getReturnBuffer(256);
	dSprintf(returnBuffer, 256, "%d %d %d %d", color->red, color->green, color->blue, color->alpha);
	return returnBuffer;
}

//-----------------------------------------------------------------------------

ConsoleSetType(TypeFluidColorI)
{
	FluidColorI* color = (FluidColorI*)dptr;
	if (argc == 1)
	{
		// Is only a single argument passed?
		if (StringUnit::getUnitCount(argv[0], " ") == 1)
		{
			// Is this a stock color name?
			if (!StockColor::isColor(argv[0]))
			{
				// No, so warn.
				Con::warnf("FluidColorI() - Invalid single argument of '%s' could not be interpreted as a stock color name.  Using default.", argv[0]);
			}

			// Set stock color (if it's invalid we'll get the default.
			color->set(argv[0]);

			return;
		}

		color->set(0, 0, 0, 255);
		S32 r, g, b, a;
		const S32 args = dSscanf(argv[0], "%d %d %d %d", &r, &g, &b, &a);
		color->red = r;
		color->green = g;
		color->blue = b;
		if (args == 4)
			color->alpha = a;
	}
	else if (argc == 3)
	{
		color->red = dAtoi(argv[0]);
		color->green = dAtoi(argv[1]);
		color->blue = dAtoi(argv[2]);
		color->alpha = 255;
	}
	else if (argc == 4)
	{
		color->red = dAtoi(argv[0]);
		color->green = dAtoi(argv[1]);
		color->blue = dAtoi(argv[2]);
		color->alpha = dAtoi(argv[3]);
	}
	else
		Con::printf("Color must be set as { r, g, b [,a] }, { r g b [b] }  or { stockColorName }");
}