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

function EditorDialog::onAdd(%this)
{
	ThemeManager.setProfile(%this, "overlayProfile");

	%this.window = new GuiWindowCtrl()
	{
		class = "EditorDialogWindow";
		HorizSizing = "center";
		VertSizing = "center";
		Position = "0 0";
		Extent = %this.dialogSize;
		Text = %this.dialogText;
		canClose = %this.dialogCanClose;
		canMove = true;
		CanMinimize = false;
		CanMaximize = false;
		titleHeight = 30;
		dialog = %this;
	};
	ThemeManager.setProfile(%this.window, "windowProfile");
	ThemeManager.setProfile(%this.window, "windowContentProfile", "contentProfile");
	ThemeManager.setProfile(%this.window, "windowButtonProfile", "MinButtonProfile");
	ThemeManager.setProfile(%this.window, "windowButtonProfile", "MaxButtonProfile");
	ThemeManager.setProfile(%this.window, "windowButtonProfile", "CloseButtonProfile");
	%this.add(%this.window);

	%this.content = new GuiScrollCtrl()
	{
		HorizSizing = "width";
		VertSizing = "height";
		Position = "0 0";
		Extent = (getWord(%this.dialogSize, 0) - 8) SPC (getWord(%this.dialogSize, 1) - 34);//we assume a 4 pixel border
		hScrollBar = "dynamic";
		vScrollBar = "dynamic";
		constantThumbHeight="0";
		showArrowButtons = 0;
		scrollBarThickness = 16;
	};
	ThemeManager.setProfile(%this.content, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.content, "scrollingPanelThumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.content, "scrollingPanelTrackProfile", TrackProfile);
	ThemeManager.setProfile(%this.content, "scrollingPanelArrowProfile", ArrowProfile);
	%this.window.add(%this.content);
}

function EditorDialogWindow::onClose(%this)
{
	%this.dialog.call("onClose");
}
