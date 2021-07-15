
function NewAnimationAssetDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Image Asset", %width SPC 30);
	%this.imageDropDown = %form.createDropDownItem(%item);
	%this.populateImageDropDown();

	%item = %form.addFormItem("Target Folder", %width SPC 30);
	%this.folderBox = %form.createFolderOpenItem(%item, "Select Target Folder");
	%this.folderBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Asset Name", %width SPC 30);
	%this.assetNameBox = %form.createTextEditItem(%item);
	%this.assetNameBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Target Module", %width SPC 30);
	%this.moduleNameBox = %form.createTextEditItem(%item);
	%this.moduleNameBox.setActive(false);

	%content.add(%form);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 220";
		Extent = (%width - 24) SPC 80;
		text = "Select an Image Asset to get started!";
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

	%this.createButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 318";
		Extent = "100 34";
		Text = "Create";
		Command = %this.getID() @ ".onCreate();";
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.feedback);
	%content.add(%this.cancelButton);
	%content.add(%this.createButton);

	%this.prevFolder = "";
}

function NewAnimationAssetDialog::Validate(%this)
{
	%this.createButton.active = false;

	%assetID = %this.imageDropDown.getText();
	if(%assetID $= "")
	{
		%this.feedback.setText("Select an Image Asset to get started!");
		return false;
	}
	%imageAssetFilePath = AssetDatabase.getAssetFilePath(%assetID);

	if(%this.folderBox.getText() $= "")
	{
		%this.folderBox.setText(makeRelativePath(filePath(%imageAssetFilePath), getMainDotCsDir()));
	}

	if(%this.assetNameBox.getText() $= "")
	{
		%this.assetNameBox.setText(getUnit(%assetID, 1, ":") @ "_AN");
	}

	%folderPath = %this.getFolderPath();
	%assetName = %this.assetNameBox.getText();

	if(%folderPath !$= %this.prevFolder)
	{
		%modSig = EditorCore.findModuleOfPath(%folderPath @ "a.txt");
		%this.moduleNameBox.setText(%modSig);
		%this.prevFolder = %folderPath;
	}
	%assetPath = %folderPath @ %assetName @ ".asset.taml";
	%moduleName = getUnit(%this.moduleNameBox.getText(), 0, "_");
	%moduleVersion = getUnit(%this.moduleNameBox.getText(), 1, "_");
	%assetID = %moduleName @ ":" @ %assetName;

	if(%folderPath $= "")
	{
		%this.feedback.setText("Please select a target folder.");
		return false;
	}

	if(%assetName $= "")
	{
		%this.feedback.setText("An animation asset must have an Asset Name.");
		return false;
	}

	if(%moduleName $= "")
	{
		%this.feedback.setText("You can only create an animation asset inside of a module.");
		return false;
	}

	%button = AssetAdmin.Dictionary["AnimationAsset"].getButton(%assetID);
	if(isObject(%button))
	{
		%this.feedback.setText("An asset by this name already exists in this module. Try choosing a different name.");
		return false;
	}

	%this.createButton.active = true;
	%this.feedback.setText("Press the Create button to open the new asset for editing.");
	return true;
}

function NewAnimationAssetDialog::onClose(%this)
{
	Canvas.popDialog(%this);
}

function NewAnimationAssetDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%folderPath = %this.getFolderPath();
		%assetName = %this.assetNameBox.getText();
		%assetPath = %folderPath @ %assetName @ ".asset.taml";
		%moduleName = getUnit(%this.moduleNameBox.getText(), 0, "_");
		%moduleVersion = getUnit(%this.moduleNameBox.getText(), 1, "_");
		%assetID = %moduleName @ ":" @ %assetName;

		//Time to create a new file
		%newAsset = new AnimationAsset()
		{
			assetName = %assetName;
			Image = %this.imageDropDown.getText();
			AnimationFrames = "0 1";
			AnimationTime = "1";
			AnimationCycle = "1";
		};

		%assetImportSuccessful = TAMLWrite(%newAsset, %assetPath);

		%moduleDef = ModuleDatabase.findModule(%moduleName, %moduleVersion);
		AssetDatabase.addDeclaredAsset(%moduleDef, %assetPath);

		//Do we already have this button?
		%button = AssetAdmin.Dictionary["AnimationAsset"].getButton(%assetID);

		if(!isObject(%button))
		{
			//Load it into the image dictionary
			%button = AssetAdmin.Dictionary["AnimationAsset"].addButton(%assetID);
		}
		%button.onClick();
		%this.onClose();
	}
}

function NewAnimationAssetDialog::onFolderOpened(%this, %textBox)
{
	%this.Validate();
}

function NewAnimationAssetDialog::getFolderPath(%this)
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

function NewAnimationAssetDialog::onDropDownClosed(%this, %dropDown)
{
	%this.validate();
}

function NewAnimationAssetDialog::populateImageDropDown(%this)
{
	%this.imageDropDown.clearItems();

	%query = new AssetQuery();
	AssetDatabase.findAllAssets(%query);
	AssetDatabase.findAssetType(%query, "ImageAsset", true);

	for(%i = 0; %i < %query.getCount(); %i++)
	{
		%assetID = %query.getAsset(%i);

		if(!AssetDatabase.isAssetInternal(%assetID))
		{
			%imageAsset = AssetDatabase.acquireAsset(%assetID);

			if(%imageAsset.getFrameCount() > 1)
			{
				%this.imageDropDown.addItem(%assetID);
			}
		}
	}
	%query.delete();

	%this.imageDropDown.sortByText();
	%this.imageDropDown.insertItem(0, "");
	%this.imageDropDown.setSelected(0);
}