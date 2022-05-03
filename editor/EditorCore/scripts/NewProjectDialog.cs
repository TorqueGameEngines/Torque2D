
function NewProjectDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Title", %width SPC 30);
	%this.titleBox = %form.createTextEditItem(%item);
	%this.titleBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Directory", %width SPC 30);
	%this.dirBox = %form.createTextEditItem(%item);
	%this.dirBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Description", %width SPC 130);
	%this.descBox = %form.createTextEditItem(%item);
	%this.descBox.Command = %this.getId() @ ".Validate();";

	%content.add(%form);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 170";
		Extent = (%width - 24) SPC 80;
		text = "";
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

	%this.createButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 268";
		Extent = "100 34";
		Text = "Create";
		Command = %this.getID() @ ".onCreate();";
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.feedback);
	%content.add(%this.cancelButton);
	%content.add(%this.createButton);

	%this.validate();
}

function NewProjectDialog::Validate(%this)
{
	%this.createButton.active = false;

	%title = %this.titleBox.getText();
	%directory = %this.dirBox.getText();
	%description = %this.descBox.getText();

	if(%title $= "")
	{
		%this.feedback.setText("Please enter a title for your project.");
		return false;
	}

	if(%directory $= "")
	{
		%this.feedback.setText("Please select a name for the directory that your project will launch from.");
		return false;
	}
	else
	{
		%path = makeFullPath(%directory, getMainDotCsDir());
		if(isDirectory(%path))
		{
			%fileList = getFileList(%path);
			%dirList = getDirectoryList(%path);
			if(getWordCount(%fileList) > 0 || getWordCount(%dirList) > 0)
			{
				%this.feedback.setText("The directory should be an empty directory or a non-existing directory in the main T2d directory.");
				return false;
			}
		}
	}

	if(%description $= "")
	{
		%this.feedback.setText("Please add a short, meaningful description for your project.");
		return false;
	}

	%this.createButton.active = true;
	%this.feedback.setText("Press the Create button to create your new project!");
	return true;
}

function NewProjectDialog::onClose(%this)
{
	Canvas.popDialog(%this);
	%this.postEvent("DialogClosed", %this);
}

function NewProjectDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%title = %this.titleBox.getText();
		%directory = %this.dirBox.getText();
		%description = %this.descBox.getText();

		%path = makeFullPath(%directory, getMainDotCsDir());
		if(getSubStr(%path, strlen(%path) - 1, 1) !$= "\\")
		{
			%path = %path @ "\\";
		}
		createPath(%path);

		ModuleDatabase.scanModules(pathConcat(getMainDotCsDir(), "library"));
		ModuleDatabase.CopyModule("AppCore", 1, "AppCore", pathConcat(%path, "AppCore"), false);
		ModuleDatabase.CopyModule("Audio", 1, "Audio", pathConcat(%path, "Audio"), false);
		ModuleDatabase.clearDatabase();

		%file = TamlRead(pathConcat(%path, "AppCore\\module.taml"));
		%file.Project = %title;
		%file.ProjectDescription = %description;
		TamlWrite(%file, pathConcat(%path, "AppCore\\module.taml"));

		%data = new ScriptObject()
		{
			title = %title;
			directory = %directory;
			description = %description;
			icon = pathConcat(%path, "AppCore", %file.Icon);
		};

		%this.postEvent("ProjectCreated", %directory);
		%this.onClose();
	}
}
