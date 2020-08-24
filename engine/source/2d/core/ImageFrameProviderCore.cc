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

#include "2d/core/ImageFrameProviderCore.h"

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

#ifndef _RENDER_PROXY_H_
#include "2d/core/RenderProxy.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

ImageFrameProviderCore::ImageFrameProviderCore() : mpImageAsset(NULL), mpAnimationAsset(NULL)
{
}

//-----------------------------------------------------------------------------

ImageFrameProviderCore::~ImageFrameProviderCore()
{
    // Reset the state.
    resetState();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::allocateAssets( AssetPtr<ImageAsset>* pImageAssetPtr, AssetPtr<AnimationAsset>* pAnimationAssetPtr )
{
    // Sanity!
    AssertFatal( mpImageAsset == NULL && mpAnimationAsset == NULL, "ImageFrameProviderCore: Assets already allocated." );
    AssertFatal( pImageAssetPtr != NULL, "ImageFrameProviderCore: Image-Asset pointer cannot be NULL." );
    AssertFatal( pAnimationAssetPtr != NULL, "ImageFrameProviderCore: Animation-Asset pointer cannot be NULL." );

    // Reset the state.
    resetState();

    // Set asset pointers.
    mpImageAsset = pImageAssetPtr;
    mpAnimationAsset = pAnimationAssetPtr;

    // Set static/animation frame provider.
    mStaticProvider = mpImageAsset->notNull();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::resetState( void )
{
    mSelfTick = false;

    mCurrentFrameIndex = 0;
    mLastFrameIndex = 0;
    mMaxFrameIndex = 0;
    mCurrentTime = 0.0f;
    mPausedTime = 0.0f;
    mAnimationTimeScale = 1.0f;
    mTotalIntegrationTime = 0.0f;
    mFrameIntegrationTime = 0.0f;
    mAnimationPaused = false;
    mAnimationFinished = true;

    clearAssets();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::copyTo( ImageFrameProviderCore* pImageFrameProviderCore ) const
{
    // Sanity!
    AssertFatal(pImageFrameProviderCore != NULL, "ImageFrameProviderCore::copyTo - Copy object cannot be NULL.");

    // Set self ticking.
    pImageFrameProviderCore->mSelfTick = mSelfTick;

    // Static provider?
    if ( isStaticFrameProvider() )
    {
        // Yes, so use the image/frame if we have an asset.
        if ( mpImageAsset->notNull() )
        {
            // Named image frame?
            if ( !isUsingNamedImageFrame() )
                pImageFrameProviderCore->setImage( getImage(), getImageFrame() );
            else
                pImageFrameProviderCore->setImage( getImage(), getNamedImageFrame() );
        }
    }
    else if ( mpAnimationAsset->notNull() )
    {
        // No, so use current animation if we have an asset.
        if ( mpAnimationAsset->notNull() )
            pImageFrameProviderCore->setAnimation(getAnimation() );
    }
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::update( const F32 elapsedTime )
{
    // Static provider?
    if ( isStaticFrameProvider() )
    {
        // Yes, so turn-off tick processing.
        setProcessTicks( false );

        return false;
    }

    // Finish if the animation has finished.
    if ( isAnimationFinished() )
        return false;

    // Finish if animation is paused.
    if ( isAnimationPaused() )
        return true;

    // Update the animation.
    updateAnimation( Tickable::smTickSec );

    // Finish if the animation has NOT finished.
    if ( !isAnimationFinished() )
        return false;

    // Turn-off tick processing.
    setProcessTicks( false );

    // Perform callback.
    onAnimationEnd();

    // Flag animation as just finished.
    return true;
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::processTick( void )
{
    // Update using tick period.
    update( Tickable::smTickSec );
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::validRender( void ) const
{
    // Are we in static mode?
    if ( isStaticFrameProvider() )
    {
        // Yes, so we must have an image asset and the frame must be in bounds.
        if (!isUsingNamedImageFrame())
            return mpImageAsset->notNull() && ( getImageFrame() < (*mpImageAsset)->getFrameCount() );
        else
            return mpImageAsset->notNull() && getNamedImageFrame() != StringTable->EmptyString && ( (*mpImageAsset)->containsFrame(getNamedImageFrame()) );
    }

    // No, so if the animation must be valid.
    return isAnimationValid();
}

//------------------------------------------------------------------------------

const ImageAsset::FrameArea& ImageFrameProviderCore::getProviderImageFrameArea( void ) const
{
    // If this does not have a valid render state, return a bad frame
    if (!validRender())
        return BadFrameArea;
    
    // If it is a static frame and it's not using named frames, get the image area based mImageFrame
    // If it is a static frame and it's using named frames, get the image area based on mImageNameFrame
    // Otherwise, get the current animation frame
    if (isStaticFrameProvider())
        return !isUsingNamedImageFrame() ? (*mpImageAsset)->getImageFrameArea(mImageFrame) : (*mpImageAsset)->getImageFrameArea(mNamedImageFrame);
    else
        return !(*mpAnimationAsset)->getNamedCellsMode() ? (*mpAnimationAsset)->getImage()->getImageFrameArea(getCurrentAnimationFrame()) :
                                                           (*mpAnimationAsset)->getImage()->getImageFrameArea(getCurrentNamedAnimationFrame());
    
    // If we got here for some reason, that's bad. So return a bad area frame
    return BadFrameArea;
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::render(
	const bool flipX,
	const bool flipY,
	const Vector2& vertexPos0,
	const Vector2& vertexPos1,
	const Vector2& vertexPos2,
	const Vector2& vertexPos3,
	BatchRender* pBatchRenderer) const
{
	// Finish if we can't render.
	if (!validRender())
		return;

	// Fetch texel area.
	ImageAsset::FrameArea::TexelArea texelArea = getProviderImageFrameArea().mTexelArea;

	// Flip texture coordinates appropriately.
	texelArea.setFlip(flipX, flipY);

	// Fetch lower/upper texture coordinates.
	const Vector2& texLower = texelArea.mTexelLower;
	const Vector2& texUpper = texelArea.mTexelUpper;

	// Submit batched quad.
	pBatchRenderer->SubmitQuad(
		vertexPos0,
		vertexPos1,
		vertexPos2,
		vertexPos3,
		Vector2(texLower.x, texUpper.y),
		Vector2(texUpper.x, texUpper.y),
		Vector2(texUpper.x, texLower.y),
		Vector2(texLower.x, texLower.y),
		getProviderTexture());
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::render(
	const bool flipX,
	const bool flipY,
	const Vector2& vertexPos0,
	const Vector2& vertexPos1,
	const Vector2& vertexPos2,
	const Vector2& vertexPos3,
	const Vector2& uvPos0,
	const Vector2& uvPos1,
	const Vector2& uvPos2,
	const Vector2& uvPos3,
	BatchRender* pBatchRenderer) const
{
	// Finish if we can't render.
	if (!validRender())
		return;

	// Submit batched quad.
	pBatchRenderer->SubmitQuad(
		vertexPos0,
		vertexPos1,
		vertexPos2,
		vertexPos3,
		uvPos0,
		uvPos1,
		uvPos2,
		uvPos3,
		getProviderTexture());
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::render(
		const U32 vertexCount,
		const Vector2 *vertexArray,
		const Vector2 *textureArray,
		const ColorF *colorArray,
		BatchRender* pBatchRenderer) const
{
	// Finish if we can't render.
	if (!validRender() || !vertexCount)
		return;

	// Submit mesh list
	pBatchRenderer->SubmitTriangles(
		vertexCount,
		vertexArray,
		textureArray,
		colorArray,
		getProviderTexture());
}

//------------------------------------------------------------------------------
void ImageFrameProviderCore::renderGui( GuiControl& owner, Point2I offset, const RectI &updateRect ) const
{
    // Validate frame provider.
    if ( ( isStaticFrameProvider() && (mpImageAsset->isNull() || mImageFrame >= (*mpImageAsset)->getFrameCount()) ) ||
        ( !isStaticFrameProvider() && (mpAnimationAsset->isNull() ) ) )
    {
        // Invalid so fetch the 'cannot render' proxy.
        RenderProxy* pNoImageRenderProxy = Sim::findObject<RenderProxy>( CANNOT_RENDER_PROXY_NAME );

        // Check that the render proxy can render.
        if ( pNoImageRenderProxy != NULL && pNoImageRenderProxy->validRender() )
        {
            // Render using render-proxy.
            pNoImageRenderProxy->renderGui( owner, offset, updateRect );
        }

        // Update control.
        owner.setUpdate();
    }
    else
    {
        // Valid, so calculate source region.
        const ImageAsset::FrameArea& frameArea = getProviderImageFrameArea();
        RectI sourceRegion( frameArea.mPixelArea.mPixelOffset, Point2I(frameArea.mPixelArea.mPixelWidth, frameArea.mPixelArea.mPixelHeight) );

        // Calculate destination region.
        RectI destinationRegion(offset, owner.mBounds.extent);

        // Render image.
        dglSetBitmapModulation( owner.mProfile->mFillColor );
        dglDrawBitmapStretchSR( getProviderTexture(), destinationRegion, sourceRegion );
        dglClearBitmapModulation();
    }

    // Render child controls.
    owner.renderChildControls(offset, updateRect, updateRect);
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImage( const char* pImageAssetId, const U32 frame )
{
    // Finish if invalid image asset.
    if ( pImageAssetId == NULL )
        return false;

    // Set asset.
    mpImageAsset->setAssetId( pImageAssetId );

    // Set the image frame if the image asset was set.
    if ( mpImageAsset->notNull() )
        setImageFrame( frame );

    // Set as static provider.
    mStaticProvider = true;

    // Turn-off tick processing.
    setProcessTicks( false );

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImage( const char* pImageAssetId, const char* pNamedFrame )
{
    // Finish if invalid image asset.
    if ( pImageAssetId == NULL )
        return false;
    
    // Set asset.
    mpImageAsset->setAssetId( pImageAssetId );
    
    // Set the image frame if the image asset was set.
    if ( mpImageAsset->notNull() )
        setNamedImageFrame( pNamedFrame );
    
    // Set as static provider.
    mStaticProvider = true;
    
    // Turn-off tick processing.
    setProcessTicks( false );
    
    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mpImageAsset->isNull() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setImageFrame() - Cannot set Frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= (*mpImageAsset)->getFrameCount() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setImageFrame() - Invalid Frame #%d for asset Id '%s'.", frame, mpImageAsset->getAssetId());
        // Return Here.
        return false;
    }

    // Set Frame.
    mImageFrame = frame;

    // Using a numerical frame index.
    mUsingNamedFrame = false;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::setNamedImageFrame(const char* pNamedFrame)
{
    // Check Existing Image.
    if ( mpImageAsset->isNull() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setNamedImageFrame() - Cannot set Frame without existing asset Id.");
        
        // Return Here.
        return false;
    }
    
    // Check Frame Validity.
    if ( pNamedFrame == StringTable->EmptyString )
    {
        // Warn.
        Con::warnf( "ImageFrameProviderCore::setNamedImageFrame() - Invalid Frame %s for asset Id '%s'.", pNamedFrame, mpImageAsset->getAssetId() );
        // Return Here.
        return false;
    }
    
    // Set Frame.
    mNamedImageFrame = StringTable->insert(pNamedFrame);
    mUsingNamedFrame = true;
    
    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

const U32 ImageFrameProviderCore::getCurrentAnimationFrame( void ) const
{
    // Sanity!
    AssertFatal( mpAnimationAsset->notNull(), "Animation controller requested current image frame but no animation asset assigned." );

    // Fetch validated frames.
    const Vector<S32>& validatedFrames = (*mpAnimationAsset)->getValidatedAnimationFrames();

    // Sanity!
    AssertFatal( mCurrentFrameIndex < validatedFrames.size(), "Animation controller requested the current frame but it is out of bounds of the validated frames." );

    return validatedFrames[mCurrentFrameIndex];
};

//-----------------------------------------------------------------------------

const char* ImageFrameProviderCore::getCurrentNamedAnimationFrame( void ) const
{
    // Sanity!
    AssertFatal( mpAnimationAsset->notNull(), "Animation controller requested current image frame but no animation asset assigned." );

    // Fetch validated frames.
    const Vector<StringTableEntry>& validatedFrames = (*mpAnimationAsset)->getValidatedNamedAnimationFrames();

    // Sanity!
    AssertFatal( mCurrentFrameIndex < validatedFrames.size(), "Animation controller requested the current frame but it is out of bounds of the validated frames." );

    return validatedFrames[mCurrentFrameIndex];
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::isAnimationValid( void ) const
{
    // Not valid if no animation asset.
    if ( mpAnimationAsset->isNull() )
        return false;

    S32 validatedFrameSize = 0;

    if ((*mpAnimationAsset)->getNamedCellsMode())
        validatedFrameSize = (*mpAnimationAsset)->getValidatedNamedAnimationFrames().size();
    else
        validatedFrameSize = (*mpAnimationAsset)->getValidatedAnimationFrames().size();

    // Not valid if current frame index is out of bounds of the validated frames.
    if ( mCurrentFrameIndex >= validatedFrameSize )
        return false;

    // Fetch image asset.
    const AssetPtr<ImageAsset>& imageAsset = (*mpAnimationAsset)->getImage();

    // Not valid if no image asset.
    if ( imageAsset.isNull() )
        return false;

    if (!(*mpAnimationAsset)->getNamedCellsMode())
    {
        // Fetch current frame.
        const U32 currentFrame = getCurrentAnimationFrame();

        // Not valid if current frame is out of bounds of the image asset.
        if ( currentFrame >= imageAsset->getFrameCount() )
            return false;
    }
    else
    {
        // Fetch the current name frame.
        const char* frameName = getCurrentNamedAnimationFrame();

        if (!imageAsset->containsFrame(frameName))
            return false;
    }

    // Valid.
    return true;
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::setAnimation( const char* pAnimationAssetId )
{
    // Set as dynamic provider.
    mStaticProvider = false;

    // Ensure animation is un-paused.
    mAnimationPaused = false;

    // Reset static asset.
    mpImageAsset->clear();

    // Fetch animation asset.
    mpAnimationAsset->setAssetId( pAnimationAssetId );

    // Finish if we didn't get an animation.
    if ( mpAnimationAsset->isNull() )
        return false;

    // Play Animation.
    if ( !playAnimation( *mpAnimationAsset ) )
        return false;

    // Turn-on tick processing.
    setProcessTicks( true );

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::playAnimation( const AssetPtr<AnimationAsset>& animationAsset )
{
    // Debug Profiling.
    PROFILE_SCOPE(AnimationController_PlayAnimation);

    // Stop animation.
    stopAnimation();

    // Finish if no animation asset.
    if ( animationAsset.isNull() )
        return true;

    // Set as dynamic provider.
    mStaticProvider = false;

    // Fetch validated frames.
    U32 validatedFrameSize = 0;

    if (animationAsset->getNamedCellsMode())
        validatedFrameSize = animationAsset->getValidatedNamedAnimationFrames().size();
    else
        validatedFrameSize = animationAsset->getValidatedAnimationFrames().size();

    // Check we've got some frames.
    if ( validatedFrameSize == 0 )
    {
        Con::warnf( "ImageFrameProviderCore::playAnimation() - Cannot play AnimationAsset '%s' - Animation has no validated frames!", mpAnimationAsset->getAssetId() );
        return false;
    }

    // Set animation asset.
    mpAnimationAsset->setAssetId( animationAsset.getAssetId() );

    // Set Maximum Frame Index.
    mMaxFrameIndex = validatedFrameSize-1;

    // Calculate Total Integration Time.
    mTotalIntegrationTime = (*mpAnimationAsset)->getAnimationTime();

    // Calculate Frame Integration Time.
    mFrameIntegrationTime = mTotalIntegrationTime / validatedFrameSize;

    // No, so random Start?
    if ( (*mpAnimationAsset)->getRandomStart() )
    {
        // Yes, so calculate start time.
        mCurrentTime = CoreMath::mGetRandomF(0.0f, mTotalIntegrationTime*0.999f);
    }
    else
    {
        // No, so set first frame.
        mCurrentTime = 0.0f;
    }

    // Reset animation finished flag.
    mAnimationFinished = false;

    // Do an initial animation update.
    updateAnimation(0.0f);

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::updateAnimation( const F32 elapsedTime )
{
    // Debug Profiling.
    PROFILE_SCOPE(AnimationController_UpdateAnimation);

    // Finish if animation asset is not valid.
    if ( mpAnimationAsset->isNull() || (*mpAnimationAsset)->getImage().isNull() )
        return false;

    // Finish if animation has finished.
    if ( mAnimationFinished )
        return false;

    // Check for validity in the different frame lists
    if ((*mpAnimationAsset)->getNamedCellsMode())
    {
        // Fetch the validated name frames.
        const Vector<StringTableEntry>& validatedFrames = (*mpAnimationAsset)->getValidatedNamedAnimationFrames();

        // Finish if there are no validated frames.
        if ( validatedFrames.size() == 0 )
            return false;
    }
    else
    {
        // Fetch validated frames.
        const Vector<S32>& validatedFrames = (*mpAnimationAsset)->getValidatedAnimationFrames();

        // Finish if there are no validated frames.
        if ( validatedFrames.size() == 0 )
            return false;
    }

    // Calculate scaled time.
    const F32 scaledTime = elapsedTime * mAnimationTimeScale;

    // Update Current Time.
    mCurrentTime += scaledTime;

    // Check if the animation has finished.
    if ( !(*mpAnimationAsset)->getAnimationCycle() && mGreaterThanOrEqual(mCurrentTime, mTotalIntegrationTime) )
    {
        // Animation has finished.
        mAnimationFinished = true;

        // Fix Animation at end of frames.
        mCurrentTime = mTotalIntegrationTime - (mFrameIntegrationTime * 0.5f);
    }

    // Update Current Mod Time.
    mCurrentModTime = mFmod( mCurrentTime, mTotalIntegrationTime );

    // Calculate Current Frame.
    mCurrentFrameIndex = (S32)(mCurrentModTime / mFrameIntegrationTime);

    // Calculate if frame has changed.
    bool frameChanged = (mCurrentFrameIndex != mLastFrameIndex);

    // Reset Last Frame.
    mLastFrameIndex = mCurrentFrameIndex;

    // Return Frame-Changed Flag.
    return frameChanged;
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::setAnimationFrame( const U32 frameIndex )
{
    // Do we have a valid animation asset?
    if ( mpAnimationAsset->isNull() )
    {
        // No, so warn.
        Con::warnf("ImageFrameProviderCore::setAnimationFrame() - Cannot set frame; animation is finished or is invalid!");
        return;
    }

    // Validate Frame Index?
    if ( (S32)frameIndex < 0 || frameIndex > mMaxFrameIndex )
    {
        // No, so warn.
        Con::warnf("ImageFrameProviderCore::setAnimationFrame() - Animation Frame-Index Invalid (frame#%d of %d in %s)", frameIndex, mMaxFrameIndex, mpAnimationAsset->getAssetId() );
        // Finish here.
        return;
    }

    // Calculate current time.
    mCurrentTime = frameIndex*mFrameIntegrationTime;

    // Do an immediate animation update.
    updateAnimation(0.0f);
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::clearAssets( void )
{
    // Clear assets.
    if ( mpAnimationAsset != NULL )
        mpAnimationAsset->clear();

    if ( mpImageAsset != NULL )
        mpImageAsset->clear();

    // Reset remaining state.
    mImageFrame = 0;
    mNamedImageFrame = StringTable->EmptyString;
    mStaticProvider = true;
    setProcessTicks( false );
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::onAssetRefreshed( AssetPtrBase* pAssetPtrBase )
{
    // Don't perform any action if the animation is not already playing.
    if ( mAnimationFinished )
        return;

    // Attempt to restart the animation.
    playAnimation( *mpAnimationAsset );
}
