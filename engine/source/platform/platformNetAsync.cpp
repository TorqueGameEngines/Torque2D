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
#include "platform/threads/mutex.h"
#include "platform/threads/thread.h"
#include "platform/platformNetAsync.h"
#include "console/console.h"

#if defined(TORQUE_OS_WIN)
#  include <winsock.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#include <errno.h>
#include <string.h>

NetAsync gNetAsync;

#define LOOKUP_REQUEST_CHECK_INTERVAL	500

void* gNetAsyncMutex = NULL;

static void lockNetAsyncMutex()
{
   if(!gNetAsyncMutex)
      gNetAsyncMutex = Mutex::createMutex();

   AssertFatal(gNetAsyncMutex, "Could not create gNetAsyncMutex!");
   
   Mutex::lockMutex(gNetAsyncMutex);
}

static void unlockNetAsyncMutex()
{
   if(!gNetAsyncMutex)
      gNetAsyncMutex = Mutex::createMutex();

   AssertFatal(gNetAsyncMutex, "Could not create gNetAsyncMutex!");
   
   Mutex::unlockMutex(gNetAsyncMutex);
}

// internal structure for storing information about a name lookup request
struct NameLookupRequest
{
      NetSocket sock;
      char remoteAddr[4096];
      char out_h_addr[4096];
      int out_h_length;
      bool complete;

      NameLookupRequest()
      {
         sock = NetSocket::INVALID;
         remoteAddr[0] = 0;
         out_h_addr[0] = 0;
         out_h_length = -1;
         complete = false;
      }
};

void NetAsync::queueLookup(const char* remoteAddr, NetSocket socket)
{
   lockNetAsyncMutex();
   // do we have it already?
   unsigned int i = 0;
   for (i = 0; i < mLookupRequests.size(); ++i)
   {
      if (mLookupRequests[i]->sock == socket)
         // found it.  ignore more than one lookup at a time for a socket.
         break;
   }

   if (i == mLookupRequests.size())
   {
      // not found, so add it
      NameLookupRequest* lookupRequest = new NameLookupRequest();
      lookupRequest->sock = socket;
      dStrncpy(lookupRequest->remoteAddr, remoteAddr, 
               sizeof(lookupRequest->remoteAddr));
      mLookupRequests.push_back(lookupRequest);
   }
   unlockNetAsyncMutex();
}

void NetAsync::run()
{
   if (isRunning())
      return;

   mRunning = true;
   NameLookupRequest* lookupRequest = NULL;

   while (isRunning())
   {
      lookupRequest = NULL;

      // lock 
      lockNetAsyncMutex();
      // if there is a request...
      if (mLookupRequests.size() > 0)
      {
         // assign the first incomplete request
         for (unsigned int i = 0; i < mLookupRequests.size(); ++i)
            if (!mLookupRequests[i]->complete)
               lookupRequest = mLookupRequests[i];
      }

      // unlock so that more requests can be added
      unlockNetAsyncMutex();

      // if we have a lookup request
      if (lookupRequest != NULL)
      {
          NetAddress address;
	       Net::Error error = Net::stringToAddress(lookupRequest->remoteAddr, &address, true);
         // do it
         if (error != Net::NoError)
         {
            // oh well!  leave the lookup data unmodified (h_length) should
            // still be -1 from initialization
            lookupRequest->complete = true;
         }
         else
         {
            // copy the stuff we need from the hostent 
            dMemset(lookupRequest->out_h_addr, 0, 
                   sizeof(lookupRequest->out_h_addr));
            dMemcpy(lookupRequest->out_h_addr, &address, sizeof(address));

            lookupRequest->out_h_length = sizeof(address);
            lookupRequest->complete = true;
         }
      }
      else
      {
         // no lookup request.  sleep for a bit
         Platform::sleep(LOOKUP_REQUEST_CHECK_INTERVAL);
      }
   };
}

bool NetAsync::checkLookup(NetSocket socket, void* out_h_addr, 
                           int* out_h_length, int out_h_addr_size)
{
   lockNetAsyncMutex();
   bool found = false;
   // search for the socket
   Vector<NameLookupRequest*>::iterator iter;
   for (iter = mLookupRequests.begin(); 
        iter != mLookupRequests.end(); 
        ++iter)
      // if we found it and it is complete...
      if (socket == (*iter)->sock && (*iter)->complete)
      {
         // copy the lookup data to the callers parameters
         dMemcpy(out_h_addr, (*iter)->out_h_addr, out_h_addr_size);
         *out_h_length = (*iter)->out_h_length;
         found = true;
         break;
      }

   // we found the socket, so we are done with it.  erase.
   if (found)
   {
      delete *iter;
      mLookupRequests.erase(iter);
   }
   unlockNetAsyncMutex();

   return found;
}

// this is called by the pthread module to start the thread
static void StartThreadFunc(S32 nothing)
{
   if (gNetAsync.isRunning())
      return;

   gNetAsync.run();
   return;
}


void NetAsync::startAsync()
{
  if (gNetAsync.isRunning())
     return;

  // create the thread...
   Thread *zThread = new Thread((ThreadRunFunction)StartThreadFunc, 0, true);

  if (!zThread)
     Con::errorf("Error starting net async thread.");
}

void NetAsync::stopAsync()
{
   if (gNetAsync.isRunning())
      gNetAsync.stop();
}
