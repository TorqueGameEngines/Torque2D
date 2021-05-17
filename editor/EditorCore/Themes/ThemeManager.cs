//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

function ThemeManager::onAdd(%this)
{
	exec("./BaseTheme/BaseTheme.cs");
	exec("./LabCoat/LabCoatTheme.cs");
	exec("./ForestRobe/ForestRobeTheme.cs");
	exec("./TorqueSuit/TorqueSuitTheme.cs");

	%this.themeList = new SimSet();
	%this.controlList = new SimSet();

	%constructionVest = new ScriptObject()
	{
		class = "BaseTheme";
	};

	%labCoat = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "LabCoatTheme";
	};

	%forestRobe = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "ForestRobeTheme";
	};

	%torqueSuit = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "TorqueSuitTheme";
	};

	%this.registerTheme(%constructionVest);
	%this.registerTheme(%labCoat);
	%this.registerTheme(%forestRobe);
	%this.registerTheme(%torqueSuit);
	%this.setTheme(0);
}

function ThemeManager::setTheme(%this, %i)
{
	%i = mClamp(%i, 0, %this.themeList.getCount() - 1);
	%this.curTheme = %i;
	%theme = %this.themeList.getObject(%i);
	%this.activeTheme = %theme;
	%this.refreshProfiles();
}

function ThemeManager::nextTheme(%this)
{
	%this.setTheme(%this.curTheme+1);
}

function ThemeManager::prevTheme(%this)
{
	%this.setTheme(%this.curTheme-1);
}

function ThemeManager::registerTheme(%this, %theme)
{
	%this.themeList.add(%theme);
}

function ThemeManager::refreshProfiles(%this)
{
	for (%i = 0; %i < %this.controlList.getCount(); %i++)
	{
		%obj = %this.controlList.getObject(%i);

		if(isObject(%obj.gui))
		{
			if(!isObject(%this.activeTheme.getFieldValue(%obj.profileName)))
			{
				error("ThemeManager::setProfile - Unable to find profile" SPC %obj.profileName SPC "for theme" SPC %this.activeTheme.name @ "!");
			}

			%obj.gui.setEditFieldValue(%obj.profileTag, %this.activeTheme.getFieldValue(%obj.profileName));
		}
		else
		{
			//let's remove this corpse
			%this.controlList.remove(%obj);
			%this.i--;
		}
	}
}

function ThemeManager::setProfile(%this, %gui, %profileName, %profileTag)
{
	if(%profileTag $= "")
	{
		%profileTag = "Profile";
	}

	if(!isObject(%this.activeTheme.getFieldValue(%profileName)))
	{
		error("ThemeManager::setProfile - Unable to find profile" SPC %profileName SPC "for theme" SPC %this.activeTheme.name @ "!");
	}

	%gui.setEditFieldValue(%profileTag, %this.activeTheme.getFieldValue(%profileName));
	%this.controlList.add(
		new ScriptObject()
		{
			gui = %gui;
			profileTag = %profileTag;
			profileName = %profileName;
		}
	);
}

function ThemeManager::createProfile(%this, %profileName, %parentName, %settings)
{
	if(!isObject(%this.activeTheme.getFieldValue(%parentName)))
	{
		error("ThemeManager::createProfile - Unable to find parent profile" SPC %parentName SPC "for theme" SPC %this.activeTheme.name @ "!");
	}

	%this.themeList.callOnChildren("createProfile", %profileName, %parentName, %settings);
}

function ThemeManager::populateFonts(%this)
{
	echo("ThemeManager: Populating fonts...");
	%oldPath = $GUI::fontCacheDirectory;
	$GUI::fontCacheDirectory = %this.activeTheme.fontDirectory;
	echo("  Path set to" SPC $GUI::fontCacheDirectory);

	for(%i = 1; %i <= %this.activeTheme.fontCount; %i++)
	{
		echo("  Populating" SPC %this.activeTheme.font[%i]);
		populateFontCacheRange(%this.activeTheme.font[%i],12,0,65535);
		populateFontCacheRange(%this.activeTheme.font[%i],14,0,65535);
		populateFontCacheRange(%this.activeTheme.font[%i],16,0,65535);
		populateFontCacheRange(%this.activeTheme.font[%i],18,0,65535);
		populateFontCacheRange(%this.activeTheme.font[%i],24,0,65535);
		writeSingleFontCache(%this.activeTheme.font[%i]);
	}
	$GUI::fontCacheDirectory = %oldPath;
}
