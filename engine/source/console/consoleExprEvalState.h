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

#ifndef _CONSOLE_EXPREVALSTATE_H_
#define _CONSOLE_EXPREVALSTATE_H_

#ifndef _CONSOLE_DICTIONARY_H_
#include "console/consoleDictionary.h"
#endif

//-----------------------------------------------------------------------------

class ExprEvalState
{
public:
    /// @name Expression Evaluation
    /// @{

    ///
    SimObject *thisObject;
    Dictionary::Entry *currentVariable;
    bool traceOn;
	bool editorModeOn;

    ExprEvalState();
    ~ExprEvalState();

    /// @}

    /// @name Stack Management
    /// @{

    ///
    Dictionary globalVars;
    Vector<Dictionary *> stack;
    void setCurVarName(StringTableEntry name);
    void setCurVarNameCreate(StringTableEntry name);
    S32 getIntVariable();
    F64 getFloatVariable();
    const char *getStringVariable();
    void setIntVariable(S32 val);
    void setFloatVariable(F64 val);
    void setStringVariable(const char *str);

    void pushFrame(StringTableEntry frameName, Namespace *ns);
    void popFrame();

    /// Puts a reference to an existing stack frame
    /// on the top of the stack.
    void pushFrameRef(S32 stackIndex);

    /// @}
};

#endif // _CONSOLE_EXPREVALSTATE_H_
