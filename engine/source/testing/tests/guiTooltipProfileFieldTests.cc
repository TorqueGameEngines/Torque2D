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

#ifndef _TAML_H_
#include "persistence/taml/taml.h"
#endif

#include <stdio.h>

//-----------------------------------------------------------------------------
// The tooltipprofile field. A control with no tooltip profile of its own reads
// back as "", and that has to mean "none" when it is written again, so the
// control keeps finding its theme's tooltip profile when it draws a tip. A
// deep clone copies every field this way, and so does a TAML round trip.
//-----------------------------------------------------------------------------

static StringTableEntry tooltipField()
{
    return StringTable->insert( "tooltipprofile" );
}

static GuiControl* makeTooltipControl()
{
    GuiControl* ctrl = new GuiControl();
    ctrl->registerObject();
    return ctrl;
}

TEST( GuiTooltipProfileFieldTests, AControlStartsWithNoTooltipProfile )
{
    GuiControl* ctrl = makeTooltipControl();
    ASSERT_STREQ( "", ctrl->getDataField( tooltipField(), NULL ) );
    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, AnEmptyTooltipProfileMeansNone )
{
    GuiControl* ctrl = makeTooltipControl();
    ctrl->setDataField( tooltipField(), NULL, "" );
    ASSERT_STREQ( "", ctrl->getDataField( tooltipField(), NULL ) );
    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, AnEmptyTooltipProfileClearsANamedOne )
{
    GuiControl* ctrl = makeTooltipControl();
    ctrl->setDataField( tooltipField(), NULL, "GuiDefaultProfile" );
    ASSERT_STREQ( "GuiDefaultProfile", ctrl->getDataField( tooltipField(), NULL ) );
    ctrl->setDataField( tooltipField(), NULL, "" );
    ASSERT_STREQ( "", ctrl->getDataField( tooltipField(), NULL ) );
    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, ACloneOfAControlWithNoTooltipProfileHasNone )
{
    GuiControl* ctrl = makeTooltipControl();
    SimObject* clone = ctrl->deepClone();
    ASSERT_TRUE( clone != NULL );
    ASSERT_STREQ( "", clone->getDataField( tooltipField(), NULL ) );
    clone->deleteObject();
    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, ACloneKeepsANamedTooltipProfile )
{
    GuiControl* ctrl = makeTooltipControl();
    ctrl->setDataField( tooltipField(), NULL, "GuiDefaultProfile" );
    SimObject* clone = ctrl->deepClone();
    ASSERT_TRUE( clone != NULL );
    ASSERT_STREQ( "GuiDefaultProfile", clone->getDataField( tooltipField(), NULL ) );
    clone->deleteObject();
    ctrl->deleteObject();
}

//-----------------------------------------------------------------------------
// A tooltip profile the control picked for itself. renderTooltip gives a
// control that has none the Tooltip member of its theme the first time it draws
// a tip, and keeps re-picking as the control is re-themed. That pick is not the
// control's own, so the field does not report it: a copy or a save of a control
// that has shown a tip used to carry the pick over as a deliberate profile,
// pinned to that theme for good.
//-----------------------------------------------------------------------------

// What renderTooltip leaves behind when it picks, which needs a canvas to do
// for real.
static GuiControl* makePickedTooltipControl( GuiControlProfile*& picked )
{
    picked = dynamic_cast<GuiControlProfile*>( Sim::findObject( "GuiDefaultProfile" ) );
    GuiControl* ctrl = makeTooltipControl();
    ctrl->mTooltipProfile = picked;
    ctrl->mLazyTooltipProfile = picked;
    return ctrl;
}

TEST( GuiTooltipProfileFieldTests, APickedTooltipProfileIsNotTheControlsOwn )
{
    GuiControlProfile* picked;
    GuiControl* ctrl = makePickedTooltipControl( picked );
    ASSERT_TRUE( picked != NULL );

    ASSERT_STREQ( "", ctrl->getDataField( tooltipField(), NULL ) );

    // Which profile a tip will draw with is still there to ask for.
    ASSERT_STREQ( "GuiDefaultProfile", Con::evaluatef( "return %d.getActiveTooltipProfile();", ctrl->getId() ) );

    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, ACloneOfAControlThatPickedOneHasNone )
{
    GuiControlProfile* picked;
    GuiControl* ctrl = makePickedTooltipControl( picked );

    SimObject* clone = ctrl->deepClone();
    ASSERT_TRUE( clone != NULL );
    ASSERT_TRUE( static_cast<GuiControl*>( clone )->mTooltipProfile == NULL )
        << "The copy picks for itself, from whatever theme it is wearing.";

    clone->deleteObject();
    ctrl->deleteObject();
}

TEST( GuiTooltipProfileFieldTests, AControlThatPickedOneSavesNone )
{
    const char* fileName = "unitTestPickedTooltip.taml";

    GuiControlProfile* picked;
    GuiControl* ctrl = makePickedTooltipControl( picked );

    Taml taml;
    ASSERT_TRUE( taml.write( ctrl, fileName ) );
    ctrl->deleteObject();

    FILE* file = fopen( fileName, "rb" );
    ASSERT_TRUE( file != NULL );
    char text[4096];
    const size_t length = fread( text, 1, sizeof( text ) - 1, file );
    text[length] = '\0';
    fclose( file );
    Platform::fileDelete( fileName );

    ASSERT_TRUE( dStrstr( (const char*)text, (const char*)"tooltipprofile" ) == NULL ) << text;
}

TEST( GuiTooltipProfileFieldTests, ChoosingThePickedProfileMakesItTheControlsOwn )
{
    GuiControlProfile* picked;
    GuiControl* ctrl = makePickedTooltipControl( picked );

    ctrl->setDataField( tooltipField(), NULL, "GuiDefaultProfile" );
    ASSERT_STREQ( "GuiDefaultProfile", ctrl->getDataField( tooltipField(), NULL ) )
        << "Set on purpose, it is a deliberate profile even though it is the same one.";

    ctrl->deleteObject();
}

#endif // TORQUE_SHIPPING
