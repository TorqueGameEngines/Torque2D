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
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platform/platformInput.h"
#include "torqueConfig.h"
#include "memory/frameAllocator.h"

// Buffer for expanding script filenames.
static char scriptFilenameBuffer[1024];

//-------------------------------------- Helper Functions
static void forwardslash(char *str)
{
   while(*str)
   {
      if(*str == '\\')
         *str = '/';
      str++;
   }
}

static void backslash(char *str)
{
   while(*str)
   {
      if(*str == '/')
         *str = '\\';
      str++;
   }
}


//----------------------------------------------------------------

static ResourceObject *firstMatch = NULL;

ConsoleFunctionGroupBegin( FileSystem, "Functions allowing you to search for files, read them, write them, and access their properties.");

/*! @addtogroup FileSystem File System
	@ingroup TorqueScriptFunctions
	@{
*/

/*! 
    @param strPattern The string pattern to search for.
    @return Returns a string representing the first file in the directory system matching the given pattern.
*/
ConsoleFunctionWithDocs(findFirstFile, ConsoleString, 2, 2, ( strPattern ))
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   firstMatch = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
      return fn;
   else
      return "";
}

/*!  
    @param strPattern The string pattern to search for.
    @return Returns the next file matching a search begun in findFirstFile.
*/
ConsoleFunctionWithDocs(findNextFile, ConsoleString, 2, 2, ( strPattern ))
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatch(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
      return fn;
   else
      return "";
}

/*!  
    @param strPattern The string pattern to search for.
    @return Returns the number of files in the directory tree that match the given pattern
*/
ConsoleFunctionWithDocs(getFileCount, ConsoleInt, 2, 2, (strPattern))
{
   TORQUE_UNUSED( argc );
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatch(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatch(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

/*!  
    @param strPattern The string pattern to search for.
    @return Returns the first file in the directory system matching the given pattern.
*/
ConsoleFunctionWithDocs(findFirstFileMultiExpr, ConsoleString, 2, 2, (strPattern))
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   firstMatch = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, NULL);

   if(firstMatch)
      return fn;
   else
      return "";
}

/*! Returns the next file matching a search begun in findFirstFile.
*/
ConsoleFunctionWithDocs(findNextFileMultiExpr, ConsoleString, 2, 2, (string pattern))
{
   TORQUE_UNUSED( argc );
   const char *fn = NULL;
   if(Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]))
      firstMatch = ResourceManager->findMatchMultiExprs(scriptFilenameBuffer, &fn, firstMatch);
   else
      firstMatch = NULL;

   if(firstMatch)
      return fn;
   else
      return "";
}

/*!  
    @param strPattern The string pattern to search for.
    @return Returns the number of files in the directory tree that match the given pattern
*/
ConsoleFunctionWithDocs(getFileCountMultiExpr, ConsoleInt, 2, 2, (strPattern))
{
   TORQUE_UNUSED( argc );
   const char* fn;
   U32 count = 0;
   firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, NULL);
   if ( firstMatch )
   {
      count++;
      while ( 1 )
      {
         firstMatch = ResourceManager->findMatchMultiExprs(argv[1], &fn, firstMatch);
         if ( firstMatch )
            count++;
         else
            break;
      }
   }

   return( count );
}

/*! 
    @param filename The string representing the file from which to get the CRC
    @return An integer
*/
ConsoleFunctionWithDocs(getFileCRC, ConsoleInt, 2, 2, (filename))
{
   TORQUE_UNUSED( argc );
   U32 crcVal;
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);

   if(!ResourceManager->getCrc(scriptFilenameBuffer, crcVal))
      return(-1);
   return(S32(crcVal));
}

/*!  @param pathName Path to check. @return Returns true if the given path is a folder/directory, false otherwise
*/
ConsoleFunctionWithDocs(isDirectory, ConsoleBool, 2, 2, (path))
{
    bool doesExist = Platform::isDirectory(argv[1]);
    if(doesExist)
        return true;

    return false;
}

/*! 
    @param fileName Filename to check.
    @return Returns true if the given filename is an existing file or false otherwise
*/
ConsoleFunctionWithDocs(isFile, ConsoleBool, 2, 2, (fileName))
{
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return bool(ResourceManager->find(scriptFilenameBuffer));
}

/*! 
    @param fileName Filename to check.
    @return Returns true if the given filename is an existing file and is not Read-Only or false otherwise
*/
ConsoleFunctionWithDocs(isWriteableFileName, ConsoleBool, 2, 2, (fileName))
{
   TORQUE_UNUSED( argc );

   char filename[1024];
   Con::expandPath(filename, sizeof(filename), argv[1]);

   if (*filename == 0)
      return false;

   // in a writeable directory?
   if(!ResourceManager->isValidWriteFileName(filename))
      return(false);

   // exists?
   FileStream fs;
   if(!fs.open(filename, FileStream::Read))
      return(true);

   // writeable? (ReadWrite will create file if it does not exist)
   fs.close();
   if(!fs.open(filename, FileStream::ReadWrite))
      return(false);

   return(true);
}

/*! 
    @param strPath The path in which to check
    @param intDepth The depth in which to return (default 0 if not specified)
    @return The directory contents
*/
ConsoleFunctionWithDocs(getDirectoryList, ConsoleString, 2, 3, (strPath, [intDepth]?))
{
   // Grab the full path.
   char path[1024];
   Platform::makeFullPathName(dStrcmp(argv[1], "/") == 0 ? "" : argv[1], path, sizeof(path));

   // Append a trailing backslash if it's not present already.
   if (path[dStrlen(path) - 1] != '/')
   {
      S32 pos = dStrlen(path);
      path[pos] = '/';
      path[pos + 1] = '\0';
   }

   // Grab the depth to search.
   S32 depth = 0;
   if (argc > 2)
      depth = dAtoi(argv[2]);

   // Dump the directories.
   Vector<StringTableEntry> directories;
   Platform::dumpDirectories(path, directories, depth, true);

   if( directories.empty() )
      return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < directories.size(); i++)
      length += dStrlen(directories[i]) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < directories.size(); i++)
   {
      dStrcpy(p, directories[i]);
      p += dStrlen(directories[i]);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

/*! Gets all the files in the specified directory.
    @param strPath The path in which to check
    @return A list of files in the specified directory.
*/
ConsoleFunctionWithDocs(getFileList, ConsoleString, 2, 2, (strPath))
{
   // Grab the full path.
   char basePath[1024];
   Con::expandPath( basePath, sizeof(basePath), argv[1] );

   Vector<Platform::FileInfo> files;
   if ( !Platform::dumpPath( basePath, files, 0 ) )
   {
       Con::warnf("Failed to get file list in directory '%s'", basePath );
       return "";
   }

   if ( files.size() == 0 )
       return "";

   // Grab the required buffer length.
   S32 length = 0;

   for (S32 i = 0; i < files.size(); i++)
       length += dStrlen(files[i].pFileName) + 1;

   // Get a return buffer.
   char* buffer = Con::getReturnBuffer(length);
   char* p = buffer;

   // Copy the directory names to the buffer.
   for (S32 i = 0; i < files.size(); i++)
   {
       dStrcpy(p, files[i].pFileName);
       p += dStrlen(files[i].pFileName);
      // Tab separated.
      p[0] = '\t';
      p++;
   }
   p--;
   p[0] = '\0';

   return buffer;
}

/*! 
    @param fileName The name of the file to check. 
    @return Returns the size of the file as an integer or -1 if file not found
*/
ConsoleFunctionWithDocs(fileSize, ConsoleInt, 2, 2, (fileName))
{
   TORQUE_UNUSED( argc );
   Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
   return Platform::getFileSize( scriptFilenameBuffer );
}

/*! 
    @param fileName Name of the file to remove
    @return Returns true on success and false on failure
*/
ConsoleFunctionWithDocs(fileDelete, ConsoleBool, 2,2, (fileName))
{
   static char fileName[1024];
   static char sandboxFileName[1024];

   Con::expandPath( fileName, sizeof( fileName ), argv[1] );
   Platform::makeFullPathName(fileName, sandboxFileName, sizeof(sandboxFileName));

   return Platform::fileDelete(sandboxFileName);
}

/*! 
    @param directoryName Name of the directory to remove
    @return Returns true on success and false on failure
*/
ConsoleFunctionWithDocs(directoryDelete, ConsoleBool, 2,2, (directoryName))
{
   static char directoryName[1024];
   static char sandboxdirectoryName[1024];

   Con::expandPath( directoryName, sizeof( directoryName ), argv[1] );
   Platform::makeFullPathName(directoryName, sandboxdirectoryName, sizeof(sandboxdirectoryName));

   return Platform::deleteDirectory(sandboxdirectoryName);
}

//----------------------------------------------------------------

/*! 
    @param filePath Full path of file to check.
    @return Returns true if the given filename is a valid image file or false otherwise
*/
ConsoleFunctionWithDocs(isValidImageFile, ConsoleBool, 2, 2, (string filePath))
{
    Con::expandPath(scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[1]);
    
    // does file exist?
    if (!ResourceManager->find(scriptFilenameBuffer))
        return false;

    const char *ext = dStrrchr(argv[1], '.');
    if (!ext)
        return false;	// no extension

    Stream *stream = ResourceManager->openStream(scriptFilenameBuffer);
    if(stream == NULL)
        return false;

    bool ret = false;

    if (dStricmp(ext, ".jpg") == 0)
    {
        U8 bArray[2];
        stream->read(2, bArray);
        // check header signature
        ret = ((bArray[0] == 0xFF) && (bArray[1] == 0xD8));
    } 
    else if (dStricmp(ext, ".png") == 0)
    {
        int i = 0;
        U8 bArray[8];
        stream->read(8, bArray);
        // check header signature
        ret = ((bArray[i++] == 0x89) && (bArray[i++] == 0x50) && (bArray[i++] == 0x4E) && (bArray[i++] == 0x47));
    }

    ResourceManager->closeStream(stream);

    return ret;
}

/*!  
    @param Name of file from which to extract extension
    @return A string containing the file's extension (assuming all data after first '.' is the extension) or returns empty string on failure
*/
ConsoleFunctionWithDocs(fileExt, ConsoleString, 2, 2, (fileName))
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrrchr(argv[1], '.');
   if(ret)
      return ret;
   return "";
}

/*! 
*/
ConsoleFunctionWithDocs(fileBase, ConsoleString, 2, 2, (fileName))
{

   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);
   
   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      path = szPathCopy;
   else
      path++;
   char *ret = Con::getReturnBuffer(dStrlen(path) + 1);
   dStrcpy(ret, path);
   char *ext = dStrrchr(ret, '.');
   if(ext)
      *ext = 0;
   return ret;
}

/*! Extract the filename from the full path description
*/
ConsoleFunctionWithDocs(fileName, ConsoleString, 2, 2, (filePathName))
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *name = dStrrchr(szPathCopy, '/');
   if(!name)
      name = szPathCopy;
   else
      name++;
   char *ret = Con::getReturnBuffer(dStrlen(name));
   dStrcpy(ret, name);
   return ret;
}

/*! Extract the file path from a file's full desciption
*/
ConsoleFunctionWithDocs(filePath, ConsoleString, 2, 2, (fileName))
{
   S32 pathLen = dStrlen( argv[1] );
   FrameTemp<char> szPathCopy( pathLen + 1);

   dStrcpy( szPathCopy, argv[1] );
   forwardslash( szPathCopy );

   TORQUE_UNUSED( argc );
   const char *path = dStrrchr(szPathCopy, '/');
   if(!path)
      return "";
   U32 len = (U32)(path - (char*)szPathCopy);
   char *ret = Con::getReturnBuffer(len + 1);
   dStrncpy(ret, szPathCopy, len);
   ret[len] = 0;
   return ret;
}

/*! Gets the current date and time.
    @param reverse Determines if the format will be Month/Day/Year/Hour:Min:Seconds or Year/Month/Day/Hour:Min:Seconds
*/
ConsoleFunctionWithDocs(getCurrentDate, ConsoleString, 2, 2, (bool reverse))
{
    Platform::LocalTime lt;
    Platform::getLocalTime(lt);

    char temp[128];
    
    if (dAtob(argv[1]))
        dSprintf(temp, 256, "%d/%d/%d:%d:%d:%d", lt.year+1900, lt.month+1, lt.monthday, lt.hour, lt.min, lt.sec);
    else
        dSprintf(temp, 256, "%d/%d/%d:%d:%d:%d", lt.month+1, lt.monthday, lt.year+1900, lt.hour, lt.min, lt.sec);

    char* returnBuffer = Con::getReturnBuffer(dStrlen(temp + 1));
    dStrcpy(returnBuffer, temp);

    return returnBuffer;
}

/*! ;
*/
ConsoleFunctionWithDocs(openFolder, ConsoleVoid, 2 ,2, (path))
{
   Platform::openFolder( argv[1] );
}

/*! 
*/
ConsoleFunctionWithDocs(pathCopy, ConsoleBool, 3, 4, (fromFile, toFile, [nooverwrite = true]?))
{
   bool nooverwrite = true;

   if( argc > 3 )
      nooverwrite = dAtob( argv[3] );

   static char fromFile[1024];
   static char toFile[1024];

   static char qualifiedFromFile[1024];
   static char qualifiedToFile[1024];

   Con::expandPath( fromFile, sizeof( fromFile ), argv[1] );
   Con::expandPath( toFile, sizeof( toFile ), argv[2] );

   Platform::makeFullPathName(fromFile, qualifiedFromFile, sizeof(qualifiedFromFile));
   Platform::makeFullPathName(toFile, qualifiedToFile, sizeof(qualifiedToFile));

   return Platform::pathCopy( qualifiedFromFile, qualifiedToFile, nooverwrite );
}

/*! 
*/
ConsoleFunctionWithDocs(getCurrentDirectory, ConsoleString, 1, 1, ())
{
   return Platform::getCurrentDirectory();
}

/*! 
*/
ConsoleFunctionWithDocs( setCurrentDirectory, ConsoleBool, 2, 2, (absolutePathName))
{
   return Platform::setCurrentDirectory( StringTable->insert( argv[1] ) );

}

/*! 
*/
ConsoleFunctionWithDocs(getExecutableName, ConsoleString, 1, 1, ())
{
   return Platform::getExecutableName();
}

/*!
*/
ConsoleFunctionWithDocs(getMainDotCsDir, ConsoleString, 1, 1, ())
{
	return Platform::getMainDotCsDir();
}

/*!
*/
ConsoleFunctionWithDocs(setMainDotCsDir, ConsoleVoid, 2, 2, (path))
{
	Platform::setMainDotCsDir(argv[1]);
}

/*! 
*/
ConsoleFunctionWithDocs(makeFullPath, ConsoleString, 2, 3, (string path, [string currentWorkingDir]?))
{
   char *buf = Con::getReturnBuffer(512);
   Platform::makeFullPathName(argv[1], buf, 512, argc > 2 ? argv[2] : NULL);
   return buf;
}

/*! 
*/
ConsoleFunctionWithDocs(makeRelativePath, ConsoleString, 3, 3, (string path, string to))
{
   return Platform::makeRelativePathName(argv[1], argv[2]);
}

/*! 
*/
ConsoleFunctionWithDocs(pathConcat, ConsoleString, 3, 0, (string path, string file1, [... fileN]*))
{
   char *buf = Con::getReturnBuffer(1024);
   char pathBuf[1024];
   dStrcpy(buf, argv[1]);

   for(S32 i = 2;i < argc;++i)
   {
      Platform::makeFullPathName(argv[i], pathBuf, 1024, buf);
      dStrcpy(buf, pathBuf);
   }
   return buf;
}

/*! 
*/
ConsoleFunctionWithDocs(restartInstance, ConsoleVoid, 1, 1, ())
{
   Game->setRestart( true );
   Platform::postQuitMessage( 0 );
}


/*! Creates the path, verifing all the elements in a path exist or creating them if they do not.
    Note that the path should end with a slash (/).  Otherwise, the last element in the path will be assumed to be a filename and not a path component, and it will not be created.
    For example \"data/stage2/part1\" will verify or create \"data/stage2/\" and not \"part1\".
*/
ConsoleFunctionWithDocs( createPath, ConsoleBool, 2,2, (pathName))
{
   static char pathName[1024];

   Con::expandPath( pathName, sizeof( pathName ), argv[1] );

   return Platform::createPath( pathName );
}

ConsoleFunctionGroupEnd(FileSystem)

/*! @} */ // group FileSystem
