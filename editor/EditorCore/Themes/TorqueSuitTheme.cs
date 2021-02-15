function TorqueSuitTheme::init(%this)
{
	%this.name = "Torque Suit";

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

	%this.color1 = "34 19 30 255";
	%this.color2 = "62 32 52 255";
	%this.color3 = "166 38 70 255";
	%this.color4 = "246 75 72 255";
	%this.color5 = "33 191 132 255";

	%this.borderSize = 4;
}

function TorqueSuitTheme::makeButtonProfile(%this)
{
	%buttonBorder = new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 6;
		paddingNA = 6;
	};

	%this.buttonProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.adjustValue(%this.color2, 10);
		fillColorSL = %this.adjustValue(%this.color2, -20);
		fillColorNA = %this.setAlpha(%this.color2, 80);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = "255 255 255 150";
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderDefault = %buttonBorder;
	};
}

function TorqueSuitTheme::makeTabProfile(%this)
{
	%tabBorderTop = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 1;
		marginSL = 0;
		marginNA = 2;

		border = 3;
		borderHL = 4;
		borderSL = 5;
		borderNA = 3;

		padding = 8;
		paddingHL = 8;
		paddingSL = 8;
		paddingNA = 8;

		borderColor = "255 255 255 20";
		borderColorHL = %this.adjustValue(%this.color5, -10);
		borderColorSL = %this.color5;
		borderColorNA = "255 255 255 20";
	};

	%tabBorderSide = new GuiBorderProfile()
	{
		margin = 2;
		marginHL = 1;
		marginSL = 0;
		marginNA = 2;

		padding = 10;
		paddingHL = 11;
		paddingSL = 12;
		paddingNA = 10;
	};

	%tabBorderBottom = new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 6;
		paddingNA = 6;
	};

	%this.tabProfileTop = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color4;
		fillColorNA = %this.color2;

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderTop;
		borderLeft = %tabBorderSide;
		borderRight = %tabBorderSide;
		borderBottom = %tabBorderBottom;
	};

	%this.tabProfileBottom = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color4;
		fillColorNA = %this.color2;

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderBottom;
		borderLeft = %tabBorderSide;
		borderRight = %tabBorderSide;
		borderBottom = %tabBorderTop;
	};

	%this.tabProfileLeft = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color4;
		fillColorNA = %this.color2;

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderSide;
		borderLeft = %tabBorderTop;
		borderRight = %tabBorderBottom;
		borderBottom = %tabBorderSide;
	};

	%this.tabProfileRight = new GuiControlProfile ()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color4;
		fillColorNA = %this.color2;

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = Center;

		borderTop = %tabBorderSide;
		borderLeft = %tabBorderBottom;
		borderRight = %tabBorderTop;
		borderBottom = %tabBorderSide;
	};

	%bookBorderBottom = new GuiBorderProfile()
	{
		margin = 0;
		border = 1;
		borderColor = "0 0 0 150";
		padding = 0;
	};

	%bookBorderDefault = new GuiBorderProfile()
	{
		padding = 6;
	};

	%bookBorderSide = new GuiBorderProfile()
	{
		padding = 5;
	};

	%this.tabBookProfileTop = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderLeft = %bookBorderSide;
		borderRight = %bookBorderSide;
		borderBottom = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileBottom = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderLeft = %bookBorderSide;
		borderRight = %bookBorderSide;
		borderTop = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileLeft = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderTop = %bookBorderSide;
		borderBottom = %bookBorderSide;
		borderRight = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabBookProfileRight = new GuiControlProfile ()
	{
		borderDefault = %bookBorderDefault;
		borderTop = %bookBorderSide;
		borderBottom = %bookBorderSide;
		borderLeft = %bookBorderBottom;
		fillcolor = %this.color2;
	};

	%this.tabPageProfile = new GuiControlProfile ()
	{
	    fillColor = %this.setAlpha(%this.color1, 150);
	};
}

function TorqueSuitTheme::makeTextEditProfile(%this)
{
	//border for text boxes never use the HL state.
	%textBorderV = new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;

		border = 1;
		borderHL = 1;
		borderSL = 1;
		borderNA = 1;

		borderColor = %this.color2;
		borderColorHL = %this.adjustValue(%this.color2, 10);
		borderColorSL = %this.color2;
		borderColorNA = %this.adjustValue(%this.color1, 10);
	};

	%textBorderLeft = new GuiBorderProfile()
	{
		padding = 10;
		paddingHL = 10 - (%this.borderSize - 1);
		paddingSL = 10 - (%this.borderSize - 1);
		paddingNA = 10;

		border = 1;
		borderHL = %this.borderSize;
		borderSL = %this.borderSize;
		borderNA = 1;

		borderColor = %this.color2;
		borderColorHL = %this.adjustValue(%this.color5, -10);
		borderColorSL = %this.color5;
		borderColorNA = %this.adjustValue(%this.color1, 10);
	};

	%textBorderRight = new GuiBorderProfile()
	{
		padding = 10;
		paddingHL = 10;
		paddingSL = 10;
		paddingNA = 10;

		border = 1;
		borderHL = 1;
		borderSL = 1;
		borderNA = 1;

		borderColor = %this.color2;
		borderColorHL = %this.adjustValue(%this.color2, 10);
		borderColorSL = %this.color2;
		borderColorNA = %this.adjustValue(%this.color1, 10);
	};

	%this.textEditProfile = new GuiControlProfile()
	{
		fillColor = %this.color1;
		fillColorHL = %this.adjustValue(%this.color1, 20);//used for selected text
		fillColorSL = %this.color1;
		fillColorNA = %this.setAlpha(%this.color1, 80);

		fontType = %this.font;
		fontSize = %this.fontSize + 2;
		fontColor = %this.setAlpha(%this.color4, 220);
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color4;
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = left;
		cursorColor = %this.color5;

		borderDefault = %textBorderV;
		borderRight = %textBorderRight;
		borderLeft = %textBorderLeft;

		tab = false;
		canKeyFocus = true;
		returnTab = true;
	};
}

function TorqueSuitTheme::makeScrollProfile(%this)
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
			fillColorHL = %this.setAlpha(%this.color2, 100);
			fillColorSL = %this.setAlpha(%this.adjustValue(%this.color2, 10), 150);
			fillColorNA = %this.setAlpha(%this.color2, 50);
			borderDefault = %trackBorder;
	};

	%thumbBorder = new GuiBorderProfile()
	{
		margin = 3;
		marginHL = 3;
		marginSL = 3;
		marginNA = 3;
	};

	%this.thumbProfile = new GuiControlProfile()
	{
		fillColor = %this.color3;
		fillColorHL = %this.adjustValue(%this.color3, 10);
		fillColorSL = %this.color5;
		fillColorNA = %this.setAlpha(%this.color3, 80);

		borderDefault = %thumbBorder;
	};

	%arrowBorder = new GuiBorderProfile()
	{
		padding = 3;
		paddingHL = 3;
		paddingSL = 3;
		paddingNA = 3;
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
		fontColorSL = %this.adjustValue(%this.color5, -20);
		fontColorNA = %this.setAlpha(%this.color2, 80);

		borderDefault = %arrowBorder;
	};

	%mainBorder = new GuiBorderProfile()
	{
		margin = 0;
		padding = 8;
		border = 0;
	};
	%this.scrollProfile = new GuiControlProfile()
	{
		opaque = true;
		fillColor = %this.setAlpha(%this.color1, 240);

		borderDefault = %mainBorder;
	};
}

function TorqueSuitTheme::makeConsoleProfile(%this)
{
	%this.consoleProfile = new GuiControlProfile()
	{
		fontType = %this.font;
		fontColor = %this.color4; //Normal text
		fontColorHL = %this.setAlpha(%this.color4, 140); //Warnings
		fontColorNA = %this.color5; //Errors
	};
}
