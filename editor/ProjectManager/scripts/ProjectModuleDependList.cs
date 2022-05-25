
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

	for(%i = 0; %i < %module.getdependencyCount(); %i++)
	{
		%dep = %module.getDependency(%i);
		%name = getWord(%dep, 0);
		%version = getWord(%dep, 1);
		if(%version $= "*" || %version $= "0")
		{
			%version = "Latest";
		}
		%this.addDependItem(%name, %version);
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

function ProjectModuleDependList::addDependItem(%this, %name, %version)
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
