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

#ifndef _X86UNIXSTATE_H_
#define _X86UNIXSTATE_H_

#include "math/mPoint.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/platformGL.h"

#include <libgen.h> // for basename

struct SDL_Window;

class x86UNIXPlatformState
{
   private:
      Point2I              mDesktopSize;
      Point2I              mWindowSize;
      S32                  mDesktopBpp;
      SDL_Window           *mSDLWindow;
      char                 mWindowName[40];
      char                 mExePathName[4096];
      char                 mExeName[40];
      bool                 mWindowCreated;
      bool                 mWindowActive;
      bool                 mWindowLocked;
      bool                 mDedicated;
      bool                 mDSleep;

   public:
      U32      currentTime;

      // for compatibility, convert 24 bpp to 32
      void setDesktopBpp( S32 bpp )
      {
         if (bpp == 24)
            mDesktopBpp = 32;
         else
            mDesktopBpp = bpp;
      }
      S32 getDesktopBpp() { return mDesktopBpp; }

      void setDesktopSize( S32 horizontal, S32 vertical )
          { mDesktopSize.set( horizontal, vertical ); }
      Point2I getDesktopSize() { return mDesktopSize; }

      // The one window the engine draws into, created by OpenGLDevice::
      // setScreenMode with its GL context and kept until shutdown; NULL until
      // then, and always on a dedicated server.
      void setSDLWindow( SDL_Window *window ) { mSDLWindow = window; }
      SDL_Window *getSDLWindow() { return mSDLWindow; }

      void setWindowSize (S32 horizontal, S32 vertical )
          { mWindowSize.set ( horizontal, vertical ); }
      void setWindowSize( Point2I size ) { mWindowSize = size; }
      Point2I& getWindowSize() { return ( mWindowSize ); }

      void setWindowName (const char * windowName)
      {
         if (windowName == NULL)
            dStrncpy( mWindowName, "", sizeof( mWindowName ));
         else
            dStrncpy( mWindowName, windowName, sizeof( mWindowName ) );
      }
      const char * getWindowName() { return mWindowName; }

      void setExePathName(const char* exePathName)
      {
         if (exePathName == NULL)
            dStrncpy(mExePathName, "", sizeof(mExePathName));
         else
            dStrncpy(mExePathName, exePathName, sizeof(mExePathName));

         // set the base exe name field
         char tempBuf[2048];
         dStrncpy(tempBuf, mExePathName, 2048);
         dStrncpy(mExeName, basename(tempBuf), sizeof(mExeName));
      }
      const char * getExePathName() { return mExePathName; }
      const char * getExeName() { return mExeName; }

      bool windowCreated() { return mWindowCreated; }
      bool windowActive() { return mWindowActive; }
      bool windowLocked() { return mWindowLocked; }
      void setWindowCreated(bool windowCreated)
          { mWindowCreated = windowCreated; }
      void setWindowActive(bool windowActive)
          { mWindowActive = windowActive; }
      void setWindowLocked(bool windowLocked)
          { mWindowLocked = windowLocked; }

      bool isDedicated() { return mDedicated; }
      void setDedicated(bool dedicated) { mDedicated = dedicated; }

      bool getDSleep() { return mDSleep; }
      void setDSleep(bool enabled) { mDSleep = enabled; }

      x86UNIXPlatformState()
      {
         currentTime = 0;
         mDesktopBpp = 16;
         mDesktopSize.set( 0, 0 );
         mWindowSize.set( 800, 600 );
         mSDLWindow = NULL;
         setWindowName("Torque");
         setExePathName(NULL);
         mWindowCreated = mWindowActive = mWindowLocked = false;
         mDedicated = false;
         mDSleep = false;
      }
};

extern x86UNIXPlatformState  * x86UNIXState;

#endif // _X86UNIXSTATE_H_
