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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#ifndef _SPRITE_BASE_H_
#include "2d/core/SpriteBase.h"
#endif

#ifndef _FADETOCOLOR_H_
#include "graphics/FadeToColor.h"
#endif

//------------------------------------------------------------------------------

class Sprite : public SpriteBase
{
    typedef SpriteBase Parent;

private:
    /// Render flipping.
    bool mFlipX;
    bool mFlipY;

	FadeToColor				mFadeToColorTL;
	FadeToColor				mFadeToColorTR;
	FadeToColor				mFadeToColorBL;
	FadeToColor				mFadeToColorBR;

public:
    Sprite();
    virtual ~Sprite();

    static void initPersistFields();
    virtual void copyTo(SimObject* object);

    /// Render flipping.
    void setFlip( const bool flipX, const bool flipY )  { mFlipX = flipX; mFlipY = flipY; }
    void setFlipX( const bool flipX )                   { setFlip( flipX, mFlipY ); }
    void setFlipY( const bool flipY )                   { setFlip( mFlipX, flipY ); }
    inline bool getFlipX( void ) const                  { return mFlipX; }
    inline bool getFlipY( void ) const                  { return mFlipY; }

    virtual void sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer );

	// Complex Colors (Using 4 blend colors, one for each corner)
	inline void setUseComplexColor(const bool complexColor) { mUseComplexColor = complexColor; }
	inline bool getUseComplexColor(void) const { return mUseComplexColor; }
	inline void setComplexColor(const ColorF& blendColor0, const ColorF& blendColor1, const ColorF& blendColor2, const ColorF& blendColor3) { mComplexColor0 = blendColor0; mComplexColor1 = blendColor1; mComplexColor2 = blendColor2; mComplexColor3 = blendColor3; }
	const ColorF& getComplexColor(const S8 cornerID);
	static Corner getCornerEnum(const char* label);

	// FadeTo for Complex Colors
	bool					fadeToComplex(const S8 cornerID, const ColorF& targetColor, const F32 deltaRed, const F32 deltaGreen, const F32 deltaBlue, const F32 deltaAlpha);
	void					updateBlendColor(const F32 elapsedTime);
	void					checkFadeComplete();

    /// Declare Console Object.
    DECLARE_CONOBJECT( Sprite );

protected:
    static bool writeFlipX( void* obj, StringTableEntry pFieldName )        { return static_cast<Sprite*>(obj)->getFlipX() == true; }
    static bool writeFlipY( void* obj, StringTableEntry pFieldName )        { return static_cast<Sprite*>(obj)->getFlipY() == true; }

	static bool writeUseComplexColor( void* obj, StringTableEntry pFieldName ) {return static_cast<Sprite*>(obj)->getUseComplexColor() == true; }

	bool					mUseComplexColor;
	ColorF					mComplexColor0;
	ColorF					mComplexColor1;
	ColorF					mComplexColor2;
	ColorF					mComplexColor3;
};

#endif // _SPRITE_H_
