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

#include <string>
#include "winVersion.h"

/*! @addtogroup WindowsPlatform Windows Platform
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Use the enableWinConsole function to tell TGB to create an external console window, either as a separate DOS window or as a new window under OSX/Linux/*NIX.
    Subsequent calls to this function do nothing. Only one external console is allowed
    @param enable A boolean. If this value is set to true, a new console window will be created.
    @return No return value.
*/
ConsoleFunctionWithDocs(enableWinConsole, ConsoleVoid, 2, 2, ( enable ))
{
   argc;
   WindowsConsole->enable(dAtob(argv[1]));
}

ConsoleFunctionWithDocs(getWinNTVersion, ConsoleVoid, 1, 1, ())
{
	std::string ver = getWinNTVersion();
	Con::printf("%s", ver.c_str());
}

/*! @} */ // end group WindowsPlatform
