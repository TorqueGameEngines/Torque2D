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

//-----------------------------------------------------------------------------

new GuiCursor(DefaultCursor)
{
    hotSpot = "3 3";
    renderOffset = "0 0";
    bitmapName = "^Sandbox/gui/images/defaultCursor";
};

if (!isObject(GuiDefaultBorderProfile)) new GuiBorderProfile (GuiDefaultBorderProfile)
{
	border = 0;
    borderColor   = $color1;
    borderColorHL = AdjustColorValue($color1, 10);
    borderColorSL = AdjustColorValue($color1, 10);
    borderColorNA = SetColorAlpha($color1, 100);
};

if (!isObject(GuiBrightBorderProfile)) new GuiBorderProfile (GuiBrightBorderProfile : GuiDefaultBorderProfile)
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
};

if (!isObject(GuiDarkBorderProfile)) new GuiBorderProfile (GuiDarkBorderProfile : GuiBrightBorderProfile)
{
	borderColor = "0 0 0 50";
	borderColorHL = "0 0 0 50";
	borderColorSL = "0 0 0 50";
	borderColorNA = "0 0 0 50";
};

if(!isObject(GuiDefaultProfile)) new GuiControlProfile (GuiDefaultProfile)
{
    // fill color
    fillColor = "0 0 0 0";

    // font
    fontType = $platformFontType;
    fontSize = $platformFontSize;
    fontColor = "255 255 255 255";
	align = center;
	vAlign = middle;

    // bitmap information
	cursorColor = "0 0 0 255";

	borderDefault = GuiDefaultBorderProfile;
};

if(!isObject(GuiTransparentProfile)) new GuiControlProfile (GuiTransparentProfile : GuiDefaultProfile);

if(!isObject(GuiSpriteProfile)) new GuiControlProfile (GuiSpriteProfile : GuiDefaultProfile)
{
	FillColor = "255 255 255 255";
};

// ----------------------------------------------------------------------------

if (!isObject(GuiSolidBorderProfile)) new GuiBorderProfile (GuiSolidBorderProfile : GuiDefaultBorderProfile)
{
	border = 1;

	padding = 10;
	paddingHL = 10;
	paddingSL = 10;
	paddingNA = 10;
};

if(!isObject(GuiSolidProfile)) new GuiControlProfile (GuiSolidProfile : GuiDefaultProfile)
{
	// fill color
    fillColor = $color1;
    fillColorHL = AdjustColorValue($color1, 10);
    fillColorSL = AdjustColorValue($color1, 15);
    fillColorNA = SetColorAlpha($color1, 100);

	borderDefault = GuiSolidBorderProfile;
};

// ----------------------------------------------------------------------------

if (!isObject(GuiToolTipProfile)) new GuiControlProfile (GuiToolTipProfile : GuiSolidProfile)
{
    fillColor = SetColorAlpha($color1, 220);
	fontColor = $color5;

	borderDefault = GuiBrightBorderProfile;
};

// ----------------------------------------------------------------------------

if (!isObject(GuiPopUpMenuDefault)) new GuiControlProfile (GuiPopUpMenuDefault : GuiDefaultProfile)
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
};

// ----------------------------------------------------------------------------

if (!isObject(GuiPopUpMenuProfile)) new GuiControlProfile (GuiPopUpMenuProfile : GuiPopUpMenuDefault)
{
    textOffset = "6 3";
    align = "center";
    bitmap = "^Sandbox/gui/images/dropDown";
    profileForChildren = GuiPopUpMenuDefault;
};

//-----------------------------------------------------------------------------

if (!isObject(GuiTextProfile)) new GuiControlProfile (GuiTextProfile : GuiDefaultProfile)
{
    fontColor = $color3;

    align = "left";
    autoSizeWidth = false;
    autoSizeHeight = false;
    returnTab = false;
    numbersOnly = false;
    cursorColor = "0 0 0 255";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiCheckBoxProfile)) new GuiControlProfile (GuiCheckBoxProfile : GuiDefaultProfile)
{

    fillColor = $color3;
    fillColorHL = AdjustColorValue($color3, -10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color3, 100);

    fontColor = $color3;
	fontColorHL = $color4;
	fontColorSL = $color5;
	fontColorNA = SetColorAlpha($color3, 100);
    align = "left";
	tab = true;

	borderDefault = "GuiBrightBorderProfile";
	borderRight = "GuiDarkBorderProfile";
	borderBottom = "GuiDarkBorderProfile";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiTextEditProfile)) new GuiControlProfile (GuiTextEditProfile : GuiDefaultProfile)
{
    fillColor = "232 240 248 255";
    fillColorHL = "251 170 0 255";
    fillColorNA = "127 127 127 52";
    bitmap = "^Sandbox/gui/images/textEdit.png";
    fontColor = "27 59 95 255";
    fontColorHL = "232 240 248 255";
    fontColorNA = "0 0 0 52";
    fontColorSL = "0 0 0 255";
    textOffset = "5 2";
    autoSizeWidth = false;
    autoSizeHeight = false;
    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

//-----------------------------------------------------------------------------

if(!isObject(GuiNumberEditProfile)) new GuiControlProfile (GuiNumberEditProfile: GuiTextEditProfile)
{
   numbersOnly = true;
};

//-----------------------------------------------------------------------------

if(!isObject(GuiScrollTrackProfile)) new GuiControlProfile (GuiScrollTrackProfile : GuiDefaultProfile)
{
	fillColor = $color1;
    fillColorHL = $color1;
    fillColorSL = $color1;
    fillColorNA = $color1;
};

if (!isObject(GuiScrollBrightBorderProfile)) new GuiBorderProfile (GuiScrollBrightBorderProfile : GuiBrightBorderProfile)
{
	padding = 3;
	paddingHL = 2;
	paddingSL = 2;
	paddingNA = 3;

	border = 1;
	borderHL = 2;
	borderSL = 2;
	borderNA = 1;
};

if (!isObject(GuiScrollDarkBorderProfile)) new GuiBorderProfile (GuiScrollDarkBorderProfile : GuiScrollBrightBorderProfile)
{
	borderColor = "0 0 0 20";
	borderColorHL = "0 0 0 20";
	borderColorSL = "0 0 0 20";
	borderColorNA = "0 0 0 20";
};

if(!isObject(GuiScrollThumbProfile)) new GuiControlProfile (GuiScrollThumbProfile : GuiDefaultProfile)
{
	fillColor = $color3;
    fillColorHL = AdjustColorValue($color3, 10);
    fillColorSL = $color4;
    fillColorNA = SetColorAlpha($color3, 100);

	borderDefault = GuiScrollBrightBorderProfile;
	borderRight = GuiScrollDarkBorderProfile;
	borderBottom = GuiScrollDarkBorderProfile;
};

if(!isObject(GuiScrollArrowProfile)) new GuiControlProfile (GuiScrollArrowProfile : GuiScrollThumbProfile)
{
	fontColor = "0 0 0 100";
    fontColorHL = "0 0 0 150";
    fontColorSL = $color5;
    fontColorNA = "0 0 0 50";

	borderDefault = GuiScrollBrightBorderProfile;
	borderRight = GuiScrollDarkBorderProfile;
	borderBottom = GuiScrollDarkBorderProfile;
};

if(!isObject(GuiScrollProfile)) new GuiControlProfile (GuiScrollProfile)
{
    fillColor = $color2;
    borderDefault = GuiDefaultBorderProfile;
};

//-----------------------------------------------------------------------------

if(!isObject(GuiTransparentScrollProfile)) new GuiControlProfile (GuiTransparentScrollProfile : GuiScrollProfile)
{
   fillColor = "255 255 255 0";
};

//-----------------------------------------------------------------------------

if(!isObject(GuiToolboxProfile)) new GuiControlProfile( GuiToolboxProfile : GuiScrollProfile )
{
    fillColor = "255 255 255 220";
};

//-----------------------------------------------------------------------------

if(!isObject(SandboxWindowProfile)) new GuiControlProfile (SandboxWindowProfile : GuiDefaultProfile)
{
    // fill color
    fillColor = "0 0 0 100";

    // font
    fontColor = "255 255 255 255";
	lockMouse = "0";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiButtonProfile)) new GuiControlProfile (GuiButtonProfile : GuiDefaultProfile)
{
    fontColor = "255 255 255 255";
    fontColorHL = $color5;
    bitmap = "^Sandbox/gui/images/smallButtonContainer";
};

//-----------------------------------------------------------------------------

if (!isObject(BlueButtonProfile)) new GuiControlProfile (BlueButtonProfile : GuiButtonProfile)
{
    fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/blueButton.png";
};

//-----------------------------------------------------------------------------

if (!isObject(RedButtonProfile)) new GuiControlProfile (RedButtonProfile : GuiButtonProfile)
{
    fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/redButton.png";
};

//-----------------------------------------------------------------------------

if (!isObject(GreenButtonProfile)) new GuiControlProfile (GreenButtonProfile : GuiButtonProfile)
{
	fontColor = "240 240 240 255";
    fontColorHL = "255 255 255 255";
    bitmap = "^Sandbox/gui/images/greenButton.png";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiRadioProfile)) new GuiControlProfile (GuiRadioProfile : GuiDefaultProfile)
{
    fillColor = "232 232 232 255";
    bitmap = "^Sandbox/gui/images/radioButton.png";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiSliderProfile)) new GuiControlProfile (GuiSliderProfile)
{
    bitmap = "^Sandbox/gui/images/slider.png";
    fontType = $platformFontType;
    fontSize = $platformFontSize;
    fontColor = "white";
};

//-----------------------------------------------------------------------------

if (!isObject(GuiSliderNoTextProfile)) new GuiControlProfile (GuiSliderNoTextProfile)
{
    bitmap = "^Sandbox/gui/images/slider.png";
    fontColor = "white";
    fontSize = 1;
};

//-----------------------------------------------------------------------------

if (!isObject(GuiSpinnerProfile)) new GuiControlProfile (GuiSpinnerProfile : GuiDefaultProfile)
{
    fillColor = $color3;
    fillColorHL = $color5;
	fillColorSL = $color3;
    fillColorNA = SetColorAlpha(%this.color3, 100);
    numbersOnly = true;

	fontSize = $platformFontSize + 2;
    fontColor = $color1;
    fontColorHL = $color4;
	fontColorSL = $color1;
    fontColorNA = SetColorAlpha($color1, 100);

	borderTop = "GuiDarkBorderProfile";
	borderBottom = "GuiBrightBorderProfile";

    tab = false;
    canKeyFocus = true;
    returnTab = true;
};

//-----------------------------------------------------------------------------

if (!isObject(GuiSunkenContainerProfile)) new GuiControlProfile (GuiSunkenContainerProfile : GuiSolidProfile)
{
    fillColor = SetColorAlpha($color1, 150);
};

if (!isObject(GuiHeaderProfile)) new GuiControlProfile (GuiHeaderProfile : GuiSolidProfile)
{
    fillColor = $color2;
	fontColor = $color3;
	fontSize = $platformFontSize + 2;
	align = "left";

	borderDefault = "GuiBrightBorderProfile";
	borderRight = "GuiDarkBorderProfile";
	borderBottom = "GuiDarkBorderProfile";
};

if (!isObject(GuiLabelProfile)) new GuiControlProfile (GuiLabelProfile : GuiDefaultProfile)
{
	fontColor = "255 255 255 255";
	fontSize = $platformFontSize;
	align = "left";
};
