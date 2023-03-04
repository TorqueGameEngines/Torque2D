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

#include "platform//platform.h"
#include "platformWin32/platformWin32.h"
#include "platform/platformFileIO.h"
#include "collection/vector.h"
#include "string/stringTable.h"
#include "console/console.h"
#include "io/resource/resourceManager.h"
#include "string/unicode.h"

// Microsoft VC++ has this POSIX header in the wrong directory
#if defined(TORQUE_COMPILER_VISUALC)
#include <sys/utime.h>
#elif defined (TORQUE_COMPILER_GCC)
#include <time.h>
#include <sys/utime.h>
#else
#include <utime.h>
#endif
//------------------------------------------------------------------------------

enum DriveType
{
   DRIVETYPE_FIXED = 0,
   DRIVETYPE_REMOVABLE = 1,
   DRIVETYPE_REMOTE = 2,
   DRIVETYPE_CDROM = 3,
   DRIVETYPE_RAMDISK = 4,
   DRIVETYPE_UNKNOWN = 5
};

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

//-----------------------------------------------------------------------------
bool Platform::fileDelete(const char * name)
{
   if(!name || (dStrlen(name) >= MAX_PATH))
      return(false);
   //return(::DeleteFile(name));
   if(Platform::isFile(name))
      return(remove(name) == 0);
   else
      return ::RemoveDirectoryA(name) != 0;
}

bool Platform::fileRename(const char *oldName, const char *newName)
{
   if(oldName == NULL || newName == NULL || dStrlen(oldName) >= MAX_PATH || dStrlen(newName) > MAX_PATH)
      return false;

   char oldf[MAX_PATH], newf[MAX_PATH];
   dStrcpy(oldf, oldName);
   dStrcpy(newf, newName);
   backslash(oldf);
   backslash(newf);

   return rename(oldf, newf) == 0;
}

bool Platform::fileTouch(const char * name)
{
   // change the modified time to the current time (0byte WriteFile fails!)
   return(utime(name, 0) != -1);
};

static S32 QSORT_CALLBACK pathCompare(const void* a,const void* b)
{
   const char *aa = *((const char **)a), *ptrA;
   const char *bb = *((const char **)b), *ptrB;

   while(aa && bb && *aa && *bb)
   {
      ptrA = dStrchr(aa, '/');
      ptrB = dStrchr(bb, '/');

      if(ptrA && ptrB)
      {
         ++ptrA; ++ptrB;

         S32 len = ptrA - aa > ptrB - bb ? ptrA - aa : ptrB - bb;
         S32 ret = dStrnicmp(aa, bb, len);
         if(ret != 0)
            return ret;
      }

      aa = ptrA;
      bb = ptrB;
   }

   if(aa && ! bb)
      return -1;
   if(bb && ! aa)
      return 1;
   if(aa && bb)
      return dStricmp(aa, bb);

   return 0;
}

bool Platform::pathCopy(const char *fromName, const char *toName, bool nooverwrite)
{
   if(!fromName || (dStrlen(fromName) >= MAX_PATH) ||
      !toName || (dStrlen(toName) >= MAX_PATH))
      return(false);

   static char filebuf[2048];
   dStrcpy(filebuf, fromName);
   backslash(filebuf);
   fromName = filebuf;

   static char filebuf2[2048];
   dStrcpy(filebuf2, toName);
   backslash(filebuf2);
   toName = filebuf2;

   // Copy File
   if (Platform::isFile(fromName))
   {
#ifdef UNICODE
      UTF16 f[1024], t[1024];
      convertUTF8toUTF16((UTF8 *)fromName, f, sizeof(f));
      convertUTF8toUTF16((UTF8 *)toName, t, sizeof(t));
#else
      char *f = (char*)fromName;
      char *t = (char*)toName;
#endif
      if(::CopyFile((LPCWSTR)f, (LPCWSTR)t, nooverwrite))
      {
         return true;
      }

      return false;
   }

   // Copy Path
   else if (Platform::isDirectory(fromName))
   {
      // If the destination path exists and we don't want to overwrite, return.
      if ((Platform::isDirectory(toName) || Platform::isFile(toName)) && nooverwrite)
         return false;

      Vector<StringTableEntry> directoryInfo;
      Platform::dumpDirectories(fromName, directoryInfo, -1);

      ResourceManager->initExcludedDirectories();

      Vector<Platform::FileInfo> fileInfo;
      Platform::dumpPath(fromName, fileInfo);

      Platform::clearExcludedDirectories();

      // Create all the directories.
      for (S32 i = 0; i < directoryInfo.size(); i++)
      {
         const char* from = directoryInfo[i];

         char to[1024];
         Platform::makeFullPathName(from + dStrlen(fromName) + (dStricmp(from, fromName) ? 1 : 0), to, sizeof(to), toName);
         if(*(to + dStrlen(to) - 1) != '/')
            dStrcat(to, "/");
         forwardslash(to);

         if (!Platform::createPath(to))
         {
            // New directory should be deleted here.
            return false;
         }
      }

      for (S32 i = 0; i < fileInfo.size(); i++)
      {
         char from[1024];
         dSprintf(from, 1024, "%s/%s", fileInfo[i].pFullPath, fileInfo[i].pFileName);

         char to[1024];
         Platform::makeFullPathName(fileInfo[i].pFullPath + dStrlen(fromName) + (dStricmp(fileInfo[i].pFullPath, fromName) ? 1 : 0), to, sizeof(to), toName);
         dStrcat(to, "/");
         dStrcat(to, fileInfo[i].pFileName);

         backslash(from);
         backslash(to);
         
#ifdef UNICODE
         UTF16 f[1024], t[1024];
         convertUTF8toUTF16((UTF8 *)from, f, sizeof(f));
         convertUTF8toUTF16((UTF8 *)to, t, sizeof(t));
#else
         char *f = (char*)from;
         char *t = (char*)to;
#endif

         if (!::CopyFile((LPCWSTR)f, (LPCWSTR)t, nooverwrite))
         {
            // New directory should be deleted here.
            return false;
         }

      }

      return true;
   }

   return false;
}

//-----------------------------------------------------------------------------
// Constructors & Destructor
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// After construction, the currentStatus will be Closed and the capabilities
// will be 0.
//-----------------------------------------------------------------------------
File::File()
: currentStatus(Closed), capability(0)
{
    AssertFatal(sizeof(HANDLE) == sizeof(void *), "File::File: cannot cast void* to HANDLE");

    handle = (void *)INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------
// insert a copy constructor here... (currently disabled)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
File::~File()
{
    close();
    handle = (void *)INVALID_HANDLE_VALUE;
}


//-----------------------------------------------------------------------------
// Open a file in the mode specified by openMode (Read, Write, or ReadWrite).
// Truncate the file if the mode is either Write or ReadWrite and truncate is
// true.
//
// Sets capability appropriate to the openMode.
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::open(const char *filename, const AccessMode openMode)
{
   static char filebuf[2048];
   dStrcpy(filebuf, filename);
   backslash(filebuf);
#ifdef UNICODE
   UTF16 fname[2048];
   convertUTF8toUTF16((UTF8 *)filebuf, fname, sizeof(fname));
#else
   char *fname;
   fname = filebuf;
#endif

    AssertFatal(NULL != fname, "File::open: NULL fname");
    AssertWarn(INVALID_HANDLE_VALUE == (HANDLE)handle, "File::open: handle already valid");

    // Close the file if it was already open...
    if (Closed != currentStatus)
        close();

    // create the appropriate type of file...
    switch (openMode)
    {
    case Read:
        handle = (void *)CreateFile((LPCWSTR)fname,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
        break;
    case Write:
        handle = (void *)CreateFile((LPCWSTR)fname,
                                    GENERIC_WRITE,
                                    0,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
        break;
    case ReadWrite:
        handle = (void *)CreateFile((LPCWSTR)fname,
                                    GENERIC_WRITE | GENERIC_READ,
                                    0,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
        break;
    case WriteAppend:
        handle = (void *)CreateFile((LPCWSTR)fname,
                                    GENERIC_WRITE,
                                    0,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                                    NULL);
        break;

    default:
        AssertFatal(false, "File::open: bad access mode");    // impossible
      }

    if (INVALID_HANDLE_VALUE == (HANDLE)handle)                // handle not created successfully
    {
        return setStatus();
    }
    else
    {
        // successfully created file, so set the file capabilities...
        switch (openMode)
        {
        case Read:
            capability = U32(FileRead);
            break;
        case Write:
        case WriteAppend:
            capability = U32(FileWrite);
            break;
        case ReadWrite:
            capability = U32(FileRead)  |
                         U32(FileWrite);
            break;
        default:
            AssertFatal(false, "File::open: bad access mode");
        }
        return currentStatus = Ok;                                // success!
    }
}

//-----------------------------------------------------------------------------
// Get the current position of the file pointer.
//-----------------------------------------------------------------------------
U32 File::getPosition() const
{
    AssertFatal(Closed != currentStatus, "File::getPosition: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::getPosition: invalid file handle");

    return SetFilePointer((HANDLE)handle,
                          0,                                    // how far to move
                          NULL,                                    // pointer to high word
                          FILE_CURRENT);                        // from what point
}

//-----------------------------------------------------------------------------
// Set the position of the file pointer.
// Absolute and relative positioning is supported via the absolutePos
// parameter.
//
// If positioning absolutely, position MUST be positive - an IOError results if
// position is negative.
// Position can be negative if positioning relatively, however positioning
// before the start of the file is an IOError.
//
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::setPosition(S32 position, bool absolutePos)
{
    AssertFatal(Closed != currentStatus, "File::setPosition: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::setPosition: invalid file handle");

    if (Ok != currentStatus && EOS != currentStatus)
        return currentStatus;

    U32 finalPos;
    switch (absolutePos)
    {
    case true:                                                    // absolute position
        AssertFatal(0 <= position, "File::setPosition: negative absolute position");

        // position beyond EOS is OK
        finalPos = SetFilePointer((HANDLE)handle,
                                  position,
                                  NULL,
                                  FILE_BEGIN);
        break;
    case false:                                                    // relative position
        AssertFatal((getPosition() >= (U32)abs(position) && 0 > position) || 0 <= position, "File::setPosition: negative relative position");

        // position beyond EOS is OK
        finalPos = SetFilePointer((HANDLE)handle,
                                  position,
                                  NULL,
                                  FILE_CURRENT);
    }

    if (0xffffffff == finalPos)
        return setStatus();                                        // unsuccessful
    else if (finalPos >= getSize())
        return currentStatus = EOS;                                // success, at end of file
    else
        return currentStatus = Ok;                                // success!
}

//-----------------------------------------------------------------------------
// Get the size of the file in bytes.
// It is an error to query the file size for a Closed file, or for one with an
// error status.
//-----------------------------------------------------------------------------
U32 File::getSize() const
{
    AssertWarn(Closed != currentStatus, "File::getSize: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::getSize: invalid file handle");

    if (Ok == currentStatus || EOS == currentStatus)
    {
        DWORD high;
        return GetFileSize((HANDLE)handle, &high);                // success!
    }
    else
        return 0;                                                // unsuccessful
}

//-----------------------------------------------------------------------------
// Flush the file.
// It is an error to flush a read-only file.
// Returns the currentStatus of the file.
//-----------------------------------------------------------------------------
File::Status File::flush()
{
    AssertFatal(Closed != currentStatus, "File::flush: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::flush: invalid file handle");
    AssertFatal(true == hasCapability(FileWrite), "File::flush: cannot flush a read-only file");

    if (0 != FlushFileBuffers((HANDLE)handle))
        return setStatus();                                        // unsuccessful
    else
        return currentStatus = Ok;                                // success!
}

//-----------------------------------------------------------------------------
// Close the File.
//
// Returns the currentStatus
//-----------------------------------------------------------------------------
File::Status File::close()
{
    // check if it's already closed...
    if (Closed == currentStatus)
        return currentStatus;

    // it's not, so close it...
    if (INVALID_HANDLE_VALUE != (HANDLE)handle)
    {
        if (0 == CloseHandle((HANDLE)handle))
            return setStatus();                                    // unsuccessful
    }
    handle = (void *)INVALID_HANDLE_VALUE;
    return currentStatus = Closed;
}

//-----------------------------------------------------------------------------
// Self-explanatory.
//-----------------------------------------------------------------------------
File::Status File::getStatus() const
{
    return currentStatus;
}

//-----------------------------------------------------------------------------
// Sets and returns the currentStatus when an error has been encountered.
//-----------------------------------------------------------------------------
File::Status File::setStatus()
{
    switch (GetLastError())
    {
    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_ACCESS:
    case ERROR_TOO_MANY_OPEN_FILES:
    case ERROR_FILE_NOT_FOUND:
    case ERROR_SHARING_VIOLATION:
    case ERROR_HANDLE_DISK_FULL:
          return currentStatus = IOError;

    default:
          return currentStatus = UnknownError;
    }
}

//-----------------------------------------------------------------------------
// Sets and returns the currentStatus to status.
//-----------------------------------------------------------------------------
File::Status File::setStatus(File::Status status)
{
    return currentStatus = status;
}

//-----------------------------------------------------------------------------
// Read from a file.
// The number of bytes to read is passed in size, the data is returned in src.
// The number of bytes read is available in bytesRead if a non-Null pointer is
// provided.
//-----------------------------------------------------------------------------
File::Status File::read(U32 size, char *dst, U32 *bytesRead)
{
    AssertFatal(Closed != currentStatus, "File::read: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::read: invalid file handle");
    AssertFatal(NULL != dst, "File::read: NULL destination pointer");
    AssertFatal(true == hasCapability(FileRead), "File::read: file lacks capability");
    AssertWarn(0 != size, "File::read: size of zero");

    if (Ok != currentStatus || 0 == size)
        return currentStatus;
    else
    {
        DWORD lastBytes;
        DWORD *bytes = (NULL == bytesRead) ? &lastBytes : (DWORD *)bytesRead;
        if (0 != ReadFile((HANDLE)handle, dst, size, bytes, NULL))
        {
            if(*((U32 *)bytes) != size)
                return currentStatus = EOS;                        // end of stream
        }
        else
            return setStatus();                                    // unsuccessful
    }
    return currentStatus = Ok;                                    // successfully read size bytes
}

//-----------------------------------------------------------------------------
// Write to a file.
// The number of bytes to write is passed in size, the data is passed in src.
// The number of bytes written is available in bytesWritten if a non-Null
// pointer is provided.
//-----------------------------------------------------------------------------
File::Status File::write(U32 size, const char *src, U32 *bytesWritten)
{
    AssertFatal(Closed != currentStatus, "File::write: file closed");
    AssertFatal(INVALID_HANDLE_VALUE != (HANDLE)handle, "File::write: invalid file handle");
    AssertFatal(NULL != src, "File::write: NULL source pointer");
    AssertFatal(true == hasCapability(FileWrite), "File::write: file lacks capability");
    AssertWarn(0 != size, "File::write: size of zero");

    if ((Ok != currentStatus && EOS != currentStatus) || 0 == size)
        return currentStatus;
    else
    {
        DWORD lastBytes;
        DWORD *bytes = (NULL == bytesWritten) ? &lastBytes : (DWORD *)bytesWritten;
        if (0 != WriteFile((HANDLE)handle, src, size, bytes, NULL))
            return currentStatus = Ok;                            // success!
        else
            return setStatus();                                    // unsuccessful
    }
}

//-----------------------------------------------------------------------------
// Self-explanatory.
//-----------------------------------------------------------------------------
bool File::hasCapability(Capability cap) const
{
    return (0 != (U32(cap) & capability));
}

S32 Platform::compareFileTimes(const FileTime &a, const FileTime &b)
{
   if(a.v2 > b.v2)
      return 1;
   if(a.v2 < b.v2)
      return -1;
   if(a.v1 > b.v1)
      return 1;
   if(a.v1 < b.v1)
      return -1;
   return 0;
}

static bool recurseDumpPath(const char *path, const char *pattern, Vector<Platform::FileInfo> &fileVector, S32 recurseDepth )
{
   char buf[1024], fullPath[1024];
   WIN32_FIND_DATA findData;

   Platform::makeFullPathName(path, fullPath, sizeof(fullPath));

   dSprintf(buf, sizeof(buf), "%s/%s", fullPath, pattern);

#ifdef UNICODE
   UTF16 search[1024];
   convertUTF8toUTF16((UTF8 *)buf, search, sizeof(search));
#else
   char *search = buf;
#endif
   
   HANDLE handle = FindFirstFile(search, &findData);
   if (handle == INVALID_HANDLE_VALUE)
      return false;

   do
   {
#ifdef UNICODE
      char fnbuf[1024];
      convertUTF16toUTF8(findData.cFileName, (UTF8 *)fnbuf, sizeof(fnbuf));
#else
      char *fnbuf = findData.cFileName;
#endif

      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         // make sure it is a directory
         if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )                             
            continue;

         // skip . and .. directories
         if (dStrcmp(fnbuf, ".") == 0 || dStrcmp(fnbuf, "..") == 0)
            continue;

         // Skip excluded directores
         if(Platform::isExcludedDirectory(fnbuf))
            continue;

         char child[1024];
         dSprintf(child, sizeof(child), "%s/%s", path, fnbuf);
         if( recurseDepth > 0 )
            recurseDumpPath(child, pattern, fileVector, recurseDepth - 1);
         else if (recurseDepth == -1)
            recurseDumpPath(child, pattern, fileVector, -1);
      }      
      else
      {
         // make sure it is the kind of file we're looking for
         if (findData.dwFileAttributes & 
             (FILE_ATTRIBUTE_DIRECTORY|                                      
              FILE_ATTRIBUTE_OFFLINE|
              FILE_ATTRIBUTE_SYSTEM|
              FILE_ATTRIBUTE_TEMPORARY) )                             
            continue;
         
         // add it to the list
         fileVector.increment();
         Platform::FileInfo& rInfo = fileVector.last();

         rInfo.pFullPath = StringTable->insert(path);
         rInfo.pFileName = StringTable->insert(fnbuf);
         rInfo.fileSize  = findData.nFileSizeLow;
      }

   }while(FindNextFile(handle, &findData));

   FindClose(handle);
   return true;
}


//--------------------------------------

bool Platform::getFileTimes(const char *filePath, FileTime *createTime, FileTime *modifyTime)
{
   WIN32_FIND_DATA findData;
#ifdef UNICODE
   UTF16 fp[512];
   convertUTF8toUTF16((UTF8 *)filePath, fp, sizeof(fp));
#else
   const char *fp = filePath;
#endif

   HANDLE h = FindFirstFile(fp, &findData);
   if(h == INVALID_HANDLE_VALUE)
      return false;

   if(createTime)
   {
      createTime->v1 = findData.ftCreationTime.dwLowDateTime;
      createTime->v2 = findData.ftCreationTime.dwHighDateTime;
   }
   if(modifyTime)
   {
      modifyTime->v1 = findData.ftLastWriteTime.dwLowDateTime;
      modifyTime->v2 = findData.ftLastWriteTime.dwHighDateTime;
   }
   FindClose(h);
   return true;
}

//--------------------------------------
bool Platform::createPath(const char *file)
{
   char pathbuf[1024];
   const char *dir;
   pathbuf[0] = 0;
   U32 pathLen = 0;

   while((dir = dStrchr(file, '/')) != NULL)
   {
      dStrncpy(pathbuf + pathLen, file, dir - file);
      pathbuf[pathLen + dir-file] = 0;
#ifdef UNICODE
      UTF16 b[1024];
      convertUTF8toUTF16((UTF8 *)pathbuf, b, sizeof(b));
      bool ret = CreateDirectory(b, NULL);
#else
      bool ret = CreateDirectory(pathbuf, NULL);
#endif
      pathLen += dir - file;
      pathbuf[pathLen++] = '/';
      file = dir + 1;
   }
   return true;
}


//--------------------------------------
bool Platform::dumpPath(const char *path, Vector<Platform::FileInfo> &fileVector, S32 recurseDepth)
{
   return recurseDumpPath(path, "*", fileVector, recurseDepth );
}


//--------------------------------------

StringTableEntry Platform::getCurrentDirectory()
{
   char cwd_buf[2048];
   GetCurrentDirectoryA(2047, cwd_buf);
   forwardslash(cwd_buf);
   return StringTable->insert(cwd_buf);
}

bool Platform::setCurrentDirectory(StringTableEntry newDir)
{
   //StringTableEntry lastDir = Platform::getCurrentDirectory();
   char cwd_buf[2048];
   dStrncpy(cwd_buf, newDir, sizeof(cwd_buf)-1);
   cwd_buf[sizeof(cwd_buf)-1] = 0;
   backslash(cwd_buf);
   return SetCurrentDirectoryA(cwd_buf);
}

StringTableEntry Platform::getExecutableName()
{
   static StringTableEntry cen = NULL;
   if (!cen)
   {
      char cen_buf[2048];
      GetModuleFileNameA( NULL, cen_buf, 2047);
      forwardslash(cen_buf);

      char *delimiter = dStrrchr( cen_buf, '/' );

      if( delimiter != NULL )
      {
         *delimiter = 0x00;
         delimiter++;
         cen = StringTable->insert(delimiter);
      }
      else
         cen = StringTable->insert(cen_buf);
   }
   return cen;
}

StringTableEntry Platform::getExecutablePath()
{
   static StringTableEntry cen = NULL;
   if (!cen)
   {
      char cen_buf[2048];
      GetModuleFileNameA( NULL, cen_buf, 2047);
      forwardslash(cen_buf);

      char *delimiter = dStrrchr( cen_buf, '/' );

      if( delimiter != NULL )
         *delimiter = 0x00;

      cen = StringTable->insert(cen_buf);
   }
   return cen;
}

//--------------------------------------
bool Platform::isFile(const char *pFilePath)
{
   if (!pFilePath || !*pFilePath)
      return false;

   // Get file info
   WIN32_FIND_DATA findData;
#ifdef UNICODE
   UTF16 b[512];
   convertUTF8toUTF16((UTF8 *)pFilePath, b, sizeof(b));
   HANDLE handle = FindFirstFile(b, &findData);
#else
   HANDLE handle = FindFirstFile(pFilePath, &findData);
#endif

   // [neo, 4/6/2007]
   // This used to be after the FindClose() call 
   if(handle == INVALID_HANDLE_VALUE)
      return false;

   FindClose(handle);

   // if the file is a Directory, Offline, System or Temporary then FALSE
   if (findData.dwFileAttributes &
       (FILE_ATTRIBUTE_DIRECTORY|
        FILE_ATTRIBUTE_OFFLINE|
        FILE_ATTRIBUTE_SYSTEM|
        FILE_ATTRIBUTE_TEMPORARY) )
      return false;

   // must be a real file then
   return true;
}

//--------------------------------------
S32 Platform::getFileSize(const char *pFilePath)
{
   if (!pFilePath || !*pFilePath)
      return -1;

   // Get file info
   WIN32_FIND_DATAA findData;
   HANDLE handle = FindFirstFileA(pFilePath, &findData);
   FindClose(handle);

   if(handle == INVALID_HANDLE_VALUE)
      return -1;

   // if the file is a Directory, Offline, System or Temporary then FALSE
   if (findData.dwFileAttributes &
       (FILE_ATTRIBUTE_DIRECTORY|
        FILE_ATTRIBUTE_OFFLINE|
        FILE_ATTRIBUTE_SYSTEM|
        FILE_ATTRIBUTE_TEMPORARY) )
      return -1;

   // must be a real file then
   return findData.nFileSizeLow;;
}


//--------------------------------------
bool Platform::isDirectory(const char *pDirPath)
{
   if (!pDirPath || !*pDirPath)
      return false;

   // Get file info
   WIN32_FIND_DATA findData;
#ifdef UNICODE
   UTF16 b[512];
   convertUTF8toUTF16((UTF8 *)pDirPath, b, sizeof(b));
   HANDLE handle = FindFirstFile(b, &findData);
#else
   HANDLE handle = FindFirstFile(pDirPath, &findData);
#endif

   // [neo, 5/15/2007]
   // This check was AFTER FindClose for some reason - this is most probably the
   // original intent.
   if(handle == INVALID_HANDLE_VALUE)
      return false;

   FindClose(handle);
   
   // if the file is a Directory, Offline, System or Temporary then FALSE
   if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
   {
      // make sure it's a valid game directory
      if (findData.dwFileAttributes & (FILE_ATTRIBUTE_OFFLINE|FILE_ATTRIBUTE_SYSTEM) )
         return false;

      // must be a directory
      return true;
   }

   return false;
}



//--------------------------------------
bool Platform::isSubDirectory(const char *pParent, const char *pDir)
{
   if (!pParent || !*pDir)
      return false;

   // this is somewhat of a brute force method but we need to be 100% sure
   // that the user cannot enter things like ../dir or /dir etc,...
   WIN32_FIND_DATAA findData;
   HANDLE handle = FindFirstFileA(avar("%s/*", pParent), &findData);
   if (handle == INVALID_HANDLE_VALUE)
      return false;
   do
   {
      // if it is a directory...
      if (findData.dwFileAttributes &
          (FILE_ATTRIBUTE_DIRECTORY|
           FILE_ATTRIBUTE_OFFLINE|
           FILE_ATTRIBUTE_SYSTEM|
           FILE_ATTRIBUTE_TEMPORARY) )
      {
         // and the names match
         if (dStrcmp(pDir, findData.cFileName ) == 0)
         {
            // then we have a real sub directory
            FindClose(handle);
            return true;
         }
      }
   }while(FindNextFileA(handle, &findData));

   FindClose(handle);
   return false;
}

bool Platform::hasSubDirectory(const char *pPath)
{
   if( !pPath )
      return false;

   ResourceManager->initExcludedDirectories();

   char search[1024];
   WIN32_FIND_DATAA findData;

   // Compose our search string - Format : ([path]/[subpath]/*)
   char trail = pPath[ dStrlen(pPath) - 1 ];
   if( trail == '/' )
      dStrcpy( search, pPath );
   else
      dSprintf(search, 1024, "%s/*", pPath );

   // See if we get any hits
   HANDLE handle = FindFirstFileA(search, &findData);
   if (handle == INVALID_HANDLE_VALUE)
      return false;

   do
   {
      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         // skip . and .. directories
         if (dStrcmp(findData.cFileName, ".") == 0 || dStrcmp(findData.cFileName, "..") == 0)
            continue;

         if( Platform::isExcludedDirectory( findData.cFileName ) )
            continue;

         Platform::clearExcludedDirectories();
         FindClose(handle);
         
         return true;
      }      
   }
   while(FindNextFileA(handle, &findData));

   FindClose(handle);

   Platform::clearExcludedDirectories();

   return false;
}


static bool recurseDumpDirectories(const char *basePath, const char *subPath, Vector<StringTableEntry> &directoryVector, S32 currentDepth, S32 recurseDepth, bool noBasePath)
{
   char search[1024];

   //-----------------------------------------------------------------------------
   // Compose our search string - Format : ([path]/[subpath]/*)
   //-----------------------------------------------------------------------------

   dsize_t trLen = basePath ? dStrlen(basePath) : 0;
   dsize_t subtrLen = subPath ? dStrlen(subPath) : 0;
   char trail = trLen > 0 ? basePath[trLen - 1] : '\0';
   char subTrail = subtrLen > 0 ? subPath[subtrLen - 1] : '\0';
   char subLead = subtrLen > 0 ? subPath[0] : '\0';

   if (trail == '/')
   {
      // we have a sub path and it's not an empty string
      if (subPath && (dStrncmp(subPath, "", 1) != 0))
      {
         if (subTrail == '/')
            dSprintf(search, 1024, "%s%s*", basePath, subPath);
         else
            dSprintf(search, 1024, "%s%s/*", basePath, subPath);
      }
      else
         dSprintf(search, 1024, "%s*", basePath);
   }
   else
   {
      if (subPath && (dStrncmp(subPath, "", 1) != 0))
         if (subTrail == '/')
            dSprintf(search, 1024, "%s%s*", basePath, subPath);
         else
            dSprintf(search, 1024, "%s%s/*", basePath, subPath);
      else
         dSprintf(search, 1024, "%s/*", basePath);
   }

   //-----------------------------------------------------------------------------
   // See if we get any hits
   //-----------------------------------------------------------------------------
   WIN32_FIND_DATAA findData;
   HANDLE handle = FindFirstFileA(search, &findData);
   if (handle == INVALID_HANDLE_VALUE)
      return false;

   //-----------------------------------------------------------------------------
   // add path to our return list ( provided it is valid )
   //-----------------------------------------------------------------------------
   if (!Platform::isExcludedDirectory(subPath))
   {

      if (noBasePath)
      {
         // We have a path and it's not an empty string or an excluded directory
         if ((subPath && (dStrncmp(subPath, "", 1) != 0)))
            directoryVector.push_back(StringTable->insert(subPath));
      }
      else
      {
         if ((subPath && (dStrncmp(subPath, "", 1) != 0)))
         {
            char szPath[1024];
            dMemset(szPath, 0, 1024);
            if (trail == '/')
            {
               if (subLead == '/')
                  dSprintf(szPath, 1024, "%s%s", basePath, &subPath[1]);
               else
                  dSprintf(szPath, 1024, "%s%s", basePath, subPath);
            }
            else
            {
               if (subLead == '/')
                  dSprintf(szPath, 1024, "%s%s", basePath, subPath);
               else
                  dSprintf(szPath, 1024, "%s/%s", basePath, subPath);
            }
            directoryVector.push_back(StringTable->insert(szPath));
         }
         else
            directoryVector.push_back(StringTable->insert(basePath));
      }
   }

   //-----------------------------------------------------------------------------
   // Iterate through and grab valid directories
   //-----------------------------------------------------------------------------

   do
   {
      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         // skip . and .. directories
         if (dStrcmp(findData.cFileName, ".") == 0 || dStrcmp(findData.cFileName, "..") == 0)
            continue;

         // skip excluded directories
         if (Platform::isExcludedDirectory(findData.cFileName))
            continue;

         if ((subPath && (dStrncmp(subPath, "", 1) != 0)))
         {
            char child[1024];

            if (subTrail == '/')
               dSprintf(child, sizeof(child), "%s%s", subPath, findData.cFileName);
            else
               dSprintf(child, sizeof(child), "%s/%s", subPath, findData.cFileName);

            if (currentDepth < recurseDepth || recurseDepth == -1)
               recurseDumpDirectories(basePath, child, directoryVector, currentDepth + 1, recurseDepth, noBasePath);

         }
         else
         {
            char child[1024];

            if (trail == '/')
               dStrcpy(child, findData.cFileName);
            else
               dSprintf(child, sizeof(child), "/%s", findData.cFileName);

            if (currentDepth < recurseDepth || recurseDepth == -1)
               recurseDumpDirectories(basePath, child, directoryVector, currentDepth + 1, recurseDepth, noBasePath);
         }
      }
   } while (FindNextFileA(handle, &findData));

   FindClose(handle);
   return true;
}


bool Platform::dumpDirectories( const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath )
{
   ResourceManager->initExcludedDirectories();

   bool retVal = recurseDumpDirectories( path, "", directoryVector, -1, depth, noBasePath );

   clearExcludedDirectories();

   return retVal;
}

//-----------------------------------------------------------------------------

StringTableEntry Platform::osGetTemporaryDirectory()
{
//#pragma message("Implement UNICODE support for osGetTemporaryDirectory [01/23/2007 tom]" )

   char buf[512];
   DWORD len = GetTempPathA(sizeof(buf), buf);
   // Remove the trailing slash
   buf[len-1] = 0;

   forwardslash(buf);
   return StringTable->insert(buf);
}
