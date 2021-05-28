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

#include "gui/containers/guiPanelCtrl.h"
#include "graphics/dgl.h"
#include "guiPanelCtrl_ScriptBinding.h"
#include "gui/guiDefaultControlRender.h"

IMPLEMENT_CONOBJECT(GuiPanelCtrl);

GuiPanelCtrl::GuiPanelCtrl()
{
   mActive = true;
   mIsContainer = true;
	mHeader = NULL;
}

GuiPanelCtrl::~GuiPanelCtrl()
{
}

void GuiPanelCtrl::initPersistFields()
{
  Parent::initPersistFields();
}

bool GuiPanelCtrl::onAdd()
{
	if (!Parent::onAdd())
		return false;

	if (!mHeader)
	{
		mHeader = new GuiButtonCtrl();
		AssertFatal(mHeader, "GuiPaneCtrl::onAdd() Cannot create a GuiButtonCtrl for the header.");
		if (mHeader)
		{
			mHeader->setField("horizSizing", "width");
			mHeader->setField("vertSizing", "bottom");
			mHeader->setField("extent", "64 64");
			mHeader->setField("minExtent", "16 16");
			mHeader->setField("position", "0 0");
			
			mHeader->registerObject();
			addObject(mHeader);
			mHeader->setConsoleCommand(avar("%d.setExpanded(!%d.getExpanded());", getId(), getId()));
		}
	}

	return true;
}

void GuiPanelCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	if (mText != mHeader->getText())
	{
		mHeader->setText(mText);
	}
	if (mCollapsedExtent != mHeader->mBounds.extent)
	{
		mHeader->mBounds.extent.set(mCollapsedExtent.x, mCollapsedExtent.y);
	}
	if (mHeader->mProfile != mProfile)
	{
		mHeader->setControlProfile(mProfile);
	}

	//Render the childen
	RectI contentRect = RectI(offset, mBounds.extent);
	renderChildControls(offset, contentRect, updateRect);
}

void GuiPanelCtrl::setControlProfile(GuiControlProfile *prof)
{
	Parent::setControlProfile(prof);
	if(mHeader)
		mHeader->setControlProfile(prof);
}

bool GuiPanelCtrl::calcExpandedExtent()
{
	if (!size())
		return false;

	mExpandedExtent = Point2I(0, 0);
	for (iterator itr = begin(); itr != end(); ++itr)
	{
		GuiControl* child = dynamic_cast<GuiControl*>(*itr);
		mExpandedExtent.setMax(child->getExtent() + child->getPosition());
	}

	mExpandedExtent.set(getMax(mCollapsedExtent.x, mExpandedExtent.x), getMax(mCollapsedExtent.y, mExpandedExtent.y));

	return true;
}