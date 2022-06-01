
function ProjectModulePanel::init(%this, %title)
{
	%this.leftPanel = new GuiControl()
	{
		HorizSizing="right";
		VertSizing="height";
		Position="0 0";
		Extent="200 768";
	};
	ThemeManager.setProfile(%this.leftPanel, "overlayProfile");
	%this.add(%this.leftPanel);

	%this.title = new GuiControl()
	{
		Position = "0 4";
		Extent = "200 34";
		Text = %title;
	};
	ThemeManager.setProfile(%this.title, "titleProfile");
	%this.leftPanel.add(%this.title);

	%this.buttonBarContainer = new GuiControl()
	{
		Position = "0 38";
		Extent = "200 34";
	};
	ThemeManager.setProfile(%this.buttonBarContainer, "panelProfile");
	%this.leftPanel.add(%this.buttonBarContainer);

	%this.buttonBar = new GuiChainCtrl()
	{
		Class = "EditorButtonBar";
		Position = "0 4";
		Extent = "0 30";
		ChildSpacing = 4;
		IsVertical = false;
		Tool = %this;
	};
	ThemeManager.setProfile(%this.buttonBar, "emptyProfile");
	%this.buttonBarContainer.add(%this.buttonBar);

	%this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 72";
		Extent="200 696";
		hScrollBar="alwaysOff";
		vScrollBar="dynamic";
		constantThumbHeight="0";
		showArrowButtons="0";
		scrollBarThickness="6";
	};
	ThemeManager.setProfile(%this.scroller, "emptyProfile");
	ThemeManager.setProfile(%this.scroller, "tinyThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.scroller, "tinyTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.scroller, "tinyScrollArrowProfile", "ArrowProfile");
	%this.leftPanel.add(%this.scroller);

	%this.list = new GuiListBoxCtrl()
	{
		Class = "ProjectModulePanelList";
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="200 10";
		AllowMultipleSelections=true;
		FitParentWidth=true;
		FontSizeAdjust="0.9";
	};
	ThemeManager.setProfile(%this.list, "listBoxProfile");
	%this.scroller.add(%this.list);
	%this.startListening(%this.list);

	%this.card = new GuiScrollCtrl()
	{
		Class="ProjectModuleCard";
		HorizSizing="width";
		VertSizing="height";
		Position="200 0";
		Extent="312 768";
		hScrollBar="alwaysOff";
		vScrollBar="dynamic";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
		visible = false;
	};
	ThemeManager.setProfile(%this.card, "scrollProfile");
	ThemeManager.setProfile(%this.card, "thumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.card, "trackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.card, "scrollArrowProfile", "ArrowProfile");
	%this.add(%this.card);
	%this.startListening(%this.card);

	%this.gray = "80 80 80 150";
	%this.yellow = "255 255 100 255";
	%this.red = "255 80 80 255";
	%this.darkRed = "200 0 0 200";
	%this.purple = "255 100 255 255";
}

function ProjectModulePanel::setTitle(%this, %title)
{
	%this.title.setText(%title);
}

function ProjectModulePanel::clearModules(%this)
{
	%this.list.clearItems();
}

function ProjectModulePanel::getModuleName(%this, %module)
{
	%name = %module.ModuleId;
	if(%module.VersionId !$= "1")
	{
		%name = %module.ModuleId @ ":" @ %module.VersionId;
	}
	return %name;
}

function ProjectModulePanelList::onSelect(%this)
{
	%index = %this.getSelectedItem();
	if(%index != -1)
	{
		%module = %this.getItemID(%index);
		%this.postEvent("ModuleSelected", %module);
	}
}

function ProjectModulePanel::onModuleSelected(%this, %module)
{
	%this.card.show(%module);
}

function ProjectModulePanel::onClose(%this)
{
	%this.list.clearSelection();
	%this.card.visible = false;
}

function ProjectModulePanel::sortModules(%this)
{
	%this.list.sortByText();
}
