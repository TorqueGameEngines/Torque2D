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

#ifndef _PLATFORMVIDEO_H_
#define _PLATFORMVIDEO_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif
#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif
#ifndef _PLATFORMASSERT_H_
#include "platform/platformAssert.h"
#endif

enum devices
{
   NO_DEVICE = -1,
   OPENGL_DEVICE,
   VOODOO2_DEVICE,
   N_DEVICES
};

struct Resolution;
class DisplayDevice;


class Video
{
private:
   static Vector<DisplayDevice *> smDeviceList;
   static DisplayDevice*   smCurrentDevice;
   static bool smCritical;

public:
   static bool smNeedResurrect;

   static void init();
   static void destroy();                             // clean up and shut down
   static bool installDevice( DisplayDevice *dev );
   static bool setDevice( const char *renderName, U32 width, U32 height, U32 bpp, bool fullScreen );   // set the current display device
   static void resetCanvas();
   static bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen );
   static void deactivate( bool force = false );      // deactivate current display device
   static void reactivate( bool force = false );      // reactivate current display device
   static bool setResolution( U32 width, U32 height, U32 bpp );   // set the current resolution
   static bool toggleFullScreen();                    // toggle full screen mode
   static DisplayDevice* getDevice( const char* renderName );
   static const char* getDeviceList();                  // get a tab-separated list of all the installed display devices
   static const char* getResolutionList();            // get a tab-separated list of all the available resolutions for the current device
   static const char* getDriverInfo();                  // get info about the current display device driver
   static bool prevRes();                             // switch to the next smaller available resolution with the same bit depth
   static bool nextRes();                             // switch to the next larger available resolution with the same bit depth
   static Resolution getResolution();                 // get the current resolution
   static Resolution getDesktopResolution();          // get the current desktop resolution
   static bool isFullScreen();                        // return the current screen state
   static void swapBuffers();                         // page flip
   static bool getGammaCorrection(F32 &g);            // get gamma correction
   static bool setGammaCorrection(F32 g);             // set gamma correction
   static bool getVerticalSync();                     // get current state of vertical sync
   static bool setVerticalSync( bool on );            // enable/disable vertical sync
};


struct Resolution
{
   U32 w, h, bpp;

   Resolution( U32 _w = 0, U32 _h = 0, U32 _bpp = 0 )
   {
      w = _w;
      h = _h;
      bpp = _bpp;
   }

   bool operator==( const Resolution& otherRes ) const
   {
      return ( ( w == otherRes.w ) && ( h == otherRes.h ) && ( bpp == otherRes.bpp ) );
   }

   void operator=( const Resolution& otherRes )
   {
      w = otherRes.w;
      h = otherRes.h;
      bpp = otherRes.bpp;
   }
};


class DisplayDevice
{
   public:
      const char* mDeviceName;

   protected:
      static Resolution    smCurrentRes;
      static bool          smIsFullScreen;

      Vector<Resolution>   mResolutionList;
      bool                 mFullScreenOnly;

   public:
      DisplayDevice();
      virtual ~DisplayDevice() {}

      virtual void initDevice() = 0;
      virtual bool activate( U32 width, U32 height, U32 bpp, bool fullScreen ) = 0;
      virtual void shutdown() = 0;

      virtual bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true ) = 0;
      virtual bool setResolution( U32 width, U32 height, U32 bpp );
      virtual bool toggleFullScreen();
      virtual void swapBuffers() = 0;
      virtual const char* getDriverInfo() = 0;
      virtual bool getGammaCorrection(F32 &g) = 0;
      virtual bool setGammaCorrection(F32 g) = 0;
      virtual bool getVerticalSync() = 0;
      virtual bool setVerticalSync( bool on ) = 0;

      bool prevRes();
      bool nextRes();
      const char* getResolutionList();
      bool isFullScreenOnly()   { return( mFullScreenOnly ); }

      static void       init();
      static Resolution getResolution();
      static bool       isFullScreen();
};


//------------------------------------------------------------------------------
inline bool DisplayDevice::setResolution( U32 width, U32 height, U32 bpp )
{
   return setScreenMode( width, height, bpp, smIsFullScreen );
}


//------------------------------------------------------------------------------
inline bool DisplayDevice::toggleFullScreen()
{
   return setScreenMode( smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp, !smIsFullScreen );
}


//------------------------------------------------------------------------------
inline Resolution DisplayDevice::getResolution()
{
   return smCurrentRes;
}


//------------------------------------------------------------------------------
inline bool DisplayDevice::isFullScreen()
{
   return smIsFullScreen;
}

#endif // _H_PLATFORMVIDEO
