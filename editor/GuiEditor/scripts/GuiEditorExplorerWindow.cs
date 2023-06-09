
function GuiEditorExplorerWindow::onAdd(%this)
{
    %this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="392 355";
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
		class="GuiEditorExplorerTree";
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="228 355";
	};
	ThemeManager.setProfile(%this.tree, "listboxProfile");
	%this.scroller.add(%this.tree);
}

function GuiEditorExplorerWindow::open(%this, %object)
{
    %this.tree.open(%object);
}

function GuiEditorExplorerWindow::onInspect(%this, %ctrl)
{
	%index = %this.tree.findItemID(%ctrl.getID());
	%this.tree.clearSelection();
	%this.tree.setSelected(%index, true);
}

function GuiEditorExplorerWindow::onAlsoInspect(%this, %ctrl)
{
	%this.tree.setSelected(%ctrl, true);
}

function GuiEditorExplorerWindow::onClearInspect(%this, %ctrl)
{
	%this.tree.setSelected(%ctrl, false);
}

function GuiEditorExplorerWindow::onClearInspectAll(%this)
{
	%this.tree.clearSelection();
}

function GuiEditorExplorerWindow::onObjectRemoved(%this, %ctrl)
{
	%index = %this.tree.findItemID(%ctrl.getID());
	%this.tree.deleteItem(%index);
}

function GuiEditorExplorerWindow::onAddControl(%this, %ctrl)
{
	%this.tree.refresh();
}