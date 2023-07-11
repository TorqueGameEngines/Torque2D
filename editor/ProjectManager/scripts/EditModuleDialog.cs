
function EditModuleDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Module Name", %width SPC 30);
	%this.moduleNameBox = %form.createTextEditItem(%item);
	%this.moduleNameBox.text = %this.module.moduleID;

	%item = %form.addFormItem("Description", %width SPC 30);
	%this.descriptionBox = %form.createTextEditItem(%item);
	%this.descriptionBox.text = %this.module.description;

	%item = %form.addFormItem("Version", %width SPC 30);
	%this.versionBox = %form.createTextEditItem(%item);
	%this.versionBox.inputMode = "number";
	%this.versionBox.text = %this.module.versionID;

	%item = %form.addFormItem("Build", %width SPC 30);
	%this.buildBox = %form.createTextEditItem(%item);
	%this.buildBox.inputMode = "number";
	%this.buildBox.text = %this.module.buildID;

	%item = %form.addFormItem("Type", %width SPC 30);
	%this.typeBox = %form.createTextEditItem(%item);
	%this.typeBox.text = %this.module.type;

	%item = %form.addFormItem("Author", %width SPC 30);
	%this.authorBox = %form.createTextEditItem(%item);
	%this.authorBox.text = %this.module.author;

	%content.add(%form);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 320";
		Extent = (%width - 24) SPC 80;
		text = "Saving changes to a module will require the module to be unloaded and will likely require the game to be restarted. Also, changing the name or version of a module may break dependencies. You will need to find and fix these.";
		textWrap = true;
		textExtend = true;
	};
	ThemeManager.setProfile(%this.feedback, "infoProfile");

	%this.cancelButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "278 450";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.saveButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "388 448";
		Extent = "100 34";
		Text = "Save";
		Command = %this.getID() @ ".onSave();";
	};
	ThemeManager.setProfile(%this.saveButton, "primaryButtonProfile");

	%content.add(%this.feedback);
	%content.add(%this.cancelButton);
	%content.add(%this.saveButton);

	%this.validate();
}

function EditModuleDialog::onKeyPressed(%this, %textBox)
{
	%this.validate();
}

function EditModuleDialog::onReturnPressed(%this, %textBox)
{
	%this.onSave();
}

function EditModuleDialog::Validate(%this)
{
	%this.createButton.active = false;

	%name = %this.moduleNameBox.getText();
	%desc = %this.descriptionBox.getText();
	%version = %this.versionBox.getText();
	%build = %this.buildBox.getText();
	%type = %this.typeBox.getText();
	%author = %this.authorBox.getText();

	if(%name $= "" || %version $= "" || %build $= "")
	{
		return false;
	}

	%this.moduleNameBox.text = stripChars(%name, " ");

	%this.saveButton.active = true;
	return true;
}

function EditModuleDialog::onSave(%this)
{
	if(%this.validate())
	{
		%name = %this.moduleNameBox.getText();
		%desc = %this.descriptionBox.getText();
		%version = %this.versionBox.getText();
		%build = %this.buildBox.getText();
		%type = %this.typeBox.getText();
		%author = %this.authorBox.getText();

		%data = new ScriptObject()
		{
			moduleID = %name;
			description = %desc;
			versionID = %version;
			buildID = %build;
			type = %type;
			author = %author;
		};

		%this.postEvent("ModuleEdited", %data);
		%data.delete();
		%this.onClose();
	}
}
