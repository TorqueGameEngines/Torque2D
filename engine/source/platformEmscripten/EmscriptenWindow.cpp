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



#include "console/console.h"
#include "io/fileStream.h"
#include "game/resource.h"
#include "game/version.h"
#include "math/mRandom.h"
#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/EmscriptenConsole.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platform.h"
#include "platform/platformAL.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "debug/profiler.h"
#include "platformEmscripten/platformGL.h"
#include "platformEmscripten/EmscriptenOGLVideo.h"

#ifndef DEDICATED
#include "platformEmscripten/EmscriptenInputManager.h"
#include "platformSDL/sdlTextInput.h"
#endif

#include <stdlib.h>

#ifndef DEDICATED
#include <SDL.h>

extern bool InitOpenGL();
extern void Cleanup(bool minimal=false);
#endif

EmscriptenPlatState::EmscriptenPlatState()
{
    captureDisplay = true;
    fadeWindows = true;
    backgrounded = false;
    minimized = false;
    mouseLocked = false;

    quit = false;

    portrait = true;//-Mat Android is in portrait mode by default


    // start with something reasonable.
    fullscreen = true;

    osVersion = 0;

    dStrcpy(appWindowTitle, "Emscripten Torque Game Engine");

    // directory that contains main.cs . This will help us detect whether we are
    // running with the scripts in the bundle or not.
    mainDotCsDir = NULL;

    useRedirect = true;
    windowCreated = false;
    sdlWindow = NULL;
    dedicated = false;

    printf("platstate init\n");
}

//------------------------------------------------------------------------------
static void InitWindow(const Point2I &initialSize, const char *name)
{
    gPlatState.windowSize = initialSize;
    gPlatState.setWindowTitle(name);
}

#ifndef DEDICATED
//------------------------------------------------------------------------------
static bool InitSDL()
{
   if (SDL_Init(SDL_INIT_VIDEO) != 0)
      return false;

   // SDL starts with text input on. It stays off until a text field asks for
   // it (platformSDL/sdlTextInput.h). While it is off, SDL's keyboard handler
   // keeps the browser from acting on the keys the game reads; while it is
   // on, it lets a typing key's keydown through to the browser, which is what
   // makes the browser send the keypress SDL reads the character from.
   SDL_StopTextInput();

   return true;
}

//------------------------------------------------------------------------------
// Input is active while the page has the keyboard. Losing it drops input only
// if the mouse is locked to the canvas, so that an unlocked mouse still moves
// the canvas's cursor while another window, or the browser's own address bar,
// has the keyboard -- as on Linux.
static void SetAppState()
{
   SDL_Window* window = gPlatState.sdlWindow;
   if (window == NULL)
      return;

   const bool active = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;

   // if we're not active but we have focus, set window active and
   // reactivate input
   if (active && (gPlatState.backgrounded || !Input::isActive()))
   {
      gPlatState.backgrounded = false;
      Input::reactivate();
   }
   // if we are active, but we don't have focus, deactivate input (if window
   // locked) and clear windowActive
   else if (!active && !gPlatState.backgrounded)
   {
      if (gPlatState.mouseLocked)
         Input::deactivate();
      gPlatState.backgrounded = true;
   }
}

//------------------------------------------------------------------------------
static void ProcessWindowEvent(const SDL_WindowEvent& event)
{
   switch (event.event)
   {
      case SDL_WINDOWEVENT_SIZE_CHANGED:
         // The canvas changed size: the page resized it (only a page that
         // sizes the canvas with CSS does), it went into or out of fullscreen,
         // or setScreenMode resized it. SDL has already resized the drawing
         // buffer, so following it is only a matter of telling the canvas.
         OpenGLDevice::followWindow();
         Game->refreshWindow();
         break;

      case SDL_WINDOWEVENT_EXPOSED:
         Game->refreshWindow();
         break;

      case SDL_WINDOWEVENT_FOCUS_GAINED:
      case SDL_WINDOWEVENT_FOCUS_LOST:
         // The page gained or lost the keyboard. SDL 2 says so (the SDL 1.2
         // port never sent SDL_ACTIVEEVENT), and on losing it has already let
         // go of every key held down, whose releases follow.
         SetAppState();
         break;
   }
}

//------------------------------------------------------------------------------
// The event loop. SDL's queue is emptied here, once a frame and in one place:
// window events are handled here and input goes to the input manager, as on
// Linux. The SDL 1.2 back-end copied each frame's events into a list for the
// input manager to walk later, which re-entered itself whenever handling one
// reset the input.
static bool ProcessMessages()
{
   // Whatever the last frame's input asked of text input -- a text field
   // taking the keyboard or letting it go -- happens now, before this frame's
   // keys are read (platformSDL/sdlTextInput.h).
   SDLTextInput::processTextInputState();

   UInputManager* inputManager = dynamic_cast<UInputManager*>(Input::getManager());

   SDL_Event event;
   while (SDL_PollEvent(&event))
   {
      switch (event.type)
      {
         case SDL_QUIT:
            return false;

         case SDL_WINDOWEVENT:
            ProcessWindowEvent(event.window);
            break;

         case SDL_KEYDOWN:
         case SDL_KEYUP:
         case SDL_TEXTINPUT:
         case SDL_MOUSEMOTION:
         case SDL_MOUSEBUTTONDOWN:
         case SDL_MOUSEBUTTONUP:
         case SDL_MOUSEWHEEL:
            if (inputManager)
               inputManager->processEvent(event);
            break;
      }
   }

   return true;
}
#endif // DEDICATED

//------------------------------------------------------------------------------
void Platform::setMouseLock(bool locked)
{
#ifndef DEDICATED
   gPlatState.mouseLocked = locked;

   UInputManager* uInputManager =
      dynamic_cast<UInputManager*>( Input::getManager() );

   if ( uInputManager && uInputManager->isEnabled() &&
      Input::isActive() )
      uInputManager->setWindowLocked(locked);
#endif
}

//------------------------------------------------------------------------------
// Nothing here sleeps, in the background or out of it. The browser calls the
// main loop once a frame, and slows a page it is not showing by itself; a
// sleep in a page is a busy wait (Emscripten's nanosleep spins), which would
// only hold the page up.
void Platform::process()
{
   PROFILE_START(XUX_PlatformProcess);

   if (gPlatState.windowCreated)
   {
#ifndef DEDICATED
      // process window events
      PROFILE_START(XUX_ProcessMessages);
      bool quit = !ProcessMessages();
      PROFILE_END();
      if(quit)
      {
         // generate a quit event
         Event quitEvent;
         quitEvent.type = QuitEventType;
         Game->postEvent(quitEvent);
      }

      // process input events
      PROFILE_START(XUX_InputProcess);
      Input::process();
      PROFILE_END();
#endif
   }

   PROFILE_END();
}

//------------------------------------------------------------------------------
const Point2I &Platform::getWindowSize()
{
   return gPlatState.windowSize;
}

//------------------------------------------------------------------------------
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
    gPlatState.windowSize = Point2I(newWidth, newHeight);
}

//------------------------------------------------------------------------------
// A page has no window of its own to minimize or restore.
void Platform::minimizeWindow()
{
}

//------------------------------------------------------------------------------
void Platform::restoreWindow()
{
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   Cleanup();
}

//------------------------------------------------------------------------------
void Platform::init()
{
   Con::printf("Platform::init");

   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "x86UNIX" );
   Con::setVariable( "$platformUnixType", "emscripten" );

   EmscriptenConsole::create();

#ifndef DEDICATED
   Con::printf("Dedicated == %i", gPlatState.dedicated);
   // if we're not dedicated do more initialization
   //if (!gPlatState.dedicated)
   {
     printf("Init non-dedicated\n");
      // init SDL
      if (!InitSDL())
      {
         Con::printf( "   Unable to initialize SDL: %s", SDL_GetError() );
         Platform::AlertOK("Error", "Unable to initialize SDL.");
         exit(1);
      }
      else
      {
         SDL_version linked;
         SDL_GetVersion(&linked);
         Con::printf("SDL %d.%d.%d initialized (video driver: %s)",
            linked.major, linked.minor, linked.patch, SDL_GetCurrentVideoDriver());
      }

      // initialize input
      Input::init();

      Con::printf( "Video Init:" );

      // initialize video
      Video::init();
      if ( Video::installDevice( OpenGLDevice::create() ) )
         Con::printf( "   OpenGL display device detected." );
      else
         Con::printf( "   OpenGL display device not detected." );

      Con::printf(" ");
   }
#endif
}

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
#ifndef DEDICATED
   // initialize window
   InitWindow(initialSize, name);
   if (!InitOpenGL()) {
      Con::printf( "   Unable to init OpenGL window." );
      exit(1);
   }
#endif
}

//------------------------------------------------------------------------------
// Web browser function:
//------------------------------------------------------------------------------
bool Platform::openWebBrowser( const char* webAddress )
{
    return false;
}

//------------------------------------------------------------------------------
ConsoleFunction( getDesktopResolution, const char*, 1, 1,
   "getDesktopResolution()" )
{
   if (!gPlatState.windowCreated)
      return "0 0 0";

   const Resolution desktop = Video::getDesktopResolution();

   char* returnString = Con::getReturnBuffer( 64 );
   dSprintf( returnString, 64, "%d %d %d", desktop.w, desktop.h, desktop.bpp );
   return( returnString );
}

//------------------------------------------------------------------------------
// Silly Korean registry key checker:
//------------------------------------------------------------------------------
ConsoleFunction( isKoreanBuild, bool, 1, 1, "isKoreanBuild()" )
{
   Con::printf("WARNING: isKoreanBuild() is unimplemented");
   return false;
}

//------------------------------------------------------------------------------

// The title SDL 2 gives a window here is the page's: document.title.
void Platform::setWindowTitle( const char* title )
{
#ifndef DEDICATED
   gPlatState.setWindowTitle(title ? title : "");
   if (gPlatState.sdlWindow)
      SDL_SetWindowTitle(gPlatState.sdlWindow, gPlatState.appWindowTitle);
#endif
}

//------------------------------------------------------------------------------
// The screen the page is on, as the browser reports it (screen.width and
// screen.height, in page pixels) -- SDL 2's Emscripten driver gives it as the
// one display mode it has. The SDL 1.2 back-end made up a 1024x768 desktop in
// platform.js.
Resolution Video::getDesktopResolution()
{
   Resolution result( 0, 0, 32 );

#ifndef DEDICATED
   SDL_DisplayMode mode;
   if ( SDL_WasInit( SDL_INIT_VIDEO ) && SDL_GetDesktopDisplayMode( 0, &mode ) == 0 )
   {
      result.w   = mode.w;
      result.h   = mode.h;
      result.bpp = SDL_BITSPERPIXEL( mode.format );
   }
#endif

   return result;
}

//-----------------------------------------------------------------------------
void Platform::outputDebugString( const char *string )
{
    fprintf(stderr, "%s", string);
    fprintf(stderr, "\n" );
    fflush(stderr);
}
