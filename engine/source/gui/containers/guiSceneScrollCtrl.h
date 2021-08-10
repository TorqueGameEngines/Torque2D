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

#ifndef _GUISCENESCROLLCTRL_H_
#define _GUISCENESCROLLCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

#ifndef _SCENE_WINDOW_H_
#include "2d/gui/SceneWindow.h"
#endif

class GuiSceneScrollCtrl : public GuiScrollCtrl
{
private:
   typedef GuiScrollCtrl Parent;

public:
   GuiSceneScrollCtrl(SceneWindow* window);
   static void initPersistFields();
   virtual bool onWake();
   virtual void onSleep();

   virtual void scrollTo(S32 x, S32 y);
   virtual void scrollByRegion(Region reg);
   virtual void scrollRectVisible(RectI rect);
   virtual void scrollDelta(S32 x, S32 y);
   virtual void onTouchDown(const GuiEvent &event);
   virtual void onTouchDragged(const GuiEvent &event);

   virtual Point2I localToGlobalCoord(const Point2I &src);
   virtual Point2I globalToLocalCoord(const Point2I &src);

   void onRender(Point2I offset, const RectI &updateRect);
   
protected:
	SceneWindow* mWindow;

	virtual void calcContentExtents();
	virtual bool calcChildExtents();
	virtual void calcScrollRects(RectI &fillRect);
	virtual void calcScrollOffset();

	virtual void mouseLock(GuiControl *lockingControl);
	virtual void mouseLock();
	virtual void mouseUnlock();
	virtual bool isMouseLocked();
};

#endif //_GUISCENESCROLLCTRL_H_
