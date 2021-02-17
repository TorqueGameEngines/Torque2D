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

#ifndef _GUIWINDOWCTRL_H_
#define _GUIWINDOWCTRL_H_

#ifndef _GUITEXTCTRL_H_
#include "gui/guiTextCtrl.h"
#endif

/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class GuiWindowCtrl : public GuiTextCtrl
{
   private:
      typedef GuiTextCtrl Parent;

      bool mResizeWidth;
      bool mResizeHeight;
      bool mCanMove;
      bool mCanClose;
      bool mCanMinimize;
      bool mCanMaximize;
      bool mPressClose;
      bool mPressMinimize;
      bool mPressMaximize;
      Point2I mMinSize;

      StringTableEntry mCloseCommand;

      S32 mTitleHeight;
      S32 mResizeRightWidth;
      S32 mResizeBottomHeight;

      bool mMouseMovingWin;
      bool mMouseResizeWidth;
      bool mMouseResizeHeight;
      bool mMinimized;
      bool mMaximized;

      Point2I mMouseDownPosition;
      RectI mOrigBounds;
      RectI mStandardBounds;

      RectI mCloseButton;
      RectI mMinimizeButton;
      RectI mMaximizeButton;
      S32 mMinimizeIndex;
      S32 mTabIndex;

      void PositionButtons(void);

   protected:
      enum BitmapIndices
      {
         BmpClose,
         BmpMaximize,
         BmpNormal,
         BmpMinimize,

         BmpCount
      };
      enum {
         BorderTopLeftKey = 12,
         BorderTopRightKey,
         BorderTopKey,
         BorderTopLeftNoKey,
         BorderTopRightNoKey,
         BorderTopNoKey,
         BorderLeft,
         BorderRight,
         BorderBottomLeft,
         BorderBottom,
         BorderBottomRight,
         NumBitmaps
      };

      enum BitmapStates
      {
         BmpDefault = 0,
         BmpHilite,
         BmpDisabled,

         BmpStates
      };
      RectI *mBitmapBounds;  //bmp is [3*n], bmpHL is [3*n + 1], bmpNA is [3*n + 2]
      TextureHandle mTextureHandle;


      void drawWinRect(const RectI &myRect);

   public:
      GuiWindowCtrl();
      DECLARE_CONOBJECT(GuiWindowCtrl);
      static void initPersistFields();

      bool onWake();
      void onSleep();

      bool isMinimized(S32 &index);

      virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);

      void setFont(S32 fntTag);

      GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);
      void resize(const Point2I &newPosition, const Point2I &newExtent);

      void onMouseDown(const GuiEvent &event);
      void onMouseDragged(const GuiEvent &event);
      void onMouseUp(const GuiEvent &event);

      //only cycle tabs through the current window, so overwrite the method
      GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
      GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

      bool onKeyDown(const GuiEvent &event);

      S32 getTabIndex(void) { return mTabIndex; }
      void selectWindow(void);

      void onRender(Point2I offset, const RectI &updateRect);
};
/// @}

#endif //_GUI_WINDOW_CTRL_H
