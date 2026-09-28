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

#ifndef _EDITORTOYTOOL_H_
#include "2d/editorToy/EditorToyTool.h"
#endif

//-----------------------------------------------------------------------------
// A key press, as script hears it
//
// Con::executef takes its arguments as strings. The key and its modifiers used
// to go in as bare integers, which the console read as pointers: any tool whose
// class defined onKeyPress crashed the engine on the first key.
//-----------------------------------------------------------------------------

TEST( EditorToyToolTests, KeyPressReachesScriptAsNumbers )
{
    Con::evaluate( "function UnitTestKeyTool::onKeyPress( %this, %ascii, %modifier ) { $UnitTestKeyAscii = %ascii; $UnitTestKeyModifier = %modifier; }" );
    Con::setVariable( "$UnitTestKeyAscii", "unset" );
    Con::setVariable( "$UnitTestKeyModifier", "unset" );

    const char* id = Con::evaluate( "return new EditorToyTool() { class = \"UnitTestKeyTool\"; };" );
    EditorToyTool* tool = dynamic_cast<EditorToyTool*>( Sim::findObject( id ) );
    ASSERT_TRUE( tool != NULL );

    GuiEvent event;
    dMemset( &event, 0, sizeof( event ) );
    event.ascii = 'a';
    event.modifier = 4;

    tool->onInputEvent( event );

    ASSERT_STREQ( Con::getVariable( "$UnitTestKeyAscii" ), "97" );
    ASSERT_STREQ( Con::getVariable( "$UnitTestKeyModifier" ), "4" );

    tool->deleteObject();

    SUCCEED();
}

#endif // TORQUE_SHIPPING
