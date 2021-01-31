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

function AssetDictionaryButton::onAdd(%this)
{
	%this.call("load" @ %this.type, %this.assetID);
	%this.assetID = "";
}

function AssetDictionaryButton::loadImageAsset(%this, %assetID)
{
	%imageAsset = AssetDatabase.acquireAsset(%assetID);

	%extent = %this.getIconExtent(%imageAsset.getFrameSize(0));
	%texture = %this.buildIcon(%extent);
	%texture.setImage(%assetID);

	%this.ImageAsset = %imageAsset;
	%this.ImageAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::loadAnimationAsset(%this, %assetID)
{
	%animationAsset = AssetDatabase.acquireAsset(%assetID);
	%imageAssetID = %animationAsset.getImage();
	%imageAsset = AssetDatabase.acquireAsset(%imageAssetID);

	%extent = %this.getIconExtent(%imageAsset.getFrameSize(0));
	%texture = %this.buildIcon(%extent);
	%texture.setAnimation(%assetID);

	%this.ImageAsset = %imageAsset;
	%this.imageAssetID = %imageAssetID;
	%this.AnimationAsset = %animationAsset;
	%this.AnimationAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::getIconExtent(%this, %size)
{
	%x = getWord(%size, 0);
	%y = getWord(%size, 1);
	%ratio = %x / %y;
	%extent = "50 50";
	if(%x > %y)
	{
		%extent = "50" SPC (50/%ratio);
	}
	else if(%x < %y)
	{
		%extent = (50*%ratio) SPC "50";
	}
	return %extent;
}

function AssetDictionaryButton::buildIcon(%this, %extent)
{
	%texture = new GuiSpriteCtrl()
	{
		class = "AssetDictionarySprite";
		HorizSizing="center";
		VertSizing="center";
		Extent = %extent;
		minExtent=%extent;
		Position = "0 0";
	};
	ThemeManager.setProfile(%texture, "spriteProfile");
	return %texture;
}

function AssetDictionaryButton::onClick(%this)
{
	if(isObject(%this.AnimationAsset) && %this.AnimationAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayAnimationAsset(%this.imageAsset, %this.AnimationAsset, %this.AnimationAssetID);
	}
	else if(isObject(%this.ImageAsset) && %this.ImageAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayImageAsset(%this.ImageAsset, %this.ImageAssetID);
	}
}

function AssetDictionaryButton::onRemove(%this)
{
	if(isObject(%this.ImageAsset))
	{
		AssetDatabase.releaseAsset(%this.ImageAssetID);
	}
	if(isObject(%this.AnimationAsset))
	{
		AssetDatabase.releaseAsset(%this.AnimationAssetID);
	}
}
