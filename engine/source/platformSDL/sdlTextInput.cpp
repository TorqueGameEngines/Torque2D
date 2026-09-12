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

#include "platform/platform.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "input/actionMap.h"
#include "string/unicode.h"
#include "platformSDL/sdlTextInput.h"

#include <SDL.h>

//------------------------------------------------------------------------------
// Turning text input on and off
//
// Torque3D's rule (windowManager/sdl/sdlWindowMgr.h there): a change asked for
// while the frame's input is being handled waits until that is done, so that
// the key which opens the console does not also type into it. Here the input a
// pump delivers is queued and handled after it, so "done" is the start of the
// next pump. Waiting also means a frame in which the keyboard passes straight
// from one text field to another asks for off, then on, and gets neither --
// turning text input off throws away any text SDL is still holding.

static SDLTextInput::KeyboardInputState sInputState = SDLTextInput::NONE;

void SDLTextInput::updateSDLTextInputState( KeyboardInputState state )
{
   sInputState = state;
}

void SDLTextInput::processTextInputState()
{
   if ( sInputState == NONE )
      return;

   const bool wantText = ( sInputState == TEXT_INPUT );
   sInputState = NONE;

   if ( wantText == ( SDL_IsTextInputActive() == SDL_TRUE ) )
      return;

   if ( wantText )
      SDL_StartTextInput();
   else
      SDL_StopTextInput();
}

//------------------------------------------------------------------------------
// A key the GlobalActionMap is bound to types nothing
//
// Torque3D's rule (windowManager/sdl/sdlWindow.cpp there): a text field that
// has the keyboard leaves a globally bound key to the map, and the key's text
// is thrown away. By the time the key is read here SDL has queued its text
// behind it; SDL_StopTextInput discards that (it flushes the queued
// SDL_TEXTINPUT events). Text input is asked back for the next pump, and if
// the binding took the keyboard away meanwhile -- a key that closes the
// field's dialog -- the field's own request to turn it off comes later and
// wins. GuiTextEditCtrl::onKeyDown leaves the same keys to the map, bar the
// field's own editing keys, which type nothing anyway.
void SDLTextInput::withholdGlobalKeyText( const InputEvent& event )
{
   if ( event.action != SI_MAKE && event.action != SI_REPEAT )
      return;

   if ( SDL_IsTextInputActive() != SDL_TRUE )
      return;

   ActionMap* globalMap = ActionMap::getGlobalMap();
   if ( globalMap == NULL || !globalMap->isAction( event.deviceType, event.deviceInst, event.modifier, event.objInst ) )
      return;

   SDL_StopTextInput();
   updateSDLTextInputState( TEXT_INPUT );
}

//------------------------------------------------------------------------------
void Platform::enableKeyboardTranslation(void)
{
   SDLTextInput::updateSDLTextInputState( SDLTextInput::TEXT_INPUT );
}

//------------------------------------------------------------------------------
void Platform::disableKeyboardTranslation(void)
{
   SDLTextInput::updateSDLTextInputState( SDLTextInput::RAW_INPUT );
}

//------------------------------------------------------------------------------
// Decoding
//
// oneUTF8toUTF32 does the walking, so a malformed byte is replaced here exactly
// as the engine replaces it everywhere else. It reads a character beyond the
// BMP correctly -- four bytes -- but gives U+FFFD for it, since the engine's
// strings stop at the BMP; those four bytes are decoded again below and sent on
// as the surrogate pair a UTF-16 text field stores.
//
// Torque3D copies SDL_TEXTINPUT's bytes straight into 16-bit slots instead,
// one byte a slot, so anything outside ASCII arrives as a string of Latin-1
// junk.

static const UTF32 ReplacementChar = 0xFFFD;

// The code point a four-byte sequence names, or ReplacementChar if it names
// one that four bytes may not: below U+10000 (an overlong form) or above
// U+10FFFF. oneUTF8toUTF32 has already checked that the three bytes after the
// first are continuation bytes.
static UTF32 decodeFourBytes( const U8* bytes )
{
   const UTF32 codepoint = ( ( bytes[0] & 0x07 ) << 18 ) |
                           ( ( bytes[1] & 0x3F ) << 12 ) |
                           ( ( bytes[2] & 0x3F ) << 6 ) |
                             ( bytes[3] & 0x3F );

   if ( codepoint < 0x10000 || codepoint > 0x10FFFF )
      return ReplacementChar;

   return codepoint;
}

U32 SDLTextInput::decodeText( const char* utf8, UTF16* units, U32 maxUnits )
{
   U32 count = 0;
   if ( utf8 == NULL || units == NULL )
      return 0;

   const UTF8* walk = reinterpret_cast<const UTF8*>( utf8 );
   while ( *walk != 0 )
   {
      U32 walked = 1;
      UTF32 codepoint = oneUTF8toUTF32( walk, &walked );
      if ( codepoint == ReplacementChar && walked == 4 )
         codepoint = decodeFourBytes( reinterpret_cast<const U8*>( walk ) );
      walk += walked;

      // Half a surrogate pair, encoded on its own, is not a character.
      // oneUTF8toUTF32 means to replace one, but its range test leaves out
      // both ends, so U+D800 and U+DFFF get through it; they are replaced here,
      // since a text field handed one would store half a pair.
      if ( codepoint >= 0xD800 && codepoint <= 0xDFFF )
         codepoint = ReplacementChar;

      // SDL does not send control characters as text, but a text field must
      // never be handed one as typing, so none gets through here either --
      // an encoded NUL included, which oneUTF8toUTF32 reads as 0.
      if ( codepoint < 0x20 || codepoint == 0x7F )
         continue;

      if ( codepoint < 0x10000 )
      {
         if ( count + 1 > maxUnits )
            break;
         units[count++] = (UTF16)codepoint;
      }
      else
      {
         if ( count + 2 > maxUnits )
            break;
         codepoint -= 0x10000;
         units[count++] = (UTF16)( 0xD800 + ( codepoint >> 10 ) );
         units[count++] = (UTF16)( 0xDC00 + ( codepoint & 0x3FF ) );
      }
   }

   return count;
}

//------------------------------------------------------------------------------
void SDLTextInput::fillCharEvent( InputEvent& event, UTF16 unit, U8 action )
{
   event.deviceInst = 0;
   event.deviceType = KeyboardDeviceType;
   event.objType    = SI_KEY;
   event.objInst    = KEY_NULL;
   event.action     = action;
   event.modifier   = 0;
   event.ascii      = unit;
   event.fValues[0] = ( action == SI_MAKE ) ? 1.0f : 0.0f;
}

//------------------------------------------------------------------------------
void SDLTextInput::postText( const char* utf8 )
{
   // SDL_TEXTINPUT carries less than SDL_TEXTINPUTEVENT_TEXT_SIZE bytes, and
   // no byte decodes to more than one code unit.
   UTF16 units[SDL_TEXTINPUTEVENT_TEXT_SIZE];
   const U32 count = decodeText( utf8, units, SDL_TEXTINPUTEVENT_TEXT_SIZE );

   for ( U32 i = 0; i < count; ++i )
   {
      InputEvent event;

      fillCharEvent( event, units[i], SI_MAKE );
      Game->postEvent( event );

      fillCharEvent( event, units[i], SI_BREAK );
      Game->postEvent( event );
   }
}
