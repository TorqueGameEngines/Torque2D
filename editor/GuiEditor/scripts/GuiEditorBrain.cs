
function GuiEditorBrain::onAdd(%this)
{
    %this.setFirstResponder();
    %this.setSnapToGrid("10");
}

function GuiEditorBrain::onControlDragged(%this, %payload, %position)
{
	%pos = VectorSub(%position, %this.getLocalPosition());
	%x = getWord(%pos, 0);
	%y = getWord(%pos, 1);
	%target = GuiEditor.content.findHitControl(%x, %y);

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

   if(%x < %this.root.position.x || %y < %this.root.poisition.y || 
    %x > (%this.root.extent.x + %this.root.position.x) || %y > (%this.root.extent.y + %this.root.position.y))
   {
      messageBox("Error", "Cannot add a control outside the root gui element!");
      return;
   }

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
function GuiEditorBrain::onSelect(%this, %ctrl)
{
	%this.clearSelection();
	%this.select(%ctrl);
    %this.postEvent("Inspect", %ctrl);
}

function GuiEditorBrain::onRemoveSelected(%this,%ctrl)
{
    %this.postEvent("ClearInspect", %ctrl);
}

function GuiEditorBrain::onClearSelected(%this)
{
    %this.postEvent("ClearInspectAll");
}

function GuiEditorBrain::onAddSelected(%this, %ctrl)
{
    %this.postEvent("AlsoInspect", %ctrl);
}

function GuiEditorBrain::onDelete(%this)
{
	%this.postEvent("ObjectRemoved", %ctrl);
}

function GuiEditorBrain::onSelectionParentChange(%this, %parent)
{
    %this.postEvent("ParentChange", %parent);
}

//Receiving Callbacks - Events that happened at other controls and need to be reflected with this control.
function GuiEditorBrain::onInspect(%this, %ctrl)
{
    %this.startRadioSilence();
    %this.clearSelection();
	%this.select(%ctrl);
    %this.endRadioSilence();
}

function GuiEditorBrain::onAlsoInspect(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.addSelection(%ctrl);
    %this.endRadioSilence();
}

function GuiEditorBrain::onClearInspect(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.removeSelection(%ctrl);
    %this.endRadioSilence();
}

function GuiEditorBrain::onClearInspectAll(%this)
{
    %this.startRadioSilence();
	%this.clearSelection();
    %this.endRadioSilence();
}

function GuiEditorBrain::onObjectRemoved(%this, %ctrl)
{
    %this.startRadioSilence();
	%this.deleteSelection();
    %this.endRadioSilence();
}