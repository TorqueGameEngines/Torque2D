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

#ifndef _GUIFORMCTRL_H_
#define _GUIFORMCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUICANVAS_H_
#include "gui/guiCanvas.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

class GuiMenuBar;

/// Collapsable pane control.
///
/// This class wraps a single child control and displays a header with caption
/// above it. If you click the header it will collapse or expand. The control
/// resizes itself based on its collapsed/expanded size.
///
/// In the GUI editor, if you just want the header you can make collapsable 
/// false. The caption field lets you set the caption. It expects a bitmap
/// (from the GuiControlProfile) that contains two images - the first is
/// displayed when the control is expanded and the second is displayed when
/// it is collapsed. The header is sized based off of the first image.
class GuiFormCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

   Resource<GFont>  mFont;
   StringTableEntry mCaption;
   bool             mCanMove;
   bool             mUseSmallCaption;
   StringTableEntry mSmallCaption;
   StringTableEntry mContentLibrary;
   StringTableEntry mContent;

   Point2I          mThumbSize;
   bool             mHasMenu;

   GuiMenuBar*      mMenuBar;

   bool mMouseMovingWin;

   Point2I mMouseDownPosition;
   RectI mOrigBounds;
   RectI mStandardBounds;

   RectI mCloseButton;
   RectI mMinimizeButton;
   RectI mMaximizeButton;

   bool mMouseOver;
   bool mDepressed;

public:
   GuiFormCtrl();
   virtual ~GuiFormCtrl();

   void setCaption(const char* caption);

   void resize(const Point2I &newPosition, const Point2I &newExtent);
   void onRender(Point2I offset, const RectI &updateRect);

   // DAW: Called when the GUI theme changes and a bitmap arrary may need updating
  // void onThemeChange();

   U32  getMenuBarID();

   bool onAdd();
   bool onWake();
   void onSleep();

   virtual void addObject(SimObject *newObj );

   void onTouchDragged(const GuiEvent &event);
   void onTouchDown(const GuiEvent &event);
   void onTouchUp(const GuiEvent &event);
   void onTouchMove(const GuiEvent &event);
   void onTouchLeave(const GuiEvent &event);
   void onTouchEnter(const GuiEvent &event);

   static void initPersistFields();
   DECLARE_CONOBJECT(GuiFormCtrl);
};

#endif