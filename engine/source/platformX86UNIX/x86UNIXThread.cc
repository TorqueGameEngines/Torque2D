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

#include <pthread.h>
#include "platform/threads/thread.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platformSemaphore.h"

//--------------------------------------------------------------------------
struct PlatformThreadData
{
  ThreadRunFunction       mRunFunc;
  void*                   mRunArg;
  Thread*                 mThread;
  Semaphore               mGateway;
  pthread_t               mThreadID;
  bool                    mDead;
};

//--------------------------------------------------------------------------
Thread::Thread(ThreadRunFunction func, void* arg, bool start_thread, bool autodelete)
{
  mData = new PlatformThreadData;
  mData->mRunFunc   = func;
  mData->mRunArg    = arg;
  mData->mThread    = this;
  mData->mThreadID = 0;
  mData->mDead = false;
  autoDelete = autodelete;

  if (start_thread)
    start();
}

Thread::~Thread()
{
  stop();
  if (isAlive())
      join();

  delete mData;
}

//-----------------------------------------------------------------------------
// Function:    ThreadRunHandler
// Summary:     Calls Thread::run() with the thread's specified run argument.
//               Neccesary because Thread::run() is provided as a non-threaded
//               way to execute the thread's run function. So we have to keep
//               track of the thread's lock here.
static void *ThreadRunHandler(void * arg)
{
   PlatformThreadData *mData = reinterpret_cast<PlatformThreadData*>(arg);
   Thread *thread = mData->mThread;

   // mThreadID is filled in twice, once here and once in pthread_create().
   // We fill in mThreadID here as well as in pthread_create() because addThread()
   // can execute before pthread_create() returns and sets mThreadID.
   // The value from pthread_create() and pthread_self() are guaranteed to be equivalent (but not identical)
   mData->mThreadID = pthread_self();
   
   ThreadManager::addThread(thread);
   thread->run(mData->mRunArg);
   ThreadManager::removeThread(thread);

   bool autoDelete = thread->autoDelete;
   
   mData->mThreadID = 0;
   mData->mDead = true;
   mData->mGateway.release();
   
   if( autoDelete )
      delete thread;
      
   // return value for pthread lib's benefit
   return NULL;
   // the end of this function is where the created pthread will die.
}

void Thread::start()
{
   mData->mGateway.acquire();

   shouldStop = false;

   mData->mDead = false;

   pthread_create(&mData->mThreadID, NULL, ThreadRunHandler, mData);
}

bool Thread::join()
{ 
   // not using pthread_join here because pthread_join cannot deal
   // with multiple simultaneous calls.
   
   mData->mGateway.acquire();
   AssertFatal( !isAlive(), "Thread::join() - thread not dead after join()" );
   mData->mGateway.release();
   
   return true;
}

void Thread::run(void* arg)
{
  if(mData->mRunFunc)
    mData->mRunFunc(arg);
}

bool Thread::isAlive()
{
  return ( !mData->mDead );
}

ThreadIdent Thread::getId()
{
   return (ThreadIdent)mData->mThreadID;
}

ThreadIdent ThreadManager::getCurrentThreadId()
{
   return pthread_self();
}

bool ThreadManager::compare(ThreadIdent threadId_1, ThreadIdent threadId_2)
{
   return pthread_equal((pthread_t)threadId_1, (pthread_t)threadId_2);
}

class PlatformThreadStorage
{
	public:
		  pthread_key_t mThreadKey;
};

/*ThreadStorage::ThreadStorage()
{
	  mThreadStorage = (PlatformThreadStorage *) mStorage;
	    constructInPlace(mThreadStorage);

	      pthread_key_create(&mThreadStorage->mThreadKey, NULL);
}

ThreadStorage::~ThreadStorage()
{
	  pthread_key_delete(mThreadStorage->mThreadKey);
}

void *ThreadStorage::get()
{
	  return pthread_getspecific(mThreadStorage->mThreadKey);
}

void ThreadStorage::set(void *value)
{
	  pthread_setspecific(mThreadStorage->mThreadKey, value);
}
*/

