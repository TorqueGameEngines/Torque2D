function ForestRobeTheme::init(%this)
{
	%this.name = "Forest Robe";
	//fonts and font sizes
	%this.fontCount = 3;
	%this.font[1] = "raleway";//Most common font
	%this.font[2] = "cinzel decorative bold";//Title fontType
	%this.font[3] = "fira code semibold";//Code and console font
	%this.fontDirectory = expandPath("^EditorCore/Themes/ForestRobe/Fonts");
	%this.fontSize = 20;

	%this.color1 = "43 53 66 255";
	%this.color2 = "48 88 84 255";
	%this.color3 = "85 138 132 255";
	%this.color4 = "240 207 196 255";
	%this.color5 = "216 114 107 255";

	%this.borderSize = 3;
}
