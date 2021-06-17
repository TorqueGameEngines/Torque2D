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

	%texture = %this.buildIcon();
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

	%texture = %this.buildIcon();
	%texture.setAnimation(%assetID);

	%this.ImageAsset = %imageAsset;
	%this.imageAssetID = %imageAssetID;
	%this.AnimationAsset = %animationAsset;
	%this.AnimationAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::loadParticleAsset(%this, %assetID)
{
	%particleAsset = AssetDatabase.acquireAsset(%assetID);

	%texture = %this.buildIcon();
	%texture.setImage("AssetAdmin:assetIcons");
	%texture.setImageFrame(0);

	%this.particleAsset = %particleAsset;
	%this.particleAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::loadFontAsset(%this, %assetID)
{
	%fontAsset = AssetDatabase.acquireAsset(%assetID);

	%texture = %this.buildIcon();
	%texture.setImage("AssetAdmin:assetIcons");
	%texture.setImageFrame(1);

	%this.fontAsset = %fontAsset;
	%this.fontAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::loadAudioAsset(%this, %assetID)
{
	%audioAsset = AssetDatabase.acquireAsset(%assetID);

	%texture = %this.buildIcon();
	%texture.setImage("AssetAdmin:assetIcons");
	%texture.setImageFrame(3);

	%this.audioAsset = %audioAsset;
	%this.audioAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::loadSpineAsset(%this, %assetID)
{
	%spineAsset = AssetDatabase.acquireAsset(%assetID);

	%texture = %this.buildIcon();
	%texture.setImage("AssetAdmin:assetIcons");
	%texture.setImageFrame(2);

	%this.spineAsset = %spineAsset;
	%this.spineAssetID = %assetID;

	%this.add(%texture);
}

function AssetDictionaryButton::buildIcon(%this)
{
	%texture = new GuiSpriteCtrl()
	{
		class = "AssetDictionarySprite";
		HorizSizing="center";
		VertSizing="center";
		Extent = "50 50";
		minExtent = "50 50";
		Position = "0 0";
		constrainProportions = "1";
		fullSize = "1";
	};
	ThemeManager.setProfile(%texture, "spriteProfile");
	return %texture;
}

function AssetDictionaryButton::onClick(%this)
{
	if(AssetAdmin.chosenButton != %this)
	{
		if(isObject(AssetAdmin.chosenButton))
		{
			ThemeManager.setProfile(AssetAdmin.chosenButton, "itemSelectProfile");
		}
		ThemeManager.setProfile(%this, "itemSelectedProfile");
	}

	AssetAdmin.audioPlayButtonContainer.setVisible(false);
	AssetAdmin.AssetWindow.setVisible(true);

	if(isObject(%this.AnimationAsset) && %this.AnimationAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayAnimationAsset(%this.imageAsset, %this.AnimationAsset, %this.AnimationAssetID);
		AssetAdmin.inspector.loadAnimationAsset(%this.AnimationAsset, %this.AnimationAssetID);
	}
	else if(isObject(%this.ImageAsset) && %this.ImageAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayImageAsset(%this.ImageAsset, %this.ImageAssetID);
		AssetAdmin.inspector.loadImageAsset(%this.ImageAsset, %this.ImageAssetID);
	}
	else if(isObject(%this.ParticleAsset) && %this.ParticleAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayParticleAsset(%this.ParticleAsset, %this.ParticleAssetID);
		AssetAdmin.inspector.loadParticleAsset(%this.ParticleAsset, %this.ParticleAssetID);
	}
	else if(isObject(%this.FontAsset) && %this.FontAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayFontAsset(%this.FontAsset, %this.FontAssetID);
		AssetAdmin.inspector.loadFontAsset(%this.FontAsset, %this.FontAssetID);
	}
	else if(isObject(%this.AudioAsset) && %this.AudioAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displayAudioAsset(%this.AudioAsset, %this.AudioAssetID);
		AssetAdmin.inspector.loadAudioAsset(%this.AudioAsset, %this.AudioAssetID);
	}
	else if(isObject(%this.SpineAsset) && %this.SpineAssetID !$= "")
	{
		AssetAdmin.AssetWindow.displaySpineAsset(%this.SpineAsset, %this.SpineAssetID);
		AssetAdmin.inspector.loadSpineAsset(%this.SpineAsset, %this.SpineAssetID);
	}

	AssetAdmin.chosenButton = %this;
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
