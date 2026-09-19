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

#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/console.h"
#include "platformX86UNIX/x86UNIXState.h"
#include "platformX86UNIX/x86UNIXInputManager.h"
#include "platformX86UNIX/x86UNIXInput_ScriptBinding.h"

#include <SDL.h>

#ifdef LOG_INPUT
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <platformX86UNIX/x86UNIXUtils.h>

extern int x86UNIXOpen(const char *path, int oflag);
extern int x86UNIXClose(int fd);
extern ssize_t x86UNIXWrite(int fd, const void *buf, size_t nbytes);
#endif

// Input::getKeyCode, Input::getAscii and the clipboard are
// platformSDL/sdlInput.cpp's, shared with the other back-ends on SDL 2.

// Static class variables:
InputManager*  Input::smManager; //= 0;
CursorManager* Input::smCursorManager = 0;

// smActive is not maintained under unix.  Use Input::isActive()
// instead
bool           Input::smActive = false;

// unix platform state
extern x86UNIXPlatformState * x86UNIXState;

#ifdef LOG_INPUT
S32 gInputLog = -1;
#endif

//------------------------------------------------------------------------------
void Input::init()
{
   Con::printf( "Input Init:" );

   destroy();

#ifdef LOG_INPUT
   struct tm* newTime;
   time_t aclock;
   time( &aclock );
   newTime = localtime( &aclock );
   asctime( newTime );

   gInputLog = x86UNIXOpen("input.log", O_WRONLY | O_CREAT);
   log("Input log opened at %s\n", asctime( newTime ) );
   log("Operating System:\n" );
   log("  %s", UUtils->getOSName());
   log("\n");
#endif

   smActive = false;
   smManager = NULL;

   UInputManager *uInputManager = new UInputManager;
   if ( !uInputManager->enable() )
   {
      Con::errorf( "   Failed to enable Input Manager." );
      delete uInputManager;
      return;
   }

   uInputManager->init();

   smManager = uInputManager;

   Con::printf("   Input initialized");
   Con::printf(" ");
}

//------------------------------------------------------------------------------
void Input::destroy()
{
#ifdef LOG_INPUT
   if ( gInputLog != -1 )
   {
      log( "*** CLOSING LOG ***\n" );
      x86UNIXClose(gInputLog);
      gInputLog = -1;
   }
#endif

   if ( smManager && smManager->isEnabled() )
   {
      smManager->disable();
      delete smManager;
      smManager = NULL;
   }
}

//------------------------------------------------------------------------------
bool Input::enable()
{
   if ( smManager && !smManager->isEnabled() )
      return( smManager->enable() );

   return( false );
}

//------------------------------------------------------------------------------
void Input::disable()
{
   if ( smManager && smManager->isEnabled() )
      smManager->disable();
}

//------------------------------------------------------------------------------
void Input::activate()
{
   if ( smManager && smManager->isEnabled() && !isActive())
   {
#ifdef LOG_INPUT
      Input::log( "Activating Input...\n" );
#endif
      UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
      if ( uInputManager )
         uInputManager->activate();
   }
}

//------------------------------------------------------------------------------
void Input::deactivate()
{
   if ( smManager && smManager->isEnabled() && isActive() )
   {
#ifdef LOG_INPUT
      Input::log( "Deactivating Input...\n" );
#endif
      UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
      if ( uInputManager )
         uInputManager->deactivate();
   }
}

//------------------------------------------------------------------------------
void Input::reactivate()
{
   Input::deactivate();
   Input::activate();
}

//------------------------------------------------------------------------------
bool Input::isEnabled()
{
   if ( smManager )
      return smManager->isEnabled();
   return false;
}

//------------------------------------------------------------------------------
bool Input::isActive()
{
   UInputManager* uInputManager = dynamic_cast<UInputManager*>( smManager );
   if ( uInputManager )
      return uInputManager->isActive();
   return false;
}

//------------------------------------------------------------------------------
void Input::process()
{
   if ( smManager )
      smManager->process();
}

//------------------------------------------------------------------------------
InputManager* Input::getManager()
{
   return smManager;
}

//------------------------------------------------------------------------------
void Input::setCursorPos(S32 x, S32 y)
{
   SDL_Window* window = x86UNIXState->getSDLWindow();
   if (window)
      SDL_WarpMouseInWindow(window, x, y);
}

void Input::pushCursor(S32 cursorID)
{
   CursorManager* cm = getCursorManager();
   if (cm)
     cm->pushCursor(cursorID);
}

void Input::popCursor()
{
   CursorManager* cm = getCursorManager();
   if (cm)
     cm->popCursor();
}

void Input::refreshCursor()
{
   CursorManager* cm = getCursorManager();
   if (cm)
     cm->refreshCursor();
}

void Input::setCursorState(bool on)
{
   SDL_ShowCursor(on ? SDL_ENABLE : SDL_DISABLE);
}

void Input::setCursorShape(U32 cursorID)
{
}

U32 Input::getDoubleClickTime()
{
   return 1000;
}

S32 Input::getDoubleClickWidth()
{
   // Arbitrary value
   return 10;
}

S32 Input::getDoubleClickHeight()
{
   return Input::getDoubleClickWidth();
}
