
function DeleteAssetDialog::init(%this, %width, %height)
{
	//Get the dialog contents
	%window = %this.getObject(0);
	%content = %window.getObject(0);

	%this.feedback = new GuiControl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "12 12";
		Extent = (%width - 24) SPC 70;
		text = "Whoa there! Once you delete this asset, it's long gone! There's no undo. Are you sure you want to move forward?";
		textWrap = true;
	};
	ThemeManager.setProfile(%this.feedback, "infoProfile");
	%content.add(%this.feedback);

	//Create the file text box
	%form = new GuiGridCtrl()
	{
		class = "EditorForm";
		extent = %width SPC %height;
		Position = "0 80";
		cellSizeX = %width / 2;
		cellSizeY = 50;
	};
	%form.addListener(%this);

	%item = %form.addFormItem("Delete Dependencies", %width SPC 30);
	%this.deleteDependenaciesBox = %form.createCheckboxItem(%item);
	%this.deleteDependenaciesBox.textExtent = (%width / 2) SPC "30";

	%item = %form.addFormItem("Delete Loose Files", %width SPC 30);
	%this.deleteLooseFilesBox = %form.createCheckboxItem(%item);
	%this.deleteLooseFilesBox.textExtent = (%width / 2) SPC "30";

	%content.add(%form);

	%this.cancelButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "478 160";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.deleteButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 158";
		Extent = "100 34";
		Text = "Delete";
		Command = %this.getID() @ ".onDelete();";
	};
	ThemeManager.setProfile(%this.deleteButton, "primaryButtonProfile");

	%content.add(%this.cancelButton);
	%content.add(%this.deleteButton);
}

function DeleteAssetDialog::onClose(%this)
{
	Canvas.popDialog(%this);
}

function DeleteAssetDialog::onDelete(%this)
{
	%assetID = %this.doomedAsset.getAssetId();
	%deleteDependants = %this.deleteDependenaciesBox.getStateOn();

	//Remove the asset...
	AssetDatabase.deleteAsset(%assetID, %this.deleteLooseFilesBox.getStateOn(), %deleteDependants);

	//Remove the button, but we don't know who has it. So just try them all...
	%killedAnImage = AssetAdmin.Dictionary["ImageAsset"].removeButton(%assetID);
	%killedAnAnimation = AssetAdmin.Dictionary["AnimationAsset"].removeButton(%assetID);
	AssetAdmin.Dictionary["ParticleAsset"].removeButton(%assetID);
	AssetAdmin.Dictionary["FontAsset"].removeButton(%assetID);
	AssetAdmin.Dictionary["AudioAsset"].removeButton(%assetID);

	if(%killedAnImage && %deleteDependants)
	{
		AssetAdmin.Dictionary["AnimationAsset"].reload();
		AssetAdmin.Dictionary["FontAsset"].reload();
	}

	if((%killedAnImage || %killedAnAnimation) && %deleteDependants)
	{
		AssetAdmin.Dictionary["ParticleAsset"].reload();
	}

	AssetAdmin.inspector.hideInspector();

	AssetAdmin.AssetScene.clear(true);
	AssetAdmin.audioPlayButtonContainer.setVisible(false);
	AssetAdmin.AssetWindow.setVisible(false);

	%this.onClose();
}
