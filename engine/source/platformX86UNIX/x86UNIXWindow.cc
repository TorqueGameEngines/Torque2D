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
#include "platformX86UNIX/x86UNIXMessageBox.h"
#include "platformX86UNIX/x86UNIXInputManager.h"
#endif

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h> // fork, execvp, chdir
#include <time.h> // nanosleep

#ifndef DEDICATED
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_version.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

x86UNIXPlatformState *x86UNIXState;

bool DisplayPtrManager::sgDisplayLocked = false;
LockFunc_t DisplayPtrManager::sgLockFunc = NULL;
LockFunc_t DisplayPtrManager::sgUnlockFunc = NULL;

static U32 lastTimeTick;

#ifndef DEDICATED
extern bool InitOpenGL();
// This is called when some X client sends 
// a selection event (e.g. SelectionRequest)
// to the window
extern void NotifySelectionEvent(XEvent& event);
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

static void DetectWindowingSystem()
{
#ifndef DEDICATED
   Display* dpy = XOpenDisplay(NULL);
   if (dpy != NULL)
   {
      x86UNIXState->setXWindowsRunning(true);
      XCloseDisplay(dpy);
   }
#endif
}

//------------------------------------------------------------------------------
static void InitWindow(const Point2I &initialSize, const char *name)
{
   x86UNIXState->setWindowSize(initialSize);
   x86UNIXState->setWindowName(name);
}

#ifndef DEDICATED
//------------------------------------------------------------------------------
static bool InitSDL()
{
   // Ask for the X11 video driver unless the user named one.  This back-end is
   // an X11 back-end: it takes its Display, screen and display lock functions
   // straight out of SDL_GetWMInfo's x11 union below, and everything from the
   // keymap to the clipboard is built on them.  Genuine SDL 1.2 only ever had
   // X11 to offer, but the SDL2/3-backed sdl12-compat shim that distributions
   // now ship as SDL 1.2 defaults to Wayland on a Wayland desktop, where
   // SDL_GetWMInfo answers "No SysWM support available" and the engine used to
   // stop at "Unable to initialize SDL".  Xwayland serves us fine.
   setenv("SDL_VIDEODRIVER", "x11", 0);

   if (SDL_Init(SDL_INIT_VIDEO) != 0)
      return false;

   atexit(SDL_Quit);

   SDL_SysWMinfo sysinfo;
   SDL_VERSION(&sysinfo.version);
   if (SDL_GetWMInfo(&sysinfo) == 0)
      return false;

   x86UNIXState->setDisplayPointer(sysinfo.info.x11.display);
   DisplayPtrManager::setDisplayLockFunction(sysinfo.info.x11.lock_func);
   DisplayPtrManager::setDisplayUnlockFunction(sysinfo.info.x11.unlock_func);

   DisplayPtrManager xdisplay;
   Display* display = xdisplay.getDisplayPointer();

   x86UNIXState->setScreenNumber( 
      DefaultScreen( display ) );
   x86UNIXState->setScreenPointer( 
      DefaultScreenOfDisplay( display ) );

   x86UNIXState->setDesktopSize( 
      (S32) DisplayWidth( 
         display,
         x86UNIXState->getScreenNumber()),
      (S32) DisplayHeight( 
         display,
         x86UNIXState->getScreenNumber())
      );
   x86UNIXState->setDesktopBpp( 
      (S32) DefaultDepth( 
         display,
         x86UNIXState->getScreenNumber()));

   // indicate that we want sys WM messages
   SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

   return true;
}

//------------------------------------------------------------------------------
// Debounced window-geometry state (see ProcessMessages).  File scope because a
// change can be reported by SDL or by the window watcher below, and both are
// settled the same way.
static bool      sGeometryPending = false;
static U32       sGeometryChangedMs = 0;
static const U32 GeometrySettleMs = 150;

static void NoteGeometryChanged()
{
   sGeometryPending = true;
   sGeometryChangedMs = Platform::getRealMilliseconds();
}

//------------------------------------------------------------------------------
// The window watcher: a connection of our own to the X server, selecting
// structure and property events on the window the window manager manages.
// Neither SDL 1.2 reports what the window manager does to the window: genuine
// SDL 1.2 passes on its resizes but not its _NET_WM_STATE, and sdl12-compat
// passes on no X events at all -- and while the window manager has the window
// fullscreen (SUPER+F in Hyprland), not even its resizes, which SDL3 sees but
// drops on the way up to the SDL 1.2 API.  Any client may select events on any
// window, so a second connection sees all of it without disturbing SDL's.
static Display* sWatchDisplay = NULL;
static Window   sWatchedWindow = 0;
static Atom     sNetWMState = None;
static Atom     sNetWMStateFullScreen = None;

void WatchWMWindow(Window wmWindow)
{
   if (sWatchDisplay == NULL)
   {
      sWatchDisplay = XOpenDisplay(NULL);
      if (sWatchDisplay == NULL)
         return;
      sNetWMState = XInternAtom(sWatchDisplay, "_NET_WM_STATE", False);
      sNetWMStateFullScreen =
         XInternAtom(sWatchDisplay, "_NET_WM_STATE_FULLSCREEN", False);
   }
   if (wmWindow == sWatchedWindow)
      return;
   sWatchedWindow = wmWindow;
   XSelectInput(sWatchDisplay, wmWindow, StructureNotifyMask | PropertyChangeMask);
   XFlush(sWatchDisplay);
}

static void ProcessWatcherEvents()
{
   if (sWatchDisplay == NULL)
      return;
   while (XPending(sWatchDisplay))
   {
      XEvent event;
      XNextEvent(sWatchDisplay, &event);
      if (event.xany.window != sWatchedWindow)
         continue;
      if (event.type == ConfigureNotify ||
          (event.type == PropertyNotify && event.xproperty.atom == sNetWMState))
         NoteGeometryChanged();
   }
}

//------------------------------------------------------------------------------
// The managed window's real size, and whether the window manager has it
// fullscreen (_NET_WM_STATE_FULLSCREEN).
static bool GetWindowGeometry(Point2I& size, bool& fullScreen)
{
   XWindowAttributes attributes;
   if (sWatchDisplay == NULL || sWatchedWindow == 0 ||
       !XGetWindowAttributes(sWatchDisplay, sWatchedWindow, &attributes))
      return false;
   size.set(attributes.width, attributes.height);

   fullScreen = false;
   Atom type;
   int format;
   unsigned long count, remaining;
   unsigned char* data = NULL;
   if (XGetWindowProperty(sWatchDisplay, sWatchedWindow, sNetWMState, 0, 64,
         False, XA_ATOM, &type, &format, &count, &remaining, &data) == Success &&
       data != NULL)
   {
      Atom* atoms = (Atom*) data;
      for (unsigned long i = 0; i < count; ++i)
         if (atoms[i] == sNetWMStateFullScreen)
            fullScreen = true;
      XFree(data);
   }
   return true;
}

//------------------------------------------------------------------------------
// Let go of a window created at an exact size (OpenGLDevice::setScreenMode)
// once the window manager has had time to float it.  It decides a moment after
// the window maps, not at the map itself, and a window whose hints are gone by
// then is tiled; holding them 100 ms was already enough on Hyprland.  Straight
// to the X server rather than through SDL_SetVideoMode, which would rebuild the
// surface and reload every texture to change nothing else; both SDL 1.2s go on
// reporting the window's resizes either way.
static const U32 ExactSizeHoldMs = 500;

static void ReleaseExactSize()
{
   if (!OpenGLDevice::smHoldingExactSize ||
       Platform::getRealMilliseconds() - OpenGLDevice::smExactSizeHeldSince < ExactSizeHoldMs)
      return;
   OpenGLDevice::smHoldingExactSize = false;
   if (sWatchDisplay == NULL || sWatchedWindow == 0)
      return;
   XSizeHints hints;
   long supplied;
   if (!XGetWMNormalHints(sWatchDisplay, sWatchedWindow, &hints, &supplied))
      hints.flags = 0;
   hints.flags &= ~(PMinSize | PMaxSize);
   XSetWMNormalHints(sWatchDisplay, sWatchedWindow, &hints);
   XFlush(sWatchDisplay);
}

//------------------------------------------------------------------------------
// sdl12-compat -- SDL 1.2 implemented on SDL2/SDL3, which distributions now ship
// as SDL 1.2 -- reports itself as 1.2.50 or later; genuine SDL 1.2 ended at
// 1.2.15.  They go fullscreen differently: sdl12-compat asks the window manager
// (_NET_WM_STATE_FULLSCREEN), while genuine SDL 1.2 covers the screen with an
// override-redirect window of its own that the window manager never sees.
static bool SDLIsCompat()
{
   return SDL_Linked_Version()->patch >= 50;
}

//------------------------------------------------------------------------------
static void ProcessSYSWMEvent(const SDL_Event& event)
{
   XEvent& xevent = event.syswm.msg->event.xevent;
   //Con::printf("xevent : %d", xevent.type);
   switch (xevent.type)
   {
      case SelectionRequest:
         // somebody wants our clipboard
         NotifySelectionEvent(xevent);
         break;
   }
}

//------------------------------------------------------------------------------
static void SetAppState()
{
   U8 state = SDL_GetAppState();

   // if we're not active but we have appactive and inputfocus, set window
   // active and reactivate input
   if ((!x86UNIXState->windowActive() || !Input::isActive()) &&
      state & SDL_APPACTIVE &&
      state & SDL_APPINPUTFOCUS)
   {
      x86UNIXState->setWindowActive(true);
      Input::reactivate();
   }
   // if we are active, but we don't have appactive or input focus,
   // deactivate input (if window not locked) and clear windowActive
   else if (x86UNIXState->windowActive() && 
      !(state & SDL_APPACTIVE && state & SDL_APPINPUTFOCUS))
   {
      if (x86UNIXState->windowLocked())
         Input::deactivate();
      x86UNIXState->setWindowActive(false);
   }
}

//------------------------------------------------------------------------------
static S32 NumEventsPending()
{
   static const int MaxEvents = 255;
   static SDL_Event events[MaxEvents];

   SDL_PumpEvents();
   return SDL_PeepEvents(events, MaxEvents, SDL_PEEKEVENT, SDL_ALLEVENTS);
}

//------------------------------------------------------------------------------
static void PrintSDLEventQueue()
{
   static const int MaxEvents = 255;
   static SDL_Event events[MaxEvents];

   SDL_PumpEvents();
   S32 numEvents = SDL_PeepEvents(
      events, MaxEvents, SDL_PEEKEVENT, SDL_ALLEVENTS);
   if (numEvents <= 0)
   {
      dPrintf("SDL Event Queue is empty\n");
      return;
   }

   dPrintf("SDL Event Queue:\n");
   for (int i = 0; i < numEvents; ++i)
   {
      const char *eventType;
      switch (events[i].type)
      {
         case SDL_NOEVENT: eventType = "SDL_NOEVENT"; break;
         case SDL_ACTIVEEVENT: eventType = "SDL_ACTIVEEVENT"; break;
         case SDL_KEYDOWN: eventType = "SDL_KEYDOWN"; break; 
         case SDL_KEYUP: eventType = "SDL_KEYUP"; break; 
         case SDL_MOUSEMOTION: eventType = "SDL_MOUSEMOTION"; break; 
         case SDL_MOUSEBUTTONDOWN: eventType = "SDL_MOUSEBUTTONDOWN"; break; 
         case SDL_MOUSEBUTTONUP: eventType = "SDL_MOUSEBUTTONUP"; break; 
         case SDL_JOYAXISMOTION: eventType = "SDL_JOYAXISMOTION"; break; 
         case SDL_JOYBALLMOTION: eventType = "SDL_JOYBALLMOTION"; break; 
         case SDL_JOYHATMOTION: eventType = "SDL_JOYHATMOTION"; break; 
         case SDL_JOYBUTTONDOWN: eventType = "SDL_JOYBUTTONDOWN"; break; 
         case SDL_JOYBUTTONUP: eventType = "SDL_JOYBUTTONUP"; break; 
         case SDL_QUIT: eventType = "SDL_QUIT"; break; 
         case SDL_SYSWMEVENT: eventType = "SDL_SYSWMEVENT"; break; 
         case SDL_VIDEORESIZE: eventType = "SDL_VIDEORESIZE"; break; 
         case SDL_VIDEOEXPOSE: eventType = "SDL_VIDEOEXPOSE"; break; 
       /* Events SDL_USEREVENT through SDL_MAXEVENTS-1 are for your use */
         case SDL_USEREVENT: eventType = "SDL_USEREVENT"; break; 
         default: eventType = "UNKNOWN!"; break;
      }
      dPrintf("Event %d: %s\n", i, eventType);
   }
}

//------------------------------------------------------------------------------
static bool ProcessMessages()
{
   static const int MaxEvents = 255;
   static const U32 Mask = 
      SDL_QUITMASK | SDL_VIDEORESIZEMASK | SDL_VIDEOEXPOSEMASK |
      SDL_ACTIVEEVENTMASK | SDL_SYSWMEVENTMASK | 
      SDL_EVENTMASK(SDL_USEREVENT);
   static SDL_Event events[MaxEvents];
 
   // Window geometry is debounced. Under SDL 1.2 the only way to resize the GL
   // surface is SDL_SetVideoMode, which recreates the GL context and forces a
   // full texture reload -- unlike the Win32 back-end, where the GL drawable
   // follows the window for free (WM_SIZE just calls Platform::setWindowSize).
   // Rebuilding on every SDL_VIDEORESIZE during a live drag is painfully laggy,
   // so the surface is rebuilt only once the user stops resizing for
   // GeometrySettleMs (one texture reload per resize gesture instead of one per
   // frame). The state lives at file scope, beside NoteGeometryChanged.

   SDL_PumpEvents();
   S32 numEvents = SDL_PeepEvents(events, MaxEvents, SDL_GETEVENT, Mask);

   for (int i = 0; i < numEvents; ++i)
   {
      SDL_Event& event = events[i];
      switch (event.type)
      {
         case SDL_QUIT:
            return false;
            break;
         case SDL_VIDEORESIZE:
            // The surface is rebuilt once the drag settles (see below), at
            // the size the X server reports then.
            NoteGeometryChanged();
            break;
         case SDL_VIDEOEXPOSE:
            Game->refreshWindow();
            break;
         case SDL_USEREVENT:
            if (event.user.code == TORQUE_SETVIDEOMODE)
            {
               SetAppState();
               // SDL will send a motion event to restore the mouse position
               // on the new window.  Ignore that if the window is locked.
               if (x86UNIXState->windowLocked())
               {
                  SDL_Event tempEvent;
                  SDL_PeepEvents(&tempEvent, 1, SDL_GETEVENT,
                     SDL_MOUSEMOTIONMASK);
               }
            }
            break;
         case SDL_ACTIVEEVENT:
            SetAppState();
            break;
         case SDL_SYSWMEVENT:
            ProcessSYSWMEvent(event);
            break;
      }
   }

   ProcessWatcherEvents();
   ReleaseExactSize();

   // Once things have settled, bring the surface into line with the window.
   // This runs every frame (not gated on events) so it still fires after the
   // events stop. setScreenMode also updates Platform::getWindowSize(), which
   // drives the canvas extent (GuiCanvas::maintainSizing) and GL viewport
   // (dglSetClipRect).
   if (sGeometryPending &&
       (Platform::getRealMilliseconds() - sGeometryChangedMs) >= GeometrySettleMs)
   {
      sGeometryPending = false;
      Point2I size;
      bool wmFullScreen;
      if (GetWindowGeometry(size, wmFullScreen))
      {
         const U32 bpp = Video::getResolution().bpp;
         if (SDLIsCompat() && wmFullScreen != Video::isFullScreen())
         {
            if (wmFullScreen)
            {
               // The window manager made the window fullscreen (SUPER+F). Go
               // fullscreen with it: sdl12-compat takes a windowed
               // SDL_SetVideoMode now as a request to leave again. forceIt:
               // this is the size the window manager chose, not a mode to
               // check against the resolution list.
               DisplayDevice* device = Video::getDevice("OpenGL");
               if (device)
                  device->setScreenMode(size.x, size.y, bpp, true, true);
            }
            else
            {
               // ...and has taken it out of fullscreen again.
               Video::setScreenMode(size.x, size.y, bpp, false);
            }
         }
         else if (!Video::isFullScreen() && size != Platform::getWindowSize())
         {
            // Resized as a window -- a drag, or a tiling window manager laying
            // it out. Under genuine SDL 1.2 this is also how a window the
            // window manager makes fullscreen is followed, since SDL_FULLSCREEN
            // there would put up an override-redirect window of its own. A
            // window the game made fullscreen keeps the size the game gave it:
            // it may have asked for 1024x768, which SDL scales to fill.
            Video::setScreenMode(size.x, size.y, bpp, false);
         }
      }
      Game->refreshWindow();
   }

   return true;
}

//------------------------------------------------------------------------------
// send a destroy window event to the window.  assumes
// window is created.
void SendQuitEvent()
{
   SDL_Event quitevent;
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
struct AlertWinState
{
      bool fullScreen;
      bool cursorHidden;
      bool inputGrabbed;
};

//------------------------------------------------------------------------------
void DisplayErrorAlert(const char* errMsg, bool showSDLError)
{
   char fullErrMsg[2048];
   dStrncpy(fullErrMsg, errMsg, sizeof(fullErrMsg));
   
   if (showSDLError)
   {
      char* sdlerror = SDL_GetError();
      if (sdlerror != NULL && dStrlen(sdlerror) > 0)
      {
         dStrcat(fullErrMsg, "  (Error: ");
         dStrcat(fullErrMsg, sdlerror);
         dStrcat(fullErrMsg, ")");
      }
   }
   
   Platform::AlertOK("Error", fullErrMsg);
}


//------------------------------------------------------------------------------
static inline void AlertDisableVideo(AlertWinState& state)
{

   state.fullScreen = Video::isFullScreen();
   state.cursorHidden = (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE);
   state.inputGrabbed = (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON);

   if (state.fullScreen)
      SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
   if (state.cursorHidden)
      SDL_ShowCursor(SDL_ENABLE);
   if (state.inputGrabbed)
      SDL_WM_GrabInput(SDL_GRAB_OFF);
}

//------------------------------------------------------------------------------
static inline void AlertEnableVideo(AlertWinState& state)
{
   if (state.fullScreen)
      SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
   if (state.cursorHidden)
      SDL_ShowCursor(SDL_DISABLE);
   if (state.inputGrabbed)
      SDL_WM_GrabInput(SDL_GRAB_ON);
}
#endif // DEDICATED

//------------------------------------------------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
#ifndef DEDICATED
   if (x86UNIXState->isXWindowsRunning())
   {
      AlertWinState state;
      AlertDisableVideo(state);

      DisplayPtrManager xdisplay;
      XMessageBox mBox(xdisplay.getDisplayPointer());
      mBox.alertOK(windowTitle, message);

      AlertEnableVideo(state);
   }
   else
#endif
   {
      if (Con::isActive() && StdConsole::isEnabled())
         Con::printf("Alert: %s %s", windowTitle, message);
      else
         dPrintf("Alert: %s %s\n", windowTitle, message);
   }
}

//------------------------------------------------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
#ifndef DEDICATED
   if (x86UNIXState->isXWindowsRunning())
   {
      AlertWinState state;
      AlertDisableVideo(state);

      DisplayPtrManager xdisplay;
      XMessageBox mBox(xdisplay.getDisplayPointer());
      bool val = 
         mBox.alertOKCancel(windowTitle, message) == XMessageBox::OK;

      AlertEnableVideo(state);
      return val;
   }
   else
#endif
   {
      if (Con::isActive() && StdConsole::isEnabled())
         Con::printf("Alert: %s %s", windowTitle, message);
      else
         dPrintf("Alert: %s %s\n", windowTitle, message);
      return false;
   }
}

//------------------------------------------------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
#ifndef DEDICATED
   if (x86UNIXState->isXWindowsRunning())
   {
      AlertWinState state;
      AlertDisableVideo(state);

      DisplayPtrManager xdisplay;
      XMessageBox mBox(xdisplay.getDisplayPointer());
      bool val = 
         mBox.alertRetryCancel(windowTitle, message) == XMessageBox::Retry;
      
      AlertEnableVideo(state);
      return val;
   }
   else
#endif
   {
      if (Con::isActive() && StdConsole::isEnabled())
         Con::printf("Alert: %s %s", windowTitle, message);
      else
         dPrintf("Alert: %s %s\n", windowTitle, message);
      return false;
   }
}

//Luma: YesNo alert message
bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
#ifndef DEDICATED
   if (x86UNIXState->isXWindowsRunning())
   {
      AlertWinState state;
      AlertDisableVideo(state);

      DisplayPtrManager xdisplay;
      XMessageBox mBox(xdisplay.getDisplayPointer());
      bool val = 
         mBox.alertYesNo(windowTitle, message) == XMessageBox::OK;

      AlertEnableVideo(state);
      return val;
   }
   else
#endif
   {
      if (Con::isActive() && StdConsole::isEnabled())
         Con::printf("Alert: %s %s", windowTitle, message);
      else
         dPrintf("Alert: %s %s\n", windowTitle, message);
      return false;
   }
}

// Very hacky, doesn't support buttons or icons
S32 Platform::messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons, MBIcons icon)
{
   Platform::AlertOK(title, message);
   return 0;
}

//------------------------------------------------------------------------------
void Platform::enableKeyboardTranslation(void)
{
#ifndef DEDICATED
   // JMQ: not sure if this is needed for i18n keyboards
   //SDL_EnableUNICODE( 1 );
//    SDL_EnableKeyRepeat(
//       SDL_DEFAULT_REPEAT_DELAY, 
//       SDL_DEFAULT_REPEAT_INTERVAL);
#endif
}

//------------------------------------------------------------------------------
void Platform::disableKeyboardTranslation(void)
{
#ifndef DEDICATED
   //SDL_EnableUNICODE( 0 );
   //   SDL_EnableKeyRepeat(0, 0);
#endif
}

//------------------------------------------------------------------------------
void Platform::minimizeWindow()
{
#ifndef DEDICATED
   if (x86UNIXState->windowCreated())
      SDL_WM_IconifyWindow();
#endif
}

void Platform::restoreWindow()
{
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

#ifndef DEDICATED
#if 0 
// JMQ: disabled this because it may fire mistakenly in some configurations.
// sdl's default event handling scheme should be enough.
   // crude check to make sure that we're not loading up events.  the sdl 
   // event queue should never have more than (say) 25 events in it at this
   // point
   const int MaxEvents = 25;
   if (NumEventsPending() > MaxEvents)
   {
      PrintSDLEventQueue();
      AssertFatal(false, "The SDL event queue has too many events!");
   }
#endif
#endif
   PROFILE_END();
}

// extern U32 calculateCRC(void * buffer, S32 len, U32 crcVal );

// #if defined(DEBUG) || defined(INTERNAL_RELEASE)
// static U32 stubCRC = 0;
// #else
// static U32 stubCRC = 0xEA63F56C;
// #endif

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
         Con::printf("SDL Initialized");
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

   // look for a browser preference variable
   // JMQTODO: be nice to implement some UI to customize this
   const char* webBrowser = Con::getVariable("Pref::Unix::WebBrowser");
   if (dStrlen(webBrowser) == 0)
      webBrowser = NULL;

   pid_t pid = fork();
   if (pid == -1)
   {
      Con::printf("WARNING: Platform::openWebBrowser failed to fork");
      return false;
   }
   else if (pid != 0)
   {
      // parent
      if (Video::isFullScreen())
         Video::toggleFullScreen();

      return true;
   }
   else if (pid == 0)
   {
      // child
      char* argv[3];
      argv[0] = 0;
      argv[1] = const_cast<char*>(webAddress);
      argv[2] = 0;

      int ok = -1;

      // if execvp returns, it means it couldn't execute the program
      if (webBrowser != NULL)
         ok = execvp(webBrowser, argv);

      ok = execvp("xdg-open", argv);
      ok = execvp("firefox", argv);
      ok = execvp("konqueror", argv);
      ok = execvp("mozilla", argv);
      // use dPrintf instead of Con here since we're now in another process, 
      dPrintf("WARNING: Platform::openWebBrowser: couldn't launch a web browser\n");
      _exit(-1);     
      return false;
   }
   else
   {
      Con::printf("WARNING: Platform::openWebBrowser: forking problem");
      return false;
   }
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
   if (!x86UNIXState->windowCreated())
      return "0 0 0";

   char buffer[256];
   char* returnString = Con::getReturnBuffer( dStrlen( buffer ) + 1 );

   dSprintf( buffer, sizeof( buffer ), "%d %d %d", 
      x86UNIXState->getDesktopSize().x,
      x86UNIXState->getDesktopSize().y, 
      x86UNIXState->getDesktopBpp() );
   dStrcpy( returnString, buffer );
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

   // check to see if X is running
   DetectWindowingSystem();

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
   SDL_WM_SetCaption(x86UNIXState->getWindowName(), NULL);
#endif
}

Resolution Video::getDesktopResolution()
{
   Resolution  Result;
   Result.w   = x86UNIXState->getDesktopSize().x;
   Result.h   = x86UNIXState->getDesktopSize().y;
   Result.bpp = x86UNIXState->getDesktopBpp();

  return Result;
}


