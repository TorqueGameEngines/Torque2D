# Changelog

Notable changes to Torque2D 4.0 "Rocket Edition", from Early Access 1 onward.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/). Entries
describe what changed for someone building a game on the engine: what you can do,
what your scripts have to say, and what ends up in your files.

## [4.0-ea4] - 2026-08-18

Early Access 4 builds out the GUI Editor that Early Access 3 introduced, gives the
Asset Manager a dirty/save model and a purpose-built inspector for every asset kind,
and makes CMake the only build system.

### Breaking

- Editing an asset no longer writes its file. `refreshAsset()` now only marks the asset as having unsaved changes and tells everything watching it that it changed; `saveAsset()` is the only thing that writes. Any script that relied on a setter persisting its change must now call `saveAsset()` (or `AssetDatabase.saveAllDirtyAssets()`) itself, and a running game no longer rewrites its own content as a side effect of a setter.
- `AnimationAsset.setNamedCellsMode()` was removed. Named-cell mode is now read live from the animation's image asset, so an animation uses names exactly when its image is in explicit-cell mode. Old files still load: an unrecognized `NamedCellsMode` attribute is read as a dynamic field and dropped on the next save, and an image that does not state its mode is inferred from the presence of its `Cells` node.
- AppCore no longer ships GUI profiles. Its roughly seventy hand-written `Gui*Profile` objects were replaced by themes, so a project that named one must either apply a theme (the Gui Editor's Set Theme button does it in one click) or define the profile itself. `GuiDefaultProfile` and its border moved into C++ and always exist.
- The hand-maintained Visual Studio solutions, Xcode projects, Linux makefiles and Emscripten recipe under `engine/compilers/` were deleted. Generate a project from the root `CMakeLists.txt` instead, either directly or through the root scripts (`generate-vs2022.bat`, `generate-vs2026.bat`, `generate-xcode.command`, `generate-xcode-ios.command`, `build-linux.sh`, `generate-emscripten.sh`). Only `engine/compilers/android-studio` remains, and its Gradle build drives the same CMake.
- The horizontal and vertical sizing flags were named for the edge they hold: `anchorLeft`, `anchorRight`, `anchorTop`, `anchorBottom`, and `scale` for the old `relative`. The old spellings still load, but a Gui saved by this build cannot be opened by an older one, which has no `anchorLeft` in its table and silently falls back to the first entry.
- `hidden` and `locked` are no longer written to a file. They are editor working state, not properties of the document; they still read and set normally.
- `GuiImageButtonCtrl` was removed. A profile's `imageAsset` is already indexed by control state, so a plain `GuiButtonCtrl` wearing a four-frame strip draws the same Normal, Hover, Down and Inactive faces.
- `GuiEditCtrl.saveSelection()` and `loadSelection()` were removed. They were an unused prefab mechanism writing the old console-object format; the Gui Editor's clipboard replaces them.
- `ParticleAsset.getFieldValue(time)` and `ParticleAssetEmitter.getFieldValue(time)` are now `getFieldValueAtTime(time)`. They were silently shadowing `SimObject.getFieldValue(fieldName)`, so asking a particle asset for any field's value returned a curve sample instead.
- `GuiTreeViewCtrl` drag-to-reorder now needs the new `AllowReorder` field, which is off by default. It used to be implied by `BindToGuiEditor`, and a tree holding anything other than `GuiControl`s crashed when a row was dragged.
- `GuiButtonCtrl` no longer seeds its caption with "Button". An empty caption is written as an absent attribute, so a default one could never survive its own round trip. `GuiCheckBoxCtrl` and `GuiRadioCtrl` inherit the change; `GuiDropDownCtrl` keeps its "none", which is an empty state rather than a caption.
- Toy modules and projects that carried their own `AppCore` copy should take the library's, which now loads themes and cursors at boot. The toybox's older copy is deliberately left alone and its module says so.

### Added

- The Gui Editor gained a control palette: thirty illustrated tiles in four collapsible groups, in a picture grid or a labeled row list, dropped onto the canvas by dragging or placed for you by clicking.
- The Gui Editor gained undo and redo across placing, moving, resizing, nudging, deleting, reparenting, retheming and field edits, plus cut, copy, paste and duplicate, which work within a Gui and between them.
- A properties pane replaced the generic inspector in the Gui Editor. It shows only the fields the selected control's class actually reads, with purpose-built editors for the common ones: an anchor picker for sizing, color swatches, an image-asset picker you choose by looking at it, a text block, and editors for things that were previously unreachable, such as a list box's rows, a tab book's pages and a menu bar's items.
- An Explorer panel shows the whole control tree with a 16-pixel picture of each control's class, columns for hiding and locking, and drag-to-reparent.
- Controls take their appearance from a `GuiProfileTheme`: a set of profiles, borders and cursors derived from three fonts, six colors and a border size. A theme is authored in the Gui Profile Editor against a live preview, a control dropped on the canvas arrives already wearing the right profile, Set Theme re-skins a whole Gui, and AppCore loads a project's `themes/` folder at runtime so a themed Gui looks the same outside the editor. A stock theme ships in `library/themes` and a new project gets a copy.
- Cursors became the theme's third member family: per-theme art under `<project>/themes/cursors/<Theme>`, a hot-spot editor that draws the art a pixel at a time, and `installThemeCursors` so a game can swap cursor sets at runtime.
- The Gui Editor asks before throwing away a Gui you have changed, on New, Open, Close Project and Exit, and File gained Revert. The document's name sits in the Gui Tools title with an asterisk when it is dirty.
- List boxes and drop downs carry the rows they were authored with. Items are saved as a TAML custom node, drawn on the canvas as they are typed, and reachable from script through `getItemList()` / `setItemList()`. Saving as `.gui` script now warns which state that format cannot keep.
- A tab book dropped into a Gui arrives holding a page and draws a "+" for adding more; a menu bar arrives holding a menu and drops an editable box under it for its commands.
- The Asset Manager gained a dirty/save model: Save, Revert, Duplicate and Undo/Redo on the inspector title bar, a badge on unsaved library tiles, and a Save All / Discard All / Cancel prompt in front of Close Project and Exit. `isAssetDirty`, `saveAsset`, `saveAllDirtyAssets`, `revertAsset`, `duplicateAsset`, `getDirtyAssetCount` and `findAssetDirty` are on `AssetManager`; `isAssetDirty`, `saveAsset`, `revertAsset`, `createStateSnapshot` and `restoreStateSnapshot` are on `AssetBase`.
- The asset library gained tile and row views, a search box matching name, description and category, and sorting by name or category. The choice is remembered between runs in a new editor preferences file.
- Image, animation, font, audio and particle assets each got an inspector pane of their own in place of the stock inspector: only the fields worth showing, reflowing into one to four columns, and a readout saying what actually loaded — an image's real size and frame count, an animation's duration and frame rate, a font's native size and glyph count, a sound's length and format, and warnings for the ways each fails silently.
- An animation editor replaced the preview for animation assets: the art playing on the left, every frame the image offers on the right, and the frames the animation plays along the bottom, with drag-and-drop, scrubbing, a transport bar, a loop toggle, a keep-frame-rate option and a "frames 28 to 32" range builder including ping-pong and hold.
- An animation can name its frames. An image in explicit-cell mode gives every cell a name (auto-generated as `Frame<N>` where one is missing), and an animation listing names survives the sheet being re-cut or re-ordered. A name whose cell is gone draws as an outlined empty cell rather than being silently dropped. New bindings: `getFrameCount`, `getAnimationFrameCount`, `getMissingFrames`.
- The particle emitter graph gained a combined Color Channel view: red, green and blue on one plot with a strip underneath showing the color they actually mix to across a particle's life.
- Each editor now owns its own menus and lends them to the shared bar while it is in front, so the Asset Manager has its own File and Edit with its own Save, Revert, Undo, Redo, Duplicate and Delete.
- Audio descriptions gained a `Priority` flag, settable in an `AudioAsset`'s TAML, so a looping non-positional sound such as background music is not voice-stolen and restarted when the mixer runs out of voices. `alxPlayPreview` auditions a sound at full volume on a reserved channel regardless of the game's mix, and `OpenALIsInitialized` answers whether the driver is already up (`OpenALInit` begins by shutting down, so asking first matters).
- New script API on `GuiControl`: `rendersChildren()`, `applySizing()`, a `childrenReordered` callback, and the `canBeChildOf` / `isGeometryEditable` virtuals the editor consults. `GuiFrameSetCtrl` gained `getFrameLayout()` / `setFrameLayout()`. `GuiTreeViewCtrl` gained `IconImage`, `IconSize`, `IndentSize` and an `onGetItemIcon` callback. `GuiSliderCtrl` gained a `thumbProfile`.
- `SimObject.deepClone()` copies fields, dynamic fields and the whole child tree without running any script lifecycle, which is what makes the editor's clipboard faithful.
- `getInstalledFonts()` exposes the platform font enumerator to script (implemented on Windows, macOS and, newly, Linux via fontconfig), with `getUncachedFonts()`, `clearUncachedFonts()` and `writeOneFontCache()` for baking only the face-and-size pairs that were actually rendered. `setLogFileName()` names the console log. `isEditorMode()` reports whether names are being shadowed.
- `GuiColorPopupCtrl` gained two optional rows: a wrapping grid of script-supplied swatches (`addSwatchI`, `addSwatchF`, `clearSwatches`, `selectSwatch`) and one numeric box per channel in 0-255 or 0.0-1.0. Both are off by default.
- The web target builds and runs. `generate-emscripten.sh` produces a WebAssembly build that boots in a browser, with FreeType compiled to wasm so any face and size renders rather than only the pre-baked `.uft` caches.
- Android builds through the root CMake and renders the editor on-device; iOS builds and runs on both the simulator and a real device.
- `PlanetX`, a complete twin-stick demo game, ships as a reference project: a title screen, a noise-generated level, co-op, weapon upgrades chosen between levels, an options screen with rebindable controls, and a pause menu. It is also the reference implementation for `TORQUE_SCRIPT.md`, the new prescriptive style guide for TorqueScript.
- `TruckToy` was reskinned as an alien-world space rover with a camera-driven parallax background and object-pooled effects.
- The test suites got runners: `tests\run.ps1` (and `tests/run.sh` for macOS and Linux) for the TorqueScript integration suites, and `tests\run-unit.ps1` for the GoogleTest unit suite.
- The New Project dialog asks for a **Module Name**, an **Author** and a **Description**, and offers a **Game Core** template to copy from. Module Name defaults to the title stripped to identifier characters with `Game` appended, and follows the title until you edit it.
- `BlankGame` ships the folders it declares. It had declared eight asset paths and shipped three, so every project made from it warned four times the first time it was opened. Each new folder carries a readme saying what belongs in it and how a file there becomes an asset.

### Changed

- The engine boots the desktop-class editor on every platform, so every platform now gets the 3 MB frame allocator (iOS, Android and Web were on 512 KB and overran it).
- A `GuiScrollCtrl` leaves room for its own scroll bar. An axis whose bar is `alwaysOff` has a real size and hands it to children; an axis that can scroll does not, and `fill` and `center` are stripped there rather than on both axes.
- `GuiWindowCtrl`'s default title height went from 20 to 28, which is what a themed title bar with a real font and a chrome border needs.
- The Explorer tree indents by twelve pixels rather than a full row height, since two gutter columns, a triangle and a class icon already sit in front of the name.
- Windows ships OpenAL Soft 1.24.3 per architecture, staged next to the executable by CMake. The 32-bit `OpenAL32.dll` committed at the repo root since 2.0 could not be loaded by an x64 build, so audio failed to initialize on every modern Windows build.
- An emitter's `BlendMode`, `SrcBlendFactor` and `DstBlendFactor` are now actually read. They round-tripped through TAML and were ignored; one setting on the `ParticlePlayer` covered every emitter. Existing particle assets may render differently.
- `QuantityVariation` on an emitter now applies. It was initialized from the quantity base field, so every emitter got a spurious half-base jitter instead of the variation you set — the stock bonfire emitted 5 to 15 per interval and now emits ten.
- `setEmitterAngle` takes and returns degrees, matching what the persist field and the renderer already meant. It used to store radians and convert back on read, so it agreed with itself and with nothing else.
- Zoom works on the 0-to-1 particle graphs (all four color channels and alpha), which had exactly one zoom level and two dead buttons.
- A `GuiControlProfile`'s `bitmap` and a `GuiCursor`'s `bitmapName` are written relative to the game root when they point inside it. Both are `TypeFilename`, which expands to an absolute path the moment it is set, so what got saved named a folder on one developer's machine. TAML no longer collapses a path that is already relative back into an absolute one.
- Font caches are no longer baked while you edit a theme; a save bakes each face and size the theme actually rendered at. Changing a font size went from several seconds of frozen engine to nothing.
- A new project's game module is named after the project rather than being called `BlankGame`. The Author and Description you type now reach that module too, instead of only AppCore. Existing projects are unaffected -- their module keeps the name it was made with.
- Renaming a module rewrites the module's own source, not just its `module.taml`. A ModuleId is also the namespace the engine calls `<ModuleId>::create()` on and the front half of every asset id, so changing only the definition file left a module that loaded, reported its new name everywhere the editor looked, and silently never ran. All three rename paths go through one place now.

### Fixed

- Numeric fields were silently dropped from saved TAML on glibc, so every saved scene, Gui and profile quietly lost its extents, positions, paddings and unnamed colors on Linux. `getPrefixedDataField` formatted a value into the same console return buffer it read it from, which is undefined and comes back empty on glibc.
- A word too long to wrap left an empty line under it, which threw off vertical alignment, `textExtend` sizing and the decision about whether text fits at all.
- A newline drew the font's missing-glyph box and took up width, because `isValidChar` answered true for every character on Windows. An empty multi-line text box had no caret, because building paragraphs with `getline` could not tell empty text from no text. Pressing Return left two carets blinking.
- Deleting a border profile left every `GuiControlProfile` that referenced it pointing at freed memory.
- A control that changed parent kept the size it had. A scaled control applied the old parent's proportion to the new parent's extent, so a 200-wide button dropped into a 200-wide container arrived 50 wide.
- A control hidden in the editor is now out of the way rather than merely out of sight: it stops taking clicks aimed at what is behind it, loses its sizing handles, and is skipped by rubber-band selection.
- A drop was accepted anywhere on the screen, so dragging a control back onto the palette to change your mind added it behind the palette. A click-placed control is now centered in the visible part of its container rather than in a middle that may be off-screen.
- Fixed on macOS and iOS: a frozen simulation clock, frozen color fade-outs, duplicate windows, a crash enumerating display modes with the display asleep, a crash sorting list-box items under the hardened `std::sort` in newer toolchains, a font enumerator that returned nothing and one that over-released, an assert on a character a font legitimately lacks, `createPath` on a bare file name, and a file's size not counting bytes still in the stdio buffer.
- Fixed on Linux: `getDirectoryList` returned only the path it was given, so the project selector found no projects; the window was not resizable; `Platform::pathCopy` and `Platform::fileRename` were stubs returning false, so creating a project produced an empty folder; and `dStrcatl`, `dStrrev` and `dItoa` were each wrong.
- Fixed on Windows: `pathCopy` could not create the folder its destination sat in and would copy a directory into itself; the folder picker crashed on x64 because a 64-bit window procedure pointer was truncated.
- A `.gui.taml` naming a cursor that is not registered took the editor down with a fatal assert; the type now warns and leaves the field unset.
- An animation kept the frame numbers from its image's previous cut, and went on playing out of the wrong frames with nothing said, because re-cutting an image did not revalidate the animations depending on it.
- `AssetBase::copyTo` was wrong in four ways, all reachable through `clone()` and `acquireAsset(id, true)`: an image copied its cell count into its cell offset, never copied image layers and dropped explicit cells; and an animation and a particle emitter both chose between numbered and named frames by reading the target's mode rather than the source's.
- Selecting a particle asset in the Asset Manager crashed the editor: `PixelArea`'s region name was never initialized, so every frame of every ordinary cell-mode image carried an indeterminate pointer.
- A tooltip on a control with no tooltip profile asserted on a zero reference count when the control slept.
- The editor's modules were never unloaded at exit, so no editor teardown ever ran and the profiles a live preview was wearing could be freed out from under it.
- Accelerators from a grayed-out menu still fired, so Ctrl+N ran the Gui Editor's New Gui from inside the Asset Manager.
- `findAssetPrivate`'s five-argument form called `findAssetInternal`.
- `alxGetAudioLength` acquired the asset and released it on none of its return paths.
- A bitmap font was never initialized and never cleared, so an asset pointed at a missing `.fnt` kept the glyphs of the font it used to have and pointing one at a second `.fnt` left the union of both.
- `GuiMenuItemCtrl`'s `Radio` field was declared as an integer over a one-byte member, so a plain command could read back as a radio item.
- A menu bar's `findHitControl` hid rather than overrode its base, so clicking a menu on an authored bar handed the editor the wrong control.
- Declared asset paths kept the case they were written in. `Path` and `Extension` were plain strings, which the string table interns case-insensitively and hands back whichever spelling reached it first -- so a module copied from a template could come out declaring `Path="Sprites"` where the template said `sprites`. On Windows nobody noticed; on Linux that directory does not exist, so images dropped into `sprites/` were never scanned and never became assets, silently, with a `module.taml` that looked correct.
- Directory names survive a scan with the case they have on disk. `readdir` results were interned case-insensitively, and two of the most ordinary asset folder names -- `sprites` and `fonts` -- are interned during static initialisation by unrelated engine code, so those two could never come back correctly. A folder could be reported by a scan and then fail to open.
- The engine reported its version as 1.0. `TORQUE_GAME_ENGINE` had never been raised past `1000`, so `getVersionNumber()` answered `1000` where 4.0 is `4000`, and the server-query compatibility check compared every build as 1.0. `getVersionString()` returned `"Open Source"`, which is not a version at all; it now returns `"4.0 Early Access 4"`, which is also what Linux prints at start-up.
- On Linux, per-user files were written next to the executable. `getUserDataDirectory()`, `getUserHomeDirectory()` and the temporary directory all answered `"~/"`, which only a shell expands -- so the engine read it as a relative path and resolved it against the working directory, putting the editor's preferences in a folder literally named `~` inside whatever project was running, and scratch files from `getTemporaryFileName()` beside them. They now follow the XDG Base Directory specification, as the equivalents on Windows and macOS have always followed theirs: `$XDG_DATA_HOME` or `~/.local/share` for user data, `$HOME` for the home directory, and `$TMPDIR` or `/tmp` for temporaries. A project's preferences move to `~/.local/share/<Company>/<Product>/`; anything under an old `~` folder can be moved there or discarded.
- On Linux, a relative path was resolved against the directory the process was launched from rather than the one the game runs in. The Unix back-end carried a "pref dir" redirect, meant to route created files under the user's home on the assumption that the game's own folder is not writable. It was never switched on -- it was gated on a `USE_FILE_REDIRECT` macro that nothing ever defined -- so the pref dir was only ever the working directory, but it was cached at the first file open, which happens before the engine changes directory to the folder holding `main.cs`. Launching `/path/to/Torque2D /path/to/main.cs` from anywhere else therefore wrote `console.log` into the folder you happened to be standing in, and read a relative name from there in preference to the game's own copy. A relative path is now resolved against the working directory as it stands at the call, which is what Windows and macOS have always done. The `-nohomedir` command-line switch, which turned off a redirect that was already off, is gone.
- On Linux, the engine can be built and run against the SDL 1.2 that distributions actually ship. It used to call `X11_KeyToUnicode`, a private symbol of SDL 1.2's X11 driver that was never in a public header, to find out which character each key types; the ascii table is now read from the X keymap through Xlib instead. That symbol is missing or wrong everywhere except a hand-built genuine SDL 1.2 -- the `sdl12-compat` shim now shipped as SDL 1.2 by Ubuntu 24.04+ and Arch does export one, but it is a US-layout `toupper()` stub that answers `1` for shift-`1` and `;` for shift-`;`, so building against a distribution package used to link and then mistype every shifted punctuation character. The table is unchanged on a US layout; on an international layout the AltGr row now holds the characters AltGr actually produces (on a German keyboard AltGr-q is `@`) rather than repeating the unshifted ones.
- On Linux, the engine no longer stops at "Unable to initialize SDL" on a Wayland desktop. The X11 back-end takes its display, screen and locking from `SDL_GetWMInfo`, and the SDL2/3-backed `sdl12-compat` defaults to the Wayland video driver, where that call has nothing to hand back. The back-end now asks for SDL's X11 video driver, so it runs through Xwayland; setting `SDL_VIDEODRIVER` yourself still overrides it.

### Removed

- The `OpenAL32.dll` tracked at the repository root; it is now a build artifact staged by CMake.
- `GuiEditorColorWindow`, a scratch window for comparing color picker modes that was still exec'd on every editor start.

## [4.0-ea3] - 2026-04-18

The release that introduced the GUI Editor as a real editor tab.

### Breaking

- `GuiTreeViewCtrl` was rewritten and its script API replaced wholesale. Thirty-seven methods went — among them `clear`, `open`, `insertItem`, `removeItem`, `selectItem`, `getSelectedItem`, `getItemText`, `findItemByName`, `moveItemUp`, `scrollVisible` and `buildVisibleTree` — in favor of seven: `inspect`, `uninspect`, `refresh`, `refreshItemText`, `getItemOpen`, `setItemOpen` and `getItemParent`. A tree is now driven by pointing it at an object rather than by filling it item by item. The fields `tabSize`, `itemHeight`, `fullRowSelect`, `destroyTreeOnSleep`, `MouseDragging`, `MultipleSelections`, `DeleteObjectAllowed` and `DragToItemAllowed` went with them.
- `GuiFrameSetCtrl` was rewritten and its script API replaced. `addColumn`, `addRow`, `removeColumn`, `removeRow`, `getColumnCount`, `getRowCount`, `getColumnOffset`, `setColumnOffset`, `getRowOffset`, `setRowOffset`, `frameBorder`, `frameMinExtent` and `frameMovable` were replaced by `createHorizontalSplit`, `createVerticalSplit`, `setFrameSize` and `anchorFrame`, and the fields `columns`, `rows`, `borderWidth`, `borderEnable`, `borderMovable`, `autoBalance` and `fudgeFactor` are gone.
- `GuiControlProfile` lost `mouseOverSelected`, `profileForChildren`, `soundButtonDown` and `soundButtonOver`, with no successors. A profile still setting button sounds silently stops making them; use the button's callbacks instead.
- `GuiControl` lost the deprecated `Modal` field and `SetFirstResponder`, and `GuiColorPickerCtrl.getSelectorPos2()` was removed.

### Added

- The Gui Editor: a control list you drag onto the canvas, an inspector, a control tree with reordering, arrow-key nudging (Ctrl for extent, Shift to move faster), a grid with a size dialog, menus, and New / Open / Save for both the `.gui` script format and TAML.
- `GuiFrameSetCtrl`: frames that position their children and resize when a divider is dragged, windows that can be dragged out of the frame set to float and docked back in, and windows that stack into generated tab books and can be pulled back out. Its layout saves in both formats.
- `GuiColorPopupCtrl`, a ready-to-use color popup, alongside a reworked `GuiColorPickerCtrl` with repaired display modes, a profile-driven selector, a checkered backdrop behind the alpha slider and text support. The inspector uses the popup for color fields, with text boxes for the four channel values.
- Complex Colors: a separate blend color per corner of a sprite, usable on the individual sprites of a `CompositeSprite`, which makes lighting-like blends possible.
- `SceneWindow` event pass-through: an event a scene window does not consume can be passed to controls behind it, so a sprite-built UI window can sit over a game world.
- Mounted cameras can zoom.
- A `fill` resize mode that keeps a control at 0,0 matching its parent's content area.
- `GuiProgressCtrl` gained instant setting plus `onDisplayChange` and `onProgressComplete` callbacks.
- The inspector remembers which panels were open between objects, and gained a `GuiCursor` field type.
- The Asset Admin was rebuilt on the frame set.
- A first pass at a CMake build, and the Screen Fade library module gained dialog swapping.

### Changed

- GoogleTest went from 1.6.0 to 1.17.0, which moved the engine from C++14 to C++17.
- Tabbing through inspector controls works: check boxes and drop downs show first responder, hidden controls cannot be tabbed to, a text box loses focus on a click outside it, and a text box tabbed into selects its contents.
- The frame set clips a child that cannot fit inside its frame rather than modifying the child's minimum extent.

### Fixed

- `GuiControlProfile` reference counts were wrong, usually high, which held resources far too long and occasionally tripped a fatal assert. Fixing it exposed a second bug: the code that reinstated a profile's image assets when it came back into use had not worked for a long time.
- Buttons and other controls inside a tab page could not be dragged, because the page always stole the selection.
- `mRound(-0.49)` returned negative zero.
- A non-ASCII character in the executable's path broke startup on Win32.
- Sizable bordered textures could exceed their bounds at sizes smaller than their corner pieces.
- Tabs did not resize when their text was updated.
- Deprecated fields no longer trip assert failures on load.

### Removed

- The Gui Editor toy, replaced by the full Gui Editor.
- `GuiInspectorTypeColor`, split into `GuiInspectorTypeFluidColorI` and `GuiInspectorTypeGuiCursor`.

## [4.0-ea2.1] - 2023-05-02

A maintenance release with two new asset features.

### Added

- Layered image assets: several textures composed onto an existing image asset and used as one, with an editor for building them.
- A `ScreenFade` library module for switching canvas contents by fading out and in, also usable as a backdrop for popup dialogs.
- `RandomNumberGenerator`, a PCG-based random number object.
- `GuiSpriteCtrl` gained `ClampImage` (with `getClampImage()` / `setClampImage()`), which decides whether an oversized image is pinned to the top-left or centered and cropped evenly.

### Changed

- Visual Studio 2022 replaced Visual Studio 2017 as a supported toolchain.
- The Linux build moved to `-std=c++17`.
- A drop down always renders its down arrow.

### Fixed

- The modulus operator gave different results on different hardware for negative numbers, because a signed float was converted straight to an unsigned integer.
- Relative positioning did not work correctly with a minimum extent and scroll bars.
- The expand control now re-centers as it expands, fires `onResized` each iteration, and handles a child being resized while expanded.
- White artifacts around the edge of images using bilinear blending and layered images.
- Explicit cell mode could not be turned off through the editor, because it refused to remove the last cell.
- A `GuiSpriteCtrl` rendered its children incorrectly, and its image can now overflow the control's content area.
- A batch of null dereferences, unfreed memory, unclosed handles and out-of-bounds indices across the engine.

## [4.0-ea2] - 2022-06-27

The release that added the Project Manager and the module library.

### Breaking

- `GuiMouseEventCtrl` was removed; its callbacks are on the base `GuiControl`. `onMouseWheelUp` and `onMouseWheelDown` now return void, since nothing used the return value. A script callback on `GuiControl` can return true to consume an event and stop it bubbling.
- `GuiScriptNotifyCtrl`, `GuiFilterCtrl` and `GuiRolloutCtrl` were removed. The first is covered by optional callbacks on `GuiControl`; the second was an ancient way to plot a graph.
- `GuiMLTextCtrl` and `GuiMLTextEditCtrl` were removed, and the `StripMLControlChars()` global went with them. A `GuiControl` carries wrapped text and a `GuiTextEditCtrl` with text wrap on edits it.
- The `modal` field became `UseInput` and moved onto `GuiControl`, with input events now bubbling up through the control tree rather than being handled at one level. `UseInput` should generally not be needed.
- Mouse enter and leave no longer pair up: entering a child no longer leaves its parent, so you may get a dozen enter events walking down to a child and the corresponding leaves only as the pointer leaves the stack.
- `GuiTextEditCtrl` was rewritten around `std::string` and supports multi-line editing by turning on text wrap. UTF16 strings are no longer supported in a text box, double-clicking a word selects that word rather than everything, and a text box changes on hover using its highlight state. Two profile colors were added for selected text.
- Each asset kind now has its own file extension, and a module's `<DeclaredAssets>` glob must match it.

### Added

- The Project Manager: a startup screen for picking or creating a project, a module list with install and update, dependency syncing, a new-module dialog with library templates, editing of a module's declared-asset folders and basic data, and a launch-on-startup flag.
- Library modules a project can import: an `AppCore` with a starter profile file, an `Audio` module handling music on channel 0 and effects on channel 1, an Art Pack template, and a `BlankGame` template.
- `NoiseGenerator`, a Perlin noise object, and a Noise Toy demonstrating it.
- `FontSizeAdjust` on any `GuiControl`: a multiplier on the profile's font size, so one profile serves every size.
- A font color override (`FontColor` plus `OverrideFontColor`) and an alignment override, both on the control, so a minor adjustment no longer needs its own profile.
- `GuiControl.textExtend`, which sizes a control to its text.
- Explicit cell support in the Asset Manager: cells can be added, removed, edited and reordered in the UI, and image assets can be grouped.
- The particle editor shows variation in the base graph.
- `ModuleManager` gained functions for getting a module definition's path and clearing a module database, and its copy function takes a source module.
- `GuiTextEditCtrl` gained a `ReturnCommand`, and `GuiControl` gained keyboard-event callbacks.
- `SimSet.callOnChildrenNoRecurse()` was restored.
- 64-bit Linux builds.

### Fixed

- `ImageAsset.removeExplicitCell()` never worked; `ImageAsset.getExplicitCellIndex()` now returns -1 when there is no such cell.
- Only visible controls have `render` and `preRender` called.
- Controls drifted out of position when relative positioning was on and the window was resized a few times.
- The console no longer runs its contents when the console loses focus, which typically happens as it closes.
- Windows and tab pages tried to pass events back down, which became an infinite loop once events bubbled up.

## [4.0-ea1] - 2021-08-10

The first Early Access release: a reworked GUI system, the first editor, and the
content layout 4.0 uses.

### Breaking

- The content layout changed. The `modules/` folder was replaced by `toybox/` for the example toys, `library/` for reusable importable modules, and `editor/` for the in-engine tools.
- Borders were reworked around a new `GuiBorderProfile` object that can be applied to one or all of a control's sides, with margin and padding so profiles follow the CSS box model. `borderDefault`, `borderTop`, `borderBottom`, `borderLeft` and `borderRight` on a `GuiControlProfile` now name border profiles, and the old flat border settings — `borderThickness`, `borderColor`, `borderColorHL`, `borderColorNA`, `bevelColorHL` and `bevelColorLL` — are gone.
- `GuiControlProfile` lost several more fields with no direct successor: `opaque`, `modal`, `numbersOnly`, `returnTab`, `autoSizeWidth`, `autoSizeHeight` and `fontColorSEL`. `justify` was replaced by the separate `align` and `vAlign`, and `fillColorSL`, `fontColorSL`, `fontDirectory`, `category` and `useInput` were added.
- Buttons went from ten classes to four, and many GUI callbacks were renamed from `onMouse...` to `onTouch...`.
- Thirty-two console classes were removed, effectively the whole legacy control set. `GuiTextCtrl` went because every `GuiControl` now carries text; `GuiBitmapCtrl` and `GuiFadeinBitmapCtrl` were replaced by an upgraded `GuiSpriteCtrl`; `GuiPopUpMenuCtrl` and `GuiPopUpMenuCtrlEx` by the new `GuiDropDownCtrl`; `GuiMenuBar` and `GuiFormCtrl` by the new `GuiMenuBarCtrl` and `GuiMenuItemCtrl`; `GuiPaneControl` by `GuiExpandCtrl` and `GuiPanelCtrl`; `GuiStackControl` by `GuiChainCtrl`; and `GuiControlArrayControl`, `GuiDynamicCtrlArrayControl` and `GuiGridControl` by the rewritten `GuiGridCtrl`. Also gone: `GuiArrayCtrl` (now an abstract base requiring `renderCell`, with its unused row and column headers dropped), `GuiTickCtrl` (every `GuiControl` can animate and processes ticks only if it asks to), `GuiBitmapButtonCtrl`, `GuiBitmapButtonTextCtrl`, `GuiBitmapBorderCtrl`, `GuiBorderButtonCtrl`, `GuiButtonBaseCtrl`, `GuiIconButtonCtrl`, `GuiToolboxButtonCtrl`, `GuiAutoScrollCtrl`, `GuiBackgroundCtrl`, `GuiBubbleTextCtrl`, `GuiConsoleTextCtrl`, `GuiTextListCtrl`, `GuiControlListPopUp`, `GuiSeparatorCtrl` and `GuiImageList`.
- `GuiDragAndDropControl` was renamed `GuiDragAndDropCtrl`.
- `SceneWindow.getWindowExtents()` is now `SceneWindow.getWindowArea()`, the one break outside the GUI.
- `GuiListBoxCtrl.setMultipleSelection()` is now `setMultiSelection()`, with a matching `getMultiSelection()`. `GuiScrollCtrl.getUseScrollEvents()` and `setUseScrollEvents()` were removed with no replacement.
- Spine support was removed: `SkeletonObject` and `SkeletonAsset` are gone, along with every method on them. A branch off master keeps the Spine code.
- Physics particles were removed from the emitter, pending a dedicated emitter.
- `GuiTextEditCtrl`'s `validate` command was removed, along with a handful of long-obsolete features.

### Added

- The Asset Manager: view, create and delete image, animation, font, particle and audio assets, with changes reflected live in the preview above, buttons to create, reorder and delete particle emitters, and support for images whose frames are explicitly defined.
- A graph control for editing particle effect fields, which is what makes particle effects authorable at all.
- Any control can render itself from an `ImageAsset`, a bitmap, or the default drawing, chosen by its profile — with either one frame per state or nine frames per state, for both bitmaps and image assets.
- Themes, including switching themes dynamically, and a Torque 4.0 theme and icon.
- `GuiChainCtrl`, which orders its children in a single line while letting each keep its own size.
- `GuiDropDownCtrl`, combining the button and list box with all their features, plus item IDs, per-item active state, sorting by text or id, and arrow, enter and delete key handling shared with `GuiListBoxCtrl`.
- `GuiExpandCtrl` and `GuiPanelCtrl` replaced the old pane control, and button color changes gained easing.
- `GuiMenuBarCtrl`, a new menu bar.
- Text wrapping on any GUI control, with `getTextWrap` and `setTextWrap`.
- `SceneWindow` scroll bars, so the camera can be moved with the mouse wheel and bars — used by the Asset Admin for zooming and panning around an asset.
- `mEase()`, which eases a value from a starting point to an ending point.
- `Array`, Daniel Neilsen's array object, with push, pop, move, sort and insert.
- `AssetDatabase.PreloadAsset()`, which loads an asset into memory before it is used.
- The cursor hides itself when a touch screen is used and comes back when the mouse is, controlled by `$pref::Gui::hideCursorWhenTouchEventDetected`.
- `GuiTabBookCtrl.getSelectedPage()`, and stock color names on `GuiSpriteCtrl`.
- `Hidden` and `Locked` flags on `SimObject` for the editors, written only when true, and a `TypeName` field.
- `GuiWindowCtrl` was brought up to date with callbacks, multiple profiles and GUI cursors for resizing; `GuiListBoxCtrl` items apply borders and background; `GuiProgressCtrl` animates smoothly.
- `GuiTextEditCtrl` gained an `InputMode` that restricts input to plain text, a number or a decimal value.
- The inspector gained tooltips, adjustable profiles, a grid layout, and a hidden-field list so an editor can keep a field out of reach.
- 64-bit Windows builds, and Android changes required by the Play Store along with a working OpenAL implementation.

### Fixed

- Field validators did not fire in most cases, and `setField` was broken.
- A range of GUI clipping problems, and an extent that shrank below `minExtent` was not restored correctly when there was room again.
- Tabs were not correctly removed from a tab book, and `GuiRadioCtrl` could crash.
- `ImageAsset`'s filter mode.
- The toybox, which several rounds of GUI changes had broken to the point of being unusable.

### Removed

- The Spine and Leap toys.

---

Releases before 4.0 (3.1 through 3.5) are not covered here; see the git log for
that history.
