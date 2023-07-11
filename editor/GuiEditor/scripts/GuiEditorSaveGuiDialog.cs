
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

	if(%this.defaultFileName !$= "")
	{
		%name = %this.defaultFileName;
	}
	else if(GuiEditor.rootGui.getName() !$= "")
	{
		%name = GuiEditor.rootGui.getName() @ ".gui";
	}
	else
	{
		%name = "untitled.gui";
	}
	%this.guiNameBox.text = %name;

	%item = %form.addFormItem("Save Format", %width SPC 30);
	%this.guiFormatDropDown = %form.createDropDownItem(%item);
	%this.guiFormatDropDown.Command = %this.getId() @ ".Validate();";
	%this.populateFormatDropDown();
	%this.guiFormatDropDown.setSelected(%this.formatIndex);

	%item = %form.addFormItem("Target Folder", %width SPC 30);
	%this.folderBox = %form.createFolderOpenItem(%item, "Select Target Folder");
	%this.folderBox.Command = %this.getId() @ ".Validate();";
	if(%this.defaultFolder !$= "")
	{
		%this.folderBox.setText(%this.defaultFolder);
	}

	%item = %form.addFormItem("Target Module", %width SPC 30);
	%this.moduleNameBox = %form.createTextEditItem(%item);
	%this.moduleNameBox.setActive(false);
	if(%this.defaultModule !$= "")
	{
		%this.moduleNameBox.setText(%this.defaultModule);
	}

	%content.add(%form);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 220";
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
		Position = "478 320";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.saveButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 318";
		Extent = "100 34";
		Text = "Save";
		Command = %this.getID() @ ".onSave();";
	};
	ThemeManager.setProfile(%this.saveButton, "primaryButtonProfile");

	%content.add(%this.feedback);
	%content.add(%this.cancelButton);
	%content.add(%this.saveButton);

	%this.prevFolder = "";

	%this.validate();
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
		%formatIndex = %this.guiFormatDropDown.getSelectedItem();
		GuiEditor.SaveCore(%filePath, %formatIndex, %this.folderBox.getText(), %this.moduleNameBox.getText());

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
	if(%this.guiFormatDropDown.getSelectedItem() == 0)
	{
		if(%length >= 4)
		{
			%sub = getSubStr(%filePath, %length - 4, 4);
		}
		if(%sub !$= ".gui")
		{
			%filePath = %filePath @ ".gui";
		}
	}
	else 
	{
		if(%length >= 4)
		{
			%sub4 = getSubStr(%filePath, %length - 4, 4);
		}
		if(%length >= 9)
		{
			%sub9 = getSubStr(%filePath, %length - 9, 9);
		}
		if(%sub9 !$= ".gui.taml")
		{
			if(%sub4 $= ".gui")
			{
				%filePath = %filePath @ ".taml";
			}
			else
			{
				%filePath = %filePath @ ".gui.taml";
			}
		}
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

function GuiEditorSaveGuiDialog::onDropDownClosed(%this, %dropDown)
{
	%this.validate();
}

function GuiEditorSaveGuiDialog::populateFormatDropDown(%this)
{
	%this.guiFormatDropDown.clearItems();
	%this.guiFormatDropDown.addItem("GUI File (*.gui)");
	%this.guiFormatDropDown.addItem("TAML (*.gui.taml)");
	%this.guiFormatDropDown.setSelected(0);
}