//GuiEditorControlListWindow.cs

function GuiEditorControlListWindow::onAdd(%this)
{
    %this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="242 355";
		hScrollBar="alwaysOff";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.scroller, "emptyProfile");
	ThemeManager.setProfile(%this.scroller, "thumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.scroller, "trackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.scroller, "scrollArrowProfile", "ArrowProfile");
	%this.add(%this.scroller);

    %this.listBox = new GuiListBoxCtrl()
    {
		class = "GuiEditorControlListBox";
        HorizSizing="width";
		VertSizing="height";
		Position="0 0";
        AllowMultipleSelections = "0";
        fitParentWidth = "1";
    };
	ThemeManager.setProfile(%this.listBox, "listBoxProfile");
    %this.scroller.add(%this.listBox);

    %this.populate();
}

function GuiEditorControlListWindow::onRemove(%this)
{
    if(isObject(%this.scroller))
    {
        %this.scroller.delete();
    }
}

function GuiEditorControlListWindow::populate(%this)
{
    %controls = enumerateConsoleClasses("GuiControl");
	%this.listBox.clearItems();
	for(%i = 0; %i < getFieldCount(%controls); %i++)
	{
		%field = getField(%controls, %i);

        if(%field !$= "GuiCanvas" && (%field $= "SceneWindow" || getSubStr(%field, 0, 3) $= "Gui") && 
            getSubStr(%field, 0, 10) !$= "GuiConsole" && getSubStr(%field, 0, 7) !$= "GuiEdit" && 
            getSubStr(%field, 0, 12) !$= "GuiInspector" && %field !$= "GuiMessageVectorCtrl" &&
            %field !$= "GuiParticleGraphInspector" && %field !$= "GuiGraphCtrl" && %field !$= "GuiSceneObjectCtrl")
        {
		    %this.listBox.addItem(%field);
        }
	}
}