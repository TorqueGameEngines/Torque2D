
function ProjectLibraryPanel::onAdd(%this)
{
	%this.init("Library");
}

function ProjectLibraryPanel::load(%this)
{
	%this.manager = new ModuleManager();
	%this.manager.addListener(AssetDatabase);
	%this.manager.EchoInfo = false;

	%this.manager.ScanModules(pathConcat(getMainDotCsDir(), "library"));

	%allModules = %this.manager.findModules(false);

	for(%i = 0; %i < getWordCount(%allModules); %i++)
	{
		%mod = getWord(%allModules, %i);
		%this.addModule(%mod);
	}
	%this.list.sortByText();
}

function ProjectLibraryPanel::addModule(%this, %module)
{
	%this.list.addItemWithID(%this.getModuleName(%module), %module);
}

function ProjectLibraryPanel::onOpen(%this, %allModules)
{
	%count = %this.list.getItemCount();
	for(%i = 0; %i < %count; %i++)
	{
		%module = %this.list.getItemID(%i);
		%this.refreshColor(%module, %i, %allModules);
	}
}

function ProjectLibraryPanel::refreshColor(%this, %module, %index, %projectModules)
{
	%color = %this.gray;
	if(%module.Deprecated)
	{
		%color = %this.darkRed;
	}
	for(%i = 0; %i < getWordCount(%projectModules); %i++)
	{
		%projectModule = getWord(%projectModules, %i);
		if(%projectModule.ModuleId $= %module.ModuleId && %projectModule.VersionId $= %module.VersionId)
		{
			%color = %this.yellow;
			if(%module.Deprecated)
			{
				%color = %this.darkRed;
			}
			else if(%module.BuildID > %projectModule.BuildID)
			{
				%color = %this.purple;
			}
		}
	}

	%this.list.setItemColor(%index, %color);
}

function ProjectLibraryPanel::onInstallClick(%this)
{
	%index = %this.list.getSelectedItem();
	%module = %this.list.getItemID(%index);

	if(isObject(%module))
	{
		%newModulePath = pathConcat(ProjectManager.getProjectFolder(), %module.moduleID);
		%this.manager.CopyModule(%module.moduleID, %module.versionID, %module.moduleID, %newModulePath, false);
		//%this.manager.synchronizeDependencies(%module, pathConcat(ProjectManager.getProjectFolder()));

		%file = TamlRead(pathConcat(%newModulePath, "module.taml"));
		%file.Group = "";
		TamlWrite(%file, pathConcat(%newModulePath, "module.taml"));

		ModuleDatabase.ScanModules(%newModulePath);
		ModuleDatabase.LoadExplicit(%module.moduleID, %module.versionID);
		%installedModule = ModuleDatabase.findModule(%module.moduleID, %module.versionID);

		%this.list.setItemColor(%index, "255 255 100 255");

		%file = TamlRead(pathConcat(%newModulePath, "module.taml"));
		%file.Group = "gameBase";
		TamlWrite(%file, pathConcat(%newModulePath, "module.taml"));

		%this.postEvent("ModuleInstalled", %installedModule);
	}
}

function ProjectLibraryPanel::onUpdateClick(%this)
{

}
