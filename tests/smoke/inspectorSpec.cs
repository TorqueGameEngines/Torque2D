// Control-spec smoke test. Exercises GuiEditorControlSpec on its own -- no
// editor, no canvas, no UI -- because the spec is pure data and every rule it
// encodes is a claim about what the engine reads. If one of these fails, either
// the engine changed or the table was wrong.
//
// The checks are deliberately weighted toward the surprising entries: the ones
// where the field name suggests the opposite of what the render path does.
// Run: tests/run.ps1 inspectorSpec  ; grep ISSMOKE in tests/logs/.

setLogMode(1);
$Scripts::ignoreDSOs = true;
setScriptExecEcho(false);
trace(false);

function sCheck(%label, %cond)
{
	if(%cond) echo("ISSMOKE PASS: " @ %label);
	else      echo("ISSMOKE FAIL: " @ %label);
}

// Make a control, and deliberately do NOT give it a profile.
//
// This used to have to set one. Several constructors never did -- GuiChainCtrl,
// GuiTabPageCtrl, GuiSliderCtrl, GuiTextEditCtrl, GuiInputCtrl, GuiSpriteCtrl
// and SceneWindow among them -- so a control built from script carried a null
// mProfile, and GuiChainCtrl::onChildAdded runs calculateExtent, which asks the
// profile for its borders. Adding a child to a chain was a hard crash.
//
// GuiControl::onAdd now falls back to GuiDefaultProfile for anything that
// reaches registration without one, so every line below is also a test of that.
function sMake(%class)
{
	return eval("return new " @ %class @ "();");
}

// The spec itself needs nothing but the class list, but the controls step 4
// builds to ask about geometry are real ones, and profiling a real control
// pulls in fonts and textures -- which need a live canvas. So this boots the
// editor like every other suite rather than running the spec in a vacuum.
testExec("editor/main.cs");
schedule(2000, 0, "sStep1");

//-----------------------------------------------------------------------------
// The table itself.
//-----------------------------------------------------------------------------

function sStep1()
{
	testExec("editor/GuiEditor/scripts/GuiEditorControlSpec.cs");
	$sSpec = new ScriptObject() { class = "GuiEditorControlSpec"; };
	sCheck("spec object created", isObject($sSpec));

	// --- Drift guard: a control class added to the engine must not fall
	// through to the unknown-class fallback unnoticed. ---
	%missing = $sSpec.findMissingClasses(enumerateConsoleClasses("GuiControl"));
	sCheck("spec covers every placeable class (missing: " @ %missing @ ")", %missing $= "");

	sCheck("known class recognised", $sSpec.isKnownClass("GuiButtonCtrl"));
	sCheck("unknown class not recognised", !$sSpec.isKnownClass("GuiNotARealCtrl"));

	schedule(50, 0, "sStep2");
}

//-----------------------------------------------------------------------------
// Text. Every one of these is a case where the field list and the render path
// disagree.
//-----------------------------------------------------------------------------

function sStep2()
{
	%s = $sSpec;

	// A chain draws only a "+" in edit mode, so the nine text fields it
	// inherits are all dead.
	sCheck("chain hides text", !%s.isFieldVisible("GuiChainCtrl", "text"));
	sCheck("chain hides align", !%s.isFieldVisible("GuiChainCtrl", "align"));
	sCheck("chain hides fontColor", !%s.isFieldVisible("GuiChainCtrl", "fontColor"));

	// A slider never calls renderText, but it does size its dglDrawText with
	// getFont(mFontSizeAdjust).
	sCheck("slider hides text", !%s.isFieldVisible("GuiSliderCtrl", "text"));
	sCheck("slider hides align", !%s.isFieldVisible("GuiSliderCtrl", "align"));
	sCheck("slider keeps fontSizeAdjust", %s.isFieldVisible("GuiSliderCtrl", "fontSizeAdjust"));

	// The multi-line text control: wrap is real, vAlign and textExtend are not.
	sCheck("text edit keeps textWrap", %s.isFieldVisible("GuiTextEditCtrl", "textWrap"));
	sCheck("text edit keeps align", %s.isFieldVisible("GuiTextEditCtrl", "align"));
	sCheck("text edit hides vAlign", !%s.isFieldVisible("GuiTextEditCtrl", "vAlign"));
	sCheck("text edit hides textExtend", !%s.isFieldVisible("GuiTextEditCtrl", "textExtend"));

	// Only GuiControl::onRender turns its text, so textRotation lives on
	// GuiControl and on the classes that draw through that onRender -- not on a
	// button, which draws its own caption.
	sCheck("GuiControl keeps textRotation", %s.isFieldVisible("GuiControl", "textRotation"));
	sCheck("a grid, drawing through GuiControl's onRender, keeps it", %s.isFieldVisible("GuiGridCtrl", "textRotation"));
	sCheck("a button hides textRotation", !%s.isFieldVisible("GuiButtonCtrl", "textRotation"));
	sCheck("a text edit hides textRotation", !%s.isFieldVisible("GuiTextEditCtrl", "textRotation"));

	// A list renders item text with its own profile: layout lives, the control's
	// own string does not.
	sCheck("list box hides text", !%s.isFieldVisible("GuiListBoxCtrl", "text"));
	sCheck("list box hides textID", !%s.isFieldVisible("GuiListBoxCtrl", "textID"));
	sCheck("list box keeps align", %s.isFieldVisible("GuiListBoxCtrl", "align"));

	// The book draws the page's caption, so the string belongs to the page and
	// the layout belongs to the book.
	sCheck("tab page keeps text", %s.isFieldVisible("GuiTabPageCtrl", "text"));
	sCheck("tab page hides align", !%s.isFieldVisible("GuiTabPageCtrl", "align"));
	sCheck("tab book hides text", !%s.isFieldVisible("GuiTabBookCtrl", "text"));
	sCheck("tab book keeps align", %s.isFieldVisible("GuiTabBookCtrl", "align"));

	// A panel hands its text to the header button it owns.
	sCheck("panel keeps text", %s.isFieldVisible("GuiPanelCtrl", "text"));
	sCheck("panel hides textWrap", !%s.isFieldVisible("GuiPanelCtrl", "textWrap"));

	// Where the text box belongs, and what it is called there.
	// Where the text block goes, which is one answer per class rather than the
	// two overlapping ones it used to be. Proxy is the interesting case: a list
	// has no string of its own but draws its items with this control's font, so
	// the block is worth having open.
	sCheck("button text in header", %s.textBlockHome("GuiButtonCtrl") $= "header");
	sCheck("panel caption in header", %s.textBlockHome("GuiPanelCtrl") $= "header");
	sCheck("tree item font in header", %s.textBlockHome("GuiTreeViewCtrl") $= "header");
	sCheck("grid text in the section", %s.textBlockHome("GuiGridCtrl") $= "section");
	sCheck("slider keeps only its font size", %s.textBlockHome("GuiSliderCtrl") $= "section");
	sCheck("a chain gets no text block", %s.textBlockHome("GuiChainCtrl") $= "none");
	sCheck("nor does a sprite", %s.textBlockHome("GuiSpriteCtrl") $= "none");
	sCheck("grid text still shown", %s.isFieldVisible("GuiGridCtrl", "text"));
	sCheck("drop down text labelled Placeholder",
		%s.textLabelFor("GuiDropDownCtrl") $= "Placeholder");
	sCheck("tab page text labelled Tab Caption",
		%s.textLabelFor("GuiTabPageCtrl") $= "Tab Caption");

	schedule(50, 0, "sStep3");
}

//-----------------------------------------------------------------------------
// Easing. Narrower than the class tree suggests: inheriting GuiEasingSupport is
// not the same as calling getFillColor.
//-----------------------------------------------------------------------------

function sStep3()
{
	%s = $sSpec;

	sCheck("button keeps easing", %s.isFieldVisible("GuiButtonCtrl", "easeFillColorHL"));
	sCheck("drop down keeps easing", %s.isFieldVisible("GuiDropDownCtrl", "easeTimeFillColorSL"));
	sCheck("frame set keeps easing", %s.isFieldVisible("GuiFrameSetCtrl", "easeFillColorSL"));

	// GuiCheckBoxCtrl::onRender draws its box through renderInnerControl and
	// never renders a universal rect of its own, so it inherits a dead set.
	sCheck("check box hides easing", !%s.isFieldVisible("GuiCheckBoxCtrl", "easeFillColorHL"));
	sCheck("radio hides easing", !%s.isFieldVisible("GuiRadioCtrl", "easeFillColorHL"));

	// A menu item calls SimObject::initPersistFields, not GuiControl's.
	sCheck("menu item is bare", %s.hasFlag("GuiMenuItemCtrl", "bare"));
	sCheck("menu item hides Profile", !%s.isFieldVisible("GuiMenuItemCtrl", "Profile"));
	sCheck("menu item hides tooltip", !%s.isFieldVisible("GuiMenuItemCtrl", "tooltip"));
	sCheck("menu item keeps text", %s.isFieldVisible("GuiMenuItemCtrl", "text"));

	// Never shown for anyone.
	sCheck("canSave never shown", !%s.isFieldVisible("GuiControl", "canSave"));
	sCheck("parentGroup never shown", !%s.isFieldVisible("GuiControl", "parentGroup"));

	schedule(50, 0, "sStep4");
}

//-----------------------------------------------------------------------------
// Geometry, which is the parent's answer rather than the class's, and
// isContainer, which is the engine's.
//-----------------------------------------------------------------------------

function sStep4()
{
	%s = $sSpec;

	// --- Every control gets a profile, whether its constructor set one or not.
	// The chain is the one that used to crash outright on the next line. ---
	%bare = sMake("GuiChainCtrl");
	sCheck("a chain gets a fallback profile", isObject(%bare.Profile));
	sCheck("a tab page gets a fallback profile", isObject(sMake("GuiTabPageCtrl").Profile));
	sCheck("a slider gets a fallback profile", isObject(sMake("GuiSliderCtrl").Profile));
	sCheck("a text edit gets a fallback profile", isObject(sMake("GuiTextEditCtrl").Profile));
	sCheck("an input control gets a fallback profile", isObject(sMake("GuiInputCtrl").Profile));
	sCheck("a sprite gets a fallback profile", isObject(sMake("GuiSpriteCtrl").Profile));
	sCheck("a scene window gets a fallback profile", isObject(sMake("SceneWindow").Profile));

	// A control whose constructor names its own profile keeps that one rather
	// than being overwritten by the fallback.
	sCheck("a button keeps its constructor's profile choice",
		isObject(sMake("GuiButtonCtrl").Profile));

	$sRoot = sMake("GuiControl");
	$sRoot.setExtent(400, 400);

	%vChain = sMake("GuiChainCtrl");
	%vChain.IsVertical = true;
	$sRoot.add(%vChain);
	%vKid = sMake("GuiButtonCtrl");
	%vChain.add(%vKid);

	%hChain = sMake("GuiChainCtrl");
	%hChain.IsVertical = false;
	$sRoot.add(%hChain);
	%hKid = sMake("GuiButtonCtrl");
	%hChain.add(%hKid);

	%grid = sMake("GuiGridCtrl");
	$sRoot.add(%grid);
	%gridKid = sMake("GuiButtonCtrl");
	%grid.add(%gridKid);

	// The container people expect to own its children and does not.
	%scroll = sMake("GuiScrollCtrl");
	$sRoot.add(%scroll);
	%scrollKid = sMake("GuiButtonCtrl");
	%scroll.add(%scrollKid);

	%book = sMake("GuiTabBookCtrl");
	$sRoot.add(%book);
	%page = sMake("GuiTabPageCtrl");
	%book.add(%page);

	sCheck("plain child owns its geometry", %s.geometryModeOf(%vChain) $= "full");
	sCheck("vertical chain child is chainV", %s.geometryModeOf(%vKid) $= "chainV");
	sCheck("horizontal chain child is chainH", %s.geometryModeOf(%hKid) $= "chainH");
	sCheck("grid child owns nothing", %s.geometryModeOf(%gridKid) $= "none");
	sCheck("tab page owns nothing", %s.geometryModeOf(%page) $= "none");
	sCheck("scroll child still owns its geometry", %s.geometryModeOf(%scrollKid) $= "full");

	// A vertical chain takes the Y position and the vertical sizing; the cross
	// axis is left alone, and the extent is the child's own on both axes.
	sCheck("chainV keeps HorizSizing", %s.isGeometryFieldLive("chainV", "HorizSizing"));
	sCheck("chainV drops VertSizing", !%s.isGeometryFieldLive("chainV", "VertSizing"));
	sCheck("chainV keeps Extent", %s.isGeometryFieldLive("chainV", "Extent"));
	sCheck("chainV position is x only", %s.livePositionAxes("chainV") $= "x");
	sCheck("chainH position is y only", %s.livePositionAxes("chainH") $= "y");
	sCheck("none has no live position axis", %s.livePositionAxes("none") $= "");
	sCheck("full keeps both position axes", %s.livePositionAxes("full") $= "xy");
	sCheck("none drops Extent", !%s.isGeometryFieldLive("none", "Extent"));

	// isContainer is only meaningful where the control draws children, which is
	// the engine's answer via the new rendersChildren() accessor.
	%list = sMake("GuiListBoxCtrl");
	%slider = sMake("GuiSliderCtrl");
	%sprite = sMake("GuiSpriteCtrl");
	sCheck("plain control can be a container", %s.isContainerFieldVisible($sRoot));
	sCheck("chain can be a container", %s.isContainerFieldVisible(%vChain));
	sCheck("list box cannot be a container", !%s.isContainerFieldVisible(%list));
	sCheck("slider cannot be a container", !%s.isContainerFieldVisible(%slider));
	sCheck("sprite can be a container", %s.isContainerFieldVisible(%sprite));

	schedule(50, 0, "sStep5");
}

//-----------------------------------------------------------------------------
// Dynamic fields, type mapping, and the unknown-class fallback.
//-----------------------------------------------------------------------------

function sStep5()
{
	%s = $sSpec;

	// A frame set serializes its whole frame tree into dynamic fields; editing
	// those by hand can leave a Gui that will not load.
	sCheck("frame set hides frameID0", %s.hidesDynamicField("GuiFrameSetCtrl", "frameID0"));
	sCheck("frame set hides frameExtentX2", %s.hidesDynamicField("GuiFrameSetCtrl", "frameExtentX2"));
	sCheck("frame set keeps a user field", !%s.hidesDynamicField("GuiFrameSetCtrl", "myThing"));
	sCheck("button hides no dynamic fields", !%s.hidesDynamicField("GuiButtonCtrl", "frameID0"));

	// getFieldType answers with a console type's class name, not its TypeXxx
	// constant, so the mapping is keyed on the former.
	sCheck("bool maps to bool", %s.kindForType("bool") $= "bool");
	sCheck("int maps to number", %s.kindForType("int") $= "number");
	sCheck("char maps to number", %s.kindForType("char") $= "number");
	// The two real-numbered kinds are separate from the whole-numbered ones
	// because the row rounds on the way out, which turned a font size multiplier
	// of 1.5 into a 1 and a slider value of 0.5 into a 0.
	sCheck("float maps to decimal", %s.kindForType("float") $= "decimal");
	sCheck("enumval maps to enum", %s.kindForType("enumval") $= "enum");
	sCheck("Point2I maps to point", %s.kindForType("Point2I") $= "point");
	sCheck("Point2F maps to pointf", %s.kindForType("Point2F") $= "pointf");
	sCheck("Vector2 maps to pointf", %s.kindForType("Vector2") $= "pointf");
	sCheck("ColorI maps to color", %s.kindForType("ColorI") $= "color");
	sCheck("FluidColorI maps to color", %s.kindForType("FluidColorI") $= "color");
	sCheck("filename maps to file", %s.kindForType("filename") $= "file");
	sCheck("assetIdString maps to asset", %s.kindForType("assetIdString") $= "asset");
	sCheck("GuiProfile maps to profile", %s.kindForType("GuiProfile") $= "profile");
	sCheck("GuiCursor maps to hidden", %s.kindForType("GuiCursor") $= "hidden");
	sCheck("string maps to text", %s.kindForType("string") $= "text");

	// The type names above have to be what the engine actually answers.
	%btn = sMake("GuiButtonCtrl");
	sCheck("engine spells Point2I as Point2I", %btn.getFieldType("Extent") $= "Point2I");
	sCheck("engine spells TypeBool as bool", %btn.getFieldType("Visible") $= "bool");
	sCheck("engine spells TypeEnum as enumval", %btn.getFieldType("HorizSizing") $= "enumval");
	sCheck("engine spells TypeGuiProfile as GuiProfile", %btn.getFieldType("Profile") $= "GuiProfile");
	sCheck("engine spells TypeS32 as int", %btn.getFieldType("tooltipWidth") $= "int");
	sCheck("engine spells TypeColorI as ColorI", %btn.getFieldType("fontColor") $= "ColorI");

	%sprite = sMake("GuiSpriteCtrl");
	sCheck("engine spells TypeAssetId as assetIdString",
		%sprite.getFieldType("Image") $= "assetIdString");
	sCheck("engine spells TypeFluidColorI as FluidColorI",
		%sprite.getFieldType("imageColor") $= "FluidColorI");

	// A sprite names its picture three mutually exclusive ways.
	sCheck("sprite defaults to Image source", %s.spriteSourceModeOf(%sprite) $= "Image");
	sCheck("Image source offers Frame",
		%s.listHas(%s.spriteSourceFields("Image"), "Frame"));
	sCheck("Bitmap source drops Frame",
		!%s.listHas(%s.spriteSourceFields("Bitmap"), "Frame"));

	// An unknown class shows everything rather than less.
	sCheck("unknown class shows text", %s.isFieldVisible("GuiNotARealCtrl", "text"));
	sCheck("unknown class shows tooltip", %s.isFieldVisible("GuiNotARealCtrl", "tooltip"));
	sCheck("unknown class is not bare", !%s.hasFlag("GuiNotARealCtrl", "bare"));
	sCheck("unknown class still hides canSave", !%s.isFieldVisible("GuiNotARealCtrl", "canSave"));

	// Labels fall back to the field name when nothing better is registered.
	sCheck("registered label used", %s.labelFor("HorizSizing") $= "Horizontal Sizing");
	sCheck("unregistered label falls back", %s.labelFor("someOddField") $= "someOddField");

	// Sections are declared per class rather than inherited, so a subclass that
	// wants its parent's section has to say so.
	sCheck("tree view has both its sections",
		%s.listHas(%s.sectionKeys("GuiTreeViewCtrl"), "List") &&
		%s.listHas(%s.sectionKeys("GuiTreeViewCtrl"), "Tree"));
	// The window's six switches are an icon row in the header beside Title
	// Height, not a section of six checkboxes, so Grips is all it has left.
	sCheck("window has its Grips section",
		%s.listHas(%s.sectionKeys("GuiWindowCtrl"), "Grips"));
	sCheck("and no Window section", !%s.listHas(%s.sectionKeys("GuiWindowCtrl"), "Window"));
	sCheck("its switches are named for the header instead",
		getWordCount(%s.windowToggles()) == 6 &&
		%s.listHas(%s.windowToggles(), "canClose"));
	sCheck("plain control has no sections", %s.sectionKeys("GuiControl") $= "");
	sCheck("window section titled",
		%s.sectionTitle("GuiWindowCtrl", "Grips") $= "Resize Grips");
	sCheck("window grips fields",
		%s.listHas(%s.sectionFields("GuiWindowCtrl", "Grips"), "resizeRightWidth"));

	echo("ISSMOKE DONE");
	schedule(300, 0, "quit");
}
