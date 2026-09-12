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

// We don't want tests in a shipping version.
#ifndef TORQUE_SHIPPING

// platformSDL is compiled only into the back-ends built on SDL 2.
#ifdef TORQUE_SDL

#ifndef _UNIT_TESTING_H_
#include "testing/unitTesting.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

#ifndef _SDLTEXTINPUT_H_
#include "platformSDL/sdlTextInput.h"
#endif

// platformSDL/sdlTextInput.cpp: SDL_TEXTINPUT's UTF-8 decoded into the UTF-16
// code units the character events carry, and the character events themselves
// -- the contract between the SDL back-ends and the GUI's text fields.
//
// The byte strings are written out in hex, and split wherever a letter follows
// one: "\xE2\x82" "a", since "\xE2\x82a" would be read as the escape \x82a.

namespace
{
    U32 decode( const char* utf8, UTF16* units, U32 maxUnits = 16 )
    {
        return SDLTextInput::decodeText( utf8, units, maxUnits );
    }
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, AsciiIsOneUnitPerCharacter )
{
    UTF16 units[16];

    ASSERT_EQ( 3u, decode( "abc", units ) );
    ASSERT_EQ( (UTF16)'a', units[0] );
    ASSERT_EQ( (UTF16)'b', units[1] );
    ASSERT_EQ( (UTF16)'c', units[2] );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, TwoAndThreeByteCharactersAreOneUnit )
{
    UTF16 units[16];

    // e acute
    ASSERT_EQ( 1u, decode( "\xC3\xA9", units ) );
    ASSERT_EQ( (UTF16)0x00E9, units[0] );

    // the euro sign
    ASSERT_EQ( 1u, decode( "\xE2\x82\xAC", units ) );
    ASSERT_EQ( (UTF16)0x20AC, units[0] );

    // two CJK ideographs, as an input method hands them over: together
    ASSERT_EQ( 2u, decode( "\xE6\x97\xA5\xE6\x9C\xAC", units ) );
    ASSERT_EQ( (UTF16)0x65E5, units[0] );
    ASSERT_EQ( (UTF16)0x672C, units[1] );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, CharactersBeyondTheBMPAreSurrogatePairs )
{
    UTF16 units[16];

    // U+1F600, a grinning face: high half first.
    ASSERT_EQ( 2u, decode( "\xF0\x9F\x98\x80", units ) );
    ASSERT_EQ( (UTF16)0xD83D, units[0] );
    ASSERT_EQ( (UTF16)0xDE00, units[1] );

    // The first and last characters beyond the BMP.
    ASSERT_EQ( 2u, decode( "\xF0\x90\x80\x80", units ) );
    ASSERT_EQ( (UTF16)0xD800, units[0] );
    ASSERT_EQ( (UTF16)0xDC00, units[1] );

    ASSERT_EQ( 2u, decode( "\xF4\x8F\xBF\xBF", units ) );
    ASSERT_EQ( (UTF16)0xDBFF, units[0] );
    ASSERT_EQ( (UTF16)0xDFFF, units[1] );

    // In among other text.
    ASSERT_EQ( 4u, decode( "a" "\xF0\x9F\x98\x80" "b", units ) );
    ASSERT_EQ( (UTF16)'a', units[0] );
    ASSERT_EQ( (UTF16)0xD83D, units[1] );
    ASSERT_EQ( (UTF16)0xDE00, units[2] );
    ASSERT_EQ( (UTF16)'b', units[3] );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, MalformedBytesAreEachReplaced )
{
    UTF16 units[16];

    // A byte that can start nothing, and one that can only continue.
    ASSERT_EQ( 1u, decode( "\xFF", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
    ASSERT_EQ( 1u, decode( "\x80", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );

    // A character cut short: each of its bytes is replaced, and what follows
    // still comes through.
    ASSERT_EQ( 3u, decode( "\xE2\x82" "a", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
    ASSERT_EQ( (UTF16)0xFFFD, units[1] );
    ASSERT_EQ( (UTF16)'a', units[2] );

    ASSERT_EQ( 3u, decode( "\xF0\x9F\x98", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
    ASSERT_EQ( (UTF16)0xFFFD, units[1] );
    ASSERT_EQ( (UTF16)0xFFFD, units[2] );

    // Half a surrogate pair, encoded on its own, is not a character -- the two
    // ends of the range included, which the engine's own decoder lets through.
    ASSERT_EQ( 1u, decode( "\xED\xA0\x80", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
    ASSERT_EQ( 1u, decode( "\xED\xBF\xBF", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
    ASSERT_EQ( 1u, decode( "\xED\xB0\x80", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );

    // Nor is anything past U+10FFFF.
    ASSERT_EQ( 1u, decode( "\xF4\x90\x80\x80", units ) );
    ASSERT_EQ( (UTF16)0xFFFD, units[0] );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, ControlCharactersAreNeverText )
{
    UTF16 units[16];

    ASSERT_EQ( 0u, decode( "\t\r\n\x1B\x7F", units ) );

    // An encoded NUL, which the engine's decoder reads as a 0.
    ASSERT_EQ( 0u, decode( "\xC0\x80", units ) );

    ASSERT_EQ( 2u, decode( "a\x01" "b", units ) );
    ASSERT_EQ( (UTF16)'a', units[0] );
    ASSERT_EQ( (UTF16)'b', units[1] );

    // A space is text.
    ASSERT_EQ( 1u, decode( " ", units ) );
    ASSERT_EQ( (UTF16)' ', units[0] );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, OutputStopsAtTheLimitWithoutSplittingAPair )
{
    UTF16 units[4] = { 0, 0, 0, 0 };

    ASSERT_EQ( 1u, decode( "ab", units, 1 ) );
    ASSERT_EQ( (UTF16)'a', units[0] );
    ASSERT_EQ( (UTF16)0, units[1] );

    // Room for the a, but not for both halves of the pair after it.
    ASSERT_EQ( 1u, decode( "a" "\xF0\x9F\x98\x80", units, 2 ) );
    ASSERT_EQ( (UTF16)'a', units[0] );
    ASSERT_EQ( (UTF16)0, units[1] );

    ASSERT_EQ( 0u, decode( "a", units, 0 ) );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, NoTextIsNoUnits )
{
    UTF16 units[4];

    ASSERT_EQ( 0u, decode( "", units ) );
    ASSERT_EQ( 0u, SDLTextInput::decodeText( NULL, units, 4 ) );
}

//-----------------------------------------------------------------------------

TEST( SdlTextInputTests, CharacterEventsCarryTheUnitOnKeyNull )
{
    InputEvent make;
    SDLTextInput::fillCharEvent( make, 0x00E9, SI_MAKE );

    ASSERT_EQ( (U32)InputEventType, (U32)make.type );
    ASSERT_EQ( (U32)KeyboardDeviceType, (U32)make.deviceType );
    ASSERT_EQ( (U32)SI_KEY, (U32)make.objType );
    ASSERT_EQ( (U32)KEY_NULL, (U32)make.objInst );
    ASSERT_EQ( (U32)SI_MAKE, (U32)make.action );
    ASSERT_EQ( 0u, (U32)make.modifier );
    ASSERT_EQ( 0x00E9u, (U32)make.ascii );
    ASSERT_EQ( 1.0f, make.fValues[0] );

    // The break matches the make. A modifier left in the event from before is
    // not carried on: a character is what was typed, Shift and all.
    InputEvent brk;
    brk.modifier = SI_LSHIFT;
    SDLTextInput::fillCharEvent( brk, 0xD83D, SI_BREAK );

    ASSERT_EQ( (U32)KEY_NULL, (U32)brk.objInst );
    ASSERT_EQ( (U32)SI_BREAK, (U32)brk.action );
    ASSERT_EQ( 0u, (U32)brk.modifier );
    ASSERT_EQ( 0xD83Du, (U32)brk.ascii );
    ASSERT_EQ( 0.0f, brk.fValues[0] );
}

#endif // TORQUE_SDL

#endif // TORQUE_SHIPPING
