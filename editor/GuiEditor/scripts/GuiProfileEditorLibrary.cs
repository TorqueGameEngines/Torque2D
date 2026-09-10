
//-----------------------------------------------------------------------------
// The data side of the Gui Profile Editor. Owned by GuiEditor and persistent
// across dialog sessions, so theme member profiles stay alive for the guis
// that reference them (and appear in the Gui Editor's profile dropdowns).
// Loads themes and standalone profiles from the project's themes folder,
// maintains the proxy hierarchy the dialog tree displays, tracks dirty
// roots, saves them to their files, and reverts them by re-reading those
// files.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::onAdd(%this)
{
	%this.themeGroup = new SimGroup();

	%this.proxyRoot = new SimGroup()
	{
		kind = "root";
		treeLabel = "Gui Themes";
	};

	%this.standaloneFolder = new SimGroup()
	{
		kind = "folder";
		treeLabel = "Stand Alone";
	};
	%this.proxyRoot.add(%this.standaloneFolder);

	%this.dirtySet = new SimSet();
	%this.doomedFileCount = 0;

	// Filled on first use and invalidated by a bake; see getFontFaceList.
	%this.fontFaceList = "";
}

function GuiProfileEditorLibrary::onRemove(%this)
{
	if(isObject(%this.dirtySet))
	{
		%this.dirtySet.delete();
	}
	if(isObject(%this.proxyRoot))
	{
		%this.proxyRoot.delete();
	}
	if(isObject(%this.themeGroup))
	{
		// What this library read, it owns. A theme goes down with its members,
		// but a bundle is a set and does not own the profile it names, so each
		// root is taken down through deleteRoot rather than left to the group.
		for(%i = %this.themeGroup.getCount() - 1; %i >= 0; %i--)
		{
			%this.deleteRoot(%this.themeGroup.getObject(%i));
		}
		%this.themeGroup.delete();
	}
}

function GuiProfileEditorLibrary::getThemesPath(%this)
{
	// An explicitly set project folder always wins.
	if(ProjectManager.projectFolder !$= "")
	{
		%projectPath = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder());
		return pathConcat(%projectPath, "themes");
	}

	// Otherwise anchor on the loaded AppCore module, which sits at
	// <project>/AppCore/<version>. This holds whether getModulePath returns
	// a relative or an absolute path; ProjectManager's derivation caches a
	// bogus value when the module path is relative or the module database
	// state shifts, which sent themes to the wrong folder.
	%appCore = ModuleDatabase.findModule("AppCore", 1);
	if(isObject(%appCore))
	{
		%projectPath = filePath(filePath(%appCore.getModulePath()));
		return pathConcat(getMainDotCsDir(), %projectPath, "themes");
	}

	%projectPath = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder());
	return pathConcat(%projectPath, "themes");
}

// The one folder a project keeps its font caches in. Predetermined rather than
// chosen: a cache is keyed by face and size alone, so a second location can only
// hold a duplicate of what the first one already has, and asking the developer
// where to put it is a question with no useful answer.
//
// Every theme and standalone profile the editor creates is pointed here, which
// is also what the game reads at runtime -- a member profile carries the path
// stamped from its theme, and $GUI::fontCacheDirectory (set by AppCore) names
// the same folder for anything that carries none.
//
// Not $GUI::fontCacheDirectory itself: while the editor is loaded, EditorCore
// overrides that with its OWN font folder, so reading it here would send a
// game's caches into editor/EditorCore/gui/fonts.
function GuiProfileEditorLibrary::getFontsPath(%this)
{
	return pathConcat(%this.getThemesPath(), "fonts");
}

// The same path a theme file stores: relative to the game root, so a project
// stays portable.
function GuiProfileEditorLibrary::getRelativeFontsPath(%this)
{
	return makeRelativePath(%this.getFontsPath(), getMainDotCsDir());
}

//-----------------------------------------------------------------------------
// Cursor art.
//
// Unlike fonts, which are keyed by face and size and so can share one folder,
// every theme gets a folder of its own: two themes in a project may want
// cursors that look nothing alike - a menu pointer and a combat reticle - and a
// shared folder would mean one theme overwriting the other's files.
//
// The art is copied rather than referenced so a theme and its cursors travel
// together, and it is copied from the stock grayscale set so a new theme's
// cursors are tinted to its palette from the moment it exists.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::getThemeCursorsPath(%this, %theme)
{
	if(!isObject(%theme) || %theme.getName() $= "")
	{
		return "";
	}
	return pathConcat(%this.getThemesPath(), "cursors", %theme.getName());
}

// The stock set to copy from: the project's own AppCore if it has one, and
// otherwise the editor's, which is always loaded. The two hold the same seven
// files under the same names.
function GuiProfileEditorLibrary::getStockCursorsPath(%this)
{
	%appCore = ModuleDatabase.findModule("AppCore", 1);
	if(isObject(%appCore))
	{
		%path = pathConcat(makeFullPath(%appCore.getModulePath(), getMainDotCsDir()), "gui/images/cursors");
		if(isDirectory(%path))
		{
			return %path;
		}
	}

	%editorCore = EditorManager.findModule("EditorCore", 1);
	if(isObject(%editorCore))
	{
		return pathConcat(makeFullPath(%editorCore.getModulePath(), getMainDotCsDir()), "Themes/BaseTheme/images/cursors");
	}

	return "";
}

// Give %theme its own copy of the stock art and point it at the folder. Safe to
// call on every load: pathCopy is asked not to overwrite, so art the user has
// replaced or edited is never clobbered.
function GuiProfileEditorLibrary::seedThemeCursors(%this, %theme)
{
	%target = %this.getThemeCursorsPath(%theme);
	if(%target $= "")
	{
		return false;
	}

	%source = %this.getStockCursorsPath();
	if(%source $= "" || !isDirectory(%source))
	{
		warn("GuiProfileEditorLibrary::seedThemeCursors: no stock cursor art to copy from.");
		return false;
	}

	createPath(%target @ "/");

	%categories = %theme.getCursorCategoryNames();
	for(%i = 0; %i < getWordCount(%categories); %i++)
	{
		%file = %theme.getCursorStockFile(getWord(%categories, %i));
		if(%file !$= "")
		{
			pathCopy(pathConcat(%source, %file), pathConcat(%target, %file));
		}
	}

	// Assigning the folder restamps, which fills in the bitmap of any cursor
	// that has none yet. One already pointing at art keeps it.
	%directory = makeRelativePath(%target, getMainDotCsDir());
	if(%theme.cursorDirectory !$= %directory)
	{
		%theme.cursorDirectory = %directory;
	}

	return true;
}

// Follow a theme rename with its art. The files are named for nothing but their
// category, so this is a folder copy plus a rewrite of every member still
// pointing into the old folder; art the user pointed somewhere else entirely is
// left alone. The old folder is dropped on save, like any other doomed file.
function GuiProfileEditorLibrary::moveThemeCursors(%this, %theme, %oldName)
{
	%target = %this.getThemeCursorsPath(%theme);
	%source = pathConcat(%this.getThemesPath(), "cursors", %oldName);
	if(%target $= "" || %oldName $= "" || %source $= %target || !isDirectory(%source))
	{
		// Nothing to move: seeding gives the renamed theme a folder of its own.
		return %this.seedThemeCursors(%theme);
	}

	createPath(%target @ "/");
	%newPrefix = makeRelativePath(%target, getMainDotCsDir());

	%categories = %theme.getCursorCategoryNames();
	for(%i = 0; %i < getWordCount(%categories); %i++)
	{
		%category = getWord(%categories, %i);

		// The stock file for the category, whether or not anything points at it
		// yet: a member with no art is filled from it at the next restamp, and
		// that has to find it in the new folder.
		%this.moveCursorFile(%theme.getCursorStockFile(%category), %source, %target);

		// Then every member's own file. Driven by what the members actually
		// name rather than by the stock list, because an extra's art is named
		// after the member -- a rename that copied only the stock files left
		// every extra pointing into the new folder at a file still sitting in
		// the old one.
		%members = %theme.getCursors(%category);
		for(%m = 0; %m < getWordCount(%members); %m++)
		{
			%this.moveCursorArt(getWord(%members, %m), %source, %target, %newPrefix);
		}
	}

	%theme.cursorDirectory = %newPrefix;
	return true;
}

// Copy one file between the two folders and drop the original at the next save.
// Silent about a file that is not there: the stock art of a category whose
// member was pointed elsewhere never existed in the old folder either.
function GuiProfileEditorLibrary::moveCursorFile(%this, %name, %source, %target)
{
	if(%name $= "")
	{
		return;
	}

	%from = pathConcat(%source, %name);
	if(pathCopy(%from, pathConcat(%target, %name)))
	{
		%this.doomFile(%from);
	}
}

// Follow one cursor's art into the new folder, if that is where it lives. Art
// the user chose from somewhere else is left exactly where it is - it is not
// the theme's to move.
function GuiProfileEditorLibrary::moveCursorArt(%this, %cursor, %source, %target, %newPrefix)
{
	%current = %cursor.bitmapName;
	if(%current $= "" || filePath(makeFullPath(%current, getMainDotCsDir())) !$= %source)
	{
		return;
	}

	%name = fileName(%current);
	%this.moveCursorFile(%name, %source, %target);
	%cursor.bitmapName = pathConcat(%newPrefix, %name);
}

// Point a loaded or new root at the project's font folder. A theme restamps its
// members on assignment, so its profiles follow along (an overridden field is
// left alone, as with any other stamp).
function GuiProfileEditorLibrary::applyFontsPath(%this, %root)
{
	%dir = %this.getRelativeFontsPath();

	if(%root.getClassName() $= "GuiProfileTheme")
	{
		if(%root.fontDirectory !$= %dir)
		{
			%root.fontDirectory = %dir;
		}
		return;
	}

	%profile = (%root.getClassName() $= "GuiControlProfile") ? %root : %this.bundleProfile(%root);
	if(isObject(%profile) && %profile.fontDirectory !$= %dir)
	{
		%profile.fontDirectory = %dir;
	}
}

// Every theme this editor knows about: the ones AppCore loaded at boot, which
// stay in the Gui data group, and the ones this library read or created, which
// live in its own group. A theme is in exactly one of the two, so the lists never
// overlap.
function GuiProfileEditorLibrary::getThemes(%this)
{
	%themes = %this.collectThemes(GuiDataGroup, "");
	return %this.collectThemes(%this.themeGroup, %themes);
}

function GuiProfileEditorLibrary::collectThemes(%this, %group, %themes)
{
	if(!isObject(%group))
	{
		return %themes;
	}

	for(%i = 0; %i < %group.getCount(); %i++)
	{
		%object = %group.getObject(%i);
		if(%object.getClassName() $= "GuiProfileTheme")
		{
			%themes = (%themes $= "") ? %object.getId() : (%themes SPC %object.getId());
		}
	}

	return %themes;
}

// Find a profile by name among the ones this library knows about - the members
// of every loaded theme and the stand-alone profiles.
//
// Needed because isObject/nameToID cannot always answer. The Gui Editor runs the
// engine in editor mode, where SimObject::assignName stashes an object's name
// instead of registering it, so that naming a control being edited does not
// create a global. Anything created during an editor session is invisible to the
// name dictionary until it has been saved and read back.
function GuiProfileEditorLibrary::findProfileByName(%this, %name)
{
	if(%name $= "")
	{
		return 0;
	}

	%themes = %this.getThemes();
	for(%i = 0; %i < getWordCount(%themes); %i++)
	{
		%theme = getWord(%themes, %i);
		%categories = %theme.getCategoryNames();
		for(%c = 0; %c < getWordCount(%categories); %c++)
		{
			%members = %theme.getProfiles(getWord(%categories, %c));
			for(%m = 0; %m < getWordCount(%members); %m++)
			{
				%member = getWord(%members, %m);
				if(%member.getName() $= %name)
				{
					return %member;
				}
			}
		}
	}

	for(%i = 0; %i < %this.standaloneFolder.getCount(); %i++)
	{
		%profile = %this.standaloneFolder.getObject(%i).target;
		if(isObject(%profile) && %profile.getName() $= %name)
		{
			return %profile;
		}
	}

	return 0;
}

// The cursor counterpart of findProfileByName, and needed for the same reason:
// a cursor made during an editor session may carry a name the Sim dictionary
// never registered, and reading its slot as empty would let a re-theme quietly
// overwrite a deliberate choice.
function GuiProfileEditorLibrary::findCursorByName(%this, %name)
{
	if(%name $= "")
	{
		return 0;
	}

	%themes = %this.getThemes();
	for(%i = 0; %i < getWordCount(%themes); %i++)
	{
		%theme = getWord(%themes, %i);
		%categories = %theme.getCursorCategoryNames();
		for(%c = 0; %c < getWordCount(%categories); %c++)
		{
			%members = %theme.getCursors(getWord(%categories, %c));
			for(%m = 0; %m < getWordCount(%members); %m++)
			{
				%member = getWord(%members, %m);
				if(%member.getName() $= %name)
				{
					return %member;
				}
			}
		}
	}

	return 0;
}

// Is this one of the stand-alone profiles the editor manages? Asked before an
// apply overwrites a slot: a stand-alone profile is the supported alternative to
// theming and is left alone unless the user says otherwise. A script profile -
// GuiDefaultProfile, one of AppCore's - is not one of these and gets no such
// protection.
function GuiProfileEditorLibrary::isStandaloneProfile(%this, %profile)
{
	for(%i = 0; %i < %this.standaloneFolder.getCount(); %i++)
	{
		if(%this.standaloneFolder.getObject(%i).target == %profile)
		{
			return true;
		}
	}
	return false;
}

// The standalone profiles stamped for one category, as a space-separated list
// of ids. Asked by the Gui Editor's properties pane, which offers a control's
// profile slot the members of the slot's category and nothing else.
//
// %category is matched exactly, and "" is a real answer rather than a wildcard:
// a standalone starts with no category (the profile form shows that as "Any")
// and the pane treats those differently from a stamped one -- offering them
// wherever a slot is already on show, but never letting one be the reason a
// slot appears.
function GuiProfileEditorLibrary::getStandaloneProfiles(%this, %category)
{
	%list = "";
	for(%i = 0; %i < %this.standaloneFolder.getCount(); %i++)
	{
		%profile = %this.standaloneFolder.getObject(%i).target;
		if(!isObject(%profile) || %profile.category !$= %category)
		{
			continue;
		}
		%list = (%list $= "") ? %profile.getId() : (%list SPC %profile.getId());
	}
	return %list;
}

// Load any theme files not already loaded. Safe to call on every dialog
// open: files belonging to live objects are skipped.
function GuiProfileEditorLibrary::scanThemes(%this)
{
	// AppCore reads the project's themes at boot, and the editor loads the
	// project's AppCore - so by the time this runs the objects usually exist
	// already. Take them over rather than reading their files a second time,
	// which would produce a second theme colliding on every member name.
	%this.adoptLoadedThemes();

	%path = %this.getThemesPath();
	createPath(%path @ "/");

	%pattern = %path @ "/*.taml";
	for(%file = findFirstFile(%pattern); %file !$= ""; %file = findNextFile(%pattern))
	{
		if(%this.loadedFile[%file])
		{
			continue;
		}

		%object = TAMLRead(%file);
		if(!isObject(%object))
		{
			warn("GuiProfileEditorLibrary::scanThemes: could not read " @ %file);
			continue;
		}

		%this.adoptFile(%object, %file);
	}
}

// Take a theme or stand-alone profile just read from disk into the library:
// hold it, remember where it came from, point it at the project's font folder
// and build its tree proxies. Returns what the library now tracks - the bundle,
// for a stand-alone profile - or 0 when the file held nothing usable, in which
// case what was read has been deleted.
//
// Shared by the initial scan and by revert, which re-reads the same files.
function GuiProfileEditorLibrary::adoptFile(%this, %object, %file)
{
	%class = %object.getClassName();

	if(%class $= "GuiProfileTheme")
	{
		if(%object.getName() $= "")
		{
			warn("GuiProfileEditorLibrary::adoptFile: skipping " @ %file @ " - the theme has no name (possibly a name collision).");
			%object.delete();
			return 0;
		}

		%this.themeGroup.add(%object);
		%this.rememberFile(%object, %file);
		// A theme written before the font folder was fixed (or copied in from
		// another project) names a folder that is no longer where this project
		// keeps its caches. Repair it on load, but don't mark the theme dirty
		// over it: the corrected path is written the next time it is saved.
		%this.applyFontsPath(%object);
		// Likewise the cursor art: a theme that arrived from another project, or
		// one written before cursors existed, gets its own folder here rather
		// than having none.
		%this.seedThemeCursors(%object);
		%this.addThemeProxies(%object);
		return %object;
	}

	if(!%this.isBundle(%object) && %class !$= "GuiControlProfile")
	{
		warn("GuiProfileEditorLibrary::adoptFile: skipping " @ %file @ " - not a theme or profile.");
		%object.delete();
		return 0;
	}

	%bundle = %this.adoptStandalone(%object);
	if(!isObject(%bundle))
	{
		warn("GuiProfileEditorLibrary::adoptFile: skipping " @ %file @ " - the bundle holds no profile.");
		%object.delete();
		return 0;
	}

	%this.themeGroup.add(%bundle);
	%this.rememberFile(%bundle, %file);
	%this.applyFontsPath(%bundle);
	%this.addStandaloneProxy(%this.bundleProfile(%bundle), %bundle);
	return %bundle;
}

function GuiProfileEditorLibrary::rememberFile(%this, %root, %file)
{
	%this.sourceFile[%root.getId()] = %file;
	%this.loadedFile[%file] = true;
}

// Bring a stand-alone profile file into the bundle shape this library keeps:
// a bare profile is wrapped, a SimSet bundle is already right, and a SimGroup
// bundle - what versions before this wrote - is rebuilt.
//
// The rebuild matters: a SimGroup owns what it holds, so reading one took the
// profile out of the Gui data group, and that group is the only place the
// engine looks when it fills a control's Profile dropdown. Hand the children
// back and rewrap them in a set, which serializes identically without claiming
// them. The file itself is corrected the next time it is saved.
function GuiProfileEditorLibrary::adoptStandalone(%this, %object)
{
	if(%object.getClassName() $= "GuiControlProfile")
	{
		return %this.wrapStandalone(%object);
	}

	if(!isObject(%this.bundleProfile(%object)))
	{
		return 0;
	}

	if(!%object.isMemberOfClass("SimGroup"))
	{
		return %object;
	}

	%bundle = %this.newBundle();
	while(%object.getCount() > 0)
	{
		// Reparenting to the Gui data group is what removes the child from the
		// old group, so the loop drains it; the set add that follows leaves it
		// where it now belongs. Order is preserved, which the borders rely on.
		%child = %object.getObject(0);
		GuiDataGroup.add(%child);
		%bundle.add(%child);
	}
	%object.delete();
	return %bundle;
}

// Take over the themes already in memory: give each one a proxy and a source
// file so the tree, dirty tracking and saving treat it like any other, and mark
// its file loaded so the scan above skips it.
//
// They are deliberately left in the Gui data group rather than moved into
// themeGroup: a theme AppCore loaded belongs to the running project, and this
// library's onRemove deletes what themeGroup holds.
function GuiProfileEditorLibrary::adoptLoadedThemes(%this)
{
	if(!isObject(GuiDataGroup))
	{
		return;
	}

	for(%i = 0; %i < GuiDataGroup.getCount(); %i++)
	{
		%theme = GuiDataGroup.getObject(%i);
		if(%theme.getClassName() !$= "GuiProfileTheme" || isObject(%this.themeProxy[%theme.getId()]))
		{
			continue;
		}

		if(%theme.getName() $= "")
		{
			warn("GuiProfileEditorLibrary::adoptLoadedThemes: skipping an unnamed theme.");
			continue;
		}

		// Where the file is, rather than where it was read from. Nothing records
		// the latter on the theme: a dynamic field would be written into the theme
		// file itself (an absolute path, on whichever machine last loaded it), and
		// saving names the file after the theme anyway - so the two only differ for
		// a file somebody renamed by hand.
		%file = pathConcat(%this.getThemesPath(), %theme.getName() @ ".taml");

		%this.sourceFile[%theme.getId()] = %file;
		%this.loadedFile[%file] = true;
		%this.applyFontsPath(%theme);
		%this.seedThemeCursors(%theme);
		%this.addThemeProxies(%theme);
	}
}

//-----------------------------------------------------------------------------
// Proxy tree.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::addThemeProxies(%this, %theme)
{
	%proxy = new SimGroup()
	{
		kind = "theme";
		target = %theme;
		baseLabel = %theme.getName();
		treeLabel = %theme.getName();
	};
	%this.themeProxy[%theme.getId()] = %proxy;

	%profileFolder = new SimGroup()
	{
		kind = "folder";
		treeLabel = "Profiles";
	};
	%proxy.add(%profileFolder);

	%categoryNames = %theme.getCategoryNames();
	for(%i = 0; %i < getWordCount(%categoryNames); %i++)
	{
		%category = getWord(%categoryNames, %i);
		%categoryProxy = new SimGroup()
		{
			kind = "category";
			theme = %theme;
			category = %category;
			treeLabel = %category;
		};
		%this.categoryProxy[%theme.getId() @ "_" @ %category] = %categoryProxy;
		%profileFolder.add(%categoryProxy);

		// A loaded theme file can carry extra profiles in this category.
		%profiles = %theme.getProfiles(%category);
		for(%p = 1; %p < getWordCount(%profiles); %p++)
		{
			%this.addExtraProxy(%theme, %category, getWord(%profiles, %p));
		}
	}

	%borderFolder = new SimGroup()
	{
		kind = "folder";
		treeLabel = "Borders";
	};
	%proxy.add(%borderFolder);

	%borderNames = %theme.getBorderCategoryNames();
	for(%i = 0; %i < getWordCount(%borderNames); %i++)
	{
		%category = getWord(%borderNames, %i);
		%borderProxy = new ScriptObject()
		{
			kind = "border";
			theme = %theme;
			category = %category;
			treeLabel = %category;
		};
		%borderFolder.add(%borderProxy);
	}

	// Cursors follow the profile shape rather than the border one: a category
	// row that holds the default member, with any extras beneath it. A theme
	// offering two cursors for the same job is what makes the Gui Editor show a
	// choice on a control's cursor slot.
	%cursorFolder = new SimGroup()
	{
		kind = "folder";
		treeLabel = "Cursors";
	};
	%proxy.add(%cursorFolder);

	%cursorNames = %theme.getCursorCategoryNames();
	for(%i = 0; %i < getWordCount(%cursorNames); %i++)
	{
		%category = getWord(%cursorNames, %i);
		%cursorProxy = new SimGroup()
		{
			kind = "cursorCategory";
			theme = %theme;
			category = %category;
			treeLabel = %category;
		};
		%this.cursorCategoryProxy[%theme.getId() @ "_" @ %category] = %cursorProxy;
		%cursorFolder.add(%cursorProxy);

		%cursors = %theme.getCursors(%category);
		for(%c = 1; %c < getWordCount(%cursors); %c++)
		{
			%this.addCursorExtraProxy(%theme, %category, getWord(%cursors, %c));
		}
	}

	%this.proxyRoot.add(%proxy);

	// The Stand Alone folder always stays at the bottom of the tree.
	%this.proxyRoot.pushToBack(%this.standaloneFolder);
}

function GuiProfileEditorLibrary::addExtraProxy(%this, %theme, %category, %profile)
{
	%categoryProxy = %this.categoryProxy[%theme.getId() @ "_" @ %category];
	if(!isObject(%categoryProxy))
	{
		return;
	}

	%label = %profile.getName();
	if(%label $= "")
	{
		%label = "(unnamed)";
	}

	%leaf = new ScriptObject()
	{
		kind = "extra";
		theme = %theme;
		target = %profile;
		category = %category;
		treeLabel = %label;
	};
	%this.extraProxy[%profile.getId()] = %leaf;
	%categoryProxy.add(%leaf);
}

function GuiProfileEditorLibrary::addCursorExtraProxy(%this, %theme, %category, %cursor)
{
	%categoryProxy = %this.cursorCategoryProxy[%theme.getId() @ "_" @ %category];
	if(!isObject(%categoryProxy))
	{
		return;
	}

	%label = %cursor.getName();
	if(%label $= "")
	{
		%label = "(unnamed)";
	}

	%leaf = new ScriptObject()
	{
		kind = "cursorExtra";
		theme = %theme;
		target = %cursor;
		category = %category;
		treeLabel = %label;
	};
	%this.cursorExtraProxy[%cursor.getId()] = %leaf;
	%categoryProxy.add(%leaf);
}

function GuiProfileEditorLibrary::addStandaloneProxy(%this, %profile, %bundle)
{
	%label = %profile.getName();
	if(%label $= "")
	{
		%label = "(unnamed)";
	}

	%leaf = new ScriptObject()
	{
		kind = "standalone";
		target = %profile;
		root = %bundle;
		baseLabel = %label;
		treeLabel = %label;
	};
	%this.standaloneProxy[%bundle.getId()] = %leaf;
	%this.standaloneFolder.add(%leaf);
}

// The GuiControlProfile inside a standalone bundle.
function GuiProfileEditorLibrary::bundleProfile(%this, %bundle)
{
	for(%i = 0; %i < %bundle.getCount(); %i++)
	{
		%child = %bundle.getObject(%i);
		if(%child.getClassName() $= "GuiControlProfile")
		{
			return %child;
		}
	}
	return 0;
}

// Is this a stand-alone profile's save root? SimGroup derives from SimSet, so
// this also answers yes for the bundles older versions wrote (which
// adoptStandalone rebuilds on load).
function GuiProfileEditorLibrary::isBundle(%this, %object)
{
	return isObject(%object) && %object.isMemberOfClass("SimSet");
}

// A stand-alone profile is saved with the custom borders it wears, so the file
// needs one object naming them all. That object is a SimSet and not a SimGroup
// on purpose: a group takes what it holds out of whatever group it was in, and
// a profile belongs in the Gui data group - GuiControlProfile::onAdd puts it
// there, and it is the only place the engine looks when it fills a control's
// Profile dropdown. A set serializes its children exactly the same way and
// leaves their membership alone, which is how a theme's members behave too.
//
// The cost is that a set does not own what it holds: deleteRoot deletes the
// contents explicitly.
function GuiProfileEditorLibrary::newBundle(%this)
{
	return new SimSet() { class = "GuiProfileBundle"; };
}

// Wrap a bare profile in a fresh bundle (used for creation and legacy loads).
function GuiProfileEditorLibrary::wrapStandalone(%this, %profile)
{
	%bundle = %this.newBundle();
	%bundle.add(%profile);
	return %bundle;
}

// Destroy a save root and everything it stands for. A theme owns its members
// and takes them with it; a bundle is a set, which by design does not, so its
// profile and custom borders go explicitly.
function GuiProfileEditorLibrary::deleteRoot(%this, %root)
{
	if(!isObject(%root))
	{
		return;
	}

	if(%this.isBundle(%root))
	{
		%root.deleteObjects();
	}
	%root.delete();
}

function GuiProfileEditorLibrary::getRootProxy(%this, %root)
{
	%proxy = %this.themeProxy[%root.getId()];
	if(!isObject(%proxy))
	{
		%proxy = %this.standaloneProxy[%root.getId()];
	}
	return %proxy;
}

function GuiProfileEditorLibrary::removeProxiesFor(%this, %root)
{
	%proxy = %this.getRootProxy(%root);
	if(isObject(%proxy))
	{
		%proxy.delete();
	}
	%this.themeProxy[%root.getId()] = "";
	%this.standaloneProxy[%root.getId()] = "";
}

//-----------------------------------------------------------------------------
// Dirty tracking.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::markDirty(%this, %root)
{
	if(!isObject(%root))
	{
		return;
	}

	if(!%this.dirtySet.isMember(%root))
	{
		%this.dirtySet.add(%root);
	}

	%proxy = %this.getRootProxy(%root);
	if(isObject(%proxy) && !%proxy.isDirtyMarked)
	{
		%proxy.isDirtyMarked = true;
		%proxy.treeLabel = %proxy.baseLabel @ " *";
		if(isObject(%this.dialog))
		{
			%this.dialog.tree.refresh();
		}
	}
}

function GuiProfileEditorLibrary::unmarkDirty(%this, %root)
{
	%proxy = %this.getRootProxy(%root);
	if(isObject(%proxy) && %proxy.isDirtyMarked)
	{
		%proxy.isDirtyMarked = false;
		%proxy.treeLabel = %proxy.baseLabel;
	}
}

function GuiProfileEditorLibrary::isDirty(%this)
{
	return (%this.dirtySet.getCount() > 0 || %this.doomedFileCount > 0);
}

//-----------------------------------------------------------------------------
// Save and revert.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::saveAll(%this)
{
	%path = %this.getThemesPath();
	createPath(%path @ "/");

	while(%this.dirtySet.getCount() > 0)
	{
		%root = %this.dirtySet.getObject(0);
		%this.dirtySet.remove(%root);

		%file = %this.sourceFile[%root.getId()];
		if(%file $= "")
		{
			%rootName = %this.rootName(%root);
			if(%rootName $= "")
			{
				warn("GuiProfileEditorLibrary::saveAll: cannot save an unnamed theme or profile - name it first.");
				continue;
			}
			%file = pathConcat(%path, %rootName @ ".taml");
		}

		TAMLWrite(%root, %file);
		%this.sourceFile[%root.getId()] = %file;
		%this.loadedFile[%file] = true;
		%this.unmarkDirty(%root);

		// Bake the font caches the saved theme needs. This is the one place a
		// pause is acceptable, and it is why nothing bakes while editing.
		%this.bakeFontsFor(%root);
	}

	for(%i = 0; %i < %this.doomedFileCount; %i++)
	{
		if(isFile(%this.doomedFile[%i]))
		{
			fileDelete(%this.doomedFile[%i]);
		}
	}
	%this.doomedFileCount = 0;
}

// Discard the session's edits: every dirty root is deleted and, when it has
// a source file, re-read from it. Doomed files are simply forgotten (they
// were never deleted from disk), so a deleted theme returns at the next
// scan.
function GuiProfileEditorLibrary::revertAll(%this)
{
	while(%this.dirtySet.getCount() > 0)
	{
		%root = %this.dirtySet.getObject(0);
		%this.dirtySet.remove(%root);

		%file = %this.sourceFile[%root.getId()];
		%this.removeProxiesFor(%root);
		%this.releaseRoot(%root);
		%this.deleteRoot(%root);

		if(%file !$= "" && isFile(%file))
		{
			%object = TAMLRead(%file);
			if(isObject(%object) && isObject(%this.adoptFile(%object, %file)))
			{
				continue;
			}

			warn("GuiProfileEditorLibrary::revertAll: could not re-read " @ %file);
			%this.loadedFile[%file] = "";
		}
		else if(%file !$= "")
		{
			%this.loadedFile[%file] = "";
		}
	}

	%this.doomedFileCount = 0;

	// The re-read themes are new objects; the Gui that was wearing the old ones
	// was left on GuiDefaultProfile by releaseRoot and has to be put back.
	if(isObject(%this.owner))
	{
		%this.owner.reattachTheme();
	}
}

//-----------------------------------------------------------------------------
// Freeing a theme or profile the Gui under edit may be wearing.
//
// A control's profile field is a raw pointer that nothing updates when the
// profile goes away, so anything about to delete one tells the owner first and
// lets it move the affected controls onto GuiDefaultProfile.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::releaseRoot(%this, %root)
{
	if(!isObject(%this.owner))
	{
		return;
	}

	if(%root.getClassName() $= "GuiProfileTheme")
	{
		%this.owner.detachTheme(%root, 0);
	}
	else
	{
		%this.owner.detachTheme(0, %this.bundleProfile(%root));
	}
}

function GuiProfileEditorLibrary::releaseProfile(%this, %profile)
{
	if(isObject(%this.owner))
	{
		%this.owner.detachTheme(0, %profile);
	}
}

//-----------------------------------------------------------------------------
// Naming.
//
// A profile has to have a real, registered name: a Gui refers to its profile by
// name, and that is what TAML writes on both sides. But the Gui Editor runs the
// engine in editor mode, where SimObject::assignName stashes the name on the
// object instead of registering it - which is right for a control being edited
// (naming one should not create a global) and wrong for everything this library
// makes. Step out of editor mode while naming, and put it back.
//-----------------------------------------------------------------------------

function GuiProfileEditorLibrary::beginNaming(%this)
{
	%this.namingLeftEditorMode = isEditorMode();
	if(%this.namingLeftEditorMode)
	{
		editorMode(false);
	}
}

function GuiProfileEditorLibrary::endNaming(%this)
{
	if(%this.namingLeftEditorMode)
	{
		editorMode(true);
	}
	%this.namingLeftEditorMode = false;
}

//-----------------------------------------------------------------------------
// Operations.
//-----------------------------------------------------------------------------

// Mark a file for deletion at the next save. Nothing is removed from disk until
// then, so Cancel keeps it.
function GuiProfileEditorLibrary::doomFile(%this, %file)
{
	if(%file $= "")
	{
		return;
	}
	%this.doomedFile[%this.doomedFileCount] = %file;
	%this.doomedFileCount++;
}

function GuiProfileEditorLibrary::doomSourceFile(%this, %root)
{
	%file = %this.sourceFile[%root.getId()];
	if(%file !$= "")
	{
		%this.doomFile(%file);
		%this.sourceFile[%root.getId()] = "";
		%this.loadedFile[%file] = "";
	}
}

function GuiProfileEditorLibrary::createTheme(%this, %name)
{
	if(%name $= "" || isObject(%name))
	{
		warn("GuiProfileEditorLibrary::createTheme: the name '" @ %name @ "' is empty or already taken.");
		return 0;
	}

	%this.beginNaming();
	%theme = new GuiProfileTheme();
	%this.themeGroup.add(%theme);
	%named = %theme.renameTheme(%name);
	%this.endNaming();

	if(!%named)
	{
		%theme.delete();
		return 0;
	}

	// Friendlier starting point than the C++ ctor defaults: borders on so the
	// recipes' bevels and rims actually show (a theme looks intentional out of
	// the box) and a readable 16px base font. Each assignment restamps, fine for
	// a fresh theme.
	%theme.borderSize = 1;
	%theme.fontSize = 16;
	%this.applyFontsPath(%theme);
	%this.seedThemeCursors(%theme);

	%this.sourceFile[%theme.getId()] = "";
	%this.addThemeProxies(%theme);
	%this.markDirty(%theme);
	return %theme;
}

function GuiProfileEditorLibrary::deleteTheme(%this, %theme)
{
	// The file is only removed on save; cancel keeps it (and the next scan
	// reloads it).
	%this.doomSourceFile(%theme);

	%this.dirtySet.removeIfMember(%theme);
	%this.removeProxiesFor(%theme);
	%this.releaseRoot(%theme);
	%this.deleteRoot(%theme);
}

function GuiProfileEditorLibrary::renameThemeTo(%this, %theme, %name)
{
	%oldName = %theme.getName();

	%this.beginNaming();
	%renamed = %theme.renameTheme(%name);
	%this.endNaming();

	if(!%renamed)
	{
		return false;
	}

	// The art folder is named for the theme, so it follows the rename. Members
	// still pointing into the old folder are repointed; art the user chose from
	// somewhere else is left where it is.
	%this.moveThemeCursors(%theme, %oldName);

	// The old file no longer matches the theme; replace it on save.
	%this.doomSourceFile(%theme);

	%proxy = %this.themeProxy[%theme.getId()];
	if(isObject(%proxy))
	{
		%proxy.baseLabel = %name;
		%proxy.isDirtyMarked = false;
	}
	%this.markDirty(%theme);

	// Member names changed too, so refresh every extra label in this theme.
	%categoryNames = %theme.getCategoryNames();
	for(%i = 0; %i < getWordCount(%categoryNames); %i++)
	{
		%profiles = %theme.getProfiles(getWord(%categoryNames, %i));
		for(%p = 1; %p < getWordCount(%profiles); %p++)
		{
			%profile = getWord(%profiles, %p);
			%leaf = %this.extraProxy[%profile.getId()];
			if(isObject(%leaf))
			{
				%leaf.treeLabel = %profile.getName();
			}
		}
	}

	%cursorNames = %theme.getCursorCategoryNames();
	for(%i = 0; %i < getWordCount(%cursorNames); %i++)
	{
		%cursors = %theme.getCursors(getWord(%cursorNames, %i));
		for(%c = 1; %c < getWordCount(%cursors); %c++)
		{
			%cursor = getWord(%cursors, %c);
			%leaf = %this.cursorExtraProxy[%cursor.getId()];
			if(isObject(%leaf))
			{
				%leaf.treeLabel = %cursor.getName();
			}
		}
	}

	return true;
}

function GuiProfileEditorLibrary::createExtraProfile(%this, %theme, %category)
{
	%this.beginNaming();
	%profile = %theme.createProfile(%category);
	%this.endNaming();

	if(!isObject(%profile))
	{
		return 0;
	}

	%this.addExtraProxy(%theme, %category, %profile);
	%this.markDirty(%theme);
	return %profile;
}

// A second (or third) cursor in a category, with art of its own so editing it
// cannot change what the category's default looks like. The copy is named for
// the member, which is what keeps two extras in one category apart.
function GuiProfileEditorLibrary::createExtraCursor(%this, %theme, %category)
{
	%this.beginNaming();
	%cursor = %theme.createCursor(%category);
	%this.endNaming();

	if(!isObject(%cursor))
	{
		return 0;
	}

	%folder = %this.getThemeCursorsPath(%theme);
	%stock = %theme.getCursorStockFile(%category);
	if(%folder !$= "" && %stock !$= "")
	{
		%file = pathConcat(%folder, %cursor.getName() @ fileExt(%stock));
		if(pathCopy(pathConcat(%folder, %stock), %file))
		{
			%cursor.bitmapName = makeRelativePath(%file, getMainDotCsDir());
		}
	}

	%this.addCursorExtraProxy(%theme, %category, %cursor);
	%this.markDirty(%theme);
	return %cursor;
}

// Removes the cursor and answers with the art file it leaves behind, or "" if
// there is nothing worth asking about. The file is NOT deleted here: the art
// may be the user's own, it may still be in use, and either way throwing away
// a picture is not something to do as a side effect of removing the thing that
// happened to be pointing at it. The caller asks; deleteCursorArt does it.
function GuiProfileEditorLibrary::removeExtraCursor(%this, %theme, %cursor)
{
	%leaf = %this.cursorExtraProxy[%cursor.getId()];
	%bitmap = %cursor.bitmapName;

	%removed = %theme.removeCursor(%cursor);
	if(!%removed)
	{
		return "";
	}

	if(isObject(%leaf))
	{
		%leaf.delete();
	}
	%this.cursorExtraProxy[%cursor.getId()] = "";
	%this.markDirty(%theme);

	return %this.orphanedCursorArt(%theme, %bitmap);
}

// Is this art now unused, ours to remove, and actually there? Only then is
// there a question to ask. Called after the cursor is gone, so "unused" is a
// plain search of what remains.
//
// Three ways to answer no, and each is a file that must survive:
//   - a path outside the theme's own cursor folder is the user's own picture,
//     chosen with the Find button; this editor did not put it there.
//   - a path another cursor still names is in use, and the obvious case is an
//     extra pointed at the category's stock art, which the default member uses.
//   - a path with no file behind it has nothing to delete.
function GuiProfileEditorLibrary::orphanedCursorArt(%this, %theme, %bitmap)
{
	if(%bitmap $= "")
	{
		return "";
	}

	%full = makeFullPath(%bitmap, getMainDotCsDir());
	%folder = %this.getThemeCursorsPath(%theme);
	if(%folder $= "" || filePath(%full) !$= %folder)
	{
		return "";
	}

	if(%this.isCursorArtInUse(%bitmap) || !isFile(%full))
	{
		return "";
	}

	return %full;
}

// Does any cursor in any loaded theme still name this art? Compared as the
// relative paths a theme stores, which is the one form they all agree on.
function GuiProfileEditorLibrary::isCursorArtInUse(%this, %bitmap)
{
	%themes = %this.getThemes();
	for(%i = 0; %i < getWordCount(%themes); %i++)
	{
		%theme = getWord(%themes, %i);
		%categories = %theme.getCursorCategoryNames();
		for(%c = 0; %c < getWordCount(%categories); %c++)
		{
			%members = %theme.getCursors(getWord(%categories, %c));
			for(%m = 0; %m < getWordCount(%members); %m++)
			{
				if(getWord(%members, %m).bitmapName $= %bitmap)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// Drop the file at the next save, like every other file this editor removes -
// so Cancel keeps it, exactly as Cancel keeps a deleted theme.
function GuiProfileEditorLibrary::deleteCursorArt(%this, %file)
{
	%this.doomFile(%file);
}

function GuiProfileEditorLibrary::removeExtraProfile(%this, %theme, %profile)
{
	// Only an extra has a leaf, and only an extra can be removed - so this is also
	// the test for "is about to be deleted", which is when the Gui has to let go
	// of it.
	%leaf = %this.extraProxy[%profile.getId()];
	if(isObject(%leaf))
	{
		%this.releaseProfile(%profile);
	}

	%removed = %theme.removeProfile(%profile);
	if(!%removed)
	{
		return false;
	}

	if(isObject(%leaf))
	{
		%leaf.delete();
	}
	%this.extraProxy[%profile.getId()] = "";
	%this.markDirty(%theme);
	return true;
}

// The filename stem for a save root: a theme/profile by its own name, a
// standalone bundle by its inner profile's name.
function GuiProfileEditorLibrary::rootName(%this, %root)
{
	if(%this.isBundle(%root))
	{
		%profile = %this.bundleProfile(%root);
		return isObject(%profile) ? %profile.getName() : "";
	}
	return %root.getName();
}

function GuiProfileEditorLibrary::createStandalone(%this, %name)
{
	if(%name $= "" || isObject(%name))
	{
		warn("GuiProfileEditorLibrary::createStandalone: the name '" @ %name @ "' is empty or already taken.");
		return 0;
	}

	%this.beginNaming();
	%profile = new GuiControlProfile(%name);
	%this.endNaming();

	// Without this the profile carries no font directory, and the first control to
	// wear it takes $GUI::fontCacheDirectory instead -- the EDITOR's font folder
	// while the editor is loaded.
	%this.applyFontsPath(%profile);
	%bundle = %this.wrapStandalone(%profile);
	%this.themeGroup.add(%bundle);
	%this.sourceFile[%bundle.getId()] = "";
	%this.addStandaloneProxy(%profile, %bundle);
	%this.markDirty(%bundle);
	return %profile;
}

// Delete a stand-alone profile, taking the custom borders it owns with it. As
// with a theme, the file is only removed on save: cancel keeps it, and the next
// scan reads it back.
//
// Nothing can list the Guis that wear the profile - a saved .gui names it and
// nothing indexes that - so those are the user's to repair, and a control that
// asks for a profile no longer there gets GuiDefaultProfile. The document open
// in the editor is the one case that can be handled, and releaseRoot handles it.
function GuiProfileEditorLibrary::deleteStandalone(%this, %bundle)
{
	%this.doomSourceFile(%bundle);

	%this.dirtySet.removeIfMember(%bundle);
	%this.removeProxiesFor(%bundle);
	%this.releaseRoot(%bundle);
	%this.deleteRoot(%bundle);
}

// Rename a stand-alone profile. The counterpart to renameThemeTo, and it costs
// the same thing: a Gui saved earlier names the profile its controls wear, so
// renaming one leaves those controls looking for a profile that is no longer
// there. The controls in memory hold the object itself and are unaffected.
function GuiProfileEditorLibrary::renameStandaloneTo(%this, %bundle, %name)
{
	%profile = %this.bundleProfile(%bundle);
	if(!isObject(%profile))
	{
		return false;
	}

	// Confirming the name dialog without editing it is not an error.
	if(%profile.getName() $= %name)
	{
		return true;
	}

	if(%name $= "" || isObject(%name))
	{
		warn("GuiProfileEditorLibrary::renameStandaloneTo: the name '" @ %name @ "' is empty or already taken.");
		return false;
	}

	%this.beginNaming();
	%profile.setName(%name);
	%this.endNaming();

	// assignName refuses a name the dictionary already holds and says so on the
	// console rather than reporting back, so the only way to know is to look.
	if(%profile.getName() !$= %name)
	{
		warn("GuiProfileEditorLibrary::renameStandaloneTo: the engine refused the name '" @ %name @ "'.");
		return false;
	}

	// A stand-alone profile's file is named after it, so the old one no longer
	// matches; replace it on save.
	%this.doomSourceFile(%bundle);

	%proxy = %this.standaloneProxy[%bundle.getId()];
	if(isObject(%proxy))
	{
		%proxy.baseLabel = %name;
		%proxy.isDirtyMarked = false;
	}
	%this.markDirty(%bundle);

	return true;
}

//-----------------------------------------------------------------------------
// Font discovery and cache baking. A theme and an individual profile both name
// a face, so the enumeration and baking live here on the shared library rather
// than on either form -- they are reachable from both panes via dialog.library.
//-----------------------------------------------------------------------------

// The faces to offer: everything installed on this machine, plus anything the
// project's font folder already holds a cache for, plus whatever the field is
// set to now. The installed fonts are the point -- a developer styles a theme
// with a font they have, and Save bakes a cache so it renders on a machine that
// doesn't. The cached faces matter for the other direction: a theme that arrived
// with its caches names a face nobody here has installed, and it must stay
// choosable rather than vanish from the list.
//
// Cached on the library because a pane repopulates on every tree selection while
// this list changes only when a bake writes new files. Enumerating a few hundred
// families and a directory listing per click is a cost with nothing to show for
// it. Call invalidateFontFaceList after anything that adds a cache file.
function GuiProfileEditorLibrary::getFontFaceList(%this, %current)
{
	if(%this.fontFaceList $= "")
	{
		%this.fontFaceList = %this.sortTabList(
			%this.mergeTabLists(getInstalledFonts(), %this.enumerateFonts(%this.getFontsPath())));
	}

	// The current value is merged per call rather than cached: it is the one part
	// of the list that differs between the profile being edited and the next one.
	if(%current !$= "" && !%this.tabListContains(%this.fontFaceList, %current))
	{
		return %this.sortTabList(%this.fontFaceList TAB %current);
	}

	return %this.fontFaceList;
}

function GuiProfileEditorLibrary::invalidateFontFaceList(%this)
{
	%this.fontFaceList = "";
}

// Append the second list's entries that the first one doesn't already have.
function GuiProfileEditorLibrary::mergeTabLists(%this, %list, %additions)
{
	%n = getFieldCount(%additions);
	for(%i = 0; %i < %n; %i++)
	{
		%value = getField(%additions, %i);
		if(%value $= "" || %this.tabListContains(%list, %value))
		{
			continue;
		}
		%list = (%list $= "") ? %value : (%list TAB %value);
	}
	return %list;
}

// Returns a tab-separated, sorted, de-duplicated list of face names found in
// the directory (both source .ttf/.otf faces and baked .uft/.fnt caches).
function GuiProfileEditorLibrary::enumerateFonts(%this, %dir)
{
	if(%dir $= "")
	{
		return "";
	}
	%path = makeFullPath(%dir, getMainDotCsDir());
	if(!isDirectory(%path))
	{
		return "";
	}

	%files = getFileList(%path);
	%out = "";
	%n = getFieldCount(%files);
	for(%i = 0; %i < %n; %i++)
	{
		%file = getField(%files, %i);
		if(%file $= "")
		{
			continue;
		}

		%ext = strlwr(fileExt(%file));
		if(getSubStr(%ext, 0, 1) $= ".")
		{
			%ext = getSubStr(%ext, 1, strlen(%ext) - 1);
		}

		%face = "";
		if(%ext $= "ttf" || %ext $= "otf")
		{
			%face = fileBase(fileName(%file));
		}
		else if(%ext $= "uft" || %ext $= "fnt")
		{
			%face = %this.faceFromCacheName(fileBase(fileName(%file)));
		}
		else
		{
			continue;
		}

		if(%face $= "" || %this.tabListContains(%out, %face))
		{
			continue;
		}
		%out = (%out $= "") ? %face : (%out TAB %face);
	}

	return %this.sortTabList(%out);
}

// A baked cache is named "<face> <size> (<charset>).uft"; recover the face.
function GuiProfileEditorLibrary::faceFromCacheName(%this, %base)
{
	%paren = strpos(%base, " (");
	if(%paren >= 0)
	{
		%base = getSubStr(%base, 0, %paren);
	}
	%wc = getWordCount(%base);
	if(%wc >= 2)
	{
		%last = getWord(%base, %wc - 1);
		if(%last $= (%last + 0))
		{
			%base = getWords(%base, 0, %wc - 2);
		}
	}
	return trim(%base);
}

// Case-insensitively, matching how sortTabList orders. A face is the same face
// however it was capitalized: a cache file named "arial 12 (ansi).uft" and the
// installed "Arial" are one entry in a font list, not two, and the lookup finds
// the file either way.
function GuiProfileEditorLibrary::tabListContains(%this, %list, %value)
{
	%n = getFieldCount(%list);
	for(%i = 0; %i < %n; %i++)
	{
		if(stricmp(getField(%list, %i), %value) == 0)
		{
			return true;
		}
	}
	return false;
}

function GuiProfileEditorLibrary::sortTabList(%this, %list)
{
	%n = getFieldCount(%list);
	if(%n < 2)
	{
		return %list;
	}
	for(%i = 0; %i < %n; %i++)
	{
		%arr[%i] = getField(%list, %i);
	}
	for(%i = 1; %i < %n; %i++)
	{
		%key = %arr[%i];
		%j = %i - 1;
		while(%j >= 0 && stricmp(%arr[%j], %key) > 0)
		{
			%arr[%j + 1] = %arr[%j];
			%j--;
		}
		%arr[%j + 1] = %key;
	}
	%out = %arr[0];
	for(%i = 1; %i < %n; %i++)
	{
		%out = %out TAB %arr[%i];
	}
	return %out;
}

// Fill a drop-down with the faces, selecting %selected. The current selection is
// never lost, even when the directory did not list it.
function GuiProfileEditorLibrary::fillFontDropdown(%this, %drop, %faces, %selected)
{
	%drop.clearItems();

	%selIndex = -1;
	%count = 0;
	%n = getFieldCount(%faces);
	for(%i = 0; %i < %n; %i++)
	{
		%face = getField(%faces, %i);
		if(%face $= "")
		{
			continue;
		}
		%drop.addItem(%face);
		if(%face $= %selected)
		{
			%selIndex = %count;
		}
		%count++;
	}

	if(%selIndex < 0 && %selected !$= "")
	{
		%drop.insertItem(0, %selected);
		%selIndex = 0;
	}
	if(%selIndex >= 0)
	{
		%drop.setSelected(%selIndex);
	}
}

// Best-effort bake of one font cache. Does nothing if the cache is already
// there or the inputs are incomplete.
//
// This is deliberately NOT called while the user is editing. Nothing needs it
// to be: GFont::create synthesizes the face from the platform font and
// rasterizes glyphs on demand, so the preview draws a newly chosen face or size
// without any cache at all. The cache only matters once the theme is saved and
// has to render somewhere the platform font may not exist.
//
// Baking on every commit is also expensive enough to be felt. Measured cold on
// a debug build: populating the whole BMP-0 range costs ~4.3 seconds (and warns
// once per unmapped code point), where the printable Latin-1 range costs
// nothing measurable; the old writeSingleFontCache then added a flat ~1.7
// seconds because it scanned the project for every *.uft and rewrote each one
// whose name contained the face. Hence the narrow range and the targeted
// writeFontCache, which writes exactly this face and size.
function GuiProfileEditorLibrary::bakeFont(%this, %face, %dir, %size)
{
	if(%face $= "" || %dir $= "" || %size <= 0)
	{
		return;
	}

	if(%this.cacheFileExists(%dir, %face, %size))
	{
		return;
	}

	%prev = $GUI::fontCacheDirectory;
	$GUI::fontCacheDirectory = %dir;
	// Printable Latin-1. The end of the range is exclusive, so 256 is the bound
	// that bakes 0xFF -- 255 stops one code point short of it. Anything outside
	// the range still renders; it just rasterizes on demand instead of coming
	// from the cache.
	populateFontCacheRange(%face, %size, 32, 256);
	writeOneFontCache(%face, %size);
	$GUI::fontCacheDirectory = %prev;
}

// Is this face and size already baked? Asked of the filesystem, not isFile:
// isFile answers out of the resource manager, and GFont::create registers every
// font it synthesizes under the .uft path it looked for and didn't find. So the
// moment the editor renders a face, isFile claims its cache exists -- and the
// bake this guards would skip every font the developer could see, which is all
// of them.
function GuiProfileEditorLibrary::cacheFileExists(%this, %dir, %face, %size)
{
	%path = makeFullPath(%dir, getMainDotCsDir());
	if(!isDirectory(%path))
	{
		return false;
	}
	return %this.tabListContains(getFileList(%path), %face SPC %size SPC "(ansi).uft");
}

// Bake every face/size a root actually uses. Called from the dialog's Save,
// where a short pause is expected, rather than from the edit path.
function GuiProfileEditorLibrary::bakeFontsFor(%this, %root)
{
	if(!isObject(%root))
	{
		return;
	}

	if(%root.getClassName() $= "GuiProfileTheme")
	{
		// The theme's three faces at its base size, plus whatever its members
		// ended up with -- a recipe may offset the size, and a member may have
		// overridden the face outright.
		%this.bakeFont(%root.fontTitle, %root.fontDirectory, %root.fontSize);
		%this.bakeFont(%root.fontBody, %root.fontDirectory, %root.fontSize);
		%this.bakeFont(%root.fontCode, %root.fontDirectory, %root.fontSize);

		%names = %root.getCategoryNames();
		for(%i = 0; %i < getWordCount(%names); %i++)
		{
			%profiles = %root.getProfiles(getWord(%names, %i));
			for(%p = 0; %p < getWordCount(%profiles); %p++)
			{
				%this.bakeProfileFont(getWord(%profiles, %p));
			}
		}
	}
	else
	{
		// A standalone bundle: just the profile it wraps.
		%this.bakeProfileFont(%this.bundleProfile(%root));
	}

	%this.bakeRequestedFonts();
	%this.invalidateFontFaceList();
}

function GuiProfileEditorLibrary::bakeProfileFont(%this, %profile)
{
	if(isObject(%profile))
	{
		%this.bakeFont(%profile.fontType, %profile.fontDirectory, %profile.fontSize);
	}
}

// Bake the sizes the walk above cannot see. A control's fontSizeAdjust multiplies
// its profile's fontSize, so what actually got rendered is not what any field
// says: a profile set to 16 worn by a control adjusting 1.2 asks GFont for 19,
// and only a machine with the face installed would ever draw it. The engine
// records each face/size it had to rasterize for want of a cache; this bakes the
// ones belonging to this project's font folder and forgets the rest -- the editor
// misses its own chrome fonts against its own folder, and those are not ours.
function GuiProfileEditorLibrary::bakeRequestedFonts(%this)
{
	%dir = %this.getFontsPath();
	%rows = getUncachedFonts();
	%count = getRecordCount(%rows);

	for(%i = 0; %i < %count; %i++)
	{
		%row = getRecord(%rows, %i);
		%face = getField(%row, 0);
		%size = getField(%row, 1);
		%rowDir = getField(%row, 2);

		if(%face $= "" || %size <= 0 || %rowDir $= "")
		{
			continue;
		}

		// Compared as full paths: a profile carries the folder relative to the game
		// root, while getFontsPath is absolute.
		if(makeFullPath(%rowDir, getMainDotCsDir()) !$= %dir)
		{
			continue;
		}

		%this.bakeFont(%face, %rowDir, %size);
	}

	clearUncachedFonts();
}
