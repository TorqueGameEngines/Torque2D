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

#ifndef _PLATFORM_CPU_H_
#define _PLATFORM_CPU_H_

#ifdef TORQUE_OS_WIN
#include <intrin.h>
#endif // TORQUE_OS_WIN


//-----------------------------------------------------------------------------

enum ProcessorType
{
   // x86
   CPU_X86Compatible,
   CPU_Intel_Unknown,
   CPU_Intel_486,
   CPU_Intel_Pentium,
   CPU_Intel_PentiumMMX,
   CPU_Intel_PentiumPro,
   CPU_Intel_PentiumII,
   CPU_Intel_PentiumCeleron,
   CPU_Intel_PentiumIII,
   CPU_Intel_Pentium4,
   CPU_Intel_PentiumM,
   CPU_Intel_Core,
   CPU_Intel_Core2,
   CPU_Intel_Corei7Xeon, // Core i7 or Xeon
   CPU_AMD_K6,
   CPU_AMD_K6_2,
   CPU_AMD_K6_3,
   CPU_AMD_Athlon,
   CPU_AMD_Phenom,
   CPU_AMD_PhenomII,
   CPU_AMD_Bulldozer,
   CPU_AMD_Unknown,
   CPU_Cyrix_6x86,
   CPU_Cyrix_MediaGX,
   CPU_Cyrix_6x86MX,
   CPU_Cyrix_GXm,          ///< Media GX w/ MMX
   CPU_Cyrix_Unknown,

   // PowerPC
   CPU_PowerPC_Unknown,
   CPU_PowerPC_601,
   CPU_PowerPC_603,
   CPU_PowerPC_603e,
   CPU_PowerPC_603ev,
   CPU_PowerPC_604,
   CPU_PowerPC_604e,
   CPU_PowerPC_604ev,
   CPU_PowerPC_G3,
   CPU_PowerPC_G4,
   CPU_PowerPC_G4_7450,
   CPU_PowerPC_G4_7455,
   CPU_PowerPC_G4_7447,
   CPU_PowerPC_G5,
};

//-----------------------------------------------------------------------------

enum ProcessprProperties
{
   CPU_PROP_C = (1 << 0),  ///< We should use C fallback math functions.
   CPU_PROP_FPU = (1 << 1),  ///< Has an FPU. (It better!)
   CPU_PROP_MMX = (1 << 2),  ///< Supports MMX instruction set extension.
   CPU_PROP_3DNOW = (1 << 3),  ///< Supports AMD 3dNow! instruction set extension.
   CPU_PROP_SSE = (1 << 4),  ///< Supports SSE instruction set extension.
   CPU_PROP_RDTSC = (1 << 5),  ///< Supports Read Time Stamp Counter op.
   CPU_PROP_SSE2 = (1 << 6),  ///< Supports SSE2 instruction set extension.
   CPU_PROP_SSE3 = (1 << 7),  ///< Supports SSE3 instruction set extension.  
   CPU_PROP_SSE3xt = (1 << 8),  ///< Supports extended SSE3 instruction set  
   CPU_PROP_SSE4_1 = (1 << 9),  ///< Supports SSE4_1 instruction set extension.  
   CPU_PROP_SSE4_2 = (1 << 10), ///< Supports SSE4_2 instruction set extension.  
   CPU_PROP_MP = (1 << 11), ///< This is a multi-processor system.
   CPU_PROP_LE = (1 << 12), ///< This processor is LITTLE ENDIAN.  
   CPU_PROP_64bit = (1 << 13), ///< This processor is 64-bit capable
};

//-----------------------------------------------------------------------------

enum PPCProperties
{
    // PowerPC properties
    CPU_PROP_PPCMIN    = (1<<0),
    CPU_PROP_ALTIVEC   = (1<<1),     // Float-SIMD
    CPU_PROP_PPCMP     = (1<<7)      // Multi-processor system
};

//-----------------------------------------------------------------------------

struct Processor
{
   static void init();
};

class CPUID
{
   U32 reg[4];
   public:
      explicit CPUID(U32 funcId, U32 subFuncId) {
#ifdef TORQUE_OS_WIN
         __cpuidex((int *)reg, (int)funcId, (int)subFuncId);
#endif
      }

      const U32 &EAX() const { return reg[0]; }
      const U32 &EBX() const { return reg[1]; }
      const U32 &ECX() const { return reg[2]; }
      const U32 &EDX() const { return reg[3]; }
};

//-----------------------------------------------------------------------------

struct TorqueSystemInfo
{
   struct Processor
   {
      ProcessorType  type;
      const char*    name;
      U32            mhz;
      bool           isMultiCore;
      bool           isHyperThreaded;
      U32            numLogicalProcessors;
      U32            numPhysicalProcessors;
      U32            numAvailableCores;
      U32            properties;      // CPU type specific enum
   } processor;
};

extern TorqueSystemInfo PlatformSystemInfo;

#endif // _PLATFORM_CPU_H_
