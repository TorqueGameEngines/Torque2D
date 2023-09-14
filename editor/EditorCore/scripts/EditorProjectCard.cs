function EditorProjectCard::onAdd(%this)
{
	%this.header = new GuiSpriteCtrl()
	{
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "200 20";
		SingleFrameBitmap = "1";
		ConstrainProportions = "0";
		FullSize = "1";
		ImageColor = "255 255 255 255";
		PositionOffset = "0 0";
		UseInput = false;
	};
	ThemeManager.setProfile(%this.header, "spriteProfile");
	ThemeManager.setImage(%this.header, "fixedHeader200x20");
	%this.add(%this.header);

	%this.projectIcon = new GuiSpriteCtrl()
	{
		Bitmap = %this.bitmap;
		Extent = "200 160";
		Position = "0 20";
		SingleFrameBitmap = "1";
		constrainProportions = "1";
		FullSize = "1";
		ImagePosition = "0 0";
		UseInput = false;
	};
	ThemeManager.setProfile(%this.projectIcon, "spriteProfile");
	%this.add(%this.projectIcon);

	%this.projectIconShadow = new GuiSpriteCtrl()
	{
		Image = "EditorCore:projectTileShadow";
		Extent = "200 160";
		Position = "0 20";
		SingleFrameBitmap = "1";
		constrainProportions = "1";
		FullSize = "1";
		ImagePosition = "0 0";
		UseInput = false;
	};
	ThemeManager.setProfile(%this.projectIconShadow, "spriteProfile");
	%this.add(%this.projectIconShadow);

	%this.scroller = new GuiScrollCtrl()
	{
		class = EditorProjectCardScroller;
		Position="0 180";
		Extent="200 120";
		hScrollBar="alwaysOff";
		vScrollBar="alwaysOn";
		HorizSizing = "right";
		VertSizing = "bottom";
		constantThumbHeight="0";
		showArrowButtons="0";
		scrollBarThickness="6";
	};
	ThemeManager.setProfile(%this.scroller, "emptyProfile");
	ThemeManager.setProfile(%this.scroller, "tinyThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.scroller, "tinyTrackProfile", "TrackProfile");
	%this.add(%this.scroller);
	%this.startListening(%this.scroller);

	%this.chain = new GuiChainCtrl()
	{
		Position="0 0";
		Extent = "194 120";
		IsVertical = "1";
		ChildSpacing = -4;
	};
	ThemeManager.setProfile(%this.chain, "emptyProfile");
	%this.scroller.add(%this.chain);

	%this.titleText = new GuiControl()
	{
		Position = "0 13";
		Extent = "200 38";
		Text = %this.title;
		TextWrap = 1;
		TextExtend = 1;
	};
	ThemeManager.setProfile(%this.titleText, "titleProfile");
	%this.chain.add(%this.titleText);

	%this.directoryText = new GuiControl()
	{
		Position = "0 77";
		Extent = "200 128";
		Text = "\\" @ %this.directory @ "\\";
		TextWrap = 1;
		TextExtend = 1;
	};
	ThemeManager.setProfile(%this.directoryText, "codeProfile");
	%this.chain.add(%this.directoryText);

	if(%this.isNew)
	{
		%this.directoryText.text = %this.directory;
	}

	%this.descriptionText = new GuiControl()
	{
		Position = "0 43";
		Extent = "200 70";
		Text = %this.description;
		TextWrap = 1;
		TextExtend = 1;
	};
	ThemeManager.setProfile(%this.descriptionText, "normalTextProfile");
	%this.chain.add(%this.descriptionText);
}

function EditorProjectCard::onClick(%this)
{
	if(!%this.isNew)
	{
		%this.postEvent("ProjectSelected", %this.directory);
	}
	else
	{
		%this.postEvent("NewProject");
	}
}
