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

 /* JMQ:
 
    Here's the scoop on unix file IO.  The windows platform makes an assumption
    about fileio that is usually false on linux: that the file system is
    case-insensitive.  The MungePath code compensates.  It will test whether a
    given path specified by the engine exists.  If not, it will use the MungeCase
    function, which will try to determine if an actual filesystem path matches
    the specified path case insensitive.  If one is found, the actual path
    transparently (we hope) replaces the one requested by the engine.
 
    It used to compensate for a second assumption as well -- that the platform
    can write to the directory the game is running in -- by routing every created
    file into a "pref dir" under the user's home, reading from there first and
    falling back to the game directory.  That was never switched on.  It was gated
    on USE_FILE_REDIRECT, which nothing in the tree ever defined, so the pref dir
    was only ever the working directory, and every path was built twice to the
    same place.  It was also cached at the first file open, which happens before
    the engine chdirs to the folder holding main.cs -- so a relative write such
    as console.log landed wherever the process was launched from rather than
    beside the game, which is not what Win32 or macOS do with the same name.
 
    A relative path is now resolved against the working directory as it stands at
    the call, as it is on every other desktop platform.  A file that genuinely
    belongs to the user rather than to the game has a portable way to say so:
    Platform::getPrefsPath, over Platform::getUserDataDirectory below.
 */
 
 #include "platformX86UNIX/x86UNIXState.h"
 // evil hack to get around insane X windows #define-happy header files
 #ifdef Status
 #undef Status
 #endif
 
 #include "platformX86UNIX/platformX86UNIX.h"
 #include "platform/platformFileIO.h"
 #include "collection/vector.h"
 #include "string/stringTable.h"
 #include "console/console.h"
 #include "io/resource/resourceManager.h"
 #include "game/gameInterface.h" 

 #if defined(__FreeBSD__)
    #include <sys/types.h>
 #endif
 
 /* include sys/param.h for MAXPATHLEN */
 #include <sys/param.h>
 #ifndef MAX_PATH
 #define MAX_PATH MAXPATHLEN
 #endif
 
 /* these are for reading directors, getting stats, etc. */
 #include <dirent.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <unistd.h>
 #include <pwd.h>
 #include <fcntl.h>
 #include <errno.h>
 #include <stdlib.h>
 
 extern int x86UNIXOpen(const char *path, int oflag);
 extern int x86UNIXClose(int fd);
 extern ssize_t x86UNIXRead(int fd, void *buf, size_t nbytes);
 extern ssize_t x86UNIXWrite(int fd, const void *buf, size_t nbytes);
 
 const int MaxPath = 2048;
 
 // Various handy utility functions:
 //------------------------------------------------------------------------------
 // find all \ in a path and convert them in place to /
 static void ForwardSlash(char *str)
 {
    while(*str)
    {
       if(*str == '\\')
          *str = '/';
       str++;
    }
 }

 //------------------------------------------------------------------------------
 // munge the case of the specified pathName.  This means try to find the actual
 // filename in with case-insensitive matching on the specified pathName, and
 // store the actual found name.
 static void MungeCase(char* pathName, S32 pathNameSize)
 {
    char tempBuf[MaxPath];
    dStrncpy(tempBuf, pathName, pathNameSize);
 
    AssertFatal(pathName[0] == '/', "PATH must be absolute");
 
    struct stat filestat;
    const int MaxPathEl = 200;
    char *currChar = pathName;
    char testPath[MaxPath];
    char pathEl[MaxPathEl];
    bool done = false;
 
    dStrncpy(tempBuf, "/", MaxPath);
    currChar++;
 
    while (!done)
    {
       char* termChar = dStrchr(currChar, '/');
       if (termChar == NULL)
          termChar = dStrchr(currChar, '\0');
       AssertFatal(termChar, "Can't find / or NULL terminator");
 
       S32 pathElLen = (termChar - currChar);
       dStrncpy(pathEl, currChar, pathElLen);
       pathEl[pathElLen] = '\0';
       dStrncpy(testPath, tempBuf, MaxPath);
       dStrcat(testPath, pathEl);
       if (stat(testPath, &filestat) != -1)
       {
          dStrncpy(tempBuf, testPath, MaxPath);
       }
       else
       {
          DIR *dir = opendir(tempBuf);
          struct dirent* ent;
          bool foundMatch = false;
          while (dir != NULL && (ent = readdir(dir)) != NULL)
          {
             if (dStricmp(pathEl, ent->d_name) == 0)
             {
                foundMatch = true;
                dStrcat(tempBuf, ent->d_name);
                break;
             }
          }
 
          if (!foundMatch)
             dStrncpy(tempBuf, testPath, MaxPath);
          if (dir)
             closedir(dir);
       }
       if (*termChar == '/')
       {
          dStrcat(tempBuf, "/");
          termChar++;
          currChar = termChar;
       }
       else
          done = true;
    }
 
    dStrncpy(pathName, tempBuf, pathNameSize);  
 }
 
 //-----------------------------------------------------------------------------
 // Returns true if the pathname exists, false otherwise.  If isFile is true,
 // the pathname is assumed to be a file path, and only the directory part
 // will be examined (everything before last /)
 bool DirExists(char* pathname, bool isFile)
 {
    static char testpath[20000];
    dStrncpy(testpath, pathname, sizeof(testpath));
    if (isFile)
    {
       // find the last / and make it into null
       char* lastSlash = dStrrchr(testpath, '/');
       if (lastSlash != NULL)
          *lastSlash = 0;
    }
    return Platform::isDirectory(testpath);
 }
 
 //-----------------------------------------------------------------------------
 // Munge the specified path.  
 static void MungePath(char* dest, S32 destSize, 
    const char* src, const char* absolutePrefix)
 {
    char tempBuf[MaxPath];
    dStrncpy(dest, src, MaxPath);
 
    // translate all \ to /
    ForwardSlash(dest);
 
    // if it is relative, make it absolute with the absolutePrefix
    if (dest[0] != '/')
    {
       AssertFatal(absolutePrefix, "Absolute Prefix must not be NULL");
 
       dSprintf(tempBuf, MaxPath, "%s/%s", 
          absolutePrefix, dest);
 
       // copy the result back into dest
       dStrncpy(dest, tempBuf, destSize);
    }
 
    // if the path exists, we're done
    struct stat filestat;
    if (stat(dest, &filestat) != -1)
       return;
 
    // otherwise munge the case of the path
    MungeCase(dest, destSize);
 }
 
 //-----------------------------------------------------------------------------
 static bool RecurseDumpPath(const char *path, const char* relativePath, const char *pattern, Vector<Platform::FileInfo> &fileVector, int recurseDepth) 
{
    char search[1024];
 
    dSprintf(search, sizeof(search), "%s", path, pattern);
    
    DIR *directory = opendir(search);
 
    if (directory == NULL)
       return false;
 
    struct dirent *fEntry;
    fEntry = readdir(directory);		// read the first "file" in the directory
 
    if (fEntry == NULL)
       return false;
 
    do
    {
       char filename[BUFSIZ+1];
       struct stat fStat;
 
       dSprintf(filename, sizeof(filename), "%s/%s", search, fEntry->d_name); // "construct" the file name
       stat(filename, &fStat); // get the file stats
 
       if ( (fStat.st_mode & S_IFMT) == S_IFDIR )
       {
          // Directory
          // skip . and .. directories
          if (dStrcmp(fEntry->d_name, ".") == 0 || dStrcmp(fEntry->d_name, "..") == 0)
             continue;
            
        // skip excluded directories
        if( Platform::isExcludedDirectory(fEntry->d_name))
            continue;
 
 
          char child[MaxPath];
          dSprintf(child, sizeof(child), "%s/%s", path, fEntry->d_name);
          char* childRelative = NULL;
          char childRelativeBuf[MaxPath];
          if (relativePath)
          {
             dSprintf(childRelativeBuf, sizeof(childRelativeBuf), "%s/%s", 
                relativePath, fEntry->d_name);
             childRelative = childRelativeBuf;
          }

          if (recurseDepth > 0)
        RecurseDumpPath(child, childRelative, pattern, fileVector, recurseDepth - 1);
      else if (recurseDepth == -1)
        RecurseDumpPath(child, childRelative, pattern, fileVector, -1);
       }      
       else
       {
          // File
          
          // add it to the list
          fileVector.increment();
          Platform::FileInfo& rInfo = fileVector.last();
 
          if (relativePath)
             rInfo.pFullPath = StringTable->insert(relativePath, true);
          else
             rInfo.pFullPath = StringTable->insert(path, true);
          rInfo.pFileName = StringTable->insert(fEntry->d_name, true);
          rInfo.fileSize  = fStat.st_size;
          //dPrintf("Adding file: %s/%s\n", rInfo.pFullPath, rInfo.pFileName);
       }
 
    } while( (fEntry = readdir(directory)) != NULL );
 
    closedir(directory);
    return true;
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
 //    AssertFatal(sizeof(int) == sizeof(void *), "File::File: cannot cast void* to int");
 
     handle = (void *)NULL;
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
     handle = (void *)NULL;
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
    AssertFatal(NULL != filename, "File::open: NULL filename");
    AssertWarn(NULL == handle, "File::open: handle already valid");
    
    // Close the file if it was already open...
    if (Closed != currentStatus)
       close();
 
    // One path, against the working directory as it stands at the call. Win32
    // hands the name to CreateFile and macOS to fopen, both of which do this.
    char pathName[MaxPath];
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
    MungePath(pathName, MaxPath, filename, cwd);
 
    int oflag;
    handle = (void *)dRealMalloc(sizeof(int));
 
    switch (openMode)
    {
       case Read:
          oflag = O_RDONLY;
          break;
       case Write:
          oflag = O_WRONLY | O_CREAT | O_TRUNC;
          break;
       case ReadWrite:
          oflag = O_RDWR | O_CREAT;
          break;
       case WriteAppend:
          oflag = O_WRONLY | O_CREAT | O_APPEND;
          break;
       default:
          AssertFatal(false, "File::open: bad access mode");    // impossible
    }
 
    // if we are writing, make sure output path exists
    if (openMode == Write || openMode == ReadWrite || openMode == WriteAppend)
        Platform::createPath(pathName);
 
    int fd = x86UNIXOpen(pathName, oflag);
 
    dMemcpy(handle, &fd, sizeof(int));
     
 #ifdef DEBUG
 //   fprintf(stdout,"fd = %d, handle = %d\n", fd, *((int *)handle));
 #endif
 
    if (*((int *)handle) == -1)                
    {
       // handle not created successfully
       Con::errorf("Can't open file: %s", filename);
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
     AssertFatal(NULL != handle, "File::getPosition: invalid file handle");
 
 #ifdef DEBUG
 //   fprintf(stdout, "handle = %d\n",*((int *)handle));fflush(stdout);
 #endif
     return (U32) lseek(*((int *)handle), 0, SEEK_CUR);
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
     AssertFatal(NULL != handle, "File::setPosition: invalid file handle");
     
     if (Ok != currentStatus && EOS != currentStatus)
         return currentStatus;
     
     U32 finalPos = 0;
     switch (absolutePos)
     {
     case true:                                                    // absolute position
         AssertFatal(0 <= position, "File::setPosition: negative absolute position");
         
         // position beyond EOS is OK
         finalPos = lseek(*((int *)handle), position, SEEK_SET);
         break;
     case false:                                                    // relative position
         AssertFatal((getPosition() >= (U32)abs(position) && 0 > position) || 0 <= position, "File::setPosition: negative relative position");
         
         // position beyond EOS is OK
         finalPos = lseek(*((int *)handle), position, SEEK_CUR);
    break;
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
     AssertFatal(NULL != handle, "File::getSize: invalid file handle");
     
     if (Ok == currentStatus || EOS == currentStatus)
     {
    long currentOffset = getPosition();                  // keep track of our current position
    long fileSize;
    lseek(*((int *)handle), 0, SEEK_END);                     // seek to the end of the file
    fileSize = getPosition();                               // get the file size
    lseek(*((int *)handle), currentOffset, SEEK_SET);         // seek back to our old offset
         return fileSize;                                        // success!
     }
     else
         return 0;                                               // unsuccessful
 }
 
 //-----------------------------------------------------------------------------
 // Flush the file.
 // It is an error to flush a read-only file.
 // Returns the currentStatus of the file.
 //-----------------------------------------------------------------------------
 File::Status File::flush()
 {
     AssertFatal(Closed != currentStatus, "File::flush: file closed");
     AssertFatal(NULL != handle, "File::flush: invalid file handle");
     AssertFatal(true == hasCapability(FileWrite), "File::flush: cannot flush a read-only file");
 
     if (fsync(*((int *)handle)) == 0)
         return currentStatus = Ok;                                // success!
     else
         return setStatus();                                       // unsuccessful
 }
 
 //-----------------------------------------------------------------------------
 // Close the File.
 //
 // Returns the currentStatus
 //-----------------------------------------------------------------------------
 File::Status File::close()
 {
    // if the handle is non-NULL, close it if necessary and free it
    if (NULL != handle)
    {
       // make a local copy of the handle value and
       // free the handle
       int handleVal = *((int *)handle);
       dRealFree(handle);
       handle = (void *)NULL;
       
       // close the handle if it is valid
       if (handleVal != -1 && x86UNIXClose(handleVal) != 0)
          return setStatus();                                    // unsuccessful
    }
    // Set the status to closed
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
    Con::printf("File IO error: %s", strerror(errno));
    return currentStatus = IOError;
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
 #ifdef DEBUG
 //   fprintf(stdout,"reading %d bytes\n",size);fflush(stdout);
 #endif
     AssertFatal(Closed != currentStatus, "File::read: file closed");
     AssertFatal(NULL != handle, "File::read: invalid file handle");
     AssertFatal(NULL != dst, "File::read: NULL destination pointer");
     AssertFatal(true == hasCapability(FileRead), "File::read: file lacks capability");
     AssertWarn(0 != size, "File::read: size of zero");
 
 /* show stats for this file */
 #ifdef DEBUG
 //struct stat st;
 //fstat(*((int *)handle), &st);
 //fprintf(stdout,"file size = %d\n", st.st_size);
 #endif
 /****************************/
     
     if (Ok != currentStatus || 0 == size)
         return currentStatus;
     else
     {
         long lastBytes;
         long *bytes = (NULL == bytesRead) ? &lastBytes : (long *)bytesRead;
         if ( (*((U32 *)bytes) = x86UNIXRead(*((int *)handle), dst, size)) == -1)
         {
 #ifdef DEBUG
 //   fprintf(stdout,"unsuccessful: %d\n", *((U32 *)bytes));fflush(stdout);
 #endif
            return setStatus();                                    // unsuccessful
         } else {
 //            dst[*((U32 *)bytes)] = '\0';
             if (*((U32 *)bytes) != size || *((U32 *)bytes) == 0) {
 #ifdef DEBUG
 //  fprintf(stdout,"end of stream: %d\n", *((U32 *)bytes));fflush(stdout);
 #endif
                 return currentStatus = EOS;                        // end of stream
             }
         }
     }
 //    dst[*bytesRead] = '\0';
 #ifdef DEBUG
 //fprintf(stdout, "We read:\n");
 //fprintf(stdout, "====================================================\n");
 //fprintf(stdout, "%s\n",dst);
 //fprintf(stdout, "====================================================\n");
 //fprintf(stdout,"read ok: %d\n", *bytesRead);fflush(stdout);
 #endif
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
    // JMQ: despite the U32 parameters, the maximum filesize supported by this
    // function is probably the max value of S32, due to the unix syscall
    // api.
    AssertFatal(Closed != currentStatus, "File::write: file closed");
    AssertFatal(NULL != handle, "File::write: invalid file handle");
    AssertFatal(NULL != src, "File::write: NULL source pointer");
    AssertFatal(true == hasCapability(FileWrite), "File::write: file lacks capability");
    AssertWarn(0 != size, "File::write: size of zero");
     
    if ((Ok != currentStatus && EOS != currentStatus) || 0 == size)
       return currentStatus;
    else
    {
       S32 numWritten = x86UNIXWrite(*((int *)handle), src, size);
       if (numWritten < 0)
          return setStatus();
 
       if (bytesWritten)
          *bytesWritten = static_cast<U32>(numWritten);
       return currentStatus = Ok;
    }
 }
 
 //-----------------------------------------------------------------------------
 // Self-explanatory.  JMQ: No explanation needed.  Move along.  These aren't 
 // the droids you're looking for.
 //-----------------------------------------------------------------------------
 bool File::hasCapability(Capability cap) const
 {
     return (0 != (U32(cap) & capability));
 }
 
 //-----------------------------------------------------------------------------
 S32 Platform::compareFileTimes(const FileTime &a, const FileTime &b)
 {
    if(a > b)
       return 1;
    if(a < b)
       return -1;
    return 0;
 }
 
 //-----------------------------------------------------------------------------
 static bool GetFileTimes(const char *filePath, FileTime *createTime, FileTime *modifyTime)
 {
    struct stat fStat;
 
    if (stat(filePath, &fStat) == -1)
       return false;
 
    if(createTime)
    {
       // no where does SysV/BSD UNIX keep a record of a file's
       // creation time.  instead of creation time I'll just use
       // changed time for now.
       *createTime = fStat.st_ctime;
    }
    if(modifyTime)
    {
       *modifyTime = fStat.st_mtime;
    }
 
    return true;
 }
 
 //-----------------------------------------------------------------------------
 bool Platform::getFileTimes(const char *filePath, FileTime *createTime, FileTime *modifyTime)
 {
    // Resolved against the working directory, so a relative path is answered for
    // where the game is actually running.
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
 
    char pathName[MaxPath];
    MungePath(pathName, MaxPath, filePath, cwd);
    return GetFileTimes(pathName, createTime, modifyTime);
 }
 
 //-----------------------------------------------------------------------------
 bool Platform::createPath(const char *file)
 {
    char pathbuf[MaxPath];
    const char *dir;
    pathbuf[0] = 0;
    U32 pathLen = 0;
 
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
 
    char pathName[MaxPath];
    MungePath(pathName, MaxPath, file, cwd);
    file = pathName;
 
    // does the directory exist already?
    if (DirExists(pathName, true)) // true means that the path is a filepath
       return true;
    
    while((dir = dStrchr(file, '/')) != NULL)
    {
       dStrncpy(pathbuf + pathLen, file, dir - file);
       pathbuf[pathLen + dir-file] = 0;
       bool ret = mkdir(pathbuf, 0700);
       pathLen += dir - file;
       pathbuf[pathLen++] = '/';
       file = dir + 1;
    }
    return true;
 }
 
 // JMQ: Platform:cdFileExists in unimplemented
 //------------------------------------------------------------------------------
 // bool Platform::cdFileExists(const char *filePath, const char *volumeName, 
 //    S32 serialNum)
 // {
 // }
 
 //-----------------------------------------------------------------------------
 bool Platform::dumpPath(const char *path, Vector<Platform::FileInfo> &fileVector, int depth)
 {
    const char* pattern = "*";
 
    // Resolved against the working directory as it stands at the call, which is
    // how Win32 and macOS resolve a relative name.
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
 
    char mungedPath[MaxPath];
    MungePath(mungedPath, MaxPath, path, cwd);
    return RecurseDumpPath(mungedPath, path, pattern, fileVector, depth);
 }
 
 //-----------------------------------------------------------------------------
 bool Platform::isFile(const char *pFilePath)
 {
    if (!pFilePath || !*pFilePath)
       return false;
    // Get file info
    struct stat fStat;
    if (stat(pFilePath, &fStat) < 0)
       return false;
 
    // if the file is a "regular file" then true
    if ( (fStat.st_mode & S_IFMT) == S_IFREG)
       return true;
    // must be some other file (directory, device, etc.)
    return false;
 }
 
 //-----------------------------------------------------------------------------
 S32 Platform::getFileSize(const char *pFilePath)
 {
   if (!pFilePath || !*pFilePath)
     return -1;
   // Get the file info
   struct stat fStat;
   if (stat(pFilePath, &fStat) < 0)
     return -1;
   // if the file is a "regular file" then return the size
   if ( (fStat.st_mode & S_IFMT) == S_IFREG)
     return fStat.st_size;
   // Must be something else or we can't read the file.
   return -1;
 }
 
 //-----------------------------------------------------------------------------
 bool Platform::isDirectory(const char *pDirPath)
 {
    if (!pDirPath || !*pDirPath)
       return false;
  
    // Get file info
    struct stat fStat;
    if (stat(pDirPath, &fStat) < 0)
       return false;
       
    // if the file is a Directory then true
    if ( (fStat.st_mode & S_IFMT) == S_IFDIR)
       return true;
 
    return false;
 }
 
 //-----------------------------------------------------------------------------
 bool Platform::isSubDirectory(const char *pParent, const char *pDir)
 {
    if (!pParent || !*pDir)
       return false;
    
    // this is somewhat of a brute force method but we need to be 100% sure
    // that the user cannot enter things like ../dir or /dir etc,...
    DIR *directory;
 
    directory = opendir(pParent);
    if (directory == NULL)
       return false;
 
    struct dirent *fEntry;
    fEntry = readdir(directory);
    if ( fEntry == NULL )
       return false;
 
    do
    {
       char dirBuf[MAXPATHLEN];
       struct stat fStat;
 
       dSprintf(dirBuf, sizeof(dirBuf), "%s/%s", pParent, fEntry->d_name);
       if (stat(dirBuf, &fStat) < 0)
          continue;
       // if it is a directory...
       if ( (fStat.st_mode & S_IFMT) == S_IFDIR)
       {
          // and the names match
          if (dStrcmp(pDir, fEntry->d_name ) == 0)
          {
             // then we have a real sub directory
             closedir(directory);
             return true;
          }
       }
    } while( (fEntry = readdir(directory)) != NULL );
    
    closedir(directory);
    return false;
 }
 
 //-----------------------------------------------------------------------------
 
  bool Platform::hasSubDirectory(const char *pPath)
 {
   if (!pPath)
     return false;
   ResourceManager->initExcludedDirectories();
   
   struct dirent *d;
   DIR           *dip;
   dip = opendir(pPath);
   if (dip == NULL)
     return false;
 
   while (d = readdir(dip))
     {
    bool isDir = false;
    if (d->d_type == DT_UNKNOWN) 
    {
        char child [1024];
        if ((pPath[dStrlen(pPath) - 1] == '/'))
            dSprintf(child, 1024, "%s%s", pPath, d->d_name);
        else
            dSprintf(child, 1024, "%s/%s", pPath, d->d_name);
        isDir = Platform::isDirectory (child);
    }
    else if (d->d_type & DT_DIR)
        isDir = true;
        if( isDir )
        {
        // Skip the . and .. directories
        if (dStrcmp(d->d_name, ".") == 0 ||dStrcmp(d->d_name, "..") == 0)
            continue;
        if (Platform::isExcludedDirectory(d->d_name))
            continue;
          Platform::clearExcludedDirectories();
          closedir(dip);
          return true;
    }
     }
   closedir(dip);
   Platform::clearExcludedDirectories();
   return false;
 }
 
 static bool recurseDumpDirectories(const char *basePath, const char *subPath, Vector<StringTableEntry> &directoryVector, S32 currentDepth, S32 recurseDepth, bool noBasePath)
 {
   // Assemble a path to open.
   char Path[1024];
   dMemset(Path, 0, 1024);

   // Check if subPath is not null, and not an empty string. 
   if (subPath && (dStrncmp(subPath, "", 1) != 0)){
      // Concatenate it onto the basepath, accounting for if the basepath ends in a / or not
       if ((basePath[dStrlen(basePath) - 1]) == '/')
         dSprintf(Path, 1024, "%s%s", basePath, subPath);
       else
         dSprintf(Path, 1024, "%s/%s", basePath, subPath);
   } else {
      // subPath is null or an empty string.
      // Just look in the basePath then.
      dSprintf(Path, 1024, "%s", basePath);
   }

   // Open the sanitized path constructed above.
   DIR *dip;
   dip = opendir(Path);

   // Quit if it didn't open.
   if (dip == NULL)
     return false;

   // Add path to our return list (provided it is valid).
   //
   // Interned CASE SENSITIVELY, and that is load bearing on this platform. The
   // string table's hash is case insensitive by construction, so an ordinary
   // insert returns whichever spelling of a name reached the table first -- and
   // several spellings get there during static initialisation, before any of
   // this runs: SpriteBatch.cc interns "Sprites" as a taml node name and
   // guiProfileTheme.cc interns "Fonts" as a field group. The result was that a
   // directory genuinely called sprites or fonts came back out of readdir as
   // "Sprites" or "Fonts", and every caller that then tried to open it on a case
   // sensitive filesystem failed: deleteDirectory could not recurse into it,
   // getDirectoryList reported a name nothing could stat, and scanModules
   // skipped it.
   //
   // The file names in RecurseDumpPath above are interned the same way, and the
   // two have to stay in step with ResManager::getPaths and the ResDictionary:
   // that dictionary hashes by pointer value, so a half-applied change misses
   // the bucket rather than merely comparing false.
   if (!Platform::isExcludedDirectory(subPath)){
      if (noBasePath){
         // No base path requested: store only non-empty subpaths, and NEVER the
         // base directory itself. This matches the Win32 back-end and is what
         // getDirectoryList() relies on to return immediate child names — the old
         // code fell into the else below for the empty-subPath root call and
         // pushed the base path, so getDirectoryList() returned just the path.
         if (subPath && (dStrncmp(subPath, "", 1) != 0))
            directoryVector.push_back(StringTable->insert(subPath, true));
      } else {
         // There is a base path. Store the concatenated path.
         directoryVector.push_back(StringTable->insert(Path, true));
      }
   }

   // Iterate through the items in the current directory.
   struct dirent *d;
   while (d = readdir(dip)) {
      bool	isDir;
      isDir = false;
      if (d->d_type == DT_UNKNOWN) {
         // If it's an unknown type, try to construct it as a path.
         char child [1024];

         // Use "/" as a separator correctly.
         if ((Path[dStrlen(Path) - 1] == '/'))
            dSprintf(child, 1024, "%s%s", Path, d->d_name);
         else
            dSprintf(child, 1024, "%s/%s", Path, d->d_name);
         
         // Now ask the platform if the constructed path is a directory.
         isDir = Platform::isDirectory(child);
      }
      else if (d->d_type & DT_DIR){
         // If it's a directory, cool.
         isDir = true;
      }

      // If the current directory item is itself a directory, iterate into it.
      if (isDir){
         // Bail for current directory or parent directory shorthands.
         if (dStrcmp(d->d_name, ".") == 0 || dStrcmp(d->d_name, "..") == 0)
            continue;

         // Bail for excluded directories.
         if (Platform::isExcludedDirectory(d->d_name))
            continue;

         // Now recurse into this directory.
         if ( (subPath && (dStrncmp(subPath, "", 1) != 0))){
            // The subpath we're basing on isn't an empty string.

            // To recurse down, cat the child directory's name with the current subpath.
            char child[1024];

            // Use "/" as a separator correctly.
            if ((subPath[dStrlen(subPath) - 1] == '/'))
               dSprintf(child, 1024, "%s%s", subPath, d->d_name);
            else
               dSprintf(child, 1024, "%s/%s", subPath, d->d_name);

            // If we haven't recursed too far, recurse once more with the synthesized child pathname.
            if (currentDepth < recurseDepth || recurseDepth == -1 ){
               recurseDumpDirectories(basePath, child, directoryVector, currentDepth + 1, recurseDepth, noBasePath);
            }

         } else {
            // The subpath we're basing on is an empty string.
            // Just name sure that we're not duplicating a "/" by concatenating the child directory with the base path.
            char child[1024];
            if ( (basePath[dStrlen(basePath) - 1]) == '/'){
               dStrcpy (child, d->d_name);
            }
            else{
               dSprintf(child, 1024, "/%s", d->d_name);
            }

            // If we haven't recursed too far, recurse once more with the synthesized child pathname.
            if (currentDepth < recurseDepth || recurseDepth == -1){
               recurseDumpDirectories(basePath, child, directoryVector, currentDepth + 1, recurseDepth, noBasePath);
            }
         }
      }
   }

   // Close this directory.
   closedir(dip);

   // End recursive calls.
   return true;
}
 
 bool Platform::dumpDirectories(const char *path, Vector<StringTableEntry> &directoryVector, S32 depth, bool noBasePath)
 {
   ResourceManager->initExcludedDirectories();
   // Start the recursion at currentDepth = -1 (NOT 0) to match the Win32 back-end:
   // the child-recursion guard is `currentDepth < recurseDepth`, so with the common
   // depth==0 call (e.g. getDirectoryList) a start of 0 gives `0 < 0` == false and
   // descends into NO children, returning an empty/just-base list. Starting at -1
   // makes depth==0 enumerate the immediate child directories as intended.
   bool retVal = recurseDumpDirectories(path, "", directoryVector, -1, depth, noBasePath);
   clearExcludedDirectories();
   return retVal;
 }
 
StringTableEntry Platform::getExecutableName()
{
    static StringTableEntry cwd = NULL;

    if (!cwd)
    {
        cwd = StringTable->insert(x86UNIXState->getExeName());
    }
    return cwd;

}

//-----------------------------------------------------------------------------
void Platform::restartInstance()
{
        // execl() leaves open file descriptors open, that's the main reason it's not
        // used here. We want to start fresh.

        if (Game->isRunning() )
        {
                Con::errorf("The game is still running, we cant relaunch now!");
                return;
        }

        char cmd[MaxPath];
        sprintf(cmd, "\"%s &\"", x86UNIXState->getExePathName());
        Con::printf("---- %s -----",cmd);
        if(!system(cmd))
        {
                Con::errorf("Cannot fork new instance.");
                return;
        }

        exit(0);
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::getCurrentDirectory()
{
   // get the current directory, the one that would be opened if we did a fopen(".")
   char* cwd = getcwd(NULL, 0);
   StringTableEntry ret = StringTable->insert(cwd);
   free(cwd);
   return ret;
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::getExecutablePath()
{
   // No obvious ways to find this with what we have
   return Platform::getCurrentDirectory();
}

//-----------------------------------------------------------------------------
bool Platform::setCurrentDirectory(StringTableEntry newDir)
{
   return (chdir(newDir) == 0);
}

//-----------------------------------------------------------------------------
// Where a per-user file belongs.
//
// These three used to answer "~/", which names a home directory only to a
// shell. POSIX does not expand a tilde -- to open(2) it is an ordinary
// character -- so the string reached MungePath above with a first character
// that is not '/', was judged relative, and was hung off the current
// directory. What that produced was a directory literally named "~" inside
// whatever folder the executable was launched from, holding the preferences of
// every project run out of it. The same calls answer with %APPDATA% on Windows
// and ~/Library/Application Support on macOS.
//
// The Linux equivalent is the XDG Base Directory specification, which the rest
// of the desktop already follows: an environment variable when the user has
// set one, a well-known default when they have not. A variable holding a
// relative path is ignored rather than honoured -- the spec asks for that, and
// it is also what stops a stray value from landing us back where we started.
//-----------------------------------------------------------------------------

// Defined further down, beside the file copy that needs it: every directory
// component of a path before its last '/'.
static void CreateParentDirectories(const char* path);

// A directory name without a trailing slash, because every caller here appends
// its own separator. Never eats a lone "/".
static void StripTrailingSlashes(char* path)
{
   S32 length = dStrlen(path);
   while (length > 1 && path[length - 1] == '/')
      path[--length] = '\0';
}

// CreateParentDirectories makes every component before the last '/', so a
// trailing slash is what makes the named directory itself one of them.
static void CreateDirectoryPath(const char* directory)
{
   char buffer[MaxPath];
   dSprintf(buffer, MaxPath, "%s/", directory);
   CreateParentDirectories(buffer);
}

// The user's home, from the environment or -- when that has been stripped, as
// it is for a service or a bare su -- from the passwd database.
static const char* GetHomeDirectory()
{
   const char* home = getenv("HOME");
   if (home != NULL && home[0] == '/')
      return home;

   struct passwd* entry = getpwuid(getuid());
   if (entry != NULL && entry->pw_dir != NULL && entry->pw_dir[0] == '/')
      return entry->pw_dir;

   return NULL;
}

// An XDG base directory: $<variable> when it holds an absolute path, otherwise
// the given default below the home directory. False means there is no home to
// fall back on, which leaves each caller to decide what that costs it.
static bool GetXDGDirectory(const char* variable, const char* fallback,
   char* dest, S32 destSize)
{
   const char* value = getenv(variable);
   if (value != NULL && value[0] == '/')
      dStrncpy(dest, value, destSize - 1);
   else
   {
      const char* home = GetHomeDirectory();
      if (home == NULL)
         return false;

      dSprintf(dest, destSize, "%s/%s", home, fallback);
   }

   dest[destSize - 1] = '\0';
   StripTrailingSlashes(dest);
   return true;
}

//-----------------------------------------------------------------------------
const char* Platform::getUserDataDirectory()
{
   static StringTableEntry sUserDataDirectory = NULL;
   if (sUserDataDirectory != NULL)
      return sUserDataDirectory;

   char path[MaxPath];
   if (!GetXDGDirectory("XDG_DATA_HOME", ".local/share", path, MaxPath))
   {
      // Nowhere to write but where we are: the old behaviour, minus the "~"
      // folder, and where every relative path already goes.
      sUserDataDirectory = Platform::getCurrentDirectory();
      return sUserDataDirectory;
   }

   // Handed back ready to be written into, because Windows and macOS both
   // create theirs and callers were written against a directory that exists.
   CreateDirectoryPath(path);

   sUserDataDirectory = StringTable->insert(path);
   return sUserDataDirectory;
}

//-----------------------------------------------------------------------------
const char* Platform::getUserHomeDirectory() 
{
   static StringTableEntry sUserHomeDirectory = NULL;
   if (sUserHomeDirectory != NULL)
      return sUserHomeDirectory;

   // Windows and macOS answer this one with the Documents folder. Linux has no
   // folder reliably there under any name -- ~/Documents exists only if the
   // user's desktop made it, and is called something else in another language
   // -- so the home directory itself is both the honest answer and the one the
   // function's name promises.
   const char* home = GetHomeDirectory();

   sUserHomeDirectory = (home != NULL)
      ? StringTable->insert(home)
      : Platform::getCurrentDirectory();

   return sUserHomeDirectory;
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::osGetTemporaryDirectory()
{
   static StringTableEntry sTemporaryDirectory = NULL;
   if (sTemporaryDirectory != NULL)
      return sTemporaryDirectory;

   // $TMPDIR is what POSIX tells a program to honour; /tmp is what is there
   // when nobody has said otherwise. Note that XDG_RUNTIME_DIR is deliberately
   // not consulted: it is wiped when the session ends, which is a promise this
   // engine's temporary files do not need and users would not expect.
   const char* temporary = getenv("TMPDIR");
   if (temporary == NULL || temporary[0] != '/')
      temporary = "/tmp";

   char path[MaxPath];
   dStrncpy(path, temporary, MaxPath - 1);
   path[MaxPath - 1] = '\0';
   StripTrailingSlashes(path);

   sTemporaryDirectory = StringTable->insert(path);
   return sTemporaryDirectory;
}

//-----------------------------------------------------------------------------
// Copying, which the editors lean on harder than the name suggests: it is how a
// new project is stamped out of a template, and how a theme is given its own
// copy of the stock cursor art.
//
// Paths are used as handed over, like isFile and isDirectory and fileDelete
// above, rather than sent through MungePath the way createPath and File::open
// are. Every caller builds an absolute path first and then asks isFile whether
// the copy arrived, so a copy that landed anywhere else would read as a failure
// -- and MungePath leaves an absolute path alone in any case.
//-----------------------------------------------------------------------------

static const U32 sCopyBufferSize = 32768;

// The directories leading up to a file, made on the path exactly as given.
// Platform::createPath is not usable here: it sends the path through MungePath
// first, whose case-fixing can substitute a directory that already exists for
// the one that was named, so it can make the parents somewhere other than where
// the copy is then written. It also makes them 0700, where a copied tree should
// follow the umask like the files in it do.
static void CreateParentDirectories(const char* path)
{
   char buffer[MaxPath];
   dStrncpy(buffer, path, MaxPath - 1);
   buffer[MaxPath - 1] = '\0';

   for (char* walk = dStrchr(buffer, '/'); walk != NULL; walk = dStrchr(walk + 1, '/'))
   {
      if (walk == buffer)
         continue;   // the leading slash of an absolute path

      *walk = '\0';
      mkdir(buffer, 0777);
      *walk = '/';
   }
}

static bool CopyOneFile(const char* fromName, const char* toName, bool nooverwrite)
{
   if (nooverwrite && (Platform::isFile(toName) || Platform::isDirectory(toName)))
      return false;

   struct stat fromStat;
   if (stat(fromName, &fromStat) < 0)
      return false;

   // The destination's folder may not exist yet -- copying a tree creates the
   // directories as it walks, but a lone file copied into a new folder does not.
   CreateParentDirectories(toName);

   S32 fromFd = open(fromName, O_RDONLY);
   if (fromFd < 0)
      return false;

   // Carry the mode across, so a copied executable is still executable.
   S32 toFd = open(toName, O_WRONLY | O_CREAT | O_TRUNC, fromStat.st_mode & 0777);
   if (toFd < 0)
   {
      close(fromFd);
      return false;
   }

   char buffer[sCopyBufferSize];
   bool ok = true;
   for (;;)
   {
      const ssize_t got = read(fromFd, buffer, sizeof(buffer));
      if (got == 0)
         break;
      if (got < 0)
      {
         if (errno == EINTR)
            continue;
         ok = false;
         break;
      }

      ssize_t written = 0;
      while (written < got)
      {
         const ssize_t put = write(toFd, buffer + written, got - written);
         if (put < 0)
         {
            if (errno == EINTR)
               continue;
            ok = false;
            break;
         }
         written += put;
      }

      if (!ok)
         break;
   }

   close(fromFd);
   if (close(toFd) < 0)
      ok = false;

   // A half-written file is worse than none: the next run would find it with
   // isFile and take it for good art.
   if (!ok)
      unlink(toName);

   return ok;
}

static bool CopyOneDirectory(const char* fromName, const char* toName)
{
   DIR* dir = opendir(fromName);
   if (dir == NULL)
      return false;

   struct stat fromStat;
   if (stat(fromName, &fromStat) == 0)
      mkdir(toName, fromStat.st_mode & 0777);
   else
      mkdir(toName, 0700);

   bool ok = true;
   struct dirent* entry;
   while ((entry = readdir(dir)) != NULL)
   {
      if (dStrcmp(entry->d_name, ".") == 0 || dStrcmp(entry->d_name, "..") == 0)
         continue;

      char fromChild[MaxPath];
      char toChild[MaxPath];
      dSprintf(fromChild, sizeof(fromChild), "%s/%s", fromName, entry->d_name);
      dSprintf(toChild, sizeof(toChild), "%s/%s", toName, entry->d_name);

      // Asking the filesystem rather than trusting d_type, which is DT_UNKNOWN
      // on filesystems that do not carry the kind in the directory entry.
      if (Platform::isDirectory(fromChild))
      {
         if (!CopyOneDirectory(fromChild, toChild))
            ok = false;
      }
      else
      {
         // Overwriting freely: the caller's nooverwrite was already answered
         // against the top of the tree, and stopping here would leave a
         // half-copied project behind.
         if (!CopyOneFile(fromChild, toChild, false))
            ok = false;
      }
   }

   closedir(dir);
   return ok;
}

bool Platform::pathCopy(const char* source, const char* dest, bool nooverwrite)
{
   if (source == NULL || dest == NULL || !*source || !*dest)
      return false;

   if (Platform::isFile(source))
      return CopyOneFile(source, dest, nooverwrite);

   if (Platform::isDirectory(source))
   {
      if (nooverwrite && (Platform::isDirectory(dest) || Platform::isFile(dest)))
         return false;

      // Refuse to copy a tree into itself, which would recurse until the path
      // outgrew MaxPath. Platform::isSubDirectory is no help here: it matches a
      // bare child name against the parent's entries, not one path inside
      // another.
      dsize_t sourceLen = dStrlen(source);
      while (sourceLen > 1 && source[sourceLen - 1] == '/')
         sourceLen--;   // a trailing slash would put dest past the comparison

      if (dStrncmp(source, dest, sourceLen) == 0 &&
          (dest[sourceLen] == '/' || dest[sourceLen] == '\0'))
      {
         Con::errorf("Platform::pathCopy: %s is inside %s", dest, source);
         return false;
      }

      CreateParentDirectories(dest);
      return CopyOneDirectory(source, dest);
   }

   Con::errorf("Platform::pathCopy: nothing to copy at %s", source);
   return false;
}

bool Platform::fileRename(const char* source, const char* dest)
{
   if (source == NULL || dest == NULL || !*source || !*dest)
      return false;

   if (!Platform::isFile(source) && !Platform::isDirectory(source))
   {
      Con::errorf("Platform::fileRename: no file exists at %s", source);
      return false;
   }

   if (Platform::isFile(dest) || Platform::isDirectory(dest))
      Con::warnf("Platform::fileRename: overwriting %s", dest);

   CreateParentDirectories(dest);

   if (rename(source, dest) == 0)
      return true;

   // rename cannot cross a filesystem, and source and destination are not always
   // on the same one -- a project stamped out of a template that lives on another
   // mount, say. Fall back to moving it by hand.
   if (errno != EXDEV)
      return false;

   if (!Platform::pathCopy(source, dest, false))
      return false;

   if (Platform::isDirectory(source))
      return Platform::deleteDirectory(source);

   return Platform::fileDelete(source);
}

bool Platform::fileDelete(const char* name)
{
   if (!name)
      return false;

   if (dStrlen(name) > MaxPath) {
      Con::warnf("Platform::fileDelete() - Filename is pretty long...");
   }

   return (remove(name) == 0);
  return false;
}

void Platform::openFolder(const char* path)
{
}

