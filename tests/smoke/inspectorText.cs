// Category picker and text block smoke test.
//
// Both halves of one bug report: a GuiControl dropped into a panel, given the
// text "High Scores", could not be made to look like a heading. Its category
// was guessed once when it was dropped -- no text yet, so Empty -- and nothing
// re-ran the guess or let it be corrected, so the Profile drop-down had one
// entry in it. Reaching for the font size instead did not work either, because
// the pane hid all nine text fields whenever the header carried the text box
// and the header carried three of them.
//
// So: the Category row exists where the class is ambiguous and nowhere else,
// picking one moves the control onto that category's profile, and every field
// GuiControl::renderText reads has a home.
// Run: tests/run.ps1 inspectorText  ; grep ITSMOKE in tests/logs/.

setLogMode(1);
$Scripts::ignoreDSOs = true;
setScriptExecEcho(false);
trace(false);

function tCheck(%label, %cond)
{
	if(%cond) echo("ITSMOKE PASS: " @ %label);
	else      echo("ITSMOKE FAIL: " @ %label);
}

function tPane()
{
	return GuiEditor.inspectorWindow.pane;
}

function tBind(%ctrl)
{
	%pane = tPane();
	%pane.bind(%ctrl);
	return %pane;
}

function tRowShown(%pane, %field)
{
	%row = %pane.row[%field];
	return isObject(%row) && %row.isVisible();
}

function tItemCount(%row)
{
	return %row.editor.getItemCount();
}

function tOffers(%row, %name)
{
	return %row.editor.findItemText(%name, false) >= 0;
}

// A profile field reads back as a NAME, and in editor mode that name was never
// registered with the Sim -- so comparing what the field holds against a
// profile id is comparing a string to a number and always answering no. The
// applier already knows how to resolve one; ask it.
function tProfileOf(%ctrl)
{
	return GuiEditor.themeApplier.fieldProfile(%ctrl, "Profile");
}

testExec("editor/main.cs");
schedule(2000, 0, "tStep1");

//-----------------------------------------------------------------------------
// The report, reproduced: a panel with a GuiControl inside it that wants to be
// a heading.
//-----------------------------------------------------------------------------

function tStep1()
{
	ProjectManager.setProjectFolder("inspectorTextSmokeProject");
	GuiEditor.open();

	$tTheme = GuiEditor.themeLibrary.createTheme("ITSmoke");
	tCheck("theme created", isObject($tTheme));
	GuiEditor.themeName = $tTheme.getName();

	// The panel is a child of the simulated canvas, so it is the Gui's root and
	// takes Panel. The heading is a child of it with no text yet, which is
	// exactly the state the guess reads as Empty.
	$tPanel = new GuiControl();
	GuiEditor.rootGui.add($tPanel);
	$tHeading = new GuiControl();
	$tPanel.add($tHeading);
	GuiEditor.themeApplier.applyToBranch($tPanel, $tTheme, true);

	echo("ITSMOKE: panel wears " @ tProfileOf($tPanel).getName() @
		", heading wears " @ tProfileOf($tHeading).getName() @
		" (category '" @ tProfileOf($tHeading).category @ "')");

	tCheck("root GuiControl took Panel",
		tProfileOf($tPanel) == $tTheme.getProfile("Panel"));
	tCheck("the child took Empty",
		tProfileOf($tHeading) == $tTheme.getProfile("Empty"));

	// Typing the caption is what the user did next. It changes nothing about the
	// profile -- the guess ran when the control was dropped.
	$tHeading.text = "High Scores";

	%pane = tBind($tHeading);
	%row = %pane.header.categoryRow;
	tCheck("the category row is on show", %row.isVisible());
	tCheck("and reads the category the control is on", %row.getValue() $= "Empty");
	tCheck("it offers all four", tOffers(%row, "Empty") && tOffers(%row, "Panel") &&
		tOffers(%row, "Label") && tOffers(%row, "Overlay"));

	// The bug: one entry, and no way to reach a Label profile.
	tCheck("the profile row offers only the Empty member",
		tItemCount(%pane.header.profileRow) == 1);

	schedule(200, 0, "tStep2");
}

//-----------------------------------------------------------------------------
// Correcting the guess.
//-----------------------------------------------------------------------------

function tStep2()
{
	%pane = tPane();
	%row = %pane.header.categoryRow;

	%row.applyValue("Label");
	%row.commit();

	tCheck("picking Label moved the control onto the Label profile",
		tProfileOf($tHeading) == $tTheme.getProfile("Label"));
	tCheck("the category row now reads Label", %row.getValue() $= "Label");
	tCheck("and the profile row offers the theme's Label member",
		tOffers(%pane.header.profileRow, $tTheme.getProfile("Label").getName()));

	// The choice is recorded by the profile the control wears, so it survives a
	// reselect with no state of its own.
	%pane = tBind($tPanel);
	%pane = tBind($tHeading);
	tCheck("the category survives a reselect",
		%pane.header.categoryRow.getValue() $= "Label");

	// And back, to prove it is not one-way.
	%row = %pane.header.categoryRow;
	%row.applyValue("Overlay");
	%row.commit();
	tCheck("and moves again to Overlay",
		tProfileOf($tHeading) == $tTheme.getProfile("Overlay"));

	%row.applyValue("Label");
	%row.commit();

	schedule(200, 0, "tStep3");
}

//-----------------------------------------------------------------------------
// Only the ambiguous class gets one. Everything else is pinned by its class,
// where a picker would be a way to make a check box look like a scrollbar.
//-----------------------------------------------------------------------------

function tStep3()
{
	$tButton = new GuiButtonCtrl();
	GuiEditor.rootGui.add($tButton);
	GuiEditor.themeApplier.applyToBranch($tButton, $tTheme, true);

	%pane = tBind($tButton);
	tCheck("a button has no category row", !%pane.header.categoryRow.isVisible());
	tCheck("and still offers its own profile",
		tOffers(%pane.header.profileRow, $tTheme.getProfile("Button").getName()));

	schedule(200, 0, "tStep4");
}

//-----------------------------------------------------------------------------
// The five fields that had nowhere to go. This is the half of the report that
// was never about categories at all.
//-----------------------------------------------------------------------------

function tStep4()
{
	%pane = tBind($tHeading);

	tCheck("font size is reachable", tRowShown(%pane, "fontSizeAdjust"));
	tCheck("font color is reachable", tRowShown(%pane, "fontColor"));
	tCheck("the text box is reachable", tRowShown(%pane, "text"));

	%block = %pane.activeTextBlock();
	tCheck("the block is the header's", %block == %pane.header.textBlock);
	tCheck("wrap is reachable", %block.wrapButton.isVisible());
	tCheck("extend is reachable", %block.extendButton.isVisible());
	tCheck("both alignments are reachable",
		%block.alignRow.isVisible() && %block.vAlignRow.isVisible());

	// The quarter turn, which a plain GuiControl's onRender honors. Its unturned
	// value leads the row, as "default" leads the alignments.
	%turn = %block.rotationRow;
	tCheck("rotation is reachable", %turn.isVisible());
	tCheck("rotation leads with none", %turn.choiceValue[0] $= "none" && %turn.getValue() $= "none");
	%turn.choiceButton[1].performClick();
	tCheck("choosing ccw reached the control", $tHeading.textRotation $= "ccw");
	%turn.choiceButton[2].performClick();
	tCheck("choosing cw reached the control", $tHeading.textRotation $= "cw");
	%turn.choiceButton[0].performClick();
	tCheck("and none turns it back", $tHeading.textRotation $= "none");

	// textID went back to Localization, which is the only place that builds it
	// now. It used to be built twice and hidden by the text filter with the rest.
	tCheck("text id is reachable", tRowShown(%pane, "textID"));

	// The size the user could not reach. A multiplier, so the value that matters
	// is a fractional one: the row used to round on the way out, which would
	// have made reaching the field no better than not reaching it.
	%row = %pane.row["fontSizeAdjust"];
	tCheck("the font size row takes decimals", %row.kind $= "decimal");

	%row.applyValue("1.5");
	%row.commit();
	tCheck("setting the font size reaches the control", $tHeading.fontSizeAdjust == 1.5);
	tCheck("and the row reads it back whole", %row.getValue() == 1.5);

	// The arrow keys step a multiplier by a tenth rather than by one.
	%row.editor.onUpArrow();
	tCheck("an arrow key steps it by a tenth", $tHeading.fontSizeAdjust == 1.6);

	// Only a box that wants the arrows to step a value may claim them.
	// GuiTextEditCtrl hands a script onUpArrow the key before its own caret
	// movement, and isMethod answers for the class -- so the spinner class on a
	// text box swallowed both arrows and left the caret unable to change line.
	tCheck("the spinner class is on the number box",
		%row.editor.class $= "EditorFieldRowInput");
	tCheck("and not on the text box", %pane.row["text"].editor.class $= "");
	tCheck("so the text box has no onUpArrow to claim the key",
		!%pane.row["text"].editor.isMethod("onUpArrow"));

	%row.applyValue("2");
	%row.commit();

	schedule(200, 0, "tStepTyping");
}

//-----------------------------------------------------------------------------
// Typing. The control fills in as you type, but the edit still reaches it as
// one change: the text box writes the control directly per keystroke and the
// pane writes it properly once, when the box loses focus.
//-----------------------------------------------------------------------------

function tStepTyping()
{
	%pane = tBind($tHeading);
	%block = %pane.activeTextBlock();
	%row = %pane.row["text"];

	$tTextBefore = $tHeading.text;
	tCheck("nothing is being typed yet", !%block.typing);

	// What a keystroke does: the box's buffer changes and the engine runs its
	// Command. Set the buffer and call the same handler the Command names.
	%row.editor.setText("High Scores!");
	%block.onTextTyped();

	tCheck("the control filled in as we typed", $tHeading.text $= "High Scores!");
	tCheck("the edit is open", %block.typing);
	tCheck("and it remembers what the control said before",
		%block.textBeforeEdit $= $tTextBefore);
	tCheck("and which control it belongs to", %block.typingTarget == $tHeading);

	// Another keystroke must not move the stash: the whole edit is one change.
	%row.editor.setText("High Scores!!");
	%block.onTextTyped();
	tCheck("a second keystroke keeps the original text stashed",
		%block.textBeforeEdit $= $tTextBefore);

	// Blur. The row commits, the pane puts back what the control held when the
	// edit began, and writes the new value once.
	%row.commit();
	tCheck("the commit closed the edit", !%block.typing);
	tCheck("and the control kept the typed text", $tHeading.text $= "High Scores!!");

	// A commit with nothing typed must not write anything.
	%row.commit();
	tCheck("committing again is a no-op", $tHeading.text $= "High Scores!!");

	$tHeading.text = "High Scores";
	%pane.refresh();

	schedule(200, 0, "tStep5");
}

//-----------------------------------------------------------------------------
// Wrap and extend. Extend does something in both wrap states -- guiControl.cc
// grows the width when wrap is off and the height when it is on -- so it must
// not be disabled with wrap off.
//-----------------------------------------------------------------------------

function tStep5()
{
	%pane = tPane();
	%block = %pane.activeTextBlock();

	tCheck("extend is live while wrap is off",
		!$tHeading.textWrap && %block.extendButton.isActive());

	%block.extendButton.performClick();
	tCheck("extend reached the control", $tHeading.textExtend);
	// The tip is two lines: what the switch is and how it is set, then what that
	// means. The second line is the one that has to follow the wrap state, since
	// extend grows a different axis depending on it.
	tCheck("its tooltip names the switch and its state",
		getRecord(%block.extendButton.Tooltip, 0) $= "Extend To Fit Text - On");
	tCheck("and says which way it grows",
		strstr(getRecord(%block.extendButton.Tooltip, 1), "wider") >= 0);

	%block.wrapButton.performClick();
	tCheck("wrap reached the control", $tHeading.textWrap);
	tCheck("and the tooltip changed with it",
		strstr(getRecord(%block.extendButton.Tooltip, 1), "taller") >= 0);

	// Off again, and loaded back the same way on a rebind.
	%block.wrapButton.performClick();
	%block.extendButton.performClick();
	tCheck("both cleared", !$tHeading.textWrap && !$tHeading.textExtend);

	%pane = tBind($tHeading);
	%block = %pane.activeTextBlock();
	tCheck("the toggles reload from the control",
		!%block.wrapButton.getValue() && !%block.extendButton.getValue());

	schedule(200, 0, "tStep6");
}

//-----------------------------------------------------------------------------
// Font color, which is two fields wearing one swatch.
//-----------------------------------------------------------------------------

function tStep6()
{
	%pane = tPane();
	%row = %pane.row["fontColor"];

	tCheck("nothing is overridden to begin with", !$tHeading.overrideFontColor);
	tCheck("so the swatch shows the profile's color",
		%row.getValue() $= tProfileOf($tHeading).fontColor);
	tCheck("and there is nothing to revert", !%row.resetButton.isVisible());

	%row.editor.setColorI("10 20 30 255");
	%row.commit();
	tCheck("picking a color wrote it", $tHeading.fontColor $= "10 20 30 255");
	tCheck("and turned the override on", $tHeading.overrideFontColor);
	tCheck("the revert appeared", %row.resetButton.isVisible());

	// The revert is the only way back to the profile's color.
	%pane.onFieldRowReset(%row);
	tCheck("revert turned the override off", !$tHeading.overrideFontColor);
	tCheck("the swatch fell back to the profile's color",
		%row.getValue() $= tProfileOf($tHeading).fontColor);
	tCheck("and the revert went away", !%row.resetButton.isVisible());

	schedule(200, 0, "tStep7");
}

//-----------------------------------------------------------------------------
// Where the block lives, class by class. There are two of it and only ever one
// on show.
//-----------------------------------------------------------------------------

function tStep7()
{
	$tDrop = new GuiDropDownCtrl();
	GuiEditor.rootGui.add($tDrop);
	$tTree = new GuiTreeViewCtrl();
	GuiEditor.rootGui.add($tTree);
	$tPage = new GuiPanelCtrl();
	GuiEditor.rootGui.add($tPage);
	$tGrid = new GuiGridCtrl();
	GuiEditor.rootGui.add($tGrid);
	$tSprite = new GuiSpriteCtrl();
	GuiEditor.rootGui.add($tSprite);

	%pane = tBind($tDrop);
	tCheck("a drop-down's placeholder is in the header",
		%pane.header.textBlock.isVisible() && !%pane.textPanel.isVisible());

	%pane = tBind($tPage);
	tCheck("a panel's header text is in the header",
		%pane.header.textBlock.isVisible() && !%pane.textPanel.isVisible());
	tCheck("but its dead layout fields are not offered",
		!tRowShown(%pane, "fontSizeAdjust") &&
		!%pane.header.textBlock.alignRow.isVisible());

	// A list draws its items with this control's font, so the layout half is
	// live even though the text field itself is never drawn.
	%pane = tBind($tTree);
	tCheck("a tree's item font is in the header",
		%pane.header.textBlock.isVisible() && !%pane.textPanel.isVisible());
	tCheck("with no string of its own to edit", !tRowShown(%pane, "text"));
	tCheck("but the font size it draws them at", tRowShown(%pane, "fontSizeAdjust"));

	%pane = tBind($tGrid);
	tCheck("a grid's text is in the section",
		!%pane.header.textBlock.isVisible() && %pane.textPanel.isVisible());

	%pane = tBind($tSprite);
	tCheck("a sprite gets no text block at all",
		!%pane.header.textBlock.isVisible() && !%pane.textPanel.isVisible());

	echo("ITSMOKE DONE");
	schedule(300, 0, "quit");
}
