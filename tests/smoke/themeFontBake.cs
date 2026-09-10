// Theme font bake smoke test. The editor's own text is drawn from caches baked
// into each theme's fonts folder, and a machine without the theme faces
// installed has nothing else to fall back on -- so a cache that ships holding no
// glyphs is text that renders as nothing, everywhere but here.
//
// That is what shipped. A control's fontSizeAdjust multiplies its profile's
// fontSize, so the editor asks for sizes no field anywhere declares (16 x 1.2 is
// 19); GFont answers by building the cache on demand and filling it with only
// the characters that got drawn; and the bake only ever populated five hardcoded
// sizes, none of which were the ones being made this way. Nineteen caches across
// the four themes were sitting at a few hundred bytes.
//
// Run: tests/run.ps1 themeFontBake  ; grep TFSMOKE in tests/logs/.

// Mode 1: opens, appends and closes on every write, so a crash mid-run still
// leaves every line that got as far as being echoed.
setLogMode(1);
$Scripts::ignoreDSOs = true;
setScriptExecEcho(false);
trace(false);

function tfCheck(%label, %cond)
{
	if(%cond) echo("TFSMOKE PASS: " @ %label);
	else      echo("TFSMOKE FAIL: " @ %label);
}

// Emptied by name rather than with directoryDelete, which does not recurse: a
// cache left behind by an earlier run would be loaded by GFont::create and make
// the refill below look like it worked when nothing had happened.
function tfClearDir(%dir)
{
	if(!isDirectory(%dir))
	{
		return;
	}

	%files = getFileList(%dir);
	%count = getFieldCount(%files);
	for(%i = 0; %i < %count; %i++)
	{
		fileDelete(pathConcat(%dir, fileName(getField(%files, %i))));
	}
}

function tfCachePath(%dir, %face, %size)
{
	return pathConcat(%dir, %face SPC %size SPC "(ansi).uft");
}

testExec("editor/main.cs");
schedule(2000, 0, "tfStep1");

//-----------------------------------------------------------------------------
// The cache-name parse. A face may have spaces of its own and may end in digits,
// so the size is the last word rather than the second.
//-----------------------------------------------------------------------------

function tfStep1()
{
	tfCheck("a size is read from a one-word face",
		ThemeManager.cacheSizeFor("raleway 20 (ansi).uft", "raleway") == 20);
	tfCheck("a size is read from a face with spaces",
		ThemeManager.cacheSizeFor("black ops one 21 (ansi).uft", "black ops one") == 21);
	tfCheck("a face ending in digits keeps its digits",
		ThemeManager.cacheSizeFor("vt323 12 (ansi).uft", "vt323") == 12);
	tfCheck("a full path is read the same as a bare name",
		ThemeManager.cacheSizeFor("editor/EditorCore/Themes/LabCoat/fonts/zen dots 19 (ansi).uft", "zen dots") == 19);
	tfCheck("another face's cache is not counted",
		ThemeManager.cacheSizeFor("raleway 20 (ansi).uft", "black ops one") == 0);
	tfCheck("a source face is not a cache",
		ThemeManager.cacheSizeFor("raleway.ttf", "raleway") == 0);

	// Compared by word: strstr would find "4" inside both 14 and 24.
	tfCheck("a size in the list is found", ThemeManager.sizeListContains("12 14 16 18 24", 24));
	tfCheck("a size is not found inside another", !ThemeManager.sizeListContains("12 14 16 18 24", 4));

	schedule(100, 0, "tfStep2");
}

//-----------------------------------------------------------------------------
// Seed the bug: a cache at a size the bake never asked for, holding five glyphs.
//-----------------------------------------------------------------------------

function tfStep2()
{
	$tfTheme = ThemeManager.activeTheme;
	tfCheck("a theme is active", isObject($tfTheme));

	$tfFace = $tfTheme.font[1];
	$tfRealDir = $tfTheme.fontDirectory;
	// Under tests/logs, which is gitignored and which the runner already makes.
	$tfDir = testRoot("tests/logs/themeFontBake");

	createPath(pathConcat($tfDir, "x"));
	tfClearDir($tfDir);

	%prev = $GUI::fontCacheDirectory;
	$GUI::fontCacheDirectory = $tfDir;
	populateFontCacheRange($tfFace, 19, 65, 70);
	writeOneFontCache($tfFace, 19);
	$GUI::fontCacheDirectory = %prev;

	$tfSparse = tfCachePath($tfDir, $tfFace, 19);
	$tfSparseBytes = fileSize($tfSparse);
	echo("TFSMOKE seeded" SPC $tfSparse SPC "at" SPC $tfSparseBytes SPC "bytes");
	tfCheck("the sparse cache was seeded", $tfSparseBytes > 0);

	schedule(100, 0, "tfStep3");
}

//-----------------------------------------------------------------------------
// Bake, pointed at the scratch folder so the real theme caches are untouched.
//-----------------------------------------------------------------------------

function tfStep3()
{
	$tfTheme.fontDirectory = $tfDir;
	ThemeManager.populateFonts();
	$tfTheme.fontDirectory = $tfRealDir;

	tfCheck("the theme's font folder was put back", $tfTheme.fontDirectory $= $tfRealDir);

	// 19 is in none of the five sizes the bake used to know about. It was found
	// because the folder already held a cache for it, which is the whole fix.
	%grown = fileSize($tfSparse);
	echo("TFSMOKE refilled" SPC $tfSparse SPC "from" SPC $tfSparseBytes SPC "to" SPC %grown SPC "bytes");
	tfCheck("an undeclared size already on disk was refilled", %grown > ($tfSparseBytes * 4));

	// And the sizes every theme declares are there for all three of its faces.
	for(%i = 1; %i <= $tfTheme.fontCount; %i++)
	{
		%face = $tfTheme.font[%i];
		%missing = "";
		for(%s = 0; %s < 5; %s++)
		{
			%size = getWord("12 14 16 18 24", %s);
			if(fileSize(tfCachePath($tfDir, %face, %size)) <= 0)
			{
				%missing = %missing SPC %size;
			}
		}
		tfCheck("the base sizes were baked for" SPC %face, %missing $= "");
	}

	schedule(100, 0, "tfStep4");
}

//-----------------------------------------------------------------------------
// The range's end is exclusive, which is what made the Gui Profile Editor's bake
// stop at 0xFE. Baked into two folders so the two are separate GFont resources.
//-----------------------------------------------------------------------------

function tfStep4()
{
	%dirTo255 = pathConcat($tfDir, "to255");
	%dirTo256 = pathConcat($tfDir, "to256");
	createPath(pathConcat(%dirTo255, "x"));
	createPath(pathConcat(%dirTo256, "x"));
	tfClearDir(%dirTo255);
	tfClearDir(%dirTo256);

	%prev = $GUI::fontCacheDirectory;

	$GUI::fontCacheDirectory = %dirTo255;
	populateFontCacheRange($tfFace, 13, 32, 255);
	writeOneFontCache($tfFace, 13);

	$GUI::fontCacheDirectory = %dirTo256;
	populateFontCacheRange($tfFace, 13, 32, 256);
	writeOneFontCache($tfFace, 13);

	$GUI::fontCacheDirectory = %prev;

	%a = fileSize(tfCachePath(%dirTo255, $tfFace, 13));
	%b = fileSize(tfCachePath(%dirTo256, $tfFace, 13));
	echo("TFSMOKE 32..255 =" SPC %a SPC "bytes, 32..256 =" SPC %b SPC "bytes");
	tfCheck("256 bakes the code point 255 stops short of", %b > %a);

	tfClearDir(%dirTo255);
	tfClearDir(%dirTo256);
	directoryDelete(%dirTo255);
	directoryDelete(%dirTo256);
	tfClearDir($tfDir);
	directoryDelete($tfDir);

	schedule(100, 0, "tfDone");
}

function tfDone()
{
	echo("TFSMOKE done");
	quit();
}
