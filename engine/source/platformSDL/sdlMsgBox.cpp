//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

// Torque3D's platformSDL/sdlMsgBox.cpp: the alerts and Platform::messageBox on
// SDL_ShowMessageBox, shared by every back-end built on SDL 2.

#include "platform/platform.h"
#include "platform/nativeDialogs/msgBox.h"
#include "console/console.h"

#include <SDL.h>

namespace
{
   const int ReturnKey = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
   const int EscapeKey = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;

   // Return picks the first button and Escape the one that backs out. Torque3D
   // ends its Save/Don't Save/Cancel box with its Retry button, which returns
   // MROk, so its "cancel" saved; this one ends with Cancel.
   const SDL_MessageBoxButtonData MBOkData[]                = { { ReturnKey | EscapeKey, MROk, "Ok" } };
   const SDL_MessageBoxButtonData MBOkCancelData[]          = { { ReturnKey, MROk, "Ok" }, { EscapeKey, MRCancel, "Cancel" } };
   const SDL_MessageBoxButtonData MBRetryCancelData[]       = { { ReturnKey, MROk, "Retry" }, { EscapeKey, MRCancel, "Cancel" } };
   const SDL_MessageBoxButtonData MBSaveDontSaveData[]      = { { ReturnKey, MROk, "Save" }, { EscapeKey, MRDontSave, "Don't Save" } };
   const SDL_MessageBoxButtonData MBSaveDontSaveCancelData[] = { { ReturnKey, MROk, "Save" }, { 0, MRDontSave, "Don't Save" }, { EscapeKey, MRCancel, "Cancel" } };

   // AlertYesNo has no MBButtons value of its own.
   const SDL_MessageBoxButtonData MBYesNoData[]             = { { ReturnKey, MROk, "Yes" }, { EscapeKey, MRCancel, "No" } };

   //---------------------------------------------------------------------------
   // Where a box goes when there is no desktop to put it on: a dedicated server
   // never starts SDL's video, and SDL_ShowMessageBox fails without a display.
   void printAlert( const char* title, const char* message )
   {
      if ( Con::isActive() )
         Con::printf( "Alert: %s %s", title, message );
      else
         dPrintf( "Alert: %s %s\n", title, message );
   }

   //---------------------------------------------------------------------------
   // Show the box over the engine's window and wait for an answer; returns the
   // button's id, or 0 if no box could be shown.
   //
   // The window is the one the engine draws into -- found through its GL
   // context, which is how the back-ends find it without sharing anything else
   // -- and the box is made its dialog, so a window manager keeps it on top.
   // Like Torque3D this lets go of the mouse first, so the player can reach the
   // buttons; unlike it, the mouse is taken back and the cursor hidden again
   // afterwards, since the canvas draws its own.
   S32 showBox( const char* title, const char* message, const SDL_MessageBoxButtonData* buttons, int numButtons, MBIcons icon )
   {
      if ( !SDL_WasInit( SDL_INIT_VIDEO ) )
         return 0;

      SDL_Window* window = SDL_GL_GetCurrentWindow();

      const bool wasGrabbed = window && SDL_GetWindowGrab( window );
      const bool wasRelative = SDL_GetRelativeMouseMode() == SDL_TRUE;
      const int cursorShown = SDL_ShowCursor( SDL_QUERY );
      if ( wasRelative )
         SDL_SetRelativeMouseMode( SDL_FALSE );
      if ( wasGrabbed )
         SDL_SetWindowGrab( window, SDL_FALSE );
      SDL_ShowCursor( SDL_ENABLE );

      Uint32 flags = SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
      switch ( icon )
      {
         case MIStop:    flags |= SDL_MESSAGEBOX_ERROR; break;
         case MIWarning: flags |= SDL_MESSAGEBOX_WARNING; break;
         default:        flags |= SDL_MESSAGEBOX_INFORMATION; break;
      }

      SDL_MessageBoxData boxData;
      boxData.flags = flags;
      boxData.window = window;
      boxData.title = title;
      boxData.message = message;
      boxData.numbuttons = numButtons;
      boxData.buttons = buttons;
      boxData.colorScheme = NULL;

      int res = 0;
      if ( SDL_ShowMessageBox( &boxData, &res ) != 0 )
         res = 0;

      SDL_ShowCursor( cursorShown );
      if ( wasGrabbed )
         SDL_SetWindowGrab( window, SDL_TRUE );
      if ( wasRelative )
         SDL_SetRelativeMouseMode( SDL_TRUE );

      return res;
   }

   //---------------------------------------------------------------------------
   // An alert with the given buttons; true if the first -- the one that goes
   // ahead -- was chosen. Printed, and false, if there is nowhere to show it.
   bool alert( const char* title, const char* message, const SDL_MessageBoxButtonData* buttons, int numButtons, MBIcons icon )
   {
      const S32 res = showBox( title, message, buttons, numButtons, icon );
      if ( res == 0 )
      {
         printAlert( title, message );
         return false;
      }
      return res == MROk;
   }
}

//--------------------------------------
S32 Platform::messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons, MBIcons icon)
{
   const SDL_MessageBoxButtonData* data = MBOkData;
   int numButtons = 1;

   switch(buttons)
   {
      case MBOk:                 data = MBOkData;                 numButtons = 1; break;
      case MBOkCancel:           data = MBOkCancelData;           numButtons = 2; break;
      case MBRetryCancel:        data = MBRetryCancelData;        numButtons = 2; break;
      case MBSaveDontSave:       data = MBSaveDontSaveData;       numButtons = 2; break;
      case MBSaveDontSaveCancel: data = MBSaveDontSaveCancelData; numButtons = 3; break;
   }

   const S32 res = showBox( title, message, data, numButtons, icon );
   if ( res == 0 )
      printAlert( title, message );

   return res;
}

//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
   alert( windowTitle, message, MBOkData, 1, MIWarning );
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
   return alert( windowTitle, message, MBOkCancelData, 2, MIWarning );
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
   return alert( windowTitle, message, MBRetryCancelData, 2, MIWarning );
}

//--------------------------------------
bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
   return alert( windowTitle, message, MBYesNoData, 2, MIQuestion );
}
