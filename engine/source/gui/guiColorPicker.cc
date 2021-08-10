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
#include "console/console.h"
#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/guiCanvas.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "gui/guiColorPicker.h"

/// @name Common colors we use
/// @{
ColorF colorWhite(1.,1.,1.);
ColorF colorWhiteBlend(1.,1.,1.,.75);
ColorF colorBlack(0.0f,0.0f,0.0f);
ColorF colorAlpha(0.0f, 0.0f, 0.0f, 0.0f);
ColorF colorAlphaW(1.0f, 1.0f, 1.0f, 0.0f);

ColorI GuiColorPickerCtrl::mColorRange[9] = {
   ColorI(255,255,255), // White
   ColorI(255,0,0),     // Red
    ColorI(255,0,255),   // Pink
    ColorI(0,0,255),     // Blue
    ColorI(0,255,255),   // Light blue
    ColorI(0,255,0),     // Green
    ColorI(255,255,0),   // Yellow
    ColorI(255,0,0),     // Red
   ColorI(0,0,0)        // Black
};
/// @}

IMPLEMENT_CONOBJECT(GuiColorPickerCtrl);

//--------------------------------------------------------------------------
GuiColorPickerCtrl::GuiColorPickerCtrl()
{
   mBounds.extent.set(140, 30);
   mDisplayMode = pPallet;
   mBaseColor = ColorF(1.,.0,1.);
   mPickColor = ColorF(.0,.0,.0);
   mSelectorPos = Point2I(0,0);
   mMouseDown = mMouseOver = false;
   mActive = true;
   mPositionChanged = false;
   mSelectorGap = 1;
   mActionOnMove = false;
   mShowSelector = false;
}

//--------------------------------------------------------------------------
static EnumTable::Enums gColorPickerModeEnums[] =
{
   { GuiColorPickerCtrl::pPallet,		"Pallet"   },
   { GuiColorPickerCtrl::pHorizColorRange,	"HorizColor"},
   { GuiColorPickerCtrl::pVertColorRange,	"VertColor" },
   { GuiColorPickerCtrl::pHorizColorBrightnessRange,	"HorizBrightnessColor"},
   { GuiColorPickerCtrl::pVertColorBrightnessRange,	"VertBrightnessColor" },
   { GuiColorPickerCtrl::pBlendColorRange,	"BlendColor"},
   { GuiColorPickerCtrl::pHorizAlphaRange,	"HorizAlpha"},
   { GuiColorPickerCtrl::pVertAlphaRange,	"VertAlpha" },
   { GuiColorPickerCtrl::pDropperBackground,	"Dropper" },
};

static EnumTable gColorPickerModeTable( 9, gColorPickerModeEnums );

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::initPersistFields()
{
   Parent::initPersistFields();
   
   addGroup("ColorPicker");
   addField("BaseColor", TypeColorF, Offset(mBaseColor, GuiColorPickerCtrl));
   addField("PickColor", TypeColorF, Offset(mPickColor, GuiColorPickerCtrl));
   addField("SelectorGap", TypeS32,  Offset(mSelectorGap, GuiColorPickerCtrl)); 
   addField("DisplayMode", TypeEnum, Offset(mDisplayMode, GuiColorPickerCtrl), 1, &gColorPickerModeTable );
   addField("ActionOnMove", TypeBool,Offset(mActionOnMove, GuiColorPickerCtrl));
   addField("ShowSelector", TypeBool, Offset(mShowSelector, GuiColorPickerCtrl));
   endGroup("ColorPicker");
}

//--------------------------------------------------------------------------
// Function to draw a box which can have 4 different colors in each corner blended together
#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
void dglDrawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
   GLfloat left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   GLfloat top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);
    
    const GLfloat verts[] = {
        left, top,
        right,  top,
        left,  bottom,
        right,   bottom,
    };
    const GLubyte squareColors[] = {
        static_cast<GLubyte>(255 * c1.red),  static_cast<GLubyte>(255 * c1.green),  static_cast<GLubyte>(255 * c1.blue),  static_cast<GLubyte>(255 * c1.alpha),
        static_cast<GLubyte>(255 * c2.red),  static_cast<GLubyte>(255 * c2.green),  static_cast<GLubyte>(255 * c2.blue),  static_cast<GLubyte>(255 * c2.alpha),
        static_cast<GLubyte>(255 * c3.red),  static_cast<GLubyte>(255 * c3.green),  static_cast<GLubyte>(255 * c3.blue),  static_cast<GLubyte>(255 * c3.alpha),
        static_cast<GLubyte>(255 * c4.red),  static_cast<GLubyte>(255 * c4.green),  static_cast<GLubyte>(255 * c4.blue),  static_cast<GLubyte>(255 * c4.alpha),
    };
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

//--------------------------------------------------------------------------
/// Function to draw a set of boxes blending throughout an array of colors
void dglDrawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
   S32 left = bounds.point.x, right = bounds.point.x + bounds.extent.x - 1;
   S32 top = bounds.point.y, bottom = bounds.point.y + bounds.extent.y - 1;

   // Calculate increment value
   S32 x_inc = S32(mFloor((right - left) / (numColors-1)));
   S32 y_inc = S32(mFloor((bottom - top) / (numColors-1)));
   

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

    GLfloat verts[] = {
        0.0f,	0.0f,	0.0f,	0.0f,
        0.0f,	0.0f,	0.0f,	0.0f,
    };	
    
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glEnableClientState(GL_VERTEX_ARRAY);

    for (U16 i=0;i<numColors-1;i++) 
    {
        // If we are at the end, x_inc and y_inc need to go to the end (otherwise there is a rendering bug)
        if (i == numColors-2) 
        {
            x_inc += right-left-1;
            y_inc += bottom-top-1;
        }
        
        if (vertical)  // Vertical (+y)		colors go up and down
        {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            verts[0] = (F32)left;
            verts[1] = (F32)top;
            verts[2] = (F32)(left+x_inc);
            verts[3] = (F32)top;
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            
            verts[4] = (F32)left;
            verts[5] = (F32)bottom;
            verts[6] = (F32)(left+x_inc);
            verts[7] = (F32)bottom;			
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            left += x_inc;
        }
        else  // Horizontal (+x)	colors go lateral
        {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            verts[0] = (F32)left;
            verts[1] = (F32)(top+y_inc);
            verts[2] = (F32)right;
            verts[3] = (F32)(top+y_inc);
            
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            verts[4] = (F32)left;
            verts[5] = (F32)top;
            verts[6] = (F32)right;
            verts[7] = (F32)top;			
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            top += y_inc;
        }
    }	
}
//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(RectI &bounds)
{
   RectI pickerBounds;
   pickerBounds.point.x = bounds.point.x+1;
   pickerBounds.point.y = bounds.point.y+1;
   pickerBounds.extent.x = bounds.extent.x-1;
   pickerBounds.extent.y = bounds.extent.y-1;
   
   //if (mProfile->mBorder)
   //   dglDrawRect(bounds, mProfile->mBorderColor);
      
   Point2I selectorPos = Point2I(bounds.point.x+mSelectorPos.x+1, bounds.point.y+mSelectorPos.y+1);

   // Draw color box differently depending on mode
   RectI blendRect;
   switch (mDisplayMode)
   {
   case pHorizColorRange:
      dglDrawBlendRangeBox( pickerBounds, false, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorRange:
      dglDrawBlendRangeBox( pickerBounds, true, 7, mColorRange + 2);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pHorizColorBrightnessRange:
      blendRect = pickerBounds;
      blendRect.point.y++;
      blendRect.extent.y += 2;
      dglDrawBlendRangeBox( pickerBounds, false, 9, mColorRange);
      // This is being drawn slightly offset from the larger rect so as to insure 255 and 0
      // can both be selected for every color.
      dglDrawBlendBox( blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack );
      blendRect.point.y += blendRect.extent.y - 1;
      //blendRect.extent.y = 2;
      dglDrawRect( blendRect, colorBlack);
      drawSelector( pickerBounds, selectorPos, sVertical );
      drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pVertColorBrightnessRange:
      dglDrawBlendRangeBox( pickerBounds, true, 9, mColorRange);
      dglDrawBlendBox( pickerBounds, colorAlpha, colorAlpha, colorBlack, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pHorizAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorWhite, colorBlack, colorWhite );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorBlack, colorWhite, colorWhite );
      drawSelector( pickerBounds, selectorPos, sHorizontal ); 
   break;
   case pBlendColorRange:
      dglDrawBlendBox( pickerBounds, colorWhite, mBaseColor, colorBlack, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );      
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pDropperBackground:
   break;
   case pPallet:
   default:
      dglDrawRectFill( pickerBounds, mBaseColor );
   break;
   }
}

#else

void dglDrawBlendBox(RectI &bounds, ColorF &c1, ColorF &c2, ColorF &c3, ColorF &c4)
{
   F32 l = (F32)(bounds.point.x + 1);
   F32 r =(F32)(bounds.point.x + bounds.extent.x - 2);
   F32 t = (F32)(bounds.point.y + 1);
   F32 b = (F32)(bounds.point.y + bounds.extent.y - 2);
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);

      // Color
      glColor4fv(c2.address());
      glVertex2f(l, t);
      glColor4fv(c2.address());
      glVertex2f(r, t);
      glColor4fv(c2.address());
      glVertex2f(r, b);
      glColor4fv(c2.address());
      glVertex2f(l, b);

      // White
      glColor4fv(c1.address());
      glVertex2f(l, t);
      glColor4fv(colorAlphaW.address());
      glVertex2f(r, t);
      glColor4fv(colorAlphaW.address());
      glVertex2f(r, b);
      glColor4fv(c1.address());
      glVertex2f(l, b);

      // Black
      glColor4fv(c3.address());
      glVertex2f(l, t);
      glColor4fv(c3.address());
      glVertex2f(r, t);
      glColor4fv(c4.address());
      glVertex2f(r, b);
      glColor4fv(c4.address());
      glVertex2f(l, b);

      // Top right
      glColor4fv(c2.address());
      glVertex2f(r, t);
      glColor4fv(c2.address());
      glVertex2f(r, t+1);
      glColor4fv(c2.address());
      glVertex2f(r-1, t+1);
      glColor4fv(c2.address());
      glVertex2f(r-1, t);
      
      // Top-Right Corner
      glColor4fv(c2.address());
      glVertex2f(r, t);
      glColor4fv(c2.address());
      glVertex2f(r, t-1);
      glColor4fv(c2.address());
      glVertex2f(r+1, t-1);
      glColor4fv(c2.address());
      glVertex2f(r+1, t);

      // Top-Right Corner
      glColor4fv(c1.address());
      glVertex2f(l, t);
      glColor4fv(c1.address());
      glVertex2f(l, t-1);
      glColor4fv(c1.address());
      glVertex2f(l-1, t-1);
      glColor4fv(c1.address());
      glVertex2f(l-1, t);

      // Top row
      glColor4fv(c1.address());
      glVertex2f(l, t);
      glColor4fv(c1.address());
      glVertex2f(l, t-1);
      glColor4fv(c2.address());
      glVertex2f(r, t-1);
      glColor4fv(c2.address());
      glVertex2f(r, t);
      

      // Right side
      glColor4fv(c2.address());
      glVertex2f(r, t);
      glColor4fv(c2.address());
      glVertex2f(r+1, t);
      glColor4fv(c4.address());
      glVertex2f(r+1, b);
      glColor4fv(c4.address());
      glVertex2f(r, b);

      // Left side
      glColor4fv(c1.address());
      glVertex2f(l, t);
      glColor4fv(c1.address());
      glVertex2f(l-1, t);
      glColor4fv(c4.address());
      glVertex2f(l-1, b);
      glColor4fv(c4.address());
      glVertex2f(l, b);

      // Bottom row
      glColor4fv(c4.address());
      glVertex2f(l-1, b);
      glColor4fv(c4.address());
      glVertex2f(l-1, b+1);
      glColor4fv(c4.address());
      glVertex2f(r+1, b+1);
      glColor4fv(c4.address());
      glVertex2f(r+1, b);

   glEnd();
}

//--------------------------------------------------------------------------
/// Function to draw a set of boxes blending throughout an array of colors
void dglDrawBlendRangeBox(RectI &bounds, bool vertical, U8 numColors, ColorI *colors)
{
   F32 l = (F32)bounds.point.x;
   F32 r = (F32)(bounds.point.x + bounds.extent.x - 1);
   F32 t = (F32)bounds.point.y + 1;
   F32 b = (F32)(bounds.point.y + bounds.extent.y - 2);
   
   // Calculate increment value
   F32 x_inc = F32((r - l) / (numColors-1));
   F32 y_inc = F32((b - t) / (numColors-1));
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glBegin(GL_QUADS);
      if (!vertical)  // Horizontal (+x)
      {
            glColor4ub(colors[0].red, colors[0].green, colors[0].blue, colors[0].alpha);
            glVertex2f(l, t);
            glVertex2f(l, b);
            glVertex2f(l-1, b);
            glVertex2f(l-1, t);
            glVertex2f(r, t);
            glVertex2f(r, b);
            glVertex2f(r+1, b);
            glVertex2f(r+1, t);
      }
      else   // Vertical (+y)
      {
            glColor4ub(colors[0].red, colors[0].green, colors[0].blue, colors[0].alpha);
            glVertex2f(l, t);
            glVertex2f(r, t);
            glVertex2f(r, t-1);
            glVertex2f(l, t-1);
            glVertex2f(l, b);
            glVertex2f(r, b);
            glVertex2f(r, b+1);
            glVertex2f(l, b+1);
      }

      for (U16 i=0;i<numColors-1;i++) 
      {
         if (!vertical)  // Horizontal (+x)
         {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            glVertex2f(l, t);
            glVertex2f(l, b);
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            glVertex2f(l+x_inc, b);
            glVertex2f(l+x_inc, t);
            l += x_inc;
         }
         else  // Vertical (+y)
         {
            // First color
            glColor4ub(colors[i].red, colors[i].green, colors[i].blue, colors[i].alpha);
            glVertex2f(l, t);
            glVertex2f(r, t);
            // Second color
            glColor4ub(colors[i+1].red, colors[i+1].green, colors[i+1].blue, colors[i+1].alpha);
            glVertex2f(r, t+y_inc);
            glVertex2f(l, t+y_inc);
            t += y_inc;
        }
     }
   glEnd();
}

//--------------------------------------------------------------------------
/// Function to invoke calls to draw the picker box and selector
void GuiColorPickerCtrl::renderColorBox(RectI &bounds)
{
   RectI pickerBounds;
   pickerBounds.point.x = bounds.point.x+1;
   pickerBounds.point.y = bounds.point.y+1;
   pickerBounds.extent.x = bounds.extent.x-1;
   pickerBounds.extent.y = bounds.extent.y-1;
   
   if (mProfile->mBorderDefault && mProfile->mBorderDefault->mBorder > 0)
      dglDrawRect(bounds, mProfile->mBorderDefault->mBorderColor[0]);
      
   Point2I selectorPos = Point2I(bounds.point.x+mSelectorPos.x+1, bounds.point.y+mSelectorPos.y+1);

   // Draw color box differently depending on mode
   RectI blendRect;
   switch (mDisplayMode)
   {
   case pHorizColorRange:
      dglDrawBlendRangeBox( pickerBounds, false, 7, mColorRange + 1);
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorRange:
      dglDrawBlendRangeBox( pickerBounds, true, 7, mColorRange + 1);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
   break;
   case pHorizColorBrightnessRange:
      blendRect = pickerBounds;
      blendRect.point.y++;
      blendRect.extent.y -= 2;
      dglDrawBlendRangeBox( pickerBounds, false, 9, mColorRange);
      // This is being drawn slightly offset from the larger rect so as to insure 255 and 0
      // can both be selected for every color.
      dglDrawBlendBox( blendRect, colorAlpha, colorAlpha, colorBlack, colorBlack );
      blendRect.point.y += blendRect.extent.y - 1;
      blendRect.extent.y = 2;
      dglDrawRect( blendRect, colorBlack);
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertColorBrightnessRange:
      dglDrawBlendRangeBox( pickerBounds, true, 9, mColorRange);
      dglDrawBlendBox( pickerBounds, colorAlpha, colorBlack, colorBlack, colorAlpha );
      drawSelector( pickerBounds, selectorPos, sHorizontal );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pHorizAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorWhite, colorWhite, colorBlack );
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pVertAlphaRange:
      dglDrawBlendBox( pickerBounds, colorBlack, colorBlack, colorWhite, colorWhite );
      drawSelector( pickerBounds, selectorPos, sHorizontal ); 
   break;
   case pBlendColorRange:
      dglDrawBlendBox( pickerBounds, colorWhite, mBaseColor, colorAlpha, colorBlack );
      drawSelector( pickerBounds, selectorPos, sHorizontal );      
      drawSelector( pickerBounds, selectorPos, sVertical );
   break;
   case pDropperBackground:
   break;
   case pPallet:
   default:
      dglDrawRectFill( pickerBounds, mBaseColor );
   break;
   }
}
#endif

void GuiColorPickerCtrl::drawSelector(RectI &bounds, Point2I &selectorPos, SelectorMode mode)
{
    if (!mShowSelector)
        return;

    U16 sMax = mSelectorGap*2;
    switch (mode)
    {
        case sVertical:
            // Now draw the vertical selector
            // Up -> Pos
            if (selectorPos.y != bounds.point.y+1)
                dglDrawLine(selectorPos.x, bounds.point.y, selectorPos.x, selectorPos.y-sMax-1, colorWhiteBlend);
            // Down -> Pos
            if (selectorPos.y != bounds.point.y+bounds.extent.y) 
                dglDrawLine(selectorPos.x,	selectorPos.y + sMax, selectorPos.x, bounds.point.y + bounds.extent.y, colorWhiteBlend);
        break;
        case sHorizontal:
            // Now draw the horizontal selector
            // Left -> Pos
            if (selectorPos.x != bounds.point.x) 
            dglDrawLine(bounds.point.x, selectorPos.y-1, selectorPos.x-sMax, selectorPos.y-1, colorWhiteBlend);
            // Right -> Pos
            if (selectorPos.x != bounds.point.x) 
            dglDrawLine(bounds.point.x+mSelectorPos.x+sMax, selectorPos.y-1, bounds.point.x + bounds.extent.x, selectorPos.y-1, colorWhiteBlend);
        break;
    }
}

void GuiColorPickerCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   RectI boundsRect(offset, mBounds.extent); 
   renderColorBox(boundsRect);

   if (mPositionChanged) 
   {
      mPositionChanged = false;
      Point2I extent = Canvas->getExtent();
      // If we are anything but a pallete, change the pick color
      if (mDisplayMode != pPallet) 
      {
         // To simplify things a bit, just read the color via glReadPixels()

         // [neo, 5/30/2007 - #3175]
         // glReadPixels does not even look at the format passed in, it just assumes 
         // GL_RGBA and a dword buffer, so we  can't use GL_FLOAT, etc. glReadPixels
         // was borked in D3D and not calculating the correct offset rectangle etc 
         // so I changed it to match the OGL version functionality exactly.
         // [see glReadPixels in OpenGL2D3C.cc]
         
         // glFloat rBuffer[4];
         //glReadPixels( buf_x, buf_y, 1, 1, GL_RGBA, GL_FLOAT, rBuffer);
         //mPickColor.red = rBuffer[0];
         //mPickColor.green = rBuffer[1];
         //mPickColor.blue = rBuffer[2];
         //mPickColor.alpha = rBuffer[3]; 

         GLubyte rBuffer[4] = { 255, 255, 255, 255 };

         glReadBuffer( GL_BACK );

         U32 buf_x = offset.x + mSelectorPos.x + 1;
         U32 buf_y = extent.y - ( offset.y + mSelectorPos.y + 1 );
                  
         glReadPixels( buf_x, buf_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rBuffer );
      
         mPickColor = ColorI( rBuffer[ 0 ], rBuffer[ 1 ], rBuffer[ 2 ], 255 );

         // Now do onAction() if we are allowed
         if (mActionOnMove) 
            onAction();
      }
   }
   
   //render the children
   renderChildControls( offset, mBounds, updateRect);
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::setSelectorPos(const Point2I &pos)
{
   Point2I extent = mBounds.extent;
   RectI rect;
   if (mDisplayMode != pDropperBackground) 
   {
      extent.x -= 2;
      extent.y -= 2;
      rect = RectI(Point2I(0,1), extent);
   }
   else
   {
      rect = RectI(Point2I(0,0), extent);
   }
   
   if (rect.pointInRect(pos)) 
   {
      mSelectorPos = pos;
      mPositionChanged = true;
      // We now need to update
      setUpdate();
   }

   else
   {
      if ((pos.x > rect.point.x) && (pos.x < (rect.point.x + rect.extent.x)))
         mSelectorPos.x = pos.x;
      else if (pos.x <= rect.point.x)
         mSelectorPos.x = rect.point.x;
      else if (pos.x >= (rect.point.x + rect.extent.x))
         mSelectorPos.x = rect.point.x + rect.extent.x - 1;

      if ((pos.y > rect.point.y) && (pos.y < (rect.point.y + rect.extent.y)))
         mSelectorPos.y = pos.y;
      else if (pos.y <= rect.point.y)
         mSelectorPos.y = rect.point.y;
      else if (pos.y >= (rect.point.y + rect.extent.y))
         mSelectorPos.y = rect.point.y + rect.extent.y - 1;

      mPositionChanged = true;
      setUpdate();
   }
}

//--------------------------------------------------------------------------
F32 colorHue(ColorF color)
{
    F32 b = mSqrt(3) * (color.green - color.blue);
    F32 a = (2 * color.red) - color.green - color.blue;
    F32 hue;
    
    if (b == 0 && a == 0)
        hue = 0;
    else
        hue = mRadToDeg(atan2(b, a));

    while (hue < 0)
        hue += 360;

    return hue;
}

Point2I GuiColorPickerCtrl::getRangeBoxColorPos(RectI &bounds, bool vertical, ColorF targetColor)
{
    // Calculate hue
    F32 hue = colorHue(targetColor);

    // Transform the hue value to [0-1].
    F32 hueFraction = hue / 360;

    // Calculate position in range box
    Point2I position;
    if (vertical)
    {
        position.x = (S32)(0.5f * (bounds.extent.x));
        position.y = (S32)((1.0f - hueFraction) * (bounds.extent.y));
    }
    else
    {
        position.x = (S32)((1.0f - hueFraction) * (bounds.extent.x));
        position.y = (S32)(0.5f * (bounds.extent.y));
    }

    return position;
}

Point2I GuiColorPickerCtrl::getBlendBoxColorPos(RectI &bounds, ColorF targetColor)
{
    // Calculate hue
    F32 hue = colorHue(targetColor);

    
    // Calculate the largest, smallest RGB components of the hue
    F32 largest, smallest, baseLargest, baseSmallest;
    if (hue >= 0 && hue < 60)
    {
        largest = targetColor.red;
        smallest = targetColor.blue;
        baseLargest = mBaseColor.red;
        baseSmallest = mBaseColor.blue;
    }
    else if (hue >= 60 && hue < 120)
    {
        largest = targetColor.green;
        smallest = targetColor.blue;
        baseLargest = mBaseColor.green;
        baseSmallest = mBaseColor.blue;
    }
    else if (hue >= 120 && hue < 180)
    {
        largest = targetColor.green;
        smallest = targetColor.red;
        baseLargest = mBaseColor.green;
        baseSmallest = mBaseColor.red;
    }
    else if (hue >= 180 && hue < 240)
    {
        largest = targetColor.blue;
        smallest = targetColor.red;
        baseLargest = mBaseColor.blue;
        baseSmallest = mBaseColor.red;
    }
    else if (hue >= 240 && hue < 300)
    {
        largest = targetColor.blue;
        smallest = targetColor.green;
        baseLargest = mBaseColor.blue;
        baseSmallest = mBaseColor.green;
    }
    else if (hue >= 300 && hue < 360)
    {
        largest = targetColor.red;
        smallest = targetColor.green;
        baseLargest = mBaseColor.red;
        baseSmallest = mBaseColor.green;
    }
    else
    {
        // Color hue must be between 0 and 360.
        Con::errorf("getBlendBoxColorPos - Color hue is outside allowable range.");
        return Point2I(0,0);
    }

    // Calculate position in range box
    Point2I position;

    F32 h = 1.0f - (smallest / largest);
    F32 v = 1.0f - largest;

    position.x = (S32)(h * (bounds.extent.x - 1));
    position.y = (S32)(v * (bounds.extent.y - 1));

    return position;
}

Point2I GuiColorPickerCtrl::getSelectorPositionForColor(RectI &bounds, ColorF targetColor)
{
    Point2I position(0,0);

    switch (mDisplayMode)
    {
        case pHorizColorRange:
            position = getRangeBoxColorPos(bounds, false, targetColor);
            break;
        case pVertColorRange:
            position = getRangeBoxColorPos(bounds, true, targetColor);
            break;

        case pBlendColorRange:
            position = getBlendBoxColorPos(bounds, targetColor);
            break;

        default:
             Con::errorf("Display mode does not support the function: getSelectorPositionForColor().");
            break;
    }

    return position;
}


//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchDown(const GuiEvent &event)
{
   if (!mActive)
      return;
   
   if (mDisplayMode == pDropperBackground)
      return;

   mouseLock(this);
   
   if (mProfile->mCanKeyFocus)
      setFirstResponder();

   // Update the picker cross position
   if (mDisplayMode != pPallet)
      setSelectorPos(globalToLocalCoord(event.mousePoint)); 
   
   mMouseDown = true;

   Con::executef(this, 1, "onTouchDown");
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchDragged(const GuiEvent &event)
{
   if ((mActive && mMouseDown) || (mActive && (mDisplayMode == pDropperBackground)))
   {
      // Update the picker cross position
      if (mDisplayMode != pPallet)
         setSelectorPos(globalToLocalCoord(event.mousePoint));
   }

   if (!mActionOnMove && mAltConsoleCommand[0] )
      Con::evaluate( mAltConsoleCommand, false );

   Con::executef(this, 1, "onTouchDragged");
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchMove(const GuiEvent &event)
{
   // Only for dropper mode
   if (mActive && (mDisplayMode == pDropperBackground))
      setSelectorPos(globalToLocalCoord(event.mousePoint));

   Con::executef(this, 1, "onTouchMove");
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchEnter(const GuiEvent &event)
{
   mMouseOver = true;

   Con::executef(this, 1, "onTouchEnter");
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchLeave(const GuiEvent &)
{
   // Reset state
   mMouseOver = false;

   Con::executef(this, 1, "onTouchLeave");
}

//--------------------------------------------------------------------------
void GuiColorPickerCtrl::onTouchUp(const GuiEvent &)
{
   //if we released the mouse within this control, perform the action
   if (mActive && mMouseDown && (mDisplayMode != pDropperBackground)) 
   {
      onAction();
      mMouseDown = false;
   }
   else if (mActive && (mDisplayMode == pDropperBackground)) 
   {
      // In a dropper, the alt command executes the mouse up action (to signal stopping)
      if ( mAltConsoleCommand[0] )
         Con::evaluate( mAltConsoleCommand, false );
   }
   
   mouseUnlock();

   Con::executef(this, 1, "onTouchUp");
}

//--------------------------------------------------------------------------
const char *GuiColorPickerCtrl::getScriptValue()
{
   static char temp[256];
   ColorF color = getValue();
   dSprintf(temp,256,"%g %g %g %g",color.red, color.green, color.blue, color.alpha);
   return temp; 
}

//--------------------------------------------------------------------------    
void GuiColorPickerCtrl::setScriptValue(const char *value)
{
   ColorF newValue;
   dSscanf(value, "%g %g %g %g", &newValue.red, &newValue.green, &newValue.blue, &newValue.alpha);
   setValue(newValue);
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos, const char*, 2, 2, "() Gets the current position of the selector\n"
              "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPos();
   dSprintf(temp,256,"%d %d",pos.x, pos.y); 
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPos2, const char*, 2, 2, "() Gets the current position of the selector\n"
              "@return Returns the position of the selector as space-separted x,y coordinates.")
{
   char *temp = Con::getReturnBuffer(256);
   Point2I pos;
   pos = object->getSelectorPos();
   dSprintf(temp,256,"%d %d",pos.x, pos.y); 
   return temp;
}

ConsoleMethod(GuiColorPickerCtrl, getSelectorPosForColor, const char*, 3, 6, "(float red, float green, float blue, [float alpha = 1.0]) - Gets the selector position for the specified color."
                                                          "The display mode must be pHorizColorRange, pVertColorRange, or pBlendColorRange.\n"
                                                          "@param red The red value.\n"
                                                          "@param green The green value.\n"
                                                          "@param blue The blue value.\n"
                                                          "@param alpha The alpha value.\n"
              "@return Returns the position of the selector as space-separted x,y coordinates.")
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Grab the element count.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Space separated.
    if (argc < 4)
    {
        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
            alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
            return StringTable->EmptyString;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("GuiColorPickerCtrl::getSelectorPosForColor() - Invalid Number of parameters!");
        return StringTable->EmptyString;
    }

    char *temp = Con::getReturnBuffer(256);
    Point2I pos;
    pos = object->getSelectorPositionForColor(object->mBounds, ColorF(red, green, blue, alpha));
    dSprintf(temp,256,"%d %d",pos.x, pos.y); 
    return temp;
}

ConsoleMethod(GuiColorPickerCtrl, setSelectorPos, void, 3, 3, "(\"x y\")Sets the current position of the selector"
              "@param The coordinates with space-separated formating.\n"
              "@return No return value.")
{
   Point2I newPos;
   dSscanf(argv[2], "%d %d", &newPos.x, &newPos.y);
   object->setSelectorPos(newPos);
}

ConsoleMethod(GuiColorPickerCtrl, updateColor, void, 2, 2, "() Forces update of pick color\n"
              "@return No return value.")
{
    object->updateColor();
}
