#ifndef _FADETOCOLOR_H_
#define _FADETOCOLOR_H_

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

class FadeToColor
{
public:
	FadeToColor() : TargetColor(1.0f, 1.0f, 1.0f, 1.0f), DeltaRed(0.0f), DeltaGreen(0.0f), DeltaBlue(0.0f), DeltaAlpha(0.0f) { }
	~FadeToColor() { }

	ColorF					TargetColor;
	F32						DeltaRed;
	F32						DeltaGreen;
	F32						DeltaBlue;
	F32						DeltaAlpha;

	void set(ColorF targetColor, F32 deltaRed, F32 deltaGreen, F32 deltaBlue, F32 deltaAlpha)
	{
		TargetColor = targetColor;
		DeltaRed = deltaRed;
		DeltaGreen = deltaGreen;
		DeltaBlue = deltaBlue;
		DeltaAlpha = deltaAlpha;
	}
};

#endif //_FADETOCOLOR_H_