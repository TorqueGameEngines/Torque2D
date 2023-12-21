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

#include "consoleExprEvalState.h"
#include "consoleNamespace.h"

#include "platform/platform.h"
#include "console/console.h"

#include "console/ast.h"
#include "collection/findIterator.h"
#include "io/resource/resourceManager.h"

#include "string/findMatch.h"
#include "io/fileStream.h"
#include "console/compiler.h"

#include "consoleExprEvalState_ScriptBinding.h"

extern ExprEvalState gEvalState;

void ExprEvalState::pushFrame(StringTableEntry frameName, Namespace *ns)
{
   Dictionary *newFrame = new Dictionary(this);
   newFrame->scopeName = frameName;
   newFrame->scopeNamespace = ns;
   stack.push_back(newFrame);
}

void ExprEvalState::popFrame()
{
   Dictionary *last = stack.last();
   stack.pop_back();
   delete last;
}

void ExprEvalState::pushFrameRef(S32 stackIndex)
{
   AssertFatal( stackIndex >= 0 && stackIndex < stack.size(), "You must be asking for a valid frame!" );
   Dictionary *newFrame = new Dictionary(this, stack[stackIndex]);
   stack.push_back(newFrame);
}

ExprEvalState::ExprEvalState()
{
   VECTOR_SET_ASSOCIATION(stack);
   globalVars.setState(this);
   thisObject = NULL;
   traceOn = false;
   editorModeOn = false;
}

ExprEvalState::~ExprEvalState()
{
   while(stack.size())
      popFrame();
}
