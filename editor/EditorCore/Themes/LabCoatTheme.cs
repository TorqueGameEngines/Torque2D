function LabCoatTheme::init(%this)
{
	%this.name = "Lab Coat";

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

	%this.color1 = "255 255 255 255";
	%this.color2 = "203 217 222 255";
	%this.color3 = "142 157 161 255";
	%this.color4 = "33 33 46 255";
	%this.color5 = "16 162 235 255";

	%this.borderSize = 2;
}
