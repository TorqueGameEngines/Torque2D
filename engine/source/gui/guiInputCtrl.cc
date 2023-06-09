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

#include "gui/guiInputCtrl.h"
#include "input/actionMap.h"

IMPLEMENT_CONOBJECT(GuiInputCtrl);

GuiInputCtrl::GuiInputCtrl()
{
	mIsContainer = false;
}

//------------------------------------------------------------------------------
bool GuiInputCtrl::onWake()
{
   // Set the default profile on start-up:
   if ( !mProfile )
   {
      SimObject *obj = Sim::findObject("GuiInputCtrlProfile");
      if ( obj )
         mProfile = dynamic_cast<GuiControlProfile*>( obj );
   }

   if ( !Parent::onWake() )
      return( false );

   mouseLock();
   setFirstResponder();

   return( true );
}


//------------------------------------------------------------------------------
void GuiInputCtrl::onSleep()
{
   Parent::onSleep();
   mouseUnlock();
   clearFirstResponder();
}


//------------------------------------------------------------------------------
static bool isModifierKey( U16 keyCode )
{
   switch ( keyCode )
   {
      case KEY_LCONTROL:
      case KEY_RCONTROL:
      case KEY_LALT:
      case KEY_RALT:
      case KEY_LSHIFT:
      case KEY_RSHIFT:
         return( true );
   }

   return( false );
}

//------------------------------------------------------------------------------
bool GuiInputCtrl::onInputEvent( const InputEvent &event )
{
   if ( event.action == SI_MAKE )
   {
      if ( event.objType == SI_BUTTON
        || event.objType == SI_POV
        || ( ( event.objType == SI_KEY ) && !isModifierKey( event.objInst ) ) )
      {
         char deviceString[32];
         if ( !ActionMap::getDeviceName( event.deviceType, event.deviceInst, deviceString ) )
            return( false );

         const char* actionString = ActionMap::buildActionString( &event );

         Con::executef( this, 4, "onInputEvent", deviceString, actionString, "1" );
         return( true );
      }
   }
   else if ( event.action == SI_BREAK )
   {
      if ( ( event.objType == SI_KEY ) && isModifierKey( event.objInst ) )
      {
         char keyString[32];
         if ( !ActionMap::getKeyString( event.objInst, keyString ) )
            return( false );

         Con::executef( this, 4, "onInputEvent", "keyboard", keyString, "0" );
         return( true );
      }
   }

   return( false );
}
