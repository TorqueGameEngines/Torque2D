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
   bool mEventBubbled;
   bool mCalcGuard;
   bool mResizeGuard;
   bool mNotifyGuard;

protected:

   // note - it is implicit in the scroll view that the buttons all have the same
   // arrow length and that horizontal and vertical scroll bars have the
   // same thickness

	S32 mScrollBarDragTolerance;    // maximal distance from scrollbar at which a scrollbar drag is still valid

	bool mHBarEnabled;				//True if the children extent is greater than the content area.
	bool mVBarEnabled;
	bool mHasHScrollBar;			//The final word on if the bar should be shown. Adjusted internally.
	bool mHasVScrollBar;

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
      Content
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
   S32 mScrollBarThickness;        // determined by the width of the vertical page bmp
   bool mShowArrowButtons;			//True if the arrow buttons should appear

   Region findHitRegion(const Point2I &);

   GuiControlProfile *mThumbProfile; //Used to render the thumb and arrow buttons
   GuiControlProfile *mTrackProfile; //Used to render the tracks
   GuiControlProfile *mArrowProfile; //Used to render the arrow buttons

protected:

	virtual void calcContentExtents();
   virtual bool calcChildExtents();
   virtual void calcScrollRects(RectI &fillRect);
   virtual void calcThumbs();
   virtual void calcScrollOffset();

   //--------------------------------------

   //--------------------------------------

public:
   GuiScrollCtrl();
   DECLARE_CONOBJECT(GuiScrollCtrl);
   
   static void initPersistFields();

   virtual void scrollTo(S32 x, S32 y);
   virtual void scrollDelta(S32 x, S32 y);
   virtual void scrollRectVisible(RectI rect);
   virtual void scrollByRegion(Region reg);

   virtual void computeSizes();

   /// @name The size a child is given
   ///
   /// A scroll control is the one container that does not always have a size to
   /// offer a child. In an axis it can scroll, the content is as long as it
   /// wants to be and the control merely shows a window onto it -- there is no
   /// fixed size to hand down. In an axis whose bar is alwaysOff there is no
   /// scrolling, so the space IS bounded, and a child may be sized to it.
   ///
   /// These say which case an axis is in, and what is left over once whatever
   /// bars are showing have taken their share.
   /// @{

   /// True when this axis scrolls, and so has no fixed size to offer a child.
   bool canScrollHorizontally() const { return mForceHScrollBar != ScrollBarAlwaysOff; }
   bool canScrollVertically() const   { return mForceVScrollBar != ScrollBarAlwaysOff; }

   /// What is left of an extent once the showing bars have taken their space.
   /// Pure, and the single definition of that arithmetic: the visible rect, the
   /// rect children are rendered into and the rect fill resolves against are all
   /// this same subtraction, and used to be three separate ones.
   static Point2I subtractScrollBars(const Point2I &extent, const bool hasHBar, const bool hasVBar, const S32 barThickness);

   /// Which bars a scroller of this configuration shows.
   ///
   /// Pure, so that the one genuinely circular part of the layout is testable on
   /// its own: a vertical bar narrows the content, which can be what pushes the
   /// content wide enough to need a horizontal one.
   static void calcBarPresence(const S32 forceHBar, const S32 forceVBar, const Point2I &childExtent,
      const Point2I &contentExtent, const S32 barThickness, bool &outHasHBar, bool &outHasVBar);

   /// The visible content rect: the inner rect less the bars that are showing.
   virtual RectI getInnerRect(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile);

   /// Strips the sizing modes a child may not use in an axis that scrolls.
   void preventUnsizedModes(GuiControl *child);

   /// Tells the children that the bars took, or gave back, their share.
   ///
   /// Only the bars' share: an outer resize has already been passed down by
   /// GuiControl::resize, and counting it twice would move every child that
   /// sizes on a delta.
   void notifyChildrenOfBarChange(const Point2I &barFreeExtent, const bool hadHBar, const bool hadVBar);
   /// @}

   virtual void addObject(SimObject *obj);
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);
   virtual void childResized(GuiControl *child);
   virtual S32 scrollBarThickness() const                        { return(mScrollBarThickness); }
   virtual bool hasHScrollBar() const                            { return(mHasHScrollBar); }
   virtual bool hasVScrollBar() const                            { return(mHasVScrollBar); }
   virtual bool enabledHScrollBar() const                        { return(mHBarEnabled); }
   virtual bool enabledVScrollBar() const                        { return(mVBarEnabled); }

   virtual bool isScrolledToBottom() { return mChildExt.y <= mScrollOffset.y + mContentExt.y; }

   virtual Region getCurHitRegion(void) { return curHitRegion; }

   virtual void onTouchMove(const GuiEvent &event);
   virtual bool onKeyDown(const GuiEvent &event);
   virtual void onTouchDown(const GuiEvent &event);
   virtual void onTouchUp(const GuiEvent &event);
   virtual void onTouchDragged(const GuiEvent &event);
   virtual void onTouchLeave(const GuiEvent &event);
   virtual void onMouseWheelUp(const GuiEvent &event);
   virtual void onMouseWheelDown(const GuiEvent &event);
   virtual bool onMouseDownEditor(const GuiEvent& event, const Point2I& offset);
   virtual bool onMouseUpEditor(const GuiEvent& event, const Point2I& offset);
   virtual bool onMouseDraggedEditor(const GuiEvent& event, const Point2I& offset);

   virtual bool onWake();
   virtual void onSleep();
   virtual void inspectPostApply();
   /// Redoes the layout when a field it is worked out from is written.
   virtual void onStaticModified(const char* slotName, const char* newValue = NULL);
   virtual void setControlThumbProfile(GuiControlProfile* prof);
   virtual void setControlTrackProfile(GuiControlProfile* prof);
   virtual void setControlArrowProfile(GuiControlProfile* prof);

   virtual void onPreRender();
   virtual void onRender(Point2I offset, const RectI &updateRect);
   virtual RectI applyScrollBarSpacing(Point2I offset, Point2I extent);
   virtual GuiControlState getRegionCurrentState(GuiScrollCtrl::Region region);
   virtual void renderBorderedRectWithArrow(RectI& bounds, GuiControlProfile* profile, GuiControlState state, GuiDirection direction);
   virtual void renderVScrollBar(const Point2I& offset);
   virtual void renderHScrollBar(const Point2I& offset);
   virtual GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);
   virtual void renderChildControls(const Point2I& offset, const RectI& content, const RectI& updateRect);
};

#endif //_GUI_SCROLL_CTRL_H
