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
#include "gui/guiMLTextCtrl.h"
#include "gui/guiTextEditCtrl.h"
#include "gui/guiDefaultControlRender.h"
#include "memory/frameAllocator.h"
#include "string/unicode.h"

#include "guiTextEditCtrl_ScriptBinding.h"

IMPLEMENT_CONOBJECT(GuiTextEditCtrl);

U32 GuiTextEditCtrl::smNumAwake = 0;

GuiTextEditCtrl::GuiTextEditCtrl()
{
   mInsertOn = true;
   mBlockStart = 0;
   mBlockEnd = 0;
   mCursorPos = 0;
   mCursorOn = false;
   mNumFramesElapsed = 0;

   mDragHit = false;
   mScrollDir = 0;

   mUndoBlockStart = 0;
   mUndoBlockEnd = 0;
   mUndoCursorPos = 0;
   mPasswordText = false;

   mReturnCausesTab = false;
   mSinkAllKeyEvents = false;

   mActive = true;

   mTextOffsetReset = true;

   mHistoryDirty = false;
   mHistorySize = 0;
   mHistoryLast = -1;
   mHistoryIndex = 0;
   mHistoryBuf = NULL;

   mEscapeCommand = StringTable->EmptyString;
   mPasswordMask = StringTable->insert( "*" );

   mEditCursor = NULL;
   mMaxStrLen = MAX_STRING_LENGTH;

   mInputMode = AllText;
}

GuiTextEditCtrl::~GuiTextEditCtrl()
{
   //delete the history buffer if it exists
   if (mHistoryBuf)
   {
      for (S32 i = 0; i < mHistorySize; i++)
         delete [] mHistoryBuf[i];

      delete [] mHistoryBuf;
   }
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
   addField("escapeCommand",     TypeString,    Offset(mEscapeCommand,     GuiTextEditCtrl));
   addField("sinkAllKeyEvents",  TypeBool,      Offset(mSinkAllKeyEvents,  GuiTextEditCtrl));
   addField("password", TypeBool, Offset(mPasswordText, GuiTextEditCtrl));
   addField("returnCausesTab", TypeBool, Offset(mReturnCausesTab, GuiTextEditCtrl));
   addProtectedField("maxLength", TypeS32,		Offset(mMaxStrLen,			GuiTextEditCtrl), &setMaxLengthProperty, &defaultProtectedGetFn, "The max number of characters that can be entered into the text edit box.");
   addProtectedField("inputMode", TypeEnum,		Offset(mInputMode,			GuiTextEditCtrl), &setInputMode, &getInputMode, &writeInputMode, 1, &gInputModeTable, "InputMode allows different characters to be entered.");	
   endGroup("Text Edit");
}

bool GuiTextEditCtrl::onAdd()
{
   if ( ! Parent::onAdd() )
      return false;

   //create the history buffer
   if ( mHistorySize > 0 )
   {
      mHistoryBuf = new UTF16*[mHistorySize];
      for ( S32 i = 0; i < mHistorySize; i++ )
      {
         mHistoryBuf[i] = new UTF16[MAX_STRING_LENGTH + 1];
         mHistoryBuf[i][0] = '\0';
      }
   }

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
   // Execute the console command!
   if ( mConsoleCommand[0] )
   {
      char buf[16];
      dSprintf( buf, sizeof( buf ), "%d", getId() );
      Con::setVariable( "$ThisControl", buf );
      Con::evaluate( mConsoleCommand, false );
   }

   // Update the console variable:
   if ( mConsoleVariable[0] )
      Con::setVariable( mConsoleVariable, mTextBuffer.getPtr8() );
}

void GuiTextEditCtrl::updateHistory( StringBuffer *inTxt, bool moveIndex )
{
   const UTF16* txt = inTxt->getPtr();

   if(!txt)
      return;

   if(!mHistorySize)
      return;

   // see if it's already in
   if(mHistoryLast == -1 || dStrcmp(txt, mHistoryBuf[mHistoryLast]))
   {
      if(mHistoryLast == mHistorySize-1) // we're at the history limit... shuffle the pointers around:
      {
         UTF16 *first = mHistoryBuf[0];
         for(U32 i = 0; i < (U32)(mHistorySize - 1); i++)
            mHistoryBuf[i] = mHistoryBuf[i+1];
         mHistoryBuf[mHistorySize-1] = first;
         if(mHistoryIndex > 0)
            mHistoryIndex--;
      }
      else
         mHistoryLast++;

      inTxt->getCopy(mHistoryBuf[mHistoryLast], MAX_STRING_LENGTH);
      mHistoryBuf[mHistoryLast][MAX_STRING_LENGTH] = '\0';
   }

   if(moveIndex)
      mHistoryIndex = mHistoryLast + 1;
}

void GuiTextEditCtrl::getText( char *dest )
{
   if ( dest )
      mTextBuffer.getCopy8((UTF8*)dest, MAX_STRING_LENGTH+1);
}  
 
void GuiTextEditCtrl::setText( const UTF8 *txt )
{
	//make sure we don't call this before onAdd();
	if (!mProfile)
		return;

	//Make sure we have a font
	mProfile->incRefCount();

	//If the font isn't found, we want to decrement the profile usage and return now or we may crash!
	if (mProfile->mFont.isNull())
	{
		//decrement the profile referrence
		mProfile->decRefCount();
		return;
	}

	setVariable((char*)mText);
	setUpdate();

	//decrement the profile referrence
	mProfile->decRefCount();

   if(txt && txt[0] != 0)
   {
      Parent::setText(txt);
      mTextBuffer.set( txt );
   }
   else
      mTextBuffer.set( "" );

    //respect the max size
    int diff = mTextBuffer.length() - mMaxStrLen; 
    if( diff > 0 ) {
        mTextBuffer.cut( mMaxStrLen, diff );
    }
   mCursorPos = mTextBuffer.length();
}

void GuiTextEditCtrl::setText( const UTF16* txt)
{
   if(txt && txt[0] != 0)
   {
      UTF8* txt8 = convertUTF16toUTF8( txt );
      Parent::setText( txt8 );
	  setText(txt8);
      delete[] txt8;
   }
   else
   {
      Parent::setText("");
      setText("");
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

void GuiTextEditCtrl::selectAllText()
{
   mBlockStart = 0;
   mBlockEnd = mTextBuffer.length();
   setUpdate();
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

void GuiTextEditCtrl::reallySetCursorPos( const S32 newPos )
{
   S32 charCount = mTextBuffer.length();
   S32 realPos = newPos > charCount ? charCount : newPos < 0 ? 0 : newPos;
   if ( realPos != mCursorPos )
   {
      mCursorPos = realPos;
      setUpdate();
   }
}

S32 GuiTextEditCtrl::setCursorPos( const Point2I &offset )
{
   Point2I ctrlOffset = localToGlobalCoord( Point2I( 0, 0 ) );
   S32 charLength = 0;
   S32 curX;

   curX = offset.x - ctrlOffset.x;
   setUpdate();

   //if the cursor is too far to the left
   if ( curX < 0 )
      return -1;

   //if the cursor is too far to the right
   if ( curX >= ctrlOffset.x + mBounds.extent.x )
      return -2;

   curX = offset.x - mTextOffset.x;
   S32 count=0;
   if(mTextBuffer.length() == 0)
      return 0;

   for(count=0; count< (S32)mTextBuffer.length(); count++)
   {
      UTF16 c = mTextBuffer.getChar(count);
      if(!mPasswordText && !mProfile->mFont->isValidChar(c))
         continue;
         
      if(mPasswordText)
         charLength += mProfile->mFont->getCharXIncrement( mPasswordMask[0] );
      else
         charLength += mProfile->mFont->getCharXIncrement( c );

      if ( charLength > curX )
         break;      
   }

   return count;
}

void GuiTextEditCtrl::onTouchDown( const GuiEvent &event )
{
   mDragHit = false;

   // If we have a double click, select all text.  Otherwise
   // act as before by clearing any selection.
   bool doubleClick = (event.mouseClickCount > 1);
   if(doubleClick)
   {
      selectAllText();

   } else
   {
      //undo any block function
      mBlockStart = 0;
      mBlockEnd = 0;
   }

   //find out where the cursor should be
   S32 pos = setCursorPos( event.mousePoint );

   // if the position is to the left
   if ( pos == -1 )
      mCursorPos = 0;
   else if ( pos == -2 ) //else if the position is to the right
      mCursorPos = mTextBuffer.length();
   else //else set the mCursorPos
      mCursorPos = pos;

   //save the mouseDragPos
   mMouseDragStart = mCursorPos;

   // lock the mouse
   mouseLock();

   //set the drag var
   mDragHit = true;

   //let the parent get the event
   setFirstResponder();

   // Notify Script.
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
   S32 pos = setCursorPos( event.mousePoint );

   // if the position is to the left
   if ( pos == -1 )
      mScrollDir = -1;
   else if ( pos == -2 ) // the position is to the right
      mScrollDir = 1;
   else // set the new cursor position
   {
      mScrollDir = 0;
      mCursorPos = pos;
   }

   // update the block:
   mBlockStart = getMin( mCursorPos, mMouseDragStart );
   mBlockEnd = getMax( mCursorPos, mMouseDragStart );
   if ( mBlockStart < 0 )
      mBlockStart = 0;

   if ( mBlockStart == mBlockEnd )
      mBlockStart = mBlockEnd = 0;

   //let the parent get the event
   Parent::onTouchDragged(event);

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
   mDragHit = false;
   mScrollDir = 0;
   mouseUnlock();

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

void GuiTextEditCtrl::saveUndoState()
{
   //save the current state
   mUndoText.set(&mTextBuffer);
   mUndoBlockStart = mBlockStart;
   mUndoBlockEnd   = mBlockEnd;
   mUndoCursorPos  = mCursorPos;
}

void GuiTextEditCtrl::onCopy(bool andCut)
{
   // Don't copy/cut password field!
   if(mPasswordText)
      return;

   if (mBlockEnd > 0)
   {
      //save the current state
      saveUndoState();

      //copy the text to the clipboard
      UTF8* clipBuff = mTextBuffer.createSubstring8(mBlockStart, mBlockEnd - mBlockStart);
      Platform::setClipboard(clipBuff);
      delete[] clipBuff;

      //if we pressed the cut shortcut, we need to cut the selected text from the control...
      if (andCut)
      {
         mTextBuffer.cut(mBlockStart, mBlockEnd - mBlockStart);
         mCursorPos = mBlockStart;
      }

      mBlockStart = 0;
      mBlockEnd = 0;
   }

}

void GuiTextEditCtrl::onPaste()
{           
   //first, make sure there's something in the clipboard to copy...
   const UTF8 *clipboard = Platform::getClipboard();
   if(dStrlen(clipboard) <= 0)
      return;

   //save the current state
   saveUndoState();

   //delete anything hilited
   if (mBlockEnd > 0)
   {
      mTextBuffer.cut(mBlockStart, mBlockEnd - mBlockStart);
      mCursorPos = mBlockStart;
      mBlockStart = 0;
      mBlockEnd = 0;
   }

   // We'll be converting to UTF16, and maybe trimming the string,
   // so let's use a StringBuffer, for convinience.
   StringBuffer pasteText(clipboard);

   // Space left after we remove the highlighted text
   S32 stringLen = mTextBuffer.length();

   // Trim down to fit in a buffer of size mMaxStrLen
   S32 pasteLen = pasteText.length();

   if(stringLen + pasteLen > mMaxStrLen)
   {
      pasteLen = mMaxStrLen - stringLen;

      pasteText.cut(pasteLen, pasteText.length() - pasteLen);
   }

   if (mCursorPos == stringLen)
   {
      mTextBuffer.append(pasteText);
   }
   else
   {
      mTextBuffer.insert(mCursorPos, pasteText);
   }

   mCursorPos += pasteLen;
}

void GuiTextEditCtrl::onUndo()
{
    StringBuffer tempBuffer;
    S32 tempBlockStart;
    S32 tempBlockEnd;
    S32 tempCursorPos;

    //save the current
    tempBuffer.set(&mTextBuffer);
    tempBlockStart = mBlockStart;
    tempBlockEnd   = mBlockEnd;
    tempCursorPos  = mCursorPos;

    //restore the prev
    mTextBuffer.set(&mUndoText);
    mBlockStart = mUndoBlockStart;
    mBlockEnd   = mUndoBlockEnd;
    mCursorPos  = mUndoCursorPos;

    //update the undo
    mUndoText.set(&tempBuffer);
    mUndoBlockStart = tempBlockStart;
    mUndoBlockEnd   = tempBlockEnd;
    mUndoCursorPos  = tempCursorPos;
}

bool GuiTextEditCtrl::onKeyDown(const GuiEvent &event)
{
   if(! isActive())
      return false;

   S32 stringLen = mTextBuffer.length();
   setUpdate();

   if (event.keyCode == KEY_BACKSPACE)
   {
	   handleBackSpace();
	   return true;
   }

   if (event.modifier & SI_SHIFT)
    {
        switch (event.keyCode)
        {
            case KEY_TAB:
               return tabPrev();

            case KEY_HOME:
               mBlockStart = 0;
               mBlockEnd = mCursorPos;
               mCursorPos = 0;
               return true;

            case KEY_END:
                mBlockStart = mCursorPos;
                mBlockEnd = stringLen;
                mCursorPos = stringLen;
                return true;

            case KEY_LEFT:
                if ((mCursorPos > 0) & (stringLen > 0))
                {
                    //if we already have a selected block
                    if (mCursorPos == mBlockEnd)
                    {
                        mCursorPos--;
                        mBlockEnd--;
                        if (mBlockEnd == mBlockStart)
                        {
                            mBlockStart = 0;
                            mBlockEnd = 0;
                        }
                    }
                    else {
                        mCursorPos--;
                        mBlockStart = mCursorPos;

                        if (mBlockEnd == 0)
                        {
                            mBlockEnd = mCursorPos + 1;
                        }
                    }
                }
                return true;

            case KEY_RIGHT:
                if (mCursorPos < stringLen)
                {
                    if ((mCursorPos == mBlockStart) && (mBlockEnd > 0))
                    {
                        mCursorPos++;
                        mBlockStart++;
                        if (mBlockStart == mBlockEnd)
                        {
                            mBlockStart = 0;
                            mBlockEnd = 0;
                        }
                    }
                    else
                    {
                        if (mBlockEnd == 0)
                        {
                            mBlockStart = mCursorPos;
                            mBlockEnd = mCursorPos;
                        }
                        mCursorPos++;
                        mBlockEnd++;
                    }
                }
                return true;
        }
    }
   else if (event.modifier & SI_CTRL)
   {
      switch(event.keyCode)
      {
         // Added UNIX emacs key bindings - just a little hack here...

         // BJGTODO: Add vi bindings.

         // Ctrl-B - move one character back
         case KEY_B:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_LEFT;
            return(onKeyDown(new_event));
         }

         // Ctrl-F - move one character forward
         case KEY_F:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_RIGHT;
            return(onKeyDown(new_event));
         }

         // Ctrl-A - move to the beginning of the line
         case KEY_A:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_HOME;
            return(onKeyDown(new_event));
         }

         // Ctrl-E - move to the end of the line
         case KEY_E:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_END;
            return(onKeyDown(new_event));
         }

         // Ctrl-P - move backward in history
         case KEY_P:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_UP;
            return(onKeyDown(new_event));
         }

         // Ctrl-N - move forward in history
         case KEY_N:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_DOWN;
            return(onKeyDown(new_event));
         }

         // Ctrl-D - delete under cursor
         case KEY_D:
         { 
            GuiEvent new_event;
            new_event.modifier = 0;
            new_event.keyCode = KEY_DELETE;
            return(onKeyDown(new_event));
         }

         case KEY_U:
         { 
            GuiEvent new_event;
            new_event.modifier = SI_CTRL;
            new_event.keyCode = KEY_DELETE;
            return(onKeyDown(new_event));
         }

         // End added UNIX emacs key bindings
#if !(defined(TORQUE_OS_OSX) || defined(TORQUE_OS_IOS))
         // windows style cut / copy / paste / undo keybinds
         case KEY_C:
         case KEY_X:
         {
            // copy, and cut the text if we hit ctrl-x
            onCopy( event.keyCode==KEY_X );
            return true;
         }
         case KEY_V:
         {
            onPaste();

            // Execute the console command!
            execConsoleCallback();
            return true;
         }

         case KEY_Z:
            if (! mDragHit)
            {
               onUndo();
               return true;
            }
#endif

         case KEY_DELETE:
         case KEY_BACKSPACE:
            //save the current state
            saveUndoState();

            //delete everything in the field
            mTextBuffer.set("");
            mCursorPos  = 0;
            mBlockStart = 0;
            mBlockEnd   = 0;

            execConsoleCallback();

            return true;

         // [neo, 5/24/2007 - #2986]
         // We don't want to embed control characters in the text, so just return false
         // so that any that any other consumer can have a bash at the input.
         default:
            return false;
      }
   }
#if (defined(TORQUE_OS_OSX) || defined(TORQUE_OS_IOS))
   // mac style cut / copy / paste / undo keybinds
   else if (event.modifier & SI_ALT)
   {
      // Added Mac cut/copy/paste/undo keys
      // Mac command key maps to alt in torque.
      switch(event.keyCode)
      {
         case KEY_C:
         case KEY_X:
         {
            // copy, and cut the text if we hit cmd-x
            onCopy( event.keyCode==KEY_X );
            return true;
         }
         case KEY_V:
         {
            onPaste();

            // Execute the console command!
            execConsoleCallback();

            return true;
         }
            
         case KEY_Z:
            if (! mDragHit)
            {
               onUndo();
               return true;
            }
      }
   }
#endif
   else
   {
      switch(event.keyCode)
      {
         case KEY_ESCAPE:
            if ( mEscapeCommand[0] )
            {
               Con::evaluate( mEscapeCommand );
               return( true );
            }
            return( Parent::onKeyDown( event ) );

         case KEY_RETURN:
         case KEY_NUMPADENTER:
            //first validate
            if (!validate())
            {
				//The contents must be invalid. Stop here.
				return true;
            }

            updateHistory(&mTextBuffer, true);
            mHistoryDirty = false;

            //next exec the alt console command
            execAltConsoleCallback();

            // Notify of Return
            if ( isMethod("onReturn") )
               Con::executef( this, 1, "onReturn" );

            if (mReturnCausesTab)
            {
               tabNext();
            }
            return true;

         case KEY_UP:
         {
            if(mHistoryDirty)
            {
               updateHistory(&mTextBuffer, false);
               mHistoryDirty = false;
            }

            mHistoryIndex--;
            
            if(mHistoryIndex >= 0 && mHistoryIndex <= mHistoryLast)
               setText(mHistoryBuf[mHistoryIndex]);
            else if(mHistoryIndex < 0)
               mHistoryIndex = 0;
            
            return true;
         }

         case KEY_DOWN:
            if(mHistoryDirty)
            {
               updateHistory(&mTextBuffer, false);
               mHistoryDirty = false;
            }
            mHistoryIndex++;
            if(mHistoryIndex > mHistoryLast)
            {
               mHistoryIndex = mHistoryLast + 1;
               setText("");
            }
            else
               setText(mHistoryBuf[mHistoryIndex]);
            return true;

         case KEY_LEFT:
            mBlockStart = 0;
            mBlockEnd = 0;
            if (mCursorPos > 0)
            {
                mCursorPos--;
            }
            return true;

         case KEY_RIGHT:
            mBlockStart = 0;
            mBlockEnd = 0;
            if (mCursorPos < stringLen)
            {
                mCursorPos++;
            }
            return true;

         case KEY_BACKSPACE:
			//This should have been handled above, but it is here again for safety.
            handleBackSpace();
            return true;

         case KEY_DELETE:
            //save the current state
            saveUndoState();

            if (mBlockEnd > 0)
            {
               mHistoryDirty = true;
               mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);

               mCursorPos = mBlockStart;
               mBlockStart = 0;
               mBlockEnd = 0;

               // Execute the console command!
               execConsoleCallback();
            }
            else if (mCursorPos < stringLen)
            {
               mHistoryDirty = true;
               mTextBuffer.cut(mCursorPos, 1);

               // Execute the console command!
               execConsoleCallback();
            }
            return true;

         case KEY_INSERT:
            mInsertOn = !mInsertOn;
            return true;

         case KEY_HOME:
            mBlockStart = 0;
            mBlockEnd   = 0;
            mCursorPos  = 0;
            return true;

         case KEY_END:
            mBlockStart = 0;
            mBlockEnd   = 0;
            mCursorPos  = stringLen;
            return true;

         }
   }

   switch ( event.keyCode )
   {
      case KEY_TAB:
		  return tabNext();
      case KEY_UP:
      case KEY_DOWN:
      case KEY_ESCAPE:
         return Parent::onKeyDown( event );
   }

   if(mProfile->mFont.isNull())
       return false;

   if ( mProfile->mFont->isValidChar( event.ascii ) )
   {
      // Get the character ready to add to a UTF8 string.
      UTF16 convertedChar[2] = { event.ascii, 0 };

		//Stop characters that aren't allowed based on InputMode
		if (!inputModeValidate(event.ascii, mCursorPos))
		{
			keyDenied();
			return true;
		}

      //save the current state
      saveUndoState();

      //delete anything highlighted
      if ( mBlockEnd > 0 )
      {
         mTextBuffer.cut(mBlockStart, mBlockEnd-mBlockStart);
         mCursorPos  = mBlockStart;
         mBlockStart = 0;
         mBlockEnd   = 0;
      }

      if ( ( mInsertOn && ( stringLen < mMaxStrLen ) ) ||
          ( !mInsertOn && ( mCursorPos < mMaxStrLen ) ) )
      {
         if ( mCursorPos == stringLen )
         {
            mTextBuffer.append(convertedChar);
            mCursorPos++;
         }
         else
         {
            if ( mInsertOn )
            {
               mTextBuffer.insert(mCursorPos, convertedChar);
               mCursorPos++;
            }
            else
            {
               mTextBuffer.cut(mCursorPos, 1);
               mTextBuffer.insert(mCursorPos, convertedChar);
               mCursorPos++;
            }
         }
      }
      else
		  keyDenied();

      //reset the history index
      mHistoryDirty = true;

      //execute the console command if it exists
      execConsoleCallback();

      return true;
   }

   //not handled - pass the event to it's parent

   // Or eat it if that's appropriate.
   if (mSinkAllKeyEvents)
      return true;

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

void GuiTextEditCtrl::handleBackSpace()
{
	//save the current state
	saveUndoState();

	if (mBlockEnd > 0)
	{
		mTextBuffer.cut(mBlockStart, mBlockEnd - mBlockStart);
		mCursorPos = mBlockStart;
		mBlockStart = 0;
		mBlockEnd = 0;
		mHistoryDirty = true;
	}
	else if (mCursorPos > 0)
	{
		mTextBuffer.cut(mCursorPos - 1, 1);
		mCursorPos--;
		mHistoryDirty = true;
	}

	// Execute the console command!
	execConsoleCallback();
}

void GuiTextEditCtrl::setFirstResponder()
{
   Parent::setFirstResponder();
   
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
   Platform::enableKeyboardTranslation();
#endif	
}

void GuiTextEditCtrl::onLoseFirstResponder()
{
   Platform::disableKeyboardTranslation();

   //first, update the history
   updateHistory( &mTextBuffer, true );

   //execute the validate command
   bool valid = validate();

   if (valid)
   {
	   execAltConsoleCallback();
   }

   if( isMethod( "onLoseFirstResponder" ) )
      Con::executef( this, 2, "onLoseFirstResponder", valid);

   // Redraw the control:
   setUpdate();
}


void GuiTextEditCtrl::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   Parent::parentResized( oldParentExtent, newParentExtent );
   mTextOffsetReset = true;
}

void GuiTextEditCtrl::onRender(Point2I offset, const RectI & updateRect)
{
	//Notice that there's no Highlight state. The HL colors are used for selected text - not hover.
	//The selected state is used when the box has the focus and can be typed in.
	GuiControlState currentState = NormalState;
	if (!mActive)
	{
		currentState = DisabledState;
	}
	else if (isFirstResponder())
	{
		currentState = SelectedState;
	}

	RectI ctrlRect = applyMargins(offset, mBounds.extent, currentState, mProfile);

	if (!ctrlRect.isValidRect())
	{
		return;
	}

	renderUniversalRect(ctrlRect, mProfile, currentState);

	//Render Text
	dglSetBitmapModulation(mProfile->mFontColor);
	RectI fillRect = applyBorders(ctrlRect.point, ctrlRect.extent, NormalState, mProfile);
	RectI contentRect = applyPadding(fillRect.point, fillRect.extent, NormalState, mProfile);

	if (contentRect.isValidRect())
	{
		drawText(contentRect, currentState);

		//Render the childen
		renderChildControls(offset, contentRect, updateRect);
	}
}

void GuiTextEditCtrl::onPreRender()
{
   if ( isFirstResponder() )
   {
      U32 timeElapsed = Platform::getVirtualMilliseconds() - mTimeLastCursorFlipped;
      mNumFramesElapsed++;
      if ( ( timeElapsed > 500 ) && ( mNumFramesElapsed > 3 ) )
      {
         mCursorOn = !mCursorOn;
         mTimeLastCursorFlipped = Platform::getVirtualMilliseconds();
         mNumFramesElapsed = 0;
         setUpdate();
      }

      //update the cursor if the text is scrolling
      if ( mDragHit )
      {
         if ( ( mScrollDir < 0 ) && ( mCursorPos > 0 ) )
            mCursorPos--;
         else if ( ( mScrollDir > 0 ) && ( mCursorPos < (S32) mTextBuffer.length() ) )
            mCursorPos++;
      }
   }
}

void GuiTextEditCtrl::drawText( const RectI &drawRect, GuiControlState currentState )
{
   RectI old = dglGetClipRect();
   RectI clipRect = RectI(drawRect.point, drawRect.extent);
   if (clipRect.intersect(old))
   {
	   dglSetClipRect(clipRect);

	   // Just a little sanity.
	   if(mCursorPos > (S32)mTextBuffer.length()) 
		  mCursorPos = (S32)mTextBuffer.length();
	   if(mCursorPos < 0)
		   mCursorPos = 0;

	   StringBuffer textBuffer;
	   Point2I drawPoint = drawRect.point;

	   // Apply password masking (make the masking char optional perhaps?)
	   if(mPasswordText)
	   {
		  for(U32 i = 0; i<mTextBuffer.length(); i++)
			 textBuffer.append(mPasswordMask);
	   }
	   else
	   {
		  // Or else just copy it over.
		  textBuffer.set(&mTextBuffer);
	   }

	   // Center vertically:
	   S32 h = mProfile->mFont->getHeight();
	   drawPoint.y += ( ( drawRect.extent.y - h ) / 2 );

	   // Align horizontally:
   
	   S32 textWidth = mProfile->mFont->getStrNWidth(textBuffer.getPtr(), textBuffer.length());

	   if ( drawRect.extent.x > textWidth )
	   {
		  switch( mProfile->mAlignment )
		  {
		  case GuiControlProfile::RightAlign:
			 drawPoint.x += ( drawRect.extent.x - textWidth );
			 break;
		  case GuiControlProfile::CenterAlign:
			 drawPoint.x += ( ( drawRect.extent.x - textWidth ) / 2 );
			 break;
		  }
	   }

	   ColorI fontColor = mActive ? mProfile->mFontColor : mProfile->mFontColorNA;

	   // now draw the text
	   Point2I cursorStart, cursorEnd;
	   mTextOffset = drawPoint;
	   if ( mTextOffsetReset )
	   {
		  mTextOffset.x = drawPoint.x;
		  mTextOffsetReset = false;
	   }

	   if ( drawRect.extent.x > textWidth )
		  mTextOffset.x = drawPoint.x;
	   else
	   {
		  // Alignment affects large text
		  if ( mProfile->mAlignment == GuiControlProfile::RightAlign
			 || mProfile->mAlignment == GuiControlProfile::CenterAlign )
		  {
			 if ( mTextOffset.x + textWidth < (drawRect.point.x + drawRect.extent.x))
				mTextOffset.x = (drawRect.point.x + drawRect.extent.x) - textWidth;
		  }
	   }

	   // calculate the cursor
	   if ( currentState == SelectedState )
	   {
		  // Where in the string are we?
		  S32 cursorOffset=0, charWidth=0;
		  UTF16 tempChar = mTextBuffer.getChar(mCursorPos);

		  // Alright, we want to terminate things momentarily.
		  if(mCursorPos > 0)
		  {
			 cursorOffset = mProfile->mFont->getStrNWidth(textBuffer.getPtr(), mCursorPos);
		  }
		  else
			 cursorOffset = 0;

		  if ( tempChar )
			 charWidth = mProfile->mFont->getCharWidth( tempChar );
		  else
			 charWidth = 0;

		  if( mTextOffset.x + cursorOffset + charWidth >= (drawRect.point.x + drawRect.extent.x))
		  {
			 // Cursor somewhere beyond the textcontrol,
			 // skip forward roughly 25% of the total width (if possible)
			 S32 skipForward = drawRect.extent.x / 4;

			 if ( cursorOffset + skipForward > textWidth )
				mTextOffset.x = (drawRect.point.x + drawRect.extent.x) - textWidth;
			 else
				mTextOffset.x -= skipForward;
		  }
		  else if( mTextOffset.x + cursorOffset < drawRect.point.x)
		  {
			 // Cursor somewhere before the textcontrol
			 // skip backward roughly 25% of the total width (if possible)
			 S32 skipBackward = drawRect.extent.x / 4;

			 if ( cursorOffset - skipBackward < 0 )
				mTextOffset.x = drawRect.point.x;
			 else
				mTextOffset.x += skipBackward;
		  }
		  cursorStart.x = mTextOffset.x + cursorOffset;
		  cursorEnd.x = cursorStart.x;

		  S32 cursorHeight = mProfile->mFont->getHeight();
		  if ( cursorHeight < drawRect.extent.y )
		  {
			 cursorStart.y = drawPoint.y;
			 cursorEnd.y = cursorStart.y + cursorHeight;
		  }
		  else
		  {
			 cursorStart.y = drawRect.point.y;
			 cursorEnd.y = cursorStart.y + drawRect.extent.y;
		  }
	   }

	   //draw the text
	   if ( currentState != SelectedState )
		  mBlockStart = mBlockEnd = 0;

	   //also verify the block start/end
	   if ((mBlockStart > (S32)textBuffer.length() || (mBlockEnd > (S32)textBuffer.length()) || (mBlockStart > mBlockEnd)))
		  mBlockStart = mBlockEnd = 0;

	   Point2I tempOffset = mTextOffset;

	   //draw the portion before the highlight
	   if ( mBlockStart > 0 )
	   {
		  dglSetBitmapModulation( fontColor );

		  const UTF16* preString2 = textBuffer.getPtr();
		  dglDrawTextN( mProfile->mFont, tempOffset, preString2, mBlockStart, mProfile->mFontColors);
		  tempOffset.x += mProfile->mFont->getStrNWidth(preString2, mBlockStart);
	   }

	   //draw the hilighted portion
	   if ( mBlockEnd > 0 )
	   {
		  const UTF16* highlightBuff = textBuffer.getPtr() + mBlockStart;
		  U32 highlightBuffLen = mBlockEnd-mBlockStart;

		  S32 highlightWidth = mProfile->mFont->getStrNWidth(highlightBuff, highlightBuffLen);

		  dglDrawRectFill( Point2I( tempOffset.x, drawRect.point.y + 1 ),
			 Point2I( tempOffset.x + highlightWidth, drawRect.point.y + drawRect.extent.y - 1),
			 mProfile->mFillColorHL );

		  dglSetBitmapModulation( mProfile->mFontColorHL );
		  dglDrawTextN( mProfile->mFont, tempOffset, highlightBuff, highlightBuffLen, mProfile->mFontColors );
		  tempOffset.x += highlightWidth;
	   }

	   //draw the portion after the highlite
	   if(mBlockEnd < (S32)mTextBuffer.length())
	   {
			const UTF16* finalBuff = textBuffer.getPtr() + mBlockEnd;
			U32 finalBuffLen = mTextBuffer.length() - mBlockEnd;

			dglSetBitmapModulation( fontColor );
			dglDrawTextN( mProfile->mFont, tempOffset, finalBuff, finalBuffLen, mProfile->mFontColors );
	   }

	   //draw the cursor
	   if (currentState == SelectedState && mCursorOn )
		  dglDrawLine( cursorStart, cursorEnd, mProfile->mCursorColor );

	   dglSetClipRect(old);
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
   return StringTable->insert(mTextBuffer.getPtr8());
}

void GuiTextEditCtrl::setScriptValue(const char *value)
{
   mTextBuffer.set(value);
   mCursorPos = getMin((S32)(mTextBuffer.length() - 1), 0);
}


StringBuffer GuiTextEditCtrl::truncate(StringBuffer buffer, StringBuffer terminationString, S32 width)
{
	// Check if the buffer width exceeds the specified width
	S32 bufferWidth = textBufferWidth(buffer);

	// If not, just return the unmodified buffer
	if (bufferWidth <= width)
		return buffer;

	// Get the width of the termination string
	S32 terminationWidth = textBufferWidth(terminationString) + 6; // add an extra bit of space at the end

	// Calculate the new target width with space allowed for the termination string
	S32 targetWidth = width - terminationWidth;

	// If the target width is zero or less, just replace the entire buffer with the termination string
	if (targetWidth <= 0)
		return terminationString;

	// Step backwards in the buffer until we find the character that fits within the target width
	S32 currentWidth = 0;
	S32 count = 0;
	for (S32 i = 0; i < (S32)buffer.length(); i++)
	{
		if (currentWidth >= targetWidth)
			break;

		UTF16 c = buffer.getChar(i);
		currentWidth += mProfile->mFont->getCharXIncrement(c);
		count++;
	}

	// Get the substring
	StringBuffer retBuffer = buffer.substring(0, count - 2);

	// Append terminating string
	retBuffer.append(terminationString);

	return retBuffer;
}

S32 GuiTextEditCtrl::textBufferWidth(StringBuffer buffer)
{
	S32 charLength = 0;

	for (S32 count = 0; count < (S32)buffer.length(); count++)
	{
		UTF16 c = buffer.getChar(count);
		if (!mProfile->mFont->isValidChar(c))
			continue;

		charLength += mProfile->mFont->getCharXIncrement(c);
	}

	return charLength;
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
			if (cursorPos != 0 || (mInsertOn && mTextBuffer.getChar(0) == '-'))
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
			if (!mInsertOn && mTextBuffer.getChar(cursorPos) == '.')
			{
				return true;
			}

			const char* dot = dStrchr(mTextBuffer.getPtr8(), '.');
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

	//Let's start by clearing the history.
	mHistoryIndex = 0;
	mHistoryLast = -1;

	//Time to set the mode
	mInputMode = mode;

	//now let's parse that buffer and get rid of invalid characters
	if (mode != AllText)
	{
		bool oldInsert = mInsertOn;
		mInsertOn = false;
		for (S32 i = 0; i < MAX_STRING_LENGTH; i++)
		{
			const UTF16 character = mTextBuffer.getChar(i);
			if (character == '\0')
			{
				//Done and done.
				break;
			}
			
			if (!inputModeValidate(character, i))
			{
				//Bad Character! Let's remove it.
				mTextBuffer.cut(i, 1);

				//Step it back
				i--;
			}
		}
		mInsertOn = oldInsert;
	}
}