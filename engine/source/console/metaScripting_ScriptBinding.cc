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

#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/resource/resourceManager.h"
#include "io/fileStream.h"
#include "console/compiler.h"

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

// Buffer for expanding script filenames.
static char pathBuffer[1024];

static U32 execDepth = 0;

extern S32 QSORT_CALLBACK ACRCompare(const void *aptr, const void *bptr);

ConsoleFunctionGroupBegin(MetaScripting, "Functions that let you manipulate the scripting engine programmatically.");

/*! @defgroup MetaScriptingFunctions Meta-Scripting
		@ingroup TorqueScriptFunctions
		@{
*/

/*! Use the call function to dynamically build and call a function.
    @param funcName A string containing the unadorned name of a function to be executed.
    @param args .. - Any arguments that should be passed to the function.
    @return Returns a string containing the results from the function that is built and called.
    @sa eval
*/
ConsoleFunctionWithDocs(call, ConsoleString, 2, 0, ( funcName, [args ... ]?))
{
   return Con::execute(argc - 1, argv + 1);
}

static StringTableEntry getDSOPath(const char *scriptPath)
{
   const char *slash = dStrrchr(scriptPath, '/');
   return StringTable->insertn(scriptPath, (U32)(slash - scriptPath), true);
}

/*! Returns the DSO path of the given filename
    @param scriptFileName A string contains the file to search for
    @return Returns either the requested pathname as a string, or an empty string if not found.
*/
ConsoleFunctionWithDocs(getDSOPath, ConsoleString, 2, 2, (scriptFileName))
{
   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);  

   const char *filename = getDSOPath(pathBuffer);
   if(filename == NULL || *filename == 0)
      return "";

   return filename;
}

/*! Use the compile function to pre-compile a script file without executing the contents.
    @param fileName A path to the script to compile.
    @return Returns 1 if the script compiled without errors and 0 if the file did not compile correctly or if the path is wrong. Also, ff the path is invalid, an error will print to the console.
    @sa exec
*/
ConsoleFunctionWithDocs(compile, ConsoleBool, 2, 2, ( fileName ))
{
   TORQUE_UNUSED( argc );
   char nameBuffer[512];
   char* script = NULL;
   U32 scriptSize = 0;

   FileTime comModifyTime, scrModifyTime;

   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(pathBuffer);

   // If the script file extention is '.ed.cs' then compile it to a different compiled extention
   bool isEditorScript = false;
   const char *ext = dStrrchr( pathBuffer, '.' );
   if( ext && ( dStricmp( ext, ".cs" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( ext && ( dStricmp( ext, ".gui" ) == 0 ) )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }

   const char *filenameOnly = dStrrchr(pathBuffer, '/');
   if(filenameOnly)
      ++filenameOnly;
   else
      filenameOnly = pathBuffer;

   if( isEditorScript )
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, ".edso", NULL);
   else
      dStrcpyl(nameBuffer, sizeof(nameBuffer), dsoPath, "/", filenameOnly, ".dso", NULL);
   
   ResourceObject *rScr = ResourceManager->find(pathBuffer);
   ResourceObject *rCom = ResourceManager->find(nameBuffer);

   if(rCom)
      rCom->getFileTimes(NULL, &comModifyTime);
   if(rScr)
      rScr->getFileTimes(NULL, &scrModifyTime);

   Stream *s = ResourceManager->openStream(pathBuffer);
   if(s)
   {
      scriptSize = ResourceManager->getSize(pathBuffer);
      script = new char [scriptSize+1];
      s->read(scriptSize, script);
      ResourceManager->closeStream(s);
      script[scriptSize] = 0;
   }

   if (!scriptSize || !script)
   {
      delete [] script;
      Con::errorf(ConsoleLogEntry::Script, "compile: invalid script file %s.", pathBuffer);
      return false;
   }
   // compile this baddie.
// -Mat reducing console noise
#if defined(TORQUE_DEBUG)
   Con::printf("Compiling %s...", pathBuffer);
#endif
   CodeBlock *code = new CodeBlock();
   code->compile(nameBuffer, pathBuffer, script);
   delete code;
   code = NULL;

   delete[] script;
   return true;
}

/*! 
*/
ConsoleFunctionWithDocs(compilePath, ConsoleString, 2, 2, ( path ))
{
    if ( !Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]) )
        return "-1 0";
    
    const char *compileArgs[2] = { "compile", NULL };
    
    S32 failedScripts = 0;
    S32 totalScripts = 0;
    ResourceObject *match = NULL;
    
    while ( (match = ResourceManager->findMatch( pathBuffer, &compileArgs[1], match )) )
    {
        if ( !ccompile( NULL, 1, compileArgs ) )
            failedScripts++;
        
        totalScripts++;
    }
    
    char* result = Con::getReturnBuffer(32);
    dSprintf( result, 32, "%d %d", failedScripts, totalScripts );
   return result;
}

static bool scriptExecutionEcho = false;
/*! Whether to echo script file execution or not.
*/
ConsoleFunctionWithDocs(setScriptExecEcho, ConsoleVoid, 2, 2, (echo?))
{
    scriptExecutionEcho = dAtob(argv[1]);
}

/*! Use the exec function to compile and execute a normal script, or a special journal script.
    If $Pref::ignoreDSOs is set to true, the system will use .cs before a .dso file if both are found.
    @param fileName A string containing a path to the script to be compiled and executed.
    @param nocalls A boolean value. If this value is set to true, then all function calls encountered while executing the script file will be skipped and not called. This allows us to re-define function definitions found in a script file, without re-executing other worker scripts in the same file.
    @param journalScript A boolean value. If this value is set tot true, and if a journal is being played, the engine will attempt to read this script from the journal stream. If no journal is playing, this field is ignored.
    @return Returns true if the file compiled and executed w/o errors, false otherwise.
    @sa compile
*/
ConsoleFunctionWithDocs(exec, ConsoleBool, 2, 4, ( fileName, [nocalls]?, [journalScript ]?))
{
   execDepth++;

#ifdef TORQUE_ALLOW_JOURNALING
   bool journal = false;

   if(journalDepth >= execDepth)
      journalDepth = execDepth + 1;
   else
      journal = true;
#endif //TORQUE_ALLOW_JOURNALING

   bool noCalls = false;
   bool ret = false;

   if(argc >= 3 && dAtoi(argv[2]))
      noCalls = true;

#ifdef TORQUE_ALLOW_JOURNALING
   if(argc >= 4 && dAtoi(argv[3]) && !journal)
   {
      journal = true;
      journalDepth = execDepth;
   }
#endif //TORQUE_ALLOW_JOURNALING

   // Determine the filename we actually want...
   Con::expandPath(pathBuffer, sizeof(pathBuffer), argv[1]);

   // Figure out where to put DSOs
   StringTableEntry dsoPath = getDSOPath(pathBuffer);

   const char *ext = dStrrchr(pathBuffer, '.');

   if(!ext)
   {
      // We need an extension!
      Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file name %s.", pathBuffer);
      execDepth--;
      return false;
   }

   // Check Editor Extensions
   bool isEditorScript = false;

#ifdef TORQUE_ALLOW_DSO_GENERATION
   // If the script file extension is '.ed.cs' then compile it to a different compiled extension
   if( dStricmp( ext, ".cs" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.cs" ) == 0 )
         isEditorScript = true;
   }
   else if( dStricmp( ext, ".gui" ) == 0 )
   {
      const char* ext2 = ext - 3;
      if( dStricmp( ext2, ".ed.gui" ) == 0 )
         isEditorScript = true;
   }
#endif //TORQUE_ALLOW_DSO_GENERATION

   // rdbhack: if we can't find the script file in the game directory, look for it
   //   in the Application Data directory. This makes it possible to keep the user
   //   ignorant of where the files are actually saving to, thus eliminating the need
   //   for the script functions: execPrefs, getUserDataDirectory, etc.
   //
   //   This works because we know that script files located in the prefs path will 
   //   not have compiled versions (it checks for this further down). Otherwise this
   //   would be a big problem!
   
   StringTableEntry scriptFileName = StringTable->EmptyString;


//Luma : This is redundant, we wont be building dso's on the device - 
//plus saving dso to the user directory when attempting build for the
//release tests on iPhone is irrelevant.
#ifdef TORQUE_ALLOW_DSO_GENERATION

   if(!ResourceManager->find(pathBuffer))
   {
      // NOTE: this code is pretty much a duplication of code much further down in this
      //       function...

      // our work just got a little harder.. if we couldn't find the .cs, then we need to
      // also look for the .dso BEFORE we can try the prefs path.. UGH
      const char *filenameOnly = dStrrchr(pathBuffer, '/');
      if(filenameOnly)
         ++filenameOnly;
      else
         filenameOnly = pathBuffer;

      // we could skip this step and rid ourselves of a bunch of nonsense but we can't be
      // certain the dso path is the same as the path given to use in scriptFileNameBuffer
      char pathAndFilename[1024]; 
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), dsoPath);

      char nameBuffer[1024];
      if( isEditorScript ) // this should never be the case since we are a PLAYER not a TOOL, but you never know
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".edso", NULL);
      else
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".dso", NULL);

      if(!ResourceManager->find(nameBuffer))
         scriptFileName = Platform::getPrefsPath(Platform::stripBasePath(pathBuffer));
      else
         scriptFileName = StringTable->insert(pathBuffer);
   }
   else
      scriptFileName = StringTable->insert(pathBuffer);
#else //TORQUE_ALLOW_DSO_GENERATION

    //Luma : Just insert the file name.
    scriptFileName = StringTable->insert(pathBuffer);

#endif //TORQUE_ALLOW_DSO_GENERATION

    //Luma : Something screwed up so get out early
   if(scriptFileName == NULL || *scriptFileName == 0)
   {
      execDepth--;
      return false;
   }

#ifdef TORQUE_ALLOW_JOURNALING
   
   bool compiled = dStricmp(ext, ".mis") && !journal && !Con::getBoolVariable("Scripts::ignoreDSOs");
#else
   bool compiled = dStricmp(ext, ".mis")  && !Con::getBoolVariable("Scripts::ignoreDSOs");
#endif //TORQUE_ALLOW_JOURNALING

   // [tom, 12/5/2006] stripBasePath() messes up if the filename is not in the exe
   // path, current directory or prefs path. Thus, getDSOFilename() will also mess
   // up and so this allows the scripts to still load but without a DSO.
   if(Platform::isFullPath(Platform::stripBasePath(pathBuffer)))
      compiled = false;

   // [tom, 11/17/2006] It seems to make sense to not compile scripts that are in the
   // prefs directory. However, getDSOPath() can handle this situation and will put
   // the dso along with the script to avoid name clashes with tools/game dsos.
#ifdef TORQUE_ALLOW_DSO_GENERATION
    // Is this a file we should compile? (anything in the prefs path should not be compiled)
    StringTableEntry prefsPath = Platform::getPrefsPath();

   if( dStrlen(prefsPath) > 0 && dStrnicmp(scriptFileName, prefsPath, dStrlen(prefsPath)) == 0)
      compiled = false;
#endif //TORQUE_ALLOW_DSO_GENERATION

   // If we're in a journaling mode, then we will read the script
   // from the journal file.
#ifdef TORQUE_ALLOW_JOURNALING
   if(journal && Game->isJournalReading())
   {
      char fileNameBuf[256];
      bool fileRead;
      U32 fileSize;

      Game->getJournalStream()->readString(fileNameBuf);
      Game->getJournalStream()->read(&fileRead);
      if(!fileRead)
      {
         Con::errorf(ConsoleLogEntry::Script, "Journal script read (failed) for %s", fileNameBuf);
         execDepth--;
         return false;
      }
      Game->journalRead(&fileSize);
      char *script = new char[fileSize + 1];
      Game->journalRead(fileSize, script);
      script[fileSize] = 0;
      Con::printf("Executing (journal-read) %s.", scriptFileName);
      CodeBlock *newCodeBlock = new CodeBlock();
      newCodeBlock->compileExec(scriptFileName, script, noCalls, 0);
      delete [] script;

      execDepth--;
      return true;
   }
#endif //TORQUE_ALLOW_JOURNALING

   // Ok, we let's try to load and compile the script.
   ResourceObject *rScr = ResourceManager->find(scriptFileName);
   ResourceObject *rCom = NULL;

   char nameBuffer[512];
   char* script = NULL;
   U32 scriptSize = 0;
   U32 version;

   Stream *compiledStream = NULL;
   FileTime comModifyTime, scrModifyTime;

   // Check here for .edso
   //bool edso = false;
   //if( dStricmp( ext, ".edso" ) == 0  && rScr )
   //{
   //   edso = true;
   //   rCom = rScr;
   //   rScr = NULL;

   //   rCom->getFileTimes( NULL, &comModifyTime );
   //   dStrcpy( nameBuffer, scriptFileName );
   //}

   // If we're supposed to be compiling this file, check to see if there's a DSO
   if(compiled /*&& !edso*/)
   {
      const char *filenameOnly = dStrrchr(scriptFileName, '/');
      if(filenameOnly)
         ++filenameOnly; //remove the / at the front
      else
         filenameOnly = scriptFileName;

      char pathAndFilename[1024];
      Platform::makeFullPathName(filenameOnly, pathAndFilename, sizeof(pathAndFilename), dsoPath);

      if( isEditorScript )
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".edso", NULL);
      else
         dStrcpyl(nameBuffer, sizeof(nameBuffer), pathAndFilename, ".dso", NULL);

      rCom = ResourceManager->find(nameBuffer);

      if(rCom)
         rCom->getFileTimes(NULL, &comModifyTime);
      if(rScr)
         rScr->getFileTimes(NULL, &scrModifyTime);
   }

   // Let's do a sanity check to complain about DSOs in the future.
   //
   // MM:	This doesn't seem to be working correctly for now so let's just not issue
   //		the warning until someone knows how to resolve it.
   //
   //if(compiled && rCom && rScr && Platform::compareFileTimes(comModifyTime, scrModifyTime) < 0)
   //{
   //Con::warnf("exec: Warning! Found a DSO from the future! (%s)", nameBuffer);
   //}

    // If we had a DSO, let's check to see if we should be reading from it.
    if((compiled && rCom) && (!rScr || Platform::compareFileTimes(comModifyTime, scrModifyTime) >= 0))
    {
      compiledStream = ResourceManager->openStream(nameBuffer);
      if (compiledStream)
      {
         // Check the version!
         compiledStream->read(&version);
         if(version != DSO_VERSION)
         {
            Con::warnf("exec: Found an old DSO (%s, ver %d < %d), ignoring.", nameBuffer, version, DSO_VERSION);
            ResourceManager->closeStream(compiledStream);
            compiledStream = NULL;
         }
      }
    }

#ifdef TORQUE_ALLOW_JOURNALING
   // If we're journalling, let's write some info out.
   if(journal && Game->isJournalWriting())
      Game->getJournalStream()->writeString(scriptFileName);
#endif //TORQUE_ALLOW_JOURNALING

   if(rScr && !compiledStream)
   {
      // If we have source but no compiled version, then we need to compile
      // (and journal as we do so, if that's required).

       //Con::errorf( "No DSO found! : %s", scriptFileName );
       
      Stream *s = ResourceManager->openStream(scriptFileName);
       
#ifdef	TORQUE_ALLOW_JOURNALING
      if(journal && Game->isJournalWriting())
         Game->getJournalStream()->write(bool(s != NULL));
#endif	//TORQUE_ALLOW_JOURNALING

      if(s)
      {
         scriptSize = ResourceManager->getSize(scriptFileName);
         script = new char [scriptSize+1];
         s->read(scriptSize, script);

#ifdef	TORQUE_ALLOW_JOURNALING
         if(journal && Game->isJournalWriting())
         {
            Game->journalWrite(scriptSize);
            Game->journalWrite(scriptSize, script);
         }
#endif	//TORQUE_ALLOW_JOURNALING
         ResourceManager->closeStream(s);
         script[scriptSize] = 0;
      }

      if (!scriptSize || !script)
      {
         delete [] script;
         Con::errorf(ConsoleLogEntry::Script, "exec: invalid script file %s.", scriptFileName);
         execDepth--;
         return false;
      }

//Luma: Sven -
// no dsos in the editor, seems to fail with so many console changes and version crap. Leaving it to
// work with cs files as is, as they are included with the source either way.

//Also, no DSO generation on iPhone
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
      if(false) 
#else
      if(compiled)
#endif
      {
         // compile this baddie.
         #if defined(TORQUE_DEBUG)
         Con::printf("Compiling %s...", scriptFileName);
         #endif
         CodeBlock *code = new CodeBlock();
         code->compile(nameBuffer, scriptFileName, script);
         delete code;
         code = NULL;

         compiledStream = ResourceManager->openStream(nameBuffer);
         if(compiledStream)
         {
            compiledStream->read(&version);
         }
         else
         {
            // We have to exit out here, as otherwise we get double error reports.
            delete [] script;
            execDepth--;
            return false;
         }
      }
   }
   else
   {
#ifdef	TORQUE_ALLOW_JOURNALING
      if(journal && Game->isJournalWriting())
         Game->getJournalStream()->write(bool(false));
#endif	//TORQUE_ALLOW_JOURNALING
   }
    
    //Luma : Load compiled script here
   if(compiledStream)
   {
      // Delete the script object first to limit memory used
      // during recursive execs.
      delete [] script;
      script = 0;

      // We're all compiled, so let's run it.
      
      //Luma: Profile script executions 
      F32 st1 = (F32)Platform::getRealMilliseconds();

      CodeBlock *code = new CodeBlock;
      code->read(scriptFileName, *compiledStream);
      ResourceManager->closeStream(compiledStream);
      code->exec(0, scriptFileName, NULL, 0, NULL, noCalls, NULL, 0);

        F32 et1 = (F32)Platform::getRealMilliseconds();
        
        F32 etf = et1 - st1;

        if ( scriptExecutionEcho )
            Con::printf("Loaded compiled script %s. Took %.0f ms", scriptFileName, etf);

      ret = true;
   }
   else if(rScr) //Luma : Load normal cs file here.
   {
         // No compiled script,  let's just try executing it
         // directly... this is either a mission file, or maybe
         // we're on a readonly volume.
        
         CodeBlock *newCodeBlock = new CodeBlock();
         StringTableEntry name = StringTable->insert(scriptFileName);

      
      //Luma: Profile script executions 
         F32 st1 = (F32)Platform::getRealMilliseconds();
         
         newCodeBlock->compileExec(name, script, noCalls, 0);

         F32 et1 = (F32)Platform::getRealMilliseconds();

         F32 etf = et1 - st1;
         
        if ( scriptExecutionEcho )
            Con::printf("Executed %s. Took %.0f ms", scriptFileName, etf);

         ret = true;
   }
   else
   {
      // Don't have anything.
      Con::warnf(ConsoleLogEntry::Script, "Missing file: %s!", pathBuffer);
      ret = false;
   }

   delete [] script;
   execDepth--;
   return ret;
}

/*! Use the eval function to execute any valid script statement.
    If you choose to eval a multi-line statement, be sure that there are no comments or comment blocks embedded in the script string.
    @param script A string containing a valid script statement. This may be a single line statement or multiple lines concatenated together with new-line characters.
    @return Returns the result of executing the script statement.
    @sa call
*/
ConsoleFunctionWithDocs(eval, ConsoleString, 2, 2, ( script ))
{
   TORQUE_UNUSED( argc );
   return Con::evaluate(argv[1], false, NULL);
}

/*! Grabs the relevant data for the variable represented by the given string
    @param varName A String representing the variable to check
    @return Returns a string containing component data for the requested variable or an empty string if not found.
*/
ConsoleFunctionWithDocs(getVariable, ConsoleString, 2, 2, (string varName))
{
   return Con::getVariable(argv[1]);
}

/*! Checks whether given name represents a current valid function.
    @param funcName The name of the function to check.
    @return Returns either true if the string represents a valid function or false if not.
*/
ConsoleFunctionWithDocs(isFunction, ConsoleBool, 2, 2, (string funcName))
{
   return Con::isFunction(argv[1]);
}

/*! Checks whether the given method name represents a valid method within the given namespace.
    @param namespace A string representing the namespace in which the method should reside.
    @param method The name of the method in question.
    @return Returns a boolean value which is true if the given mathod name represents a valid method in the namespace and false otherwise.
*/
ConsoleFunctionWithDocs(isMethod, ConsoleBool, 3, 3, (string namespace, string method))
{
   Namespace* ns = Namespace::find( StringTable->insert( argv[1] ) );
   Namespace::Entry* nse = ns->lookup( StringTable->insert( argv[2] ) );
   if( !nse )
      return false;

   return true;
}

/*! Attempts to extract a mod directory from path. Returns empty string on failure.
*/
ConsoleFunctionWithDocs(getModNameFromPath, ConsoleString, 2, 2, (string path))
{
   StringTableEntry modPath = Con::getModNameFromPath(argv[1]);
   return modPath ? modPath : "";
}

//----------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs(getPrefsPath, ConsoleString, 1, 2, ([fileName]?))
{
   const char *filename = Platform::getPrefsPath(argc > 1 ? argv[1] : NULL);
   if(filename == NULL || *filename == 0)
      return "";
     
   return filename;
}

/*! 
*/
ConsoleFunctionWithDocs(execPrefs, ConsoleBool, 2, 4, (fileName, [nocalls]?, [journalScript]?))
{
   const char *filename = Platform::getPrefsPath(argv[1]);
   if (filename == NULL || *filename == 0)
      return false;

   if ( !Platform::isFile(filename) )
    return false;

   argv[0] = "exec";
   argv[1] = filename;
   return dAtob(Con::execute(argc, argv));
}

/*! Use the export function to save all global variables matching the specified name pattern in wildCard to a file, either appending to that file or over-writing it.
    @param wildCard A string identifying what variable(s) to export. All characters used to create a global are allowed and the special symbol \*\, meaning 0 or more instances of any character.
    @param fileName A string containing a path to a file in which to save the globals and their definitions.
    @param append A boolean value. If this value is true, the file will be appended to if it exists, otherwise it will be created/over-written.
    @return No return value
*/
ConsoleFunctionWithDocs(export, ConsoleVoid, 2, 4, ( wildCard, [fileName]?, [append]?))
{
    // Fetch the wildcard.
    const char* pWildcard = argv[1];

    // Fetch the filename.
    const char* pFilename = NULL;
    if ( argc >= 3 )
    {
        Con::expandPath( pathBuffer, sizeof(pathBuffer), argv[2] );
        pFilename = pathBuffer;
    }

    // Fetch append flag.
    const bool append = argc >= 4 ? dAtob(argv[3] ) : false;

    // Export the variables.
    gEvalState.globalVars.exportVariables( pWildcard, pFilename, append );
}

/*! Use the deleteVariables function to delete any global variable matching the wildCard statement.
    @param wildCard A string identifying what variable(s) to delete. All characters used to create a global are allowed and the special symbol \*\, meaning 0 or more instances of any character.
    @return No return value
*/
ConsoleFunctionWithDocs(deleteVariables, ConsoleVoid, 2, 2, ( wildCard ))
{
   TORQUE_UNUSED( argc );
   gEvalState.globalVars.deleteVariables(argv[1]);
}

//----------------------------------------------------------------

/*! Use the trace function to enable (or disable) function call tracing. If enabled, tracing will print a message every time a function is entered, showing what arguments it received, and it will print a message every time a function is exited, showing the return value (or last value of last statement) for that function.
    @param enable A boolean value. If set to true, tracing is enabled, otherwise it is disabled.
    @return No return value
*/
ConsoleFunctionWithDocs(trace, ConsoleVoid, 2, 2, ( enable ))
{
   TORQUE_UNUSED( argc );
   gEvalState.traceOn = dAtob(argv[1]);
   Con::printf("Console trace is %s", gEvalState.traceOn ? "on." : "off.");
}

//----------------------------------------------------------------

#if defined(TORQUE_DEBUG) || defined(INTERNAL_RELEASE)
/*! Use the debug function to cause the engine to issue a debug break and to break into an active debugger.
    For this to work, the engine must have been compiled with either TORQUE_DEBUG, or INTERNAL_RELEASE defined
    @return No return value.
*/
ConsoleFunctionWithDocs(debug, ConsoleVoid, 1, 1, ())
{
   TORQUE_UNUSED( argc );
   TORQUE_UNUSED( argv );
   Platform::debugBreak();
}
#endif

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_OSX)
//Some code for allowing torsion to connect, this code returns the ipads local ip address
//code was obtained from http://blog.zachwaugh.com/post/309927273/programmatically-retrieving-ip-address-of-iphone
//adapted to iT2D by me
/*! Gets the Apple hardware local IP on wifi. Should work on OS X and iOS
*/
ConsoleFunctionWithDocs(getAppleDeviceIPAddress, ConsoleString, 1, 1, ())
{
    char *address = Con::getReturnBuffer(32);
    dStrcpy(address, "error");
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;

    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0)
    {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL)
        {
            if(temp_addr->ifa_addr->sa_family == AF_INET)
            {
                // Check if interface is en0 which is the wifi connection on the iPhone
                // Note: Could be different on MacOSX and simulator and may need modifying
                if(dStrcmp(temp_addr->ifa_name, "en0") == 0)
                {
                    dStrcpy(address, inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr));
                }
            }

            temp_addr = temp_addr->ifa_next;
        }
    }

    // Free memory
    freeifaddrs(interfaces);

    return address;
}
#endif


//----------------------------------------------------------------

/*! 
*/
ConsoleFunctionWithDocs( enumerateConsoleClasses, ConsoleString, 1, 2, ([baseClass]?))
{

   AbstractClassRep *base = NULL;    
   if(argc > 1)
   {
      base = AbstractClassRep::findClassRep(argv[1]);
      if(!base)
         return "";
   }
   
   Vector<AbstractClassRep*> classes;
   U32 bufSize = 0;
   for(AbstractClassRep *rep = AbstractClassRep::getClassList(); rep; rep = rep->getNextClass())
   {
      if( !base || rep->isClass(base))
      {
         classes.push_back(rep);
         bufSize += dStrlen(rep->getClassName()) + 1;
      }
   }
   
   if(!classes.size())
      return "";

   dQsort(classes.address(), classes.size(), sizeof(AbstractClassRep*), ACRCompare);

   char* ret = Con::getReturnBuffer(bufSize);
   dStrcpy( ret, classes[0]->getClassName());
   for( U32 i=0; i< (U32)classes.size(); i++)
   {
      dStrcat( ret, "\t" );
      dStrcat( ret, classes[i]->getClassName() );
   }
   
   return ret;
}

/*! @} */ // group MetaScriptFunctions
