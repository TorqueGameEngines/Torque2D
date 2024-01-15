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

#import <pthread.h>
#import <stdlib.h>
#import <errno.h>
#import "memory/safeDelete.h"
#import "platform/threads/thread.h"
#import "platform/platformSemaphore.h"
#import "platform/threads/mutex.h"
#import "console/console.h"
#import "platformOSX/platformOSX.h"

//-----------------------------------------------------------------------------

#pragma mark ---- Thread Class Methods ----

struct PlatformThreadData
{
    ThreadRunFunction       mRunFunc;
    void*                   mRunArg;
    Thread*                 mThread;
    Semaphore               mGateway; // default count is 1
    ThreadIdent             mThreadID;
};

//-----------------------------------------------------------------------------

static void *ThreadRunHandler( void* arg )
{
    // Fetch the platform thread data.
    PlatformThreadData* mData = reinterpret_cast<PlatformThreadData*>(arg);
    
    // Fetch the thread.
    Thread* thread = mData->mThread;
    mData->mThreadID = ThreadManager::getCurrentThreadId();
    
    // Add the thread.
    ThreadManager::addThread(thread);
    
    // Run the thread against a pool.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    thread->run(mData->mRunArg);
    [pool drain];
    
    // Release the thread.
    mData->mGateway.release();
    
    // Does the thread want to be auto-deleted?
    if(thread->autoDelete)
    {
        // Yes, so remove the thread.
        ThreadManager::removeThread(thread);
        
        // Delete the thread.
        delete thread;
    }
    
    // This is for pthread.
    return NULL;
}

//-----------------------------------------------------------------------------

Thread::Thread(ThreadRunFunction func, void* arg, bool start_thread, bool autodelete)
{
    mData = new PlatformThreadData;
    mData->mRunFunc = func;
    mData->mRunArg = arg;
    mData->mThread = this;
    mData->mThreadID = 0;
    autoDelete = autodelete;
    
    if(start_thread)
        start();
}

//-----------------------------------------------------------------------------

Thread::~Thread()
{
    stop();
    join();
    
    SAFE_DELETE(mData);
}

//-----------------------------------------------------------------------------

void Thread::start()
{
    if(isAlive())
        return;
    
    // cause start to block out other pthreads from using this Thread,
    // at least until ThreadRunHandler exits.
    mData->mGateway.acquire();
    
    // reset the shouldStop flag, so we'll know when someone asks us to stop.
    shouldStop = false;
    
    pthread_create((pthread_t*)(&mData->mThreadID), NULL, ThreadRunHandler, mData);
}

//-----------------------------------------------------------------------------

bool Thread::join()
{
    if(!isAlive())
        return true;
    
    // not using pthread_join here because pthread_join cannot deal
    // with multiple simultaneous calls.
    mData->mGateway.acquire();
    mData->mGateway.release();
    return true;
}

//-----------------------------------------------------------------------------

void Thread::run(void* arg)
{
    if(mData->mRunFunc)
        mData->mRunFunc(arg);
}

//-----------------------------------------------------------------------------

bool Thread::isAlive()
{
    if(mData->mThreadID == 0)
        return false;
    
    if( mData->mGateway.acquire(false) )
    {
        mData->mGateway.release();
        return false; // we got the lock, it aint alive.
    }
    else
        return true; // we could not get the lock, it must be alive.
}

//-----------------------------------------------------------------------------

ThreadIdent Thread::getId()
{
   return mData->mThreadID;
}

#pragma mark ---- ThreadManager Class Methods ----

//-----------------------------------------------------------------------------

ThreadIdent ThreadManager::getCurrentThreadId()
{
   return (ThreadIdent)pthread_self();
}

//-----------------------------------------------------------------------------

bool ThreadManager::compare( ThreadIdent threadId_1, ThreadIdent threadId_2 )
{
   return (bool)pthread_equal((pthread_t)threadId_1, (pthread_t)threadId_2);
}

class ExecuteThread : public Thread
{
    const char* zargs;
    const char* directory;
    const char* executable;
public:
    ExecuteThread(const char *_executable, const char *_args /* = NULL */, const char *_directory /* = NULL */) : Thread(0, NULL, false, true)
    {
        zargs = dStrdup(_args);
        directory = dStrdup(_directory);
        executable = dStrdup(_executable);
        start();
    }
    static U32 runNoThread(const char *_executable, const char *_args /* = NULL */, const char *_directory /* = NULL */);
    virtual void run(void* arg);
};

static char* _unDoubleQuote(char* arg)
{
    U32 len = dStrlen(arg);
    if(!len)
        return arg;
    
    if(arg[0] == '"' && arg[len-1] == '"')
    {
        arg[len - 1] = '\0';
        return arg + 1;
    }
    return arg;
}

// this is an externably callable blocking shellExecute!!!
U32 ExecuteThread::runNoThread( const char* executable, const char* zargs, const char* directory  )
{
    printf("creating nstask\n");
    NSTask *aTask = [[NSTask alloc] init];
    NSMutableArray *array = [NSMutableArray array];
    
    // scan the args list, breaking it up, space delimited, backslash escaped.
    U32 len = dStrlen(zargs);
    char args[len+1];
    dStrncpy(args, zargs, len+1);
    char *lastarg = args;
    bool escaping = false;
    for(int i = 0; i< len; i++)
    {
        char c = args[i];
        // a backslash escapes the next character
        if(escaping)
            continue;
        if(c == '\\')
            escaping = true;
        
        if(c == ' ')
        {
            args[i] = '\0';
            if(*lastarg)
                [array addObject:[NSString stringWithUTF8String: _unDoubleQuote(lastarg)]];
            lastarg = args + i + 1;
        }
    }
    if(*lastarg)
        [array addObject:[NSString stringWithUTF8String: _unDoubleQuote(lastarg)]];
    
    [aTask setArguments: array];
    
    [aTask setCurrentDirectoryPath:[NSString stringWithUTF8String: directory]];
    [aTask setLaunchPath:[NSString stringWithUTF8String:executable]];
    [aTask launch];
    [aTask waitUntilExit];
    U32 ret = [aTask terminationStatus];
    return ret;
}

void ExecuteThread::run(void* arg)
{
    // call the common run, but since we're in a thread, this won't block other processes
    U32 ret = runNoThread( this->executable, this->zargs, this->directory );
    Con::executef(2, "onExecuteDone", Con::getIntArg(ret));
    printf("done nstask\n");
}

ConsoleFunction(shellExecute, bool, 2, 4, "(executable, [args], [directory])")
{
    ExecuteThread *et;
    et = new ExecuteThread(argv[1], argc > 2 ? argv[2] : NULL, argc > 3 ? argv[3] : NULL);
    return true; // Bug: BPNC error: need feedback on whether the command was sucessful
}

ConsoleFunction(shellExecuteBlocking, int, 2, 4, "(executable, [args], [directory])")
{
    return (int)ExecuteThread::runNoThread( argv[1], argc > 2 ? argv[2] : NULL, argc > 3 ? argv[3] : NULL );
}