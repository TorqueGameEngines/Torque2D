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

#ifndef _PLATFORM_FILEIO_H_
#include "platform/platformFileIO.h"
#endif

//-----------------------------------------------------------------------------

#define PLATFORM_UNITTEST_FILEIO_FILE           "_unitTestFile_RemoveMe.txt"
#define PLATFORM_UNITTEST_FILEIO_FILEMESSAGE    "Write a line of text."

//-----------------------------------------------------------------------------
TEST( PlatformFileIOTests, FileWriteRead )
{
    File testWriteFile;
    File::Status fileStatus;

    // Open the file for writing.
    fileStatus = testWriteFile.open( PLATFORM_UNITTEST_FILEIO_FILE, File::Write );

    // Check the file was opened.
    ASSERT_EQ( fileStatus, File::Ok ) << "Failed to open file for (over)write.";

    // Check we can write to the file.
    ASSERT_TRUE( testWriteFile.hasCapability( File::FileWrite ) ) << "File cannot be written to.";

    // Check test message length (we may not have executed any string tests yet).
    const S32 fileMessageLength = (S32)dStrlen(PLATFORM_UNITTEST_FILEIO_FILEMESSAGE);
    ASSERT_GT( fileMessageLength, 0 ) << "Test message must be a non-zero length.";

    // Write to the file.
    U32 bytesWritten;
    fileStatus = testWriteFile.write( fileMessageLength, PLATFORM_UNITTEST_FILEIO_FILEMESSAGE, &bytesWritten );

    // Check write operation.
    ASSERT_EQ( fileStatus, File::Ok ) << "Test message write operation failed.";
    ASSERT_EQ( bytesWritten, fileMessageLength ) << "Test message was not written fully.";

    // Check position and size.
    ASSERT_EQ( testWriteFile.getPosition(), bytesWritten ) << "File position is invalid.";
    ASSERT_EQ( testWriteFile.getSize(), bytesWritten ) << "File size is incorrect.";    

    // Close the file.
    fileStatus = testWriteFile.close();

    // Check status.
    ASSERT_EQ( fileStatus, File::Closed ) << "Write file was not closed.";

    /// Test file reading...
    File testReadFile;

    // Open the file for reading.
    fileStatus = testReadFile.open( PLATFORM_UNITTEST_FILEIO_FILE, File::Read );

    // Check the file was opened.
    ASSERT_EQ( fileStatus, File::Ok ) << "Failed to open file for read.";

    // Check we can read from the file.
    ASSERT_TRUE( testReadFile.hasCapability( File::FileRead ) ) << "File cannot be read from.";

    // Create a read buffer.
    char* pReadBuffer = new char[fileMessageLength+1];
    pReadBuffer[fileMessageLength] = 0;
    U32 bytesRead;

    // Read from the file.
    fileStatus = testReadFile.read( fileMessageLength, pReadBuffer, &bytesRead );

    // Check read operation.
    ASSERT_EQ( fileStatus, File::Ok ) << "Test message read operation failed.";
    ASSERT_EQ( bytesRead, fileMessageLength ) << "Test message was not read fully.";

    // Check position and size.
    ASSERT_EQ( testReadFile.getPosition(), bytesRead ) << "File position is invalid.";
    ASSERT_EQ( testReadFile.getSize(), bytesRead ) << "File size is incorrect."; 

    // Check contents.
    ASSERT_STREQ( PLATFORM_UNITTEST_FILEIO_FILEMESSAGE, pReadBuffer ) << "Test message read incorrectly.";

    // Destroy the read buffer.
    delete [] pReadBuffer;

    // Close the file.
    fileStatus = testReadFile.close();

    // Check status.
    ASSERT_EQ( fileStatus, File::Closed ) << "Read file was not closed.";

    // Check the file has been deleted.
    ASSERT_TRUE( Platform::fileDelete( PLATFORM_UNITTEST_FILEIO_FILE ) );

	SUCCEED();
}

//-----------------------------------------------------------------------------
// Copying and renaming. Both were unimplemented stubs returning false on the
// Unix back-end, which is not something a caller can tell from a copy that
// simply did not happen: the editors stamp a new project out of a template and
// give a theme its own cursor art this way, and both silently produced nothing.
//
// Everything here is done against absolute paths built from the working
// directory, because a relative path handed to Platform::createPath is routed
// into the pref directory while the copy itself is written where it was asked
// for -- so a relative test would be checking two different places.
//-----------------------------------------------------------------------------

#define PLATFORM_UNITTEST_COPY_ROOT "_unitTestCopy_RemoveMe"

static void unitTestCopyPath( char* buffer, U32 bufferSize, const char* relative )
{
    dSprintf( buffer, bufferSize, "%s/%s/%s",
        Platform::getCurrentDirectory(), PLATFORM_UNITTEST_COPY_ROOT, relative );
}

static bool unitTestWriteFile( const char* path, const char* contents )
{
    // Every one of these writes into a folder that does not exist yet -- the
    // scratch root is deleted at the top of the test, and pathCopy making its
    // destination is part of what is being tested. File::open(Write) does not
    // make one, it just fails, so the very first write failed and took the whole
    // case with it.
    Platform::createPath( path );

    File file;
    if ( file.open( path, File::Write ) != File::Ok )
        return false;

    U32 written = 0;
    const bool ok = file.write( (U32)dStrlen( contents ), contents, &written ) == File::Ok;
    file.close();
    return ok;
}

TEST( PlatformFileIOTests, PathCopyAndRename )
{
    char root[1024];
    dSprintf( root, sizeof( root ), "%s/%s",
        Platform::getCurrentDirectory(), PLATFORM_UNITTEST_COPY_ROOT );

    // Anything left by a previous run would make the overwrite checks lie.
    // Asked only when there is something there, because deleteDirectory
    // complains about a directory it cannot read.
    if ( Platform::isDirectory( root ) )
        Platform::deleteDirectory( root );

    char source[1024], target[1024], nested[1024];

    // --- A single file, into a folder that does not exist yet. ---
    unitTestCopyPath( source, sizeof( source ), "src/one.txt" );
    unitTestCopyPath( target, sizeof( target ), "dst/one.txt" );

    ASSERT_TRUE( unitTestWriteFile( source, "hello" ) ) << "Could not write the source file.";
    ASSERT_TRUE( Platform::isFile( source ) ) << "The source file was not created.";

    ASSERT_TRUE( Platform::pathCopy( source, target, true ) ) << "Copying a file failed.";
    ASSERT_TRUE( Platform::isFile( target ) ) << "The copy is not there.";
    ASSERT_EQ( Platform::getFileSize( source ), Platform::getFileSize( target ) ) << "The copy is a different size.";

    // --- nooverwrite is honoured in both directions. ---
    ASSERT_FALSE( Platform::pathCopy( source, target, true ) ) << "Copying over an existing file should be refused.";
    ASSERT_TRUE( Platform::pathCopy( source, target, false ) ) << "Copying over an existing file should be allowed when asked.";

    // --- A missing source fails, and leaves nothing behind. ---
    unitTestCopyPath( source, sizeof( source ), "src/nosuch.txt" );
    unitTestCopyPath( target, sizeof( target ), "dst/nosuch.txt" );
    ASSERT_FALSE( Platform::pathCopy( source, target, true ) ) << "Copying a file that does not exist should fail.";
    ASSERT_FALSE( Platform::isFile( target ) ) << "A failed copy left a file behind.";

    // --- A whole tree, which is how a project is stamped from a template. ---
    unitTestCopyPath( source, sizeof( source ), "tree/top.txt" );
    ASSERT_TRUE( unitTestWriteFile( source, "top" ) ) << "Could not write the tree's top file.";
    unitTestCopyPath( nested, sizeof( nested ), "tree/deep/deeper/bottom.txt" );
    ASSERT_TRUE( unitTestWriteFile( nested, "bottom" ) ) << "Could not write the tree's nested file.";

    unitTestCopyPath( source, sizeof( source ), "tree" );
    unitTestCopyPath( target, sizeof( target ), "treeCopy" );
    ASSERT_TRUE( Platform::pathCopy( source, target, true ) ) << "Copying a directory failed.";

    unitTestCopyPath( target, sizeof( target ), "treeCopy/top.txt" );
    ASSERT_TRUE( Platform::isFile( target ) ) << "The tree's top file did not come across.";
    unitTestCopyPath( target, sizeof( target ), "treeCopy/deep/deeper/bottom.txt" );
    ASSERT_TRUE( Platform::isFile( target ) ) << "The tree's nested file did not come across.";
    unitTestCopyPath( target, sizeof( target ), "treeCopy/deep/deeper" );
    ASSERT_TRUE( Platform::isDirectory( target ) ) << "The tree's nested folder did not come across.";

    // --- A tree copied into itself would recurse until the path ran out. ---
    unitTestCopyPath( source, sizeof( source ), "tree" );
    unitTestCopyPath( target, sizeof( target ), "tree/inner" );
    ASSERT_FALSE( Platform::pathCopy( source, target, true ) ) << "A directory should not be copied into itself.";

    // --- Renaming, which has no script binding and so is only reachable here. ---
    unitTestCopyPath( source, sizeof( source ), "dst/one.txt" );
    unitTestCopyPath( target, sizeof( target ), "dst/two.txt" );
    ASSERT_TRUE( Platform::fileRename( source, target ) ) << "Renaming a file failed.";
    ASSERT_TRUE( Platform::isFile( target ) ) << "The renamed file is not at its new name.";
    ASSERT_FALSE( Platform::isFile( source ) ) << "The renamed file is still at its old name.";

    unitTestCopyPath( source, sizeof( source ), "dst/nosuch.txt" );
    unitTestCopyPath( target, sizeof( target ), "dst/three.txt" );
    ASSERT_FALSE( Platform::fileRename( source, target ) ) << "Renaming a file that does not exist should fail.";

    // Tidy up, and check the cleanup itself worked so the next run starts clean.
    ASSERT_TRUE( Platform::deleteDirectory( root ) ) << "Could not remove the test directory.";
    ASSERT_FALSE( Platform::isDirectory( root ) ) << "The test directory is still there.";

    SUCCEED();
}
//-----------------------------------------------------------------------------
// The per-user directories.
//
// On the Unix back-end all three of these answered the literal string "~/",
// which is a home directory only to a shell. Nothing in the engine expands one,
// so the answer was taken for a relative path and resolved against the working
// directory -- the folder the executable was launched from -- and the editor's
// preferences were written into a directory named "~" inside the repository.
//
// These assertions are about shape rather than about any particular folder,
// because the right answer differs per platform: %APPDATA% on Windows,
// ~/Library/Application Support on macOS, $XDG_DATA_HOME on Linux. What all
// three have to be is somewhere real, named absolutely, and reached without a
// shell.
//-----------------------------------------------------------------------------

static void unitTestUserDirectory( const char* what, const char* path )
{
    ASSERT_TRUE( path != NULL && *path != 0 ) << what << " is empty.";
    ASSERT_TRUE( Platform::isFullPath( path ) ) << what << " is not an absolute path: " << path;
    ASSERT_TRUE( dStrchr( path, '~' ) == NULL ) << what << " holds an unexpanded tilde: " << path;
    ASSERT_TRUE( Platform::isDirectory( path ) ) << what << " does not exist: " << path;
}

TEST( PlatformFileIOTests, UserDirectories )
{
    unitTestUserDirectory( "The user data directory", Platform::getUserDataDirectory() );
    unitTestUserDirectory( "The user home directory", Platform::getUserHomeDirectory() );

    // The unwrapped one: Platform::getTemporaryDirectory falls back to the
    // working directory when this does not name a real folder, which is exactly
    // how a bad answer here stayed invisible.
    unitTestUserDirectory( "The temporary directory", Platform::osGetTemporaryDirectory() );

    // getPrefsPath is what actually names a file, and it is where the tilde did
    // its damage: it hangs the company and product off the user data directory,
    // so a relative answer above put every project's preferences under whatever
    // folder the game happened to be launched from.
    const char* prefsPath = Platform::getPrefsPath( "unitTestPreferences.taml" );
    ASSERT_TRUE( prefsPath != NULL && *prefsPath != 0 ) << "There is no preferences path.";
    ASSERT_TRUE( Platform::isFullPath( prefsPath ) ) << "The preferences path is not absolute: " << prefsPath;
    ASSERT_TRUE( dStrchr( prefsPath, '~' ) == NULL ) << "The preferences path holds an unexpanded tilde: " << prefsPath;

    // Nothing is created here. getPrefsPath names a folder that need not exist
    // yet, and the point of the check is that it is named somewhere sane.

    SUCCEED();
}
//-----------------------------------------------------------------------------

#endif // TORQUE_SHIPPING
