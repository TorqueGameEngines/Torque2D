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
