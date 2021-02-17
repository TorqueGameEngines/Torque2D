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

#include "guiSpriteCtrl_ScriptBindings.h"

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(GuiSpriteCtrl);

//-----------------------------------------------------------------------------

GuiSpriteCtrl::GuiSpriteCtrl( void ) :
    mImageAssetId(StringTable->EmptyString),
    mImageFrameId(0),
    mNamedImageFrameId(StringTable-> EmptyString),
    mAnimationAssetId(StringTable->EmptyString)
{
    // Set to self ticking.
    mSelfTick = true;
    
    // Default to static provider.
    mStaticProvider = true;
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

    addProtectedField("Image", TypeAssetId, Offset(mImageAssetId, GuiSpriteCtrl), &setImage, &defaultProtectedGetFn, &writeImage, "The image asset Id used for the image.");
    addProtectedField("Frame", TypeS32, Offset(mImageFrameId, GuiSpriteCtrl), &setImageFrame, &defaultProtectedGetFn, &writeImageFrame, "The image frame used for the image.");
    addProtectedField("NamedFrame", TypeString, Offset(mNamedImageFrameId, GuiSpriteCtrl), &setNamedImageFrame, &defaultProtectedGetFn, &writeNamedImageFrame, "The named image frame used for the image");
    addProtectedField("Animation", TypeAssetId, Offset(mAnimationAssetId, GuiSpriteCtrl), &setAnimation, &defaultProtectedGetFn, &writeAnimation, "The animation to use.");
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

    // Are we in static mode?
    if ( mImageAssetId != StringTable->EmptyString )
    {
        if ( mNamedImageFrameId != StringTable->EmptyString)
        {
            // Set the image asset and named frame
            ImageFrameProvider::setImage( mImageAssetId, mNamedImageFrameId );
        }
        else
        {
            // Set image asset and numerical frame.
            ImageFrameProvider::setImage( mImageAssetId, mImageFrameId );
        }
    }
    else if ( mAnimationAssetId != StringTable->EmptyString )
    {
        // Play animation asset.
        ImageFrameProvider::setAnimation( mAnimationAssetId );
    }
    else
    {
        // Not good, so warn.
        Con::warnf("GuiSpriteCtrl::onWake() - No Image or Animation Asset defined.");
    }

    return true;
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::onSleep()
{
    // Clear assets.
    ImageFrameProvider::clearAssets();

    // Call parent.
    Parent::onSleep();
}

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::processTick(void)
{
	// Update using tick period.
	update(Tickable::smTickSec);
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImage( const char* pImageAssetId, const U32 frame )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

    // Reset animation.
    if ( mAnimationAssetId != StringTable->EmptyString )
        mAnimationAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    if ( mImageAssetId != pImageAssetId )
        mImageAssetId = StringTable->insert(pImageAssetId);

    // Set the image frame if the image asset was set.
    if ( mImageAssetId != StringTable->EmptyString )
        setImageFrame(frame);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setImage(pImageAssetId, frame) )
        return false;

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImage( const char* pImageAssetId, const char* pNamedFrame )
{
    // Sanity!
    AssertFatal( pImageAssetId != NULL, "Cannot use a NULL asset Id." );

    // Reset animation.
    if ( mAnimationAssetId != StringTable->EmptyString )
        mAnimationAssetId = StringTable->EmptyString;

    // Fetch the asset Id.
    if ( mImageAssetId != pImageAssetId )
        mImageAssetId = StringTable->insert(pImageAssetId);

    // Set the image frame if the image asset was set.
    if ( mImageAssetId != StringTable->EmptyString )
        setNamedImageFrame(pNamedFrame);

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setImage(pImageAssetId, pNamedFrame) )
        return false;

    // Update control.
    setUpdate();

    return true;
}

//-----------------------------------------------------------------------------

bool GuiSpriteCtrl::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mImageAssetId == StringTable->EmptyString )
    {
        // Warn.
        Con::warnf("GuiSpriteCtrl::setImageFrame() - Cannot set frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Set frame.
    mImageFrameId = frame;

    // Finish if not awake.
    if ( !isAwake() )
        return true;

    // Call parent.
    if ( !ImageFrameProvider::setImageFrame(frame) )
        return false;

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

//-----------------------------------------------------------------------------

void GuiSpriteCtrl::onRender( Point2I offset, const RectI &updateRect)
{
    // Call parent.
    ImageFrameProvider::renderGui( *this, offset, updateRect );
}

//------------------------------------------------------------------------------

void GuiSpriteCtrl::onAnimationEnd( void )
{
    // Clear assets.
    ImageFrameProvider::clearAssets();
}
