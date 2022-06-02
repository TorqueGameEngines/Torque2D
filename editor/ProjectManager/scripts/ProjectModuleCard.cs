
 function ProjectModuleCard::onAdd(%this)
 {
	 %this.toggleLaunchGroupCount = 0;
	 %this.chain = new GuiChainCtrl()
	 {
		 HorizSizing="width";
 		 VertSizing="height";
		 Position = "0 0";
		 Extent = "286 100";
		 IsVertical = "1";
		 ChildSpacing = -4;
	 };
 	ThemeManager.setProfile(%this.chain, "emptyProfile");
 	%this.add(%this.chain);

	%this.titleText = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 13";
		Extent = "286 38";
		Text = "";
		FontSizeAdjust = "1.4";
	};
	ThemeManager.setProfile(%this.titleText, "titleProfile");
	%this.chain.add(%this.titleText);

	%this.authorText = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 0";
		Extent = "180 22";
		MinExtent = "180 22";
		Text = "Module by Torque2D";
		TextWrap = 1;
		TextExtend = 1;
		FontSizeAdjust = "1.1";
		FontColor = ThemeManager.activeTheme.color5;
		OverrideFontColor = true;
	};
	ThemeManager.setProfile(%this.authorText, "codeProfile");
	%this.chain.add(%this.authorText);

	%this.versionText = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 0";
		Extent = "180 26";
		Text = "Version: 1     Build: 1";
		FontColor = ThemeManager.activeTheme.setAlpha(ThemeManager.activeTheme.color4, 150);
		OverrideFontColor = true;
	};
	ThemeManager.setProfile(%this.versionText, "codeProfile");
	%this.chain.add(%this.versionText);

	%this.addSpacer(%this.chain);

	%this.descriptionText = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 10";
		Extent = "286 150";
		MinExtent = "250 150";
		Text = "";
		TextWrap = 1;
		TextExtend = 1;
	};
	ThemeManager.setProfile(%this.descriptionText, "normalTextProfile");
	%this.chain.add(%this.descriptionText);

	%this.optionGroup = %this.addSubSection("Module Options");

	%this.launchGroupCheckBox = new GuiCheckBoxCtrl()
	{
		Class="ProjectManagerLaunchGroupCheckBox";
		HorizSizing="width";
		Position = "0 0";
		Extent = "286 26";
		Text = "Start on Project Launch";
		BoxOffset = "0 2";
		TextOffset = "22 0";
		TextExtent = "264 26";
		vAlign = "Middle";
		Tooltip = "This change will be applied after restarting.";
	};
	ThemeManager.setProfile(%this.launchGroupCheckBox, "checkboxProfile");
	ThemeManager.setProfile(%this.launchGroupCheckBox, "tipProfile", "TooltipProfile");
	%this.optionGroup.add(%this.launchGroupCheckBox);
	%this.launchGroupCheckBox.addListener(%this);

	%this.addEmptySpace(30, %this.optionGroup);

	%this.addSubSection("Dependencies");

	%this.dependList = new GuiChainCtrl()
	{
		Class = ProjectModuleDependList;
		HorizSizing="width";
		VertSizing="height";
		Position = "0 0";
		Extent = "286 100";
		IsVertical = "1";
		ChildSpacing = 2;
	};
   ThemeManager.setProfile(%this.dependList, "emptyProfile");
   %this.chain.add(%this.dependList);

   %this.addEmptySpace(30, %this.chain);

	%this.addSubSection("Declared Assets");

	%this.assetList = new GuiChainCtrl()
	{
		Class = ProjectModuleAssetList;
		HorizSizing="width";
		VertSizing="height";
		Position = "0 0";
		Extent = "286 100";
		IsVertical = "1";
		ChildSpacing = 2;
	};
   ThemeManager.setProfile(%this.assetList, "emptyProfile");
   %this.chain.add(%this.assetList);

   %this.button = new GuiButtonCtrl()
   {
	   Class="ProjectModuleCardButton";
	   HorizSizing="left";
	   VertSizing="bottom";
	   Position = "184 40";
	   Extent = "100 30";
	   MinExtent = "100 30";
	   Text = "";
   };
   ThemeManager.setProfile(%this.button, "primaryButtonProfile");
   %this.add(%this.button);
   %this.startListening(%this.button);

	%this.startListening(ThemeManager);
 }

function ProjectModuleCard::addSubSection(%this, %name)
{
	%chain = new GuiChainCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position = "0 0";
		Extent = "286 0";
		IsVertical = "1";
		ChildSpacing = -4;
	};
	ThemeManager.setProfile(%chain, "emptyProfile");
	%this.chain.add(%chain);

	%subSection = new GuiControl()
	{
		HorizSizing="width";
		Extent = "286 24";
		Text = %name;
		FontSizeAdjust = "0.8";
	};
	ThemeManager.setProfile(%subSection, "titleProfile");
	%chain.add(%subSection);

	%this.addSpacer(%chain);
	%this.addEmptySpace(8, %chain);

	return %chain;
}

function ProjectModuleCard::addSpacer(%this, %chain)
{
   %spacer = new GuiControl()
   {
	   HorizSizing="width";
	   Position = "0 0";
	   Extent = "286 6";
   };
   ThemeManager.setProfile(%spacer, "spacerProfile");
   %chain.add(%spacer);

   return %spacer;
}

function ProjectModuleCard::addEmptySpace(%this, %size, %chain)
{
	%empty = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 0";
		Extent = "286" SPC %size;
	};
	ThemeManager.setProfile(%empty, "emptyProfile");
	%chain.add(%empty);

	return %empty;
}

function ProjectModuleCard::onThemeChange(%this, %theme)
{
	%this.authorText.FontColor = %theme.color5;
	%this.versionText.FontColor = %theme.setAlpha(%theme.color4, 150);
}

function ProjectModuleCard::show(%this, %module)
{
	if(isObject(%module))
	{
		%this.activeModule = %module;
		%this.moduleID = %module.moduleID;
		%this.versionID = %module.versionID;

		%this.titleText.setText(%module.moduleID);
		%this.setAuthor(%module);
		%this.setVersion(%module);
		%this.descriptionText.setText(%module.description);

		%this.button.visible = false;
		%projectModule = %this.getInstalledModule(%module.moduleID, %module.versionID);
		if(%projectModule != %module)
		{
			if(!isObject(%projectModule))
			{
				%this.button.setText("Install");
				%this.button.visible = true;
			}
			else if(%projectmodule.buildID < %module.buildID)
			{
				%this.button.setText("Update");
				%this.button.visible = true;
			}
		}
		else
		{

		}

		%this.optionGroup.visible = false;
		if(!%module.Synchronized)
		{
			%this.optionGroup.visible = true;
			%this.launchGroupCheckBox.setStateOn(%module.Group $= "launch");
			for(%i = 0; %i < %this.toggleLaunchGroupCount; %i++)
			{
				if(%this.toggleLaunchGroup[%i] $= %this.activeModule.getModulePath())
				{
					%this.launchGroupCheckBox.setStateOn(!%this.launchGroupCheckBox.getStateOn());
					break;
				}
			}
		}

		%this.visible = true;
		%this.dependList.show(%module);
		%this.assetList.show(%module);
	}
}

function ProjectModuleCard::hide(%this)
{
	%this.activeModule = 0;
	%this.moduleID = "";
	%this.versionID = 0;
	%this.visible = false;
}

function ProjectModuleCard::setAuthor(%this, %module)
{
	%author = %module.author;
	if(%author $= "")
	{
		%author = "Torque2D";
	}

	%type = %module.type;
	if(%type $= "")
	{
		%type = "Module";
	}

	%this.authorText.setText(%type SPC "by" SPC %author);
}

function ProjectModuleCard::setVersion(%this, %module)
{
	%version = %module.versionID;
	if(%version $= "")
	{
		%version = 1;
	}

	%build = %module.buildID;
	if(%build $= "")
	{
		%build = 0;
	}

	%this.versionText.setText("Version:" SPC %version @ "     Build:" SPC %build);
}

function ProjectModuleCard::getInstalledModule(%this, %moduleID, %versionID)
{
	%allModules = ModuleDatabase.findModules(false);
	%count = getWordCount(%allModules);
	for(%i = 0; %i < %count; %i++)
	{
		%projectModule = getWord(%allModules, %i);
		if(%moduleID $= %projectModule.ModuleID && %versionID == %projectModule.VersionID)
		{
			return %projectModule;
		}
	}
	return 0;
}

function ProjectModuleCardButton::onClick(%this)
{
	%this.postEvent("ButtonClick");
}

function ProjectModuleCard::onButtonClick(%this)
{
	%this.postEvent(%this.button.getText() @ "Click");
}

function ProjectManagerLaunchGroupCheckBox::onClick(%this)
{
	%state = %this.getStateOn();
	%this.postEvent("LauchGroupClick", %state);
}

function ProjectModuleCard::onLauchGroupClick(%this, %state)
{
	%this.startListening(ProjectManager);
	if(isObject(%this.activeModule))
	{
		if((%state && %this.activeModule.group $= "launch") || (!%state && %this.activeModule.group !$= "launch"))
		{
			%path = %this.activeModule.getModulePath();
			for(%i = 0; %i < %this.toggleLaunchGroupCount; %i++)
			{
				if(%this.toggleLaunchGroup[%i] $= %path)
				{
					%this.toggleLaunchGroup[%i] = "";
				}
			}
		}
		else
		{
			%this.toggleLaunchGroup[%this.toggleLaunchGroupCount] = %this.activeModule.getModulePath();
			%this.toggleLaunchGroupCount++;
		}
	}
}

function ProjectModuleCard::onShutDown(%this)
{
	for(%i = 0; %i < %this.toggleLaunchGroupCount; %i++)
	{
		%path = pathConcat(%this.toggleLaunchGroup[%i], "module.taml");
		if(%path !$= "" && isFile(%path))
		{
			%this.toggleLaunchGroupForPath(%path);
		}
	}
}

function ProjectModuleCard::toggleLaunchGroupForPath(%this, %path)
{
	%module = TamlRead(%path);
	if(%module.group $= "launch")
	{
		%module.group = "";
	}
	else
	{
		%module.group = "launch";
	}
	TamlWrite(%module, %path);
}
