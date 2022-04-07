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

#include "platformWin32/platformGL.h"
#include "platformWin32/platformWin32.h"
#include "platformWin32/winOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/fileStream.h"
#include "winOGLVideo_ScriptBinding.h"

//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
   initDevice();
}


//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
   // Set the device name:
   mDeviceName = "OpenGL";

   // Let Parent enumerate valid resolutions.
   Parent::initDevice();
}


//------------------------------------------------------------------------------
bool OpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   Con::printf( "Activating the OpenGL display device..." );

   bool needResurrect = false;

   // If the rendering context exists, delete it:
   if ( winState.hGLRC )
   {
      Con::printf( "Killing the texture manager..." );
      Game->textureKill();
      needResurrect = true;

      Con::printf( "Making the rendering context not current..." );
      if ( !dwglMakeCurrent( NULL, NULL ) )
      {
         AssertFatal( false, "OpenGLDevice::activate\ndwglMakeCurrent( NULL, NULL ) failed!" );
         return false;
      }

      Con::printf( "Deleting the rendering context ..." );
      if ( !dwglDeleteContext( winState.hGLRC ) )
      {
         AssertFatal( false, "OpenGLDevice::activate\ndwglDeleteContext failed!" );
         return false;
      }
      winState.hGLRC = NULL;
   }

   // [neo, 5/31/2007 - #3174]
   if( winState.appMenu )
   {
      DestroyMenu( winState.appMenu );

      winState.appMenu = NULL;
   }

   // If the window already exists, kill it so we can start fresh:
   if ( winState.appWindow )
   {
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }

      Con::printf( "Destroying the window..." );
      DestroyWindow( winState.appWindow );
      winState.appWindow = NULL;
   }

   // If OpenGL library already loaded, shut it down and reload it:
   if ( winState.hinstOpenGL )
      GL_Shutdown();

   GL_Init( "opengl32", "glu32" );

    // Set the resolution:
   if ( !setScreenMode( width, height, bpp, ( fullScreen || mFullScreenOnly ), true, false ) )
      return false;

   // Get original gamma ramp
   mRestoreGamma = GetDeviceGammaRamp(winState.appDC, mOriginalRamp);

   // Output some driver info to the console:
   const char* vendorString   = (const char*) glGetString( GL_VENDOR );
   const char* rendererString = (const char*) glGetString( GL_RENDERER );
   const char* versionString  = (const char*) glGetString( GL_VERSION );
   Con::printf( "OpenGL driver information:" );
   if ( vendorString )
      Con::printf( "  Vendor: %s", vendorString );
   if ( rendererString )
      Con::printf( "  Renderer: %s", rendererString );
   if ( versionString )
      Con::printf( "  Version: %s", versionString );

   if ( needResurrect )
   {
      // Reload the textures:
      Con::printf( "Resurrecting the texture manager..." );
      Game->textureResurrect();
   }

   GL_EXT_Init();

   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

   // Do this here because we now know about the extensions:
   if ( gGLState.suppSwapInterval )
      setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ) );

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
   Con::printf( "Shutting down the OpenGL display device..." );

   if ( winState.hGLRC )
   {
      if (mRestoreGamma)
         SetDeviceGammaRamp(winState.appDC, mOriginalRamp);

      Con::printf( "Making the GL rendering context not current..." );
      dwglMakeCurrent( NULL, NULL );
        if ( Con::getBoolVariable("$pref::Video::deleteContext", true) )
      {
         Con::printf( "Deleting the GL rendering context..." );
            dwglDeleteContext( winState.hGLRC );
      }
      winState.hGLRC = NULL;
   }

   if ( winState.appDC )
   {
      Con::printf( "Releasing the device context..." );
      ReleaseDC( winState.appWindow, winState.appDC );
      winState.appDC = NULL;
   }

   if ( smIsFullScreen )
   {
      Con::printf( "Restoring the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
   }
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool OpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{
   HWND curtain = NULL;
   char errorMessage[256];
   Resolution newRes( width, height, bpp );
   bool newFullScreen = fullScreen;
   bool safeModeOn = Con::getBoolVariable( "$pref::Video::safeModeOn" );

   if ( !newFullScreen && mFullScreenOnly )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - device or desktop color depth does not allow windowed mode!" );
      newFullScreen = true;
   }

   if ( !newFullScreen && ( (S32)newRes.w >= winState.desktopClientWidth || (S32)newRes.h >= winState.desktopClientHeight ) )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't switch to resolution larger than desktop in windowed mode!" );
      S32 resIndex = 0;
      S32 bestScore = 0, thisScore = 0;
      for ( S32 i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else if( (S32)newRes.h <= winState.desktopClientHeight && (S32)newRes.w <= winState.desktopClientWidth )
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
               + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
               + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

   }

   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   if ( newRes.w < MIN_RESOLUTION_X || newRes.h < MIN_RESOLUTION_Y )
   {
      Con::warnf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode -- can't go smaller than 640x480!" );
      return false;
   }

   if ( newFullScreen )
   {
      if (newRes.bpp != 16 && mFullScreenOnly)
         newRes.bpp = 16;

      // Match the new resolution to one in the list:
      U32 resIndex = 0;
      U32 bestScore = 0, thisScore = 0;
      for ( int i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
                      + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
                      + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

      newRes = mResolutionList[resIndex];
   }
   else
   {
      // Basically ignore the bit depth parameter:
      newRes.bpp = winState.desktopBitsPixel;
   }

   // Return if already at this resolution:
   if ( !forceIt && newRes == smCurrentRes && newFullScreen == smIsFullScreen )
      return true;

   Con::printf( "Setting screen mode to %dx%dx%d (%s)...", newRes.w, newRes.h, newRes.bpp, ( newFullScreen ? "fs" : "w" ) );

   bool needResurrect = false;

   if ( ( newRes.bpp != smCurrentRes.bpp ) || ( safeModeOn && ( ( smIsFullScreen != newFullScreen ) || newFullScreen ) ) )
   {
      // Delete the rendering context:
      if ( winState.hGLRC )
      {
         if (!Video::smNeedResurrect)
            {
            Con::printf( "Killing the texture manager..." );
            Game->textureKill();
            needResurrect = true;
            }

         Con::printf( "Making the rendering context not current..." );
         if ( !dwglMakeCurrent( NULL, NULL ) )
         {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent( NULL, NULL ) failed!" );
            return false;
         }

         Con::printf( "Deleting the rendering context..." );
         if ( Con::getBoolVariable("$pref::Video::deleteContext",true) &&
              !dwglDeleteContext( winState.hGLRC ) )
         {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglDeleteContext failed!" );
            return false;
         }
         winState.hGLRC = NULL;
      }

      // Release the device context:
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }

      // [neo, 5/31/2007 - #3174]
      if( winState.appMenu )
      {
         DestroyMenu( winState.appMenu );

         winState.appMenu = NULL;
      }

      // Destroy the window:
      if ( winState.appWindow )
      {
         Con::printf( "Destroying the window..." );
         DestroyWindow( winState.appWindow );
         winState.appWindow = NULL;
      }
   }
   else if ( smIsFullScreen != newFullScreen )
   {
      // Change the window style:
      Con::printf( "Changing the window style..." );
      S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      if ( newFullScreen )
         windowStyle |= ( WS_MAXIMIZE | WS_VISIBLE);
      else
         windowStyle |= ( WS_OVERLAPPEDWINDOW );

      if ( winState.appWindow && !SetWindowLong( winState.appWindow, GWL_STYLE, windowStyle ) )
         Con::errorf( "SetWindowLong failed to change the window style!" );
   }

   if( winState.appWindow && !newFullScreen )
      ShowWindow( winState.appWindow, SW_NORMAL );
   else
      ShowWindow( winState.appWindow, SW_MAXIMIZE );


   U32 test;
   if ( newFullScreen )
   {
      // Change the display settings:
      DEVMODE devMode;
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize       = sizeof( devMode );
      devMode.dmPelsWidth  = newRes.w;
      devMode.dmPelsHeight = newRes.h;
      devMode.dmBitsPerPel = newRes.bpp;
      devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

      Con::printf( "Changing the display settings to %dx%dx%d...", newRes.w, newRes.h, newRes.bpp );
      curtain = CreateCurtain( newRes.w, newRes.h );
      test = ChangeDisplaySettings( &devMode, CDS_FULLSCREEN );
      if ( test != DISP_CHANGE_SUCCESSFUL )
      {
         smIsFullScreen = false;
         Con::setBoolVariable( "$pref::Video::fullScreen", false );

         ChangeDisplaySettings( NULL, 0 );
         Con::errorf( ConsoleLogEntry::General, "OpenGLDevice::setScreenMode - ChangeDisplaySettings failed." );
         switch( test )
         {
            case DISP_CHANGE_RESTART:
               Platform::AlertOK( "Display Change Failed", "You must restart your machine to get the specified mode." );
               break;

            case DISP_CHANGE_BADMODE:
               Platform::AlertOK( "Display Change Failed", "The specified mode is not supported by this device." );
               break;

            default:
               Platform::AlertOK( "Display Change Failed", "Hardware failed to switch to the specified mode." );
               break;
         };
         DestroyWindow( curtain );
         return false;
      }
      else
         smIsFullScreen = true;
   }
   else if ( smIsFullScreen )
   {
      Con::printf( "Changing to the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
      smIsFullScreen = false;
   }

   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );


   bool newWindow = false;
   if ( !winState.appWindow )
   {
      Con::printf( "Creating a new %swindow...", ( fullScreen ? "full-screen " : "" ) );
      winState.appWindow = CreateOpenGLWindow( newRes.w, newRes.h, newFullScreen, true );
      if ( !winState.appWindow )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nFailed to create a new window!" );
         return false;
      }
      newWindow = true;
   }

   // Move the window:
   if ( !newFullScreen )
   {
      // Adjust the window rect to compensate for the window style:
      RECT windowRect;
      windowRect.left = windowRect.top = 0;
      windowRect.right = newRes.w;
      windowRect.bottom = newRes.h;

      AdjustWindowRect( &windowRect, GetWindowLong( winState.appWindow, GWL_STYLE ), false );
      U32 adjWidth = windowRect.right - windowRect.left;
      U32 adjHeight = windowRect.bottom - windowRect.top;

      // Center the window on the desktop:
      U32 xPos = 0, yPos = 0;

      if(adjWidth < (U32)winState.desktopClientWidth)
         xPos = ( winState.desktopClientWidth - adjWidth ) / 2;

      if(adjHeight < (U32)winState.desktopClientHeight)
         yPos = ( winState.desktopClientHeight - adjHeight ) / 2;

      test = SetWindowPos( winState.appWindow, 0, xPos, yPos, adjWidth, adjHeight, SWP_NOZORDER );
      if ( !test )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed trying to move the window to (%d,%d) and size it to %dx%d.", xPos, yPos, newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }

      AnimateWindow( winState.appWindow, 90, AW_BLEND );
   }
   else if ( !newWindow )
   {
      // Move and size the window to take up the whole screen:
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

   bool newDeviceContext = false;
   if ( !winState.appDC )
   {
      // Get a new device context:
      Con::printf( "Acquiring a new device context..." );
      winState.appDC = GetDC( winState.appWindow );
      if ( !winState.appDC )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nGetDC failed to get a valid device context!" );
         return false;
      }
      newDeviceContext = true;
   }

   if ( newWindow )
   {
      // Set the pixel format of the new window:
      PIXELFORMATDESCRIPTOR pfd;
      CreatePixelFormat( &pfd, newRes.bpp, 24, 8, false );
      S32 chosenFormat = ChooseBestPixelFormat( winState.appDC, &pfd );
      if ( !chosenFormat )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nNo valid pixel formats found!" );
         return false;
      }
      dwglDescribePixelFormat( winState.appDC, chosenFormat, sizeof( pfd ), &pfd );
      if ( !SetPixelFormat( winState.appDC, chosenFormat, &pfd ) )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\nFailed to set the pixel format!" );
         return false;
      }
      Con::printf( "Pixel format set:" );
      Con::printf( "  %d color bits, %d depth bits, %d stencil bits", pfd.cColorBits, pfd.cDepthBits, pfd.cStencilBits );
   }

   if ( !winState.hGLRC )
   {
      // Create a new rendering context:
      Con::printf( "Creating a new rendering context..." );
      winState.hGLRC = dwglCreateContext( winState.appDC );
      if ( !winState.hGLRC )
      {
         AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglCreateContext failed to create an OpenGL rendering context!" );
         return false;
      }

      // Make the new rendering context current:
      Con::printf( "Making the new rendering context current..." );
      if ( !dwglMakeCurrent( winState.appDC, winState.hGLRC ) )
      {
            AssertFatal( false, "OpenGLDevice::setScreenMode\ndwglMakeCurrent failed to make the rendering context current!" );
            return false;
      }

      // Just for kicks.  Seems a relatively central place to put this...
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      if ( needResurrect )
      {
         // Reload the textures:
         Con::printf( "Resurrecting the texture manager..." );
         Game->textureResurrect();
      }
   }

   // Just for kicks.  Seems a relatively central place to put this...
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   if ( newDeviceContext && gGLState.suppSwapInterval )
      setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ) );

   smCurrentRes = newRes;
   Platform::setWindowSize( newRes.w, newRes.h );
   char tempBuf[15];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d", smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   if( fullScreen )
      Con::setVariable( "$pref::Video::resolution", tempBuf );
   else
      Con::setVariable( "$pref::Video::windowedRes", tempBuf );

   if ( curtain )
      DestroyWindow( curtain );

   // Doesn't hurt to do this even it isn't necessary:
   ShowWindow( winState.appWindow, SW_SHOW );
   SetForegroundWindow( winState.appWindow );
   SetFocus( winState.appWindow );

   if (newFullScreen)
   {
      // Doesn't hurt to do this an extra time, and seems to help in some cases
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "OpenGLDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

    //Luma:	Clear window at first, as it is showing previous gl color buffer stuff.
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if ( repaint )
      Video::resetCanvas();

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
   dwglSwapBuffers( winState.appDC );
}


//------------------------------------------------------------------------------
const char* OpenGLDevice::getDriverInfo()
{
   // Output some driver info to the console:
   const char* vendorString   = (const char*) glGetString( GL_VENDOR );
   const char* rendererString = (const char*) glGetString( GL_RENDERER );
   const char* versionString  = (const char*) glGetString( GL_VERSION );
   const char* extensionsString = (const char*) glGetString( GL_EXTENSIONS );

   U32 bufferLen = ( vendorString ? dStrlen( vendorString ) : 0 )
                 + ( rendererString ? dStrlen( rendererString ) : 0 )
                 + ( versionString  ? dStrlen( versionString ) : 0 )
                 + ( extensionsString ? dStrlen( extensionsString ) : 0 )
                 + 4;

   char* returnString = Con::getReturnBuffer( bufferLen );
   dSprintf( returnString, bufferLen, "%s\t%s\t%s\t%s",
         ( vendorString ? vendorString : "" ),
         ( rendererString ? rendererString : "" ),
         ( versionString ? versionString : "" ),
         ( extensionsString ? extensionsString : "" ) );

   return( returnString );
}


//------------------------------------------------------------------------------
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
   U16 ramp[256*3];

   if (!GetDeviceGammaRamp(winState.appDC, ramp))
      return false;

   F32 csum = 0.0;
   U32 ccount = 0;

   for (U16 i = 0; i < 256; ++i)
   {
      if (i != 0 && ramp[i] != 0 && ramp[i] != 65535)
      {
         F64 b = (F64) i/256.0;
         F64 a = (F64) ramp[i]/65535.0;
         F32 c = (F32) (mLog(a)/mLog(b));

         csum += c;
         ++ccount;
      }
   }
   g = csum/ccount;

   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
   U16 ramp[256*3];

   for (U16 i = 0; i < 256; ++i)
      ramp[i] = (U16)((U16)mPow((F32) i/256.0f, g) * 65535.0f);
   dMemcpy(&ramp[256],ramp,256*sizeof(U16));
   dMemcpy(&ramp[512],ramp,256*sizeof(U16));

   return SetDeviceGammaRamp(winState.appDC, ramp);
}

//------------------------------------------------------------------------------
bool OpenGLDevice::getVerticalSync()
{
    if ( !gGLState.suppSwapInterval )
        return( false );

    //Note that dwglGetSwapIntervalEXT returns the number of frames between Swaps.
    //The function returns 0 / false if SwapInterval has not been specified.
    return (dwglGetSwapIntervalEXT());
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   if ( !gGLState.suppSwapInterval )
      return( false );

   return( dwglSwapIntervalEXT( on ? 1 : 0 ) );
}

//------------------------------------------------------------------------------
DisplayDevice* OpenGLDevice::create()
{

   // Get Bit Depth Switching Info.
   enumerateBitDepths();

   bool result = false;
   bool fullScreenOnly = false;

   //------------------------------------------------------------------------------
   // This shouldn't happen, but just to be safe...
   //------------------------------------------------------------------------------
   if ( winState.hinstOpenGL )
      GL_Shutdown();

   //------------------------------------------------------------------------------
   // Initialize GL
   //------------------------------------------------------------------------------
   if (!GL_Init( "opengl32", "glu32" ))
      return NULL;

   //------------------------------------------------------------------------------
   // Create a test window to see if OpenGL hardware acceleration is available:
   //------------------------------------------------------------------------------
   WNDCLASS wc;
   dMemset(&wc, 0, sizeof(wc));
   wc.style         = CS_OWNDC;
   wc.lpfnWndProc   = DefWindowProc;
   wc.hInstance     = winState.appInstance;
   wc.lpszClassName = dT("OGLTest");
   RegisterClass( &wc );

   //------------------------------------------------------------------------------
   // Create the Test Window
   //------------------------------------------------------------------------------
   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   HWND testWindow = CreateWindow( dT("OGLTest"),dT(""), WS_POPUP, 0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y, NULL, NULL, winState.appInstance, NULL );
   if ( !testWindow )
   {
      // Unregister the Window Class
      UnregisterClass( dT("OGLTest"), winState.appInstance );

      // Shutdown GL
      GL_Shutdown();

      // Return Failure
      return NULL;
   }

   //------------------------------------------------------------------------------
   // Attempt to Grab a handle to the DeviceContext of our window.
   //------------------------------------------------------------------------------
   HDC testDC = GetDC( testWindow );
   if ( !testDC )
   {
      // Destroy the Window
      DestroyWindow( testWindow );

      // Unregister the Window Class
      UnregisterClass( dT("OGLTest"), winState.appInstance );

      // Shutdown GL
      GL_Shutdown();

      // Return Failure.
      return NULL;
   }

   //------------------------------------------------------------------------------
   // Create Pixel Format ( Default 16bpp )
   //------------------------------------------------------------------------------
   PIXELFORMATDESCRIPTOR pfd;
   CreatePixelFormat( &pfd, 16, 16, 8, false );


   U32 chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
   if ( chosenFormat != 0 )
   {
      dwglDescribePixelFormat( testDC, chosenFormat, sizeof( pfd ), &pfd );

      result = !( pfd.dwFlags & PFD_GENERIC_FORMAT );

      if ( result && winState.desktopBitsPixel < 16 && !smCanDo32Bit)
      {
         // If Windows 95 cannot switch bit depth, it should only attempt 16-bit cards
         // with a 16-bit desktop

         // See if we can get a 32-bit pixel format:
         PIXELFORMATDESCRIPTOR pfd;

         CreatePixelFormat( &pfd, 32, 24, 8, false );
         S32 chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
         if ( chosenFormat != 0 )
         {
            dwglDescribePixelFormat( winState.appDC, chosenFormat, sizeof( pfd ), &pfd );

            if (pfd.cColorBits == 16)
            {
               Platform::AlertOK("Requires 16-Bit Desktop",
                  "You must run in 16-bit color to run a Torque game.\nPlease quit the game, set your desktop color depth to 16-bit\nand then restart the application.");

               result = false;
            }
         }
      }
   }
   else if ( winState.desktopBitsPixel < 16 && smCanSwitchBitDepth )
   {
      // Try again after changing the display to 16-bit:
      ReleaseDC( testWindow, testDC );
      DestroyWindow( testWindow );

      DEVMODE devMode;
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize       = sizeof( devMode );
      devMode.dmBitsPerPel = 16;
      devMode.dmFields     = DM_BITSPERPEL;

      U32 test = ChangeDisplaySettings( &devMode, 0 );
      if ( test == DISP_CHANGE_SUCCESSFUL )
      {
    //MIN_RESOLUTION defined in platformWin32/platformGL.h
         testWindow = CreateWindow( dT("OGLTest"), dT(""), WS_OVERLAPPED | WS_CAPTION, 0, 0, MIN_RESOLUTION_X, MIN_RESOLUTION_Y, NULL, NULL, winState.appInstance, NULL );
         if ( testWindow )
         {
            testDC = GetDC( testWindow );
            if ( testDC )
            {
               CreatePixelFormat( &pfd, 16, 16, 8, false );
               chosenFormat = ChooseBestPixelFormat( testDC, &pfd );
               if ( chosenFormat != 0 )
               {
                  dwglDescribePixelFormat( testDC, chosenFormat, sizeof( pfd ), &pfd );

                  result = !( pfd.dwFlags & PFD_GENERIC_FORMAT );
                  if ( result )
                     fullScreenOnly = true;
               }
            }
         }
      }
      ChangeDisplaySettings( NULL, 0 );
   }
   //------------------------------------------------------------------------------
   // Can't do even 16 bit, alert user they need to upgrade.
   //------------------------------------------------------------------------------
   else if ( winState.desktopBitsPixel < 16 && !smCanSwitchBitDepth )
   {
      Platform::AlertOK("Requires 16-Bit Desktop", "You must run in 16-bit color to run a Torque game.\nPlease quit the game, set your desktop color depth to 16-bit\nand then restart the application.");
   }

   ReleaseDC( testWindow, testDC );
   DestroyWindow( testWindow );

   UnregisterClass( dT("OGLTest"), winState.appInstance );

   GL_Shutdown();

   if ( result )
   {
      OpenGLDevice* newOGLDevice = new OpenGLDevice();
      if ( newOGLDevice )
      {
         newOGLDevice->mFullScreenOnly = fullScreenOnly;
         return (DisplayDevice*) newOGLDevice;
      }
      else
         return NULL;
   }
   else
      return NULL;
}
