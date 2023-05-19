
function GuiEditorBrain::onAdd(%this)
{
    %this.setFirstResponder();
    %this.setSnapToGrid("8");
}

function GuiEditorBrain::onControlDragged(%this, %payload, %position)
{
    echo("GuiEditorBrain::onControlDragged - Enter");
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
	//GuiEditorCtrlProperties.update(%ctrl);
    echo("GuiEditorBrain::onControlDragged - Leave");
}

function GuiEditorBrain::onControlDropped(%this, %payload, %position)
{
    echo("GuiEditorBrain::onControlDropped - Enter");
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
    echo("GuiEditorBrain::onControlDropped - Leave");
}