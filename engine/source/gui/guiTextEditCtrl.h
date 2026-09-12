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

class GuiTextEditSelection
{
private:
	U32	mBlockAnchor;
	U32 mBlockStart;
	U32 mBlockEnd;
	U32 mCursorPos;
	bool mCursorAtEOL;
	bool mIsFirstResponder;
	RectI mGlobalUnadjustedCursorRect;
	bool mCursorRendered;
	U32 mTextLength;

	U32      mNumFramesElapsed;
	U32      mTimeLastCursorFlipped;
	bool     mCursorOn;

public:
	GuiTextEditSelection();
	GuiTextEditSelection(const GuiTextEditSelection& selector);
	inline U32 getSelStart() const { return mBlockStart; }
	inline U32 getSelEnd() const { return mBlockEnd; }
	inline U32 getCursorPos() const { return mCursorPos; }
	inline void setCursorPosition(const U32 newPos) { mBlockAnchor = mCursorPos = mClamp(newPos, 0, mTextLength); clearSelection(); }
	inline void setTextLength(const U32 length) { mTextLength = length; if (mCursorPos > length) mCursorPos = length; }
	inline const bool isCursorRendered() const { return mCursorRendered; }
	inline Point2I getCursorCenter() const { return mGlobalUnadjustedCursorRect.centre(); }
	inline RectI getCursorRect() const { return mGlobalUnadjustedCursorRect; }
	inline void setCursorRect(RectI rect) { mGlobalUnadjustedCursorRect = rect; mCursorRendered = true; }
	inline void setCursorAtEOL(const bool isEOL) { mCursorAtEOL = isEOL; }
	inline void setFirstResponder(const bool isFirstResponder) { mIsFirstResponder = isFirstResponder; mCursorRendered = mCursorRendered && isFirstResponder; }
	inline void clearSelection() { mBlockStart = mBlockEnd = 0; }
	void selectTo(const U32 target);
	inline bool hasSelection() { return mBlockEnd > mBlockStart; }
	void onPreRender(const U32 time);
	bool isIbeamOnLine(const U32 start, const U32 end, const bool isLastLine) const;
	bool renderIbeam(const Point2I& startPoint, const Point2I& extent, const string line, const U32 start, const U32 end, const bool isLastLine, GuiControlProfile* profile, GFont* font);
	inline string getSelection(const string& fullText) { return hasSelection() ? fullText.substr(mBlockStart, mBlockEnd - mBlockStart) : string(); }
	void eraseSelection(string& fullText);
	void stepCursorForward(const string& fullText);
	void stepCursorBackward(const string& fullText);
	void resetCursorBlink();
	void selectWholeWord(const string& text);

	/// The text is UTF-8 and every caret position is a byte offset into it, so
	/// a caret move of "one character" is one to four bytes. These find the
	/// start of the character after, or before, the one at pos, so that a caret
	/// never lands inside a character -- anything typed there would split it.
	static U32 nextCharacterStart(const string& text, const U32 pos);
	static U32 previousCharacterStart(const string& text, const U32 pos);
};

class GuiTextEditTextBlock
{
private:
	RectI mGlobalBounds;
	S32 mTextOffsetX;
	S32 mTextScrollX;
	U32 mLineStartIbeamValue;
	string mText;

public:
	GuiTextEditTextBlock();
	inline const U32 getStartValue() const { return mLineStartIbeamValue; }
	inline RectI getGlobalBounds() const { return mGlobalBounds; }
	inline Point2I getGlobalTextStart() { return Point2I(mGlobalBounds.point.x + mTextOffsetX - mTextScrollX, mGlobalBounds.point.y); }
	void render(const RectI& bounds, string line, U32 ibeamStartValue, GuiControlProfile* profile, GuiControlState currentState, GuiTextEditSelection& selector, AlignmentType align, GFont* font, bool isLastLine, bool overrideFontColor = false);
	U32 renderTextSection(const Point2I& startPoint, const U32 subStrStart, const U32 subStrLen, GuiControlProfile* profile, const GuiControlState currentState, GFont* font, bool isSelectedText = false, bool overrideFontColor = false);
	void performScrollJumpX(const S32 targetX, const S32 areaStart, const S32 areaEnd);
	U32 calculateIbeamPositionInLine(const S32 targetX, GFont* font);
	inline bool calculateCursorAtEOL(const U32 cursorPos) { return cursorPos == mText.length(); }
	inline void resetScroll() { mTextScrollX = 0; }
	void processScrollVelocity(const S32 delta, const S32 extentX, GFont* font);
	void processTextAlignment(const string line, GFont* font, AlignmentType align);
};

class GuiTextEditCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;
   typedef vector<GuiTextEditTextBlock> TextBlockList;

protected:

	S32 mMaxStrLen; 
   string mTextBuffer;
   TextBlockList mTextBlockList;

   StringTableEntry mReturnCommand;
   StringTableEntry mEscapeCommand;

public:
   //input mode
   enum InputMode
   {
		AllText = 0,
		Decimal,
		Number,
		Alpha,
		AlphaNumeric
   };

protected:
   InputMode mInputMode;
   bool mReturnCausesTab;

   //Edit Cursor
   GuiCursor*  mEditCursor;

   bool     mInsertOn;
   bool		mMouseOver;

   GuiTextEditSelection mSelector;

   S32 mTextOffsetY;
   F32 mScrollVelocity;
   const U32 SCROLL_EDGE_SIZE = 20;
   const F32 SCROLL_VELOCITY_PER_SEC = 200.0f;
   U32 mTimeLastScrollProcess;
   bool mSuspendVerticalScrollJump;

   //undo members
   string mUndoText;
   GuiTextEditSelection mUndoSelector;
   void saveUndoState();

   bool                 mPasswordText;
   StringTableEntry     mPasswordMask;

   /// If set, any non-ESC key is handled here or not at all
   bool    mSinkAllKeyEvents;

   /// The first half of a character outside the BMP, held until its second
   /// half arrives in the next event. See composeTypedCharacter.
   UTF16   mPendingHighSurrogate;

   const RectI getGlobalInnerRect();
   S32 calculateIbeamPosition(const Point2I &offset);
   S32 calculateIbeamPosition(const Point2I& globalMousePoint, const RectI& globalInnerRect);
   S32 calculateIbeamPositionInLine(string line, const S32 startX, const S32 targetX);
   S32 getLineAdjustedIbeamPosition(S32 heightAdjustment);

   virtual bool handleKeyDownWithShift(const GuiEvent& event);
   virtual bool handleKeyDownWithCtrl(const GuiEvent& event);
   virtual bool handleKeyDownWithAlt(const GuiEvent& event);
   virtual bool handleKeyDownWithNoModifier(const GuiEvent& event);
   virtual bool handleCharacterInput(const GuiEvent& event);
   virtual bool canDisplayCharacter(const UTF16 character);
   virtual bool insertCharacter(const string& character);
   virtual bool handleEscapeKey();
   virtual bool handleEnterKey();
   virtual bool insertNewLine();
   virtual bool handleArrowKey(GuiDirection direction);
   virtual bool handleShiftArrowKey(GuiDirection direction);
   virtual bool handleBackSpace();
   virtual bool handleDelete();
   void modifySelectBlock(const U32 target);

   /// What letting go of the keyboard means to the box itself: text input
   /// off, the caret gone, and half a character forgotten. Script hears about
   /// it separately, in onLoseFirstResponder -- except when the box is going to
   /// sleep, which lets go through this alone. See onSleep.
   void releaseKeyboard();

   S32 textBufferWidth(StringBuffer buffer);
   StringBuffer truncate(StringBuffer buffer, StringBuffer terminationString, S32 width);

public:
   GuiTextEditCtrl();
   DECLARE_CONOBJECT(GuiTextEditCtrl);
   static void initPersistFields();

   bool onAdd();
   void inspectPostApply();
   bool onWake();
   void onSleep();

   virtual const char* getText();
   virtual void getCursor(GuiCursor*& cursor, bool& showCursor, const GuiEvent& lastGuiEvent);

   virtual void setText(const UTF8* txt);
   virtual void setText(const UTF16* txt);
   inline virtual void setText(const string txt) { setText(txt.c_str()); }
   void enforceMaxLength();
   virtual void setTextID(S32 id);
   virtual void setTextID(const char *id);
   inline U32   getIbeamPosition()   { return mSelector.getCursorPos(); }
   inline void  setIbeamPosition(const U32 newPos) { mSelector.setCursorPosition(newPos); }
   
   void selectAllText();
   bool validate();
   const char *getScriptValue();
   void setScriptValue(const char *value);

   bool onKeyDown(const GuiEvent &event);

   /// Typed characters. A keyboard event's ascii is one UTF-16 code unit, and
   /// it arrives in one of two ways, depending on the platform: on the key-down
   /// of the key that typed it (Windows, macOS), or in an event of its own with
   /// no key behind it -- keyCode KEY_NULL -- after a key-down that carries
   /// nothing (SDL's text input, and IME input on Windows). Either way it goes
   /// into the text as UTF-8, which is how the box stores it.
   ///
   /// Whether a code unit is text at all: not NUL, and not a control code,
   /// which is what a key like Backspace or Ctrl+C reports as its character.
   static bool isTypedCharacter(const UTF16 unit);
   /// A character in an event of its own. It is typed text and nothing else --
   /// never a shortcut, whatever modifier happens to be set on it.
   static bool isCharacterEvent(const GuiEvent& event);
   /// A character typed with AltGr on Windows, which reports AltGr as left
   /// Ctrl plus right Alt. It is text, not a Ctrl shortcut: on a Polish
   /// keyboard AltGr+C is a letter, not Copy.
   static bool isAltGrCharacter(const GuiEvent& event);
   /// A key that types text when pressed with these modifiers -- a letter, a
   /// digit, space, punctuation, or a number-pad digit or operator, pressed
   /// alone, with Shift, or with AltGr -- and so belongs to a box that has the
   /// keyboard, whether or not its text rides on the key. On SDL it never does:
   /// the text follows in an event of its own, and the key itself used to go
   /// on to the game's action maps. With Ctrl, or with Alt (Cmd on a Mac), a
   /// key is a shortcut instead.
   static bool isTypingKey(const U8 keyCode, const U32 modifier);
   /// Whether the GlobalActionMap is bound to the key of this event. A key it
   /// is bound to is the map's while a box has the keyboard, unless it is one
   /// of the box's own editing keys, and it types nothing. See onKeyDown.
   static bool isGlobalKey(const GuiEvent& event);
   /// The UTF-8 one code unit adds to the text, written to outBuffer (which
   /// holds 3 bytes); returns how many bytes that is, 0 for none. A character
   /// outside the BMP arrives as two units, high surrogate first: the first is
   /// held in pendingHighSurrogate and adds nothing, and the second completes
   /// it. A surrogate without its partner adds nothing.
   static U32 composeTypedCharacter(const UTF16 unit, UTF16& pendingHighSurrogate, UTF8* outBuffer);

   void onTouchDown(const GuiEvent &event);
   void onTouchDragged(const GuiEvent &event);
   void onTouchUp(const GuiEvent& event);
   void onTouchEnter(const GuiEvent& event);
   void onTouchLeave(const GuiEvent& event);
   void onMouseWheelUp(const GuiEvent& event);
   void onMouseWheelDown(const GuiEvent& event);
   
   void onCopy(bool andCut);
   void onPaste();
   void onUndo();

   StringTableEntry mTextOnFocus;
   void onFocus(bool foundFirstResponder);
   virtual void setFirstResponder();
   virtual void onLoseFirstResponder();

   void parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
   bool hasText();

   void onStaticModified(const char* slotName);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
   virtual void renderLineList(const Point2I& offset, const Point2I& extent, const S32 startOffsetY, const vector<string> lineList, GuiControlProfile* profile, const TextRotationOptions rot = tRotateNone);

   GuiControlState getCurrentState();
   const ColorI& getCurrentFontColor();
	
	bool inputModeValidate(const U16 key, S32 cursorPos);
	void keyDenied();
	void execConsoleCallback();

	inline void setEscapeCommand(const char *newCmd) { mEscapeCommand = newCmd ? StringTable->insert(newCmd) : StringTable->EmptyString; }
	inline const char * getEscapeCommand() { return mEscapeCommand; }

	inline void setReturnCausesTab(bool setting) { mReturnCausesTab = setting; }
	inline bool getReturnCausesTab() { return mReturnCausesTab; }

	inline void setSinkAllKeyEvents(bool setting) { mSinkAllKeyEvents = setting; }
	inline bool getSinkAllKeyEvents() { return mSinkAllKeyEvents; }

	inline void setIsPassword(bool setting) { mPasswordText = setting; }
	inline bool getIsPassword() { return mPasswordText; }

	void setMaxLength(S32 max);
	inline S32 getMaxLength() { return mMaxStrLen; }
	static bool setMaxLengthProperty(void* obj, const char* data) { static_cast<GuiTextEditCtrl*>(obj)->setMaxLength(dAtoi(data)); return true; }

	void setInputMode(const InputMode mode);
	inline InputMode getInputMode() { return mInputMode; }
	static bool setInputMode(void* obj, const char* data)
	{
		// Fetch body type.
		const InputMode mode = getInputModeEnum(data);

		// Check for error.
		if (mode < AllText || mode > AlphaNumeric)
			return false;

		static_cast<GuiTextEditCtrl*>(obj)->setInputMode(mode);
		return true;
	}
	static const char* getInputMode(void* obj, const char* data) { return getInputModeDescription(static_cast<GuiTextEditCtrl*>(obj)->getInputMode()); }
	static bool writeInputMode(void* obj, StringTableEntry pFieldName) { return static_cast<GuiTextEditCtrl*>(obj)->getInputMode() != AllText; }
	static InputMode getInputModeEnum(const char* label);
	static const char* getInputModeDescription(const InputMode mode);

	string applyPasswordMasking();
	
	void adjustScrollVelocity(const Point2I& globalMousePoint, const RectI& globalInnerRect);
	
	void performScrollJumpY();
	void processScrollVelocity();

	enum Constants { MAX_STRING_LENGTH = 1024 };

	static bool writeSinkAllKeyEventsFn(void* obj, StringTableEntry pFieldName) { return static_cast<GuiTextEditCtrl*>(obj)->getSinkAllKeyEvents(); }
	static bool writePasswordFn(void* obj, StringTableEntry pFieldName) { return static_cast<GuiTextEditCtrl*>(obj)->getIsPassword(); }
	static bool writeReturnCausesTabFn(void* obj, StringTableEntry pFieldName) { return static_cast<GuiTextEditCtrl*>(obj)->getReturnCausesTab(); }

private:
	bool tabNext();
	bool tabPrev();
};

#endif //_GUI_TEXTEDIT_CTRL_H
