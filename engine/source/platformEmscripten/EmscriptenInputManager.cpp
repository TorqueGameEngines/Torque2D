//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
// Portions Copyright (c) 2014 James S Urquhart
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



#include "platformEmscripten/platformEmscripten.h"
#include "console/consoleTypes.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platformEmscripten/EmscriptenInputManager.h"
#include "math/mMathFn.h"

#include <SDL/SDL.h>

// ascii table
AsciiData AsciiTable[NUM_KEYS];

// keymap table
static const U32 SDLtoTKeyMapSize = SDLK_LAST;
static U8 SDLtoTKeyMap[SDLtoTKeyMapSize];
static bool keyMapsInitialized = false;

// helper functions
static void MapKey(Uint16 SDLkey, U8 tkey);
static void InitKeyMaps();
static inline U8 TranslateSDLKeytoTKey(SDLKey keysym);

// constants

static const U32 MouseMask = SDL_MOUSEMOTION | SDL_MOUSEBUTTONUP;//SDL_MOUSEEVENTMASK;
static const U32 KeyboardMask = SDL_KEYUP;//SDL_KEYUPMASK | SDL_KEYDOWNMASK;
static const U32 JoystickMask = SDL_JOYAXISMOTION | SDL_JOYBUTTONUP;//SDL_JOYEVENTMASK;

static const U32 AllInputEvents = MouseMask | KeyboardMask | JoystickMask;

//==============================================================================
// Static helper functions
//==============================================================================
static void MapKey(Uint16 SDLkey, U8 tkey)
{
   SDLtoTKeyMap[SDLkey] = tkey;

   // Only printable-ASCII keysyms carry a character ascii. SDL's special keys
   // (modifiers, function keys, arrows, keypad, locks, navigation -- all keysyms
   // >= 0x100) and the 0x7F-0xFF range must map to ascii 0; otherwise they're
   // treated as character input and insert phantom glyphs into text fields (e.g.
   // pressing Ctrl typed a stray "box" character). The desktop x86UNIX back-end
   // gets this for free by reading the X keymap (which yields nothing for a
   // non-character key), but a browser canvas has no keymap to read, hence this
   // explicit filter. Control keys (< 0x20: Tab/Enter/Backspace/Esc) also map to 0
   // here and are handled by keycode, not by character input. Shifted variants of
   // the printable keys are still set by the switch below.
   const U16 charAscii = (SDLkey >= 0x20 && SDLkey <= 0x7E) ? SDLkey : 0;
   AsciiTable[tkey].lower.ascii = charAscii;
   AsciiTable[tkey].upper.ascii = charAscii;
   AsciiTable[tkey].goofy.ascii = charAscii;

   switch (tkey)
   {
      case KEY_0:
         AsciiTable[tkey].upper.ascii = (U16)')';
         break;
      case KEY_1:
         AsciiTable[tkey].upper.ascii = (U16)'!';
         break;
      case KEY_2:
         AsciiTable[tkey].upper.ascii = (U16)'@';
         break;
      case KEY_3:
         AsciiTable[tkey].upper.ascii = 163;
         break;
      case KEY_4:
         AsciiTable[tkey].upper.ascii = (U16)'$';
         break;
      case KEY_5:
         AsciiTable[tkey].upper.ascii = (U16)'%';
         break;
      case KEY_6:
         AsciiTable[tkey].upper.ascii = (U16)'^';
         break;
      case KEY_7:
         AsciiTable[tkey].upper.ascii = (U16)'&';
         break;
      case KEY_8:
         AsciiTable[tkey].upper.ascii = (U16)'*';
         break;
      case KEY_9:
         AsciiTable[tkey].upper.ascii = (U16)'(';
         break;
      case KEY_MINUS:
         AsciiTable[tkey].upper.ascii = (U16)'_';
         break;
      case KEY_EQUALS:
         AsciiTable[tkey].upper.ascii = (U16)'+';
         break;
      case KEY_TILDE:
         AsciiTable[tkey].upper.ascii = (U16)126;
         break;
      case KEY_SEMICOLON:
         AsciiTable[tkey].upper.ascii = ':';
         break;
      case KEY_APOSTROPHE:
         AsciiTable[tkey].upper.ascii = '"';
         break;
      case KEY_BACKSLASH:
         AsciiTable[tkey].upper.ascii = '|';
         break;
      case KEY_LBRACKET:
         AsciiTable[tkey].upper.ascii = '{';
         break;
      case KEY_RBRACKET:
         AsciiTable[tkey].upper.ascii = '}';
         break;
      case KEY_COMMA:
         AsciiTable[tkey].upper.ascii = '<';
         break;
      case KEY_PERIOD:
         AsciiTable[tkey].upper.ascii = '>';
         break;
   }
}

//------------------------------------------------------------------------------
void InitKeyMaps()
{
   Con::printf("InitKeyMaps size == %i, ASCIITABLE SIZE == %i", SDLtoTKeyMapSize, NUM_KEYS);

   dMemset( &AsciiTable, 0, sizeof( AsciiTable ) );
   dMemset(SDLtoTKeyMap, KEY_NULL, sizeof( SDLtoTKeyMap ));
   
   // set up the X to Torque key map
   // stuff
   MapKey(SDLK_BACKSPACE, KEY_BACKSPACE);
   MapKey(SDLK_TAB, KEY_TAB);
   MapKey(SDLK_RETURN, KEY_RETURN);
   MapKey(SDLK_PAUSE, KEY_PAUSE);
   MapKey(SDLK_CAPSLOCK, KEY_CAPSLOCK);
   MapKey(SDLK_ESCAPE, KEY_ESCAPE);

   // more stuff
   MapKey(SDLK_SPACE, KEY_SPACE);
   MapKey(SDLK_PAGEDOWN, KEY_PAGE_DOWN);
   MapKey(SDLK_PAGEUP, KEY_PAGE_UP);
   MapKey(SDLK_END, KEY_END);
   MapKey(SDLK_HOME, KEY_HOME);
   MapKey(SDLK_LEFT, KEY_LEFT);
   MapKey(SDLK_UP, KEY_UP);
   MapKey(SDLK_RIGHT, KEY_RIGHT);
   MapKey(SDLK_DOWN, KEY_DOWN);
   MapKey(SDLK_PRINT, KEY_PRINT);
   MapKey(SDLK_INSERT, KEY_INSERT);
   MapKey(SDLK_DELETE, KEY_DELETE);
   
   S32 keysym;
   S32 tkeycode;
   // main numeric keys
   for (keysym = SDLK_0, tkeycode = KEY_0;
        keysym <= SDLK_9; 
        ++keysym, ++tkeycode)
      MapKey(static_cast<SDLKey>(keysym), tkeycode);
   
   // lowercase letters
   for (keysym = SDLK_a, tkeycode = KEY_A; 
        keysym <= SDLK_z; 
        ++keysym, ++tkeycode)
      MapKey(static_cast<SDLKey>(keysym), tkeycode);

   // various punctuation
   MapKey('|', KEY_TILDE);
   MapKey(SDLK_BACKQUOTE, KEY_TILDE);
   MapKey(SDLK_MINUS, KEY_MINUS);
   MapKey(SDLK_EQUALS, KEY_EQUALS);
   MapKey(SDLK_LEFTBRACKET, KEY_LBRACKET);
   MapKey('{', KEY_LBRACKET);
   MapKey(SDLK_RIGHTBRACKET, KEY_RBRACKET);
   MapKey('}', KEY_RBRACKET);
   MapKey(SDLK_BACKSLASH, KEY_BACKSLASH);
   MapKey(SDLK_SEMICOLON, KEY_SEMICOLON);
   MapKey(SDLK_QUOTE, KEY_APOSTROPHE);
   MapKey(SDLK_COMMA, KEY_COMMA);
   MapKey(SDLK_PERIOD, KEY_PERIOD);
   MapKey(SDLK_SLASH, KEY_SLASH); 

   // numpad numbers
   for (keysym = SDLK_KP0, tkeycode = KEY_NUMPAD0; 
        keysym <= SDLK_KP9; 
        ++keysym, ++tkeycode)
      MapKey(static_cast<SDLKey>(keysym), tkeycode);

   // other numpad stuff
   MapKey(SDLK_KP_MULTIPLY, KEY_MULTIPLY);
   MapKey(SDLK_KP_PLUS, KEY_ADD);
   MapKey(SDLK_KP_EQUALS, KEY_SEPARATOR);
   MapKey(SDLK_KP_MINUS, KEY_SUBTRACT);
   MapKey(SDLK_KP_PERIOD, KEY_DECIMAL);
   MapKey(SDLK_KP_DIVIDE, KEY_DIVIDE);
   MapKey(SDLK_KP_ENTER, KEY_NUMPADENTER);

   // F keys
   for (keysym = SDLK_F1, tkeycode = KEY_F1; 
        keysym <= SDLK_F15; 
        ++keysym, ++tkeycode)
      MapKey(static_cast<SDLKey>(keysym), tkeycode);

   // various modifiers
   MapKey(SDLK_NUMLOCK, KEY_NUMLOCK);
   MapKey(SDLK_SCROLLOCK, KEY_SCROLLLOCK);
   MapKey(SDLK_LCTRL, KEY_LCONTROL);
   MapKey(SDLK_RCTRL, KEY_RCONTROL);
   MapKey(SDLK_LALT, KEY_LALT);
   MapKey(SDLK_RALT, KEY_RALT);
   MapKey(313, KEY_RALT);   
   MapKey(SDLK_LSHIFT, KEY_LSHIFT);
   MapKey(SDLK_RSHIFT, KEY_RSHIFT);
   MapKey(SDLK_LSUPER, KEY_WIN_LWINDOW);
   MapKey(SDLK_RSUPER, KEY_WIN_RWINDOW);
   MapKey(SDLK_MENU, KEY_WIN_APPS);
   MapKey(SDLK_MODE, KEY_OEM_102);

   keyMapsInitialized = true;
};

//------------------------------------------------------------------------------
U8 TranslateSDLKeytoTKey(SDLKey keysym)
{
   if (!keyMapsInitialized)
   {
      Con::printf("WARNING: SDLkeysymMap is not initialized");
      return 0;
   }
   if (keysym < 0 || 
       static_cast<U32>(keysym) >= SDLtoTKeyMapSize)
   {
      Con::printf("WARNING: invalid keysym: %d", keysym);
      return 0;
   }
   return SDLtoTKeyMap[keysym];
}

//==============================================================================
// UInputManager
//==============================================================================
UInputManager::UInputManager()
{
   mActive = false;
   mEnabled = false;
   mLocking = true; // locking enabled by default
   mKeyboardEnabled = mMouseEnabled = mJoystickEnabled = false;
   mKeyboardActive = mMouseActive = mJoystickActive = false;
}

//------------------------------------------------------------------------------
void UInputManager::init()
{
   Con::addVariable( "pref::Input::KeyboardEnabled",  
      TypeBool, &mKeyboardEnabled );
   Con::addVariable( "pref::Input::MouseEnabled",     
      TypeBool, &mMouseEnabled );
   Con::addVariable( "pref::Input::JoystickEnabled",  
      TypeBool, &mJoystickEnabled );
}

//------------------------------------------------------------------------------
bool UInputManager::enable()
{
   disable();
#ifdef LOG_INPUT
   Input::log( "Enabling Input...\n" );
#endif

   mModifierKeys = 0;
   dMemset( mMouseButtonState, 0, sizeof( mMouseButtonState ) );
   dMemset( mKeyboardState, 0, 256 );

   InitKeyMaps();

   mJoystickEnabled = false;
   initJoystick();

   mEnabled = true;
   mMouseEnabled = true;
   mKeyboardEnabled = true;

   SDL_EnableKeyRepeat(
      SDL_DEFAULT_REPEAT_DELAY, 
      SDL_DEFAULT_REPEAT_INTERVAL);

   return true;     
}

//------------------------------------------------------------------------------
void UInputManager::disable()
{
   deactivate();
   mEnabled = false;
   return;
}

//------------------------------------------------------------------------------
void UInputManager::initJoystick()
{
   mJoystickList.clear();

   // initialize SDL joystick system
   if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
   {
      Con::warnf("   Unable to initialize joystick: %s", SDL_GetError());
      return;
   }

   int numJoysticks = SDL_NumJoysticks();
   if (numJoysticks == 0)
      Con::printf("   No joysticks found.");

   // disable joystick events (use polling instead)
   SDL_JoystickEventState(SDL_IGNORE);

   // install joysticks
   for(int i = 0; i < numJoysticks; i++ ) 
   {
      JoystickInputDevice* newDevice = new JoystickInputDevice(i);
      addObject(newDevice);
      mJoystickList.push_back(newDevice);
      Con::printf("   %s: %s", 
         newDevice->getDeviceName(), newDevice->getName());
#ifdef LOG_INPUT
      Input::log("   %s: %s\n", 
         newDevice->getDeviceName(), newDevice->getName());
#endif
   }

   mJoystickEnabled = true;
}

//------------------------------------------------------------------------------
void UInputManager::activate()
{
   if (mEnabled && !isActive())
   {
      mActive = true;
      SDL_ShowCursor(SDL_DISABLE);
      resetInputState();
      // hack; if the mouse or keyboard has been disabled, re-enable them.
      // prevents scripts like default.cs from breaking our input, although
      // there is probably a better solution
      mMouseEnabled = mKeyboardEnabled = true;
      activateMouse();
      activateKeyboard();
      activateJoystick();
      if (gPlatState.mouseLocked)
         lockInput();
   }
}

//------------------------------------------------------------------------------
void UInputManager::deactivate()
{
   if (mEnabled && isActive())
   {
      unlockInput();
      deactivateKeyboard();
      deactivateMouse();
      deactivateJoystick();
      resetInputState();
      SDL_ShowCursor(SDL_ENABLE);
      mActive = false;
   }
}

//------------------------------------------------------------------------------
void UInputManager::resetKeyboardState()
{
   // unpress any pressed keys; in the future we may want
   // to actually sync with the keyboard state
   for (int i = 0; i < 256; ++i)
   {
      if (mKeyboardState[i])
      {
         InputEvent event;
         
         event.deviceInst = 0;
         event.deviceType = KeyboardDeviceType;
         event.objType = SI_KEY;
         event.objInst = i;
         event.action = SI_BREAK;
         event.fValues[0] = 0.0;
         Game->postEvent(event);
      }
   }
   dMemset(mKeyboardState, 0, 256);

   // clear modifier keys
   mModifierKeys = 0;
}

//------------------------------------------------------------------------------
void UInputManager::resetMouseState()
{
   // unpress any buttons; in the future we may want
   // to actually sync with the mouse state
   for (int i = 0; i < 3; ++i)
   {
      if (mMouseButtonState[i])
      {
         // add KEY_BUTTON0 to the index to get the real
         // button ID
         S32 buttonID = i + KEY_BUTTON0;
         InputEvent event;
        
         event.deviceInst = 0;
         event.deviceType = MouseDeviceType;
         event.objType = SI_BUTTON;
         event.objInst = buttonID;
         event.action = SI_BREAK;
         event.fValues[0] = 0.0;
         Game->postEvent(event);
      }
   }

   dMemset(mMouseButtonState, 0, 3);
}

//------------------------------------------------------------------------------
void UInputManager::resetInputState()
{
   resetKeyboardState();
   resetMouseState();

   // reset joysticks
   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin(); 
        iter != mJoystickList.end();
        ++iter)
   {
      (*iter)->reset();
   }
   gPlatState.eventList.clear();

#ifndef DUMMY_PLATFORM
   // Need to do this since there seems to be no flush method!
   SDL_Event e;
   while (SDL_PollEvent(&e))
   {
      // ....
   }
#endif
}

//------------------------------------------------------------------------------
void UInputManager::setLocking(bool enabled)
{
   mLocking = enabled;
   if (mLocking)
      lockInput();
   else
      unlockInput();
}

//------------------------------------------------------------------------------
void UInputManager::lockInput()
{
   if (!gPlatState.backgrounded && !gPlatState.mouseLocked && 
      mLocking &&
      SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF)
      SDL_WM_GrabInput(SDL_GRAB_ON);
}

//------------------------------------------------------------------------------
void UInputManager::unlockInput()
{
   if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON)
      SDL_WM_GrabInput(SDL_GRAB_OFF);
}

//------------------------------------------------------------------------------
void UInputManager::onDeleteNotify( SimObject* object )
{
   Parent::onDeleteNotify( object );
}

//------------------------------------------------------------------------------
bool UInputManager::onAdd()
{
   if ( !Parent::onAdd() )
      return false;

   return true;
}

//------------------------------------------------------------------------------
void UInputManager::onRemove()
{
   deactivate();
   Parent::onRemove();
}

//------------------------------------------------------------------------------
void UInputManager::mouseMotionEvent(const SDL_Event& event)
{
//    Con::printf("motion event: %d %d %d %d",
//       event.motion.xrel, event.motion.yrel,
//       event.motion.x, event.motion.y);
   if (gPlatState.mouseLocked)
   {
      InputEvent ievent;
      ievent.deviceInst = 0;
      ievent.deviceType = MouseDeviceType;
      ievent.objInst = 0;
      ievent.modifier = mModifierKeys;
      ievent.ascii = 0;
      ievent.action = SI_MOVE;
            
      // post events if things have changed
      if (event.motion.xrel != 0)
      {
         ievent.objType = SI_XAXIS;
         ievent.fValues[0] = event.motion.xrel;
         Game->postEvent(ievent);
      }
      if (event.motion.yrel != 0)
      {
         ievent.objType = SI_YAXIS;
         ievent.fValues[0] = event.motion.yrel; 
         Game->postEvent(ievent);
      }
#ifdef LOG_INPUT
#ifdef LOG_MOUSEMOVE
         Input::log( "EVENT (Input): Mouse relative move (%.1f, %.1f).\n",
            event.motion.xrel != 0 ? F32(event.motion.xrel) : 0.0,
            event.motion.yrel != 0 ? F32(event.motion.yrel) : 0.0);
#endif
#endif
   }
   else
   {
      MouseMoveEvent mmevent;
      mmevent.xPos = mLastMouseX = event.motion.x;
      mmevent.yPos = mLastMouseY = event.motion.y;
      mmevent.modifier = mModifierKeys;
      Game->postEvent(mmevent);
#ifdef LOG_INPUT
#ifdef LOG_MOUSEMOVE
         Input::log( "EVENT (Input): Mouse absolute move (%.1f, %.1f).\n",
            F32(event.motion.x),
            F32(event.motion.y));
#endif
#endif
   }
}

//------------------------------------------------------------------------------
void UInputManager::joyButtonEvent(const SDL_Event& event)
{
   joyButtonEvent(event.jbutton.which, event.jbutton.button, 
      event.type == SDL_JOYBUTTONDOWN);
}

//------------------------------------------------------------------------------
void UInputManager::joyButtonEvent(U8 deviceID, U8 buttonNum, bool pressed)

{
   S32 action = pressed ? SI_MAKE : SI_BREAK;
   S32 objInst = buttonNum + KEY_BUTTON0;

   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = SI_BUTTON;
   ievent.objInst = objInst;
   ievent.action = action;
   ievent.fValues[0] = (action == SI_MAKE) ? 1.0 : 0.0;

   Game->postEvent(ievent);
#ifdef LOG_INPUT
   Input::log( "EVENT (Input): joystick%d button%d %s. MODS:%c%c%c \n",
      deviceID,
      buttonNum,
      pressed ? "pressed" : "released",
      ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
      ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
      ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
}

//------------------------------------------------------------------------------
void UInputManager::joyHatEvent(U8 deviceID, U8 hatNum, 
   U8 prevHatState, U8 currHatState)
{
   if (prevHatState == currHatState)
      return;

   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = SI_POV;

   // first break any positions that are no longer valid
   ievent.action = SI_BREAK;
   ievent.fValues[0] = 0.0;

   if (prevHatState & SDL_HAT_UP && !(currHatState & SDL_HAT_UP))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Up POV released.\n");
#endif
      ievent.objInst = SI_UPOV;
      Game->postEvent(ievent);
   }
   else if (prevHatState & SDL_HAT_DOWN && !(currHatState & SDL_HAT_DOWN))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Down POV released.\n");
#endif
      ievent.objInst = SI_DPOV;
      Game->postEvent(ievent);
   }
   if (prevHatState & SDL_HAT_LEFT && !(currHatState & SDL_HAT_LEFT))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Left POV released.\n");
#endif
      ievent.objInst = SI_LPOV;
      Game->postEvent(ievent);
   }
   else if (prevHatState & SDL_HAT_RIGHT && !(currHatState & SDL_HAT_RIGHT))
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Right POV released.\n");
#endif
      ievent.objInst = SI_RPOV;
      Game->postEvent(ievent);
   }

   // now do the make events
   ievent.action = SI_MAKE;
   ievent.fValues[0] = 1.0;

   if (!(prevHatState & SDL_HAT_UP) && currHatState & SDL_HAT_UP)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Up POV pressed.\n");
#endif
      ievent.objInst = SI_UPOV;
      Game->postEvent(ievent);
   }
   else if (!(prevHatState & SDL_HAT_DOWN) && currHatState & SDL_HAT_DOWN)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Down POV pressed.\n");
#endif
      ievent.objInst = SI_DPOV;
      Game->postEvent(ievent);
   }
   if (!(prevHatState & SDL_HAT_LEFT) && currHatState & SDL_HAT_LEFT)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Left POV pressed.\n");
#endif
      ievent.objInst = SI_LPOV;
      Game->postEvent(ievent);
   }
   else if (!(prevHatState & SDL_HAT_RIGHT) && currHatState & SDL_HAT_RIGHT)
   {
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): Right POV pressed.\n");
#endif
      ievent.objInst = SI_RPOV;
      Game->postEvent(ievent);
   }
}

//------------------------------------------------------------------------------
void UInputManager::joyAxisEvent(const SDL_Event& event)
{
   joyAxisEvent(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
}

//------------------------------------------------------------------------------
void UInputManager::joyAxisEvent(U8 deviceID, U8 axisNum, S16 axisValue)
{
   JoystickInputDevice* stick;

   stick = mJoystickList[deviceID];
   AssertFatal(stick, "JoystickInputDevice* is NULL");
   JoystickAxisInfo axisInfo = stick->getAxisInfo(axisNum);

   if (axisInfo.type == -1)
      return;

   // scale the value to [-1,1]
   F32 scaledValue = 0;  
   if (axisValue < 0)
      scaledValue = -F32(axisValue) / axisInfo.minValue;
   else if (axisValue > 0)
      scaledValue = F32(axisValue) / axisInfo.maxValue;

//    F32 range = F32(axisInfo.maxValue - axisInfo.minValue);
//    F32 scaledValue = F32((2 * axisValue) - axisInfo.maxValue -
//       axisInfo.minValue) / range;

   if (scaledValue > 1.f)
      scaledValue = 1.f;
   else if (scaledValue < -1.f)
      scaledValue = -1.f;

   // create and post the event
   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;
   ievent.objType = axisInfo.type;
   ievent.objInst = 0;
   ievent.action = SI_MOVE;
   ievent.fValues[0] = scaledValue;

   Game->postEvent(ievent);

#ifdef LOG_INPUT
      Input::log( "EVENT (Input): joystick axis %d moved: %.1f.\n",
         axisNum, ievent.fValues[0]);
#endif

}

//------------------------------------------------------------------------------
void UInputManager::mouseButtonEvent(const SDL_Event& event)
{
   S32 action = (event.type == SDL_MOUSEBUTTONDOWN) ? SI_MAKE : SI_BREAK;
   S32 objInst = -1;
   // JMQTODO: support wheel delta like windows version?
   // JMQTODO: make this value configurable?
   S32 wheelDelta = 10;
   bool wheel = false;

   switch (event.button.button)
   {
      case SDL_BUTTON_LEFT:
         objInst = KEY_BUTTON0;
         break;
      case SDL_BUTTON_RIGHT:
         objInst = KEY_BUTTON1;
         break;
      case SDL_BUTTON_MIDDLE:
         objInst = KEY_BUTTON2;
         break;
/*      case Button4:
         wheel = true;
         break;
      case Button5:
         wheel = true;
         wheelDelta = -wheelDelta;
         break;*/
   }

   if (objInst == -1 && !wheel)
      // unsupported button
      return;

   InputEvent ievent;

   ievent.deviceInst = 0;
   ievent.deviceType = MouseDeviceType;
   ievent.modifier = mModifierKeys;
   ievent.ascii = 0;

   if (wheel)
   {
      // SDL generates a button press/release for each wheel move,
      // so ignore breaks to translate those into a single event
      if (action == SI_BREAK)
         return;
      ievent.objType = SI_ZAXIS;
      ievent.objInst = 0;
      ievent.action = SI_MOVE;
      ievent.fValues[0] = wheelDelta;
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): mouse wheel moved %s: %.1f. MODS:%c%c%c\n",
         wheelDelta > 0 ? "up" : "down",
         ievent.fValues[0],
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
   }
   else // regular button
   {
      S32 buttonID = (objInst - KEY_BUTTON0);
      if (buttonID < 3)
         mMouseButtonState[buttonID] = ( action == SI_MAKE ) ? true : false;

      ievent.objType = SI_BUTTON;
      ievent.objInst = objInst;
      ievent.action = action;
      ievent.fValues[0] = (action == SI_MAKE) ? 1.0 : 0.0;
#ifdef LOG_INPUT
      Input::log( "EVENT (Input): mouse button%d %s. MODS:%c%c%c\n",
         buttonID,
         action == SI_MAKE ? "pressed" : "released",
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
   }

   Game->postEvent(ievent);
}

//------------------------------------------------------------------------------
const char* getKeyName( U16 key )
{
   switch ( key )
   {
      case KEY_BACKSPACE:     return "Backspace";
      case KEY_TAB:           return "Tab";
      case KEY_RETURN:        return "Return";
      case KEY_PAUSE:         return "Pause";
      case KEY_CAPSLOCK:      return "CapsLock";
      case KEY_ESCAPE:        return "Esc";

      case KEY_SPACE:         return "SpaceBar";
      case KEY_PAGE_DOWN:     return "PageDown";
      case KEY_PAGE_UP:       return "PageUp";
      case KEY_END:           return "End";
      case KEY_HOME:          return "Home";
      case KEY_LEFT:          return "Left";
      case KEY_UP:            return "Up";
      case KEY_RIGHT:         return "Right";
      case KEY_DOWN:          return "Down";
      case KEY_PRINT:         return "PrintScreen";
      case KEY_INSERT:        return "Insert";
      case KEY_DELETE:        return "Delete";
      case KEY_HELP:          return "Help";

      case KEY_NUMPAD0:       return "Numpad 0";
      case KEY_NUMPAD1:       return "Numpad 1";
      case KEY_NUMPAD2:       return "Numpad 2";
      case KEY_NUMPAD3:       return "Numpad 3";
      case KEY_NUMPAD4:       return "Numpad 4";
      case KEY_NUMPAD5:       return "Numpad 5";
      case KEY_NUMPAD6:       return "Numpad 6";
      case KEY_NUMPAD7:       return "Numpad 7";
      case KEY_NUMPAD8:       return "Numpad 8";
      case KEY_NUMPAD9:       return "Numpad 9";
      case KEY_MULTIPLY:      return "Multiply";
      case KEY_ADD:           return "Add";
      case KEY_SEPARATOR:     return "Separator";
      case KEY_SUBTRACT:      return "Subtract";
      case KEY_DECIMAL:       return "Decimal";
      case KEY_DIVIDE:        return "Divide";
      case KEY_NUMPADENTER:   return "Numpad Enter";

      case KEY_F1:            return "F1";
      case KEY_F2:            return "F2";
      case KEY_F3:            return "F3";
      case KEY_F4:            return "F4";
      case KEY_F5:            return "F5";
      case KEY_F6:            return "F6";
      case KEY_F7:            return "F7";
      case KEY_F8:            return "F8";
      case KEY_F9:            return "F9";
      case KEY_F10:           return "F10";
      case KEY_F11:           return "F11";
      case KEY_F12:           return "F12";
      case KEY_F13:           return "F13";
      case KEY_F14:           return "F14";
      case KEY_F15:           return "F15";
      case KEY_F16:           return "F16";
      case KEY_F17:           return "F17";
      case KEY_F18:           return "F18";
      case KEY_F19:           return "F19";
      case KEY_F20:           return "F20";
      case KEY_F21:           return "F21";
      case KEY_F22:           return "F22";
      case KEY_F23:           return "F23";
      case KEY_F24:           return "F24";

      case KEY_NUMLOCK:       return "NumLock";
      case KEY_SCROLLLOCK:    return "ScrollLock";
      case KEY_LCONTROL:      return "LCtrl";
      case KEY_RCONTROL:      return "RCtrl";
      case KEY_LALT:          return "LAlt";
      case KEY_RALT:          return "RAlt";
      case KEY_LSHIFT:        return "LShift";
      case KEY_RSHIFT:        return "RShift";

      case KEY_WIN_LWINDOW:   return "LWin";
      case KEY_WIN_RWINDOW:   return "RWin";
      case KEY_WIN_APPS:      return "Apps";
   }

   static char returnString[5];
   dSprintf( returnString, sizeof( returnString ), "%c", Input::getAscii( key, STATE_UPPER ) );
   return returnString;
}


//------------------------------------------------------------------------------
void UInputManager::keyEvent(const SDL_Event& event)
{
   S32 action = (event.type == SDL_KEYDOWN) ? SI_MAKE : SI_BREAK;
   InputEvent ievent;

//   Con::printf("keyEvent: %u", event.key.keysym.sym);

   ievent.deviceInst = 0;
   ievent.deviceType = KeyboardDeviceType;
   ievent.objType = SI_KEY;
   ievent.objInst = TranslateSDLKeytoTKey(event.key.keysym.sym);
//   Con::printf("keyEvent: objInst %u", ievent.objInst);
   // if the action is a make but this key is already pressed, 
   // count it as a repeat
   if (action == SI_MAKE && mKeyboardState[ievent.objInst])
      action = SI_REPEAT;
   ievent.action = action;
   ievent.fValues[0] = (action == SI_MAKE || action == SI_REPEAT) ? 1.0 : 0.0;

   processKeyEvent(ievent);
   Game->postEvent(ievent);

#if 0
   if (ievent.action == SI_MAKE)
      dPrintf("key event: : %s key pressed. MODS:%c%c%c\n",
         getKeyName(ievent.objInst),
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
   else if (ievent.action == SI_REPEAT)
      dPrintf("key event: : %s key repeated. MODS:%c%c%c\n",
         getKeyName(ievent.objInst),
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
   else if (ievent.action == SI_BREAK)
      dPrintf("key event: : %s key released. MODS:%c%c%c\n",
         getKeyName(ievent.objInst),
         ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
         ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
         ( mModifierKeys & SI_ALT ? 'A' : '.' ));
   else
      dPrintf("unknown key event!\n");
#endif

#ifdef LOG_INPUT
   Input::log( "EVENT (Input): %s key %s. MODS:%c%c%c\n",
      getKeyName(ievent.objInst),
      action == SI_MAKE ? "pressed" : "released",
      ( mModifierKeys & SI_SHIFT ? 'S' : '.' ), 
      ( mModifierKeys & SI_CTRL ? 'C' : '.' ), 
      ( mModifierKeys & SI_ALT ? 'A' : '.' ));
#endif
}

//------------------------------------------------------------------------------
// This function was ripped from DInputDevice almost entirely intact.  
bool UInputManager::processKeyEvent( InputEvent &event )
{
   if ( event.deviceType != KeyboardDeviceType || event.objType != SI_KEY )
      return false;

   bool modKey = false;
   U8 keyCode = event.objInst;

   if ( event.action == SI_MAKE || event.action == SI_REPEAT)
   {
      // Maintain the key structure:
      mKeyboardState[keyCode] = true;

      switch ( event.objInst )
      {
         case KEY_LSHIFT:
//   Con::printf("keyEvent: LSHIFT");
            mModifierKeys |= SI_LSHIFT;
            modKey = true;
            break;

         case KEY_RSHIFT:
//   Con::printf("keyEvent: RSHIFT");
            mModifierKeys |= SI_RSHIFT;
            modKey = true;
            break;

         case KEY_LCONTROL:
            mModifierKeys |= SI_LCTRL;
            modKey = true;
            break;

         case KEY_RCONTROL:
            mModifierKeys |= SI_RCTRL;
            modKey = true;
            break;

         case KEY_LALT:
            mModifierKeys |= SI_LALT;
            modKey = true;
            break;

         case KEY_RALT:
            mModifierKeys |= SI_RALT;
            modKey = true;
            break;
      }
   }
   else
   {
      // Maintain the keys structure:
      mKeyboardState[keyCode] = false;

      switch ( event.objInst )
      {
         case KEY_LSHIFT:
//   Con::printf("keyEvent UNSET: LSHIFT");
            mModifierKeys &= ~SI_LSHIFT;
            modKey = true;
            break;

         case KEY_RSHIFT:
//   Con::printf("keyEvent UNSET: RSHIFT");
            mModifierKeys &= ~SI_RSHIFT;
            modKey = true;
            break;

         case KEY_LCONTROL:
            mModifierKeys &= ~SI_LCTRL;
            modKey = true;
            break;

         case KEY_RCONTROL:
            mModifierKeys &= ~SI_RCTRL;
            modKey = true;
            break;

         case KEY_LALT:
            mModifierKeys &= ~SI_LALT;
            modKey = true;
            break;

         case KEY_RALT:
            mModifierKeys &= ~SI_RALT;
            modKey = true;
            break;
      }
   }

   if ( modKey )
      event.modifier = 0;
   else
      event.modifier = mModifierKeys;

   // TODO: alter this getAscii call
   KEY_STATE state = STATE_LOWER;
   if (event.modifier & (SI_CTRL|SI_ALT) )
   {
      state = STATE_GOOFY;
   }
   if ( event.modifier & SI_SHIFT )
   {
//   Con::printf("keyEvent: UPCASE GET KEY");
      state = STATE_UPPER;
   }

   event.ascii = Input::getAscii( event.objInst, state );

   return modKey;
}

//------------------------------------------------------------------------------
void UInputManager::setWindowLocked(bool locked)
{
   if (locked)
      lockInput();
   else
   {
      unlockInput();
      // SDL keeps track of abs mouse position in fullscreen mode, which means
      // that if you switch to unlocked mode while fullscreen, the mouse will
      // suddenly warp to someplace unexpected on screen.  To fix this, we 
      // warp the mouse to the last known Torque abs mouse position.
      if (mLastMouseX != -1 && mLastMouseY != -1)
         SDL_WarpMouse(mLastMouseX, mLastMouseY);
   }
}

//------------------------------------------------------------------------------
void UInputManager::process()
{
   if (!mEnabled || !isActive())
      return;

#ifndef DUMMY_PLATFORM
   if (mMouseActive || mKeyboardActive)
   {
      S32 numEvents = gPlatState.eventList.size();
      for (int i = 0; i < numEvents; ++i)
      {
         switch (gPlatState.eventList[i].type) 
         {
            case SDL_MOUSEMOTION:
               if (mMouseActive)
                  mouseMotionEvent(gPlatState.eventList[i]);
               break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
               if (mMouseActive)
                  mouseButtonEvent(gPlatState.eventList[i]);
               break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
               if (mKeyboardActive)
                  keyEvent(gPlatState.eventList[i]);
               break;
         }
      }
   }
#endif

   // poll joysticks
   if (!mJoystickActive)
      return;

   SDL_JoystickUpdate();

   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin(); 
        iter != mJoystickList.end();
        ++iter)
   {
      (*iter)->process();
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableKeyboard()
{
   if ( !isEnabled() )
      return( false );

   if ( isKeyboardEnabled() && isKeyboardActive() )
      return( true );

   mKeyboardEnabled = true;
   if ( isActive() )
      mKeyboardEnabled = activateKeyboard();

   if ( mKeyboardEnabled )
   {
      Con::printf( "Keyboard enabled." );
#ifdef LOG_INPUT
      Input::log( "Keyboard enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Keyboard failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Keyboard failed to enable!\n" );
#endif
   }
      
   return( mKeyboardEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableKeyboard()
{
   if ( !isEnabled() || !isKeyboardEnabled())
      return;

   deactivateKeyboard();
   mKeyboardEnabled = false;
   Con::printf( "Keyboard disabled." );
#ifdef LOG_INPUT
   Input::log( "Keyboard disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateKeyboard()
{
   if ( !isEnabled() || !isActive() || !isKeyboardEnabled() )
      return( false );

   mKeyboardActive = true;
#ifdef LOG_INPUT
   Input::log( mKeyboardActive ? "Keyboard activated.\n" : "Keyboard failed to activate!\n" );
#endif
   return( mKeyboardActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateKeyboard()
{
   if ( isEnabled() && isKeyboardActive() )
   {
      mKeyboardActive = false;
#ifdef LOG_INPUT
      Input::log( "Keyboard deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableMouse()
{
   if ( !isEnabled() )
      return( false );

   if ( isMouseEnabled() && isMouseActive() )
      return( true );

   mMouseEnabled = true;
   if ( isActive() )
      mMouseEnabled = activateMouse();

   if ( mMouseEnabled )
   {
      Con::printf( "Mouse enabled." );
#ifdef LOG_INPUT
      Input::log( "Mouse enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Mouse failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Mouse failed to enable!\n" );
#endif
   }

   return( mMouseEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableMouse()
{
   if ( !isEnabled() || !isMouseEnabled())
      return;

   deactivateMouse();
   mMouseEnabled = false;
   Con::printf( "Mouse disabled." );
#ifdef LOG_INPUT
   Input::log( "Mouse disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateMouse()
{
   if ( !isEnabled() || !isActive() || !isMouseEnabled() )
      return( false );

   Con::printf("Mouse Activated");

   mMouseActive = true;
#ifdef LOG_INPUT
   Input::log( mMouseActive ? 
      "Mouse activated.\n" : "Mouse failed to activate!\n" );
#endif
   return( mMouseActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateMouse()
{

   Con::printf("Mouse Deactivated");
   if ( isEnabled() && isMouseActive() )
   {
      mMouseActive = false;
#ifdef LOG_INPUT
      Input::log( "Mouse deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
bool UInputManager::enableJoystick()
{
   if ( !isEnabled() )
      return( false );

   if ( isJoystickEnabled() && isJoystickActive() )
      return( true );

   mJoystickEnabled = true;
   if ( isActive() )
      mJoystickEnabled = activateJoystick();

   if ( mJoystickEnabled )
   {
      Con::printf( "Joystick enabled." );
#ifdef LOG_INPUT
      Input::log( "Joystick enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Joystick failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Joystick failed to enable!\n" );
#endif
   }

   return( mJoystickEnabled );
}

//------------------------------------------------------------------------------
void UInputManager::disableJoystick()
{
   if ( !isEnabled() || !isJoystickEnabled())
      return;

   deactivateJoystick();
   mJoystickEnabled = false;
   Con::printf( "Joystick disabled." );
#ifdef LOG_INPUT
   Input::log( "Joystick disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool UInputManager::activateJoystick()
{
   if ( !isEnabled() || !isActive() || !isJoystickEnabled() )
      return( false );

   mJoystickActive = false;
   JoystickInputDevice* dptr;
   for ( iterator ptr = begin(); ptr != end(); ptr++ )
   {
      dptr = dynamic_cast<JoystickInputDevice*>( *ptr );
      if ( dptr && dptr->getDeviceType() == JoystickDeviceType)
         if ( dptr->activate() )
            mJoystickActive = true;
   }
#ifdef LOG_INPUT
   Input::log( mJoystickActive ? 
      "Joystick activated.\n" : "Joystick failed to activate!\n" );
#endif
   return( mJoystickActive );
}

//------------------------------------------------------------------------------
void UInputManager::deactivateJoystick()
{
   if ( isEnabled() && isJoystickActive() )
   {
      mJoystickActive = false;
      JoystickInputDevice* dptr;
      for ( iterator ptr = begin(); ptr != end(); ptr++ )
      {
         dptr = dynamic_cast<JoystickInputDevice*>( *ptr );
         if ( dptr && dptr->getDeviceType() == JoystickDeviceType)
            dptr->deactivate();
      }
#ifdef LOG_INPUT
      Input::log( "Joystick deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
const char* UInputManager::getJoystickAxesString( U32 deviceID )
{
   for (Vector<JoystickInputDevice*>::iterator iter = mJoystickList.begin();
        iter != mJoystickList.end();
        ++iter)
   {
      if ((*iter)->getDeviceID() == deviceID)
         return (*iter)->getJoystickAxesString();
   }
   return( "" );
}

//==============================================================================
// JoystickInputDevice
//==============================================================================
JoystickInputDevice::JoystickInputDevice(U8 deviceID)
{
   mActive = false;
   mStick = NULL;
   mAxisList.clear();
   mDeviceID = deviceID;
   dSprintf(mName, 29, "joystick%d", mDeviceID);

   mButtonState.clear();
   mHatState.clear();
   mNumAxes = mNumButtons = mNumHats = mNumBalls = 0;

   loadJoystickInfo();

   // initialize state variables
   for (int i = 0; i < mNumButtons; ++i)
      mButtonState.push_back(false); // all buttons unpressed initially

   for (int i = 0; i < mNumHats; ++i)
      mHatState.push_back(SDL_HAT_CENTERED); // hats centered initially
}

//------------------------------------------------------------------------------
JoystickInputDevice::~JoystickInputDevice()
{
   if (isActive())
      deactivate();
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::activate()
{
   if (isActive())
      return true;

   // open the stick
   mStick = SDL_JoystickOpen(mDeviceID);
   if (mStick == NULL)
   {
      Con::printf("Unable to activate %s: %s", getDeviceName(), SDL_GetError());
      return false;
   }

   // reload axis mapping info
   loadAxisInfo();

   mActive = true;
   return true;
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::deactivate()
{
   if (!isActive())
      return true;

   if (mStick != NULL)
   {
      SDL_JoystickClose(mStick);
      mStick = NULL;
   }

   mActive = false;
   return true;
}

//------------------------------------------------------------------------------
const char* JoystickInputDevice::getName()
{
   return SDL_JoystickName(mDeviceID);
}

//------------------------------------------------------------------------------
void JoystickInputDevice::reset()
{
   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (!manager)
      return;

   // clear joystick state variables

   // buttons
   for (int i = 0; i < mButtonState.size(); ++i)
      if (mButtonState[i])
      {
         manager->joyButtonEvent(mDeviceID, i, false);
         mButtonState[i] = false;
      }

   // hats
   for (int i = 0; i < mHatState.size(); ++i)
      if (mHatState[i] != SDL_HAT_CENTERED)
      {
         manager->joyHatEvent(mDeviceID, i, mHatState[i], SDL_HAT_CENTERED);
         mHatState[i] = SDL_HAT_CENTERED;
      }

   // axis and ball state is not maintained
}

//------------------------------------------------------------------------------
bool JoystickInputDevice::process()
{
   if (!isActive())
      return false;

   UInputManager* manager = dynamic_cast<UInputManager*>(Input::getManager());
   if (!manager)
      return false;

   // axes
   for (int i = 0; i < mNumAxes; ++i)
   {
      // skip the axis if we don't have a mapping for it
      if (mAxisList[i].type == -1)
         continue;
      manager->joyAxisEvent(mDeviceID, i, SDL_JoystickGetAxis(mStick, i));
   }

   // buttons
   for (int i = 0; i < mNumButtons; ++i)
   {
      if (bool(SDL_JoystickGetButton(mStick, i)) == 
         mButtonState[i])
         continue;
      mButtonState[i] = !mButtonState[i];
      manager->joyButtonEvent(mDeviceID, i, mButtonState[i]);
   }

   // hats
   for (int i = 0; i < mNumHats; ++i)
   {
      U8 currHatState = SDL_JoystickGetHat(mStick, i);
      if (mHatState[i] == currHatState)
         continue;
         
      manager->joyHatEvent(mDeviceID, i, mHatState[i], currHatState);
      mHatState[i] = currHatState;
   }
      
   // ballz
   // JMQTODO: how to map ball events (xaxis,yaxis?)
   return true;
}

//------------------------------------------------------------------------------
static S32 GetAxisType(S32 axisNum, const char* namedType)
{
   S32 axisType = -1;

   if (namedType != NULL)
   {
      if (dStricmp(namedType, "xaxis")==0)
         axisType = SI_XAXIS;
      else if (dStricmp(namedType, "yaxis")==0)
         axisType = SI_YAXIS;
      else if (dStricmp(namedType, "zaxis")==0)
         axisType = SI_ZAXIS;
      else if (dStricmp(namedType, "rxaxis")==0)
         axisType = SI_RXAXIS;
      else if (dStricmp(namedType, "ryaxis")==0)
         axisType = SI_RYAXIS;
      else if (dStricmp(namedType, "rzaxis")==0)
         axisType = SI_RZAXIS;
      else if (dStricmp(namedType, "slider")==0)
         axisType = SI_SLIDER;
   }

   if (axisType == -1)
   {
      // use a hardcoded default mapping if possible
      switch (axisNum)
      {
         case 0:
            axisType = SI_XAXIS;
            break;
         case 1:
            axisType = SI_YAXIS;
            break;
         case 2: 
            axisType = SI_RZAXIS;
            break;
         case 3:
            axisType = SI_SLIDER;
            break;
      }
   }

   return axisType;
}

//------------------------------------------------------------------------------
void JoystickInputDevice::loadJoystickInfo()
{
   bool opened = false;
   if (mStick == NULL)
   {
      mStick = SDL_JoystickOpen(mDeviceID);
      if (mStick == NULL)
      {
         Con::printf("Unable to open %s: %s", getDeviceName(), SDL_GetError());
         return;
      }
      opened = true;
   }

   // get the number of thingies on this joystick
   mNumAxes = SDL_JoystickNumAxes(mStick);
   mNumButtons = SDL_JoystickNumButtons(mStick);
   mNumHats = SDL_JoystickNumHats(mStick);
   mNumBalls = SDL_JoystickNumBalls(mStick);

   // load axis mapping info
   loadAxisInfo();

   if (opened)
      SDL_JoystickClose(mStick);
}

//------------------------------------------------------------------------------
// for each axis on a joystick, torque needs to know the type of the axis 
// (SI_XAXIS, etc), the minimum value, and the maximum value.  However none of
// this information is generally available with the unix/linux api.  All you
// get is a device and axis number and a value.  Therefore,
// we allow the user to specify these values in preferences.  hopefully 
// someday we can implement a gui joystick calibrator that takes care of this
// cruft for the user.
void JoystickInputDevice::loadAxisInfo()
{
   mAxisList.clear();

   AssertFatal(mStick, "mStick is NULL");

   static int AxisDefaults[] = { SI_XAXIS, SI_YAXIS, SI_ZAXIS, 
                                 SI_RXAXIS, SI_RYAXIS, SI_RZAXIS,
                                 SI_SLIDER };

   int numAxis = SDL_JoystickNumAxes(mStick);
   for (int i = 0; i < numAxis; ++i)
   {
      JoystickAxisInfo axisInfo;

      // defaults
      axisInfo.type = -1;
      axisInfo.minValue = -32768;
      axisInfo.maxValue = 32767;

      // look in console to see if there is mapping information for this axis
      const int TempBufSize = 1024;
      char tempBuf[TempBufSize];
      dSprintf(tempBuf, TempBufSize, "$Pref::Input::Joystick%d::Axis%d", 
         mDeviceID, i);

      const char* axisStr = Con::getVariable(tempBuf);
      if (axisStr == NULL || dStrlen(axisStr) == 0)
      {
         if (i < sizeof(AxisDefaults))
            axisInfo.type = AxisDefaults[i];
      }
      else
      {
         // format is "TorqueAxisName MinValue MaxValue";
         dStrncpy(tempBuf, axisStr, TempBufSize);
         char* temp = dStrtok( tempBuf, " \0" );
         if (temp)
         {
            axisInfo.type = GetAxisType(i, temp);
            temp = dStrtok( NULL, " \0" );
            if (temp)
            {
               axisInfo.minValue = dAtoi(temp);
               temp = dStrtok( NULL, "\0" );
               if (temp)
               {
                  axisInfo.maxValue = dAtoi(temp);
               }
            }
         }
      }

      mAxisList.push_back(axisInfo);
   }
}

//------------------------------------------------------------------------------
const char* JoystickInputDevice::getJoystickAxesString()
{
   char buf[64];
   dSprintf( buf, sizeof( buf ), "%d", mAxisList.size());

   for (Vector<JoystickAxisInfo>::iterator iter = mAxisList.begin();
        iter != mAxisList.end();
        ++iter)
   {
      switch ((*iter).type)
      {
         case SI_XAXIS:
            dStrcat( buf, "\tX" );
            break;
         case SI_YAXIS:
            dStrcat( buf, "\tY" );
            break;
         case SI_ZAXIS:
            dStrcat( buf, "\tZ" );
            break;
         case SI_RXAXIS:
            dStrcat( buf, "\tR" );
            break;
         case SI_RYAXIS:
            dStrcat( buf, "\tU" );
            break;
         case SI_RZAXIS:
            dStrcat( buf, "\tV" );
            break;
         case SI_SLIDER:
            dStrcat( buf, "\tS" );
            break;
      }
   }

   char* returnString = Con::getReturnBuffer( dStrlen( buf ) + 1 );
   dStrcpy( returnString, buf );
   return( returnString );
}


//==============================================================================
// Console Functions
//==============================================================================
ConsoleFunction( activateKeyboard, bool, 1, 1, "activateKeyboard()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->activateKeyboard() );

   return( false );
}

// JMQ: disabled deactivateKeyboard since the script calls it but there is
// no fallback keyboard input in unix, resulting in a permanently disabled
// keyboard
//------------------------------------------------------------------------------
ConsoleFunction( deactivateKeyboard, void, 1, 1, "deactivateKeyboard()" )
{
#if 0
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->deactivateKeyboard();
#endif
}

//------------------------------------------------------------------------------
ConsoleFunction( enableMouse, bool, 1, 1, "enableMouse()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->enableMouse() );

   return ( false );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableMouse, void, 1, 1, "disableMouse()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->disableMouse();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableJoystick, bool, 1, 1, "enableJoystick()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      return( mgr->enableJoystick() );

   return ( false );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableJoystick, void, 1, 1, "disableJoystick()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->disableJoystick();
}

//------------------------------------------------------------------------------
ConsoleFunction( enableLocking, void, 1, 1, "enableLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(true);
}

//------------------------------------------------------------------------------
ConsoleFunction( disableLocking, void, 1, 1, "disableLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(false);
}

//------------------------------------------------------------------------------
ConsoleFunction( toggleLocking, void, 1, 1, "toggleLocking()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr )
      mgr->setLocking(!mgr->getLocking());
}

//------------------------------------------------------------------------------
ConsoleFunction( echoInputState, void, 1, 1, "echoInputState()" )
{
   UInputManager* mgr = dynamic_cast<UInputManager*>( Input::getManager() );
   if ( mgr && mgr->isEnabled() )
   {
      Con::printf( "Input is enabled %s.", 
         mgr->isActive() ? "and active" : "but inactive" );
      Con::printf( "- Keyboard is %sabled and %sactive.", 
         mgr->isKeyboardEnabled() ? "en" : "dis",
         mgr->isKeyboardActive() ? "" : "in" );
      Con::printf( "- Mouse is %sabled and %sactive.", 
         mgr->isMouseEnabled() ? "en" : "dis",
         mgr->isMouseActive() ? "" : "in" );
      Con::printf( "- Joystick is %sabled and %sactive.", 
         mgr->isJoystickEnabled() ? "en" : "dis",
         mgr->isJoystickActive() ? "" : "in" );
   }
   else
      Con::printf( "Input is not enabled." );
}

