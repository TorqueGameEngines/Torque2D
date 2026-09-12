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

// The keyboard half of Torque3D's platformSDL/sdlInput.cpp: the scancode table,
// the layout-aware Input::getAscii and Input::getKeyCode, and the clipboard. The
// rest of that file -- Input::init and friends -- stays with each back-end here,
// because each has its own input manager.

#include "platform/platform.h"
#include "platform/platformInput.h"
#include "platform/event.h"
#include "console/console.h"
#include "platformSDL/sdlInput.h"

#include <SDL.h>

#define NUM_KEYS ( KEY_OEM_102 + 1 )

//------------------------------------------------------------------------------
// The character a key types unshifted on the current layout. SDL names every
// such key by that character -- its SDL_Keycode is the character's code point,
// lower case for a letter -- and every other key (the arrows, F1, the numpad) by
// a code with SDLK_SCANCODE_MASK set. Control characters are left out: the
// Return and Tab keys have keycodes '\r' and '\t', but no one asks for them by
// character, and the key names in actionMap.cc cover them.
//
// Torque3D asks SDL_GetKeyName instead and keeps only one-byte names, which
// drops every key that types something outside ASCII -- the o-umlaut key on a
// German keyboard, say.
static U16 getUnshiftedCharacter( U32 torqueKey )
{
   const SDL_Keycode key = SDL_GetKeyFromScancode(
      (SDL_Scancode)KeyMapSDL::getSDLScanCodeFromTorque( torqueKey ) );

   if ( key < 0x20 || key == 0x7F || key > 0xFFFF )
      return 0;

   return (U16)key;
}

//------------------------------------------------------------------------------
U16 Input::getKeyCode( U16 asciiCode )
{
   if ( asciiCode == 0 )
      return 0;

   // The key that types this character on the current layout, so that a
   // script's bind(keyboard, "z", ...) lands on the key marked z, wherever the
   // layout puts it (Torque3D a78235dd). A character is its own keycode (see
   // above), so it goes to SDL_GetScancodeFromKey as it is; Torque3D goes
   // through SDL_GetKeyFromName, which only takes UTF-8 and so turns away every
   // character above 127. Only unshifted characters are found: SDL's keymap
   // does not say what a key types with Shift held.
   SDL_Keycode key = asciiCode;
   if ( key >= 'A' && key <= 'Z' )
      key += 'a' - 'A';

   return (U16)KeyMapSDL::getTorqueScanCodeFromSDL( SDL_GetScancodeFromKey( key ) );
}

//------------------------------------------------------------------------------
U16 Input::getAscii( U16 keyCode, KEY_STATE keyState )
{
   if ( keyCode >= NUM_KEYS )
      return 0;

   const U16 ret = getUnshiftedCharacter( keyCode );

   // As in Torque3D, only a letter has a case; anything else types the same
   // character whatever the state.
   if ( ret < 'a' || ret > 'z' )
      return ret;

   switch ( keyState )
   {
      case STATE_LOWER:
         return ret;
      case STATE_UPPER:
         return ret - ( 'a' - 'A' );
      case STATE_GOOFY:
         return 0; // SDL's keymap says nothing about AltGr
      default:
         return 0;
   }
}

//-----------------------------------------------------------------------------
// Clipboard functions
//
// SDL owns the selection on the engine's behalf and answers other programs'
// requests for it from its own event loop, in UTF-8.
const char* Platform::getClipboard()
{
   // SDL_GetClipboardText hands back a copy for the caller to free, which
   // Torque3D never does. This copies it into the console's return buffer, as
   // the Windows back-end does, and frees SDL's. It never returns NULL.
   if ( !SDL_HasClipboardText() )
      return "";

   char* text = SDL_GetClipboardText();
   if ( text == NULL )
      return "";

   char* returnBuffer = Con::getReturnBuffer( dStrlen( text ) + 1 );
   dStrcpy( returnBuffer, text );
   SDL_free( text );

   return returnBuffer;
}

//-----------------------------------------------------------------------------
bool Platform::setClipboard(const char *text)
{
   if (!text)
      return false;

   return SDL_SetClipboardText(text) == 0;
}

//------------------------------------------------------------------------------
// The scancode table. SDL numbers its scancodes up to SDL_NUM_SCANCODES (512),
// past the 256 entries Torque3D gives its table -- a media key's scancode read
// off the end of it -- so the SDL side is sized to SDL's range here, and both
// lookups check theirs.
namespace
{
   const U32 TorqueTableSize = 256;
   U32 SDL_T2D[SDL_NUM_SCANCODES];
   U32 T2D_SDL[TorqueTableSize];
   static bool _buildScanCode = true;
}

static void mapScanCode(U32 sdl, U32 torque)
{
   SDL_T2D[sdl] = torque;
   T2D_SDL[torque] = sdl;
}

static void buildScanCodeArray()
{
   _buildScanCode = false;

   for(U32 i = 0; i < SDL_NUM_SCANCODES; ++i)
      SDL_T2D[i] = KEY_NULL;
   for(U32 i = 0; i < TorqueTableSize; ++i)
      T2D_SDL[i] = SDL_SCANCODE_UNKNOWN;

   // SDL, Torque
   mapScanCode(SDL_SCANCODE_A, KEY_A);
   mapScanCode(SDL_SCANCODE_B, KEY_B);
   mapScanCode(SDL_SCANCODE_C, KEY_C);
   mapScanCode(SDL_SCANCODE_D, KEY_D);
   mapScanCode(SDL_SCANCODE_E, KEY_E);
   mapScanCode(SDL_SCANCODE_F, KEY_F);
   mapScanCode(SDL_SCANCODE_G, KEY_G);
   mapScanCode(SDL_SCANCODE_H, KEY_H);
   mapScanCode(SDL_SCANCODE_I, KEY_I);
   mapScanCode(SDL_SCANCODE_J, KEY_J);
   mapScanCode(SDL_SCANCODE_K, KEY_K);
   mapScanCode(SDL_SCANCODE_L, KEY_L);
   mapScanCode(SDL_SCANCODE_M, KEY_M);
   mapScanCode(SDL_SCANCODE_N, KEY_N);
   mapScanCode(SDL_SCANCODE_O, KEY_O);
   mapScanCode(SDL_SCANCODE_P, KEY_P);
   mapScanCode(SDL_SCANCODE_Q, KEY_Q);
   mapScanCode(SDL_SCANCODE_R, KEY_R);
   mapScanCode(SDL_SCANCODE_S, KEY_S);
   mapScanCode(SDL_SCANCODE_T, KEY_T);
   mapScanCode(SDL_SCANCODE_U, KEY_U);
   mapScanCode(SDL_SCANCODE_V, KEY_V);
   mapScanCode(SDL_SCANCODE_W, KEY_W);
   mapScanCode(SDL_SCANCODE_X, KEY_X);
   mapScanCode(SDL_SCANCODE_Y, KEY_Y);
   mapScanCode(SDL_SCANCODE_Z, KEY_Z);

   mapScanCode(SDL_SCANCODE_1, KEY_1);
   mapScanCode(SDL_SCANCODE_2, KEY_2);
   mapScanCode(SDL_SCANCODE_3, KEY_3);
   mapScanCode(SDL_SCANCODE_4, KEY_4);
   mapScanCode(SDL_SCANCODE_5, KEY_5);
   mapScanCode(SDL_SCANCODE_6, KEY_6);
   mapScanCode(SDL_SCANCODE_7, KEY_7);
   mapScanCode(SDL_SCANCODE_8, KEY_8);
   mapScanCode(SDL_SCANCODE_9, KEY_9);
   mapScanCode(SDL_SCANCODE_0, KEY_0);

   // The generic KEY_CONTROL, KEY_ALT and KEY_SHIFT only matter going from
   // Torque to SDL; the sided codes further down win going the other way.
   mapScanCode(SDL_SCANCODE_BACKSPACE, KEY_BACKSPACE);
   mapScanCode(SDL_SCANCODE_TAB, KEY_TAB);
   mapScanCode(SDL_SCANCODE_RETURN, KEY_RETURN);
   mapScanCode(SDL_SCANCODE_LCTRL, KEY_CONTROL);
   mapScanCode(SDL_SCANCODE_RCTRL, KEY_CONTROL);
   mapScanCode(SDL_SCANCODE_LALT, KEY_ALT);
   mapScanCode(SDL_SCANCODE_RALT, KEY_ALT);
   mapScanCode(SDL_SCANCODE_LSHIFT, KEY_SHIFT);
   mapScanCode(SDL_SCANCODE_RSHIFT, KEY_SHIFT);
   mapScanCode(SDL_SCANCODE_PAUSE, KEY_PAUSE);
   mapScanCode(SDL_SCANCODE_CAPSLOCK, KEY_CAPSLOCK);
   mapScanCode(SDL_SCANCODE_ESCAPE, KEY_ESCAPE);
   mapScanCode(SDL_SCANCODE_SPACE, KEY_SPACE);
   mapScanCode(SDL_SCANCODE_PAGEDOWN, KEY_PAGE_DOWN);
   mapScanCode(SDL_SCANCODE_PAGEUP, KEY_PAGE_UP);
   mapScanCode(SDL_SCANCODE_END, KEY_END);
   mapScanCode(SDL_SCANCODE_HOME, KEY_HOME);
   mapScanCode(SDL_SCANCODE_LEFT, KEY_LEFT);
   mapScanCode(SDL_SCANCODE_UP, KEY_UP);
   mapScanCode(SDL_SCANCODE_RIGHT, KEY_RIGHT);
   mapScanCode(SDL_SCANCODE_DOWN, KEY_DOWN);
   mapScanCode(SDL_SCANCODE_PRINTSCREEN, KEY_PRINT);
   mapScanCode(SDL_SCANCODE_INSERT, KEY_INSERT);
   mapScanCode(SDL_SCANCODE_DELETE, KEY_DELETE);
   mapScanCode(SDL_SCANCODE_HELP, KEY_HELP);

   mapScanCode(SDL_SCANCODE_GRAVE, KEY_TILDE);
   mapScanCode(SDL_SCANCODE_MINUS, KEY_MINUS);
   mapScanCode(SDL_SCANCODE_EQUALS, KEY_EQUALS);
   mapScanCode(SDL_SCANCODE_LEFTBRACKET, KEY_LBRACKET);
   mapScanCode(SDL_SCANCODE_RIGHTBRACKET, KEY_RBRACKET);
   mapScanCode(SDL_SCANCODE_BACKSLASH, KEY_BACKSLASH);
   mapScanCode(SDL_SCANCODE_SEMICOLON, KEY_SEMICOLON);
   mapScanCode(SDL_SCANCODE_APOSTROPHE, KEY_APOSTROPHE);
   mapScanCode(SDL_SCANCODE_COMMA, KEY_COMMA);
   mapScanCode(SDL_SCANCODE_PERIOD, KEY_PERIOD);
   mapScanCode(SDL_SCANCODE_SLASH, KEY_SLASH);
   mapScanCode(SDL_SCANCODE_KP_0, KEY_NUMPAD0);
   mapScanCode(SDL_SCANCODE_KP_1, KEY_NUMPAD1);
   mapScanCode(SDL_SCANCODE_KP_2, KEY_NUMPAD2);
   mapScanCode(SDL_SCANCODE_KP_3, KEY_NUMPAD3);
   mapScanCode(SDL_SCANCODE_KP_4, KEY_NUMPAD4);
   mapScanCode(SDL_SCANCODE_KP_5, KEY_NUMPAD5);
   mapScanCode(SDL_SCANCODE_KP_6, KEY_NUMPAD6);
   mapScanCode(SDL_SCANCODE_KP_7, KEY_NUMPAD7);
   mapScanCode(SDL_SCANCODE_KP_8, KEY_NUMPAD8);
   mapScanCode(SDL_SCANCODE_KP_9, KEY_NUMPAD9);
   mapScanCode(SDL_SCANCODE_KP_MULTIPLY, KEY_MULTIPLY);
   mapScanCode(SDL_SCANCODE_KP_PLUS, KEY_ADD);
   mapScanCode(SDL_SCANCODE_KP_EQUALS, KEY_SEPARATOR);
   mapScanCode(SDL_SCANCODE_KP_MINUS, KEY_SUBTRACT);
   mapScanCode(SDL_SCANCODE_KP_PERIOD, KEY_DECIMAL);
   mapScanCode(SDL_SCANCODE_KP_DIVIDE, KEY_DIVIDE);
   mapScanCode(SDL_SCANCODE_KP_ENTER, KEY_NUMPADENTER);

   mapScanCode(SDL_SCANCODE_F1, KEY_F1);
   mapScanCode(SDL_SCANCODE_F2, KEY_F2);
   mapScanCode(SDL_SCANCODE_F3, KEY_F3);
   mapScanCode(SDL_SCANCODE_F4, KEY_F4);
   mapScanCode(SDL_SCANCODE_F5, KEY_F5);
   mapScanCode(SDL_SCANCODE_F6, KEY_F6);
   mapScanCode(SDL_SCANCODE_F7, KEY_F7);
   mapScanCode(SDL_SCANCODE_F8, KEY_F8);
   mapScanCode(SDL_SCANCODE_F9, KEY_F9);
   mapScanCode(SDL_SCANCODE_F10, KEY_F10);
   mapScanCode(SDL_SCANCODE_F11, KEY_F11);
   mapScanCode(SDL_SCANCODE_F12, KEY_F12);
   mapScanCode(SDL_SCANCODE_F13, KEY_F13);
   mapScanCode(SDL_SCANCODE_F14, KEY_F14);
   mapScanCode(SDL_SCANCODE_F15, KEY_F15);
   mapScanCode(SDL_SCANCODE_F16, KEY_F16);
   mapScanCode(SDL_SCANCODE_F17, KEY_F17);
   mapScanCode(SDL_SCANCODE_F18, KEY_F18);
   mapScanCode(SDL_SCANCODE_F19, KEY_F19);
   mapScanCode(SDL_SCANCODE_F20, KEY_F20);
   mapScanCode(SDL_SCANCODE_F21, KEY_F21);
   mapScanCode(SDL_SCANCODE_F22, KEY_F22);
   mapScanCode(SDL_SCANCODE_F23, KEY_F23);
   mapScanCode(SDL_SCANCODE_F24, KEY_F24);

   // Torque3D leaves the rest of this block commented out -- the lock keys
   // under SDL's LOCKING* scancodes, which no ordinary keyboard sends. These
   // are the scancodes the keys do send, and the ones the SDL 1.2 back-end
   // mapped by keysym: Num Lock, Scroll Lock, the two Windows keys, the menu
   // key, and the extra key an ISO keyboard has beside the left Shift.
   mapScanCode(SDL_SCANCODE_NUMLOCKCLEAR, KEY_NUMLOCK);
   mapScanCode(SDL_SCANCODE_SCROLLLOCK, KEY_SCROLLLOCK);
   mapScanCode(SDL_SCANCODE_LCTRL, KEY_LCONTROL);
   mapScanCode(SDL_SCANCODE_RCTRL, KEY_RCONTROL);
   mapScanCode(SDL_SCANCODE_LALT, KEY_LALT);
   mapScanCode(SDL_SCANCODE_RALT, KEY_RALT);
   mapScanCode(SDL_SCANCODE_LSHIFT, KEY_LSHIFT);
   mapScanCode(SDL_SCANCODE_RSHIFT, KEY_RSHIFT);
   mapScanCode(SDL_SCANCODE_LGUI, KEY_WIN_LWINDOW);
   mapScanCode(SDL_SCANCODE_RGUI, KEY_WIN_RWINDOW);
   mapScanCode(SDL_SCANCODE_APPLICATION, KEY_WIN_APPS);
   mapScanCode(SDL_SCANCODE_NONUSBACKSLASH, KEY_OEM_102);
}

U32 KeyMapSDL::getTorqueScanCodeFromSDL(U32 sdl)
{
   if(_buildScanCode)
      buildScanCodeArray();

   if(sdl >= SDL_NUM_SCANCODES)
      return KEY_NULL;

   return SDL_T2D[sdl];
}

U32 KeyMapSDL::getSDLScanCodeFromTorque(U32 torque)
{
   if(_buildScanCode)
      buildScanCodeArray();

   if(torque >= TorqueTableSize)
      return SDL_SCANCODE_UNKNOWN;

   return T2D_SDL[torque];
}

//------------------------------------------------------------------------------
// Torque3D's getTorqueModFromSDL (windowManager/sdl/sdlWindow.cpp there), with
// one difference: Torque3D adds SI_SHIFT, SI_CTRL or SI_ALT beside the sided
// bit, but each of those is both sides' bits together, so a left Shift came
// out as both Shifts. Here each side reports only itself, as the SDL 1.2
// back-end did. A binding on "shift" matches either all the same: ActionMap
// widens a sided bit to both sides before it compares.
U32 KeyMapSDL::getTorqueModFromSDL(U16 mod)
{
   U32 ret = 0;

   if (mod & KMOD_LSHIFT)
      ret |= SI_LSHIFT;

   if (mod & KMOD_RSHIFT)
      ret |= SI_RSHIFT;

   if (mod & KMOD_LCTRL)
      ret |= SI_LCTRL;

   if (mod & KMOD_RCTRL)
      ret |= SI_RCTRL;

   if (mod & KMOD_LALT)
      ret |= SI_LALT;

   if (mod & KMOD_RALT)
      ret |= SI_RALT;

   return ret;
}
