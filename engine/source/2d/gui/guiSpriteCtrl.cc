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

#ifndef _GUISPRITECTRL_H_
#include "2d/gui/guiSpriteCtrl.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/guiDefaultControlRender.h"
#endif

#include "guiSpriteCtrl_ScriptBindings.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiSpriteCtrl);

//-----------------------------------------------------------------------------

GuiSpriteCtrl::GuiSpriteCtrl( void ) :
    mImageAssetId(StringTable->EmptyString),
    mFrame(0),
    mNamedImageFrameId(StringTable-> EmptyString),
    mAnimationAssetId(StringTable->EmptyString),
	mBitmapName(StringTable->EmptyString)
{
    // Set to self ticking.
    mSelfTick = true;
    
    // Default to static provider.
    mStaticProvider = true;

	mTileImage = false;
	mPositionOffset.set(0, 0);
	mImageColor = FluidColorI(255, 255, 255, 255);
	mImageSize.set(10, 10);
	mFullSize = true;
	mConstrainProportions = true;
	mSingleFrameBitmap = true;
}

//-----------------------------------------------------------------------------

GuiSpriteCtrl::~GuiSpriteCtrl()
{
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

	addGroup("GuiSpriteCtrl");
    addProtectedField("Image", TypeAssetId, Offset(mImageAssetId, GuiSpriteCtrl), &setImage, &defaultProtectedGetFn, &writeImage, "The image asset Id used for the image.");
    addProtectedField("Frame", TypeS32, Offset(mFrame, GuiSpriteCtrl), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "The image frame used for the image or bitmap.");
    addProtectedField("NamedFrame", TypeString, Offset(mNamedImageFrameId, GuiSpriteCtrl), &setNamedImageFrame, &defaultProtectedGetFn, &writeNamedImageFrame, "The named image frame used for the image.");
    addProtectedField("Animation", TypeAssetId, Offset(mAnimationAssetId, GuiSpriteCtrl), &setAnimation, &defaultProtectedGetFn, &writeAnimation, "The animation to use.");
	addProtectedField("Bitmap", TypeFilename, Offset(mBitmapName, GuiSpriteCtrl), &setBitmapName, &getBitmapName, &writeBitmapName, "The bitmap to use in absence of an asset.");
	addProtectedField("singleFrameBitmap", TypeBool, Offset(mSingleFrameBitmap, GuiSpriteCtrl), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeBitmapName, "If true, will assume there is a single frame when scanning the bitmap.");
	addField("tileImage", TypeBool, Offset(mTileImage, GuiSpriteCtrl));
	addField("positionOffset", TypePoint2I, Offset(mPositionOffset, GuiSpriteCtrl));
	addField("imageColor", TypeFluidColorI, Offset(mImageColor, GuiSpriteCtrl));
	addField("imageSize", TypePoint2I, Offset(mImageSize, GuiSpriteCtrl));
	addField("fullSize", TypeBool, Offset(mFullSize, GuiSpriteCtrl));
	addField("constrainProportions", TypeBool, Offset(mConstrainProportions, GuiSpriteCtrl));
	endGroup("GuiSpriteCtrl");
}

//------------------------------------------------------------------------------

void GuiSpriteCtrl::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to control.
    GuiSpriteCtrl* pGuiSpriteCtrl = static_cast<GuiSpriteCtrl*>(object);

    // Sanity!
    AssertFatal(pGuiSpriteCtrl != NULL, "GuiSpriteCtrl::copyTo() - Object is not the correct type.");

    // Copy asset fields.
    if ( mImageAssetId != StringTable->EmptyString )
    {
        if ( !isUsingNamedImageFrame() )
            pGuiSpriteCtrl->setImage( getImage(), getImageFrame() );
        else
            pGuiSpriteCtrl->setImage( getImage(), getNamedImageFrame() ); 
    }
    else if ( mAnimationAssetId != StringTable->EmptyString )
    {
        pGuiSpriteCtrl->setAnimation( getAnimation() );
    }
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::onWake()
{
    // Call parent.
    if (!Parent::onWake())
        return false;

	mActive = true;

    if ( mImageAssetId != StringTable->EmptyString )
    {
        if ( mNamedImageFrameId != StringTable->EmptyString)
        {
            // Set the image asset and named frame
            setImage( mImageAssetId, mNamedImageFrameId );
        }
        else
        {
            // Set image asset and numerical frame.
            setImage( mImageAssetId, mFrame );
        }
    }
    else if ( mAnimationAssetId != StringTable->EmptyString )
    {
        // Play animation asset.
        setAnimation( mAnimationAssetId );
    }
	else if (mBitmapName != StringTable->EmptyString)
	{
		setBitmap(mBitmapName);
	}
    else
    {
        // Not good, so warn.
        Con::warnf("GuiSpriteCtrl::onWake() - No Image, Animation, or bitmap defined.");
    }

    return true;
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::onSleep()
{
    // Clear assets.
    ImageFrameProvider::clearAssets();

	mBitmapTextureHandle = NULL;

    // Call parent.
    Parent::onSleep();
}

void GuiSpriteCtrl::setBitmap(const char *name)
{
	mBitmapName = StringTable->insert(name);
	if (*mBitmapName) {
		mBitmapTextureHandle = TextureHandle(mBitmapName, TextureHandle::BitmapTexture, true);
		mBitmapTextureHandle.setFilter(GL_LINEAR);
	}
	else
		mBitmapTextureHandle = NULL;
}

U32 GuiSpriteCtrl::constructBitmapArray()
{
	if (mSingleFrameBitmap)
		return 0;

	if (mBitmapArrayRects.size())
		return mBitmapArrayRects.size();

	GBitmap *bmp = mBitmapTextureHandle.getBitmap();

	// Make sure the texture exists.
	if (!bmp)
		return 0;

	//get the separator color
	ColorI sepColor;
	if (!bmp || !bmp->getColor(0, 0, sepColor))
	{
		Con::errorf("GuiSpriteCtrl::constructBitmapArray: Failed to create bitmap array from %s - couldn't ascertain seperator color!", mBitmapName);
		AssertFatal(false, avar("GuiSpriteCtrl::constructBitmapArray: Failed to create bitmap array from %s - couldn't ascertain seperator color!", mBitmapName));
		return 0;
	}

	//now loop through all the scroll pieces, and find the bounding rectangle for each piece in each state
	S32 curY = 0;

	// ascertain the height of this row...
	ColorI color;
	mBitmapArrayRects.clear();
	while (curY < (S32)bmp->getHeight())
	{
		// skip any sep colors
		bmp->getColor(0, curY, color);
		if (color == sepColor)
		{
			curY++;
			continue;
		}
		// ok, process left to right, grabbing bitmaps as we go...
		S32 curX = 0;
		while (curX < (S32)bmp->getWidth())
		{
			bmp->getColor(curX, curY, color);
			if (color == sepColor)
			{
				curX++;
				continue;
			}
			S32 startX = curX;
			while (curX < (S32)bmp->getWidth())
			{
				bmp->getColor(curX, curY, color);
				if (color == sepColor)
					break;
				curX++;
			}
			S32 stepY = curY;
			while (stepY < (S32)bmp->getHeight())
			{
				bmp->getColor(startX, stepY, color);
				if (color == sepColor)
					break;
				stepY++;
			}
			mBitmapArrayRects.push_back(RectI(startX, curY, curX - startX, stepY - curY));
		}
		// ok, now skip to the next separation color on column 0
		while (curY < (S32)bmp->getHeight())
		{
			bmp->getColor(0, curY, color);
			if (color == sepColor)
				break;
			curY++;
		}
	}
	return mBitmapArrayRects.size();
}

Point2I GuiSpriteCtrl::constrain(Point2I &point, bool grow)
{
	if (!mConstrainProportions)
	{
		return point;
	}

	F32 targetRatio = getAspectRatio();
	F32 currentRatio = (F32)(point.y / point.x);
	if (targetRatio > currentRatio) //Too short
	{
		return grow ? Point2I(point.x, mRound(point.x * targetRatio)) : Point2I(mRound(point.y / targetRatio), point.y);
	}
	else if (targetRatio < currentRatio) //Too tall
	{
		return grow ? Point2I(mRound(point.y / targetRatio), point.y) : Point2I(point.x, mRound(point.x * targetRatio));
	}
	return point;
}

Point2I GuiSpriteCtrl::constrainLockX(Point2I &point)
{
	if (!mConstrainProportions)
	{
		return point;
	}

	F32 targetRatio = getAspectRatio();
	return Point2I(point.x, mRound(point.x * targetRatio));
}

Point2I GuiSpriteCtrl::constrainLockY(Point2I &point)
{
	if (!mConstrainProportions)
	{
		return point;
	}

	F32 targetRatio = getAspectRatio();
	return Point2I(point.y / targetRatio, point.y);
}

F32 GuiSpriteCtrl::getAspectRatio()
{
	RectI src = getSourceRect(usesAsset() ? getProviderTexture() : mBitmapTextureHandle);
	if (src.isValidRect())
	{
		return ((F32)src.extent.y / (F32)src.extent.x);
	}
	return 1.0;
}

Point2I& GuiSpriteCtrl::applyAlignment(RectI &bounds, Point2I &size)
{
	Point2I offset = Point2I(0, 0);

	if (mProfile->mAlignment == GuiControlProfile::AlignmentType::RightAlign)
	{
		offset.x = bounds.extent.x - size.x;
	}
	else if (mProfile->mAlignment == GuiControlProfile::AlignmentType::CenterAlign)
	{
		S32 halfW = mRound(size.x / 2);
		offset.x = mRound(bounds.extent.x / 2) - halfW;
	}

	if (mProfile->mVAlignment == GuiControlProfile::VertAlignmentType::BottomVAlign)
	{
		offset.y = bounds.extent.y - size.y;
	}
	else if (mProfile->mVAlignment == GuiControlProfile::VertAlignmentType::MiddleVAlign)
	{
		S32 halfH = mRound(size.y / 2);
		offset.y = mRound(bounds.extent.y / 2) - halfH;
	}

	//Apply the image offset
	S32 maxX = bounds.extent.x - size.x;
	S32 maxY = bounds.extent.y - size.y;
	offset.x = mClamp(offset.x + mPositionOffset.x, 0, maxX);
	offset.y = mClamp(offset.y + mPositionOffset.y, 0, maxY);

	return offset;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImage( const char* pImageAssetId, const U32 frame )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

	//Capture the current tick state
	bool isTicking = isProcessingTicks();

    // Reset animation.
    if ( mAnimationAssetId != StringTable->EmptyString )
        mAnimationAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    if ( mImageAssetId != pImageAssetId )
        mImageAssetId = StringTable->insert(pImageAssetId);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setImage(pImageAssetId, frame) )
        return false;

	//Restore the tick state
	if (isProcessingTicks() != isTicking)
	{
		setProcessTicks(true);
	}

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImage( const char* pImageAssetId, const char* pNamedFrame )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

	//Capture the current tick state
	bool isTicking = isProcessingTicks();

    // Reset animation.
    if ( mAnimationAssetId != StringTable->EmptyString )
        mAnimationAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    if ( mImageAssetId != pImageAssetId )
        mImageAssetId = StringTable->insert(pImageAssetId);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setImage(pImageAssetId, pNamedFrame) )
        return false;

	//Restore the tick state
	if (isProcessingTicks() != isTicking)
	{
		setProcessTicks(true);
	}

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mImageAssetId == StringTable->EmptyString && mBitmapName == StringTable->EmptyString )
    {
        // Warn.
        Con::warnf("GuiSpriteCtrl::setImageFrame() - Cannot set frame without existing image asset or bitmap.");

        // Return Here.
        return false;
    }

    // Set frame.
    mFrame = frame;

    // Finish if not awake.
    if ( !isAwake() )
        return true;

	if(usesAsset())
	{
		// Call parent.
		if ( !ImageFrameProvider::setImageFrame(frame) )
			return false;
	}

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setNamedImageFrame( const char* pNamedFrame )
{
    // Check Existing Image.
    if ( mImageAssetId == StringTable->EmptyString )
    {
        // Warn.
        Con::warnf("GuiSpriteCtrl::setNamedImageFrame() - Cannot set named frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Set named frame.
    mNamedImageFrameId = StringTable->insert(pNamedFrame);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setNamedImageFrame(pNamedFrame) )
        return false;

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setAnimation( const char* pAnimationAssetId )
{
    // Sanity!
    AssertFatal( pAnimationAssetId != NULL, "Cannot use a NULL asset Id." );

    // Reset the image asset Id.
    if ( mImageAssetId != StringTable->EmptyString )
        mImageAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    if ( mAnimationAssetId != pAnimationAssetId )
        mAnimationAssetId = StringTable->insert(pAnimationAssetId);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Play animation asset if it's valid.
    if ( mAnimationAssetId != StringTable->EmptyString )
        ImageFrameProvider::setAnimation( mAnimationAssetId );

    return true;
}

void GuiSpriteCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	RectI ctrlRect = applyMargins(offset, mBounds.extent, NormalState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, NormalState);

	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

	if (contentRect.isValidRect() && (usesAsset() || mBitmapName != StringTable->EmptyString))
	{
		RectI oldClip = dglGetClipRect();
		RectI clipRect = contentRect;
		if (clipRect.intersect(oldClip))
		{
			dglSetClipRect(clipRect);
			dglSetBitmapModulation(ColorF(mImageColor));

			Point2I offset = Point2I(0, 0);
			Point2I size = constrain(mImageSize);

			if (mTileImage) //Tile the image
			{
				offset = mPositionOffset;
				offset.x = (mPositionOffset.x % size.x);
				if (offset.x > 0)
				{
					offset.x -= size.x;
				}
				offset.y = (mPositionOffset.y % size.y);
				if (offset.y > 0)
				{
					offset.y -= size.y;
				}

				RenderTiledImage(contentRect, offset, size);
			}
			else if (mFullSize) //Fill with the image
			{
				size = constrain(contentRect.extent, false);
				if (mConstrainProportions)
				{
					offset = applyAlignment(contentRect, size);
				}
				RenderImage(contentRect, offset, size);
			}
			else //Position the image by profile alignment
			{
				size = constrain(mImageSize);
				if (size.x > contentRect.extent.x)
				{
					size.x = contentRect.extent.x;
					size = constrainLockX(size);
				}
				if (size.y > contentRect.extent.y)
				{
					size.y = contentRect.extent.y;
					size = constrainLockY(size);
				}
				offset = applyAlignment(contentRect, size);

				RenderImage(contentRect, offset, size);
			}
			dglClearBitmapModulation();
			dglSetClipRect(oldClip);

			//Render the childen
			renderChildControls(offset, contentRect, updateRect);
		}

		//One more thing...
		if (usesAsset() && !isStaticFrameProvider() && !isAnimationFinished() && !isAnimationPaused() && !isProcessingTicks())
		{
			setProcessTicks(true);
		}
	}
}

void GuiSpriteCtrl::RenderImage(RectI &bounds, Point2I &offset, Point2I &size)
{
	TextureObject* texture = usesAsset() ?  (TextureObject *) getProviderTexture() : (TextureObject *)mBitmapTextureHandle;

	RectI srcRegion = getSourceRect(texture);
	if (!srcRegion.isValidRect())
	{
		return;
	}

	RectI dstRegion = RectI(bounds.point + offset, size);
	dglDrawBitmapStretchSR(texture, dstRegion, srcRegion, false);
}

void GuiSpriteCtrl::RenderTiledImage(RectI &bounds, Point2I &start, Point2I &size)
{
	TextureObject* texture = usesAsset() ? (TextureObject *)getProviderTexture() : (TextureObject *)mBitmapTextureHandle;

	RectI srcRegion = getSourceRect(texture);
	if (!srcRegion.isValidRect())
	{
		return;
	}

	RectI dstRegion;
	float xdone = ((float)(bounds.extent.x - start.x) / (float)size.x) + 1;
	float ydone = ((float)(bounds.extent.y - start.y) / (float)size.y) + 1;

	// We manually draw each repeat because non power of two textures will 
	// not tile correctly when rendered with dglDrawBitmapTile().
	for (int y = 0; y < ydone; ++y)
	{
		for (int x = 0; x < xdone; ++x)
		{
			dstRegion.set((size.x * x) + start.x, (size.y * y) + start.y, size.x, size.y);
			dglDrawBitmapStretchSR(texture, dstRegion, srcRegion, false);
		}
	}
}

RectI GuiSpriteCtrl::getSourceRect(TextureObject* texture)
{
	RectI srcRegion = RectI(0, 0, 0, 0);
	bool hasAsset = usesAsset();
	if (hasAsset)
	{
		const ImageAsset::FrameArea& frameArea = getProviderImageFrameArea();
		srcRegion = RectI(frameArea.mPixelArea.mPixelOffset, Point2I(frameArea.mPixelArea.mPixelWidth, frameArea.mPixelArea.mPixelHeight));
	}
	else if (!hasAsset && mSingleFrameBitmap)
	{
		srcRegion = RectI(0, 0, texture->getBitmapWidth(), texture->getBitmapHeight());
	}
	else if (!hasAsset && constructBitmapArray() > mFrame)
	{
		RectI* frameRect = mBitmapArrayRects.address();
		srcRegion = frameRect[mFrame];
	}
	return srcRegion;
}

void GuiSpriteCtrl::moveTo(S32 x, S32 y, S32 time, EasingFunction ease)
{
	mStartOffset.set(mPositionOffset.x, mPositionOffset.y);
	mTargetOffset.set(x, y);
	mFluidMoveTo.setAnimationLength(getMax(time, 0));
	mFluidMoveTo.setEasingFunction(ease);

	mFluidMoveTo.startFluidAnimation();
	setProcessTicks(true);
}

void GuiSpriteCtrl::growTo(S32 x, S32 y, S32 time, EasingFunction ease)
{
	mStartSize.set(mImageSize.x, mImageSize.y);
	mTargetSize.set(x, y);
	mFluidGrowTo.setAnimationLength(getMax(time, 0));
	mFluidGrowTo.setEasingFunction(ease);

	mFluidGrowTo.startFluidAnimation();
	setProcessTicks(true);
}

void GuiSpriteCtrl::fadeTo(const ColorI &color, S32 time, EasingFunction ease)
{
	mImageColor.setAnimationLength(getMax(time, 0));
	mImageColor.setEasingFunction(ease);

	mImageColor.startFluidAnimation(color);
	setProcessTicks(true);
}

void GuiSpriteCtrl::processTick()
{
	bool shouldWeContinue = false;

	//Animating
	shouldWeContinue |= update(Tickable::smTickSec);
	shouldWeContinue |= animateMoveTo();
	shouldWeContinue |= animateGrowTo();
	shouldWeContinue |= animateFadeTo();

	if (!shouldWeContinue)
	{
		setProcessTicks(false);
	}
}

bool GuiSpriteCtrl::update(const F32 elapsedTime)
{
	//Note: although this overrides ImageFrameProviderCore::update, the return value has a different meaning.
	//True means the control should continue ticking. False means the animation now longer needs tickets.

	// Static provider or bitmap?
	if (!usesAsset() || isStaticFrameProvider())
	{
		return false;
	}

	// Finish if the animation has finished or paused.
	if (isAnimationFinished() || isAnimationPaused())
		return false;

	// Update the animation.
	if (updateAnimation(elapsedTime))
	{
		setUpdate();
	}

	// Finish if the animation has NOT finished.
	if (!isAnimationFinished())
		return true;

	// Perform callback.
	onAnimationEnd();

	//We no longer need ticks.
	return false;
}

bool GuiSpriteCtrl::animateMoveTo()
{
	if (!mFluidMoveTo.isAnimating())
	{
		return false;
	}

	setUpdate();
	F32 progress = mFluidMoveTo.getProgress(32.0f);
	mPositionOffset.set(
		mFluidMoveTo.processValue(progress, mStartOffset.x, mTargetOffset.x),
		mFluidMoveTo.processValue(progress, mStartOffset.y, mTargetOffset.y));

	if (!mFluidMoveTo.isAnimating() && isMethod("onMoveToComplete"))
	{
		Con::executef(this, 1, "onMoveToComplete");
	}

	return mFluidMoveTo.isAnimating();
}

bool GuiSpriteCtrl::animateGrowTo()
{
	if (!mFluidGrowTo.isAnimating())
	{
		return false;
	}

	setUpdate();
	F32 progress = mFluidGrowTo.getProgress(32.0f);
	mImageSize.set(
		mFluidGrowTo.processValue(progress, mStartSize.x, mTargetSize.x),
		mFluidGrowTo.processValue(progress, mStartSize.y, mTargetSize.y));

	if (!mFluidGrowTo.isAnimating() && isMethod("onGrowToComplete"))
	{
		Con::executef(this, 1, "onGrowToComplete");
	}

	return mFluidGrowTo.isAnimating();
}

bool GuiSpriteCtrl::animateFadeTo()
{
	if (!mImageColor.isAnimating())
	{
		return false;
	}

	setUpdate();
	mImageColor.processTick();

	if (!mImageColor.isAnimating() && isMethod("onFadeToComplete"))
	{
		Con::executef(this, 1, "onFadeToComplete");
	}

	return mImageColor.isAnimating();
}

//------------------------------------------------------------------------------

void GuiSpriteCtrl::onAnimationEnd( void )
{
	// We've arrived at the end - pause it
	ImageFrameProvider::pauseAnimation(true);

	// Send a callback
	if(isMethod("onAnimationEnd"))
		Con::executef(this, 3, "onAnimationEnd", ImageFrameProvider::getCurrentAnimationAssetId());
}
