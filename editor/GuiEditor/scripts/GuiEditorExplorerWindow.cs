
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
	%this.tree.startRadioSilence();
	%index = %this.tree.findItemID(%ctrl.getID());
	%this.tree.clearSelection();
	%this.tree.setSelected(%index, true);
	%this.tree.endRadioSilence();
}

function GuiEditorExplorerWindow::onAlsoInspect(%this, %ctrl)
{
	%this.tree.startRadioSilence();
	%index = %this.tree.findItemID(%ctrl.getID());
	%this.tree.setSelected(%index, true);
	%this.tree.endRadioSilence();
}

function GuiEditorExplorerWindow::onClearInspect(%this, %ctrl)
{
	%this.tree.startRadioSilence();
	%this.tree.setSelected(%ctrl, false);
	%this.tree.endRadioSilence();
}

function GuiEditorExplorerWindow::onClearInspectAll(%this)
{
	%this.tree.startRadioSilence();
	%this.tree.clearSelection();
	%this.tree.endRadioSilence();
}

function GuiEditorExplorerWindow::onObjectRemoved(%this, %ctrl)
{
	%this.tree.startRadioSilence();
	%index = %this.tree.findItemID(%ctrl.getID());
	%this.tree.deleteItem(%index);
	%this.tree.endRadioSilence();
}

function GuiEditorExplorerWindow::onAddControl(%this, %ctrl)
{
	%this.tree.refresh();
}

function GuiEditorExplorerWindow::onParentChange(%this, %parent)
{
	%index = %this.tree.findItemID(%parent);
	while(%index != -1)
	{
		%this.tree.setItemOpen(%index, true);
		%index = %this.tree.getItemParent(%index);
	} 
	%this.tree.refresh();
}