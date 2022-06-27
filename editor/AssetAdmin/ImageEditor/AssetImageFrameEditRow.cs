
function AssetImageFrameEditRow::onAdd(%this)
{
	%this.errorColor = "255 0 0 255";
	%this.indexBox = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="20 40";
		Align = center;
		vAlign = middle;
		Text = %this.CellIndex;
		FontSizeAdjust = 1.4;
		FontColor = %this.errorColor;
	};
	ThemeManager.setProfile(%this.indexBox, "codeProfile");
	%this.add(%this.indexBox);

	%this.nameBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="20 3";
		Extent="200 32";
		Text = %this.CellName;
		AltCommand = %this.getID() @ ".CellNameChange();";
		FontColor = %this.errorColor;
		InputMode = "AlphaNumeric";
	};
	ThemeManager.setProfile(%this.nameBox, "textEditProfile");
	%this.add(%this.nameBox);

	%this.offsetXBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="224 3";
		Extent="80 32";
		Align = right;
		Text = getWord(%this.CellOffset, 0);
		AltCommand = %this.getID() @ ".CellOffsetXChange();";
		FontColor = %this.errorColor;
		InputMode = "Number";
	};
	ThemeManager.setProfile(%this.offsetXBox, "textEditProfile");
	%this.add(%this.offsetXBox);

	%this.offsetYBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="308 3";
		Extent="80 32";
		Align = right;
		Text = getWord(%this.CellOffset, 1);
		AltCommand = %this.getID() @ ".CellOffsetYChange();";
		FontColor = %this.errorColor;
		InputMode = "Number";
	};
	ThemeManager.setProfile(%this.offsetYBox, "textEditProfile");
	%this.add(%this.offsetYBox);

	%this.widthBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="392 3";
		Extent="80 32";
		Align = right;
		Text = %this.CellWidth;
		AltCommand = %this.getID() @ ".CellWidthChange();";
		FontColor = %this.errorColor;
		InputMode = "Number";
	};
	ThemeManager.setProfile(%this.widthBox, "textEditProfile");
	%this.add(%this.widthBox);

	%this.heightBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="476 3";
		Extent="80 32";
		Align = right;
		Text = %this.CellHeight;
		AltCommand = %this.getID() @ ".CellHeightChange();";
		FontColor = %this.errorColor;
		InputMode = "Number";
	};
	ThemeManager.setProfile(%this.heightBox, "textEditProfile");
	%this.add(%this.heightBox);

	%this.buttonBar = new GuiChainCtrl()
	{
		Class = "EditorButtonBar";
		Position = "564 5";
		Extent = "0 24";
		ChildSpacing = 4;
		IsVertical = false;
		Tool = %this;
	};
	ThemeManager.setProfile(%this.buttonBar, "emptyProfile");
	%this.add(%this.buttonBar);
	%this.buttonBar.addButton("MoveCellUp", 2, "Move Cell Up", "getMoveCellUpEnabled");
	%this.buttonBar.addButton("MoveCellDown", 6, "Move Cell Down", "getMoveCellDownEnabled");
	%this.buttonBar.addButton("RemoveCell", 23, "Remove Cell", "getRemoveCellEnabled");
}

function AssetImageFrameEditRow::CellNameChange(%this)
{
	%name = %this.nameBox.getText();
	%name = stripChars(%name, " ");
	%this.nameBox.setText(%name);
	if(%name !$= %this.CellName)
	{
		if(%name $= "")
		{
			%this.setNameError(true);
		}
		else
		{
			%this.setNameError(false);
			%this.postEvent("CellNameChange", %this SPC %name);
		}
	}
}

function AssetImageFrameEditRow::CellOffsetXChange(%this)
{
	%x = %this.offsetXBox.getText();
	%x = stripChars(%x, " ");
	if(%x $= "")
	{
		%x = 0;
	}
	%x = mClamp(%x, 0, %this.ImageWidth - 1);
	%this.offsetXBox.setText(%x);

	if(%x !$= getWord(%this.CellOffset, 0))
	{
		%width = %this.CellWidth;
		if((%x + %width) > %this.ImageWidth)
		{
			%width = %this.ImageWidth - %x;
			%this.widthBox.setText(%width);
		}
		%this.postEvent("CellSizeChange", %this SPC %x SPC getWord(%this.CellOffset, 1) SPC %width SPC %this.CellHeight);
	}
}

function AssetImageFrameEditRow::CellOffsetYChange(%this)
{
	%y = %this.offsetYBox.getText();
	%y = stripChars(%y, " ");
	if(%y $= "")
	{
		%y = 0;
	}
	%y = mClamp(%y, 0, %this.ImageHeight - 1);
	%this.offsetYBox.setText(%y);

	if(%y !$= getWord(%this.CellOffset, 1))
	{
		%height = %this.CellHeight;
		if((%y + %height) > %this.ImageHeight)
		{
			%height = %this.ImageHeight - %y;
			%this.heightBox.setText(%height);
		}
		%this.postEvent("CellSizeChange", %this SPC getWord(%this.CellOffset, 0) SPC %y SPC %this.CellWidth SPC %height);
	}
}

function AssetImageFrameEditRow::CellWidthChange(%this)
{
	%width = %this.widthBox.getText();
	%width = stripChars(%width, " ");
	if(%width $= "")
	{
		%width = 1;
	}
	%width = mClamp(%width, 1, %this.ImageWidth);
	%this.widthBox.setText(%width);

	if(%width !$= %this.CellWidth)
	{
		%x = getWord(%this.CellOffset, 0);
		if((%x + %width) > %this.ImageWidth)
		{
			%x = %this.ImageWidth - %width;
			%this.offsetXBox.setText(%x);
		}
		%this.postEvent("CellSizeChange", %this SPC %x SPC getWord(%this.CellOffset, 1) SPC %width SPC %this.CellHeight);
	}
}

function AssetImageFrameEditRow::CellHeightChange(%this)
{
	%height = %this.heightBox.getText();
	%height = stripChars(%height, " ");
	if(%height $= "")
	{
		%height = 1;
	}
	%height = mClamp(%height, 1, %this.ImageHeight);
	%this.heightBox.setText(%height);

	if(%height !$= %this.CellHeight)
	{
		%y = getWord(%this.CellOffset, 1);
		if((%y + %height) > %this.ImageHeight)
		{
			%y = %this.ImageHeight - %height;
			%this.offsetYBox.setText(%y);
		}
		%this.postEvent("CellSizeChange", %this SPC getWord(%this.CellOffset, 0) SPC %y SPC %this.CellWidth SPC %height);
	}
}

function AssetImageFrameEditRow::setNameError(%this, %hasError)
{
	%this.nameBox.overrideFontColor = %hasError;
	%this.indexBox.overrideFontColor = %hasError;
}

function AssetImageFrameEditRow::getMoveCellUpEnabled(%this)
{
	return %this.CellIndex != 0;
}

function AssetImageFrameEditRow::getMoveCellDownEnabled(%this)
{
	return %this.CellIndex < (%this.CellCount - 1);
}

function AssetImageFrameEditRow::getRemoveCellEnabled(%this)
{
	return %this.CellCount > 1;
}

function AssetImageFrameEditRow::updateCellCount(%this, %newCount)
{
	%this.CellCount = %newCount;
	%this.buttonBar.refreshEnabled();
}

function AssetImageFrameEditRow::MoveCellUp(%this)
{
	%this.postEvent("swapCells", (%this.CellIndex - 1) SPC %this.CellIndex);
}

function AssetImageFrameEditRow::MoveCellDown(%this)
{
	%this.postEvent("swapCells", %this.CellIndex SPC (%this.CellIndex + 1));
}

function AssetImageFrameEditRow::RemoveCell(%this)
{
	%this.postEvent("removeCell", %this.CellIndex);
}

function AssetImageFrameEditRow::refresh(%this)
{
	%this.indexBox.setText(%this.CellIndex);
	%this.nameBox.setText(%this.CellName);
	%this.offsetXBox.setText(getWord(%this.CellOffset, 0));
	%this.offsetYBox.setText(getWord(%this.CellOffset, 1));
	%this.widthBox.setText(%this.CellWidth);
	%this.heightBox.setText(%this.CellHeight);
}

function AssetImageFrameEditRow::onRemove(%this)
{
	%this.deleteObjects();
}
