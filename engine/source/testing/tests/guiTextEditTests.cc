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

#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif

//-----------------------------------------------------------------------------
// The caret in a multi-line text box, and the line list it stands on.
//
// A text box draws itself one line block at a time, and each block is asked
// whether the caret belongs to it. A caret is a position BETWEEN two
// characters, so the seam between two lines is a single position with two
// homes: the end of the line above and the start of the line below. Exactly one
// of them may draw it, or the box blinks two carets at once.
//
// That decision is GuiTextEditSelection::isIbeamOnLine, and these tests are the
// whole of it. They walk a line list the way GuiTextEditCtrl::renderLineList
// does -- a line's first caret position is the sum of the lengths of the lines
// above it -- and count how many lines claim the caret. The answer is one.
//
// The line lists below are what GuiControl::getLineList returns for the text
// each test names. The caret arithmetic here is only right for as long as the
// line list keeps that shape, and both halves have broken in turn -- see the
// note at the foot of this file for what holds the two together.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// A caret that is showing: the box holds the first responder and the blink is
// in its "on" half-second. Text length first -- setCursorPosition clamps to it.
static void placeCaret( GuiTextEditSelection& selector, const U32 textLength, const U32 cursorPos, const bool atEOL )
{
    selector.setTextLength( textLength );
    selector.setCursorPosition( cursorPos );
    selector.setCursorAtEOL( atEOL );
    selector.setFirstResponder( true );
    selector.resetCursorBlink();
}

// Every line of the list that would draw the caret, by index. One entry means a
// working text box; two means the bug this file exists for.
static vector<U32> caretLines( const GuiTextEditSelection& selector, const vector<string>& lineList )
{
    vector<U32> claimed = vector<U32>();
    U32 ibeamPos = 0;
    for ( U32 i = 0; i < lineList.size(); i++ )
    {
        const bool isLastLine = ( i == ( lineList.size() - 1 ) );
        if ( selector.isIbeamOnLine( ibeamPos, ibeamPos + lineList[i].length(), isLastLine ) )
        {
            claimed.push_back( i );
        }
        ibeamPos += lineList[i].length();
    }
    return claimed;
}

static vector<string> lines( const char* a, const char* b = NULL, const char* c = NULL )
{
    vector<string> lineList = vector<string>();
    lineList.push_back( string( a ) );
    if ( b != NULL ) lineList.push_back( string( b ) );
    if ( c != NULL ) lineList.push_back( string( c ) );
    return lineList;
}

// The whole text back out of a line list, so a test states its text once.
static U32 textLengthOf( const vector<string>& lineList )
{
    U32 length = 0;
    for ( U32 i = 0; i < lineList.size(); i++ )
    {
        length += lineList[i].length();
    }
    return length;
}

//-----------------------------------------------------------------------------
// One caret, wherever it is
//-----------------------------------------------------------------------------

TEST( GuiTextEditTests, CaretShowsOnceAtEveryPositionInASingleLine )
{
    const vector<string> lineList = lines( "abc" );

    for ( U32 pos = 0; pos <= 3; pos++ )
    {
        GuiTextEditSelection selector;
        placeCaret( selector, 3, pos, false );

        const vector<U32> claimed = caretLines( selector, lineList );
        ASSERT_EQ( claimed.size(), 1 ) << "One line, so one caret, at position " << pos << ".";
        ASSERT_EQ( claimed[0], 0 );
    }

    SUCCEED();
}

TEST( GuiTextEditTests, CaretShowsOnceInAnEmptyBox )
{
    // Empty text is one blank line, and the caret sits at the start of it.
    const vector<string> lineList = lines( "" );

    GuiTextEditSelection selector;
    placeCaret( selector, 0, 0, false );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 ) << "An empty box still shows where typing would go.";
    ASSERT_EQ( claimed[0], 0 );

    SUCCEED();
}

TEST( GuiTextEditTests, WrapSeamGivesTheCaretToTheLineBelow )
{
    // "hello world" wrapped: the space is kept on the line it ended.
    const vector<string> lineList = lines( "hello ", "world" );

    GuiTextEditSelection selector;
    placeCaret( selector, 11, 6, false );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 ) << "Position 6 is the end of one line and the start of the next.";
    ASSERT_EQ( claimed[0], 1 ) << "Not at end of line, so the caret is on the lower line.";

    SUCCEED();
}

TEST( GuiTextEditTests, WrapSeamGivesTheCaretToTheLineAboveWhenAtEOL )
{
    const vector<string> lineList = lines( "hello ", "world" );

    GuiTextEditSelection selector;
    placeCaret( selector, 11, 6, true );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 );
    ASSERT_EQ( claimed[0], 0 ) << "At end of line, so the caret stays on the upper line.";

    SUCCEED();
}

TEST( GuiTextEditTests, CaretShowsOnceAtTheEndOfTheText )
{
    const vector<string> lineList = lines( "hello ", "world" );

    GuiTextEditSelection selector;
    placeCaret( selector, 11, 11, false );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 ) << "There is no line after the last one to hand it to.";
    ASSERT_EQ( claimed[0], 1 );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// The line a return makes
//-----------------------------------------------------------------------------

TEST( GuiTextEditTests, CaretShowsOnceOnTheEmptyLineAReturnMakes )
{
    // "abc" and a return: the newline stays on the end of its paragraph and the
    // empty paragraph after it is a line of its own. The caret is at position 4,
    // which is both the end of the first line and the start of the second --
    // and the end of the whole text, which is what used to make both of them
    // draw it.
    const vector<string> lineList = lines( "abc\n", "" );

    GuiTextEditSelection selector;
    placeCaret( selector, 4, 4, false );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 ) << "Two carets: one at the end of the old line, one on the new line.";
    ASSERT_EQ( claimed[0], 1 ) << "Return moves the caret to the new line, not the end of the old one.";

    SUCCEED();
}

TEST( GuiTextEditTests, CaretShowsOnceAfterTwoReturns )
{
    // The blank line in the middle is a paragraph holding nothing but its own
    // newline, so it is a line with length, unlike the empty one at the end.
    const vector<string> lineList = lines( "abc\n", "\n", "" );

    GuiTextEditSelection selector;
    placeCaret( selector, 5, 5, false );

    const vector<U32> claimed = caretLines( selector, lineList );
    ASSERT_EQ( claimed.size(), 1 );
    ASSERT_EQ( claimed[0], 2 ) << "The caret is on the last of the empty lines.";

    SUCCEED();
}

TEST( GuiTextEditTests, CaretShowsOnceAtEveryPositionOfTextEndingInAReturn )
{
    // The sweep the two tests above are samples of: every caret position of
    // "ab\ncd\n", either side of the end-of-line flag, is owned by one line.
    const vector<string> lineList = lines( "ab\n", "cd\n", "" );
    const U32 textLength = textLengthOf( lineList );

    for ( U32 pos = 0; pos <= textLength; pos++ )
    {
        for ( U32 eol = 0; eol <= 1; eol++ )
        {
            GuiTextEditSelection selector;
            placeCaret( selector, textLength, pos, ( eol == 1 ) );

            const vector<U32> claimed = caretLines( selector, lineList );
            ASSERT_EQ( claimed.size(), 1 )
                << "Position " << pos << ( eol == 1 ? " at end of line" : "" ) << " drew " << claimed.size() << " carets.";
        }
    }

    SUCCEED();
}

//-----------------------------------------------------------------------------
// No caret at all
//-----------------------------------------------------------------------------

TEST( GuiTextEditTests, CaretIsHiddenWhenTheBoxIsNotTheFirstResponder )
{
    const vector<string> lineList = lines( "abc\n", "" );

    GuiTextEditSelection selector;
    placeCaret( selector, 4, 4, false );
    selector.setFirstResponder( false );

    ASSERT_EQ( caretLines( selector, lineList ).size(), 0 ) << "A box that is not being edited has no caret.";

    SUCCEED();
}

TEST( GuiTextEditTests, CaretIsHiddenWhileTheBlinkIsOff )
{
    const vector<string> lineList = lines( "abc\n", "" );

    // A fresh selection has not blinked on yet, so this is the dark half of the
    // blink without waiting half a second for it.
    GuiTextEditSelection selector;
    selector.setTextLength( 4 );
    selector.setCursorPosition( 4 );
    selector.setFirstResponder( true );

    ASSERT_EQ( caretLines( selector, lineList ).size(), 0 );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Return, from the control
//-----------------------------------------------------------------------------

// Reaches the key handler and the caret state through the class itself rather
// than through new public methods: nothing here exists for the test's benefit.
class TestTextEditCtrl : public GuiTextEditCtrl
{
public:
    using GuiTextEditCtrl::mSelector;
    using GuiTextEditCtrl::insertNewLine;
};

TEST( GuiTextEditTests, ReturnPutsTheCaretOnTheNewLine )
{
    TestTextEditCtrl* box = new TestTextEditCtrl();
    box->registerObject();
    box->setTextWrap( true );
    box->setText( "hello world" );

    // What a click at the end of a wrapped line leaves behind: the box is being
    // edited, and the caret is at the seam, belonging to the line above.
    box->mSelector.setTextLength( 11 );
    box->mSelector.setCursorPosition( 6 );
    box->mSelector.setCursorAtEOL( true );
    box->mSelector.setFirstResponder( true );
    box->mSelector.resetCursorBlink();

    box->insertNewLine();

    ASSERT_EQ( box->mSelector.getCursorPos(), 7 ) << "The caret steps over the line break it just made.";

    // "hello \n" is now a paragraph of its own, so position 7 is the seam
    // between it and "world". A caret still flagged end-of-line draws at the end
    // of the line above -- the line the user just left.
    const vector<string> lineList = lines( "hello \n", "world" );
    const vector<U32> claimed = caretLines( box->mSelector, lineList );

    ASSERT_EQ( claimed.size(), 1 );
    ASSERT_EQ( claimed[0], 1 ) << "Return moves the caret onto the new line.";

    box->deleteObject();

    SUCCEED();
}

TEST( GuiTextEditTests, ReturnInsertsALineBreakAtTheCaret )
{
    TestTextEditCtrl* box = new TestTextEditCtrl();
    box->registerObject();
    box->setTextWrap( true );
    box->setText( "abcdef" );

    box->mSelector.setTextLength( 6 );
    box->mSelector.setCursorPosition( 3 );

    box->insertNewLine();

    ASSERT_STREQ( box->getText(), "abc\ndef" );
    ASSERT_EQ( box->mSelector.getCursorPos(), 4 );

    box->deleteObject();

    SUCCEED();
}

//-----------------------------------------------------------------------------
// The caret and the length of the text it is in
//
// Every caret move clamps to the length of the text, so the selection has to be
// told that length by whoever changes the text. Both halves of that have been
// missing: the length started as uninitialized memory, and setText -- the one
// path that changes the text with no keystroke behind it -- never passed it on.
//-----------------------------------------------------------------------------

TEST( GuiTextEditTests, AFreshSelectionHoldsNoText )
{
    GuiTextEditSelection selector;

    ASSERT_EQ( selector.getCursorPos(), 0 );

    // setCursorPosition clamps to the text length. Left uninitialized, that
    // length is whatever was in the memory, so the caret lands anywhere.
    selector.setCursorPosition( 5 );

    ASSERT_EQ( selector.getCursorPos(), 0 ) << "No text, so there is nowhere for the caret to go.";

    SUCCEED();
}

TEST( GuiTextEditTests, TheCaretReachesTextThatWasSetRatherThanTyped )
{
    TestTextEditCtrl* box = new TestTextEditCtrl();
    box->registerObject();
    box->setText( "abc" );

    // What a control loaded from TAML has: text, and no keystroke or click
    // behind it to have told the selection how long that text is.
    box->setIbeamPosition( 2 );

    ASSERT_EQ( box->getIbeamPosition(), 2 ) << "The caret can go anywhere inside text the box was given.";

    box->deleteObject();

    SUCCEED();
}

TEST( GuiTextEditTests, TheCaretCannotBePlacedPastTheEndOfTheText )
{
    TestTextEditCtrl* box = new TestTextEditCtrl();
    box->registerObject();
    box->setText( "abc" );

    box->setIbeamPosition( 99 );

    ASSERT_EQ( box->getIbeamPosition(), 3 ) << "Past the end is the end.";

    box->deleteObject();

    SUCCEED();
}

TEST( GuiTextEditTests, ClearingTheTextBringsTheCaretBack )
{
    TestTextEditCtrl* box = new TestTextEditCtrl();
    box->registerObject();
    box->setText( "abc" );
    box->setIbeamPosition( 3 );

    box->setText( "" );

    ASSERT_EQ( box->getIbeamPosition(), 0 ) << "The text it pointed into is gone.";

    box->deleteObject();

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Losing focus, as script hears it
//
// A box that loses focus tells script whether its text passed onValidate, in
// onLoseFirstResponder and in onBlur. Con::executef takes its arguments as
// strings, and that verdict used to go in as a bare bool, which the console
// read as a pointer: any box whose class defined either callback crashed the
// engine the moment it lost focus, before the callback's first line ran.
//-----------------------------------------------------------------------------

// A box built by script, so it has a class for its callbacks to live in.
static GuiTextEditCtrl* newScriptedBox( const char* className )
{
    const char* id = Con::evaluatef( "return new GuiTextEditCtrl() { class = \"%s\"; };", className );
    return dynamic_cast<GuiTextEditCtrl*>( Sim::findObject( id ) );
}

TEST( GuiTextEditTests, LosingFocusTellsOnLoseFirstResponderTheTextIsValid )
{
    Con::evaluate( "function UnitTestLoseFocusBox::onLoseFirstResponder( %this, %valid ) { $UnitTestLoseFocus = %valid; }" );
    Con::setVariable( "$UnitTestLoseFocus", "unset" );

    GuiTextEditCtrl* box = newScriptedBox( "UnitTestLoseFocusBox" );
    ASSERT_TRUE( box != NULL );

    box->onLoseFirstResponder();

    ASSERT_STREQ( Con::getVariable( "$UnitTestLoseFocus" ), "1" ) << "No onValidate, so the text stands.";

    box->deleteObject();

    SUCCEED();
}

TEST( GuiTextEditTests, LosingFocusTellsOnBlurTheTextWasRefused )
{
    Con::evaluate( "function UnitTestBlurBox::onValidate( %this ) { return false; }" );
    Con::evaluate( "function UnitTestBlurBox::onBlur( %this, %valid ) { $UnitTestBlur = %valid; }" );
    Con::setVariable( "$UnitTestBlur", "unset" );

    GuiTextEditCtrl* box = newScriptedBox( "UnitTestBlurBox" );
    ASSERT_TRUE( box != NULL );

    box->onLoseFirstResponder();

    ASSERT_STREQ( Con::getVariable( "$UnitTestBlur" ), "0" ) << "onValidate said no, and onBlur should hear it.";

    box->deleteObject();

    SUCCEED();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// The line list the caret arithmetic rests on
//
// GuiControl::getLineList is two jobs: splitting text into paragraphs on its
// line breaks, and wrapping each of those to a width. Only the second needs a
// font, so only the second is out of reach here -- measuring text loads a font,
// a font registers a texture, and TextureManager::refresh asserts because this
// suite runs with no canvas and so no GL context to make one in.
//
// The split is the half the caret cares about, and the half that has broken:
// every line list written out by hand above is what it returns.
//-----------------------------------------------------------------------------

TEST( GuiTextEditTests, EmptyTextIsOneParagraph )
{
    const vector<string> paragraphs = GuiControl::splitParagraphs( "" );

    ASSERT_EQ( paragraphs.size(), 1 ) << "No paragraph means no line block, and a line block is what draws the caret.";
    ASSERT_EQ( paragraphs[0].length(), 0 );

    SUCCEED();
}

TEST( GuiTextEditTests, ALineBreakStaysOnTheEndOfItsParagraph )
{
    const vector<string> paragraphs = GuiControl::splitParagraphs( "ab\ncd" );

    ASSERT_EQ( paragraphs.size(), 2 );
    ASSERT_STREQ( paragraphs[0].c_str(), "ab\n" ) << "Dropping the break here moves the caret on every line below it.";
    ASSERT_STREQ( paragraphs[1].c_str(), "cd" );

    SUCCEED();
}

TEST( GuiTextEditTests, ATrailingReturnMakesAnEmptyLastParagraph )
{
    const vector<string> paragraphs = GuiControl::splitParagraphs( "abc\n" );

    ASSERT_EQ( paragraphs.size(), 2 ) << "The caret has to have a line to sit on after a return.";
    ASSERT_STREQ( paragraphs[0].c_str(), "abc\n" );
    ASSERT_EQ( paragraphs[1].length(), 0 );

    SUCCEED();
}

TEST( GuiTextEditTests, ABlankLineBetweenTwoParagraphsIsKept )
{
    const vector<string> paragraphs = GuiControl::splitParagraphs( "a\n\nb" );

    ASSERT_EQ( paragraphs.size(), 3 );
    ASSERT_STREQ( paragraphs[1].c_str(), "\n" ) << "A blank line is a paragraph holding nothing but its own break.";

    SUCCEED();
}

TEST( GuiTextEditTests, ParagraphLengthsSumToTheTextLength )
{
    // The invariant the caret stands on: renderLineList finds a line's first
    // caret position by summing the lengths of the lines above it, so a
    // character dropped or invented here moves the caret.
    const char* texts[] = { "", "abc", "abc\n", "abc\n\n", "\n", "ab\ncd\n", "hello world" };

    for ( U32 i = 0; i < ( sizeof( texts ) / sizeof( texts[0] ) ); i++ )
    {
        const vector<string> paragraphs = GuiControl::splitParagraphs( texts[i] );

        U32 sum = 0;
        for ( U32 p = 0; p < paragraphs.size(); p++ )
        {
            sum += paragraphs[p].length();
        }

        ASSERT_EQ( sum, dStrlen( texts[i] ) )
            << "The paragraphs of '" << texts[i] << "' hold " << sum << " characters, not " << dStrlen( texts[i] ) << ".";
    }

    SUCCEED();
}

//-----------------------------------------------------------------------------
// The wrapping half is covered by tests/smoke/textEdit.cs, which runs in a real
// engine with a real canvas: it reads the line count off a control that sizes
// itself to its text.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#endif // TORQUE_SHIPPING
