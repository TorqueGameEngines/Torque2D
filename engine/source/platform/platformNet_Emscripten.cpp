//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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
#if defined(TORQUE_OS_EMSCRIPTEN)
#include "platform/platformNet.h"
#include "platform/event.h"
#include "console/console.h"
#include "game/gameInterface.h"

NetSocket NetSocket::INVALID = NetSocket::fromHandle(-1);

static void IPSocketToNetAddress(const struct sockaddr_in *sockAddr, NetAddress *address);
static void IPSocket6ToNetAddress(const struct sockaddr_in6 *sockAddr, NetAddress *address);

bool Net::init()
{
   return(true);
}

void Net::shutdown()
{

   
}

// ipv4 version of name routines

static void NetAddressToIPSocket(const NetAddress *address, struct sockaddr_in *sockAddr)
{
  
}

static void IPSocketToNetAddress(const struct sockaddr_in *sockAddr, NetAddress *address)
{

}

// ipv6 version of name routines

static void NetAddressToIPSocket6(const NetAddress *address, struct sockaddr_in6 *sockAddr)
{

}

static void IPSocket6ToNetAddress(const struct sockaddr_in6 *sockAddr, NetAddress *address)
{
}

//

NetSocket Net::openListenPort(U16 port, NetAddress::Type addressType)
{
	return NetSocket::INVALID;
}

NetSocket Net::openConnectTo(const char *addressString)
{
	return NetSocket::INVALID;
}

void Net::closeConnectTo(NetSocket handleFd)
{
   
}

Net::Error Net::sendtoSocket(NetSocket handleFd, const U8 *buffer, S32  bufferSize, S32 *outBufferWritten)
{
	return NoError;
}

bool Net::openPort(S32 port, bool doBind)
{
	return false;
}

NetSocket Net::getPort()
{
	return NetSocket::INVALID;
}

void Net::closePort()
{

}

Net::Error Net::sendto(const NetAddress *address, const U8 *buffer, S32  bufferSize)
{
	return NoError;
}

void Net::process()
{
   
}

void Net::processListenSocket(NetSocket socketHandle)
{
}

NetSocket Net::openSocket()
{
	return NetSocket::INVALID;
}

Net::Error Net::closeSocket(NetSocket handleFd)
{
	return NoError;
}

Net::Error Net::connect(NetSocket handleFd, const NetAddress *address)
{
	return NoError;
}

Net::Error Net::listen(NetSocket handleFd, S32 backlog)
{
	return NoError;
}

NetSocket Net::accept(NetSocket handleFd, NetAddress *remoteAddress)
{
   return NetSocket::INVALID;
}

Net::Error Net::bindAddress(const NetAddress &address, NetSocket handleFd, bool useUDP)
{
	return NoError;
}

Net::Error Net::setBufferSize(NetSocket handleFd, S32 bufferSize)
{
	return NoError;
}

Net::Error Net::setBroadcast(NetSocket handleFd, bool broadcast)
{
	return NoError;
}

Net::Error Net::setBlocking(NetSocket handleFd, bool blockingIO)
{
	return NoError;
}

Net::Error Net::getListenAddress(const NetAddress::Type type, NetAddress *address, bool forceDefaults)
{
	return NoError;
}

void Net::getIdealListenAddress(NetAddress *address)
{
}

Net::Error Net::send(NetSocket handleFd, const U8 *buffer, S32 bufferSize, S32 *outBytesWritten)
{
	return NoError;
}

Net::Error Net::recv(NetSocket handleFd, U8 *buffer, S32 bufferSize, S32  *bytesRead)
{
   return NoError;
}

bool Net::compareAddresses(const NetAddress *a1, const NetAddress *a2)
{
	return false;
}

Net::Error Net::stringToAddress(const char *addressString, NetAddress  *address, bool hostLookup, int requiredFamily)
{
   return NoError;
}

void Net::addressToString(const NetAddress *address, char  addressString[256])
{
}

void Net::enableMulticast()
{
}

void Net::disableMulticast()
{
}

bool Net::isMulticastEnabled()
{
   return false;
}

U32 NetAddress::getHash() const
{
   return 0;
}

bool Net::isAddressTypeAvailable(NetAddress::Type addressType)
{
   return false;
}
#endif