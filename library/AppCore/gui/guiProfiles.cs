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

function AppCore::SetProfileColors(%this)
{
	%this.color[1] = "43 43 43 255";
	%this.color[2] = "81 92 102 255";
	%this.color[3] = "224 224 224 255";
	%this.color[4] = "54 135 196 255";
	%this.color[5] = "245 210 50 255";
	%this.color[6] = "196 54 71 255";
}

function AppCore::SetProfileFont(%this)
{
	if ($platform $= "windows")
		%this.platformFontType = "share tech mono";
	else if ($platform $= "Android")
		%this.platformFontType = "Droid";
	else
		%this.platformFontType = "monaco";
	if ($platform $= "ios")
		%this.platformFontSize = 18;
	else if ($platform $= "Android")
		%this.platformFontSize = 14;
	else
		%this.platformFontSize = 12;
}

function AppCore::AdjustColorValue(%this, %color, %percent)
{
	%red = getWord(%color, 0);
	%green = getWord(%color, 1);
	%blue = getWord(%color, 2);
	%alpha = getWord(%color, 3);

	%largest = mGetMax(%red, mGetMax(%blue, %green));
	%currentValue = %largest / 255;
	%fullRed = %red / %currentValue;
	%fullGreen = %green / %currentValue;
	%fullBlue = %blue / %currentValue;

	%newValue = %currentValue += (%percent/100);
	%newValue = mClamp(%newValue, 0, 100);
	%newColor = mRound(mClamp((%fullRed * %newValue), 0, 255)) SPC
		mRound(mClamp((%fullGreen * %newValue), 0, 255)) SPC
		mRound(mClamp((%fullBlue * %newValue), 0, 255)) SPC %alpha;

	return %newColor;
}

function AppCore::SetColorAlpha(%this, %color, %newAlpha)
{
	%red = getWord(%color, 0);
	%green = getWord(%color, 1);
	%blue = getWord(%color, 2);
	return %red SPC %green SPC %blue SPC mRound(mClamp(%newAlpha, 0, 255));
}

function AppCore::SafeCreateNamedObject(%this, %name, %object)
{
	if(isObject(%name))
	{
		%originalObject = nameToID(%name);
		if(%originalObject.getClassName() !$= %object.getClassName())
		{
			warn("Attempted to change the class of the named object " @ %name @ "!");
			warn("Original Class: " @ %originalObject.getClassName());
			warn("New Class: " @ %object.getClassName());
			return;
		}
		%originalObject.assignFieldsFrom(%object);
		%object.delete();
	}
	else
	{
		%object.setName(%name);
	}
}

function AppCore::createGuiProfiles(%this)
{
	%this.SetProfileColors();
	%this.SetProfileFont();

	%this.SafeCreateNamedObject("DefaultCursor", new GuiCursor()
	{
	    hotSpot = "1 1";
	    renderOffset = "0 0";
	    bitmapName = "^AppCore/gui/images/cursors/defaultCursor";
	});

	%this.SafeCreateNamedObject("LeftRightCursor", new GuiCursor()
	{
	   hotSpot = "0.5 0";
	   renderOffset = "0.5 0.4";
	   bitmapName = "^AppCore/gui/images/cursors/leftRight";
	});

	%this.SafeCreateNamedObject("UpDownCursor", new GuiCursor()
	{
	   hotSpot = "1 1";
	   renderOffset = "0.5 0.5";
	   bitmapName = "^AppCore/gui/images/cursors/upDown";
	});

	%this.SafeCreateNamedObject("NWSECursor", new GuiCursor()
	{
	   hotSpot = "1 1";
	   renderOffset = "0.5 0.5";
	   bitmapName = "^AppCore/gui/images/cursors/NWSE";
	});

	%this.SafeCreateNamedObject("NESWCursor", new GuiCursor()
	{
	   hotSpot = "1 1";
	   renderOffset = "0.5 0.5";
	   bitmapName = "^AppCore/gui/images/cursors/NESW";
	});

	%this.SafeCreateNamedObject("MoveCursor", new GuiCursor()
	{
	   hotSpot = "1 1";
	   renderOffset = "0.5 0.5";
	   bitmapName = "^AppCore/gui/images/cursors/move";
	});

	%this.SafeCreateNamedObject("EditCursor", new GuiCursor()
	{
	   hotSpot = "0 0";
	   renderOffset = "0.5 0.5";
	   bitmapName = "^AppCore/gui/images/cursors/ibeam";
	});

	//Changing the default gui profile and border profile might cause engine instability! Consider making a new child profile instead.
	%this.SafeCreateNamedObject("GuiDefaultBorderProfile", new GuiBorderProfile()
	{
		// Default margin
		margin = 0;
		marginHL = 0;
		marginSL = 0;
		marginNA = 0;
		//Default Border
		border = 0;
		borderHL = 0;
		borderSL = 0;
		borderNA = 0;
		//Default border color
		borderColor   = %this.color1;
	    borderColorHL = %this.AdjustColorValue(%this.color1, 10);
	    borderColorSL = %this.AdjustColorValue(%this.color1, 10);
	    borderColorNA = %this.SetColorAlpha(%this.color1, 100);
		//Default Padding
		padding = 0;
		paddingHL = 0;
		paddingSL = 0;
		paddingNA = 0;
		//Default underfill
		underfill = true;
	});

	//See the warning above! You should avoid changing this.
	%this.SafeCreateNamedObject("GuiDefaultProfile", new GuiControlProfile()
	{
	    // fill color
	    fillColor = "0 0 0 0";

	    // font
	    fontType = %this.platformFontType;
		fontDirectory = expandPath( "^AppCore/fonts" );
	    fontSize = %this.platformFontSize;
	    fontColor = "255 255 255 255";
		align = center;
		vAlign = middle;

		cursorColor = "0 0 0 255";

		borderDefault = GuiDefaultBorderProfile;
		category = "default";
	});

	%this.SafeCreateNamedObject("GuiBrightBorderProfile", new GuiBorderProfile()
	{
		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		borderColor = "255 255 255 50";
		borderColorHL = "255 255 255 50";
		borderColorSL = "255 255 255 50";
		borderColorNA = "255 255 255 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiDarkBorderProfile", new GuiBorderProfile()
	{
		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		borderColor = "0 0 0 50";
		borderColorHL = "0 0 0 50";
		borderColorSL = "0 0 0 50";
		borderColorNA = "0 0 0 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiPanelProfile", new GuiControlProfile()
	{
		fillColor = %this.color1;
	    fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	    fillColorSL = %this.AdjustColorValue(%this.color1, 15);
	    fillColorNA = %this.SetColorAlpha(%this.color1, 100);
		borderDefault = GuiBrightBorderProfile;
		category = "defaultPanel";
	});

	%this.SafeCreateNamedObject("GuiListBoxBorderProfile", new GuiBorderProfile()
	{
		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;

		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;
	});

	%this.SafeCreateNamedObject("GuiListBoxProfile", new GuiControlProfile()
	{
	    // fill color
	    fillColor = %this.color1;
	    fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color1, 100);
		align = left;

		tab = false;
		canKeyFocus = true;
		category = "defaultListBox";

		fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color3, 20);
		fontColorSL = %this.AdjustColorValue(%this.color3, 20);
		fontColorNA = %this.AdjustColorValue(%this.color3, -30);

		borderDefault = GuiListBoxBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiWindowBorderProfile", new GuiBorderProfile()
	{
		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 4;
	});

	%this.SafeCreateNamedObject("GuiWindowProfile", new GuiControlProfile()
	{
	   fillColor = %this.AdjustColorValue(%this.color1, 10);
	   fillColorHL = %this.AdjustColorValue(%this.color1, 12);
	   fillColorSL = %this.color4;
	   fillColorNA = %this.color1;
	   category = "defaultWindow";
	   align = "Left";

	   fontColorSL = %this.color5;

	   borderLeft = GuiWindowBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiWindowContentBorderProfile", new GuiBorderProfile()
	{
		borderColor = %this.AdjustColorValue(%this.color1, 10);
		borderColorSL = %this.AdjustColorValue(%this.color4, -10);

		border = 3;
		borderSL = 3;
	});

	%this.SafeCreateNamedObject("GuiWindowContentProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -10);
		fillColorSL = %this.AdjustColorValue(%this.color1, -10);

		borderDefault = GuiWindowContentBorderProfile;
		borderTop = GuiDefaultBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiWindowButtonBorderProfile", new GuiBorderProfile()
	{
		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;

		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;
	});

	%this.SafeCreateNamedObject("GuiWindowCloseButtonProfile", new GuiControlProfile()
	{
	   fillColor = %this.SetColorAlpha(%this.color1, 150);
	   fillColorHL = %this.SetColorAlpha(%this.color6, 150);
	   fillColorSL = %this.AdjustColorValue(%this.color6, 10);
	   fillColorNA = %this.color1;

	   fontColor = %this.SetColorAlpha(%this.color3, 150);
	   fontColorHL = %this.SetColorAlpha(%this.color3, 170);
	   fontColorSL = %this.color5;
	   fontColorNA = %this.SetColorAlpha(%this.color3, 150);

	   borderDefault = GuiWindowButtonBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiWindowMinButtonProfile", new GuiControlProfile()
	{
	   fillColor = %this.SetColorAlpha(%this.color1, 150);
	   fillColorHL = %this.SetColorAlpha(%this.color4, 150);
	   fillColorSL = %this.AdjustColorValue(%this.color4, 10);
	   fillColorNA = %this.color1;

	   fontColor = %this.SetColorAlpha(%this.color3, 150);
	   fontColorHL = %this.SetColorAlpha(%this.color3, 170);
	   fontColorSL = %this.color5;
	   fontColorNA = %this.SetColorAlpha(%this.color3, 150);

	   borderDefault = GuiWindowButtonBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiWindowMaxButtonProfile", new GuiControlProfile()
	{
		fillColor = %this.SetColorAlpha(%this.color1, 150);
	    fillColorHL = %this.SetColorAlpha(%this.color4, 150);
	    fillColorSL = %this.AdjustColorValue(%this.color4, 10);
	    fillColorNA = %this.color1;

	    fontColor = %this.SetColorAlpha(%this.color3, 150);
	    fontColorHL = %this.SetColorAlpha(%this.color3, 170);
	    fontColorSL = %this.color5;
	    fontColorNA = %this.SetColorAlpha(%this.color3, 150);

	    borderDefault = GuiWindowButtonBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiTransparentProfile", new GuiControlProfile());

	%this.SafeCreateNamedObject("GuiGridProfile", new GuiControlProfile());

	%this.SafeCreateNamedObject("GuiChainProfile", new GuiControlProfile());

	%this.SafeCreateNamedObject("GuiTabBookProfile", new GuiControlProfile()
	{
		fillColor = %this.SetColorAlpha(%this.color1, 100);
		category = "defaultTabBook";
	});
	%this.SafeCreateNamedObject("GuiTabProfile", new GuiControlProfile()
	{
		fontColor = "255 255 255 255";
	    fontColorHL = "232 240 248 255";
	    fontColorSL= "255 255 255 255";
	    fontColorNA = "0 0 0 255";
		fillColor = %this.color1;
		fillColorHL = %this.AdjustColorValue(%this.color1, 10);
		fillColorSL = %this.AdjustColorValue(%this.color1, 15);
		fillColorNA = %this.SetColorAlpha(%this.color1, 100);
		borderDefault = GuiBrightBorderProfile;
		align = Center;
		category = "defaultTab";
	});
	%this.SafeCreateNamedObject("GuiTabPageProfile", new GuiControlProfile()
	{
		fillColor = %this.color1;
		fillColorHL = %this.AdjustColorValue(%this.color1, 10);
		fillColorSL = %this.AdjustColorValue(%this.color1, 15);
		fillColorNA = %this.SetColorAlpha(%this.color1, 100);
		category = "defaultTabPage";
	});


	%this.SafeCreateNamedObject("GuiSpriteProfile", new GuiControlProfile());

	%this.SafeCreateNamedObject("GuiTreeViewProfile", new GuiControlProfile()
	{
		fontColor = "255 255 255 255";
	    fontColorHL = "232 240 248 255";
	    fontColorSL= "255 255 255 255";
	    fontColorNA = "0 0 0 255";
		fillColor = %this.color1;
		fillColorHL = %this.AdjustColorValue(%this.color1, 10);
		fillColorSL = %this.AdjustColorValue(%this.color1, 15);
		fillColorNA = %this.SetColorAlpha(%this.color1, 100);
		bitmap = "./images/treeView";
		canKeyFocus = true;
		autoSizeHeight = true;
	});

	%this.SafeCreateNamedObject("GuiSolidBorderProfile", new GuiBorderProfile()
	{
		border = 1;

		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 10;
	});

	%this.SafeCreateNamedObject("GuiSolidProfile", new GuiControlProfile()
	{
		// fill color
	    fillColor = %this.color1;
	    fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	    fillColorSL = %this.AdjustColorValue(%this.color1, 15);
	    fillColorNA = %this.SetColorAlpha(%this.color1, 100);

		borderDefault = GuiSolidBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiToolTipProfile", new GuiControlProfile()
	{
	    fillColor = %this.SetColorAlpha(%this.color1, 220);
		fontColor = %this.color5;

		borderDefault = GuiBrightBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiTextProfile", new GuiControlProfile()
	{
	    fontColor = %this.color3;

	    align = "left";
	    autoSizeWidth = false;
	    autoSizeHeight = false;
	    returnTab = false;
	    numbersOnly = false;
	    cursorColor = "0 0 0 255";
	});

	%this.SafeCreateNamedObject("GuiTextArrayProfile", new GuiControlProfile()
	{
		fontColor = %this.color3;

	    align = "left";
	    autoSizeWidth = false;
	    autoSizeHeight = false;
	    returnTab = false;
	    numbersOnly = false;
	    cursorColor = "0 0 0 255";

	   fontColorHL = %this.color3;
	   fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	});

	%this.SafeCreateNamedObject("GuiTextRightProfile", new GuiControlProfile()
	{
		fontColor = %this.color3;

	    align = "right";
	    autoSizeWidth = false;
	    autoSizeHeight = false;
	    returnTab = false;
	    numbersOnly = false;
	    cursorColor = "0 0 0 255";
	});

	%this.SafeCreateNamedObject("GuiCheckBoxProfile", new GuiControlProfile()
	{
	    fillColor = %this.color3;
	    fillColorHL = %this.AdjustColorValue(%this.color3, -10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color3, 100);

	    fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color5, -10);
		fontColorSL = %this.color5;
		fontColorNA = %this.SetColorAlpha(%this.color3, 100);
	    align = "left";
		tab = true;

		borderDefault = "GuiBrightBorderProfile";
		borderRight = "GuiDarkBorderProfile";
		borderBottom = "GuiDarkBorderProfile";
	});

	%this.SafeCreateNamedObject("GuiTextEditProfile", new GuiControlProfile()
	{
	    fillColor = "232 240 248 255";
	    fillColorHL = "242 250 255 255";
	    fillColorNA = "127 127 127 52";
		fillColorTextSL = "251 170 0 255";
	    fontColor = "27 59 95 255";
	    fontColorHL = "27 59 95 255";
	    fontColorNA = "0 0 0 52";
	    fontColorSL = "0 0 0 255";
	    textOffset = "5 2";
	    tab = false;
	    canKeyFocus = true;
	    returnTab = true;
	});

	%this.SafeCreateNamedObject("GuiScrollTrackProfile", new GuiControlProfile()
	{
		fillColor = %this.color1;
	    fillColorHL = %this.color1;
	    fillColorSL = %this.color1;
	    fillColorNA = %this.color1;
	});

	%this.SafeCreateNamedObject("GuiScrollBrightBorderProfile", new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 2;
		paddingSL = 2;
		paddingNA = 3;

		border = 1;
		borderHL = 2;
		borderSL = 2;
		borderNA = 1;

		borderColor = "255 255 255 50";
		borderColorHL = "255 255 255 50";
		borderColorSL = "255 255 255 50";
		borderColorNA = "255 255 255 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiScrollDarkBorderProfile", new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 2;
		paddingSL = 2;
		paddingNA = 3;

		border = 1;
		borderHL = 2;
		borderSL = 2;
		borderNA = 1;

		borderColor = "0 0 0 20";
		borderColorHL = "0 0 0 20";
		borderColorSL = "0 0 0 20";
		borderColorNA = "0 0 0 20";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiScrollThumbProfile", new GuiControlProfile()
	{
		fillColor = %this.color3;
	    fillColorHL = %this.AdjustColorValue(%this.color3, 10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color3, 100);

		borderDefault = GuiScrollBrightBorderProfile;
		borderRight = GuiScrollDarkBorderProfile;
		borderBottom = GuiScrollDarkBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiScrollArrowProfile", new GuiControlProfile()
	{
		fillColor = %this.color3;
	    fillColorHL = %this.AdjustColorValue(%this.color3, 10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color3, 100);

		fontColor = "0 0 0 100";
	    fontColorHL = "0 0 0 150";
	    fontColorSL = %this.color5;
	    fontColorNA = "0 0 0 50";

		borderDefault = GuiScrollBrightBorderProfile;
		borderRight = GuiScrollDarkBorderProfile;
		borderBottom = GuiScrollDarkBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiScrollProfile", new GuiControlProfile()
	{
	    fillColor = %this.color2;
	    borderDefault = GuiDefaultBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiTransparentScrollProfile", new GuiControlProfile()
	{
	   fillColor = "255 255 255 0";
	   fillColor = %this.color2;
	   borderDefault = GuiDefaultBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiDarkButtonBorderProfile", new GuiBorderProfile()
	{
		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;

		border = 3;
		borderHL = 3;
		borderSL = 3;
		borderNA = 3;

		borderColor = "0 0 0 50";
		borderColorHL = "0 0 0 50";
		borderColorSL = "0 0 0 50";
		borderColorNA = "0 0 0 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiBrightButtonBorderProfile", new GuiBorderProfile()
	{
		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;

		border = 3;
		borderHL = 3;
		borderSL = 3;
		borderNA = 3;

		borderColor = "255 255 255 50";
		borderColorHL = "255 255 255 50";
		borderColorSL = "255 255 255 50";
		borderColorNA = "255 255 255 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiButtonProfile", new GuiControlProfile()
	{
		fillColor = %this.color3;
		fillColorHL = %this.AdjustColorValue(%this.color3, 10);
		fillColorSL = %this.AdjustColorValue(%this.color3, 15);
		fillColorNA = %this.SetColorAlpha(%this.color3, 80);

		fontColor = %this.color1;
		fontColorHL = %this.color4;
		fontColorSL = %this.color1;
		fontColorNA = %this.SetColorAlpha(%this.color1, 100);
		align = center;
		vAlign = middle;

		borderLeft = GuiBrightButtonBorderProfile;
		borderRight = GuiDarkButtonBorderProfile;
		borderTop = GuiBrightButtonBorderProfile;
		borderBottom = GuiDarkButtonBorderProfile;

		canKeyFocus = true;
		tab = true;
	});

	%this.SafeCreateNamedObject("GuiRadioProfile", new GuiControlProfile()
	{
		fillColor = %this.color3;
	    fillColorHL = %this.AdjustColorValue(%this.color3, -10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color3, 100);

	    fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color5, -10);
		fontColorSL = %this.color5;
		fontColorNA = %this.SetColorAlpha(%this.color3, 100);
	    align = "left";
		tab = true;

		borderLeft = GuiBrightButtonBorderProfile;
		borderRight = GuiDarkButtonBorderProfile;
		borderTop = GuiBrightButtonBorderProfile;
		borderBottom = GuiDarkButtonBorderProfile;

		canKeyFocus = true;
		tab = true;
	});

	%this.SafeCreateNamedObject("GuiHeaderProfile", new GuiControlProfile()
	{
	    fillColor = %this.color2;
	    fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	    fillColorSL = %this.AdjustColorValue(%this.color1, 15);
	    fillColorNA = %this.SetColorAlpha(%this.color1, 100);

		fontColor = %this.color3;
		fontSize = %this.platformFontSize + 2;
		align = "left";

		borderDefault = "GuiBrightBorderProfile";
		borderRight = "GuiDarkBorderProfile";
		borderBottom = "GuiDarkBorderProfile";
	});

	%this.SafeCreateNamedObject("GuiLabelProfile", new GuiControlProfile()
	{
		fontColor = "255 255 255 255";
		fontSize = %this.platformFontSize;
		align = "left";
	});

	%this.SafeCreateNamedObject("GuiDragAndDropProfile", new GuiControlProfile()
	{
	   fillColor = %this.SetColorAlpha(%this.color4, 50);
	   fontColor = %this.color5;
	});

	%this.SafeCreateNamedObject("GuiProgressBorderProfile", new GuiBorderProfile()
	{
		padding = 2;
		paddingHL = 0;
		paddingSL = 0;

		border = 2;
		borderHL = 2;
		borderSL = 2;

		margin = 0;
		marginHL = 3;
		marginSL = 3;

		borderColor = %this.AdjustColorValue(%this.color1, -10);
		borderColorHL = "255 255 255 50";
		borderColorSL = "255 255 255 50";
	});

	%this.SafeCreateNamedObject("GuiProgressDarkBorderProfile", new GuiBorderProfile()
	{
		padding = 2;
		paddingHL = 0;
		paddingSL = 0;

		border = 2;
		borderHL = 2;
		borderSL = 2;

		margin = 0;
		marginHL = 3;
		marginSL = 3;

		borderColor = %this.AdjustColorValue(%this.color1, -10);
		borderColorHL = "0 0 0 50";
		borderColorSL = "0 0 0 50";
	});

	%this.SafeCreateNamedObject("GuiProgressProfile", new GuiControlProfile()
	{
	   fillColor = %this.color1;
	   fillColorHL = %this.color4;
	   fillColorSL = %this.AdjustColorValue(%this.color4, 10);

	   fontColorHL = %this.color3;
	   fontColorSL = %this.color5;

	   borderDefault = GuiProgressBorderProfile;
	   borderBottom = GuiProgressDarkBorderProfile;
	   borderRight = GuiProgressDarkBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiDropDownDarkBorderProfile", new GuiBorderProfile()
	{
		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		borderColor = "0 0 0 50";
		borderColorHL = "0 0 0 50";
		borderColorSL = "0 0 0 50";
		borderColorNA = "0 0 0 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiDropDownBrightBorderProfile", new GuiBorderProfile()
	{
		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		borderColor = "255 255 255 50";
		borderColorHL = "255 255 255 50";
		borderColorSL = "255 255 255 50";
		borderColorNA = "255 255 255 50";

		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiDropDownProfile", new GuiControlProfile()
	{
	    // fill color
	    fillColor = %this.AdjustColorValue(%this.color3, -15);
		fillColorHL = %this.AdjustColorValue(%this.color3, -8);
		fillColorSL = %this.color4;
		fillColorNA = %this.SetColorAlpha(%this.color3, 100);

	    fontColor = %this.color1;
		fontColorHL = %this.color1;
		fontColorSL = %this.color3;
		fontColorNA = %this.SetColorAlpha(%this.color1, 100);
		align = "left";

		tab = true;
		canKeyFocus = true;

		borderDefault = GuiDropDownBrightBorderProfile;
		borderRight = GuiDropDownDarkBorderProfile;
		borderBottom = GuiDropDownDarkBorderProfile;
		category = "dropDown";
	});

	%this.SafeCreateNamedObject("GuiMenuBarBorderProfile", new GuiBorderProfile()
	{
		padding = 2;
	});

	%this.SafeCreateNamedObject("GuiMenuBarProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -7);
		canKeyFocus = true;
		borderDefault = GuiMenuBarBorderProfile;
		category = "defaultMenuBar";
	});

	%this.SafeCreateNamedObject("GuiMenuBorderProfile", new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 0;
		marginSL = 0;
		marginNA = 2;

		border = 0;
		borderHL = 2;
		borderSL = 2;
		borderNA = 0;

		borderColorHL = "255 255 255 30";
		borderColorSL = %this.color4;
	});

	%this.SafeCreateNamedObject("GuiMenuBottomBorderProfile", new GuiBorderProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -7);
		canKeyFocus = true;
		borderDefault = GuiMenuBarBorderProfile;
		category = "defaultMenuBar";

		paddingSL = 2;
		marginSL = 0;
		borderSL = 0;
	});

	%this.SafeCreateNamedObject("GuiMenuSideBorderProfile", new GuiBorderProfile()
	{
		border = 0;
		borderHL = 2;
		borderSL = 2;
		borderNA = 0;

		padding = 10;
		paddingHL = 8;
		paddingSL = 8;
		paddingNA = 10;

		borderColorHL = "255 255 255 30";
		borderColorSL = %this.color4;
	});

	%this.SafeCreateNamedObject("GuiMenuProfile", new GuiControlProfile()
	{
		fillColor = "0 0 0 0";
		fillColorHL = "255 255 255 10";
		fillColorSL = %this.AdjustColorValue(%this.color4, -15);
		fillColorNA = "0 0 0 0";

		borderDefault = GuiMenuBorderProfile;
		borderLeft = GuiMenuSideBorderProfile;
		borderRight = GuiMenuSideBorderProfile;
		borderBottom = GuiMenuBottomBorderProfile;
		category = "defaultMenuBar";

		fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color3, 10);
		fontColorSL = %this.color3;
		fontColorNA = %this.SetColorAlpha(%this.color3, 100);
	});

	%this.SafeCreateNamedObject("GuiMenuContentVertBorderProfile", new GuiBorderProfile()
	{
		border = 2;
		padding = 4;
		borderColor = %this.color4;
	});

	%this.SafeCreateNamedObject("GuiMenuContentSideBorderProfile", new GuiBorderProfile()
	{
		border = 2;
		padding = 0;
		borderColor = %this.color4;
	});

	%this.SafeCreateNamedObject("GuiMenuContentProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -5);

		borderDefault = GuiMenuContentSideBorderProfile;
		borderTop = GuiMenuContentVertBorderProfile;
		borderBottom = GuiMenuContentVertBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiMenuItemBorderTopProfile", new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 4;
		borderSL = 1;
		borderColorSL = "0 0 0 50";
	});

	%this.SafeCreateNamedObject("GuiMenuItemBorderBottomProfile", new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 4;
		borderSL = 1;
		borderColorSL = "255 255 255 50";
	});

	%this.SafeCreateNamedObject("GuiMenuItemBorderSideProfile", new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 0;
		borderSL = 0;
		paddingSL = 6;
	});

	%this.SafeCreateNamedObject("GuiMenuItemProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -5);
		fillColorHL = %this.AdjustColorValue(%this.color4, -15);
		fillColorNA = %this.color1;
		align = left;

		fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color3, 10);
		fontColorNA = %this.SetColorAlpha(%this.color3, 150);

		borderDefault = GuiMenuItemBorderSideProfile;
		borderTop = GuiMenuItemBorderTopProfile;
		borderBottom = GuiMenuItemBorderBottomProfile;
	});

	%this.SafeCreateNamedObject("GuiTextEditProfile", new GuiControlProfile()
	{
		fillColor = %this.color4;
		fillColorHL = %this.AdjustColorValue(%this.color4, 10);
		fillColorSL = %this.color4;
		fillColorNA = %this.AdjustColorValue(%this.color4, 80);
		fillColorTextSL = %this.color5;

		fontColor = %this.SetColorAlpha(%this.color1, 220);
		fontColorHL = %this.AdjustColorValue(%this.color1, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.SetColorAlpha(%this.color1, 100);
		fontColorTextSL = %this.color1;
		align = left;
		vAlign = middle;
		cursorColor = %this.color2;

		tab = true;
		canKeyFocus = true;
		returnTab = true;
	});

	%this.SafeCreateNamedObject("GuiButtonProfile", new GuiControlProfile()
	{
		fillColor = %this.color4;
		fillColorHL = %this.AdjustColorValue(%this.color4, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.SetColorAlpha(%this.color4, 80);

		fontColor = %this.color1;
		fontColorHL = %this.AdjustColorValue(%this.color1, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.SetColorAlpha(%this.color1, 100);
		align = center;
		vAlign = middle;

		borderLeft = GuiBrightBorderProfile;
		borderRight = GuiDarkBorderProfile;
		borderTop = GuiBrightBorderProfile;
		borderBottom = GuiDarkBorderProfile;

		canKeyFocus = true;
		tab = true;
	});

	%this.SafeCreateNamedObject("GuiCheckboxProfile", new GuiControlProfile()
	{
		fillColor = %this.color4;
		fillColorHL = %this.AdjustColorValue(%this.color4, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.SetColorAlpha(%this.color4, 80);

		fontColor = %this.color1;
		fontColorHL = %this.AdjustColorValue(%this.color1, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.SetColorAlpha(%this.color1, 100);
		align = center;
		vAlign = middle;

		borderLeft = GuiBrightBorderProfile;
		borderRight = GuiDarkBorderProfile;
		borderTop = GuiBrightBorderProfile;
		borderBottom = GuiDarkBorderProfile;

		canKeyFocus = true;
		tab = true;
	});

	%this.SafeCreateNamedObject("GuiColorPopupBorderProfile", new GuiBorderProfile()
	{
		border = 1;
		borderHL = 1;
		borderSL = 1;
		borderNA = 1;

		borderColor = %this.AdjustColorValue(%this.color1, -5);
		borderColorHL = %this.AdjustColorValue(%this.color1, -5);
		borderColorSL = %this.AdjustColorValue(%this.color1, -5);
		borderColorNA = %this.AdjustColorValue(%this.color1, -5);

		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;
	});

	%this.SafeCreateNamedObject("GuiColorPopupProfile", new GuiControlProfile()
	{
		fillColor = %this.color1;
		fillColorHL = %this.color1;
		fillColorSL = %this.color1;
		fillColorNA = %this.color1;

		borderDefault = GuiColorPopupBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiColorPickerBorderProfile", new GuiBorderProfile()
	{
		border = 1;
		borderHL = 1;
		borderSL = 1;
		borderNA = 1;

		borderColor = %this.color1;
		borderColorHL = %this.color2;
		borderColorSL = %this.color4;
		borderColorNA = %this.color1;

		padding = 1;
		paddingHL = 1;
		paddingSL = 1;
		paddingNA = 1;

		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;
	});

	%this.SafeCreateNamedObject("GuiColorPickerProfile", new GuiControlProfile()
	{
		fillColor = "0 0 0 0";
		fillColorHL = "0 0 0 0";
		fillColorSL = "0 0 0 0";
		fillColorNA = "0 0 0 0";
		
		borderDefault = GuiColorPickerBorderProfile;

		tab = false;
		canKeyFocus = true;
		underfill = true;
	});

	%this.SafeCreateNamedObject("GuiColorSelectorBorderProfile", new GuiBorderProfile()
	{
		border = 1;
		borderHL = 1;
		borderSL = 1;

		borderColor = "0 0 0 180";
		borderColorHL = "0 0 0 190";
		borderColorSL = "0 0 0 220";

		//Margin and padding are used to determine the size of the selector ring in the normal state.
		padding = 2;//Radius of the outer circle minus the inner circle. The meat of the donut.
		margin = 1;//Radius of the inner circle plus 1. This is different from the normal use of the margin.
	});

	%this.SafeCreateNamedObject("GuiColorSelectorProfile", new GuiControlProfile()
	{
		fillColor = "240 240 240 255";
		fillColorHL = "250 250 250 255";
		fillColorSL = %this.color5;
		
		borderDefault = GuiColorSelectorBorderProfile;
	});
}
