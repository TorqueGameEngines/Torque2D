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

#ifndef _UNIT_TESTING_H_
#include "testing/unitTesting.h"
#endif

// guiTextEditCtrl.h names GuiControl as a base without including it, so the
// order here matters.
#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

//-----------------------------------------------------------------------------
// Typing a character into a text box.
//
// A keyboard event carries its character as one UTF-16 code unit, and a text
// box keeps its text as UTF-8 with the caret as a byte offset into it. The box
// used to insert the code unit as a single byte -- string(1, ascii) -- so 'e'
// with an acute accent went in as the lone byte 0xE9, which is not UTF-8, and
// anything past U+00FF lost its high byte and became some other character.
//
// The character reaches the box one of two ways. On Windows and macOS it rides
// on the key-down of the key that typed it. On SDL's text input it arrives in
// an event of its own with no key behind it -- keyCode KEY_NULL -- after a
// key-down that carries nothing; Windows IME input already arrives that way.
// Both are covered here, as is the rest of what a multi-byte character asks
// of the box: the caret, Backspace, Delete and the arrows have to step over all
// of it, or the next thing typed lands inside it.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Every character used below, spelled out in bytes so that what is under test
// does not depend on how this file itself was saved.
static const char* const E_ACUTE = "\xC3\xA9";        // U+00E9, 2 bytes
static const char* const Y_DIAERESIS = "\xC3\xBF";    // U+00FF, the top of Latin-1
static const char* const EURO = "\xE2\x82\xAC";       // U+20AC, 3 bytes
static const char* const REPLACEMENT = "\xEF\xBF\xBD"; // U+FFFD

// What composeTypedCharacter wrote, as a string to compare.
static string typedBytes( const UTF16 unit, UTF16& pending )
{
    UTF8 buffer[3] = { 0, 0, 0 };
    const U32 length = GuiTextEditCtrl::composeTypedCharacter( unit, pending, buffer );
    return string( (const char*)buffer, length );
}

static GuiEvent keyEvent( const U8 keyCode, const U16 ascii, const U8 modifier = 0 )
{
    GuiEvent event = GuiEvent();
    event.keyCode = keyCode;
    event.ascii = ascii;
    event.modifier = modifier;
    return event;
}

// Reaches the key handlers and the caret state through the class itself. The
// one override stands in for the font: the suite has no canvas, and asking a
// font about a glyph loads it, which registers a texture, which asserts.
class TypingTextEditCtrl : public GuiTextEditCtrl
{
public:
    using GuiTextEditCtrl::mSelector;
    using GuiTextEditCtrl::mInsertOn;
    using GuiTextEditCtrl::insertCharacter;
    using GuiTextEditCtrl::handleBackSpace;
    using GuiTextEditCtrl::handleDelete;
    using GuiTextEditCtrl::handleArrowKey;
    using GuiTextEditCtrl::handleShiftArrowKey;

    virtual bool canDisplayCharacter( const UTF16 character ) { return true; }
};

// A box holding text, the caret at a byte offset into it. It deletes the box
// however the test ends: a failed ASSERT returns from the test at once, and a
// control left registered is torn down at exit, after the profile it wears.
class TypingBox
{
public:
    TypingBox( const string& text, const U32 cursorPos )
    {
        mBox = new TypingTextEditCtrl();
        mBox->registerObject();
        mBox->setText( text );
        mBox->mSelector.setTextLength( text.length() );
        mBox->mSelector.setCursorPosition( cursorPos );
    }
    ~TypingBox() { mBox->deleteObject(); }
    TypingTextEditCtrl* operator->() { return mBox; }

private:
    TypingBox( const TypingBox& );
    TypingBox& operator=( const TypingBox& );

    TypingTextEditCtrl* mBox;
};

//-----------------------------------------------------------------------------
// One code unit in, the UTF-8 that goes into the text out
//-----------------------------------------------------------------------------

TEST( GuiTextInputTests, AsciiIsOneByte )
{
    UTF16 pending = 0;
    ASSERT_STREQ( typedBytes( 'a', pending ).c_str(), "a" );
    ASSERT_EQ( pending, 0 );

    SUCCEED();
}

TEST( GuiTextInputTests, LatinOneIsTwoBytesNotOne )
{
    // The bug: this used to go in as the single byte 0xE9.
    UTF16 pending = 0;
    ASSERT_STREQ( typedBytes( 0x00E9, pending ).c_str(), E_ACUTE );
    ASSERT_STREQ( typedBytes( 0x00FF, pending ).c_str(), Y_DIAERESIS );

    SUCCEED();
}

TEST( GuiTextInputTests, PastLatinOneKeepsItsHighByte )
{
    // Truncated to one byte, the euro sign came out as 0xAC.
    UTF16 pending = 0;
    ASSERT_STREQ( typedBytes( 0x20AC, pending ).c_str(), EURO );

    SUCCEED();
}

TEST( GuiTextInputTests, ASurrogatePairIsOneCharacter )
{
    // U+1F600 arrives as two events. The first adds nothing and is held.
    UTF16 pending = 0;
    ASSERT_EQ( typedBytes( 0xD83D, pending ).length(), 0 ) << "Half a character is not text yet.";
    ASSERT_EQ( pending, 0xD83D );

    // The second completes it. The engine's text is BMP-only -- its UTF-16 and
    // UTF-8 conversions replace anything beyond it -- so one replacement
    // character, not two broken halves.
    ASSERT_STREQ( typedBytes( 0xDE00, pending ).c_str(), REPLACEMENT );
    ASSERT_EQ( pending, 0 ) << "The pair is finished with.";

    SUCCEED();
}

TEST( GuiTextInputTests, ALowSurrogateAloneAddsNothing )
{
    UTF16 pending = 0;
    ASSERT_EQ( typedBytes( 0xDE00, pending ).length(), 0 );
    ASSERT_EQ( pending, 0 );

    SUCCEED();
}

TEST( GuiTextInputTests, AHighSurrogateWithoutItsPartnerIsDropped )
{
    // The next unit is an ordinary character: the held half goes, and the
    // character goes in as itself.
    UTF16 pending = 0xD83D;
    ASSERT_STREQ( typedBytes( 'x', pending ).c_str(), "x" );
    ASSERT_EQ( pending, 0 );

    // A second high half replaces the first.
    pending = 0xD83D;
    ASSERT_EQ( typedBytes( 0xD83C, pending ).length(), 0 );
    ASSERT_EQ( pending, 0xD83C );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Which events are text
//-----------------------------------------------------------------------------

TEST( GuiTextInputTests, ControlCodesAreNotText )
{
    // What Windows reports for NUL, Backspace, Tab, Return, Escape, Ctrl+C,
    // Ctrl+Backspace; and the C1 block.
    const UTF16 controls[] = { 0x00, 0x03, 0x08, 0x09, 0x0D, 0x1B, 0x1F, 0x7F, 0x80, 0x9F };
    for ( U32 i = 0; i < ( sizeof( controls ) / sizeof( controls[0] ) ); i++ )
    {
        ASSERT_FALSE( GuiTextEditCtrl::isTypedCharacter( controls[i] ) ) << "0x" << std::hex << controls[i];
    }

    const UTF16 text[] = { ' ', 'a', '~', 0x00A0, 0x00E9, 0x20AC, 0xD83D, 0xDE00, 0xFFFD };
    for ( U32 i = 0; i < ( sizeof( text ) / sizeof( text[0] ) ); i++ )
    {
        ASSERT_TRUE( GuiTextEditCtrl::isTypedCharacter( text[i] ) ) << "0x" << std::hex << text[i];
    }

    SUCCEED();
}

TEST( GuiTextInputTests, ACharacterEventHasNoKey )
{
    ASSERT_TRUE( GuiTextEditCtrl::isCharacterEvent( keyEvent( KEY_NULL, 0x00E9 ) ) );

    // A modifier set on it does not make it a chord: it is still just text.
    ASSERT_TRUE( GuiTextEditCtrl::isCharacterEvent( keyEvent( KEY_NULL, 0x00E9, SI_LCTRL ) ) );

    // A key carrying its character is a key, and goes through the key handling.
    ASSERT_FALSE( GuiTextEditCtrl::isCharacterEvent( keyEvent( KEY_A, 'a' ) ) );

    // An event with neither is nothing.
    ASSERT_FALSE( GuiTextEditCtrl::isCharacterEvent( keyEvent( KEY_NULL, 0 ) ) );

    SUCCEED();
}

TEST( GuiTextInputTests, AltGrIsNotCtrl )
{
    // Windows reports AltGr as left Ctrl plus right Alt. AltGr+Q on a German
    // keyboard is '@'; AltGr+C on a Polish one is c-acute.
    ASSERT_TRUE( GuiTextEditCtrl::isAltGrCharacter( keyEvent( KEY_Q, '@', SI_LCTRL | SI_RALT ) ) );
    ASSERT_TRUE( GuiTextEditCtrl::isAltGrCharacter( keyEvent( KEY_C, 0x0107, SI_LCTRL | SI_RALT ) ) );

    // Ctrl alone is a shortcut, and a Ctrl+Alt key that types nothing is too.
    ASSERT_FALSE( GuiTextEditCtrl::isAltGrCharacter( keyEvent( KEY_C, 0x03, SI_LCTRL ) ) );
    ASSERT_FALSE( GuiTextEditCtrl::isAltGrCharacter( keyEvent( KEY_C, 'c', SI_LCTRL ) ) );
    ASSERT_FALSE( GuiTextEditCtrl::isAltGrCharacter( keyEvent( KEY_C, 0, SI_LCTRL | SI_RALT ) ) );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// The character in the text, and the caret after it
//-----------------------------------------------------------------------------

TEST( GuiTextInputTests, TheCaretStepsPastTheWholeCharacter )
{
    TypingBox box( "ab", 1 );

    box->insertCharacter( E_ACUTE );
    ASSERT_STREQ( box->getText(), ( string( "a" ) + E_ACUTE + "b" ).c_str() );
    ASSERT_EQ( box->mSelector.getCursorPos(), 3 ) << "Past both bytes of the character, not between them.";

    // So the next character goes after it rather than inside it.
    box->insertCharacter( "x" );
    ASSERT_STREQ( box->getText(), ( string( "a" ) + E_ACUTE + "xb" ).c_str() );

    SUCCEED();
}

TEST( GuiTextInputTests, OverwriteReplacesAWholeCharacter )
{
    TypingBox box( string( EURO ) + "a", 0 );
    box->mInsertOn = false;

    box->insertCharacter( "x" );
    ASSERT_STREQ( box->getText(), "xa" ) << "All three bytes of the euro sign, not the first of them.";
    ASSERT_EQ( box->mSelector.getCursorPos(), 1 );

    SUCCEED();
}

TEST( GuiTextInputTests, ACharacterThatWouldNotFitIsRefused )
{
    // maxLength bounds the buffer, which is bytes: two bytes will not go into
    // the one that is left.
    TypingBox box( "ab", 2 );
    box->setMaxLength( 3 );

    box->insertCharacter( E_ACUTE );
    ASSERT_STREQ( box->getText(), "ab" ) << "Half a character does not go in to fill the space.";

    box->insertCharacter( "c" );
    ASSERT_STREQ( box->getText(), "abc" );

    SUCCEED();
}

TEST( GuiTextInputTests, BackspaceRemovesTheWholeCharacter )
{
    TypingBox box( string( "a" ) + E_ACUTE, 3 );

    box->handleBackSpace();
    ASSERT_STREQ( box->getText(), "a" ) << "One press, one character -- not a byte, leaving half of one.";
    ASSERT_EQ( box->mSelector.getCursorPos(), 1 );

    SUCCEED();
}

TEST( GuiTextInputTests, DeleteRemovesTheWholeCharacter )
{
    TypingBox box( string( EURO ) + "a", 0 );

    box->handleDelete();
    ASSERT_STREQ( box->getText(), "a" );
    ASSERT_EQ( box->mSelector.getCursorPos(), 0 );

    SUCCEED();
}

TEST( GuiTextInputTests, ArrowsStepOverWholeCharacters )
{
    // a | euro | e-acute | b, at byte offsets 0, 1, 4, 6, 7.
    const string text = string( "a" ) + EURO + E_ACUTE + "b";
    TypingBox box( text, 0 );

    const U32 stops[] = { 1, 4, 6, 7, 7 };
    for ( U32 i = 0; i < ( sizeof( stops ) / sizeof( stops[0] ) ); i++ )
    {
        box->handleArrowKey( GuiDirection::Right );
        ASSERT_EQ( box->mSelector.getCursorPos(), stops[i] ) << "Right arrow, press " << ( i + 1 );
    }

    const U32 backStops[] = { 6, 4, 1, 0, 0 };
    for ( U32 i = 0; i < ( sizeof( backStops ) / sizeof( backStops[0] ) ); i++ )
    {
        box->handleArrowKey( GuiDirection::Left );
        ASSERT_EQ( box->mSelector.getCursorPos(), backStops[i] ) << "Left arrow, press " << ( i + 1 );
    }

    SUCCEED();
}

TEST( GuiTextInputTests, ShiftArrowsSelectWholeCharacters )
{
    TypingBox box( string( "a" ) + E_ACUTE, 3 );

    box->handleShiftArrowKey( GuiDirection::Left );
    ASSERT_EQ( box->mSelector.getSelStart(), 1 );
    ASSERT_EQ( box->mSelector.getSelEnd(), 3 );

    SUCCEED();
}

TEST( GuiTextInputTests, CharacterBoundariesSkipContinuationBytes )
{
    const string text = string( "a" ) + EURO + E_ACUTE;

    ASSERT_EQ( GuiTextEditSelection::nextCharacterStart( text, 0 ), 1 );
    ASSERT_EQ( GuiTextEditSelection::nextCharacterStart( text, 1 ), 4 );
    ASSERT_EQ( GuiTextEditSelection::nextCharacterStart( text, 4 ), 6 );
    ASSERT_EQ( GuiTextEditSelection::nextCharacterStart( text, 6 ), 6 ) << "The end is the end.";
    ASSERT_EQ( GuiTextEditSelection::nextCharacterStart( text, 99 ), 6 );

    ASSERT_EQ( GuiTextEditSelection::previousCharacterStart( text, 6 ), 4 );
    ASSERT_EQ( GuiTextEditSelection::previousCharacterStart( text, 4 ), 1 );
    ASSERT_EQ( GuiTextEditSelection::previousCharacterStart( text, 1 ), 0 );
    ASSERT_EQ( GuiTextEditSelection::previousCharacterStart( text, 0 ), 0 );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Both ways a character reaches the box, through onKeyDown
//-----------------------------------------------------------------------------

TEST( GuiTextInputTests, ACharacterOnItsKeyDownIsTyped )
{
    // Windows and macOS: the key-down carries the character.
    TypingBox box( "", 0 );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_2, 0x00E9 ) ) );
    ASSERT_STREQ( box->getText(), E_ACUTE );
    ASSERT_EQ( box->mSelector.getCursorPos(), 2 );

    SUCCEED();
}

TEST( GuiTextInputTests, ACharacterInAnEventOfItsOwnIsTypedOnce )
{
    // SDL's text input: the key-down carries nothing and types nothing, and
    // the character follows by itself.
    TypingBox box( "", 0 );

    box->onKeyDown( keyEvent( KEY_E, 0 ) );
    ASSERT_STREQ( box->getText(), "" ) << "A key-down with no character types nothing.";

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_NULL, 0x00E9 ) ) );
    ASSERT_STREQ( box->getText(), E_ACUTE );

    SUCCEED();
}

TEST( GuiTextInputTests, ASurrogatePairAcrossTwoEventsIsOneCharacter )
{
    TypingBox box( "", 0 );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_NULL, 0xD83D ) ) ) << "The first half is taken, and held.";
    ASSERT_STREQ( box->getText(), "" );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_NULL, 0xDE00 ) ) );
    ASSERT_STREQ( box->getText(), REPLACEMENT );
    ASSERT_EQ( box->mSelector.getCursorPos(), 3 );

    SUCCEED();
}

TEST( GuiTextInputTests, ACharacterEventIsNeverAShortcut )
{
    // 'v' with Ctrl set on it, arriving as text: typed, not pasted. Ctrl+V as
    // a key would have gone to the clipboard.
    TypingBox box( "", 0 );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_NULL, 'v', SI_LCTRL ) ) );
    ASSERT_STREQ( box->getText(), "v" );

    SUCCEED();
}

TEST( GuiTextInputTests, AltGrTypesRatherThanRunningAShortcut )
{
    TypingBox box( "", 0 );

    // AltGr+C on a Polish keyboard, as Windows reports it.
    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_C, 0x0107, SI_LCTRL | SI_RALT ) ) );
    ASSERT_STREQ( box->getText(), "\xC4\x87" );

    SUCCEED();
}

TEST( GuiTextInputTests, CtrlStillRunsShortcutsAndTypesNothing )
{
    // Ctrl+Z as Windows reports it, with its control code as the character.
    TypingBox box( "ab", 2 );
    box->onKeyDown( keyEvent( KEY_NULL, 'c' ) );
    ASSERT_STREQ( box->getText(), "abc" );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_Z, 0x1A, SI_LCTRL ) ) );
    ASSERT_STREQ( box->getText(), "ab" ) << "Undo ran, and nothing was typed.";

    // Ctrl+Shift+letter reaches the character handling through the Shift
    // branch, and used to type its control code into the text.
    ASSERT_FALSE( box->onKeyDown( keyEvent( KEY_A, 0x01, SI_LCTRL | SI_LSHIFT ) ) );
    ASSERT_STREQ( box->getText(), "ab" );

    SUCCEED();
}

TEST( GuiTextInputTests, KeysWithoutACharacterStillWork )
{
    // Every key-down on SDL's text input carries no character, and must still
    // do what the key does.
    TypingBox box( string( "a" ) + E_ACUTE, 3 );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_LEFT, 0 ) ) );
    ASSERT_EQ( box->mSelector.getCursorPos(), 1 );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_DELETE, 0 ) ) );
    ASSERT_STREQ( box->getText(), "a" );

    ASSERT_TRUE( box->onKeyDown( keyEvent( KEY_BACKSPACE, 0 ) ) );
    ASSERT_STREQ( box->getText(), "" );

    SUCCEED();
}

#endif // TORQUE_SHIPPING
