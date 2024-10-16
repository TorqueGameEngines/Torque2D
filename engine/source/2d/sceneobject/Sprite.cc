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
#include "Sprite.h"
#endif

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

// Script bindings.
#include "Sprite_ScriptBinding.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(Sprite);

//------------------------------------------------------------------------------

Sprite::Sprite() :
    mFlipX(false),
    mFlipY(false)
{
	mUseComplexColor = false;
	mComplexColor0 = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	mComplexColor1 = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	mComplexColor2 = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	mComplexColor3 = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
}

//------------------------------------------------------------------------------

Sprite::~Sprite()
{
}

//------------------------------------------------------------------------------

void Sprite::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to sprite.
    Sprite* pSprite = static_cast<Sprite*>(object);

    // Sanity!
    AssertFatal(pSprite != NULL, "Sprite::copyTo() - Object is not the correct type.");

    /// Render flipping.
    pSprite->setFlip( getFlipX(), getFlipY() );
}

//------------------------------------------------------------------------------

void Sprite::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    /// Render flipping.
    addField("FlipX", TypeBool, Offset(mFlipX, Sprite), &writeFlipX, "");
    addField("FlipY", TypeBool, Offset(mFlipY, Sprite), &writeFlipY, "");

	addField("useComplexColor", TypeBool, Offset(mUseComplexColor, Sprite), &writeUseComplexColor, "");
}

//------------------------------------------------------------------------------

void Sprite::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
	if (mUseComplexColor)
	{
		ImageFrameProviderCore::renderComplex(
			getFlipX(), getFlipY(),
			mRenderOOBB[0],
			mRenderOOBB[1],
			mRenderOOBB[2],
			mRenderOOBB[3],
			pBatchRenderer,
			mComplexColor0,
			mComplexColor1,
			mComplexColor2,
			mComplexColor3);
	}
	else 
	{
		ImageFrameProvider::render(
			getFlipX(), getFlipY(),
			mRenderOOBB[0],
			mRenderOOBB[1],
			mRenderOOBB[2],
			mRenderOOBB[3],
			pBatchRenderer );
	}
}

//------------------------------------------------------------------------------

const ColorF& Sprite::getComplexColor(const S8 cornerID)
{
	if (cornerID == 1)
	{
		return mComplexColor0;
	}
	else if (cornerID == 2)
	{
		return mComplexColor1;
	}
	else if (cornerID == 3)
	{
		return mComplexColor2;
	}
	else if (cornerID == 4)
	{
		return mComplexColor3;
	}
	else
	{
		return ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

//-----------------------------------------------------------------------------

void Sprite::updateBlendColor(const F32 elapsedTime)
{
	if (!mUseComplexColor)
	{
		SceneObject::updateBlendColor(elapsedTime);
	}
	else
	{
		updateFadeColor(mComplexColor0, mFadeToColorTL, elapsedTime);
		updateFadeColor(mComplexColor1, mFadeToColorTR, elapsedTime);
		updateFadeColor(mComplexColor2, mFadeToColorBR, elapsedTime);
		updateFadeColor(mComplexColor3, mFadeToColorBL, elapsedTime);
	}
}

//-----------------------------------------------------------------------------

void Sprite::checkFadeComplete()
{
	if (mFadeActive && 
		((mUseComplexColor && 
		mComplexColor0 == mFadeToColorTL.TargetColor &&
		mComplexColor1 == mFadeToColorTR.TargetColor &&
		mComplexColor2 == mFadeToColorBR.TargetColor &&
		mComplexColor3 == mFadeToColorBL.TargetColor) ||
		(!mUseComplexColor &&
		mBlendColor == mFadeToColor.TargetColor)))
	{
		mFadeActive = false;

		PROFILE_SCOPE(Sprite_onFadeToComplete);
		Con::executef(this, 1, "onFadeToComplete");
	}
}

bool Sprite::fadeToComplex(const S8 cornerID, const ColorF& targetColor, const F32 deltaRed, const F32 deltaGreen, const F32 deltaBlue, const F32 deltaAlpha)
{
	// Check in a scene.
	if (!getScene())
	{
		Con::warnf("Sprite::fadeToComplex() - Cannot fade object (%d) to a color as it is not in a scene.", getId());
		return false;
	}

	// Check targetColor.
	if (!targetColor.isValidColor())
	{
		Con::warnf("Sprite::fadeToComplex() - Cannot fade object (%d) because the color is invalid.", getId());
		return false;
	}

	// Check that the sprite is using complex colors
	if (!mUseComplexColor)
	{
		Con::warnf("Sprite::fadeToComplex() - Cannot fade object (%d) because the sprite is not using complex colors.", getId());
		return false;
	}

	// Only set fading active if the target color is not the blending color.
	if (targetColor != getComplexColor(cornerID))
	{
		mFadeActive = true;
		if (cornerID == 1) { mFadeToColorTL.set(targetColor, deltaRed, deltaGreen, deltaBlue, deltaAlpha); }
		else if (cornerID == 2) { mFadeToColorTR.set(targetColor, deltaRed, deltaGreen, deltaBlue, deltaAlpha); }
		else if (cornerID == 3) { mFadeToColorBL.set(targetColor, deltaRed, deltaGreen, deltaBlue, deltaAlpha); }
		else if (cornerID == 4) { mFadeToColorBR.set(targetColor, deltaRed, deltaGreen, deltaBlue, deltaAlpha); }
	}

	return true;
}