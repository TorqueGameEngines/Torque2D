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
#include "io/fileStream.h"
#include "game/resource.h"
#include "game/version.h"
#include "math/mRandom.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/x86UNIXStdConsole.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platform.h"
#include "platform/platformAL.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "debug/profiler.h"
#include "platformX86UNIX/platformGL.h"
#include "platformX86UNIX/x86UNIXOGLVideo.h"
#include "platformX86UNIX/x86UNIXState.h"

#ifndef DEDICATED
#include "platformX86UNIX/x86UNIXInputManager.h"
#include "platformSDL/sdlTextInput.h"
#endif

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h> // fork, execvp, chdir
#include <time.h> // nanosleep
#include <sys/wait.h> // waitpid

#ifndef DEDICATED
#include <SDL.h>
#endif

x86UNIXPlatformState *x86UNIXState;

static U32 lastTimeTick;

#ifndef DEDICATED
extern bool InitOpenGL();
#endif

//------------------------------------------------------------------------------
static S32 ParseCommandLine(S32 argc, const char **argv,
   Vector<char*>& newCommandLine)
{
   x86UNIXState->setExePathName(argv[0]);
   bool foundDedicated = false;

   for ( int i=0; i < argc; i++ )
   {
      // look for platform specific args
      if (dStrcmp(argv[i], "-version") == 0)
      {
         dPrintf("%s (built on %s)\n", getVersionString(), getCompileTimeString());
         dPrintf("gcc: %s\n", __VERSION__);
         return 1;
      }
      if (dStrcmp(argv[i], "-dedicated") == 0)
      {
         foundDedicated = true;
         // no continue because dedicated is also handled by script
      }
      if (dStrcmp(argv[i], "-dsleep") == 0)
      {
         x86UNIXState->setDSleep(true);
         continue;
      }
      if (dStrcmp(argv[i], "-chdir") == 0)
      {
         if ( ++i >= argc )
         {
            dPrintf("Follow -chdir option with the desired working directory.\n");
            return 1;
         }
         if (chdir(argv[i]) == -1)
         {
            dPrintf("Unable to chdir to %s: %s\n", argv[i], strerror(errno));
            return 1;
         }
         continue;
      }

      // copy the arg into newCommandLine
      int argLen = dStrlen(argv[i]) + 1;
      char* argBuf = new char[argLen]; // this memory is deleted in main()
      dStrncpy(argBuf, argv[i], argLen);
      newCommandLine.push_back(argBuf);
   }
   x86UNIXState->setDedicated(foundDedicated);
#if defined(DEDICATED) && !defined(TORQUE_ENGINE)
   if (!foundDedicated)
   {
      dPrintf("This is a dedicated server build.  You must supply the -dedicated command line parameter.\n");
      return 1;
   }
#endif
   return 0;
}

//------------------------------------------------------------------------------
static void InitWindow(const Point2I &initialSize, const char *name)
{
   x86UNIXState->setWindowSize(initialSize);
   x86UNIXState->setWindowName(name);
}

#ifndef DEDICATED
//------------------------------------------------------------------------------
// The desktop the window is on -- the first display, before there is a window
// -- as SDL has it. Kept in the platform state, where the resolution list
// reads it.
static void UpdateDesktopMode()
{
   if (!SDL_WasInit(SDL_INIT_VIDEO))
      return;

   SDL_Window* window = x86UNIXState->getSDLWindow();
   int displayIndex = window ? SDL_GetWindowDisplayIndex(window) : 0;
   if (displayIndex < 0)
      displayIndex = 0;

   SDL_DisplayMode mode;
   if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0)
      return;

   x86UNIXState->setDesktopSize(mode.w, mode.h);
   x86UNIXState->setDesktopBpp(SDL_BITSPERPIXEL(mode.format));
}

//------------------------------------------------------------------------------
static bool InitSDL()
{
   // This back-end runs on SDL 2's X11 driver, and under Wayland on Xwayland.
   // SDL 2 tries X11 first anyway where it has both; saying so here keeps it
   // that way whatever SDL's own order becomes, while SDL_VIDEODRIVER in the
   // environment still wins, since a hint set at default priority gives way to
   // it. Native Wayland is for later: the fonts (x86UNIXFont.cc) open an X
   // display of their own. A hint rather than setenv, which would also have
   // reached every program the engine starts, the web browser included.
   SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");

   if (SDL_Init(SDL_INIT_VIDEO) != 0)
      return false;

   atexit(SDL_Quit);

   // SDL starts with text input on. It stays off until a text field asks for
   // it (platformSDL/sdlTextInput.h).
   SDL_StopTextInput();

   UpdateDesktopMode();

   return true;
}

//------------------------------------------------------------------------------
// Input is active while the window has the keyboard and is not minimized.
// Losing it drops input only if the mouse is locked to the window, so that an
// unlocked mouse still moves the canvas's cursor over a window in the
// background.
static void SetAppState()
{
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window == NULL)
      return;

   const Uint32 flags = SDL_GetWindowFlags(window);
   const bool active = (flags & SDL_WINDOW_INPUT_FOCUS) &&
      !(flags & SDL_WINDOW_MINIMIZED);

   // if we're not active but we have focus, set window active and
   // reactivate input
   if (active && (!x86UNIXState->windowActive() || !Input::isActive()))
   {
      x86UNIXState->setWindowActive(true);
      Input::reactivate();
   }
   // if we are active, but we don't have focus, deactivate input (if window
   // locked) and clear windowActive
   else if (!active && x86UNIXState->windowActive())
   {
      if (x86UNIXState->windowLocked())
         Input::deactivate();
      x86UNIXState->setWindowActive(false);
   }
}

//------------------------------------------------------------------------------
static void ProcessWindowEvent(const SDL_WindowEvent& event)
{
   switch (event.event)
   {
      case SDL_WINDOWEVENT_SIZE_CHANGED:
         // Resized -- by a drag, by a tiling window manager laying the window
         // out, into or out of fullscreen, or by setScreenMode itself. Under
         // SDL 2 the GL drawable follows the window, as it does on Windows, so
         // following a resize is only a matter of telling the canvas, and it
         // is done straight away. SDL 1.2 had to rebuild the surface and reload
         // every texture, so the SDL 1.2 back-end waited for a drag to settle.
         OpenGLDevice::followWindow();
         Game->refreshWindow();
         break;

      case SDL_WINDOWEVENT_EXPOSED:
         Game->refreshWindow();
         break;

      case SDL_WINDOWEVENT_FOCUS_GAINED:
      case SDL_WINDOWEVENT_FOCUS_LOST:
      case SDL_WINDOWEVENT_MINIMIZED:
      case SDL_WINDOWEVENT_RESTORED:
         SetAppState();
         break;
   }
}

//------------------------------------------------------------------------------
// The event loop. SDL's queue is emptied here, once a frame and in one place:
// window events are handled here and input goes to the input manager. The
// SDL 1.2 back-end ran two loops, each picking its own kinds of event out of
// the queue by mask, which SDL 2's SDL_PeepEvents no longer takes.
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

   OpenGLDevice::releaseExactSize();

   return true;
}

//------------------------------------------------------------------------------
// send a destroy window event to the window.  assumes
// window is created.
void SendQuitEvent()
{
   SDL_Event quitevent;
   SDL_zero(quitevent);
   quitevent.type = SDL_QUIT;
   SDL_PushEvent(&quitevent);
}
#endif // DEDICATED

//------------------------------------------------------------------------------
static inline void Sleep(int secs, int nanoSecs)
{
   timespec sleeptime;
   sleeptime.tv_sec = secs;
   sleeptime.tv_nsec = nanoSecs;
   nanosleep(&sleeptime, NULL);
}

#ifndef DEDICATED
//------------------------------------------------------------------------------
void DisplayErrorAlert(const char* errMsg, bool showSDLError)
{
   char fullErrMsg[2048];
   dStrncpy(fullErrMsg, errMsg, sizeof(fullErrMsg));

   if (showSDLError)
   {
      const char* sdlerror = SDL_GetError();
      if (sdlerror != NULL && dStrlen(sdlerror) > 0)
      {
         dStrcat(fullErrMsg, "  (Error: ");
         dStrcat(fullErrMsg, sdlerror);
         dStrcat(fullErrMsg, ")");
      }
   }

   Platform::AlertOK("Error", fullErrMsg);
}
#endif // DEDICATED

// The alerts and Platform::messageBox are platformSDL/sdlMsgBox.cpp's, and
// Platform::enableKeyboardTranslation and disableKeyboardTranslation are
// platformSDL/sdlTextInput.cpp's.

//------------------------------------------------------------------------------
void Platform::minimizeWindow()
{
#ifndef DEDICATED
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window)
      SDL_MinimizeWindow(window);
#endif
}

void Platform::restoreWindow()
{
#ifndef DEDICATED
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window)
      SDL_RestoreWindow(window);
#endif
}

//------------------------------------------------------------------------------
void Platform::process()
{
   PROFILE_START(XUX_PlatformProcess);
   stdConsole->process();

   if (x86UNIXState->windowCreated())
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

      // if we're not the foreground window, sleep for 1 ms
      if (!x86UNIXState->windowActive())
         Sleep(0, getBackgroundSleepTime() * 1000000);
#endif
   }
   else
   {
      // no window
      // if we're not in journal mode, sleep for 1 ms
      // JMQ: since linux's minimum sleep latency seems to be 20ms, this can
      // increase player pings by 10-20ms in the dedicated server.  So
      // you have to use -dsleep to enable it.  the server sleeps anyway when
      // there are no players connected.
      // JMQ: recent kernels (such as RH 8.0 2.4.18) reduce the latency
      // to 2-4 ms on average.
      if (!Game->isJournalReading() && (x86UNIXState->getDSleep() ||
             Con::getIntVariable("Server::PlayerCount") -
             Con::getIntVariable("Server::BotCount") <= 0))
      {
         PROFILE_START(XUX_Sleep);
         Sleep(0, getBackgroundSleepTime() * 1000000);
         PROFILE_END();
      }
   }

   PROFILE_END();
}

//------------------------------------------------------------------------------
const Point2I &Platform::getWindowSize()
{
   return x86UNIXState->getWindowSize();
}


//------------------------------------------------------------------------------
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
   x86UNIXState->setWindowSize( (S32) newWidth, (S32) newHeight );
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   Cleanup();
}

//------------------------------------------------------------------------------
void Platform::init()
{
   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "x86UNIX" );
#if defined(__linux__)
   Con::setVariable( "$platformUnixType", "Linux" );
#elif defined(__OpenBSD__)
   Con::setVariable( "$platformUnixType", "OpenBSD" );
#else
   Con::setVariable( "$platformUnixType", "Unknown" );
#endif

   StdConsole::create();

#ifndef DEDICATED
   // if we're not dedicated do more initialization
   if (!x86UNIXState->isDedicated())
   {
      // init SDL
      if (!InitSDL())
      {
         DisplayErrorAlert("Unable to initialize SDL.");
         ImmediateShutdown(1);
      } else {
         SDL_version linked;
         SDL_GetVersion(&linked);
         Con::printf("SDL %d.%d.%d initialized (video driver: %s)",
            linked.major, linked.minor, linked.patch, SDL_GetCurrentVideoDriver());
      }

      // initialize input
      Input::init();

      Con::printf( "Video Init:" );

      // load gl library
      if (!GLLoader::OpenGLInit())
      {
         DisplayErrorAlert("Unable to initialize OpenGL.");
         ImmediateShutdown(1);
      }

      // initialize video
      Video::init();
      if ( Video::installDevice( OpenGLDevice::create() ) )
         Con::printf( "   OpenGL display device detected." );
      else
         Con::printf( "   OpenGL display device not detected." );

      Con::printf(" ");
   }
#endif
   // if we are dedicated, do sleep timing and display results
   if (x86UNIXState->isDedicated())
   {
      const S32 MaxSleepIter = 10;
      U32 totalSleepTime = 0;
      U32 start;
      for (S32 i = 0; i < MaxSleepIter; ++i)
      {
         start = Platform::getRealMilliseconds();
         Sleep(0, 1000000);
         totalSleepTime += Platform::getRealMilliseconds() - start;
      }
      U32 average = static_cast<U32>(totalSleepTime / MaxSleepIter);

      Con::printf("Sleep latency: %ums", average);
      // dPrintf as well, since console output won't be visible yet
      dPrintf("Sleep latency: %ums\n", average);
      if (!x86UNIXState->getDSleep() && average < 10)
      {
         const char* msg = "Sleep latency ok, enabling dsleep for lower cpu " \
            "utilization";
         Con::printf("%s", msg);
         dPrintf("%s\n", msg);
         x86UNIXState->setDSleep(true);
      }
   }
}

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
#ifndef DEDICATED
   // initialize window
   InitWindow(initialSize, name);
   if (!InitOpenGL())
      ImmediateShutdown(1);
#endif
}

//------------------------------------------------------------------------------
// Web browser function:
//------------------------------------------------------------------------------
bool Platform::openWebBrowser( const char* webAddress )
{
   if (!webAddress || dStrlen(webAddress)==0)
      return false;

   // Out of fullscreen first, so the page doesn't open behind the game.
   if (Video::isFullScreen())
      Video::toggleFullScreen();

   // A browser named in $Pref::Unix::WebBrowser is started directly, from a
   // child that exits at once: the browser is left to init, so the engine
   // never has a zombie to wait for. It used to rely on ignoring SIGCHLD for
   // that, which also broke every wait for a child of SDL's own -- the X11
   // message box runs in one (x86UNIXProcessControl.cc).
   const char* webBrowser = Con::getVariable("Pref::Unix::WebBrowser");
   if (webBrowser[0] != '\0')
   {
      const pid_t pid = fork();
      if (pid == 0)
      {
         if (fork() == 0)
         {
            char* argv[3] = { const_cast<char*>(webBrowser),
                              const_cast<char*>(webAddress), NULL };
            execvp(webBrowser, argv);
            _exit(127);
         }
         _exit(0);
      }
      if (pid > 0)
      {
         waitpid(pid, NULL, 0);
         return true;
      }
      Con::warnf("Platform::openWebBrowser: couldn't start %s", webBrowser);
      return false;
   }

   // Otherwise whatever the desktop opens web pages with, through xdg-open --
   // Torque3D's way. (The fallbacks this used to try, xdg-open, then firefox,
   // konqueror and mozilla, were all started without the address: argv[0]
   // was NULL, which ends the argument list before it begins.)
   if (SDL_OpenURL(webAddress) != 0)
   {
      Con::warnf("Platform::openWebBrowser: couldn't open %s: %s", webAddress, SDL_GetError());
      return false;
   }
   return true;
}

void Platform::setMouseLock(bool locked)
{
  // Not implemented
}

//-------------------------------------------------------------------------------
void TimeManager::process()
{
   U32 curTime = Platform::getRealMilliseconds();
   TimeEvent event;
   event.elapsedTime = curTime - lastTimeTick;
   if(event.elapsedTime > sgTimeManagerProcessInterval)
   {
      lastTimeTick = curTime;
      Game->postEvent(event);
   }
}

//------------------------------------------------------------------------------
ConsoleFunction( getDesktopResolution, const char*, 1, 1,
   "getDesktopResolution()" )
{
#ifndef DEDICATED
   if (!SDL_WasInit(SDL_INIT_VIDEO))
      return "0 0 0";
#endif

   const Resolution desktop = Video::getDesktopResolution();

   char* returnString = Con::getReturnBuffer( 64 );
   dSprintf( returnString, 64, "%d %d %d", desktop.w, desktop.h, desktop.bpp );
   return( returnString );
}

//------------------------------------------------------------------------------
int main(S32 argc, const char **argv)
{
   // init platform state
   x86UNIXState = new x86UNIXPlatformState;

   // parse the command line for unix-specific params
   Vector<char *> newCommandLine;
   S32 returnVal = ParseCommandLine(argc, argv, newCommandLine);
   if (returnVal != 0)
      return returnVal;

   // init lastTimeTick for TimeManager::process()
   lastTimeTick = Platform::getRealMilliseconds();

   // init process control stuff
   ProcessControlInit();

   Game->mainInitialize(argc, argv);

   // run the game
   while ( Game->isRunning() )
   {
      Game->mainLoop();
   }

   Game->mainShutdown();

   // dispose of command line
   for(U32 i = 0; i < newCommandLine.size(); i++)
      delete [] newCommandLine[i];

   // dispose of state
   delete x86UNIXState;

   return returnVal;
}

void Platform::setWindowTitle( const char* title )
{
#ifndef DEDICATED
   x86UNIXState->setWindowName(title);
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window)
      SDL_SetWindowTitle(window, title ? title : "");
#endif
}

Resolution Video::getDesktopResolution()
{
#ifndef DEDICATED
   UpdateDesktopMode();
#endif

   Resolution  Result;
   Result.w   = x86UNIXState->getDesktopSize().x;
   Result.h   = x86UNIXState->getDesktopSize().y;
   Result.bpp = x86UNIXState->getDesktopBpp();

  return Result;
}

