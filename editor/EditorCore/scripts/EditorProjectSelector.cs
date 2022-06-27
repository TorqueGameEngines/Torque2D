
function EditorProjectSelector::onAdd(%this)
{
	%this.header = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="bottom";
		Position="0 0";
		Extent="1024 84";
		Visible = false;
	};
	ThemeManager.setProfile(%this.header, "panelProfile");
	%this.add(%this.header);

	%this.headerGlow = new GuiSpriteCtrl()
	{
		HorizSizing="width";
		VertSizing="bottom";
		Position="0 0";
		Extent="1024 84";
		Bitmap = "^EditorCore/images/headerGlow.png";
		SingleFrameBitmap = "1";
		ConstrainProportions = "1";
		FullSize = "0";
		ImageSize = "532 84";
		ImageColor = "255 255 255 255";
		PositionOffset = "0 0";
		Visible = false;
	};
	ThemeManager.setProfile(%this.headerGlow, "spriteProfile");
	%this.add(%this.headerGlow);

	%this.torque2d = new GuiSpriteCtrl()
	{
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="512 84";
		Bitmap = "^EditorCore/images/t2d_logo_med.png";
		SingleFrameBitmap = "1";
		ConstrainProportions = "1";
		FullSize = "0";
		ImageSize = "438 65";
		ImageColor = "255 255 255 255";
		PositionOffset = "-20 0";
		Visible = false;
	};
	ThemeManager.setProfile(%this.torque2d, "spriteProfile");
	%this.add(%this.torque2d);

	%this.versionText = new GuiControl()
	{
		HorizSizing="left";
		VertSizing="bottom";
		Position = "824 62";
		Extent = "200 20";
		Text = getEngineVersion();
		Visible = false;
		FontSizeAdjust = 1.2;
		Align = "right";
		OverrideFontColor = true;
		FontColor = ThemeManager.activeTheme.color5;
	};
	ThemeManager.setProfile(%this.versionText, "codeProfile");
	%this.add(%this.versionText);

	%this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 84";
		Extent="1024 684";
		hScrollBar="alwaysOff";
		vScrollBar="dynamic";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.scroller, "emptyProfile");
	ThemeManager.setProfile(%this.scroller, "scrollingPanelThumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.scroller, "scrollingPanelTrackProfile", TrackProfile);
	ThemeManager.setProfile(%this.scroller, "scrollingPanelArrowProfile", ArrowProfile);
	%this.add(%this.scroller);

	%this.grid = new GuiGridCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="20 20";
		Extent = "984 684";
		CellSizeX = 210;
		CellSizeY = 310;
		CellModeX = "fixed";
		CellSpacingX = 20;
		CellSpacingY = 20;
		OrderMode = "LRTB";
	};
	ThemeManager.setProfile(%this.grid, "emptyProfile");
	%this.scroller.add(%this.grid);

	%this.scroller.computeSizes();

	%this.torqueCurtain = new GuiSpriteCtrl()
	{
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		SingleFrameBitmap = "1";
		ConstrainProportions = "0";
		FullSize = "1";
		ImageColor = "255 255 255 255";
		PositionOffset = "0 0";
		Visible = false;
		UseInput = false;
	};
	ThemeManager.setProfile(%this.torqueCurtain, "spriteProfile");
	ThemeManager.setImage(%this.torqueCurtain, "editorBG");
	%this.add(%this.torqueCurtain);
}

function EditorProjectSelector::show(%this)
{
	%this.torqueCurtain.setVisible(true);
	%this.header.setVisible(true);
	%this.versionText.setVisible(true);
	%this.headerGlow.setVisible(true);
	%this.torque2d.setVisible(true);

	%folderList = getDirectoryList(getMainDotCsDir());
	for(%i = 0; %i < getFieldCount(%folderList); %i++)
	{
		%folder = getField(%folderList, %i);
		if(%folder !$= "library" && %folder !$= "editor" && %folder !$= "engine" && getSubStr(%folder, 0, 1) !$= ".")
		{
			%this.scanFolder(%folder);
		}
	}

	%this.showProject("New Project", pathConcat(getMainDotCsDir(), "editor/EditorCore/images/newProject.png"), "Blast off!", "Time to launch something new?", true);

	%this.torqueCurtain.fadeTo("255 255 255 0", 500, "easeInOut");
}

function EditorProjectSelector::hide(%this)
{
	%this.torqueCurtain.fadeTo("255 255 255 255", 500, "easeOut");
	%this.schedule(500, "postEvent", "ProjectSelected");
}

function EditorProjectSelector::scanFolder(%this, %folder)
{
	//echo("scanning folder " @ %folder);
	ModuleDatabase.ScanModules(%folder);
	%appCore = ModuleDatabase.findModule("AppCore", 1);
	if(isObject(%appCore))
	{
		%this.showProject(%appCore.project, pathConcat(%appCore.getModulePath(), %appCore.icon), %folder, %appCore.projectDescription, false);
	}

	//Awesome, now unscan everything we just scanned.
	ModuleDatabase.clearDatabase();
}

function EditorProjectSelector::showProject(%this, %title, %bitmap, %directory, %description, %isNew)
{
	%container = new GuiButtonCtrl()
	{
		Class = EditorProjectCard;
		Extent = "210 310";
		title = %title;
		bitmap = %bitmap;
		directory = %directory;
		description = %description;
		isNew = %isNew;
	};
	ThemeManager.setProfile(%container, "displayBoxProfile");
	%this.grid.add(%container);

	%container.addListener(%this);
}

function EditorProjectSelector::onProjectSelected(%this, %folder)
{
	ModuleDatabase.ScanModules(%folder);
	%this.hide();
}

function EditorProjectSelector::onNewProject(%this)
{
	%width = 700;
	%height = 340;
	%dialog = new GuiControl()
	{
		class = "NewProjectDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Project";
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
}

function EditorProjectSelector::onDialogClosed(%this, %dialog)
{
	%this.dialog = %dialog;
	%this.schedule(100, "deleteDialog");
}

function EditorProjectSelector::deleteDialog(%this)
{
	%this.dialog.delete();
}

function EditorProjectSelector::onProjectCreated(%this, %folder)
{
	%this.scanFolder(%folder);
}
