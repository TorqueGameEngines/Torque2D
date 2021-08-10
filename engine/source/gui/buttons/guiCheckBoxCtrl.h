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

#ifndef _GUICHECKBOXCTRL_H_
#define _GUICHECKBOXCTRL_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif

class GuiCheckBoxCtrl : public GuiButtonCtrl
{
   typedef GuiButtonCtrl Parent;

protected:
	bool mStateOn;
	Point2I mBoxOffset;
	Point2I mBoxExtent;
	Point2I mTextOffset;
	Point2I mTextExtent;
	
public:
   DECLARE_CONOBJECT(GuiCheckBoxCtrl);
   GuiCheckBoxCtrl();

   bool onWake();
   static void initPersistFields();

   bool getStateOn(void) { return mStateOn; }
   void setStateOn(bool bStateOn);

   const Point2I&   getBoxOffset() { return mBoxOffset; } 
   const Point2I&   getBoxExtent() { return mBoxExtent; }

   void setBoxOffset(const Point2I &newOffset) { mBoxOffset = newOffset; }
   void setBoxExtent(const Point2I &newExtent) { mBoxExtent = newExtent; }

   const Point2I&   getTextOffset() { return mTextOffset; }
   const Point2I&   getTextExtent() { return mTextExtent; }

   void setTextOffset(const Point2I &newOffset) { mTextOffset = newOffset; }
   void setTextExtent(const Point2I &newExtent) { mTextExtent = newExtent; }

   void onRender(Point2I offset, const RectI &updateRect);
   virtual void renderInnerControl(RectI &boxRect, const GuiControlState currentState);
   void setScriptValue(const char *value);
   const char *getScriptValue();

   void onMessage(GuiControl *, S32 msg);
   void onAction();
};

#endif //_GUI_CHECKBOX_CTRL_H
