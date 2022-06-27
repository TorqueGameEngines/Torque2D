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
#include "console/compiler.h"
#include "console/codeBlock.h"
#include "io/resource/resourceManager.h"
#include "math/mMath.h"

#include "debug/telnetDebugger.h"

#ifndef _REMOTE_DEBUGGER_BASE_H_
#include "debug/remote/RemoteDebuggerBase.h"
#endif

using namespace Compiler;

bool           CodeBlock::smInFunction = false;
U32            CodeBlock::smBreakLineCount = 0;
CodeBlock *    CodeBlock::smCodeBlockList = NULL;
CodeBlock *    CodeBlock::smCurrentCodeBlock = NULL;
ConsoleParser *CodeBlock::smCurrentParser = NULL;

//-------------------------------------------------------------------------

CodeBlock::CodeBlock()
{
   globalStrings = NULL;
   functionStrings = NULL;
   globalFloats = NULL;
   functionFloats = NULL;
   lineBreakPairs = NULL;
   breakList = NULL;
   breakListSize = 0;

   refCount = 0;
   code = NULL;
   name = NULL;
   fullPath = NULL;
   modPath = NULL;
   mRoot = StringTable->EmptyString;
}

CodeBlock::~CodeBlock()
{
   // Make sure we aren't lingering in the current code block...
   AssertFatal(smCurrentCodeBlock != this, "CodeBlock::~CodeBlock - Caught lingering in smCurrentCodeBlock!")

   if(name)
      removeFromCodeList();
   delete[] const_cast<char*>(globalStrings);
   delete[] const_cast<char*>(functionStrings);
   delete[] globalFloats;
   delete[] functionFloats;
   delete[] code;
   delete[] breakList;
}

//-------------------------------------------------------------------------

StringTableEntry CodeBlock::getCurrentCodeBlockName()
{
   if (CodeBlock::getCurrentBlock())
      return CodeBlock::getCurrentBlock()->name;
   else
      return NULL;
}   

StringTableEntry CodeBlock::getCurrentCodeBlockFullPath()
{
   if (CodeBlock::getCurrentBlock())
      return CodeBlock::getCurrentBlock()->fullPath;
   else
      return NULL;
}

StringTableEntry CodeBlock::getCurrentCodeBlockModName()
{
   if (CodeBlock::getCurrentBlock())
      return CodeBlock::getCurrentBlock()->modPath;
   else
      return NULL;
}

CodeBlock *CodeBlock::find(StringTableEntry name)
{
   for(CodeBlock *walk = CodeBlock::getCodeBlockList(); walk; walk = walk->nextFile)
      if(walk->name == name)
         return walk;
   return NULL;
}

//-------------------------------------------------------------------------

void CodeBlock::addToCodeList()
{
   // remove any code blocks with my name
   for(CodeBlock **walk = &smCodeBlockList; *walk;walk = &((*walk)->nextFile))
   {
      if((*walk)->name == name)
      {
         *walk = (*walk)->nextFile;
         break;
      }
   }
   nextFile = smCodeBlockList;
   smCodeBlockList = this;
}

void CodeBlock::clearAllBreaks()
{
   if(!lineBreakPairs)
      return;
   for(U32 i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      code[p[1]] = p[0] & 0xFF;
   }
}

void CodeBlock::clearBreakpoint(U32 lineNumber)
{
   if(!lineBreakPairs)
      return;
   for(U32 i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      if((p[0] >> 8) == lineNumber)
      {
         code[p[1]] = p[0] & 0xFF;
         return;
      }
   }
}

void CodeBlock::setAllBreaks()
{
   if(!lineBreakPairs)
      return;
   for(U32 i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      code[p[1]] = OP_BREAK;
   }
}

bool CodeBlock::setBreakpoint(U32 lineNumber)
{
   if(!lineBreakPairs)
      return false;

   for(U32 i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      if((p[0] >> 8) == lineNumber)
      {
         code[p[1]] = OP_BREAK;
         return true;
      }
   }

   return false;
}

U32 CodeBlock::findFirstBreakLine(U32 lineNumber)
{
   if(!lineBreakPairs)
      return 0;

   for(U32 i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      U32 line = (p[0] >> 8);

      if( lineNumber <= line )
         return line;
   }

   return 0;
}

struct LinePair
{
   U32 instLine;
   U32 ip;
};

void CodeBlock::findBreakLine(U32 ip, U32 &line, U32 &instruction)
{
   U32 min = 0;
   U32 max = lineBreakPairCount - 1;
   LinePair *p = (LinePair *) lineBreakPairs;

   U32 found;
   if(!lineBreakPairCount || p[min].ip > ip || p[max].ip < ip)
   {
      line = 0;
      instruction = OP_INVALID;
      return;
   }
   else if(p[min].ip == ip)
      found = min;
   else if(p[max].ip == ip)
      found = max;
   else
   {
      for(;;)
      {
         if(min == max - 1)
         {
            found = min;
            break;
         }
         U32 mid = (min + max) >> 1;
         if(p[mid].ip == ip)
         {
            found = mid;
            break;
         }
         else if(p[mid].ip > ip)
            max = mid;
         else
            min = mid;
      }
   }
   instruction = p[found].instLine & 0xFF;
   line = p[found].instLine >> 8;
}

const char *CodeBlock::getFileLine(U32 ip)
{
   static char nameBuffer[256];
   U32 line, inst;
   findBreakLine(ip, line, inst);

   dSprintf(nameBuffer, sizeof(nameBuffer), "%s (%d)", name ? name : "<input>", line);
   return nameBuffer;
}

void CodeBlock::removeFromCodeList()
{
   for(CodeBlock **walk = &smCodeBlockList; *walk; walk = &((*walk)->nextFile))
   {
      if(*walk == this)
      {
         *walk = nextFile;

         // clear out all breakpoints
         clearAllBreaks();
         break;
      }
   }

   // Let the telnet debugger know that this code
   // block has been unloaded and that it needs to
   // remove references to it.
   if ( TelDebugger )
      TelDebugger->clearCodeBlockPointers( this );

   // Notify the remote debugger.
   RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   if ( pRemoteDebugger != NULL )
       pRemoteDebugger->removeCodeBlock( this );
}

void CodeBlock::calcBreakList()
{
   U32 size = 0;
   S32 line = -1;
   U32 seqCount = 0;
   U32 i;
   for(i = 0; i < lineBreakPairCount; i++)
   {
      U32 lineNumber = lineBreakPairs[i * 2];
      if(lineNumber == U32(line + 1))
         seqCount++;
      else
      {
         if(seqCount)
            size++;
         size++;
         seqCount = 1;
      }
      line = lineNumber;
   }
   if(seqCount)
      size++;

   breakList = new U32[size];
   breakListSize = size;
   line = -1;
   seqCount = 0;
   size = 0;

   for(i = 0; i < lineBreakPairCount; i++)
   {
      U32 lineNumber = lineBreakPairs[i * 2];

      if(lineNumber == U32(line + 1))
         seqCount++;
      else
      {
         if(seqCount)
            breakList[size++] = seqCount;
         breakList[size++] = lineNumber - getMax(0, line) - 1;
         seqCount = 1;
      }

      line = lineNumber;
   }

   if(seqCount)
      breakList[size++] = seqCount;

   for(i = 0; i < lineBreakPairCount; i++)
   {
      U32 *p = lineBreakPairs + i * 2;
      p[0] = (p[0] << 8) | code[p[1]];
   }

   // Let the telnet debugger know that this code
   // block has been loaded and that it can add break
   // points it has for it.
   if ( TelDebugger )
      TelDebugger->addAllBreakpoints( this );

   // Notify the remote debugger.
   RemoteDebuggerBase* pRemoteDebugger = RemoteDebuggerBase::getRemoteDebugger();
   if ( pRemoteDebugger != NULL )
       pRemoteDebugger->addCodeBlock( this );
}

bool CodeBlock::read(StringTableEntry fileName, Stream &st)
{
   const StringTableEntry exePath = Platform::getMainDotCsDir();
   const StringTableEntry cwd = Platform::getCurrentDirectory();

   name = fileName;

   if(fileName)
   {
      fullPath = NULL;

      if(Platform::isFullPath(fileName))
         fullPath = fileName;

      if(dStrnicmp(exePath, fileName, dStrlen(exePath)) == 0)
         name = StringTable->insert(fileName + dStrlen(exePath) + 1, true);
      else if(dStrnicmp(cwd, fileName, dStrlen(cwd)) == 0)
         name = StringTable->insert(fileName + dStrlen(cwd) + 1, true);

      if(fullPath == NULL)
      {
         char buf[1024];
         fullPath = StringTable->insert(Platform::makeFullPathName(fileName, buf, sizeof(buf)), true);
      }

      modPath = Con::getModNameFromPath(fileName);
   }
   
   //
   if (name)
   {
      if (const char *slash = dStrchr(this->name, '/'))
      {
         char root[512];
         dStrncpy(root, this->name, slash-this->name);
         root[slash-this->name] = 0;
         mRoot = StringTable->insert(root);
      }
   }

   //
   addToCodeList();

   U32 globalSize,size,i;
   st.read(&size);
   if(size)
   {
      globalSize = size;
      globalStrings = new char[size];
      st.read(size, globalStrings);
   }
   st.read(&size);
   if(size)
   {
      functionStrings = new char[size];
      st.read(size, functionStrings);
   }
   st.read(&size);
   if(size)
   {
      globalFloats = new F64[size];
      for(U32 i = 0; i < size; i++)
         st.read(&globalFloats[i]);
   }
   st.read(&size);
   if(size)
   {
      functionFloats = new F64[size];
      for(U32 i = 0; i < size; i++)
         st.read(&functionFloats[i]);
   }
   U32 codeSize;
   st.read(&codeSize);
   st.read(&lineBreakPairCount);

   U32 totSize = codeSize + lineBreakPairCount * 2;
   code = new U32[totSize];

   for(i = 0; i < codeSize; i++)
   {
      U8 b;
      st.read(&b);
      if(b == 0xFF)
         st.read(&code[i]);
      else
         code[i] = b;
   }

   for(i = codeSize; i < totSize; i++)
      st.read(&code[i]);

   lineBreakPairs = code + codeSize;

   // StringTable-ize our identifiers.
   U32 identCount;
   st.read(&identCount);
   while(identCount--)
   {
      U32 offset;
      st.read(&offset);
      StringTableEntry ste;
      if(offset < globalSize)
         ste = StringTable->insert(globalStrings + offset);
      else
         ste = StringTable->EmptyString;
      
      U32 count;
      st.read(&count);
      while(count--)
      {
         U32 ip;
         st.read(&ip);
#ifdef TORQUE_CPU_X64
         *(U64*)(code+ip) = (U64)ste;
#else
         code[ip] = (U32)ste;
#endif
      }
   }

   if(lineBreakPairCount)
      calcBreakList();

   return true;
}


bool CodeBlock::compile(const char *codeFileName, StringTableEntry fileName, const char *script)
{
   gSyntaxError = false;

   consoleAllocReset();

   STEtoCode = compileSTEtoCode;

   statementList = NULL;

   // Set up the parser.
   smCurrentParser = getParserForFile(fileName);
   AssertISV(smCurrentParser, avar("CodeBlock::compile - no parser available for '%s'!", fileName));

   // Now do some parsing.
   smCurrentParser->setScanBuffer(script, fileName);
   smCurrentParser->restart(NULL);
   smCurrentParser->parse();

   if(gSyntaxError)
   {
      consoleAllocReset();
      return false;
   }   

   FileStream st;
   if(!ResourceManager->openFileForWrite(st, codeFileName)) 
      return false;
   st.write(DSO_VERSION);

   // Reset all our value tables...
   resetTables();

   smInFunction = false;
   smBreakLineCount = 0;
   setBreakCodeBlock(this);

   if(statementList)
      codeSize = precompileBlock(statementList, 0) + 1;
   else
      codeSize = 1;

   lineBreakPairCount = smBreakLineCount;
   code = new U32[codeSize + smBreakLineCount * 2];
   lineBreakPairs = code + codeSize;

   // Write string table data...
   getGlobalStringTable().write(st);
   getFunctionStringTable().write(st);

   // Write float table data...
   getGlobalFloatTable().write(st);
   getFunctionFloatTable().write(st);

   smBreakLineCount = 0;
   U32 lastIp;
   if(statementList)
      lastIp = compileBlock(statementList, code, 0, 0, 0);
   else
      lastIp = 0;

   if(lastIp != codeSize - 1)
      Con::errorf(ConsoleLogEntry::General, "CodeBlock::compile - precompile size mismatch, a precompile/compile function pair is probably mismatched.");

   code[lastIp++] = OP_RETURN;
   U32 totSize = codeSize + smBreakLineCount * 2;
   st.write(codeSize);
   st.write(lineBreakPairCount);

   // Write out our bytecode, doing a bit of compression for low numbers.
   U32 i;   
   for(i = 0; i < codeSize; i++)
   {
      if(code[i] < 0xFF)
         st.write(U8(code[i]));
      else
      {
         st.write(U8(0xFF));
         st.write(code[i]);
      }
   }

   // Write the break info...
   for(i = codeSize; i < totSize; i++)
      st.write(code[i]);

   getIdentTable().write(st);

   consoleAllocReset();
   st.close();

   return true;
}

const char *CodeBlock::compileExec(StringTableEntry fileName, const char *string, bool noCalls, int setFrame)
{
   STEtoCode = evalSTEtoCode;
   consoleAllocReset();

   name = fileName;

   if(fileName)
   {
      const StringTableEntry exePath = Platform::getMainDotCsDir();
      const StringTableEntry cwd = Platform::getCurrentDirectory();

      fullPath = NULL;
      
      if(Platform::isFullPath(fileName))
         fullPath = fileName;

      if(dStrnicmp(exePath, fileName, dStrlen(exePath)) == 0)
         name = StringTable->insert(fileName + dStrlen(exePath) + 1, true);
      else if(dStrnicmp(cwd, fileName, dStrlen(cwd)) == 0)
         name = StringTable->insert(fileName + dStrlen(cwd) + 1, true);

      if(fullPath == NULL)
      {
         char buf[1024];
         fullPath = StringTable->insert(Platform::makeFullPathName(fileName, buf, sizeof(buf)), true);
      }

      modPath = Con::getModNameFromPath(fileName);
   }

   if(name)
      addToCodeList();
   
   statementList = NULL;

   // Set up the parser.
   smCurrentParser = getParserForFile(fileName);
   AssertISV(smCurrentParser, avar("CodeBlock::compile - no parser available for '%s'!", fileName));

   // Now do some parsing.
   smCurrentParser->setScanBuffer(string, fileName);
   smCurrentParser->restart(NULL);
   smCurrentParser->parse();

   if(!statementList)
   {
      delete this;
      return "";
   }

   resetTables();

   smInFunction = false;
   smBreakLineCount = 0;
   setBreakCodeBlock(this);

   codeSize = precompileBlock(statementList, 0) + 1;

   lineBreakPairCount = smBreakLineCount;

   globalStrings   = getGlobalStringTable().build();
   functionStrings = getFunctionStringTable().build();
   globalFloats    = getGlobalFloatTable().build();
   functionFloats  = getFunctionFloatTable().build();

   code = new U32[codeSize + lineBreakPairCount * 2];
   lineBreakPairs = code + codeSize;

   smBreakLineCount = 0;
   U32 lastIp = compileBlock(statementList, code, 0, 0, 0);
   code[lastIp++] = OP_RETURN;
   
   consoleAllocReset();

   if(lineBreakPairCount && fileName)
      calcBreakList();

   if(lastIp != codeSize)
      Con::warnf(ConsoleLogEntry::General, "precompile size mismatch");

   return exec(0, fileName, NULL, 0, 0, noCalls, NULL, setFrame);
}

//-------------------------------------------------------------------------

void CodeBlock::incRefCount()
{
   refCount++;
}

void CodeBlock::decRefCount()
{
   refCount--;
   if(!refCount)
      delete this;
}
