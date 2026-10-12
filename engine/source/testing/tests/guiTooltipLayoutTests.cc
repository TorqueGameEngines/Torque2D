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

//-----------------------------------------------------------------------------
// Where a tooltip goes, and where its text starts
//
// A tooltip used to be its text plus a fixed 5px gutter on every side, whatever
// its profile said, so a profile drawing a frame wider than 5px -- the 9-slice
// art of a game's tooltip, or the stock theme's 1px rim and 10px padding --
// had the text drawn over it. The box is now the text plus the profile's margin,
// border and padding on each side, never less than the old gutter, and the text
// starts inside all three.
//
// GuiControl::layoutTooltip is that arithmetic on its own: measuring the text
// needs a font, and a font needs a GL context these tests do not have.
//-----------------------------------------------------------------------------

static const Point2I bigScreen( 1920, 1080 );

static GuiBorderProfile* newBorder( const char* margin, const char* border, const char* padding )
{
    GuiBorderProfile* side = new GuiBorderProfile();
    side->registerObject();
    side->setDataField( StringTable->insert( "margin" ), NULL, margin );
    side->setDataField( StringTable->insert( "border" ), NULL, border );
    side->setDataField( StringTable->insert( "padding" ), NULL, padding );
    return side;
}

static GuiControlProfile* newProfile( GuiBorderProfile* left, GuiBorderProfile* right, GuiBorderProfile* top, GuiBorderProfile* bottom )
{
    GuiControlProfile* profile = new GuiControlProfile();
    profile->registerObject();
    profile->setLeftProfile( left );
    profile->setRightProfile( right );
    profile->setTopProfile( top );
    profile->setBottomProfile( bottom );
    return profile;
}

TEST( GuiTooltipLayoutTests, AnUnborderedProfileKeepsTheFivePixelGutter )
{
    GuiBorderProfile* none = newBorder( "0", "0", "0" );
    GuiControlProfile* profile = newProfile( none, none, none, none );

    Point2I textOrigin;
    const RectI box = GuiControl::layoutTooltip( Point2I( 100, 20 ), Point2I( 50, 50 ), bigScreen, profile, textOrigin );

    ASSERT_EQ( box.point, Point2I( 50, 72 ) ) << "Just below the cursor, as it always was.";
    ASSERT_EQ( box.extent, Point2I( 110, 30 ) ) << "A plain tooltip looks exactly as it did.";
    ASSERT_EQ( textOrigin, Point2I( 55, 77 ) );

    profile->deleteObject();
    none->deleteObject();

    SUCCEED();
}

TEST( GuiTooltipLayoutTests, MarginBorderAndPaddingAreAllRoomAroundTheText )
{
    GuiBorderProfile* frame = newBorder( "3", "2", "20" );
    GuiControlProfile* profile = newProfile( frame, frame, frame, frame );

    Point2I textOrigin;
    const RectI box = GuiControl::layoutTooltip( Point2I( 100, 20 ), Point2I( 50, 50 ), bigScreen, profile, textOrigin );

    ASSERT_EQ( box.extent, Point2I( 150, 70 ) ) << "25px a side, not the 5px gutter.";
    ASSERT_EQ( textOrigin, box.point + Point2I( 25, 25 ) ) << "The text starts clear of the frame.";

    profile->deleteObject();
    frame->deleteObject();

    SUCCEED();
}

TEST( GuiTooltipLayoutTests, EachSideTakesItsOwnBorder )
{
    GuiBorderProfile* wide = newBorder( "0", "0", "20" );
    GuiBorderProfile* thin = newBorder( "0", "1", "0" );
    GuiControlProfile* profile = newProfile( wide, thin, thin, wide );

    Point2I textOrigin;
    const RectI box = GuiControl::layoutTooltip( Point2I( 100, 20 ), Point2I( 50, 50 ), bigScreen, profile, textOrigin );

    // Left and bottom pad 20; right and top are a 1px rim, which the gutter
    // still keeps the text 5px from.
    ASSERT_EQ( box.extent, Point2I( 125, 45 ) );
    ASSERT_EQ( textOrigin, box.point + Point2I( 20, 5 ) );

    profile->deleteObject();
    wide->deleteObject();
    thin->deleteObject();

    SUCCEED();
}

TEST( GuiTooltipLayoutTests, TheWholeBoxIsKeptOnScreen )
{
    GuiBorderProfile* frame = newBorder( "0", "0", "20" );
    GuiControlProfile* profile = newProfile( frame, frame, frame, frame );

    // A cursor near the bottom-right corner. Kept on screen by the size of the
    // text plus the old gutter, a padded box hung off both edges.
    Point2I textOrigin;
    const RectI box = GuiControl::layoutTooltip( Point2I( 100, 20 ), Point2I( 1900, 1070 ), bigScreen, profile, textOrigin );

    ASSERT_EQ( box.point.x + box.extent.x, bigScreen.x );
    ASSERT_EQ( box.point.y + box.extent.y, bigScreen.y );
    ASSERT_EQ( textOrigin, box.point + Point2I( 20, 20 ) ) << "The text moves with the box.";

    profile->deleteObject();
    frame->deleteObject();

    SUCCEED();
}

#endif // TORQUE_SHIPPING
