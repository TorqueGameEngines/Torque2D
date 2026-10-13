
//-----------------------------------------------------------------------------
// The Gui Editor's properties pane, in place of the generic C++ GuiInspector.
//
// The inspector reflected every registered field into flat alphabetical groups,
// which meant it offered a GuiChainCtrl nine text fields it never draws and a
// GuiTabPageCtrl four geometry fields its book overwrites on every layout pass.
// This asks GuiEditorControlSpec what the selected class actually reads and
// shows that, with the fields that matter most in a header that is always open.
//
// Layout is a vertical chain of blocks, the same arrangement
// GuiProfileEditorProfileForm uses and for the same reason: each block lays its
// fields out in a GuiGridCtrl, so widening the Properties frame reflows the
// cells into more columns instead of leaving dead space.
//
// Two kinds of block, and the difference matters:
//
//   shared    Every control has these fields, so the rows are built once and
//             filtered with setVisible. Nothing is ever freed, so a selection
//             change can never delete a control the engine is mid-dispatch on.
//   class     The class's own sections and the header's value block. These
//             fields do not exist on other classes, so there is no shared row
//             to hide and they are rebuilt when the selected class changes.
//
// Rebuilding is safe here in a way it was not in the Profile Editor's preview,
// because nothing inside this pane can change the target's class: rebuilds only
// ever arrive from a selection change, which originates on the canvas or in the
// explorer tree, never on a widget this pane owns. The one exception -- a
// sprite's source mode, which a commit CAN change -- is deferred to schedule(0)
// rather than run inside the commit.
//
// The pane owns every write to the control; its rows only marshal values. The
// creator sets paneWidth and window inline, then calls build() once after
// adding the pane to its scroller.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::onAdd(%this)
{
	ThemeManager.setProfile(%this, "emptyProfile");

	// The font color swatch gets the popup that offers the theme's six colors.
	// EditorFieldRow lives in EditorCore and cannot name a Gui Editor class, so
	// the pane that wants one says so -- see its header.
	%this.swatchClass = "GuiProfileEditorColorPopup";

	%this.spec = new ScriptObject()
	{
		class = "GuiEditorControlSpec";
	};
}

function GuiEditorInspectorPane::onRemove(%this)
{
	%this.unbind();

	if(isObject(%this.spec))
	{
		%this.spec.delete();
	}
}

//-----------------------------------------------------------------------------
// Construction.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::build(%this)
{
	%w = %this.paneWidth;

	// The narrowest a cell column may get. The grids run in Variable mode, so
	// they fit as many columns as the pane can hold at this width and share the
	// remainder evenly -- which is what makes dragging the frame wider add
	// columns rather than whitespace.
	%this.rowWidth = 220;

	// Half of that, for the sections whose fields are read in pairs: an easing
	// and the time it takes, a tooltip's width and its delay, a window's title
	// height and its six switches.
	%this.pairWidth = 152;

	%this.rowFields = "";
	%this.panelList = "";
	%this.classPanels = "";
	%this.boundClass = "";

	%this.header = new GuiChainCtrl()
	{
		class = "GuiEditorHeaderBlock";
		HorizSizing = "width";
		Position = "0 0";
		Extent = %w SPC 40;
		IsVertical = true;
		ChildSpacing = 2;
		blockWidth = %w;
		pane = %this;
		spec = %this.spec;
	};
	%this.add(%this.header);
	%this.header.build();

	// The class's own sections live in a chain of their own so that rebuilding
	// them cannot change where they sit: added straight to the outer chain they
	// would land after the shared sections every time they were replaced.
	%this.classChain = new GuiChainCtrl()
	{
		HorizSizing = "width";
		Position = "0 0";
		Extent = %w SPC 4;
		IsVertical = true;
		ChildSpacing = 6;
	};
	ThemeManager.setProfile(%this.classChain, "emptyProfile");
	%this.add(%this.classChain);

	// Variants get a chain of their own because they are rebuilt more often
	// than the class sections: whether a slot is worth showing depends on what
	// the theme holds right now, which the Profile Editor can change while the
	// same control stays selected.
	%this.variantsChain = new GuiChainCtrl()
	{
		HorizSizing = "width";
		Position = "0 0";
		Extent = %w SPC 4;
		IsVertical = true;
		ChildSpacing = 6;
	};
	ThemeManager.setProfile(%this.variantsChain, "emptyProfile");
	%this.add(%this.variantsChain);

	// The shared sections, in the order the work usually goes.
	%this.buildTextSection();
	%this.buildItemsSection();

	// isContainer and useInput are both in the header's icon row now that there
	// is art for them, so neither has a row here.
	%this.buildSection("Layout", "Layout", "MinExtent");
	%this.buildSection("Command", "Command", "Command AltCommand Variable Accelerator");

	// Each easing beside the time it takes: hover on one line, press on the next.
	%this.buildSection("Animation", "Animation", %this.spec.easingFields(), %this.pairWidth);

	%this.buildTooltipSection();
	%this.buildSection("Localization", "Localization", "langTableMod textID");
	%this.buildSection("Scripting", "Scripting", "class superclass internalName");

	// Dynamic fields last, and in a section of their own rather than through
	// buildSection: what it holds is not a fixed field list, so it owns its own
	// rows and its own commits.
	%this.dynamicPanel = %this.makeSectionPanel("Dynamic Fields");
	%this.add(%this.dynamicPanel);

	%this.dynamicFields = new GuiChainCtrl()
	{
		class = "GuiEditorDynamicFields";
		HorizSizing = "width";
		Position = "0 24";
		Extent = %w SPC 4;
		IsVertical = true;
		ChildSpacing = 4;
		blockWidth = %w;
		pane = %this;
	};
	%this.dynamicPanel.add(%this.dynamicFields);
	%this.dynamicFields.build();

	%this.forceLayout();
}

// A GuiPanelCtrl learns its collapsed height only from parentResized -- its
// constructor defaults to 64x64 whatever Extent it was given, and a chain
// positions its children without ever resizing them. Nudging the width by a
// pixel and back forces exactly one parentResized through every child and
// leaves the widths where they started.
function GuiEditorInspectorPane::forceLayout(%this)
{
	%w = %this.paneWidth;
	%h = getWord(%this.getExtent(), 1);
	%this.resize(0, 0, %w + 1, %h);
	%this.resize(0, 0, %w, %h);
}

// The grid configuration every block here uses, matching what the native
// inspector gave its group grids. A hidden cell is skipped rather than left as
// a hole, so filtering closes the gap (GuiGridCtrl::resize).
// %cellW is the NARROWEST a column may be, not its width: the grid fits as many
// columns as the pane can hold at that size and shares the remainder evenly. So
// a section that wants its fields in pairs asks for half a pane rather than
// arranging them itself -- and still reflows to one column when the frame is
// dragged narrow. Omit it for the ordinary one-field-per-row width.
function GuiEditorInspectorPane::makeCellGrid(%this, %y, %cellW)
{
	%grid = new GuiGridCtrl()
	{
		HorizSizing = "width";
		Position = "0" SPC %y;
		Extent = %this.paneWidth SPC 4;
		CellModeX = "variable";
		CellModeY = "variable";
		CellSizeX = (%cellW $= "") ? %this.rowWidth : %cellW;
		CellSizeY = 48;
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

// A collapsible section. Its cells sit in an inner grid rather than directly on
// the panel: GuiExpandCtrl::toggleHiddenChildren force-writes mVisible on every
// direct child whenever it expands, collapses or resizes, which would undo the
// filter. Grandchildren are left alone and the grid skips the hidden ones.
function GuiEditorInspectorPane::makeSectionPanel(%this, %title)
{
	%headerH = 24;

	%panel = new GuiPanelCtrl()
	{
		HorizSizing = "width";
		Text = %title;
		Position = "0 0";
		Extent = %this.paneWidth SPC %headerH;
		MinExtent = "80" SPC %headerH;
	};
	ThemeManager.setProfile(%panel, "panelProfile");
	return %panel;
}

// The text block's second home. Not a buildSection: it holds one component
// rather than a list of rows, so its visibility is decided by which home the
// class wants (GuiEditorControlSpec::textBlockHome) and not by counting visible
// rows. It stays out of panelList for that reason.
//
// The three field rows inside a block are named here instead, with no row
// behind them yet: applyFilter points them at whichever block is in use, and
// then the shared filter and the shared value loop reach them like any other
// field. That is what stops two blocks fighting over one entry in the registry.
function GuiEditorInspectorPane::buildTextSection(%this)
{
	%this.textPanel = %this.makeSectionPanel("Text");
	%this.add(%this.textPanel);

	%this.sectionText = %this.makeTextBlock(%this.textPanel, 24);

	%fields = %this.spec.textBlockRowFields();
	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.row[%field] = "";
		%this.rowFields = (%this.rowFields $= "") ? %field : (%this.rowFields SPC %field);
	}
}

// The tooltip is a paragraph rather than a caption -- it is where an editor
// answers a question instead of sending someone looking -- so it gets the same
// three-line box the text block uses, at the full width of the pane. Its width
// and its delay are two small numbers that read as a pair, so they share the
// line beneath it.
function GuiEditorInspectorPane::buildTooltipSection(%this)
{
	%panel = %this.makeSectionPanel("Tooltip");
	%this.add(%panel);

	%chain = new GuiChainCtrl()
	{
		HorizSizing = "width";
		Position = "0 24";
		Extent = %this.paneWidth SPC 4;
		IsVertical = true;
		ChildSpacing = 2;
	};
	ThemeManager.setProfile(%chain, "emptyProfile");
	%panel.add(%chain);

	%this.panel["Tooltip"] = %panel;
	%this.panelFields["Tooltip"] = %this.spec.tooltipFields();
	%this.panelList = (%this.panelList $= "") ? "Tooltip" : (%this.panelList SPC "Tooltip");

	%this.addFieldRow(%chain, "tooltip", %this.spec.labelFor("tooltip"), "multiline", "");

	%grid = %this.makeCellGrid(0, %this.pairWidth);
	%chain.add(%grid);
	%this.addFieldRow(%grid, "tooltipWidth", %this.spec.labelFor("tooltipWidth"), "number", "");
	%this.addFieldRow(%grid, "hovertime", %this.spec.labelFor("hovertime"), "number", "");
}

// The static rows of a list box or a drop down, directly under the text section
// because they are what the control says: a list's rows are its caption. Built
// once and shown per class rather than rebuilt with the class sections, for the
// reason the header comment gives -- and it stays out of panelList and out of
// the row registry, because what it holds is not a field.
function GuiEditorInspectorPane::buildItemsSection(%this)
{
	%this.itemsPanel = %this.makeSectionPanel("Items");
	%this.add(%this.itemsPanel);

	%this.itemsBlock = new GuiChainCtrl()
	{
		class = "GuiEditorItemsBlock";
		HorizSizing = "width";
		Position = "0 24";
		Extent = %this.paneWidth SPC 4;
		IsVertical = true;
		ChildSpacing = 4;
		blockWidth = %this.paneWidth;
		pane = %this;
	};
	%this.itemsPanel.add(%this.itemsBlock);
	%this.itemsBlock.build();
}

// A row was added or removed, so the section is a different height. Same shape
// as onDynamicFieldsChanged, and for the same reason.
function GuiEditorInspectorPane::onItemsChanged(%this)
{
	%this.itemsBlock.resize(0, 24, %this.paneWidth, getWord(%this.itemsBlock.getExtent(), 1));
	%this.forceLayout();
}

function GuiEditorInspectorPane::makeTextBlock(%this, %parent, %y)
{
	%block = new GuiChainCtrl()
	{
		class = "GuiEditorTextBlock";
		HorizSizing = "width";
		Position = "0" SPC %y;
		Extent = %this.paneWidth SPC 4;
		IsVertical = true;
		ChildSpacing = 2;
		blockWidth = %this.paneWidth;
		pane = %this;
		spec = %this.spec;
	};
	%parent.add(%block);
	%block.build();
	return %block;
}

function GuiEditorInspectorPane::buildSection(%this, %key, %title, %fields, %cellW)
{
	%panel = %this.makeSectionPanel(%title);
	%this.add(%panel);

	%grid = %this.makeCellGrid(24, %cellW);
	%panel.add(%grid);

	%this.panel[%key] = %panel;
	%this.panelFields[%key] = %fields;
	%this.panelList = (%this.panelList $= "") ? %key : (%this.panelList SPC %key);

	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.addFieldRow(%grid, %field, %this.spec.labelFor(%field),
			%this.sharedKindFor(%field), %this.sharedEnumItemsFor(%field));
	}
}

// Build a row without claiming a name for it. Two rows here are not fields of
// the control -- the Category picker, and the text block's copy in whichever of
// its two homes is not in use -- so they must stay out of the registry the
// filter and the value loop walk.
function GuiEditorInspectorPane::makeFieldRow(%this, %container, %field, %label, %kind, %enumItems)
{
	%row = new GuiControl()
	{
		class = "EditorFieldRow";

		// A grid resizes every cell it lays out, which makes the flag moot there;
		// a chain does not, so a row in one follows the pane's width from here.
		HorizSizing = "width";
		Position = "0 0";
		Extent = getWord(%container.getExtent(), 0) SPC 48;
		fieldName = %field;
		labelText = %label;
		kind = %kind;
		enumItems = %enumItems;
		owner = %this;
	};
	%container.add(%row);
	%row.build();

	// This pane has no reset-to-default, so the row's reset button -- which
	// means "back to the theme's stamped value" and has no analogue here --
	// never appears. The one exception is the font color row, where the button
	// means "stop overriding the profile" and the text block asks for it back.
	%row.resetButton.setVisible(false);
	return %row;
}

function GuiEditorInspectorPane::addFieldRow(%this, %container, %field, %label, %kind, %enumItems)
{
	%row = %this.makeFieldRow(%container, %field, %label, %kind, %enumItems);

	%this.row[%field] = %row;
	%this.rowFields = (%this.rowFields $= "") ? %field : (%this.rowFields SPC %field);
	return %row;
}

// Forget rows that are about to be deleted, so a later refresh does not reach
// through a dangling handle.
function GuiEditorInspectorPane::clearRows(%this, %fields)
{
	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.row[%field] = "";
		%this.rowFields = trim(strreplace(" " @ %this.rowFields @ " ", " " @ %field @ " ", " "));
	}
}

//-----------------------------------------------------------------------------
// Field presentation. The shared rows are built before any control is selected,
// so their kinds come from a small table; a class row can ask the control
// itself, which is always more accurate.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::sharedKindFor(%this, %field)
{
	switch$(%field)
	{
		case "MinExtent": return "point";
		case "isContainer" or "textWrap" or "textExtend" or "overrideFontColor" or "useInput": return "bool";
		case "tooltipWidth" or "hovertime": return "number";

		// A multiplier on the profile's font size, so 1.25 is an ordinary value
		// for it and a whole-number row would round every one of them away.
		case "fontSizeAdjust": return "decimal";
		case "fontColor": return "color";
		case "align" or "vAlign" or "textRotation": return "enum";
		case "easeFillColorHL" or "easeFillColorSL": return "enum";
		case "easeTimeFillColorHL" or "easeTimeFillColorSL": return "number";
	}
	return "text";
}

function GuiEditorInspectorPane::sharedEnumItemsFor(%this, %field)
{
	switch$(%field)
	{
		// "default" is offered now that the engine's tables expose it -- it is
		// the value a control starts on, meaning "inherit the profile's".
		case "align": return "default" TAB "left" TAB "center" TAB "right";
		case "vAlign": return "default" TAB "top" TAB "middle" TAB "bottom";
		case "textRotation": return "none" TAB "ccw" TAB "cw";
		case "easeFillColorHL" or "easeFillColorSL": return %this.easingItems();
	}
	return "";
}

// The easing names the engine offers, taken from gEasingTable in guiControl.cc.
function GuiEditorInspectorPane::easingItems(%this)
{
	return "Linear" TAB "EaseIn" TAB "EaseOut" TAB "EaseInOut";
}

// A class row can read its own type off the control, which is exact.
function GuiEditorInspectorPane::kindFor(%this, %ctrl, %field)
{
	%kind = %this.spec.kindForType(%ctrl.getFieldType(%field));

	// A profile slot is a short list of the theme's members for its category,
	// not a free-text name.
	if(%kind $= "profile")
	{
		return "dropdown";
	}
	return %kind;
}

// An enum's legal values are not exposed to script, so the ones that reach a
// class row are listed here. Anything missing falls back to a text box, which
// still edits the field correctly -- it just does not offer the choices.
function GuiEditorInspectorPane::enumItemsFor(%this, %ctrl, %field)
{
	switch$(%field)
	{
		// The anchor names, not the originals: these say which edge stays put
		// rather than which one moves. The old set still loads but is never
		// offered (guiControl.cc).
		case "HorizSizing": return "anchorLeft" TAB "anchorRight" TAB "width" TAB "center" TAB "scale" TAB "fill";
		case "VertSizing": return "anchorTop" TAB "anchorBottom" TAB "height" TAB "center" TAB "scale" TAB "fill";
		case "hScrollBar" or "vScrollBar": return "alwaysOn" TAB "alwaysOff" TAB "dynamic";
		case "CellModeX" or "CellModeY": return "absolute" TAB "variable";
		case "OrderMode": return "lrtb" TAB "tblr";
		case "TabPosition": return "Top" TAB "Bottom";
		case "DisplayMode": return "Dropper" TAB "Pallet" TAB "BlendRange" TAB "HueRange" TAB "AlphaRange";
		case "valueMode": return "RGB" TAB "HSB" TAB "Hex";
		case "inputMode": return "AllText" TAB "Number" TAB "Decimal" TAB "AlphaNumeric";
	}
	return "";
}

//-----------------------------------------------------------------------------
// Binding.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::bind(%this, %ctrl)
{
	if(!isObject(%ctrl))
	{
		%this.unbind();
		return;
	}

	// The sizing stash belongs to whichever control it was taken from. Moving
	// the selection abandons it rather than carrying a stale position onto
	// something else -- it exists only so that clicking through the modes on
	// one control can be undone.
	if(%ctrl != %this.target)
	{
		%this.clearStash("h");
		%this.clearStash("v");
	}

	%this.target = %ctrl;
	%class = %ctrl.getClassName();

	if(%class !$= %this.boundClass)
	{
		%this.buildClassSections(%class);
		%this.boundClass = %class;
	}

	// Always, not only on a class change: the header's value block depends on
	// the control as well as its class (a sprite shows the source it is using).
	%this.header.bindClass(%ctrl, %class);
	%this.buildVariantsSection();
	%this.dynamicFields.bind(%ctrl);
	%this.itemsBlock.bind(%this.spec.hasItemList(%class) ? %ctrl : "");

	%this.applyFilter();
	%this.refresh();
	%this.forceLayout();
	%this.setVisible(true);
}

// Nothing selected. The blocks keep their rows -- rebuilding them is what this
// pane goes out of its way to avoid -- and the whole pane simply stops drawing,
// so no stale values are left on show.
function GuiEditorInspectorPane::unbind(%this)
{
	%this.target = "";
	%this.clearStash("h");
	%this.clearStash("v");
	%this.setVisible(false);
}

// Rebuild the sections that belong to this class alone. The shared sections and
// everything in the header shell are left standing.
function GuiEditorInspectorPane::buildClassSections(%this, %class)
{
	%count = getWordCount(%this.classPanels);
	for(%i = 0; %i < %count; %i++)
	{
		%key = getWord(%this.classPanels, %i);
		%this.clearRows(%this.panelFields[%key]);
		%this.panel[%key] = "";
		%this.panelFields[%key] = "";
	}
	%this.classChain.deleteObjects();
	%this.classPanels = "";

	%keys = %this.spec.sectionKeys(%class);
	%keyCount = getWordCount(%keys);
	for(%i = 0; %i < %keyCount; %i++)
	{
		%key = getWord(%keys, %i);
		%this.buildClassSection(%class, %key);
	}

	// A class the table has never heard of gets everything it registers that is
	// not already on show, so an uncovered control degrades to roughly what the
	// native inspector did rather than to nothing.
	if(!%this.spec.isKnownClass(%class))
	{
		%this.buildOtherSection(%class);
	}
}

function GuiEditorInspectorPane::buildClassSection(%this, %class, %key)
{
	%fields = %this.spec.sectionFields(%class, %key);
	%panel = %this.makeSectionPanel(%this.spec.sectionTitle(%class, %key));
	%this.classChain.add(%panel);

	%grid = %this.makeCellGrid(24);
	%panel.add(%grid);

	%this.panel[%key] = %panel;
	%this.panelFields[%key] = %fields;
	%this.classPanels = (%this.classPanels $= "") ? %key : (%this.classPanels SPC %key);

	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.addFieldRow(%grid, %field, %this.spec.labelFor(%field),
			%this.kindFor(%this.target, %field),
			%this.enumItemsFor(%this.target, %field));
	}
}

// Everything the control registers that nothing above has claimed. Uses the
// control's own field list, so it needs no knowledge of the class at all.
function GuiEditorInspectorPane::buildOtherSection(%this, %class)
{
	%claimed = %this.rowFields SPC %this.spec.geometryFields() SPC
		%this.spec.deadFields() SPC %this.spec.runtimeToggles() SPC
		%this.spec.editorToggles() SPC "name Profile";

	%fields = "";
	%count = %this.target.getFieldCount();
	for(%i = 0; %i < %count; %i++)
	{
		%field = %this.target.getField(%i);
		if(%this.spec.listHas(%claimed, %field))
		{
			continue;
		}
		if(%this.spec.kindForType(%this.target.getFieldType(%field)) $= "hidden")
		{
			continue;
		}
		%fields = (%fields $= "") ? %field : (%fields SPC %field);
	}

	if(%fields $= "")
	{
		return;
	}

	%panel = %this.makeSectionPanel("Other");
	%this.classChain.add(%panel);
	%grid = %this.makeCellGrid(24);
	%panel.add(%grid);

	%this.panel["Other"] = %panel;
	%this.panelFields["Other"] = %fields;
	%this.classPanels = (%this.classPanels $= "") ? "Other" : (%this.classPanels SPC "Other");

	%fieldCount = getWordCount(%fields);
	for(%i = 0; %i < %fieldCount; %i++)
	{
		%field = getWord(%fields, %i);
		%this.addFieldRow(%grid, %field, %this.spec.labelFor(%field),
			%this.kindFor(%this.target, %field),
			%this.enumItemsFor(%this.target, %field));
	}
}

//-----------------------------------------------------------------------------
// Variants: the control's secondary profile slots, shown only where there is
// something to choose between.
//
// Every slot other than Profile itself -- contentProfile, thumbProfile,
// closeButtonProfile and the rest -- is assigned by GuiEditorThemeApplier from
// the Gui's theme, and in the ordinary case there is exactly one profile in the
// theme for that slot's category. Showing a dropdown with one entry in it is
// noise, so the row only exists once a second candidate does.
//
// The count and the contents are deliberately different sets. An uncategorised
// standalone -- "Any" in the Profile Editor -- is offered in a row that already
// exists but never causes one to appear, because otherwise a single "Any"
// profile would sprout a Variants row on every slot of every control, which is
// exactly the complexity this pane exists to remove.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::buildVariantsSection(%this)
{
	if(isObject(%this.panel["Variants"]))
	{
		%this.clearRows(%this.panelFields["Variants"]);
		%this.panel["Variants"] = "";
		%this.panelFields["Variants"] = "";
		%this.panelList = trim(strreplace(" " @ %this.panelList @ " ", " Variants ", " "));
	}
	%this.variantsChain.deleteObjects();

	%fields = %this.variantSlots(%this.target);
	if(%fields $= "")
	{
		return;
	}

	%panel = %this.makeSectionPanel("Variants");
	%this.variantsChain.add(%panel);
	%grid = %this.makeCellGrid(24);
	%panel.add(%grid);

	%this.panel["Variants"] = %panel;
	%this.panelFields["Variants"] = %fields;
	%this.panelList = %this.panelList SPC "Variants";

	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.addFieldRow(%grid, %field, %this.spec.labelFor(%field), "dropdown", "");
	}
}

// The slots worth showing: every TypeGuiProfile field except the control's own
// Profile, which the header already carries, and only where the narrow
// candidate set has more than one member.
//
// Cursor slots join on the same terms. A themed Gui gets its cursors filled by
// GuiEditorThemeApplier without being asked, and a theme almost always offers
// exactly one cursor per category -- so a row appears only once the theme holds
// a second cursor for that job and there is genuinely a choice to make.
function GuiEditorInspectorPane::variantSlots(%this, %ctrl)
{
	if(!isObject(%ctrl) || %this.spec.hasFlag(%ctrl.getClassName(), "bare"))
	{
		return "";
	}

	%fields = "";
	%count = %ctrl.getFieldCount();
	for(%i = 0; %i < %count; %i++)
	{
		%field = %ctrl.getField(%i);
		%type = %ctrl.getFieldType(%field);

		if(%type $= "GuiProfile" && %field !$= "Profile")
		{
			if(getWordCount(%this.profileAnchors(%ctrl, %field)) > 1)
			{
				%fields = (%fields $= "") ? %field : (%fields SPC %field);
			}
		}
		else if(%type $= "GuiCursor")
		{
			if(getWordCount(%this.cursorAnchors(%ctrl, %field)) > 1)
			{
				%fields = (%fields $= "") ? %field : (%fields SPC %field);
			}
		}
	}
	return %fields;
}

// A cursor slot's candidates: the active theme's members of the slot's cursor
// category, plus whatever the slot holds now. There is no stand-alone cursor to
// merge in - a cursor belongs to a theme or to nothing.
function GuiEditorInspectorPane::cursorAnchors(%this, %ctrl, %field)
{
	%category = GuiEditor.themeApplier.cursorCategoryForField(%field);
	if(%category $= "")
	{
		return "";
	}

	%theme = GuiEditor.themeByName(GuiEditor.themeName);
	%list = isObject(%theme) ? %theme.getCursors(%category) : "";

	%current = GuiEditor.themeApplier.fieldCursor(%ctrl, %field);
	if(isObject(%current))
	{
		%list = %this.addUnique(%list, %current);
	}

	return %list;
}

// Why does this control show the Variants rows it shows? Type
//
//     GuiEditor.inspectorWindow.pane.dumpSlots();
//
// into the console with something selected. For each profile slot it prints the
// category, what the slot holds, and every candidate with the theme that owns
// it -- which is the only way to tell a theme member from a standalone that
// happens to share a name, and the fastest way to find out why a row appeared.
function GuiEditorInspectorPane::dumpSlots(%this)
{
	if(!isObject(%this.target))
	{
		echo("dumpSlots: nothing selected.");
		return;
	}

	%applier = GuiEditor.themeApplier;
	%library = GuiEditor.themeLibrary;
	%themes = %library.getThemes();

	echo("dumpSlots: " @ %this.target.getClassName() @ " '" @ %this.target.getName() @
		"', active theme '" @ GuiEditor.themeName @ "', " @ getWordCount(%themes) @ " theme(s) loaded.");

	for(%i = 0; %i < getWordCount(%themes); %i++)
	{
		%t = getWord(%themes, %i);
		echo("  theme " @ %t.getName() @ " (" @ %t @ ") members=" @ %t.getProfileCount());
	}
	for(%i = 0; %i < %library.standaloneFolder.getCount(); %i++)
	{
		%p = %library.standaloneFolder.getObject(%i).target;
		if(isObject(%p))
		{
			echo("  standalone " @ %p.getName() @ " (" @ %p @ ") category='" @ %p.category @ "'");
		}
	}

	// themeOf needs the applier's theme list, which it only holds between these.
	%applier.beginApply();
	%count = %this.target.getFieldCount();
	for(%i = 0; %i < %count; %i++)
	{
		%field = %this.target.getField(%i);
		if(%this.target.getFieldType(%field) !$= "GuiProfile")
		{
			continue;
		}

		%cat = %this.categoryForSlot(%this.target, %field);
		%cur = %applier.fieldProfile(%this.target, %field);
		%anchors = %this.profileAnchors(%this.target, %field);

		echo("  slot " @ %field @ " category='" @ %cat @ "' current=" @
			(isObject(%cur) ? %cur.getName() @ "(" @ %cur @ ")" : "none") @
			" anchors=" @ getWordCount(%anchors) @
			(getWordCount(%anchors) > 1 ? "  <-- ROW SHOWN" : ""));

		for(%a = 0; %a < getWordCount(%anchors); %a++)
		{
			%p = getWord(%anchors, %a);
			%owner = %applier.themeOf(%p);
			echo("      " @ %p.getName() @ " (" @ %p @ ") from " @
				(isObject(%owner) ? "theme " @ %owner.getName() : "standalone/unowned"));
		}
	}
	%applier.endApply();
}

//-----------------------------------------------------------------------------
// Filtering. Nothing here creates or deletes a control -- it only decides what
// is visible and what is inert.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::applyFilter(%this)
{
	%spec = %this.spec;
	%class = %this.boundClass;

	// Which copy of the text block this class uses, before anything reads a row:
	// its three field rows are shared names pointing at whichever block is in
	// play, so the mapping has to be right before the filter walks them.
	%home = %spec.textBlockHome(%class);
	%block = %this.activeTextBlock();
	%this.mapTextRows(%block);

	%count = getWordCount(%this.rowFields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%this.rowFields, %i);
		%row = %this.row[%field];
		if(isObject(%row))
		{
			%row.setVisible(%spec.isFieldVisible(%class, %field));
		}
	}

	// isContainer is the engine's answer rather than the table's: a control
	// that cannot draw children has the field forced false, so the switch would
	// be wired to nothing.
	// isContainer moved to the header's icon row, which decides its own
	// visibility from the same rule -- see GuiEditorHeaderBlock::bindClass.

	// The block's own parts -- the caption, the two flags and the two alignments
	// -- are its to filter. The header shows or hides its copy from the same
	// answer (GuiEditorHeaderBlock::bindClass).
	%this.textPanel.setVisible(%home $= "section");
	if(isObject(%block))
	{
		%block.bindClass(%this.target, %class);
	}

	// A section with nothing left to show gets out of the way entirely.
	//
	// The class sections as well as the shared ones. They were left out, and a
	// class section whose every field turned out to be hidden stayed on screen as
	// a header that could not be opened: the grid drops hidden cells, so it
	// measures zero high, and GuiPanelCtrl's expanded extent comes out equal to
	// its collapsed one. Clicking it flipped the arrow and nothing else.
	%panels = %this.panelList SPC %this.classPanels;
	%count = getWordCount(%panels);
	for(%i = 0; %i < %count; %i++)
	{
		%key = getWord(%panels, %i);
		%this.panel[%key].setVisible(%this.anyRowVisible(%this.panelFields[%key]));
	}

	// The Add row is always worth showing, so the section stays open whenever a
	// control is bound at all -- unlike the others, an empty one is still the
	// only way to put a field on the control.
	%this.dynamicPanel.setVisible(isObject(%this.target));

	// Same for Items, on the two classes that have any: an empty list is exactly
	// the case the section exists to fix.
	%this.itemsPanel.setVisible(isObject(%this.target) && %spec.hasItemList(%class));

	%this.header.applyGeometryMode(%spec.geometryModeOf(%this.target));
}

// A field was added or removed, so the section's height changed under the
// chain. Nothing above it moved, but the panel has to be told to re-measure.
function GuiEditorInspectorPane::onDynamicFieldsChanged(%this)
{
	%this.dynamicFields.resize(0, 24, %this.paneWidth, getWord(%this.dynamicFields.getExtent(), 1));
	%this.forceLayout();
}

// Which of the two text blocks the bound class uses, or nothing where none of
// it applies (a sprite draws no text and reads no font).
function GuiEditorInspectorPane::activeTextBlock(%this)
{
	switch$(%this.spec.textBlockHome(%this.boundClass))
	{
		case "header": return %this.header.textBlock;
		case "section": return %this.sectionText;
	}
	return "";
}

// Point the three shared names at the block in use. Without this the second
// block to be built would own the registry and the first would never load or
// filter -- the rows would be on screen holding whatever the last selection of
// the other kind left in them.
function GuiEditorInspectorPane::mapTextRows(%this, %block)
{
	%fields = %this.spec.textBlockRowFields();
	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%fields, %i);
		%this.row[%field] = isObject(%block) ? %block.row[%field] : "";
	}
}

function GuiEditorInspectorPane::anyRowVisible(%this, %fields)
{
	%count = getWordCount(%fields);
	for(%i = 0; %i < %count; %i++)
	{
		%row = %this.row[getWord(%fields, %i)];
		if(isObject(%row) && %row.isVisible())
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Loading values. The populating guard keeps every setText / setColorI /
// setStateOn from echoing straight back through the commits.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::refresh(%this)
{
	if(!isObject(%this.target))
	{
		return;
	}

	%this.populating = true;

	// Candidates before values: a drop-down row keeps a selection that is not
	// in its list by inserting it, so filling the list afterwards would drop
	// what the control actually wears.
	%this.refreshProfileChoices();

	%count = getWordCount(%this.rowFields);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%this.rowFields, %i);
		%row = %this.row[%field];

		// Profile slots were just loaded by name above. Reading one back off
		// the control here would undo that: the field answers with whatever
		// name it holds, and a profile created this session has one the Sim
		// never registered.
		//
		// fontColor is skipped for a different reason: what its swatch shows is
		// the profile's color while the control is not overriding it, which is
		// not what the field holds. The block works that out.
		if(!isObject(%row) || %this.isProfileSlot(%field) || %field $= "fontColor")
		{
			continue;
		}
		%row.setValue(%this.readField(%field));
	}

	// What the text block holds beyond its three field rows: two segmented rows,
	// two toggles, and the font color swatch.
	%block = %this.activeTextBlock();
	if(isObject(%block))
	{
		%block.load(%this.target);
	}

	// A menu item's fields are none of them in the registry above -- the block
	// keeps them out of it deliberately, so the bare rule cannot hide them -- so
	// it reloads itself. This is the path an undo comes back through.
	if(%this.header.menuItemBlock.isVisible())
	{
		%this.header.menuItemBlock.bind(%this.target);
	}

	// The static rows, for the same reason: they are not fields, so nothing above
	// reaches them, and a replayed step can put back a caption, a switch or the
	// whole order.
	if(%this.itemsPanel.isVisible())
	{
		%this.itemsBlock.refresh();
	}

	%this.refreshToggles();
	%this.header.anchorPicker.readEnums(
		%this.target.getFieldValue("HorizSizing"),
		%this.target.getFieldValue("VertSizing"));

	%this.populating = false;
}

// The anchor picker moved. Both enums go in: one click can change either, and
// clearing a Fill hands the axis back to its pins, which the other field has to
// hear about too.
//
// Then the control is laid out immediately. Center and fill are positions the
// control should always be in rather than reactions to a size change, so
// leaving them until the next parent resize means picking one appears to do
// nothing -- the control sat still until you nudged it.
//
// Because those two overwrite geometry the user typed, the position and extent
// they are about to destroy are kept first, per axis, and handed back when the
// axis returns to a mode that does not own them. That makes clicking through
// Fill, Scale and back a lossless way to look at the options. The stash is
// deliberately shallow: it lives until the selection changes and is then gone,
// so it never has to be reconciled with anything else that moves the control.
function GuiEditorInspectorPane::onSizingChanged(%this, %horiz, %vert)
{
	if(%this.populating || !isObject(%this.target))
	{
		return;
	}

	// What the whole change starts from. Setting an axis to center or fill hands
	// that axis's geometry to the engine, which lays the control out then and
	// there -- so the enum and the move it causes are one change, and undoing
	// the enum alone would leave the control sitting where centring put it.
	%recorder = GuiEditor.undoRecorder;
	%horizBefore = %this.target.getFieldValue("HorizSizing");
	%vertBefore = %this.target.getFieldValue("VertSizing");
	%posBefore = %this.target.getPosition();
	%extentBefore = %this.target.getExtent();

	// Recorded afterwards as the one net change, rather than as each write the
	// body makes: leaving or entering a mode writes the geometry twice, and an
	// undo replaying those in reverse would stop on the intermediate values.
	%recorder.suspend();

	// Order matters, and got this wrong once. The stash has to be taken while
	// the geometry is still the user's, which is now -- writing the enum alone
	// moves nothing. The restore has to wait until AFTER the enum is written,
	// because a control still set to center or fill overrides any position or
	// extent written to it: the write appeared to land and read straight back
	// out as the centred value.
	%this.stashIfNeeded("h", %horiz);
	%this.stashIfNeeded("v", %vert);

	%this.writeField("HorizSizing", %horiz);
	%this.writeField("VertSizing", %vert);

	// A no-op for every mode that needs a size delta, which is what we want:
	// they have nothing to react to. Center and fill apply now.
	%this.target.applySizing();

	%this.restoreIfNeeded("h", %horiz);
	%this.restoreIfNeeded("v", %vert);

	%recorder.resume();

	// Geometry first and the enums last, because the ops replay in reverse for
	// an undo and the enums have to be off center or fill before the position
	// and extent are written back -- the same rule the body above follows.
	%recorder.begin("Change Sizing", "");
	%recorder.recordField(%this.target, "Position", %posBefore, %this.target.getPosition(), false);
	%recorder.recordField(%this.target, "Extent", %extentBefore, %this.target.getExtent(), false);
	%recorder.recordField(%this.target, "HorizSizing", %horizBefore,
		%this.target.getFieldValue("HorizSizing"), false);
	%recorder.recordField(%this.target, "VertSizing", %vertBefore,
		%this.target.getFieldValue("VertSizing"), false);
	%recorder.end();

	%this.refreshGeometry();
	%this.afterCommit();
}

// The two modes that overwrite what the user set. Center takes the position;
// fill takes the position and the extent.
function GuiEditorInspectorPane::sizingOwnsGeometry(%this, %mode)
{
	return %mode $= "center" || %mode $= "fill";
}

// Entering one of those: keep what it is about to overwrite, unless something
// is already kept -- the first value is the one worth returning to.
function GuiEditorInspectorPane::stashIfNeeded(%this, %axis, %mode)
{
	if(!%this.sizingOwnsGeometry(%mode) || %this.stashed[%axis] !$= "")
	{
		return;
	}

	%this.stashed[%axis] = true;
	%this.stashPos[%axis] = %this.target.getPosition();
	%this.stashExtent[%axis] = %this.target.getExtent();
}

// Leaving one: give back what it took, on this axis only -- the other may still
// be filled, and restoring both would undo it.
function GuiEditorInspectorPane::restoreIfNeeded(%this, %axis, %mode)
{
	if(%this.sizingOwnsGeometry(%mode) || %this.stashed[%axis] $= "")
	{
		return;
	}

	%pos = %this.target.getPosition();
	%extent = %this.target.getExtent();
	if(%axis $= "h")
	{
		%pos = getWord(%this.stashPos[%axis], 0) SPC getWord(%pos, 1);
		%extent = getWord(%this.stashExtent[%axis], 0) SPC getWord(%extent, 1);
	}
	else
	{
		%pos = getWord(%pos, 0) SPC getWord(%this.stashPos[%axis], 1);
		%extent = getWord(%extent, 0) SPC getWord(%this.stashExtent[%axis], 1);
	}

	// Through the fields, not setPosition/setExtent. "scale" does not recompute
	// its proportion every layout -- relPosBatteryH caches it in
	// mStoredRelativePosH and keeps using it until resetStoredRelPos runs, and
	// the only things that call that are the Position and Extent field setters
	// (guiControl.h setPositionFn / setExtentFn). Restoring with the console
	// methods left the proportion captured while the control was filled, so the
	// next layout stretched it straight back out again.
	%this.writeField("Position", %pos);
	%this.writeField("Extent", %extent);
	%this.clearStash(%axis);
}

function GuiEditorInspectorPane::clearStash(%this, %axis)
{
	%this.stashed[%axis] = "";
	%this.stashPos[%axis] = "";
	%this.stashExtent[%axis] = "";
}

// Position and extent move without the pane being rebuilt -- dragging a control
// on the canvas ends in an Edit event -- so this is the cheap path that reloads
// values and touches nothing else.
function GuiEditorInspectorPane::refreshGeometry(%this)
{
	if(!isObject(%this.target))
	{
		return;
	}

	%this.populating = true;
	%this.header.positionRow.setValue(%this.target.getPosition());
	%this.header.extentRow.setValue(%this.target.getExtent());
	%this.populating = false;
}

function GuiEditorInspectorPane::readField(%this, %field)
{
	// getFieldValue answers "" for a name in editor mode, where assignName
	// stashes the name rather than registering it, so ask the object instead.
	if(%field $= "name")
	{
		return %this.target.getName();
	}
	return %this.target.getFieldValue(%field);
}

function GuiEditorInspectorPane::writeField(%this, %field, %value)
{
	// Through the recorder, which does the setEditFieldValue itself. Every write
	// the pane makes is an edit the user can take back, and routing them all
	// through one place is what makes that true without each caller remembering
	// to say so.
	//
	// (setEditFieldValue rather than a plain assignment brackets the write with
	// inspectPreApply / inspectPostApply, which is what lets a control react to
	// being re-profiled or resized from here.)
	GuiEditor.undoRecorder.writeField(%this.target, %field, %value);
}

function GuiEditorInspectorPane::refreshToggles(%this)
{
	%header = %this.header;

	// hidden and locked are not here. They are editor working state, never
	// written to the document, and they live in the Explorer tree's two columns.
	%header.visibleButton.setValue(%this.target.Visible);
	%header.activeButton.setValue(%this.target.Active);
	%header.inputButton.setValue(%this.target.useInput);
	%header.containerButton.setValue(%this.target.isContainer);

	%header.refreshWindowToggles(%this.target);
}

// A segmented row in the header picked a value. Same contract as the toggles:
// the widget holds the choice, the pane does the writing.
function GuiEditorInspectorPane::onHeaderChoiceChanged(%this, %field, %value)
{
	if(%this.populating || !isObject(%this.target))
	{
		return;
	}

	%this.writeField(%field, %value);
	%this.afterCommit();
}

// A toggle reports the click and the value it flipped itself to, and the pane
// does the writing -- so that every write to the control still goes through one
// place, and a new toggle needs no case of its own here.
function GuiEditorInspectorPane::onToggleChanged(%this, %field, %value)
{
	if(%this.populating || !isObject(%this.target))
	{
		return;
	}

	%this.writeField(%field, %value);
	%this.refreshToggles();
	%this.afterCommit();
}

//-----------------------------------------------------------------------------
// Profile slots. A slot is a choice among the theme's members for its category,
// never a list of every profile in the sim -- which is what the native
// inspector offered, applied by name, silently failing for anything made this
// session (editor mode does not register names).
//-----------------------------------------------------------------------------

// The narrow set, which decides whether a slot is worth showing at all: this
// theme's members of the slot's category, any standalone profile stamped for
// that category, and whatever the slot currently holds. An uncategorised
// standalone is deliberately absent -- one of those would otherwise make a
// Variants row appear on every slot of every control.
//
// The current value counts so that a slot wearing something the theme does not
// offer is visible and changeable rather than silently stuck. In the ordinary
// case it is already one of the theme's members and dedupes away.
function GuiEditorInspectorPane::profileAnchors(%this, %ctrl, %field)
{
	%category = %this.categoryForSlot(%ctrl, %field);
	if(%category $= "")
	{
		return "";
	}

	return %this.profileAnchorsFor(%category,
		GuiEditor.themeApplier.fieldProfile(%ctrl, %field));
}

// The same set for a category named outright. Changing a control's category has
// to ask what the category it is moving TO can offer, which it cannot get by
// asking the control -- the control still wears the old one.
function GuiEditorInspectorPane::profileAnchorsFor(%this, %category, %current)
{
	%library = GuiEditor.themeLibrary;
	%theme = GuiEditor.themeByName(GuiEditor.themeName);

	%list = isObject(%theme) ? %theme.getProfiles(%category) : "";
	%list = %this.addUnique(%list, %library.getStandaloneProfiles(%category));

	if(isObject(%current))
	{
		%list = %this.addUnique(%list, %current);
	}

	return %list;
}

// The wider set, which is what the drop-down actually offers once it exists.
// "Any" means usable as any control's main profile, not usable in any slot.
function GuiEditorInspectorPane::profileOptions(%this, %ctrl, %field)
{
	%list = %this.profileAnchors(%ctrl, %field);
	return %this.addUnique(%list, GuiEditor.themeLibrary.getStandaloneProfiles(""));
}

function GuiEditorInspectorPane::categoryForSlot(%this, %ctrl, %field)
{
	return GuiEditor.themeApplier.categoryForField(%field, %this.currentCategory(%ctrl));
}

//-----------------------------------------------------------------------------
// The control's own category. For every class but one this is the class's
// answer and there is nothing to ask: a check box wants a CheckBox profile.
//
// A bare GuiControl is the exception. The applier guesses at drop time from
// what the control holds -- root, or text, or neither -- and typing a caption
// afterwards re-runs nothing, so the guess needs to be correctable. What makes
// that work with no new state is that a profile carries the category it was
// stamped for: the control wearing a Label profile IS the record that it was
// told to be a Label, and it survives being saved, reloaded, and re-themed
// (GuiEditorThemeApplier::applyToControl carries it across a theme switch).
//
// Only a category the class actually offers counts. Anything else -- a
// hand-written Gui wearing a ListBox profile on a plain GuiControl -- falls
// back to the guess, and the profile it holds still shows up as a candidate.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::currentCategory(%this, %ctrl)
{
	%applier = GuiEditor.themeApplier;
	%isRoot = (%ctrl.getParent() == %applier.rootContainer);
	%guess = %applier.categoryForControl(%ctrl, %isRoot);

	%choices = %this.spec.categoryChoices(%ctrl.getClassName());
	if(%choices $= "")
	{
		return %guess;
	}

	%current = %applier.fieldProfile(%ctrl, "Profile");
	if(isObject(%current))
	{
		%match = %this.spec.matchCategory(%choices, %current.category);
		if(%match !$= "")
		{
			return %match;
		}
	}

	return %guess;
}

// Move the control onto a category: its main profile becomes that category's,
// so the picker and the profile can never disagree. The theme's own member
// first, then any standalone stamped for the category, and if the category is
// empty the list is refilled and the profile left alone rather than cleared.
function GuiEditorInspectorPane::setCategory(%this, %category)
{
	if(!isObject(%this.target) || %category $= "")
	{
		return;
	}

	%theme = GuiEditor.themeByName(GuiEditor.themeName);
	%profile = isObject(%theme) ? %theme.getProfile(%category) : 0;
	if(!isObject(%profile))
	{
		%profile = getWord(%this.profileAnchorsFor(%category, 0), 0);
	}

	if(isObject(%profile))
	{
		// By id, not by name: a profile made this session carries a name the Sim
		// never registered, because the editor runs with assignName stashing
		// names rather than adding them.
		GuiEditor.undoRecorder.begin("Change Category", "");
		%this.writeField("Profile", %profile.getId());
		GuiEditor.undoRecorder.end();
	}

	// Everything downstream of the profile moves with it -- which profiles the
	// slot now offers, and the font color the swatch falls back to.
	%this.refresh();
	%this.afterCommit();
}

function GuiEditorInspectorPane::addUnique(%this, %list, %additions)
{
	%count = getWordCount(%additions);
	for(%i = 0; %i < %count; %i++)
	{
		%item = getWord(%additions, %i);
		if(%item $= "" || %this.spec.listHas(%list, %item))
		{
			continue;
		}
		%list = (%list $= "") ? %item : (%list SPC %item);
	}
	return %list;
}

// Fill every profile drop-down -- the header's Profile and any Variants row --
// with its candidates and select what the control wears.
function GuiEditorInspectorPane::refreshProfileChoices(%this)
{
	if(!isObject(%this.target))
	{
		return;
	}

	if(%this.header.categoryRow.isVisible())
	{
		%this.fillCategoryRow();
	}

	if(%this.header.profileRow.isVisible())
	{
		%this.fillProfileRow(%this.header.profileRow, "Profile");
	}

	%slots = %this.panelFields["Variants"];
	%count = getWordCount(%slots);
	for(%i = 0; %i < %count; %i++)
	{
		%field = getWord(%slots, %i);
		%row = %this.row[%field];
		if(!isObject(%row))
		{
			continue;
		}

		if(%this.isCursorSlot(%field))
		{
			%this.fillCursorRow(%row, %field);
		}
		else
		{
			%this.fillProfileRow(%row, %field);
		}
	}
}

// A cursor slot's candidates, by name, exactly as a profile slot's: the name is
// the label and the commit looks it back up, because a cursor created this
// session has a name the Sim may not resolve.
function GuiEditorInspectorPane::fillCursorRow(%this, %row, %field)
{
	%ids = %this.cursorAnchors(%this.target, %field);
	%items = "";
	%count = getWordCount(%ids);
	for(%i = 0; %i < %count; %i++)
	{
		%cursor = getWord(%ids, %i);
		%name = %cursor.getName();
		if(%name $= "")
		{
			continue;
		}
		%items = (%items $= "") ? %name : (%items TAB %name);
		%this.cursorByName[%name] = %cursor;
	}

	%row.currentItem = "";
	%row.fillItems(%items);

	%current = GuiEditor.themeApplier.fieldCursor(%this.target, %field);
	%row.setValue(isObject(%current) ? %current.getName() : "");
}

// The categories this class may take, and which one it is on. Unlike a profile
// row these are plain strings, so the name in the list is the value.
function GuiEditorInspectorPane::fillCategoryRow(%this)
{
	%row = %this.header.categoryRow;
	%choices = %this.spec.categoryChoices(%this.boundClass);

	%items = "";
	%count = getWordCount(%choices);
	for(%i = 0; %i < %count; %i++)
	{
		%item = getWord(%choices, %i);
		%items = (%items $= "") ? %item : (%items TAB %item);
	}

	// Same reason as a profile row: the list is recomputed from scratch on every
	// bind, so a selection the new list does not hold is a ghost of the last one
	// rather than something worth preserving.
	%row.currentItem = "";
	%row.fillItems(%items);
	%row.setValue(%this.currentCategory(%this.target));
}

// One slot's candidates, listed by name. The name is only a label: a commit
// looks the choice back up and writes the id, because a profile made during
// this editor session has a name the Sim cannot resolve.
function GuiEditorInspectorPane::fillProfileRow(%this, %row, %field)
{
	%ids = %this.profileOptions(%this.target, %field);
	%items = "";
	%count = getWordCount(%ids);
	for(%i = 0; %i < %count; %i++)
	{
		%profile = getWord(%ids, %i);
		%name = %profile.getName();
		if(%name $= "")
		{
			continue;
		}
		%items = (%items $= "") ? %name : (%items TAB %name);
		%this.profileByName[%name] = %profile;
	}

	// Forget what the row was showing before refilling it. fillItems deliberately
	// preserves the current selection and re-inserts it when the new list does
	// not contain it -- right for the Profile Editor's font list, where a face
	// outside the directory must not be silently dropped, but wrong here: the
	// candidates are recomputed from scratch every bind, so a name that is no
	// longer one of them is a ghost of the last fill. It is how a profile from
	// the previous theme survived a Set Theme.
	%row.currentItem = "";
	%row.fillItems(%items);

	%current = GuiEditor.themeApplier.fieldProfile(%this.target, %field);
	%row.setValue(isObject(%current) ? %current.getName() : "");
}

//-----------------------------------------------------------------------------
// Commits. Every write to the control goes through here.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::onFieldRowCommit(%this, %row)
{
	if(%this.populating || !isObject(%this.target))
	{
		return;
	}

	%field = %row.fieldName;

	// The font color swatch is two fields, and the second of them is why the
	// changed test cannot come first: picking the color the profile already
	// draws in is a real edit when the override was off.
	if(%field $= "fontColor")
	{
		%this.commitFontColor(%row);
		return;
	}

	// The text box has been writing to the control on every keystroke, so the
	// changed test cannot come first here either -- by now the control already
	// says what the row says.
	if(%field $= "text")
	{
		%this.commitText(%row);
		return;
	}

	// A text box commits on blur, so most commits arrive from a field the user
	// only tabbed through. Writing one anyway would put an edit in the undo
	// record -- and mark the Gui dirty -- for something that never happened.
	if(!%row.hasChanged())
	{
		return;
	}

	// Category names no field on the control. It picks the control's Profile,
	// and has to be intercepted here or writeField would put a dynamic field
	// called "category" on it.
	if(%field $= "category")
	{
		%row.markClean();
		%this.setCategory(%row.getValue());
		return;
	}

	// A profile slot is chosen by name in the list but written by id: a profile
	// created this session carries a name the Sim cannot resolve, because the
	// editor runs with assignName stashing names rather than registering them.
	// GuiEditorThemeApplier::applyToControl writes ids for exactly this reason.
	if(%this.isProfileSlot(%field))
	{
		%profile = %this.profileByName[%row.getValue()];
		if(isObject(%profile))
		{
			%this.writeField(%field, %profile.getId());
		}
	}
	else if(%this.isCursorSlot(%field))
	{
		%cursor = %this.cursorByName[%row.getValue()];
		if(isObject(%cursor))
		{
			%this.writeField(%field, %cursor.getId());
		}
	}
	else
	{
		%this.writeField(%field, %row.getValue());
	}

	%row.markClean();

	// Changing a sprite's source swaps which fields the header shows, which
	// means deleting the row this commit arrived from. Deferred to the next
	// tick so the engine is not mid-dispatch on a control being freed.
	if(%this.boundClass $= "GuiSpriteCtrl" && %this.isSpriteSourceField(%field))
	{
		%this.schedule(0, "rebindDeferred");
	}

	%this.afterCommit();
}

function GuiEditorInspectorPane::isProfileSlot(%this, %field)
{
	return isObject(%this.target) &&
		%this.target.getFieldType(%field) $= "GuiProfile";
}

function GuiEditorInspectorPane::isCursorSlot(%this, %field)
{
	return isObject(%this.target) &&
		%this.target.getFieldType(%field) $= "GuiCursor";
}

function GuiEditorInspectorPane::isSpriteSourceField(%this, %field)
{
	return %this.spec.listHas("Image Animation Bitmap", %field);
}

function GuiEditorInspectorPane::rebindDeferred(%this)
{
	if(isObject(%this.target))
	{
		%this.bind(%this.target);
	}
}

//-----------------------------------------------------------------------------
// Text, which reaches the control twice: once per keystroke so the canvas keeps
// up, and once here so the change is recorded as the single edit it was.
//
// The pair matters because a field write is the editor's unit of change --
// inspectPreApply / inspectPostApply, and whatever an undo record is eventually
// hung on. Eleven keystrokes are one edit, not eleven, so the control is put
// back to what it held when the first key landed and the new value written over
// it once.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::commitText(%this, %row)
{
	%block = %this.activeTextBlock();

	// Nobody typed: an ordinary commit, from a box the user tabbed through or a
	// value that arrived from script. Or the selection moved while an edit was
	// open, in which case the stash belongs to a control this row is no longer
	// showing -- the typed text is already on it, so there is nothing to save.
	if(!isObject(%block) || !%block.typing || %block.typingTarget != %this.target)
	{
		if(%row.hasChanged())
		{
			%this.writeField("text", %row.getValue());
			%row.markClean();
			%this.afterCommit();
		}
		if(isObject(%block))
		{
			%block.endTyping();
		}
		return;
	}

	%value = %row.getValue();
	%before = %block.textBeforeEdit;
	%block.endTyping();
	%row.markClean();

	// strcmp, not $=: $= runs dStricmp, so retyping a caption in a different
	// case would read as no change at all.
	if(strcmp(%value, %before) == 0)
	{
		return;
	}

	%this.target.text = %before;
	%this.writeField("text", %value);
	%this.afterCommit();
}

//-----------------------------------------------------------------------------
// Font color, which is two fields wearing one widget. overrideFontColor is
// what decides whether fontColor is used at all (guiControl.cc renderText), and
// on its own it is a checkbox that does nothing visible -- so the swatch is
// both: picking a color turns the override on, and the row's reset button
// turns it off again. With it off the swatch shows the profile's own color,
// so the row always says what the control will actually draw in.
//-----------------------------------------------------------------------------

function GuiEditorInspectorPane::commitFontColor(%this, %row)
{
	// Nothing to do only when the color is unchanged AND the override was
	// already on. Picking the profile's exact color while it was off is the
	// user asking to pin that color down, which is a change to the control
	// even though the swatch looks the same.
	if(!%row.hasChanged() && %this.target.overrideFontColor)
	{
		return;
	}

	// Two fields, one edit.
	GuiEditor.undoRecorder.begin("Change Font Color", "");
	%this.writeField("fontColor", %row.getValue());
	%this.writeField("overrideFontColor", true);
	GuiEditor.undoRecorder.end();

	%row.markClean();
	%row.setOverridden(true);
	%this.afterCommit();
}

// The row widget's reset means "back to the theme's stamped value" everywhere
// else, which has no analogue for a control -- so every other row here hides
// the button. The font color row keeps it, meaning "stop overriding the
// profile's".
function GuiEditorInspectorPane::onFieldRowReset(%this, %row)
{
	if(%row.fieldName !$= "fontColor" || !isObject(%this.target))
	{
		return;
	}

	%this.writeField("overrideFontColor", false);

	%block = %this.activeTextBlock();
	if(isObject(%block))
	{
		%this.populating = true;
		%block.loadFontColor(%this.target);
		%this.populating = false;
	}

	%this.afterCommit();
}

// The two flags in the text block's caption row. They change the control's size
// rather than only its look -- textExtend grows the width when wrap is off and
// the height when it is on -- so the geometry rows are stale the moment either
// is written.
function GuiEditorInspectorPane::onTextFlagChanged(%this, %field, %value)
{
	if(%this.populating || !isObject(%this.target))
	{
		return;
	}

	%this.writeField(%field, %value);
	%this.refreshGeometry();
	%this.afterCommit();
}

// Everything a write has to tell the rest of the editor. The canvas has to
// redraw and the explorer tree may be showing the name that just changed.
function GuiEditorInspectorPane::afterCommit(%this)
{
	if(isObject(%this.window))
	{
		%this.window.onPaneCommit(%this.target);
	}
}
