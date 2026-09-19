//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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
#include <unistd.h>
#include "platform/platform.h"
#include "console/console.h"
#include "string/stringTable.h"
#include "platform/platformInput.h"
#include "platform/threads/thread.h"

//-----------------------------------------------------------------------------
const char* Platform::getUserDataDirectory() 
{
	return StringTable->insert("/.data");
}

//-----------------------------------------------------------------------------
const char* Platform::getUserHomeDirectory() 
{
	return StringTable->insert("/home");
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::osGetTemporaryDirectory()
{
	return StringTable->insert("/tmp");
}

//-----------------------------------------------------------------------------
S32 Platform::messageBox(const UTF8 *title, const UTF8 *message, MBButtons buttons, MBIcons icon)
{
    Platform::AlertOK( title, message );
    return 1;
}

ConsoleFunction(shellExecute, bool, 2, 4, "(executable, [args], [directory])")
{
   return true;
}


void Input::setCursorShape(U32 cursorID)
{
    //no cursors on Android except Torque cursors
}

// Input::setCursorState is EmscriptenInput.cpp's.

