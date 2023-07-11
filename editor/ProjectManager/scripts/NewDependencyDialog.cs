
function NewDependencyDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Dependency", %width SPC 30);
	%this.moduleDropDown = %form.createDropDownItem(%item);

	%item = %form.addFormItem("Version", %width SPC 30);
	%this.versionDropDown = %form.createDropDownItem(%item);

	%this.populateModuleDropDown();
	%this.populateVersionDropDown();

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
		Text = "Add";
		Command = %this.getID() @ ".onCreate();";
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.cancelButton);
	%content.add(%this.createButton);

	%this.validate();
}

function NewDependencyDialog::populateModuleDropDown(%this)
{
	%this.moduleDropDown.clearItems();

	for(%i = 0; %i < getWordCount(%this.dependList); %i++)
	{
		%mod = getWord(%this.dependList, %i);
		if(%mod.ModuleID !$= "AppCore")
		{
			if(%this.versionList[%mod.ModuleID] $= "")
			{
				%this.moduleDropDown.addItem(%mod.ModuleID);
				%this.versionList[%mod.ModuleID] = %mod.VersionID;
			}
			else
			{
				%this.versionList[%mod.ModuleID] = %this.versionList[%mod.ModuleID] SPC %mod.VersionID;
			}
		}
	}
	%this.moduleDropDown.sortByText();
	%this.moduleDropDown.insertItem(0, "");
	%this.moduleDropDown.setSelected(0);
}

function NewDependencyDialog::onDropDownClosed(%this, %dropDown)
{
	%this.validate();
}

function NewDependencyDialog::onDropDownSelect(%this, %dropDown)
{
	if(%dropDown == %this.moduleDropDown)
	{
		%this.populateVersionDropDown();
	}
}

function NewDependencyDialog::populateVersionDropDown(%this)
{
	%this.versionDropDown.clearItems();

	if(%this.moduleDropDown.getText() !$= "")
	{
		%versionList = %this.versionList[%this.moduleDropDown.getText()];
		for(%i = 0; %i < getWordCount(%versionList); %i++)
		{
			%version = getWord(%versionList, %i);
			%this.versionDropDown.addItem(%version);
		}
		%this.versionDropDown.sortByText();
		%this.versionDropDown.insertItem(0, "Latest");
		%this.versionDropDown.setSelected(0);
	}
}

function NewDependencyDialog::Validate(%this)
{
	%this.createButton.active = false;

	%module = %this.moduleDropDown.getText();

	if(%module $= "")
	{
		return false;
	}

	%this.createButton.active = true;
	return true;
}

function NewDependencyDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%module = %this.moduleDropDown.getText();
		%version = %this.versionDropDown.getText();
		if(%version $= "Latest")
		{
			%version = "0";
		}

		%data = new ScriptObject()
		{
			module = %module;
			version = %version;
		};

		%this.postEvent("DependencyAdded", %data);
		%data.delete();
		%this.onClose();
	}
}
