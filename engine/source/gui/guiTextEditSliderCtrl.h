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

#ifndef _GUITEXTEDITSLIDERCTRL_H_
#define _GUITEXTEDITSLIDERCTRL_H_

#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif
#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif

class GuiTextEditSliderCtrl : public GuiTextEditCtrl
{
private:
   typedef GuiTextEditCtrl Parent;
   Point2F mRange;
   F32 mIncAmount;
   F32 mValue;
   F32 mIncCounter;
   F32 mMulInc;
   StringTableEntry mFormat;
   U32 mMouseDownTime;
   // max string len, must be less then or equal to 255
public:
   enum CtrlArea
   {
      None,
      Slider,
      ArrowUp,
      ArrowDown
   };
private:
   CtrlArea mTextAreaHit;
public:
   GuiTextEditSliderCtrl();
   ~GuiTextEditSliderCtrl();
   DECLARE_CONOBJECT(GuiTextEditSliderCtrl);
   static void initPersistFields();

   void getText(char *dest);  // dest must be of size
                                      // StructDes::MAX_STRING_LEN + 1

   void setText(const char *txt);

   void setValue();
   void checkRange();
   void checkIncValue();
   void timeInc(U32 elapseTime);

   bool onKeyDown(const GuiEvent &event);
   void onTouchDown(const GuiEvent &event);
   void onTouchDragged(const GuiEvent &event);
   void onTouchUp(const GuiEvent &event);


   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);


};

#endif //_GUI_TEXTEDIT_CTRL_H
