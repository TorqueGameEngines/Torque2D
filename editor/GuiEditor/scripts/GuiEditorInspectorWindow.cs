

function GuiEditorInspectorWindow::onAdd(%this)
{
    %this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="352 355";
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

    %this.inspector = new GuiInspector()
	{
		Class = "GuiEditorInspector";
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="338 355";
		FieldCellSize="288 40";
		ControlOffset="10 18";
		ConstantThumbHeight=false;
		ScrollBarThickness=12;
		ShowArrowButtons=true;
	};
	ThemeManager.setProfile(%this.inspector, "emptyProfile");
	ThemeManager.setProfile(%this.inspector, "panelProfile", "GroupPanelProfile");
	ThemeManager.setProfile(%this.inspector, "emptyProfile", "GroupGridProfile");
	ThemeManager.setProfile(%this.inspector, "labelProfile", "LabelProfile");
	ThemeManager.setProfile(%this.inspector, "textEditProfile", "textEditProfile");
	ThemeManager.setProfile(%this.inspector, "dropDownProfile", "dropDownProfile");
	ThemeManager.setProfile(%this.inspector, "dropDownItemProfile", "dropDownItemProfile");
	ThemeManager.setProfile(%this.inspector, "emptyProfile", "backgroundProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelProfile", "ScrollProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelArrowProfile", "ArrowProfile");
	ThemeManager.setProfile(%this.inspector, "checkboxProfile", "checkboxProfile");
	ThemeManager.setProfile(%this.inspector, "buttonProfile", "buttonProfile");
	ThemeManager.setProfile(%this.inspector, "tipProfile", "tooltipProfile");
	%this.scroller.add(%this.inspector);

    %this.inspectList = new SimSet();

	%this.inspector.addHiddenField("isContainer");
}

function GuiEditorInspectorWindow::onRemove(%this)
{
    if(isObject(%this.inspectList))
    {
        %this.inspectList.deleteObjects();
        %this.inspectList.delete();
    }
}

function GuiEditorInspectorWindow::onInspect(%this, %object)
{
    %this.inspectList.clear();
    %this.inspectList.add(%object);
    %this.inspector.inspect(%object);
}

function GuiEditorInspectorWindow::onClearInspect(%this, %object)
{
    if(isObject(%object))
    {
        %this.inspectList.removeIfMember(%object);
        %count = %this.inspectList.getCount();
        if(%count > 0)
        {
            %this.inspector.inspect(%this.inspectList.getObject(%count - 1));
        }
    }
}

function GuiEditorInspectorWindow::onClearInspectAll(%this)
{
	%this.inspectList.clear();
    %this.inspector.clear();
}

function GuiEditorInspectorWindow::onAlsoInspect(%this, %object)
{
    %this.inspectList.add(%object);
    %this.inspector.inspect(%object);
}