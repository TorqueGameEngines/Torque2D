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

#ifndef _GUITEXTEDITCTRL_H_
#define _GUITEXTEDITCTRL_H_

#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif
#ifndef _GUITEXTCTRL_H_
#include "gui/guiTextCtrl.h"
#endif

class GuiTextEditCtrl : public GuiTextCtrl
{
private:
   typedef GuiTextCtrl Parent;

   static U32 smNumAwake;

protected:

   StringBuffer mTextBuffer;

   StringTableEntry mValidateCommand;
   StringTableEntry mEscapeCommand;
   AssetPtr<AudioAsset>  mDeniedSound;

   // for animating the cursor
   S32      mNumFramesElapsed;
   U32      mTimeLastCursorFlipped;
   ColorI   mCursorColor;
   bool     mCursorOn;

   //Edit Cursor
   GuiCursor*  mEditCursor;

   bool     mInsertOn;
   S32      mMouseDragStart;
   Point2I  mTextOffset;
   bool     mTextOffsetReset;
   bool     mDragHit;
   bool     mTabComplete;
   S32      mScrollDir;

   //undo members
   StringBuffer mUndoText;
   S32      mUndoBlockStart;
   S32      mUndoBlockEnd;
   S32      mUndoCursorPos;
   void saveUndoState();

   S32      mBlockStart;
   S32      mBlockEnd;
   S32      mCursorPos;
   S32      setCursorPos(const Point2I &offset);
   
   bool                 mHistoryDirty;
   S32                  mHistoryLast;
   S32                  mHistoryIndex;
   S32                  mHistorySize;
   bool                 mPasswordText;
   StringTableEntry     mPasswordMask;

   /// If set, any non-ESC key is handled here or not at all
   bool    mSinkAllKeyEvents;   
   UTF16   **mHistoryBuf;
   void updateHistory(StringBuffer *txt, bool moveIndex);


public:
   GuiTextEditCtrl();
   ~GuiTextEditCtrl();
   DECLARE_CONOBJECT(GuiTextEditCtrl);
   static void initPersistFields();

   bool onAdd();
   bool onWake();
   void onSleep();

   /// Get the contents of the control.
   ///
   /// dest should be of size GuiTextCtrl::MAX_STRING_LENGTH+1.
   void getText(char *dest);

   virtual void setText(const UTF8* txt);
   virtual void setText(const UTF16* txt);
   S32   getCursorPos()   { return( mCursorPos ); }
   void  reallySetCursorPos( const S32 newPos );
   
   void selectAllText();
   void forceValidateText();
   const char *getScriptValue();
   void setScriptValue(const char *value);

   bool onKeyDown(const GuiEvent &event);
   void onTouchDown(const GuiEvent &event);
   void onTouchDragged(const GuiEvent &event);
   void onTouchUp(const GuiEvent &event);
   
   void onCopy(bool andCut);
   void onPaste();
   void onUndo();

   virtual void setFirstResponder();
   virtual void onLoseFirstResponder();

   void parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
   bool hasText();

   void onStaticModified(const char* slotName);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
   virtual void drawText( const RectI &drawRect, GuiControlState currentState );
	
	void playDeniedSound();
	void execConsoleCallback();	
};

#endif //_GUI_TEXTEDIT_CTRL_H
