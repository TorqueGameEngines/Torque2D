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

#ifndef _GUICONSOLEEDITCTRL_H_
#define _GUICONSOLEEDITCTRL_H_

#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif
#ifndef _GUITEXTEDITCTRL_H_
#include "gui/guiTextEditCtrl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif

class GuiConsoleEditHistory
{
private:
	vector<string> historyList;
	string workingText;
	U32 index;

public:
	GuiConsoleEditHistory();
	inline void clearHistory() { historyList.clear(); workingText.clear(); index = 0; }
	inline void updateHistory(const string& text) { historyList.push_back(text); index = historyList.size(); workingText.clear(); }
	inline void setWorkingText(const string& text) { workingText = text; index = historyList.size(); }
	const string& getPrevHistory();
	const string& getNextHistory();

};

class GuiConsoleEditCtrl : public GuiTextEditCtrl
{
private:
   typedef GuiTextEditCtrl Parent;
   GuiConsoleEditHistory mHistory;

protected:
   bool mUseSiblingScroller;
   GuiScrollCtrl* mSiblingScroller;
   virtual bool handleCharacterInput(const GuiEvent& event);
   virtual bool handleEnterKey();
   virtual bool handleArrowKey(GuiDirection direction);
   virtual bool handleBackSpace();
   virtual bool handleDelete();

public:
   GuiConsoleEditCtrl();
   DECLARE_CONOBJECT(GuiConsoleEditCtrl);

   static void initPersistFields();

   bool onKeyDown(const GuiEvent &event);
};

#endif //_GUI_TEXTEDIT_CTRL_H
