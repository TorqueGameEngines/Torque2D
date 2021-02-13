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
#include "platformWin32/platformWin32.h"
#include "console/console.h"
#include "string/stringTable.h"
#include <math.h>
#include <intrin.h>

extern void PlatformBlitInit();
extern void SetProcessorInfo(TorqueSystemInfo::Processor& pInfo,
   char* vendor, U32 processor, U32 properties, U32 properties2); // platform/platformCPU.cc


#if defined(TORQUE_SUPPORTS_NASM)
// asm cpu detection routine from platform code
extern "C"
{
   void detectX86CPUInfo(char *vendor, U32 *processor, U32 *properties);
}
#endif


void Processor::init()
{
   Con::printSeparator();
   Con::printf("Processor Initialization:");

   // Reference:
   //    www.cyrix.com
   //    www.amd.com
   //    www.intel.com
   //       http://developer.intel.com/design/PentiumII/manuals/24512701.pdf

   PlatformSystemInfo.processor.type = CPU_X86Compatible;
   PlatformSystemInfo.processor.name = StringTable->insert("Unknown x86 Compatible");
   PlatformSystemInfo.processor.mhz  = 0;
   PlatformSystemInfo.processor.properties = CPU_PROP_C;

   char  vendor[0x20];
   dMemset(vendor, 0, sizeof(vendor));
   U32   properties = 0;
   U32   processor = 0;
   U32   properties2 = 0;

   S32 vendorInfo[4];
   __cpuid(vendorInfo, 0);
   *reinterpret_cast<int*>(vendor) = vendorInfo[1];     // ebx
   *reinterpret_cast<int*>(vendor + 4) = vendorInfo[3]; // edx
   *reinterpret_cast<int*>(vendor + 8) = vendorInfo[2]; // ecx

   S32 cpuInfo[4];
   __cpuid(cpuInfo, 1);
   processor = cpuInfo[0];   // eax
   properties = cpuInfo[3];  // edx
   properties2 = cpuInfo[2]; // ecx

#if defined(TORQUE_SUPPORTS_NASM)

   detectX86CPUInfo(vendor, &processor, &properties);

#elif defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)
   __asm
   {
      //--------------------------------------
      // is CPUID supported
      push     ebx
      push     edx
      push     ecx
      pushfd
      pushfd                     // save EFLAGS to stack
      pop      eax               // move EFLAGS into EAX
      mov      ebx, eax
      xor      eax, 0x200000     // flip bit 21
      push     eax
      popfd                      // restore EFLAGS
      pushfd
      pop      eax
      cmp      eax, ebx
      jz       EXIT              // doesn't support CPUID instruction

      //--------------------------------------
      // Get Vendor Informaion using CPUID eax==0
      xor      eax, eax
      cpuid

      mov      DWORD PTR vendor, ebx
      mov      DWORD PTR vendor+4, edx
      mov      DWORD PTR vendor+8, ecx

      // get Generic Extended CPUID info
      mov      eax, 1
      cpuid                      // eax=1, so cpuid queries feature information

      and      eax, 0x0ff0
      mov      processor, eax    // just store the model bits
      mov      properties, edx

      // Want to check for 3DNow(tm).  Need to see if extended cpuid functions present.
      mov      eax, 0x80000000
      cpuid
      cmp      eax, 0x80000000
      jbe      MAYBE_3DLATER
      mov      eax, 0x80000001
      cpuid
      and      edx, 0x80000000      // 3DNow if bit 31 set -> put bit in our properties
      or       properties, edx
   MAYBE_3DLATER:


   EXIT:
      popfd
      pop      ecx
      pop      edx
      pop      ebx
   }
#endif

   SetProcessorInfo(PlatformSystemInfo.processor, vendor, processor, properties, properties2);

// now calculate speed of processor...
   U16 nearmhz = 0; // nearest rounded mhz
   U16 mhz = 0; // calculated value.
#if defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM) || defined(TORQUE_COMPILER_MINGW)
   //--------------------------------------
   // if RDTSC support calculate the aproximate Mhz of the CPU
   if (PlatformSystemInfo.processor.properties & CPU_PROP_RDTSC &&
       PlatformSystemInfo.processor.properties & CPU_PROP_FPU)
   {
      const U32 msToWait = 1000; // bigger this is, more accurate we are.
      U32 tsLo1 = 0, tsHi1 = 0; // time stamp storage.
      U32 tsLo2 = 0, tsHi2 = 0; // time stamp storage.
      U16 Nearest66Mhz = 0, Delta66Mhz = 0;
      U16 Nearest50Mhz = 0, Delta50Mhz = 0;
      F64 tsFirst, tsSecond, tsDelta;
      U32 ms;

      // starting time marker.
      ms = GetTickCount(); // !!!!TBD - this function may have too high an error... dunno.

   #if defined(TORQUE_COMPILER_MINGW)
      asm ("rdtsc" : "=a" (tsLo1), "=d" (tsHi1));
   #elif defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)  // VC|CW
      __asm
      {
           push  eax
           push  edx
         rdtsc
         mov  tsLo1, eax
         mov  tsHi1, edx
           pop  edx
           pop  eax
      }
   #endif

      // the faster this check and exit is, the more accurate the time-stamp-delta will be.
      while(GetTickCount() < ms + msToWait) {};

   #if defined(TORQUE_COMPILER_MINGW)
      asm ("rdtsc" : "=a" (tsLo2), "=d" (tsHi2));
   #elif defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)  // VC|CW
      __asm
      {
           push  eax
           push  edx
         rdtsc
         mov  tsLo2, eax
         mov  tsHi2, edx
           pop  edx
           pop  eax
      }
   #endif

      // do calculations in doubles for accuracy, since we're working with 64-bit math here...
      // grabbed this from the MINGW sample.
      tsFirst = ((F64)tsHi1 * (F64)0x10000 * (F64)0x10000) + (F64)tsLo1;
      tsSecond = ((F64)tsHi2 * (F64)0x10000 * (F64)0x10000) + (F64)tsLo2;
      // get the timestamp delta.  potentially large number here, as it's in Hz.
      tsDelta = tsSecond - tsFirst;

      // adjust for slightly-off-delay -- better to assume +1ms than try to really calc.
      tsDelta *= (F64)(msToWait + 1);
      tsDelta /= (F64)msToWait;
      // factor back into 1s of time.
      tsDelta *= ((F64)1000/(F64)msToWait);
      // then convert into Mhz
      tsDelta /= (F64)1000000;
      tsDelta += 0.5f; // trying to get closer to the right values, effectively rounding up.
      mhz = (U32)tsDelta;

      // Find nearest full/half multiple of 66/133 MHz
      Nearest66Mhz = ((((mhz * 3) + 100) / 200) * 200) / 3;
      // 660 = 1980 = 2080 = 100 = 2000 = 666
      // 440 = 1320 = 1420 = 70 = 1400 = 466

      // find delta to nearest 66 multiple.
      Delta66Mhz = abs(Nearest66Mhz - mhz);

      // Find nearest full/half multiple of 100 MHz
      Nearest50Mhz = (((mhz + 25) / 50) * 50);
      // 440 = 465 = 9 = 450

      // find delta to nearest 50 multiple.
      Delta50Mhz = abs(Nearest50Mhz - mhz);

      if (Delta50Mhz < Delta66Mhz) // closer to a 50 boundary
         nearmhz = Nearest50Mhz;
      else
      {
         nearmhz = Nearest66Mhz;
         if (nearmhz==666) // hack around -- !!!!TBD - other cases?!?!
            nearmhz = 667;
      }

      // !!!TBD
      // would be nice if we stored both the calculated and the adjusted/guessed values.
      PlatformSystemInfo.processor.mhz = nearmhz; // hold onto adjusted value only.
   }
#endif

   LONG result;
   DWORD data = 0;
   DWORD dataSize = 4;
   HKEY hKey;

   result = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);
   if (result == ERROR_SUCCESS)
   {
      result = ::RegQueryValueExA(hKey, "~MHz", NULL, NULL, (LPBYTE)&data, &dataSize);

      if (result == ERROR_SUCCESS)
         nearmhz = mhz = data;

      ::RegCloseKey(hKey);
   }

   PlatformSystemInfo.processor.mhz = mhz;

   if (mhz==0)
   {
      Con::printf("   %s, (Unknown) Mhz", PlatformSystemInfo.processor.name);
      // stick SOMETHING in so it isn't ZERO.
      PlatformSystemInfo.processor.mhz = 200; // seems a decent value.
   }
   else
   {
      if (nearmhz >= 1000)
         Con::printf("   %s, ~%.2f Ghz", PlatformSystemInfo.processor.name, ((float)nearmhz)/1000.0f);
      else
         Con::printf("   %s, ~%d Mhz", PlatformSystemInfo.processor.name, nearmhz);
      if (nearmhz != mhz)
      {
         if (mhz >= 1000)
            Con::printf("     (timed at roughly %.2f Ghz)", ((float)mhz)/1000.0f);
         else
            Con::printf("     (timed at roughly %d Mhz)", mhz);
      }
   }

   if (PlatformSystemInfo.processor.numAvailableCores > 0
      || PlatformSystemInfo.processor.numPhysicalProcessors > 0
      || PlatformSystemInfo.processor.isHyperThreaded)
      PlatformSystemInfo.processor.properties |= CPU_PROP_MP;

   if (PlatformSystemInfo.processor.properties & CPU_PROP_FPU)
      Con::printf("   FPU detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_MMX)
      Con::printf("   MMX detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_3DNOW)
      Con::printf("   3DNow detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_SSE)
      Con::printf("   SSE detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_SSE2)
      Con::printf("   SSE2 detected");
   if (PlatformSystemInfo.processor.isHyperThreaded)
      Con::printf("   HT detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_MP)
      Con::printf("   MP detected [%i cores, %i logical, %i physical]",
         PlatformSystemInfo.processor.numAvailableCores,
         PlatformSystemInfo.processor.numLogicalProcessors,
         PlatformSystemInfo.processor.numPhysicalProcessors);
   Con::printf(" ");

   PlatformBlitInit();
}
