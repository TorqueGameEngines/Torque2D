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

function AssetParticleGraphTool::onAdd(%this)
{
	%this.init();
	%this.addItem("Lifetime Scale");
	%this.addItem("Quantity Scale");
	%this.addItem("SizeX Scale");
	%this.addItem("SizeY Scale");
	%this.addItem("Speed Scale");
	%this.addItem("Spin Scale");
	%this.addItem("Fixed Force Scale");
	%this.addItem("Random Motion Scale");
	%this.addItem("Alpha Channel Scale");
}

function AssetParticleGraphEmitterTool::onAdd(%this)
{
	%this.init();
	%this.initEmitter();
	%this.addItem("Lifetime");
	%this.addItem("Quantity");
	%this.addItem("SizeX");
	%this.addItem("SizeY");
	%this.addItem("Speed");
	%this.addItem("Spin");
	%this.addItem("Fixed Force");
	%this.addItem("Random Motion");
	%this.addItem("Emission Force");
	%this.addItem("Emission Angle");
	%this.addItem("Emission Arc");
	%this.addItem("Red Channel");
	%this.addItem("Green Channel");
	%this.addItem("Blue Channel");
	%this.addItem("Alpha Channel");
}

function AssetParticleGraphTool::init(%this)
{
	%this.listScroll = new GuiScrollCtrl()
	{
		HorizSizing="right";
		VertSizing="height";
		Position="0 0";
		Extent="200" SPC getWord(%this.extent, 1);
		hScrollBar="alwaysOff";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.listScroll, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.listScroll, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.listScroll, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.listScroll, "scrollingPanelArrowProfile", "ArrowProfile");
	%this.add(%this.listScroll);

	%this.baseList = new GuiListBoxCtrl()
	{
		Class = "AssetParticleGraphToolList";
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent= "200 100";
		hScrollBar="dynamic";
		vScrollBar="dynamic";
	};
	%this.startListening(%this.baseList);
	ThemeManager.setProfile(%this.baseList, "listBoxProfile");
	%this.listScroll.add(%this.baseList);

	%this.toolScroll = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="200 0";
		Extent= (getWord(%this.extent, 0) - 200) SPC getWord(%this.extent, 1);
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

	%itemWidth = 360;
	%this.toolGrid = new GuiGridCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent = (getWord(%this.toolScroll.Extent, 0) - 14) SPC getWord(%this.extent, 1);
		CellSizeX = %itemWidth;
		CellSizeY = 0;
		CellModeX = variable;
		CellModeY = variable;
		CellSpacingX = 4;
		CellSpacingY = 4;
		OrderMode = "LRTB";
	};
	ThemeManager.setProfile(%this.toolGrid, "emptyProfile");
	%this.toolScroll.add(%this.toolGrid);

	%this.baseGraph = new GuiControl()
	{
		Class = "AssetParticleGraphUnit";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent= %itemWidth SPC (getWord(%this.extent, 1) - 30);
		Text = "Base Value";
	};
	ThemeManager.setProfile(%this.baseGraph, "labelProfile");
	%this.toolGrid.add(%this.baseGraph);
}

function AssetParticleGraphEmitterTool::initEmitter(%this)
{
	%this.variGraph = new GuiControl()
	{
		Class = "AssetParticleGraphUnit";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent= %itemWidth SPC (getWord(%this.extent, 1) - 30);
		Text = "Variation";
		Tool = %this.toolGrid;
	};
	ThemeManager.setProfile(%this.variGraph, "labelProfile");
	%this.toolGrid.add(%this.variGraph);

	%this.lifeGraph = new GuiControl()
	{
		Class = "AssetParticleGraphUnit";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent= %itemWidth SPC (getWord(%this.extent, 1) - 30);
		Text = "Scale Over Particle Lifetime";
		Tool = %this.toolGrid;
	};
	ThemeManager.setProfile(%this.lifeGraph, "labelProfile");
	%this.toolGrid.add(%this.lifeGraph);
}

function AssetParticleGraphTool::addItem(%this, %item, %color)
{
	if(%color !$= "")
	{
		%this.baseList.addItem(%item, %color);
	}
	else
	{
		%this.baseList.addItem(%item);
	}
}

function AssetParticleGraphTool::inspect(%this, %asset, %emitterID)
{
	%this.asset = %asset;
	%this.baseGraph.graph.inspect(%asset);
	if(isObject(%this.variGraph))
	{
		%this.variGraph.graph.inspect(%asset);
	}
	if(isObject(%this.lifeGraph))
	{
		%this.lifeGraph.graph.inspect(%asset);
	}
	%this.baseList.clearSelection();
	%this.emitterID = %emitterID;
	%this.baseList.setCurSel(0);
}

function AssetParticleGraphToolList::onSelect(%this, %index, %text, %id)
{
	%this.postEvent("Select", %index);
}

function AssetParticleGraphTool::onSelect(%this, %index)
{
	%i = 0;
	%graphTable[%i] = "LifeTimeScale"; %i++;
	%graphTable[%i] = "QuantityScale"; %i++;
	%graphTable[%i] = "SizeXScale"; %i++;
	%graphTable[%i] = "SizeYScale"; %i++;
	%graphTable[%i] = "SpeedScale"; %i++;
	%graphTable[%i] = "SpinScale"; %i++;
	%graphTable[%i] = "FixedForceScale"; %i++;
	%graphTable[%i] = "RandomMotionScale"; %i++;
	%graphTable[%i] = "AlphaChannelScale";

	%name = %graphTable[%index];
	%this.baseGraph.setToScale(%name);
	%this.baseGraph.setValueController(%this.getValueController(%name));
	%this.baseGraph.setTimeController(%this.getTimeController(%name));
}

function AssetParticleGraphTool::getValueController(%this, %name)
{
	if(%name $= "")
	{
		return "";
	}
	return new ScriptObject()
	{
		class = ParticleGraphCameraController;
		fieldName = %name;
		isTime = false;
		asset = %this.asset;
	};
}

function AssetParticleGraphTool::getTimeController(%this, %name)
{
	if(%name $= "")
	{
		return "";
	}
	return new ScriptObject()
	{
		class = ParticleGraphCameraController;
		fieldName = %name;
		isTime = true;
		asset = %this.asset;
	};
}

function AssetParticleGraphEmitterTool::onSelect(%this, %index)
{
	%i = 0;
	%graphTable[%i] = "Lifetime"; %i++;
	%graphTable[%i] = "Quantity"; %i++;
	%graphTable[%i] = "SizeX"; %i++;
	%graphTable[%i] = "SizeY"; %i++;
	%graphTable[%i] = "Speed"; %i++;
	%graphTable[%i] = "Spin"; %i++;
	%graphTable[%i] = "FixedForce"; %i++;
	%graphTable[%i] = "RandomMotion"; %i++;
	%graphTable[%i] = "EmissionForce"; %i++;
	%graphTable[%i] = "EmissionAngle"; %i++;
	%graphTable[%i] = "EmissionArc"; %i++;
	%graphTable[%i] = "RedChannel"; %i++;
	%graphTable[%i] = "GreenChannel"; %i++;
	%graphTable[%i] = "BlueChannel"; %i++;
	%graphTable[%i] = "AlphaChannel";

	for(%i = 0; %i < 11; %i++)
	{
		%varTable[%i] = %graphTable[%i] @ "Variation";
	}

	for(%i = 2; %i < 8; %i++)
	{
		%lifeTable[%i] = %graphTable[%i] @ "Life";
	}

	%name = %graphTable[%index];
	%this.baseGraph.setToBase(%name, %varTable[%index], %this.emitterID);
	%this.baseGraph.setValueController(%this.getValueController(%name));
	%this.baseGraph.setTimeController(%this.getTimeController(%name));

	%name = %varTable[%index];
	%this.variGraph.setToVari( %name, %this.emitterID);
	%this.variGraph.setValueController(%this.getValueController(%name));
	%this.variGraph.setTimeController(%this.getTimeController(%name));

	%name = %lifeTable[%index];
	%this.lifeGraph.setToLife( %name, %this.emitterID);
	%this.lifeGraph.setValueController(%this.getValueController(%name));
	%this.lifeGraph.setTimeController(%this.getTimeController(%name));
}
