//-----------------------------------------------------------------------------
// GuiControl::sleep, when a child's onSleep moves a control into the parent
// that is putting its children to sleep.
//
// A child's sleep ends in its script onSleep, and that script may move
// controls about -- a popup that puts itself back where it belongs when the
// control it was showing for goes to sleep, say, when where it belongs is
// inside the screen being put to sleep. The parent is still awake while it
// walks its children (it runs its own onSleep after the walk), so the moved
// control is woken by addObject and has to be put to sleep with the rest.
//
// The walk used an iterator, and adding a child can reallocate the child list
// under it. Whether it does depends on how many children are there already, so
// this sweeps the count: one of the forty grows the list mid-walk, and the old
// walk read freed memory there -- an access violation, or worse, a carry-on.
//-----------------------------------------------------------------------------

setLogMode(2);
setScriptExecEcho(false);
trace(false);
$Scripts::ignoreDSOs = true;
setCompanyAndProduct("Torque Game Engines", "Torque2D");
ModuleDatabase.EchoInfo = false;
AssetDatabase.EchoInfo = false;

testExec("editor/main.cs");

function scCheck(%label, %condition)
{
	if(%condition) echo("SCSMOKE PASS: " @ %label);
	else           echo("SCSMOKE FAIL: " @ %label);
}

// Going to sleep, moves the traveller into home.
function SleepMover::onSleep(%this)
{
	%this.home.add(%this.traveller);
}

schedule(2500, 0, "scRun");

function scRun()
{
	%asleep = 0;
	%movedHome = 0;
	for(%n = 1; %n <= 40; %n++)
	{
		%root = new GuiControl() { Extent = "200 200"; };
		%home = new GuiControl() { Extent = "100 100"; };
		%root.add(%home);

		for(%i = 0; %i < %n; %i++)
		{
			%home.add(new GuiControl() { Extent = "10 10"; });
		}

		%mover = new GuiControl() { class = "SleepMover"; Extent = "10 10"; };
		%home.add(%mover);

		// A later child of root, so it is still awake when the mover sleeps.
		%traveller = new GuiControl() { Extent = "10 10"; };
		%root.add(%traveller);

		%mover.home = %home;
		%mover.traveller = %traveller;

		Canvas.pushDialog(%root);
		Canvas.popDialog(%root);

		if(!%traveller.isAwake())
			%asleep++;
		if(%traveller.getParent() == %home)
			%movedHome++;

		%root.delete();
	}

	scCheck("a control moved in mid-walk was put to sleep, all 40 times (" @ %asleep @ ")", %asleep == 40);
	scCheck("and it ended up where it was moved to, all 40 times (" @ %movedHome @ ")", %movedHome == 40);

	echo("SCSMOKE DONE");
	schedule(300, 0, "quit");
}
