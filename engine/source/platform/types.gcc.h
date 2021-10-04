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

#ifndef INCLUDED_TYPES_GCC_H
#define INCLUDED_TYPES_GCC_H


// For additional information on GCC predefined macros
// http://gcc.gnu.org/onlinedocs/gcc-3.0.2/cpp.html


//--------------------------------------
// Types
typedef signed long long    S64;
typedef unsigned long long  U64;


//--------------------------------------
// Compiler Version
#define TORQUE_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)


//--------------------------------------
// Identify the compiler string

#if defined(__MINGW32__)
#  define TORQUE_COMPILER_STRING "GCC (MinGW)"
#  define TORQUE_COMPILER_MINGW
#elif defined(__CYGWIN__)
#  define TORQUE_COMPILER_STRING "GCC (Cygwin)"
#  define TORQUE_COMPILER_MINGW
#else
#  define TORQUE_COMPILER_STRING "GCC "
#endif

//--------------------------------------
// Identify the Operating System
#if defined(_WIN64)
#  define TORQUE_OS_STRING "Win64"
#  define TORQUE_OS_WIN
#  define TORQUE_OS_WIN64
#  include "platform/types.win32.h"
#elif defined(__WIN32__) || defined(_WIN32)
#  define TORQUE_OS_STRING "Win32"
#  define TORQUE_OS_WIN32
#  define TORQUE_SUPPORTS_NASM
#  define TORQUE_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.win32.h"
#elif defined(__ANDROID__)
#ifndef TORQUE_OS_ANDROID
		#define TORQUE_OS_ANDROID
#endif
#  include "platform/types.arm.h"
#elif defined(EMSCRIPTEN)
#  define TORQUE_OS_STRING "Emscripten"
#  define TORQUE_OS_EMSCRIPTEN
#  include "platform/types.posix.h"
#elif defined(linux)
#  define TORQUE_OS_STRING "Linux"
#  define TORQUE_OS_LINUX
// Don't use assembly when building linux.
//#  define TORQUE_SUPPORTS_NASM
//#  define TORQUE_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(__OpenBSD__)
#  define TORQUE_OS_STRING "OpenBSD"
#  define TORQUE_OS_OPENBSD
#  define TORQUE_SUPPORTS_NASM
#  define TORQUE_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(__FreeBSD__)
#  define TORQUE_OS_STRING "FreeBSD"
#  define TORQUE_OS_FREEBSD
#  define TORQUE_SUPPORTS_NASM
#  define TORQUE_SUPPORTS_GCC_INLINE_X86_ASM
#  include "platform/types.posix.h"

#elif defined(TORQUE_OS_IOS)
	#ifndef TORQUE_OS_IOS
		#define TORQUE_OS_IOS
	#endif
#  include "platform/types.arm.h"

#elif defined(__APPLE__)
#ifndef TORQUE_OS_MAC
#  define TORQUE_OS_MAC
#endif
#ifndef TORQUE_OS_OSX
#  define TORQUE_OS_OSX
#endif
#if defined(i386)
#  define TORQUE_SUPPORTS_NASM
#endif
#  include "platform/types.ppc.h"

#else 
#  error "GCC: Unsupported Operating System"
#endif


//--------------------------------------
// Identify the CPU
#if defined(i386)
#  define TORQUE_CPU_STRING "Intel x86"
#  define TORQUE_CPU_X86
#  define TORQUE_LITTLE_ENDIAN

#elif defined(__amd64__)
#  define TORQUE_CPU_STRING "Intel x86-64"
#  define TORQUE_CPU_X86_64
#  define TORQUE_LITTLE_ENDIAN
#  define TORQUE_CPU_X64

#elif defined(__ppc__)
#  define TORQUE_CPU_STRING "PowerPC"
#  define TORQUE_CPU_PPC
#  define TORQUE_BIG_ENDIAN

#elif defined(__arm__)
#  define TORQUE_CPU_STRING "ARM"
#  define TORQUE_CPU_ARM
#  define TORQUE_LITTLE_ENDIAN

#elif defined(__arm64__)
#  define TORQUE_CPU_STRING "ARM"
#  define TORQUE_CPU_ARM
#  define TORQUE_LITTLE_ENDIAN
#  define TORQUE_CPU_X64

#elif defined(EMSCRIPTEN)

#  define TORQUE_OS_STRING "Emscripten"
#  define TORQUE_LITTLE_ENDIAN

#else
#  error "GCC: Unsupported Target CPU"
#endif


#endif // INCLUDED_TYPES_GCC_H

