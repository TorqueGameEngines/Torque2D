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

/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class GuiWindowCtrl : public GuiControl
{
   private:
      typedef GuiControl Parent;

	  //Allow these behaviors.
      bool mResizeWidth;
      bool mResizeHeight;
      bool mCanMove;
      bool mCanClose;
      bool mCanMinimize;
      bool mCanMaximize;

	  //Did the touch down happen inside these buttons?
      bool mPressClose;
      bool mPressMinimize;
      bool mPressMaximize;

	  //Window settings.
      S32 mTitleHeight;					//The height of the title bar before appling margin/border/padding.
      S32 mResizeRightWidth;			//The size of the area inset from the right edge that can be clicked to resize horizontally.
      S32 mResizeBottomHeight;			//The size of the area inset from the bottom edge that can be clicked to resize vertically.

	  //Current State
      bool mMouseMovingWin;
      bool mMouseResizeWidth;
      bool mMouseResizeHeight;
      bool mMinimized;
      bool mMaximized;

      Point2I mMouseDownPosition;
      RectI mOrigBounds;
      RectI mStandardBounds;

	  //The location of the window parts.
      RectI mCloseButton;
      RectI mMinimizeButton;
      RectI mMaximizeButton;
	  RectI mTitleBar;

      S32 mMinimizeIndex;
      S32 mTabIndex;

	  //Additional profiles used by the window.
	  GuiControlProfile *mContentProfile; //Used to render the content section of the window.
	  GuiControlProfile *mCloseButtonProfile; //Used to render the close button.
	  GuiControlProfile *mMinButtonProfile; //Used to render the close button.
	  GuiControlProfile *mMaxButtonProfile; //Used to render the close button.

	  // Sizing Cursors
	  GuiCursor*        mLeftRightCursor;
	  GuiCursor*        mUpDownCursor;
	  GuiCursor*        mNWSECursor;

	  void ResizeComplete();
	  void MoveComplete();

   public:
	   enum Region
	   {
		   TitleBar,
		   CloseButton,
		   MinButton,
		   MaxButton,
		   None
	   };

	   enum Icon
	   {
		   Close,
		   Min,
		   Max
	   };

	   bool mDepressed;
	   Region curHitRegion;

	   Region findHitRegion(const Point2I &);
	   GuiControlState getRegionCurrentState(GuiWindowCtrl::Region region);

      GuiWindowCtrl();
      DECLARE_CONOBJECT(GuiWindowCtrl);
      static void initPersistFields();

      bool onWake();
      void onSleep();

      bool isMinimized(S32 &index);

      virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);

      GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);
      void resize(const Point2I &newPosition, const Point2I &newExtent);

	  void onTouchMove(const GuiEvent &event);
      void onTouchDown(const GuiEvent &event);
      void onTouchDragged(const GuiEvent &event);
      void onTouchUp(const GuiEvent &event);
	  void onTouchLeave(const GuiEvent &event);

	  virtual void onFocus();

      //only cycle tabs through the current window, so overwrite the method
      GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
      GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

      bool onKeyDown(const GuiEvent &event);

      S32 getTabIndex(void) { return mTabIndex; }
      void selectWindow(void);

      void onRender(Point2I offset, const RectI &updateRect);
	  RectI renderButtons(const Point2I &offset, const RectI &contentRect);
	  RectI renderButton(const RectI &contentRect, S32 distanceFromEdge, GuiControlState buttonState, GuiControlProfile *profile, Icon defaultIcon);

	  void setControlContentProfile(GuiControlProfile* prof);
	  void setControlCloseButtonProfile(GuiControlProfile* prof);
	  void setControlMinButtonProfile(GuiControlProfile* prof);
	  void setControlMaxButtonProfile(GuiControlProfile* prof);

	  void setControlLeftRightCursor(GuiCursor* cursor);
	  void setControlUpDownCursor(GuiCursor* cursor);
	  void setControlNWSECursor(GuiCursor* cursor);
};
/// @}

#endif //_GUI_WINDOW_CTRL_H
