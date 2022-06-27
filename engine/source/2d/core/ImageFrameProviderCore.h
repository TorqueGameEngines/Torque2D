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

#ifndef _IMAGE_FRAME_PROVIDER_CORE_H
#define _IMAGE_FRAME_PROVIDER_CORE_H

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef _ANIMATION_ASSET_H_
#include "2d/assets/AnimationAsset.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BATCH_RENDER_H_
#include "BatchRender.h"
#endif

#ifndef _FACTORY_CACHE_H_
#include "memory/factoryCache.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

///-----------------------------------------------------------------------------

class ImageFrameProviderCore :
    public virtual Tickable,
    public IFactoryObjectReset,
    protected AssetPtrCallback
{
protected:
    bool                                    mSelfTick;

    bool                                    mStaticProvider;
    
    bool                                    mUsingNamedFrame;

    U32                                     mImageFrame;
    StringTableEntry                        mNamedImageFrame;
    AssetPtr<ImageAsset>*                   mpImageAsset;
    AssetPtr<AnimationAsset>*               mpAnimationAsset;

    S32                                     mLastFrameIndex;
    S32                                     mCurrentFrameIndex;
    U32                                     mMaxFrameIndex;
    F32                                     mCurrentTime;
    F32                                     mPausedTime;
    F32                                     mCurrentModTime;
    F32                                     mAnimationTimeScale;
    F32                                     mTotalIntegrationTime;
    F32                                     mFrameIntegrationTime;
    bool                                    mAnimationPaused;
    bool                                    mAnimationFinished;

public:
    ImageFrameProviderCore();
    virtual ~ImageFrameProviderCore();

    void allocateAssets( AssetPtr<ImageAsset>* pImageAssetPtr, AssetPtr<AnimationAsset>* pAnimationAssetPtr );
    inline void deallocateAssets( void ) { mpImageAsset = NULL; mpAnimationAsset = NULL; }

    virtual void copyTo( ImageFrameProviderCore* pImageFrameProviderCore ) const;

    /// Integration.
    virtual bool update( const F32 elapsedTime );
    virtual void processTick();
    virtual void interpolateTick( F32 delta ) {};
    virtual void advanceTime( F32 timeDelta ) {};
    virtual void setProcessTicks( bool tick  ) { Tickable::setProcessTicks( mSelfTick ? tick : false ); }
    bool updateAnimation( const F32 elapsedTime );

    virtual bool validRender( void ) const;

    virtual void render(
        const bool flipX,
        const bool flipY,
        const Vector2& vertexPos0,
        const Vector2& vertexPos1,
        const Vector2& vertexPos2,
        const Vector2& vertexPos3,
        BatchRender* pBatchRenderer ) const;

	// Use uv coordinates explicitly given.
	virtual void render(
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
		BatchRender* pBatchRenderer) const;

	// Render the given list of vertex information directly.
	void render(
		const U32 vertexCount,
		const Vector2 *vertexArray,
		const Vector2 *textureArray,
		const ColorF *colorArray,
		BatchRender* pBatchRenderer) const;

	void renderGui( GuiControl& owner, Point2I offset, const RectI &updateRect ) const;

    /// Static-Image Frame.
    inline bool setImage( const char* pImageAssetId ) { return setImage( pImageAssetId, mImageFrame ); }
    virtual bool setImage( const char* pImageAssetId, const U32 frame );
    virtual bool setImage( const char* pImageAssetId, const char* pNamedFrame );
    inline StringTableEntry getImage( void ) const{ return mpImageAsset->getAssetId(); }
    virtual bool setImageFrame( const U32 frame );
    inline U32 getImageFrame( void ) const { return mImageFrame; }
    virtual bool setNamedImageFrame( const char* frame );
    inline StringTableEntry getNamedImageFrame( void ) const { return mNamedImageFrame; }
	
    /// Animated-Image Frame.
    virtual bool setAnimation( const char* pAnimationAssetId );
    inline StringTableEntry getAnimation( void ) const { return mpAnimationAsset->getAssetId(); }
    void setAnimationFrame( const U32 frameIndex );
    inline S32 getAnimationFrame( void ) const { return mCurrentFrameIndex; }
    void setAnimationTimeScale( const F32 scale ) { mAnimationTimeScale = scale; }
    inline F32 getAnimationTimeScale( void ) const { return mAnimationTimeScale; }
    bool playAnimation( const AssetPtr<AnimationAsset>& animationAsset);
    inline void pauseAnimation( const bool animationPaused ) { mAnimationPaused = animationPaused; }
    inline void stopAnimation( void ) { mAnimationFinished = true; mAnimationPaused = false; }
    inline void resetAnimationTime( void ) { mCurrentTime = 0.0f; }
    inline bool isAnimationPaused( void ) const { return mAnimationPaused; }
    inline bool isAnimationFinished( void ) const { return mAnimationFinished; };
    bool isAnimationValid( void ) const;

    /// Frame provision.
    inline bool isStaticFrameProvider( void ) const { return mStaticProvider; }
    inline bool isUsingNamedImageFrame( void ) const { return mUsingNamedFrame; }
    inline TextureHandle& getProviderTexture( void ) const { return !validRender() ? BadTextureHandle : isStaticFrameProvider() ? (*mpImageAsset)->getImageTexture() : (*mpAnimationAsset)->getImage()->getImageTexture(); };
    const ImageAsset::FrameArea& getProviderImageFrameArea( void ) const;
    inline const AnimationAsset* getCurrentAnimation( void ) const { return mpAnimationAsset->notNull() ? *mpAnimationAsset : NULL; };
    inline const StringTableEntry getCurrentAnimationAssetId( void ) const { return mpAnimationAsset->getAssetId(); };
    const U32 getCurrentAnimationFrame( void ) const;
    const char* getCurrentNamedAnimationFrame( void ) const;
    inline const F32 getCurrentAnimationTime( void ) const { return mCurrentTime; };

    void clearAssets( void );

    virtual void resetState( void );

protected:
    virtual void onAnimationEnd( void ) {}
    virtual void onAssetRefreshed( AssetPtrBase* pAssetPtrBase );
};


#endif // _IMAGE_FRAME_PROVIDER_CORE_H
