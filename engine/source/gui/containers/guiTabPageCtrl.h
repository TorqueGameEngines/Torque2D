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

#ifndef _GUITABPAGECTRL_H_
#define _GUITABPAGECTRL_H_

class GuiTabPageCtrl : public GuiControl
{
   private:
      typedef GuiControl Parent;

      S32            mTabIndex;

   public:
      GuiTabPageCtrl();
      DECLARE_CONOBJECT(GuiTabPageCtrl);
      static void initPersistFields();

      bool onWake();    ///< The page awakens (becomes active)!
      void onSleep();   ///< The page sleeps (zzzzZZ - becomes inactive)

      GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1); ///< Find which control is hit by the mouse starting at a specified layer

      void onTouchDown(const GuiEvent &event);  ///< Called when a mouseDown event occurs
      bool onMouseDownEditor(const GuiEvent &event, Point2I offset );   ///< Called when a mouseDown event occurs and the GUI editor is active

      S32 getTabIndex(void) { return mTabIndex; }  ///< Get the tab index of this control

      //only cycle tabs through the current window, so overwrite the method
      GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
      GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

      void selectWindow(void);               ///< Select this window

      virtual void setText(const char *txt = NULL); ///< Override setText function to signal parent we need to update.
	  void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUITABPAGECTRL_H_
