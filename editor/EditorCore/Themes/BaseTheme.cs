function BaseTheme::onAdd(%this)
{
	%this.init();

	%this.emptyProfile = new GuiControlProfile()
	{
		fillColor = "0 0 0 0";

		fontType = %this.font;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
		align = center;
		vAlign = middle;
	};

	%this.makeTipProfile();
	%this.makeGeneralProfiles();
	%this.makePanelProfile();
	%this.makeButtonProfile();
	%this.makeTabProfile();
	%this.makeTextEditProfile();
	%this.makeScrollProfile();
	%this.makeConsoleProfile();
}

function BaseTheme::init(%this)
{
	%this.name = "Construction Vest";
	//fonts and font sizes
	%this.font = "monaco";
	%this.fontSize = 12;
	if ($platform $= "windows")
	{
		%this.font = "lucida console";
	}
	else if ($platform $= "Android")
	{
		%this.font = "Droid";
		%this.fontSize = 14;
	}
	else if($platform $= "ios")
	{
		%this.fontSize = 18;
	}

	%this.color1 = "10 10 10 255";//Most commonly used for backgrounds
	%this.color2 = "70 70 70 255";//Normally used for things that stand off the background but are still background
	%this.color3 = "120 120 120 255";//Text set on background
	%this.color4 = "255 255 255 255";//Similar to color 3 but with more contrast
	%this.color5 = "255 133 0 255";//Flavor color that mostly is used on objects during interaction

	%this.borderSize = 3;
}

function BaseTheme::createProfile(%this, %profileName, %parentName, %settings)
{

}

function BaseTheme::makeTipProfile(%this)
{
	%tipBorder = new GuiBorderProfile()
	{
		padding = 4;
		border = 1;
		borderColor = %this.color1;
	};

	%this.tipProfile = new GuiControlProfile()
	{
		fillColor = %this.setAlpha(%this.color3, 220);

		fontType = %this.font;
		fontSize = %this.fontSize;
		fontColor = %this.color1;

		borderDefault = %tipBorder;
	};
}

function BaseTheme::makeGeneralProfiles(%this)
{
	//Useful as a background
	%this.overlayProfile = new GuiControlProfile()
	{
		fillColor = %this.setAlpha(%this.color1, 150);

		fontType = %this.font;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
	};

	%simpleBorder = new GuiBorderProfile()
	{
		padding = 0;
		border = %this.borderSize;
		borderColor = %this.color5;
	};
	//A simple, well-rounded profile for text or containers
	%this.simpleProfile = new GuiControlProfile()
	{
		fillColor = %this.color1;

		fontType = %this.font;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
		align = center;
		vAlign = middle;

		borderDefault = %simpleBorder;
	};

	%simpleBorder2 = new GuiBorderProfile()
	{
		padding = 10;
		border = %this.borderSize;
		borderColor = %this.color5;
	};
	//A simple, well-rounded profile for text or containers
	%this.simpleProfile2 = new GuiControlProfile()
	{
		fillColor = %this.color1;

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		align = center;
		vAlign = middle;

		borderDefault = %simpleBorder2;
	};

	%this.spriteProfile = new GuiControlProfile()
	{
		fillColor = "255 255 255 255";
		useInput = false;
	};
}

function BaseTheme::makePanelProfile(%this)
{
	%panelBorderH = new GuiBorderProfile()
	{
		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 10;
	};

	%panelBorderV = new GuiBorderProfile()
	{
		padding = 4;
		paddingHL = 4;
		paddingSL = 4;
		paddingNA = 4;

		border = 1;
		borderHL = 1;
		borderSL = 1;
		borderNA = 1;

		borderColor = %this.adjustValue(%this.color3, -50);
		borderColorHL = %this.adjustValue(%this.color3, -60);
		borderColorSL = %this.adjustValue(%this.color3, -60);
		borderColorNA = %this.adjustValue(%this.color3, -50);
	};

	%this.panelProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.adjustValue(%this.color2, 5);
		fillColorSL = %this.adjustValue(%this.color2, 5);
		fillColorNA = %this.setAlpha(%this.color2, 80);

		fontType = %this.font;
		fontSize = 14;
		fontColor = %this.color5;
		fontColorHL = %this.adjustValue(%this.color5, 10);
		fontColorSL = %this.adjustValue(%this.color5, 20);
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = left;
		vAlign = middle;

		borderDefault = %panelBorderH;
		borderTop = %panelBorderV;
		borderBottom = %panelBorderV;
	};

	//Scrolling panel - a less space demanding scroll bar.
	%this.scrollingPanelTrackProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.color2;
		fillColorSL = %this.color2;
		fillColorNA = %this.setAlpha(%this.color2, 50);
	};

	%thumbBorder = new GuiBorderProfile()
	{
		margin = 3;
		marginHL = 3;
		marginSL = 3;
		paddingNA = 3;
	};

	%this.scrollingPanelThumbProfile = new GuiControlProfile()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 0);

		borderRight = %thumbBorder;
		borderLeft = %thumbBorder;
	};

	%buttonBorder = new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;
	};

	%this.scrollingPanelArrowProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.color2;
		fillColorSL = %this.color2;
		fillColorNA = %this.setAlpha(%this.color2, 50);

		fontType = %this.font;
		fontColor = %this.color3;
		fontColorHL = %this.adjustValue(%this.color3, 10);
		fontColorSL = %this.color5;
		fontColorNA = %this.setAlpha(%this.color3, 80);

		borderDefault = %buttonBorder;
	};
	%this.scrollingPanelProfile = new GuiControlProfile()
	{
	    fillColor = %this.color1;
	};
}

function BaseTheme::makeButtonProfile(%this)
{
	%buttonBorderV = new GuiBorderProfile()
	{
		padding = %this.borderSize;
		paddingHL = 0;
		paddingSL = 0;
		paddingNA = %this.borderSize;

		border = 0;
		borderHL = %this.borderSize;
		borderSL = %this.borderSize;
		borderNA = 0;

		borderColor = "0 0 0 0";
		borderColorHL = %this.setAlpha(%this.adjustValue(%this.color2, 10), 170);
		borderColorSL = %this.setAlpha(%this.adjustValue(%this.color5, 10), 170);
		borderColorNA = "0 0 0 0";

		underfill = false;
	};

	%buttonBorderH = new GuiBorderProfile()
	{
		padding = %this.borderSize;
		paddingHL = 0;
		paddingSL = 0;
		paddingNA = %this.borderSize;

		border = 0;
		borderHL = %this.borderSize;
		borderSL = %this.borderSize;
		borderNA = 0;

		borderColor = "0 0 0 0";
		borderColorHL = %this.setAlpha(%this.adjustValue(%this.color2, 10), 170);
		borderColorSL = %this.setAlpha(%this.adjustValue(%this.color5, 10), 170);
		borderColorNA = "0 0 0 0";

		underfill = false;
	};

	%this.buttonProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.adjustValue(%this.color2, 10);
		fillColorSL = %this.color3;
		fillColorNA = %this.setAlpha(%this.color2, 80);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = "0 0 0 255";
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderDefault = %buttonBorderV;
		borderRight = %buttonBorderH;
		borderLeft = %buttonBorderH;
	};
}

function BaseTheme::makeTabProfile(%this)
{
	%tabBorderTop = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 1;
		marginSL = 0;
		marginNA = 2;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		padding = 5;
		paddingHL = 6;
		paddingSL = 7;
		paddingNA = 5;

		borderColor = "255 255 255 90";
		borderColorHL = "255 255 255 90";
		borderColorSL = "255 255 255 90";
		borderColorNA = "255 255 255 90";
	};

	%tabBorderTopAlt = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 1;
		marginSL = 0;
		marginNA = 2;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		padding = 5;
		paddingHL = 6;
		paddingSL = 7;
		paddingNA = 5;

		borderColor = "0 0 0 90";
		borderColorHL = "0 0 0 90";
		borderColorSL = "0 0 0 90";
		borderColorNA = "0 0 0 90";
	};

	%tabBorderLeft = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 2;
		marginSL = 2;
		marginNA = 2;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 10;

		borderColor = "255 255 255 90";
		borderColorHL = "255 255 255 90";
		borderColorSL = "255 255 255 90";
		borderColorNA = "255 255 255 90";
	};

	%tabBorderRight = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 2;
		marginSL = 2;
		marginNA = 2;

		border = 2;
		borderHL = 2;
		borderSL = 2;
		borderNA = 2;

		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 10;

		borderColor = "0 0 0 90";
		borderColorHL = "0 0 0 90";
		borderColorSL = "0 0 0 90";
		borderColorNA = "0 0 0 90";
	};

	%tabBorderBottom = new GuiBorderProfile()
	{
		padding = 5;
		paddingHL = 5;
		paddingSL = 5;
		paddingNA = 5;
	};

	%this.tabProfileTop = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 150);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderTop;
		borderLeft = %tabBorderLeft;
		borderRight = %tabBorderRight;
		borderBottom = %tabBorderBottom;
	};

	%this.tabProfileBottom = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 150);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderBottom;
		borderLeft = %tabBorderLeft;
		borderRight = %tabBorderRight;
		borderBottom = %tabBorderTopAlt;
	};

	%this.tabProfileLeft = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 150);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderLeft;
		borderLeft = %tabBorderTop;
		borderRight = %tabBorderBottom;
		borderBottom = %tabBorderRight;
	};

	%this.tabProfileRight = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 150);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderLeft;
		borderLeft = %tabBorderBottom;
		borderRight = %tabBorderTopAlt;
		borderBottom = %tabBorderRight;
	};

	%bookBorderBottom = new GuiBorderProfile()
	{
		margin = 0;
		border = %this.borderSize;
		borderColor = "0 0 0 150";
		padding = 0;
	};

	%bookBorderDefault = new GuiBorderProfile()
	{
		padding = 5;
	};

	%this.tabBookProfileTop = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderBottom = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileBottom = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderTop = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileLeft = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderRight = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileRight = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderLeft = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabPageProfile = new GuiControlProfile ()
	{
	    fillColor = %this.setAlpha(%this.color1, 150);
	};
}

function BaseTheme::makeTextEditProfile(%this)
{
	//border for text boxes never use the HL state.
	%textBorderV = new GuiBorderProfile()
	{
		padding = 2;
		paddingSL = 2;
		paddingNA = 2;

		border = %this.borderSize;
		borderSL = %this.borderSize;
		borderNA = %this.borderSize;

		borderColor = %this.color3;
		borderColorSL = %this.color5;
		borderColorNA = %this.setAlpha(%this.color3, 100);
	};

	%textBorderH = new GuiBorderProfile()
	{
		padding = 10;
		paddingSL = 10;
		paddingNA = 10;

		border = %this.borderSize;
		borderSL = %this.borderSize;
		borderNA = %this.borderSize;

		borderColor = %this.color3;
		borderColorSL = %this.color5;
		borderColorNA = %this.setAlpha(%this.color3, 100);
	};

	%this.textEditProfile = new GuiControlProfile()
	{
		fillColor = %this.color4;
		fillColorHL = %this.color5;//used for selected text
		fillColorSL = %this.color4;
		fillColorNA = %this.setAlpha(%this.color4, 80);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.setAlpha(%this.color1, 220);
		fontColorHL = %this.adjustValue(%this.color1, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color1, 100);
		align = left;
		cursorColor = %this.color1;

		borderDefault = %textBorderV;
		borderRight = %textBorderH;
		borderLeft = %textBorderH;

		tab = false;
		canKeyFocus = true;
		returnTab = true;
	};
}

function BaseTheme::makeScrollProfile(%this)
{
	%trackBorder = new GuiBorderProfile()
	{
		margin = 0;
		padding = 0;
		border = 0;
	};

	%this.trackProfile = new GuiControlProfile()
	{
			fillColor = %this.setAlpha(%this.color2, 150);
			fillColorHL = %this.setAlpha(%this.color2, 150);
			fillColorSL = %this.setAlpha(%this.adjustValue(%this.color2, 10), 150);
			fillColorNA = %this.setAlpha(%this.color2, 50);
			borderDefault = %trackBorder;
	};

	%buttonBorderLT = new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 2;
		paddingSL = 2;
		paddingNA = 3;

		border = 1;
		borderHL = 2;
		borderSL = 2;
		borderNA = 1;

		borderColor = %this.adjustValue(%this.color3, 20);
		borderColorHL = %this.adjustValue(%this.color3, 30);
		borderColorSL = %this.adjustValue(%this.color5, 20);
		borderColorNA = %this.setAlpha(%this.adjustValue(%this.color3, 20), 80);

		underfill = true;
	};

	%buttonBorderRB = new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 2;
		paddingSL = 2;
		paddingNA = 3;

		border = 1;
		borderHL = 2;
		borderSL = 2;
		borderNA = 1;

		borderColor = %this.adjustValue(%this.color3, -20);
		borderColorHL = %this.adjustValue(%this.color3, -10);
		borderColorSL = %this.adjustValue(%this.color5, -20);
		borderColorNA = %this.setAlpha(%this.adjustValue(%this.color3, -20), 80);

		underfill = true;
	};

	%this.thumbProfile = new GuiControlProfile()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 80);

		borderDefault = %buttonBorderLT;
		borderRight = %buttonBorderRB;
		borderBottom = %buttonBorderRB;
	};

	%this.scrollArrowProfile = new GuiControlProfile()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 80);

		fontType = %this.font;
		fontColor = %this.color2;
		fontColorHL = %this.color2;
		fontColorSL = %this.color4;
		fontColorNA = %this.setAlpha(%this.color2, 80);

		borderDefault = %buttonBorderLT;
		borderRight = %buttonBorderRB;
		borderBottom = %buttonBorderRB;
	};

	%mainBorder = new GuiBorderProfile()
	{
		margin = 5;
		padding = 5;
		border = 3;
		borderColor = %this.color5;
	};
	%this.scrollProfile = new GuiControlProfile()
	{
	    fillColor = %this.setAlpha(%this.color2, 180);

		borderDefault = %mainBorder;
	};
}

function BaseTheme::makeConsoleProfile(%this)
{
	%this.consoleProfile = new GuiControlProfile()
	{
		fontType = %this.font;
		fontColor = %this.color4; //Normal text
		fontColorHL = %this.setAlpha(%this.color4, 140); //Warnings
		fontColorNA = "255 0 0 255"; //Errors
	};
}

//Positive values are brighter, negative are darker
function BaseTheme::adjustValue(%this, %color, %percent)
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

function BaseTheme::setAlpha(%this, %color, %newAlpha)
{
	%red = getWord(%color, 0);
	%green = getWord(%color, 1);
	%blue = getWord(%color, 2);
	return %red SPC %green SPC %blue SPC mRound(mClamp(%newAlpha, 0, 255));
}
