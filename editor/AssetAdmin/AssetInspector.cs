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

function AssetInspector::onAdd(%this)
{
	%this.titlebar = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="top";
		Position="0 0";
		Extent="700 34";
		MinExtent="350 34";
		Text = "";
	};
	ThemeManager.setProfile(%this.titlebar, "panelProfile");
	%this.add(%this.titlebar);

	%this.insScroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="top";
		Position="0 34";
		Extent="700 290";
		MinExtent="350 145";
		hScrollBar="alwaysOff";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.insScroller, "scrollingPanelProfile");
	ThemeManager.setProfile(%this.insScroller, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.insScroller, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.insScroller, "scrollingPanelArrowProfile", "ArrowProfile");
	%this.add(%this.insScroller);

	%this.inspector = new GuiInspector()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="686 290";
		MinExtent="343 145";
		FieldCellSize="300 40";
		ControlOffset="10 18";
		ConstantThumbHeight=false;
		ScrollBarThickness=12;
		ShowArrowButtons=true;
	};
	ThemeManager.setProfile(%this.inspector, "emptyProfile");
	ThemeManager.setProfile(%this.inspector, "panelProfile", "GroupPanelProfile");
	ThemeManager.setProfile(%this.inspector, "emptyProfile", "GroupGridProfile");
	ThemeManager.setProfile(%this.inspector, "labelProfile", "LabelProfile");
	ThemeManager.setProfile(%this.inspector, "textEditProfile", "textEditProfile");
	ThemeManager.setProfile(%this.inspector, "dropDownProfile", "dropDownProfile");
	ThemeManager.setProfile(%this.inspector, "dropDownItemProfile", "dropDownItemProfile");
	ThemeManager.setProfile(%this.inspector, "emptyProfile", "backgroundProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelProfile", "ScrollProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelTrackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.inspector, "scrollingPanelArrowProfile", "ArrowProfile");
	ThemeManager.setProfile(%this.inspector, "checkboxProfile", "checkboxProfile");
	ThemeManager.setProfile(%this.inspector, "buttonProfile", "buttonProfile");
	ThemeManager.setProfile(%this.inspector, "tipProfile", "tooltipProfile");
	%this.insScroller.add(%this.inspector);
	$ins = %this.inspector;
}

function AssetInspector::loadImageAsset(%this, %imageAsset, %assetID)
{
	%this.titlebar.setText("Image Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%imageAsset);
}

function AssetInspector::loadAnimationAsset(%this, %animationAsset, %assetID)
{
	%this.titlebar.setText("Animation Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%animationAsset);
}

function AssetInspector::loadParticleAsset(%this, %particleAsset, %assetID)
{
	%this.titlebar.setText("Particle Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%particleAsset);
}

function AssetInspector::loadFontAsset(%this, %fontAsset, %assetID)
{
	%this.titlebar.setText("Font Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%fontAsset);
}

function AssetInspector::loadAudioAsset(%this, %audioAsset, %assetID)
{
	%this.titlebar.setText("Audio Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%audioAsset);
}

function AssetInspector::loadSpineAsset(%this, %spineAsset, %assetID)
{
	%this.titlebar.setText("Spine Asset");
	%this.inspector.clearHiddenFields();
	%this.inspector.addHiddenField("hidden");
	%this.inspector.addHiddenField("locked");
	%this.inspector.addHiddenField("AssetInternal");
	%this.inspector.inspect(%spineAsset);
}
