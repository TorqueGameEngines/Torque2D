//-----------------------------------------------------------------------------
// A control's text turned a quarter turn (textRotation), in a real canvas.
//
// Where each line starts is unit tested (guiTextRotationTests.cc). What needs a
// font is here: textExtend sizing a turned control along the turned axis -- the
// length of its text runs UP the control, and its lines stack ACROSS it -- so a
// turned label is the same size as an unturned one, on its side.
//
// It also writes shots/textRotation.png, a grid of 80x96 cells with "Bonus" in
// each: rows none / ccw / cw, columns align left / center / right. Look at it to
// check the one thing no assertion can: which way 90 degrees turns on screen.
// ccw reads bottom to top with align="left" starting at the bottom-left corner;
// cw reads top to bottom starting at the top-right.
//-----------------------------------------------------------------------------

setLogMode(2);
setScriptExecEcho(false);
trace(false);
$Scripts::ignoreDSOs = true;
setCompanyAndProduct("Torque Game Engines", "Torque2D");
ModuleDatabase.EchoInfo = false;
AssetDatabase.EchoInfo = false;

testExec("editor/main.cs");

function trCheck(%label, %condition)
{
	if(%condition) echo("TRSMOKE PASS: " @ %label);
	else           echo("TRSMOKE FAIL: " @ %label);
}

// An extending label, so that textExtend's answer is what the test reads.
function trProbe(%stage, %position, %extent, %rotation, %wrap, %text)
{
	%probe = new GuiControl()
	{
		Position = %position;
		Extent = %extent;
		textRotation = %rotation;
		textWrap = %wrap;
		textExtend = true;
		Text = %text;
	};
	ThemeManager.setProfile(%probe, "labelProfile");
	%stage.add(%probe);
	return %probe;
}

createPath(testRoot("shots/"));
schedule(2500, 0, "trStep1");

function trStep1()
{
	// The Gui Editor owns the profiles these wear.
	GuiEditor.open();

	%stage = new GuiControl()
	{
		Position = "0 0";
		Extent = "1024 768";
	};
	ThemeManager.setProfile(%stage, "overlayProfile");

	// The grid to look at.
	%rotations = "none ccw cw";
	%aligns = "left center right";
	for(%row = 0; %row < 3; %row++)
	{
		for(%col = 0; %col < 3; %col++)
		{
			%cell = new GuiControl()
			{
				Position = (60 + %col * 110) SPC (60 + %row * 120);
				Extent = "80 96";
				Text = "Bonus";
				textRotation = getWord(%rotations, %row);
				align = getWord(%aligns, %col);
				vAlign = "top";
			};
			ThemeManager.setProfile(%cell, "panelProfile");
			%stage.add(%cell);
		}
	}

	// Turned and unturned twins: one line, then a wrapped block.
	$trLine = trProbe(%stage, "500 60", "10 20", "none", false, "Passenger");
	$trLineCcw = trProbe(%stage, "500 120", "20 10", "ccw", false, "Passenger");
	$trLineCw = trProbe(%stage, "540 120", "20 10", "cw", false, "Passenger");
	$trBlock = trProbe(%stage, "600 60", "60 20", "none", true, "Bonus crew aboard the ship");
	$trBlockCcw = trProbe(%stage, "600 200", "20 60", "ccw", true, "Bonus crew aboard the ship");

	Canvas.pushDialog(%stage, 50);
	schedule(800, 0, "trStep2");
}

function trStep2()
{
	%length = $trLine.extent.x;
	trCheck("an unturned label grows its width to its text (" @ $trLine.extent @ ")", %length > 10 && $trLine.extent.y == 20);

	trCheck("a ccw label grows its height to its text instead (" @ $trLineCcw.extent @ ")",
		$trLineCcw.extent.y == %length && $trLineCcw.extent.x == 20);
	trCheck("and so does a cw one (" @ $trLineCw.extent @ ")",
		$trLineCw.extent.y == %length && $trLineCw.extent.x == 20);

	%depth = $trBlock.extent.y;
	trCheck("an unturned wrapped label grows its height to its lines (" @ $trBlock.extent @ ")", %depth > 20 && $trBlock.extent.x == 60);
	trCheck("a turned wrapped label grows its width to them instead (" @ $trBlockCcw.extent @ ")",
		$trBlockCcw.extent.x == %depth && $trBlockCcw.extent.y == 60);

	screenShot(testRoot("shots/textRotation.png"), "PNG");
	echo("TRSMOKE: wrote shots/textRotation.png");

	echo("TRSMOKE DONE");
	schedule(300, 0, "quit");
}
