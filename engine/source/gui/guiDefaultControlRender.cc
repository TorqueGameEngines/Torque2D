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

#include "graphics/dgl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiTypes.h"
#include "graphics/gColor.h"
#include "math/mRect.h"

void renderBorderedRect(RectI &bounds, GuiControlProfile *profile, GuiControlState state)
{
	ColorI fillColor = profile->getFillColor(state);
	renderBorderedRect(bounds, profile, state, fillColor);
}

void renderBorderedRect(RectI &bounds, GuiControlProfile *profile, GuiControlState state, const ColorI &fillColor)
{
	//Get the border profiles
	GuiBorderProfile *leftProfile = profile->getLeftBorder();
	GuiBorderProfile *rightProfile = profile->getRightBorder();
	GuiBorderProfile *topProfile = profile->getTopBorder();
	GuiBorderProfile *bottomProfile = profile->getBottomBorder();

	//Get the colors
	ColorI leftColor = (leftProfile) ? leftProfile->getBorderColor(state) : ColorI();
	ColorI rightColor = (rightProfile) ? rightProfile->getBorderColor(state) : ColorI();
	ColorI topColor = (topProfile) ? topProfile->getBorderColor(state) : ColorI();
	ColorI bottomColor = (bottomProfile) ? bottomProfile->getBorderColor(state) : ColorI();

	S32 leftSize = (leftProfile) ? leftProfile->getBorder(state) : 0;
	S32 rightSize = (rightProfile) ? rightProfile->getBorder(state) : 0;
	S32 topSize = (topProfile) ? topProfile->getBorder(state) : 0;
	S32 bottomSize = (bottomProfile) ? bottomProfile->getBorder(state) : 0;
	
	//Get the inner rect
	RectI innerRect = RectI(bounds.point.x + leftSize, bounds.point.y + topSize, (bounds.extent.x - leftSize) - rightSize, (bounds.extent.y - topSize) - bottomSize);

	//Draw the fill
	if(fillColor.alpha > 0)
	{
		S32 fillWidth = innerRect.extent.x + ((leftProfile && leftProfile->mUnderfill) ? leftSize : 0) + ((rightProfile && rightProfile->mUnderfill) ? rightSize : 0);
		S32 fillHeight = innerRect.extent.y + ((topProfile && topProfile->mUnderfill) ? topSize : 0) + ((bottomProfile && bottomProfile->mUnderfill) ? bottomSize : 0);
		RectI fillRect = RectI((leftProfile && leftProfile->mUnderfill) ? bounds.point.x : innerRect.point.x,
			(topProfile && topProfile->mUnderfill) ? bounds.point.y : innerRect.point.y, fillWidth, fillHeight);
		dglDrawRectFill(fillRect, fillColor);
	}
	
	//Draw the borders
	//Points for outer bounds starting top left and traveling counter-clockwise
	Point2I p1 = Point2I(bounds.point);
	Point2I p2 = Point2I(bounds.point.x, bounds.point.y + bounds.extent.y);
	Point2I p3 = Point2I(bounds.point.x + bounds.extent.x, bounds.point.y + bounds.extent.y);
	Point2I p4 = Point2I(bounds.point.x + bounds.extent.x, bounds.point.y);

	//Points for inner bounds starting top left and traveling counter-clockwise
	Point2I p5 = Point2I(innerRect.point);
	Point2I p6 = Point2I(innerRect.point.x, innerRect.point.y + innerRect.extent.y);
	Point2I p7 = Point2I(innerRect.point.x + innerRect.extent.x, innerRect.point.y + innerRect.extent.y);
	Point2I p8 = Point2I(innerRect.point.x + innerRect.extent.x, innerRect.point.y);

	if (leftSize > 0)
	{
		dglDrawQuadFill(p1, p2, p6, p5, leftColor);
	}
	if (rightSize > 0)
	{
		dglDrawQuadFill(p8, p7, p3, p4, rightColor);
	}
	if (topSize > 0)
	{
		dglDrawQuadFill(p1, p5, p8, p4, topColor);
	}
	if (bottomSize > 0)
	{
		dglDrawQuadFill(p6, p2, p3, p7, bottomColor);
	}
}

void renderBorderedCircle(Point2I &center, S32 radius, GuiControlProfile *profile, GuiControlState state)
{
	//Get the border profiles
	GuiBorderProfile *borderProfile = profile->mBorderDefault;

	//Get the colors
	ColorI fillColor = profile->getFillColor(state);
	ColorI borderColor = (profile->mBorderDefault) ? profile->mBorderDefault->getBorderColor(state) : ColorI();
	S32 borderSize = (profile->mBorderDefault) ? profile->mBorderDefault->getBorder(state) : 0;

	//Draw the fill
	S32 fillRadius = (profile->mBorderDefault && profile->mBorderDefault->mUnderfill) ? radius : radius - borderSize;
	dglDrawCircleFill(center, (F32)fillRadius, fillColor);

	//Draw the border
	dglDrawCircle(center, (F32)radius, borderColor, (F32)borderSize);
}

// DAW: Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   S32 NumBitmaps = 9;
   S32 startIndex = NumBitmaps * (baseMultiplier - 1);

   renderSizableBitmapBordersFilledIndex(bounds, startIndex, profile);
}


// DAW: Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 9;
   S32 BorderTopLeft =     startIndex;
   S32 BorderTop =         1 + BorderTopLeft;
   S32 BorderTopRight =    2 + BorderTopLeft;
   S32 BorderLeft =        3 + BorderTopLeft;
   S32 Fill =              4 + BorderTopLeft;
   S32 BorderRight =       5 + BorderTopLeft;
   S32 BorderBottomLeft =  6 + BorderTopLeft;
   S32 BorderBottom =      7 + BorderTopLeft;
   S32 BorderBottomRight = 8 + BorderTopLeft;

   dglClearBitmapModulation();
   if (profile->mBitmapArrayRects.size() >= (NumBitmaps + startIndex))
   {
	   RectI destRect;
	   RectI stretchRect;
	   RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

	   // Draw all corners first.

	   //top left border
	   dglDrawBitmapSR(profile->mTextureHandle, Point2I(bounds.point.x, bounds.point.y), mBitmapBounds[BorderTopLeft]);
	   //top right border
	   dglDrawBitmapSR(profile->mTextureHandle, Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x, bounds.point.y), mBitmapBounds[BorderTopRight]);

	   //bottom left border
	   dglDrawBitmapSR(profile->mTextureHandle, Point2I(bounds.point.x, bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y), mBitmapBounds[BorderBottomLeft]);
	   //bottom right border
	   dglDrawBitmapSR(profile->mTextureHandle, Point2I(
		   bounds.point.x + bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x,
		   bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomRight].extent.y),
		   mBitmapBounds[BorderBottomRight]);

	   // End drawing corners

	   // Begin drawing sides and top stretched borders

	   //start with top line stretch
	   destRect.point.x = bounds.point.x + mBitmapBounds[BorderTopLeft].extent.x;
	   destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x - mBitmapBounds[BorderTopLeft].extent.x;
	   destRect.extent.y = mBitmapBounds[BorderTop].extent.y;
	   destRect.point.y = bounds.point.y;
	   //stretch it
	   stretchRect = mBitmapBounds[BorderTop];
	   stretchRect.inset(1, 0);
	   //draw it
	   dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);
	   //bottom line stretch
	   destRect.point.x = bounds.point.x + mBitmapBounds[BorderBottomLeft].extent.x;
	   destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x - mBitmapBounds[BorderBottomLeft].extent.x;
	   destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
	   destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
	   //stretch it
	   stretchRect = mBitmapBounds[BorderBottom];
	   stretchRect.inset(1, 0);
	   //draw it
	   dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);
	   //left line stretch
	   destRect.point.x = bounds.point.x;
	   destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
	   destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopLeft].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
	   destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopLeft].extent.y;
	   //stretch it
	   stretchRect = mBitmapBounds[BorderLeft];
	   stretchRect.inset(0, 1);
	   //draw it
	   dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);
	   //right line stretch
	   destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x;
	   destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
	   destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopRight].extent.y - mBitmapBounds[BorderBottomRight].extent.y;
	   destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopRight].extent.y;
	   //stretch it
	   stretchRect = mBitmapBounds[BorderRight];
	   stretchRect.inset(0, 1);
	   //draw it
	   dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);
	   //fill stretch
	   destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
	   destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
	   destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTop].extent.y - mBitmapBounds[BorderBottom].extent.y;
	   destRect.point.y = bounds.point.y + mBitmapBounds[BorderTop].extent.y;
	   //stretch it
	   stretchRect = mBitmapBounds[Fill];
	   stretchRect.inset(1, 1);
	   //draw it
	   dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);

	   // End drawing sides and top stretched borders
   }
}



// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     NumBitmaps * baseMultiplier - NumBitmaps;
   S32 Fill =              1 + BorderLeft;
   S32 BorderRight =       2 + BorderLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (NumBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderLeft]);
      //right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x,bounds.point.y),mBitmapBounds[BorderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = mBitmapBounds[Fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing fill
   }
}

// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     startIndex;
   S32 Fill =              1 + startIndex;
   S32 BorderRight =       2 + startIndex;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + NumBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderLeft]);
      //right border
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x,bounds.point.y),mBitmapBounds[BorderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = mBitmapBounds[Fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);

      // End drawing fill
   }
}

// DAW: Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersStretchYFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   // DAW: Indices into the bitmap array
   S32 NumBitmaps = 3;
   S32 BorderLeft =     NumBitmaps * baseMultiplier - NumBitmaps;
   S32 Fill =              1 + BorderLeft;
   S32 BorderRight =       2 + BorderLeft;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (NumBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      dglDrawBitmapStretchSR(profile->mTextureHandle, RectI( bounds.point.x, bounds.point.y, mBitmapBounds[BorderLeft].extent.x, bounds.extent.y ), mBitmapBounds[BorderLeft] );
      //right border
      dglDrawBitmapStretchSR(profile->mTextureHandle, RectI(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x, bounds.point.y, mBitmapBounds[BorderRight].extent.x, bounds.extent.y ), mBitmapBounds[BorderRight] );

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle, destRect, stretchRect);

      // End drawing fill
   }
}

