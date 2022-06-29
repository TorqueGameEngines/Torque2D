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

function EditorCore::SetProfileColors(%this)
{
	%this.color[1] = "43 43 43 255";
	%this.color[2] = "81 92 102 255";
	%this.color[3] = "224 224 224 255";
	%this.color[4] = "54 135 196 255";
	%this.color[5] = "245 210 50 255";
	%this.color[6] = "196 54 71 255";
}

function EditorCore::SetProfileFont(%this)
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

function EditorCore::AdjustColorValue(%this, %color, %percent)
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

function EditorCore::SetColorAlpha(%this, %color, %newAlpha)
{
	%red = getWord(%color, 0);
	%green = getWord(%color, 1);
	%blue = getWord(%color, 2);
	return %red SPC %green SPC %blue SPC mRound(mClamp(%newAlpha, 0, 255));
}

function EditorCore::SafeCreateNamedObject(%this, %name, %object)
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

function EditorCore::createGuiProfiles(%this)
{
	%this.SetProfileColors();
	%this.SetProfileFont();

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
		fontDirectory = "^EditorCore/gui/fonts";
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

	%listBorderProfile = new GuiBorderProfile()
	{
		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;

		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;
	};

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

		borderDefault = %listBorderProfile;
	});

	%windowBorderProfile = new GuiBorderProfile()
	{
		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 4;
	};

	%this.SafeCreateNamedObject("GuiWindowProfile", new GuiControlProfile()
	{
	   fillColor = %this.AdjustColorValue(%this.color1, 10);
	   fillColorHL = %this.AdjustColorValue(%this.color1, 12);
	   fillColorSL = %this.color4;
	   fillColorNA = %this.color1;
	   category = "defaultWindow";
	   align = "Left";

	   fontColorSL = %this.color5;

	   borderLeft = %windowBorderProfile;
	});

	%windowContentProfile = new GuiBorderProfile()
	{
		borderColor = %this.AdjustColorValue(%this.color1, 10);
		borderColorSL = %this.AdjustColorValue(%this.color4, -10);

		border = 3;
		borderSL = 3;
	};

	%this.SafeCreateNamedObject("GuiWindowContentProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -10);
		fillColorSL = %this.AdjustColorValue(%this.color1, -10);

		borderDefault = %windowContentProfile;
		borderTop = GuiDefaultBorderProfile;
	});

	%windowButtonBorderProfile = new GuiBorderProfile()
	{
		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;

		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;
	};

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

		borderDefault = %windowButtonBorderProfile;
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

		borderDefault = %windowButtonBorderProfile;
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

		borderDefault = %windowButtonBorderProfile;
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

	%this.SafeCreateNamedObject("GuiToolTipProfile", new GuiControlProfile()
	{
		// fill color
	    fillColor = %this.SetColorAlpha(%this.color1, 220);
	    fillColorHL = %this.AdjustColorValue(%this.color1, 10);
	    fillColorSL = %this.AdjustColorValue(%this.color1, 15);
	    fillColorNA = %this.SetColorAlpha(%this.color1, 100);

		fontColor = %this.color5;

		borderDefault = GuiBrightBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiScrollTrackProfile", new GuiControlProfile()
	{
		fillColor = %this.color1;
	    fillColorHL = %this.color1;
	    fillColorSL = %this.color1;
	    fillColorNA = %this.color1;
	});

	%this.SafeCreateNamedObject("GuiScrollThumbProfile", new GuiControlProfile()
	{
		fillColor = %this.color3;
	    fillColorHL = %this.AdjustColorValue(%this.color3, 10);
	    fillColorSL = %this.color4;
	    fillColorNA = %this.SetColorAlpha(%this.color3, 100);

		borderDefault = GuiBrightBorderProfile;
		borderRight = GuiDarkBorderProfile;
		borderBottom = GuiDarkBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiScrollArrowProfile", new GuiControlProfile()
	{
		fontColor = "0 0 0 100";
	    fontColorHL = "0 0 0 150";
	    fontColorSL = %this.color5;
	    fontColorNA = "0 0 0 50";

		borderDefault = GuiBrightBorderProfile;
		borderRight = GuiDarkBorderProfile;
		borderBottom = GuiDarkBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiScrollProfile", new GuiControlProfile()
	{
	    fillColor = %this.color2;
	    borderDefault = GuiDefaultBorderProfile;
	});

	%this.SafeCreateNamedObject("GuiProgressProfile", new GuiControlProfile()
	{
	   fillColor = %this.color1;
	   fillColorHL = %this.color4;
	   fillColorSL = %this.AdjustColorValue(%this.color4, 10);

	   fontColorHL = %this.color3;
	   fontColorSL = %this.color5;

	   borderDefault = GuiBrightBorderProfile;
	   borderBottom = GuiDarkBorderProfile;
	   borderRight = GuiDarkBorderProfile;
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

		borderDefault = GuiBrightBorderProfile;
		borderRight = GuiDarkBorderProfile;
		borderBottom = GuiDarkBorderProfile;
		category = "dropDown";
	});

	%this.SafeCreateNamedObject("GuiMenuBarProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -7);
		canKeyFocus = true;
		category = "defaultMenuBar";
	});

	%this.SafeCreateNamedObject("GuiMenuProfile", new GuiControlProfile()
	{
		fillColor = "0 0 0 0";
		fillColorHL = "255 255 255 10";
		fillColorSL = %this.AdjustColorValue(%this.color4, -15);
		fillColorNA = "0 0 0 0";

		borderDefault = GuiDarkBorderProfile;
		category = "defaultMenuBar";

		fontColor = %this.color3;
		fontColorHL = %this.AdjustColorValue(%this.color3, 10);
		fontColorSL = %this.color3;
		fontColorNA = %this.SetColorAlpha(%this.color3, 100);
	});

	%this.SafeCreateNamedObject("GuiMenuContentProfile", new GuiControlProfile()
	{
		fillColor = %this.AdjustColorValue(%this.color1, -5);

		borderDefault = GuiDefaultBorderProfile;
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

		borderDefault = GuiDefaultBorderProfile;
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
}
