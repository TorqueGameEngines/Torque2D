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
#include "game/gameInterface.h"

#include <SDL.h>
#include <unistd.h>
//--------------------------------------
void Platform::getLocalTime(LocalTime &lt)
{
   // TODO
	dMemset(&lt, '\0', sizeof(LocalTime));
}   

/// Gets the time in seconds since the Epoch
U32 Platform::getTime()
{
   // TODO
   return SDL_GetTicks() / 1000;
}

/// Gets the time in milliseconds since some epoch. In this case, system start time.
/// Storing milisec in a U32 overflows every 49.71 days
U32 Platform::getRealMilliseconds()
{
   return SDL_GetTicks();
}   

U32 Platform::getVirtualMilliseconds()
{
   return gPlatState.currentTime;   
}   

void Platform::advanceTime(U32 delta)
{
   gPlatState.currentTime += delta;
}   

void Platform::sleep(U32 ms)
{
   usleep( ms * 1000 );
}

#pragma mark ---- TimeManager ----

//--------------------------------------
// The interval is the same in the background as in front. The browser slows a
// page it is not showing by itself, and waiting out Pref::backgroundSleepTime
// here would be a busy wait of up to 200 ms a frame, since a sleep in a page
// spins. The SDL 1.2 back-end never heard that the page had lost the keyboard;
// SDL 2 says so (EmscriptenWindow.cpp).
void TimeManager::process()
{
   gPlatState.sleepTicks = sgTimeManagerProcessInterval;

   U32 curTime = Platform::getRealMilliseconds(); // GTC returns Milliseconds, FYI.
   S32 elapsedTime = curTime - gPlatState.lastTimeTick;

   if(elapsedTime <= gPlatState.sleepTicks)
   {
      Platform::sleep(gPlatState.sleepTicks - elapsedTime);
   }

   gPlatState.lastTimeTick = Platform::getRealMilliseconds();

   TimeEvent event;
   event.elapsedTime = elapsedTime;
   Game->postEvent(event);
}
