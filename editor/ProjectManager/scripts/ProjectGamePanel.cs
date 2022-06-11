
function ProjectGamePanel::onAdd(%this)
{
	%this.init("Project");

	%this.buttonBar.addButton("createNewModule", 11, "Create Module", "");
	%this.buttonBar.addButton("editModule", 49, "Edit Module", "editModuleAvailable");
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
	else
	{
		%this.buttonBar.refreshEnabled();
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

function ProjectGamePanel::createNewModule(%this)
{
	%width = 500;
	%height = 190;
	%dialog = new GuiControl()
	{
		class = "NewModuleDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "Create Module";
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
}

function ProjectGamePanel::onDialogClosed(%this, %dialog)
{
	%this.dialog = %dialog;
	%this.schedule(100, "deleteDialog");
}

function ProjectGamePanel::deleteDialog(%this)
{
	%this.dialog.delete();
}

function ProjectGamePanel::onModuleCreated(%this, %data)
{
	if(%data.template !$= "none")
	{
		%templatePath = pathConcat(getMainDotCsDir(), "library", %data.template);
		if(isDirectory(%templatePath))
		{
			pathCopy(%templatePath, %data.path);
			%obj = TamlRead(pathConcat(%data.path, "module.taml"));
			%obj.ModuleID = %data.moduleName;
			%obj.Type = "";
			TamlWrite(%obj, pathConcat(%data.path, "module.taml"));
		}
	}
	else
	{
		%obj = new ModuleDefinition()
		{
			ModuleID = %data.moduleName;
			VersionID = "1";
			BuildID = "1";
			Synchronized = false;
			Description = "";
			Author = "";
		};
		createPath(%data.path);
		TamlWrite(%obj, pathConcat(%data.path, "module.taml"));
	}
	ModuleDatabase.scanModules(%data.path);
	%this.onOpen(ModuleDatabase.findModules(false));
}

function ProjectGamePanel::editModule(%this)
{
	%width = 500;
	%height = 520;
	%dialog = new GuiControl()
	{
		class = "EditModuleDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "Edit Module";
		module = %this.card.activeModule;
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
}

function ProjectGamePanel::editModuleAvailable(%this)
{
	if(isObject(%this.card.activeModule) && !%this.card.activeModule.Synchronized)
	{
		return true;
	}
	return false;
}

function ProjectGamePanel::onModuleEdited(%this, %data)
{
	if(%this.moduleHasChanged(%data))
	{
		%module = %this.card.activeModule;
		%moduleID = %this.card.activeModule.moduleID;
		%projectPath = ProjectManager.getProjectFolder();
		%modulePath = pathConcat(%projectPath, %module.moduleID);
		if(ModuleDatabase.isModuleLoaded(%module.moduleID))
		{
			if(%module.group !$= "")
			{
				ModuleDatabase.unloadGroup(%module.group);
			}
			else
			{
				ModuleDatabase.unloadExplicit(%module.moduleID);
			}
		}
		ModuleDatabase.unregisterModule(%module.moduleID, %module.versionID);
		%newModulePath = pathConcat(%projectPath, %data.moduleID);
		if(%moduleID !$= %data.moduleID && !isDirectory(%newModulePath))
		{
			if(pathCopy(%modulePath, %newModulePath))
			{
				directoryDelete(%modulePath);
				%modulePath = %newModulePath;
			}
		}
		echo("Editing Module at " @ %modulePath);
		%file = TamlRead(pathConcat(%modulePath, "module.taml"));
		%file.moduleID = %data.moduleID;
		%file.versionID = %data.versionID;
		%file.buildID = %data.buildID;
		%file.description = %data.description;
		%file.type = %data.type;
		%file.author = %data.author;
		TamlWrite(%file, pathConcat(%modulePath, "module.taml"));
		ModuleDatabase.scanModules(%modulePath, true);
		%this.card.moduleID = %data.moduleID;
		%this.card.versionID = %data.versionID;
		%allModules = ModuleDatabase.findModules(false);
		%this.onOpen(%allModules);
	}
}

function ProjectGamePanel::moduleHasChanged(%this, %data)
{
	%module = %this.card.activeModule;
	return %module.moduleID !$= %data.moduleID ||
		%module.versionID !$= %data.versionID ||
		%module.buildID !$= %data.buildID ||
		%module.description !$= %data.description ||
		%module.type !$= %data.type ||
		%module.author !$= %data.author;
}
