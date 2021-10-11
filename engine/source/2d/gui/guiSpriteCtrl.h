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
#define _GUISPRITECTRL_H_

#ifndef _IMAGE_FRAME_PROVIDER_H
#include "2d/core/ImageFrameProvider.h"
#endif

//-----------------------------------------------------------------------------

class GuiSpriteCtrl : public GuiControl, public ImageFrameProvider
{
private:
    typedef GuiControl Parent;

protected:
    StringTableEntry                mImageAssetId;
    U32                             mFrame;
    StringTableEntry                mNamedImageFrameId;
    StringTableEntry                mAnimationAssetId;
	StringTableEntry				mBitmapName;
	TextureHandle					mBitmapTextureHandle;
	Vector<RectI>					mBitmapArrayRects;

	bool mTileImage; //If true, the image will tile over the content area
	Point2I mPositionOffset; //Offset from the set location of the image
	FluidColorI mImageColor; //The blend color used on the image
	Point2I mImageSize; //The size of the image, reduced, if needed, to fit the content area
	bool mFullSize; //If true, the image will take all available space
	bool mConstrainProportions; //If true, the image will maintain its aspect ratio 
	bool mSingleFrameBitmap; //If true and bitmaps are used, this will assume there's only one frame when scanning the bitmap

	Fluid mFluidMoveTo;
	Point2I mStartOffset;
	Point2I mTargetOffset;

	Fluid mFluidGrowTo;
	Point2I mStartSize;
	Point2I mTargetSize;

public:
    GuiSpriteCtrl();
    virtual ~GuiSpriteCtrl();
	static void initPersistFields();
    bool onWake();
    void onSleep();
    virtual void copyTo(SimObject* object);

	//Rendering
	void onRender(Point2I offset, const RectI &updateRect);
	void RenderImage(RectI &bounds, Point2I &offset, Point2I &size);
	void RenderTiledImage(RectI &bounds, Point2I &start, Point2I &size);

	//positioning and sizing
	Point2I constrain(Point2I &point, bool grow = true);
	Point2I constrainLockX(Point2I &point);
	Point2I constrainLockY(Point2I &point);
	F32 getAspectRatio();
	Point2I& applyAlignment(RectI &bounds, Point2I &size);

	//Animation Functions
	void moveTo(S32 x, S32 y, S32 time, EasingFunction ease = Linear);
	void growTo(S32 x, S32 y, S32 time, EasingFunction ease = Linear);
	void fadeTo(const ColorI &color, S32 time, EasingFunction ease = Linear);
	bool animateMoveTo();
	bool animateGrowTo();
	bool animateFadeTo();

    // Static and Animated Assets.
    inline bool setImage( const char* pImageAssetId ) { return setImage( pImageAssetId, mImageFrame ); }
    virtual bool setImage( const char* pImageAssetId, const U32 frame );
    virtual bool setImage( const char* pImageAssetId, const char* pNamedFrame );
    inline StringTableEntry getImage( void ) const{ return mImageAssetId; }
    virtual bool setImageFrame( const U32 frame );
    inline U32 getImageFrame( void ) const { return mFrame; }
    virtual bool setNamedImageFrame ( const char* namedFrame );
    inline StringTableEntry getNamedImageFrame( void ) const { return mNamedImageFrameId; }
    virtual bool setAnimation( const char* pAnimationAssetId );
    inline StringTableEntry getAnimation( void ) const { return mAnimationAssetId; }

	//Accessors
	inline bool getTileImage() { return mTileImage; }
	inline void setTileImage(bool setting) { mTileImage = setting; }
	inline Point2I getPositionOffset() { return mPositionOffset; }
	inline void setPositionOffset(S32 x, S32 y) { mPositionOffset = Point2I(x, y); }
	inline ColorI getImageColor() { return ColorI(mImageColor); }
	inline void setImageColor(ColorI color) { mImageColor = FluidColorI(color); }
	inline Point2I getImageSize() { return mImageSize; }
	inline void setImageSize(Point2I size) { mImageSize = size; }
	inline bool getFullSize() { return mFullSize; }
	inline void setFullSize(bool isFull) { mFullSize = isFull; }
	inline bool getConstrainProportions() { return mConstrainProportions; }
	inline void setConstrainProportions(bool setting) { mConstrainProportions = setting; }
	inline bool getSingleFrameBitmap() { return mSingleFrameBitmap; }
	inline void setSingleFrameBitmap(bool isSingleFrame) { mSingleFrameBitmap = isSingleFrame; }

	//Bitmap members
	void setBitmap(const char *name);
	inline void setBitmap(const TextureHandle &handle) { mBitmapTextureHandle = handle; }
	inline StringTableEntry getBitmapName(void) const { return mBitmapName; }
	U32 constructBitmapArray();
	inline U32 getBitmapFrame() { return mSingleFrameBitmap ? 1 : constructBitmapArray(); }
	inline bool usesAsset() { return (mImageAssetId != StringTable->EmptyString) || (mAnimationAssetId != StringTable->EmptyString); }

    // Declare type.
    DECLARE_CONOBJECT(GuiSpriteCtrl);

protected:
	virtual bool update(const F32 elapsedTime);
    virtual void onAnimationEnd( void );
	virtual void processTick();
	virtual void interpolateTick(F32 delta) {};
	virtual void advanceTime(F32 timeDelta) {};
	RectI getSourceRect(TextureObject* texture);

protected:
    static bool setImage(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setImage( data ); return false; }
    static bool writeImage(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if( !pCastObject->usesAsset() || !pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mImageAssetId != StringTable->EmptyString; }
    static bool setImageFrame(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setImageFrame( dAtoi(data) ); return false; }
    static bool writeImageFrame(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if( !pCastObject->usesAsset() ) { return pCastObject->getBitmapFrame(); } if ( !pCastObject->isStaticFrameProvider() || pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->getImageFrame() > 0; }
    static bool setNamedImageFrame(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setNamedImageFrame(data); return false; }
    static bool writeNamedImageFrame(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if( !pCastObject->usesAsset() || !pCastObject->isStaticFrameProvider() || !pCastObject->isUsingNamedImageFrame() ) return false; return pCastObject->mNamedImageFrameId != StringTable->EmptyString; }
    static bool setAnimation(void* obj, const char* data) { static_cast<GuiSpriteCtrl*>(obj)->setAnimation(data); return false; };
    static bool writeAnimation(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if( !pCastObject->usesAsset() || pCastObject->isStaticFrameProvider() ) return false; return pCastObject->mAnimationAssetId != StringTable->EmptyString; }
	static bool setBitmapName(void *obj, const char *data) { static_cast<GuiSpriteCtrl *>(obj)->setBitmap(data); return false; }
	static const char *getBitmapName(void *obj, const char *data) { return static_cast<GuiSpriteCtrl*>(obj)->getBitmapName(); }
	static bool writeBitmapName(void* obj, StringTableEntry pFieldName) { GuiSpriteCtrl* pCastObject = static_cast<GuiSpriteCtrl*>(obj); if( pCastObject->usesAsset() ) return false; return pCastObject->mBitmapName != StringTable->EmptyString; }
};

#endif //_GUISPRITECTRL_H_
