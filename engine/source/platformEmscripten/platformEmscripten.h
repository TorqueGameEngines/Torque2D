//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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

#ifndef _PLATFORMEMSCRIPTEN_H_
#define _PLATFORMEMSCRIPTEN_H_

#include "platform/platform.h"
#include "math/mMath.h"

#include "platformEmscripten/EmscriptenOGLVideo.h"

struct SDL_Window;

#define PREF_DIR_ROOT "/.torque"
#define PREF_DIR_GAME_NAME "T2D"

class EmscriptenPlatState
{
public:
    bool              captureDisplay;
    bool              fadeWindows;

    char              appWindowTitle[256];
    bool              quit;
    bool              ctxNeedsUpdate;

    bool			portrait;

    U32               currentTime;
    bool				 fullscreen;

    U32               osVersion;

    U32               firstThreadId;
    U32               torqueThreadId;

    RandomLCG         platRandom;

    bool              mouseLocked;

    // Set while the page does not have the keyboard (EmscriptenWindow.cpp).
    bool              backgrounded;
    bool              minimized;

    U32               sleepTicks;
    U32               lastTimeTick;

    Point2I           windowSize;
    bool              windowCreated;

    // The SDL 2 window, which is the page's canvas. It is made with the GL
    // context by the first OpenGLDevice::setScreenMode and kept for the life
    // of the page.
    SDL_Window*       sdlWindow;

    U32               appReturn;

    int               argc;
    const char**      argv;

    bool              useRedirect;

    bool              dedicated;

    StringTableEntry  mainDotCsDir;

    EmscriptenPlatState();


    void setWindowTitle(const char *title)
    {
        dStrncpy(appWindowTitle, title, sizeof(appWindowTitle));
    }
};

/// Global singleton that encapsulates a lot of mac platform state & globals.
extern EmscriptenPlatState gPlatState;

/// @name Misc Emscripten Plat Functions
/// Functions that are used by multiple files in the mac plat, but too trivial
/// to require their own header file.
/// @{
/// Fills gGLState with info about this gl renderer's capabilities.
void getGLCapabilities(void);


/// Display a file dialog.
/// calls FileDialog::Execute() on \p dialog
/// @param dialog The FileDialog object to Execute. A void* is used to cut down on header dependencies.
/// @see platform/nativeDialogs/fileDialog.h
void EmscriptenShowDialog(void* dialog);
/// @}


#endif //_PLATFORMEMSCRIPTEN_H_

