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

#include "platformEmscripten/platformEmscripten.h"
#include "platformEmscripten/EmscriptenConsole.h"
#include "platform/event.h"
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "game/gameInterface.h"


#include <SDL.h>

void Platform::postQuitMessage(const U32 in_quitVal)
{
   gPlatState.quit = true;
   Event quitEvent;
   quitEvent.type = QuitEventType;

   Game->postEvent(quitEvent);
}

void Platform::debugBreak()
{
   Platform::outputDebugString((const char *)"\pDEBUG_BREAK!");
}

void Platform::forceShutdown(S32 returnValue)
{
   exit(returnValue);
}   

void Platform::restartInstance()
{
   if( Game->isRunning() )
   {
      Con::errorf("The game is still running, we cant relaunch now!");
      return;
   }
   
   Con::errorf("restartInstance is not supported on Emscripten");
}

//-----------------------------------------------------------------------------
void Cleanup(bool minimal)
{
   if (!minimal)
   {
      Video::destroy();
      Input::destroy();
   }

   EmscriptenConsole::destroy();
#ifndef DEDICATED
   SDL_Quit();
#endif
}

