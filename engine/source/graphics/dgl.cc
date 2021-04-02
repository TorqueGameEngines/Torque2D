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

#include "math/mPoint.h"
#include "graphics/TextureManager.h"
#include "graphics/dgl.h"
#include "graphics/gColor.h"
#include "math/mPoint.h"
#include "math/mRect.h"
#include "graphics/gFont.h"
#include "console/console.h"
#include "math/mMatrix.h"
#include "memory/frameAllocator.h"
#include "debug/profiler.h"
#include "string/unicode.h"

#include "dglMac_ScriptBinding.h"
#include "dgl_ScriptBinding.h"

#include <vector>

namespace {

ColorI sg_bitmapModulation(255, 255, 255, 255);
ColorI sg_textAnchorColor(255, 255, 255, 255);
ColorI sg_stackColor(255, 255, 255, 255);
RectI sgCurrentClipRect;

} // namespace {}


//--------------------------------------------------------------------------
void dglSetBitmapModulation(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_bitmapModulation = c;
   sg_textAnchorColor = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorF* color)
{
   *color = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorI* color)
{
   *color = sg_bitmapModulation;
}

void dglClearBitmapModulation()
{
   sg_bitmapModulation.set(255, 255, 255, 255);
}

void dglSetTextAnchorColor(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_textAnchorColor = c;
}


//--------------------------------------------------------------------------
void dglDrawBitmapStretchSR(TextureObject* texture,
                       const RectI&   dstRect,
                       const RectI&   srcRect,
                       const U32   in_flip,
                       F32			 fSpin,
                       bool				bSilhouette)
{	
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretchSR: NULL Handle");
   if(!dstRect.isValidRect())
      return;
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapStretchSR: routines assume normal rects");

   glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture->getGLTextureName());
   //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   if (bSilhouette)
   {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
   
      ColorF kModulationColor;
      dglGetBitmapModulation(&kModulationColor);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, kModulationColor.address());
   }
   else
   {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   }
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   F32 texLeft   = F32(srcRect.point.x)                    / F32(texture->getTextureWidth());
   F32 texRight  = F32(srcRect.point.x + srcRect.extent.x) / F32(texture->getTextureWidth());
   F32 texTop    = F32(srcRect.point.y)                    / F32(texture->getTextureHeight());
   F32 texBottom = F32(srcRect.point.y + srcRect.extent.y) / F32(texture->getTextureHeight());
     Point2F scrPoints[4];
   if(fSpin == 0.0f)
   {
            scrPoints[0].x = (F32)dstRect.point.x;
            scrPoints[0].y = (F32)dstRect.point.y;
            scrPoints[1].x = (F32)(dstRect.point.x + dstRect.extent.x);
            scrPoints[1].y = (F32)dstRect.point.y;
            scrPoints[2].x = (F32)dstRect.point.x;
            scrPoints[2].y = (F32)(dstRect.point.y + dstRect.extent.y);
            scrPoints[3].x = (F32)(dstRect.point.x + dstRect.extent.x);
            scrPoints[3].y = (F32)(dstRect.point.y + dstRect.extent.y);
            //screenLeft   = dstRect.point.x;
            //screenRight  = dstRect.point.x + dstRect.extent.x;
            //screenTop    = dstRect.point.y;
            //screenBottom = dstRect.point.y + dstRect.extent.y;
        }
        else
   {
     //WE NEED TO IMPLEMENT A FAST 2D ROTATION -- NOT THIS SLOWER 3D ROTATION
     MatrixF rotMatrix( EulerF( 0.0, 0.0, mDegToRad(fSpin) ) );

     Point3F offset( dstRect.point.x + dstRect.extent.x / 2.0f,
                                         dstRect.point.y + dstRect.extent.y / 2.0f, 0.0 );
         Point3F points[4];
   
     points[0] = Point3F(-dstRect.extent.x / 2.0f,  dstRect.extent.y / 2.0f, 0.0);
     points[1] = Point3F( dstRect.extent.x / 2.0f,  dstRect.extent.y / 2.0f, 0.0);
     points[2] = Point3F(-dstRect.extent.x / 2.0f, -dstRect.extent.y / 2.0f, 0.0);
     points[3] = Point3F( dstRect.extent.x / 2.0f, -dstRect.extent.y / 2.0f, 0.0);

     for( int i=0; i<4; i++ )
     {
        rotMatrix.mulP( points[i] );
        points[i] += offset;
        scrPoints[i].x = points[i].x;
        scrPoints[i].y = points[i].y;
     }
   }
  

   if(in_flip & GFlip_X)
   {
      F32 temp = texLeft;
      texLeft = texRight;
      texRight = temp;
   }
   if(in_flip & GFlip_Y)
   {
      F32 temp = texTop;
      texTop = texBottom;
      texBottom = temp;
   }

   glColor4ub(sg_bitmapModulation.red,
             sg_bitmapModulation.green,
             sg_bitmapModulation.blue,
             sg_bitmapModulation.alpha);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)

    GLfloat verts[] = {
        (GLfloat)scrPoints[0].x, (GLfloat)scrPoints[0].y,
        (GLfloat)scrPoints[1].x, (GLfloat)scrPoints[1].y,
        (GLfloat)scrPoints[2].x, (GLfloat)scrPoints[2].y,
        (GLfloat)scrPoints[3].x, (GLfloat)scrPoints[3].y,
    };
    GLfloat texVerts[] = {
        (GLfloat)texLeft, (GLfloat)texTop,
        (GLfloat)texRight, (GLfloat)texTop,		
        (GLfloat)texLeft, (GLfloat)texBottom,
        (GLfloat)texRight, (GLfloat)texBottom,
    };
    
    
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#else
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(texLeft, texBottom);
      glVertex2f(scrPoints[2].x, scrPoints[2].y);

      glTexCoord2f(texRight, texBottom);
      glVertex2f(scrPoints[3].x, scrPoints[3].y);

      glTexCoord2f(texRight, texTop);
      glVertex2f(scrPoints[1].x, scrPoints[1].y);

      glTexCoord2f(texLeft, texTop);
      glVertex2f(scrPoints[0].x, scrPoints[0].y);
   glEnd();
#endif
   if (bSilhouette)
   {
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, ColorF(0.0f, 0.0f, 0.0f, 0.0f).address());
   }

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
}

void dglDrawBitmap(TextureObject* texture, const Point2I& in_rAt, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmap: NULL Handle");

   // All non-StretchSR bitmaps are transformed into StretchSR calls...
   //
   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   RectI stretch(in_rAt.x, in_rAt.y,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          subRegion,
                          in_flip);
}

void dglDrawBitmapTile(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapTile: NULL Handle");
   // since the texture coords are calculated from the texture sub-rect we pass in,
   // but no actual indexing of that sub-rect happens, we can pass any texture coords
   // that we like to dglDrawBitmapSR(). We use this to force tiling.
   RectI subregion(0,0, dstRect.extent.x, dstRect.extent.y);
   dglDrawBitmapStretchSR(texture, dstRect, subregion, in_flip, fSpin, bSilhouette);
}

void dglDrawBitmapStretch(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretch: NULL Handle");
   AssertFatal(dstRect.isValidRect() == true,
               "GSurface::drawBitmapStretch: routines assume normal rects");

   RectI subRegion(0, 0,
                   texture->getBitmapWidth(),
                   texture->getBitmapHeight());
   dglDrawBitmapStretchSR(texture,
                          dstRect,
                          subRegion,
                          in_flip,
                          fSpin,
                          bSilhouette);
}

void dglDrawBitmapSR(TextureObject *texture, const Point2I& in_rAt, const RectI& srcRect, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapSR: NULL Handle");
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapSR: routines assume normal rects");

   RectI stretch(in_rAt.x, in_rAt.y,
                 srcRect.len_x(),
                 srcRect.len_y());
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          srcRect,
                          in_flip);
}

U32 dglDrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF16*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   return dglDrawTextN(font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot);
}

U32 dglDrawText(GFont*   font,
          const Point2I& ptDraw,
          const UTF8*    in_string,
          const ColorI*  colorTable,
          const U32      maxColorIndex,
          F32            rot)
{
   // Just a note - dStrlen(utf8) isn't strictly correct but it's guaranteed to be
   // as long or longer than the real length. dglDrawTextN fails gracefully
   // if you specify overlong, so this is ok.
   return dglDrawTextN(font, ptDraw, in_string, dStrlen((const UTF8 *) in_string), colorTable, maxColorIndex, rot);
}

struct TextVertex
{
   Point2F p;
   Point2F t;
   ColorI c;
    TextVertex() { set( 0.0f, 0.0f, 0.0f, 0.0f, ColorI(0, 0, 0) ); }
   void set(F32 x, F32 y, F32 tx, F32 ty, ColorI color)
   {
      p.x = x;
      p.y = y;
      t.x = tx;
      t.y = ty;
      c = color;
   }
};

//------------------------------------------------------------------------------

U32 dglDrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF8*     in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   PROFILE_START(DrawText_UTF8);
   
   U32 len = dStrlen(in_string) + 1;
   FrameTemp<UTF16> ubuf(len);
   convertUTF8toUTF16(in_string, ubuf, len);
   U32 tmp = dglDrawTextN(font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot);

   PROFILE_END();

   return tmp;
}

//-----------------------------------------------------------------------------

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
U32 dglDrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF16*    in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)

{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;


   MatrixF rotMatrix( EulerF( 0.0, 0.0, mDegToRad( rot ) ) );
   Point3F offset( ptDraw.x, ptDraw.y, 0.0 );
   Point3F points[4];

   U32 nCharCount = 0;

   Point2I     pt;
   UTF16       c;
   pt.x                 = 0;

   ColorI                  currentColor;
   S32                     currentPt = 0;

   TextureObject *lastTexture = NULL;

   currentColor      = sg_bitmapModulation;

   FrameTemp<TextVertex> vert(4*n);

   glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   //Luma: Optimise by setting states once before inner loop
   glEnableClientState ( GL_VERTEX_ARRAY );
   glEnableClientState ( GL_COLOR_ARRAY );
   glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
   glVertexPointer     ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].p) );
   glColorPointer      ( 4, GL_UNSIGNED_BYTE, sizeof(TextVertex), &(vert[0].c) );
   glTexCoordPointer   ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].t) );

   // first build the point, color, and coord arrays
   U32 i;

   for(i = 0,c = in_string[i];in_string[i] && i < n;i++,c = in_string[i])
   {
      nCharCount++;
      if(nCharCount > n)
          break;

      // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
      if ((c >=  1 && c <=  7) ||
         (c >= 11 && c <= 12) ||
         (c == 14))
      {
         // Color code
         if (colorTable)
         {
            static U8 remap[15] =
            {
               0x0, // 0 special null terminator
               0x0, // 1 ascii start-of-heading??
               0x1, 
               0x2, 
               0x3, 
               0x4, 
               0x5, 
               0x6, 
               0x0, // 8 special backspace
               0x0, // 9 special tab
               0x0, // a special \n
               0x7, 
               0x8,
               0x0, // a special \r
               0x9 
            };

            U8 remapped = remap[c];
            // Ignore if the color is greater than the specified max index:
            if ( remapped <= maxColorIndex )
            {
               const ColorI &clr = colorTable[remapped];
               sg_bitmapModulation = clr;
               currentColor = clr;
            }
         }
         continue;
      }

      // reset color?
      if ( c == 15 )
      {
         currentColor = sg_textAnchorColor;
         sg_bitmapModulation = sg_textAnchorColor;
         continue;
      }

      // push color:
      if ( c == 16 )
      {
         sg_stackColor = sg_bitmapModulation;
         continue;
      }

      // pop color:
      if ( c == 17 )
      {
         currentColor = sg_stackColor;
         sg_bitmapModulation = sg_stackColor;
         continue;
      }

      // Tab character
      if ( c == dT('\t') ) 
      {
          const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
          pt.x += ci.xIncrement * GFont::TabWidthInSpaces;
          continue;
      }

      if( !font->isValidChar( c ) )  
         continue;

      const PlatformFont::CharInfo &ci = font->getCharInfo(c);

      if(ci.bitmapIndex == -1)
      {
         pt.x += ci.xOrigin + ci.xIncrement;
         continue;
      }

      TextureObject *newObj = font->getTextureHandle(ci.bitmapIndex);
      if(newObj != lastTexture)
      {
         if(currentPt)
         {
            glBindTexture(GL_TEXTURE_2D, lastTexture->getGLTextureName());

            //Luma:	More optimal rendering
            for (S32 i=0; i<currentPt; i+=4) 
            {
                glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
            }
            currentPt = 0;
         }
         lastTexture = newObj;
      }
      if(ci.width != 0 && ci.height != 0)
      {
         pt.y = font->getBaseline() - ci.yOrigin;
         pt.x += ci.xOrigin;

         F32 texLeft   = F32(ci.xOffset)             / F32(lastTexture->getTextureWidth());
         F32 texRight  = F32(ci.xOffset + ci.width)  / F32(lastTexture->getTextureWidth());
         F32 texTop    = F32(ci.yOffset)             / F32(lastTexture->getTextureHeight());
         F32 texBottom = F32(ci.yOffset + ci.height) / F32(lastTexture->getTextureHeight());

         F32 screenLeft   = pt.x;
         F32 screenRight  = pt.x + ci.width;
         F32 screenTop    = pt.y;
         F32 screenBottom = pt.y + ci.height;

         points[0] = Point3F(screenLeft, screenTop, 0.0);
         points[1] = Point3F(screenRight,  screenTop, 0.0);
         points[2] = Point3F( screenLeft,  screenBottom, 0.0);
         points[3] = Point3F( screenRight, screenBottom, 0.0);

         for( int i=0; i<4; i++ )
         {
            rotMatrix.mulP( points[i] );
            points[i] += offset;
         }
         vert[currentPt++].set(points[0].x, points[0].y, texLeft, texTop, currentColor);
         vert[currentPt++].set(points[1].x, points[1].y, texRight, texTop, currentColor);
         vert[currentPt++].set(points[2].x, points[2].y, texLeft, texBottom, currentColor);
         vert[currentPt++].set(points[3].x, points[3].y, texRight, texBottom, currentColor);
         pt.x += ci.xIncrement - ci.xOrigin;
      }
      else
         pt.x += ci.xIncrement;
   }
   if(currentPt)
   {
       //Luma:	More optimal rendering
       glBindTexture(GL_TEXTURE_2D, lastTexture->getGLTextureName());
       for (S32 i=0; i<currentPt; i+=4) 
       {
            glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
       }
   }

   glDisableClientState ( GL_VERTEX_ARRAY );
   glDisableClientState ( GL_COLOR_ARRAY );
   glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);

   pt.x += ptDraw.x; // DAW: Account for the fact that we removed the drawing point from the text start at the beginning.

   AssertFatal(pt.x >= ptDraw.x, "How did this happen?");
   PROFILE_END();

   return pt.x - ptDraw.x;
}

#else

U32 dglDrawTextN(GFont*          font,
                 const Point2I&  ptDraw,
                 const UTF16*    in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;
   PROFILE_START(DrawText);

   MatrixF rotMatrix( EulerF( 0.0, 0.0, mDegToRad( rot ) ) );
   Point3F offset( (F32)ptDraw.x, (F32)ptDraw.y, 0.0f );
   Point3F points[4];

   U32 nCharCount = 0;

   Point2I     pt;
   UTF16       c;
   pt.x                 = 0;

   ColorI                  currentColor;
   S32                     currentPt = 0;

   TextureObject *lastTexture = NULL;

   currentColor      = sg_bitmapModulation;

   FrameTemp<TextVertex> vert(4*n);

   glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   glEnableClientState ( GL_VERTEX_ARRAY );
   glVertexPointer     ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].p) );

   glEnableClientState ( GL_COLOR_ARRAY );
   glColorPointer      ( 4, GL_UNSIGNED_BYTE, sizeof(TextVertex), &(vert[0].c) );

   glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
   glTexCoordPointer   ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].t) );

   // first build the point, color, and coord arrays
   U32 i;

   for(i = 0,c = in_string[i];in_string[i] && i < n;i++,c = in_string[i])
   {
      nCharCount++;
      if(nCharCount > n)
          break;

      // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
      if ((c >=  1 && c <=  7) ||
         (c >= 11 && c <= 12) ||
         (c == 14))
      {
         // Color code
         if (colorTable)
         {
            static U8 remap[15] =
            {
               0x0, // 0 special null terminator
               0x0, // 1 ascii start-of-heading??
               0x1, 
               0x2, 
               0x3, 
               0x4, 
               0x5, 
               0x6, 
               0x0, // 8 special backspace
               0x0, // 9 special tab
               0x0, // a special \n
               0x7, 
               0x8,
               0x0, // a special \r
               0x9 
            };

            U8 remapped = remap[c];
            // Ignore if the color is greater than the specified max index:
            if ( remapped <= maxColorIndex )
            {
               const ColorI &clr = colorTable[remapped];
               sg_bitmapModulation = clr;
               currentColor = clr;
            }
         }
         continue;
      }

      // reset color?
      if ( c == 15 )
      {
         currentColor = sg_textAnchorColor;
         sg_bitmapModulation = sg_textAnchorColor;
         continue;
      }

      // push color:
      if ( c == 16 )
      {
         sg_stackColor = sg_bitmapModulation;
         continue;
      }

      // pop color:
      if ( c == 17 )
      {
         currentColor = sg_stackColor;
         sg_bitmapModulation = sg_stackColor;
         continue;
      }

      // Tab character
      if ( c == dT('\t') ) 
      {
          const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
          pt.x += ci.xIncrement * GFont::TabWidthInSpaces;
          continue;
      }

      if( !font->isValidChar( c ) )  
         continue;

      const PlatformFont::CharInfo &ci = font->getCharInfo(c);

      if(ci.bitmapIndex == -1)
      {
         pt.x += ci.xOrigin + ci.xIncrement;
         continue;
      }

      TextureObject *newObj = font->getTextureHandle(ci.bitmapIndex);
      if(newObj != lastTexture)
      {
         if(currentPt)
         {
            glBindTexture(GL_TEXTURE_2D, lastTexture->getGLTextureName());
            glDrawArrays( GL_QUADS, 0, currentPt );
            currentPt = 0;
         }
         lastTexture = newObj;
      }
      if(ci.width != 0 && ci.height != 0)
      {
         pt.y = font->getBaseline() - ci.yOrigin;
         pt.x += ci.xOrigin;

         F32 texLeft   = F32(ci.xOffset)             / F32(lastTexture->getTextureWidth());
         F32 texRight  = F32(ci.xOffset + ci.width)  / F32(lastTexture->getTextureWidth());
         F32 texTop    = F32(ci.yOffset)             / F32(lastTexture->getTextureHeight());
         F32 texBottom = F32(ci.yOffset + ci.height) / F32(lastTexture->getTextureHeight());

         F32 screenLeft   = (F32)pt.x;
         F32 screenRight  = (F32)(pt.x + ci.width);
         F32 screenTop    = (F32)pt.y;
         F32 screenBottom = (F32)(pt.y + ci.height);

         points[0] = Point3F(screenLeft, screenBottom, 0.0);
         points[1] = Point3F(screenRight,  screenBottom, 0.0);
         points[2] = Point3F( screenRight,  screenTop, 0.0);
         points[3] = Point3F( screenLeft, screenTop, 0.0);

         for( int i=0; i<4; i++ )
         {
            rotMatrix.mulP( points[i] );
            points[i] += offset;
         }

         vert[currentPt++].set(points[0].x, points[0].y, texLeft, texBottom, currentColor);
         vert[currentPt++].set(points[1].x, points[1].y, texRight, texBottom, currentColor);
         vert[currentPt++].set(points[2].x, points[2].y, texRight, texTop, currentColor);
         vert[currentPt++].set(points[3].x, points[3].y, texLeft, texTop, currentColor);
         pt.x += ci.xIncrement - ci.xOrigin;
      }
      else
         pt.x += ci.xIncrement;
   }
   if(currentPt)
   {
      glBindTexture(GL_TEXTURE_2D, lastTexture->getGLTextureName());
      glDrawArrays( GL_QUADS, 0, currentPt );
   }

   glDisableClientState ( GL_VERTEX_ARRAY );
   glDisableClientState ( GL_COLOR_ARRAY );
   glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);

   pt.x += ptDraw.x; // DAW: Account for the fact that we removed the drawing point from the text start at the beginning.

   AssertFatal(pt.x >= ptDraw.x, "How did this happen?");
   PROFILE_END();

   return pt.x - ptDraw.x;
}
#endif

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Drawing primitives

void dglDrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        (GLfloat)(x1 + 0.5f), (GLfloat)(y1 + 0.5f),
        (GLfloat)(x2 + 0.5f), (GLfloat)(y2 + 0.5f),
    };
    
    glVertexPointer(2, GL_FLOAT, 0, verts );
    
    glDrawArrays(GL_LINES, 0, 2);//draw last two
#else
   glBegin(GL_LINES);
   glVertex2f((F32)x1 + 0.5f,  (F32)y1 + 0.5f);
   glVertex2f((F32)x2 + 0.5f,    (F32)y2 + 0.5f);
   glEnd();
    //glBegin(GL_POINTS);
    //glVertex2f((F32)x2 + 0.5, (F32)y2 + 0.5);
    //glEnd();
#endif
}

void dglDrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color)
{
    dglDrawLine(startPt.x, startPt.y, endPt.x, endPt.y, color);
}

void dglDrawTriangleFill(const Point2I &pt1, const Point2I &pt2, const Point2I &pt3, const ColorI &color)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	glColor4ub(color.red, color.green, color.blue, color.alpha);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
	GLfloat vertices[] = {
		(GLfloat)pt1.x, (GLfloat)pt1.y,
		(GLfloat)pt2.x, (GLfloat)pt2.y,
		(GLfloat)pt3.x, (GLfloat)pt3.y,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
#else
	glBegin(GL_TRIANGLES);
	glVertex2f((GLfloat)pt1.x, (GLfloat)pt1.y);
	glVertex2f((GLfloat)pt2.x, (GLfloat)pt2.y);
	glVertex2f((GLfloat)pt3.x, (GLfloat)pt3.y);
	glEnd();
#endif
}

void dglDrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color, const float &lineWidth)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glLineWidth(lineWidth);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        (GLfloat)(upperL.x), (GLfloat)(upperL.y),
        (GLfloat)(lowerR.x), (GLfloat)(upperL.y),
        (GLfloat)(lowerR.x), (GLfloat)(lowerR.y),
        (GLfloat)(upperL.x), (GLfloat)(lowerR.y),
    };
    
    glVertexPointer(2, GL_FLOAT, 0, verts );
    glDrawArrays(GL_LINE_LOOP, 0, 4 );//draw last two
#else
   glBegin(GL_LINE_LOOP);
      glVertex2f((F32)upperL.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)lowerR.y + 0.5f);
      glVertex2f((F32)upperL.x + 0.5f, (F32)lowerR.y + 0.5f);
   glEnd();
#endif
}

// the fill convention for lined rects is that they outline the rectangle border of the
// filled region specified.

void dglDrawRect(const RectI &rect, const ColorI &color, const float &lineWidth)
{
   Point2I lowerR(rect.point.x + rect.extent.x - 1, rect.point.y + rect.extent.y - 1);
   dglDrawRect(rect.point, lowerR, color, lineWidth);
}

// the fill convention says that pixel at upperL will be filled and
// that pixel at lowerR will NOT be filled.

void dglDrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat vertices[] = {
        (GLfloat)upperL.x, (GLfloat)upperL.y,
        (GLfloat)upperL.x, (GLfloat)lowerR.y,
        (GLfloat)lowerR.x, (GLfloat)upperL.y,
        (GLfloat)lowerR.x, (GLfloat)lowerR.y,
    };
    
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
   glRecti((S32)upperL.x, (S32)upperL.y, (S32)lowerR.x, (S32)lowerR.y);
#endif
}
void dglDrawRectFill(const RectI &rect, const ColorI &color)
{
   Point2I lowerR(rect.point.x + rect.extent.x, rect.point.y + rect.extent.y);
   dglDrawRectFill(rect.point, lowerR, color);
}

//Start in the top left and move counter-clockwise around the quad.
void dglDrawQuadFill(const Point2I &point1, const Point2I &point2, const Point2I &point3, const Point2I &point4, const ColorI &color)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	glColor4ub(color.red, color.green, color.blue, color.alpha);

	//Points 3 and 4 are switched by design.
	GLfloat vertices[] = {
		(GLfloat)point1.x, (GLfloat)point1.y,
		(GLfloat)point2.x, (GLfloat)point2.y,
		(GLfloat)point4.x, (GLfloat)point4.y,
		(GLfloat)point3.x, (GLfloat)point3.y,
	};

	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glEnableClientState(GL_VERTEX_ARRAY);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void dglDrawDot(const Point2F &screenPoint,const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glBegin(GL_POINTS);
   glColor4ub(color.red, color.green, color.blue, color.alpha);
   glVertex2f(screenPoint.x, screenPoint.y);
   glDisable(GL_BLEND);
   glEnd();
}

void dglDraw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle )
{
   width *= 0.5;

   MatrixF rotMatrix( EulerF( 0.0, 0.0, spinAngle ) );

   Point3F offset( screenPoint.x, screenPoint.y, 0.0 );
   Point3F points[4];

   points[0] = Point3F(-width, -width, 0.0);
   points[1] = Point3F(-width,  width, 0.0);
   points[2] = Point3F( width,  width, 0.0);
   points[3] = Point3F( width, -width, 0.0);

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      points[i] += offset;
   }

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,//may need to switch last two
        1.0, 1.0,
    };

    GLfloat texVerts[] = {
        (GLfloat)points[0].x, (GLfloat)points[0].y,
        (GLfloat)points[1].x, (GLfloat)points[1].y,
        (GLfloat)points[3].x, (GLfloat)points[3].y,
        (GLfloat)points[2].x, (GLfloat)points[2].y,
    };
    
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#else
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0, 0.0);
      glVertex2fv(points[0]);

      glTexCoord2f(0.0, 1.0);
      glVertex2fv(points[1]);

      glTexCoord2f(1.0, 1.0);
      glVertex2fv(points[2]);

      glTexCoord2f(1.0, 0.0);
      glVertex2fv(points[3]);
   glEnd();
#endif
}

void dglDrawBillboard( const Point3F &position, F32 width, F32 spinAngle )
{
   MatrixF modelview;
   dglGetModelview( &modelview );
   modelview.transpose();


   width *= 0.5;
   Point3F points[4];
   points[0] = Point3F(-width, 0.0, -width);
   points[1] = Point3F(-width, 0.0,  width);
   points[2] = Point3F( width, 0.0,  width);
   points[3] = Point3F( width, 0.0, -width);


   MatrixF rotMatrix( EulerF( 0.0, spinAngle, 0.0 ) );

   for( int i=0; i<4; i++ )
   {
      rotMatrix.mulP( points[i] );
      modelview.mulP( points[i] );
      points[i] += position;
   }


#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
    GLfloat verts[] = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 1.0,//may need to switch last two
        1.0, 0.0,
    };
    
    GLfloat texVerts[] = {
        (GLfloat)points[0].x, (GLfloat)points[0].y,
        (GLfloat)points[1].x, (GLfloat)points[1].y,
        (GLfloat)points[3].x, (GLfloat)points[3].y,
        (GLfloat)points[2].x, (GLfloat)points[2].y,
    };
    
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#else
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0, 1.0);
      glVertex3fv(points[0]);

      glTexCoord2f(0.0, 0.0);
      glVertex3fv(points[1]);

      glTexCoord2f(1.0, 0.0);
      glVertex3fv(points[2]);

      glTexCoord2f(1.0, 1.0);
      glVertex3fv(points[3]);
   glEnd();
#endif
}

void dglWireCube(const Point3F & extent, const Point3F & center)
{
   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };

   glDisable(GL_CULL_FACE);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
//PUAP -Mat untested
   for (S32 i = 0; i < 6; i++)
   {
       GLfloat verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] =	 cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       glVertexPointer(3, GL_FLOAT, 0, verts);
       glDrawArrays(GL_LINE_LOOP, 0, 4);
   }
#else
   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_LINE_LOOP);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
            cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
#endif
}


void dglSolidCube(const Point3F & extent, const Point3F & center)
{
   static Point3F cubePoints[8] =
   {
      Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
         Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
   };

   static U32 cubeFaces[6][4] =
   {
      { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
      { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
   };

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
//PUAP -Mat untested
   for (S32 i = 0; i < 6; i++)
   {
       GLfloat verts[] = {
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
           0, 0, 0,
       };
       int j = 0;
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         //glVertex3f(cubePoints[idx].x * extent.x + center.x,
            //cubePoints[idx].y * extent.y + center.y,
            //cubePoints[idx].z * extent.z + center.z);
          verts[j] = cubePoints[idx].x * extent.x + center.x;
          verts[++j] = cubePoints[idx].y * extent.y + center.y;
          verts[++j] = cubePoints[idx].z * extent.z + center.z;
      }
       glVertexPointer(3, GL_FLOAT, 0, verts);
       glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
   }
#else
   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_TRIANGLE_FAN);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
            cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
#endif
}

//Draws an unfilled circle with line segments.
//Circle drawing code was modified from this source with gratitude. It is in the public domain.
//http://slabode.exofire.net/circle_draw.shtml
void dglDrawCircle(const Point2I &center, const F32 radius, const ColorI &color, const F32 &lineWidth)
{
	F32 adjustedRadius = radius - (lineWidth/2);
	const S32 num_segments = (const S32)round(10 * sqrtf(adjustedRadius));
	F32 theta = 2 * 3.1415926f / F32(num_segments);
	F32 c = cosf(theta);//precalculate the sine and cosine
	F32 s = sinf(theta);
	F32 t;

	F32 x = adjustedRadius;//we start at angle = 0 
	F32 y = 0;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	glLineWidth(lineWidth);

	glColor4ub(color.red, color.green, color.blue, color.alpha);

	vector<GLfloat> verts;
	for (int ii = 0; ii < num_segments; ii++)
	{
		verts.push_back(GLfloat(x + center.x));
		verts.push_back(GLfloat(y + center.y));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	verts.push_back(GLfloat(verts[0]));
	verts.push_back(GLfloat(verts[1]));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts.data());
	glDrawArrays(GL_LINE_LOOP, 0, num_segments + 1);//draw last two
	glDisableClientState(GL_VERTEX_ARRAY);
}

void dglDrawCircleFill(const Point2I &center, const F32 radius, const ColorI &color)
{
	const S32 num_segments = (const S32)round(10 * sqrtf(radius));
	F32 theta = 2 * 3.1415926f / F32(num_segments);
	F32 c = cosf(theta);//precalculate the sine and cosine
	F32 s = sinf(theta);
	F32 t;

	F32 x = radius;//we start at angle = 0 
	F32 y = 0;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	glColor4ub(color.red, color.green, color.blue, color.alpha);

	vector<GLfloat> verts;
	verts.push_back(GLfloat(center.x));
	verts.push_back(GLfloat(center.y));
	for (int ii = 0; ii < num_segments; ii++)
	{
		verts.push_back(GLfloat(x + center.x));
		verts.push_back(GLfloat(y + center.y));

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	verts.push_back(GLfloat(verts[2]));
	verts.push_back(GLfloat(verts[3]));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts.data());
	glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments+2);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void dglSetClipRect(const RectI &clipRect)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   U32 screenHeight = Platform::getWindowSize().y;

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID)
   glOrthof(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0,
           0, 1);
#else
   glOrtho(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0,
           0, 1);
#endif

   glTranslatef(0.0f, (F32)-clipRect.point.y, 0.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glViewport(clipRect.point.x, screenHeight - (clipRect.point.y + clipRect.extent.y),
              clipRect.extent.x, clipRect.extent.y);

   sgCurrentClipRect = clipRect;
}

const RectI& dglGetClipRect()
{
   return sgCurrentClipRect;
}

bool dglPointToScreen( Point3F &point3D, Point3F &screenPoint )
{
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
   GLfloat       glMV[16];
   GLfloat       glPR[16];
   GLint          glVP[4];

   glGetFloatv(GL_PROJECTION_MATRIX, glPR);
   glGetFloatv(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   dglGetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   dglGetModelview(&mv);
   MatrixF pr;
   dglGetProjection(&pr);

   F64 x, y, z;
// PUAP -Mat untested
   int result = gluProject( point3D.x, point3D.y, point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );

   screenPoint.x = x;
   screenPoint.y = y;
   screenPoint.z = z;


   return (result == GL_TRUE);
#else
   GLdouble       glMV[16];
   GLdouble       glPR[16];
   GLint          glVP[4];


   glGetDoublev(GL_PROJECTION_MATRIX, glPR);
   glGetDoublev(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   dglGetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   dglGetModelview(&mv);
   MatrixF pr;
   dglGetProjection(&pr);

   F64 x, y, z;
   int result = gluProject( (GLdouble)point3D.x, (GLdouble)point3D.y, (GLdouble)point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );
   screenPoint.x = (F32)x;
   screenPoint.y = (F32)y;
   screenPoint.z = (F32)z;


   return (result == GL_TRUE);
#endif
    
}



bool dglIsInCanonicalState()
{
   bool ret = true;

   // Canonical state:
   //  BLEND disabled
   //  TEXTURE_2D disabled on both texture units.
   //  ActiveTexture set to 0
   //  LIGHTING off
   //  winding : clockwise ?
   //  cullface : disabled

   ret &= glIsEnabled(GL_BLEND) == GL_FALSE;
   ret &= glIsEnabled(GL_CULL_FACE) == GL_FALSE;
   GLint temp;
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
   if (dglDoesSupportARBMultitexture() == true) {
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      //glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      //glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      //glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } else {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   //if (dglDoesSupportFogCoord())
      //ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;

#else

   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } else {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   if (dglDoesSupportFogCoord())
      ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;
#endif
   return ret;
}


void dglSetCanonicalState()
{
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (dglDoesSupportARBMultitexture() == true) {
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } else {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   //if (dglDoesSupportFogCoord())
      //glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
#else
   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } else {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   if (dglDoesSupportFogCoord())
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
#endif
}

void dglGetTransformState(S32* mvDepth,
                          S32* pDepth,
                          S32* t0Depth,
                          F32* t0Matrix,
                          S32* t1Depth,
                          F32* t1Matrix,
                          S32* vp)
{
   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, (GLint*)mvDepth);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, (GLint*)pDepth);

   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t0Depth);
   glGetFloatv(GL_TEXTURE_MATRIX, t0Matrix);
   if (dglDoesSupportARBMultitexture())
   {
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB stuff
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
#else
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
   }
   else
   {
      *t1Depth = 0;
      for (U32 i = 0; i < 16; i++)
         t1Matrix[i] = 0;
   }

   RectI v;
   dglGetViewport(&v);
   vp[0] = v.point.x;
   vp[1] = v.point.y;
   vp[2] = v.extent.x;
   vp[3] = v.extent.y;
}


bool dglCheckState(const S32 mvDepth, const S32 pDepth,
                   const S32 t0Depth, const F32* t0Matrix,
                   const S32 t1Depth, const F32* t1Matrix,
                   const S32* vp)
{
   GLint md, pd;
   RectI v;

   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,  &md);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &pd);

   GLint t0d, t1d;
   GLfloat t0m[16], t1m[16];
   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t0d);
   glGetFloatv(GL_TEXTURE_MATRIX, t0m);
   if (dglDoesSupportARBMultitexture())
   {
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
// PUAP -Mat removed unsupported textureARB and Fog stuff
      //glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      //glActiveTextureARB(GL_TEXTURE0_ARB);
#else
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
   }
   else
   {
      t1d = 0;
      for (U32 i = 0; i < 16; i++)
         t1m[i] = 0;
   }

   dglGetViewport(&v);

   return ((md == mvDepth) &&
           (pd == pDepth) &&
           (t0d == t0Depth) &&
           (dMemcmp(t0m, t0Matrix, sizeof(F32) * 16) == 0) &&
           (t1d == t1Depth) &&
           (dMemcmp(t1m, t1Matrix, sizeof(F32) * 16) == 0) &&
           ((v.point.x  == vp[0]) &&
            (v.point.y  == vp[1]) &&
            (v.extent.x == vp[2]) &&
            (v.extent.y == vp[3])));
}

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
GLfloat gVertexFloats[8];
GLfloat gTextureVerts[8];
#endif
