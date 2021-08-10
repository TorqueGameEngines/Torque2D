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
#include "platform/platformInput.h"
#include "platform/platformVideo.h"
#include "platformWin32/winDirectInput.h"
#include "platform/event.h"
#include "console/console.h"

#include "platform/platformInput_ScriptBinding.h"
#include "winInput_ScriptBinding.h"

// Static class variables:
InputManager*  Input::smManager;
bool           Input::smActive;
CursorManager* Input::smCursorManager = 0; //*** DAW: Added
U8             Input::smModifierKeys; //*** DAW: Added
bool           Input::smLastKeyboardActivated;
bool           Input::smLastMouseActivated;
bool           Input::smLastJoystickActivated;

static void fillAsciiTable();

//------------------------------------------------------------------------------
//
// This function gets the standard ASCII code corresponding to our key code
// and the existing modifier key state.
//
//------------------------------------------------------------------------------
struct AsciiData
{
   struct KeyData
   {
      U16   ascii;
      bool  isDeadChar;
   };

   KeyData upper;
   KeyData lower;
   KeyData goofy;
};


#define NUM_KEYS ( KEY_OEM_102 + 1 )
#define KEY_FIRST KEY_ESCAPE
static AsciiData AsciiTable[NUM_KEYS];

//------------------------------------------------------------------------------
void Input::init()
{
   Con::printSeparator();
   Con::printf( "Input Initialization:" );

   destroy();

   smActive = false;
   smLastKeyboardActivated = true;
   smLastMouseActivated = true;
   smLastJoystickActivated = true;

   smManager = new DInputManager;
   if (!smManager->enable())
   {
	   Con::printf("   DirectInput not enabled.");

	   delete smManager;
	   smManager = NULL;
   }
   else
   {
	   DInputManager::init();
	   Con::printf("   DirectInput enabled.");
   }

   // Startup the Cursor Manager
   if(!smCursorManager)
   {
      smCursorManager = new CursorManager();
      if(smCursorManager)
      {
         // Add the arrow cursor to the stack
         smCursorManager->pushCursor(CursorManager::curArrow);
      }
      else
      {
         Con::printf("   Cursor Manager not enabled.");
      }
   }

   // Init the current modifier keys
   setModifierKeys(0);
   fillAsciiTable();
   Con::printf( "" );


}

//------------------------------------------------------------------------------
static void fillAsciiTable()
{
   //HKL   layout = GetKeyboardLayout( 0 );
   U8    state[256];
   U16   ascii[2];
   U32   dikCode, vKeyCode, keyCode;
   S32   result;

   dMemset( &AsciiTable, 0, sizeof( AsciiTable ) );
   dMemset( &state, 0, sizeof( state ) );

   for ( keyCode = KEY_FIRST; keyCode < NUM_KEYS; keyCode++ )
   {
      ascii[0] = ascii[1] = 0;
      dikCode  = Key_to_DIK( keyCode );
      if ( dikCode )
      {
         //vKeyCode = MapVirtualKeyEx( dikCode, 1, layout );
         vKeyCode = MapVirtualKey( dikCode, 1 );

         // Lower case:
         ascii[0] = ascii[1] = 0;
         //result = ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
         result = ToAscii( vKeyCode, dikCode, state, ascii, 0 );

         if ( result == 2 )
            AsciiTable[keyCode].lower.ascii = ascii[1] ? ascii[1] : ( ascii[0] >> 8 );
         else if ( result == 1 )
            AsciiTable[keyCode].lower.ascii = ascii[0];
         else if ( result < 0 )
         {
            AsciiTable[keyCode].lower.ascii = ascii[0];
            AsciiTable[keyCode].lower.isDeadChar = true;
            // Need to clear the dead character from the keyboard layout:
            //ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
            ToAscii( vKeyCode, dikCode, state, ascii, 0 );
         }

         // Upper case:
         ascii[0] = ascii[1] = 0;
         state[VK_SHIFT] = 0x80;
         //result = ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
         result = ToAscii( vKeyCode, dikCode, state, ascii, 0 );
          
         if ( result == 2 )
            AsciiTable[keyCode].upper.ascii = ascii[1] ? ascii[1] : ( ascii[0] >> 8 );
         else if ( result == 1 )
            AsciiTable[keyCode].upper.ascii = ascii[0];
         else if ( result < 0 )
         {
            AsciiTable[keyCode].upper.ascii = ascii[0];
            AsciiTable[keyCode].upper.isDeadChar = true;
            // Need to clear the dead character from the keyboard layout:
            //ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
            ToAscii( vKeyCode, dikCode, state, ascii, 0 );
         }
         state[VK_SHIFT] = 0;

         // Foreign mod case:
         ascii[0] = ascii[1] = 0;
         state[VK_CONTROL] = 0x80;
         state[VK_MENU] = 0x80;
         //result = ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
         result = ToAscii( vKeyCode, dikCode, state, ascii, 0 );
          
         if ( result == 2 )
            AsciiTable[keyCode].goofy.ascii = ascii[1] ? ascii[1] : ( ascii[0] >> 8 );
         else if ( result == 1 )
            AsciiTable[keyCode].goofy.ascii = ascii[0];
         else if ( result < 0 )
         {
            AsciiTable[keyCode].goofy.ascii = ascii[0];
            AsciiTable[keyCode].goofy.isDeadChar = true;
            // Need to clear the dead character from the keyboard layout:
            //ToAsciiEx( vKeyCode, dikCode, state, ascii, 0, layout );
            ToAscii( vKeyCode, dikCode, state, ascii, 0 );
         }
         state[VK_CONTROL] = 0;
         state[VK_MENU] = 0;
      }
   }
}

//------------------------------------------------------------------------------
U16 Input::getKeyCode( U16 asciiCode )
{
   U16 keyCode = 0;
   U16 i;

   // This is done three times so the lowerkey will always
   // be found first. Some foreign keyboards have duplicate
   // chars on some keys.
   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].lower.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].upper.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].goofy.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   return( keyCode );
}

//------------------------------------------------------------------------------
U16 Input::getAscii( U16 keyCode, KEY_STATE keyState )
{
   if ( keyCode >= NUM_KEYS )
      return 0;

   switch ( keyState )
   {
   case STATE_LOWER:
      return AsciiTable[keyCode].lower.ascii;
   case STATE_UPPER:
      return AsciiTable[keyCode].upper.ascii;
   case STATE_GOOFY:
      return AsciiTable[keyCode].goofy.ascii;
   default:
      return(0);

   }
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
#ifdef UNICODE
   winState.imeHandle = ImmGetContext( winState.appWindow );
   ImmReleaseContext( winState.appWindow, winState.imeHandle );
#endif

   DInputDevice::resetModifierKeys();
   if ( !Con::getBoolVariable( "$enableDirectInput" ) )
      return;

   if ( smManager && smManager->isEnabled() && !smActive )
   {
      Con::printf( "Activating DirectInput..." );
      smActive = true;
      DInputManager* dInputManager = dynamic_cast<DInputManager*>( smManager );
      if ( dInputManager )
      {
         if ( dInputManager->isKeyboardEnabled() && smLastKeyboardActivated )
            dInputManager->activateKeyboard();

         if ( Video::isFullScreen() )
         {
            // DirectInput Mouse Hook-Up:
            if ( dInputManager->isMouseEnabled() && smLastMouseActivated )
               dInputManager->activateMouse();
         }
         else
            dInputManager->deactivateMouse();

         if ( dInputManager->isJoystickEnabled() && smLastJoystickActivated )
            dInputManager->activateJoystick();
      }
   }
}

//------------------------------------------------------------------------------
void Input::deactivate()
{
   if ( smManager && smManager->isEnabled() && smActive )
   {
      DInputManager* dInputManager = dynamic_cast<DInputManager*>( smManager );

      if ( dInputManager )
      {
         smLastKeyboardActivated = dInputManager->isKeyboardActive();
         smLastMouseActivated    = dInputManager->isMouseActive();
         smLastJoystickActivated = dInputManager->isJoystickActive();

         dInputManager->deactivateKeyboard();
         dInputManager->deactivateMouse();
         dInputManager->deactivateJoystick();
      }

      smActive = false;
      Con::printf( "DirectInput deactivated." );
   }
}

//------------------------------------------------------------------------------
void Input::reactivate()
{
   // This is soo hacky...
   SetForegroundWindow( winState.appWindow );
   PostMessage( winState.appWindow, WM_ACTIVATE, WA_ACTIVE, NULL );
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
   return smActive;
}

//------------------------------------------------------------------------------
void Input::process()
{
   if ( smManager && smManager->isEnabled() && smActive )
      smManager->process();
}

//------------------------------------------------------------------------------
// Accesses the global input manager to see if its mouse is enabled
bool Input::isMouseEnabled()
{
	DInputManager* dInputManager = dynamic_cast<DInputManager*>( smManager );

    if ( !dInputManager || !dInputManager->isEnabled() )
        return( false );
    
	if (dInputManager->isMouseEnabled() && dInputManager->isMouseActive() )
        return( true );
    
    return false;
}

//------------------------------------------------------------------------------
// Accesses the global input manager to see if its keyboard is enabled
bool Input::isKeyboardEnabled()
{
    DInputManager* dInputManager = dynamic_cast<DInputManager*>( smManager );

    if ( !dInputManager || !dInputManager->isEnabled() )
        return( false );
    
	if ( dInputManager->isKeyboardEnabled() && dInputManager->isKeyboardActive() )
        return( true );
    
    return false;
}

//------------------------------------------------------------------------------
// Access the global input manager and enables its mouse
void Input::enableMouse()
{
    DInputManager::enableMouse();
}

//------------------------------------------------------------------------------
// Access the global input manager and disables its mouse
void Input::disableMouse()
{
    DInputManager::disableMouse();
}

//------------------------------------------------------------------------------
// Access the global input manager and enables its keyboard
void Input::enableKeyboard()
{
    DInputManager::enableKeyboard();
}

//------------------------------------------------------------------------------
// Access the global input manager and enables its keyboard
void Input::disableKeyboard()
{
    DInputManager::disableKeyboard();
}

//------------------------------------------------------------------------------
bool Input::activateKeyboard()
{
	DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   
	if ( mgr )
      return( mgr->activateKeyboard() );

   return( false );
}

//------------------------------------------------------------------------------
void Input::deactivateKeyboard()
{
	DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->deactivateKeyboard();
}

//------------------------------------------------------------------------------
bool Input::enableJoystick()
{
	return( DInputManager::enableJoystick() );
}

//------------------------------------------------------------------------------
void Input::disableJoystick()
{
	DInputManager::disableJoystick();
}

//------------------------------------------------------------------------------
void Input::echoInputState()
{
    DInputManager* mgr = dynamic_cast<DInputManager*>( Input::getManager() );
	
	if ( mgr && mgr->isEnabled() )
	{
		Con::printf( "DirectInput is enabled %s.", Input::isActive() ? "and active" : "but inactive" );
		
		Con::printf( "- Keyboard is %sabled and %sactive.",
            mgr->isKeyboardEnabled() ? "en" : "dis",
            mgr->isKeyboardActive() ? "" : "in" );
      
		Con::printf( "- Mouse is %sabled and %sactive.",
            mgr->isMouseEnabled() ? "en" : "dis",
            mgr->isMouseActive() ? "" : "in" );
      
		Con::printf( "- Joystick is %sabled and %sactive.",
            mgr->isJoystickEnabled() ? "en" : "dis",
            mgr->isJoystickActive() ? "" : "in" );

		Con::printf( "- Xinput is %sabled and %sactive.",
			mgr->isXInputEnabled() ? "en" : "dis",
			mgr->isXInputActive() ? "" : "in" );
   }
   else
    {
		Con::printf( "DirectInput is not enabled." );
    }
}

//------------------------------------------------------------------------------
void Input::setCursorPos(S32 x, S32 y)
{   
   POINT pt;   
   pt.x = x;   
   pt.y = y;   
   ClientToScreen(winState.appWindow, &pt);   
   SetCursorPos(pt.x, pt.y);
}

//------------------------------------------------------------------------------
// Set the cursor to draw (true) or not (false)
void Input::setCursorState(bool on)
{
	if (on)
	{
		while (ShowCursor(TRUE) < 0);
	}
	else
	{
		while (ShowCursor(FALSE) >= 0);
	}
}

//------------------------------------------------------------------------------
static struct { U32 id; LPTSTR resourceID; } sgCursorShapeMap[]=
{
   { CursorManager::curArrow,       IDC_ARROW },
   { CursorManager::curWait,        IDC_WAIT },
   { CursorManager::curPlus,        IDC_CROSS },
   { CursorManager::curResizeVert,  IDC_SIZEWE },
   { CursorManager::curResizeHorz,  IDC_SIZENS },
   { CursorManager::curResizeAll,   IDC_SIZEALL },
   { CursorManager::curIBeam,       IDC_IBEAM },
   { CursorManager::curResizeNESW,  IDC_SIZENESW },
   { CursorManager::curResizeNWSE,  IDC_SIZENWSE },
   { 0,                             0 },
};

void Input::setCursorShape(U32 cursorID)
{
   LPTSTR resourceID = NULL;

   for(S32 i = 0;sgCursorShapeMap[i].resourceID != NULL;++i)
   {
      if(cursorID == sgCursorShapeMap[i].id)
      {
         resourceID = sgCursorShapeMap[i].resourceID;
         break;
      }
   }

   if(resourceID == NULL)
      return;

   HCURSOR cur = LoadCursor(NULL, resourceID);
   if(cur)
      SetCursor(cur);
}

//------------------------------------------------------------------------------
// Functions to change the cursor shape using the Input class.
void Input::pushCursor(S32 cursorID)
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->pushCursor(cursorID);
}

void Input::popCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->popCursor();
}

void Input::refreshCursor()
{
   CursorManager* cm = getCursorManager();
   if(cm)
      cm->refreshCursor();
}

//------------------------------------------------------------------------------

U32 Input::getDoubleClickTime()
{
   return GetDoubleClickTime();
}

S32 Input::getDoubleClickWidth()
{
   return GetSystemMetrics(SM_CXDOUBLECLK);
}

S32 Input::getDoubleClickHeight()
{
   return GetSystemMetrics(SM_CYDOUBLECLK);
}

//------------------------------------------------------------------------------
InputManager* Input::getManager()
{
   return( smManager );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static U8 VcodeRemap[256] =
{
   0,                   // 0x00
   0,                   // 0x01  VK_LBUTTON
   0,                   // 0x02  VK_RBUTTON
   0,                   // 0x03  VK_CANCEL
   0,                   // 0x04  VK_MBUTTON
   0,                   // 0x05
   0,                   // 0x06
   0,                   // 0x07
   KEY_BACKSPACE,       // 0x08  VK_BACK
   KEY_TAB,             // 0x09  VK_TAB
   0,                   // 0x0A
   0,                   // 0x0B
   0,                   // 0x0C  VK_CLEAR
   KEY_RETURN,          // 0x0D  VK_RETURN
   0,                   // 0x0E
   0,                   // 0x0F
   KEY_SHIFT,           // 0x10  VK_SHIFT
   KEY_CONTROL,         // 0x11  VK_CONTROL
   KEY_ALT,             // 0x12  VK_MENU
   KEY_PAUSE,           // 0x13  VK_PAUSE
   KEY_CAPSLOCK,        // 0x14  VK_CAPITAL
   0,                   // 0x15  VK_KANA, VK_HANGEUL, VK_HANGUL
   0,                   // 0x16
   0,                   // 0x17  VK_JUNJA
   0,                   // 0x18  VK_FINAL
   0,                   // 0x19  VK_HANJA, VK_KANJI
   0,                   // 0x1A
   KEY_ESCAPE,          // 0x1B  VK_ESCAPE

   0,                   // 0x1C  VK_CONVERT
   0,                   // 0x1D  VK_NONCONVERT
   0,                   // 0x1E  VK_ACCEPT
   0,                   // 0x1F  VK_MODECHANGE

   KEY_SPACE,           // 0x20  VK_SPACE
   KEY_PAGE_UP,         // 0x21  VK_PRIOR
   KEY_PAGE_DOWN,       // 0x22  VK_NEXT
   KEY_END,             // 0x23  VK_END
   KEY_HOME,            // 0x24  VK_HOME
   KEY_LEFT,            // 0x25  VK_LEFT
   KEY_UP,              // 0x26  VK_UP
   KEY_RIGHT,           // 0x27  VK_RIGHT
   KEY_DOWN,            // 0x28  VK_DOWN
   0,                   // 0x29  VK_SELECT
   KEY_PRINT,           // 0x2A  VK_PRINT
   0,                   // 0x2B  VK_EXECUTE
   0,                   // 0x2C  VK_SNAPSHOT
   KEY_INSERT,          // 0x2D  VK_INSERT
   KEY_DELETE,          // 0x2E  VK_DELETE
   KEY_HELP,            // 0x2F  VK_HELP

   KEY_0,               // 0x30  VK_0   VK_0 thru VK_9 are the same as ASCII '0' thru '9' (// 0x30 - // 0x39)
   KEY_1,               // 0x31  VK_1
   KEY_2,               // 0x32  VK_2
   KEY_3,               // 0x33  VK_3
   KEY_4,               // 0x34  VK_4
   KEY_5,               // 0x35  VK_5
   KEY_6,               // 0x36  VK_6
   KEY_7,               // 0x37  VK_7
   KEY_8,               // 0x38  VK_8
   KEY_9,               // 0x39  VK_9
   0,                   // 0x3A
   0,                   // 0x3B
   0,                   // 0x3C
   0,                   // 0x3D
   0,                   // 0x3E
   0,                   // 0x3F
   0,                   // 0x40

   KEY_A,               // 0x41  VK_A      VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (// 0x41 - // 0x5A)
   KEY_B,               // 0x42  VK_B
   KEY_C,               // 0x43  VK_C
   KEY_D,               // 0x44  VK_D
   KEY_E,               // 0x45  VK_E
   KEY_F,               // 0x46  VK_F
   KEY_G,               // 0x47  VK_G
   KEY_H,               // 0x48  VK_H
   KEY_I,               // 0x49  VK_I
   KEY_J,               // 0x4A  VK_J
   KEY_K,               // 0x4B  VK_K
   KEY_L,               // 0x4C  VK_L
   KEY_M,               // 0x4D  VK_M
   KEY_N,               // 0x4E  VK_N
   KEY_O,               // 0x4F  VK_O
   KEY_P,               // 0x50  VK_P
   KEY_Q,               // 0x51  VK_Q
   KEY_R,               // 0x52  VK_R
   KEY_S,               // 0x53  VK_S
   KEY_T,               // 0x54  VK_T
   KEY_U,               // 0x55  VK_U
   KEY_V,               // 0x56  VK_V
   KEY_W,               // 0x57  VK_W
   KEY_X,               // 0x58  VK_X
   KEY_Y,               // 0x59  VK_Y
   KEY_Z,               // 0x5A  VK_Z


   KEY_WIN_LWINDOW,     // 0x5B  VK_LWIN
   KEY_WIN_RWINDOW,     // 0x5C  VK_RWIN
   KEY_WIN_APPS,        // 0x5D  VK_APPS
   0,                   // 0x5E
   0,                   // 0x5F

   KEY_NUMPAD0,         // 0x60  VK_NUMPAD0
   KEY_NUMPAD1,         // 0x61  VK_NUMPAD1
   KEY_NUMPAD2,         // 0x62  VK_NUMPAD2
   KEY_NUMPAD3,         // 0x63  VK_NUMPAD3
   KEY_NUMPAD4,         // 0x64  VK_NUMPAD4
   KEY_NUMPAD5,         // 0x65  VK_NUMPAD5
   KEY_NUMPAD6,         // 0x66  VK_NUMPAD6
   KEY_NUMPAD7,         // 0x67  VK_NUMPAD7
   KEY_NUMPAD8,         // 0x68  VK_NUMPAD8
   KEY_NUMPAD9,         // 0x69  VK_NUMPAD9
   KEY_MULTIPLY,        // 0x6A  VK_MULTIPLY
   KEY_ADD,             // 0x6B  VK_ADD
   KEY_SEPARATOR,       // 0x6C  VK_SEPARATOR
   KEY_SUBTRACT,        // 0x6D  VK_SUBTRACT
   KEY_DECIMAL,         // 0x6E  VK_DECIMAL
   KEY_DIVIDE,          // 0x6F  VK_DIVIDE
   KEY_F1,              // 0x70  VK_F1
   KEY_F2,              // 0x71  VK_F2
   KEY_F3,              // 0x72  VK_F3
   KEY_F4,              // 0x73  VK_F4
   KEY_F5,              // 0x74  VK_F5
   KEY_F6,              // 0x75  VK_F6
   KEY_F7,              // 0x76  VK_F7
   KEY_F8,              // 0x77  VK_F8
   KEY_F9,              // 0x78  VK_F9
   KEY_F10,             // 0x79  VK_F10
   KEY_F11,             // 0x7A  VK_F11
   KEY_F12,             // 0x7B  VK_F12
   KEY_F13,             // 0x7C  VK_F13
   KEY_F14,             // 0x7D  VK_F14
   KEY_F15,             // 0x7E  VK_F15
   KEY_F16,             // 0x7F  VK_F16
   KEY_F17,             // 0x80  VK_F17
   KEY_F18,             // 0x81  VK_F18
   KEY_F19,             // 0x82  VK_F19
   KEY_F20,             // 0x83  VK_F20
   KEY_F21,             // 0x84  VK_F21
   KEY_F22,             // 0x85  VK_F22
   KEY_F23,             // 0x86  VK_F23
   KEY_F24,             // 0x87  VK_F24
   0,                   // 0x88
   0,                   // 0x89
   0,                   // 0x8A
   0,                   // 0x8B
   0,                   // 0x8C
   0,                   // 0x8D
   0,                   // 0x8E
   0,                   // 0x8F

   KEY_NUMLOCK,         // 0x90  VK_NUMLOCK
   KEY_SCROLLLOCK,      // 0x91  VK_OEM_SCROLL
   0,                   // 0x92
   0,                   // 0x93
   0,                   // 0x94
   0,                   // 0x95
   0,                   // 0x96
   0,                   // 0x97
   0,                   // 0x98
   0,                   // 0x99
   0,                   // 0x9A
   0,                   // 0x9B
   0,                   // 0x9C
   0,                   // 0x9D
   0,                   // 0x9E
   0,                   // 0x9F

   KEY_LSHIFT,          // 0xA0  VK_LSHIFT
   KEY_RSHIFT,          // 0xA1  VK_RSHIFT
   KEY_LCONTROL,        // 0xA2  VK_LCONTROL
   KEY_RCONTROL,        // 0xA3  VK_RCONTROL
   KEY_LALT,            // 0xA4  VK_LMENU
   KEY_RALT,            // 0xA5  VK_RMENU
   0,                   // 0xA6
   0,                   // 0xA7
   0,                   // 0xA8
   0,                   // 0xA9
   0,                   // 0xAA
   0,                   // 0xAB
   0,                   // 0xAC
   0,                   // 0xAD
   0,                   // 0xAE
   0,                   // 0xAF
   0,                   // 0xB0
   0,                   // 0xB1
   0,                   // 0xB2
   0,                   // 0xB3
   0,                   // 0xB4
   0,                   // 0xB5
   0,                   // 0xB6
   0,                   // 0xB7
   0,                   // 0xB8
   0,                   // 0xB9
   KEY_SEMICOLON,       // 0xBA  VK_OEM_1
   KEY_EQUALS,          // 0xBB  VK_OEM_PLUS
   KEY_COMMA,           // 0xBC  VK_OEM_COMMA
   KEY_MINUS,           // 0xBD  VK_OEM_MINUS
   KEY_PERIOD,          // 0xBE  VK_OEM_PERIOD
   KEY_SLASH,           // 0xBF  VK_OEM_2
   KEY_TILDE,           // 0xC0  VK_OEM_3
   0,                   // 0xC1
   0,                   // 0xC2
   0,                   // 0xC3
   0,                   // 0xC4
   0,                   // 0xC5
   0,                   // 0xC6
   0,                   // 0xC7
   0,                   // 0xC8
   0,                   // 0xC9
   0,                   // 0xCA
   0,                   // 0xCB
   0,                   // 0xCC
   0,                   // 0xCD
   0,                   // 0xCE
   0,                   // 0xCF
   0,                   // 0xD0
   0,                   // 0xD1
   0,                   // 0xD2
   0,                   // 0xD3
   0,                   // 0xD4
   0,                   // 0xD5
   0,                   // 0xD6
   0,                   // 0xD7
   0,                   // 0xD8
   0,                   // 0xD9
   0,                   // 0xDA
   KEY_LBRACKET,        // 0xDB  VK_OEM_4
   KEY_BACKSLASH,       // 0xDC  VK_OEM_5
   KEY_RBRACKET,        // 0xDD  VK_OEM_6
   KEY_APOSTROPHE,      // 0xDE  VK_OEM_7
   0,                   // 0xDF  VK_OEM_8
   0,                   // 0xE0
   0,                   // 0xE1  VK_OEM_AX  AX key on Japanese AX keyboard
   KEY_OEM_102,         // 0xE2  VK_OEM_102
   0,                   // 0xE3
   0,                   // 0xE4

   0,                   // 0xE5  VK_PROCESSKEY

   0,                   // 0xE6
   0,                   // 0xE7
   0,                   // 0xE8
   0,                   // 0xE9
   0,                   // 0xEA
   0,                   // 0xEB
   0,                   // 0xEC
   0,                   // 0xED
   0,                   // 0xEE
   0,                   // 0xEF

   0,                   // 0xF0
   0,                   // 0xF1
   0,                   // 0xF2
   0,                   // 0xF3
   0,                   // 0xF4
   0,                   // 0xF5

   0,                   // 0xF6  VK_ATTN
   0,                   // 0xF7  VK_CRSEL
   0,                   // 0xF8  VK_EXSEL
   0,                   // 0xF9  VK_EREOF
   0,                   // 0xFA  VK_PLAY
   0,                   // 0xFB  VK_ZOOM
   0,                   // 0xFC  VK_NONAME
   0,                   // 0xFD  VK_PA1
   0,                   // 0xFE  VK_OEM_CLEAR
   0                    // 0xFF
};


//------------------------------------------------------------------------------
//
// This function translates a virtual key code to our corresponding internal
// key code using the preceding table.
//
//------------------------------------------------------------------------------
U8 TranslateOSKeyCode(U8 vcode)
{
   return VcodeRemap[vcode];
}

//-----------------------------------------------------------------------------
// Clipboard functions
const char* Platform::getClipboard()
{
   HGLOBAL hGlobal;
   LPVOID  pGlobal;

   //make sure we can access the clipboard
   if (!IsClipboardFormatAvailable(CF_TEXT))
      return "";
   if (!OpenClipboard(NULL))
      return "";

   hGlobal = GetClipboardData(CF_TEXT);
   pGlobal = GlobalLock(hGlobal);
   S32 cbLength = strlen((char *)pGlobal);
   char  *returnBuf = Con::getReturnBuffer(cbLength + 1);
   strcpy(returnBuf, (char *)pGlobal);
   returnBuf[cbLength] = '\0';
   GlobalUnlock(hGlobal);
   CloseClipboard();

   //note - this function never returns NULL
   return returnBuf;
}

//-----------------------------------------------------------------------------
bool Platform::setClipboard(const char *text)
{
   if (!text)
      return false;

   //make sure we can access the clipboard
   if (!OpenClipboard(NULL))
      return false;

   S32 cbLength = strlen(text);

   HGLOBAL hGlobal;
   LPVOID  pGlobal;

   hGlobal = GlobalAlloc(GHND, cbLength + 1);
   pGlobal = GlobalLock (hGlobal);

   strcpy((char *)pGlobal, text);

   GlobalUnlock(hGlobal);

   EmptyClipboard();
   SetClipboardData(CF_TEXT, hGlobal);
   CloseClipboard();

   return true;
}

