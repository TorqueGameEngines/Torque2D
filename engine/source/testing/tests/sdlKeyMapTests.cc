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

#ifndef _SDLINPUT_H_
#include "platformSDL/sdlInput.h"
#endif

#include <SDL.h>

// platformSDL/sdlInput.cpp's tables: which physical key is which KEY_* code,
// and how SDL's modifier bits read as Torque's. What a key *types* depends on
// the layout of whoever runs the tests, so Input::getAscii and Input::getKeyCode
// are left to the desktop.

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, LettersAndDigitsAreMappedByPosition )
{
    // A scancode names a place on the keyboard, whatever the layout prints on
    // it: SDL_SCANCODE_Z is the key left of X, which types a y on a German
    // keyboard and is KEY_Z all the same.
    ASSERT_EQ( (U32)KEY_A, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_A ) );
    ASSERT_EQ( (U32)KEY_Q, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_Q ) );
    ASSERT_EQ( (U32)KEY_W, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_W ) );
    ASSERT_EQ( (U32)KEY_Z, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_Z ) );
    ASSERT_EQ( (U32)KEY_1, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_1 ) );
    ASSERT_EQ( (U32)KEY_0, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_0 ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, ControlNavigationAndFunctionKeys )
{
    ASSERT_EQ( (U32)KEY_RETURN, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RETURN ) );
    ASSERT_EQ( (U32)KEY_ESCAPE, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_ESCAPE ) );
    ASSERT_EQ( (U32)KEY_BACKSPACE, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_BACKSPACE ) );
    ASSERT_EQ( (U32)KEY_TAB, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_TAB ) );
    ASSERT_EQ( (U32)KEY_SPACE, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_SPACE ) );
    ASSERT_EQ( (U32)KEY_LEFT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_LEFT ) );
    ASSERT_EQ( (U32)KEY_UP, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_UP ) );
    ASSERT_EQ( (U32)KEY_RIGHT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RIGHT ) );
    ASSERT_EQ( (U32)KEY_DOWN, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_DOWN ) );
    ASSERT_EQ( (U32)KEY_PAGE_UP, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_PAGEUP ) );
    ASSERT_EQ( (U32)KEY_DELETE, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_DELETE ) );
    ASSERT_EQ( (U32)KEY_F1, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_F1 ) );
    ASSERT_EQ( (U32)KEY_F12, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_F12 ) );
    ASSERT_EQ( (U32)KEY_F24, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_F24 ) );

    // The console key, and the numpad.
    ASSERT_EQ( (U32)KEY_TILDE, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_GRAVE ) );
    ASSERT_EQ( (U32)KEY_NUMPAD0, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_KP_0 ) );
    ASSERT_EQ( (U32)KEY_DECIMAL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_KP_PERIOD ) );
    ASSERT_EQ( (U32)KEY_NUMPADENTER, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_KP_ENTER ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, ModifierKeysMapToTheirOwnSide )
{
    // The table maps each of them to the generic KEY_SHIFT, KEY_CONTROL or
    // KEY_ALT first, as Torque3D's does; the sided code is the one that has to
    // win.
    ASSERT_EQ( (U32)KEY_LSHIFT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_LSHIFT ) );
    ASSERT_EQ( (U32)KEY_RSHIFT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RSHIFT ) );
    ASSERT_EQ( (U32)KEY_LCONTROL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_LCTRL ) );
    ASSERT_EQ( (U32)KEY_RCONTROL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RCTRL ) );
    ASSERT_EQ( (U32)KEY_LALT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_LALT ) );
    ASSERT_EQ( (U32)KEY_RALT, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RALT ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, KeysTorque3DLeavesOutAreMapped )
{
    ASSERT_EQ( (U32)KEY_NUMLOCK, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_NUMLOCKCLEAR ) );
    ASSERT_EQ( (U32)KEY_SCROLLLOCK, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_SCROLLLOCK ) );
    ASSERT_EQ( (U32)KEY_WIN_LWINDOW, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_LGUI ) );
    ASSERT_EQ( (U32)KEY_WIN_RWINDOW, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_RGUI ) );
    ASSERT_EQ( (U32)KEY_WIN_APPS, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_APPLICATION ) );
    ASSERT_EQ( (U32)KEY_OEM_102, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_NONUSBACKSLASH ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, KeysTorqueHasNoCodeForMapToNull )
{
    ASSERT_EQ( (U32)KEY_NULL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_UNKNOWN ) );

    // Scancodes past 255 -- the media and browser keys -- which Torque3D's
    // table, 256 entries long, reads past the end of.
    ASSERT_EQ( (U32)KEY_NULL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_AUDIONEXT ) );
    ASSERT_EQ( (U32)KEY_NULL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_SCANCODE_AC_BACK ) );
    ASSERT_EQ( (U32)KEY_NULL, KeyMapSDL::getTorqueScanCodeFromSDL( SDL_NUM_SCANCODES ) );
    ASSERT_EQ( (U32)KEY_NULL, KeyMapSDL::getTorqueScanCodeFromSDL( 0xFFFFFFFF ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, TorqueCodesMapBackToTheirScancodes )
{
    const U32 scancodes[] =
    {
        SDL_SCANCODE_A, SDL_SCANCODE_Z, SDL_SCANCODE_5, SDL_SCANCODE_RETURN,
        SDL_SCANCODE_F7, SDL_SCANCODE_KP_9, SDL_SCANCODE_GRAVE,
        SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RCTRL, SDL_SCANCODE_RALT,
        SDL_SCANCODE_NONUSBACKSLASH
    };

    for ( U32 i = 0; i < sizeof( scancodes ) / sizeof( scancodes[0] ); ++i )
    {
        const U32 torqueKey = KeyMapSDL::getTorqueScanCodeFromSDL( scancodes[i] );
        ASSERT_EQ( scancodes[i], KeyMapSDL::getSDLScanCodeFromTorque( torqueKey ) )
            << "Scancode " << scancodes[i] << " did not come back from KEY_* code " << torqueKey;
    }

    ASSERT_EQ( (U32)SDL_SCANCODE_UNKNOWN, KeyMapSDL::getSDLScanCodeFromTorque( KEY_NULL ) );

    // Mouse buttons and the like are past the keyboard's codes, and have none.
    ASSERT_EQ( (U32)SDL_SCANCODE_UNKNOWN, KeyMapSDL::getSDLScanCodeFromTorque( KEY_BUTTON0 ) );
    ASSERT_EQ( (U32)SDL_SCANCODE_UNKNOWN, KeyMapSDL::getSDLScanCodeFromTorque( KEY_ANYKEY ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, ModifiersKeepTheirSide )
{
    ASSERT_EQ( (U32)SI_LSHIFT, KeyMapSDL::getTorqueModFromSDL( KMOD_LSHIFT ) );
    ASSERT_EQ( (U32)SI_RSHIFT, KeyMapSDL::getTorqueModFromSDL( KMOD_RSHIFT ) );
    ASSERT_EQ( (U32)SI_LCTRL, KeyMapSDL::getTorqueModFromSDL( KMOD_LCTRL ) );
    ASSERT_EQ( (U32)SI_RCTRL, KeyMapSDL::getTorqueModFromSDL( KMOD_RCTRL ) );
    ASSERT_EQ( (U32)SI_LALT, KeyMapSDL::getTorqueModFromSDL( KMOD_LALT ) );
    ASSERT_EQ( (U32)SI_RALT, KeyMapSDL::getTorqueModFromSDL( KMOD_RALT ) );

    ASSERT_EQ( (U32)( SI_LSHIFT | SI_RCTRL | SI_LALT ),
        KeyMapSDL::getTorqueModFromSDL( (U16)( KMOD_LSHIFT | KMOD_RCTRL | KMOD_LALT ) ) );
    ASSERT_EQ( (U32)SI_SHIFT,
        KeyMapSDL::getTorqueModFromSDL( (U16)( KMOD_LSHIFT | KMOD_RSHIFT ) ) );
}

//-----------------------------------------------------------------------------

TEST( SdlKeyMapTests, LocksAndOtherModifiersAreNotTorqueModifiers )
{
    ASSERT_EQ( 0u, KeyMapSDL::getTorqueModFromSDL( KMOD_NONE ) );
    ASSERT_EQ( 0u, KeyMapSDL::getTorqueModFromSDL( KMOD_CAPS ) );
    ASSERT_EQ( 0u, KeyMapSDL::getTorqueModFromSDL( KMOD_NUM ) );
    ASSERT_EQ( 0u, KeyMapSDL::getTorqueModFromSDL( KMOD_LGUI ) );
    ASSERT_EQ( 0u, KeyMapSDL::getTorqueModFromSDL( KMOD_MODE ) );

    // Num Lock on doesn't disturb Shift.
    ASSERT_EQ( (U32)SI_LSHIFT, KeyMapSDL::getTorqueModFromSDL( (U16)( KMOD_NUM | KMOD_LSHIFT ) ) );
}

#endif // TORQUE_SDL

#endif // TORQUE_SHIPPING
