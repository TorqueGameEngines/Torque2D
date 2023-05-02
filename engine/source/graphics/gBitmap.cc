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

#include "io/stream.h"
#include "io/fileStream.h"
#include "graphics/gBitmap.h"
#include "graphics/gPalette.h"
#include "io/resource/resourceManager.h"
#include "platform/platform.h"
#include "memory/safeDelete.h"
#include "math/mRect.h"
#include "console/console.h"

#ifndef _TORQUECONFIG_H_
#include "torqueConfig.h"//for PNG loading setting
#endif



const U32 GBitmap::csFileVersion   = 3;
U32       GBitmap::sBitmapIdSource = 0;


GBitmap::GBitmap()
 : internalFormat(RGB),
   pBits(NULL),
   byteSize(0),
   width(0),
   height(0),
   numMipLevels(0),
   bytesPerPixel(0),
   pPalette(NULL),
   mForce16Bit(false)
{
   for (U32 i = 0; i < c_maxMipLevels; i++)
      mipLevelOffsets[i] = 0xffffffff;
}

GBitmap::GBitmap(const GBitmap& rCopy)
{

   if (rCopy.pPalette)
   {
      pPalette = new GPalette;
      pPalette->setPaletteType(rCopy.pPalette->getPaletteType());
      dMemcpy(rCopy.pPalette->getColors(), pPalette->getColors(), sizeof(ColorI)*256);
   }
   else
      pPalette = NULL;

   internalFormat = rCopy.internalFormat;

   
   mForce16Bit = rCopy.mForce16Bit;

   byteSize = rCopy.byteSize;
   pBits    = new U8[byteSize];
   dMemcpy(pBits, rCopy.pBits, byteSize);

   width        = rCopy.width;
   height       = rCopy.height;
   bytesPerPixel = rCopy.bytesPerPixel;
   numMipLevels = rCopy.numMipLevels;
   dMemcpy(mipLevelOffsets, rCopy.mipLevelOffsets, sizeof(mipLevelOffsets));
}


GBitmap::GBitmap(const U32  in_width,
                 const U32  in_height,
                 const bool in_extrudeMipLevels,
                 const BitmapFormat in_format)
 : pBits(NULL),
   byteSize(0),
   pPalette(NULL),
   mForce16Bit(false)
{
   for (U32 i = 0; i < c_maxMipLevels; i++)
      mipLevelOffsets[i] = 0xffffffff;

   allocateBitmap(in_width, in_height, in_extrudeMipLevels, in_format);
}


//--------------------------------------------------------------------------
GBitmap::~GBitmap()
{
   deleteImage();
}


//--------------------------------------------------------------------------
void GBitmap::deleteImage()
{
   delete [] pBits;
   pBits    = NULL;
   byteSize = 0;

   width        = 0;
   height       = 0;
   numMipLevels = 0;

   SAFE_DELETE(pPalette);
}

void GBitmap::clearImage(const ColorF blendColor)
{
    ColorI clearColor(blendColor.red, blendColor.green, blendColor.blue, 0);
    for (U32 h = 0; h < getHeight(); h++)
    {
        for (U32 w = 0; w < getWidth(); w++)
        {
            setColor(w, h, clearColor);
        }
    }
}

bool GBitmap::mergeLayer(const Point2I pos, const GBitmap* layer, const ColorF blendColor)
{
    if (layer != nullptr && (getFormat() == BitmapFormat::RGBA || getFormat() == BitmapFormat::RGB) && (layer->getFormat() == BitmapFormat::RGBA || layer->getFormat() == BitmapFormat::RGB))
    {
        for (U32 h = getMax(0, pos.y); h < getHeight() && (h - pos.y) < layer->getHeight(); h++)
        {
            for (U32 w = getMax(0, pos.x); w < getWidth() && (w - pos.x) < layer->getWidth(); w++)
            {
                const U8* layerByte = layer->getAddress(w - pos.x, h - pos.y);
                U8* baseByte = getAddress(w, h);

                float layerAlpha = blendColor.alpha;
                if (layer->getFormat() == BitmapFormat::RGBA)
                {
                    layerAlpha = (layerByte[3] / 255.0f) * blendColor.alpha;
                }
                if (layerAlpha <= 0.005f)
                {
                    continue;
                }

                float baseAlpha = 1.0f;
                if (getFormat() == BitmapFormat::RGBA)
                {
                    baseAlpha = (baseByte[3] / 255.0f);
                }
                float alpha = layerAlpha + baseAlpha * (1 - layerAlpha);

                float red = ((float)layerByte[0]/255.0f * layerAlpha * blendColor.red) + ((float)baseByte[0]/255.0f * baseAlpha * (1 - layerAlpha)) / alpha;
                float green = ((float)layerByte[1]/255.0f * layerAlpha * blendColor.green) + ((float)baseByte[1]/255.0f * baseAlpha * (1 - layerAlpha)) / alpha;
                float blue = ((float)layerByte[2]/255.0f * layerAlpha * blendColor.blue) + ((float)baseByte[2]/255.0f * baseAlpha * (1 - layerAlpha)) / alpha;

                baseByte[0] = (U8)mClamp(red * 255.0f, 0.0f, 255.0f);
                baseByte[1] = (U8)mClamp(green * 255.0f, 0.0f, 255.0f);
                baseByte[2] = (U8)mClamp(blue * 255.0f, 0.0f, 255.0f);

                if (getFormat() == BitmapFormat::RGBA)
                {
                    baseByte[3] = (U8)mClamp(alpha * 255.0f, 0.0f, 255.0f);
                }
            }
        }
        return true;
    }
    return false;
}


//--------------------------------------------------------------------------
void GBitmap::setPalette(GPalette* in_pPalette)
{
   SAFE_DELETE(pPalette);
   pPalette = in_pPalette;
}

void GBitmap::copyRect(const GBitmap *src, const RectI &srcRect, const Point2I &dstPt)
{
   if(src->getFormat() != getFormat())
      return;
   if(srcRect.extent.x + srcRect.point.x > (S32)src->getWidth() || srcRect.extent.y + srcRect.point.y > (S32)src->getHeight())
      return;
   if(srcRect.extent.x + dstPt.x > (S32)getWidth() || srcRect.extent.y + dstPt.y > (S32)getHeight())
      return;

   for(U32 i = 0; i < (U32)srcRect.extent.y; i++)
   {
      dMemcpy(getAddress(dstPt.x, dstPt.y + i),
              src->getAddress(srcRect.point.x, srcRect.point.y + i),
              bytesPerPixel * srcRect.extent.x);
   }
}

//--------------------------------------------------------------------------
void GBitmap::allocateBitmap(const U32 in_width, const U32 in_height, const bool in_extrudeMipLevels, const BitmapFormat in_format)
{
   //-------------------------------------- Some debug checks...
   U32 svByteSize = byteSize;
   U8 *svBits = pBits;

   AssertFatal(in_width != 0 && in_height != 0, "GBitmap::allocateBitmap: width or height is 0");

   if (in_extrudeMipLevels == true) {
      //AssertFatal(in_width <= 256 && in_height <= 256, "GBitmap::allocateBitmap: width or height is too large");
      AssertFatal(isPow2(in_width) == true && isPow2(in_height) == true, "GBitmap::GBitmap: in order to extrude miplevels, bitmap w/h must be pow2");
   }

   internalFormat = in_format;
   width          = in_width;
   height         = in_height;

   bytesPerPixel = 1;
   switch (internalFormat) {
     case Alpha:
     case Palettized:
     case Luminance:
     case Intensity:  bytesPerPixel = 1;
      break;
     case RGB:        bytesPerPixel = 3;
      break;
     case RGBA:       bytesPerPixel = 4;
      break;
     case LuminanceAlpha:
     case RGB565:
     case RGB5551:    bytesPerPixel = 2;
      break;
#ifdef TORQUE_OS_IOS
        case PVR2:
        case PVR2A:
        case PVR4:
        case PVR4A:
            // compressed textures can't be allocated!
            return;

#endif //-Mat
     default:
      AssertFatal(false, "GBitmap::GBitmap: misunderstood format specifier");
      break;
   }

   // Set up the mip levels, if necessary...
   numMipLevels       = 1;
   U32 allocPixels = in_width * in_height * bytesPerPixel;
   mipLevelOffsets[0] = 0;


   if (in_extrudeMipLevels == true) 
   {
      U32 currWidth  = in_width;
      U32 currHeight = in_height;

      do 
      {
         mipLevelOffsets[numMipLevels] = mipLevelOffsets[numMipLevels - 1] +
                                         (currWidth * currHeight * bytesPerPixel);
         currWidth  >>= 1;
         currHeight >>= 1;
         if (currWidth  == 0) currWidth  = 1;
         if (currHeight == 0) currHeight = 1;

         numMipLevels++;
         allocPixels += currWidth * currHeight * bytesPerPixel;
      } while (currWidth != 1 || currHeight != 1);
   }
   AssertFatal(numMipLevels <= c_maxMipLevels, "GBitmap::allocateBitmap: too many miplevels");

   // Set up the memory...
   byteSize = allocPixels;
   pBits    = new U8[byteSize];
    dMemset(pBits, 0xFF, byteSize);
    
   if(svBits != NULL)
   {
      dMemcpy(pBits, svBits, getMin(byteSize, svByteSize));
      delete[] svBits;
   }
}


//--------------------------------------------------------------------------
void bitmapExtrude5551_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U16 *src = (const U16 *) srcMip;
   U16 *dst = (U16 *) mip;
   U32 stride = srcHeight != 1 ? srcWidth : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            U32 a = src[0];
            U32 b = src[1];
            U32 c = src[stride];
            U32 d = src[stride+1];
#if defined(TORQUE_BIG_ENDIAN)
            dst[x] = (((  (a >> 10) + (b >> 10) + (c >> 10) + (d >> 10)) >> 2) << 10) |
                     ((( ((a >> 5) & 0x1F) + ((b >> 5) & 0x1F) + ((c >> 5) & 0x1F) + ((d >> 5) & 0x1F)) >> 2) << 5) |
                     ((( ((a >> 0) & 0x1F) + ((b >> 0) & 0x1F) + ((c >> 0) & 0x1F) + ((d >> 0) & 0x1F)) >> 2) << 0);
#else
            dst[x] = (((  (a >> 11) + (b >> 11) + (c >> 11) + (d >> 11)) >> 2) << 11) |
                     ((( ((a >> 6) & 0x1F) + ((b >> 6) & 0x1F) + ((c >> 6) & 0x1F) + ((d >> 6) & 0x1F)) >> 2) << 6) |
                     ((( ((a >> 1) & 0x1F) + ((b >> 1) & 0x1F) + ((c >> 1) & 0x1F) + ((d >> 1) & 0x1F)) >> 2) << 1);
#endif
            src += 2;
         }
         src += stride;
         dst += width;
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         U32 a = src[0];
         U32 c = src[stride];
#if defined(TORQUE_BIG_ENDIAN)
            dst[y] = ((( (a >> 10) + (c >> 10)) >> 1) << 10) |
                     ((( ((a >> 5) & 0x1F) + ((c >> 5) & 0x1f)) >> 1) << 5) |
                     ((( ((a >> 0) & 0x1F) + ((c >> 0) & 0x1f)) >> 1) << 0);
#else
            dst[y] = ((( (a >> 11) + (c >> 11)) >> 1) << 11) |
                     ((( ((a >> 6) & 0x1f) + ((c >> 6) & 0x1f)) >> 1) << 6) |
                     ((( ((a >> 1) & 0x1F) + ((c >> 1) & 0x1f)) >> 1) << 1);
#endif
         src += 1 + stride;
      }
   }
}


//--------------------------------------------------------------------------
void bitmapExtrudeRGB_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U8 *src = (const U8 *) srcMip;
   U8 *dst = (U8 *) mip;
   U32 stride = srcHeight != 1 ? (srcWidth) * 3 : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[3]) + U32(src[stride]) + U32(src[stride+3]) + 2) >> 2;
            src += 4;
         }
         src += stride;   // skip
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src += 4;

         src += stride;   // skip
      }
   }
}

//--------------------------------------------------------------------------
void bitmapExtrudePaletted_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   dMemset(mip, 0, width * height);
}

//--------------------------------------------------------------------------
void bitmapExtrudeRGBA_c(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth)
{
   const U8 *src = (const U8 *) srcMip;
   U8 *dst = (U8 *) mip;
   U32 stride = srcHeight != 1 ? (srcWidth) * 4 : 0;

   U32 width  = srcWidth  >> 1;
   U32 height = srcHeight >> 1;
   if (width  == 0) width  = 1;
   if (height == 0) height = 1;

   if (srcWidth != 1)
   {
      for(U32 y = 0; y < height; y++)
      {
         for(U32 x = 0; x < width; x++)
         {
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src++;
            *dst++ = (U32(*src) + U32(src[4]) + U32(src[stride]) + U32(src[stride+4]) + 2) >> 2;
            src += 5;
         }
         src += stride;   // skip
      }
   }
   else
   {
      for(U32 y = 0; y < height; y++)
      {
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src++;
         *dst++ = (U32(*src) + U32(src[stride]) + 1) >> 1;
         src += 5;

         src += stride;   // skip
      }
   }
}

void (*bitmapExtrude5551)(const void *srcMip, void *mip, U32 height, U32 width) = bitmapExtrude5551_c;
void (*bitmapExtrudeRGB)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudeRGB_c;
void (*bitmapExtrudeRGBA)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudeRGBA_c;
void (*bitmapExtrudePaletted)(const void *srcMip, void *mip, U32 srcHeight, U32 srcWidth) = bitmapExtrudePaletted_c;


//--------------------------------------------------------------------------
void GBitmap::extrudeMipLevels(bool clearBorders)
{
   if(numMipLevels == 1)
      allocateBitmap(getWidth(), getHeight(), true, getFormat());

   switch (getFormat())
   {
      case RGB5551:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrude5551(getBits(i - 1), getWritableBits(i), getHeight(i), getWidth(i));
         break;
      }

      case RGB:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudeRGB(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }

      case RGBA:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudeRGBA(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }

      case Palettized:
      {
         for(U32 i = 1; i < numMipLevels; i++)
            bitmapExtrudePaletted(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
         break;
      }
      
      case Intensity:
      case Luminance:
      case LuminanceAlpha:
      case Alpha:
      case RGB565:
#ifdef TORQUE_OS_IOS
      case PVR2:
      case PVR2A:
      case PVR4:
      case PVR4A:
#endif
          break;

   }
   if (clearBorders)
   {
      for (U32 i = 1; i<numMipLevels; i++)
      {
         U32 width = getWidth(i);
         U32 height = getHeight(i);
         if (height<3 || width<3)
            // bmp is all borders at this mip level
            dMemset(getWritableBits(i),0,width*height*bytesPerPixel);
         else
         {
            width *= bytesPerPixel;
            U8 * bytes = getWritableBits(i);
            U8 * end = bytes + (height-1)*width - bytesPerPixel; // end = last row, 2nd column
            // clear first row sans the last pixel
            dMemset(bytes,0,width-bytesPerPixel);
            bytes -= bytesPerPixel;
            while (bytes<end)
            {
               // clear last pixel of row N-1 and first pixel of row N
               bytes += width;
               dMemset(bytes,0,bytesPerPixel*2);
            }
            // clear last row sans the first pixel
            dMemset(bytes+2*bytesPerPixel,0,width-bytesPerPixel);
         }
      }
   }
}

//--------------------------------------------------------------------------
void GBitmap::extrudeMipLevelsDetail()
{
   AssertFatal(getFormat() == GBitmap::RGB, "Error, only handles RGB for now...");
   U32 i,j;

   if(numMipLevels == 1)
      allocateBitmap(getWidth(), getHeight(), true, getFormat());

   for (i = 1; i < numMipLevels; i++) {
      bitmapExtrudeRGB(getBits(i - 1), getWritableBits(i), getHeight(i-1), getWidth(i-1));
   }

   // Ok, now that we have the levels extruded, we need to move the lower miplevels
   //  closer to 0.5.
   for (i = 1; i < numMipLevels - 1; i++) {
      U8* pMipBits = (U8*)getWritableBits(i);
      U32 numBytes = getWidth(i) * getHeight(i) * 3;

      U32 shift    = i;
      U32 start    = ((1 << i) - 1) * 0x80;

      for (j = 0; j < numBytes; j++) {
         U32 newVal = (start + pMipBits[j]) >> shift;
         AssertFatal(newVal <= 255, "Error, oob");
         pMipBits[j] = U8(newVal);
      }
   }
   AssertFatal(getWidth(numMipLevels - 1) == 1 && getHeight(numMipLevels - 1) == 1,
               "Error, last miplevel should be 1x1!");
   ((U8*)getWritableBits(numMipLevels - 1))[0] = 0x80;
   ((U8*)getWritableBits(numMipLevels - 1))[1] = 0x80;
   ((U8*)getWritableBits(numMipLevels - 1))[2] = 0x80;
}

//--------------------------------------------------------------------------
void bitmapConvertRGB_to_5551_c(U8 *src, U32 pixels)
{
   U16 *dst = (U16 *)src;
   for(U32 j = 0; j < pixels; j++)
   {
      U32 r = src[0] >> 3;
      U32 g = src[1] >> 3;
      U32 b = src[2] >> 3;

#if defined(TORQUE_BIG_ENDIAN)
      *dst++ = (1 << 15) | (b << 10) | (g << 5) | (r << 0);
#else
      *dst++ = (b << 1) | (g << 6) | (r << 11) | 1;
#endif
      src += 3;
   }
}



void (*bitmapConvertRGB_to_5551)(U8 *src, U32 pixels) = bitmapConvertRGB_to_5551_c;


//--------------------------------------------------------------------------
bool GBitmap::setFormat(BitmapFormat fmt)
{
   if (getFormat() == fmt)
      return true;

   // this is a nasty pointer math hack
   // is there a quick way to calc pixels of a fully mipped bitmap?
   U32 pixels = 0;
   for (U32 i=0; i < numMipLevels; i++)
      pixels += getHeight(i) * getWidth(i);

   switch (getFormat())
   {
      case RGB:
         if ( fmt == RGB5551 )
         {
            bitmapConvertRGB_to_5551(pBits, pixels);
            internalFormat = RGB5551;
            bytesPerPixel  = 2;
         }
         break;

      default:
         AssertWarn(0, "GBitmap::setFormat: unable to convert bitmap to requested format.");
         return false;
   }

   U32 offset = 0;
   for (U32 j=0; j < numMipLevels; j++)
   {
      mipLevelOffsets[j] = offset;
      offset += getHeight(j) * getWidth(j) * bytesPerPixel;
   }

   return true;
}


//--------------------------------------------------------------------------
bool GBitmap::getColorBGRA(const U32 x, const U32 y, ColorI& rColor) const
{
   if(!getColor(x, y, rColor))
      return false;
   //jk - swap red and blue...
   U8 r = rColor.red;
   rColor.red = rColor.blue;
   rColor.blue = r;
   return true;
}

bool GBitmap::setColorBGRA(const U32 x, const U32 y, ColorI& rColor)
{
   //jk - copy then swap red and blue...
   //jk - using a copy so the color object provided by the caller isn't swapped...
   ColorI temp = rColor;

   U8 r = temp.red;
   temp.red = temp.blue;
   temp.blue = r;

   return setColor(x, y, temp);
}
bool GBitmap::getColor(const U32 x, const U32 y, ColorI& rColor) const
{
   if (x >= width || y >= height)
      return false;
   if (internalFormat == Palettized && pPalette == NULL)
      return false;

   const U8* pLoc = getAddress(x, y);

   switch (internalFormat) {
     case Palettized:
      rColor = pPalette->getColor(*pLoc);
      break;

     case Alpha:
     case Intensity:
     case Luminance:
      rColor.set( *pLoc, *pLoc, *pLoc, *pLoc );
      break;

     case RGB:
      rColor.set( pLoc[0], pLoc[1], pLoc[2], 255 );
      break;

     case RGBA:
      rColor.set( pLoc[0], pLoc[1], pLoc[2], pLoc[3] );
      break;
         
     case LuminanceAlpha:
      rColor.set( pLoc[0], pLoc[0], pLoc[0], pLoc[1] );
      break;
         

     case RGB5551:
#if defined(TORQUE_BIG_ENDIAN)
      rColor.red   = (*((U16*)pLoc) >> 0) & 0x1F;
      rColor.green = (*((U16*)pLoc) >> 5) & 0x1F;
      rColor.blue  = (*((U16*)pLoc) >> 10) & 0x1F;
      rColor.alpha = ((*((U16*)pLoc) >> 15) & 0x01) ? 255 : 0;
#else
      rColor.red   =  *((U16*)pLoc) >> 11;
      rColor.green = (*((U16*)pLoc) >> 6) & 0x1f;
      rColor.blue  = (*((U16*)pLoc) >> 1) & 0x1f;
      rColor.alpha = (*((U16*)pLoc) & 1) ? 255 : 0;
#endif
      break;

     default:
      AssertFatal(false, "Bad internal format");
      return false;
   }

   return true;
}


//--------------------------------------------------------------------------
bool GBitmap::setColor(const U32 x, const U32 y, ColorI& rColor)
{
   if (x >= width || y >= height)
      return false;
   if (internalFormat == Palettized && pPalette == NULL)
      return false;

   U8* pLoc = getAddress(x, y);

   switch (internalFormat) {
     case Palettized:
      rColor = pPalette->getColor(*pLoc);
      break;

     case Alpha:
     case Intensity:
     case Luminance:
      *pLoc = rColor.alpha;
      break;
         
     case LuminanceAlpha:
      *pLoc = rColor.red;
      *(pLoc+1) = rColor.alpha;
      break;
         

     case RGB:
      dMemcpy( pLoc, &rColor, 3 * sizeof( U8 ) );
      break;

     case RGBA:
      dMemcpy( pLoc, &rColor, 4 * sizeof( U8 ) );
      break;

     case RGB5551:
#if defined(TORQUE_BIG_ENDIAN)
      *((U16*)pLoc) = (((rColor.alpha>0) ? 1 : 0)<<15) | (rColor.blue << 10) | (rColor.green << 5) | (rColor.red << 0);
#else
      *((U16*)pLoc) = (rColor.blue << 1) | (rColor.green << 6) | (rColor.red << 11) | ((rColor.alpha>0) ? 1 : 0);
#endif
      break;

     default:
      AssertFatal(false, "Bad internal format");
      return false;
   }

   return true;
}

//-----------------------------------------------------------------------------

GBitmap* GBitmap::createPowerOfTwoBitmap()
{
   if (isPow2(getWidth()) && isPow2(getHeight()))
      return NULL;

   AssertFatal(getNumMipLevels() == 1,
      "Cannot have non-pow2 bitmap with miplevels");

   U32 width = getWidth();
   U32 height = getHeight();

   U32 newWidth  = getNextPow2(getWidth());
   U32 newHeight = getNextPow2(getHeight());

   GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, getFormat());

   for (U32 i = 0; i < height; i++) 
   {
      U8*       pDest = (U8*)pReturn->getAddress(0, i);
      const U8* pSrc  = (const U8*)getAddress(0, i);

      dMemcpy(pDest, pSrc, width * bytesPerPixel);

      pDest += width * bytesPerPixel;
      // set the src pixel to the last pixel in the row
      const U8 *pSrcPixel = pDest - bytesPerPixel; 

      for(U32 j = width; j < newWidth; j++)
         for(U32 k = 0; k < bytesPerPixel; k++)
            *pDest++ = pSrcPixel[k];
   }

   for(U32 i = height; i < newHeight; i++)
   {
      U8* pDest = (U8*)pReturn->getAddress(0, i);
      U8* pSrc = (U8*)pReturn->getAddress(0, height-1);
      dMemcpy(pDest, pSrc, newWidth * bytesPerPixel);
   }

   return pReturn;
}

//------------------------------------------------------------------------------
//-------------------------------------- Persistent I/O
//

#ifdef TORQUE_OS_IOS
#define EXT_ARRAY_SIZE 4
static const char* extArray[EXT_ARRAY_SIZE] = { "", ".pvr", ".jpg", ".png"};
#else
#define EXT_ARRAY_SIZE 3
static const char* extArray[EXT_ARRAY_SIZE] = { "", ".jpg", ".png"};
#endif

ResourceObject * GBitmap::findBmpResource(const char * path)
{
   char fileNameBuffer[512];
   dStrcpy( fileNameBuffer, path );

   // Try some different possible filenames.
   U32 len = dStrlen( fileNameBuffer );
   for( U32 i = 0; i < EXT_ARRAY_SIZE; i++ ) 
   {
      dStrcpy( fileNameBuffer + len, extArray[i] );
      ResourceObject * ret = ResourceManager->find( fileNameBuffer );
      if (ret)
         return ret;
   }
   return NULL;
}

GBitmap *GBitmap::load(const char *path)
{
   ResourceObject * ro = findBmpResource(path);
   if (ro)
   {
      GBitmap *bmp = (GBitmap*)ResourceManager->loadInstance(ro);
      return bmp;
   }

   // If unable to load texture in current directory
   // look in the parent directory.  But never look in the root.
   char fileNameBuffer[512];
   dStrcpy( fileNameBuffer, path );
   char *name = dStrrchr( fileNameBuffer, '/' );

   if( name ) 
   {
      *name++ = 0;
      char *parent = dStrrchr( fileNameBuffer, '/' );

      if( parent ) 
      {
         parent[1] = 0;
         dStrcat( fileNameBuffer, name );
         return load( fileNameBuffer );
      }
   }

   return NULL;
}

bool GBitmap::read(Stream& io_rStream)
{
   // Handle versioning
   U32 version;
   io_rStream.read(&version);
   AssertFatal(version == csFileVersion, "Bitmap::read: incorrect file version");

   //-------------------------------------- Read the object
   U32 fmt;
   io_rStream.read(&fmt);
   internalFormat = BitmapFormat(fmt);
   bytesPerPixel = 1;
   switch (internalFormat) {
     case Alpha:
     case Palettized:
     case Luminance:
     case Intensity:  bytesPerPixel = 1;
      break;
     case RGB:        bytesPerPixel = 3;
      break;
     case RGBA:       bytesPerPixel = 4;
      break;
     case RGB565:
     case RGB5551:    bytesPerPixel = 2;
      break;
     default:
      AssertFatal(false, "GBitmap::GBitmap: misunderstood format specifier");
      break;
   }

   io_rStream.read(&byteSize);

   pBits = new U8[byteSize];
   io_rStream.read(byteSize, pBits);

   io_rStream.read(&width);
   io_rStream.read(&height);

   io_rStream.read(&numMipLevels);
   for (U32 i = 0; i < c_maxMipLevels; i++)
      io_rStream.read(&mipLevelOffsets[i]);

   if (internalFormat == Palettized) {
      pPalette = new GPalette;
      pPalette->read(io_rStream);
   }

   return (io_rStream.getStatus() == Stream::Ok);
}

bool GBitmap::write(Stream& io_rStream) const
{
   // Handle versioning
   io_rStream.write(csFileVersion);

   //-------------------------------------- Write the object
   io_rStream.write(U32(internalFormat));

   io_rStream.write(byteSize);
   io_rStream.write(byteSize, pBits);

   io_rStream.write(width);
   io_rStream.write(height);

   io_rStream.write(numMipLevels);
   for (U32 i = 0; i < c_maxMipLevels; i++)
      io_rStream.write(mipLevelOffsets[i]);

   if (internalFormat == Palettized) {
      AssertFatal(pPalette != NULL,
                  "GBitmap::write: cannot write a palettized bitmap wo/ a palette");
      pPalette->write(io_rStream);
   }

    return true;
}


//-------------------------------------- GFXBitmap
ResourceInstance* constructBitmapJPEG(Stream &stream)
{
   GBitmap* bmp = new GBitmap;
   if (bmp->readJPEG(stream))
      return bmp;
    else
    {
      delete bmp;
      return NULL;
   }
}

ResourceInstance* constructBitmapPNG(Stream &stream)
{
   GBitmap* bmp = new GBitmap;

   //PUAP -Mat uless you compile with a custom build step 'IPHONE_OPTIMIZE_OPTIONS' set to '-skip-PNGs', you're PNGs will be altered(optimized)
    //So either deal with that youself or define this so that we load it using apple iPhone functions to get the PNG data to Torque	
#ifdef USE_APPLE_OPTIMIZED_PNGS
    if (bmp->readPNGiPhone(stream)){
#else
    if (bmp->readPNG(stream)){
#endif
      return bmp;
    }
    else
    {
      delete bmp;
      return NULL;
    }
}

ResourceInstance* constructBitmapBMP(Stream &stream)
{
   GBitmap *bmp = new GBitmap;
   if(bmp->readMSBmp(stream))
      return bmp;
   else
   {
      delete bmp;
      return NULL;
   }
}

#ifdef TORQUE_OS_IOS
ResourceInstance* constructBitmapPVR(Stream &stream)
{
    GBitmap *bmp = new GBitmap;
    if(bmp->readPvr(stream))
        return bmp;
    else
    {
        delete bmp;
        return NULL;
    }
}
#endif



