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

#ifndef _GUIDRAGANDDROPCTRL_H_
#define _GUIDRAGANDDROPCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiDragAndDropCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;


   // The mouse down offset from the upper left of the control.
   Point2I mOffset;
   bool mDeleteOnMouseUp;

   // Controls may want to react when they are dragged over, entered or exited.
   GuiControl* mLastTarget;
   
   // Convenience methods for sending events
   void sendDragEvent(GuiControl* target, const char* event);
   GuiControl* findDragTarget(Point2I mousePoint, const char* method);

public:
   GuiDragAndDropCtrl();

   void startDragging(Point2I offset = Point2I(0, 0));

   virtual void onTouchDown(const GuiEvent& event);
   virtual void onTouchDragged(const GuiEvent& event);
   virtual void onTouchUp(const GuiEvent& event);

   static void initPersistFields();

   DECLARE_CONOBJECT(GuiDragAndDropCtrl);
};

#endif