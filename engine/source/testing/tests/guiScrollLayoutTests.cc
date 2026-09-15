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

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

//-----------------------------------------------------------------------------
// How much room a scroll control has to offer a child.
//
// A scroll control is the one container that does not always have a size to
// give. In an axis it can scroll, the content is as long as it wants to be and
// the control is a window onto it -- there is no fixed size to hand down. In an
// axis whose bar is alwaysOff nothing scrolls, the room is bounded, and a child
// may be sized to it.
//
// Getting that wrong is not cosmetic. The palette in the Gui Editor laid its
// tiles out across a width that included the vertical scroll bar, so the last
// column of every row was drawn underneath it, and the names in that column read
// "Number Bo:". It was fixed in script three times -- by narrowing the container,
// by asking for fill, by measuring and subtracting -- and none of them held,
// because none of them were the thing that was wrong.
//
// These tests are the arithmetic, on its own. They construct nothing and need no
// canvas: the two functions are static and take everything they use, which is
// why they were pulled out of computeSizes in the first place.
//-----------------------------------------------------------------------------

static const S32 BarThickness = 14;

//-----------------------------------------------------------------------------
// subtractScrollBars -- what is left once the bars have taken their share.
//
// A vertical bar stands down the side and costs WIDTH; a horizontal one costs
// height. Transposing that pair is the easiest mistake in this file.
//-----------------------------------------------------------------------------

TEST( GuiScrollLayoutTests, NoBarsTakeNothing )
{
    const Point2I room = GuiScrollCtrl::subtractScrollBars( Point2I( 300, 200 ), false, false, BarThickness );

    ASSERT_EQ( room.x, 300 );
    ASSERT_EQ( room.y, 200 );

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AVerticalBarCostsWidth )
{
    const Point2I room = GuiScrollCtrl::subtractScrollBars( Point2I( 300, 200 ), false, true, BarThickness );

    ASSERT_EQ( room.x, 286 ) << "A vertical bar stands down the side; it takes width.";
    ASSERT_EQ( room.y, 200 ) << "It must not take height.";

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AHorizontalBarCostsHeight )
{
    const Point2I room = GuiScrollCtrl::subtractScrollBars( Point2I( 300, 200 ), true, false, BarThickness );

    ASSERT_EQ( room.x, 300 ) << "A horizontal bar must not take width.";
    ASSERT_EQ( room.y, 186 );

    SUCCEED();
}

TEST( GuiScrollLayoutTests, BothBarsTakeBoth )
{
    const Point2I room = GuiScrollCtrl::subtractScrollBars( Point2I( 300, 200 ), true, true, BarThickness );

    ASSERT_EQ( room.x, 286 );
    ASSERT_EQ( room.y, 186 );

    SUCCEED();
}

//-----------------------------------------------------------------------------
// calcBarPresence -- which bars a scroller of this configuration shows.
//
// The content extent passed in is the bar-free one: margins, borders and padding
// already taken off, nothing else.
//-----------------------------------------------------------------------------

// A helper, because every test below wants both answers and C++ has no tuples
// worth the trouble here.
struct BarPresence
{
    bool horizontal;
    bool vertical;

    BarPresence( const S32 forceH, const S32 forceV, const Point2I &child, const Point2I &content )
    {
        GuiScrollCtrl::calcBarPresence( forceH, forceV, child, content, BarThickness, horizontal, vertical );
    }
};

TEST( GuiScrollLayoutTests, AlwaysOffShowsNoBarHoweverBigTheContent )
{
    const BarPresence bars( GuiScrollCtrl::ScrollBarAlwaysOff, GuiScrollCtrl::ScrollBarAlwaysOff,
        Point2I( 9000, 9000 ), Point2I( 300, 200 ) );

    ASSERT_FALSE( bars.horizontal );
    ASSERT_FALSE( bars.vertical ) << "alwaysOff is a promise that the axis does not scroll.";

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AlwaysOnShowsTheBarWithNoContentAtAll )
{
    const BarPresence bars( GuiScrollCtrl::ScrollBarAlwaysOn, GuiScrollCtrl::ScrollBarAlwaysOn,
        Point2I( 0, 0 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.horizontal );
    ASSERT_TRUE( bars.vertical ) << "An always-on bar takes its space whether or not it is needed.";

    SUCCEED();
}

TEST( GuiScrollLayoutTests, DynamicStaysHiddenWhileTheContentFits )
{
    const BarPresence bars( GuiScrollCtrl::ScrollBarDynamic, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 300, 200 ), Point2I( 300, 200 ) );

    ASSERT_FALSE( bars.horizontal ) << "Exactly filling the room is not overflowing it.";
    ASSERT_FALSE( bars.vertical );

    SUCCEED();
}

TEST( GuiScrollLayoutTests, DynamicShowsTheVerticalBarWhenTheContentIsTooTall )
{
    const BarPresence bars( GuiScrollCtrl::ScrollBarAlwaysOff, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 300, 5000 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.vertical );
    ASSERT_FALSE( bars.horizontal ) << "The width still fits, and across is alwaysOff regardless.";

    SUCCEED();
}

// The palette's shape: tall content, no horizontal scrolling, a dynamic vertical
// bar. This is the case the whole bug lived in.
TEST( GuiScrollLayoutTests, TheRoomAcrossIsNarrowedByTheVerticalBar )
{
    const Point2I content( 334, 321 );
    const BarPresence bars( GuiScrollCtrl::ScrollBarAlwaysOff, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 334, 1500 ), content );

    ASSERT_TRUE( bars.vertical );

    const Point2I room = GuiScrollCtrl::subtractScrollBars( content, bars.horizontal, bars.vertical, BarThickness );

    ASSERT_EQ( room.x, 320 ) << "This is the width the palette's groups must lay out in.";
    ASSERT_EQ( room.y, 321 ) << "Nothing takes height here; across is alwaysOff.";

    SUCCEED();
}

// The circular part, and the reason calcBarPresence exists as its own function.
// The old code compared both axes against the un-narrowed extent, so its second
// look at the horizontal bar asked a question it had already answered.
TEST( GuiScrollLayoutTests, AVerticalBarCanCallAHorizontalOneIntoBeing )
{
    // 295 fits across 300, but not across the 286 left once the vertical bar
    // has taken its share.
    const BarPresence bars( GuiScrollCtrl::ScrollBarDynamic, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 295, 5000 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.vertical );
    ASSERT_TRUE( bars.horizontal )
        << "The vertical bar narrowed the content past what the width could hold.";

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AHorizontalBarCanCallAVerticalOneIntoBeing )
{
    // 195 fits down 200, but not down the 186 left once the horizontal bar has.
    const BarPresence bars( GuiScrollCtrl::ScrollBarDynamic, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 5000, 195 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.horizontal );
    ASSERT_TRUE( bars.vertical );

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AnAlwaysOnVerticalBarNarrowsTheRoomTheHorizontalOneJudges )
{
    // Nothing overflows 300 across, but the always-on vertical bar leaves 286.
    const BarPresence bars( GuiScrollCtrl::ScrollBarDynamic, GuiScrollCtrl::ScrollBarAlwaysOn,
        Point2I( 295, 50 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.vertical );
    ASSERT_TRUE( bars.horizontal )
        << "An always-on bar takes its space before the other axis is judged.";

    SUCCEED();
}

TEST( GuiScrollLayoutTests, AnAlwaysOffAxisIsNeverCalledIntoBeing )
{
    // Wide enough to overflow twice over, but across cannot scroll.
    const BarPresence bars( GuiScrollCtrl::ScrollBarAlwaysOff, GuiScrollCtrl::ScrollBarDynamic,
        Point2I( 5000, 5000 ), Point2I( 300, 200 ) );

    ASSERT_TRUE( bars.vertical );
    ASSERT_FALSE( bars.horizontal ) << "alwaysOff holds even when the content overflows it.";

    SUCCEED();
}

//-----------------------------------------------------------------------------
// A scroller built the way a script builds one.
//
// Which bars are showing is worked out by computeSizes and kept, and the room a
// child is offered is read from what was kept. The constructor starts both bar
// modes at alwaysOn, and a new{} block that writes Extent before hScrollBar and
// vScrollBar has computeSizes run -- through the Extent setter -- while they are
// still alwaysOn. Writing the modes afterwards used to recompute nothing, so the
// scroller went on believing it had two bars it did not have.
//
// A child added then measured itself against a rect two bars too small. For a
// scaled child that is not forgotten: it records its proportion of the parent
// once, against that rect, and when the bars were found not to exist after all
// it grew by the same proportion -- past the room, which really did call the
// bars into being, for good. Pirate Code 2's menu background came out 1290x730
// in a 1280x720 scroller with 10px bars, with both bars on at every window size.
//
// These build real controls, but nothing is woken and nothing measures text, so
// they need no canvas.
//-----------------------------------------------------------------------------

static StringTableEntry scrollField( const char* name )
{
    return StringTable->insert( name );
}

// Every field written before the object is registered and in this order, which
// is what a new{} block does. The order is the point: Extent before the modes.
static GuiScrollCtrl* makeScriptedScroller( const char* barMode )
{
    GuiScrollCtrl* scroll = new GuiScrollCtrl();
    scroll->setDataField( scrollField( "profile" ), NULL, "GuiDefaultProfile" );
    scroll->setDataField( scrollField( "HorizSizing" ), NULL, "relative" );
    scroll->setDataField( scrollField( "VertSizing" ), NULL, "relative" );
    scroll->setDataField( scrollField( "Position" ), NULL, "0 0" );
    scroll->setDataField( scrollField( "Extent" ), NULL, "1280 720" );
    scroll->setDataField( scrollField( "ScrollBarThickness" ), NULL, "10" );
    scroll->setDataField( scrollField( "hScrollBar" ), NULL, barMode );
    scroll->setDataField( scrollField( "vScrollBar" ), NULL, barMode );
    scroll->registerObject();
    return scroll;
}

// Exactly the size of the scroller, and scaled, so any error in the room it is
// offered comes back multiplied into its size.
static GuiControl* makeScaledChild()
{
    GuiControl* child = new GuiControl();
    child->setDataField( scrollField( "profile" ), NULL, "GuiDefaultProfile" );
    child->setDataField( scrollField( "HorizSizing" ), NULL, "relative" );
    child->setDataField( scrollField( "VertSizing" ), NULL, "relative" );
    child->setDataField( scrollField( "Position" ), NULL, "0 0" );
    child->setDataField( scrollField( "Extent" ), NULL, "1280 720" );
    child->setDataField( scrollField( "MinExtent" ), NULL, "1024 640" );
    child->registerObject();
    return child;
}

TEST( GuiScrollLayoutTests, AScaledChildThatFitsKeepsItsSize )
{
    GuiScrollCtrl* scroll = makeScriptedScroller( "dynamic" );
    GuiControl* child = makeScaledChild();
    scroll->addObject( child );

    EXPECT_EQ( child->getExtent().x, 1280 ) << "1290 is 1280/1270 of 1280: a proportion taken with a 10px bar that was never there.";
    EXPECT_EQ( child->getExtent().y, 720 );
    EXPECT_FALSE( scroll->hasHScrollBar() ) << "The child fits exactly, so a dynamic bar has nothing to do.";
    EXPECT_FALSE( scroll->hasVScrollBar() );

    scroll->deleteObject();
}

// Not a bar that turned out to be needed: an alwaysOff scroller can never have
// one, and still gave the child the wrong room.
TEST( GuiScrollLayoutTests, AnAlwaysOffScrollerTakesNoRoomFromAChild )
{
    GuiScrollCtrl* scroll = makeScriptedScroller( "alwaysOff" );
    GuiControl* child = makeScaledChild();
    scroll->addObject( child );

    EXPECT_EQ( child->getExtent().x, 1280 );
    EXPECT_EQ( child->getExtent().y, 720 );
    EXPECT_FALSE( scroll->hasHScrollBar() );
    EXPECT_FALSE( scroll->hasVScrollBar() );

    scroll->deleteObject();
}

// The same staleness from the other side: a scroller whose mode is changed
// after it has children. The bar has to come and go when the field is written,
// not at whatever resize happens to come next.
TEST( GuiScrollLayoutTests, ABarModeTakesEffectWhenItIsWritten )
{
    GuiScrollCtrl* scroll = makeScriptedScroller( "dynamic" );
    GuiControl* child = makeScaledChild();
    scroll->addObject( child );
    ASSERT_FALSE( scroll->hasVScrollBar() );

    scroll->setDataField( scrollField( "vScrollBar" ), NULL, "alwaysOn" );
    EXPECT_TRUE( scroll->hasVScrollBar() );

    scroll->setDataField( scrollField( "vScrollBar" ), NULL, "dynamic" );
    EXPECT_FALSE( scroll->hasVScrollBar() );

    scroll->deleteObject();
}

#endif // TORQUE_SHIPPING
