
function AssetImageFrameEditTool::onAdd(%this)
{
	%this.explicitModeCheckbox = new GuiCheckboxCtrl()
	{
		HorizSizing="right";
		VertSizing="bottom";
		Position="10 3";
		Extent= "285 30";
		Text = "Use Explicit Frame Mode";
		TextExtent = "280 30";
		command = %this.getID() @ ".toggleExplicitMode();";
	};
	ThemeManager.setProfile(%this.explicitModeCheckbox, "checkboxProfile");
	%this.add(%this.explicitModeCheckbox);

	%this.toolScroll = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 40";
		Extent= getWord(%this.extent, 0) SPC (getWord(%this.extent, 1) - 40);
		hScrollBar="dynamic";
		vScrollBar="dynamic";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.toolScroll, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.toolScroll, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.toolScroll, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.toolScroll, "scrollingPanelArrowProfile", "ArrowProfile");
	%this.add(%this.toolScroll);

	%this.rowChain = new GuiChainCtrl()
	{
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent= "662" SPC getWord(%this.toolScroll.extent, 1);
		IsVertical="1";
		ChildSpacing="2";
	};
	ThemeManager.setProfile(%this.rowChain, "emptyProfile");
	%this.toolScroll.add(%this.rowChain);

	%this.addNewCellButton = new GuiButtonCtrl()
	{
		HorizSizing="left";
		VertSizing="bottom";
		Position="580 5";
		Extent= "110 26";
		Text = "Add Cell";
		command = %this.getID() @ ".addNewCell();";
	};
	ThemeManager.setProfile(%this.addNewCellButton, "buttonProfile");
	%this.add(%this.addNewCellButton);
}

function AssetImageFrameEditTool::inspect(%this, %asset)
{
	%this.asset = %asset;
	%this.explicitModeCheckbox.setStateOn(%asset.ExplicitMode);

	%this.rowChain.deleteObjects();
	%this.addHeaderRow();

	%this.toggleExplicitMode();
}

function AssetImageFrameEditTool::createRows(%this)
{
	%cellCount = %this.asset.getExplicitCellCount();
	for(%i = 0; %i < %cellCount; %i++)
	{
		%name = %this.asset.getExplicitCellName(%i);
		%width = %this.asset.getExplicitCellWidth(%i);
		%height = %this.asset.getExplicitCellHeight(%i);
		%offset = %this.asset.getExplicitCellOffset(%i);

		%this.addImageFrameRow(%name, %offset, %width, %height, %i);
	}
}

function AssetImageFrameEditTool::toggleExplicitMode(%this)
{
	%explicitModeOn = %this.explicitModeCheckbox.getStateOn();
	%this.toolScroll.setVisible(%explicitModeOn);
	%this.addNewCellButton.setVisible(%explicitModeOn);
	%this.asset.setExplicitMode(%explicitModeOn);

	if(%explicitModeOn && %this.asset.getExplicitCellCount() == 0)
	{
		%this.addNewCell();
	}
	else if(%explicitModeOn)
	{
		%this.createRows();
	}
	else
	{
		%this.rowChain.deleteObjects();
		%this.addHeaderRow();
	}
}

function AssetImageFrameEditTool::addHeaderRow(%this)
{
	%row = new GuiControl()
	{
		Class = "AssetImageFrameHeaderRow";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="560 22";
	};
	ThemeManager.setProfile(%row, "emptyProfile");
	%this.rowChain.add(%row);
}

function AssetImageFrameEditTool::addImageFrameRow(%this, %name, %offset, %width, %height, %index)
{
	%row = new GuiControl()
	{
		Class = "AssetImageFrameEditRow";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="662 40";
		CellName = %name;
		CellOffset = %offset;
		CellWidth = %width;
		CellHeight = %height;
		CellIndex = %index;
		ImageWidth = %this.asset.getImageWidth();
		ImageHeight = %this.asset.getImageHeight();
		CellCount = %this.asset.getExplicitCellCount();
	};
	ThemeManager.setProfile(%row, "emptyProfile");
	%this.rowChain.add(%row);
	%this.startListening(%row);
}

function AssetImageFrameEditTool::addNewCell(%this)
{
	%index = %this.asset.getExplicitCellCount();
	%name = "Frame" @ %index;
	%x = 0;
	%y = 0;
	%width = %this.asset.getImageWidth();
	%height = %this.asset.getImageHeight();

	%this.rowChain.callOnChildrenNoRecurse("updateCellCount", %index + 1);

	%this.asset.addExplicitCell(%x, %y, %width, %height, %name);
	%this.addImageFrameRow(%name, %x SPC %y, %width, %height, %index);
}

function AssetImageFrameEditTool::onCellNameChange(%this, %data)
{
	%row = getWord(%data, 0);
	%name = getWord(%data, 1);

	if(%this.asset.getExplicitCellIndex(%name) != -1)
	{
		%row.setNameError(true);
	}
	else
	{
		%this.asset.setExplicitCell(%row.CellIndex, getWord(%row.CellOffset, 0), getWord(%row.CellOffset, 1), %row.CellWidth, %row.CellHeight, %name);
		%row.CellName = %name;
	}
}

function AssetImageFrameEditTool::onCellSizeChange(%this, %data)
{
	%row = getWord(%data, 0);
	%x = getWord(%data, 1);
	%y = getWord(%data, 2);
	%width = getWord(%data, 3);
	%height = getWord(%data, 4);

	%this.asset.setExplicitCell(%row.CellIndex, %x, %y, %width, %height, %row.CellName);
	%row.CellOffset = %x SPC %y;
	%row.CellWidth = %width;
	%row.CellHeight = %height;
}

function AssetImageFrameEditTool::onSwapCells(%this, %data)
{
	%index1 = getWord(%data, 0);
	%index2 = getWord(%data, 1);

	%name1 = %this.asset.getExplicitCellName(%index1);
	%width1 = %this.asset.getExplicitCellWidth(%index1);
	%height1 = %this.asset.getExplicitCellHeight(%index1);
	%offset1 = %this.asset.getExplicitCellOffset(%index1);

	%name2 = %this.asset.getExplicitCellName(%index2);
	%width2 = %this.asset.getExplicitCellWidth(%index2);
	%height2 = %this.asset.getExplicitCellHeight(%index2);
	%offset2 = %this.asset.getExplicitCellOffset(%index2);

	%this.asset.setExplicitCell(%index1, getWord(%offset2, 0), getWord(%offset2, 1), %width2, %height2, %name2);
	%this.asset.setExplicitCell(%index2, getWord(%offset1, 0), getWord(%offset1, 1), %width1, %height1, %name1);

	%row1 = %this.rowChain.getObject(%index1 + 1);
	%row2 = %this.rowChain.getObject(%index2 + 1);

	%row1.CellOffset = %offset2;
	%row1.CellWidth = %width2;
	%row1.CellHeight = %height2;
	%row1.CellName = %name2;

	%row2.CellOffset = %offset1;
	%row2.CellWidth = %width1;
	%row2.CellHeight = %height1;
	%row2.CellName = %name1;

	%row1.refresh();
	%row2.refresh();
}

function AssetImageFrameEditTool::onRemoveCell(%this, %index)
{
	%this.schedule(50, "onRemoveCell2", %index);
}

function AssetImageFrameEditTool::onRemoveCell2(%this, %index)
{
	%this.asset.removeExplicitCell(%index);
	%row = %this.rowChain.getObject(%index + 1);
	%row.delete();

	%count = %this.asset.getExplicitCellCount();
	for(%i = 0; %i < %this.rowChain.getCount(); %i++)
	{
		%row = %this.rowChain.getObject(%i);
		if(%row.isMethod("refresh"))
		{
			%row.CellIndex = (%i - 1);
			%row.updateCellCount(%count);
			%row.refresh();
		}
	}
}
