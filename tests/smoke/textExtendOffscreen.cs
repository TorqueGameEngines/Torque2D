//-----------------------------------------------------------------------------
// A wrapped, extending control has to know its height before it is drawn.
//
// textExtend was applied only inside renderText, and a control outside its
// parent's clip rect is never rendered -- renderChild skips it. So a column of
// wrapped labels in a scroller took their real heights only as they scrolled
// into view: the content was too short until every row had been seen once,
// the thumb shrank as you scrolled, and scrollToBottom stopped short because it
// measured against a height the rows it passed had since outgrown.
//
// Every label here carries the same text at the same width, so the first one,
// which is on screen from the start, says what every other one should be.
//-----------------------------------------------------------------------------

setLogMode(2);
setScriptExecEcho(false);
trace(false);
$Scripts::ignoreDSOs = true;
setCompanyAndProduct("Torque Game Engines", "Torque2D");
ModuleDatabase.EchoInfo = false;
AssetDatabase.EchoInfo = false;

testExec("editor/main.cs");

function teoCheck(%label, %condition)
{
	if(%condition) echo("TEOSMOKE PASS: " @ %label);
	else           echo("TEOSMOKE FAIL: " @ %label);
}

$teoText = "Calling ship: Morale +2, Life +3; Friendly ships Range 1: Morale +1, Life +2";
$teoRows = 10;
$teoAuthoredHeight = 18;

createPath(testRoot("shots/"));
schedule(2500, 0, "teoStep1");

function teoHeight(%ctrl)
{
	return getWord(%ctrl.getExtent(), 1);
}

function teoDump(%when)
{
	for(%i = 0; %i < $teoChain.getCount(); %i++)
	{
		%row = $teoChain.getObject(%i);
		echo("TEO " @ %when @ ": row " @ %i @ " at " @ %row.getPosition() @ " extent " @ %row.getExtent());
	}
	echo("TEO " @ %when @ ": chain extent " @ $teoChain.getExtent() @ ", scroll y " @ $teoScroll.getScrollPositionY());
}

// True when every row is as tall as the first, which is on screen from the
// start and so was always measured.
function teoAllRowsMatchTheFirst()
{
	%first = teoHeight($teoChain.getObject(0));
	for(%i = 1; %i < $teoChain.getCount(); %i++)
	{
		if(teoHeight($teoChain.getObject(%i)) != %first)
			return false;
	}
	return true;
}

function teoStep1()
{
	// The Gui Editor owns the profiles these wear.
	GuiEditor.open();

	%stage = new GuiControl()
	{
		Position = "0 0";
		Extent = "1024 768";
	};
	ThemeManager.setProfile(%stage, "overlayProfile");

	// A single line, on screen and outside the scroller, to say what one line
	// of this profile is. Without it a row that never wrapped at all passes
	// every check below, because nothing grew.
	$teoOneLine = new GuiControl()
	{
		Position = "500 100";
		Extent = "280" SPC $teoAuthoredHeight;
		textWrap = true;
		textExtend = true;
		vAlign = "top";
		Text = "Short";
	};
	ThemeManager.setProfile($teoOneLine, "labelProfile");
	%stage.add($teoOneLine);

	// A scroller fills its parent unless told otherwise.
	$teoScroll = new GuiScrollCtrl()
	{
		Position = "100 100";
		Extent = "300 200";
		HorizSizing = "right";
		VertSizing = "bottom";
		hScrollBar = "alwaysOff";
		vScrollBar = "dynamic";
	};
	ThemeManager.setProfile($teoScroll, "scrollProfile");
	ThemeManager.setProfile($teoScroll, "thumbProfile", ThumbProfile);
	ThemeManager.setProfile($teoScroll, "trackProfile", TrackProfile);
	ThemeManager.setProfile($teoScroll, "scrollArrowProfile", ArrowProfile);
	%stage.add($teoScroll);

	$teoChain = new GuiChainCtrl()
	{
		Position = "0 0";
		Extent = "280 18";
		HorizSizing = "fill";
		VertSizing = "bottom";
		IsVertical = true;
		ChildSpacing = 2;
	};
	ThemeManager.setProfile($teoChain, "emptyProfile");
	$teoScroll.add($teoChain);

	for(%i = 0; %i < $teoRows; %i++)
	{
		%row = new GuiControl()
		{
			Extent = "280" SPC $teoAuthoredHeight;
			HorizSizing = "fill";
			VertSizing = "bottom";
			textWrap = true;
			textExtend = true;
			vAlign = "top";
			Text = $teoText;
		};
		ThemeManager.setProfile(%row, "labelProfile");
		$teoChain.add(%row);
	}

	Canvas.pushDialog(%stage);
	schedule(1000, 0, "teoStep2");
}

function teoStep2()
{
	teoDump("on open");
	screenShot(testRoot("shots/textExtendOffscreen-open.png"), "PNG");

	%first = teoHeight($teoChain.getObject(0));
	%oneLine = teoHeight($teoOneLine);
	teoCheck("the row on screen wrapped (" @ %first @ " against one line of " @ %oneLine @ ")",
		%oneLine > $teoAuthoredHeight && %first > %oneLine);
	teoCheck("the rows below the fold wrapped before they were drawn",
		teoAllRowsMatchTheFirst());

	// Rows of %first, and ChildSpacing between each pair.
	%expected = ($teoRows * %first) + (($teoRows - 1) * 2);
	teoCheck("the chain is as tall as its rows (" @ teoHeight($teoChain) @ " >= " @ %expected @ ")",
		teoHeight($teoChain) >= %expected);

	$teoScroll.scrollToBottom();
	schedule(500, 0, "teoStep3");
}

function teoStep3()
{
	teoDump("after scrollToBottom");
	screenShot(testRoot("shots/textExtendOffscreen-bottom.png"), "PNG");

	$teoFirstBottom = $teoScroll.getScrollPositionY();

	// Asking again changes nothing if the first one got there.
	$teoScroll.scrollToBottom();
	schedule(500, 0, "teoStep4");
}

function teoStep4()
{
	%again = $teoScroll.getScrollPositionY();
	teoCheck("one scrollToBottom reaches the bottom (" @ $teoFirstBottom @ " then " @ %again @ ")",
		%again == $teoFirstBottom);

	%last = $teoChain.getObject($teoRows - 1);
	%lastBottom = getWord(%last.getPosition(), 1) + teoHeight(%last);
	teoCheck("the last row is in view (bottom " @ %lastBottom @ ", scrolled " @ %again @ ")",
		%lastBottom - %again <= getWord($teoScroll.getExtent(), 1));

	// The first row is off screen now. New text that fits on one line has to
	// shrink it without it being drawn.
	%row = $teoChain.getObject(0);
	$teoWrappedHeight = teoHeight(%row);
	%row.setText("Short");
	teoCheck("setText re-measures a row that is off screen (" @ $teoWrappedHeight @ " then " @ teoHeight(%row) @ ")",
		teoHeight(%row) < $teoWrappedHeight);

	// A row added to a list that is already open is measured as it arrives, so
	// a script can scroll to it without waiting a frame.
	%added = new GuiControl()
	{
		Extent = "280" SPC $teoAuthoredHeight;
		HorizSizing = "fill";
		VertSizing = "bottom";
		textWrap = true;
		textExtend = true;
		vAlign = "top";
		Text = $teoText;
	};
	ThemeManager.setProfile(%added, "labelProfile");
	$teoChain.add(%added);
	teoCheck("a row added to an open list is measured as it arrives (" @ teoHeight(%added) @ " against " @ $teoWrappedHeight @ ")",
		teoHeight(%added) == $teoWrappedHeight);

	$teoScroll.scrollToBottom();
	%addedBottom = getWord(%added.getPosition(), 1) + teoHeight(%added);
	%scrolled = $teoScroll.getScrollPositionY();
	teoCheck("and scrollToBottom reaches it in the same breath (bottom " @ %addedBottom @ ", scrolled " @ %scrolled @ ")",
		%addedBottom - %scrolled <= getWord($teoScroll.getExtent(), 1));

	echo("TEOSMOKE DONE");
	schedule(400, 0, "quit");
}
