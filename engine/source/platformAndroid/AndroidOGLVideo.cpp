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

// Portions taken from OpenGL Full Screen.c sample from Apple Computer, Inc.
// (that's where many of the lead helper functions originated from, but code
//  has been significantly changed & revised.)

#include "platformAndroid/platformAndroid.h"
#include "platformAndroid/platformGL.h"
#include "platformAndroid/AndroidOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/fileStream.h"
#include "platformAndroid/AndroidUtil.h"
#include "platformAndroid/AndroidEvents.h"
#include "graphics/dgl.h"
#include "debug/profiler.h"
#include "platformAndroid/T2DActivity.h"

//Luma: Tap support
extern void createMouseTapEvent(S32 nbrTaps, S32 x, S32 y);

//extern bool createAccelMoveEvent( UIAccelerationValue *accel );

// TODO: Card Profiling code isn't doing anything.
extern StringTableEntry gScreenOrientation;
extern bool gScreenUpsideDown;

//-Mat we should update the accelereometer once per frame
U32 AccelerometerUpdateMS = sgTimeManagerProcessInterval; // defines period between accelerometer updates updates in Milliseconds

#pragma mark -

//-----------------------------------------------------------------------------------------
// Creates a dummy AGL context, so that naughty objects that call OpenGL before the window
//  exists will not crash the game.
//  If for some reason we fail to get a default contet, assert -- something's very wrong.
//-----------------------------------------------------------------------------------------
void initDummyAgl(void)
{
}

U32 nAllDevs;


#pragma mark -
//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
    // Set the device name:
    mDeviceName = "OpenGL";

    // macs games are not generally full screen only
    mFullScreenOnly = false;
}

//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
    //instead of caling enum monitors and enumdisplaymodes on that, we're just going to put in the ones that we know we have

    mResolutionList.push_back(Resolution(320, 480, 16));
    mResolutionList.push_back(Resolution(480, 320, 16));

    mResolutionList.push_back(Resolution(320, 480, 32));
    mResolutionList.push_back(Resolution(480, 320, 32));

    mResolutionList.push_back(Resolution(640, 960, 16));
    mResolutionList.push_back(Resolution(960, 640, 16));

    mResolutionList.push_back(Resolution(640, 960, 32));
    mResolutionList.push_back(Resolution(960, 640, 32));

    mResolutionList.push_back(Resolution(768, 1024, 16));
    mResolutionList.push_back(Resolution(1024, 768, 16));

    mResolutionList.push_back(Resolution(768, 1024, 32));
    mResolutionList.push_back(Resolution(1024, 768, 32));

    mResolutionList.push_back(Resolution(1536, 2048, 16));
    mResolutionList.push_back(Resolution(2048, 1536, 16));

    mResolutionList.push_back(Resolution(1536, 2048, 32));
    mResolutionList.push_back(Resolution(2048, 1536, 32));

    mResolutionList.push_back(Resolution(640, 1136, 16));
    mResolutionList.push_back(Resolution(1136, 640, 16));

    mResolutionList.push_back(Resolution(640, 1136, 32));
    mResolutionList.push_back(Resolution(1136, 640, 32));

}

//------------------------------------------------------------------------------
// Activate
//  this is called once, as a result of createCanvas() in scripts.
//  dumps OpenGL driver info for the current screen
//  creates an initial window via setScreenMode
bool OpenGLDevice::activate(U32 width, U32 height, U32 bpp, bool fullScreen)
{
    Con::printf(" OpenGLDevice activating...");

    // gets opengl rendering capabilities of the screen pointed to by platState.hDisplay
    // sets up dgl with the capabilities info, & reports opengl status.

    getGLCapabilities();

    // Create the window or capture fullscreen
    if (!setScreenMode(width, height, bpp, fullScreen, true, false))
        return false;

    // set the displayDevice pref to "OpenGL"
    Con::setVariable("$pref::Video::displayDevice", mDeviceName);

    // set vertical sync now because it doesnt need setting every time we setScreenMode()
    setVerticalSync(!Con::getBoolVariable("$pref::Video::disableVerticalSync"));

    return true;
}

//------------------------------------------------------------------------------
// returns TRUE if textures need resurrecting in future...
//------------------------------------------------------------------------------
bool OpenGLDevice::cleanupContextAndWindow()
{
    Con::printf("Cleaning up the display device...");
    return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
    Con::printf("Shutting down the OpenGL display device...");
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool OpenGLDevice::setScreenMode(U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint)
{
    Con::printf(" set screen mode %i x %i x %i, %s, %s, %s", width, height, bpp,
                fullScreen ? "fullscreen" : "windowed",
                forceIt ? "force it" : "dont force it",
                repaint ? "repaint" : "dont repaint"
                );

    // validation, early outs --------------------------------------------------
    // sanity check. some scripts are liable to pass in bad values.
    if (!bpp)
        bpp = platState.desktopBitsPixel;

    Resolution newRes = Resolution(width, height, bpp);

    // if no values changing and we're not forcing a change, kick out. prevents thrashing.
    if (!forceIt && smIsFullScreen == fullScreen && smCurrentRes == newRes)
        return (true);

    // we have a new context, this is now safe to do:
    // delete any contexts or windows that exist, and kill the texture manager.
    bool needResurrect = cleanupContextAndWindow();

    Con::printf(">> Attempting to change display settings to %s %dx%dx%d...",
                fullScreen ? "fullscreen" : "windowed", newRes.w, newRes.h, newRes.bpp);

    // set torque variables ----------------------------------------------------
    // save window size for dgl
    Platform::setWindowSize(newRes.w, newRes.h);

    // update smIsFullScreen and pref
    smIsFullScreen = fullScreen;

    Con::setBoolVariable("$pref::Video::fullScreen", smIsFullScreen);

    // save resolution
    smCurrentRes = newRes;

    // save resolution to prefs
    char buf[32];

    if (fullScreen)
    {
        dSprintf(buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
        Con::setVariable("$pref::Video::resolution", buf);
    }
    else
    {
        dSprintf(buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
        Con::setVariable("$pref::Video::windowedRes", buf);
    }

    // begin rendering again ----------------------------------------------------
    if (needResurrect)
    {
        // Reload the textures gl names
        Con::printf("Resurrecting the texture manager...");
        Game->textureResurrect();
    }

    if (repaint)
        Video::resetCanvas();

    return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
	 eglSwapBuffers(platState.engine->display, platState.engine->surface);
}


//------------------------------------------------------------------------------
const char *OpenGLDevice::getDriverInfo()
{
    return NULL;
}

struct AndroidGamma
{
    F32 r, g, b;
    F32 scale;
};

//------------------------------------------------------------------------------
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
    return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
    return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::getVerticalSync()
{
    return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync(bool on)
{
    return true;
}


Resolution Video::getDesktopResolution()
{
    return Resolution(_AndroidGetScreenWidth(), _AndroidGetScreenHeight(), ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH);
}

DisplayDevice *OpenGLDevice::create()
{
    // set up a dummy default agl context.
    // this will be replaced later with the window's context,
    // but we need agl_ctx to be valid at all times,
    // since some things try to make gl calls before the device is activated.

    // create the DisplayDevice
    OpenGLDevice *newOGLDevice = new OpenGLDevice();

    newOGLDevice->initDevice();

    return newOGLDevice;
}
