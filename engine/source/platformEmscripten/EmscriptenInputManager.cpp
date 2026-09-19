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

// The Linux input manager (platformX86UNIX/x86UNIXInputManager.cc) on the web.
// Where the two differ, it is because a page may not do something a desktop
// program may -- move the pointer, hide it whenever it likes, lock it without
// the player's say -- and the comments below say so.

#include "platformEmscripten/platformEmscripten.h"
#include "console/consoleTypes.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "platformEmscripten/EmscriptenInputManager.h"
#include "platformSDL/sdlInput.h"
#include "platformSDL/sdlTextInput.h"
#include "math/mMathFn.h"

#include <SDL.h>

//------------------------------------------------------------------------------
// The modifier keys held right now, for an event that carries none of its own
// -- a mouse button, a joystick axis.
static U8 CurrentModifiers()
{
   return (U8)KeyMapSDL::getTorqueModFromSDL(SDL_GetModState());
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
   mLastMouseX = mLastMouseY = -1;
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

   dMemset( mMouseButtonState, 0, sizeof( mMouseButtonState ) );
   dMemset( mKeyboardState, 0, sizeof( mKeyboardState ) );

   mJoystickEnabled = false;
   initJoystick();

   mEnabled = true;
   mMouseEnabled = true;
   mKeyboardEnabled = true;

   // Nothing to ask of the keyboard: the browser repeats a held key by itself,
   // at the player's rate, and SDL marks the repeats (keyEvent).
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

   // A browser shows a page a gamepad only once a button on it has been
   // pressed, so at this point there are usually none.
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
// The pointer is left as the canvas has it (Input::setCursorState): hidden
// while the canvas draws a cursor of its own, shown while it uses the
// browser's. The desktop back-ends hide it here whenever input is active; in
// a page under SDL 2 that hiding is real, and would take the pointer away over
// a canvas that draws none. (The SDL 1.2 port could not hide it at all.)
void UInputManager::activate()
{
   if (mEnabled && !isActive())
   {
      mActive = true;
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
   dMemset(mKeyboardState, 0, sizeof(mKeyboardState));
}

//------------------------------------------------------------------------------
void UInputManager::resetMouseState()
{
   // unpress any buttons; in the future we may want
   // to actually sync with the mouse state
   for (int i = 0; i < NumMouseButtons; ++i)
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

   dMemset(mMouseButtonState, 0, sizeof(mMouseButtonState));
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

   // dispose of any lingering SDL keyboard, text and mouse events
   SDL_FlushEvents(SDL_KEYDOWN, SDL_MOUSEWHEEL);
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
// A locked mouse is SDL 2's relative mode, as on Linux, which on the web is
// the browser's pointer lock. The browser locks the pointer only in answer to
// the player's click or key, so SDL asks now and the lock happens at the next
// one.
void UInputManager::lockInput()
{
   SDL_Window* window = gPlatState.sdlWindow;
   if (window && !gPlatState.backgrounded && gPlatState.mouseLocked &&
      mLocking && !SDL_GetRelativeMouseMode())
   {
      SDL_SetWindowGrab(window, SDL_TRUE);
      SDL_SetRelativeMouseMode(SDL_TRUE);
   }
}

//------------------------------------------------------------------------------
void UInputManager::unlockInput()
{
   if (SDL_GetRelativeMouseMode())
      SDL_SetRelativeMouseMode(SDL_FALSE);

   SDL_Window* window = gPlatState.sdlWindow;
   if (window && SDL_GetWindowGrab(window))
      SDL_SetWindowGrab(window, SDL_FALSE);
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
   if (gPlatState.mouseLocked)
   {
      InputEvent ievent;
      ievent.deviceInst = 0;
      ievent.deviceType = MouseDeviceType;
      ievent.objInst = 0;
      ievent.modifier = CurrentModifiers();
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
      // In canvas pixels: SDL scales the browser's position on the page to
      // the canvas's size.
      MouseMoveEvent mmevent;
      mmevent.xPos = mLastMouseX = event.motion.x;
      mmevent.yPos = mLastMouseY = event.motion.y;
      mmevent.modifier = CurrentModifiers();
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
void UInputManager::joyButtonEvent(U8 deviceID, U8 buttonNum, bool pressed)

{
   S32 action = pressed ? SI_MAKE : SI_BREAK;
   S32 objInst = buttonNum + KEY_BUTTON0;

   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = CurrentModifiers();
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
      ( CurrentModifiers() & SI_SHIFT ? 'S' : '.' ),
      ( CurrentModifiers() & SI_CTRL ? 'C' : '.' ),
      ( CurrentModifiers() & SI_ALT ? 'A' : '.' ));
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
   ievent.modifier = CurrentModifiers();
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

   if (scaledValue > 1.f)
      scaledValue = 1.f;
   else if (scaledValue < -1.f)
      scaledValue = -1.f;

   // create and post the event
   InputEvent ievent;

   ievent.deviceInst = deviceID;
   ievent.deviceType = JoystickDeviceType;
   ievent.modifier = CurrentModifiers();
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

   // The browser's fourth and fifth buttons (back and forward) are SDL's X1
   // and X2. The wheel is SDL_MOUSEWHEEL (mouseWheelEvent).
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
      case SDL_BUTTON_X1:
         objInst = KEY_BUTTON3;
         break;
      case SDL_BUTTON_X2:
         objInst = KEY_BUTTON4;
         break;
   }

   if (objInst == -1)
      // unsupported button
      return;

   S32 buttonID = (objInst - KEY_BUTTON0);
   mMouseButtonState[buttonID] = ( action == SI_MAKE );

   InputEvent ievent;

   ievent.deviceInst = 0;
   ievent.deviceType = MouseDeviceType;
   ievent.modifier = CurrentModifiers();
   ievent.ascii = 0;
   ievent.objType = SI_BUTTON;
   ievent.objInst = objInst;
   ievent.action = action;
   ievent.fValues[0] = (action == SI_MAKE) ? 1.0 : 0.0;
#ifdef LOG_INPUT
   Input::log( "EVENT (Input): mouse button%d %s. MODS:%c%c%c\n",
      buttonID,
      action == SI_MAKE ? "pressed" : "released",
      ( ievent.modifier & SI_SHIFT ? 'S' : '.' ),
      ( ievent.modifier & SI_CTRL ? 'C' : '.' ),
      ( ievent.modifier & SI_ALT ? 'A' : '.' ));
#endif

   Game->postEvent(ievent);
}

//------------------------------------------------------------------------------
// A notch of the wheel is worth $pref::Input::MouseWheelSpeed, 120 unless the
// game says otherwise, as on Linux and Windows. SDL counts a notch as 100
// pixels or 3 lines of the browser's wheel event, and adds a trackpad's small
// movements up until they make one. The SDL 1.2 back-end ignored the wheel.
void UInputManager::mouseWheelEvent(const SDL_Event& event)
{
   const S32 notches = event.wheel.y;
   if (notches == 0)
      return; // sideways only, or not a whole notch yet

   InputEvent ievent;

   ievent.deviceInst = 0;
   ievent.deviceType = MouseDeviceType;
   ievent.modifier = CurrentModifiers();
   ievent.ascii = 0;
   ievent.objType = SI_ZAXIS;
   ievent.objInst = 0;
   ievent.action = SI_MOVE;
   ievent.fValues[0] = (F32)(notches *
      Con::getIntVariable("$pref::Input::MouseWheelSpeed", 120));
#ifdef LOG_INPUT
   Input::log( "EVENT (Input): mouse wheel moved %s: %.1f. MODS:%c%c%c\n",
      notches > 0 ? "up" : "down",
      ievent.fValues[0],
      ( ievent.modifier & SI_SHIFT ? 'S' : '.' ),
      ( ievent.modifier & SI_CTRL ? 'C' : '.' ),
      ( ievent.modifier & SI_ALT ? 'A' : '.' ));
#endif

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
   // The key, by where it is on the keyboard (platformSDL/sdlInput.h) -- SDL
   // reads it from the browser's KeyboardEvent.code. A key Torque has no code
   // for is dropped, since KEY_NULL is what a typed character arrives as, not
   // a key.
   const U32 keyCode = KeyMapSDL::getTorqueScanCodeFromSDL(event.key.keysym.scancode);
   if (keyCode == KEY_NULL)
      return;

   S32 action = (event.type == SDL_KEYDOWN) ? SI_MAKE : SI_BREAK;
   // SDL marks the repeats of a held key; a key that was down already for any
   // other reason counts as repeating too
   if (action == SI_MAKE && (event.key.repeat || mKeyboardState[keyCode]))
      action = SI_REPEAT;
   mKeyboardState[keyCode] = (action != SI_BREAK);

   InputEvent ievent;

   ievent.deviceInst = 0;
   ievent.deviceType = KeyboardDeviceType;
   ievent.objType = SI_KEY;
   ievent.objInst = keyCode;
   ievent.action = action;
   ievent.fValues[0] = (action == SI_BREAK) ? 0.0 : 1.0;

   // The modifiers held with the key, as SDL has them. A modifier key itself
   // goes without any, as on Linux, so that a binding on "lshift" alone
   // matches its press.
   switch (keyCode)
   {
      case KEY_LSHIFT:
      case KEY_RSHIFT:
      case KEY_LCONTROL:
      case KEY_RCONTROL:
      case KEY_LALT:
      case KEY_RALT:
         ievent.modifier = 0;
         break;
      default:
         ievent.modifier = KeyMapSDL::getTorqueModFromSDL(event.key.keysym.mod);
         break;
   }

   // What the key types, if anything, follows as character events of its own
   // (platformSDL/sdlTextInput.h). The SDL 1.2 back-end made the character up
   // from the key, from a table of what each key types on a US keyboard.
   ievent.ascii = 0;

   Game->postEvent(ievent);
   SDLTextInput::withholdGlobalKeyText(ievent);

#ifdef LOG_INPUT
   Input::log( "EVENT (Input): %s key %s. MODS:%c%c%c\n",
      getKeyName(ievent.objInst),
      action == SI_MAKE ? "pressed" : (action == SI_REPEAT ? "repeated" : "released"),
      ( ievent.modifier & SI_SHIFT ? 'S' : '.' ),
      ( ievent.modifier & SI_CTRL ? 'C' : '.' ),
      ( ievent.modifier & SI_ALT ? 'A' : '.' ));
#endif
}

//------------------------------------------------------------------------------
// Unlike Linux there is no putting the pointer back where Torque last saw it
// after unlocking: a page cannot move the pointer. The browser leaves it where
// it was when the lock began.
void UInputManager::setWindowLocked(bool locked)
{
   if (locked)
      lockInput();
   else
      unlockInput();
}

//------------------------------------------------------------------------------
void UInputManager::processEvent(const SDL_Event& event)
{
   if (!mEnabled || !isActive())
      return;

   switch (event.type)
   {
      case SDL_MOUSEMOTION:
         if (mMouseActive)
            mouseMotionEvent(event);
         break;
      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
         if (mMouseActive)
            mouseButtonEvent(event);
         break;
      case SDL_MOUSEWHEEL:
         if (mMouseActive)
            mouseWheelEvent(event);
         break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
         if (mKeyboardActive)
            keyEvent(event);
         break;
      case SDL_TEXTINPUT:
         if (mKeyboardActive)
            SDLTextInput::postText(event.text.text);
         break;
   }
}

//------------------------------------------------------------------------------
void UInputManager::process()
{
   if (!mEnabled || !isActive())
      return;

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
   return SDL_JoystickNameForIndex(mDeviceID);
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
   {
      SDL_JoystickClose(mStick);
      mStick = NULL;
   }
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
         if (i < (int)(sizeof(AxisDefaults) / sizeof(AxisDefaults[0])))
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
