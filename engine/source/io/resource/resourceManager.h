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

#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif
#ifndef _STRINGTABLE_H_
#include "string/stringTable.h"
#endif

#ifndef _FILESTREAM_H_
#include "io/fileStream.h"
#endif
#ifndef _CRC_H_
#include "algorithm/crc.h"
#endif

class Stream;
class FileStream;
class ZipSubRStream;
class ResManager;
class FindMatch;

namespace Zip
{
   class ZipArchive;
   class CentralDir;
}

extern ResManager *ResourceManager;

//------------------------------------------------------------------------------
class ResourceObject;

/// The base class for all resources.
///
/// This must be subclassed to implement a resource that can be managed.
///
/// Creating a new resource type is very simple. First, you need a function
/// which can create a new instance of the resource given a stream:
///
/// @code
/// ResourceInstance* constructBitmapPNG(Stream &stream)
/// {
///    GBitmap *bmp = new GBitmap;
///    if(bmp->readPNG(stream))
///       return bmp;
///    else
///    {
///       delete bmp;
///       return NULL;
///    }
/// }
/// @endcode
///
/// Then you need to register the extension of your resource type with the resource manager:
///
/// @code
/// ResourceManager->registerExtension(".png", constructBitmapPNG);
/// @endcode
///
/// And, of course, you need to provide a subclass of ResourceInstance:
///
/// @code
/// class GBitmap : ResourceInstance {
///    ... whatever you need for your resource goes in here ...
/// }
/// @endcode
///
/// ResourceInstance imposes very few requirements on you as the resource type
/// implementor. All you "have" to provide is a destructor to deallocate whatever
/// storage your resource might allocate. The ResourceManager will ensure that
/// there is only one instance of the ResourceInstance for each file, and that it
/// is only destroyed when no more references to it remain.
///
/// @see TerrainFile, GBitmap, AudioBuffer for more examples.
class ResourceInstance
{
private:
public:
   /// Pointer to the ResourceObject that stores all our book-keeping data.
   ResourceObject *mSourceResource;

   ResourceInstance() { mSourceResource = NULL; }
   virtual ~ResourceInstance() {}
};


typedef ResourceInstance* (*RESOURCE_CREATE_FN)(Stream &stream);


//------------------------------------------------------------------------------
#define InvalidCRC 0xFFFFFFFF

/// Wrapper around a ResourceInstance.
///
/// This contains all the book-keeping data used by ResDictionary and ResManager.
///
/// @see ResManager
class ResourceObject
{
   friend class ResDictionary;
   friend class ResManager;

   /// @name List Book-keeping
   /// @{

   ///
   ResourceObject *prev, *next;

   ResourceObject *nextEntry;    ///< This is used by ResDictionary for its hash table.

   ResourceObject *nextResource;
   ResourceObject *prevResource;
   /// @}

public:
   enum Flags
   {
      VolumeBlock   = BIT(0),
      File          = BIT(1),
      Added         = BIT(2),
   };
   S32 flags;  ///< Set from Flags.

   StringTableEntry path;     ///< Resource path.
   StringTableEntry name;     ///< Resource name.

   /// @name ZIP Archive
   /// If the resource is stored in a zip file, these members are populated.
   /// @{

   ///
   StringTableEntry zipPath;  ///< Path of zip file.
   StringTableEntry zipName;  ///< Name of zip file.

   S32 fileOffset;            ///< Offset of data in zip file.
   S32 fileSize;              ///< Size on disk of resource block.
   S32 compressedFileSize;    ///< Actual size of resource data.
   /// @}

   ResourceInstance *mInstance;  ///< Pointer to actual object instance. If the object is not loaded,
                                 ///  this may be NULL or garbage.
   S32 lockCount;                ///< Lock count; used to control load/unload of resource from memory.
   U32 crc;                      ///< CRC of resource.

   Zip::ZipArchive *mZipArchive; ///< The zip archive for reading from zips
   const Zip::CentralDir *mCentralDir; ///< The central directory for this file in the zip

   ResourceObject();
   ~ResourceObject() { unlink(); }

   void destruct();

   /// @name List Management
   /// @{

   ///
   ResourceObject* getNext() const { return next; }
   void unlink();
   void linkAfter(ResourceObject* res);
   /// @}

   /// Get some information about file times.
   ///
   /// @param  createTime  Pointer to a FileTime structure to fill with information about when this object was
   ///                     created.
   /// @param  modifyTime  Pointer to a FileTime structure to fill with information about when this object was
   ///                     modified.
   void getFileTimes(FileTime *createTime, FileTime *modifyTime);
};


inline void ResourceObject::unlink()
{
   if (next)
      next->prev = prev;
   if (prev)
      prev->next = next;
   next = prev = 0;
}

inline void ResourceObject::linkAfter(ResourceObject* res)
{
   unlink();
   prev = res;
   if ((next = res->next) != 0)
      next->prev = this;
   res->next = this;
}


//------------------------------------------------------------------------------
/// Wrapper class around a ResourceInstance subclass.
///
/// @code
///    // Loading a resource...
///    Resource<TerrainFile> terrRes;
///
///    terrRes = ResourceManager->load(fileName);
///    if(!bool(terrRes))
///       Con::errorf(ConsoleLogEntry::General, "Terraformer::terrainFile - invalid terrain file '%s'.", fileName);
/// @endcode
///
/// When the Resource<> is destroyed, it frees the lock on the resource.
///
/// @see ResManager
template <class T> class Resource
{
private:
   ResourceObject *obj;  ///< Actual resource object

   // ***WARNING***
   // Using a faster lock that bypasses the resource manger.
   // void _lock() { if (obj) obj->rm->lockResource( obj ); }
   void _lock();   ///< Increments the lock count on this object
   void _unlock(); ///< Decrements the lock count on this object

public:
   /// If assigned a ResourceObject, it's assumed to already have
   /// been locked, lock count is incremented only for copies or
   /// assignment from another Resource.
   Resource() : obj(NULL) { ; }
   Resource(ResourceObject *p) : obj(p) { ; }
   Resource(const Resource &res) : obj(res.obj) { _lock(); }
   ~Resource() { unlock(); }  ///< Decrements the lock count on this object, and if the lock count is 0 afterwards,
                              ///< adds the object to the timeoutList for deletion on execution of purge().

   const char *getFilePath() const { return (obj ? obj->path : NULL); } ///< Returns the path of the file (without the actual name)
   const char *getFileName() const { return (obj ? obj->name : NULL); } ///< Returns the actual file name (without the path)

   Resource& operator= (ResourceObject *p) { _unlock(); obj = p; return *this; }
   Resource& operator= (const Resource &r) { _unlock(); obj = r.obj; _lock(); return *this; }

   U32 getCRC() { return (obj ? obj->crc : 0); }
   bool isNull()   const { return ((obj == NULL) || (obj->mInstance == NULL)); }
   operator bool() const { return ((obj != NULL) && (obj->mInstance != NULL)); }
   T* operator->()   { return (T*)obj->mInstance; }
   T& operator*()    { return *((T*)obj->mInstance); }
   operator T*() const    { return (obj) ? (T*)obj->mInstance : (T*)NULL; }
   const T* operator->() const  { return (const T*)obj->mInstance; }
   const T& operator*() const   { return *((const T*)obj->mInstance); }
   operator const T*() const    { return (obj) ? (const T*)obj->mInstance :  (const T*)NULL; }
   void unlock();
   void purge();
};

#define INVALID_ID ((U32)(~0))

//----------------------------------------------------------------------------
/// Resource Dictionary.
///
/// Maps of names and object IDs to objects.
///
/// Provides fast lookup for name->object, id->object and
/// for fast removal of an object given a pointer to it.
///
/// @see ResManager
class ResDictionary
{
   /// @name Hash Table
   /// @{

   enum { DefaultTableSize = 1029 };

   ResourceObject **hashTable;
   S32 entryCount;
   S32 hashTableSize;
   DataChunker memPool;
   S32 hash(StringTableEntry path, StringTableEntry name);
   S32 hash(ResourceObject *obj) { return hash(obj->path, obj->name); }
   /// @}

public:
   ResDictionary();
   ~ResDictionary();

   /// Add a ResourceObject to the dictionary.
   void insert(ResourceObject *obj, StringTableEntry path, StringTableEntry file);

   /// @name Find
   /// These functions search the hash table for an individual resource.  If the resource has
   /// already been loaded, it will find the resource and return its object.  If not,
   /// it will return NULL.
   /// @{

   ResourceObject* find(StringTableEntry path, StringTableEntry file);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, StringTableEntry filePath, StringTableEntry fileName);
   ResourceObject* find(StringTableEntry path, StringTableEntry file, U32 flags);
   /// @}

   /// Move a previously added resource object to be in the list after everything that matches the specified mask.
   void pushBehind(ResourceObject *obj, S32 mask);

   /// Remove a resource object from the dictionary.
   void remove(ResourceObject *obj);
};


//------------------------------------------------------------------------------
/// A virtual file system for the storage and retrieval of ResourceObjects.
///
/// Basic resource manager behavior:
///  - Set the mod path.
///      - ResManager scans directory tree under listed base directories
///      - Any volume (.zip) file in the root directory of a mod is scanned
///        for resources.
///      - Any files currently in the resource manager become memory resources.
///      - They can be "reattached" to the first file that matches the file name.
///
/// All classes which wish to be handled by the resource manager need:
///    -# To be derived from ResourceInstance.
///    -# To register a creation function and file extension with the manager.
///
/// @nosubgrouping
class ResManager
{
private:
   /// Path to which we will write data.
   ///
   /// This is used when, for instance, we download files from a server.
   char writeablePath[1024];

   /// Primary path from which we load data.
   char primaryPath[1024];

   /// List of secondary paths to search.
   char* pathList;

   ResourceObject timeoutList;
   ResourceObject resourceList;

   ResDictionary dictionary;

   bool echoFileNames;

   bool usingVFS;

   bool isIgnoredSubdirectoryName(const char *name) const;

   /// Scan a zip file for resources.
   bool scanZip(ResourceObject *zipObject);

   /// Create a ResourceObject from the given file.
   ResourceObject* createResource(StringTableEntry path, StringTableEntry file);

   /// Create a ResourceObject from the given file in a zip file.
   ResourceObject* createZipResource(StringTableEntry path, StringTableEntry file, StringTableEntry zipPath, StringTableEntry zipFle);

   void searchPath(const char *pathStart, bool noDups = false, bool ignoreZips = false);
   bool setModZip(const char* path);

   struct RegisteredExtension
   {
      StringTableEntry     mExtension;
      RESOURCE_CREATE_FN   mCreateFn;
      RegisteredExtension  *next;
   };

   Vector<char *> mMissingFileList;                ///< List of missing files.
   bool mLoggingMissingFiles;                      ///< Are there any missing files?
   void fileIsMissing(const char *fileName);       ///< Called when a file is missing.

   RegisteredExtension *registeredList;

   static const char *smExcludedDirectories;
   ResManager();
public:
   RESOURCE_CREATE_FN getCreateFunction( const char *name );

   ~ResManager();
   /// @name Global Control
   /// These are called to initialize/destroy the resource manager at runtime.
   /// @{

   static void create();
   static void destroy();

   /// Load the excluded directories from the resource manager pref and
   /// stuff it into the platform layer.
   static void initExcludedDirectories();

   ///@}

   void setFileNameEcho(bool on);                     ///< Sets whether or not to echo filenames that have been accessed by means of openStream
   void setModPaths(U32 numPaths, const char **dirs); ///< Sets the path for the current game mod
   const char* getModPaths();                         ///< Gets the path for the current game mod

   void addPath(const char *path, bool ignoreZips=false);///< Add a path
   void removePath(const char *path);                 ///< Remove a path. Only removes resources that are not loaded.

   /// Forget a file that is no longer on disk. Returns false only if the file
   /// was still loaded and so had to be kept.
   bool removeFile(const char *fileName);

   /// Forget every file beneath a directory that is no longer on disk. Returns
   /// false if any of them was still loaded and so had to be kept.
   bool removeDirectory(const char *path);

   void setMissingFileLogging(bool log);              ///< Should we log missing files?
   bool getMissingFileList(Vector<char *> &list);     ///< Gets which files are missing
   void clearMissingFileList();                       ///< Clears the missing file list

   /// Tells the resource manager what to do with a resource that it loads
   void registerExtension(const char *extension, RESOURCE_CREATE_FN create_fn);

   S32 getSize(const char* filename);                 ///< Gets the size of the file
   const char* getFullPath(const char * filename, char * path, U32 pathLen);  ///< Gets the full path of the file
   const char* getModPathOf(const char* fileName);    ///< Gets the path of the file local to the mod
   const char* getPathOf(const char * filename);      ///< Gets the path of the file from the base directory
   const char* getBasePath();                         ///< Gets the base path

   ResourceObject* load(const char * fileName, bool computeCRC = false);   ///< loads an instance of an object
   Stream*  openStream(const char * fileName);        ///< Opens a stream for an object
   Stream*  openStream(ResourceObject *object);       ///< Opens a stream for an object
   void     closeStream(Stream *stream);              ///< Closes the stream

   /// Decrements the lock count of an object.  If the lock count is zero post-decrement,
   /// the object is added to the timeoutList for deletion upon call of flush.
   void unlock( ResourceObject* );

   /// Add a new resource instance
   bool add(const char* name, ResourceInstance *addInstance, bool extraLock = false);

   /// Searches the hash list for the filename and returns it's object if found, otherwise NULL
   ResourceObject* find(const char * fileName);
   /// Loads a new instance of an object by means of a filename
   ResourceInstance* loadInstance(const char *fileName, bool computeCRC = false);
   /// Loads a new instance of an object by means of a resource object
   ResourceInstance* loadInstance(ResourceObject *object, bool computeCRC = false);

   /// Searches the hash list for the filename and returns it's object if found, otherwise NULL
   ResourceObject* find(const char * fileName, U32 flags);

   /// Finds a resource object with given expression
   ResourceObject* findMatch(const char *expression, const char **fn, ResourceObject *start = NULL);

   /// Finds a resource object with given expressions, seperated by " "
   ResourceObject* findMatchMultiExprs(const char *multiExpression, const char **fn, ResourceObject *start = NULL);

   void purge();                                      ///< Goes through the timeoutList and deletes it all.  BURN!!!
   void purge( ResourceObject *obj );                 ///< Deletes one resource object.
   void freeResource(ResourceObject *resObject);      ///< Frees a resource!
   void serialize(VectorPtr<const char *> &filenames);///< Sorts the resource objects

   S32  findMatches( FindMatch *pFM );                ///< Finds multiple matches to an expression.
   bool findFile( const char *name );                 ///< Checks to see if a file exists.

   bool addVFSRoot(Zip::ZipArchive *vfs);
   bool isUsingVFS() { return usingVFS; }

   /// Computes the CRC of a file.
   ///
   /// By passing a different crcInitialVal, you can take the CRC of multiple files.
   bool getCrc(const char * fileName, U32 & crcVal, const U32 crcInitialVal = INITIAL_CRC_VALUE );

   void setWriteablePath(const char *path);           ///< Sets the writable path for a file to the one given.
   bool isValidWriteFileName(const char *fn);         ///< Checks to see if the given path is valid for writing.

   /// Opens a file for writing!
   bool openFileForWrite(FileStream &fs, const char *fileName, U32 accessMode = File::Write);

#ifdef TORQUE_DEBUG
   void dumpResources(const bool onlyLoaded = true);                        ///< Dumps all loaded resources to the console.
#endif
};

template<class T> inline void Resource<T>::unlock()
{
   if (obj) {
      ResourceManager->unlock( obj );
      obj=NULL;
   }
}

template<class T> inline void Resource<T>::purge()
{
   if (obj) {
      ResourceManager->unlock( obj );
      if (obj->lockCount == 0)
         ResourceManager->purge(obj);
      obj = NULL;
   }
}
template <class T> inline void Resource<T>::_lock()
{
   if (obj)
      obj->lockCount++;
}

template <class T> inline void Resource<T>::_unlock()
{
   if (obj)
      ResourceManager->unlock( obj );
}

#endif //_RESMANAGER_H_
