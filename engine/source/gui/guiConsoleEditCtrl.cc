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
 
#include "console/consoleTypes.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/guiConsoleEditCtrl.h"
#include "memory/frameAllocator.h"

GuiConsoleEditHistory::GuiConsoleEditHistory() 
{ 
    historyList = vector<string>();
}

const string& GuiConsoleEditHistory::getPrevHistory()
{
    index = static_cast<U32>(getMax(static_cast<S32>(index - 1), 0));
    return historyList.size() == 0 ? workingText : historyList[index];
}
const string& GuiConsoleEditHistory::getNextHistory()
{
    index = getMin(index + 1, historyList.size());
    return historyList.size() == index ? workingText : historyList[index];
}


IMPLEMENT_CONOBJECT(GuiConsoleEditCtrl);

GuiConsoleEditCtrl::GuiConsoleEditCtrl()
{
   mSinkAllKeyEvents = true;
   mSiblingScroller = NULL;
   mUseSiblingScroller = true;
   mReturnCausesTab = false;
   mHistory = GuiConsoleEditHistory();
}

void GuiConsoleEditCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("GuiConsoleEditCtrl");
   addField("useSiblingScroller", TypeBool, Offset(mUseSiblingScroller, GuiConsoleEditCtrl));
   endGroup("GuiConsoleEditCtrl");
}

bool GuiConsoleEditCtrl::onKeyDown(const GuiEvent& event)
{
    setUpdate();

    if (event.keyCode == KEY_TAB)
    {
        // Get a buffer that can hold the completed text...
        FrameTemp<UTF8> tmpBuff(GuiTextEditCtrl::MAX_STRING_LENGTH);
        // And copy the text to be completed into it.
        StringBuffer strBuff(mTextBuffer.c_str());
        strBuff.getCopy8(tmpBuff, GuiTextEditCtrl::MAX_STRING_LENGTH);

        // perform the completion
        bool forward = event.modifier & SI_SHIFT;
        //mCursorPos = Con::tabComplete(tmpBuff, mCursorPos, GuiTextEditCtrl::MAX_STRING_LENGTH, forward);

        // place results in our buffer.
        mTextBuffer.assign(tmpBuff);
        return true;
    }
    else if ((event.keyCode == KEY_PAGE_UP) || (event.keyCode == KEY_PAGE_DOWN))
    {
        // See if there's some other widget that can scroll the console history.
        if (mUseSiblingScroller)
        {
            if (mSiblingScroller)
            {
                return mSiblingScroller->onKeyDown(event);
            }
            else
            {
                // Let's see if we can find it...
                SimGroup* pGroup = getGroup();
                if (pGroup)
                {
                    // Find the first scroll control in the same group as us.
                    for (SimSetIterator itr(pGroup); *itr; ++itr)
                    {
                        if ((mSiblingScroller = dynamic_cast<GuiScrollCtrl*>(*itr)))
                        {
                            return mSiblingScroller->onKeyDown(event);
                        }
                    }
                }

                // No luck... so don't try, next time.
                mUseSiblingScroller = false;
            }
        }
    }

   return Parent::onKeyDown(event);
}

bool GuiConsoleEditCtrl::handleEnterKey()
{
    mHistory.updateHistory(mTextBuffer);
    
    return Parent::handleEnterKey();
}

bool GuiConsoleEditCtrl::handleArrowKey(GuiDirection direction)
{
    if (!mTextWrap && (direction == GuiDirection::Up || direction == GuiDirection::Down))
    {
        string historyText = (direction == GuiDirection::Up ? mHistory.getPrevHistory() : mHistory.getNextHistory());
        setText(historyText);
        mSelector.setTextLength(historyText.length());
        mSelector.setCursorPosition(historyText.length());
        return true;
    }
    else
    {
        return Parent::handleArrowKey(direction);
    }
}

bool GuiConsoleEditCtrl::handleCharacterInput(const GuiEvent& event)
{
    string before = mTextBuffer;
    bool result = Parent::handleCharacterInput(event);

    if (before != mTextBuffer)
    {
        mHistory.setWorkingText(mTextBuffer);
    }
    return result;
}

bool GuiConsoleEditCtrl::handleBackSpace()
{
    string before = mTextBuffer;
    bool result = Parent::handleBackSpace();

    if (before != mTextBuffer)
    {
        mHistory.setWorkingText(mTextBuffer);
    }
    return result;
}

bool GuiConsoleEditCtrl::handleDelete()
{
    string before = mTextBuffer;
    bool result = Parent::handleDelete();

    if (before != mTextBuffer)
    {
        mHistory.setWorkingText(mTextBuffer);
    }
    return result;
}


