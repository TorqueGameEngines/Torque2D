
function GuiEditorExplorerWindow::onAdd(%this)
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

    %this.tree = new GuiTreeViewCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="228 355";
	};
	ThemeManager.setProfile(%this.tree, "panelProfile");
	%this.scroller.add(%this.tree);
}

function GuiEditorExplorerWindow::open(%this, %object)
{
    %this.tree.open(%object);
}