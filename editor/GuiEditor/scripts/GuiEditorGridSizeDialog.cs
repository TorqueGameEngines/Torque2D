
function GuiEditorGridSizeDialog::init(%this, %width, %height)
{
	//Get the dialog contents
	%window = %this.getObject(0);
	%content = %window.getObject(0);

	%form = new GuiGridCtrl()
	{
		class = "EditorForm";
		extent = %width SPC %height;
		cellSizeX = %width;
		cellSizeY = 50;
	};
	%form.addListener(%this);

	%item = %form.addFormItem("Between 5 and 50", %width SPC 30);
	%this.gridSizeBox = %form.createTextEditItem(%item);
	%this.gridSizeBox.Command = %this.getId() @ ".Validate();";
    %this.gridSizeBox.setInputMode("Number");
    %this.gridSizeBox.align = "center";
    %grid = GuiEditor.brain.getGridSize();
	%this.gridSizeBox.text = %grid;
    if(%grid < 10)
    {
        %this.gridSizeBox.setCursorPos(1);
    }
    else 
    {
        %this.gridSizeBox.setCursorPos(2);
    }
    %this.gridSizeBox.ReturnCommand = %this.getId() @ ".onDone();";
    %this.gridSizeBox.EscapeCommand = %this.getId() @ ".onClose();";
    

	%content.add(%form);

	%this.okButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "188 68";
		Extent = "100 34";
		Text = "OK";
		Command = %this.getID() @ ".onDone();";
	};
	ThemeManager.setProfile(%this.okButton, "primaryButtonProfile");

	%content.add(%this.okButton);

	%this.validate();
}

function GuiEditorGridSizeDialog::Validate(%this)
{
	%this.okButton.active = false;

	%grid = %this.gridSizeBox.getText();

	if(%grid $= "" || %grid < 5 || %grid > 50)
	{
		return false;
	}

	%this.okButton.active = true;
	return true;
}

function GuiEditorGridSizeDialog::onDone(%this)
{
	if(%this.validate())
	{
	    %grid = %this.gridSizeBox.getText();
		GuiEditor.brain.setSnapToGrid(%grid);

		%this.onClose();
	}
}