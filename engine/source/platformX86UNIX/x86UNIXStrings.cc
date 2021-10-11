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



#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platform.h"
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef HAS_VSSCANF
#  undef HAS_VSSCANF
#endif

/* this routine turns a string to all uppercase - rjp */
char *__strtoup(char *str)
{
   char *newStr = str;
   if (newStr == NULL)
      return(NULL);
   while (*newStr)
   {
      *newStr = toupper(*newStr);
      *newStr++;
   }
   return(str);
}
/* this routine turns a string to all lowercase - rjp */
char *__strtolwr(char *str)
{
   char *newStr = str;
   if (newStr == NULL)
      return(NULL);
   while (*newStr)
   {
      *newStr = tolower(*newStr);
      *newStr++;
   }
   return(str);
}

char *strtolwr(char* str)
{
   return __strtolwr(str);
}

char *stristr(char *szStringToBeSearched, const char *szSubstringToSearchFor)
{
   char *pPos = NULL;
   char *szCopy1 = NULL;
   char *szCopy2 = NULL;

   // verify parameters
   if ( szStringToBeSearched == NULL || 
        szSubstringToSearchFor == NULL ) 
   {
      return szStringToBeSearched;
   }

   // empty substring - return input (consistent with strstr)
   if (strlen(szSubstringToSearchFor) == 0 ) {
      return szStringToBeSearched;
   }

   szCopy1 = __strtolwr(strdup(szStringToBeSearched));
   szCopy2 = __strtolwr(strdup(szSubstringToSearchFor));

   if ( szCopy1 == NULL || szCopy2 == NULL  ) {
      // another option is to raise an exception here
      free((void*)szCopy1);
      free((void*)szCopy2);
      return NULL;
   }

   pPos = strstr(szCopy1, szCopy2);

   if ( pPos != NULL ) {
      // map to the original string
      pPos = szStringToBeSearched + (pPos - szCopy1);
   }

   free((void*)szCopy1);
   free((void*)szCopy2);

   return pPos;
} // stristr(...)

char *dStrdup_r(const char *src, const char *fileName, size_t lineNumber)
{
   char *buffer = (char *) dMalloc_r(dStrlen(src) + 1, fileName, lineNumber);
   dStrcpy(buffer, src);
   return buffer;
}

char* dStrcat(char *dst, const char *src)
{
   return strcat(dst,src);
}   

char* dStrncat(char *dst, const char *src, size_t len)
{
   return strncat(dst,src,len);
}

// concatenates a list of src's onto the end of dst
// the list of src's MUST be terminated by a NULL parameter
// dStrcatl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcatl(char *dst, size_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcatl: destination size is set zero");
   dstSize--;  // leave room for string termination

   // find end of dst
   while (dstSize && *p++)                    
      dstSize--;   
   
   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;   
      }

   va_end(args);

   // make sure the string is terminated 
   *p = 0;

   return dst;
}   


// copy a list of src's into dst
// the list of src's MUST be terminated by a NULL parameter
// dStrccpyl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcpyl(char *dst, size_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcpyl: destination size is set zero");
   dstSize--;  // leave room for string termination

   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;   
      }

   va_end(args);

   // make sure the string is terminated 
   *p = 0;

   return dst;
}   


S32 dStrcmp(const char *str1, const char *str2)
{
   return strcmp(str1, str2);   
}

S32 dStrcmp(const UTF16 *str1, const UTF16 *str2)
{
   int ret;
   const UTF16 *a, *b;
   a = str1;
   b = str2;

   while (*a && *b && (ret = *a - *b) == 0)
   {
      a++, b++;
   }

   if (*a == 0 && *b != 0)
      return -1;

   if (*b == 0 && *a != 0)
      return 1;

   return ret;
}
 
S32 dStricmp(const char *str1, const char *str2)
{
   return strcasecmp(str1, str2);   
}  

S32 dStrncmp(const char *str1, const char *str2, size_t len)
{
   return strncmp(str1, str2, len);   
}  
 
S32 dStrnicmp(const char *str1, const char *str2, size_t len)
{
   return strncasecmp(str1, str2, len);   
}   

char* dStrcpy(char *dst, const char *src)
{
   return strcpy(dst,src);
}   

char* dStrncpy(char *dst, const char *src, size_t len)
{
   return strncpy(dst,src,len);
}   

U32 dStrlen(const char *str)
{
   return strlen(str);
}   


char* dStrupr(char *str)
{
#ifdef __MWERKS__ // metrowerks strupr is broken
   _strupr(str);
   return str;
#else
   return __strtoup(str);
#endif
}   


char* dStrlwr(char *str)
{
   return __strtolwr(str);
}   


char* dStrchr(char *str, S32 c)
{
   return strchr(str,c);
}   


const char* dStrchr(const char *str, S32 c)
{
   return strchr(str,c);
}   


const char* dStrrchr(const char *str, S32 c)
{
   return strrchr(str,c);
}   

char* dStrrchr(char *str, S32 c)
{
   return strrchr(str,c);
}   

U32 dStrspn(const char *str, const char *set)
{
   return(strspn(str, set));
}

U32 dStrcspn(const char *str, const char *set)
{
   return strcspn(str, set);
}   


char* dStrstr(char *str1, const char *str2)
{
   return strstr(str1,str2);
}

const char* dStrstr(const char *str1, const char *str2)
{
   return strstr((char *)str1,str2);
}

char* dStrtok(char *str, const char *sep)
{
   return strtok(str, sep);
}


S32 dAtoi(const char *str)
{
   return atoi(str);   
}  

F32 dAtof(const char *str)
{
   return atof(str);   
}   

bool dAtob(const char *str)
{
   return !dStricmp(str, "true") || dAtof(str);
}   


bool dIsalnum(const char c)
{
   return isalnum(c);
}

bool dIsalpha(const char c)
{
   return(isalpha(c));
}

bool dIsspace(const char c)
{
   return(isspace(c));
}

bool dIsdigit(const char c)
{
   return(isdigit(c));
}

void dPrintf(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vprintf(format, args);
   va_end(args);
}   

S32 dVprintf(const char *format, va_list arglist)
{
   S32 len = vprintf(format, arglist);
   return (len);
}   

S32 dSprintf(char *buffer, size_t bufferSize, const char *format, ...)
{
   va_list args;
   va_start(args, format);

   S32 len = vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   return len;
}   


S32 dVsprintf(char *buffer, size_t bufferSize, const char *format, va_list arglist)
{
   S32 len = vsnprintf(buffer, bufferSize, format, arglist);

   return len;
}

S32 dStrrev(char *str)
{
   // Get string length
   S32 l = dStrlen(str) - 1;
   
   for (int x = 0; x < 1; x++,l--)
   {
      // triple XOR trick
      str[x]^=str[l];

      str[l]^=str[x];
      str[x]^=str[l];
   }

   return l;
}

S32 dItoa(S32 n, char s[])
{
   S32 i, sign;

   if ((sign = n) < 0)
	n = -n;

   i = 0;
   do {
      s[i++] = n % 10 + '0';
   } while((n /= 10) > 0);

   if (sign < 0)
      s[i++] = '-';

   dStrrev(s);

   return dStrlen(s);
} 

S32 dSscanf(const char *buffer, const char *format, ...)
{
   va_list args;
#if defined(HAS_VSSCANF)
   va_start(args, format);
   return __vsscanf(buffer, format, args);   
#else
   va_start(args, format);

   // Boy is this lame.  We have to scan through the format string, and find out how many
   //  arguments there are.  We'll store them off as void*, and pass them to the sscanf
   //  function through specialized calls.  We're going to have to put a cap on the number of args that
   //  can be passed, 8 for the moment.  Sigh.
   static void* sVarArgs[20];
   U32 numArgs = 0;

   for (const char* search = format; *search != '\0'; search++) {
      if (search[0] == '%' && search[1] != '%')
         numArgs++;
   }
   AssertFatal(numArgs <= 20, "Error, too many arguments to lame implementation of dSscanf.  Fix implmentation");

   // Ok, we have the number of arguments...
   for (U32 i = 0; i < numArgs; i++)
      sVarArgs[i] = va_arg(args, void*);
   va_end(args);

   switch (numArgs) {
     case 0: return 0;
     case 1:  return sscanf(buffer, format, sVarArgs[0]);
     case 2:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1]);
     case 3:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2]);
     case 4:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3]);
     case 5:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4]);
     case 6:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5]);
     case 7:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6]);
     case 8:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7]);
     case 9:  return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8]);
     case 10: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9]);
     case 11: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10]);
     case 12: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11]);
     case 13: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12]);
     case 14: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13]);
     case 15: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14]);
     case 16: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15]);
     case 17: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16]);
     case 18: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17]);
     case 19: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17], sVarArgs[18]);
     case 20: return sscanf(buffer, format, sVarArgs[0], sVarArgs[1], sVarArgs[2], sVarArgs[3], sVarArgs[4], sVarArgs[5], sVarArgs[6], sVarArgs[7], sVarArgs[8], sVarArgs[9], sVarArgs[10], sVarArgs[11], sVarArgs[12], sVarArgs[13], sVarArgs[14], sVarArgs[15], sVarArgs[16], sVarArgs[17], sVarArgs[18], sVarArgs[19]);
   }
   return 0;
#endif
}   

S32 dFflushStdout()
{
   return fflush(stdout);
}

S32 dFflushStderr()
{
   return fflush(stderr);
}

void dQsort(void *base, U32 nelem, U32 width, S32 (QSORT_CALLBACK *fcmp)(const void *, const void *))
{
   qsort(base, nelem, width, fcmp);
}

StringTableEntry Platform::createUUID(void)
{
   return NULL;
}

