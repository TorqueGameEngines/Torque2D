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

#ifndef _BITMAP_FONT_CHARACTER_INFO_H_
#define _BITMAP_FONT_CHARACTER_INFO_H_

#ifndef _UTILITY_H_
#include "2d/core/utility.h"
#endif

#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

namespace font
{
   struct BitmapFontCharacterInfo
   {
   public:
      bool mUseColor;
      ColorF mColor;
      F32 mOffsetX;
      F32 mOffsetY;
      F32 mScaleX;
      F32 mScaleY;

      BitmapFontCharacterInfo() : mUseColor(false), mColor(1.0f, 1.0f, 1.0f, 1.0f), mOffsetX(0.0f), mOffsetY(0.0f), mScaleX(1.0f), mScaleY(1.0f)
      {

      }

      BitmapFontCharacterInfo(ColorF color) : mOffsetX(0.0f), mOffsetY(0.0f), mScaleX(1.0f), mScaleY(1.0f)
      {
         mColor = color;
         mUseColor = true;
      }

      inline bool isDefault() { return (!mUseColor && mOffsetX == 0.0f && mOffsetY == 0.0f && mScaleX == 1.0f && mScaleY == 1.0f); }
   };
}

#endif // _BITMAP_FONT_CHARACTER_INFO_H_