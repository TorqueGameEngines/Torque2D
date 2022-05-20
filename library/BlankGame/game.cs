
function BlankGame::create(%this)
{
	// Put a gui in the Canvas
	Canvas.setContent(TamlRead("./gui/defaultGui.gui.taml"));

	// Play Music
	Audio.PlayMusic("BlankGame:planetfall");
}

function BlankGame::destroy(%this)
{

}
