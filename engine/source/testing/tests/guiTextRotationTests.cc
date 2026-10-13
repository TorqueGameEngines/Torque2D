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

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif

#ifndef _TAML_H_
#include "persistence/taml/taml.h"
#endif

#include <stdio.h>

//-----------------------------------------------------------------------------
// Where a line of text starts, turned or not
//
// A control's text can be turned a quarter turn either way (textRotation).
// align then runs along the text and the line offset -- what vAlign and the
// lines above it decide -- runs across it, so a label turned counter-clockwise
// with align="left" starts at the bottom-left corner and reads up the left side.
//
// GuiControl::getLineStart is the corner a line is drawn from and the angle it
// is drawn at, given the room's extent and how wide the line is. Which way 90
// degrees actually turns on screen is GL's business and is checked by eye in
// tests/shots/textRotation.cs.
//
// Every case below is an 80x96 box holding a line 40 wide.
//-----------------------------------------------------------------------------

static const Point2I cell( 80, 96 );
static const S32 lineWidth = 40;

static Point2I lineStart( GuiControl::TextRotationOptions rot, AlignmentType align, S32 lineOffset, F32& degrees )
{
    return GuiControl::getLineStart( rot, cell, lineWidth, align, lineOffset, degrees );
}

//-----------------------------------------------------------------------------
// Not turned
//-----------------------------------------------------------------------------

TEST( GuiTextRotationTests, UnturnedTextStartsAtTheTopLeft )
{
    F32 degrees = -1.0f;
    ASSERT_EQ( lineStart( GuiControl::tRotateNone, LeftAlign, 0, degrees ), Point2I( 0, 0 ) );
    ASSERT_FLOAT_EQ( degrees, 0.0f );

    ASSERT_EQ( lineStart( GuiControl::tRotateNone, CenterAlign, 0, degrees ), Point2I( 20, 0 ) );
    ASSERT_EQ( lineStart( GuiControl::tRotateNone, RightAlign, 0, degrees ), Point2I( 40, 0 ) );
    ASSERT_EQ( lineStart( GuiControl::tRotateNone, LeftAlign, 18, degrees ), Point2I( 0, 18 ) ) << "The next line down.";

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Counter-clockwise: reading bottom to top, the tops of the letters to the left
//-----------------------------------------------------------------------------

TEST( GuiTextRotationTests, CounterClockwiseStartsAtTheBottomLeft )
{
    F32 degrees = -1.0f;
    ASSERT_EQ( lineStart( GuiControl::tRotateLeft, LeftAlign, 0, degrees ), Point2I( 0, 96 ) );
    ASSERT_FLOAT_EQ( degrees, 90.0f );

    SUCCEED();
}

TEST( GuiTextRotationTests, CounterClockwiseAlignsAlongTheLeftSide )
{
    F32 degrees;

    // The line runs up from its start, so it covers y = start - 40 to start.
    ASSERT_EQ( lineStart( GuiControl::tRotateLeft, CenterAlign, 0, degrees ), Point2I( 0, 68 ) )
        << "Centered: 28 clear above and below, not pushed off the bottom of the box.";
    ASSERT_EQ( lineStart( GuiControl::tRotateLeft, RightAlign, 0, degrees ), Point2I( 0, 40 ) )
        << "Right-aligned: the end of the line at the top of the box.";

    SUCCEED();
}

TEST( GuiTextRotationTests, CounterClockwiseLinesStackToTheRight )
{
    F32 degrees;
    ASSERT_EQ( lineStart( GuiControl::tRotateLeft, LeftAlign, 18, degrees ), Point2I( 18, 96 ) );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// Clockwise: reading top to bottom, the tops of the letters to the right
//-----------------------------------------------------------------------------

TEST( GuiTextRotationTests, ClockwiseStartsAtTheTopRight )
{
    F32 degrees = -1.0f;
    ASSERT_EQ( lineStart( GuiControl::tRotateRight, LeftAlign, 0, degrees ), Point2I( 80, 0 ) );
    ASSERT_FLOAT_EQ( degrees, -90.0f );

    SUCCEED();
}

TEST( GuiTextRotationTests, ClockwiseAlignsAlongTheRightSide )
{
    F32 degrees;
    ASSERT_EQ( lineStart( GuiControl::tRotateRight, CenterAlign, 0, degrees ), Point2I( 80, 28 ) );
    ASSERT_EQ( lineStart( GuiControl::tRotateRight, RightAlign, 0, degrees ), Point2I( 80, 56 ) );

    SUCCEED();
}

TEST( GuiTextRotationTests, ClockwiseLinesStackToTheLeft )
{
    F32 degrees;
    ASSERT_EQ( lineStart( GuiControl::tRotateRight, LeftAlign, 18, degrees ), Point2I( 62, 0 ) );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// A line too long to align
//-----------------------------------------------------------------------------

TEST( GuiTextRotationTests, ALineLongerThanTheBoxStartsAtItsStartEdge )
{
    // Each line is aligned on its own. A line with no room to align used to keep
    // the offset of the line before it.
    F32 degrees;
    ASSERT_EQ( GuiControl::getLineStart( GuiControl::tRotateNone, cell, 200, CenterAlign, 0, degrees ), Point2I( 0, 0 ) );
    ASSERT_EQ( GuiControl::getLineStart( GuiControl::tRotateLeft, cell, 200, RightAlign, 0, degrees ), Point2I( 0, 96 ) );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// The field
//-----------------------------------------------------------------------------

TEST( GuiTextRotationTests, TextRotationReadsAndWritesByName )
{
    GuiControl* ctrl = new GuiControl();
    ctrl->registerObject();

    StringTableEntry field = StringTable->insert( "textRotation" );
    ASSERT_STREQ( ctrl->getDataField( field, NULL ), "none" ) << "A control is not turned until someone turns it.";

    ctrl->setDataField( field, NULL, "ccw" );
    ASSERT_STREQ( ctrl->getDataField( field, NULL ), "ccw" );

    ctrl->setDataField( field, NULL, "cw" );
    ASSERT_STREQ( ctrl->getDataField( field, NULL ), "cw" );

    ctrl->deleteObject();

    SUCCEED();
}

// The whole of a file, for checking what was and was not written.
static string readWholeFile( const char* fileName )
{
    string text;
    FILE* file = fopen( fileName, "rb" );
    if ( file == NULL )
        return text;
    char buffer[512];
    size_t count;
    while ( ( count = fread( buffer, 1, sizeof( buffer ), file ) ) > 0 )
        text.append( buffer, count );
    fclose( file );
    return text;
}

TEST( GuiTextRotationTests, TextRotationSurvivesTamlAndIsOnlyWrittenWhenSet )
{
    const char* fileName = "unitTestTextRotation.taml";
    Taml taml;

    // An unturned control says nothing about it.
    GuiControl* plain = new GuiControl();
    plain->registerObject();
    ASSERT_TRUE( taml.write( plain, fileName ) );
    plain->deleteObject();
    ASSERT_EQ( readWholeFile( fileName ).find( "textRotation" ), string::npos ) << "none is the default, so it is not written.";

    GuiControl* turned = new GuiControl();
    turned->registerObject();
    turned->setDataField( StringTable->insert( "textRotation" ), NULL, "ccw" );
    ASSERT_TRUE( taml.write( turned, fileName ) );
    turned->deleteObject();
    ASSERT_NE( readWholeFile( fileName ).find( "textRotation=\"ccw\"" ), string::npos );

    GuiControl* loaded = taml.read<GuiControl>( fileName );
    ASSERT_TRUE( loaded != NULL );
    ASSERT_STREQ( loaded->getDataField( StringTable->insert( "textRotation" ), NULL ), "ccw" );

    loaded->deleteObject();
    Platform::fileDelete( fileName );

    SUCCEED();
}

#endif // TORQUE_SHIPPING
