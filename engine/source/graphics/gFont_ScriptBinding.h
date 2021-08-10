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
    @param rangeStart The initial Unicode point
    @param rangeEnd The final Unicode point in range
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
    @param rangeStart, rangeEnd The range of the unicode points to populate caches with
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
