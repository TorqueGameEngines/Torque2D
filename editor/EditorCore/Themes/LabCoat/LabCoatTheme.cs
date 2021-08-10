function LabCoatTheme::init(%this)
{
	%this.name = "Lab Coat";
	//fonts and font sizes
	%this.fontCount = 3;
	%this.font[1] = "roboto";//Most common font
	%this.font[2] = "zen dots";//Title fontType
	%this.font[3] = "share tech mono";//Code and console font
	%this.fontDirectory = expandPath("^EditorCore/Themes/LabCoat/Fonts");
	%this.fontSize = 20;

	%this.color1 = "255 255 255 255";
	%this.color2 = "203 217 222 255";
	%this.color3 = "142 157 161 255";
	%this.color4 = "33 33 46 255";
	%this.color5 = "16 162 235 255";

	%this.borderSize = 2;
}

function LabCoatTheme::makePanelProfile(%this)
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

		fontType = %this.font[2];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize - 1;
		fontColor = %this.color5;
		fontColorHL = %this.adjustValue(%this.color5, 10);
		fontColorSL = %this.adjustValue(%this.color5, 20);
		fontColorNA = %this.setAlpha(%this.color4, 100);
		align = left;
		vAlign = middle;

		imageAsset="EditorCore:labCoatBanner1";

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

		fontType = %this.font[1];
		fontDirectory = %this.fontDirectory;
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

function LabCoatTheme::makeListBoxProfile(%this)
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

	%this.listBoxProfile = new GuiControlProfile ()
	{
	    // fill color
	    fillColor = %this.adjustValue(%this.color1, -10);
	    fillColorHL = %this.adjustValue(%this.color1, -15);
	    fillColorSL = %this.color5;
	    fillColorNA = %this.setAlpha(%this.color1, 150);
		align = left;

		tab = false;
		canKeyFocus = true;

		fontType = %this.font[3];
		fontDirectory = %this.fontDirectory;
		fontSize = %this.fontSize;
		fontColor = %this.color4;
		fontColorHL = %this.adjustValue(%this.color4, -10);
		fontColorSL = %this.adjustValue(%this.color1, -2);
		fontColorNA = %this.adjustValue(%this.color4, 30);

		borderDefault = %borderProfile;
	};
}
