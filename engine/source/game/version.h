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

#ifndef _ENGINE_VERSION_H_
#define _ENGINE_VERSION_H_

// Engine Version.
#define T2D_ENGINE_VERSION            "v3.5"    ///< Engine Version String.
#define T2D_IPHONETOOLS_VERSION       "v3.5"    ///< Engine Version String for iPhone tools. Changing this will allow a fresh AppData folder to avoid conflicts with other builds existing on the system.

/// Gets the specified version number.  The version number is specified as a global in version.cc
U32 getVersionNumber();

/// Gets the version number in string form
const char* getVersionString();

/// Gets the compile date and time
const char* getCompileTimeString();

const char* getCompanyName();
const char* getProductName();

void setCompanyName(StringTableEntry companyName);
void setProductName(StringTableEntry productName);

#endif // _ENGINE_VERSION_H_
