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
	exec("./NewAssetButton.cs");
	exec("./NewImageAssetDialog.cs");
	exec("./NewAnimationAssetDialog.cs");
	exec("./NewParticleAssetDialog.cs");
	exec("./NewFontAssetDialog.cs");
	exec("./NewAudioAssetDialog.cs");
	exec("./DeleteAssetDialog.cs");
	exec("./ParticleEditor/exec.cs");
	exec("./ImageEditor/exec.cs");

	%this.guiPage = EditorCore.RegisterEditor("Asset Manager", %this);
	%this.content = %this.createFrameSet();
	%this.buildAssetWindow();
	%this.buildAudioPlayButton();
	%this.buildInspector();
	%this.buildLibrary();

	EditorCore.FinishRegistration(%this.guiPage);

	%this.isOpen = false;
}

function AssetAdmin::createFrameSet(%this)
{
	%content = new GuiFrameSetCtrl() {
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
        DividerThickness = 6;
	};
    ThemeManager.setProfile(%content, "frameSetProfile");
    ThemeManager.setProfile(%content, "dropButtonProfile", "dropButtonProfile");
    ThemeManager.setProfile(%content, "frameSetTabBookProfile", "tabBookProfile");
    ThemeManager.setProfile(%content, "frameSetTabProfile", "tabProfile");
    ThemeManager.setProfile(%content, "frameSetTabPageProfile", "tabPageProfile");
    %this.guiPage.add(%content);

    %idList = %content.createHorizontalSplit(1);
    %leftID = getWord(%idList, 0);
    %rightID = getWord(%idList, 1);
    %content.anchorFrame(%rightID);
    %content.setFrameSize(%rightID, 324);
    
    %ids = %content.createVerticalSplit(%leftID);
    %centerFrameID = getWord(%ids, 0);
    %inspectorFrameID = getWord(%ids, 1);
	%content.anchorFrame(%inspectorFrameID);
    %content.setFrameSize(%inspectorFrameID, 360);

	return %content;
}

function AssetAdmin::buildLibrary(%this)
{
	%this.libWindow = new GuiWindowCtrl()
    {
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "0 0";
        Extent = "330 380";
        MinExtent = "200 100";
        text = "Asset Library";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.libWindow, "windowProfile");
    ThemeManager.setProfile(%this.libWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.libWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.libWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.libWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.content.add(%this.libWindow);

	%this.libScroller = new GuiScrollCtrl()
	{
        HorizSizing = "width";
        VertSizing = "height";
		Position="0 0";
		Extent="324 356";
		MinExtent="0 0";
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
	%this.libWindow.add(%this.libScroller);

	%this.dictionaryList = new GuiChainCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
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
}

function AssetAdmin::buildDictionary(%this, %title, %type)
{
	%this.Dictionary[%type] = new GuiPanelCtrl()
	{
		Class = AssetDictionary;
		Text=%title;
		command="";
		HorizSizing="width";
		VertSizing="bottom";
		Position="0 0";
		Extent="306 22";
		MinExtent="80 22";
		Type = %type;
	};
	%this.Dictionary[%type].setExpandEase("EaseInOut", 1000);
	ThemeManager.setProfile(%this.Dictionary[%type], "panelProfile");

	return %this.Dictionary[%type];
}

function AssetAdmin::buildInspector(%this)
{
	%this.inspectorWindow = new GuiWindowCtrl()
    {
        HorizSizing = "right";
        VertSizing = "bottom";
        text = "Asset Inspector";
		Extent = "706 380";
		MinExtent = "500 250";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.inspectorWindow, "windowProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.content.add(%this.inspectorWindow);

	%this.inspector = new GuiControl()
	{
		class = "AssetInspector";
		Extent="700 370";
        HorizSizing = "fill";
        VertSizing = "fill";
	};
	ThemeManager.setProfile(%this.inspector, "overlayProfile");

	%this.inspectorWindow.add(%this.inspector);
}

function AssetAdmin::buildAssetWindow(%this)
{
	%this.background = new GuiSpriteCtrl() {
		HorizSizing = "right";
        VertSizing = "bottom";
        Position = "0 0";
        Extent = "100 100";
		imageColor = "255 255 255 255";
        Image = "EditorCore:editorGrid";
		singleFrameBitmap = "1";
		tileImage = "1";
		positionOffset = "0 0";
		imageSize = "128 128";
		fullSize = "0";
		constrainProportions = "1";
	};
    ThemeManager.setProfile(%this.background, "emptyProfile");
    %this.content.add(%this.background);

	%this.assetScene = new Scene();
	%this.assetScene.setScenePause(true);

	%this.assetWindow = new SceneWindow()
	{
		class = AssetWindow;
		profile = ThemeManager.activeTheme.overlayProfile;
		position = "0 0";
		extent = %this.background.extent;
        HorizSizing = "width";
        VertSizing = "height";
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

    %this.background.add(%this.assetWindow);
}

function AssetAdmin::buildAudioPlayButton(%this)
{
	%this.audioPlayButtonContainer = new GuiControl()
	{
		position = "0 0";
		extent = %this.background.extent;
		HorizSizing="width";
		VertSizing="height";
		Visible="0";
	};
	ThemeManager.setProfile(%this.audioPlayButtonContainer, "emptyProfile");

	%this.audioPlayButton = new GuiButtonCtrl()
	{
		class="AssetAudioPlayButton";
		HorizSizing="center";
		VertSizing="center";
		Extent="100 48";
		Text = "Play";
	};
	ThemeManager.setProfile(%this.audioPlayButton, "buttonProfile");
	%this.audioPlayButtonContainer.add(%this.audioPlayButton);

	%this.background.add(%this.audioPlayButtonContainer);
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
