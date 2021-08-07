
function EditorIconButton::onAdd(%this)
{
	%this.text = "";
	%this.extent = "24 24";
	%this.icon = new GuiSpriteCtrl()
	{
		HorizSizing="center";
		VertSizing="center";
		Extent = "20 20";
		minExtent = "20 20";
		Position = "0 0";
		constrainProportions = "1";
		fullSize = "0";
		Image = "EditorCore:EditorIcons16";
		ImageSize = "18 18";
		ImageColor = ThemeManager.activeTheme.iconButtonProfile.FontColor;
		Frame = %this.frame;
		Tooltip = %this.Tooltip;
	};
	ThemeManager.setProfile(%this.icon, "spriteProfile");
	%this.add(%this.icon);

	%this.startListening(ThemeManager);
	if(%this.Tooltip !$= "")
	{
		ThemeManager.setProfile(%this, "tipProfile", "TooltipProfile");
	}
}

function EditorIconButton::onTouchEnter(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColorHL, 200, "EaseInOut");
	%this.icon.growTo("20 20", 200, "EaseInOut");
}

function EditorIconButton::onTouchLeave(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColor, 200, "EaseInOut");
	%this.icon.growTo("18 18", 200, "EaseInOut");
}

function EditorIconButton::onTouchDown(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColorSL, 200, "EaseInOut");
}

function EditorIconButton::onTouchUp(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColorHL, 200, "EaseInOut");
}

function EditorIconButton::onActive(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColor, 200, "EaseInOut");
}

function EditorIconButton::onInactive(%this)
{
	%this.icon.fadeTo(ThemeManager.activeTheme.iconButtonProfile.fontColorNA, 200, "EaseInOut");
}

function EditorIconButton::onThemeChange(%this, %theme)
{
	if(%this.active)
	{
		%this.icon.setImageColor(ThemeManager.activeTheme.iconButtonProfile.fontColor);
	}
	else
	{
		%this.icon.setImageColor(ThemeManager.activeTheme.iconButtonProfile.fontColorNA);
	}
}
