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
// SDL_ShowMessageBox, shared by the back-ends built on SDL 2 for a desktop. The
// web build keeps its own (platformEmscripten/EmscriptenAlerts.cpp): in a
// browser SDL 2 has no SDL_ShowMessageBox.

#include "platform/platform.h"
#include "platform/nativeDialogs/msgBox.h"
#include "console/console.h"

#include <SDL.h>

// A Unix desktop falls back to zenity when SDL can't show a box (showZenityBox).
#if defined(__unix__) && !defined(__EMSCRIPTEN__) && !defined(__APPLE__)
#define TORQUE_ZENITY_MSGBOX
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

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

#ifdef TORQUE_ZENITY_MSGBOX
   //---------------------------------------------------------------------------
   // When SDL can't show a box itself. Its X11 driver draws the box in the X
   // server's core fonts, and asks for one size of them that a Wayland-first
   // desktop -- Omarchy, for one -- doesn't install, so the box fails with "No
   // message system available". SDL has a second way, zenity, which its
   // Wayland driver uses; but it only turns to another driver's box before its
   // video has started, never with a window open, so the engine asks zenity
   // itself.
   //
   // Adapted from SDL 2.32's src/video/wayland/SDL_waylandmessagebox.c
   // (Copyright (C) 1997-2025 Sam Lantinga; zlib licence). Altered: it runs
   // from here, reads zenity's output before waiting for it, picks the icon
   // from the icon bit rather than comparing all the flags (SDL's comparison
   // misses an icon combined with any other flag), and answers a box closed
   // with Escape or its close button with the box's Escape button.

   // Run zenity with argv, keeping up to outSize - 1 bytes of what it prints in
   // out; returns its exit status, or -1 if it couldn't be run.
   int runZenity( const char** argv, char* out, size_t outSize )
   {
      int fds[2];
      if ( pipe( fds ) != 0 )
         return -1;

      const pid_t pid = fork();
      if ( pid == 0 )
      {
         close( fds[0] );
         if ( dup2( fds[1], STDOUT_FILENO ) == -1 )
            _exit( 128 );
         execvp( "zenity", (char**)argv );
         _exit( 129 );
      }
      close( fds[1] );
      if ( pid < 0 )
      {
         close( fds[0] );
         return -1;
      }

      size_t used = 0;
      ssize_t got;
      while ( used < outSize - 1 && ( got = read( fds[0], out + used, outSize - 1 - used ) ) > 0 )
         used += got;
      out[used] = '\0';
      close( fds[0] );

      int status = 0;
      if ( waitpid( pid, &status, 0 ) != pid || !WIFEXITED( status ) )
         return -1;
      return WEXITSTATUS( status );
   }

   // Show the box with zenity; true if zenity showed it, with the id of the
   // button chosen in buttonId.
   bool showZenityBox( const SDL_MessageBoxData& box, int& buttonId )
   {
      if ( box.numbuttons < 1 || box.numbuttons > 3 )
         return false;

      const char* versionArgs[] = { "zenity", "--version", NULL };
      char version[32];
      if ( runZenity( versionArgs, version, sizeof( version ) ) != 0 )
         return false;
      int major = 0, minor = 0;
      sscanf( version, "%d.%d", &major, &minor );

      const char* argv[24] = { "zenity", "--question", "--switch", "--no-wrap", "--no-markup" };
      int argc = 5;

      // zenity 3.90 renamed --icon-name to --icon, with no overlap.
      argv[argc++] = ( major > 3 || ( major == 3 && minor >= 90 ) ) ? "--icon" : "--icon-name";
      if ( box.flags & SDL_MESSAGEBOX_ERROR )
         argv[argc++] = "dialog-error";
      else if ( box.flags & SDL_MESSAGEBOX_WARNING )
         argv[argc++] = "dialog-warning";
      else
         argv[argc++] = "dialog-information";

      argv[argc++] = "--title";
      argv[argc++] = box.title;
      argv[argc++] = "--text";
      argv[argc++] = box.message;
      for ( int i = 0; i < box.numbuttons; ++i )
      {
         argv[argc++] = "--extra-button";
         argv[argc++] = box.buttons[i].text;
      }
      argv[argc] = NULL;

      char chosen[128];
      const int status = runZenity( argv, chosen, sizeof( chosen ) );
      if ( status < 0 || status >= 128 )
         return false;

      // zenity prints the label of the button chosen, and a newline; nothing
      // at all if the box was closed instead.
      char* newline = strrchr( chosen, '\n' );
      if ( newline )
         *newline = '\0';

      buttonId = box.buttons[box.numbuttons - 1].buttonid;
      for ( int i = 0; i < box.numbuttons; ++i )
      {
         if ( box.buttons[i].flags & SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT )
            buttonId = box.buttons[i].buttonid;
      }
      for ( int i = 0; i < box.numbuttons; ++i )
      {
         if ( chosen[0] != '\0' && strcmp( chosen, box.buttons[i].text ) == 0 )
            buttonId = box.buttons[i].buttonid;
      }
      return true;
   }
#endif

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
      {
#ifdef TORQUE_ZENITY_MSGBOX
         if ( !showZenityBox( boxData, res ) )
#endif
            res = 0;
      }

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
