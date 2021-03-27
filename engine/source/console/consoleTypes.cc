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

#include "console/console.h"
#include "console/consoleTypes.h"
#include "string/stringTable.h"
#include "sim/simBase.h"

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

#ifndef _VECTOR2_H_
#include "2d/core/Vector2.h"
#endif

//////////////////////////////////////////////////////////////////////////
// TypeName
//////////////////////////////////////////////////////////////////////////
ConsoleType(string, TypeName, sizeof(const char*), "")

ConsoleGetType(TypeName)
{
   return *((const char **)(dptr));
}

ConsoleSetType(TypeName)
{
   if (argc == 1)
      *((const char **)dptr) = StringTable->insert(argv[0]);
   else
      Con::printf("(TypeString) Cannot set multiple args to a single string.");
}


//////////////////////////////////////////////////////////////////////////
// TypeString
//////////////////////////////////////////////////////////////////////////
ConsoleType( string, TypeString, sizeof(const char*), "" )

ConsoleGetType( TypeString )
{
   return *((const char **)(dptr));
}

ConsoleSetType( TypeString )
{
   if(argc == 1)
      *((const char **) dptr) = StringTable->insert(argv[0]);
   else
      Con::printf("(TypeString) Cannot set multiple args to a single string.");
}

/////////////////////////////////////////////////////////////////////////
// TypeStringEntryVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( string, TypeStringTableEntryVector, sizeof(Vector<StringTableEntry>), "" )

ConsoleGetType( TypeStringTableEntryVector )
{
   Vector<StringTableEntry> *vec = (Vector<StringTableEntry>*)dptr;
   char* returnBuffer = Con::getReturnBuffer(1024);
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<StringTableEntry>::iterator itr = vec->begin(); itr < vec->end(); itr++)
   {
      // concatenate the next value onto the return string
       if ( itr == vec->begin() )
       {
           dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%s", *itr);
       }
       else
       {
           dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, ",%s", *itr);
       }
      returnLeng = dStrlen(returnBuffer);
   }
   return (returnBuffer);
}

ConsoleSetType( TypeStringTableEntryVector )
{
   Vector<StringTableEntry> *vec = (Vector<StringTableEntry>*)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if (argc == 1)
   {
       const char* arg = argv[0];
       const U32 unitCount = StringUnit::getUnitCount(arg, ",");
       for( U32 unitIndex = 0; unitIndex < unitCount; ++unitIndex )
       {
           vec->push_back( StringTable->insert( StringUnit::getUnit(arg, unitIndex, ",") ) );
       }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
      {
            vec->push_back( StringTable->insert( argv[i] ) );
      }
   }
   else
      Con::printf("Vector<String> must be set as { a, b, c, ... } or \"a,b,c, ...\"");
}


//////////////////////////////////////////////////////////////////////////
// TypeCaseString
//////////////////////////////////////////////////////////////////////////
ConsoleType( caseString, TypeCaseString, sizeof(const char*), "" )

ConsoleSetType( TypeCaseString )
{
   if(argc == 1)
      *((const char **) dptr) = StringTable->insert(argv[0], true);
   else
      Con::printf("(TypeCaseString) Cannot set multiple args to a single string.");
}

ConsoleGetType( TypeCaseString )
{
   return *((const char **)(dptr));
}

//////////////////////////////////////////////////////////////////////////
// TypeFileName
//////////////////////////////////////////////////////////////////////////
ConsolePrepType( filename, TypeFilename, sizeof( const char* ), "" )

ConsoleSetType( TypeFilename )
{
   if(argc == 1)
   {
      char buffer[1024];
      if (Con::expandPath(buffer, 1024, argv[0]))
         *((const char **) dptr) = StringTable->insert(buffer);
      else
         Con::warnf("(TypeFilename) illegal filename detected: %s", argv[0]);
   }
   else
      Con::printf("(TypeFilename) Cannot set multiple args to a single filename.");
}

ConsoleGetType( TypeFilename )
{
   return *((const char **)(dptr));
}

ConsolePrepData( TypeFilename )
{
   if( Con::expandPath( buffer, bufferSize, data ) )
      return buffer;
   else
   {
      Con::warnf("(TypeFilename) illegal filename detected: %s", data);
      return data;
   }
}

//////////////////////////////////////////////////////////////////////////
// TypeS8
//////////////////////////////////////////////////////////////////////////
ConsoleType( char, TypeS8, sizeof(U8), "" )

ConsoleGetType( TypeS8 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((U8 *) dptr) );
   return returnBuffer;
}

ConsoleSetType( TypeS8 )
{
   if(argc == 1)
      *((U8 *) dptr) = dAtoi(argv[0]);
   else
      Con::printf("(TypeU8) Cannot set multiple args to a single S32.");
}

//////////////////////////////////////////////////////////////////////////
// TypeS32
//////////////////////////////////////////////////////////////////////////
ConsoleType( int, TypeS32, sizeof(S32), "" )

ConsoleGetType( TypeS32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((S32 *) dptr) );
   return returnBuffer;
}

ConsoleSetType( TypeS32 )
{
   if(argc == 1)
      *((S32 *) dptr) = dAtoi(argv[0]);
   else
      Con::printf("(TypeS32) Cannot set multiple args to a single S32.");
}

//////////////////////////////////////////////////////////////////////////
// TypeS32Vector
//////////////////////////////////////////////////////////////////////////
ConsoleType( intList, TypeS32Vector, sizeof(Vector<S32>), "" )

ConsoleGetType( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32> *)dptr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<S32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      S32 value;
      // advance through the string, pulling off S32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%d", &value) != 0)
      {
         vec->push_back(value);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtoi(argv[i]));
   }
   else
      Con::printf("Vector<S32> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeF32
//////////////////////////////////////////////////////////////////////////
ConsoleType( float, TypeF32, sizeof(F32), "" )

ConsoleGetType( TypeF32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%.9g", *((F32 *) dptr) );
   return returnBuffer;
}
ConsoleSetType( TypeF32 )
{
   if(argc == 1)
      *((F32 *) dptr) = dAtof(argv[0]);
   else
      Con::printf("(TypeF32) Cannot set multiple args to a single F32.");
}

//////////////////////////////////////////////////////////////////////////
// TypeF32Vector
//////////////////////////////////////////////////////////////////////////
ConsoleType( floatList, TypeF32Vector, sizeof(Vector<F32>), "" )

ConsoleGetType( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32> *)dptr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<F32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      F32 value;
      // advance through the string, pulling off F32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%.9g", &value) != 0)
      {
         vec->push_back(value);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtof(argv[i]));
   }
   else
      Con::printf("Vector<F32> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeBool
//////////////////////////////////////////////////////////////////////////
ConsoleType( bool, TypeBool, sizeof(bool), "" )

ConsoleGetType( TypeBool )
{
   return *((bool *) dptr) ? "1" : "0";
}

ConsoleSetType( TypeBool )
{
   if(argc == 1)
      *((bool *) dptr) = dAtob(argv[0]);
   else
      Con::printf("(TypeBool) Cannot set multiple args to a single bool.");
}


//////////////////////////////////////////////////////////////////////////
// TypeBoolVector
//////////////////////////////////////////////////////////////////////////
ConsoleType( boolList, TypeBoolVector, sizeof(Vector<bool>), "" )

ConsoleGetType( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dptr;
   char* returnBuffer = Con::getReturnBuffer(1024);
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<bool>::iterator itr = vec->begin(); itr < vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", (*itr == true ? 1 : 0));
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return(returnBuffer);
}

ConsoleSetType( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if (argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      S32 value;
      // advance through the string, pulling off bool's and advancing the pointer
      while (values < endValues && dSscanf(values, "%d", &value) != 0)
      {
         vec->push_back(value == 0 ? false : true);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtob(argv[i]));
   }
   else
      Con::printf("Vector<bool> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//////////////////////////////////////////////////////////////////////////
// TypeEnum
//////////////////////////////////////////////////////////////////////////
ConsoleType( enumval, TypeEnum, sizeof(S32), "" )

ConsoleGetType( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to getDataTypeEnum()");
   S32 dptrVal = *(S32*)dptr;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (dptrVal == tbl->table[i].index)
      {
         return tbl->table[i].label;
      }
   }

   //not found
   return "";
}

ConsoleSetType( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to setDataTypeEnum()");
   if (argc != 1) return;

   S32 val = 0;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (! dStricmp(argv[0], tbl->table[i].label))
      {
         val = tbl->table[i].index;
         break;
      }
   }
   *((S32 *) dptr) = val;
}

//////////////////////////////////////////////////////////////////////////
// TypeFlag
//////////////////////////////////////////////////////////////////////////
ConsoleType( flag, TypeFlag, sizeof(S32), "" )

ConsoleGetType( TypeFlag )
{
   BitSet32 tempFlags = *(BitSet32 *)dptr;
   if (tempFlags.test(flag)) return "true";
   else return "false";
}

ConsoleSetType( TypeFlag )
{
   bool value = true;
   if (argc != 1)
   {
      Con::printf("flag must be true or false");
   }
   else
   {
      value = dAtob(argv[0]);
   }
   ((BitSet32 *)dptr)->set(flag, value);
}


//////////////////////////////////////////////////////////////////////////
// TypeSimObjectPtr
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectPtr, TypeSimObjectPtr, sizeof(SimObject*), "" )

ConsoleSetType( TypeSimObjectPtr )
{
   if(argc == 1)
   {
      SimObject **obj = (SimObject **)dptr;
      *obj = Sim::findObject(argv[0]);
   }
   else
      Con::printf("(TypeSimObjectPtr) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeSimObjectPtr )
{
   SimObject **obj = (SimObject**)dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   const char* Id =  *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : StringTable->EmptyString;
   dSprintf(returnBuffer, 256, "%s", Id);
   return returnBuffer;
}

//////////////////////////////////////////////////////////////////////////
// TypeSimObjectName
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectName, TypeSimObjectName, sizeof(SimObject*), "" )

ConsoleSetType( TypeSimObjectName )
{
   if(argc == 1)
   {
      SimObject **obj = (SimObject **)dptr;
      *obj = Sim::findObject(argv[0]);
   }
   else
      Con::printf("(TypeSimObjectName) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeSimObjectName )
{
   SimObject **obj = (SimObject**)dptr;
   char* returnBuffer = Con::getReturnBuffer(128);
   dSprintf(returnBuffer, 128, "%s", *obj && (*obj)->getName() ? (*obj)->getName() : "");
   return returnBuffer;
}


//////////////////////////////////////////////////////////////////////////
// TypeSimObjectId
//////////////////////////////////////////////////////////////////////////
ConsoleType( SimObjectId, TypeSimObjectId, sizeof(SimObject*), "" )

ConsoleSetType( TypeSimObjectId )
{
   if(argc == 1)
   {
      SimObject **obj = (SimObject **)dptr;
      *obj = Sim::findObject(argv[0]);
   }
   else
      Con::printf("(TypeSimObjectId) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeSimObjectId )
{
   SimObject **obj = (SimObject**)dptr;
   char* returnBuffer = Con::getReturnBuffer(128);
   dSprintf(returnBuffer, 128, "%s", *obj ? (*obj)->getIdString() : StringTable->EmptyString );
   return returnBuffer;
}


