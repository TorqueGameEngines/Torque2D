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

#include "platformWin32/platformWin32.h"
#include "platform/platform.h"
#include "platformWin32/winWindow.h"
#include "platformWin32/platformGL.h"
#include "platform/platformVideo.h"
#include "platformWin32/winOGLVideo.h"
#include "platform/event.h"
#include "console/console.h"
#include "platformWin32/winConsole.h"
#include "platformWin32/winDirectInput.h"
#include "game/gameInterface.h"
#include "math/mRandom.h"
#include "io/fileStream.h"
#include "game/resource.h"
#include "string/unicode.h"
#include "gui/guiCanvas.h"
#include "platform/menus/popupMenu.h"
#include "debug/profiler.h"

//-------------------------------------- Resource Includes
#include "graphics/gBitmap.h"

#include <sys/stat.h>

extern void createFontInit();
extern void createFontShutdown();

#ifdef UNICODE
static const UTF16 *windowClassName = L"Darkstar Window Class";
static UTF16 windowName[256] = L"Darkstar Window";
#else
static const char *windowClassName = "Darkstar Window Class";
static char windowName[256] = "Darkstar Window";
#endif

static bool gWindowCreated = false;

static bool windowNotActive = false;

static RandomLCG sgPlatRandom;
static bool sgQueueEvents;

// is keyboard input a standard (non-changing) VK keycode
#define dIsStandardVK(c) (((0x08 <= (c)) && ((c) <= 0x12)) || \
   ((c) == 0x1b) ||                    \
   ((0x20 <= (c)) && ((c) <= 0x2e)) || \
   ((0x30 <= (c)) && ((c) <= 0x39)) || \
   ((0x41 <= (c)) && ((c) <= 0x5a)) || \
   ((0x70 <= (c)) && ((c) <= 0x7B)))

extern U16  DIK_to_Key( U8 dikCode );

Win32PlatState winState;

//--------------------------------------
Win32PlatState::Win32PlatState()
{
   log_fp      = NULL;
   hinstOpenGL = NULL;
   hinstGLU    = NULL;
   hinstOpenAL = NULL;
   appWindow   = NULL;
   appDC       = NULL;
   appInstance = NULL;
   currentTime = 0;
   processId   = 0;
   appMenu     = NULL;
   nMessagesPerFrame = 10; 
}

static bool windowLocked = false;

static BYTE keyboardState[256];
static bool mouseButtonState[3];
static bool capsLockDown = false;
static S32 modifierKeys = 0;
static bool windowActive = true;
static Point2I lastCursorPos(0,0);
static Point2I windowSize;
static bool sgDoubleByteEnabled = false;

//--------------------------------------
static const char *getMessageName(S32 msg)
{
   switch(msg)
   {
      case WM_KEYDOWN:
         return "WM_KEYDOWN";
      case WM_KEYUP:
         return "WM_KEYUP";
      case WM_SYSKEYUP:
         return "WM_SYSKEYUP";
      case WM_SYSKEYDOWN:
         return "WM_SYSKEYDOWN";
      default:
         return "Unknown!!";
   }
}


void Platform::restartInstance()
{
   if( Game->isRunning() )
   {
      //Con::errorf( "Error restarting instance, Game is still running!");
      return;
   }

   STARTUPINFO si;
   PROCESS_INFORMATION pi;


   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );


   char cen_buf[2048];
   GetModuleFileNameA( NULL, cen_buf, 2047);

#ifdef UNICODE
   UTF16 b[512];
   convertUTF8toUTF16((UTF8 *)cen_buf, b, sizeof(b));
#else
   const char* b = cen_buf;
#endif
   // Start the child process. 
   if( CreateProcess( b,
      NULL,            // Command line
      NULL,           // Process handle not inheritable
      NULL,           // Thread handle not inheritable
      FALSE,          // Set handle inheritance to FALSE
      0,              // No creation flags
      NULL,           // Use parent's environment block
      NULL,           // Use parent's starting directory 
      &si,            // Pointer to STARTUPINFO structure
      &pi )           // Pointer to PROCESS_INFORMATION structure
      != false )
   {
      WaitForInputIdle( pi.hProcess, 5000 );
      CloseHandle( pi.hProcess );
      CloseHandle( pi.hThread );
   }
}

//--------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
   ShowCursor(true);

#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   MessageBox(winState.appWindow, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
#else
   MessageBox(winState.appWindow, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OK);
#endif
}

//--------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
   ShowCursor(true);

#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   return MessageBox(winState.appWindow, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
#else
   return MessageBox(winState.appWindow, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_OKCANCEL) == IDOK;
#endif
}

//--------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
   ShowCursor(true);

#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   return (MessageBox(winState.appWindow, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
#else
   return (MessageBox(winState.appWindow, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_RETRYCANCEL) == IDRETRY);
#endif
}


//Luma: YesNo alert message
bool Platform::AlertYesNo(const char *windowTitle, const char *message)
{
   ShowCursor(true);

#ifdef UNICODE
   UTF16 m[1024], t[512];
   convertUTF8toUTF16((UTF8 *)windowTitle, t, sizeof(t));
   convertUTF8toUTF16((UTF8 *)message, m, sizeof(m));
   return MessageBox(winState.appWindow, m, t, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_YESNO) == IDYES;
#else
   return MessageBox(winState.appWindow, message, windowTitle, MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TASKMODAL | MB_YESNO) == IDYES;
#endif
}

//--------------------------------------
HIMC gIMEContext;

static void InitInput()
{
#ifndef TORQUE_LIB
#ifdef UNICODE
   gIMEContext = ImmGetContext(winState.appWindow);
   ImmReleaseContext( winState.appWindow, gIMEContext );
#endif
#endif

   dMemset( keyboardState, 0, 256 );
   dMemset( mouseButtonState, 0, sizeof( mouseButtonState ) );

   capsLockDown = (GetKeyState(VK_CAPITAL) & 0x01);

   if (capsLockDown)
   {
      keyboardState[VK_CAPITAL] |= 0x01;
   }
}

//--------------------------------------
static void setMouseClipping()
{
   ClipCursor(NULL);
   if(windowActive)
   {
      RECT r;
      GetWindowRect(winState.appWindow, &r);

      if(windowLocked)
      {
         POINT p;
         GetCursorPos(&p);
         lastCursorPos.set(p.x - r.left, p.y - r.top);

         ClipCursor(&r);

         S32 centerX = (r.right + r.left) >> 1;
         S32 centerY = (r.bottom + r.top) >> 1;

         if(!windowNotActive)
            SetCursorPos(centerX, centerY);
      }
      else
      {
         if(!windowNotActive && false)
            SetCursorPos(lastCursorPos.x + r.left, lastCursorPos.y + r.top);
      }
   }
}

//--------------------------------------
static bool sgTaskbarHidden = false;
static HWND sgTaskbar = NULL;

static void hideTheTaskbar()
{
   //    if ( !sgTaskbarHidden )
   //    {
   //       sgTaskbar = FindWindow( "Shell_TrayWnd", NULL );
   //       if ( sgTaskbar )
   //       {
   //          APPBARDATA abData;
   //          dMemset( &abData, 0, sizeof( abData ) );
   //          abData.cbSize = sizeof( abData );
   //          abData.hWnd = sgTaskbar;
   //          SHAppBarMessage( ABM_REMOVE, &abData );
   //          //ShowWindow( sgTaskbar, SW_HIDE );
   //          sgTaskbarHidden = true;
   //       }
   //    }
}

static void restoreTheTaskbar()
{
   //    if ( sgTaskbarHidden )
   //    {
   //       APPBARDATA abData;
   //       dMemset( &abData, 0, sizeof( abData ) );
   //       abData.cbSize = sizeof( abData );
   //       abData.hWnd = sgTaskbar;
   //       SHAppBarMessage( ABM_ACTIVATE, &abData );
   //       //ShowWindow( sgTaskbar, SW_SHOW );
   //       sgTaskbarHidden = false;
   //    }
}

//--------------------------------------
void Platform::enableKeyboardTranslation(void)
{
#ifdef UNICODE
   //   Con::printf("translating...");        
   ImmAssociateContext( winState.appWindow, winState.imeHandle );
#endif
}

//--------------------------------------
void Platform::disableKeyboardTranslation(void)
{
#ifdef UNICODE
   //   Con::printf("not translating...");
   ImmAssociateContext( winState.appWindow, NULL );
#endif
}

//--------------------------------------
void Platform::setMouseLock(bool locked)
{
   windowLocked = locked;
   setMouseClipping();
}

//--------------------------------------
void Platform::minimizeWindow()
{
   ShowWindow(winState.appWindow, SW_MINIMIZE);
   restoreTheTaskbar();
}

void Platform::restoreWindow()
{
   ShowWindow(winState.appWindow, SW_RESTORE);
}

//--------------------------------------
static void processKeyMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
   S32 repeatCount = lParam & 0xffff;
   S32 scanCode  = (lParam >> 16) & 0xff;
   S32 nVirtkey  =  dIsStandardVK(wParam) ? TranslateOSKeyCode(wParam) : DIK_to_Key(scanCode);
   S32 keyCode;
   if ( wParam == VK_PROCESSKEY && sgDoubleByteEnabled )
      keyCode = MapVirtualKey( scanCode, 1 );   // This is the REAL virtual key...
   else
      keyCode = wParam;

   bool extended = (lParam >> 24) & 1;
   bool repeat   = (lParam >> 30) & 1;
   bool make     = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);

   S32 newVirtKey = nVirtkey;
   switch(nVirtkey)
   {
   case KEY_ALT:
      newVirtKey = extended ? KEY_RALT : KEY_LALT;
      break;
   case KEY_CONTROL:
      newVirtKey = extended ? KEY_RCONTROL : KEY_LCONTROL;
      break;
   case KEY_SHIFT:
      newVirtKey = ( scanCode == 54 ) ? KEY_RSHIFT : KEY_LSHIFT;
      break;
   case KEY_RETURN:
      if ( extended )
         newVirtKey = KEY_NUMPADENTER;
      break;
   }

   S32 modKey = modifierKeys;

   if(make)
   {
      switch (newVirtKey)
      {
      case KEY_LSHIFT:     modifierKeys |= SI_LSHIFT; modKey = 0; break;
      case KEY_RSHIFT:     modifierKeys |= SI_RSHIFT; modKey = 0; break;
      case KEY_LCONTROL:   modifierKeys |= SI_LCTRL; modKey = 0; break;
      case KEY_RCONTROL:   modifierKeys |= SI_RCTRL; modKey = 0; break;
      case KEY_LALT:       modifierKeys |= SI_LALT; modKey = 0; break;
      case KEY_RALT:       modifierKeys |= SI_RALT; modKey = 0; break;
      }

      if(nVirtkey == KEY_CAPSLOCK)
      {
         capsLockDown = !capsLockDown;
         if(capsLockDown)
            keyboardState[keyCode] |= 0x01;
         else
            keyboardState[keyCode] &= 0xFE;
      }

      keyboardState[keyCode] |= 0x80;
   }
   else
   {
      switch (newVirtKey)
      {
      case KEY_LSHIFT:     modifierKeys &= ~SI_LSHIFT; modKey = 0; break;
      case KEY_RSHIFT:     modifierKeys &= ~SI_RSHIFT; modKey = 0; break;
      case KEY_LCONTROL:   modifierKeys &= ~SI_LCTRL; modKey = 0; break;
      case KEY_RCONTROL:   modifierKeys &= ~SI_RCTRL; modKey = 0; break;
      case KEY_LALT:       modifierKeys &= ~SI_LALT; modKey = 0; break;
      case KEY_RALT:       modifierKeys &= ~SI_RALT; modKey = 0; break;
      }

      keyboardState[keyCode] &= 0x7f;
   }

   U16  ascii[3];
   WORD asciiCode = 0;
   dMemset( &ascii, 0, sizeof( ascii ) );

   S32 res = ToUnicode( keyCode, scanCode, keyboardState, ascii, 3, 0 );

   // This should only happen on Window 9x/ME systems
   if (res == 0)
      res = ToAscii( keyCode, scanCode, keyboardState, ascii, 0 );

   if (res == 2)
   {
      asciiCode = ascii[1];
   }
   else if ((res == 1) || (res < 0))
   {
      asciiCode = ascii[0];
   }

   InputEvent event;
   event.deviceInst = 0;
   event.deviceType = KeyboardDeviceType;
   event.objType    = SI_KEY;
   event.objInst    = newVirtKey;
   event.action     = make ? (repeat ? SI_REPEAT : SI_MAKE ) : SI_BREAK;
   event.modifier   = modKey;
   event.ascii      = asciiCode;
   event.fValues[0]     = make ? 1.0f : 0.0f;

   // Store the current modifier keys
   Input::setModifierKeys(modifierKeys);

   Game->postEvent(event);
}

static void processCharMessage( WPARAM wParam, LPARAM lParam )
{
   TCHAR charCode = wParam;

   UTF16 tmpString[2] = { charCode, 0 };
   UTF8 out[256];

   convertUTF16toUTF8(tmpString, out, 250);

   // Con::printf("Got IME char code %x (%s)", charCode, out);

   InputEvent event;

   event.deviceInst = 0;
   event.deviceType = KeyboardDeviceType;
   event.objType    = SI_KEY;
   event.action     = SI_MAKE;
   event.ascii      = charCode;

   // Deal with some silly cases like <BS>. This might be indicative of a 
   // missing MapVirtualKey step, not quite sure how to deal with this.
   if(event.ascii == 0x8)
      event.objInst = KEY_BACKSPACE; //  Note we abuse objInst.

   Game->postEvent(event);

   // And put it back up as well, they can't hold it!
   event.action     = SI_BREAK;   
   Game->postEvent(event);
}

static S32 mouseX = 0xFFFFFFFF;
static S32 mouseY = 0xFFFFFFFF;

//--------------------------------------
static void CheckCursorPos()
{
   if(windowLocked && windowActive)
   {
      POINT mousePos;
      GetCursorPos(&mousePos);
      RECT r;

      GetWindowRect(winState.appWindow, &r);

      S32 centerX = (r.right + r.left) >> 1;
      S32 centerY = (r.bottom + r.top) >> 1;

      if(mousePos.x != centerX)
      {
         InputEvent event;

         event.deviceInst = 0;
         event.deviceType = MouseDeviceType;
         event.objType = SI_XAXIS;
         event.objInst = 0;
         event.action = SI_MOVE;
         event.modifier = modifierKeys;
         event.ascii = 0;
         event.fValues[0] = (F32)(mousePos.x - centerX);
         Game->postEvent(event);
      }

      if(mousePos.y != centerY)
      {
         InputEvent event;

         event.deviceInst = 0;
         event.deviceType = MouseDeviceType;
         event.objType = SI_YAXIS;
         event.objInst = 0;
         event.action = SI_MOVE;
         event.modifier = modifierKeys;
         event.ascii = 0;
         event.fValues[0] = (F32)(mousePos.y - centerY);
         Game->postEvent(event);
      }

      SetCursorPos(centerX, centerY);
   }
}

//--------------------------------------
static void mouseButtonEvent(S32 action, S32 objInst)
{
   CheckCursorPos();

   if(!windowLocked)
   {
      if(action == SI_MAKE)
         SetCapture(winState.appWindow);
      else
         ReleaseCapture();
   }

   U32 buttonId = objInst - KEY_BUTTON0;
   if ( buttonId < 3 )
      mouseButtonState[buttonId] = ( action == SI_MAKE ) ? true : false;

   InputEvent event;

   event.deviceInst = 0;
   event.deviceType = MouseDeviceType;
   event.objType = SI_BUTTON;
   event.objInst = objInst;
   event.action = action;
   event.modifier = modifierKeys;
   event.ascii = 0;
   event.fValues[0] = action == SI_MAKE ? 1.0f : 0.0f;

   Game->postEvent(event);
}

//--------------------------------------
static void mouseWheelEvent( S32 delta )
{
   static S32 _delta = 0;

   _delta += delta;
   if ( abs( delta ) >= WHEEL_DELTA )
   {
      _delta = 0;
      InputEvent event;

      event.deviceInst = 0;
      event.deviceType = MouseDeviceType;
      event.objType = SI_ZAXIS;
      event.objInst = 0;
      event.action = SI_MOVE;
      event.modifier = modifierKeys;
      event.ascii = 0;
      event.fValues[0] = (F32)delta;

      Game->postEvent( event );
   }
}


struct WinMessage
{
   UINT message;
   WPARAM wParam;
   LPARAM lParam;
   WinMessage() {};
   WinMessage(UINT m, WPARAM w, LPARAM l) : message(m), wParam(w), lParam(l) {}
};

Vector<WinMessage> sgWinMessages;


#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND             0x0000
#endif

bool sgKeyboardStateDirty = false;

//--------------------------------------
static LRESULT PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch ( message )
   {
   // Window DragDrop-enabled [12/14/2006 justind]
   case WM_CREATE:
      DragAcceptFiles(hWnd, TRUE);
      break;
   case WM_DESTROY:
      DragAcceptFiles(hWnd, FALSE);
      break;
   case WM_DROPFILES:
      // Retrieve Number of files
      
      // Strict policy on these things, better safe than sorry
      if( !Con::isFunction( "onDropBegin") || !Con::isFunction("onDropFile") 
         || !Con::isFunction("onDropEnd") )
         break;

      int nFileCount; // Number of Files
      HDROP hTheDrop; // The Drop Handle
      hTheDrop = (HDROP)wParam;
      nFileCount = DragQueryFile(hTheDrop, 0xFFFFFFFF, NULL, 0);

      if( nFileCount == 0 )
         break;

      // Notify Drop-Begin
      Con::executef( 2, "onDropBegin", Con::getIntArg( nFileCount ) );

         int nI;
         for (nI = 0; nI < nFileCount; nI++)
         {
            LPTSTR pszTheBuffer[MAX_PATH];
            ZeroMemory( pszTheBuffer, MAX_PATH );

            // Query it
            // FIXME: Deal with Unicode
            DragQueryFileA(hTheDrop, nI, (LPSTR)pszTheBuffer, sizeof(pszTheBuffer));

            // Notify Drop
            if (Platform::isFile( (const char*)pszTheBuffer ))
            {
               // The timeout for waiting for files (ms).
               U32 timeout = 5000;

               // Need to make sure the file is copyable. When ganking images from Firefox, it
               // isn't necessarily since Firefox insists on redownloading the file instead of
               // using the local copy.
               U32 time = Platform::getRealMilliseconds() + timeout;
               HANDLE hfile;
               while((hfile = ::CreateFileA((const char*)pszTheBuffer, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
               {
                  // Don't wait too long.
                  if(Platform::getRealMilliseconds() > time)
                     break;
               }
               if(hfile == INVALID_HANDLE_VALUE)
                  continue;

               CloseHandle(hfile);
               Con::executef( 2, "onDropFile", StringTable->insert((const char*)pszTheBuffer) );
            }
         }
         DragFinish(hTheDrop);

         // Notify Drop-Begin
         Con::executef( 2, "onDropEnd", Con::getIntArg( nFileCount ) );
      break;

   // Window Min Extents - [9/20/2006 justind]
   case WM_GETMINMAXINFO:
      MINMAXINFO *winfo;
      winfo = (MINMAXINFO*)(lParam);
      //MIN_RESOLUTION defined in platformWin32/platformGL.h
      winfo->ptMinTrackSize.x = MIN_RESOLUTION_X;
      winfo->ptMinTrackSize.y = MIN_RESOLUTION_Y;
      break;
   case WM_POWERBROADCAST:
      {
         if(wParam == PBT_APMQUERYSUSPEND)
            return BROADCAST_QUERY_DENY;
         return true;
      }
   case WM_ACTIVATEAPP:
      if ((bool) wParam)
      {         
         Video::reactivate();

         if ( Video::isFullScreen() )
            hideTheTaskbar();

         // HACK:  Windows 98 (after switching from fullscreen to windowed mode)
         SetForegroundWindow( winState.appWindow );

         // If our keyboard state is dirty, clear it
         if( sgKeyboardStateDirty == true )
         {
            sgKeyboardStateDirty = false;
            InitInput();
         }
      }
      else
      {
         // Window lost focus so set a dirty flag on the keyboard state
         if ( lParam == 0 )
            sgKeyboardStateDirty = true;
         
         Video::deactivate();
         restoreTheTaskbar();

      }
      break;
   case WM_SYSCOMMAND:
      switch(wParam)
      {
      case SC_KEYMENU:
      case SC_SCREENSAVE:
      case SC_MONITORPOWER:
         if(GetForegroundWindow() == winState.appWindow)
         {
            return 0;
         }
         break;
      // Catch Close button press event [1/5/2007 justind]
      case SC_CLOSE:
         if (Con::isFunction("onClosePressed"))
         {
            Con::executef( 1, "onClosePressed" );
            return 0;
         }
         break;
      }
      break;
   case WM_ACTIVATE:
      windowActive = LOWORD(wParam) != WA_INACTIVE;
      if( Game->isRunning() ) 
      {
         if (Con::isFunction("onWindowFocusChange"))
            Con::executef( 2, "onWindowFocusChange", windowActive ? "1" : "0" );
      }

      if ( windowActive )
      {
         setMouseClipping();
         windowNotActive = false;

         Game->refreshWindow();
         Input::activate();
      }
      else
      {
         setMouseClipping();
         windowNotActive = true;

         DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
         if ( !mgr || !mgr->isMouseActive() )
         {
            // Deactivate all the mouse triggers:
            for ( U32 i = 0; i < 3; i++ )
            {
               if ( mouseButtonState[i] )
                  mouseButtonEvent( SI_BREAK, KEY_BUTTON0 + i );
            }
         }
         Input::deactivate();
      }
      break;
   case WM_MOVE:      
      Game->refreshWindow();
      break;
   // This override will keep windows from blacking out the background, wee!
   case WM_ERASEBKGND:
      return 0;
   case WM_SIZE:
      if(wParam != SIZE_MINIMIZED) // DAW: If window is minimized, don't do an internal resize or the universe will end
      {
         S32 nWidth, nHeight;
         nWidth  = LOWORD(lParam);
         nHeight = HIWORD(lParam);

         Platform::setWindowSize( nWidth, nHeight );
         
         Game->gameReactivate();

         // MP: Commented out the if check to expose this variable no matter what the resizing is. This will help out the teams
         // that rely on knowing the windowRes for their tools (like Black Jack's card generator).
         //
         // If we're greater than MIN_RESOLUTION and less than our client desktop area, store as windowed mode pref resolution
         //if( ( nWidth > MIN_RESOLUTION_X && nWidth < winState.desktopClientWidth ) && ( nHeight > MIN_RESOLUTION_Y && nHeight < winState.desktopClientHeight ) )
         //{
            char tempBuf[32];
            dSprintf( tempBuf, sizeof(char) * 32, "%d %d %d", nWidth,nHeight, 16 );
            Con::setVariable( "$pref::Video::windowedRes", tempBuf );
         //}
      }
      else
      {
         // Deactivate ourselves when we're hidden
         Game->gameDeactivate( true );
      }
      break;
   case MM_MCINOTIFY:
      //handleRedBookCallback(wParam, lParam);
      break;

      //case WM_DESTROY:
   case WM_CLOSE:
      PostQuitMessage(0);
      break;
   case WM_SETFOCUS:
      winState.renderThreadBlocked = false;
      break;
   case WM_KILLFOCUS:
      winState.renderThreadBlocked = true;
      break;
   case WM_COMMAND:
      if(HIWORD(wParam) == 0)
      {
         winState.renderThreadBlocked = false;
         // This command came from a menu choice.  Pass along the menu ID to a
         // script function

         // [tom, 8/21/2006] Pass off to the relevant PopupMenu
         S32 numItems = GetMenuItemCount(winState.appMenu);
         for(S32 i = 0;i < numItems;i++)
         {
            MENUITEMINFOA mi;
            mi.cbSize = sizeof(mi);
            mi.fMask = MIIM_DATA;
            if(GetMenuItemInfoA(winState.appMenu, i, TRUE, &mi))
            {
               if(mi.fMask & MIIM_DATA)
               {
                  PopupMenu *mnu = (PopupMenu *)mi.dwItemData;
                  if(mnu->canHandleID(LOWORD(wParam)))
                     mnu->handleSelect(LOWORD(wParam));
               }
            }
         }
      }
      break;
   case WM_MENUSELECT:
      winState.renderThreadBlocked = true;
   case WM_PAINT:
      if(winState.renderThreadBlocked)
         Canvas->renderFrame(false);
      break;
#ifdef UNICODE
   case WM_INPUTLANGCHANGE:
      //         Con::printf("IME lang change");
      break;

   case WM_IME_SETCONTEXT:
      //         Con::printf("IME set context");
      break;

   case WM_IME_COMPOSITION:
      //         Con::printf("IME comp");
      break;

   case WM_IME_STARTCOMPOSITION:
      //         Con::printf("IME start comp");
      break;
#endif
   default:
      {
         if (sgQueueEvents)
         {
            WinMessage msg(message,wParam,lParam);

            sgWinMessages.push_front(msg);
         }
      }
   }

   return DefWindowProc(hWnd, message, wParam, lParam);
}

//--------------------------------------
static void OurDispatchMessages()
{
   WinMessage msg(0,0,0);
   UINT message;
   WPARAM wParam;
   LPARAM lParam;

   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );

   while (sgWinMessages.size())
   {
      msg = sgWinMessages.front();
      sgWinMessages.pop_front();
      message = msg.message;
      wParam = msg.wParam;
      lParam = msg.lParam;

      if ( !mgr || !mgr->isMouseActive() )
      {
         switch ( message )
         {
#ifdef UNICODE
case WM_IME_COMPOSITION:
   {
      //               Con::printf("OMG IME comp");
      break;
   }
case WM_IME_ENDCOMPOSITION:
   {
      //               Con::printf("OMG IME end comp");
      break;
   }

case WM_IME_NOTIFY:
   {
      //               Con::printf("OMG IME notify");
      break;
   }

case WM_IME_CHAR:
   processCharMessage( wParam, lParam );
   break;
#endif
   // We want to show the system cursor whenever we leave
   // our window, and it'd be simple, except for one problem:
   // showcursor isn't a toggle.  so, keep hammering it until
   // the cursor is *actually* going to be shown.
case WM_NCMOUSEMOVE:
   {
      while (ShowCursor(TRUE) < 0);
      break;
   }

case WM_MOUSEMOVE:
   // keep trying until we actually show it
   while (ShowCursor(FALSE) >= 0);
   Input::refreshCursor();

   if ( !windowLocked )
   {
      MouseMoveEvent event;
      S16 xPos = LOWORD(lParam);
      S16 yPos = HIWORD(lParam);

      event.xPos = xPos;  // horizontal position of cursor
                  if( winState.appMenu != NULL )
                  {
                      S32 menuHeight = GetSystemMetrics(SM_CYMENU);
                      event.yPos = ( yPos );  // vertical position of cursor
                  }
                  else
                     event.yPos = yPos;  // vertical position of cursor
      event.modifier = modifierKeys;

      Game->postEvent(event);
   }
   break;
case WM_SETCURSOR:
   if ((LOWORD(lParam) == HTCLIENT) && !(Canvas->getUseNativeCursor()))
      SetCursor(NULL);
   break;
case WM_LBUTTONDOWN:
   mouseButtonEvent(SI_MAKE, KEY_BUTTON0);
   break;
case WM_MBUTTONDOWN:
   mouseButtonEvent(SI_MAKE, KEY_BUTTON2);
   break;
case WM_RBUTTONDOWN:
   mouseButtonEvent(SI_MAKE, KEY_BUTTON1);
   break;
case WM_LBUTTONUP:
   mouseButtonEvent(SI_BREAK, KEY_BUTTON0);
   break;
case WM_MBUTTONUP:
   mouseButtonEvent(SI_BREAK, KEY_BUTTON2);
   break;
case WM_RBUTTONUP:
   mouseButtonEvent(SI_BREAK, KEY_BUTTON1);
   break;
case WM_MOUSEWHEEL:
   mouseWheelEvent( (S16) HIWORD( wParam ) );
   break;
         }
      }

      if ( !mgr || !mgr->isKeyboardActive() )
      {
         switch ( message )
         {
         case WM_KEYUP:
         case WM_SYSKEYUP:
         case WM_KEYDOWN:
         case WM_SYSKEYDOWN:
            processKeyMessage(message, wParam, lParam);
            break;
         }
      }
   }
}

//--------------------------------------
static bool ProcessMessages()
{
   MSG msg;

   PROFILE_SCOPE(ProcessMessages_MAIN);
   S32 nMessagesDispatched = 0;
   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
   {
      if(msg.message == WM_QUIT)
         return false;

      PROFILE_START(TranslateMessage);
      TranslateMessage(&msg);
      PROFILE_END();

      PROFILE_START(DispatchMessage);
      DispatchMessage(&msg);
      PROFILE_END();

      PROFILE_START(OurDispatchMessages);
      OurDispatchMessages();
      PROFILE_END();

      nMessagesDispatched++;
      if( nMessagesDispatched >= winState.nMessagesPerFrame )
         break;
   }

   return true;
}

//--------------------------------------
void Platform::process()
{
   PROFILE_START(GetInputManager);
   DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   PROFILE_END();

   if ( !mgr || !mgr->isMouseActive() )
   {
      PROFILE_SCOPE(InputManagerCheckCursor);
      CheckCursorPos();
   }


   PROFILE_START(WinConsoleProcess);
   WindowsConsole->process();
   PROFILE_END();

   PROFILE_START(ProcessMessages);
   if(!ProcessMessages())
   {
      PROFILE_SCOPE(PostEvent);
      // generate a quit event
      Event quitEvent;
      quitEvent.type = QuitEventType;

      Game->postEvent(quitEvent);
   }
   PROFILE_END();

   PROFILE_START(GetForegroundWindow);
   HWND window = GetForegroundWindow();
   PROFILE_END();

   // assume that we are in the foreground, 
   winState.backgrounded = false;
   // unless we see that the foreground window does not belong to this process.
   if (window && window != winState.appWindow && gWindowCreated)
   {
      DWORD foregroundProcessId;
      GetWindowThreadProcessId(window, &foregroundProcessId);
      if (foregroundProcessId != winState.processId)
         winState.backgrounded = true;
   }

   PROFILE_SCOPE(InputProcess);
   Input::process();
}

extern U32 calculateCRC(void * buffer, S32 len, U32 crcVal );

#if defined(TORQUE_DEBUG) || defined(INTERNAL_RELEASE)
static U32 stubCRC = 0;
#else
static U32 stubCRC = 0xEA63F56C;
#endif

//--------------------------------------
static void InitWindowClass()
{
   WNDCLASS wc;
   dMemset(&wc, 0, sizeof(wc));

   wc.style         = CS_OWNDC;
   wc.lpfnWndProc   = WindowProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = winState.appInstance;
   wc.hIcon         = LoadIcon(winState.appInstance, MAKEINTRESOURCE(IDI_TORQUE2D));
   wc.hCursor       = LoadCursor (NULL,IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
   wc.lpszMenuName  = 0;
   wc.lpszClassName = windowClassName;
   RegisterClass( &wc );

   // Curtain window class:
   wc.lpfnWndProc   = DefWindowProc;
   wc.hCursor       = NULL;
   wc.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
   wc.lpszClassName = dT("Curtain");
   RegisterClass( &wc );
}

//--------------------------------------
Resolution Video::getDesktopResolution()
{
   Resolution  Result;
   RECT        clientRect;
   HWND        hDesktop  = GetDesktopWindow();
   HDC         hDeskDC   = GetDC( hDesktop );

   // Retrieve Resolution Information.
   Result.bpp  = winState.desktopBitsPixel = GetDeviceCaps( hDeskDC, BITSPIXEL );
   Result.w    = winState.desktopWidth     = GetDeviceCaps( hDeskDC, HORZRES );
   Result.h    = winState.desktopHeight    = GetDeviceCaps( hDeskDC, VERTRES );

   // Release Device Context.
   ReleaseDC( hDesktop, hDeskDC );

   SystemParametersInfo(SPI_GETWORKAREA, 0, &clientRect, 0);
   winState.desktopClientWidth = clientRect.right;
   winState.desktopClientHeight = clientRect.bottom;

   // Return Result;
   return Result;
}

//--------------------------------------
HWND CreateOpenGLWindow( U32 width, U32 height, bool fullScreen, bool allowSizing )
{
   S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
   S32 exWindowStyle = 0;

   if ( fullScreen )
      windowStyle |= ( WS_POPUP | WS_MAXIMIZE );
   else
      if (!allowSizing)
         windowStyle |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
      else
         windowStyle |= ( WS_OVERLAPPEDWINDOW );

   return CreateWindowEx(
      exWindowStyle,
      windowClassName,
      windowName,
      windowStyle,
      0, 0, width, height,
      NULL, NULL,
      winState.appInstance,
      NULL);
}

//--------------------------------------
HWND CreateCurtain( U32 width, U32 height )
{
   return CreateWindow(
      dT("Curtain"),
      dT(""),
      ( WS_POPUP | WS_MAXIMIZE ),
      0, 0,
      width, height,
      NULL, NULL,
      winState.appInstance,
      NULL );
}

//--------------------------------------
void CreatePixelFormat( PIXELFORMATDESCRIPTOR *pPFD, S32 colorBits, S32 depthBits, S32 stencilBits, bool stereo )
{
   PIXELFORMATDESCRIPTOR src =
   {
      sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
         1,                      // version number
         PFD_DRAW_TO_WINDOW |    // support window
         PFD_SUPPORT_OPENGL |    // support OpenGL
         PFD_DOUBLEBUFFER,       // double buffered
         PFD_TYPE_RGBA,          // RGBA type
         colorBits,              // color depth
         0, 0, 0, 0, 0, 0,       // color bits ignored
         0,                      // no alpha buffer
         0,                      // shift bit ignored
         0,                      // no accumulation buffer
         0, 0, 0, 0,             // accum bits ignored
         depthBits,              // z-buffer
         stencilBits,            // stencil buffer
         0,                      // no auxiliary buffer
         PFD_MAIN_PLANE,         // main layer
         0,                      // reserved
         0, 0, 0                 // layer masks ignored
   };

   if ( stereo )
   {
      //ri.Printf( PRINT_ALL, "...attempting to use stereo\n" );
      src.dwFlags |= PFD_STEREO;
      //glConfig.stereoEnabled = true;
   }
   else
   {
      //glConfig.stereoEnabled = qfalse;
   }
   *pPFD = src;
}

//--------------------------------------
enum WinConstants { MAX_PFDS = 256 };

#ifndef PFD_GENERIC_ACCELERATED
#define PFD_GENERIC_ACCELERATED     0x00001000
#endif

S32 ChooseBestPixelFormat(HDC hDC, PIXELFORMATDESCRIPTOR *pPFD)
{
   PIXELFORMATDESCRIPTOR pfds[MAX_PFDS+1];
   S32 i;
   S32 bestMatch = 0;

   S32 maxPFD = dwglDescribePixelFormat(hDC, 1, sizeof(PIXELFORMATDESCRIPTOR), &pfds[0]);
   if(maxPFD > MAX_PFDS)
      maxPFD = MAX_PFDS;

   bool accelerated = false;

   for(i = 1; i <= maxPFD; i++)
   {
      dwglDescribePixelFormat(hDC, i, sizeof(PIXELFORMATDESCRIPTOR), &pfds[i]);

      // make sure this has hardware acceleration:
      if ( ( pfds[i].dwFlags & PFD_GENERIC_FORMAT ) != 0 )
         continue;

      // verify pixel type
      if ( pfds[i].iPixelType != PFD_TYPE_RGBA )
         continue;

      // verify proper flags
      if ( ( ( pfds[i].dwFlags & pPFD->dwFlags ) & pPFD->dwFlags ) != pPFD->dwFlags )
         continue;

      accelerated = !(pfds[i].dwFlags & PFD_GENERIC_FORMAT);

      //
      // selection criteria (in order of priority):
      //
      //  PFD_STEREO
      //  colorBits
      //  depthBits
      //  stencilBits
      //
      if ( bestMatch )
      {
         // check stereo
         if ( ( pfds[i].dwFlags & PFD_STEREO ) && ( !( pfds[bestMatch].dwFlags & PFD_STEREO ) ) && ( pPFD->dwFlags & PFD_STEREO ) )
         {
            bestMatch = i;
            continue;
         }

         if ( !( pfds[i].dwFlags & PFD_STEREO ) && ( pfds[bestMatch].dwFlags & PFD_STEREO ) && ( pPFD->dwFlags & PFD_STEREO ) )
         {
            bestMatch = i;
            continue;
         }

         // check color
         if ( pfds[bestMatch].cColorBits != pPFD->cColorBits )
         {
            // prefer perfect match
            if ( pfds[i].cColorBits == pPFD->cColorBits )
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if ( pfds[i].cColorBits > pfds[bestMatch].cColorBits )
            {
               bestMatch = i;
               continue;
            }
         }

         // check depth
         if ( pfds[bestMatch].cDepthBits != pPFD->cDepthBits )
         {
            // prefer perfect match
            if ( pfds[i].cDepthBits == pPFD->cDepthBits )
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if ( pfds[i].cDepthBits > pfds[bestMatch].cDepthBits )
            {
               bestMatch = i;
               continue;
            }
         }

         // check stencil
         if ( pfds[bestMatch].cStencilBits != pPFD->cStencilBits )
         {
            // prefer perfect match
            if ( pfds[i].cStencilBits == pPFD->cStencilBits )
            {
               bestMatch = i;
               continue;
            }
            // otherwise if this PFD has more bits than our best, use it
            else if ( ( pfds[i].cStencilBits > pfds[bestMatch].cStencilBits ) &&
               ( pPFD->cStencilBits > 0 ) )
            {
               bestMatch = i;
               continue;
            }
         }
      }
      else
      {
         bestMatch = i;
      }
   }

   if ( !bestMatch )
      return 0;

   else if ( pfds[bestMatch].dwFlags & PFD_GENERIC_ACCELERATED )
   {
      // MCD
   }
   else
   {
      // ICD
   }

   *pPFD = pfds[bestMatch];

   return bestMatch;
}

//--------------------------------------
//
// This function exists so DirectInput can communicate with the Windows mouse
// in windowed mode.
//
//--------------------------------------
void setModifierKeys( S32 modKeys )
{
   modifierKeys = modKeys;
}

//--------------------------------------
const Point2I &Platform::getWindowSize()
{
   return windowSize;
}

//--------------------------------------
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
   windowSize.set( newWidth, newHeight );
}

//RectI Platform::getWindowClientRect()
//{
//   RECT rClientRect;
//   GetClientRect( winState.appWindow, &rClientRect );
//
//   if( winState.appMenu == NULL )
//      return RectI( rClientRect.left, rClientRect.top, rClientRect.right - rClientRect.left, rClientRect.bottom - rClientRect.top );
//   
//   S32 menuHeight = GetSystemMetrics(SM_CYMENU);
//   return RectI( rClientRect.left, rClientRect.top + menuHeight, rClientRect.right - rClientRect.left, rClientRect.bottom - ( rClientRect.top + menuHeight ) );
//}

//--------------------------------------
static void InitWindow(const Point2I &initialSize)
{
   windowSize = initialSize;

   // The window is created when the display device is activated. BH

   winState.processId = GetCurrentProcessId();
}

//--------------------------------------
static void InitOpenGL()
{
   // The OpenGL initialization stuff has been mostly moved to the display
   // devices' activate functions. BH

   DisplayDevice::init();

   bool fullScreen = Con::getBoolVariable( "$pref::Video::fullScreen" );

   const char* resString = Con::getVariable( ( fullScreen  ? "$pref::Video::resolution" : "$pref::Video::windowedRes" ) );

   // Get the video settings from the prefs:
   char* tempBuf = new char[dStrlen( resString ) + 1];
   dStrcpy( tempBuf, resString );
   char* temp = dStrtok( tempBuf, " x\0" );
   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   U32 width = ( temp ? dAtoi( temp ) : MIN_RESOLUTION_X );
   temp = dStrtok( NULL, " x\0" );
   U32 height = ( temp ? dAtoi( temp ) : MIN_RESOLUTION_Y );
   temp = dStrtok( NULL, "\0" );
   U32 bpp = ( temp ? dAtoi( temp ) : MIN_RESOLUTION_BIT_DEPTH );
   delete [] tempBuf;



   // If no device is specified, see which ones we have...
   if ( !Video::setDevice( Con::getVariable( "$pref::Video::displayDevice" ), width, height, bpp, fullScreen ) )
   {
      // First, try the default OpenGL device:
      if ( !Video::setDevice( "OpenGL", width, height, bpp, fullScreen ) )
      {             
         AssertFatal( false, "Could not find a compatible display device!" );
          return;
      }
   }
}

//--------------------------------------
void Platform::init()
{
   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "windows" );

   WinConsole::create();
   if ( !WinConsole::isEnabled() )
      Input::init();
   InitInput();   // in case DirectInput falls through
   InitWindowClass();
   Video::getDesktopResolution();
   //installRedBookDevices();

   sgDoubleByteEnabled = GetSystemMetrics( SM_DBCSENABLED );
   sgQueueEvents = true;
}

//--------------------------------------
void Platform::shutdown()
{
   sgQueueEvents = false;   
   setMouseLock( false );
   Audio::OpenALShutdown();
   Video::destroy();
   Input::destroy();
   WinConsole::destroy();
}


class WinTimer
{
private:
   U32 mTickCountCurrent;
   U32 mTickCountNext;
   S64 mPerfCountCurrent;
   S64 mPerfCountNext;
   S64 mFrequency;
   F64 mPerfCountRemainderCurrent;
   F64 mPerfCountRemainderNext;
   bool mUsingPerfCounter;
public:
   WinTimer()
   {
      SetProcessAffinityMask( GetCurrentProcess(), 1 );

      mPerfCountRemainderCurrent = 0.0f;
      mUsingPerfCounter = QueryPerformanceFrequency((LARGE_INTEGER *) &mFrequency);
      if(mUsingPerfCounter)
         mUsingPerfCounter = QueryPerformanceCounter((LARGE_INTEGER *) &mPerfCountCurrent);
      if(!mUsingPerfCounter)
         mTickCountCurrent = GetTickCount();
   }
   U32 getElapsedMS()
   {
      if(mUsingPerfCounter)
      {
         QueryPerformanceCounter( (LARGE_INTEGER *) &mPerfCountNext);
         F64 elapsedF64 = (1000.0 * F64(mPerfCountNext - mPerfCountCurrent) / F64(mFrequency));
         elapsedF64 += mPerfCountRemainderCurrent;
         U32 elapsed = (U32)mFloor((F32)elapsedF64);
         mPerfCountRemainderNext = elapsedF64 - F64(elapsed);
         return elapsed;
      }
      else
      {
         mTickCountNext = GetTickCount();
         return mTickCountNext - mTickCountCurrent;
      }
   }
   void advance()
   {
      mTickCountCurrent = mTickCountNext;
      mPerfCountCurrent = mPerfCountNext;
      mPerfCountRemainderCurrent = mPerfCountRemainderNext;
   }
};

static WinTimer gTimer;

extern bool LinkConsoleFunctions;

//--------------------------------------
static S32 run(S32 argc, const char **argv)
{
    // Initialize fonts.
    createFontInit();

    windowSize.set(0,0);

    // Finish if the game didn't initialize.
    if(!Game->mainInitialize(argc, argv) )
        return 0;

    // run the game main loop.
    while( Game->isRunning() )
    {
        Game->mainLoop();
    }

    // Shut the game down.
    Game->mainShutdown();

    // Destroy fonts.
    createFontShutdown();

    return 0;
}

//--------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
   MSG msg;

   Con::printSeparator();
   Con::printf("Video Initialization:");
   Video::init();

   PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
   if ( Video::installDevice( OpenGLDevice::create() ) )
      Con::printf( "   Accelerated OpenGL display device detected." );
   else
      Con::printf( "   Accelerated OpenGL display device not detected." );
   Con::printf( "" );

   gWindowCreated = true;
#ifdef UNICODE
   convertUTF8toUTF16((UTF8 *)name, windowName, sizeof(windowName));
#else
   dStrcpy(windowName, name);
#endif
   PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
   InitWindow(initialSize);
   PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
   InitOpenGL();
   PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
}

void Platform::setWindowTitle( const char* title )
{
   if( !title || !title[0] )
      return;

#ifdef UNICODE
   convertUTF8toUTF16((UTF8 *)title, windowName, sizeof(windowName));
#else
   dStrcpy(windowName, title);
#endif

   if( winState.appWindow )
#ifdef UNICODE
      SetWindowTextW( winState.appWindow, windowName );
#else
      SetWindowTextA( winState.appWindow, windowName );
#endif
}


//--------------------------------------
S32 main(S32 argc, const char **argv)
{
   winState.appInstance = GetModuleHandle(NULL);
   return run(argc, argv);
}

//--------------------------------------
S32 PASCAL WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, S32)
{
   Vector<char *> argv;
   char moduleName[256];
   GetModuleFileNameA(NULL, moduleName, sizeof(moduleName));
   argv.push_back(moduleName);

   for (const char* word,*ptr = lpszCmdLine; *ptr; )  {
      // Eat white space
      for (; dIsspace(*ptr) && *ptr; ptr++)
         ;

      // Pick out the next word
      bool quote = false;
      for (word = ptr; !(dIsspace(*ptr) && !quote) && *ptr; ptr++)
      {
         if(*ptr == '\"') quote = ! quote;
      }

      if(*word == '\"') ++word;
      
      // Add the word to the argument list.
      if (*word) {
         int len = ptr - word;
         if(*(ptr-1) == '\"') --len;

         char *arg = (char *) dMalloc(len + 1);
         dStrncpy(arg, word, len);
         arg[len] = 0;
         argv.push_back(arg);
      }
   }

   winState.appInstance = hInstance;

   S32 retVal = run(argv.size(), (const char **) argv.address());

   for(U32 j = 1; j < (U32)argv.size(); j++)
      dFree(argv[j]);

   return retVal;
}

//--------------------------------------
void TimeManager::process()
{
   if( winState.backgrounded )
      winState.sleepTicks = Platform::getBackgroundSleepTime();
   else
      winState.sleepTicks = sgTimeManagerProcessInterval;

   U32 elapsedTime = gTimer.getElapsedMS();
   if( elapsedTime < winState.sleepTicks)
   {
      MsgWaitForMultipleObjects(0, NULL, false, winState.sleepTicks - elapsedTime, QS_ALLINPUT);
   }
   
   TimeEvent event;
   event.elapsedTime = gTimer.getElapsedMS();
   gTimer.advance();

   Game->postEvent(event);
}

F32 Platform::getRandom()
{
   return sgPlatRandom.randF();
}

////--------------------------------------
/// Spawn the default Operating System web browser with a URL
/// @param webAddress URL to pass to browser
/// @return true if browser successfully spawned
bool Platform::openWebBrowser( const char* webAddress )
{
   static bool sHaveKey = false;
   static wchar_t sWebKey[512];
   char utf8WebKey[512];

   {
      HKEY regKey;
      DWORD size = sizeof( sWebKey );

      if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, dT("\\http\\shell\\open\\command"), 0, KEY_QUERY_VALUE, &regKey ) != ERROR_SUCCESS )
      {
         Con::errorf( ConsoleLogEntry::General, "Platform::openWebBrowser - Failed to open the HKCR\\http registry key!!!");
         return( false );
      }

      if ( RegQueryValueEx( regKey, dT(""), NULL, NULL, (unsigned char *)sWebKey, &size ) != ERROR_SUCCESS ) 
      {
         Con::errorf( ConsoleLogEntry::General, "Platform::openWebBrowser - Failed to query the open command registry key!!!" );
         return( false );
      }

      RegCloseKey( regKey );
      sHaveKey = true;

      convertUTF16toUTF8(sWebKey,utf8WebKey,512);

#ifdef UNICODE
      char *p = dStrstr(utf8WebKey, "%1"); 
#else
      char *p = dStrstr(sWebKey, "%1"); 
#endif
      if (p) *p = 0; 

   }

   STARTUPINFO si;
   dMemset( &si, 0, sizeof( si ) );
   si.cb = sizeof( si );

   char buf[1024];
#ifdef UNICODE
   dSprintf( buf, sizeof( buf ), "%s %s", utf8WebKey, webAddress );   
   UTF16 szCommandLine[1024];
   convertUTF8toUTF16((UTF8 *)buf, szCommandLine, sizeof(szCommandLine));
#else
   UTF8 szCommandLine[1024];
   dSprintf( szCommandLine, sizeof( szCommandLine ), "%s %s", sWebKey, webAddress );   
#endif

   //Con::errorf( ConsoleLogEntry::General, "** Web browser command = %s **", buf );

   PROCESS_INFORMATION pi;
   dMemset( &pi, 0, sizeof( pi ) );
   CreateProcess( NULL,
      szCommandLine,
      NULL,
      NULL,
      false,
      CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
      NULL,
      NULL,
      &si,
      &pi );

   return( true );
}

//--------------------------------------
// Platform functions for window menu
//--------------------------------------

// Creates the menu bar for the window
void CreateWin32MenuBar( void )
{
   if( HasWin32MenuBar() )
   {
      DrawMenuBar( winState.appWindow );
      return;
   }

   HMENU menu = CreateMenu();
   if(menu)
   {
      winState.appMenu = menu;
      SetMenu(winState.appWindow, menu);
      DrawMenuBar(winState.appWindow);
   }
}

void DestroyWin32MenuBar( void )
{
   if( HasWin32MenuBar() )
      DestroyMenu( winState.appMenu );

   // Need to update platform window child rect?

   DrawMenuBar(winState.appWindow);
}

bool HasWin32MenuBar( void )
{
   return (bool)IsMenu( winState.appMenu );
}
