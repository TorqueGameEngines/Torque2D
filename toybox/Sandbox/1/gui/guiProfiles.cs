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

if ($platform $= "windows")
	$platformFontType = "lucida console";
else if ($platform $= "Android")
	$platformFontType = "Droid";
else
	$platformFontType = "monaco";
if ($platform $= "ios")
	$platformFontSize = 18;
else if ($platform $= "Android")
	$platformFontSize = 14;
else
	$platformFontSize = 12;

$color[1] = "43 43 43 255";
$color[2] = "81 92 102 255";
$color[3] = "224 224 224 255";
$color[4] = "54 135 196 255";
$color[5] = "245 210 50 255";
$color[6] = "196 54 71 255";

function AdjustColorValue(%color, %percent)
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

function SetColorAlpha(%color, %newAlpha)
{
	%red = getWord(%color, 0);
	%green = getWord(%color, 1);
	%blue = getWord(%color, 2);
	return %red SPC %green SPC %blue SPC mRound(mClamp(%newAlpha, 0, 255));
}

function SafeCreateNamedObject(%name, %object)
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

//-----------------------------------------------------------------------------

SafeCreateNamedObject("DefaultCursor", new GuiCursor()
{
    hotSpot = "3 3";
    renderOffset = "0 0";
    bitmapName = "^Sandbox/gui/images/defaultCursor";
});

SafeCreateNamedObject("LeftRightCursor", new GuiCursor()
{
   hotSpot = "0.5 0";
   renderOffset = "0.5 0";
   bitmapName = "./Images/leftRight";
});

SafeCreateNamedObject("UpDownCursor", new GuiCursor()
{
   hotSpot = "1 1";
   renderOffset = "0 1";
   bitmapName = "./Images/upDown";
});

SafeCreateNamedObject("NWSECursor", new GuiCursor()
{
   hotSpot = "1 1";
   renderOffset = "0.5 0.5";
   bitmapName = "./Images/NWSE";
});

SafeCreateNamedObject("NESWCursor", new GuiCursor()
{
   hotSpot = "1 1";
   renderOffset = "0.5 0.5";
   bitmapName = "./Images/NESW";
});

SafeCreateNamedObject("MoveCursor", new GuiCursor()
{
   hotSpot = "1 1";
   renderOffset = "0.5 0.5";
   bitmapName = "./Images/move";
});

SafeCreateNamedObject("EditCursor", new GuiCursor()
{
   hotSpot = "0 0";
   renderOffset = "0.5 0.5";
   bitmapName = "./Images/ibeam";
});

SafeCreateNamedObject("GuiDefaultBorderProfile", new GuiBorderProfile()
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
	borderColor   = $color1;
    borderColorHL = AdjustColorValue($color1, 10);
    borderColorSL = AdjustColorValue($color1, 10);
    borderColorNA = SetColorAlpha($color1, 100);
	//Default Padding
	padding = 0;
	paddingHL = 0;
	paddingSL = 0;
	paddingNA = 0;
	//Default underfill
	underfill = true;
});

SafeCreateNamedObject("GuiDefaultProfile", new GuiControlProfile()
{
    // fill color
    fillColor = "0 0 0 0";

    // font
    fontType = $platformFontType;
	fontDirectory = expandPath( "^AppCore/fonts" );
    fontSize = $platformFontSize;
    fontColor = "255 255 255 255";
	align = center;
	vAlign = middle;

	cursorColor = "0 0 0 255";

	borderDefault = GuiDefaultBorderProfile;
	category = "default";
});

SafeCreateNamedObject("GuiBrightBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiDarkBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiPanelProfile", new GuiControlProfile()
{
	fillColor = $color1;
    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = AdjustColorValue($color1, 15);
    fillColorNA = SetColorAlpha($color1, 100);
	borderDefault = GuiBrightBorderProfile;
	category = "defaultPanel";
});

SafeCreateNamedObject("GuiListBoxBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiListBoxProfile", new GuiControlProfile()
{
    // fill color
    fillColor = $color1;
    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color1, 100);
	align = left;

	tab = false;
	canKeyFocus = true;
	category = "defaultListBox";

	fontColor = $color3;
	fontColorHL = AdjustColorValue($color3, 20);
	fontColorSL = AdjustColorValue($color3, 20);
	fontColorNA = AdjustColorValue($color3, -30);

	borderDefault = GuiListBoxBorderProfile;
});

SafeCreateNamedObject("GuiWindowBorderProfile", new GuiBorderProfile()
{
	padding = 10;
	paddingHL = 10;
	paddingSL = 10;
	paddingNA = 4;
});

SafeCreateNamedObject("GuiWindowProfile", new GuiControlProfile()
{
   fillColor = AdjustColorValue($color1, 10);
   fillColorHL = AdjustColorValue($color1, 12);
   fillColorSL = $color4;
   fillColorNA = $color1;
   category = "defaultWindow";
   align = "Left";

   fontColorSL = $color5;

   borderLeft = GuiWindowBorderProfile;
});

SafeCreateNamedObject("GuiWindowContentBorderProfile", new GuiBorderProfile()
{
	borderColor = AdjustColorValue($color1, 10);
	borderColorSL = AdjustColorValue($color4, -10);

	border = 3;
	borderSL = 3;
});

SafeCreateNamedObject("GuiWindowContentProfile", new GuiControlProfile()
{
	fillColor = AdjustColorValue($color1, -10);
	fillColorSL = AdjustColorValue($color1, -10);

	borderDefault = GuiWindowContentBorderProfile;
	borderTop = GuiDefaultBorderProfile;
});

SafeCreateNamedObject("GuiWindowButtonBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiWindowCloseButtonProfile", new GuiControlProfile()
{
   fillColor = SetColorAlpha($color1, 150);
   fillColorHL = SetColorAlpha($color6, 150);
   fillColorSL = AdjustColorValue($color6, 10);
   fillColorNA = $color1;

   fontColor = SetColorAlpha($color3, 150);
   fontColorHL = SetColorAlpha($color3, 170);
   fontColorSL = $color5;
   fontColorNA = SetColorAlpha($color3, 150);

   borderDefault = GuiWindowButtonBorderProfile;
});

SafeCreateNamedObject("GuiWindowMinButtonProfile", new GuiControlProfile()
{
   fillColor = SetColorAlpha($color1, 150);
   fillColorHL = SetColorAlpha($color4, 150);
   fillColorSL = AdjustColorValue($color4, 10);
   fillColorNA = $color1;

   fontColor = SetColorAlpha($color3, 150);
   fontColorHL = SetColorAlpha($color3, 170);
   fontColorSL = $color5;
   fontColorNA = SetColorAlpha($color3, 150);

   borderDefault = GuiWindowButtonBorderProfile;
});

SafeCreateNamedObject("GuiWindowMaxButtonProfile", new GuiControlProfile()
{
	fillColor = SetColorAlpha($color1, 150);
    fillColorHL = SetColorAlpha($color4, 150);
    fillColorSL = AdjustColorValue($color4, 10);
    fillColorNA = $color1;

    fontColor = SetColorAlpha($color3, 150);
    fontColorHL = SetColorAlpha($color3, 170);
    fontColorSL = $color5;
    fontColorNA = SetColorAlpha($color3, 150);

    borderDefault = GuiWindowButtonBorderProfile;
});
SafeCreateNamedObject("GuiTransparentProfile", new GuiControlProfile());

SafeCreateNamedObject("GuiGridProfile", new GuiControlProfile());

SafeCreateNamedObject("GuiChainProfile", new GuiControlProfile());

SafeCreateNamedObject("GuiTabBookProfile", new GuiControlProfile()
{
	fillColor = SetColorAlpha($color1, 100);
	category = "defaultTabBook";
});
SafeCreateNamedObject("GuiTabProfile", new GuiControlProfile()
{
	fontColor = "255 255 255 255";
    fontColorHL = "232 240 248 255";
    fontColorSL= "255 255 255 255";
    fontColorNA = "0 0 0 255";
	fillColor = $color1;
	fillColorHL = AdjustColorValue($color1, 10);
	fillColorSL = AdjustColorValue($color1, 15);
	fillColorNA = SetColorAlpha($color1, 100);
	borderDefault = GuiBrightBorderProfile;
	align = Center;
	category = "defaultTab";
});
SafeCreateNamedObject("GuiTabPageProfile", new GuiControlProfile()
{
	fillColor = $color1;
	fillColorHL = AdjustColorValue($color1, 10);
	fillColorSL = AdjustColorValue($color1, 15);
	fillColorNA = SetColorAlpha($color1, 100);
	category = "defaultTabPage";
});


SafeCreateNamedObject("GuiSpriteProfile", new GuiControlProfile());

// ----------------------------------------------------------------------------
SafeCreateNamedObject("GuiTreeViewProfile", new GuiControlProfile()
{
	fontColor = "255 255 255 255";
    fontColorHL = "232 240 248 255";
    fontColorSL= "255 255 255 255";
    fontColorNA = "0 0 0 255";
	fillColor = $color1;
	fillColorHL = AdjustColorValue($color1, 10);
	fillColorSL = AdjustColorValue($color1, 15);
	fillColorNA = SetColorAlpha($color1, 100);
	bitmap = "./images/treeView";
	canKeyFocus = true;
	autoSizeHeight = true;
});
// ----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSolidBorderProfile", new GuiBorderProfile()
{
	border = 1;

	padding = 10;
	paddingHL = 10;
	paddingSL = 10;
	paddingNA = 10;
});

SafeCreateNamedObject("GuiSolidProfile", new GuiControlProfile()
{
	// fill color
    fillColor = $color1;
    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = AdjustColorValue($color1, 15);
    fillColorNA = SetColorAlpha($color1, 100);

	borderDefault = GuiSolidBorderProfile;
});

SafeCreateNamedObject("GuiToolTipProfile", new GuiControlProfile()
{
    fillColor = SetColorAlpha($color1, 220);
	fontColor = $color5;

	borderDefault = GuiBrightBorderProfile;
});

// ----------------------------------------------------------------------------

SafeCreateNamedObject("GuiPopUpMenuDefault", new GuiControlProfile ()
{
    // fill color
    fillColor = "255 255 255 192";
    fillColorHL = "255 0 0 192";
    fillColorSL = "255 0 0 192";
    fillColorNA = "0 0 255 255";

    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorSL= "255 255 255 255";
    fontColorNA = "0 0 0 255";
});

// ----------------------------------------------------------------------------

SafeCreateNamedObject("GuiPopUpMenuProfile", new GuiControlProfile ()
{
	// fill color
	fillColor = "255 255 255 192";
	fillColorHL = "255 0 0 192";
	fillColorSL = "255 0 0 192";
	fillColorNA = "0 0 255 255";

	fontColor = "27 59 95 255";
	fontColorHL = "232 240 248 255";
	fontColorSL= "255 255 255 255";
	fontColorNA = "0 0 0 255";

    align = "center";
    bitmap = "^Sandbox/gui/images/dropDown1";
    profileForChildren = GuiPopUpMenuDefault;
});

SafeCreateNamedObject("GuiPopUpMenuProfile2", new GuiControlProfile ()
{
	// fill color
	fillColor = "255 255 255 192";
	fillColorHL = "255 0 0 192";
	fillColorSL = "255 0 0 192";
	fillColorNA = "0 0 255 255";

	fontColor = $color3;
	fontColorHL = "232 240 248 255";
	fontColorSL= "255 255 255 255";
	fontColorNA = "0 0 0 255";

    align = "center";
    bitmap = "^Sandbox/gui/images/dropDown2";
    profileForChildren = GuiPopUpMenuDefault;
});


//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiTextProfile", new GuiControlProfile()
{
    fontColor = $color3;

    align = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";
});

SafeCreateNamedObject("GuiTextArrayProfile", new GuiControlProfile()
{
	fontColor = $color3;

    align = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";

   fontColorHL = $color3;
   fillColorHL = AdjustColorValue($color1, 10);
});

SafeCreateNamedObject("GuiTextRightProfile", new GuiControlProfile()
{
	fontColor = $color3;

    align = "right";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiCheckBoxProfile", new GuiControlProfile()
{
    fillColor = $color3;
    fillColorHL = AdjustColorValue($color3, -10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color3, 100);

    fontColor = $color3;
	fontColorHL = AdjustColorValue($color5, -10);
	fontColorSL = $color5;
	fontColorNA = SetColorAlpha($color3, 100);
    align = "left";
	tab = true;

	borderDefault = "GuiBrightBorderProfile";
	borderRight = "GuiDarkBorderProfile";
	borderBottom = "GuiDarkBorderProfile";
	bitmap = "^Sandbox/gui/images/checkBox.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiTextEditProfile", new GuiControlProfile()
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

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiScrollTrackProfile", new GuiControlProfile()
{
	fillColor = $color1;
    fillColorHL = $color1;
    fillColorSL = $color1;
    fillColorNA = $color1;
});

SafeCreateNamedObject("GuiScrollBrightBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiScrollDarkBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiScrollThumbProfile", new GuiControlProfile()
{
	fillColor = $color3;
    fillColorHL = AdjustColorValue($color3, 10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color3, 100);

	borderDefault = GuiScrollBrightBorderProfile;
	borderRight = GuiScrollDarkBorderProfile;
	borderBottom = GuiScrollDarkBorderProfile;
});

SafeCreateNamedObject("GuiScrollArrowProfile", new GuiControlProfile()
{
	fillColor = $color3;
    fillColorHL = AdjustColorValue($color3, 10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color3, 100);

	fontColor = "0 0 0 100";
    fontColorHL = "0 0 0 150";
    fontColorSL = $color5;
    fontColorNA = "0 0 0 50";

	borderDefault = GuiScrollBrightBorderProfile;
	borderRight = GuiScrollDarkBorderProfile;
	borderBottom = GuiScrollDarkBorderProfile;
});

SafeCreateNamedObject("GuiScrollProfile", new GuiControlProfile()
{
    fillColor = $color2;
    borderDefault = GuiDefaultBorderProfile;
});

SafeCreateNamedObject("GuiTransparentScrollProfile", new GuiControlProfile()
{
   fillColor = "255 255 255 0";
   fillColor = $color2;
   borderDefault = GuiDefaultBorderProfile;
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("SandboxWindowProfile", new GuiControlProfile ()
{
    // fill color
    fillColor = "0 0 0 100";

    // font
    fontColor = "255 255 255 255";
	lockMouse = "0";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiButtonProfile", new GuiControlProfile ()
{
    fontColor = "255 255 255 255";
    fontColorHL = $color5;
    bitmap = "^Sandbox/gui/images/smallButtonContainer";
});

SafeCreateNamedObject("GuiButtonDynProfile", new GuiControlProfile ()
{
    fontColor = "0 0 0 255";
    fontColorHL = $color5;
    bitmap = "^Sandbox/gui/images/smallButtonContainer";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiNorthButtonProfile", new GuiControlProfile ()
{
    imageAsset = "Sandbox:northArrow";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSouthButtonProfile", new GuiControlProfile ()
{
    imageAsset = "Sandbox:southArrow";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiPlusButtonProfile", new GuiControlProfile ()
{
    imageAsset = "Sandbox:plusButton";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiMinusButtonProfile", new GuiControlProfile ()
{
    imageAsset = "Sandbox:minusButton";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("BlueButtonProfile", new GuiControlProfile ()
{
    fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/blueButton.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("RedButtonProfile", new GuiControlProfile ()
{
    fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/redButton.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GreenButtonProfile", new GuiControlProfile ()
{
	fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/greenButton.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiRadioProfile", new GuiControlProfile ()
{
    fillColor = "232 232 232 255";
    bitmap = "^Sandbox/gui/images/radioButton.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSliderProfile", new GuiControlProfile ()
{
    bitmap = "^Sandbox/gui/images/slider.png";
    fontType = $platformFontType;
    fontSize = $platformFontSize;
    fontColor = "white";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSliderNoTextProfile", new GuiControlProfile ()
{
    bitmap = "^Sandbox/gui/images/slider.png";
    fontColor = "white";
    fontSize = 1;
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSpinnerProfile", new GuiControlProfile ()
{
    fillColor = $color3;
    fillColorHL = $color3;
	fillColorSL = $color3;
    fillColorNA = SetColorAlpha(%this.color3, 100);
	fillColorTextSL = $color5;
    numbersOnly = true;

	fontSize = $platformFontSize + 2;
    fontColor = $color1;
    fontColorHL = $color1;
	fontColorSL = $color1;
    fontColorNA = SetColorAlpha($color1, 100);
	fontColorTextSL = $color4;

	borderTop = "GuiDarkBorderProfile";
	borderBottom = "GuiBrightBorderProfile";

    tab = false;
    canKeyFocus = true;
    returnTab = true;
	bitmap = "^Sandbox/gui/images/textEdit_noSides.png";
});

//-----------------------------------------------------------------------------

SafeCreateNamedObject("GuiSunkenContainerProfile", new GuiControlProfile ()
{
    fillColor = SetColorAlpha($color1, 150);
    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = AdjustColorValue($color1, 15);
    fillColorNA = SetColorAlpha($color1, 100);
	bitmap = "^Sandbox/gui/images/sunkenContainer.png";

	borderDefault = GuiSolidBorderProfile;
});

SafeCreateNamedObject("GuiHeaderProfile", new GuiControlProfile ()
{
    fillColor = $color2;
	fontColor = $color3;
	fontSize = $platformFontSize + 2;
	align = "left";

	borderDefault = "GuiBrightBorderProfile";
	borderRight = "GuiDarkBorderProfile";
	borderBottom = "GuiDarkBorderProfile";

    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = AdjustColorValue($color1, 15);
    fillColorNA = SetColorAlpha($color1, 100);
});

SafeCreateNamedObject("GuiLabelProfile", new GuiControlProfile ()
{
	fontColor = "255 255 255 255";
	fontSize = $platformFontSize;
	align = "left";
});

SafeCreateNamedObject("GuiDragAndDropProfile", new GuiControlProfile ()
{
   fillColor = SetColorAlpha($color4, 50);
   fontColor = $color5;
});

SafeCreateNamedObject("GuiProgressBorderProfile", new GuiBorderProfile()
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

	borderColor = AdjustColorValue($color1, -10);
	borderColorHL = "255 255 255 50";
	borderColorSL = "255 255 255 50";
});

SafeCreateNamedObject("GuiProgressDarkBorderProfile", new GuiBorderProfile()
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

	borderColor = AdjustColorValue($color1, -10);
	borderColorHL = "0 0 0 50";
	borderColorSL = "0 0 0 50";
});

SafeCreateNamedObject("GuiProgressProfile", new GuiControlProfile()
{
   fillColor = $color1;
   fillColorHL = $color4;
   fillColorSL = AdjustColorValue($color4, 10);

   fontColorHL = $color3;
   fontColorSL = $color5;

   borderDefault = GuiProgressBorderProfile;
   borderBottom = GuiProgressDarkBorderProfile;
   borderRight = GuiProgressDarkBorderProfile;
});

SafeCreateNamedObject("GuiDropDownDarkBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiDropDownBrightBorderProfile", new GuiBorderProfile()
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

SafeCreateNamedObject("GuiDropDownProfile", new GuiControlProfile()
{
    // fill color
    fillColor = AdjustColorValue($color3, -15);
	fillColorHL = AdjustColorValue($color3, -8);
	fillColorSL = $color4;
	fillColorNA = SetColorAlpha($color3, 100);

    fontColor = $color1;
	fontColorHL = $color1;
	fontColorSL = $color3;
	fontColorNA = SetColorAlpha($color1, 100);
	align = "left";

	tab = true;
	canKeyFocus = true;

	borderDefault = GuiDropDownBrightBorderProfile;
	borderRight = GuiDropDownDarkBorderProfile;
	borderBottom = GuiDropDownDarkBorderProfile;
	category = "dropDown";
});

SafeCreateNamedObject("GuiMenuBarBorderProfile", new GuiBorderProfile()
{
	padding = 2;
});

SafeCreateNamedObject("GuiMenuBarProfile", new GuiControlProfile()
{
	fillColor = AdjustColorValue($color1, -7);
	canKeyFocus = true;
	borderDefault = GuiMenuBarBorderProfile;
	category = "defaultMenuBar";
});

SafeCreateNamedObject("GuiMenuBorderProfile", new GuiBorderProfile()
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
	borderColorSL = $color4;
});

SafeCreateNamedObject("GuiMenuBottomBorderProfile", new GuiBorderProfile()
{
	fillColor = AdjustColorValue($color1, -7);
	canKeyFocus = true;
	borderDefault = GuiMenuBarBorderProfile;
	category = "defaultMenuBar";

	paddingSL = 2;
	marginSL = 0;
	borderSL = 0;
});

SafeCreateNamedObject("GuiMenuSideBorderProfile", new GuiBorderProfile()
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
	borderColorSL = $color4;
});

SafeCreateNamedObject("GuiMenuProfile", new GuiControlProfile()
{
	fillColor = "0 0 0 0";
	fillColorHL = "255 255 255 10";
	fillColorSL = AdjustColorValue($color4, -15);
	fillColorNA = "0 0 0 0";

	borderDefault = GuiMenuBorderProfile;
	borderLeft = GuiMenuSideBorderProfile;
	borderRight = GuiMenuSideBorderProfile;
	borderBottom = GuiMenuBottomBorderProfile;
	category = "defaultMenuBar";

	fontColor = $color3;
	fontColorHL = AdjustColorValue($color3, 10);
	fontColorSL = $color3;
	fontColorNA = SetColorAlpha($color3, 100);
});

SafeCreateNamedObject("GuiMenuContentVertBorderProfile", new GuiBorderProfile()
{
	border = 2;
	padding = 4;
	borderColor = $color4;
});

SafeCreateNamedObject("GuiMenuContentSideBorderProfile", new GuiBorderProfile()
{
	border = 2;
	padding = 0;
	borderColor = $color4;
});

SafeCreateNamedObject("GuiMenuContentProfile", new GuiControlProfile()
{
	fillColor = AdjustColorValue($color1, -5);

	borderDefault = GuiMenuContentSideBorderProfile;
	borderTop = GuiMenuContentVertBorderProfile;
	borderBottom = GuiMenuContentVertBorderProfile;
});

SafeCreateNamedObject("GuiMenuItemBorderTopProfile", new GuiBorderProfile()
{
	padding = 6;
	paddingHL = 6;
	paddingSL = 0;
	paddingNA = 6;

	marginSL = 4;
	borderSL = 1;
	borderColorSL = "0 0 0 50";
});

SafeCreateNamedObject("GuiMenuItemBorderBottomProfile", new GuiBorderProfile()
{
	padding = 6;
	paddingHL = 6;
	paddingSL = 0;
	paddingNA = 6;

	marginSL = 4;
	borderSL = 1;
	borderColorSL = "255 255 255 50";
});

SafeCreateNamedObject("GuiMenuItemBorderSideProfile", new GuiBorderProfile()
{
	padding = 6;
	paddingHL = 6;
	paddingSL = 0;
	paddingNA = 6;

	marginSL = 0;
	borderSL = 0;
	paddingSL = 6;
});

SafeCreateNamedObject("GuiMenuItemProfile", new GuiControlProfile()
{
	fillColor = AdjustColorValue($color1, -5);
	fillColorHL = AdjustColorValue($color4, -15);
	fillColorNA = $color1;
	align = left;

	fontColor = $color3;
	fontColorHL = AdjustColorValue($color3, 10);
	fontColorNA = SetColorAlpha($color3, 150);

	borderDefault = GuiMenuItemBorderSideProfile;
	borderTop = GuiMenuItemBorderTopProfile;
	borderBottom = GuiMenuItemBorderBottomProfile;
});
