
function ProjectGamePanel::onAdd(%this)
{
	%this.init("Project");
}

function ProjectGamePanel::onOpen(%this, %allModules)
{
	%appCore = ModuleDatabase.findModule("AppCore", 1);
	if(isObject(%appCore))
	{
		%this.setTitle(%appCore.project);
	}

	%loadedModules = ModuleDatabase.findModules(true);

	%this.clearModules();
	for(%i = 0; %i < getWordCount(%allModules); %i++)
	{
		%mod = getWord(%allModules, %i);
		%this.addModule(%mod, %loadedModules);
	}
	%this.list.sortByText();

	if(%this.card.visible)
	{
		%this.refreshCard();
	}
}

function ProjectGamePanel::addModule(%this, %module, %loadedModules)
{
	%color = %this.gray;
	if(%module.Deprecated)
	{
		%color = %this.darkRed;
	}
	for(%i = 0; %i < getWordCount(%loadedModules); %i++)
	{
		%loadedModule = getWord(%loadedModules, %i);
		if(%loadedModule == %module)
		{
			%color = %this.yellow;
			if(%module.Deprecated)
			{
				%color = %this.darkRed;
			}
		}
	}

	%this.list.addItemWithID(%this.getModuleName(%module), %module);
	%index = %this.list.findItemID(%module);
	%this.list.setItemColor(%index, %color);
}

function ProjectGamePanel::refreshCard(%this)
{
	%module = ModuleDatabase.findModule(%this.card.moduleID, %this.card.versionID);
	%indexList = %this.list.findItemText(%this.getModuleName(%module));
	%index = getWord(%indexList, 0);
	if(%index != -1)
	{
		%this.list.setCurSel(%index);
	}
	%this.card.show(%module);
}
