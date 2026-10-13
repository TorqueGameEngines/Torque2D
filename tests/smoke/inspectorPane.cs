// Properties-pane smoke test. Drives GuiEditorInspectorPane -- the custom pane
// that replaced the native GuiInspector in the Gui Editor -- through a control
// of each family, checking that what it shows matches what the class can
// actually use, that a commit reaches the control, and that reparenting into a
// layout container makes the geometry it no longer owns go inert.
// Run: tests/run.ps1 inspectorPane  ; grep IPSMOKE in tests/logs/.

setLogMode(1);
$Scripts::ignoreDSOs = true;
setScriptExecEcho(false);
trace(false);

function pCheck(%label, %cond)
{
	if(%cond) echo("IPSMOKE PASS: " @ %label);
	else      echo("IPSMOKE FAIL: " @ %label);
}

// Drop a control into the Gui being edited and select it, which is the path a
// real selection takes -- the brain themes it on arrival and posts Inspect.
function pAdd(%class)
{
	%ctrl = eval("return new " @ %class @ "();");
	GuiEditor.rootGui.add(%ctrl);

	%theme = GuiEditor.themeByName(GuiEditor.themeName);
	if(isObject(%theme))
	{
		GuiEditor.themeApplier.applyToBranch(%ctrl, %theme, false);
	}
	return %ctrl;
}

function pBind(%ctrl)
{
	GuiEditor.inspectorWindow.pane.bind(%ctrl);
	return GuiEditor.inspectorWindow.pane;
}

// Is a row on show? A row that was never built is not visible either, and
// isVisible() on nothing is false -- so ask both, or a missing row would read
// as a deliberate hide (which is exactly how a stale check once passed for
// years; see tests/README.md).
function pRowShown(%pane, %field)
{
	%row = %pane.row[%field];
	return isObject(%row) && %row.isVisible();
}

function pRowBuilt(%pane, %field)
{
	return isObject(%pane.row[%field]);
}

// Does the control really carry this dynamic field? Comparing the value against
// "" proves nothing: an absent field and an empty one read back identically,
// because an empty one is exactly what the engine deletes.
function pHasDynamicField(%ctrl, %name)
{
	for(%i = 0; %i < %ctrl.getDynamicFieldCount(); %i++)
	{
		if(getWord(%ctrl.getDynamicField(%i), 0) $= %name)
		{
			return true;
		}
	}
	return false;
}

testExec("editor/main.cs");
schedule(2000, 0, "pStep1");

//-----------------------------------------------------------------------------
// The pane exists and replaced the inspector.
//-----------------------------------------------------------------------------

function pStep1()
{
	ProjectManager.setProjectFolder("inspectorPaneSmokeProject");
	GuiEditor.open();

	%w = GuiEditor.inspectorWindow;
	pCheck("inspector window exists", isObject(%w));
	pCheck("pane built", isObject(%w.pane));
	pCheck("native inspector gone", !isObject(%w.inspector));
	pCheck("pane owns a spec", isObject(%w.pane.spec));
	pCheck("header built", isObject(%w.pane.header));

	schedule(200, 0, "pStep2");
}

//-----------------------------------------------------------------------------
// A button: text in the header, easing shown, no class sections beyond that.
//-----------------------------------------------------------------------------

function pStep2()
{
	$pButton = pAdd("GuiButtonCtrl");
	%pane = pBind($pButton);

	pCheck("bound to the button", %pane.target == $pButton);
	pCheck("pane visible once bound", %pane.isVisible());
	pCheck("button text is in the header", %pane.header.textGrid.isVisible());

	// --- Alignment is a segmented row per axis, "default" first and blank. ---
	// "default" is not an absence: getAlignmentType resolves it to the profile's
	// own alignment, and the engine's tables only started exposing it with this
	// change (the count said 3 while the array held 4).
	%align = %pane.header.alignRow;
	pCheck("align row has four choices", %align.choiceCount == 4);
	pCheck("align row leads with default", %align.choiceValue[0] $= "default");
	pCheck("the default choice has no icon", %align.choiceIcon[0] $= "");
	pCheck("the default button draws no icon", !%align.choiceButton[0].icon.isVisible());
	pCheck("the other three do", %align.choiceButton[1].icon.isVisible());
	pCheck("v-align row has four choices", %pane.header.vAlignRow.choiceCount == 4);
	pCheck("a button draws its own caption, so it has no rotation row",
		!%pane.header.textBlock.rotationRow.isVisible());

	$pButton.align = "default";
	%pane.refresh();
	pCheck("default reads back by name now the table exposes it",
		$pButton.align $= "default");
	pCheck("row shows default chosen", %align.getValue() $= "default");
	pCheck("only the default button is down",
		%align.choiceButton[0].getValue() && !%align.choiceButton[2].getValue());

	// Picking one writes it and moves the pressed button.
	%align.choiceButton[2].performClick();
	pCheck("choosing centre reached the control", $pButton.align $= "center");
	pCheck("the chosen button is down", %align.choiceButton[2].getValue());
	pCheck("and the previous one is up", !%align.choiceButton[0].getValue());

	// A radio cannot be un-picked, only replaced.
	%align.choiceButton[2].performClick();
	pCheck("clicking the chosen one again keeps it", $pButton.align $= "center");
	pCheck("and leaves it looking chosen", %align.choiceButton[2].getValue());

	%align.choiceButton[0].performClick();
	pCheck("going back to default writes default", $pButton.align $= "default");
	pCheck("button shows easing", pRowShown(%pane, "easeFillColorHL"));
	pCheck("button shows tooltip", pRowShown(%pane, "tooltip"));
	pCheck("button can be a container", %pane.header.containerButton.isVisible());
	// One text block, in one place. The row the pane filters and loads IS the
	// header block's -- there is no second copy of it to disagree with.
	pCheck("the text row on show is the header block's",
		pRowShown(%pane, "text") && %pane.row["text"] == %pane.header.textBlock.row["text"]);
	pCheck("the text section is not also showing", !%pane.textPanel.isVisible());

	// The header loaded the control's actual values.
	pCheck("name row loaded", %pane.header.nameRow.getValue() $= $pButton.getName());
	pCheck("extent row loaded", %pane.header.extentRow.getValue() $= $pButton.getExtent());

	// --- A commit reaches the control. ---
	// applyValue, not setValue: setValue also records the value as the row's
	// baseline, which is what tells a later commit that nothing was edited. A
	// user typing into the box changes the widget without touching the
	// baseline, and applyValue is the half that does that.
	%pane.header.textRow.applyValue("Smoke");
	%pane.header.textRow.commit();
	pCheck("text commit reached the control", $pButton.getText() $= "Smoke");

	%pane.header.extentRow.applyValue("123 45");
	%pane.header.extentRow.commit();
	pCheck("extent commit reached the control", $pButton.getExtent() $= "123 45");

	// A row that lost focus without being edited must not write.
	$pButton.setText("Untouched");
	%pane.refresh();
	%pane.header.textRow.commit();
	pCheck("unchanged row does not write", $pButton.getText() $= "Untouched");

	schedule(200, 0, "pStep3");
}

//-----------------------------------------------------------------------------
// A chain: no text at all, and its own two fields promoted to the header.
//-----------------------------------------------------------------------------

function pStep3()
{
	$pChain = pAdd("GuiChainCtrl");
	%pane = pBind($pChain);

	pCheck("chain hides the header text block", !%pane.header.textGrid.isVisible());
	pCheck("chain hides the shared text row", !pRowShown(%pane, "text"));
	// A chain draws no text at all, so the block is in neither of its homes --
	// checked at both ends, because "no row" and "a hidden row" read the same
	// through pRowShown and only one of them is the answer here.
	pCheck("chain hides the text section", !%pane.textPanel.isVisible());
	pCheck("chain has no text row at all", !pRowBuilt(%pane, "text"));
	pCheck("chain hides easing", !pRowShown(%pane, "easeFillColorHL"));
	pCheck("chain promotes IsVertical", pRowBuilt(%pane, "IsVertical"));
	pCheck("chain promotes ChildSpacing", pRowBuilt(%pane, "ChildSpacing"));
	pCheck("chain can be a container", %pane.header.containerButton.isVisible());

	// A grid draws text through the base onRender, so it keeps the fields --
	// but in the collapsed section rather than the header.
	$pGrid = pAdd("GuiGridCtrl");
	%pane = pBind($pGrid);
	pCheck("grid hides the header text block", !%pane.header.textGrid.isVisible());
	pCheck("grid keeps the shared text row", pRowShown(%pane, "text"));
	pCheck("grid text section shown", %pane.textPanel.isVisible());
	pCheck("and it is the section block's row",
		%pane.row["text"] == %pane.sectionText.row["text"]);
	pCheck("grid promotes CellModeX", pRowBuilt(%pane, "CellModeX"));
	pCheck("grid has its Grid section", pRowBuilt(%pane, "MaxColCount"));

	schedule(200, 0, "pStep4");
}

//-----------------------------------------------------------------------------
// Classes whose sections have to be rebuilt as the selection moves between
// them -- the one part of the pane that is not filtered but replaced.
//-----------------------------------------------------------------------------

function pStep4()
{
	$pWindow = pAdd("GuiWindowCtrl");
	%pane = pBind($pWindow);
	// The six switches are icons in the header's value block now, sharing a line
	// with Title Height rather than costing a section of six checkboxes.
	pCheck("window has no Window section", !pRowBuilt(%pane, "canClose"));
	pCheck("its switches are in the header",
		isObject(%pane.header.windowToggleRow) &&
		isObject(%pane.header.windowButton["canClose"]));
	pCheck("window has its Grips section", pRowBuilt(%pane, "resizeRightWidth"));
	pCheck("window keeps its title text", %pane.header.textGrid.isVisible());
	pCheck("window promotes titleHeight", pRowBuilt(%pane, "titleHeight"));

	// Moving to a slider must take the window's fields away again.
	$pSlider = pAdd("GuiSliderCtrl");
	%pane = pBind($pSlider);
	pCheck("window fields gone after reselect", !pRowBuilt(%pane, "resizeRightWidth"));
	pCheck("and its switch row went with them", !isObject(%pane.header.windowToggleRow));
	pCheck("slider has its ticks", pRowBuilt(%pane, "ticks"));
	pCheck("slider promotes range", pRowBuilt(%pane, "range"));
	pCheck("slider hides the header text block", !%pane.header.textGrid.isVisible());
	pCheck("slider keeps fontSizeAdjust", pRowShown(%pane, "fontSizeAdjust"));
	pCheck("slider cannot be a container", !%pane.header.containerButton.isVisible());

	// And back again, to prove the rebuild is not one-way.
	%pane = pBind($pWindow);
	pCheck("window fields returned", pRowBuilt(%pane, "resizeRightWidth"));
	pCheck("and so did its switch row", isObject(%pane.header.windowToggleRow));
	pCheck("slider fields gone", !pRowBuilt(%pane, "ticks"));

	schedule(200, 0, "pStep5");
}

//-----------------------------------------------------------------------------
// Geometry, which belongs to the parent. This is the check that would have
// caught the fields the old inspector let you edit into a silent revert.
//-----------------------------------------------------------------------------

function pStep5()
{
	%pane = pBind($pButton);
	%anchor = %pane.header.anchorPicker;
	pCheck("free control edits its position",
		$pButton.getParent() == GuiEditor.rootGui &&
		%pane.header.positionRow.editor.isActive());
	pCheck("free control edits its extent", %pane.header.extentRow.editor.isActive());
	pCheck("free control edits horizontal sizing", %anchor.leftPin.isActive());

	// --- The anchor picker and the sizing names it speaks. ---
	// The deprecated set still loads: "right" is the old name for anchorLeft,
	// and it has to keep working because every Gui already on disk uses it.
	$pButton.HorizSizing = "right";
	$pButton.VertSizing = "height";
	pCheck("a deprecated name still sets the field",
		$pButton.HorizSizing $= "anchorLeft");
	pCheck("and reads back as the anchor name, which is what TAML will write",
		$pButton.HorizSizing !$= "right");

	%pane.refresh();
	pCheck("anchorLeft reads back as a left-edge pin",
		%anchor.pinLeft && !%anchor.pinRight);
	pCheck("\"height\" reads back as both vertical pins",
		%anchor.pinTop && %anchor.pinBottom);
	pCheck("readout names the resolved pair",
		%anchor.readout.getText() $= "anchorLeft / height");

	// The other three deprecated spellings.
	$pButton.HorizSizing = "left";
	pCheck("\"left\" still maps to anchorRight", $pButton.HorizSizing $= "anchorRight");
	$pButton.VertSizing = "top";
	pCheck("\"top\" still maps to anchorBottom", $pButton.VertSizing $= "anchorBottom");
	$pButton.VertSizing = "bottom";
	pCheck("\"bottom\" still maps to anchorTop", $pButton.VertSizing $= "anchorTop");
	$pButton.HorizSizing = "relative";
	pCheck("\"relative\" still maps to scale", $pButton.HorizSizing $= "scale");

	// And the picker reads a control that was set the old way.
	$pButton.HorizSizing = "right";
	$pButton.VertSizing = "height";
	%pane.refresh();

	// The pins are toggle icons, so performClick drives the real path: the
	// checkbox flips itself and reports what it became.
	// Pinning the right edge as well must produce "width", not "left".
	%anchor.rightPin.performClick();
	pCheck("both horizontal pins resolve to width", $pButton.HorizSizing $= "width");
	%anchor.leftPin.performClick();
	pCheck("right pin alone resolves to anchorRight", $pButton.HorizSizing $= "anchorRight");
	%anchor.rightPin.performClick();
	pCheck("no horizontal pin resolves to center", $pButton.HorizSizing $= "center");

	// Fill supersedes the pins and clearing it hands the axis back to them.
	%anchor.leftPin.performClick();
	%anchor.onChipClicked("h", "fill");
	pCheck("fill chip supersedes the pins", $pButton.HorizSizing $= "fill");
	pCheck("fill chip shows as on", %anchor.hFill.getStateOn());
	%anchor.onChipClicked("h", "fill");
	pCheck("clearing fill returns to the pins", $pButton.HorizSizing $= "anchorLeft");

	// Scale and fill are exclusive on an axis.
	%anchor.onChipClicked("h", "scale");
	pCheck("scale chip applies", $pButton.HorizSizing $= "scale");
	%anchor.onChipClicked("h", "fill");
	pCheck("fill replaces scale", $pButton.HorizSizing $= "fill");
	pCheck("scale turned off", !%anchor.hRel.getStateOn());

	// Touching a pin drops the special.
	%anchor.leftPin.performClick();
	pCheck("a pin click clears the special", $pButton.HorizSizing !$= "fill");

	// The vertical axis is written even when only the horizontal one moved.
	// Checked here, while everything above has touched H alone.
	pCheck("vertical sizing survived horizontal edits", $pButton.VertSizing $= "height");

	// --- Center and fill take effect at once, and are reversible. ---
	// Both describe a position the control should always be in rather than a
	// reaction to a size change, so waiting for the next parent resize would
	// make picking one look like it did nothing.
	$pButton.HorizSizing = "anchorLeft";
	$pButton.VertSizing = "anchorTop";
	$pButton.setPosition(37, 41);
	$pButton.setExtent(120, 26);
	%pane.refresh();

	%parentW = getWord($pButton.getParent().getExtent(), 0);
	%anchor.onChipClicked("h", "fill");
	pCheck("fill moved the control to the left edge immediately",
		getWord($pButton.getPosition(), 0) == 0);
	pCheck("fill widened the control immediately",
		getWord($pButton.getExtent(), 0) > 120);
	pCheck("fill left the other axis alone",
		getWord($pButton.getPosition(), 1) == 41 &&
		getWord($pButton.getExtent(), 1) == 26);

	pCheck("fill stashed what it overwrote",
		%pane.stashPos["h"] $= "37 41" && %pane.stashExtent["h"] $= "120 26");

	// Clicking away from fill gives back exactly what it overwrote.
	%anchor.onChipClicked("h", "scale");
	pCheck("leaving fill cleared the stash", %pane.stashed["h"] $= "");
	pCheck("leaving fill restored the x position",
		getWord($pButton.getPosition(), 0) == 37);
	pCheck("leaving fill restored the width",
		getWord($pButton.getExtent(), 0) == 120);

	// Center owns the position but not the extent. Clearing scale hands the
	// axis back to its pins, which is anchorLeft, so one click empties them.
	%anchor.onChipClicked("h", "scale");
	%anchor.leftPin.performClick();
	pCheck("no pins resolves to center", $pButton.HorizSizing $= "center");
	pCheck("center recentred the control immediately",
		getWord($pButton.getPosition(), 0) != 37);
	pCheck("center left the width alone", getWord($pButton.getExtent(), 0) == 120);

	%anchor.leftPin.performClick();
	pCheck("leaving center restored the x position",
		getWord($pButton.getPosition(), 0) == 37);

	// The stash belongs to the control it came from: selecting something else
	// throws it away rather than carrying a stale position across.
	%anchor.onChipClicked("h", "fill");
	pBind($pWindow);
	%pane = pBind($pButton);
	pCheck("the stash does not survive a selection change",
		%pane.stashed["h"] $= "");
	$pButton.HorizSizing = "anchorLeft";
	$pButton.setPosition(37, 41);
	$pButton.setExtent(120, 26);
	%pane.refresh();

	// With Fill in effect the pins are not what is happening, so they read off
	// -- and a disabled axis must refuse the click entirely.
	%anchor.setAxisEnabled(false, true);
	%before = $pButton.HorizSizing;
	%anchor.leftPin.performClick();
	pCheck("a disabled axis ignores its pins", $pButton.HorizSizing $= %before);
	%anchor.setAxisEnabled(true, true);

	$pButton.HorizSizing = "right";
	$pButton.VertSizing = "bottom";
	%pane.refresh();

	// Into a vertical chain: it takes the Y position and the vertical sizing,
	// and leaves the X position and the extent alone.
	$pChain.IsVertical = true;
	$pChain.add($pButton);
	%pane = pBind($pButton);
	pCheck("chain child keeps X position", %pane.header.positionRow.editor.isActive());
	pCheck("chain child loses Y position", !%pane.header.positionRow.editorY.isActive());
	pCheck("chain child keeps horizontal anchoring",
		%pane.header.anchorPicker.leftPin.isActive());
	pCheck("chain child loses vertical anchoring",
		!%pane.header.anchorPicker.topPin.isActive());
	pCheck("chain child keeps its extent", %pane.header.extentRow.editor.isActive());

	// Into a grid: the cell owns everything.
	$pGrid.add($pButton);
	%pane = pBind($pButton);
	pCheck("grid child loses X position", !%pane.header.positionRow.editor.isActive());
	pCheck("grid child loses Y position", !%pane.header.positionRow.editorY.isActive());
	pCheck("grid child loses its extent", !%pane.header.extentRow.editor.isActive());
	pCheck("grid child loses both sizings",
		!%pane.header.anchorPicker.leftPin.isActive() &&
		!%pane.header.anchorPicker.topPin.isActive());

	// A scroller is the container that looks like it owns its children and does
	// not -- it only scrolls them.
	$pScroll = pAdd("GuiScrollCtrl");
	$pScroll.add($pButton);
	%pane = pBind($pButton);
	pCheck("scroll child keeps its position", %pane.header.positionRow.editor.isActive());
	pCheck("scroll child keeps its extent", %pane.header.extentRow.editor.isActive());

	schedule(200, 0, "pStep6");
}

//-----------------------------------------------------------------------------
// Toggles, the profile picker, and clearing the selection.
//-----------------------------------------------------------------------------

function pStep6()
{
	%pane = pBind($pButton);
	%header = %pane.header;

	// The toggle's box has to cover the whole control for it to read as a
	// button rather than a checkbox. It got this wrong once: setBoxOffset and
	// setBoxExtent document one argument and read two, so a single "0 0" left
	// the box at (0,32) -- drawn entirely below the control.
	pCheck("toggle box covers the whole control",
		%header.visibleButton.getBoxOffset() $= "0 0" &&
		%header.visibleButton.getBoxExtent() $= %header.visibleButton.getExtent());

	// The four runtime state flags are icon toggles now that the sheet has art
	// for them. hidden and locked are not among them any more -- they are editor
	// working state and moved out to the Explorer tree's columns.
	pCheck("the pane no longer offers hidden", !isObject(%header.hiddenButton));
	pCheck("the pane no longer offers locked", !isObject(%header.lockedButton));

	// And they must not come back through the side door. Both are real persist
	// fields on SimObject, and buildOtherSection sweeps up every field no section
	// claimed -- so unless editorToggles() keeps naming them, removing the two
	// buttons does not remove the two controls from the pane. It turns them into
	// a pair of generic checkboxes in "Other", which is worse than where they
	// started: same working state, now filed under the leftovers.
	pCheck("hidden did not reappear as a generic row", !pRowBuilt(%pane, "hidden"));
	pCheck("locked did not reappear as a generic row", !pRowBuilt(%pane, "locked"));
	pCheck("the spec still claims both",
		%pane.spec.editorToggles() $= "hidden locked");

	pCheck("visible toggle reflects the control",
		%header.visibleButton.getValue() == $pButton.Visible);

	%header.visibleButton.performClick();
	pCheck("visible toggle reached the control", !$pButton.Visible);
	%header.visibleButton.performClick();
	pCheck("visible toggle restored", $pButton.Visible);

	%header.inputButton.performClick();
	pCheck("accepts-input toggle reached the control", !$pButton.useInput);
	%header.inputButton.performClick();

	%header.containerButton.performClick();
	pCheck("accepts-children toggle reached the control", $pButton.isContainer);
	%header.containerButton.performClick();

	// A control that cannot draw children has the field forced false, so the
	// button goes rather than sitting there wired to nothing.
	%listPane = pBind(pAdd("GuiListBoxCtrl"));
	pCheck("accepts-children hidden where it is dead",
		!%listPane.header.containerButton.isVisible());
	%pane = pBind($pButton);
	%header = %pane.header;
	pCheck("accepts-children shown where it is live",
		%header.containerButton.isVisible());

	// A toggle icon is a checkbox wearing an icon, so it holds its own state.
	// performClick drives the real path: the checkbox flips itself and its
	// Command tells the pane what it became. activeButton has a true on/off pair,
	// so it is the one that can prove the icon follows the value.
	%header.activeButton.performClick();
	pCheck("active toggle reached the control", !$pButton.Active);
	pCheck("active icon shows the off frame",
		%header.activeButton.icon.getImageFrame() == %header.activeButton.frameOff);
	%header.activeButton.performClick();
	pCheck("active toggle flips back", $pButton.Active);
	pCheck("active icon shows the on frame",
		%header.activeButton.icon.getImageFrame() == %header.activeButton.frameOn);

	// A disabled toggle must not act. The engine hands touch events to inactive
	// controls -- findHitControl checks mVisible and mUseInput, never mActive --
	// so this is the checkbox's own guard doing the work.
	%header.activeButton.setActive(false);
	%header.activeButton.performClick();
	pCheck("a disabled toggle does not change the control", $pButton.Active);
	pCheck("a disabled toggle keeps its own state on", %header.activeButton.getValue());
	%header.activeButton.setActive(true);

	// The profile picker offers the theme's members for the control's category,
	// not every profile in the sim.
	%items = %header.profileRow.editor.getItemCount();
	pCheck("profile picker has candidates (" @ %items @ ")", %items > 0);
	%current = GuiEditor.themeApplier.fieldProfile($pButton, "Profile");
	pCheck("profile picker shows what the control wears",
		isObject(%current) && %header.profileRow.getValue() $= %current.getName());

	// A menu item has no GuiControl fields at all, so the header sheds everything
	// generic and shows a block of its own instead - caption included, on one
	// line, because a menu label is one line and it is also how wide the menu is.
	$pMenuItem = new GuiMenuItemCtrl();
	%pane = pBind($pMenuItem);
	%block = %pane.header.menuItemBlock;
	pCheck("menu item hides the profile row", !%pane.header.profileRow.isVisible());
	pCheck("menu item hides geometry", !%pane.header.geometryGrid.isVisible());
	pCheck("menu item shows its own block", %block.isVisible());
	pCheck("which stands the shared text block down", !%pane.header.textBlock.isVisible());
	pCheck("its caption is single line", %block.textRow.kind $= "text");
	pCheck("it carries the command fields", %block.commandRow.isVisible() &&
		%block.acceleratorRow.isVisible());

	// Visible and Active are GuiControl's names, but a menu item registers them
	// again for itself, so those two switches really do work here.
	pCheck("menu item keeps Visible and Active", %pane.header.visibleButton.isVisible() &&
		%pane.header.activeButton.isVisible());
	pCheck("but not Accepts Input", !%pane.header.inputButton.isVisible());

	// The section those fields used to be in is gone, so nothing is left showing
	// a header that cannot open.
	pCheck("no dead Menu Item section", !isObject(%pane.panel["Item"]) ||
		!%pane.panel["Item"].isVisible());

	// An ordinary control is untouched by any of it.
	%pane = pBind($pButton);
	pCheck("an ordinary control shows no menu item block",
		!%pane.header.menuItemBlock.isVisible());
	pCheck("and keeps the shared text block", %pane.header.textBlock.isVisible());

	schedule(200, 0, "pStep7");
}

//-----------------------------------------------------------------------------
// Dynamic fields: built fresh rather than ported, and filtered so a frame set's
// serialized layout tree cannot be hand-edited into an unloadable Gui.
//-----------------------------------------------------------------------------

function pStep7()
{
	%pane = pBind($pButton);
	%dyn = %pane.dynamicFields;

	pCheck("dynamic section built", isObject(%dyn));
	pCheck("dynamic section shown for a bound control", %pane.dynamicPanel.isVisible());
	pCheck("a plain control starts with no dynamic fields", !%dyn.hasFields());

	// Add through the widget, exactly as a click would. A dynamic field cannot
	// hold an empty value -- SimFieldDictionary::setFieldValue frees the entry
	// when the value is empty -- so Add produces a row, and giving that row a
	// value is what puts the field on the control.
	%dyn.nameBox.setText("smokeTag");
	%dyn.onAddClicked();
	pCheck("add built a row for the new name", isObject(%dyn.row["smokeTag"]));
	pCheck("name box cleared after adding", %dyn.nameBox.getText() $= "");
	pCheck("naming alone does not create the field",
		!pHasDynamicField($pButton, "smokeTag"));

	// Edit its value: this is what creates it.
	%dyn.row["smokeTag"].applyValue("hello");
	%dyn.row["smokeTag"].commit();
	pCheck("dynamic value commit reached the control", $pButton.smokeTag $= "hello");
	pCheck("the field now really exists", pHasDynamicField($pButton, "smokeTag"));

	// A name that is already a registered field must not be accepted -- writing
	// it would set the real field and quietly do something else entirely.
	%dyn.nameBox.setText("Extent");
	%dyn.onAddClicked();
	pCheck("a built-in field name is refused", !isObject(%dyn.row["Extent"]));
	%dyn.nameBox.setText("");

	// A frame set hides the fields it serializes its own layout into.
	$pFrameSet = pAdd("GuiFrameSetCtrl");
	$pFrameSet.frameID0 = "1";
	$pFrameSet.myOwnField = "keep";
	%pane = pBind($pFrameSet);
	%dyn = %pane.dynamicFields;
	pCheck("frame set hides its serialized layout field", !isObject(%dyn.row["frameID0"]));
	pCheck("frame set still shows a user field", isObject(%dyn.row["myOwnField"]));

	schedule(200, 0, "pStep8");
}

function pStep8()
{
	%pane = pBind($pButton);
	%dyn = %pane.dynamicFields;
	pCheck("dynamic field survived reselect", isObject(%dyn.row["smokeTag"]));

	// Remove, which is the row's reset button repurposed.
	%dyn.onFieldRowReset(%dyn.row["smokeTag"]);
	schedule(100, 0, "pStep9");
}

function pStep9()
{
	%pane = GuiEditor.inspectorWindow.pane;
	pCheck("remove took the field off the control", !pHasDynamicField($pButton, "smokeTag"));
	pCheck("remove took the row with it", !isObject(%pane.dynamicFields.row["smokeTag"]));

	// Nothing selected: the pane stops drawing rather than showing stale values.
	%pane.unbind();
	pCheck("pane hides when nothing is selected", !%pane.isVisible());

	echo("IPSMOKE DONE");
	schedule(300, 0, "quit");
}
