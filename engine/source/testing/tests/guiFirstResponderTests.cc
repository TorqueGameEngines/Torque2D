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
// Where the canvas may send a key.
//
// The canvas sends every key to its first responder, and it never asked
// whether that control could still take one. A control that went to sleep or
// out of sight without letting go of the keyboard went on being sent keys --
// on Windows, where a key-down carries its character, a text box nobody could
// see went on being typed into. canTakeKeyboard is what the canvas asks now.
//
// Nothing here is woken: waking a control loads its profile's font, which
// registers a texture, which this suite cannot. The flags are set directly.
//-----------------------------------------------------------------------------

class FocusProbeCtrl : public GuiControl
{
public:
    using GuiControl::mAwake;
    using GuiControl::mVisible;
};

// A child in a parent, both put back to sleep and deleted however the test
// ends: removeObject asserts that a child is exactly as awake as its parent.
class ProbePair
{
public:
    ProbePair()
    {
        mParent = new FocusProbeCtrl();
        mParent->registerObject();
        mChild = new FocusProbeCtrl();
        mChild->registerObject();
        mParent->addObject( mChild );
    }
    ~ProbePair()
    {
        mChild->mAwake = false;
        mParent->mAwake = false;
        mParent->deleteObject();
    }

    FocusProbeCtrl* mParent;
    FocusProbeCtrl* mChild;

private:
    ProbePair( const ProbePair& );
    ProbePair& operator=( const ProbePair& );
};

TEST( GuiFirstResponderTests, AControlAsleepTakesNoKeys )
{
    ProbePair pair;

    ASSERT_FALSE( pair.mChild->canTakeKeyboard() ) << "Never woken.";

    pair.mChild->mAwake = true;
    ASSERT_TRUE( pair.mChild->canTakeKeyboard() );

    SUCCEED();
}

TEST( GuiFirstResponderTests, AHiddenControlTakesNoKeys )
{
    ProbePair pair;
    pair.mChild->mAwake = true;

    pair.mChild->mVisible = false;
    ASSERT_FALSE( pair.mChild->canTakeKeyboard() );

    pair.mChild->mVisible = true;
    ASSERT_TRUE( pair.mChild->canTakeKeyboard() );

    SUCCEED();
}

TEST( GuiFirstResponderTests, NorDoesOneInsideAHiddenParent )
{
    // A text box on a page of a tab book that has been hidden: the box's own
    // flag still says visible.
    ProbePair pair;
    pair.mChild->mAwake = true;

    pair.mParent->mVisible = false;
    ASSERT_FALSE( pair.mChild->canTakeKeyboard() );

    pair.mParent->mVisible = true;
    ASSERT_TRUE( pair.mChild->canTakeKeyboard() );

    SUCCEED();
}

#endif // TORQUE_SHIPPING
