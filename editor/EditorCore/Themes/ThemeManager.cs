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
	%this.spriteList = new SimSet();

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
	%this.refreshSprites();
	%this.postEvent("ThemeChange", %theme);
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

			if(%obj.profileName $= "textEditProfile")
			{
				%obj.gui.editCursor = %this.activeTheme.editCursor;
			}
		}
		else
		{
			//let's remove this corpse
			%this.controlList.remove(%obj);
			%this.i--;
		}
	}
}

function ThemeManager::refreshSprites(%this)
{
	for (%i = 0; %i < %this.spriteList.getCount(); %i++)
	{
		%obj = %this.spriteList.getObject(%i);

		if(isObject(%obj.sprite))
		{
			%obj.sprite.setImage(%this.activeTheme.getFieldValue(%obj.imageName), %obj.frame);
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

	if(%profileName $= "textEditProfile")
	{
		%gui.editCursor = %this.activeTheme.editCursor;
	}

	if(%profileName $= "windowProfile")
	{
		%gui.leftRightCursor = %this.activeTheme.leftRightCursor;
		%gui.upDownCursor = %this.activeTheme.upDownCursor;
		%gui.NWSECursor = %this.activeTheme.NWSECursor;
	}
}

function ThemeManager::setImage(%this, %sprite, %imageName, %frame)
{
	if(%frame $= "")
	{
		%frame = 0;
	}
	%sprite.setImage(%this.activeTheme.getFieldValue(%imageName), %frame);
	%this.spriteList.add(
		new ScriptObject()
		{
			sprite = %sprite;
			frame = %frame;
			imageName = %imageName;
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

// Bake the active theme's faces into its font folder, so the editor draws the
// same on a machine without them installed as on one that has them.
//
// Three things decide which sizes to bake, and only the first is declared
// anywhere: a base set every theme uses, whatever sizes the folder already
// holds, and the sizes the engine had to rasterize during this session. That
// last group is the one a hardcoded list keeps missing -- a recipe may offset a
// profile's fontSize, and a control's fontSizeAdjust multiplies it, so a profile
// set to 16 worn by a control adjusting 1.2 asks GFont for 19 and no field
// anywhere says 19. A missed size does not fail loudly: GFont builds the cache
// on demand from the installed face and fills it with only the characters that
// got drawn, which looks perfect here and renders blank anywhere else.
//
// The range is printable Latin-1 rather than the whole of BMP-0. The full range
// warns once per unmapped code point and costs about 2MB a file, where this
// costs a few KB and covers everything the editor puts on screen.
function ThemeManager::populateFonts(%this)
{
	echo("ThemeManager: Populating fonts...");
	%oldPath = $GUI::fontCacheDirectory;
	%dir = %this.activeTheme.fontDirectory;
	$GUI::fontCacheDirectory = %dir;
	echo("  Path set to" SPC %dir);

	for(%i = 1; %i <= %this.activeTheme.fontCount; %i++)
	{
		%face = %this.activeTheme.font[%i];
		%sizes = %this.getFontSizes(%face, %dir);
		echo("  Populating" SPC %face SPC "at" SPC %sizes);

		for(%s = 0; %s < getWordCount(%sizes); %s++)
		{
			%this.populateFont(%face, getWord(%sizes, %s));
		}
	}

	%this.populateRequestedFonts(%dir);
	$GUI::fontCacheDirectory = %oldPath;
}

// One face at one size, written on its own. writeSingleFontCache would do the
// writing, but it scans the project for every *.uft whose path contains the face
// name -- so it rewrites that face in every other theme that shares it, and gets
// slower as caches accumulate. writeOneFontCache touches exactly this file.
function ThemeManager::populateFont(%this, %face, %size)
{
	// Space through the end of the Miscellaneous Symbols block, which is every
	// character the editor can put on screen: Latin-1, Latin Extended, Greek,
	// Cyrillic, the punctuation nobody thinks about until it is missing (curly
	// quotes, en and em dashes, the ellipsis), currency including the euro,
	// fractions, arrows, math operators and the box-drawing set.
	//
	// Below 32 is deliberately excluded: those are the C0 control codes, and a
	// font asked to draw one answers with its missing-glyph box, so caching them
	// only makes a stray control character render as a visible box.
	//
	// The end of the range is EXCLUSIVE, so this bakes up to 0x26FF. Costs about
	// 30 bytes a code point in the file whether or not the face has the glyph --
	// roughly 300KB a cache. Going further is what made these files 2MB each: the
	// whole of BMP-0 is 65,536 code points of that table, and the faces here have
	// at most a few hundred real glyphs above it.
	populateFontCacheRange(%face, %size, 32, 9984);

	if(!writeOneFontCache(%face, %size))
	{
		warn("  Could not write the font cache for" SPC %face SPC %size @ ".");
	}
}

// The base set, plus every size this face already has a cache for. The folder is
// the record of what earlier sessions asked for, so a size that appeared once
// keeps being filled instead of decaying back to the handful of glyphs that were
// on screen when it was created.
function ThemeManager::getFontSizes(%this, %face, %dir)
{
	%sizes = "12 14 16 18 24";

	// Asked as a full path: a relative one would resolve against this script's
	// own folder rather than the game root.
	%path = makeFullPath(%dir, getMainDotCsDir());
	if(%dir $= "" || !isDirectory(%path))
	{
		return %sizes;
	}

	%files = getFileList(%path);
	%count = getFieldCount(%files);
	for(%i = 0; %i < %count; %i++)
	{
		%size = %this.cacheSizeFor(getField(%files, %i), %face);
		if(%size > 0 && !%this.sizeListContains(%sizes, %size))
		{
			%sizes = %sizes SPC %size;
		}
	}

	return %sizes;
}

// A baked cache is named "<face> <size> (<charset>).uft". Returns the size this
// file holds for the given face, or zero if it is not that face's cache. The
// face may have spaces of its own, which is why the size is read as the last
// word rather than the second.
function ThemeManager::cacheSizeFor(%this, %file, %face)
{
	if(strlwr(fileExt(%file)) !$= ".uft")
	{
		return 0;
	}

	%base = fileBase(fileName(%file));
	%paren = strpos(%base, " (");
	if(%paren >= 0)
	{
		%base = getSubStr(%base, 0, %paren);
	}

	%wc = getWordCount(%base);
	if(%wc < 2)
	{
		return 0;
	}

	%size = getWord(%base, %wc - 1);
	if(%size !$= (%size + 0) || getWords(%base, 0, %wc - 2) !$= %face)
	{
		return 0;
	}

	return %size;
}

// The sizes nothing on disk and no field declares. The engine records each
// face/size it had to rasterize for want of a cache; the rows belonging to this
// theme's folder are ours to bake, and the rest -- a game's own fonts -- are not.
//
// Expects $GUI::fontCacheDirectory to already name %dir, which is where the
// bake reads and writes; populateFonts sets it around this call.
function ThemeManager::populateRequestedFonts(%this, %dir)
{
	%rows = getUncachedFonts();
	%path = makeFullPath(%dir, getMainDotCsDir());
	%count = getRecordCount(%rows);

	for(%i = 0; %i < %count; %i++)
	{
		%row = getRecord(%rows, %i);
		%face = getField(%row, 0);
		%size = getField(%row, 1);
		%rowDir = getField(%row, 2);

		if(%face $= "" || %size <= 0 || %rowDir $= "")
		{
			continue;
		}

		// Compared as full paths: a profile may carry the folder relative to the
		// game root, where a theme's fontDirectory is already expanded.
		if(makeFullPath(%rowDir, getMainDotCsDir()) !$= %path)
		{
			continue;
		}

		echo("  Populating" SPC %face SPC %size SPC "(rendered, never cached)");
		%this.populateFont(%face, %size);
	}

	clearUncachedFonts();
}

function ThemeManager::sizeListContains(%this, %list, %size)
{
	%count = getWordCount(%list);
	for(%i = 0; %i < %count; %i++)
	{
		if(getWord(%list, %i) == %size)
		{
			return true;
		}
	}
	return false;
}
