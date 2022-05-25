
function ProjectModuleDependList::onAdd(%this)
{
	%this.startListening(ThemeManager);
}

function ProjectModuleDependList::onThemeChange(%this, %theme)
{
	if(isObject(%this.activeModule))
	{
		%this.show(%this.activeModule);
	}
}

function ProjectModuleDependList::show(%this, %module)
{
	%this.activeModule = %module;
	%this.clearItems();
	%isSynced = %module.Synchronized;

	for(%i = 0; %i < %module.getdependencyCount(); %i++)
	{
		%dep = %module.getDependency(%i);
		%name = getWord(%dep, 0);
		%version = getWord(%dep, 1);
		if(%version $= "*" || %version $= "0")
		{
			%version = "Latest";
		}
		%this.addDependItem(%name, %version, %isSynced);
	}

	if(!%module.Synchronized)
	{
		%this.addAddButton();
	}
}

function ProjectModuleDependList::hide(%this)
{
	%this.activeModule = 0;
	%this.clearItems();
}

function ProjectModuleDependList::clearItems(%this)
{
	for(%i = %this.getCount() - 1; %i >= 0; %i--)
	{
		%item = %this.getObject(%i);
		%item.delete();
	}
}

function ProjectModuleDependList::addDependItem(%this, %name, %version, %isSynced)
{
	%width = getWord(%this.extent, 0) - 6;
	%text = new GuiControl()
	{
		HorizSizing="width";
		Position = "3 0";
		Extent = %width SPC "26";
		Text = %name @ ":" @ %version;
		Align = "Left";
		VAlign = "Middle";
	};
	ThemeManager.setProfile(%text, "subListProfile");
	%this.add(%text);

	if(!%isSynced)
	{
		%deleteButton = new GuiButtonCtrl()
		{
			Class="ProjectModuleDependButton";
			ButtonEvent = "RemoveDepend";
			ButtonData = %name;
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

function ProjectModuleDependList::addAddButton(%this)
{
	%width = 140;
	%addButton = new GuiButtonCtrl()
	{
		Class="ProjectModuleDependButton";
		ButtonEvent = "AddDepend";
		ButtonData = "";
		HorizSizing="right";
		Position = "3 0";
		Extent = %width SPC "26";
		MinExtent = %width SPC "26";
		Text = "+Add Dependency";
		Align = "Center";
		VAlign = "Middle";
		TextExtend = 1;
	};
	ThemeManager.setProfile(%addButton, "subListProfile");
	%this.add(%addButton);
	%this.startListening(%addButton);
}

function ProjectModuleDependButton::onClick(%this)
{
	%this.postEvent(%this.buttonEvent, %this.buttonData);
}

function ProjectModuleDependList::onAddDepend(%this)
{
	%width = 500;
	%height = 190;
	%dialog = new GuiControl()
	{
		class = "NewDependencyDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "New Dependency";
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
}

function ProjectModuleDependList::onDependencyAdded(%this, %data)
{
	%this.activeModule.addDependency(%data.module, %data.version);
	%this.activeModule.save();
	%this.show(%this.activeModule);
}

function ProjectModuleDependList::onRemoveDepend(%this, %data)
{
	%this.activeModule.removeDependency(%data);
	%this.activeModule.save();
	%this.schedule(50, "show", %this.activeModule);
}

function ProjectModuleDependList::onDialogClosed(%this, %dialog)
{
	%this.dialog = %dialog;
	%this.schedule(100, "deleteDialog");
}

function ProjectModuleDependList::deleteDialog(%this)
{
	%this.dialog.delete();
}
