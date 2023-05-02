
function AssetImageLayersEditTool::onAdd(%this)
{
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

	%this.addNewLayerButton = new GuiButtonCtrl()
	{
		HorizSizing="left";
		VertSizing="bottom";
		Position="580 5";
		Extent= "110 26";
		Text = "Add Layer";
		command = %this.getID() @ ".addNewLayer();";
	};
	ThemeManager.setProfile(%this.addNewLayerButton, "buttonProfile");
	%this.add(%this.addNewLayerButton);
}

function AssetImageLayersEditTool::inspect(%this, %asset)
{
	%this.asset = %asset;

	%this.rowChain.deleteObjects();
	%this.addHeaderRow();
	%this.createRows();
}

function AssetImageLayersEditTool::createRows(%this)
{
	%this.addImageLayerRow(%this.asset.getRelativeImageFile(), "0 0", %this.asset.getBlendColor(), 0);

	%count = %this.asset.getLayerCount();
	for(%i = 1; %i <= %count; %i++)
	{
		%image = %this.asset.getLayerImage(%i);
		%position = %this.asset.getLayerPosition(%i);
		%color = %this.asset.getLayerBlendColor(%i);

		%this.addImageLayerRow(%image, %position, %color, %i);
	}
}

function AssetImageLayersEditTool::addHeaderRow(%this)
{
	%row = new GuiControl()
	{
		Class = "AssetImageLayersHeaderRow";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="560 22";
	};
	ThemeManager.setProfile(%row, "emptyProfile");
	%this.rowChain.add(%row);
}

function AssetImageLayersEditTool::addImageLayerRow(%this, %image, %position, %color, %index)
{
	%row = new GuiControl()
	{
		Class = "AssetImageLayersEditRow";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="662 40";
		LayerImage = %image;
		LayerPosition = %position;
		LayerColor = %color;
		LayerIndex = %index;
		LayerCount = %this.asset.getLayerCount();
	};
	ThemeManager.setProfile(%row, "emptyProfile");
	%this.rowChain.add(%row);
	%this.startListening(%row);
}

function AssetImageLayersEditTool::addNewLayer(%this)
{
	%index = %this.asset.getLayerCount();
	%image = %this.asset.getRelativeImageFile();

	%this.rowChain.callOnChildrenNoRecurse("updateLayerCount", %index + 1);

	%this.asset.addLayer(%image, "0 0", "1 1 1 1");
	%this.addImageLayerRow(%image, "0 0", "1 1 1 1", %index + 1);
}

function AssetImageLayersEditTool::onLayerImageChange(%this, %data)
{
	%row = getWord(%data, 0);
	%image = getWord(%data, 1);

	%this.asset.setLayerImage(%row.LayerIndex, %image);
	%row.LayerImage = %image;
}

function AssetImageLayersEditTool::onLayerPositionChange(%this, %data)
{
	%row = getWord(%data, 0);
	%x = getWord(%data, 1);
	%y = getWord(%data, 2);

	%this.asset.setLayerPosition(%row.LayerIndex, %x SPC %y);
	%row.LayerPosition = %x SPC %y;
}

function AssetImageLayersEditTool::onLayerColorChange(%this, %data)
{
	%row = getWord(%data, 0);
	%color = getWord(%data, 1) SPC getWord(%data, 2) SPC getWord(%data, 3) SPC getWord(%data, 4);

	if(%row.LayerIndex == 0)
	{
		%this.asset.setBlendColor(%color);
	}
	else
	{
		%this.asset.setLayerBlendColor(%row.LayerIndex, %color);
	}
	%row.LayerColor = %color;
}

function AssetImageLayersEditTool::onSwapLayers(%this, %data)
{
	%index1 = getWord(%data, 0);
	%index2 = getWord(%data, 1);

	%image1 = %this.asset.getLayerImage(%index1);
	%offset1 = %this.asset.getLayerPosition(%index1);
	%color1 = %this.asset.getLayerBlendColor(%index1);

	%image2 = %this.asset.getLayerImage(%index2);
	%offset2 = %this.asset.getLayerPosition(%index2);
	%color2 = %this.asset.getLayerBlendColor(%index2);

	%this.asset.moveLayerForward(%index1);

	%row1 = %this.rowChain.getObject(%index1 + 1);
	%row2 = %this.rowChain.getObject(%index2 + 1);

	%row1.LayerPosition = %offset2;
	%row1.LayerColor = %color2;
	%row1.LayerImage = %image2;

	%row2.LayerPosition = %offset1;
	%row2.LayerColor = %color1;
	%row2.LayerImage = %image1;

	%row1.refresh();
	%row2.refresh();
}

function AssetImageLayersEditTool::onRemoveLayer(%this, %index)
{
	%this.schedule(50, "onRemoveLayer2", %index);
}

function AssetImageLayersEditTool::onRemoveLayer2(%this, %index)
{
	%this.asset.removeLayer(%index);
	%row = %this.rowChain.getObject(%index + 1);
	%row.delete();

	%count = %this.asset.getLayerCount();
	for(%i = 0; %i < %this.rowChain.getCount(); %i++)
	{
		%row = %this.rowChain.getObject(%i);
		if(%row.isMethod("refresh"))
		{
			%row.LayerIndex = (%i - 1);
			%row.updateLayerCount(%count);
			%row.refresh();
		}
	}
}
