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

#ifndef _GPALETTE_H_
#define _GPALETTE_H_

//Includes
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

//-------------------------------------- Forward decls.
class Stream;

//------------------------------------------------------------------------------
//-------------------------------------- GPalette
//
class GPalette
{
  public:
   enum PaletteType {
      RGB,
      RGBA
   };

  protected:
   PaletteType m_paletteType;
   ColorI      m_pColors[256];

  public:
   GPalette();
   virtual ~GPalette();

   PaletteType getPaletteType() const;
   void setPaletteType(const PaletteType pt) { m_paletteType = pt; }

   const ColorI* getColors() const;
   ColorI*       getColors();
   const ColorI& getColor(const U32 in_index) const;
   ColorI&       getColor(const U32 in_index);

   //-------------------------------------- Supplimentary output members
  public:
   bool readMSPalette(Stream& io_rStream);
   bool readMSPalette(const char* in_pFileName);
   bool writeMSPalette(Stream& io_rStream) const;
   bool writeMSPalette(const char* in_pFileName) const;

   //-------------------------------------- Persistent members
  public:
   bool read(Stream& io_rStream);
   bool write(Stream& io_rStream) const;
  private:
   static const U32 csm_fileVersion;
};

//------------------------------------------------------------------------------
//-------------------------------------- Inlines (Trust)
//
inline GPalette::PaletteType
GPalette::getPaletteType() const
{
   return m_paletteType;
}

inline const ColorI*
GPalette::getColors() const
{
   return m_pColors;
}

inline ColorI*
GPalette::getColors()
{
   return m_pColors;
}

inline const ColorI&
GPalette::getColor(const U32 in_index) const
{
   AssertFatal(in_index < 256, "Out of range index");

   return m_pColors[in_index];
}

inline ColorI&
GPalette::getColor(const U32 in_index)
{
   AssertFatal(in_index < 256, "Out of range index");

   return m_pColors[in_index];
}

#endif //_GPALETTE_H_
