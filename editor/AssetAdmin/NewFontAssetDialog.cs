
function NewFontAssetDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Font File", %width SPC 30);
	%this.imageFileBox = %form.createFileOpenItem(%item, "Bitmap Font (*.FNT)|*.FNT", "Open Bitmap Font File");
	%this.imageFileBox.Command = %this.getId() @ ".Validate();";

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
		Position = "12 170";
		Extent = (%width - 24) SPC 80;
		text = "Select a Bimap Font File to get started!";
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

	%this.prevFile = "";
}

function NewFontAssetDialog::Validate(%this)
{
	%this.createButton.active = false;

	%file = %this.imageFileBox.getText();
	%assetName = %this.assetNameBox.getText();

	if(%file !$= %this.prevFile)
	{
		//remove the extention
		%fileNameSansExt = fileBase(fileName(%file));
		if(%fileNameSansExt !$= "" && %assetName $= "")
		{
			%assetName = %fileNameSansExt;
			%this.assetNameBox.setText(%fileNameSansExt);
		}
		%modSig = EditorCore.findModuleOfPath(%file);
		%this.moduleNameBox.setText(%modSig);
		%this.prevFile = %file;
	}
	%assetPath = filePath(%file) @ "/" @ %assetName @ ".asset.taml";
	%moduleName = getUnit(%this.moduleNameBox.getText(), 0, "_");
	%moduleVersion = getUnit(%this.moduleNameBox.getText(), 1, "_");
	%assetID = %moduleName @ ":" @ %assetName;

	if(!isFile(%file))
	{
		//We need a real image file!
		%this.feedback.setText("An existing font file must be used to create a font asset!");
		return false;
	}

	if(%file $= "")
	{
		%this.feedback.setText("Select a Bitmap Font File to get started!");
		return false;
	}

	if(%assetName $= "")
	{
		%this.feedback.setText("A font asset must have an Asset Name.");
		return false;
	}

	if(%moduleName $= "")
	{
		%this.feedback.setText("You can only create a font asset inside of a module.");
		return false;
	}

	%button = AssetAdmin.Dictionary["FontAsset"].getButton(%assetID);
	if(isObject(%button))
	{
		%this.feedback.setText("An asset by this name already exists in this module. Try choosing a different name.");
		return false;
	}

	%this.createButton.active = true;
	%this.feedback.setText("Press the Create button to open the new asset for editing.");
	return true;
}

function NewFontAssetDialog::onClose(%this)
{
	Canvas.popDialog(%this);
}

function NewFontAssetDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%file = makeFullPath(%this.imageFileBox.getText());
		%assetName = %this.assetNameBox.getText();
		%assetPath = filePath(%file) @ "/" @ %assetName @ ".asset.taml";
		%moduleName = getUnit(%this.moduleNameBox.getText(), 0, "_");
		%moduleVersion = getUnit(%this.moduleNameBox.getText(), 1, "_");
		%assetID = %moduleName @ ":" @ %assetName;

		//Time to create a new file
		%newAsset = new FontAsset()
		{
			assetName = %assetName;
			fontFile = %file;
		};

		%assetImportSuccessful = TAMLWrite(%newAsset, %assetPath);

		%moduleDef = ModuleDatabase.findModule(%moduleName, %moduleVersion);
		AssetDatabase.addDeclaredAsset(%moduleDef, %assetPath);

		//Refresh the asset so that the loose file will be a path relative to the asset file.
		AssetDatabase.refreshAsset(%assetID);

		//Do we already have this button?
		%button = AssetAdmin.Dictionary["FontAsset"].getButton(%assetID);

		if(!isObject(%button))
		{
			//Load it into the image dictionary
			%button = AssetAdmin.Dictionary["FontAsset"].addButton(%assetID);
		}
		%button.onClick();
		%this.onClose();
	}
}

function NewFontAssetDialog::onFileOpened(%this, %textBox)
{
	%this.Validate();
}
