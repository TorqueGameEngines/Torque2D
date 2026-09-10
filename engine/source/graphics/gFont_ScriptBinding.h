//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

/*! @defgroup FontFunctions Font
	@ingroup TorqueScriptFunctions
	@{
*/

/*! List the font faces installed on this machine.
    These are the faces the platform can rasterize without any cache on disk --
    what a tool offers the user to choose from. Sorted and de-duplicated here
    rather than in script: Windows enumerates a family once per character set,
    and sorting a few hundred names in TorqueScript is not free.
    Platforms with no font enumeration (iOS, Android, the web build) return an
    empty string, so a caller should treat the list as an offer, not the whole
    truth -- a face can still be usable through a cached .uft it finds on disk.
    @return A tab-separated list of face names.
*/
ConsoleFunctionWithDocs(getInstalledFonts, ConsoleString, 1, 1, ())
{
	Vector<StringTableEntry> fonts;
	PlatformFont::enumeratePlatformFonts(fonts);

	// Insertion sort, skipping any name already placed. A few hundred short
	// strings, once per session on the caller's side.
	Vector<StringTableEntry> sorted;
	for (U32 i = 0; i < (U32)fonts.size(); i++)
	{
		if (fonts[i] == NULL || fonts[i][0] == '\0')
			continue;

		U32 at = 0;
		bool duplicate = false;
		for (; at < (U32)sorted.size(); at++)
		{
			const S32 order = dStricmp(sorted[at], fonts[i]);
			if (order == 0)
			{
				duplicate = true;
				break;
			}
			if (order > 0)
				break;
		}

		if (!duplicate)
		{
			sorted.insert(at);
			sorted[at] = fonts[i];
		}
	}

	U32 length = 1;
	for (U32 i = 0; i < (U32)sorted.size(); i++)
		length += dStrlen(sorted[i]) + 1;

	char *buffer = Con::getReturnBuffer(length);
	buffer[0] = '\0';

	for (U32 i = 0; i < (U32)sorted.size(); i++)
	{
		if (i != 0)
			dStrcat(buffer, "\t");
		dStrcat(buffer, sorted[i]);
	}

	return buffer;
}

/*! List the face/size pairs that had to be rasterized because no cache existed.
    A tool bakes exactly these: the face is installed here, so it can be written,
    but a machine without it would have nothing to fall back on. The list covers
    sizes no field declares -- a control's fontSizeAdjust multiplies its profile's
    fontSize -- which is why it is recorded as it happens rather than derived.
    Each row also carries the font directory that was searched, so a caller can
    keep one project's caches out of another's folder.
    @return Newline-separated rows of "face<tab>size<tab>directory".
*/
ConsoleFunctionWithDocs(getUncachedFonts, ConsoleString, 1, 1, ())
{
	U32 length = 1;
	for (U32 i = 0; i < (U32)smUncachedFonts.size(); i++)
	{
		// face + tab + size (at most 10 digits) + tab + directory + newline
		length += dStrlen(smUncachedFonts[i].faceName) + dStrlen(smUncachedFonts[i].directory) + 13;
	}

	char *buffer = Con::getReturnBuffer(length);
	buffer[0] = '\0';

	U32 at = 0;
	for (U32 i = 0; i < (U32)smUncachedFonts.size(); i++)
	{
		at += dSprintf(buffer + at, length - at, "%s%s\t%d\t%s",
			(i == 0) ? "" : "\n",
			smUncachedFonts[i].faceName,
			smUncachedFonts[i].size,
			smUncachedFonts[i].directory);
	}

	return buffer;
}

/*! Forget every recorded font cache miss.
    Called after baking, so the next report lists only what has come up since.
    @return No return value.
*/
ConsoleFunctionWithDocs(clearUncachedFonts, ConsoleVoid, 1, 1, ())
{
	smUncachedFonts.clear();
}

/*!
    Populate the font cache for the specified font with characters from the specified string.
    @param faceName The font's name
    @param size The size of the font.
    @param string The string to use to fill font cache
    @return No return value.
*/
ConsoleFunctionWithDocs(populateFontCacheString, ConsoleVoid, 4, 4, (faceName, size, string))
{
   Resource<GFont> f = GFont::create(argv[1], dAtoi(argv[2]), Con::getVariable("$GUI::fontCacheDirectory"));

   if(f.isNull())
   {
      Con::errorf("populateFontCacheString - could not load font '%s %d'!", argv[1], dAtoi(argv[2]));
      return;
   }

   if(!f->hasPlatformFont())
   {
      Con::errorf("populateFontCacheString - font '%s %d' has no platform font! Cannot generate more characters.", argv[1], dAtoi(argv[2]));
      return;
   }

   // This has the side effect of generating character info, including the bitmaps.
   f->getStrWidthPrecise(argv[3]);
}

/*! 
    Populate the font cache for the specified font with Unicode code points in the specified range. 
    Note we only support BMP-0, so code points range from 0 to 65535.
    @param faceName The name of the font
    @param size The size of the font.
    @param rangeStart The first Unicode point to generate.
    @param rangeEnd One past the last Unicode point to generate -- the range is
    half-open, so 32 to 256 covers printable Latin-1 including 0xFF, and 255
    stops one code point short of it.
    @return No return value
*/
ConsoleFunctionWithDocs(populateFontCacheRange, ConsoleVoid, 5, 5, (faceName, size, rangeStart, rangeEnd))
{
   Resource<GFont> f = GFont::create(argv[1], dAtoi(argv[2]), Con::getVariable("$GUI::fontCacheDirectory"));

   if(f.isNull())
   {
      Con::errorf("populateFontCacheRange - could not load font '%s %d'!", argv[1], dAtoi(argv[2]));
      return;
   }

   U32 rangeStart = dAtoi(argv[3]);
   U32 rangeEnd   = dAtoi(argv[4]);

   if(rangeStart > rangeEnd)
   {
      Con::errorf("populateFontCacheRange - range start is after end!");
      return;
   }

   if(!f->hasPlatformFont())
   {
      Con::errorf("populateFontCacheRange - font '%s %d' has no platform font! Cannot generate more characters.", argv[1], dAtoi(argv[2]));
      return;
   }

   // This has the side effect of generating character info, including the bitmaps.
   for(U32 i=rangeStart; i<rangeEnd; i++)
   {
      if(f->isValidChar(i))
         f->getCharWidth(i);
      else
         Con::warnf("populateFontCacheRange - skipping invalid char 0x%x",  i);
   }

   // All done!
}

/*! Dump a full description 
    of all cached fonts, along with info on the codepoints each contains.
    @return No return value
*/
ConsoleFunctionWithDocs(dumpFontCacheStatus, ConsoleVoid, 1, 1, ())
{
   FindMatch match("*.uft", 4096);
   ResourceManager->findMatches(&match);

   Con::printf("--------------------------------------------------------------------------");
   Con::printf("   Font Cache Usage Report (%d fonts found)", match.numMatches());

   for (U32 i = 0; i < (U32)match.numMatches(); i++)
   {
      char *curMatch = match.matchList[i];
      Resource<GFont> font = ResourceManager->load(curMatch);

      // Deal with inexplicably missing or failed to load fonts.
      if (font.isNull())
      {
         Con::errorf(" o Couldn't load font : %s", curMatch);
         continue;
      }

      // Ok, dump info!
      font->dumpInfo();
   }
}

/*! force all cached fonts to
    serialize themselves to the cache.
    @return No return value
*/
ConsoleFunctionWithDocs(writeFontCache, ConsoleVoid, 1, 1, ())
{
   FindMatch match("*.uft", 4096);
   ResourceManager->findMatches(&match);

   Con::printf("--------------------------------------------------------------------------");
   Con::printf("   Writing font cache to disk (%d fonts found)", match.numMatches());

   for (U32 i = 0; i < (U32)match.numMatches(); i++)
   {
      char *curMatch = match.matchList[i];
      Resource<GFont> font = ResourceManager->load(curMatch);

      // Deal with inexplicably missing or failed to load fonts.
      if (font.isNull())
      {
         Con::errorf(" o Couldn't find font : %s", curMatch);
         continue;
      }

      // Ok, dump info!
      FileStream stream;
      if(ResourceManager->openFileForWrite(stream, curMatch)) 
      {
         Con::printf("      o Writing '%s' to disk...", curMatch);
         font->write(stream);
         stream.close();
      }
      else
      {
         Con::errorf("      o Could not open '%s' for write!", curMatch);
      }
   }
}

/*! Force all cached fonts with a given font name in their paths to serialize themselves to the cache.
	@param fontName The name of the font to search for in the path. Will save if found.
	@return No return value
*/
ConsoleFunctionWithDocs(writeSingleFontCache, ConsoleVoid, 2, 2, (fontName))
{
	FindMatch match("*.uft", 4096);
	ResourceManager->findMatches(&match);

	Con::printf("--------------------------------------------------------------------------");
	Con::printf("   Writing font cache for font face %s to disk", argv[1]);

	for (U32 i = 0; i < (U32)match.numMatches(); i++)
	{
		char *curMatch = match.matchList[i];
		if(dStrstr(curMatch, argv[1]) != NULL )
		{
			Resource<GFont> font = ResourceManager->load(curMatch);

			// Deal with inexplicably missing or failed to load fonts.
			if (font.isNull())
			{
				Con::errorf(" o Couldn't find font : %s", curMatch);
				continue;
			}

			// Ok, dump info!
			FileStream stream;
			if (ResourceManager->openFileForWrite(stream, curMatch))
			{
				Con::printf("      o Writing '%s' to disk...", curMatch);
				font->write(stream);
				stream.close();
			}
			else
			{
				Con::errorf("      o Could not open '%s' for write!", curMatch);
			}
		}
	}
}

/*! Write one font's cache file to disk.
    The other two writers are broad: writeFontCache() serializes every cached
    font, and writeSingleFontCache() scans the project for *.uft and rewrites
    each one whose path contains the face name. This one touches exactly the
    face and size asked for, which is what a tool wants after populating a
    single font -- the scanning version costs the same second and a half however
    little changed, and gets slower as caches accumulate.
    The cache path comes from GFont::getFontCacheFilename, so it lands under
    $GUI::fontCacheDirectory -- the same place populateFontCacheRange reads from.
	@param faceName The name of the font.
	@param size The size of the font.
	@return True if the cache was written.
*/
ConsoleFunctionWithDocs(writeOneFontCache, ConsoleBool, 3, 3, (faceName, size))
{
	const U32 faceSize = dAtoi(argv[2]);

	Resource<GFont> font = GFont::create(argv[1], faceSize, Con::getVariable("$GUI::fontCacheDirectory"));
	if (font.isNull())
	{
		Con::errorf("writeOneFontCache - could not load font '%s %d'!", argv[1], faceSize);
		return false;
	}

	char fileName[1024];
	GFont::getFontCacheFilename(argv[1], faceSize, sizeof(fileName), fileName);

	FileStream stream;
	if (!ResourceManager->openFileForWrite(stream, fileName))
	{
		Con::errorf("writeOneFontCache - could not open '%s' for write!", fileName);
		return false;
	}

	font->write(stream);
	stream.close();
	return true;
}

/*!
    Populate the font cache for all fonts with characters from the specified string.
    @param inString The string to use to set the font caches
    @return No return value.
*/
ConsoleFunctionWithDocs(populateAllFontCacheString, ConsoleVoid, 2, 2, (string inString))
{
   FindMatch match("*.uft", 4096);
   ResourceManager->findMatches(&match);

   Con::printf("Populating font cache with string '%s' (%d fonts found)", argv[1], match.numMatches());

   for (U32 i = 0; i < (U32)match.numMatches(); i++)
   {
      char *curMatch = match.matchList[i];
      Resource<GFont> font = ResourceManager->load(curMatch);

      // Deal with inexplicably missing or failed to load fonts.
      if (font.isNull())
      {
         Con::errorf(" o Couldn't load font : %s", curMatch);
         continue;
      }

      if(!font->hasPlatformFont())
      {
         Con::errorf("populateAllFontCacheString - font '%s' has no platform font! Cannot generate more characters.", curMatch);
         continue;
      }

      // This has the side effect of generating character info, including the bitmaps.
      font->getStrWidthPrecise(argv[1]);
   }
}

/*! 
    Populate the font cache for all fonts with Unicode code points in the specified range. 
    Note we only support BMP-0, so code points range from 0 to 65535.
    @param rangeStart, rangeEnd The half-open range of Unicode points to populate
    caches with. rangeEnd is one past the last point generated, so 32 to 256
    covers printable Latin-1 including 0xFF.
    @return No return value
*/
ConsoleFunctionWithDocs(populateAllFontCacheRange, ConsoleVoid, 3, 3, (rangeStart, rangeEnd))
{
   U32 rangeStart = dAtoi(argv[1]);
   U32 rangeEnd   = dAtoi(argv[2]);

   if(rangeStart > rangeEnd)
   {
      Con::errorf("populateAllFontCacheRange - range start is after end!");
      return;
   }

   FindMatch match("*.uft", 4096);
   ResourceManager->findMatches(&match);

   Con::printf("Populating font cache with range 0x%x to 0x%x (%d fonts found)", rangeStart, rangeEnd, match.numMatches());

   for (U32 i = 0; i < (U32)match.numMatches(); i++)
   {
      char *curMatch = match.matchList[i];
      Resource<GFont> font = ResourceManager->load(curMatch);

      // Deal with inexplicably missing or failed to load fonts.
      if (font.isNull())
      {
         Con::errorf(" o Couldn't load font : %s", curMatch);
         continue;
      }

      if(!font->hasPlatformFont())
      {
         Con::errorf("populateAllFontCacheRange - font '%s' has no platform font! Cannot generate more characters.", curMatch);
         continue;
      }

      // This has the side effect of generating character info, including the bitmaps.
      Con::printf("   o Populating font '%s'", curMatch);
      for(U32 i=rangeStart; i<rangeEnd; i++)
      {
         if(font->isValidChar(i))
            font->getCharWidth(i);
         else
            Con::warnf("populateAllFontCacheRange - skipping invalid char 0x%x",  i);
      }
   }
   // All done!
}

/*! 
    Export specified font to the specified filename as a PNG. The 
    image can then be processed in Photoshop or another tool and 
    reimported using importCachedFont. Characters in the font are
    exported as one long strip.
    @param fontName The name of the font to export.
    @param size The size of the font
    @param fileName The export file name.
    @param padding Desired padding settings.
    @param kerning Kerning settings (space between elements)
    @return No return value.
*/
ConsoleFunctionWithDocs(exportCachedFont, ConsoleVoid, 6, 6, (fontName, size, fileName, padding, kerning))
{
   // Read in some params.
   const char *fileName = argv[3];
   S32 padding    = dAtoi(argv[4]);
   S32 kerning    = dAtoi(argv[5]);

   // Tell the font to export itself.
   Resource<GFont> f = GFont::create(argv[1], dAtoi(argv[2]), Con::getVariable("$GUI::fontCacheDirectory"));

   if(f.isNull())
   {
      Con::errorf("populateFontCacheString - could not load font '%s %d'!", argv[1], dAtoi(argv[2]));
      return;
   }

   f->exportStrip(fileName, padding, kerning);
}

/*! 
    Import an image strip from exportCachedFont. Call with the 
    same parameters you called exportCachedFont.
    @param fontName The name of the font to import.
    @param size The size of the font
    @param fileName The imported file name.
    @param padding Desired padding settings.
    @param kerning Kerning settings (space between elements)
    @return No return value.
*/
ConsoleFunctionWithDocs(importCachedFont, ConsoleVoid, 6, 6, (fontName, size, fileName, padding, kerning))
{
   // Read in some params.
   const char *fileName = argv[3];
   S32 padding    = dAtoi(argv[4]);
   S32 kerning    = dAtoi(argv[5]);

   // Tell the font to import itself.
   Resource<GFont> f = GFont::create(argv[1], dAtoi(argv[2]), Con::getVariable("$GUI::fontCacheDirectory"));

   if(f.isNull())
   {
      Con::errorf("populateFontCacheString - could not load font '%s %d'!", argv[1], dAtoi(argv[2]));
      return;
   }

   f->importStrip(fileName, padding, kerning);
}

/*! 
    Copy the specified old font to a new name. The new copy will not have a 
    platform font backing it, and so will never have characters added to it. 
    But this is useful for making copies of fonts to add postprocessing effects 
    to via exportCachedFont.
    @param oldFontName The original font.
    @param oldFontSize The original font's size property.
    @param newFontName The name to set the copy to.
    @return No return value.
*/
ConsoleFunctionWithDocs(duplicateCachedFont, ConsoleVoid, 4, 4, (oldFontName, oldFontSize, newFontName))
{
   char newFontFile[256];
   GFont::getFontCacheFilename(argv[3], dAtoi(argv[2]), 256, newFontFile);

   // Load the original font.
   Resource<GFont> font = GFont::create(argv[1], dAtoi(argv[2]), Con::getVariable("$GUI::fontCacheDirectory"));

   // Deal with inexplicably missing or failed to load fonts.
   if (font.isNull())
   {
      Con::errorf(" o Couldn't find font : %s", newFontFile);
      return;
   }

   // Ok, dump info!
   FileStream stream;
   if(ResourceManager->openFileForWrite(stream, newFontFile)) 
   {
      Con::printf("      o Writing duplicate font '%s' to disk...", newFontFile);
      font->write(stream);
      stream.close();
   }
   else
   {
      Con::errorf("      o Could not open '%s' for write!", newFontFile);
   }
}

/*! @} */ // group FontFunctions
