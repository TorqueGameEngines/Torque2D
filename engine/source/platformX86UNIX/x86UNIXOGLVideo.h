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



#ifndef _X86UNIXOGLVIDEO_H_
#define _X86UNIXOGLVIDEO_H_

#ifndef _PLATFORMVIDEO_H_
#include "platform/platformVideo.h"
#endif

class OpenGLDevice : public DisplayDevice
{
      static bool smCanSwitchBitDepth;

      bool mRestoreGamma;
      U16  mOriginalRamp[256*3];

      void addResolution(S32 width, S32 height, bool check=true);

   public:
      // Set by InitOpenGL: the window is to be created at exactly the size
      // asked for, so a tiling window manager floats it instead of tiling it.
      static bool smCreateAtExactSize;
      // While set, the window is being held at that size (see setScreenMode);
      // releaseExactSize lets go of it once the window manager has decided.
      static bool smHoldingExactSize;
      static U32  smExactSizeHeldSince;

      OpenGLDevice();
      virtual ~OpenGLDevice();

      virtual void initDevice();
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

      // Called once a frame by the event loop (x86UNIXWindow.cc): make the
      // window resizable once it has been held at its exact size long enough.
      static void releaseExactSize();

      // Called by the event loop when the window's size has changed, whoever
      // changed it: bring the canvas, the current resolution and
      // $pref::Video::fullScreen into line with the window as it now is.
      static void followWindow();

      // Whether the window is fullscreen right now -- including when the
      // window manager made it so, which SDL does not notice.
      static bool isWindowFullScreen();

      static DisplayDevice* create();
};

#endif // _H_X86UNIXOGLVIDEO

