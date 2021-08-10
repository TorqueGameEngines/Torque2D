
function EditorButtonBar::addButton(%this, %click, %frame, %tooltip, %enabled)
{
	if(!isObject(%this.tool))
	{
		warn("EditorButtonBar::addButton: Cannot add a button without a tool to handle callback.");
		return;
	}

	%button = new GuiButtonCtrl()
	{
		Class = "EditorIconButton";
		Frame = %frame;
		Position = "0 0";
		Command = %this.tool.getId() @ "." @ %click @ "();";
		Tooltip = %tooltip;
		EnabledFunction = %enabled;
	};
	ThemeManager.setProfile(%button, "iconButtonProfile");
	%this.add(%button);

	if(%enabled !$= "")
	{
		%button.setActive(%this.tool.call(%enabled));
	}
}

function EditorButtonBar::refreshEnabled(%this)
{
	for(%i = 0; %i < %this.getCount(); %i++)
	{
		%button = %this.getObject(%i);
		if(%button.EnabledFunction !$= "")
		{
			%button.setActive(%this.tool.call(%button.EnabledFunction));
		}
	}
}
