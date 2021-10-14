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
 
    Here's the scoop on unix file IO.  The windows platform makes some 
    assumptions about fileio: 1) the file system is case-insensitive, and 
    2) the platform can write to the directory in which
    the game is running.  Both of these are usually false on linux.  So, to 
    compensate, we "route" created files and directories to the user's home
    directory (see GetPrefPath()).  When a file is to be accessed, the code 
    looks in the home directory first.  If the file is not found there and the
    open mode is read only, the code will look in the game installation 
    directory.  Files are never created or modified in the game directory.
 
    For case-sensitivity, the MungePath code will test whether a given path
    specified by the engine exists.  If not, it will use the MungeCase function
    which will try to determine if an actual filesystem path matches the 
    specified path case insensitive.  If one is found, the actual path 
    transparently (we hope) replaces the one requested by the engine.
 
    The preference directory is global to all torque executables with the same
    name.  You should make sure you keep it clean if you build from multiple
    torque development trees.
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
 #include <utime.h>
 
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
 // copy a file from src to dest
 static bool CopyFile(const char* src, const char* dest)
 {
    S32 srcFd = x86UNIXOpen(src, O_RDONLY);
    S32 destFd = x86UNIXOpen(dest, O_WRONLY | O_CREAT | O_TRUNC);
    bool error = false;
 
    if (srcFd != -1 && destFd != -1)
    {
       const int BufSize = 8192;
       char buf[BufSize];
       S32 bytesRead = 0;
       while ((bytesRead = x86UNIXRead(srcFd, buf, BufSize)) > 0)
       {
          // write data
          if (x86UNIXWrite(destFd, buf, bytesRead) == -1)
          {
             error = true;
             break;
          }
       }
 
       if (bytesRead == -1)
          error = true;
    }
 
    if (srcFd != -1)
       x86UNIXClose(srcFd);
    if (destFd != -1)
       x86UNIXClose(destFd);
 
    if (error)
    {
       Con::errorf("Error copying file: %s, %s", src, dest);
       remove(dest);
    }
    return error;
 }

bool dPathCopy(const char *fromName, const char *toName, bool nooverwrite)
{
    return CopyFile(fromName,toName);
}
 
 //-----------------------------------------------------------------------------
 static char sgPrefDir[MaxPath];
 static bool sgPrefDirInitialized = false;
 
 // get the "pref dir", which is where game output files are stored.  the pref
 // dir is ~/PREF_DIR_ROOT/PREF_DIR_GAME_NAME
 static const char* GetPrefDir()
 {
    if (sgPrefDirInitialized)
       return sgPrefDir;
 
    if (x86UNIXState->getUseRedirect())
    {
       const char *home = getenv("HOME");
       AssertFatal(home, "HOME environment variable must be set");
 
       dSprintf(sgPrefDir, MaxPath, "%s/%s/%s", 
          home, PREF_DIR_ROOT, PREF_DIR_GAME_NAME);
    }
    else
    {
       getcwd(sgPrefDir, MaxPath);
    }
 
    sgPrefDirInitialized = true;
    return sgPrefDir;
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
 enum
 {
    TOUCH,
    DELETE
 };
 
 //-----------------------------------------------------------------------------
 // perform a modification on the specified file.  allowed modifications are 
 // specified in the enum above.
 bool ModifyFile(const char * name, S32 modType)
 {
    if(!name || (dStrlen(name) >= MAX_PATH) || dStrstr(name, "../") != NULL)
       return(false);
 
    // if its absolute skip it
    if (name[0]=='/' || name[0]=='\\')
       return(false);
 
    // only modify files in home directory
    char prefPathName[MaxPath];
    MungePath(prefPathName, MaxPath, name, GetPrefDir());
 
    if (modType == TOUCH)
       return(utime(prefPathName, 0) != -1);
    else if (modType == DELETE)
       return (remove(prefPathName) != -1);
    else 
       AssertFatal(false, "Unknown File Mod type");
    return false;
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
             rInfo.pFullPath = StringTable->insert(relativePath);
          else
             rInfo.pFullPath = StringTable->insert(path);
          rInfo.pFileName = StringTable->insert(fEntry->d_name);
          rInfo.fileSize  = fStat.st_size;
          //dPrintf("Adding file: %s/%s\n", rInfo.pFullPath, rInfo.pFileName);
       }
 
    } while( (fEntry = readdir(directory)) != NULL );
 
    closedir(directory);
    return true;
 }   
 
 //-----------------------------------------------------------------------------
 bool dFileDelete(const char * name)
 {
    return ModifyFile(name, DELETE);
 }
 
 //-----------------------------------------------------------------------------
 bool dFileTouch(const char * name)
 {
    return ModifyFile(name, TOUCH);
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
 
    char prefPathName[MaxPath];
    char gamePathName[MaxPath];
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
    MungePath(prefPathName, MaxPath, filename, GetPrefDir());
    MungePath(gamePathName, MaxPath, filename, cwd);
 
    int oflag;
    struct stat filestat;
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
          // if the file does not exist copy it before reading/writing
          if (stat(prefPathName, &filestat) == -1)
             bool ret = CopyFile(gamePathName, prefPathName);
          break;
       case WriteAppend:
          oflag = O_WRONLY | O_CREAT | O_APPEND;
          // if the file does not exist copy it before appending
          if (stat(prefPathName, &filestat) == -1)
              bool ret = CopyFile(gamePathName, prefPathName);
          break;
       default:
          AssertFatal(false, "File::open: bad access mode");    // impossible
    }
 
    // if we are writing, make sure output path exists
    if (openMode == Write || openMode == ReadWrite || openMode == WriteAppend)
        Platform::createPath(prefPathName);
 
    int fd = -1;
    fd = x86UNIXOpen(prefPathName, oflag);
    if (fd == -1 && openMode == Read)
       // for read only files we can use the gamePathName
       fd = x86UNIXOpen(gamePathName, oflag);
 
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
    char pathName[MaxPath];
 
    // if it starts with cwd, we need to strip that off so that we can look for
    // the file in the pref dir
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
    if (dStrstr(filePath, cwd) == filePath)
       filePath = filePath + dStrlen(cwd) + 1;
    
    // if its relative, first look in the pref dir
    if (filePath[0] != '/' && filePath[0] != '\\')
    {
       MungePath(pathName, MaxPath, filePath, GetPrefDir());
       if (GetFileTimes(pathName, createTime, modifyTime))
          return true;
    }
 
    // here if the path is absolute or not in the pref dir
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
 
    // all paths should be created in home directory
    char prefPathName[MaxPath];
    MungePath(prefPathName, MaxPath, file, GetPrefDir());
    file = prefPathName;
 
    // does the directory exist already?
    if (DirExists(prefPathName, true)) // true means that the path is a filepath
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
 
    // if it is not absolute, dump the pref dir first
    if (path[0] != '/' && path[0] != '\\')
    {
       char prefPathName[MaxPath];
       MungePath(prefPathName, MaxPath, path, GetPrefDir());
       RecurseDumpPath(prefPathName, path, pattern, fileVector, depth);
    }
 
    // munge the requested path and dump it
    char mungedPath[MaxPath];
    char cwd[MaxPath];
    getcwd(cwd, MaxPath);
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
   if (!Platform::isExcludedDirectory(subPath)){
      if (noBasePath && (subPath && (dStrncmp (subPath, "", 1) != 0)) ){
         // There is no base path to concatenate with, and this subpath is nonempty.
         // Store the subPath as a starting point.
         directoryVector.push_back(StringTable->insert(subPath));
      } else {
         // There is a base path. Store the concatenated path.
         directoryVector.push_back(StringTable->insert(Path));
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
   bool retVal = recurseDumpDirectories(path, "", directoryVector, 0, depth, noBasePath);
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
const char* Platform::getUserDataDirectory() 
{
	return StringTable->insert("~/");

}

//-----------------------------------------------------------------------------
const char* Platform::getUserHomeDirectory() 
{
	return StringTable->insert("~/");
}

//-----------------------------------------------------------------------------
StringTableEntry Platform::osGetTemporaryDirectory()
{
	return StringTable->insert("~/");
}

bool Platform::pathCopy(const char* source, const char* dest, bool nooverwrite)
{
   return false;
}

bool Platform::fileRename(const char* source, const char* dest)
{
   return false;
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

