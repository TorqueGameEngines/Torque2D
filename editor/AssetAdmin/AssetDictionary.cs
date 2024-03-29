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

function AssetDictionary::onAdd(%this)
{
	%this.newButton = new GuiButtonCtrl()
	{
		class = "NewAssetButton";
		Position = "5 27";
		Extent = "300 30";
		Text = "New";
	};
	ThemeManager.setProfile(%this.newButton, "buttonProfile");
	%this.add(%this.newButton);

	%this.grid = new GuiGridCtrl()
	{
		Position="0 62";
		Extent = "310 50";
		CellSizeX = 60;
		CellSizeY = 60;
		CellModeX = variable;
		CellSpacingX = 4;
		CellSpacingY = 4;
		OrderMode = "LRTB";
	};
	ThemeManager.setProfile(%this.grid, "emptyProfile");
	%this.add(%this.grid);
}

function AssetDictionary::load(%this)
{
	%query = new AssetQuery();
	AssetDatabase.findAllAssets(%query);
	AssetDatabase.findAssetType(%query, %this.Type, true);

	for(%i = 0; %i < %query.getCount(); %i++)
	{
		%assetID = %query.getAsset(%i);

		if(!AssetDatabase.isAssetInternal(%assetID))
		{
			%this.addButton(%assetID);
		}
	}
	%query.delete();

	%this.newButton.text = "New" SPC %this.type;
	%this.newButton.type = %this.type;
}

function AssetDictionary::addButton(%this, %assetID)
{
	%button = new GuiButtonCtrl()
	{
		Class = AssetDictionaryButton;
		HorizSizing="center";
		VertSizing="center";
		Extent = "100 100";
		Tooltip = AssetDatabase.getAssetName(%assetID);
		Text = "";
		AssetID = %assetID;
		Type = %this.Type;
	};
	ThemeManager.setProfile(%button, "itemSelectProfile");
	ThemeManager.setProfile(%button, "tipProfile", "TooltipProfile");
	%this.grid.add(%button);

	%this.fixSize();

	return %button;
}

function AssetDictionary::removeButton(%this, %assetID)
{
	%button = %this.getButton(%assetID);
	if(isObject(%button))
	{
		%button.delete();
		%this.fixSize();
		return true;
	}
	return false;
}

function AssetDictionary::fixSize(%this)
{
	if(%this.getExpanded())
	{
		%this.setExpanded(false);
		%this.setExpanded(true);
	}
}

function AssetDictionary::getButton(%this, %assetID)
{
	for(%i = 0; %i < %this.grid.getCount(); %i++)
	{
		%button = %this.grid.getObject(%i);
		if(%button.AssetID $= %assetID)
		{
			return %button;
		}
	}
	return 0;
}

function AssetDictionary::unload(%this)
{
	//Remove all the child gui controls
	for(%i = %this.grid.getCount() - 1; %i >= 0; %i--)
	{
		%obj = %this.grid.getObject(%i);
		%obj.delete();
	}
}

function AssetDictionary::reload(%this)
{
	%this.unload();
	%this.load();
}

function AssetDictionarySprite::onAnimationEnd(%this, %animationAssetID)
{
	%this.schedule(2000, "restartAnimation", %animationAssetID);
}

function AssetDictionarySprite::restartAnimation(%this, %animationAssetID)
{
	%this.setAnimation(%animationAssetID);
}
