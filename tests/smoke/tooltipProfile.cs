// Tooltip-profile smoke test. Covers what GuiControl::renderTooltip does for a
// control that was never given a tooltip profile:
//
//   1. it picks the Tooltip member of the theme its own profile belongs to;
//   2. it re-picks when the control is re-themed, rather than keeping the first
//      theme's tip forever;
//   3. it takes a reference when it picks, so the control can sleep again --
//      without that, closing a color popup after hovering a value box asserted
//      "GuiControlProfile::GuiToolTipProfile::decRefCount: zero ref count".
//
// A tooltip profile is only chosen when a tip actually draws, so this needs a
// real hover: drive it with tooltipProfile.input.ps1, which posts mouse
// moves and holds them past the 1000ms hover time. Run alone and nothing hovers,
// so every check fails -- that is the driver's job, not a bug.

setLogMode(1);
$Scripts::ignoreDSOs = true;
setScriptExecEcho(false);
trace(false);

function tCheck(%label, %cond)
{
	if(%cond) echo("TIPSMOKE PASS: " @ %label);
	else      echo("TIPSMOKE FAIL: " @ %label);
}

testExec("editor/main.cs");
schedule(2500, 0, "tStep1");

//-----------------------------------------------------------------------------
// A themed control, hovered at (470,420) by the driver.
//-----------------------------------------------------------------------------

function tStep1()
{
	$tThemeA = new GuiProfileTheme(TipThemeA);
	$tThemeB = new GuiProfileTheme(TipThemeB);

	// Pushed as a dialog rather than added to the content, so the editor's own
	// windows cannot sit between it and the cursor -- the hover has to land on
	// this control for a tip to draw at all.
	$tCtrl = new GuiControl()
	{
		Position = "400 400";
		Extent = "140 40";
		Text = "hover me";
		Tooltip = "A tip needs a profile";
		Active = true;
	};
	$tCtrl.setProfile($tThemeA.getProfile("Button"));

	Canvas.pushDialog($tCtrl, 50);

	tCheck("no tooltip profile before a tip is drawn",
		$tCtrl.tooltipProfile $= "" && $tCtrl.getActiveTooltipProfile() $= "");
	echo("TIPSMOKE: hover (470,420) now");

	schedule(4100, 0, "tStep2");
}

function tStep2()
{
	%wanted = $tThemeA.getProfile("Tooltip").getName();
	tCheck("themed control took its own theme's tooltip profile (" @ $tCtrl.getActiveTooltipProfile() @ ")",
		$tCtrl.getActiveTooltipProfile() $= %wanted);

	// The pick is the control's to revisit, not a profile it was given, so the
	// field does not report it -- a copy or a save would pin it to this theme.
	tCheck("and the tooltipProfile field still says it has none of its own",
		$tCtrl.tooltipProfile $= "");

	// setProfile goes through setControlProfile, which keeps the reference counts
	// straight on a control that is already awake.
	$tCtrl.setProfile($tThemeB.getProfile("Button"));
	echo("TIPSMOKE: re-themed, keep hovering");

	schedule(3000, 0, "tStep3");
}

function tStep3()
{
	%wanted = $tThemeB.getProfile("Tooltip").getName();
	tCheck("re-theming re-picked the tooltip profile (" @ $tCtrl.getActiveTooltipProfile() @ ")",
		$tCtrl.getActiveTooltipProfile() $= %wanted);
	tCheck("the two themes really do have different tooltip profiles",
		$tThemeA.getProfile("Tooltip") != $tThemeB.getProfile("Tooltip"));

	// A profile someone set deliberately is not ours to re-pick.
	$tCtrl.tooltipProfile = $tThemeA.getProfile("Tooltip");
	$tCtrl.setProfile($tThemeB.getProfile("Panel"));
	echo("TIPSMOKE: explicit tooltip profile set, keep hovering");

	schedule(3000, 0, "tStep4");
}

function tStep4()
{
	tCheck("a deliberate tooltip profile survives a re-theme (" @ $tCtrl.tooltipProfile @ ")",
		$tCtrl.tooltipProfile $= $tThemeA.getProfile("Tooltip").getName());
	tCheck("and it is the one tips are drawn with",
		$tCtrl.getActiveTooltipProfile() $= $tThemeA.getProfile("Tooltip").getName());

	//-------------------------------------------------------------------------
	// The popup hands its tooltip profile to the value boxes, and survives the
	// sleep that closing it causes.
	//-------------------------------------------------------------------------

	// Deliberately garish, because the only way to tell the boxes took it rather
	// than resolving one for themselves is to look at the tip they draw.
	$tTipProfile = new GuiControlProfile(TipLoudProfile)
	{
		fillColor = "255 0 255 255";
		fontColor = "255 255 0 255";
		fontSize = 18;
	};

	$tPopup = new GuiColorPopupCtrl()
	{
		Position = "20 20";
		Extent = "40 40";
		showColorValues = true;
	};
	// Set while the popup is still asleep, so no reference book-keeping is owed.
	$tPopup.tooltipProfile = $tTipProfile;

	%content = Canvas.getContent();
	%content.add($tPopup);
	$tPopup.openColorPopup();

	echo("TIPSMOKE: hover (45,280) now");
	schedule(4100, 0, "tStep5");
}

function tStep5()
{
	createPath(testRoot("shots/"));
	screenShot(testRoot("shots/colorPopup_valueTip.png"), "PNG");
	echo("TIPSMOKE: wrote shots/colorPopup_valueTip.png - the tip over the R box");
	echo("TIPSMOKE:   should be magenta with yellow text if the popup's profile");
	echo("TIPSMOKE:   reached the box");

	schedule(400, 0, "tStep6");
}

function tStep6()
{
	// Before the fix this asserted inside popDialogControl and never returned.
	$tPopup.closeColorPopup();
	tCheck("closing the popup after hovering a value box did not assert", true);

	echo("TIPSMOKE DONE");
	schedule(300, 0, "quit");
}
