//NewAssetButton.cs

function NewAssetButton::onClick(%this)
{
	%this.call("onNew" @ %this.type);
}

function NewAssetButton::onNewImageAsset(%this)
{
	%width = 700;
	%height = 340;
	%dialog = new GuiControl()
	{
		class = "NewImageAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Image Asset";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}

function NewAssetButton::onNewAnimationAsset(%this)
{
	%width = 700;
	%height = 390;
	%dialog = new GuiControl()
	{
		class = "NewAnimationAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Animation Asset";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}

function NewAssetButton::onNewParticleAsset(%this)
{
	%width = 700;
	%height = 440;
	%dialog = new GuiControl()
	{
		class = "NewParticleAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Particle Asset";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}

function NewAssetButton::onNewFontAsset(%this)
{
	%width = 700;
	%height = 340;
	%dialog = new GuiControl()
	{
		class = "NewFontAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Bitmap Font Asset";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}

function NewAssetButton::onNewAudioAsset(%this)
{
	%width = 700;
	%height = 340;
	%dialog = new GuiControl()
	{
		class = "NewAudioAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Audio Asset";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}
