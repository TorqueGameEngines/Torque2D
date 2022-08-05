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

#ifndef _GBITMAP_H_
#define _GBITMAP_H_

//Includes
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _RESMANAGER_H_
#include "io/resource/resourceManager.h"
#endif
#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif

//-------------------------------------- Forward decls.
class Stream;
class GPalette;
class RectI;

extern ResourceInstance* constructBitmapBMP(Stream& stream);
extern ResourceInstance* constructBitmapPNG(Stream& stream);
extern ResourceInstance* constructBitmapJPEG(Stream& stream);

#ifdef TORQUE_OS_IOS
extern ResourceInstance* constructBitmapPVR(Stream& stream);
#endif

//------------------------------------------------------------------------------
//-------------------------------------- GBitmap
//
class GBitmap: public ResourceInstance
{
   //-------------------------------------- public enumerants and structures
  public:
   /// BitmapFormat and UsageHint are
   ///  written to the stream in write(...),
   ///  be sure to maintain compatability
   ///  if they are changed.
   enum BitmapFormat {
      Palettized = 0,
      Intensity  = 1,
      RGB        = 2,
      RGBA       = 3,
      Alpha      = 4,
      RGB565     = 5,
      RGB5551    = 6,
      Luminance  = 7,
      LuminanceAlpha = 8
#ifdef TORQUE_OS_IOS
       , PVR2 = 9,
       PVR2A = 10,
       PVR4 = 11,
       PVR4A = 12
#endif
   };

   enum Constants {
      c_maxMipLevels = 12 //(2^(12 + 1) = 2048)
   };

  public:

   static GBitmap *load(const char *path);
   static ResourceObject * findBmpResource(const char * path);

   GBitmap();
   GBitmap(const GBitmap&);
   GBitmap(const U32  in_width,
           const U32  in_height,
           const bool in_extrudeMipLevels = false,
           const BitmapFormat in_format = RGB);
   virtual ~GBitmap();

   void allocateBitmap(const U32  in_width,
                       const U32  in_height,
                       const bool in_extrudeMipLevels = false,
                       const BitmapFormat in_format = RGB);

   void extrudeMipLevels(bool clearBorders = false);
   void extrudeMipLevelsDetail();

   GBitmap *createPowerOfTwoBitmap();

   void copyRect(const GBitmap *src, const RectI &srcRect, const Point2I &dstPoint);

   BitmapFormat getFormat()       const;
   bool         setFormat(BitmapFormat fmt);
   U32          getNumMipLevels() const;
   U32          getWidth(const U32 in_mipLevel  = 0) const;
   U32          getHeight(const U32 in_mipLevel = 0) const;

   U8*         getAddress(const S32 in_x, const S32 in_y, const U32 mipLevel = U32(0));
   const U8*   getAddress(const S32 in_x, const S32 in_y, const U32 mipLevel = U32(0)) const;

   const U8*   getBits(const U32 in_mipLevel = 0) const;
   U8*         getWritableBits(const U32 in_mipLevel = 0);

   bool        getColorBGRA(const U32 x, const U32 y, ColorI& rColor) const;
   bool        setColorBGRA(const U32 x, const U32 y, ColorI& rColor);
   bool        getColor(const U32 x, const U32 y, ColorI& rColor) const;
   bool        setColor(const U32 x, const U32 y, ColorI& rColor);

   /// Note that on set palette, the bitmap deletes its palette.
   GPalette const* getPalette() const;
   void            setPalette(GPalette* in_pPalette);

   //-------------------------------------- Internal data/operators
   static U32 sBitmapIdSource;

   void deleteImage();
   void clearImage(const ColorF blendColor);

   BitmapFormat internalFormat;
  public:

   U8* pBits;            // Master bytes
   U32 byteSize;
   U32 width;            // Top level w/h
   U32 height;
   U32 bytesPerPixel;

   U32 numMipLevels;
   U32 mipLevelOffsets[c_maxMipLevels];

   bool mForce16Bit;//-Mat some paletted images will always be 16bit
   GPalette* pPalette;      ///< Note that this palette pointer is ALWAYS
                            ///  owned by the bitmap, and will be
                            ///  deleted on exit, or written out on a
                            ///  write.

   bool mergeLayer(const Point2I pos, const GBitmap* layer, const ColorF blendColor);

   //-------------------------------------- Input/Output interface
  public:
   bool readJPEG(Stream& io_rStream);              // located in bitmapJpeg.cc
   bool writeJPEG(Stream& io_rStream) const;

   bool readPNG(Stream& io_rStream);               // located in bitmapPng.cc
   bool writePNG(Stream& io_rStream, const bool compressHard = false) const;
   bool writePNGUncompressed(Stream& io_rStream) const;

   bool readMSBmp(Stream& io_rStream);             // located in bitmapMS.cc
   bool writeMSBmp(Stream& io_rStream) const;      // located in bitmapMS.cc

#ifdef TORQUE_OS_IOS
    bool readPNGiPhone(Stream& io_rStream);               // located in iPhoneUtil.mm
    bool readPvr(Stream& io_rStream);		// located in bitmapPvr.cc for IPHONE
    bool writePvr(Stream& io_rStreeam) const;
#endif
    
   bool read(Stream& io_rStream);
   bool write(Stream& io_rStream) const;

  private:
   bool _writePNG(Stream&   stream, const U32, const U32, const U32) const;

   static const U32 csFileVersion;
};

//------------------------------------------------------------------------------
//-------------------------------------- Inlines
//

inline GBitmap::BitmapFormat GBitmap::getFormat() const
{
   return internalFormat;
}

inline U32 GBitmap::getNumMipLevels() const
{
   return numMipLevels;
}

inline U32 GBitmap::getWidth(const U32 in_mipLevel) const
{
   AssertFatal(in_mipLevel < numMipLevels,
               avar("GBitmap::getWidth: mip level out of range: (%d, %d)",
                    in_mipLevel, numMipLevels));

   U32 retVal = width >> in_mipLevel;

   return (retVal != 0) ? retVal : 1;
}

inline U32 GBitmap::getHeight(const U32 in_mipLevel) const
{
   AssertFatal(in_mipLevel < numMipLevels,
               avar("Bitmap::getHeight: mip level out of range: (%d, %d)",
                    in_mipLevel, numMipLevels));

   U32 retVal = height >> in_mipLevel;

   return (retVal != 0) ? retVal : 1;
}

inline const GPalette* GBitmap::getPalette() const
{
   AssertFatal(getFormat() == Palettized,
               "Error, incorrect internal format to return a palette");

   return pPalette;
}

inline const U8* GBitmap::getBits(const U32 in_mipLevel) const
{
   AssertFatal(in_mipLevel < numMipLevels,
               avar("GBitmap::getBits: mip level out of range: (%d, %d)",
                    in_mipLevel, numMipLevels));

   return &pBits[mipLevelOffsets[in_mipLevel]];
}

inline U8* GBitmap::getWritableBits(const U32 in_mipLevel)
{
   AssertFatal(in_mipLevel < numMipLevels,
               avar("GBitmap::getWritableBits: mip level out of range: (%d, %d)",
                    in_mipLevel, numMipLevels));

   return &pBits[mipLevelOffsets[in_mipLevel]];
}

inline U8* GBitmap::getAddress(const S32 in_x, const S32 in_y, const U32 mipLevel)
{
   return (getWritableBits(mipLevel) + ((in_y * getWidth(mipLevel)) + in_x) * bytesPerPixel);
}

inline const U8* GBitmap::getAddress(const S32 in_x, const S32 in_y, const U32 mipLevel) const
{
   return (getBits(mipLevel) + ((in_y * getWidth(mipLevel)) + in_x) * bytesPerPixel);
}


extern void (*bitmapExtrude5551)(const void *srcMip, void *mip, U32 height, U32 width);
extern void (*bitmapExtrudeRGB)(const void *srcMip, void *mip, U32 height, U32 width);
extern void (*bitmapConvertRGB_to_5551)(U8 *src, U32 pixels);
extern void (*bitmapExtrudePaletted)(const void *srcMip, void *mip, U32 height, U32 width);

void bitmapExtrudeRGB_c(const void *srcMip, void *mip, U32 height, U32 width);

#endif //_GBITMAP_H_
