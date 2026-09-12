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



#include "console/console.h"
#include "platform/event.h"
#include "platform/types.h"
#include "game/gameInterface.h"

#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/platformGL.h"
#include "platformX86UNIX/x86UNIXOGLVideo.h"
#include "platformX86UNIX/x86UNIXState.h"

#include <SDL.h>
#include <SDL_syswm.h>

//------------------------------------------------------------------------------
bool InitOpenGL()
{
   DisplayDevice::init();

   // Create the window at the size the game is about to ask for -- the choice
   // canvas.cs makes: $pref::Video::windowedRes, or defaultResolution when
   // that is empty.  This used to read $pref::Video::resolution, which nothing
   // sets before the canvas exists, so the window first appeared at an 800x600
   // placeholder and was resized a moment later.  The first map is the one a
   // tiling window manager remembers: that placeholder was the size Hyprland
   // floated the window at.
   //
   // An empty windowedRes is also how a game says it has no preference, which
   // leaves a tiling window manager free to tile the window.  A windowedRes is
   // a size the game wants exactly; see OpenGLDevice::setScreenMode.
   const char* resString = Con::getVariable( "$pref::Video::windowedRes" );
   OpenGLDevice::smCreateAtExactSize = ( resString[0] != '\0' );
   if ( !OpenGLDevice::smCreateAtExactSize )
      resString = Con::getVariable( "$pref::Video::defaultResolution" );
   char* tempBuf = new char[dStrlen( resString ) + 1];
   dStrcpy( tempBuf, resString );
   char* temp = dStrtok( tempBuf, " x\0" );
   U32 width = ( temp ? dAtoi( temp ) : 800 );
   temp = dStrtok( NULL, " x\0" );
   U32 height = ( temp ? dAtoi( temp ) : 600 );
   temp = dStrtok( NULL, "\0" );
   U32 bpp = ( temp ? dAtoi( temp ) : 16 );
   delete [] tempBuf;

   bool fullScreen = Con::getBoolVariable( "$pref::Video::fullScreen" );

   // the only supported video device in unix is OpenGL
   if ( !Video::setDevice( "OpenGL", width, height, bpp, fullScreen ) )
   {
      Con::errorf("Unable to create default OpenGL mode: %d %d %d %d",
         width, height, bpp, fullScreen);

      // if we can't create the default, attempt to create a "safe" window
      if ( !Video::setDevice( "OpenGL", 640, 480, 16, true ) )
      {
         DisplayErrorAlert("Could not find a compatible OpenGL display " \
            "resolution.  Please check your driver configuration.");
         return false;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::smCanSwitchBitDepth = false;
bool OpenGLDevice::smCreateAtExactSize = false;
bool OpenGLDevice::smHoldingExactSize = false;
U32  OpenGLDevice::smExactSizeHeldSince = 0;

// The GL context. It is made with the window and lives as long as the window
// does: SDL 2 resizes a window, and takes it in and out of fullscreen, around
// the context it already has, so no mode change here costs a texture reload.
// SDL 1.2's SDL_SetVideoMode rebuilt both, every time.
static SDL_GLContext sGLContext = NULL;

// How long a window created at an exact size is held at it; see
// releaseExactSize.
static const U32 ExactSizeHoldMs = 500;

//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
   initDevice();
}

//------------------------------------------------------------------------------
OpenGLDevice::~OpenGLDevice()
{
}

//------------------------------------------------------------------------------
void OpenGLDevice::addResolution(S32 width, S32 height, bool check)
{
   Point2I desktopSize = x86UNIXState->getDesktopSize();
   U32 desktopBpp = x86UNIXState->getDesktopBpp();

   // don't allow any resolution under this size
   if (width < 640 || height < 480)
      return;

   if (check)
   {
      // don't allow resolutions that exceed the current desktop size
      if (width > desktopSize.x || height > desktopSize.y)
         return;
   }

   if (smCanSwitchBitDepth)
   {
      // add both 16 and 32 bit resolutions
      mResolutionList.push_back(Resolution(width, height, 16));
      mResolutionList.push_back(Resolution(width, height, 32));
   }
   else
   {
      // add just the desktop resolution
      mResolutionList.push_back(Resolution(width, height, desktopBpp));
   }
}

//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
   mDeviceName = "OpenGL";
   mFullScreenOnly = false;
}

//------------------------------------------------------------------------------
void OpenGLDevice::loadResolutions()
{
   mResolutionList.clear();

   // X cannot switch bit depths on the fly.  In case this feature is
   // implemented someday, calling this function will let you take
   // advantage of it
   if (Con::getBoolVariable("$pref::Unix::CanSwitchBitDepth"))
      smCanSwitchBitDepth = true;

   // add some default resolutions
   addResolution(640, 480);
   addResolution(800, 600);
   addResolution(1024, 768);
   addResolution(1152, 864);
   addResolution(1280, 1024);
   addResolution(1600, 1200);

   // ...and the modes the display offers. SDL lists a size once for each
   // refresh rate it can be shown at; this is a list of sizes.
   SDL_Window* window = x86UNIXState->getSDLWindow();
   int displayIndex = window ? SDL_GetWindowDisplayIndex(window) : 0;
   if (displayIndex < 0)
      displayIndex = 0;

   const int numModes = SDL_GetNumDisplayModes(displayIndex);
   for (int i = 0; i < numModes; ++i)
   {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(displayIndex, i, &mode) != 0)
         continue;

      // do we already have this mode?
      bool found = false;
      for (Vector<Resolution>::iterator iter = mResolutionList.begin();
           iter != mResolutionList.end();
           ++iter)
      {
         if (iter->w == mode.w && iter->h == mode.h)
         {
            found = true;
            break;
         }
      }
      if (!found)
         // don't check these resolutions because they should be OK
         // (and checking might drop resolutions that are higher than the
         // current desktop bpp)
         addResolution(mode.w, mode.h, false);
   }
}

//------------------------------------------------------------------------------
bool OpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   if (!setScreenMode(width, height, bpp, fullScreen))
   {
      Con::printf("Unable to set screen mode.");
      return false;
   }

   // Output some driver info to the console
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

   GL_EXT_Init();

   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

   // Do this here because we now know about the extensions:
   if ( gGLState.suppSwapInterval )
      setVerticalSync(
         !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ) );
   Con::setBoolVariable("$pref::OpenGL::allowTexGen", true);

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
   // Shutdown is deferred to Platform::shutdown()
}

//------------------------------------------------------------------------------
static void PrintGLAttributes()
{
   int doubleBuf;
   int bufferSize, depthSize, stencilSize;
   int red, green, blue, alpha;
   int aRed, aGreen, aBlue, aAlpha;

   SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuf);
   SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &bufferSize);
   SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
   SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
   SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red);
   SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green);
   SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue);
   SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha);
   SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE, &aRed);
   SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE, &aGreen);
   SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE, &aBlue);
   SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, &aAlpha);

   Con::printf("OpenGL Attributes:");
   Con::printf("  DoubleBuffer: %d", doubleBuf);
   Con::printf("  BufferSize: %d, DepthSize: %d, StencilSize: %d",
      bufferSize, depthSize, stencilSize);
   Con::printf("  Red: %d, Green: %d, Blue: %d, Alpha: %d",
      red, green, blue, alpha);
   Con::printf("  Accum Red: %d, Green: %d, Blue: %d, Alpha: %d",
      aRed, aGreen, aBlue, aAlpha);
}

//------------------------------------------------------------------------------
// Make the window and its GL context: once, for the first setScreenMode.
static bool CreateGLWindow( U32 width, U32 height, bool fullScreen, bool exactSize )
{
   // The GL attributes choose the window's visual, so they are read when the
   // window is made and have to be set before it. No context version or profile
   // is asked for: the renderer is fixed-function, and SDL's default context
   // is a compatibility one.
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
// JMQ: NVIDIA 2802+ doesn't like this setting for stencil size
//   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);

   // Fullscreen is the window manager's: a borderless window over the whole
   // display at the desktop's own mode, never a mode switch.
   Uint32 flags = SDL_WINDOW_OPENGL;
   if ( fullScreen )
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
   if ( !exactSize )
      flags |= SDL_WINDOW_RESIZABLE;

   SDL_Window* window = SDL_CreateWindow( x86UNIXState->getWindowName(),
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags );
   if ( window == NULL )
   {
      Con::printf( "Unable to create the window: %s", SDL_GetError() );
      return false;
   }

   sGLContext = SDL_GL_CreateContext( window );
   if ( sGLContext == NULL )
   {
      Con::printf( "Unable to create an OpenGL context: %s", SDL_GetError() );
      SDL_DestroyWindow( window );
      return false;
   }

   x86UNIXState->setSDLWindow( window );
   x86UNIXState->setWindowCreated( true );
   return true;
}

//------------------------------------------------------------------------------
// Take the window in or out of fullscreen.
//
// SDL's own record of whether the window is fullscreen goes stale whenever the
// window manager changes it (SUPER+F in Hyprland): SDL reads _NET_WM_STATE, but
// only for whether the window is shown or maximized. And SDL_SetWindowFullscreen
// does nothing when asked for the state SDL believes the window is in already.
// So when SDL's record disagrees with the window, it is first told the truth --
// asked for the state the window is really in, which changes nothing on screen
// -- and then asked for the change. Each call waits on the window manager for a
// moment (about a tenth of a second on Hyprland), which is fine for a switch
// the player asked for.
static void SetWindowFullScreen( SDL_Window* window, bool fullScreen )
{
   const bool sdlThinksFullScreen = ( SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN ) != 0;
   if ( sdlThinksFullScreen == fullScreen )
      SDL_SetWindowFullscreen( window, fullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP );

   SDL_SetWindowFullscreen( window, fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 );
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp,
   bool fullScreen, bool forceIt, bool repaint )
{
   // load resolutions, this is done lazily so that we can check the setting
   // of smCanSwitchBitDepth, which may be overridden by console
   if (mResolutionList.size()==0)
      loadResolutions();

   if (mResolutionList.size()==0)
   {
      Con::printf("No resolutions available!");
      return false;
   }

   if (bpp == 0)
   {
      // bpp comes in as "0" when it is set to "Default"
      bpp = x86UNIXState->getDesktopBpp();
   }

   if (height == 0 || width == 0)
   {
      // paranoia check.  set it to the default to prevent crashing
      width = 800;
      height = 600;
   }

   U32 desktopDepth = x86UNIXState->getDesktopBpp();
   // if we can't switch bit depths and the requested bpp is not equal to
   // the desktop bpp, set bpp to the desktop bpp
   if (!smCanSwitchBitDepth &&
      bpp != desktopDepth)
   {
      bpp = desktopDepth;
   }

   // No size is refused. A window can be any size, and fullscreen is always the
   // desktop's own mode (SDL_WINDOW_FULLSCREEN_DESKTOP) whatever size is asked
   // for -- which matters, because Video::toggleFullScreen asks for the size the
   // window has now, and after a drag or a tile that is rarely one the display
   // lists. Under SDL 1.2 fullscreen switched the display's mode, so it had to
   // be one of them.

   SDL_Window* window = x86UNIXState->getSDLWindow();
   if ( window == NULL )
   {
      // A window the game wants at an exact size is created fixed-size: SDL
      // sets its minimum and maximum size hints to that size before the window
      // is first mapped, which is what makes a tiling window manager (Hyprland,
      // i3, sway) float it at that size instead of tiling it; a stacking window
      // manager just sees a window open at the size it asked for. It is held
      // fixed-size until releaseExactSize lets go of it, so the player can then
      // resize it. Letting go straight away loses the race: the window manager
      // decides a moment after the window maps, and by then the hints would be
      // gone. Only ever at creation: a window already mapped and tiled cannot be
      // floated.
      const bool exactSize = !fullScreen && smCreateAtExactSize;

      Con::printf( "Setting screen mode to %dx%dx%d (%s)...", width, height,
         bpp, ( fullScreen ? "fs" : ( exactSize ? "w, exact" : "w" ) ) );

      if ( !CreateGLWindow( width, height, fullScreen, exactSize ) )
         return false;
      window = x86UNIXState->getSDLWindow();

      if ( exactSize )
      {
         smHoldingExactSize = true;
         smExactSizeHeldSince = Platform::getRealMilliseconds();
      }

      PrintGLAttributes();

      // clear screen here to prevent buffer garbage from being displayed when
      // the window first shows
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   }
   else
   {
      const bool wasFullScreen = isWindowFullScreen();

      Con::printf( "Setting screen mode to %dx%dx%d (%s)...", width, height,
         bpp, ( fullScreen ? "fs" : "w" ) );

      if ( fullScreen != wasFullScreen )
         SetWindowFullScreen( window, fullScreen );

      // A windowed size is a request, which a tiling window manager may well
      // refuse and leave the window at the size it gave it. Coming out of
      // fullscreen asks for none: the window goes back to the size it had
      // before -- and Video::toggleFullScreen passes the fullscreen size along,
      // which is no one's choice of window size.
      if ( !fullScreen && !wasFullScreen )
      {
         S32 currentWidth, currentHeight;
         SDL_GetWindowSize( window, &currentWidth, &currentHeight );
         if ( currentWidth != (S32)width || currentHeight != (S32)height )
            SDL_SetWindowSize( window, width, height );
      }
   }

   // The canvas draws at the size the window is now, which is not always the
   // size asked for: a fullscreen window is the display's size, and a tiled
   // one is the tile's.
   S32 windowWidth, windowHeight;
   SDL_GetWindowSize( window, &windowWidth, &windowHeight );

   smCurrentRes = Resolution( windowWidth, windowHeight, bpp );
   Platform::setWindowSize( windowWidth, windowHeight );
   smIsFullScreen = fullScreen;
   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );
   char tempBuf[32];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d",
      smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   Con::setVariable( "$pref::Video::resolution", tempBuf );

   // repaint
   if ( repaint )
      Video::resetCanvas();

   return true;
}

//------------------------------------------------------------------------------
void OpenGLDevice::releaseExactSize()
{
   if ( !smHoldingExactSize ||
        Platform::getRealMilliseconds() - smExactSizeHeldSince < ExactSizeHoldMs )
      return;

   smHoldingExactSize = false;

   SDL_Window* window = x86UNIXState->getSDLWindow();
   if ( window == NULL )
      return;

   // The window manager has placed the window by now. Clearing the size hints
   // leaves it where it is, and lets the player resize it.
   SDL_SetWindowResizable( window, SDL_TRUE );
   Con::printf( "Window released from its exact size after %d ms; it can be resized now.",
      ExactSizeHoldMs );
}

//------------------------------------------------------------------------------
void OpenGLDevice::followWindow()
{
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if ( window == NULL )
      return;

   S32 width, height;
   SDL_GetWindowSize( window, &width, &height );
   const bool fullScreen = isWindowFullScreen();

   if ( width == smCurrentRes.w && height == smCurrentRes.h && fullScreen == smIsFullScreen )
      return;

   Con::printf( "Following the window to %dx%d (%s)", width, height,
      ( fullScreen ? "fs" : "w" ) );

   // Platform::getWindowSize is what the canvas sizes itself to
   // (GuiCanvas::maintainSizing) and the viewport is set from (dglSetClipRect).
   smCurrentRes.w = width;
   smCurrentRes.h = height;
   Platform::setWindowSize( width, height );

   smIsFullScreen = fullScreen;
   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );

   char tempBuf[32];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d",
      smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   Con::setVariable( "$pref::Video::resolution", tempBuf );
}

//------------------------------------------------------------------------------
// On X11 the answer is the window manager's, read off the window's
// _NET_WM_STATE: SDL's flags only know about the switches SDL itself made (see
// SetWindowFullScreen). It is read through SDL's own connection to the X server
// -- no second one, and only here, where the driver is known to be X11.
bool OpenGLDevice::isWindowFullScreen()
{
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if ( window == NULL )
      return false;

#if defined(SDL_VIDEO_DRIVER_X11)
   SDL_SysWMinfo info;
   SDL_VERSION( &info.version );
   if ( SDL_GetWindowWMInfo( window, &info ) && info.subsystem == SDL_SYSWM_X11 )
   {
      Display* display = info.info.x11.display;
      const Atom netWMState = XInternAtom( display, "_NET_WM_STATE", True );
      const Atom netWMStateFullScreen = XInternAtom( display, "_NET_WM_STATE_FULLSCREEN", True );

      // Atoms that do not exist yet mean no window manager has set the state
      // on any window, this one included.
      if ( netWMState == None || netWMStateFullScreen == None )
         return false;

      bool fullScreen = false;
      Atom type;
      int format;
      unsigned long count, remaining;
      unsigned char* data = NULL;
      if ( XGetWindowProperty( display, info.info.x11.window, netWMState, 0, 64,
             False, XA_ATOM, &type, &format, &count, &remaining, &data ) == Success &&
           data != NULL )
      {
         const Atom* atoms = reinterpret_cast<const Atom*>( data );
         for ( unsigned long i = 0; i < count; ++i )
            if ( atoms[i] == netWMStateFullScreen )
               fullScreen = true;
         XFree( data );
      }
      return fullScreen;
   }
#endif

   return ( SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN ) != 0;
}

//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if ( window )
      SDL_GL_SwapWindow( window );
}

//------------------------------------------------------------------------------
const char* OpenGLDevice::getDriverInfo()
{
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
   U16 redtable[256];
   U16 greentable[256];
   U16 bluetable[256];

   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window == NULL ||
       SDL_GetWindowGammaRamp(window, redtable, greentable, bluetable) == -1)
   {
      Con::warnf("getGammaCorrection error: %s", SDL_GetError());
      return false;
   }

   F32 csum = 0.0;
   U32 ccount = 0;

   for (U16 i = 0; i < 256; ++i)
   {
      if (i != 0 && redtable[i] != 0 && redtable[i] != 65535)
      {
         F64 b = (F64) i/256.0;
         F64 a = (F64) redtable[i]/65535.0;
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
   U16 redtable[256];
   U16 greentable[256];
   U16 bluetable[256];

   for (U16 i = 0; i < 256; ++i)
      redtable[i] = static_cast<U16>(mPow((F32) i/256.0f, g) * 65535.0f);
   dMemcpy(greentable,redtable,256*sizeof(U16));
   dMemcpy(bluetable,redtable,256*sizeof(U16));

   SDL_Window* window = x86UNIXState->getSDLWindow();
   S32 ok = window ? SDL_SetWindowGammaRamp(window, redtable, greentable, bluetable) : -1;
   if (ok == -1)
      Con::warnf("Error setting gamma correction: %s", SDL_GetError());

   return ok != -1;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::getVerticalSync()
{
   return SDL_GL_GetSwapInterval() != 0;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   return SDL_GL_SetSwapInterval( on ? 1 : 0 ) == 0;
}

//------------------------------------------------------------------------------
DisplayDevice* OpenGLDevice::create()
{
   return new OpenGLDevice();
}
