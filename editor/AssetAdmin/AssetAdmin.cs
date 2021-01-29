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

	%this.guiPage = EditorCore.RegisterEditor("Asset Manager", %this);

	%this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="left";
		VertSizing="height";
		Position="700 0";
		Extent="324 768";
		MinExtent="220 200";
		hScrollBar="dynamic";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.scroller, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.scroller, "scrollingPanelThumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.scroller, "scrollingPanelTrackProfile", TrackProfile);
	ThemeManager.setProfile(%this.scroller, "scrollingPanelArrowProfile", ArrowProfile);

	%this.guiPage.add(%this.scroller);

	%this.dictionaryContainer = new GuiChainCtrl()
	{
		HorizSizing="bottom";
		VertSizing="right";
		Position="0 0";
		Extent="310 768";
		MinExtent="220 200";
	};
	ThemeManager.setProfile(%this.dictionaryContainer, "emptyProfile");
	%this.scroller.add(%this.dictionaryContainer);

	%this.Dictionary["ImageAsset"] = new GuiPanelCtrl()
	{
		Class = AssetDictionary;
		Text="Image Assets";
		command="";
		HorizSizing="bottom";
		VertSizing="right";
		Position="0 0";
		Extent="310 22";
		MinExtent="80 22";
		Type = "ImageAsset";
	};
	%this.Dictionary["ImageAsset"].setExpandEase("EaseOutBounce", 1500);
	ThemeManager.setProfile(%this.Dictionary["ImageAsset"], "panelProfile");
	%this.dictionaryContainer.add(%this.Dictionary["ImageAsset"]);

	%this.Dictionary["AnimationAsset"] = new GuiPanelCtrl()
	{
		Class = AssetDictionary;
		Text="Animation Assets";
		command="";
		HorizSizing="bottom";
		VertSizing="right";
		Position="0 0";
		Extent="310 22";
		MinExtent="80 22";
		Type = "AnimationAsset";
	};
	%this.Dictionary["AnimationAsset"].setExpandEase("EaseOutBounce", 1500);
	ThemeManager.setProfile(%this.Dictionary["AnimationAsset"], "panelProfile");
	%this.dictionaryContainer.add(%this.Dictionary["AnimationAsset"]);

	EditorCore.FinishRegistration(%this.guiPage);
}

function AssetAdmin::destroy(%this)
{

}

function AssetAdmin::open(%this)
{
	%this.Dictionary["ImageAsset"].load();
	%this.Dictionary["AnimationAsset"].load();
}

function AssetAdmin::close(%this)
{
	%this.Dictionary["ImageAsset"].unload();
	%this.Dictionary["AnimationAsset"].unload();
}
