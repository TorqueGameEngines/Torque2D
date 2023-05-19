
function GuiEditorControlListBox::onTouchDragged(%this, %index, %text)
{
	echo("test start");
	%position = %this.getGlobalPosition();
	echo(%position);
	%cursorpos = Canvas.getCursorPos();
	echo(%cursorpos);

	%class = %this.getItemText(%this.getSelectedItem());
	%payload = eval("return new " @ %class @ "();");
	echo("payload is " @ %payload.getId());
	if(!isObject(%payload))
		return;

	%xOffset = getWord(%payload.extent, 0) / 2;
	%yOffset = getWord(%payload.extent, 1) / 2;
	echo(%xOffset SPC %yOffset);

	// position where the drag will start, to prevent visible jumping.
	%xPos = getWord(%cursorpos, 0) - %xOffset;
	%yPos = getWord(%cursorpos, 1) - %yOffset;
	echo(%xPos SPC %yPos);

	%dragCtrl = new GuiDragAndDropCtrl() {
		canSaveDynamicFields = "0";
		Profile = "GuiDragAndDropProfile";
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = %xPos SPC %yPos;
		extent = %payload.extent;
		MinExtent = "32 32";
		canSave = "1";
		Visible = "1";
		hovertime = "1000";
		Text = %text;
		deleteOnMouseUp = true;
	};

	%dragCtrl.add(%payload);
	GuiEditor.guiPage.add(%dragCtrl);

	%dragCtrl.startDragging(%xOffset, %yOffset);
	echo("test end");
}