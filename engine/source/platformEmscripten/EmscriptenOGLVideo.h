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

#ifndef _EMSCRIPTENOGLVIDEO_H_
#define _EMSCRIPTENOGLVIDEO_H_

#ifndef _PLATFORMVIDEO_H_
#include "platform/platformVideo.h"
#endif

class OpenGLDevice : public DisplayDevice
{
      static bool smCanSwitchBitDepth;

      void addResolution(S32 width, S32 height, bool check=true);

   public:
      OpenGLDevice();
      virtual ~OpenGLDevice();

      void initDevice();
      bool activate( U32 width, U32 height, U32 bpp, bool fullScreen );
      void shutdown();
      void destroy();
      bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true );
      void swapBuffers();
      const char* getDriverInfo();
      bool getGammaCorrection(F32 &g);
      bool setGammaCorrection(F32 g);
      bool getVerticalSync();
      bool setVerticalSync( bool on );
      void loadResolutions();

      // Called by the event loop when the canvas has changed size, whoever
      // changed it -- the page, the browser going in or out of fullscreen, or
      // setScreenMode itself: bring the canvas, the current resolution and
      // $pref::Video::fullScreen into line with the canvas as it now is.
      static void followWindow();

      // Whether the canvas is fullscreen, or has been asked to be and is
      // waiting on the player (see setScreenMode).
      static bool isWindowFullScreen();

      static DisplayDevice* create();
};

#endif //_EMSCRIPTENOGLVIDEO_H_
