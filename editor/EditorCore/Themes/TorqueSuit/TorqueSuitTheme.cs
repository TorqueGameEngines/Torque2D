function TorqueSuitTheme::init(%this)
{
	%this.name = "Torque Suit";
	//fonts and font sizes
	%this.fontCount = 3;
	%this.font[1] = "raleway";//Most common font
	%this.font[2] = "Audiowide";//Title fontType
	%this.font[3] = "vt323";//Code and console font
	%this.fontDirectory = expandPath("^EditorCore/Themes/TorqueSuit/Fonts");
	%this.fontSize = 22;

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
		fillColorSL = %this.adjustValue(%this.color2, -2);
		fillColorNA = %this.setAlpha(%this.color2, 80);

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color5;
		fontColorNA = %this.setAlpha(%this.color3, 100);
		align = Center;
		valign = Middle;

		borderDefault = %buttonBorder;
	};
}

function TorqueSuitTheme::makeIconButtonProfile(%this)
{
	%buttonBorder = new GuiBorderProfile()
	{
		padding = 2;
		paddingHL = 2;
		paddingSL = 2;
		paddingNA = 2;
	};

	%this.iconButtonProfile = new GuiControlProfile()
	{
		fillColor = %this.color2;
		fillColorHL = %this.adjustValue(%this.color2, 10);
		fillColorSL = %this.adjustValue(%this.color2, -2);
		fillColorNA = %this.setAlpha(%this.color2, 80);

		fontSize = %this.fontSize;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color5;
		fontColorNA = %this.setAlpha(%this.color3, 100);
		align = Center;
		valign = Middle;

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

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
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

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
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

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
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

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
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
	%labelBorder = new GuiBorderProfile()
	{
		padding = 2;
	};
	%this.labelProfile = new GuiControlProfile()
	{
		fillColor = "0 0 0 0";

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize - 2;
		fontColor = %this.color4;
		align = "left";
		vAlign = "top";

		borderDefault = %labelBorder;
	};

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

		fontType = %this.font[3];
		fontDirectory = %this.fontDirectory;
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

		fontType = %this.font[1];
		fontDirectory = %this.fontDirectory;
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
		fontType = %this.font[3];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize + 2;
		fontColor = %this.color4; //Normal text
		fontColorHL = %this.setAlpha(%this.color4, 140); //Warnings
		fontColorNA = %this.color5; //Errors
	};
}

function TorqueSuitTheme::makeMenuProfile(%this)
{
	%menuBarBorder = new GuiBorderProfile()
	{
		padding = 2;
	};

	%this.menuBarProfile = new GuiControlProfile()
	{
		fillColor = %this.adjustValue(%this.color1, -7);
		canKeyFocus = true;
		borderDefault = %menuBarBorder;
	};

	%menuBorder = new GuiBorderProfile()
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
		borderColorSL = %this.color5;
	};

	%menuBottomBorder = new GuiBorderProfile()
	{
		paddingSL = 2;

		margin = 2;
		marginHL = 0;
		marginSL = 0;
		marginNA = 2;

		border = 0;
		borderHL = 2;
		borderSL = 2;
		borderNA = 0;

		borderColorHL = "255 255 255 30";
		borderColorSL = %this.color5;
	};

	%menuSideBorder = new GuiBorderProfile()
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
		borderColorSL = %this.color5;
	};

	%this.menuProfile = new GuiControlProfile()
	{
		fillColor = "0 0 0 0";
		fillColorHL = "255 255 255 10";
		fillColorSL = %this.adjustValue(%this.color5, -15);
		fillColorNA = "0 0 0 0";

		borderDefault = %menuBorder;
		borderLeft = %menuSideBorder;
		borderRight = %menuSideBorder;
		borderBottom = %menuBottomBorder;

		fontType = %this.font[3];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;

		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.color1;
		fontColorNA = %this.adjustValue(%this.color4, 100);

		canKeyFocus = true;
	};

	%menuContentVertBorder = new GuiBorderProfile()
	{
		border = 2;
		padding = 4;
		borderColor = %this.color5;
	};

	%menuContentSideBorder = new GuiBorderProfile()
	{
		border = 2;
		padding = 0;
		borderColor = %this.color5;
	};

	%this.menuContentProfile = new GuiControlProfile()
	{
		fillColor = %this.adjustValue(%this.color1, -5);

		borderDefault = %menuContentSideBorder;
		borderTop = %menuContentVertBorder;
		borderBottom = %menuContentVertBorder;
	};

	%menuItemBorderTop = new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 4;
		borderSL = 1;
		borderColorSL = "0 0 0 50";
	};

	%menuItemBorderBottom = new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 4;
		borderSL = 1;
		borderColorSL = "255 255 255 50";
	};

	%menuItemBorderSide = new GuiBorderProfile()
	{
		padding = 6;
		paddingHL = 6;
		paddingSL = 0;
		paddingNA = 6;

		marginSL = 0;
		borderSL = 0;
		paddingSL = 6;
	};

	%this.menuItemProfile = new GuiControlProfile()
	{
		fillColor = %this.adjustValue(%this.color1, -5);
		fillColorHL = %this.adjustValue(%this.color5, -15);
		fillColorNA = %this.color1;
		align = left;

		tab = false;
		canKeyFocus = true;

		fontType = %this.font[3];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;

		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color1, 10);
		fontColorNA = %this.setAlpha(%this.color1, 150);

		borderDefault = %menuItemBorderSide;
		borderTop = %menuItemBorderTop;
		borderBottom = %menuItemBorderBottom;
	};
}

function TorqueSuitTheme::makeListBoxProfile(%this)
{
	%borderProfile = new GuiBorderProfile()
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

	%leftBorderProfile = new GuiBorderProfile()
	{
		margin = 1;
		marginHL = 1;
		marginSL = 1;
		marginNA = 1;

		padding = 8;
		paddingHL = 8;
		paddingSL = 8 - %this.borderSize;
		paddingNA = 8;

		borderSL = %this.borderSize;
		borderColorSL = %this.color5;
	};

	%this.listBoxProfile = new GuiControlProfile ()
	{
	    // fill color
	    fillColor = %this.adjustValue(%this.color1, 4);
	    fillColorHL = %this.adjustValue(%this.color1, 8);
	    fillColorSL = %this.color3;
	    fillColorNA = %this.setAlpha(%this.color1, 150);
		align = left;

		tab = false;
		canKeyFocus = true;

		fontType = %this.font[1];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, 10);
		fontColorSL = %this.adjustValue(%this.color1, 2);
		fontColorNA = %this.adjustValue(%this.color4, -30);

		borderDefault = %borderProfile;
		borderLeft = %leftBorderProfile;
	};
}
