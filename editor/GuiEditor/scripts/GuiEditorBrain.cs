
function GuiEditorBrain::onAdd(%this)
{
    %this.setFirstResponder();
    %this.setSnapToGrid("8");
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
   %this.schedule(100, "finishControlDropped", %payload, %x, %y);
}

function GuiEditorBrain::finishControlDropped(%this, %payload, %x, %y)
{
   %payload.setPositionGlobal(%x, %y);
}

function GuiEditorBrain::onInspect(%this, %ctrl)
{
    %this.clearSelection();
	%this.select(%ctrl);
}

function GuiEditorBrain::onSelect(%this, %ctrl)
{
	%this.clearSelection();
	%this.select(%ctrl);
    %this.postEvent("Inspect", %ctrl);
}

function GuiEditorBrain::onClearSelected(%this)
{
    %this.postEvent("ClearInspectAll");
}

function GuiEditorBrain::onSelectionParentChange(%this)
{
}

function GuiEditorBrain::onDelete(%this)
{
	%this.postEvent("ObjectRemoved", %ctrl);
}

function GuiEditorBrain::onAddSelected(%this,%ctrl)
{
    %this.postEvent("AlsoInspect", %ctrl);
}

function GuiEditorBrain::onRemoveSelected(%this,%ctrl)
{
    %this.postEvent("ClearInspect", %ctrl);
}