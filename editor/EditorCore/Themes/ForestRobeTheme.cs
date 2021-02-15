function ForestRobeTheme::init(%this)
{
	%this.name = "Forest Robe";

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

	%this.color1 = "43 53 66 255";
	%this.color2 = "48 88 84 255";
	%this.color3 = "85 138 132 255";
	%this.color4 = "240 207 196 255";
	%this.color5 = "216 114 107 255";

	%this.borderSize = 3;
}
