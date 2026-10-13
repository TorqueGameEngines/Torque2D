
//-----------------------------------------------------------------------------
// Everything GuiControl::renderText reads, in one component.
//
// These seven fields used to be nine rows in a shared "Text" section that the
// pane hid wholesale whenever the header carried the text box -- and the header
// carried three of them. The other five had nowhere to go, which is how a
// GuiControl ended up with a caption it could not resize. Putting them in one
// component makes that impossible to repeat: the block is either shown or it is
// not, and it is the only place any of these fields live.
//
//     caption row   the block's heading, and the two flags that change what the
//                   text does to the control it sits in. Wrap first, extend
//                   second; extend stays live either way, because
//                   guiControl.cc grows the width when wrap is off and the
//                   height when it is on.
//     text box      a GuiTextEditCtrl with textWrap on, which is what makes it
//                   multi-line: it wraps, scrolls and hit-tests in line space.
//                   Three lines tall, because one line of a heading is a poor
//                   view of a paragraph.
//     align grid    the two alignments, as segmented rows labelled "H:" and
//                   "V:" -- one decision each, and the icons say the rest --
//                   and the quarter turn, "R:", where the class honors it.
//     font grid     size and color, the pair anyone reaching for "this text is
//                   too small" wants.
//
// Both grids are cell grids, so they sit side by side in a wide Properties
// frame and stack in a narrow one.
//
// Two of these exist. The header holds one, for the classes whose text is a
// principal property; the Text section holds the other, for the classes that
// can draw text but are not asked to (a grid) and the one that draws none but
// still sizes it (a slider). Two cheap instances rather than reparenting a live
// control out from under a selection event -- see
// GuiEditorControlSpec::textBlockHome.
//
// Who owns what: the block owns its layout and decides which of its parts a
// class can use. It owns no values. Its three field rows report straight to the
// pane, and its choice rows and toggles are forwarded there, so the pane stays
// the only thing that writes to the control.
//
// The creator sets pane, spec and blockWidth inline, then calls build() once
// after adding it.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::onAdd(%this)
{
	ThemeManager.setProfile(%this, "emptyProfile");
}

function GuiEditorTextBlock::build(%this)
{
	%this.buildCaption();
	%this.buildText();
	%this.buildAlign();
	%this.buildFont();
}

//-----------------------------------------------------------------------------
// The caption line: what this block is about, and the two flags that belong
// with it rather than with the alignments.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::buildCaption(%this)
{
	%w = %this.blockWidth;
	%size = 24;

	%row = new GuiControl()
	{
		HorizSizing = "width";
		Position = "0 0";
		Extent = %w SPC (%size + 2);
	};
	ThemeManager.setProfile(%row, "emptyProfile");
	%this.add(%row);
	%this.captionRow = %row;

	// The label stops short of the two icons and keeps that gap as the pane
	// widens; the icons keep their distance from the right edge.
	%this.label = new GuiControl()
	{
		HorizSizing = "width";
		Position = "4 4";
		Extent = (%w - 8 - ((%size + 2) * 2)) SPC 16;
		Text = "Text";
		align = "left";
		vAlign = "middle";
	};
	ThemeManager.setProfile(%this.label, "labelProfile");
	%row.add(%this.label);

	%this.wrapButton = %this.makeIcon(%row, %w - ((%size + 2) * 2), %size, "textWrap",
		"Wrap Text", $EditorIcon::align_just,
		"Wraps onto as many lines as it needs, breaking between words. Return puts a line break in while you are typing here.",
		"Draws on one line whatever the control's width, and anything past the edge is clipped. Return finishes the edit.");

	%this.extendButton = %this.makeIcon(%row, %w - (%size + 2), %size, "textExtend",
		"Extend To Fit Text", $EditorIcon::expand, "", "");
	%this.applyExtendTip(false);
}

// One icon in the caption row, pinned to the right edge as the pane widens.
function GuiEditorTextBlock::makeIcon(%this, %row, %x, %size, %field, %label, %frame, %tipOn, %tipOff)
{
	%button = new GuiCheckBoxCtrl()
	{
		class = "EditorToggleIcon";
		HorizSizing = "left";
		Position = %x SPC 1;
		Extent = %size SPC %size;
		frameOn = %frame;
		frameOff = %frame;
		tipOn = %tipOn;
		tipOff = %tipOff;
		toggleName = %field;
		toggleLabel = %label;
		owner = %this;
	};
	ThemeManager.setProfile(%button, "iconButtonProfile");
	ThemeManager.setProfile(%button, "tipProfile", "TooltipProfile");
	%row.add(%button);
	return %button;
}

// Extend does something different depending on wrap, so its tooltip has to say
// which -- it is the only way to tell from the pane that the same flag grows
// two different axes (guiControl.cc renderText: extent.y when wrapping, extent.x
// when not).
function GuiEditorTextBlock::applyExtendTip(%this, %wrapping)
{
	%tip = %wrapping
		? "Grows taller to fit however many lines the text wraps onto, so nothing is clipped."
		: "Grows wider to fit the text on its one line, so nothing is clipped.";

	%this.extendButton.tipOn = %tip;
	%this.extendButton.tipOff = %tip;
	%this.extendButton.refresh();
}

//-----------------------------------------------------------------------------
// The text box itself, as a field row of the pane's own kind so that loading,
// committing and the changed check are the ones every other field gets.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::buildText(%this)
{
	// No caption of its own: the caption row above is its label, which is what
	// leaves room for the two icons beside it. An empty labelText is how a field
	// row is told to keep no space for one.
	%row = %this.pane.makeFieldRow(%this, "text", "", "multiline", "");
	%this.row["text"] = %row;
	%this.textRow = %row;

	// Command is free on a field row -- it commits on AltCommand (blur) and
	// ReturnCommand -- and GuiTextEditCtrl runs Command on every edit to its
	// buffer: a character, a backspace, a delete, a paste, an undo. So this is
	// the per-keystroke hook, and it is what lets the control on the canvas fill
	// in as you type.
	%row.editor.Command = %this.getID() @ ".onTextTyped();";
}

//-----------------------------------------------------------------------------
// Typing. The control is updated on every keystroke, but the edit is still one
// edit: the text the control held when the first key landed is kept here, and
// the pane writes the change once, properly, when the box loses focus.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::onTextTyped(%this)
{
	if(%this.isPopulating() || !isObject(%this.pane.target))
	{
		return;
	}

	%ctrl = %this.pane.target;

	// Taken on the first keystroke, because that is the last moment the control
	// still holds what the edit started from.
	if(!%this.typing)
	{
		%this.typing = true;
		%this.typingTarget = %ctrl;
		%this.textBeforeEdit = %ctrl.getFieldValue("text");
	}

	// Straight onto the control rather than through the pane's writeField: this
	// runs per character, and an edit is one change however many keys it took.
	%ctrl.text = %this.textRow.getValue();

	// Cheap -- the explorer tree refreshes the one row's label -- and it keeps
	// the tree reading the same as the canvas while you type.
	%this.pane.afterCommit();
}

function GuiEditorTextBlock::endTyping(%this)
{
	%this.typing = false;
	%this.typingTarget = "";
	%this.textBeforeEdit = "";
}

//-----------------------------------------------------------------------------
// Alignment. Two segmented rows with the narrowest labels the row supports --
// eight buttons and two captions have to fit a Properties frame someone has
// dragged narrow. Rotation shares their grid: it decides which way align and
// vAlign run, so it belongs beside them.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::buildAlign(%this)
{
	%grid = %this.makeGrid(128, 30);
	%this.add(%grid);
	%this.alignGrid = %grid;

	%this.alignRow = %this.makeChoiceRow(%grid, "align", "H:",
		"left" TAB $EditorIcon::align_left TAB "Align text to the left" NL
		"center" TAB $EditorIcon::align_center TAB "Centre text" NL
		"right" TAB $EditorIcon::align_right TAB "Align text to the right");

	%this.vAlignRow = %this.makeChoiceRow(%grid, "vAlign", "V:",
		"top" TAB $EditorIcon::align_top TAB "Align text to the top" NL
		"middle" TAB $EditorIcon::align_middle TAB "Centre text vertically" NL
		"bottom" TAB $EditorIcon::align_bottom TAB "Align text to the bottom");

	// "none" is the unturned value rather than an inherited one: a profile has
	// no rotation of its own to defer to.
	%this.rotationRow = %this.makeChoiceRow(%grid, "textRotation", "R:",
		"ccw" TAB $EditorIcon::undo TAB "Turn text 90 degrees counter-clockwise, reading bottom to top. H: then runs up the control and V: across it." NL
		"cw" TAB $EditorIcon::redo TAB "Turn text 90 degrees clockwise, reading top to bottom. H: then runs down the control and V: across it.",
		"none" TAB "Text is not turned");
}

// %choices is one record per value: value TAB icon TAB tooltip. An iconless
// choice leads every row -- the value a control starts on. For the alignments
// that is "default", which is not an absence but getAlignmentType resolving to
// the profile's own alignment; %first (value TAB tooltip) names another.
function GuiEditorTextBlock::makeChoiceRow(%this, %grid, %field, %label, %choices, %first)
{
	if(%first $= "")
	{
		%first = "default" TAB "Use the profile's alignment";
	}

	%row = new GuiControl()
	{
		class = "EditorChoiceRow";
		Position = "0 0";
		labelText = %label;
		labelWidth = 24;
		fieldName = %field;
		owner = %this;
	};
	%grid.add(%row);

	%row.addChoice(getField(%first, 0), "", getField(%first, 1));
	%count = getRecordCount(%choices);
	for(%i = 0; %i < %count; %i++)
	{
		%rec = getRecord(%choices, %i);
		%row.addChoice(getField(%rec, 0), getField(%rec, 1), getField(%rec, 2));
	}

	%row.build();
	return %row;
}

//-----------------------------------------------------------------------------
// Size and color.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::buildFont(%this)
{
	%grid = %this.makeGrid(128, 48);
	%this.add(%grid);
	%this.fontGrid = %grid;

	// The kind comes from the pane's table rather than a literal here, so this
	// row cannot end up spelling a field's type differently from everywhere
	// else -- fontSizeAdjust is a multiplier, and a whole-number row would round
	// every useful value of it away.
	%this.row["fontSizeAdjust"] = %this.pane.makeFieldRow(%grid, "fontSizeAdjust",
		"Font Size", %this.pane.sharedKindFor("fontSizeAdjust"), "");
	%this.fontSizeRow = %this.row["fontSizeAdjust"];

	// One widget for two fields. overrideFontColor has no row: picking a color
	// is what turns it on, and the reset button -- which every field row already
	// carries -- is what turns it off again. With it off the swatch shows the
	// profile's own color, so the row always says what the control will draw in.
	%this.row["fontColor"] = %this.pane.makeFieldRow(%grid, "fontColor",
		"Font Color", %this.pane.sharedKindFor("fontColor"), "");
	%this.fontColorRow = %this.row["fontColor"];
	%this.fontColorRow.resetButton.Tooltip = "Go back to the profile's font color";
}

// The block's own cell grid. Narrower cells than the pane's rows use: two of
// these fit where one 220-wide field row does, which is what lets the alignment
// pair and the font pair each sit on one line.
function GuiEditorTextBlock::makeGrid(%this, %cellW, %cellH)
{
	%grid = new GuiGridCtrl()
	{
		HorizSizing = "width";
		Position = "0 0";
		Extent = %this.blockWidth SPC 4;
		CellModeX = "variable";
		CellModeY = "variable";
		CellSizeX = %cellW;
		CellSizeY = %cellH;
		CellSpacingX = 4;
		CellSpacingY = 4;
		MaxColCount = 0;
		MaxRowCount = 0;
		OrderMode = "lrtb";
		IsExtentDynamic = true;
	};
	ThemeManager.setProfile(%grid, "emptyProfile");
	return %grid;
}

//-----------------------------------------------------------------------------
// Binding. Which parts apply is the spec's answer, asked per field rather than
// per role so that the per-class exceptions (a text edit has no vAlign, a tab
// book no wrap) are honoured here too.
//
// The three field rows are the pane's to show or hide -- they are in its shared
// registry and its filter walks them. What is left is this block's own.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::bindClass(%this, %ctrl, %class)
{
	%spec = %this.spec;

	%this.label.setText(%spec.textLabelFor(%class));

	%wrap = %spec.isFieldVisible(%class, "textWrap");
	%extend = %spec.isFieldVisible(%class, "textExtend");
	%this.wrapButton.setVisible(%wrap);
	%this.extendButton.setVisible(%extend);

	// The caption row is the heading for a text box and the home of the two
	// flags. With none of the three on show it is a title over nothing -- which
	// is what a slider would get, since it reads fontSizeAdjust and nothing else
	// in this block.
	%this.captionRow.setVisible(%spec.isFieldVisible(%class, "text") || %wrap || %extend);

	%this.alignRow.setVisible(%spec.isFieldVisible(%class, "align"));
	%this.vAlignRow.setVisible(%spec.isFieldVisible(%class, "vAlign"));
	%this.rotationRow.setVisible(%spec.isFieldVisible(%class, "textRotation"));

	// A container that lays out only the children it can see needs telling when
	// one of them comes or goes; nothing re-runs the layout on setVisible.
	%this.alignGrid.setVisible(%this.alignRow.isVisible() || %this.vAlignRow.isVisible() ||
		%this.rotationRow.isVisible());
	%this.fontGrid.setVisible(%spec.isFieldVisible(%class, "fontSizeAdjust") ||
		%spec.isFieldVisible(%class, "fontColor"));

	%this.resizeGrid(%this.alignGrid);
	%this.resizeGrid(%this.fontGrid);
	%this.resizeToFit();
}

// The block's own width rather than the one it was built at: blockWidth is what
// the pane authored, and a grid re-measured against it would snap back to that
// after the Properties frame had been dragged wider.
function GuiEditorTextBlock::resizeGrid(%this, %grid)
{
	%grid.resize(0, 0, getWord(%this.getExtent(), 0), getWord(%grid.getExtent(), 1));
}

// A GuiChainCtrl positions its children without resizing them, and a grid only
// learns its height once something lays it out, so nudge the width by a pixel
// and back to force exactly one parentResized through every child.
//
// Through its own position, not through zero: resize() sets the position as
// well as the extent. The header's copy lives in a chain, which puts its
// children back wherever it likes, but the Text section's copy sits at y=24
// under the panel's title bar -- and moving it to zero drew its caption and its
// two icons on top of that title, which read as a second "Text" printed over
// the first.
function GuiEditorTextBlock::resizeToFit(%this)
{
	%x = getWord(%this.getPosition(), 0);
	%y = getWord(%this.getPosition(), 1);
	%w = getWord(%this.getExtent(), 0);
	%h = getWord(%this.getExtent(), 1);

	%this.resize(%x, %y, %w + 1, %h);
	%this.resize(%x, %y, %w, %h);
}

//-----------------------------------------------------------------------------
// Loading. The pane loads the field rows it registered; what is left is the two
// choice rows, the two toggles, and the one row whose value is not simply the
// field it names.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::load(%this, %ctrl)
{
	%this.populating = true;

	// A pending edit belongs to whatever was selected when it started. Loading
	// a control abandons it -- the typed text is already on that control, so
	// there is nothing to lose, and keeping the stash would let a blur that
	// arrives after the selection moved write one control's caption onto
	// another.
	%this.endTyping();

	%this.alignRow.setValue(%ctrl.getFieldValue("align"));
	%this.vAlignRow.setValue(%ctrl.getFieldValue("vAlign"));
	%this.rotationRow.setValue(%ctrl.getFieldValue("textRotation"));

	%this.wrapButton.setValue(%ctrl.textWrap);
	%this.extendButton.setValue(%ctrl.textExtend);
	%this.applyExtendTip(%ctrl.textWrap);

	%this.loadFontColor(%ctrl);

	%this.populating = false;
}

// The swatch shows what the control will actually draw in: its own color while
// it is overriding, and the profile's while it is not. Anything else would have
// the row claim a color the control does not use.
function GuiEditorTextBlock::loadFontColor(%this, %ctrl)
{
	%override = %ctrl.overrideFontColor;
	%color = %ctrl.getFieldValue("fontColor");

	if(!%override)
	{
		%profile = GuiEditor.themeApplier.fieldProfile(%ctrl, "Profile");
		if(isObject(%profile) && %profile.fontColor !$= "")
		{
			%color = %profile.fontColor;
		}
	}

	%this.fontColorRow.setValue(%color);
	%this.fontColorRow.setOverridden(%override);
}

//-----------------------------------------------------------------------------
// Forwarding. Every write still goes through the pane.
//-----------------------------------------------------------------------------

function GuiEditorTextBlock::isPopulating(%this)
{
	return %this.populating || %this.pane.populating;
}

function GuiEditorTextBlock::onChoiceRowChanged(%this, %row)
{
	if(%this.isPopulating())
	{
		return;
	}
	%this.pane.onHeaderChoiceChanged(%row.fieldName, %row.getValue());
}

function GuiEditorTextBlock::onToggleIconChanged(%this, %toggle)
{
	if(%this.isPopulating())
	{
		return;
	}

	if(%toggle.toggleName $= "textWrap")
	{
		%this.applyExtendTip(%toggle.getValue());
	}

	%this.pane.onTextFlagChanged(%toggle.toggleName, %toggle.getValue());
}
