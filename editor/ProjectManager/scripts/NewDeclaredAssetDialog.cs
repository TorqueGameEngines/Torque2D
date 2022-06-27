
function NewDeclaredAssetDialog::init(%this, %width, %height)
{
	//Get the dialog contents
	%window = %this.getObject(0);
	%content = %window.getObject(0);

	//Create the file text box
	%form = new GuiGridCtrl()
	{
		class = "DeclaredAssetForm";
		superclass = "EditorForm";
		extent = %width SPC %height;
		cellSizeX = %width;
		cellSizeY = 50;
		module = %this.module;
	};
	%form.addListener(%this);

	%item = %form.addFormItem("Asset Directory", %width SPC 30);
	%this.folderBox = %form.createFolderOpenItem(%item, "Select Folder to Scan");
	%this.folderBox.Command = %this.getId() @ ".Validate();";

	%item = %form.addFormItem("Extension", %width SPC 30);
	%this.extensionBox = %form.createTextEditItem(%item);
	%this.extensionBox.text = "asset.taml";

	%item = %form.addFormItem("Scan Sub Directories", %width SPC 30);
	%this.recursiveCheckBox = %form.createCheckboxItem(%item);

	%content.add(%form);

	%this.cancelButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "278 170";
		Extent = "100 30";
		Text = "Cancel";
		Command = %this.getID() @ ".onClose();";
	};
	ThemeManager.setProfile(%this.cancelButton, "buttonProfile");

	%this.createButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "388 168";
		Extent = "100 34";
		Text = "Add";
		Command = %this.getID() @ ".onCreate();";
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.cancelButton);
	%content.add(%this.createButton);

	%this.validate();
}

function NewDeclaredAssetDialog::onFolderOpened(%this, %textBox)
{
	%this.Validate();
}

function NewDeclaredAssetDialog::onKeyPressed(%this, %textBox)
{
	%this.validate();
}

function NewDeclaredAssetDialog::onReturnPressed(%this, %textBox)
{
	%this.onCreate();
}

function NewDeclaredAssetDialog::Validate(%this)
{
	%this.createButton.active = false;

	%folder = %this.folderBox.getText();
	%ext = %this.extensionBox.getText();
	%isRecursive = %this.recursiveCheckBox.getStateOn();

	if(%ext $= "")
	{
		return false;
	}

	%this.createButton.active = true;
	return true;
}

function NewDeclaredAssetDialog::onClose(%this)
{
	Canvas.popDialog(%this);
	%this.postEvent("DialogClosed", %this);
}

function NewDeclaredAssetDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%folder = %this.folderBox.getText();
		%ext = %this.extensionBox.getText();
		%isRecursive = %this.recursiveCheckBox.getStateOn();

		%asset = new DeclaredAssets()
		{
			path = %folder;
			extension = %ext;
			recurse = %isRecursive;
		};

		%this.postEvent("DeclaredAssetAdded", %asset);
		%this.onClose();
	}
}
