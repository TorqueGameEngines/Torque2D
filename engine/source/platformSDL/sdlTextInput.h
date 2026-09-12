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

#ifndef _SDLTEXTINPUT_H_
#define _SDLTEXTINPUT_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

struct InputEvent;

/// Typed text, as SDL 2 reports it, turned into Torque input events. Shared by
/// every back-end built on SDL 2, so free of anything X11.
///
/// A key event says which key moved, not what it typed: that depends on the
/// layout, on dead keys and on any input method in between, and SDL works it
/// out and reports the result separately, as an SDL_TEXTINPUT event carrying
/// UTF-8. So a key event goes to the game with ascii 0, and each UTF-16 code
/// unit of the text goes as a character event of its own:
///
///   deviceType KeyboardDeviceType, objType SI_KEY, objInst KEY_NULL,
///   modifier 0, ascii the code unit -- an SI_MAKE, then a matching SI_BREAK.
///
/// A character beyond the Basic Multilingual Plane goes as two, its surrogate
/// pair, high half first.
///
/// SDL reports text only while text input is on. The back-end turns it off
/// straight after SDL_Init, and the GUI turns it on and off again through
/// Platform::enableKeyboardTranslation and disableKeyboardTranslation as a text
/// field gains and loses the keyboard.
namespace SDLTextInput
{
   /// A change to SDL's text input, waiting for the end of the frame. This is
   /// Torque3D's PlatformWindowManagerSDL::KeyboardInputState.
   enum KeyboardInputState
   {
      NONE = 0,       ///< No change asked for this frame.
      TEXT_INPUT = 1, ///< Text events as well as key events.
      RAW_INPUT = 2   ///< Key events only.
   };

   /// Ask for text input to be turned on or off at the end of the frame
   /// (Torque3D's updateSDLTextInputState). The last request in a frame wins.
   void updateSDLTextInputState( KeyboardInputState state );

   /// Make the change asked for since the last call, if any. A back-end calls
   /// this once a frame, before it pumps SDL's events.
   void processTextInputState();

   /// Decode UTF-8 from SDL_TEXTINPUT into UTF-16 code units, at most maxUnits
   /// of them; returns how many. Bytes that are not UTF-8 each become U+FFFD,
   /// the replacement character, as they do in the engine's own converters.
   /// Control characters -- below U+0020, and U+007F -- are dropped: they are
   /// never text. A pair that would not fit is left out whole.
   U32 decodeText( const char* utf8, UTF16* units, U32 maxUnits );

   /// Fill in the character event for one code unit (see above); action is
   /// SI_MAKE or SI_BREAK.
   void fillCharEvent( InputEvent& event, UTF16 unit, U8 action );

   /// Post SDL_TEXTINPUT's text to the game, a make and a break per code unit.
   void postText( const char* utf8 );

   /// Called with each key read from SDL, after it is posted. A key the
   /// GlobalActionMap is bound to types nothing, even with a text field
   /// listening -- Torque3D's rule -- so if text input is on, it is turned off
   /// on the spot, which throws away the text SDL has already queued behind
   /// the key, and asked back for the next pump.
   void withholdGlobalKeyText( const InputEvent& event );
}

#endif // _SDLTEXTINPUT_H_
