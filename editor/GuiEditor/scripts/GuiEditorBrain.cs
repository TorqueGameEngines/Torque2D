
function GuiEditorBrain::onAdd(%this)
{
    %this.setFirstResponder();
    %this.setSnapToGrid("10");
}

function GuiEditorBrain::onControlDragged(%this, %payload, %position)
{
	%x = getWord(%position, 0);
	%y = getWord(%position, 1);
	%target = %this.root.findHitControl(%x, %y);

	while(! %target.isContainer )
	{
		%target = %target.getParent();
	}

	if(%target != %this.getCurrentAddset())
	{
		%this.setCurrentAddSet(%target);
	}
}

function GuiEditorBrain::onControlDropped(%this, %payload, %position)
{
   %pos = %payload.getGlobalPosition();
   %x = getWord(%pos, 0);
   %y = getWord(%pos, 1);

   %this.addNewCtrl(%payload);
   %payload.setPositionGlobal(%x, %y);
   %this.setFirstResponder();
   %this.postEvent("AddControl", %payload);
    %this.postEvent("Inspect", %payload);
   %this.schedule(40, "finishControlDropped", %payload, %x, %y);
}

function GuiEditorBrain::finishControlDropped(%this, %payload, %x, %y)
{
   %payload.setPositionGlobal(%x, %y);
}

function GuiEditorBrain::startRadioSilence(%this)
{
    %this.removeAllListeners();
}

function GuiEditorBrain::endRadioSilence(%this)
{
    %this.addListener(GuiEditor.explorerWindow);
    %this.addListener(GuiEditor.inspectorWindow);
}

//Source callbacks - Events that happened with this control and need to be relayed to other controls.
function GuiEditorBrain::onEdit(%this, %ctrl)
{
    %this.postEvent("Edit", %ctrl);
}

function GuiEditorBrain::onRemoveSelected(%this,%ctrl)
{
    %this.postEvent("ClearInspect", %ctrl);
    %this.toggleMenuItems();
}

function GuiEditorBrain::onClearSelected(%this)
{
    %this.postEvent("ClearInspectAll");
    %this.toggleMenuItems();
}

function GuiEditorBrain::onAddSelected(%this, %ctrl)
{
    %this.postEvent("AlsoInspect", %ctrl);
    %this.toggleMenuItems();
}

function GuiEditorBrain::onDelete(%this)
{
	%this.postEvent("ObjectRemoved");
    %this.toggleMenuItems();
}

function GuiEditorBrain::onSelectionParentChange(%this, %parent)
{
    %this.postEvent("ParentChange", %parent);
    %this.toggleMenuItems();
}

//Receiving Callbacks - Events that happened at other controls and need to be reflected with this control.
function GuiEditorBrain::onInspect(%this, %ctrl)
{
    %this.startRadioSilence();
    %this.clearSelection();
	%this.select(%ctrl);
    %this.endRadioSilence();
    %this.toggleMenuItems();
}

function GuiEditorBrain::onAlsoInspect(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.addSelection(%ctrl);
    %this.endRadioSilence();
    %this.toggleMenuItems();
}

function GuiEditorBrain::onClearInspect(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.removeSelection(%ctrl);
    %this.endRadioSilence();
    %this.toggleMenuItems();
}

function GuiEditorBrain::onClearInspectAll(%this)
{
    %this.startRadioSilence();
	%this.clearSelection();
    %this.endRadioSilence();
    %this.toggleMenuItems();
}

function GuiEditorBrain::onObjectRemoved(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.deleteSelection();
    %this.endRadioSilence();
    %this.toggleMenuItems();
}

function GuiEditorBrain::toggleMenuItems(%this)
{
    %count = %this.getSelected().getCount();
    EditorCore.menuBar.setMenuActive("Deselect", %count != 0);
    EditorCore.menuBar.setMenuActive("Nudge Up", %count != 0);
    EditorCore.menuBar.setMenuActive("Nudge Down", %count != 0);
    EditorCore.menuBar.setMenuActive("Nudge Left", %count != 0);
    EditorCore.menuBar.setMenuActive("Nudge Right", %count != 0);
    EditorCore.menuBar.setMenuActive("Expand Height", %count != 0);
    EditorCore.menuBar.setMenuActive("Shrink Height", %count != 0);
    EditorCore.menuBar.setMenuActive("Expand Width", %count != 0);
    EditorCore.menuBar.setMenuActive("Shrink Width", %count != 0);
    EditorCore.menuBar.setMenuActive("Align Top", %count > 1);
    EditorCore.menuBar.setMenuActive("Align Bottom", %count > 1);
    EditorCore.menuBar.setMenuActive("Align Left", %count > 1);
    EditorCore.menuBar.setMenuActive("Align Right", %count > 1);
    EditorCore.menuBar.setMenuActive("Center Horizontally", %count > 1);
    EditorCore.menuBar.setMenuActive("Space Vertically", %count > 2);
    EditorCore.menuBar.setMenuActive("Space Horizontally", %count > 2);
    EditorCore.menuBar.setMenuActive("Bring to Front", %count == 1);
    EditorCore.menuBar.setMenuActive("Push to Back", %count == 1);
}