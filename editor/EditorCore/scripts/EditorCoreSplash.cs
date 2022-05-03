
function EditorCoreSplash::show(%this)
{
	%this.setVisible(true);
	%this.fadeTo("255 255 255 255", 1000, "easeInOut");
	%this.moveTo("0 0", 1000, "easeInOut");
	%this.schedule(2000, "hide");
}

function EditorCoreSplash::hide(%this)
{
	%this.fadeTo("255 255 255 0", 1000, "easeInOut");
	%this.moveTo("0 -20", 1000, "easeInOut");
	%this.schedule(1000, "setVisible", false);
}
