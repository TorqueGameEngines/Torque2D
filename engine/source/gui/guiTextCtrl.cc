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
#include "gui/guiTextCtrl.h"
#include "graphics/dgl.h"
#include "gui/language/lang.h"

// -----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiTextCtrl);

GuiTextCtrl::GuiTextCtrl()
{
   //default fonts
   mInitialText = StringTable->EmptyString;
   mInitialTextID = StringTable->EmptyString;
   mText[0] = '\0';
   mMaxStrLen = GuiTextCtrl::MAX_STRING_LENGTH;
   mTruncateWhenUnfocused = false;
}

ConsoleMethod( GuiTextCtrl, setText, void, 3, 3, "( newText ) Use the setText method to set the content of label to newText.\n"
																"@param newText A string representing the new value for this label.\n"
																"@return No return value")
{
   object->setText( argv[2] );
}

ConsoleMethod( GuiTextCtrl, setTextID, void, 3, 3, "( int newTextID ) Set the textID to a new value.\n"
			  "@param newTextID The desired new ID\n"
			  "@return No return value.")
{
	object->setTextID( argv[2] );
}
void GuiTextCtrl::initPersistFields()
{
   Parent::initPersistFields();
   //addField( "text",       TypeCaseString,  Offset( mInitialText, GuiTextCtrl ) );
   addProtectedField("text", TypeCaseString, Offset(mInitialText, GuiTextCtrl), setText, getTextProperty, "");
   addField( "textID", TypeString, Offset( mInitialTextID, GuiTextCtrl ) );
   addField( "maxLength", TypeS32, Offset( mMaxStrLen, GuiTextCtrl ) );
   addField("truncate", TypeBool, Offset(mTruncateWhenUnfocused, GuiTextCtrl));
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

bool GuiTextCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;
   dStrncpy(mText, (UTF8*)mInitialText, MAX_STRING_LENGTH);
   mText[MAX_STRING_LENGTH] = '\0';
   return true;
}

void GuiTextCtrl::inspectPostApply()
{
   Parent::inspectPostApply();
   if(mInitialTextID && *mInitialTextID != 0)
	   setTextID(mInitialTextID);
   else
      setText(mInitialText);
}

bool GuiTextCtrl::onWake()
{
   if ( !Parent::onWake() )
      return false;
   
   mFont = mProfile->mFont;
   AssertFatal(mFont, "GuiTextCtrl::onWake: invalid font in profile" );
   if(mInitialTextID && *mInitialTextID != 0)
	   setTextID(mInitialTextID);

   if ( mConsoleVariable[0] )
   {
      const char *txt = Con::getVariable( mConsoleVariable );
      if ( txt )
      {
         if ( dStrlen( txt ) > (U32)mMaxStrLen )
         {
            char* buf = new char[mMaxStrLen + 1];
            dStrncpy( buf, txt, mMaxStrLen );
            buf[mMaxStrLen] = 0;
            setScriptValue( buf );
            delete [] buf;
         }
         else
            setScriptValue( txt );
      }
   }
   
   //resize
   //DEPRECIATED
   /*
   if ( mProfile->mAutoSizeWidth )
   {
      if ( mProfile->mAutoSizeHeight )
         resize( mBounds.point, Point2I( mFont->getStrWidth((const UTF8 *) mText ), mFont->getHeight() + 4 ) );
      else
         resize( mBounds.point, Point2I( mFont->getStrWidth((const UTF8 *) mText ), mBounds.extent.y ) );
   }
   else if ( mProfile->mAutoSizeHeight )
      resize( mBounds.point, Point2I( mBounds.extent.x, mFont->getHeight() + 4 ) );

*/
   return true;
}

void GuiTextCtrl::onSleep()
{
   Parent::onSleep();
   mFont = NULL;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

void GuiTextCtrl::setText(const char *txt)
{
   //make sure we don't call this before onAdd();
   if( !mProfile )
      return;
   
   if (txt)
      dStrncpy(mText, (UTF8*)txt, MAX_STRING_LENGTH);
   mText[MAX_STRING_LENGTH] = '\0';
   
   //Make sure we have a font
   mProfile->incRefCount();
   mFont = mProfile->mFont;
   
	//Luma:	If the font isn't found, we want to decrement the profile usage and return now or we may crash!
	if (mFont.isNull())	
	{
		//decrement the profile referrence
		mProfile->decRefCount();
		return;
	}
      
   setVariable((char*)mText);
   setUpdate();
   
   //decrement the profile referrence
   mProfile->decRefCount();
} 

void GuiTextCtrl::setTextID(const char *id)
{
	S32 n = Con::getIntVariable(id, -1);
	if(n != -1)
	{
		mInitialTextID = StringTable->insert(id);
		setTextID(n);
	}
}
void GuiTextCtrl::setTextID(S32 id)
{
	const UTF8 *str = getGUIString(id);
	if(str)
		setText((const char*)str);
	//mInitialTextID = id;
}

//------------------------------------------------------------------------------
void GuiTextCtrl::onPreRender()
{
   const char * var = getVariable();
   if(var && var[0] && dStricmp((char*)mText, var))
      setText(var);
}

//------------------------------------------------------------------------------
void GuiTextCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	StringBuffer textBuffer(mText);

    ColorI fontColor = mProfile->mFontColor;

	if (mTruncateWhenUnfocused)
    {
		StringBuffer terminationString = "...";
        S32 width = mBounds.extent.x;
        StringBuffer truncatedBuffer = truncate(textBuffer, terminationString, width);
        const UTF8* truncatedBufferPtr = truncatedBuffer.getPtr8();
        
        dglSetBitmapModulation(fontColor);
		renderText(offset, mBounds.extent, (char*)truncatedBufferPtr, mProfile);
    }
    else
    {
		dglSetBitmapModulation(fontColor);
		renderText(offset, mBounds.extent, (char*)mText, mProfile);
	}

    //render the child controls
    renderChildControls(offset, mBounds, updateRect);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //

const char *GuiTextCtrl::getScriptValue()
{
   return getText();
}

void GuiTextCtrl::setScriptValue(const char *val)
{
   setText(val);
}

StringBuffer GuiTextCtrl::truncate(StringBuffer buffer, StringBuffer terminationString, S32 width)
{
    // Check if the buffer width exceeds the specified width
    S32 bufferWidth = textBufferWidth(buffer);

    // If not, just return the unmodified buffer
    if (bufferWidth <= width)
        return buffer;

    // Get the width of the termination string
    S32 terminationWidth =  textBufferWidth(terminationString) + 6; // add an extra bit of space at the end

    // Calculate the new target width with space allowed for the termination string
    S32 targetWidth = width - terminationWidth;

    // If the target width is zero or less, just replace the entire buffer with the termination string
    if (targetWidth <= 0)
        return terminationString;

    // Step backwards in the buffer until we find the character that fits within the target width
    S32 currentWidth = 0;
    S32 count = 0;
    for(S32 i = 0; i < (S32)buffer.length(); i++)
    {
        if (currentWidth >= targetWidth)
            break;

        UTF16 c = buffer.getChar(i);
        currentWidth += mFont->getCharXIncrement( c ); 
        count++;
    }

    // Get the substring
    StringBuffer retBuffer = buffer.substring(0, count-2);

    // Append terminating string
    retBuffer.append(terminationString);

    return retBuffer;
}

S32 GuiTextCtrl::textBufferWidth(StringBuffer buffer)
{
    S32 charLength = 0;

    for(S32 count=0; count< (S32)buffer.length(); count++)
    {
        UTF16 c = buffer.getChar(count);
        if(!mFont->isValidChar(c))
            continue;

        charLength += mFont->getCharXIncrement( c );  
    }

    return charLength;
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// EOF //
