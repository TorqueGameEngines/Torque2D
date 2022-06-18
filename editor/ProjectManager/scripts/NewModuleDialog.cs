
function NewModuleDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Module Name", %width SPC 30);
	%this.moduleNameBox = %form.createTextEditItem(%item);

	%item = %form.addFormItem("Template", %width SPC 30);
	%this.templateDropDown = %form.createDropDownItem(%item);
	%this.populateModuleDropDown();

	%content.add(%form);

	%this.cancelButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "278 120";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.createButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "388 118";
		Extent = "100 34";
		Text = "Create";
		Command = %this.getID() @ ".onCreate();";
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.cancelButton);
	%content.add(%this.createButton);

	%this.validate();
}

function NewModuleDialog::populateModuleDropDown(%this)
{
	%manager = new ModuleManager();
	%manager.EchoInfo = false;

	%manager.ScanModules(pathConcat(getMainDotCsDir(), "library"));

	%allModules = %manager.findModules(false);

	for(%i = 0; %i < getWordCount(%allModules); %i++)
	{
		%mod = getWord(%allModules, %i);
		if(%mod.type $= "template")
		{
			%this.templateDropDown.addItem(%mod.ModuleID);
		}
	}
	%this.templateDropDown.sortByText();
	%this.templateDropDown.insertItem(0, "none");
	%this.templateDropDown.setSelected(0);
}

function NewModuleDialog::onDropDownClosed(%this, %dropDown)
{
	%this.validate();
}

function NewModuleDialog::onKeyPressed(%this, %textBox)
{
	%this.validate();
}

function NewModuleDialog::onReturnPressed(%this, %textBox)
{
	%this.onCreate();
}

function NewModuleDialog::Validate(%this)
{
	%this.createButton.active = false;

	%module = %this.templateDropDown.getText();
	%name = %this.moduleNameBox.getText();
	%path = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder(), %name);

	if(%name $= "" || isDirectory(%path))
	{
		return false;
	}

	%this.moduleNameBox.text = stripChars(%name, " ");

	%this.createButton.active = true;
	return true;
}

function NewModuleDialog::onClose(%this)
{
	Canvas.popDialog(%this);
	%this.postEvent("DialogClosed", %this);
}

function NewModuleDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%module = %this.templateDropDown.getText();
		%name = %this.moduleNameBox.getText();
		%path = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder(), %name);

		%data = new ScriptObject()
		{
			template = %module;
			moduleName = %name;
			path = %path;
		};

		%this.postEvent("ModuleCreated", %data);
		%data.delete();
		%this.onClose();
	}
}
