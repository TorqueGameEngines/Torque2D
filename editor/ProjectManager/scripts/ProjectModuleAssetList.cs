
function ProjectModuleAssetList::onAdd(%this)
{
	%this.startListening(ThemeManager);
}

function ProjectModuleAssetList::onThemeChange(%this, %theme)
{
	if(isObject(%this.activeModule))
	{
		%this.show(%this.activeModule);
	}
}

function ProjectModuleAssetList::show(%this, %module)
{
	%this.activeModule = %module;
	%this.clearItems();
	%isSynced = %module.Synchronized;

	for(%i = 0; %i < %module.getCount(); %i++)
	{
		%asset = %module.getObject(%i);
		%this.addAssetItem(%asset, %isSynced);
	}

	if(!%module.Synchronized)
	{
		%this.addAddButton();
	}
}

function ProjectModuleAssetList::hide(%this)
{
	%this.activeModule = 0;
	%this.clearItems();
}

function ProjectModuleAssetList::clearItems(%this)
{
	for(%i = %this.getCount() - 1; %i >= 0; %i--)
	{
		%item = %this.getObject(%i);
		%item.delete();
	}
}

function ProjectModuleAssetList::getDeclaredAssetText(%this, %asset)
{
	if(%asset.recurse)
	{
		return pathConcat(%asset.path, "*", "*." @ %asset.extension);
	}
	return pathConcat(%asset.path, "*." @ %asset.extension);
}

function ProjectModuleAssetList::addAssetItem(%this, %asset, %isSynced)
{
	%width = getWord(%this.extent, 0) - 6;
	%text = new GuiControl()
	{
		HorizSizing="width";
		Position = "3 0";
		Extent = %width SPC "26";
		Text = %this.getDeclaredAssetText(%asset);
		Align = "Left";
		VAlign = "Middle";
	};
	ThemeManager.setProfile(%text, "subListProfile");
	%this.add(%text);

	if(!%isSynced)
	{
		%deleteButton = new GuiButtonCtrl()
		{
			Class="ProjectModuleAssetButton";
			ButtonEvent = "RemoveAsset";
			ButtonData = %asset.getID();
			HorizSizing="left";
			Position = (%width - 40) SPC "0";
			Extent = "22 22";
			MinExtent = "22 22";
			Text = "X";
			Align = "Center";
			VAlign = "Middle";
			TextExtend = 1;
		};
		%deleteButton.position = (%width - (getWord(%deleteButton.extent, 0) + 10)) SPC "0";
		ThemeManager.setProfile(%deleteButton, "subListProfile");
		%text.add(%deleteButton);
		%this.startListening(%deleteButton);
	}
}

function ProjectModuleAssetList::addAddButton(%this)
{
	%width = 140;
	%addButton = new GuiButtonCtrl()
	{
		Class="ProjectModuleAssetButton";
		ButtonEvent = "AddAsset";
		ButtonData = "";
		HorizSizing="right";
		Position = "3 0";
		Extent = %width SPC "26";
		MinExtent = %width SPC "26";
		Text = "+Add Declared Asset";
		Align = "Center";
		VAlign = "Middle";
		TextExtend = 1;
	};
	ThemeManager.setProfile(%addButton, "subListProfile");
	%this.add(%addButton);
	%this.startListening(%addButton);
}

function ProjectModuleAssetButton::onClick(%this)
{
	%this.postEvent(%this.buttonEvent, %this.buttonData);
}

function ProjectModuleAssetList::onAddAsset(%this)
{
	%width = 500;
	%height = 240;
	%dialog = new GuiControl()
	{
		class = "NewDeclaredAssetDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Declared Asset";
		module = %this.activeModule;
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
}

function ProjectModuleAssetList::onDeclaredAssetAdded(%this, %asset)
{
	%this.activeModule.add(%asset);
	%this.activeModule.save();
	%this.show(%this.activeModule);
}

function ProjectModuleAssetList::onRemoveAsset(%this, %asset)
{
	%this.activeModule.remove(%asset);
	%this.activeModule.save();
	%this.schedule(50, "show", %this.activeModule);
}

function ProjectModuleAssetList::onDialogClosed(%this, %dialog)
{
	%this.dialog = %dialog;
	%this.schedule(100, "deleteDialog");
}

function ProjectModuleAssetList::deleteDialog(%this)
{
	%this.dialog.delete();
}
