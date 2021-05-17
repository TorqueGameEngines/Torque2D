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

// Renders a rect in one of three ways: ImageAsset, bitmap, or default render. ImageAsset and bitmap can use
// nine frames per state or one frame per state. The default render is used as a fall back if neither are present.
void renderUniversalRect(RectI &bounds, GuiControlProfile *profile, GuiControlState state, const ColorI &fillColor, const bool bUseFillColor)
{
	if (profile == NULL)
	{
		return;
	}

	U8 stateIndex = state;

	//prepare
	S32 bitmapFrameCount = 0;
	S32 imageFrameCount = 0;
	if (profile->mImageAsset != NULL && profile->mImageAsset->isAssetValid())
	{
		imageFrameCount = profile->mImageAsset->getFrameCount();
	}
	else if (profile->mBitmapName != NULL)
	{
		bitmapFrameCount = profile->constructBitmapArray();
	}

	if (imageFrameCount >= (9 * (stateIndex + 1)))
	{
		renderSizableBorderedImageAsset(bounds, stateIndex, profile->mImageAsset, imageFrameCount);
	}
	else if (imageFrameCount > stateIndex && imageFrameCount < 9)
	{
		renderStretchedImageAsset(bounds, stateIndex, profile);
	}
	else if (bitmapFrameCount >= (9 * (stateIndex + 1)))
	{
		renderSizableBorderedBitmap(bounds, stateIndex, profile->mTextureHandle, profile->mBitmapArrayRects.address(), bitmapFrameCount);
	}
	else if (bitmapFrameCount > stateIndex && bitmapFrameCount < 9)
	{
		renderStretchedBitmap(bounds, stateIndex, profile);
	}
	else
	{
		if (bUseFillColor)
		{
			renderBorderedRect(bounds, profile, state, fillColor);
		}
		else
		{
			renderBorderedRect(bounds, profile, state);
		}
	}
}

void renderBorderedRect(RectI &bounds, GuiControlProfile *profile, GuiControlState state)
{
	if(profile)
	{
		ColorI fillColor = profile->getFillColor(state);
		renderBorderedRect(bounds, profile, state, fillColor);
	}
}

void renderBorderedRect(RectI &bounds, GuiControlProfile *profile, GuiControlState state, const ColorI &fillColor)
{
	if (!profile)
	{
		return;
	}

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

	if (state > 3)
	{
		RectI checkRect = RectI(bounds);
		checkRect.inset(3, 3);
		if (checkRect.isValidRect())
		{
			renderBorderedRect(checkRect, profile, GuiControlState::SelectedState);
		}
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

	if (state > 3 && radius >= 8)
	{
		dglDrawCircleFill(center, radius - 6, profile->getFillColor(GuiControlState::SelectedState));
	}
}

// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBorderedImageAsset(RectI &bounds, U8 frame, ImageAsset *imageAsset, S32 frameCount)
{
	S32 NumFrames = 9;
	S32 i = NumFrames * frame;

	if (frameCount >= (NumFrames + i))
	{
		const ImageAsset::FrameArea::PixelArea& pixelArea1 = imageAsset->getImageFrameArea((U32)i).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea2 = imageAsset->getImageFrameArea((U32)i+1).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea3 = imageAsset->getImageFrameArea((U32)i+2).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea4 = imageAsset->getImageFrameArea((U32)i+3).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea5 = imageAsset->getImageFrameArea((U32)i+4).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea6 = imageAsset->getImageFrameArea((U32)i+5).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea7 = imageAsset->getImageFrameArea((U32)i+6).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea8 = imageAsset->getImageFrameArea((U32)i+7).mPixelArea;
		const ImageAsset::FrameArea::PixelArea& pixelArea9 = imageAsset->getImageFrameArea((U32)i+8).mPixelArea;

		renderSizableBorderedTexture(bounds, imageAsset->getImageTexture(), 
			pixelArea1.toRectI(),
			RectI(pixelArea2.mPixelOffset, Point2I(pixelArea2.mPixelWidth, pixelArea2.mPixelHeight)),
			RectI(pixelArea3.mPixelOffset, Point2I(pixelArea3.mPixelWidth, pixelArea3.mPixelHeight)), 
			RectI(pixelArea4.mPixelOffset, Point2I(pixelArea4.mPixelWidth, pixelArea4.mPixelHeight)), 
			RectI(pixelArea5.mPixelOffset, Point2I(pixelArea5.mPixelWidth, pixelArea5.mPixelHeight)), 
			RectI(pixelArea6.mPixelOffset, Point2I(pixelArea6.mPixelWidth, pixelArea6.mPixelHeight)), 
			RectI(pixelArea7.mPixelOffset, Point2I(pixelArea7.mPixelWidth, pixelArea7.mPixelHeight)), 
			RectI(pixelArea8.mPixelOffset, Point2I(pixelArea8.mPixelWidth, pixelArea8.mPixelHeight)), 
			RectI(pixelArea9.mPixelOffset, Point2I(pixelArea9.mPixelWidth, pixelArea9.mPixelHeight)));
	}
}

void renderSizableBorderedBitmap(RectI &bounds, U8 frame, TextureHandle &texture, RectI *bitmapBounds, S32 frameCount)
{
   S32 NumFrames = 9;
   S32 i = NumFrames * frame;

   if (frameCount >= (NumFrames + i))
   {
	   renderSizableBorderedTexture(bounds, texture, bitmapBounds[i], bitmapBounds[i+1], bitmapBounds[i+2], bitmapBounds[i+3], bitmapBounds[i+4], bitmapBounds[i+5], bitmapBounds[i+6], bitmapBounds[i+7], bitmapBounds[i+8]);
   }
}

void renderSizableBorderedTexture(RectI &bounds, TextureHandle &texture, RectI &TopLeft, RectI &Top, RectI &TopRight, RectI &Left, RectI &Fill, RectI &Right, RectI &BottomLeft, RectI &Bottom, RectI &BottomRight)
{
	dglClearBitmapModulation();
	RectI destRect;
	RectI stretchRect;

	//top corners
	dglDrawBitmapSR(texture, bounds.point, TopLeft);
	dglDrawBitmapSR(texture, Point2I(bounds.point.x + bounds.extent.x - TopRight.extent.x, bounds.point.y), TopRight);

	//bottom corners
	dglDrawBitmapSR(texture, Point2I(bounds.point.x, bounds.point.y + bounds.extent.y - BottomLeft.extent.y), BottomLeft);
	dglDrawBitmapSR(texture, Point2I(bounds.point.x + bounds.extent.x - BottomRight.extent.x, bounds.point.y + bounds.extent.y - BottomRight.extent.y), BottomRight);

	//top line stretch
	destRect.point.x = bounds.point.x + TopLeft.extent.x;
	destRect.extent.x = bounds.extent.x - TopRight.extent.x - TopLeft.extent.x;
	destRect.extent.y = Top.extent.y;
	destRect.point.y = bounds.point.y;
	stretchRect = Top;
	stretchRect.inset(1, 0);
	dglDrawBitmapStretchSR(texture, destRect, stretchRect);

	//bottom line stretch
	destRect.point.x = bounds.point.x + BottomLeft.extent.x;
	destRect.extent.x = bounds.extent.x - BottomRight.extent.x - BottomLeft.extent.x;
	destRect.extent.y = Bottom.extent.y;
	destRect.point.y = bounds.point.y + bounds.extent.y - Bottom.extent.y;
	stretchRect = Bottom;
	stretchRect.inset(1, 0);
	dglDrawBitmapStretchSR(texture, destRect, stretchRect);

	//left line stretch
	destRect.point.x = bounds.point.x;
	destRect.extent.x = Left.extent.x;
	destRect.extent.y = bounds.extent.y - TopLeft.extent.y - BottomLeft.extent.y;
	destRect.point.y = bounds.point.y + TopLeft.extent.y;
	stretchRect = Left;
	stretchRect.inset(0, 1);
	dglDrawBitmapStretchSR(texture, destRect, stretchRect);

	//right line stretch
	destRect.point.x = bounds.point.x + bounds.extent.x - Right.extent.x;
	destRect.extent.x = Right.extent.x;
	destRect.extent.y = bounds.extent.y - TopRight.extent.y - BottomRight.extent.y;
	destRect.point.y = bounds.point.y + TopRight.extent.y;
	stretchRect = Right;
	stretchRect.inset(0, 1);
	dglDrawBitmapStretchSR(texture, destRect, stretchRect);

	//fill stretch
	destRect.point.x = bounds.point.x + Left.extent.x;
	destRect.extent.x = (bounds.extent.x) - Left.extent.x - Right.extent.x;
	destRect.extent.y = bounds.extent.y - Top.extent.y - Bottom.extent.y;
	destRect.point.y = bounds.point.y + Top.extent.y;
	stretchRect = Fill;
	stretchRect.inset(1, 1);
	dglDrawBitmapStretchSR(texture, destRect, stretchRect);
}

// Renders out the fixed bitmap borders based on a multiplier into the bitmap array.
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
	S32 NumBitmaps = 3;
	S32 startIndex = NumBitmaps * (baseMultiplier - 1);
   S32 BorderLeft =     startIndex;
   S32 Fill =              1 + startIndex;
   S32 BorderRight =       2 + startIndex;

   dglClearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + NumBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      //draw left endcap
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderLeft]);
      //draw right endcap
      dglDrawBitmapSR(profile->mTextureHandle,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x,bounds.point.y),mBitmapBounds[BorderRight]);

      //draw stretched content
      destRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x;
      destRect.extent.y = bounds.extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[Fill];
      stretchRect.inset(1,0);
      //draw it
      dglDrawBitmapStretchSR(profile->mTextureHandle,destRect,stretchRect);
   }
}

// Renders out a stretched bitmap.
void renderStretchedBitmap(RectI &bounds, U8 frame, GuiControlProfile *profile)
{
	dglClearBitmapModulation();
	if (profile->mBitmapArrayRects.size() > frame)
	{
		RectI* mBitmapBounds = profile->mBitmapArrayRects.address();
		dglDrawBitmapStretchSR(profile->mTextureHandle, bounds, mBitmapBounds[frame]);
	}
}

// Renders out a stretched image asset.
void renderStretchedImageAsset(RectI &bounds, U8 frame, GuiControlProfile *profile)
{
	dglClearBitmapModulation();
	ImageAsset *imageAsset = profile->mImageAsset;

	if (imageAsset != NULL && imageAsset->isAssetValid() && imageAsset->getFrameCount() > frame)
	{
		const ImageAsset::FrameArea::PixelArea& pixelArea = imageAsset->getImageFrameArea(frame).mPixelArea;
		RectI srcRect(pixelArea.mPixelOffset, Point2I(pixelArea.mPixelWidth, pixelArea.mPixelHeight));

		// Render image.
		dglDrawBitmapStretchSR(imageAsset->getImageTexture(), bounds, srcRect);
	}
}

//Renders a color bullet at or one pixel smaller than maxSize.
//It shrinks the given box until it is less than or equal to the 
//maxSize in the x direction.
void renderColorBullet(RectI &bounds, ColorI &color, S32 maxSize, bool useCircle)
{
	if (bounds.extent.x > maxSize)
	{
		S32 delta = mCeil((bounds.extent.x - maxSize) / 2);
		bounds.inset(delta, delta);
	}
	if (!bounds.isValidRect())
	{
		return;
	}

	if(!useCircle)
	{
		dglDrawRectFill(bounds, ColorI(0, 0, 0, 100));
		bounds.inset(1, 1);
		if (!bounds.isValidRect())
		{
			return;
		}
		dglDrawRectFill(bounds, color);
	}
	else
	{
		Point2I center = Point2I(bounds.point.x + (bounds.extent.x / 2), bounds.point.y + (bounds.extent.y / 2));
		F32 radius = (F32)(bounds.extent.x / 2);
		dglDrawCircleFill(center, radius, ColorI(0, 0, 0, 100));
		dglDrawCircleFill(center, radius-1, color);
	}
}

void renderTriangleIcon(RectI &bounds, ColorI &color, GuiDirection pointsToward, S32 maxSize)
{
	if (bounds.extent.x > maxSize)
	{
		S32 delta = mCeil((bounds.extent.x - maxSize) / 2);
		bounds.inset(delta, delta);
	}
	if (!bounds.isValidRect())
	{
		return;
	}

	if (pointsToward == GuiDirection::Up)
	{
		dglDrawTriangleFill(
			Point2I(bounds.point.x, bounds.point.y + bounds.extent.y),
			Point2I(bounds.point.x + bounds.extent.x, bounds.point.y + bounds.extent.y),
			Point2I(bounds.point.x + (bounds.extent.x / 2), bounds.point.y),
			color
		);
	}
	else if (pointsToward == GuiDirection::Down)
	{
		dglDrawTriangleFill(
			bounds.point,
			Point2I(bounds.point.x + (bounds.extent.x / 2), bounds.point.y + bounds.extent.y),
			Point2I(bounds.point.x + bounds.extent.x, bounds.point.y),
			color
		);
	}
	else if (pointsToward == GuiDirection::Right)
	{
		dglDrawTriangleFill(
			bounds.point,
			Point2I(bounds.point.x, bounds.point.y + bounds.extent.y),
			Point2I(bounds.point.x + bounds.extent.x, bounds.point.y + (bounds.extent.y / 2)),
			color
		);
	}
	else if (pointsToward == GuiDirection::Left)
	{
		dglDrawTriangleFill(
			Point2I(bounds.point.x + bounds.extent.x, bounds.point.y),
			Point2I(bounds.point.x, bounds.point.y + (bounds.extent.y / 2)),
			Point2I(bounds.point.x + bounds.extent.x, bounds.point.y + bounds.extent.y),
			color
		);
	}
}