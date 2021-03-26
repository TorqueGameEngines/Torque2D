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

extern void PlatformBlitInit();
extern void SetProcessorInfo(TorqueSystemInfo::Processor& pInfo,
   char* vendor, U32 processor, U32 properties); // platform/platformCPU.cc

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

   char     vendor[13] = {0,};
   U32   properties = 0;
   U32   processor  = 0;

   SetProcessorInfo(PlatformSystemInfo.processor, vendor, processor, properties);

// now calculate speed of processor...
   U16 nearmhz = 0; // nearest rounded mhz
   U16 mhz = 0; // calculated value.

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

   if (PlatformSystemInfo.processor.properties & CPU_PROP_FPU)
      Con::printf("   FPU detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_MMX)
      Con::printf("   MMX detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_3DNOW)
      Con::printf("   3DNow detected");
   if (PlatformSystemInfo.processor.properties & CPU_PROP_SSE)
      Con::printf("   SSE detected");
   Con::printf(" ");

   PlatformBlitInit();
}
