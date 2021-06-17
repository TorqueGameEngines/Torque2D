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

function AssetAdmin::create(%this)
{
	exec("./AssetDictionary.cs");
	exec("./AssetWindow.cs");
	exec("./AssetDictionaryButton.cs");
	exec("./AssetInspector.cs");
	exec("./AssetAudioPlayButton.cs");

	%this.guiPage = EditorCore.RegisterEditor("Asset Manager", %this);
	%this.guiPage.add(%this.buildAssetWindow());
	%this.guiPage.add(%this.buildAudioPlayButton());
	%this.guiPage.add(%this.buildLibrary());
	%this.guiPage.add(%this.buildInspector());

	EditorCore.FinishRegistration(%this.guiPage);

	%this.isOpen = false;
}

function AssetAdmin::buildLibrary(%this)
{
	%this.libScroller = new GuiScrollCtrl()
	{
		HorizSizing="left";
		VertSizing="height";
		Position="700 0";
		Extent="324 768";
		MinExtent="162 384";
		hScrollBar="dynamic";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.libScroller, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.libScroller, "scrollingPanelThumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.libScroller, "scrollingPanelTrackProfile", TrackProfile);
	ThemeManager.setProfile(%this.libScroller, "scrollingPanelArrowProfile", ArrowProfile);

	%this.dictionaryList = new GuiChainCtrl()
	{
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="310 768";
		MinExtent="220 200";
	};
	ThemeManager.setProfile(%this.dictionaryList, "emptyProfile");
	%this.libScroller.add(%this.dictionaryList);

	%this.dictionaryList.add(%this.buildDictionary("Images", "ImageAsset"));
	%this.dictionaryList.add(%this.buildDictionary("Animations", "AnimationAsset"));
	%this.dictionaryList.add(%this.buildDictionary("Particle Effects", "ParticleAsset"));
	%this.dictionaryList.add(%this.buildDictionary("Fonts", "FontAsset"));
	%this.dictionaryList.add(%this.buildDictionary("Audio", "AudioAsset"));
	//%this.dictionaryList.add(%this.buildDictionary("Spines", "SpineAsset"));

	return %this.libScroller;
}

function AssetAdmin::buildDictionary(%this, %title, %type)
{
	%this.Dictionary[%type] = new GuiPanelCtrl()
	{
		Class = AssetDictionary;
		Text=%title;
		command="";
		HorizSizing="right";
		VertSizing="bottom";
		Position="0 0";
		Extent="310 22";
		MinExtent="80 22";
		Type = %type;
	};
	%this.Dictionary[%type].setExpandEase("EaseInOut", 1000);
	ThemeManager.setProfile(%this.Dictionary[%type], "panelProfile");

	return %this.Dictionary[%type];
}

function AssetAdmin::buildInspector(%this)
{
	%this.inspector = new GuiControl()
	{
		class = "AssetInspector";
		HorizSizing="width";
		VertSizing="top";
		Position="0 444";
		Extent="700 324";
		MinExtent="350 222";
	};
	ThemeManager.setProfile(%this.inspector, "overlayProfile");

	return %this.inspector;
}

function AssetAdmin::buildAssetWindow(%this)
{
	%this.assetScene = new Scene();
	%this.assetScene.setScenePause(true);

	%this.assetWindow = new SceneWindow()
	{
		class = AssetWindow;
		profile = ThemeManager.activeTheme.overlayProfile;
		position = "0 0";
		extent = "700 444";
		HorizSizing="width";
		VertSizing="height";
		minExtent = "0 0";
		cameraPosition = "0 0";
		cameraSize = "175 111";
		useWindowInputEvents = true;
		useObjectInputEvents = true;
		constantThumbHeight = false;
		scrollBarThickness = 14;
		 showArrowButtons = false;
	};
	ThemeManager.setProfile(%this.assetWindow, "thumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.assetWindow, "trackProfile", TrackProfile);
	ThemeManager.setProfile(%this.assetWindow, "scrollArrowProfile", ArrowProfile);

	%this.assetWindow.setScene(%this.assetScene);
	%this.assetWindow.setViewLimitOn("-87.5 -55.5 87.5 55.5");
	%this.assetWindow.setShowScrollBar(true);
	%this.assetWindow.setMouseWheelScrolls(false);

	return %this.assetWindow;
}

function AssetAdmin::buildAudioPlayButton(%this)
{
	%this.audioPlayButtonContainer = new GuiControl()
	{
		position = "0 0";
		extent = "700 444";
		HorizSizing="width";
		VertSizing="height";
	};
	ThemeManager.setProfile(%this.audioPlayButtonContainer, "emptyProfile");

	%this.audioPlayButton = new GuiButtonCtrl()
	{
		class="AssetAudioPlayButton";
		HorizSizing="center";
		VertSizing="center";
		Extent="100 48";
		Visible="0";
		Text = "Play";
	};
	ThemeManager.setProfile(%this.audioPlayButton, "buttonProfile");
	%this.audioPlayButtonContainer.add(%this.audioPlayButton);

	return %this.audioPlayButtonContainer;
}

function AssetAdmin::destroy(%this)
{

}

function AssetAdmin::open(%this)
{
	%this.Dictionary["ImageAsset"].load();
	%this.Dictionary["AnimationAsset"].load();
	%this.Dictionary["ParticleAsset"].load();
	%this.Dictionary["FontAsset"].load();
	%this.Dictionary["AudioAsset"].load();
	//%this.Dictionary["SpineAsset"].load();

	%this.assetScene.setScenePause(false);
	%this.isOpen = true;
}

function AssetAdmin::close(%this)
{
	%this.Dictionary["ImageAsset"].unload();
	%this.Dictionary["AnimationAsset"].unload();
	%this.Dictionary["ParticleAsset"].unload();
	%this.Dictionary["FontAsset"].unload();
	%this.Dictionary["AudioAsset"].unload();
	//%this.Dictionary["SpineAsset"].unload();

	%this.assetScene.setScenePause(true);
	%this.isOpen = false;
}

function AssetBase::onRefresh(%this)
{
	if(AssetAdmin.isOpen  && isObject(AssetAdmin.chosenButton))
	{
		AssetAdmin.chosenButton.onClick();
	}
}
