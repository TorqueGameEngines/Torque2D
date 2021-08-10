
function NewParticleEmitterDialog::init(%this, %width, %height)
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

	%item = %form.addFormItem("Animation Asset", %width SPC 30);
	%this.animationDropDown = %form.createDropDownItem(%item);
	%this.populateAnimationDropDown();

	%item = %form.addFormItem("Emitter Name", %width SPC 30);
	%this.emitterNameBox = %form.createTextEditItem(%item);
	%this.emitterNameBox.Command = %this.getId() @ ".Validate();";

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

	%this.createButton = new GuiButtonCtrl()
	{
		HorizSizing = "right";
		VertSizing = "bottom";
		Position = "588 158";
		Extent = "100 34";
		Text = "Create";
		Command = %this.getID() @ ".onCreate();";
		Active = false;
	};
	ThemeManager.setProfile(%this.createButton, "primaryButtonProfile");

	%content.add(%this.cancelButton);
	%content.add(%this.createButton);
}

function NewParticleEmitterDialog::Validate(%this)
{
	%this.createButton.active = false;

	%emitterName = %this.emitterNameBox.getText();

	%imageAssetID = %this.imageDropDown.getText();
	%animationAssetID = %this.animationDropDown.getText();

	if(%emitterName !$= "" && (%imageAssetID $= "" || %animationAssetID $= ""))
	{
		%this.createButton.active = true;
		return true;
	}
	return false;
}

function NewParticleEmitterDialog::onClose(%this)
{
	Canvas.popDialog(%this);
}

function NewParticleEmitterDialog::onCreate(%this)
{
	if(%this.validate())
	{
		%emitter = %this.parentAsset.createEmitter();
		if(%emitter != 0)
		{
			%emitter.setEmitterName(%this.emitterNameBox.getText());

			if(%this.imageDropDown.getText() !$= "")
			{
				%emitter.setImage(%this.imageDropDown.getText());
			}
			else
			{
				%emitter.setAnimation(%this.animationDropDown.getText());
			}

			%i = %this.parentAsset.getEmitterCount();
			AssetAdmin.Inspector.titleDropDown.addItem("Emitter:" SPC %emitter.EmitterName);
			AssetAdmin.Inspector.titleDropDown.setItemColor(%i, ThemeManager.activeTheme.color5);
			AssetAdmin.Inspector.titleDropDown.setSelected(%i);
			AssetAdmin.Inspector.onChooseParticleAsset(%this.parentAsset);
		}
		else
		{
			warn("NewParticleEmitterDialog::onCreate - No emitter created!");
		}

		%this.onClose();
	}
}

function NewParticleEmitterDialog::onDropDownClosed(%this, %dropDown)
{
	if(%dropDown == %this.imageDropDown && %dropDown.getText() !$= "")
	{
		%this.animationDropDown.setSelected(0);
	}
	else if(%dropDown == %this.animationDropDown && %dropDown.getText() !$= "")
	{
		%this.imageDropDown.setSelected(0);
	}

	%this.validate();
}

function NewParticleEmitterDialog::populateImageDropDown(%this)
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
			%this.imageDropDown.addItem(%assetID);
		}
	}
	%query.delete();

	%this.imageDropDown.sortByText();
	%this.imageDropDown.insertItem(0, "");
	%this.imageDropDown.setSelected(0);
}

function NewParticleEmitterDialog::populateAnimationDropDown(%this)
{
	%this.animationDropDown.clearItems();

	%query = new AssetQuery();
	AssetDatabase.findAllAssets(%query);
	AssetDatabase.findAssetType(%query, "AnimationAsset", true);

	for(%i = 0; %i < %query.getCount(); %i++)
	{
		%assetID = %query.getAsset(%i);

		if(!AssetDatabase.isAssetInternal(%assetID))
		{
			%this.animationDropDown.addItem(%assetID);
		}
	}
	%query.delete();

	%this.animationDropDown.sortByText();
	%this.animationDropDown.insertItem(0, "");
	%this.animationDropDown.setSelected(0);
}
