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

#ifndef _GUISCROLLCTRL_H_
#define _GUISCROLLCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

class GuiScrollCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:

   // note - it is implicit in the scroll view that the buttons all have the same
   // arrow length and that horizontal and vertical scroll bars have the
   // same thickness

	S32 mScrollBarThickness;        // determined by the width of the vertical page bmp
	S32 mScrollBarDragTolerance;    // maximal distance from scrollbar at which a scrollbar drag is still valid

	bool mHBarEnabled;				//True if the children extent is greater than the content area.
	bool mVBarEnabled;
	bool mHasHScrollBar;			//The final word on if the bar should be shown. Adjusted internally.
	bool mHasVScrollBar;
	bool mShowArrowButtons;			//True if the arrow buttons should appear

	Point2I mChildExt;				//The furthest reaches of the child controls.
	Point2I mContentExt;			//The content area length and width in pixels.
	RectI mChildArea;				//The scrren space to which children were most recently rendered.

   //--------------------------------------
   // for mouse dragging the thumb
   Point2I mScrollOffsetAnchor; // the original scrollOffset when scrolling started
   S32 mThumbMouseDelta;

   S32 mLastUpdated;

   S32 mHThumbSize;
   S32 mHThumbPos;

   S32 mVThumbSize;
   S32 mVThumbPos;

   S32 mBaseThumbSize;

   RectI mUpArrowRect;
   RectI mDownArrowRect;
   RectI mLeftArrowRect;
   RectI mRightArrowRect;
   RectI mHTrackRect;
   RectI mVTrackRect;
 
public:      
   enum Region
   {
      UpArrow,
      DownArrow,
      LeftArrow,
      RightArrow,
      UpPage,
      DownPage,
      LeftPage,
      RightPage,
      VertThumb,
      HorizThumb,
      None
   };
   enum {
      ScrollBarAlwaysOn = 0,
      ScrollBarAlwaysOff = 1,
      ScrollBarDynamic = 2
   };

   bool mDepressed;
   Region curHitRegion;

   bool disabled;
   S32 mForceHScrollBar;
   S32 mForceVScrollBar;

   bool mUseConstantHeightThumb;
   Point2I mScrollOffset;			//The offset of the children

   Region findHitRegion(const Point2I &);

   GuiControlProfile *mThumbProfile; //Used to render the thumb and arrow buttons
   GuiControlProfile *mTrackProfile; //Used to render the tracks
   GuiControlProfile *mArrowProfile; //Used to render the arrow buttons

protected:

	virtual void calcContentExtents();
   virtual bool calcChildExtents();
   virtual void calcScrollRects(void);
   void calcThumbs();

   //--------------------------------------

   //--------------------------------------

public:
   GuiScrollCtrl();
   DECLARE_CONOBJECT(GuiScrollCtrl);
   void scrollByRegion(Region reg);
   static void initPersistFields();

   void scrollTo(S32 x, S32 y);
   void scrollDelta(S32 x, S32 y);
   void scrollRectVisible(RectI rect);

   void computeSizes();

   void addObject(SimObject *obj);
   void resize(const Point2I &newPosition, const Point2I &newExtent);
   void childResized(GuiControl *child);
   S32 scrollBarThickness() const                        { return(mScrollBarThickness); }
   bool hasHScrollBar() const                            { return(mHasHScrollBar); }
   bool hasVScrollBar() const                            { return(mHasVScrollBar); }
   bool enabledHScrollBar() const                        { return(mHBarEnabled); }
   bool enabledVScrollBar() const                        { return(mVBarEnabled); }

   bool isScrolledToBottom() { return mChildExt.y <= mScrollOffset.y + mContentExt.y; }

   Region getCurHitRegion(void) { return curHitRegion; }

   void onTouchMove(const GuiEvent &event);
   bool onKeyDown(const GuiEvent &event);
   void onTouchDown(const GuiEvent &event);
   void onTouchUp(const GuiEvent &event);
   void onTouchDragged(const GuiEvent &event);
   bool onMouseWheelUp(const GuiEvent &event);
   bool onMouseWheelDown(const GuiEvent &event);

   bool onWake();
   void onSleep();
   void setControlThumbProfile(GuiControlProfile* prof);
   void setControlTrackProfile(GuiControlProfile* prof);
   void setControlArrowProfile(GuiControlProfile* prof);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
   RectI applyScrollBarSpacing(Point2I offset, Point2I extent);
   GuiControlState getRegionCurrentState(GuiScrollCtrl::Region region);
   void renderBorderedRectWithArrow(RectI& bounds, GuiControlProfile* profile, GuiControlState state, GuiDirection direction);
   void renderVScrollBar(const Point2I& offset);
   void renderHScrollBar(const Point2I& offset);
   virtual GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);
   virtual void renderChildControls(Point2I offset, RectI content, const RectI& updateRect);
};

#endif //_GUI_SCROLL_CTRL_H
