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

function EditorForm::onAdd(%this)
{
	ThemeManager.setProfile(%this, "emptyProfile");

	%this.setCellSpacing("0 0");
	%this.setCellModeX(Variable);
	%this.setCellModeY(Variable);
	%this.setMaxColCount(0);
}

function EditorForm::addFormItem(%this, %text, %size)
{
	%label = new GuiControl()
	{
		Extent = %size;
		Text = %text;
	};
	ThemeManager.setProfile(%label, "labelProfile");
	%this.add(%label);
	return %label;
}

function EditorForm::createTextEditItem(%this, %label)
{
	%textEdit = new GuiTextEditCtrl()
	{
		Position = "10 16";
		Extent = (getWord(%label.extent, 0) - 24) SPC 30;
	};
	ThemeManager.setProfile(%textEdit, "textEditProfile");
	%label.add(%textEdit);
	return %textEdit;
}

function EditorForm::createFileOpenItem(%this, %label, %filters, %dialogTitle)
{
	%textEdit = %this.createTextEditItem(%label);
	%teWidth = getWord(%label.extent, 0) - 110;
	%textEdit.setExtent(%teWidth, 30);

	%button = new GuiButtonCtrl()
	{
		Position = (%teWidth + 20) SPC 16;
		Extent = "76 30";
		Text = "Find";
		Command = %this.getID() @ ".getFilePath(\"" @ %filters @ "\", \"" @ %dialogTitle @ "\", " @ %textEdit.getID() @ ");";
	};
	ThemeManager.setProfile(%button, "buttonProfile");
	%label.add(%button);

	return %textEdit;
}

function EditorForm::getFilePath(%this, %filter, %title, %textEdit)
{
	%dialog = new OpenFileDialog()
	{
		Filters = %filter;
		ChangePath = false;
		MultipleFiles = false;
		DefaultFile = "";
		defaultPath = "./";
		title = %title;
	};
	%result = %dialog.execute();

	if ( %result )
	{
		%selectedFile = makeRelativePath(%dialog.fileName, getMainDotCsDir());
		%textEdit.setText(%selectedFile);
		%textEdit.setCursorPos(999999);//move it to the far right.
	}
	// Cleanup
	%dialog.delete();

	%this.postEvent("FileOpened", %textEdit);
}

function EditorForm::createFolderOpenItem(%this, %label, %dialogTitle)
{
	%textEdit = %this.createTextEditItem(%label);
	%teWidth = getWord(%label.extent, 0) - 110;
	%textEdit.setExtent(%teWidth, 30);

	%button = new GuiButtonCtrl()
	{
		Position = (%teWidth + 20) SPC 16;
		Extent = "76 30";
		Text = "Select";
		Command = %this.getID() @ ".getFolderPath(\"" @ %dialogTitle @ "\", " @ %textEdit.getID() @ ");";
	};
	ThemeManager.setProfile(%button, "buttonProfile");
	%label.add(%button);

	return %textEdit;
}

function EditorForm::getFolderPath(%this, %title, %textEdit)
{
	%dialog = new OpenFolderDialog()
	{
		Filters = "All Files|*.*";
		ChangePath = false;
		DefaultFile = "";
		defaultPath = "./";
		title = %title;
	};
	%result = %dialog.execute();

	if ( %result )
	{
		%selectedFile = makeRelativePath(%dialog.fileName, getMainDotCsDir());
		%textEdit.setText(%selectedFile);
		%textEdit.setCursorPos(999999);//move it to the far right.
	}
	// Cleanup
	%dialog.delete();

	%this.postEvent("FolderOpened", %textEdit);
}

function EditorForm::createDropDownItem(%this, %label)
{
	%dropDown = new GuiDropDownCtrl()
	{
		class = "EditorFormDropDown";
		Position = "10 17";
		Extent = (getWord(%label.extent, 0) - 24) SPC 28;
		ConstantThumbHeight=false;
		ScrollBarThickness=12;
		ShowArrowButtons=true;
		Form = %this;
	};
	ThemeManager.setProfile(%dropDown, "dropDownProfile");
	ThemeManager.setProfile(%dropDown, "dropDownItemProfile", "listBoxProfile");
	ThemeManager.setProfile(%dropDown, "emptyProfile", "backgroundProfile");
	ThemeManager.setProfile(%dropDown, "scrollingPanelProfile", "ScrollProfile");
	ThemeManager.setProfile(%dropDown, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%dropDown, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%dropDown, "scrollingPanelArrowProfile", "ArrowProfile");
	%label.add(%dropDown);
	return %dropDown;
}

function EditorForm::createCheckboxItem(%this, %label)
{
	%box = new GuiCheckBoxCtrl()
	{
		Position = "10 16";
		Extent = (getWord(%label.extent, 0) - 24) SPC 30;
		Text = %label.text;
	};
	ThemeManager.setProfile(%box, "checkboxProfile");
	%label.add(%box);
	%label.text = "";
	return %box;
}

function EditorFormDropDown::onClose(%this)
{
	%this.form.postEvent("DropDownClosed", %this);
}
