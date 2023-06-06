
function GuiEditorBrain::onAdd(%this)
{
    %this.setFirstResponder();
    %this.setSnapToGrid("8");
}

function GuiEditorBrain::onControlDragged(%this, %payload, %position)
{
	%pos = VectorSub(%position, %this.getGlobalPosition());
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

   if(%x > %this.root.extent.x || %y > %this.root.extent.y)
   {
      messageBox("Error", "Cannot add a control outside the root gui element!");
      return;
   }

   %this.addNewCtrl(%payload);

   %payload.setPositionGlobal(%x, %y);
   %this.setFirstResponder();
    %this.postEvent("Inspect", %payload);
}

function GuiEditorBrain::onSelect(%this, %ctrl)
{
	GuiEditorBrain.clearSelection();
	GuiEditorBrain.select(%ctrl);
    %this.postEvent("Inspect", %ctrl);
}

function GuiEditorBrain::onClearSelected(%this)
{
    %this.postEvent("ClearInspect");
}

function GuiEditorBrain::onSelectionParentChange(%this)
{
}

function GuiEditorBrain::onDelete(%this)
{
	%this.postEvent("ClearInspect", %ctrl);
}

function GuiEditorBrain::onAddSelected(%this,%ctrl)
{
    %this.postEvent("AlsoInspect", %ctrl);
}

function GuiEditorBrain::onRemoveSelected(%this,%ctrl)
{
    %this.postEvent("ClearInspect", %ctrl);
}