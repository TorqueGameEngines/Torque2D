
function GuiEditorSaveGuiDialog::init(%this, %width, %height)
{
	//Get the dialog contents
	%window = %this.getObject(0);
	%content = %window.getObject(0);

	//Create the file text box
	%form = new GuiGridCtrl()
	{
		class = "EditorForm";
		extent = %width SPC %height;
		cellSizeX = %width;
		cellSizeY = 50;
	};
	%form.addListener(%this);

	%item = %form.addFormItem("Gui Name", %width SPC 30);
	%this.guiNameBox = %form.createTextEditItem(%item);
	%this.guiNameBox.Command = %this.getId() @ ".Validate();";

	if(GuiEditor.rootGui.getName() !$= "")
	{
		%name = %this.blankGui.getName() @ ".gui";
	}
	else
	{
		%name = "untitled.gui";
	}
	%this.guiNameBox.text = %name;

	%item = %form.addFormItem("Target Folder", %width SPC 30);
	%this.folderBox = %form.createFolderOpenItem(%item, "Select Target Folder");
	%this.folderBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Target Module", %width SPC 30);
	%this.moduleNameBox = %form.createTextEditItem(%item);
	%this.moduleNameBox.setActive(false);

	%content.add(%form);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 170";
		Extent = (%width - 24) SPC 80;
		text = "Select a name and target folder!";
		textWrap = true;
		textExtend = true;
	};
	ThemeManager.setProfile(%this.feedback, "infoProfile");

	%this.cancelButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "478 270";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.saveButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 268";
		Extent = "100 34";
		Text = "Save";
		Command = %this.getID() @ ".onSave();";
	};
	ThemeManager.setProfile(%this.saveButton, "primaryButtonProfile");

	%content.add(%this.feedback);
	%content.add(%this.cancelButton);
	%content.add(%this.saveButton);

	%this.prevFolder = "";
}

function GuiEditorSaveGuiDialog::Validate(%this)
{
	%this.createButton.active = false;

	%folderPath = %this.getFolderPath();
	%guiName = %this.guiNameBox.getText();

	if(%this.folderBox.getText() $= "")
	{
		%this.folderBox.setText(mgetMainDotCsDir());
	}

	if(%folderPath !$= %this.prevFolder)
	{
		%modSig = EditorCore.findModuleOfPath(%folderPath @ "a.txt");
		%this.moduleNameBox.setText(%modSig);
		%this.prevFolder = %folderPath;
	}
	%moduleName = getUnit(%this.moduleNameBox.getText(), 0, "_");
	%moduleVersion = getUnit(%this.moduleNameBox.getText(), 1, "_");

	if(%folderPath $= "")
	{
		%this.feedback.setText("Please select a target folder.");
		return false;
	}

	if(%guiName $= "")
	{
		%this.feedback.setText("A Gui file must have a name.");
		return false;
	}

	if(%moduleName $= "")
	{
		%this.feedback.setText("You can only save a Gui file inside of a module.");
		return false;
	}
	else
	{
		%module = ModuleDatabase.findModule(%moduleName, %moduleVersion);
		if(!isObject(%module))
		{
			%this.feedback.setText("There was a problem finding the module for this file.");
			return false;
		}
		else if(%module.Synchronized)
		{
			%this.feedback.setText("You cannot add files to a library module. Updates to the module would remove your files. Instead, create your own module and add files to it.");
			return false;
		}
	}

	%filePath = %this.getFilePath();
	if(isFile(%filePath) && !isWriteableFileName(%filePath))
	{
		%this.feedback.setText("A file by this name already exists and it is read-only.");
		return false;
	}
	if(isFile(%filePath))
	{
		%this.createButton.active = true;
		%this.feedback.setText("A file by this name already exists. It will be overwritten.");
		return true;
	}

	%this.createButton.active = true;
	%this.feedback.setText("A new Gui file will be created!");
	return true;
}

function GuiEditorSaveGuiDialog::onClose(%this)
{
	Canvas.popDialog(%this);
	%this.postEvent("DialogClosed", %this);
}

function GuiEditorSaveGuiDialog::onSave(%this)
{
	if(%this.validate())
	{
		%filePath = %this.getFilePath();
		%fo = new FileObject();
		%fo.openForWrite(%filePath);
		%fo.writeLine("//--- Created with the GuiEditor ---//");
		%fo.writeObject(GuiEditor.rootGui, "%guiContent = ");
		%fo.writeLine("//--- GuiEditor End ---//");
		%fo.close();
		%fo.delete();

		%this.onClose();
	}
}

function GuiEditorSaveGuiDialog::onFolderOpened(%this, %textBox)
{
	%this.Validate();
}

function GuiEditorSaveGuiDialog::getFilePath(%this)
{
	%folderPath = %this.getFolderPath();
	%fileName = %this.guiNameBox.getText();
	%filePath = pathConcat(%folderPath, %fileName);
	%length = strlen(%filePath);
	%sub = "";
	if(%length >= 4)
	{
		%sub = getSubStr(%filePath, %length - 4, 4);
	}
	if(%sub !$= ".gui")
	{
		%filePath = %filePath @ ".gui";
	}

	return %filePath;
}

function GuiEditorSaveGuiDialog::getFolderPath(%this)
{
	%folderPath = stripTrailingSpaces(makeFullPath(%this.folderBox.getText()));
	%length = strlen(%folderPath);
	%lastChar = getSubStr(%folderPath, %length - 1, 1);
	if(%lastChar $= "/")
	{
		return %folderPath;
	}
	return %folderPath @ "/";
}
