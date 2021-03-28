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

#ifndef _CONSOLETYPES_H_
#define _CONSOLETYPES_H_

#ifndef _CONSOLE_BASE_TYPE_H_
#include "console/consoleBaseType.h"
#endif

#ifndef Offset
#if defined(TORQUE_COMPILER_GCC) && (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define Offset(m,T) ((int)(&((T *)1)->m) - 1)
#else
#define Offset(x, cls) ((dsize_t)((const char *)&(((cls *)0)->x)-(const char *)0))
#endif
#endif

// Define Core Console Types
DefineConsoleType( TypeF32 )
DefineConsoleType( TypeS8 )
DefineConsoleType( TypeS32 )
DefineConsoleType( TypeS32Vector )
DefineConsoleType( TypeBool )
DefineConsoleType( TypeBoolVector )
DefineConsoleType( TypeF32Vector )
DefineConsoleType( TypeString )
DefineConsoleType( TypeName )
DefineConsoleType( TypeStringTableEntryVector )
DefineConsoleType( TypeCaseString )
DefineConsoleType( TypeFilename )
DefineConsoleType( TypeEnum )
DefineConsoleType( TypeFlag )
DefineConsoleType( TypeSimObjectPtr )
DefineConsoleType( TypeSimObjectName )
DefineConsoleType( TypeSimObjectId )

#endif
