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

/*!
   @defgroup StringFunctions String
      @ingroup TorqueScriptFunctions
      @{
*/

/*! Use the strcmp function to do a lexicographic case sensitive string comparison between string1 and string2.
    @param string1 String to be compared to string2.
    @param string2 String to be compared to string1.
    @return Returns a numeric value: <-1 (string1 is less than string2, including case), 0 (string1 is equal to string2, including case), 1 (string1 is greater than string2, including case)>.
    @sa see stricmp, strstr
*/
ConsoleFunctionWithDocs(strcmp, ConsoleInt, 3, 3, ( string1 , string2 ))
{
   TORQUE_UNUSED( argc );
   return dStrcmp(argv[1], argv[2]);
}

/*! Use the stricmp function to do a lexicographic case in-sensitive string comparison between string1 and string2.
    @param string1 String to be compared to string2.
    @param string2 String to be compared to string1.
    @return Returns a numeric value: <-1 (string1 is less than string2, ignoring case), 0 (string1 is equal to string2, ignoring case), 1 (string1 is greater than string2, ignoring case)>.
    @sa see strcmp, strstr
*/
ConsoleFunctionWithDocs(stricmp, ConsoleInt, 3, 3, ( string1 , string2 ))
{
   TORQUE_UNUSED( argc );
   return dStricmp(argv[1], argv[2]);
}

/*! Use the strlen function to determine how many characters there are in string.
    @param string The string to count characters for.
    @return Returns the number of characters in string, or 0 if string is invalid or a NULL string
*/
ConsoleFunctionWithDocs(strlen, ConsoleInt, 2, 2, ( string ))
{
   TORQUE_UNUSED( argc );
   return dStrlen(argv[1]);
}

/*! Use the strstr function to locate the first instance of searchString in sourceString.
    @param sourceString The string in which to search for searchString.
    @param searchString The string for which to search for in sourceString.
    @return Returns a numeric character index representing the position in sourceString at which searchString was found, or -1 to indicate that no instance of searchString was found.
    @sa strpos
*/
ConsoleFunctionWithDocs(strstr, ConsoleInt, 3, 3, ( sourceString , searchString ))
{
   TORQUE_UNUSED( argc );
   // returns the start of the sub string argv[2] in argv[1]
   // or -1 if not found.

   const char *retpos = dStrstr(argv[1], argv[2]);
   if(!retpos)
      return -1;
   return (S32)(retpos - argv[1]);
}

/*! Use the strPos function to locate the first instance of searchString in sourceString, starting at character 0, or at an optional offset.
    @param sourceString The string in which to search for searchString.
    @param searchString The string for which to search for in sourceString.
    @param offset An optional non-negative integer value representing the character offset within sourceString at which to begin the search.
    @return Returns a numeric character index representing the postion in sourceString at which searchString was found, or -1 to indicate that no instance of searchString was found.
    @sa strstr
*/
ConsoleFunctionWithDocs(strpos, ConsoleInt, 3, 4, ( sourceString , searchString, [offset]? ))
{
   S32 start = 0;
   if(argc == 4)
      start = dAtoi(argv[3]);
   U32 sublen = dStrlen(argv[2]);
   U32 strlen = dStrlen(argv[1]);
   if(start < 0)
      return -1;
   if(sublen + start > strlen)
      return -1;
   for(; start + sublen <= strlen; start++)
      if(!dStrncmp(argv[1] + start, argv[2], sublen))
         return start;
   return -1;
}

/*! Use the ltrim function to strip the leading white space from sourceString.
    White space is any character in this set: spaces, TABs, and NULL strings.
    @param sourceString The string to be trimmed.
    @return Returns sourceString with all the leading white spaces removed.
    @sa stripChars, stripMLControlChars, stripTrailingSpaces, rtrim, trim
*/
ConsoleFunctionWithDocs(ltrim, ConsoleString,2,2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   const char *ret = argv[1];
   while(*ret == ' ' || *ret == '\n' || *ret == '\t')
      ret++;
   return ret;
}

/*! Use the rtrim function to strip the trailing white space from sourceString.
    White space is any character in this set: spaces, TABs, and NULL strings.
    @param sourceString The string to be trimmed.
    @return Returns sourceString with all the trailing white spaces removed.
    @sa stripChars, stripMLControlChars, stripTrailingSpaces, ltrim, trim
*/
ConsoleFunctionWithDocs(rtrim, ConsoleString,2,2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = argv[1];
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dStrncpy(ret, argv[1], firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

/*! Use the trim function to strip the leading and trailing white space from sourceString.
    White space is any character in this set: spaces, TABs, and NULL strings.
    @param sourceString The string to be trimmed.
    @return Returns sourceString with all the leading and trailing white spaces removed.
    @sa stripChars, stripMLControlChars, stripTrailingSpaces, ltrim, rtrim
*/
ConsoleFunctionWithDocs(trim, ConsoleString,2,2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   const char *ptr = argv[1];
   while(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
      ptr++;
   S32 firstWhitespace = 0;
   S32 pos = 0;
   const char *str = ptr;
   while(str[pos])
   {
      if(str[pos] != ' ' && str[pos] != '\n' && str[pos] != '\t')
         firstWhitespace = pos + 1;
      pos++;
   }
   char *ret = Con::getReturnBuffer(firstWhitespace + 1);
   dStrncpy(ret, ptr, firstWhitespace);
   ret[firstWhitespace] = 0;
   return ret;
}

/*! Use the stripChars function to remove chars from sourceString.
    @param sourceString The string to be modified.
    @param chars The character or characters to search for and remove.
    @return Returns a copy of sourceString, from which all instances of chars have been removed. This may be the original sourceString, if chars was not found.
    @sa stripMLControlChars, stripTrailingSpaces
*/
ConsoleFunctionWithDocs(stripChars, ConsoleString, 3, 3, ( sourceString , chars ))
{
   TORQUE_UNUSED( argc );
   char* ret = Con::getReturnBuffer( dStrlen( argv[1] ) + 1 );
   dStrcpy( ret, argv[1] );
   U32 pos = dStrcspn( ret, argv[2] );
   while ( pos < dStrlen( ret ) )
   {
      dStrcpy( ret + pos, ret + pos + 1 );
      pos = dStrcspn( ret, argv[2] );
   }
   return( ret );
}

/*! 
    remove TorqueML color codes from the string.
    @param stringtoString The string from which to remove TorqueML color codes
    @return A string consisting of the original string minus color codes
*/
ConsoleFunctionWithDocs(stripColorCodes, ConsoleString, 2,2, (stringtoStrip))
{
   char* ret = Con::getReturnBuffer( dStrlen( argv[1] ) + 1 );
   dStrcpy(ret, argv[1]);
   Con::stripColorChars(ret);
   return ret;
}

/*! Use the strlwr function to convert all alpha characters in sourceString to lower-case equivalents.
    @param sourceString The string to be modified.
    @return Returns a copy of sourceString in which all upper-case characters have been converted to lower-case letters.
    @sa strupr
*/
ConsoleFunctionWithDocs(strlwr, ConsoleString,2,2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);
   dStrcpy(ret, argv[1]);
   return dStrlwr(ret);
}

/*! Use the strupr function to convert all alpha characters in sourceString to upper-case equivalents.
    @param sourceString The string to be modified.
    @return Returns a copy of sourceString in which all lower-case characters have been converted to upper-case letters.
    @sa strlwr
*/
ConsoleFunctionWithDocs(strupr, ConsoleString,2,2, ( sourceString ))
{
   TORQUE_UNUSED( argc );
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1);
   dStrcpy(ret, argv[1]);
   return dStrupr(ret);
}

/*! Use the strchr function to extract a sub-string of sourceString, where the sub-string is equal to the first occurence of char in sourceString followed by the remainder of sourceString.
    @param sourceString The string from which to extract a sub-string.
    @param char The character to search for in sourceString.
    @return Returns a string composed of first instance of char in sourceString, and all of the characters after it. If char is not found, a NULL string is returned.
    @sa getSubStr
*/
ConsoleFunctionWithDocs(strchr, ConsoleString,3,3, ( sourceString , char ))
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

/*! strrchr searches the sourceString for the last occurance of the given char
    @param sourceString The string to search
    @return Either a string consisting of the given string from the last occurance of the given char on or an empty string if not found
*/
ConsoleFunctionWithDocs(strrchr, ConsoleString,3,3, ( sourceString , char ))
{
   TORQUE_UNUSED( argc );
   const char *ret = dStrrchr(argv[1], argv[2][0]);
   return ret ? ret : "";
}

/*! Use the strreplace function to replace every instance of from in sourceString with to.
    This function is case-sensitive and only does exact matching
    @param sourceString The string to do replacement operations on.
    @param from The old value to be replaced.
    @param to The new value to replace old values with.
    @return Returns a new version of sourceString in which every instance of the value in from was replaced with the value in to.
*/
ConsoleFunctionWithDocs(strreplace, ConsoleString, 4, 4, ( sourceString , from , to ))
{
   TORQUE_UNUSED( argc );
   S32 fromLen = dStrlen(argv[2]);
   if(!fromLen)
      return argv[1];

   S32 toLen = dStrlen(argv[3]);
   S32 count = 0;
   const char *scan = argv[1];
   while(scan)
   {
      scan = dStrstr(scan, argv[2]);
      if(scan)
      {
         scan += fromLen;
         count++;
      }
   }
   char *ret = Con::getReturnBuffer(dStrlen(argv[1]) + 1 + (toLen - fromLen) * count);
   U32 scanp = 0;
   U32 dstp = 0;
   for(;;)
   {
      const char *scan = dStrstr(argv[1] + scanp, argv[2]);
      if(!scan)
      {
         dStrcpy(ret + dstp, argv[1] + scanp);
         return ret;
      }
      U32 len = (U32)(scan - (argv[1] + scanp));
      dStrncpy(ret + dstp, argv[1] + scanp, len);
      dstp += len;
      dStrcpy(ret + dstp, argv[3]);
      dstp += toLen;
      scanp += len + fromLen;
   }
   return ret;
}

/*! Use the getSubStr function to get a sub-string of sourceString, starting at character index start and ending at character index start + count, or the end-of-string, which ever comes first.
    If start + count is greater than the length of sourceString, the extraction will return a string shorter than count.
    @param sourceString The string from which to extract a sub-string.
    @param start The character index at which the extraction starts.
    @param count The length of the sub-string to extract.
    @return Returns a string made up of the character at start in sourceString and ending at the end of the original sourceString, or start + count, whichever comes first.
    @sa strchr
*/
ConsoleFunctionWithDocs(getSubStr, ConsoleString, 4, 4, ( sourceString , start , count ))
{
   TORQUE_UNUSED( argc );
   // Returns the substring of argv[1], starting at argv[2], and continuing
   //  to either the end of the string, or argv[3] characters, whichever
   //  comes first.
   //
   S32 startPos   = dAtoi(argv[2]);
   S32 desiredLen = dAtoi(argv[3]);
   if (startPos < 0 || desiredLen < 0) {
      Con::errorf(ConsoleLogEntry::Script, "getSubStr(...): error, starting position and desired length must be >= 0: (%d, %d)", startPos, desiredLen);

      return "";
   }

   S32 baseLen = dStrlen(argv[1]);
   if (baseLen < startPos)
      return "";

   U32 actualLen = desiredLen;
   if (startPos + desiredLen > baseLen)
      actualLen = baseLen - startPos;

   char *ret = Con::getReturnBuffer(actualLen + 1);
   dStrncpy(ret, argv[1] + startPos, actualLen);
   ret[actualLen] = '\0';

   return ret;
}

// Used?
/*! Removes all spaces after the final 
    @param string from which to remove trailing spaces
    @return the source string minus trailing spaces
*/
ConsoleFunctionWithDocs( stripTrailingSpaces, ConsoleString, 2, 2, ( string ))
{
   TORQUE_UNUSED( argc );
   S32 temp = S32(dStrlen( argv[1] ));
   if ( temp )
   {
      while ( temp >= 1 && ( argv[1][temp - 1] == ' ' || argv[1][temp - 1] == '_' ) )
         temp--;

      if ( temp )
      {
         char* returnString = Con::getReturnBuffer( temp + 1 );
         dStrncpy( returnString, argv[1], U32(temp) );
         returnString[temp] = '\0';
         return( returnString );			
      }
   }

   return( "" );	
}

/*! @} */ // group StringFunctions
