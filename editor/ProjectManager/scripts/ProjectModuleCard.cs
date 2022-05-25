
 function ProjectModuleCard::onAdd(%this)
 {
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

	%this.addSpacer();

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

function ProjectModuleCard::addSpacer(%this)
{
	%spacer = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 0";
		Extent = "286 6";
	};
	ThemeManager.setProfile(%spacer, "spacerProfile");
	%this.chain.add(%spacer);
}

function ProjectModuleCard::addSubSection(%this, %name)
{
	%subSection = new GuiControl()
	{
		HorizSizing="width";
		Extent = "286 24";
		Text = %name;
		FontSizeAdjust = "0.8";
	};
	ThemeManager.setProfile(%subSection, "titleProfile");
	%this.chain.add(%subSection);

	%this.addSpacer();

	%empty = new GuiControl()
	{
		HorizSizing="width";
		Position = "0 0";
		Extent = "286 8";
	};
	ThemeManager.setProfile(%empty, "emptyProfile");
	%this.chain.add(%empty);
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

		%this.visible = true;
		%this.dependList.show(%module);
	}
}

function ProjectModuleCard::hide(%this)
{
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
