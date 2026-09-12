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



#include "platformEmscripten/platformEmscripten.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/console.h"
#include "platformEmscripten/EmscriptenInputManager.h"

#include <SDL.h>

#ifdef LOG_INPUT
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#endif

// Input::getKeyCode, Input::getAscii and the clipboard are
// platformSDL/sdlInput.cpp's, shared with the Linux back-end. They replace a
// table of what each key types on a US keyboard, and a clipboard that was
// always empty.

// Static class variables:
InputManager*  Input::smManager = NULL;

// smActive is not maintained under unix.  Use Input::isActive()
// instead
bool           Input::smActive = false;
CursorManager* Input::smCursorManager = 0;

#ifdef LOG_INPUT
S32 gInputLog = -1;
#endif

//------------------------------------------------------------------------------
void Input::init()
{
   Con::printf( "Input Init:" );

   destroy();

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
ConsoleFunction( isJoystickDetected, bool, 1, 1, "isJoystickDetected()" )
{
   argc; argv;
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->joystickDetected();
   else
      return false;
}

//------------------------------------------------------------------------------
ConsoleFunction( getJoystickAxes, const char*, 2, 2, "getJoystickAxes( instance )" )
{
   argc; argv;
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (manager)
      return manager->getJoystickAxesString(dAtoi(argv[1]));
   else
      return "";
}

//------------------------------------------------------------------------------
void Input::destroy()
{
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

#ifdef LOG_INPUT
//------------------------------------------------------------------------------
void Input::log( const char* format, ... )
{
}

ConsoleFunction( inputLog, void, 2, 2, "inputLog( string )" )
{
   argc;
   Input::log( "%s\n", argv[1] );
}
#endif // LOG_INPUT

#pragma mark ---- Cursor Functions ----
//------------------------------------------------------------------------------
void Input::pushCursor(S32 cursorID)
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->pushCursor(cursorID);
}

//------------------------------------------------------------------------------
void Input::popCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->popCursor();
}

//------------------------------------------------------------------------------
void Input::refreshCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->refreshCursor();
}

//------------------------------------------------------------------------------
// Show or hide the pointer over the canvas. SDL 2 does it with the canvas's
// CSS cursor, so the pointer is the browser's everywhere else on the page;
// the canvas turns it off while it draws a cursor of its own. Until SDL 2 this
// did nothing, and the browser's pointer showed on top of the canvas's.
void Input::setCursorState(bool on)
{
   SDL_ShowCursor(on ? SDL_ENABLE : SDL_DISABLE);
}

#pragma mark ---- DoubleClick Functions ----
//------------------------------------------------------------------------------
U32 Input::getDoubleClickTime()
{
   return ( 50 * 60.0f * 1000.0f);
}

//------------------------------------------------------------------------------
S32 Input::getDoubleClickWidth()
{
   // this is an arbitrary value.
   return 10;
}

//------------------------------------------------------------------------------
S32 Input::getDoubleClickHeight()
{
   return getDoubleClickWidth();
}

#pragma mark -

//------------------------------------------------------------------------------
// A page cannot move the player's pointer, and SDL 2's Emscripten driver says
// as much (its WarpMouse is unsupported), so there is nothing to do.
void Input::setCursorPos(S32 x, S32 y)
{
}

