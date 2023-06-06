
function GuiEditorControlListBox::onTouchDragged(%this, %index, %text)
{
	%position = GuiEditor.guiPage.getGlobalPosition();
	%cursorpos = Canvas.getCursorPos();

	%class = %this.getItemText(%this.getSelectedItem());
	%payload = eval("return new " @ %class @ "();");
	if(!isObject(%payload))
		return;

	%xOffset = (getWord(%payload.extent, 0) / 2) + getWord(%position, 0);
	%yOffset = (getWord(%payload.extent, 1) / 2) + getWord(%position, 1);

	// position where the drag will start, to prevent visible jumping.
	%xPos = getWord(%cursorpos, 0) - %xOffset;
	%yPos = getWord(%cursorpos, 1) - %yOffset;

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
}