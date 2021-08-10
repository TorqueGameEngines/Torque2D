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

#include "gui/guiMLTextCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "gui/guiCanvas.h"
#include "memory/frameAllocator.h"
#include "string/unicode.h"

IMPLEMENT_CONOBJECT(GuiMLTextCtrl);

const U32 GuiMLTextCtrl::csmTextBufferGrowthSize = 1024;

ConsoleMethod( GuiMLTextCtrl, setText, void, 3, 3,  "(text) Use the setText method to change the current text content of the control to text. This replaces all old content.\n"
                                                                "@param text The new contents for this control.\n"
                                                                "@return No return value.\n"
                                                                "@sa addText, getText")
{
   object->setText(argv[2], dStrlen(argv[2]));
}

ConsoleMethod( GuiMLTextCtrl, getText, const char*, 2, 2, "() Use the getText method to return the current text contents of the control, including all formatting characters.\n"
                                                                "@return Returns the entire text contents of the control or indicating no contents.\n"
                                                                "@sa addText")
{
   return( object->getTextContent() );
}

ConsoleMethod( GuiMLTextCtrl, addText, void, 4, 4, "( text , reformat ) Use the addText method to add new text to the control. You may optionally request that the control be reformatted.\n"
                                                                "@param text Text to add to control.\n"
                                                                "@param reformat A boolean value that when set to true forces the control to re-evaluate the entire contents and to redisplay it.\n"
                                                                "@return No return value.\n"
                                                                "@sa getText, setText, forceReflow")
{
   object->addText(argv[2], dStrlen(argv[2]), dAtob(argv[3]));
}

ConsoleMethod( GuiMLTextCtrl, setCursorPosition, bool, 3, 3, "(newPos) Use the setCursorPosition method to offset the cursor by newPos characters into the current text contents of the control.\n"
                                                                "@param newPos An integer value indicating the character position at which to place the cursor.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToTag, scrollToTop")
{
   return object->setCursorPosition(dAtoi(argv[2]));
}

ConsoleMethod( GuiMLTextCtrl, scrollToTag, void, 3, 3, "( tagID ) Use the scrollToTag method to scroll to the first instance of a tag if it exists.\n"
                                                                "@param tagID A tag number to search for. These tags are specified by embedding TorqueML <tag:tag_number> entries in text.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToTop, setCursorPosition")
{
   object->scrollToTag( dAtoi( argv[2] ) );
}

ConsoleMethod( GuiMLTextCtrl, scrollToTop, void, 2, 2, "() Use the scrollToTop method to scroll to the top of the text.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToTag, setCursorPosition, scrollToBottom")
{
   object->scrollToTop();
}

ConsoleMethod( GuiMLTextCtrl, scrollToBottom, void, 2, 2, "() Use the scrollToBottom method to scroll to the bottom of the text.\n"
                                                                "@return No return value.\n"
                                                                "@sa scrollToTag, setCursorPosition, scrollToTop")
{
   object->scrollToBottom();
}

ConsoleFunction( StripMLControlChars, const char*, 2, 2, "( sourceString ) Use the stripMLControlChars function to remove all Torque Markup-Language (ML) symbols from sourceString.\n"
                                                                "This may not remove <br> correctly, so check before you trust this function.\n"
                                                                "@param sourceString The string to be modified.\n"
                                                                "@return Returns a copy of sourceString with all the ML symbols removed, or the original string if no ML symbols were present.\n"
                                                                "@sa stripChars")
{
   return GuiMLTextCtrl::stripControlChars(argv[1]);
}

ConsoleMethod(GuiMLTextCtrl,forceReflow,void,2,2,"() Use the forceReflow method to force the text control to re-evaluate the entire contents and to redisplay it, possibly resizing the control.\n"
                                                                "@return No return value.\n"
                                                                "@sa addText")
{
   if(!object->isAwake())
      Con::errorf("GuiMLTextCtrl::forceReflow can only be called on visible controls.");
   else
      object->reflow();
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::GuiMLTextCtrl()
{
   mIsEditCtrl = false;

   mCursorPosition     = 0;

   mMaxBufferSize = -1;
   mInitialText = StringTable->EmptyString;

   mSelectionActive = false;
   mSelectionStart  = 0;
   mSelectionEnd    = 0;

   mLineSpacingPixels = 2;
   mAllowColorChars = false;
   mBitmapList = 0;
   mBitmapRefList = 0;
   mFontList = 0;
   mDirty = true;
   mLineList = 0;
   mTagList = 0;
   mHitURL = 0;
   mActive = true;
   mAlpha = 1.0;
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::~GuiMLTextCtrl()
{
   mCursorPosition     = 0;

   mSelectionActive = false;
   mSelectionStart  = 0;
   mSelectionEnd    = 0;
   freeResources();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("lineSpacing",       TypeS32,    Offset(mLineSpacingPixels, GuiMLTextCtrl));
   addField("allowColorChars",   TypeBool,   Offset(mAllowColorChars,   GuiMLTextCtrl));
   addField("maxChars",          TypeS32,    Offset(mMaxBufferSize,     GuiMLTextCtrl));
   addField("deniedSound",       TypeAudioAssetPtr, Offset(mDeniedSound, GuiMLTextCtrl));
   addField("text",              TypeCaseString,  Offset( mInitialText, GuiMLTextCtrl ) );
}

ConsoleMethod(GuiMLTextCtrl, setAlpha, void, 3, 3, "( alpha ) Use the setAlpha method to set alpha of this control to between [0.0 , 1.0].\n"
                                                                "@param alpha A floating point value between 0.0 and 1.0 indicating the control's new alpha setting.\n"
                                                                "@return No return value")
{
   object->setAlpha(dAtof(argv[2]));
}

//--------------------------------------------------------------------------

bool GuiMLTextCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (!mTextBuffer.length() && mInitialText[0] != 0)
      setText(mInitialText, dStrlen(mInitialText)+1);
   return true;
}


//--------------------------------------------------------------------------
bool GuiMLTextCtrl::onWake()
{
   if (Parent::onWake() == false)
      return false;

   mDirty = true;
   return true;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::onPreRender()
{
   if(mDirty)
      reflow();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::drawAtomText(bool sel, U32 start, U32 end, Atom *atom, Line *line, Point2I offset)
{
   GFont *font = atom->style->font->fontRes;
   U32 xOff = 0;
   if(start != atom->textStart)
   {
      const UTF16* buff = mTextBuffer.getPtr() + atom->textStart;
      xOff += font->getStrNWidth(buff, start - atom->textStart);
   }

   Point2I drawPoint(offset.x + atom->xStart + xOff, offset.y + atom->yStart);

   ColorI color;
   if(atom->url)
   { 
      if(atom->url->mouseDown)
         color = atom->style->linkColorHL;
      else
         color = atom->style->linkColor;
   }
   else
      color = atom->style->color;
            
   const UTF16* tmp = mTextBuffer.getPtr() + start;
   U32 tmpLen = end-start;

   if(!sel)
   {
      if(atom->style->shadowOffset.x || atom->style->shadowOffset.y)
      {
         ColorI shadowColor = atom->style->shadowColor;
         shadowColor.alpha = (U8)(shadowColor.alpha * mAlpha);
         dglSetBitmapModulation(shadowColor);

         dglDrawTextN(font, drawPoint + atom->style->shadowOffset, tmp, tmpLen, mAllowColorChars ? mProfile->mFontColors : NULL);
      }

      color.alpha = (U8)(color.alpha * mAlpha);
      dglSetBitmapModulation(color);
      dglDrawTextN(font, drawPoint, tmp, end-start, mAllowColorChars ? mProfile->mFontColors : NULL);

      //if the atom was "clipped", see if we need to draw a "..." at the end
      if (atom->isClipped)
      {
         Point2I p2 = drawPoint;
         p2.x += font->getStrNWidthPrecise(tmp, tmpLen);
         dglDrawTextN(font, p2, "...", 3, mAllowColorChars ? mProfile->mFontColors : NULL);
      }
   }
   else
   {
      RectI rect;
      rect.point.x = drawPoint.x;
      rect.point.y = line->y + offset.y;
      rect.extent.x = font->getStrNWidth(tmp, tmpLen) + 1;
      rect.extent.y = line->height + 1;
      
      dglDrawRectFill(rect, mProfile->mFillColorHL);
      dglSetBitmapModulation( mProfile->mFontColorHL );  // over-ride atom color:
      dglDrawTextN(font, drawPoint, tmp, tmpLen, mAllowColorChars ? mProfile->mFontColors : NULL);

      //if the atom was "clipped", see if we need to draw a "..." at the end
      if (atom->isClipped)
      {
         Point2I p2 = drawPoint;
         p2.x += font->getStrNWidthPrecise(tmp, end - atom->textStart);
         dglDrawTextN(font, p2, "...", 3, mAllowColorChars ? mProfile->mFontColors : NULL);
      }
   }

   if(atom->url && !atom->url->noUnderline)
   {
      drawPoint.y += atom->baseLine + 2;
      Point2I p2 = drawPoint;
      p2.x += font->getStrNWidthPrecise(tmp, end - atom->textStart);
      dglDrawLine(drawPoint, p2, color);
   }
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   Parent::onRender(offset, updateRect);
   // draw all the bitmaps
   for(BitmapRef *walk = mBitmapRefList; walk; walk = walk->next)
   {
      RectI screenBounds = *walk;
      screenBounds.point += offset;
      if(!screenBounds.overlaps(updateRect))
         continue;
      dglClearBitmapModulation();
      dglDrawBitmap(walk->bitmap->bitmapHandle, screenBounds.point);
      //dglDrawRectFill(screenBounds, mProfile->mFillColor);
   }

   // draw all the text and dividerStyles
   for(Line *lwalk = mLineList; lwalk; lwalk = lwalk->next)
   {
      RectI lineRect(offset.x, offset.y + lwalk->y, mBounds.extent.x, lwalk->height);

      if(!lineRect.overlaps(updateRect))
         continue;

      if(lwalk->divStyle)
         dglDrawRectFill(lineRect, mProfile->mFillColorHL);

      for(Atom *awalk = lwalk->atomList; awalk; awalk = awalk->next)
      {
         if(!mSelectionActive || mSelectionEnd < awalk->textStart || mSelectionStart >= awalk->textStart + awalk->len)
            drawAtomText(false, awalk->textStart, awalk->textStart + awalk->len, awalk, lwalk, offset);
         else
         {
            U32 selectionStart = getMax(awalk->textStart, mSelectionStart);
            U32 selectionEnd = getMin(awalk->textStart + awalk->len, mSelectionEnd + 1);

            // draw some unselected text
            if(selectionStart > awalk->textStart)
               drawAtomText(false, awalk->textStart, selectionStart, awalk, lwalk, offset);

            // draw the selection
            drawAtomText(true, selectionStart, selectionEnd, awalk, lwalk, offset);

            if(selectionEnd < awalk->textStart + awalk->len)
               drawAtomText(false, selectionEnd, awalk->textStart + awalk->len, awalk, lwalk, offset);
         }
      }
   }
   dglClearBitmapModulation();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::freeLineBuffers()
{
   mViewChunker.freeBlocks();
   mLineList = NULL;
   mBitmapRefList = NULL;
   mTagList = NULL;
   mHitURL = 0;
   mDirty = true;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::freeResources()
{
   for(Font* walk = mFontList; walk; walk = walk->next)
   {
      delete[] walk->faceName;
      walk->fontRes = 0;
   }

   for(Bitmap* bwalk = mBitmapList; bwalk; bwalk = bwalk->next)
      bwalk->bitmapHandle = 0;

   mFontList = NULL;
   mBitmapList = NULL;
   mResourceChunker.freeBlocks();
   mDirty = true;

   freeLineBuffers();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::onSleep()
{
   freeResources();
   Parent::onSleep();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::inspectPostApply()
{
   Parent::inspectPostApply();

   if (mInitialText[0] != 0)
      setText(mInitialText, dStrlen(mInitialText));

   if ((S32)mLineSpacingPixels < 0)
      mLineSpacingPixels = 0;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::resize( const Point2I& newPosition, const Point2I& newExtent )
{
   Parent::resize( newPosition, newExtent );
   //Con::executef( this, 3, "onResize", Con::getIntArg( newExtent.x ), Con::getIntArg( newExtent.y ) );
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::parentResized(const Point2I& oldParentExtent,
                                  const Point2I& newParentExtent)
{
   Parent::parentResized(oldParentExtent, newParentExtent);
   mDirty = true;
}

//--------------------------------------------------------------------------
U32 GuiMLTextCtrl::getNumChars() const
{
   return mTextBuffer.length();
}

//--------------------------------------------------------------------------
U32 GuiMLTextCtrl::getText(char* pBuffer, const U32 bufferSize) const
{
   mTextBuffer.getCopy8(pBuffer, bufferSize);

   return getMin(mTextBuffer.length(), bufferSize);
}

//--------------------------------------------------------------------------
const char* GuiMLTextCtrl::getTextContent()
{
   if ( mTextBuffer.length() > 0 )
   {
      UTF8* returnString = Con::getReturnBuffer( mTextBuffer.getUTF8BufferSizeEstimate() );
      mTextBuffer.getCopy8(returnString, mTextBuffer.getUTF8BufferSizeEstimate() );
      return returnString;
   }

   return( "" );
}

//--------------------------------------------------------------------------
const char *GuiMLTextCtrl::getScriptValue()
{
   return getTextContent();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::setScriptValue(const char *newText)
{
   setText(newText, dStrlen(newText));
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::setText(const char* textBuffer, const U32 numChars)
{
   U32 chars = numChars;
   if(numChars >= (U32)mMaxBufferSize)
      chars = mMaxBufferSize;

   // leaving this usage because we StringBuffer.set((UTF8*)) cannot take a numChars arg.
   // perhaps it should? -paxorr
   FrameTemp<UTF8> tmp(chars+1);
   dStrncpy(tmp, textBuffer, chars);
   tmp[chars] = 0;

   mTextBuffer.set(tmp);

   //after setting text, always set the cursor to the beginning
   setCursorPosition(0);
   clearSelection();
   mDirty = true;
   scrollToTop();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::addText(const char* textBuffer, const U32 numChars, bool reformat)
{
   if(numChars >= (U32)mMaxBufferSize)
      return;

   FrameTemp<UTF8> tmp(numChars+1);
   dStrncpy(tmp, textBuffer, numChars);
   tmp[numChars] = 0;

   mTextBuffer.append(tmp);

   //after setting text, always set the cursor to the beginning
   if (reformat)
   {
      setCursorPosition(0);
      clearSelection();
      mDirty = true;
      scrollToTop();
   }
}

//--------------------------------------------------------------------------
bool GuiMLTextCtrl::setCursorPosition(const S32 newPosition)
{
   if (newPosition < 0) 
   {
      mCursorPosition = 0;
      return true;
   }
   else if ((U32)newPosition >= mTextBuffer.length()) 
   {
      mCursorPosition = mTextBuffer.length();
      return true;
   }
   else 
   {
      mCursorPosition = newPosition;
      return false;
   }
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::ensureCursorOnScreen()
{
   // If our parent isn't a scroll control, or we're not the only control
   //  in the content region, bail...
   GuiControl* pParent = getParent();
    GuiScrollCtrl *sc = dynamic_cast<GuiScrollCtrl*>(pParent);
    if(!sc)
        return;

   // Ok.  Now we know that our parent is a scroll control.  Let's find the
   //  top of the cursor, and it's bottom.  We can then scroll the parent control
   //  if appropriate...

   Point2I cursorTopP, cursorBottomP;
   ColorI color;
   getCursorPositionAndColor(cursorTopP, cursorBottomP, color);

    sc->scrollRectVisible(RectI(cursorTopP.x, cursorTopP.y, 1, cursorBottomP.y - cursorTopP.y));
}

//--------------------------------------
void GuiMLTextCtrl::getCursorPositionAndColor(Point2I &cursorTop, Point2I &cursorBottom, ColorI &color)
{
   S32 x = 0;
   S32 y = 0;
   S32 height = mProfile->mFont->getHeight();
   color = mProfile->mCursorColor;
   for(Line *walk = mLineList; walk; walk = walk->next)
   {
      if ((mCursorPosition <= walk->textStart + walk->len) || (walk->next == NULL))
      {
         // it's in the atoms on this line...
         y = walk->y;
         height = walk->height;

         for(Atom *awalk = walk->atomList; awalk; awalk = awalk->next)
         {

            if(mCursorPosition < awalk->textStart)
            {
               x = awalk->xStart;
               goto done;
            }

            if(mCursorPosition > awalk->textStart + awalk->len)
            {
               x = awalk->xStart + awalk->width;
               continue;
            }

            // it's in the text block...
            x = awalk->xStart;

            //
            // [neo, 5/7/2007]: cannot use const as GFont::getStrNWidth() is not declared as const
            //
            //const GFont *font = awalk->style->font->fontRes;
            GFont *font = awalk->style->font->fontRes;

            const UTF16* buff = mTextBuffer.getPtr() + awalk->textStart;
            x += font->getStrNWidth(buff, mCursorPosition - awalk->textStart - 1);

            color = awalk->style->color;
            goto done;
         }

         //if it's within this walk's width, but we didn't find an atom, leave the cursor at the beginning of the line...
         goto done;
      }
   }
done:
   cursorTop.set(x, y);
   cursorBottom.set(x, y + height);
}

//--------------------------------------------------------------------------
// Keyboard events...
bool GuiMLTextCtrl::onKeyDown(const GuiEvent& event)
{
   //only cut/copy work with this control...
   if (event.modifier & SI_CTRL)
   {
      switch(event.keyCode)
      {
         //copy
         case KEY_C:
         {
            //make sure we actually have something selected
            if (mSelectionActive)
            {
               copyToClipboard(mSelectionStart, mSelectionEnd);
               setUpdate();
            }
            return true;
         }
      }
   }

   // Otherwise, let the parent have the event...
   return Parent::onKeyDown(event);
}

//--------------------------------------------------------------------------
// Mousing events...
void GuiMLTextCtrl::onMouseDown(const GuiEvent& event)
{
   if(!mActive)
      return;

   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   if(hitAtom && !mIsEditCtrl)
   {
      mouseLock();
      mHitURL = hitAtom->url;
      if (mHitURL)
         mHitURL->mouseDown = true;
   }

   setFirstResponder();
   mouseLock();

   mSelectionActive = false;
   mSelectionAnchor        = getTextPosition(globalToLocalCoord(event.mousePoint));
   mSelectionAnchorDropped = event.mousePoint;
   if (mSelectionAnchor < 0)
      mSelectionAnchor = 0;
   else if ((U32)mSelectionAnchor >= mTextBuffer.length())
      mSelectionAnchor = mTextBuffer.length();

   mVertMoveAnchorValid = false;

   setUpdate();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::onMouseDragged(const GuiEvent& event)
{
   if (!mActive || (Canvas->getMouseLockedControl() != this))
      return;

   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   bool down = false;

   //note mHitURL can't be set unless this is (!mIsEditCtrl)
   if(hitAtom && hitAtom->url == mHitURL)
      down = true;

   if(mHitURL && down != mHitURL->mouseDown)
      mHitURL->mouseDown = down;

   if (!mHitURL)
   {
      S32 newSelection = 0;
      newSelection = getTextPosition(globalToLocalCoord(event.mousePoint));
      if (newSelection < 0)
         newSelection = 0;
      else if ((U32)newSelection > mTextBuffer.length())
         newSelection = mTextBuffer.length();

      if (newSelection == mSelectionAnchor) 
      {
         mSelectionActive = false;
      }
      else if (newSelection > mSelectionAnchor) 
      {
         mSelectionActive = true;
         mSelectionStart  = mSelectionAnchor;
         mSelectionEnd    = newSelection - 1;
      }
      else 
      {
         mSelectionStart  = newSelection;
         mSelectionEnd    = mSelectionAnchor - 1;
         mSelectionActive = true;
      }
      setCursorPosition(newSelection);
      mDirty = true;
   }

   setUpdate();
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::onMouseUp(const GuiEvent& event)
{
   if (!mActive || (Canvas->getMouseLockedControl() != this))
      return;

   mouseUnlock();

   //see if we've clicked on a URL
   Atom *hitAtom = findHitAtom(globalToLocalCoord(event.mousePoint));
   if (mHitURL && hitAtom && hitAtom->url == mHitURL && mHitURL->mouseDown)
   {
      mHitURL->mouseDown = false;

      // Convert URL from UTF16 to UTF8.
      UTF8* url = mTextBuffer.createSubstring8(mHitURL->textStart, mHitURL->len);
      Con::executef(this, 2, "onURL", url);
      delete[] url;
      mHitURL = NULL;

      setUpdate();
      return;
   }

   //else, update our selection
   else
   {
      if ((event.mousePoint - mSelectionAnchorDropped).len() < 3)
         mSelectionActive = false;

      setCursorPosition(getTextPosition(globalToLocalCoord(event.mousePoint)));
      mVertMoveAnchorValid = false;
      setUpdate();
   }
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::insertChars(const char* inputChars,
                                const U32   numInputChars,
                                const U32   position)
{
   AssertFatal(isSelectionActive() == false, "GuiMLTextCtrl::insertChars: don't use this function when there's a selection active!");
   AssertFatal(position <= mTextBuffer.length(), "GuiMLTextCtrl::insertChars: can't insert outside of current text!");

   //make sure the text will fit...
   S32 numCharsToInsert = numInputChars;
   if ((U32)mMaxBufferSize > 0 && mTextBuffer.length() + numInputChars + 1 > (U32)mMaxBufferSize)
      numCharsToInsert = mMaxBufferSize - mTextBuffer.length() - 1;
   if (numCharsToInsert <= 0)
   {
      // Play the "Denied" sound:
      if ( numInputChars > 0 && mDeniedSound.notNull() )
      {
         AUDIOHANDLE handle = alxCreateSource( mDeniedSound );
         alxPlay( handle );
      }
      return;
   }

   mTextBuffer.insert(position, inputChars );

   if (mCursorPosition >= position) 
   {
      // Cursor was at or after the inserted text, move forward...
      mCursorPosition += numCharsToInsert;
   }

   AssertFatal(mCursorPosition <= mTextBuffer.length(), "GuiMLTextCtrl::insertChars: bad cursor position");
   mDirty = true;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::deleteChars(const U32 rangeStart,
                                const U32 rangeEnd)
{
   AssertFatal(isSelectionActive() == false, "GuiMLTextCtrl::deleteChars: don't use this function when there's a selection active");
   AssertFatal(rangeStart <= mTextBuffer.length() && rangeEnd <= mTextBuffer.length(),
               avar("GuiMLTextCtrl::deleteChars: can't delete outside of current text (%d, %d, %d)",
                    rangeStart, rangeEnd, mTextBuffer.length()));
   AssertFatal(rangeStart <= rangeEnd, "GuiMLTextCtrl::deleteChars: invalid delete range");

   // Currently deleting text doesn't resize the text buffer, perhaps this should
   //  change?
   mTextBuffer.cut(rangeStart, rangeEnd - rangeStart);

   if (mCursorPosition <= rangeStart) 
   {
      // Cursor placed before deleted text, ignore
   }
   else if (mCursorPosition > rangeStart && mCursorPosition <= rangeEnd) 
   {
      // Cursor inside deleted text, set to start of range
      mCursorPosition = rangeStart;
   }
   else 
   {
      // Cursor after deleted text, decrement by number of chars deleted
      mCursorPosition -= (rangeEnd - rangeStart) + 1;
   }

   AssertFatal(mCursorPosition <= mTextBuffer.length(), "GuiMLTextCtrl::deleteChars: bad cursor position");
   mDirty = true;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::copyToClipboard(const U32 rangeStart, const U32 rangeEnd)
{
   AssertFatal(rangeStart < mTextBuffer.length() && rangeEnd < mTextBuffer.length(),
               avar("GuiMLTextCtrl::copyToClipboard: can't copy outside of current text (%d, %d, %d)",
                    rangeStart, rangeEnd, mTextBuffer.length()));
   AssertFatal(rangeStart <= rangeEnd, "GuiMLTextCtrl::copyToClipboard: invalid copy range");

   //copy the selection to the clipboard

   UTF8* selection = mTextBuffer.createSubstring8(rangeStart, rangeEnd-rangeStart);
   Platform::setClipboard(selection);
   delete[] selection;
}

//--------------------------------------------------------------------------
bool GuiMLTextCtrl::isSelectionActive() const
{
   return mSelectionActive;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::clearSelection()
{
   mSelectionActive = false;
   mSelectionStart  = 0;
   mSelectionEnd    = 0;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::scrollToTag( U32 id )
{
   // If the parent control is not a GuiScrollContentCtrl, then this call is invalid:
   GuiScrollCtrl *pappy = dynamic_cast<GuiScrollCtrl*>(getParent());
   if ( !pappy )
      return;

   // Find the indicated tag:
   LineTag* tag = NULL;
   for ( tag = mTagList; tag; tag = tag->next )
   {
      if ( tag->id == id )
         break;
   }

   if ( !tag )
   {
      Con::warnf( ConsoleLogEntry::General, "GuiMLTextCtrl::scrollToTag - tag id %d not found!", id );
      return;
   }
    pappy->scrollRectVisible(RectI(0, tag->y, 1, 1));
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::scrollToTop()
{
   // If the parent control is not a GuiScrollContentCtrl, then this call is invalid:
   GuiScrollCtrl *pappy = dynamic_cast<GuiScrollCtrl*>(getParent());
   if ( !pappy )
      return;
    pappy->scrollRectVisible(RectI(0,0,0,0));
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::scrollToBottom()
{
   // If the parent control is not a GuiScrollContentCtrl, then this call is invalid:
   GuiScrollCtrl *pappy = dynamic_cast<GuiScrollCtrl*>(getParent());
   if ( !pappy )
      return;

   // Figure bounds for the bottom left corner
   RectI cornerBounds (0, getPosition().y + getExtent().y, 1, 1);
   pappy->scrollRectVisible(cornerBounds);
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::Atom *GuiMLTextCtrl::findHitAtom(const Point2I localCoords)
{
   AssertFatal(mAwake, "Can't get the text position of a sleeping control.");
   if(mDirty)
      reflow();
   for(Line *walk = mLineList; walk; walk = walk->next)
   {
      if((U32)localCoords.y < walk->y)
         return NULL;

      if(localCoords.y >= (S32)walk->y && localCoords.y < (S32)(walk->y + walk->height))
      {
         for(Atom *awalk = walk->atomList; awalk; awalk = awalk->next)
         {
            if(localCoords.x < (S32)awalk->xStart)
               return NULL;
            if(localCoords.x >= (S32)(awalk->xStart + awalk->width))
               continue;
            return awalk;
         }
      }
   }
   return NULL;
}

//--------------------------------------------------------------------------
S32 GuiMLTextCtrl::getTextPosition(const Point2I& localCoords)
{
   AssertFatal(mAwake, "Can't get the text position of a sleeping control.");
   if(mDirty)
      reflow();

   for(Line *walk = mLineList; walk; walk = walk->next)
   {
      if((S32)localCoords.y < (S32)walk->y)
         return walk->textStart;

      if(localCoords.y >= (S32)walk->y && localCoords.y < (S32)(walk->y + walk->height))
      {
         for(Atom *awalk = walk->atomList; awalk; awalk = awalk->next)
         {
            if(localCoords.x < (S32)awalk->xStart)
               return awalk->textStart;
            if(localCoords.x >= (S32)(awalk->xStart + awalk->width))
               continue;
            // it's in the text block...
            
            //
            // [neo, 5/7/2007]: cannot use const as getBreakPos() is not declared as const
            //
            //const GFont *font = awalk->style->font->fontRes;
            GFont *font = awalk->style->font->fontRes;

            const UTF16 *tmp16 = mTextBuffer.getPtr() + awalk->textStart;
            U32 bp = font->getBreakPos(tmp16, awalk->len, localCoords.x - awalk->xStart, false);
            return awalk->textStart + bp;
         }
         return walk->textStart + walk->len;
      }
   }
   return mTextBuffer.length() - 1;
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::Font *GuiMLTextCtrl::allocFont(const char *faceName, U32 faceNameLen, U32 size)
{
   // check if it's in the font list currently:
   for(Font *walk = mFontList; walk; walk = walk->next)
      if(faceNameLen == walk->faceNameLen &&
         !dStrncmp(walk->faceName, faceName, faceNameLen) &&
         size == walk->size)
         return walk;

   // Create!
   Font *ret;
   ret = constructInPlace((Font *) mResourceChunker.alloc(sizeof(Font)));
   ret->faceName = new char[faceNameLen+1];
   dStrncpy(ret->faceName, faceName, faceNameLen);
   ret->faceName[faceNameLen] = '\0';
   ret->faceNameLen = faceNameLen;
   ret->size = size;
   ret->next = mFontList;
   ret->fontRes = GFont::create(ret->faceName, size, mProfile->mFontDirectory);
   if(bool(ret->fontRes))
   {
      ret->next = mFontList;
      mFontList = ret;
      return ret;
   }
   return NULL;
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::Bitmap *GuiMLTextCtrl::allocBitmap(const char *bitmapName, U32 bitmapNameLen)
{
   for(Bitmap *walk = mBitmapList; walk; walk = walk->next)
   {   
      if (bitmapNameLen == walk->bitmapNameLen && !dStrncmp(walk->bitmapName, bitmapName, bitmapNameLen))
         return walk;
   }

   Bitmap *ret = constructInPlace((Bitmap *) mResourceChunker.alloc(sizeof(Bitmap)));
   ret->bitmapName = bitmapName;
   ret->bitmapNameLen = bitmapNameLen;
   char nameBuffer[256];
   AssertFatal(sizeof(nameBuffer) >= bitmapNameLen, "GuiMLTextCtrl::allocBitmap() - bitmap name too long");
   dStrncpy(nameBuffer, bitmapName, bitmapNameLen);
   nameBuffer[bitmapNameLen] = 0;
   ret->bitmapHandle = TextureHandle(nameBuffer, TextureHandle::BitmapTexture);
   if(bool(ret->bitmapHandle))
   {
      ret->next = mBitmapList;
      mBitmapList = ret;
      return ret;
   }
   return NULL;
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::LineTag *GuiMLTextCtrl::allocLineTag(U32 id)
{
   for ( LineTag* walk = mTagList; walk; walk = walk->next )
   {
      if ( walk->id == id )
      {
         Con::warnf( ConsoleLogEntry::General, "GuiMLTextCtrl - can't add duplicate line tags!" );
         return( NULL );
      }
   }
   LineTag* newTag = (LineTag*) mViewChunker.alloc( sizeof( LineTag ) );
   newTag->id = id;
   newTag->y = mCurY;
   newTag->next = mTagList;
   mTagList = newTag;

   return( newTag );
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::emitNewLine(U32 textStart)
{
   //clear any clipping
   mCurClipX = 0;

   Line *l = (Line *) mViewChunker.alloc(sizeof(Line));
   l->height = mCurStyle->font->fontRes->getHeight();
   l->y = mCurY;
   l->textStart = mLineStart;
   l->len = textStart - l->textStart;
   mLineStart = textStart;
   l->atomList = mLineAtoms;
   l->next = 0;
   l->divStyle = mCurDiv;
   *mLineInsert = l;
   mLineInsert = &(l->next);
   mCurX = mCurLMargin;
   mCurTabStop = 0;

   if(mLineAtoms)
   {
      // scan through the atoms in the line, get the largest height
      U32 maxBaseLine = 0;
      U32 maxDescent = 0;
      Atom* walk;

      for(walk = mLineAtoms; walk; walk = walk->next)
      {
         if(walk->baseLine > maxBaseLine)
            maxBaseLine = walk->baseLine;
         if(walk->descent > maxDescent)
            maxDescent = walk->descent;
         if(!walk->next)
         {
            l->len = walk->textStart + walk->len - l->textStart;
            mLineStart = walk->textStart + walk->len;
         }
      }
      l->height = maxBaseLine + maxDescent + mLineSpacingPixels;

      for(walk = mLineAtoms; walk; walk = walk->next)
         walk->yStart = mCurY + maxBaseLine - walk->baseLine;
   }
   mCurY += l->height;
   mLineAtoms = NULL;
   mLineAtomPtr = &mLineAtoms;

   // clear out the blocker list
   BitmapRef **blockList = &mBlockList;
   while(*blockList)
   {
      BitmapRef *blk = *blockList;
      if(blk->point.y + blk->extent.y <= (S32)mCurY)
         *blockList = blk->nextBlocker;
      else
         blockList = &(blk->nextBlocker);
   }
   if(mCurY > mMaxY)
      mMaxY = mCurY;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::emitBitmapToken(GuiMLTextCtrl::Bitmap *bmp, U32 textStart, bool bitmapBreak)
{
   if(mCurRMargin <= mCurLMargin)
      return;
   if(mCurRMargin - mCurLMargin < bmp->bitmapHandle.getWidth())
      return;

   BitmapRef *ref = (BitmapRef *) mViewChunker.alloc(sizeof(BitmapRef));
   ref->bitmap = bmp;
   ref->next = mBitmapRefList;
   mBitmapRefList = ref;

   // now we gotta insert it into the blocker list and figure out where it's spos to go...
   ref->extent.x = bmp->bitmapHandle.getWidth();
   ref->extent.y = bmp->bitmapHandle.getHeight();

   // find the first space in the blocker list that will fit this thats > curLMargin

   while(bitmapBreak && mBlockList != &mSentinel)
      emitNewLine(textStart);

   for(;;)
   {
      // loop til we find a line that fits...
      // we'll have to emitLine repeatedly to clear out the block lists...

      BitmapRef **walk = &mBlockList;
      U32 minx = mCurX;
      U32 maxx = mCurRMargin;

      while(*walk)
      {
         BitmapRef *blk = *walk;

         if(blk->point.x > (S32)minx)
         {
            U32 right = maxx;
            if(blk->point.x < (S32)right)
               right = blk->point.x;
            U32 width = right - minx;

            if(right > minx && width >= (U32)ref->extent.x) // we've found the spot...
            {
               // insert it:
               U32 x = minx;
               if(mCurJustify == CenterAlign)
                  x += (width - ref->extent.x) >> 1;
               else if(mCurJustify == RightAlign)
                  x += width - ref->extent.x;
               ref->point.x = x;
               ref->point.y = mCurY;
               ref->nextBlocker = blk;
               *walk = ref;
               if(ref->point.y + ref->extent.y > (S32)mMaxY)
                  mMaxY = ref->point.y + ref->extent.y;

               return;
            }
         }
         if((S32)minx < blk->point.x + blk->extent.x)
            minx = blk->point.x + blk->extent.x;
         // move on to the next blocker...
         walk = &(blk->nextBlocker);
      }
      // go to the next line...
      emitNewLine(textStart);
   }
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::emitTextToken(U32 textStart, U32 len)
{
   if(mCurRMargin <= mCurLMargin)
      return;

   GFont *font = mCurStyle->font->fontRes;
   Atom *a = (Atom *) mViewChunker.alloc(sizeof(Atom));
   a->url = mCurURL;

   a->style = mCurStyle;
   mCurStyle->used = true;

   a->baseLine = font->getBaseline();
   a->descent = font->getDescent();
   a->textStart = textStart;
   a->len = len;
   a->isClipped = false;
   a->next = NULL;
   *mEmitAtomPtr = a;
   mEmitAtomPtr = &(a->next);
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::processEmitAtoms()
{
   Atom *atomList = mEmitAtoms;
   mEmitAtoms = NULL;
   mEmitAtomPtr = &mEmitAtoms;

   bool bailout = false;

   while(atomList)
   {
      // split the tokenlist by space
      // first find the first space that the text can go into:
      BitmapRef *br = mBlockList;
      //bool bailout = false; // Scoping error here? Moved up one scope. -pw
      Atom *list = atomList;

      while(br && atomList)
      {
         // if the blocker is before the current x, ignore it.
         if(br->point.x + br->extent.x <= (S32)mCurX)
         {
            br = br->nextBlocker;
            continue;
         }
         // if cur x is in the middle of the blocker
         // advance cur x to right edge of blocker.
         if((S32)mCurX >= br->point.x)
         {
            mCurX = br->point.x + br->extent.x;
            br = br->nextBlocker;
            continue;
         }
         // get the remaining width
         U32 right = br->point.x;
         if(right > mCurRMargin)
            right = mCurRMargin;

         //if we're clipping text, readjust
         if (mCurClipX > 0 && right > mCurClipX)
            right = mCurClipX;

         // if there's no room, break to the next line...
         if(right <= mCurX)
            break;
         // we've got some space:
         U32 width = right - mCurX;
         atomList = splitAtomListEmit(atomList, width);
         if(atomList) // there's more, so advance cur x
         {
            mCurX = br->point.x + br->extent.x;
            br = br->nextBlocker;
         }
      }
      if(mBlockList == &mSentinel && atomList == list)
      {
         if(bailout)
            return;
         else
            bailout = true;
      }
      // is there more to process for the next line?
      if(atomList)
         emitNewLine(mScanPos);
   }
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::Atom *GuiMLTextCtrl::splitAtomListEmit(Atom *list, U32 width)
{
   U32 totalWidth = 0;
   Atom *emitList = 0;
   Atom **emitPtr = &emitList;

   bool adjustClipAtom = false;
   Atom *clipAtom = NULL;
   bool emitted = false;

   while(list)
   {
      GFont *font = list->style->font->fontRes;
      U32 breakPos;

      const UTF16 *tmp16 = mTextBuffer.getPtr() + list->textStart;

      //if we're clipping the text, we don't break within an atom, we adjust the atom to only render
      //the portion of text that does fit, and to ignore the rest...
      if (mCurClipX > 0)
      {
         //find out how many character's fit within the given width
         breakPos = font->getBreakPos(tmp16, list->len, width - totalWidth, false);

         //if there isn't room for even the first character...
         if (breakPos == 0)
         {
            //set the atom's len and width to prevent it from being drawn
            list->len = 0;
            list->width = 0;
            adjustClipAtom = true;
         }

         //if our text doesn't fit within the clip region, add a "..."
         else if (breakPos != list->len)
         {
            U32 etcWidth = font->getStrNWidthPrecise("...", 3);
            breakPos = font->getBreakPos(tmp16, list->len, width - totalWidth - etcWidth, false);

            //again, if there isn't even room for a single character before the "...."
            if (breakPos == 0)
            {
               //set the atom's len and width to prevent it from being drawn
               list->len = 0;
               list->width = 0;
               adjustClipAtom = true;
            }
            else
            {
               //set the char len to the break pos, and the rest of the characters in this atom will be ignored
               list->len = breakPos;
               list->width = width - totalWidth;

               //mark this one as clipped
               list->isClipped = true;
               clipAtom = NULL;
            }
         }
      
         //otherwise no need to treat this atom any differently..
         else
         {
            //set the atom width == to the string length
            list->width = font->getStrNWidthPrecise(tmp16, breakPos);

            //set the pointer to the last atom that fit within the clip region
            clipAtom = list;
         }
      }
      else
      {
         breakPos = font->getBreakPos(tmp16, list->len, width - totalWidth, true);
         if(breakPos == 0 || (breakPos < list->len && mTextBuffer.getChar(list->textStart + breakPos - 1)!= ' ' && emitted))
            break;

         //set the atom width == to the string length
         list->width = font->getStrNWidthPrecise(tmp16, breakPos);
      }

      //update the total width
      totalWidth += list->width;
      
      // see if this is the last atom that will fit:
      Atom *emit = list;
      
      *emitPtr = emit;
      emitPtr = &(emit->next);
      emitted = true;

      //if we're clipping, don't split the atom, otherwise, see if it needs to be split
      if(!list->isClipped && breakPos != list->len)
      {
         Atom *a = (Atom *) mViewChunker.alloc(sizeof(Atom));
         a->url = list->url;
         a->textStart = list->textStart + breakPos;
         a->len = list->len - breakPos;
         a->next = list->next;
         a->baseLine = list->baseLine;
         a->descent = list->descent;
         a->style = list->style;
         a->isClipped = false;
         
         list = a;
         emit->len = breakPos;
         break;
      }
      list = list->next;
      if(totalWidth > width)
         break;
   }

   //if we had to completely clip an atom(s), the last (partially) visible atom should be modified to include a "..."
   if (adjustClipAtom && clipAtom)
   {
      GFont *font = clipAtom->style->font->fontRes;
      U32 breakPos;

      U32 etcWidth = font->getStrNWidthPrecise("...", 3);

      const UTF16 *tmp16 = mTextBuffer.getPtr() + clipAtom->textStart;

      breakPos = font->getBreakPos(tmp16, clipAtom->len, clipAtom->width - etcWidth, false);
      if (breakPos != 0)
      {
         clipAtom->isClipped = true;
         clipAtom->len = breakPos;
      }
   }

   // terminate the emit list:
   *emitPtr = 0;
   // now emit it:
   // going from mCurX to mCurX + width:
   if(mCurJustify == CenterAlign)
   {
      if ( width > totalWidth )
         mCurX += (width - totalWidth) >> 1;
   }
   else if(mCurJustify == RightAlign)
   {
      if ( width > totalWidth )
         mCurX += width - totalWidth;
   }
   while(emitList)
   {
      emitList->xStart = mCurX;
      mCurX += emitList->width;
      Atom *temp = emitList->next;
      *mLineAtomPtr = emitList;
      emitList->next = 0;
      mLineAtomPtr = &(emitList->next);
      emitList = temp;
   }
   return list;
}

//--------------------------------------------------------------------------
static bool scanforchar(const char *str, U32 &idx, char c)
{
   U32 startidx = idx;
   while(str[idx] != c && str[idx] && str[idx] != ':' && str[idx] != '>' && str[idx] != '\n')
      idx++;
   return str[idx] == c && startidx != idx;
}

//--------------------------------------------------------------------------
static S32 getHexVal(char c)
{
   if(c >= '0' && c <= '9')
      return c - '0';
   else if(c >= 'A' && c <= 'Z')
      return c - 'A' + 10;
   else if(c >= 'a' && c <= 'z')
      return c - 'a' + 10;
   return -1;
}

//--------------------------------------------------------------------------
GuiMLTextCtrl::Style *GuiMLTextCtrl::allocStyle(GuiMLTextCtrl::Style *style)
{
   Style *ret = (Style *) mViewChunker.alloc(sizeof(Style));
   ret->used = false;
   if(style)
   {
      ret->font = style->font;
      ret->color = style->color;
      ret->linkColor = style->linkColor;
      ret->linkColorHL = style->linkColorHL;
      ret->shadowColor = style->shadowColor;
      ret->shadowOffset = style->shadowOffset;
      ret->next = style->next;
   }
   else
   {
      ret->font = 0;
      ret->next = 0;
   }
   return ret;
}

//--------------------------------------------------------------------------
void GuiMLTextCtrl::reflow()
{
   AssertFatal(mAwake, "Can't reflow a sleeping control.");
   freeLineBuffers();
   mDirty = false;
   mScanPos = 0;

   mLineList = NULL;
   mLineInsert = &mLineList;

   mCurStyle = allocStyle(NULL);
   mCurStyle->font = allocFont((char *) mProfile->mFontType, dStrlen(mProfile->mFontType), mProfile->mFontSize);
   if(!mCurStyle->font)
      return;
   mCurStyle->color = mProfile->mFontColor;
   mCurStyle->shadowColor = mProfile->mFontColor;
   mCurStyle->shadowOffset.set(0,0);
   mCurStyle->linkColor = mProfile->mFontColors[GuiControlProfile::ColorUser0];
   mCurStyle->linkColorHL = mProfile->mFontColors[GuiControlProfile::ColorUser1];

   U32 width = mBounds.extent.x;

   mCurLMargin = 0;
   mCurRMargin = width;
   mCurJustify = LeftAlign;
   mCurDiv = 0;
   mCurY = 0;
   mCurX = 0;
   mCurClipX = 0;
   mLineAtoms = NULL;
   mLineAtomPtr = &mLineAtoms;

   mSentinel.point.x = width;
   mSentinel.point.y = 0;
   mSentinel.extent.x = 0;
   mSentinel.extent.y = 0x7FFFFF;
   mSentinel.nextBlocker = NULL;
   mLineStart = 0;
   mEmitAtoms = 0;
   mMaxY = 0;
   mEmitAtomPtr = &mEmitAtoms;

   mBlockList = &mSentinel;

   Font *nextFont;
   LineTag *nextTag;
   mTabStops = 0;
   mCurTabStop = 0;
   mTabStopCount = 0;
   mCurURL = 0;
   Style *newStyle;

   U32 textStart;
   U32 len;
   U32 idx;
   U32 sizidx;

   for(;;)
   {
      UTF16 curChar = mTextBuffer.getChar(mScanPos);

      if(!curChar)
         break;

      if(curChar == '\n')
      {
         textStart = mScanPos;
         len = 1;
         mScanPos++;
         processEmitAtoms();
         emitNewLine(textStart);
         mCurDiv = 0;
         continue;
      }

      if(curChar == '\t')
      {
         textStart = mScanPos;
         len = 1;
         mScanPos++;
         processEmitAtoms();
         if(mTabStopCount)
         {
            if(mCurTabStop < mTabStopCount)
            {
               if(mCurX < mTabStops[mCurTabStop])
                  mCurX = mTabStops[mCurTabStop];
            }
            mCurTabStop++;
         }
         continue;
      }

      if(curChar == '<')
      {
         // it's probably some kind of tag:

         // Get a pointer into the utf8 version of the buffer,
         // because we're still scanning text in in utf8 mode.
         const UTF8 *str = mTextBuffer.getPtr8();
         str = getNthCodepoint(str, mScanPos);

         //  And go!

         if(!dStrnicmp(str + 1, "br>", 3))
         {
            mScanPos += 4;
            len = 4;
            textStart = mScanPos + 4;
            processEmitAtoms();
            emitNewLine(textStart);
            mCurDiv = 0;
            continue;
         }

         if(!dStrnicmp(str + 1, "font:", 5))
         {
            // scan for the second colon...
            // at each level it should drop out to the text case below...
            idx = 6;
            if(!scanforchar(str, idx, ':'))
               goto textemit;

            sizidx = idx + 1;
            if(!scanforchar(str, sizidx, '>'))
               goto textemit;

            U32 size = dAtoi(str + idx + 1);
            if(!size || size > 64)
               goto textemit;
            textStart = mScanPos + 6;
            len = idx - 6;

            mScanPos += sizidx + 1;

            nextFont = allocFont(str + 6, len, size);
            if(nextFont)
            {
               if(mCurStyle->used)
                  mCurStyle = allocStyle(mCurStyle);
               mCurStyle->font = nextFont;
            }
            continue;
         }

         if ( !dStrnicmp( str + 1, "tag:", 4 ) )
         {
            idx = 5;
            if ( !scanforchar( str, idx, '>' ) )
               goto textemit;
            U32 tagId = dAtoi( str + 5 );
            nextTag = allocLineTag( tagId );

            mScanPos += idx + 1;
            continue;
         }

         if(!dStrnicmp(str +1, "color:", 6))
         {
            idx = 7;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            if(idx != 13 && idx != 15)
               goto textemit;
            ColorI color;

            color.red = getHexVal(str[7]) * 16 + getHexVal(str[8]);
            color.green = getHexVal(str[9]) * 16 + getHexVal(str[10]);
            color.blue = getHexVal(str[11]) * 16 + getHexVal(str[12]);
            if(idx == 15)
               color.alpha = getHexVal(str[13]) * 16 + getHexVal(str[14]);
            else
               color.alpha = 255;
            mScanPos += idx + 1;

            if(mCurStyle->used)
               mCurStyle = allocStyle(mCurStyle);
            mCurStyle->color = color;

            continue;
         }

         if(!dStrnicmp(str +1, "shadowcolor:", 12))
         {
            idx = 13;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            if(idx != 19 && idx != 21)
               goto textemit;
            ColorI color;

            color.red = getHexVal(str[13]) * 16 + getHexVal(str[14]);
            color.green = getHexVal(str[15]) * 16 + getHexVal(str[16]);
            color.blue = getHexVal(str[17]) * 16 + getHexVal(str[18]);
            if(idx == 21)
               color.alpha = getHexVal(str[19]) * 16 + getHexVal(str[20]);
            else
               color.alpha = 255;
            mScanPos += idx + 1;

            if(mCurStyle->used)
               mCurStyle = allocStyle(mCurStyle);
            mCurStyle->shadowColor = color;

            continue;
         }

         if(!dStrnicmp(str +1, "linkcolor:", 10))
         {
            idx = 11;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            if(idx != 17 && idx != 19)
               goto textemit;
            ColorI color;

            color.red = getHexVal(str[11]) * 16 + getHexVal(str[12]);
            color.green = getHexVal(str[13]) * 16 + getHexVal(str[14]);
            color.blue = getHexVal(str[15]) * 16 + getHexVal(str[16]);
            if(idx == 19)
               color.alpha = getHexVal(str[17]) * 16 + getHexVal(str[18]);
            else
               color.alpha = 255;
            mScanPos += idx + 1;

            if(mCurStyle->used)
               mCurStyle = allocStyle(mCurStyle);
            mCurStyle->linkColor = color;

            continue;
         }

         if(!dStrnicmp(str +1, "linkcolorhl:", 12))
         {
            idx = 13;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            if(idx != 19 && idx != 21)
               goto textemit;
            ColorI color;

            color.red = getHexVal(str[13]) * 16 + getHexVal(str[14]);
            color.green = getHexVal(str[15]) * 16 + getHexVal(str[16]);
            color.blue = getHexVal(str[17]) * 16 + getHexVal(str[18]);
            if(idx == 21)
               color.alpha = getHexVal(str[19]) * 16 + getHexVal(str[20]);
            else
               color.alpha = 255;
            mScanPos += idx + 1;

            if(mCurStyle->used)
               mCurStyle = allocStyle(mCurStyle);
            mCurStyle->linkColorHL = color;

            continue;
         }
         if(!dStrnicmp(str +1, "shadow:", 7))
         {
            idx = 8;
            if(!scanforchar(str, idx, ':'))
               goto textemit;
            U32 yidx = idx + 1;
            if(!scanforchar(str, yidx, '>'))
               goto textemit;
            mScanPos += yidx + 1;
            Point2I offset;
            offset.x = dAtoi(str + 8);
            offset.y = dAtoi(str + idx + 1);
            if(mCurStyle->used)
               mCurStyle = allocStyle(mCurStyle);
            mCurStyle->shadowOffset = offset;
            continue;
         }
         if(!dStrnicmp(str +1, "bitmap", 6))
         {
            S32 start = 8;
            bool bitBrk = false;
            if(str[7] == 'k' && str[8] == ':')
            {
               bitBrk = true;
               start = 9;
            }
            else if(str[7] != ':')
               goto textemit;

            idx = start;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            textStart = mScanPos + start;
            len = idx - start;

            mScanPos += idx + 1;

            processEmitAtoms();
            Bitmap *bmp;
            bmp = allocBitmap(str + 8, len);
            if(bmp)
               emitBitmapToken(bmp, textStart, bitBrk);
            continue;
         }

         if(!dStrnicmp(str +1, "spush>", 6))
         {
            mScanPos += 7;
            newStyle = allocStyle(mCurStyle); // copy out all the attributes...
            newStyle->next = mCurStyle;
            mCurStyle = newStyle;
            continue;
         }

         if(!dStrnicmp(str +1, "spop>", 5))
         {
            mScanPos += 6;
            if(mCurStyle->next)
               mCurStyle = mCurStyle->next;
            continue;
         }

         if(!dStrnicmp(str +1, "sbreak>", 7))
         {
            mScanPos += 8;
            processEmitAtoms();
            while(mBlockList != &mSentinel)
               emitNewLine(mScanPos);
            continue;
         }

         if(!dStrnicmp(str +1, "just:left>", 10))
         {
            processEmitAtoms();
            mCurJustify = LeftAlign;
            mScanPos += 11;
            continue;
         }

         if(!dStrnicmp(str +1, "just:right>", 11))
         {
            processEmitAtoms();
            mCurJustify = RightAlign;
            mScanPos += 12;
            continue;
         }

         if(!dStrnicmp(str +1, "just:center>", 12))
         {
            processEmitAtoms();
            mCurJustify = CenterAlign;
            mScanPos += 13;
            continue;
         }

         if(!dStrnicmp(str +1, "a:", 2))
         {
            idx = 3;
            if(!scanforchar(str, idx, '>'))
               goto textemit;

            mCurURL = (URL *) mViewChunker.alloc(sizeof(URL));
            mCurURL->mouseDown = false;
            mCurURL->textStart = mScanPos + 3;
            mCurURL->len = idx - 3;
            mCurURL->noUnderline = false;

            //if the URL is a "gamelink", don't underline...
            if (!dStrnicmp(str + 3, "gamelink", 8))
               mCurURL->noUnderline = true;

            mScanPos += idx + 1;
            continue;
         }

         if(!dStrnicmp(str+1, "/a>", 3))
         {
            mCurURL = NULL;
            mScanPos += 4;
            continue;
         }

         U32 margin;

         if(!dStrnicmp(str + 1, "lmargin%:", 9))
         {
            idx = 10;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            margin = (mBounds.extent.x * dAtoi(str + 10)) / 100;
            mScanPos += idx + 1;
            goto setleftmargin;
         }

         if(!dStrnicmp(str + 1, "lmargin:", 8))
         {
            idx = 9;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            margin = dAtoi(str + 9);
            mScanPos += idx + 1;
setleftmargin:
            processEmitAtoms();
            U32 oldLMargin;
            oldLMargin = mCurLMargin;
            mCurLMargin = margin;
            if(mCurLMargin >= width)
               mCurLMargin = width - 1;
            if(mCurX == oldLMargin)
               mCurX = mCurLMargin;
            if(mCurX < mCurLMargin)
               mCurX = mCurLMargin;
            continue;
         }

         if(!dStrnicmp(str + 1, "rmargin%:", 9))
         {
            idx = 10;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            margin = (mBounds.extent.x * dAtoi(str + 10)) / 100;
            mScanPos += idx + 1;
            goto setrightmargin;
         }

         if(!dStrnicmp(str + 1, "rmargin:", 8))
         {
            idx = 9;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            margin = dAtoi(str + 9);
            mScanPos += idx + 1;
setrightmargin:
            processEmitAtoms();
            mCurRMargin = margin;
            if(mCurLMargin >= width)
               mCurLMargin = width;
            if (mCurClipX > mCurRMargin)
               mCurClipX = mCurRMargin;
            continue;
         }

         if(!dStrnicmp(str + 1, "clip:", 5))
         {
            U32 clipWidth = 0;
            idx = 6;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            clipWidth = dAtoi(str + 6);
            mScanPos += idx + 1;
            processEmitAtoms();
            if (clipWidth > 0)
               mCurClipX = mCurX + clipWidth;
            else
               mCurClipX = 0;
            if(mCurClipX > mCurRMargin)
               mCurClipX = mCurRMargin;
            continue;
         }

         if(!dStrnicmp(str + 1, "/clip>", 6))
         {
            processEmitAtoms();
            mCurClipX = 0;
            mScanPos += 7;
            continue;
         }

         if(!dStrnicmp(str + 1, "div:", 4))
         {
            idx = 5;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            mScanPos += idx + 1;
            mCurDiv = dAtoi(str + 5);
            continue;
         }

         if(!dStrnicmp(str + 1, "tab:", 4))
         {
            idx = 5;
            if(!scanforchar(str, idx, '>'))
               goto textemit;
            // scan for tab stops...
            mTabStopCount = 1;
            idx = 5;
            while(scanforchar(str, idx, ','))
            {
               idx++;
               mTabStopCount++;
            }
            idx = 5;
            mTabStops = (U32 *) mViewChunker.alloc(sizeof(U32) * mTabStopCount);
            mTabStops[0] = dAtoi(str + idx);
            U32 i = 1;

            while(scanforchar(str, idx, ','))
            {
               idx++;
               mTabStops[i] = dAtoi(str + idx);
               i++;
            }
            mScanPos += idx + 1;
            continue;
         }
      }

      // default case:
textemit:
      textStart = mScanPos;
      idx = 1;
      while(mTextBuffer.getChar(mScanPos+idx) != '\t' && mTextBuffer.getChar(mScanPos+idx) != '<' && mTextBuffer.getChar(mScanPos+idx) != '\n' && mTextBuffer.getChar(mScanPos+idx))
         idx++;
      len = idx;
      mScanPos += idx;
      emitTextToken(textStart, len);
   }
   processEmitAtoms();
   emitNewLine(mScanPos);
   resize(mBounds.point, Point2I(mBounds.extent.x, mMaxY));
   Con::executef( this, 3, "onResize", Con::getIntArg( mBounds.extent.x ), Con::getIntArg( mMaxY ) );

   //make sure the cursor is still visible - this handles if we're a child of a scroll ctrl...
   ensureCursorOnScreen();
}

//-----------------------------------------------------------------------------
char* GuiMLTextCtrl::stripControlChars(const char *inString)
{
   if (! bool(inString))
      return NULL;
   U32 maxBufLength = 64;
   char *strippedBuffer = Con::getReturnBuffer(maxBufLength);
   char *stripBufPtr = &strippedBuffer[0];
   const char *bufPtr = (char *) inString;
   U32 idx, sizidx;

   for(;;)
   {
      //if we've reached the end of the string, or run out of room in the stripped Buffer...
      if(*bufPtr == '\0' || (U32(stripBufPtr - strippedBuffer) >= maxBufLength - 1))
         break;

      if (*bufPtr == '\n')
      {
         U32 walked;
         oneUTF8toUTF32(bufPtr,&walked);
         bufPtr += walked; 
         continue;
      }
      if(*bufPtr == '\t')
      {
         U32 walked;
         oneUTF8toUTF32(bufPtr,&walked);
         bufPtr += walked;
         continue;
      }
      if(*bufPtr < 0x20 && *bufPtr >= 0)
      {
         U32 walked;
         oneUTF8toUTF32(bufPtr,&walked);
         bufPtr += walked;
         continue;
      }

      if(*bufPtr == '<')
      {
         // it's probably some kind of tag:
         if(!dStrnicmp(bufPtr + 1, "font:", 5))
         {
            // scan for the second colon...
            // at each level it should drop out to the text case below...
            idx = 6;
            if(!scanforchar((char*)bufPtr, idx, ':'))
               goto textemit;

            sizidx = idx + 1;
            if(!scanforchar((char*)bufPtr, sizidx, '>'))
               goto textemit;

            bufPtr += sizidx + 1;
            continue;
         }

         if (!dStrnicmp(bufPtr + 1, "tag:", 4 ))
         {
            idx = 5;
            if ( !scanforchar((char*)bufPtr, idx, '>' ))
               goto textemit;

            bufPtr += idx + 1;
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "color:", 6))
         {
            idx = 7;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            if(idx != 13)
               goto textemit;

            bufPtr += 14;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "bitmap:", 7))
         {
            idx = 8;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;

            bufPtr += idx + 1;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "spush>", 6))
         {
            bufPtr += 7;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "spop>", 5))
         {
            bufPtr += 6;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "sbreak>", 7))
         {
            bufPtr += 8;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "just:left>", 10))
         {
            bufPtr += 11;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "just:right>", 11))
         {
            bufPtr += 12;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "just:center>", 12))
         {
            bufPtr += 13;
            continue;
         }

         if(!dStrnicmp(bufPtr +1, "a:", 2))
         {
            idx = 3;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;

            bufPtr += idx + 1;
            continue;
         }

         if(!dStrnicmp(bufPtr+1, "/a>", 3))
         {
            bufPtr += 4;
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "lmargin%:", 9))
         {
            idx = 10;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
            goto setleftmargin;
         }

         if(!dStrnicmp(bufPtr + 1, "lmargin:", 8))
         {
            idx = 9;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
setleftmargin:
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "rmargin%:", 9))
         {
            idx = 10;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
            goto setrightmargin;
         }

         if(!dStrnicmp(bufPtr + 1, "rmargin:", 8))
         {
            idx = 9;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
setrightmargin:
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "clip:", 5))
         {
            idx = 6;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "/clip>", 6))
         {
            bufPtr += 7;
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "div:", 4))
         {
            idx = 5;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
            continue;
         }

         if(!dStrnicmp(bufPtr + 1, "tab:", 4))
         {
            idx = 5;
            if(!scanforchar((char*)bufPtr, idx, '>'))
               goto textemit;
            bufPtr += idx + 1;
            continue;
         }
      }

      // default case:
textemit:
      *stripBufPtr++ = *bufPtr++;
      while(*bufPtr != '\t' && *bufPtr != '<' && *bufPtr != '\n' && (*bufPtr >= 0x20 || *bufPtr < 0))
         *stripBufPtr++ = *bufPtr++;
   }

   //we're finished - terminate the string
   *stripBufPtr = '\0';
   return strippedBuffer;
}

