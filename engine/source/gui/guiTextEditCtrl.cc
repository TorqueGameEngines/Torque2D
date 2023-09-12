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
#include "graphics/gColor.h"
#include "graphics/dgl.h"
#include "gui/guiCanvas.h"
#include "gui/guiTextEditCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "memory/frameAllocator.h"
#include "string/unicode.h"

#include <vector>
#include <string>

#include "guiTextEditCtrl_ScriptBinding.h"

#pragma region GuiTextEditTextBlock
GuiTextEditTextBlock::GuiTextEditTextBlock()
{
    mGlobalBounds.set(0, 0, 0, 0);
    mTextOffsetX = 0;
    mTextScrollX = 0;
    mText = string();
    mLineStartIbeamValue = 0;
}

void GuiTextEditTextBlock::render(const RectI& bounds, string line, U32 ibeamStartValue, GuiControlProfile* profile, GuiControlState currentState, GuiTextEditSelection& selector, AlignmentType align, GFont* font, bool overrideFontColor)
{
    mGlobalBounds.set(bounds.point, bounds.extent);
    mText.assign(line);
    mLineStartIbeamValue = ibeamStartValue;

    RectI clipRect = dglGetClipRect();
    if (mGlobalBounds.overlaps(clipRect))
    {
        const U32 selStart = selector.getSelStart();
        const U32 selEnd = selector.getSelEnd();

        U32 lengthOfPreBlockText = mClamp(selStart - mLineStartIbeamValue, 0, line.length());
        U32 lengthOfPostBlockText = mClamp(line.length() - (selEnd - mLineStartIbeamValue), 0, line.length());
        U32 lengthOfHighlightBlockText = mClamp(line.length() - (lengthOfPreBlockText + lengthOfPostBlockText), 0, line.length());

        processTextAlignment(line, font, align);
        Point2I movingStartPoint = getGlobalTextStart();
        movingStartPoint.x += renderTextSection(movingStartPoint, 0, lengthOfPreBlockText, profile, currentState, font, false, overrideFontColor);
        movingStartPoint.x += renderTextSection(movingStartPoint, lengthOfPreBlockText, lengthOfHighlightBlockText, profile, currentState, font, true, overrideFontColor);
        movingStartPoint.x += renderTextSection(movingStartPoint, lengthOfPreBlockText + lengthOfHighlightBlockText, lengthOfPostBlockText, profile, currentState, font, false, overrideFontColor);
    }

    Point2I textStartPoint = getGlobalTextStart();
    if (selector.renderIbeam(textStartPoint, mGlobalBounds.extent, line, mLineStartIbeamValue, mLineStartIbeamValue + line.length(), profile, font))
    {
        Point2I cursorCenter = selector.getCursorCenter();
        performScrollJumpX(cursorCenter.x, clipRect.point.x, clipRect.point.x + clipRect.extent.x);
    }
}

U32 GuiTextEditTextBlock::renderTextSection(const Point2I& startPoint, const U32 subStrStart, const U32 subStrLen, GuiControlProfile* profile, const GuiControlState currentState, GFont* font, bool isSelectedText, bool overrideFontColor)
{
    if (subStrLen != 0)
    {
        string sectionText = mText.substr(subStrStart, subStrLen);
        U32 blockStrWidth = font->getStrWidth(sectionText.c_str());
        Point2I pointToDraw = Point2I(startPoint.x, startPoint.y);
        if (isSelectedText)
        {
            if (!overrideFontColor)
            {
                dglSetBitmapModulation(profile->mFontColorTextSL);
            }
            RectI highlightRect = RectI(pointToDraw.x, pointToDraw.y, blockStrWidth, mGlobalBounds.extent.y);
            dglDrawRectFill(highlightRect, profile->mFillColorTextSL);
        }
        else if(!overrideFontColor)
        {
            const ColorI& fontColor = profile->getFontColor(currentState);
            dglSetBitmapModulation(fontColor);
        }
        dglDrawText(font, pointToDraw, sectionText.c_str(), profile->mFontColors);
        return blockStrWidth;
    }
    return 0;
}

void GuiTextEditTextBlock::performScrollJumpX(const S32 targetX, const S32 areaStart, const S32 areaEnd)
{
    S32 diff = 0;
    if (targetX < areaStart)
    {
        diff = targetX - areaStart;
    }
    else if (targetX > areaEnd)
    {
        diff = targetX - areaEnd;
    }
    mTextScrollX += diff;
}

U32 GuiTextEditTextBlock::calculateIbeamPositionInLine(const S32 targetX, GFont* font)
{
    if (mText.length() == 0)
        return mLineStartIbeamValue;

    S32 curX = getGlobalTextStart().x;
    U32 result = mText.length();

    for (U32 count = 0; count < mText.length(); count++)
    {
        char c = mText[count];
        if (!font->isValidChar(c))
            continue;

        S32 backDiff = mAbs(curX - targetX);
        curX += font->getCharXIncrement(c);

        if (curX > targetX)
        {
            S32 forwardDiff = mAbs(curX - targetX);
            if (backDiff < forwardDiff)
                result = count;
            else
                result = count + 1;
            break;
        }
    }

    return result;
}

void GuiTextEditTextBlock::processScrollVelocity(const S32 delta, const S32 extentX, GFont* font)
{
    U32 max = font->getStrWidth(mText.c_str()) - extentX;
    mTextScrollX = mClamp(mTextScrollX + delta, 0, max);
}

void GuiTextEditTextBlock::processTextAlignment(const string line, GFont* font, AlignmentType align)
{
    if (align == AlignmentType::LeftAlign ||
        mGlobalBounds.extent.x < font->getStrWidth(line.c_str()))
    {
        mTextOffsetX = 0;
    }
    else if (align == AlignmentType::RightAlign)
    {
        mTextOffsetX = mGlobalBounds.extent.x - font->getStrWidth(line.c_str());
    }
    else if (align == AlignmentType::CenterAlign)
    {
        mTextOffsetX = (S32)mRound((mGlobalBounds.extent.x - font->getStrWidth(line.c_str())) / 2);
    }
}
#pragma endregion

#pragma region GuiTextEditSelection
GuiTextEditSelection::GuiTextEditSelection()
{
    mBlockStart = 0;
    mBlockEnd = 0;
    mCursorPos = 0;
    mCursorOn = false;
    mNumFramesElapsed = 0;
    mCursorAtEOL = false;
    mIsFirstResponder = false;
    mGlobalUnadjustedCursorRect.set(0, 0, 0, 0);
    mCursorRendered = false;

    mNumFramesElapsed = 0;
    mTimeLastCursorFlipped = 0;
    mCursorOn = false;
}

bool GuiTextEditSelection::renderIbeam(const Point2I& startPoint, const Point2I& extent, const string line, const U32 start, const U32 end, GuiControlProfile* profile, GFont* font)
{
    if (!mIsFirstResponder || !mCursorOn ||
        (mCursorAtEOL && mCursorPos == start && mCursorPos != 0) ||
        (!mCursorAtEOL && mCursorPos == end && mCursorPos != mTextLength) ||
        (mCursorPos < start || mCursorPos > end))
    {
        return false;
    }

    string blockText = line.substr(0, mCursorPos - start);
    U32 blockStrWidth = font->getStrWidth(blockText.c_str());
    RectI ibeamRect = RectI(startPoint.x + blockStrWidth - 1, startPoint.y, 2, extent.y);
    setCursorRect(ibeamRect);

    RectI clipRect = dglGetClipRect();
    if (clipRect.point.x > ibeamRect.point.x)
    {
        ibeamRect.point.x = clipRect.point.x;
    }
    else if ((clipRect.point.x + clipRect.extent.x) < (ibeamRect.point.x + ibeamRect.extent.x))
    {
        ibeamRect.point.x = (clipRect.point.x + clipRect.extent.x) - ibeamRect.extent.x;
    }
    dglDrawRectFill(ibeamRect, profile->mCursorColor);

    return true;
}

void GuiTextEditSelection::selectTo(const U32 target)
{
    S32 safeTarget = mClamp(target, 0, mTextLength);

    if (mBlockStart == mBlockEnd)
    {
        mBlockAnchor = mBlockStart = mBlockEnd = mCursorPos;
    }

    if (safeTarget > mBlockAnchor)
    {
        mBlockStart = mBlockAnchor;
        mBlockEnd = mCursorPos = safeTarget;
    }
    else if (safeTarget < mBlockAnchor)
    {
        mBlockStart = mCursorPos = safeTarget;
        mBlockEnd = mBlockAnchor;
    }
    else
    {
        mCursorPos = mBlockStart = mBlockEnd = safeTarget;
    }
}

void GuiTextEditSelection::eraseSelection(string& fullText)
{
    if (hasSelection())
    {
        fullText.erase(mBlockStart, mBlockEnd - mBlockStart);
        mCursorPos = mBlockStart;
        mTextLength = fullText.length();
        clearSelection();
    }
}

void GuiTextEditSelection::onPreRender(const U32 time)
{
    if (mIsFirstResponder)
    {
        U32 timeElapsed = time - mTimeLastCursorFlipped;
        mNumFramesElapsed++;
        if ((timeElapsed > 400) && (mNumFramesElapsed > 3))
        {
            mCursorOn = !mCursorOn;
            mTimeLastCursorFlipped = time;
            mNumFramesElapsed = 0;
        }
    }
}

void GuiTextEditSelection::resetCursorBlink()
{
    mCursorOn = true;
    mNumFramesElapsed = 0;
    mTimeLastCursorFlipped = Platform::getVirtualMilliseconds();
}

void GuiTextEditSelection::selectWholeWord(const string& text)
{
    bool selectingSpace = (mCursorPos < text.length() && text[mCursorPos] == ' ');

    for (S32 i = mCursorPos; i >= 0; i--)
    {
        if (i == 0)
        {
            mBlockStart = 0;
            break;
        }

        if ((!selectingSpace && text[i - 1] == ' ') ||
            (selectingSpace && text[i - 1] != ' '))
        {
            mBlockStart = i;
            break;
        }
    }
    mBlockAnchor = mBlockStart;

    bool foundSpace = false;
    for (S32 j = mCursorPos; j <= (text.length() + 1); j++)
    {
        if (j == (text.length() + 1))
        {
            mBlockEnd = j;
            break;
        }

        if (!foundSpace && text[j] == ' ')
        {
            foundSpace = true;
        }
        else if (foundSpace && text[j] != ' ')
        {
            mBlockEnd = j;
            break;
        }
    }
    mCursorPos = mBlockEnd;
    mCursorAtEOL = true;
}

GuiTextEditSelection::GuiTextEditSelection(const GuiTextEditSelection& selector)
{
    mBlockAnchor = selector.mBlockAnchor;
    mBlockStart = selector.mBlockStart;
    mBlockEnd = selector.mBlockEnd;
    mCursorPos = selector.mCursorPos;
    mCursorAtEOL = selector.mCursorAtEOL;
    mIsFirstResponder = selector.mIsFirstResponder;
    mGlobalUnadjustedCursorRect = selector.mGlobalUnadjustedCursorRect;
    mCursorRendered = selector.mCursorRendered;
    mTextLength = selector.mTextLength;

    mNumFramesElapsed = 0;
    mTimeLastCursorFlipped = Platform::getVirtualMilliseconds();
    mCursorOn = true;
}

void GuiTextEditSelection::stepCursorForward()
{
    if (hasSelection())
    {
        setCursorPosition(mBlockEnd);
    }
    else
    {
        setCursorPosition(mCursorPos + 1);
    }
}

void GuiTextEditSelection::stepCursorBackward()
{
    if (hasSelection())
    {
        setCursorPosition(mBlockStart);
    }
    else
    {
        setCursorPosition(mCursorPos - 1);
    }
}

#pragma endregion

#pragma region GuiTextEditCtrl
IMPLEMENT_CONOBJECT(GuiTextEditCtrl);

U32 GuiTextEditCtrl::smNumAwake = 0;

GuiTextEditCtrl::GuiTextEditCtrl()
{
   mInsertOn = true;
   mMouseOver = false;
   mPasswordText = false;
   mReturnCausesTab = false;
   mSinkAllKeyEvents = false;
   mActive = true;

   mSelector = GuiTextEditSelection();
   mUndoSelector = GuiTextEditSelection();
   mUndoText = string();

   mTextOffsetY = 0;

   mReturnCommand = StringTable->EmptyString;
   mEscapeCommand = StringTable->EmptyString;
   mPasswordMask = StringTable->insert( "*" );

   mEditCursor = NULL;
   mMaxStrLen = MAX_STRING_LENGTH;

   mInputMode = AllText;
   mSuspendVerticalScrollJump = false;
   mTextBlockList = TextBlockList();
   mScrollVelocity = 0;
   mIsContainer = false;
   mBounds.extent.set(220, 30);
}

static EnumTable::Enums inputModeEnums[] =
{
	{ GuiTextEditCtrl::AllText, "AllText" },
	{ GuiTextEditCtrl::Decimal, "Decimal" },
	{ GuiTextEditCtrl::Number, "Number" },
	{ GuiTextEditCtrl::Alpha, "Alpha" },
	{ GuiTextEditCtrl::AlphaNumeric, "AlphaNumeric" }
};
static EnumTable gInputModeTable(5, &inputModeEnums[0]);

void GuiTextEditCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addDepricatedField("validate");
   addDepricatedField("truncate");
   addDepricatedField("passwordMask");
   addDepricatedField("historySize");
   addDepricatedField("tabComplete");

   addGroup("Text Edit");
   addField("returnCommand", TypeString, Offset(mReturnCommand, GuiTextEditCtrl));
   addField("escapeCommand", TypeString, Offset(mEscapeCommand, GuiTextEditCtrl));
   addField("sinkAllKeyEvents",  TypeBool,      Offset(mSinkAllKeyEvents,  GuiTextEditCtrl));
   addField("password", TypeBool, Offset(mPasswordText, GuiTextEditCtrl));
   addField("returnCausesTab", TypeBool, Offset(mReturnCausesTab, GuiTextEditCtrl));
   addProtectedField("maxLength", TypeS32,		Offset(mMaxStrLen,			GuiTextEditCtrl), &setMaxLengthProperty, &defaultProtectedGetFn, "The max number of characters that can be entered into the text edit box.");
   addProtectedField("inputMode", TypeEnum,		Offset(mInputMode,			GuiTextEditCtrl), &setInputMode, &getInputMode, &writeInputMode, 1, &gInputModeTable, "InputMode allows different characters to be entered.");
   addField("editCursor", TypeGuiCursor, Offset(mEditCursor, GuiTextEditCtrl));
   endGroup("Text Edit");
}

bool GuiTextEditCtrl::onAdd()
{
   if ( ! Parent::onAdd() )
      return false;

   if( mText[0] )
   {
      setText(mText);
   }

   return true;
}

void GuiTextEditCtrl::onStaticModified(const char* slotName)
{
   if(!dStricmp(slotName, "text"))
      setText(mText);
}

void GuiTextEditCtrl::inspectPostApply()
{
	Parent::inspectPostApply();
	if (mTextID && *mTextID != 0)
		setTextID(mTextID);
	else
		setText(mText);
}

bool GuiTextEditCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   if (mConsoleVariable[0])
   {
	   const char *txt = Con::getVariable(mConsoleVariable);
	   if (txt)
	   {
		   if (dStrlen(txt) > (U32)mMaxStrLen)
		   {
			   char* buf = new char[mMaxStrLen + 1];
			   dStrncpy(buf, txt, mMaxStrLen);
			   buf[mMaxStrLen] = 0;
			   setScriptValue(buf);
			   delete[] buf;
		   }
		   else
			   setScriptValue(txt);
	   }
   }

   // If this is the first awake text edit control, enable keyboard translation
   if (smNumAwake == 0)
      Platform::enableKeyboardTranslation();
   ++smNumAwake;

   mSuspendVerticalScrollJump = false;

   return true;
}

void GuiTextEditCtrl::onSleep()
{
   Parent::onSleep();

   // If this is the last awake text edit control, disable keyboard translation
   --smNumAwake;
   if (smNumAwake == 0)
      Platform::disableKeyboardTranslation();
}

void GuiTextEditCtrl::execConsoleCallback()
{
    setVariable(mTextBuffer.c_str());
    if ( mConsoleCommand[0] )
    {
        Con::evaluate( mConsoleCommand, false );
    }
}

const char* GuiTextEditCtrl::getText()
{
    return mTextBuffer.c_str();
}  
 
void GuiTextEditCtrl::setText( const UTF8 *txt )
{
	setUpdate();
    enforceMaxLength();

    if (txt && txt[0] != 0)
    {
        Parent::setText(txt);
        mTextBuffer.assign(txt);
    }
    else
        mTextBuffer.clear();

    setVariable(mTextBuffer.c_str());
}

void GuiTextEditCtrl::setText( const UTF16* txt)
{
   if(txt && txt[0] != 0)
   {
      UTF8* txt8 = convertUTF16toUTF8( txt );
	  setText(txt8);
      delete[] txt8;
   }
   else
   {
      setText("");
   }  
}

void GuiTextEditCtrl::enforceMaxLength()
{
    int diff = mTextBuffer.length() - mMaxStrLen;
    if (diff > 0) {
        mTextBuffer.resize(mMaxStrLen);
    }
}

void GuiTextEditCtrl::setTextID(const char *id)
{
	S32 n = Con::getIntVariable(id, -1);
	if (n != -1)
	{
		setTextID(n);
	}
}
void GuiTextEditCtrl::setTextID(S32 id)
{
	const UTF8 *str = getGUIString(id);
	if (str)
		setText((const char*)str);
}

bool GuiTextEditCtrl::validate()
{
	bool valid = true;
	if (isMethod("onValidate"))
	{
		valid = dAtob(Con::executef(this, 2, "onValidate"));
	}
	return valid;
}

const RectI GuiTextEditCtrl::getGlobalInnerRect()
{
    RectI innerRect = getInnerRect(SelectedState);

    Point2I globalCtrlOffset = localToGlobalCoord(innerRect.point);
    RectI globalInnerRect(globalCtrlOffset, innerRect.extent);

    return globalInnerRect;
}

S32 GuiTextEditCtrl::calculateIbeamPosition(const Point2I& globalMousePoint)
{
    if (mTextBuffer.length() == 0)
        return 0;

    RectI globalInnerRect = getGlobalInnerRect();
    return calculateIbeamPosition(globalMousePoint, globalInnerRect);
}

S32 GuiTextEditCtrl::calculateIbeamPosition(const Point2I& globalMousePoint, const RectI& globalInnerRect)
{
     if (mTextBuffer.length() == 0 || mTextBlockList.size() == 0)
        return 0;

    string textBuffer = applyPasswordMasking();
    GFont* font = mProfile->getFont(mFontSizeAdjust);

    if (!mTextWrap)
    {
        return mTextBlockList.front().calculateIbeamPositionInLine(globalMousePoint.x, font);
    }
    else
    {
        RectI firstLineBounds = mTextBlockList.front().getGlobalBounds();
        S32 textStartY = firstLineBounds.point.y;

        if (textStartY > globalMousePoint.y)
            return 0;

        U32 height = firstLineBounds.extent.y;

        if ((textStartY + (mTextBlockList.size() * height)) < globalMousePoint.y)
            return mTextBuffer.length();

        S32 curY = textStartY;
        for (auto block : mTextBlockList)
        {
            curY += height;
            if (curY > globalMousePoint.y)
            {
                U32 pos = block.calculateIbeamPositionInLine(globalMousePoint.x, font);
                mSelector.setCursorAtEOL(block.calculateCursorAtEOL(pos));
                return pos + block.getStartValue();
            }
        }
        mSelector.setCursorAtEOL(true);
        return mTextBlockList.back().calculateIbeamPositionInLine(globalMousePoint.x, font);
    }
}

void GuiTextEditCtrl::onTouchDown( const GuiEvent &event )
{
    mScrollVelocity = 0;
    mSuspendVerticalScrollJump = false;

    if (!mVisible || !mAwake)
        return;

   mouseLock();
   setFirstResponder();
   mSelector.resetCursorBlink();

    mSelector.setTextLength(mTextBuffer.length());
   if(event.mouseClickCount == 2)
   {
       mSelector.selectWholeWord(mTextBuffer);
   }
   else if (event.mouseClickCount < 2)
   {
       S32 newCursorPos = calculateIbeamPosition(event.mousePoint);
       if (event.modifier & SI_SHIFT)
       {
           modifySelectBlock(newCursorPos);
       }
       else
       {
           mSelector.setCursorPosition(newCursorPos);
       }
   }
    if( isMethod("onTouchDown") )
    {
        char buf[3][32];
        dSprintf(buf[0], 32, "%d", event.modifier);
        dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
        dSprintf(buf[2], 32, "%d", event.mouseClickCount);
        Con::executef(this, 4, "onTouchDown", buf[0], buf[1], buf[2]);
    }
}

void GuiTextEditCtrl::onTouchDragged( const GuiEvent &event )
{
    if (!mVisible || !mAwake)
        return;

    mSuspendVerticalScrollJump = false;
    RectI globalInnerRect = getGlobalInnerRect();
    adjustScrollVelocity(event.mousePoint, globalInnerRect);
    
    if ((mTextWrap && event.mousePoint.y < globalInnerRect.point.y) || 
        (!mTextWrap && event.mousePoint.x < globalInnerRect.point.x))
    {
        modifySelectBlock(0);
    }
    else if ((mTextWrap && event.mousePoint.y > (globalInnerRect.point.y + globalInnerRect.extent.y)) ||
        (!mTextWrap && event.mousePoint.x > (globalInnerRect.point.x + globalInnerRect.extent.x)))
    {
        modifySelectBlock(mTextBuffer.length());
    }
    else
    {
        modifySelectBlock(calculateIbeamPosition(event.mousePoint, globalInnerRect));
    }

   // Notify Script.
    if( isMethod("onTouchDragged") )
    {
        char buf[3][32];
        dSprintf(buf[0], 32, "%d", event.modifier);
        dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
        dSprintf(buf[2], 32, "%d", event.mouseClickCount);
        Con::executef(this, 4, "onTouchDragged", buf[0], buf[1], buf[2]);
    }
}

void GuiTextEditCtrl::onTouchUp(const GuiEvent &event)
{
    mScrollVelocity = 0;
    mSuspendVerticalScrollJump = false;
   mouseUnlock();

   if (!mVisible || !mAwake)
       return;

    // Notify Script.
    if( isMethod("onTouchUp") )
    {
        char buf[3][32];
        dSprintf(buf[0], 32, "%d", event.modifier);
        dSprintf(buf[1], 32, "%d %d", event.mousePoint.x, event.mousePoint.y);
        dSprintf(buf[2], 32, "%d", event.mouseClickCount);
        Con::executef(this, 4, "onTouchUp", buf[0], buf[1], buf[2]);
    }
}

void GuiTextEditCtrl::onMouseWheelUp(const GuiEvent& event)
{
    if (!mVisible || !mAwake)
        return;

    if(mTextWrap && mTextOffsetY > 0)
    {
        mScrollVelocity = 0;
        mSuspendVerticalScrollJump = true;
        mTextOffsetY = getMax(mTextOffsetY - static_cast<S32>(mProfile->getFont(mFontSizeAdjust)->getHeight()), 0);
        return;
    }

    GuiControl* parent = getParent();
    if (parent)
        parent->onMouseWheelUp(event);
}

void GuiTextEditCtrl::onMouseWheelDown(const GuiEvent& event)
{
    if (!mVisible || !mAwake)
        return;

    U32 blockHeight = mTextBlockList.size() * mProfile->getFont(mFontSizeAdjust)->getHeight();
    RectI innerRect = getGlobalInnerRect();
    S32 max = blockHeight - innerRect.extent.y;
    if (mTextWrap && innerRect.extent.y < blockHeight && mTextOffsetY < max)
    {
        mScrollVelocity = 0;
        mSuspendVerticalScrollJump = true;
        mTextOffsetY = getMin(mTextOffsetY + static_cast<S32>(mProfile->getFont(mFontSizeAdjust)->getHeight()), max);
        return;
    }

    GuiControl* parent = getParent();
    if (parent)
        parent->onMouseWheelDown(event);
}

void GuiTextEditCtrl::onTouchEnter(const GuiEvent& event)
{
    if (!mActive)
        return;

    mMouseOver = true;

    Con::executef(this, 1, "onTouchEnter");

    //update
    setUpdate();
}

void GuiTextEditCtrl::onTouchLeave(const GuiEvent& event)
{
    if (!mActive)
        return;

    mMouseOver = false;

    Con::executef(this, 1, "onTouchLeave");

    //update
    setUpdate();
}

void GuiTextEditCtrl::saveUndoState()
{
   //save the current state
   mUndoText = mTextBuffer;
   mUndoSelector = mSelector;
}

void GuiTextEditCtrl::onCopy(bool andCut)
{
   // Don't copy/cut password field!
   if(mPasswordText)
      return;

   if (mSelector.hasSelection())
   {
      //save the current state
      saveUndoState();

      //copy the text to the clipboard
      string subString = mSelector.getSelection(mTextBuffer);
      Platform::setClipboard(subString.c_str());

      //if we pressed the cut shortcut, we need to cut the selected text from the control...
      if (andCut)
      {
          mSelector.eraseSelection(mTextBuffer);
      }

      mSelector.clearSelection();
   }

}

void GuiTextEditCtrl::onPaste()
{           
   //first, make sure there's something in the clipboard to copy...
   string clipboard = Platform::getClipboard();
   if(clipboard.length() <= 0)
      return;

   //save the current state
   saveUndoState();

   //delete anything hilited
   if (mSelector.hasSelection())
   {
       mSelector.eraseSelection(mTextBuffer);
       mSelector.clearSelection();
   }

   U32 pos = mSelector.getCursorPos();
   mTextBuffer.insert(pos, clipboard);
   setText(mTextBuffer);
   pos += clipboard.length();
   mSelector.setCursorPosition(pos);

   execConsoleCallback();
}

void GuiTextEditCtrl::onUndo()
{
    string tempText = mTextBuffer;
    GuiTextEditSelection tempSelector = mSelector;

    mTextBuffer = mUndoText;
    mSelector = mUndoSelector;

    mUndoText = tempText;
    mUndoSelector = tempSelector;
}

bool GuiTextEditCtrl::onKeyDown(const GuiEvent &event)
{
   if(! isActive())
      return false;

   mSuspendVerticalScrollJump = false;

   S32 stringLen = mTextBuffer.length();
   mSelector.setTextLength(stringLen);
   setUpdate();

   bool result = false;
   if (event.modifier & SI_SHIFT)
    {
       result = handleKeyDownWithShift(event);
    }
   else if (event.modifier & SI_CTRL)
   {
       //When holding the ctrl key, events must be handled here or passed up.
       return handleKeyDownWithCtrl(event);
   }
   else if (event.modifier & SI_ALT)
   {
       result = handleKeyDownWithAlt(event);
#if (defined(TORQUE_OS_OSX) || defined(TORQUE_OS_IOS))
       //Likewise, the cmd key must be handled here or passed up.
       return result;
#endif
   }

   if(result || (!result && handleKeyDownWithNoModifier(event)))
   {
       return true;
   }

   if (handleCharacterInput(event) || mSinkAllKeyEvents)
   {
       return true;
   }

   return Parent::onKeyDown( event );
}

bool GuiTextEditCtrl::tabNext()
{
	if (isMethod("onTab"))
		Con::executef(this, 2, "onTab", "0");

	GuiCanvas *root = getRoot();
	if (root)
	{
		root->tabNext();
		return true;
	}
	return false;
}

bool GuiTextEditCtrl::tabPrev()
{
	if (isMethod("onTab"))
		Con::executef(this, 2, "onTab", "1");

	GuiCanvas *root = getRoot();
	if (root)
	{
		root->tabPrev();
		return true;
	}
	return false;
}

void GuiTextEditCtrl::setFirstResponder()
{
	selectAllText();
	if(mTextBuffer.length() == 0)
	{
		mSelector.selectTo(mTextBuffer.length());
	}
    mSelector.setFirstResponder(true);
   Parent::setFirstResponder();
   
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
   Platform::enableKeyboardTranslation();
#endif	
}

void GuiTextEditCtrl::onLoseFirstResponder()
{
   Platform::disableKeyboardTranslation();

   //execute the validate command
   bool valid = validate();

   if (valid)
   {
	   execAltConsoleCallback();
   }

   if( isMethod( "onLoseFirstResponder" ) )
      Con::executef( this, 2, "onLoseFirstResponder", valid);
   if (isMethod("onBlur"))
	   Con::executef(this, 2, "onBlur", valid);

    mSelector.setFirstResponder(false);
    mTextOffsetY = 0;
   mScrollVelocity = 0;
   if (!mTextWrap && mTextBlockList.size() > 0)
   {
       mTextBlockList.front().resetScroll();
   }

   // Redraw the control:
   setUpdate();
}

void GuiTextEditCtrl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   Parent::parentResized( oldParentExtent, newParentExtent );
   mTextOffsetY = 0;
   if (!mTextWrap && mTextBlockList.size() > 0)
   {
       mTextBlockList.front().resetScroll();
   }
}

GuiControlState GuiTextEditCtrl::getCurrentState()
{
    if (!mActive)
        return GuiControlState::DisabledState;
    else if (isFirstResponder())
        return GuiControlState::SelectedState;
    else if (mMouseOver)
        return GuiControlState::HighlightState;
    else
        return GuiControlState::NormalState;
}

const ColorI& GuiTextEditCtrl::getCurrentFontColor()
{
    auto currentState = getCurrentState();
    return mProfile->getFontColor(currentState);
}

void GuiTextEditCtrl::onPreRender()
{
    mSelector.onPreRender(Platform::getVirtualMilliseconds());
    processScrollVelocity();
}

void GuiTextEditCtrl::onRender(Point2I offset, const RectI & updateRect)
{
    GuiControlState currentState = getCurrentState();
	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	renderUniversalRect(ctrlRect, mProfile, currentState);

	//Render Text
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

	if (contentRect.isValidRect())
	{
        if (currentState != SelectedState)
            mSelector.clearSelection();
        
        string textBuffer = applyPasswordMasking();
		renderText(contentRect.point, contentRect.extent, textBuffer.c_str(), mProfile);

		//Render the childen
		renderChildControls(offset, contentRect, updateRect);
	}
}

void GuiTextEditCtrl::renderLineList(const Point2I& offset, const Point2I& extent, const S32 startOffsetY, const vector<string> lineList, GuiControlProfile* profile, const TextRotationOptions rot)
{
    GFont* font = profile->getFont(mFontSizeAdjust);
    const S32 textHeight = font->getHeight();
    S32 totalWidth = extent.x;

    if (mTextBlockList.size() > lineList.size())
    {
        mTextBlockList.resize(lineList.size());
    }

    //Now print each line
    U32 ibeamPos = 0;
    S32 offsetY = startOffsetY - mTextOffsetY;
    for (U32 i = 0; i < lineList.size(); i++)
    {
        if(mTextBlockList.size() <= i)   
        {
            mTextBlockList.push_back(GuiTextEditTextBlock());
        }

        Point2I start = Point2I(0, offsetY);
        Point2I blockExtent = Point2I(extent.x, textHeight);
        RectI blockBounds = RectI(start + offset + profile->mTextOffset, blockExtent);
        if (mOverrideFontColor)
        {
            dglSetBitmapModulation(getFontColor(profile, NormalState));
        }
        mTextBlockList[i].render(blockBounds, lineList[i], ibeamPos, mProfile, getCurrentState(), mSelector, getAlignmentType(), font, mOverrideFontColor);

        offsetY += textHeight;
        ibeamPos += lineList[i].length();
    }
    performScrollJumpY();
}

void GuiTextEditCtrl::processScrollVelocity()
{
    if (mScrollVelocity == 0)
    {
        return;
    }

    U32 timeElapsed = Platform::getVirtualMilliseconds() - mTimeLastScrollProcess;
    S32 delta = mRound((F32)(timeElapsed * mScrollVelocity) / 1000);
    if (delta != 0)
    {
        RectI innerRect = getGlobalInnerRect();
        if (mTextWrap)
        {
            U32 max = (mTextBlockList.size() * mProfile->getFont(mFontSizeAdjust)->getHeight()) - innerRect.extent.y;
            mTextOffsetY = mClamp(mTextOffsetY + delta, 0, max);
        }
        else if(mTextBlockList.size() > 0)
        {
            mTextBlockList.front().processScrollVelocity(delta, innerRect.extent.x, mProfile->getFont(mFontSizeAdjust));
        }
        mTimeLastScrollProcess = Platform::getVirtualMilliseconds();

        S32 newCursorPos = calculateIbeamPosition(Canvas->getCursorPos());
        modifySelectBlock(newCursorPos);
    }
}

void GuiTextEditCtrl::performScrollJumpY()
{
    if (mTextWrap && !mSuspendVerticalScrollJump && isFirstResponder() && mSelector.isCursorRendered())
    {
        RectI clipRect = dglGetClipRect();
        S32 areaStart = clipRect.point.y;
        S32 areaEnd = clipRect.point.y + clipRect.extent.y;

        RectI cursorRect = mSelector.getCursorRect();
        S32 lineTop = cursorRect.point.y;
        S32 lineBottom = lineTop + cursorRect.extent.y;

        S32 diff = 0;
        if (lineTop < areaStart)
        {
            diff = lineTop - areaStart;
        }
        else if (lineBottom > areaEnd)
        {
            diff = lineBottom - areaEnd;
        }
        mTextOffsetY += diff;
    }
}

void GuiTextEditCtrl::adjustScrollVelocity(const Point2I& globalMousePoint, const RectI& globalInnerRect)
{
    RectI nonScrollArea = RectI(globalInnerRect);
    if (mTextWrap)
    {
        nonScrollArea.point.y += SCROLL_EDGE_SIZE;
        nonScrollArea.extent.y -= (2 * SCROLL_EDGE_SIZE);
    }
    else
    {
        nonScrollArea.point.x += SCROLL_EDGE_SIZE;
        nonScrollArea.extent.x -= (2 * SCROLL_EDGE_SIZE);
    }

    if (nonScrollArea.pointInRect(globalMousePoint))
    {
        mScrollVelocity = 0;
    }
    else if (globalInnerRect.pointInRect(globalMousePoint))
    {
        if (mScrollVelocity == 0)
        {
            mTimeLastScrollProcess = Platform::getVirtualMilliseconds();
        }

        mScrollVelocity = SCROLL_VELOCITY_PER_SEC;
        if ((mTextWrap && globalMousePoint.y < (globalInnerRect.point.y + SCROLL_EDGE_SIZE)) ||
            (!mTextWrap && globalMousePoint.x < (globalInnerRect.point.x + SCROLL_EDGE_SIZE)))
        {
            mScrollVelocity = -SCROLL_VELOCITY_PER_SEC;
        }
    }
    else
    {
        mScrollVelocity = 0;
    }
}

bool GuiTextEditCtrl::hasText()
{
   return (mTextBuffer.length());
}

void GuiTextEditCtrl::keyDenied()
{
	if (isMethod("onDenied"))
		Con::executef(this, 1, "onDenied");
}

const char *GuiTextEditCtrl::getScriptValue()
{
   return StringTable->insert(mTextBuffer.c_str());
}

void GuiTextEditCtrl::setScriptValue(const char *value)
{
   mTextBuffer.assign(value);
   mSelector.setTextLength(mTextBuffer.length());
}

GuiTextEditCtrl::InputMode GuiTextEditCtrl::getInputModeEnum(const char* label)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(inputModeEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (dStricmp(inputModeEnums[i].label, label) == 0)
			return (InputMode)inputModeEnums[i].index;
	}

	// Warn.
	Con::warnf("GuiTextEditCtrl::getInputModeEnum() - Invalid mode of '%s'", label);

	return (InputMode)-1;
}

const char* GuiTextEditCtrl::getInputModeDescription(const InputMode mode)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(inputModeEnums) / sizeof(EnumTable::Enums)); i++)
	{
		if (inputModeEnums[i].index == mode)
			return inputModeEnums[i].label;
	}

	// Warn.
	Con::warnf("GuiTextEditCtrl::getInputModeDescription() - Invalid input mode.");

	return StringTable->EmptyString;
}

//Returns true if valid, false if invalid
bool GuiTextEditCtrl::inputModeValidate(const U16 key, S32 cursorPos)
{
	if (key == '-')
	{
		if (mInputMode == Alpha || mInputMode == AlphaNumeric)
		{
			return false;
		}
		else if (mInputMode == Decimal || mInputMode == Number)
		{
			//a minus sign only exists at the beginning, and only a single minus sign
			if (cursorPos != 0 || (mInsertOn && mTextBuffer[0] == '-'))
			{
				return false;
			}
		}
	}
	else if (key >= '0' && key <= '9')
	{
		if (mInputMode == Alpha)
		{
			return false;
		}
	}
	else if (key == '.')
	{
		if (mInputMode == Number || mInputMode == Alpha || mInputMode == AlphaNumeric)
		{
			return false;
		}
		else if (mInputMode == Decimal)
		{
			if (!mInsertOn && mTextBuffer[cursorPos] == '.')
			{
				return true;
			}

			const char* dot = dStrchr(mTextBuffer.c_str(), '.');
			if (dot != NULL)
			{
				return false;
			}
		}
	}
	else if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))
	{
		if (mInputMode == Decimal || mInputMode == Number)
		{
			return false;
		}
	}
	else if (key == 32)
	{
		if (mInputMode == Decimal || mInputMode == Number)
		{
			return false;
		}
	}
	else if (mInputMode == Decimal || mInputMode == Number || mInputMode == Alpha || mInputMode == AlphaNumeric)
	{
		//The remaining characters only go with AllText
		return false;
	}

	//Looks like we have a valid character!
	return true;
}

void GuiTextEditCtrl::setMaxLength(S32 max)
{
	mMaxStrLen = getMax(1, getMin(max, MAX_STRING_LENGTH));
}

void GuiTextEditCtrl::setInputMode(const InputMode mode)
{
	if(mInputMode == mode)
		return;

	//Time to set the mode
	mInputMode = mode;

	//now let's parse that buffer and get rid of invalid characters
	if (mode != AllText)
	{
		bool oldInsert = mInsertOn;
		mInsertOn = false;
		for (S32 i = 0; i < MAX_STRING_LENGTH; i++)
		{
			const UTF16 character = mTextBuffer[i];
			if (character == '\0')
			{
				//Done and done.
				break;
			}
			
			if (!inputModeValidate(character, i))
			{
				//Bad Character! Let's remove it.
				mTextBuffer.erase(i, 1);

				//Step it back
				i--;
			}
		}
		mInsertOn = oldInsert;
	}
}

string GuiTextEditCtrl::applyPasswordMasking()
{
    if (mPasswordText)
    {
        string passwordCover = string();
        passwordCover.resize(mTextBuffer.length(), mPasswordMask[0]);
        return passwordCover;
    }
    return mTextBuffer;
}

bool GuiTextEditCtrl::handleKeyDownWithShift(const GuiEvent& event)
{
    switch (event.keyCode)
    {
    case KEY_TAB:
        return tabPrev();

    case KEY_HOME:
        mSelector.selectTo(0);
        return true;

    case KEY_END:
        mSelector.selectTo(mTextBuffer.length());
        return true;

    case KEY_LEFT:
        return handleShiftArrowKey(GuiDirection::Left);

    case KEY_RIGHT:
        return handleShiftArrowKey(GuiDirection::Right);

    case KEY_UP:
        return handleShiftArrowKey(GuiDirection::Up);

    case KEY_DOWN:
        return handleShiftArrowKey(GuiDirection::Down);
    }
    return false;
}

bool GuiTextEditCtrl::handleKeyDownWithCtrl(const GuiEvent& event)
{
    switch (event.keyCode)
    {
#if !(defined(TORQUE_OS_OSX) || defined(TORQUE_OS_IOS))
        // windows style cut / copy / paste / undo keybinds
    case KEY_C:
    case KEY_X:
        onCopy(event.keyCode == KEY_X);
        return true;

    case KEY_V:
        onPaste();
        return true;

    case KEY_Z:
        onUndo();
        return true;
#endif

    case KEY_DELETE:
    case KEY_BACKSPACE:
        selectAllText();
        handleBackSpace();
        return true;
    }
    return false;
}

bool GuiTextEditCtrl::handleKeyDownWithAlt(const GuiEvent& event)
{
#if (defined(TORQUE_OS_OSX) || defined(TORQUE_OS_IOS))
    // Added Mac cut/copy/paste/undo keys
    // Mac command key maps to alt in torque.
    switch(event.keyCode)
    {
        case KEY_C:
        case KEY_X:
            onCopy( event.keyCode==KEY_X );
            return true;
        
        case KEY_V:
            onPaste();
            return true;

        case KEY_Z:
            onUndo();
            return true;
    }
#endif
    return false;
}

bool GuiTextEditCtrl::handleKeyDownWithNoModifier(const GuiEvent& event)
{
    switch (event.keyCode)
    {
    case KEY_TAB:
        return tabNext();

    case KEY_ESCAPE:
        if (mEscapeCommand && mEscapeCommand[0])
        {
            Con::evaluate(mEscapeCommand);
            return true;
        }
        return false;

    case KEY_RETURN:
    case KEY_NUMPADENTER:
        if (!validate())
        {
            return true;
        }
        return handleEnterKey();

    case KEY_LEFT:
        return handleArrowKey(GuiDirection::Left);

    case KEY_RIGHT:
        return handleArrowKey(GuiDirection::Right);

    case KEY_UP:
        return handleArrowKey(GuiDirection::Up);

    case KEY_DOWN:
        return handleArrowKey(GuiDirection::Down);

    case KEY_DELETE:
        return handleDelete();

    case KEY_BACKSPACE:
        return handleBackSpace();

    case KEY_INSERT:
        mInsertOn = !mInsertOn;
        return true;

    case KEY_HOME:
        mSelector.setCursorPosition(0);
        return true;

    case KEY_END:
        mSelector.setCursorPosition(mTextBuffer.length());
        return true;
    }
    return false;
}

bool GuiTextEditCtrl::handleCharacterInput(const GuiEvent& event)
{
    if (!mProfile->getFont(mFontSizeAdjust))
        return false;

    if (mProfile->getFont(mFontSizeAdjust)->isValidChar(event.ascii))
    {
        // Get the character ready to add to a UTF8 string.
        string characterToInsert = string(1, event.ascii);

        //Stop characters that aren't allowed based on InputMode
        if (!inputModeValidate(event.ascii, mSelector.getCursorPos()))
        {
            keyDenied();
            return true;
        }

        saveUndoState();

        if (mSelector.hasSelection())
        {
            mSelector.eraseSelection(mTextBuffer);
        }

        if (mTextBuffer.length() < mMaxStrLen || !mInsertOn)
        {
            if (!mInsertOn)
            {
                mTextBuffer.erase(mSelector.getCursorPos(), 1);
            }
            mTextBuffer.insert(mSelector.getCursorPos(), characterToInsert);
            mSelector.setTextLength(mTextBuffer.length());
            mSelector.stepCursorForward();
            setText(mTextBuffer);
        }
        else
            keyDenied();

        execConsoleCallback();

        return true;
    }
    return false;
}

bool GuiTextEditCtrl::handleBackSpace()
{
    if (mTextBuffer.length() == 0 || (mSelector.getCursorPos() == 0 && !mSelector.hasSelection()))
        return true;

    saveUndoState();

    if (mSelector.hasSelection())
    {
        mSelector.eraseSelection(mTextBuffer);
    }
    else
    {
        mSelector.stepCursorBackward();
        mTextBuffer.erase(mSelector.getCursorPos(), 1);
    }
    mSelector.setTextLength(mTextBuffer.length());
    setText(mTextBuffer);
    execConsoleCallback();

    return true;
}

bool GuiTextEditCtrl::handleDelete()
{
    if (mTextBuffer.length() == mSelector.getCursorPos() && !mSelector.hasSelection())
        return true;

    saveUndoState();

    if (mSelector.hasSelection())
    {
        mSelector.eraseSelection(mTextBuffer);
    }
    else
    {
        mTextBuffer.erase(mSelector.getCursorPos(), 1);
    }
    mSelector.setTextLength(mTextBuffer.length());
    setText(mTextBuffer);
    execConsoleCallback();

    return true;
}

bool GuiTextEditCtrl::handleEnterKey()
{
    if (isMethod("onReturn"))
        Con::executef(this, 1, "onReturn");

    if (mReturnCausesTab)
    {
        tabNext();
    }

    
    if (mReturnCommand && mReturnCommand[0])
    {
        Con::evaluate(mReturnCommand);
    }
    return true;
}

bool GuiTextEditCtrl::handleArrowKey(GuiDirection direction)
{
    if (direction == GuiDirection::Left)
    {
        mSelector.setCursorAtEOL(false);
        mSelector.stepCursorBackward();
    }
    else if (direction == GuiDirection::Right)
    {
        mSelector.setCursorAtEOL(false);
        mSelector.stepCursorForward();
    }
    else if (direction == GuiDirection::Up)
    {
        S32 newCursorPos = getLineAdjustedIbeamPosition(-mProfile->getFont(mFontSizeAdjust)->getHeight());
        if (newCursorPos == mSelector.getCursorPos())
        {
            newCursorPos = 0;
        }
        mSelector.setCursorPosition(newCursorPos);
    }
    else if (direction == GuiDirection::Down)
    {
        S32 newCursorPos = getLineAdjustedIbeamPosition(mProfile->getFont(mFontSizeAdjust)->getHeight());
        if (newCursorPos == mSelector.getCursorPos())
        {
            newCursorPos = mTextBuffer.length();
        }
        mSelector.setCursorPosition(newCursorPos);
    }
    setUpdate();
    mSelector.resetCursorBlink();
    return true;
}

bool GuiTextEditCtrl::handleShiftArrowKey(GuiDirection direction)
{
    if (direction == GuiDirection::Left)
    {
        mSelector.setCursorAtEOL(false);
        modifySelectBlock(mSelector.getCursorPos() - 1);
    }
    else if (direction == GuiDirection::Right)
    {
        mSelector.setCursorAtEOL(false);
        modifySelectBlock(mSelector.getCursorPos() + 1);
    }
    else if (direction == GuiDirection::Up)
    {
        S32 newCursorPos = getLineAdjustedIbeamPosition(-mProfile->getFont(mFontSizeAdjust)->getHeight());
		if (newCursorPos == mSelector.getCursorPos())
		{
			newCursorPos = 0;
		}
		modifySelectBlock(newCursorPos);
    }
    else if (direction == GuiDirection::Down)
    {
        S32 newCursorPos = getLineAdjustedIbeamPosition(mProfile->getFont(mFontSizeAdjust)->getHeight());
		if (newCursorPos == mSelector.getCursorPos())
		{
			newCursorPos = mTextBuffer.length();
		}
		modifySelectBlock(newCursorPos);
    }
    setUpdate();
    mSelector.resetCursorBlink();
    return true;
}

S32 GuiTextEditCtrl::getLineAdjustedIbeamPosition(S32 heightAdjustment)
{
    Point2I centerPoint = mSelector.getCursorCenter();
    centerPoint.y += heightAdjustment;
    return calculateIbeamPosition(centerPoint);
}

void GuiTextEditCtrl::modifySelectBlock(const U32 target)
{
    mSelector.setTextLength(mTextBuffer.length());
    mSelector.selectTo(target);
    setUpdate();
}

void GuiTextEditCtrl::selectAllText()
{
    mSelector.setCursorPosition(0);
    modifySelectBlock(mTextBuffer.length());
}

void GuiTextEditCtrl::getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent)
{
    if (mEditCursor == NULL)
    {
        SimObject* obj;
        obj = Sim::findObject("EditCursor");
        mEditCursor = dynamic_cast<GuiCursor*>(obj);
    }
    if (mEditCursor != NULL)
    {
        cursor = mEditCursor;
    }
}
#pragma endregion