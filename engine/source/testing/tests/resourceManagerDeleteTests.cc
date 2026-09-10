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

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif

#ifndef _FILESTREAM_H_
#include "io/fileStream.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

//-----------------------------------------------------------------------------
// isFile() and getFileCRC() answer out of the resource manager's dictionary
// rather than off the filesystem -- deliberately, because a file inside a
// mounted zip has no standalone path to stat. The consequence nobody maintained
// is that a delete has to tell the manager, and neither fileDelete nor
// directoryDelete did: the entry outlived the file, isFile went on reporting a
// file that was gone for the rest of the session, and getCrc -- which opens the
// entry it is handed -- opened nothing and read through the null.
//-----------------------------------------------------------------------------

#define RESMGR_TEST_DIR     "_unitTestResMgr_RemoveMe"
#define RESMGR_TEST_FILE    RESMGR_TEST_DIR "/file.txt"
#define RESMGR_TEST_KEEP    RESMGR_TEST_DIR "/keep.txt"

static bool resMgrWriteFile( const char* pPath )
{
    FileStream stream;
    if ( !ResourceManager->openFileForWrite( stream, pPath ) )
        return false;

    stream.write( 5, "hello" );
    stream.close();
    return true;
}

static bool resMgrScriptIsFile( const char* pPath )
{
    return dAtoi( Con::evaluatef( "return isFile(\"%s\");", pPath ) ) != 0;
}

static void resMgrCleanup( void )
{
    Con::evaluatef( "directoryDelete(\"%s\");", RESMGR_TEST_DIR );
}

//-----------------------------------------------------------------------------

TEST( ResourceManagerDeleteTests, FileDeleteIsSeenByIsFile )
{
    ASSERT_TRUE( resMgrWriteFile( RESMGR_TEST_FILE ) ) << "Could not create the test file.";
    ASSERT_TRUE( resMgrScriptIsFile( RESMGR_TEST_FILE ) ) << "isFile did not see the file just written.";

    ASSERT_EQ( dAtoi( Con::evaluatef( "return fileDelete(\"%s\");", RESMGR_TEST_FILE ) ), 1 )
        << "fileDelete did not report success.";

    // The file really is gone: fileSize goes to the filesystem, isFile does not.
    EXPECT_EQ( Platform::getFileSize( RESMGR_TEST_FILE ), -1 ) << "The file is still on disk.";
    EXPECT_FALSE( resMgrScriptIsFile( RESMGR_TEST_FILE ) ) << "isFile still reports a deleted file.";

    resMgrCleanup();
}

//-----------------------------------------------------------------------------

TEST( ResourceManagerDeleteTests, DirectoryDeleteIsSeenByIsFile )
{
    ASSERT_TRUE( resMgrWriteFile( RESMGR_TEST_FILE ) ) << "Could not create the test file.";
    ASSERT_TRUE( resMgrScriptIsFile( RESMGR_TEST_FILE ) ) << "isFile did not see the file just written.";

    ASSERT_EQ( dAtoi( Con::evaluatef( "return directoryDelete(\"%s\");", RESMGR_TEST_DIR ) ), 1 )
        << "directoryDelete did not report success.";

    EXPECT_FALSE( resMgrScriptIsFile( RESMGR_TEST_FILE ) )
        << "isFile still reports a file whose directory was deleted.";
}

//-----------------------------------------------------------------------------

TEST( ResourceManagerDeleteTests, DeletingOneFileKeepsItsNeighbour )
{
    ASSERT_TRUE( resMgrWriteFile( RESMGR_TEST_FILE ) );
    ASSERT_TRUE( resMgrWriteFile( RESMGR_TEST_KEEP ) );

    Con::evaluatef( "fileDelete(\"%s\");", RESMGR_TEST_FILE );

    EXPECT_FALSE( resMgrScriptIsFile( RESMGR_TEST_FILE ) ) << "The deleted file is still reported.";
    EXPECT_TRUE( resMgrScriptIsFile( RESMGR_TEST_KEEP ) ) << "Deleting one file forgot another.";

    U32 crcVal = 0;
    EXPECT_TRUE( ResourceManager->getCrc( RESMGR_TEST_KEEP, crcVal ) )
        << "The neighbouring file is no longer readable.";

    resMgrCleanup();
}

//-----------------------------------------------------------------------------
// The manager can always be handed an entry whose file has gone behind its back
// -- another process, a removable volume, a user with a shell -- so this has to
// fail rather than crash whether or not the delete came through fileDelete.
//-----------------------------------------------------------------------------

TEST( ResourceManagerDeleteTests, GetCrcFailsRatherThanCrashingOnAMissingFile )
{
    ASSERT_TRUE( resMgrWriteFile( RESMGR_TEST_FILE ) );

    U32 crcVal = 0;
    ASSERT_TRUE( ResourceManager->getCrc( RESMGR_TEST_FILE, crcVal ) )
        << "The file should CRC while it is still there.";

    // Deleted WITHOUT telling the resource manager, so the entry stays and
    // openStream will have nothing to open.
    ASSERT_TRUE( Platform::fileDelete( RESMGR_TEST_FILE ) ) << "Could not remove the test file.";

    crcVal = 0;
    EXPECT_FALSE( ResourceManager->getCrc( RESMGR_TEST_FILE, crcVal ) )
        << "getCrc should fail when its entry outlives the file.";

    resMgrCleanup();
}

//-----------------------------------------------------------------------------

TEST( ResourceManagerDeleteTests, ForgettingAnUnknownFileIsNotAnError )
{
    EXPECT_TRUE( ResourceManager->removeFile( RESMGR_TEST_DIR "/never-existed.txt" ) )
        << "Forgetting a file the manager never knew about should succeed quietly.";

    EXPECT_FALSE( ResourceManager->removeFile( NULL ) ) << "A null name should be refused.";
    EXPECT_FALSE( ResourceManager->removeDirectory( NULL ) ) << "A null path should be refused.";
}

#endif // TORQUE_SHIPPING
