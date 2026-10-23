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

#endif // TORQUE_SHIPPING
