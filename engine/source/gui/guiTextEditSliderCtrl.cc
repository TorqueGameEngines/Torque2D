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

#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/guiTextEditSliderCtrl.h"

IMPLEMENT_CONOBJECT(GuiTextEditSliderCtrl);

GuiTextEditSliderCtrl::GuiTextEditSliderCtrl()
{
   mRange.set(0.0f, 1.0f);
   mIncAmount = 1.0f;
   mValue = 0.0f;
   mMulInc = 0;
   mIncCounter = 0.0f;
   mFormat = StringTable->insert("%3.2f");
   mTextAreaHit = None;
}

GuiTextEditSliderCtrl::~GuiTextEditSliderCtrl()
{
}

void GuiTextEditSliderCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("format",    TypeString,  Offset(mFormat, GuiTextEditSliderCtrl));
   addField("range",     TypePoint2F, Offset(mRange, GuiTextEditSliderCtrl));
   addField("increment", TypeF32,     Offset(mIncAmount,     GuiTextEditSliderCtrl));
}

void GuiTextEditSliderCtrl::setText(const char *txt)
{
   mValue = dAtof(txt);
   checkRange();
   setValue();
}

bool GuiTextEditSliderCtrl::onKeyDown(const GuiEvent &event)
{
   return Parent::onKeyDown(event);
}

void GuiTextEditSliderCtrl::checkRange()
{
   if(mValue < mRange.x)
      mValue = mRange.x;
   else if(mValue > mRange.y)
      mValue = mRange.y;
}

void GuiTextEditSliderCtrl::setValue()
{
   char buf[20];
   dSprintf(buf,sizeof(buf),mFormat, mValue);
   Parent::setText(buf);
}

void GuiTextEditSliderCtrl::onTouchDown(const GuiEvent &event)
{
   mValue = dAtof(mText);

   mMouseDownTime = Sim::getCurrentTime();
   GuiControl *parent = getParent();
   if(!parent)
      return;
   Point2I camPos  = event.mousePoint;
   Point2I point = parent->localToGlobalCoord(mBounds.point);

   if(camPos.x > point.x + mBounds.extent.x - 14)
   {
      if(camPos.y > point.y + (mBounds.extent.y/2))
      {
         mValue -=mIncAmount;
         mTextAreaHit = ArrowDown;
         mMulInc = -0.15f;
      }
      else
      {
         mValue +=mIncAmount;
         mTextAreaHit = ArrowUp;
         mMulInc = 0.15f;
      }

      checkRange();
      setValue();
      mouseLock();
      return;
   }
   Parent::onTouchDown(event);
}

void GuiTextEditSliderCtrl::onTouchDragged(const GuiEvent &event)
{
   if(mTextAreaHit == None || mTextAreaHit == Slider)
   {
      mTextAreaHit = Slider;
      GuiControl *parent = getParent();
      if(!parent)
         return;
      Point2I camPos = event.mousePoint;
      Point2I point = parent->localToGlobalCoord(mBounds.point);
      F32 maxDis = 100;
      F32 val;
      if(camPos.y < point.y)
      {
         if(point.y < maxDis)
            maxDis = (F32)point.y;
         val = point.y - maxDis;
         if(point.y > 0)
            mMulInc= 1.0f-(((float)camPos.y - val) / maxDis);
         else
            mMulInc = 1.0f;
         checkIncValue();
         return;
      }
      else if(camPos.y > point.y + mBounds.extent.y)
      {
         GuiCanvas *root = getRoot();
         val = (F32)(root->mBounds.extent.y - (point.y + mBounds.extent.y));
         if(val < maxDis)
            maxDis = val;
         if( val > 0)
            mMulInc= -(float)(camPos.y - (point.y + mBounds.extent.y))/maxDis;
         else
            mMulInc = -1.0f;
         checkIncValue();
         return;
      }
      mTextAreaHit = None;
      Parent::onTouchDragged(event);
   }
}

void GuiTextEditSliderCtrl::onTouchUp(const GuiEvent &event)
{
   mMulInc = 0.0f;
   mouseUnlock();
  //if we released the mouse within this control, then the parent will call
  //the mConsoleCommand other wise we have to call it.
   Parent::onTouchUp(event);
  //if we didn't release the mouse within this control, then perform the action
   if (!cursorInControl())
      Con::evaluate(mConsoleCommand, false);
   mTextAreaHit = None;
}
void GuiTextEditSliderCtrl::checkIncValue()
{
   if(mMulInc > 1.0f)
      mMulInc = 1.0f;
   else if(mMulInc < -1.0f)
      mMulInc = -1.0f;
}

void GuiTextEditSliderCtrl::timeInc(U32 elapseTime)
{
   S32 numTimes = elapseTime / 750;
   if(mTextAreaHit != Slider && numTimes > 0)
   {
      if(mTextAreaHit == ArrowUp)
         mMulInc = 0.15f * numTimes;
      else
         mMulInc = -0.15f * numTimes;

      checkIncValue();
   }
}

void GuiTextEditSliderCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(mTextAreaHit != None)
   {
      U32 elapseTime = Sim::getCurrentTime() - mMouseDownTime;
      if(elapseTime > 750 || mTextAreaHit == Slider)
      {
         timeInc(elapseTime);
         mIncCounter += mMulInc;
         if(mIncCounter >= 1.0f || mIncCounter <= -1.0f)
         {
            mValue = (mMulInc > 0.0f) ? mValue+mIncAmount : mValue-mIncAmount;
            mIncCounter = (mIncCounter > 0.0f) ? mIncCounter-1 : mIncCounter+1;
            checkRange();
            setValue();
         }
      }
   }
	Parent::onRender(offset, updateRect);

   Point2I start(offset.x + mBounds.extent.x - 14, offset.y);
   Point2I midPoint(start.x + 7, start.y + (mBounds.extent.y/2));

   dglDrawRectFill(Point2I(start.x+1,start.y+1), Point2I(start.x+13,start.y+mBounds.extent.y-1) , mProfile->mFillColor);

   dglDrawLine(start, Point2I(start.x, start.y+mBounds.extent.y),mProfile->mFontColor);
   dglDrawLine(Point2I(start.x,midPoint.y),
               Point2I(start.x+14,midPoint.y),
               mProfile->mFontColor);

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)

   glColor4f(0,0,0,255);
	
	GLfloat verts[] = {
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
		0, 0,
	};

      if(mTextAreaHit == ArrowUp)
      {
		  verts[0] = (GLfloat)midPoint.x;
		  verts[1] = (GLfloat)start.y+1;
		  
		  verts[2] = (GLfloat)start.x+11;
		  verts[3] = (GLfloat)midPoint.y-2;
		  
		  verts[4] = (GLfloat)start.x+3;
		  verts[5] = (GLfloat)midPoint.y-2;
      }
      else
      {
		  verts[0] = (GLfloat)midPoint.x;
		  verts[1] = (GLfloat)start.y+2;
		  verts[2] = (GLfloat)start.x+11;
		  verts[3] = (GLfloat)midPoint.y-1;
		  verts[4] = (GLfloat)start.x+3;
		  verts[5] = (GLfloat)midPoint.y-1;
      }
      if(mTextAreaHit == ArrowDown)
      {
		  verts[6] = (GLfloat)midPoint.x;
		  verts[7] = (GLfloat)start.y+mBounds.extent.y-1;
		  verts[8] = (GLfloat)start.x+11;
		  verts[9] = (GLfloat)midPoint.y+3;
		  verts[10] = (GLfloat)start.x+3;
		  verts[11] = (GLfloat)midPoint.y+3;  
      }
   	else
      {
		  verts[6] = (GLfloat)midPoint.x;
		  verts[7] = (GLfloat)start.y+mBounds.extent.y-2;
		  verts[8] = (GLfloat)start.x+11;
		  verts[9] = (GLfloat)midPoint.y+2;
		  verts[10] = (GLfloat)start.x+3;
		  verts[11] = (GLfloat)midPoint.y+2; 
      }
	glVertexPointer(2, GL_FLOAT, 0, verts);	

	glDrawArrays(GL_TRIANGLES, 0, 6);	
#else
   glColor3i(0,0,0);

   glBegin(GL_TRIANGLES);
      if(mTextAreaHit == ArrowUp)
      {
         glVertex2i(midPoint.x, start.y+1);
      	glVertex2i(start.x+11,midPoint.y-2);
      	glVertex2i(start.x+3,midPoint.y-2);
      }
      else
      {
         glVertex2i(midPoint.x, start.y+2);
      	glVertex2i(start.x+11,midPoint.y-1);
      	glVertex2i(start.x+3,midPoint.y-1);
      }
      if(mTextAreaHit == ArrowDown)
      {
   	   glVertex2i(midPoint.x, start.y+mBounds.extent.y-1);
   	   glVertex2i(start.x+11,midPoint.y+3);
   	   glVertex2i(start.x+3,midPoint.y+3);
      }
   	else
      {
   	   glVertex2i(midPoint.x, start.y+mBounds.extent.y-2);
   	   glVertex2i(start.x+11,midPoint.y+2);
   	   glVertex2i(start.x+3,midPoint.y+2);
      }
   glEnd();
#endif
}


