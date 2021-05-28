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

#ifndef _GUIEXPANDCTRL_H_
#define _GUIEXPANDCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif

#ifndef _MFLUID_H_
#include "math/mFluid.h"
#endif

#include "graphics/dgl.h"
#include "console/console.h"
#include "console/consoleTypes.h"

/// The Amazing Expand Control!
///
/// This class has a set size like any other GuiControl or it can expand to fit 
/// its contents. It can be collapsed again to the set size. Expanding/collapsing
/// is only done in the right/down directions. In other words, it only affects
/// the size of the control not the position.
///
class GuiExpandCtrl : public GuiControl, public Fluid
{
private:
   typedef GuiControl Parent;

   void setCollapsedExtent(const Point2I &extent);

protected:
	bool mExpanded;
	Point2I mCollapsedExtent;
	Point2I mExpandedExtent;

	virtual bool calcExpandedExtent();

public:
	GuiExpandCtrl();

   virtual void parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);

   inline bool getExpanded() { return mExpanded; };
   void setExpanded(bool isExpanded);

   static void initPersistFields();
   DECLARE_CONOBJECT(GuiExpandCtrl);

protected:
	virtual void processTick();
	bool processExpansion();
};

#endif