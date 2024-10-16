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

/*! @addtogroup WindowsPlatform Windows Platform
	@ingroup TorqueScriptFunctions
	@{
*/

/*! Executes a process
    @param executable The program to execute
    @param args Arguments to pass to the executable
    @param directory The directory in which the program is located
    @return Returns true on success, false otherwise
*/
ConsoleFunctionWithDocs(shellExecute, ConsoleBool, 2, 4, (executable, [args]?, [directory]?))
{
   ExecuteThread *et = new ExecuteThread(argv[1], argc > 2 ? argv[2] : NULL, argc > 3 ? argv[3] : NULL);
   if(! et->isAlive())
   {
      delete et;
      return false;
   }

   return true;
}

/*! 
    @param executable The program to execute
    @param args Arguments to pass to the executable
    @param directory The directory in which the program is located
    @return Returns true on success, false otherwise
*/
ConsoleFunctionWithDocs(shellExecuteBlocking, int, 2, 6, (executable, [args]?, [directory]?))
{
    const char* executable = argv[1];
    const char* args = argc > 2 ? argv[2] : NULL;
    const char* directory = argc > 3 ? argv[3] : NULL;

    SHELLEXECUTEINFOA shl;
    dMemset(&shl, 0, sizeof(shl));

    shl.cbSize = sizeof(shl);
    shl.fMask = SEE_MASK_NOCLOSEPROCESS;
   
    char exeBuf[1024];
    Platform::makeFullPathName(executable, exeBuf, sizeof(exeBuf));
   
    shl.lpVerb = "open";
    shl.lpFile = exeBuf;
    shl.lpParameters = args;
    shl.lpDirectory = directory;

    shl.nShow = SW_HIDE;

    ShellExecuteExA(&shl);

    if ( shl.hProcess == NULL )
        return false;

    return ( WaitForSingleObject( shl.hProcess, INFINITE) == WAIT_OBJECT_0 );
}

//----------------------------------------------------------------

/*! Opens a folder of the platform
*/
ConsoleFunctionWithDocs(openFolder, void, 2, 2, (folder))
{
    Platform::openFolder(argv[1]);
}

/*! @} */ // end group WindowsPlatform
