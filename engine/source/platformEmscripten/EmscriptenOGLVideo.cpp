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
#include "game/gameInterface.h"

#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/platformGL.h"
#include "platformEmscripten/EmscriptenOGLVideo.h"

#include <SDL.h>
#include <emscripten/html5.h>

// The canvas the SDL 2 port draws into and reads input from: it always uses
// the page element with this id.
static const char* CanvasId = "#canvas";

//------------------------------------------------------------------------------
bool InitOpenGL()
{
   DisplayDevice::init();

   // The canvas starts at the size the game is about to ask for -- the choice
   // canvas.cs makes, $pref::Video::windowedRes or else defaultResolution, as
   // on Linux -- unless the page sizes it (see CreateGLWindow). This used to
   // read $pref::Video::resolution, which nothing sets before the canvas
   // exists.
   const char* resString = Con::getVariable( "$pref::Video::windowedRes" );
   if ( resString[0] == '\0' )
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

   // A page cannot make itself fullscreen: the browser allows that only in
   // answer to the player's click or key, and there has been neither while the
   // page loads. So the canvas starts in the page whatever
   // $pref::Video::fullScreen says; a fullscreen toggle bound to a key or a
   // button works.
   if ( !Video::setDevice( "OpenGL", width, height, bpp, false ) )
   {
      Con::errorf("Unable to create default OpenGL mode: %d %d %d",
         width, height, bpp);

      // if we can't create the default, attempt to create a "safe" window
      if ( !Video::setDevice( "OpenGL", 640, 480, 16, false ) )
      {
         Platform::AlertOK("Error", "Could not find a compatible OpenGL display " \
            "resolution.  Please check your driver configuration.");
         return false;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::smCanSwitchBitDepth = false;

// The GL context. It is made with the window and lives as long as the page
// does: SDL 2 resizes the canvas, and takes it in and out of fullscreen,
// around the context it already has, so no mode change here costs a texture
// reload. SDL 1.2's SDL_SetVideoMode made a new context every time.
static SDL_GLContext sGLContext = NULL;

// Whether the page sizes the canvas with CSS -- a canvas styled to fill the
// browser window, say -- rather than letting it be as big as its drawing
// buffer. If it does, the page decides the canvas's size and the game follows
// it; if not, the game decides.
static bool sPageSizesCanvas = false;

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
   Resolution res = Video::getDesktopResolution();
   Point2I desktopSize = Point2I(res.w, res.h);
   U32 desktopBpp = res.bpp;

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

   if (Con::getBoolVariable("$pref::Unix::CanSwitchBitDepth"))
      smCanSwitchBitDepth = true;

   // add some default resolutions
   addResolution(640, 480);
   addResolution(800, 600);
   addResolution(1024, 768);
   addResolution(1152, 864);
   addResolution(1280, 1024);
   addResolution(1600, 1200);

   // ...and the screen's own size, which is the one mode SDL 2's Emscripten
   // driver lists.
   const int numModes = SDL_GetNumDisplayModes(0);
   for (int i = 0; i < numModes; ++i)
   {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(0, i, &mode) != 0)
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

   getGLCapabilities();

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

   SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuf);
   SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &bufferSize);
   SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depthSize);
   SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencilSize);
   SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red);
   SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &green);
   SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &blue);
   SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha);

   Con::printf("OpenGL Attributes:");
   Con::printf("  DoubleBuffer: %d", doubleBuf);
   Con::printf("  BufferSize: %d, DepthSize: %d, StencilSize: %d",
      bufferSize, depthSize, stencilSize);
   Con::printf("  Red: %d, Green: %d, Blue: %d, Alpha: %d",
      red, green, blue, alpha);
}

//------------------------------------------------------------------------------
// Whether the page's CSS sizes the canvas. This is the test SDL makes when it
// creates the window, made here too because SDL keeps the answer to itself:
// shrink the drawing buffer to a pixel, and see whether the canvas's size on
// the page shrinks with it. Asked before the window exists, when changing the
// buffer disturbs nothing.
static bool PageSizesCanvas()
{
   double cssWidth, cssHeight;
   emscripten_set_canvas_element_size( CanvasId, 1, 1 );
   if ( emscripten_get_element_css_size( CanvasId, &cssWidth, &cssHeight ) != EMSCRIPTEN_RESULT_SUCCESS )
      return false;

   return mFloor( cssWidth ) != 1 || mFloor( cssHeight ) != 1;
}

//------------------------------------------------------------------------------
// Make the window and its GL context: once, for the first setScreenMode.
static bool CreateGLWindow( U32 width, U32 height )
{
   // The GL attributes are read when the context is made. No context version
   // or profile is asked for: SDL 2's Emscripten driver then makes a WebGL 1
   // context (GLES 2.0), the only kind LEGACY_GL_EMULATION can draw the
   // fixed-function renderer through.
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

   sPageSizesCanvas = PageSizesCanvas();

   // Always resizable, since SDL follows the browser window only for a
   // resizable one -- and only when the page sizes the canvas; otherwise the
   // flag does nothing. A page-sized canvas is sized to the page here, by SDL,
   // whatever size is asked for.
   //
   // Not SDL_WINDOW_ALLOW_HIGHDPI: that makes the drawing buffer bigger than
   // the canvas's size on the page by the screen's pixel ratio, while the
   // window size, the mouse and so the whole GUI stay in page pixels. On a
   // high-DPI screen the browser scales the canvas up instead.
   SDL_Window* window = SDL_CreateWindow( gPlatState.appWindowTitle,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
   if ( window == NULL )
   {
      Con::printf( "Unable to create the window: %s", SDL_GetError() );
      return false;
   }

   // The context must come from SDL. Its Emscripten driver makes it through
   // Emscripten's EGL, and making it that way is what starts
   // LEGACY_GL_EMULATION's immediate-mode emulation, which the renderer's
   // glBegin/glEnd and vertex-array drawing go through. A context made with
   // emscripten_webgl_create_context skips that.
   sGLContext = SDL_GL_CreateContext( window );
   if ( sGLContext == NULL )
   {
      Con::printf( "Unable to create an OpenGL context: %s", SDL_GetError() );
      SDL_DestroyWindow( window );
      return false;
   }

   if ( SDL_GL_MakeCurrent( window, sGLContext ) != 0 )
   {
      Con::printf( "Unable to make the OpenGL context current: %s", SDL_GetError() );
      SDL_GL_DeleteContext( sGLContext );
      sGLContext = NULL;
      SDL_DestroyWindow( window );
      return false;
   }

   gPlatState.sdlWindow = window;
   gPlatState.windowCreated = true;
   return true;
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

   const U32 desktopDepth = Video::getDesktopResolution().bpp;

   if (bpp == 0)
   {
      // bpp comes in as "0" when it is set to "Default"
      bpp = desktopDepth;
   }

   if (height == 0 || width == 0)
   {
      // paranoia check.  set it to the default to prevent crashing
      width = 800;
      height = 600;
   }

   // if we can't switch bit depths and the requested bpp is not equal to
   // the desktop bpp, set bpp to the desktop bpp
   if (!smCanSwitchBitDepth &&
      bpp != desktopDepth)
   {
      bpp = desktopDepth;
   }

   // No size is refused, as on Linux: the canvas can be any size, and
   // fullscreen is always the screen's size whatever size is asked for. The
   // SDL 1.2 back-end refused any size not in the resolution list.

   SDL_Window* window = gPlatState.sdlWindow;
   if ( window == NULL )
   {
      Con::printf( "Setting screen mode to %dx%dx%d (w%s)...", width, height,
         bpp, ( sPageSizesCanvas ? ", sized by the page" : "" ) );

      if ( !CreateGLWindow( width, height ) )
         return false;
      window = gPlatState.sdlWindow;

      if ( sPageSizesCanvas )
         Con::printf( "The page sizes the canvas; the game follows it." );

      PrintGLAttributes();

      // clear screen here to prevent buffer garbage from being displayed when
      // the canvas first shows
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      // (InitOpenGL never asks for fullscreen here, and no one else can: the
      // canvas does not exist before this.)
   }
   else
   {
      const bool wasFullScreen = isWindowFullScreen();

      Con::printf( "Setting screen mode to %dx%dx%d (%s)...", width, height,
         bpp, ( fullScreen ? "fs" : "w" ) );

      // Going fullscreen waits on the player. SDL asks the browser for it at
      // once, but the browser grants it only in answer to a click or a key, so
      // the request waits for the next one -- which, when the toggle is bound
      // to a key or a button, is the press that asked for it. Leaving
      // fullscreen needs no permission. Either way the canvas's new size
      // arrives later as a resize, and followWindow picks it up.
      if ( fullScreen != wasFullScreen )
         SDL_SetWindowFullscreen( window, fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 );

      // A windowed size is the game's to choose only when the page leaves it
      // the choice. A page-sized canvas keeps the page's size -- setting it
      // here would only change the drawing buffer, which the page would then
      // stretch to fit -- so the game draws at the page's size, as it draws at
      // a tile's size under a tiling window manager. Coming out of fullscreen
      // asks for no size either: the canvas goes back to the size it had.
      if ( !fullScreen && !wasFullScreen && !sPageSizesCanvas )
      {
         S32 currentWidth, currentHeight;
         SDL_GetWindowSize( window, &currentWidth, &currentHeight );
         if ( currentWidth != (S32)width || currentHeight != (S32)height )
            SDL_SetWindowSize( window, width, height );
      }
   }

   // The canvas draws at the size it is now, which is not always the size
   // asked for: the page may size it, and a fullscreen canvas is the screen's
   // size once the browser has made it so.
   S32 windowWidth, windowHeight;
   SDL_GetWindowSize( window, &windowWidth, &windowHeight );

   smCurrentRes = Resolution( windowWidth, windowHeight, bpp );
   Platform::setWindowSize( windowWidth, windowHeight );
   smIsFullScreen = isWindowFullScreen();
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
void OpenGLDevice::followWindow()
{
   SDL_Window* window = gPlatState.sdlWindow;
   if ( window == NULL )
      return;

   S32 width, height;
   SDL_GetWindowSize( window, &width, &height );
   const bool fullScreen = isWindowFullScreen();

   if ( width == smCurrentRes.w && height == smCurrentRes.h && fullScreen == smIsFullScreen )
      return;

   Con::printf( "Following the canvas to %dx%d (%s)", width, height,
      ( fullScreen ? "fs" : "w" ) );

   // Platform::getWindowSize is what the canvas sizes itself to
   // (GuiCanvas::maintainSizing) and the viewport is set from (dglSetClipRect).
   // SDL has already resized the drawing buffer to match.
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
// SDL's flags are right here, unlike on X11: SDL makes every change itself,
// and hears from the browser when the player leaves fullscreen with Esc.
bool OpenGLDevice::isWindowFullScreen()
{
   SDL_Window* window = gPlatState.sdlWindow;
   if ( window == NULL )
      return false;

   return ( SDL_GetWindowFlags( window ) & SDL_WINDOW_FULLSCREEN ) != 0;
}

//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
   SDL_Window* window = gPlatState.sdlWindow;
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
// A page has no say over the screen's gamma ramp, and SDL 2's Emscripten
// driver has none to set. (SDL 1.2's SDL_SetGammaRamp did nothing either.)
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
   return false;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
   return false;
}

//------------------------------------------------------------------------------
// On the web, vertical sync is a matter of when the browser runs the main
// loop, not of the context: SDL_GL_SetSwapInterval sets it with
// emscripten_set_main_loop_timing, which takes only once the main loop exists,
// and the device is activated from mainInitialize, before main.cpp starts it.
// So it is left alone, and the loop runs at the rate main.cpp gives it
// (getGLCapabilities reports no swap interval).
bool OpenGLDevice::getVerticalSync()
{
   return false;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   return false;
}

//------------------------------------------------------------------------------
DisplayDevice* OpenGLDevice::create()
{
   return new OpenGLDevice();
}
