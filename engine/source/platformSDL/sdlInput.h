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

#ifndef _SDLINPUT_H_
#define _SDLINPUT_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

/// The keyboard as SDL 2 reports it, in Torque's terms. This is Torque3D's
/// platformSDL/sdlInput.h, shared by every back-end built on SDL 2 -- Linux now,
/// the web build next -- so it must stay free of anything X11.
///
/// A key is identified by its SDL_Scancode: the physical key, wherever the
/// layout puts its letters. So KEY_Z is the key at the bottom left on every
/// keyboard, and a game binding movement to WASD gets the same four keys on an
/// AZERTY one. What a key *types* is the layout's business, and is answered
/// through it: Input::getAscii and Input::getKeyCode ask SDL which character the
/// key produces now, so that bind(keyboard, "z", ...) in a script still lands on
/// whichever key types a z. Text itself comes from SDL_TEXTINPUT (sdlTextInput.h).
namespace KeyMapSDL
{
   /// The KEY_* code for an SDL_Scancode, or KEY_NULL for a key Torque has no
   /// code for.
   U32 getTorqueScanCodeFromSDL(U32 sdl);

   /// The SDL_Scancode for a KEY_* code, or SDL_SCANCODE_UNKNOWN.
   U32 getSDLScanCodeFromTorque(U32 torque);

   /// The SI_* modifier bits for an SDL_Keymod -- a keysym's mod, or
   /// SDL_GetModState() for an event that has none of its own.
   U32 getTorqueModFromSDL(U16 mod);
}

#endif // _SDLINPUT_H_
