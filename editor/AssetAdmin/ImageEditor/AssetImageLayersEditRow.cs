
function AssetImageLayersEditRow::onAdd(%this)
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
		Text = %this.LayerIndex;
		FontSizeAdjust = 1.4;
		FontColor = %this.errorColor;
	};
	ThemeManager.setProfile(%this.indexBox, "codeProfile");
	%this.add(%this.indexBox);

	%this.imageBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="20 3";
		Extent="200 32";
		Text = %this.LayerImage;
		AltCommand = %this.getID() @ ".LayerImageChange();";
		FontColor = %this.errorColor;
		InputMode = "AllText";
	};
	ThemeManager.setProfile(%this.imageBox, "textEditProfile");
	%this.add(%this.imageBox);

	%this.offsetXBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="224 3";
		Extent="80 32";
		Align = right;
		Text = getWord(%this.LayerPosition, 0);
		AltCommand = %this.getID() @ ".LayerPositionXChange();";
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
		Text = getWord(%this.LayerPosition, 1);
		AltCommand = %this.getID() @ ".LayerPositionYChange();";
		FontColor = %this.errorColor;
		InputMode = "Number";
	};
	ThemeManager.setProfile(%this.offsetYBox, "textEditProfile");
	%this.add(%this.offsetYBox);

	%this.LayerColor = %this.scrubColor(%this.LayerColor);
	%this.colorBox = new GuiTextEditCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="392 3";
		Extent="164 32";
		Align = right;
		Text = %this.LayerColor;
		AltCommand = %this.getID() @ ".LayerColorChange();";
		FontColor = %this.errorColor;
		InputMode = "AllText";
	};
	ThemeManager.setProfile(%this.colorBox, "textEditProfile");
	%this.add(%this.colorBox);

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

	if(%this.LayerIndex > 0)
	{
		%this.buttonBar.addButton("MoveLayerUp", 2, "Move Layer Up", "getMoveLayerUpEnabled");
		%this.buttonBar.addButton("MoveLayerDown", 6, "Move Layer Down", "getMoveLayerDownEnabled");
		%this.buttonBar.addButton("RemoveLayer", 23, "Remove Layer", "");
	}
	else
	{
		%this.imageBox.active = false;
		%this.offsetXBox.active = false;
		%this.offsetYBox.active = false;
	}
}

function AssetImageLayersEditRow::LayerImageChange(%this)
{
	%name = %this.imageBox.getText();
	%name = stripChars(%name, " ");
	%this.imageBox.setText(%name);
	if(%name !$= %this.LayerImage)
	{
		if(%name $= "")
		{
			%this.setNameError(true);
		}
		else
		{
			%this.setNameError(false);
			%this.postEvent("LayerImageChange", %this SPC %name);
		}
	}
}

function AssetImageLayersEditRow::LayerPositionXChange(%this)
{
	%x = %this.offsetXBox.getText();
	%x = stripChars(%x, " ");
	if(%x $= "")
	{
		%x = 0;
	}
	%this.offsetXBox.setText(%x);

	if(%x !$= getWord(%this.LayerPosition, 0))
	{
		%this.postEvent("LayerPositionChange", %this SPC %x SPC getWord(%this.LayerPosition, 1));
	}
}

function AssetImageLayersEditRow::LayerPositionYChange(%this)
{
	%y = %this.offsetYBox.getText();
	%y = stripChars(%y, " ");
	if(%y $= "")
	{
		%y = 0;
	}
	%this.offsetYBox.setText(%y);

	if(%y !$= getWord(%this.CellOffset, 1))
	{
		%this.postEvent("LayerPositionChange", %this SPC getWord(%this.LayerPosition, 0) SPC %y);
	}
}

function AssetImageLayersEditRow::LayerColorChange(%this)
{
	%color = %this.scrubColor(%this.colorBox.getText());
	%this.colorBox.setText(%color);

	if(%color !$= %this.LayerColor)
	{
		%this.postEvent("LayerColorChange", %this SPC %color);
	}
}

function AssetImageLayersEditRow::setNameError(%this, %hasError)
{
	%this.imageBox.overrideFontColor = %hasError;
	%this.indexBox.overrideFontColor = %hasError;
}

function AssetImageLayersEditRow::getMoveLayerUpEnabled(%this)
{
	return %this.LayerIndex != 1;
}

function AssetImageLayersEditRow::getMoveLayerDownEnabled(%this)
{
	return %this.LayerIndex != %this.LayerCount;
}

function AssetImageLayersEditRow::getRemoveCellEnabled(%this)
{
	return true;
}

function AssetImageLayersEditRow::updateLayerCount(%this, %newCount)
{
	%this.LayerCount = %newCount;
	%this.buttonBar.refreshEnabled();
}

function AssetImageLayersEditRow::MoveLayerUp(%this)
{
	%this.postEvent("swapLayers", (%this.LayerIndex - 1) SPC %this.LayerIndex);
}

function AssetImageLayersEditRow::MoveLayerDown(%this)
{
	%this.postEvent("swapLayers", %this.LayerIndex SPC (%this.LayerIndex + 1));
}

function AssetImageLayersEditRow::RemoveLayer(%this)
{
	%this.postEvent("removeLayer", %this.LayerIndex);
}

function AssetImageLayersEditRow::refresh(%this)
{
	%this.indexBox.setText(%this.LayerIndex);
	%this.imageBox.setText(%this.LayerImage);
	%this.offsetXBox.setText(getWord(%this.LayerPosition, 0));
	%this.offsetYBox.setText(getWord(%this.LayerPosition, 1));
	%this.colorBox.setText(%this.LayerColor);
}

function AssetImageLayersEditRow::onRemove(%this)
{
	%this.deleteObjects();
}

function AssetImageLayersEditRow::scrubColor(%this, %color)
{
	%red = %this.scrubChannel(getWord(%color, 0));
	%green = %this.scrubChannel(getWord(%color, 1));
	%blue = %this.scrubChannel(getWord(%color, 2));
	%alpha = %this.scrubChannel(getWord(%color, 3));

	return %red SPC %green SPC %blue SPC %alpha;
}

function AssetImageLayersEditRow::scrubChannel(%this, %val)
{
	%val = mFloatLength(mClamp(%val, 0, 1), 3);
	if(getSubStr(%val, 4, 1) != "0")
	{
		return %val;
	}
	%val = getSubStr(%val, 0, 4);

	if(getSubStr(%val, 3, 1) != "0")
	{
		return %val;
	}
	%val = getSubStr(%val, 0, 3);

	if(getSubStr(%val, 2, 1) != "0")
	{
		return %val;
	}
	return getSubStr(%val, 0, 1);
}
