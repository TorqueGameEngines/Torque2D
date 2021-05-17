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

#ifndef _GUICANVAS_H_
#define _GUICANVAS_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif
#ifndef _EVENT_H_
#include "platform/event.h"
#endif
#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _PLATFORMINPUT_H_
#include "platform/platformInput.h"
#endif
/// A canvas on which rendering occurs.
///
///
/// @section GuiCanvas_contents What a GUICanvas Can Contain...
///
/// @subsection GuiCanvas_content_contentcontrol Content Control
/// A content control is the top level GuiControl for a screen. This GuiControl
/// will be the parent control for all other GuiControls on that particular
/// screen.
///
/// @subsection GuiCanvas_content_dialogs Dialogs
///
/// A dialog is essentially another screen, only it gets overlaid on top of the
/// current content control, and all input goes to the dialog. This is most akin
/// to the "Open File" dialog box found in most operating systems. When you
/// choose to open a file, and the "Open File" dialog pops up, you can no longer
/// send input to the application, and must complete or cancel the open file
/// request. Torque keeps track of layers of dialogs. The dialog with the highest
/// layer is on top and will get all the input, unless the dialog is
/// modeless, which is a profile option.
///
/// @see GuiControlProfile
///
/// @section GuiCanvas_dirty Dirty Rectangles
///
/// The GuiCanvas is based on dirty regions.
///
/// Every frame the canvas paints only the areas of the canvas that are 'dirty'
/// or need updating. In most cases, this only is the area under the mouse cursor.
/// This is why if you look in guiCanvas.cc the call to glClear is commented out.
/// If you want a really good idea of what exactly dirty regions are and how they
/// work, un-comment that glClear line in the renderFrame method of guiCanvas.cc
///
/// What you will see is a black screen, except in the dirty regions, where the
/// screen will be painted normally. If you are making an animated GuiControl
/// you need to add your control to the dirty areas of the canvas.
///
class GuiCanvas : public GuiControl
{

protected:
   typedef GuiControl Parent;
   typedef SimObject Grandparent;

    /// Background color.
    ColorF                      mBackgroundColor;
    bool                        mUseBackgroundColor;


   /// @name Rendering
   /// @{

   ///
   RectI      mOldUpdateRects[2];
   RectI      mCurUpdateRect;
   F32        rLastFrameTime;
   /// @}

   /// @name Cursor Properties
   /// @{


   F32         mPixelsPerMickey; ///< This is the scale factor which relates
                                 ///  mouse movement in pixels (one unit of
                                 ///  mouse movement is a mickey) to units in
                                 ///  the GUI.
   bool        cursorON;
   bool        mShowCursor;
   bool        mRenderFront;
   Point2F     cursorPt;
   Point2I     lastCursorPt;
   GuiCursor   *defaultCursor;
   GuiCursor   *lastCursor;
   bool        lastCursorON;
   bool        mUseNativeCursor;
   /// @}

   /// @name Mouse Input
   /// @{

   SimObjectPtr<GuiControl>   mMouseCapturedControl;  ///< All mouse events will go to this ctrl only
   SimObjectPtr<GuiControl>   mMouseControl;          ///< the control the mouse was last seen in unless some other one captured it
   bool                       mMouseControlClicked;   ///< whether the current ctrl has been clicked - used by helpctrl
   U32                        mPrevMouseTime;         ///< this determines how long the mouse has been in the same control
   U32                        mNextMouseTime;         ///< used for onMouseRepeat()
   U32                        mInitialMouseDelay;     ///< also used for onMouseRepeat()
   bool                       mMouseButtonDown;       ///< Flag to determine if the button is depressed
   bool                       mMouseRightButtonDown;  ///< bool to determine if the right button is depressed
   bool                       mMouseMiddleButtonDown; ///< Middle button flag
   GuiEvent                   mLastEvent;

   U8                         mLastMouseClickCount;
   S32                        mLastMouseDownTime;
   bool                       mLeftMouseLast;
   bool                       mMiddleMouseLast;
   bool                       mRightMouseLast;
   Point2F                    mMouseDownPoint;
   S32                        mDoubleClickWidth;
   S32                        mDoubleClickHeight;
   S32                        mDoubleClickTime;

   Point2F					  mPrevMouseMovePosition; ///< Holds the previous position of the mouse the last time a mouse move event was processed.
   S32						  mTouchDetectionSize; ///< Changes in the x or y position of the mouse greater than this value will could be touch events.
   bool						  mPotentialTouchEvent; ///< True if the mouse made a jump that looks like a touch event.
   U8						  mPotentialMouseEventCount; ///< Counts how many small mouse movements have occured in a row that to determine if touch has been abandoned.
   bool						  mHideCursorBecauseOfTouch; ///< Touch event has been detected. Hide the cursor.

   virtual void findMouseControl(const GuiEvent &event);
   virtual void refreshMouseControl();
   /// @}

   /// @name Keyboard Input
   /// @{

   GuiControl   *keyboardControl;                     ///<  All keyboard events will go to this ctrl first
   U32          nextKeyTime;


   /// Accelerator key map
   struct AccKeyMap
   {
      GuiControl *ctrl;
      U32 index;
      U32 keyCode;
      U32 modifier;

	  inline bool IsKeyCodeEqual(U32 testCode)
	  {
		  return (testCode == keyCode ||
			  (keyCode == KEY_ALT && (testCode == KEY_RALT || testCode == KEY_LALT)) ||
			  (keyCode == KEY_SHIFT && (testCode == KEY_RSHIFT || testCode == KEY_LSHIFT)) || 
			  (keyCode == KEY_CONTROL && (testCode == KEY_RCONTROL || testCode == KEY_LCONTROL)) || 
			  (keyCode == KEY_MAC_OPT && (testCode == KEY_MAC_ROPT || testCode == KEY_MAC_LOPT)));
	  }
   };
   Vector <AccKeyMap> mAcceleratorMap;

   //for tooltip rendering
   U32 hoverControlStart;
   GuiControl* hoverControl;
   Point2I hoverPosition;
   bool hoverPositionSet;
   U32 hoverLeftControlTime;

   /// @}

public:
   DECLARE_CONOBJECT(GuiCanvas);
   GuiCanvas();
   virtual ~GuiCanvas();

    static void             initPersistFields();


    /// Background color.
    inline void             setBackgroundColor( const ColorF& backgroundColor ) { mBackgroundColor = backgroundColor; }
    inline const ColorF&    getBackgroundColor( void ) const            { return mBackgroundColor; }
    inline void             setUseBackgroundColor( const bool useBackgroundColor ) { mUseBackgroundColor = useBackgroundColor; }
    inline bool             getUseBackgroundColor( void ) const         { return mUseBackgroundColor; }

   /// @name Rendering methods
   ///
   /// @{

   /// Repaints the dirty regions of the canvas
   /// @param   preRenderOnly   If set to true, only the onPreRender methods of all the GuiControls will be called
   /// @param   bufferSwap      If set to true, it will swap buffers at the end. This is to support canvas-subclassing.
   virtual void renderFrame(bool preRenderOnly, bool bufferSwap = true);

   /// Repaints the entire canvas by calling resetUpdateRegions() and then renderFrame()
   virtual void paint();

   /// Adds a dirty area to the canvas so it will be updated on the next frame
   /// @param   pos   Screen-coordinates of the upper-left hand corner of the dirty area
   /// @param   ext   Width/height of the dirty area
   virtual void addUpdateRegion(Point2I pos, Point2I ext);

   /// Resets the update regions so that the next call to renderFrame will
   /// repaint the whole canvas
   virtual void resetUpdateRegions();

   /// Resizes the content control to match the canvas size.
   void maintainSizing();

   /// This builds a rectangle which encompasses all of the dirty regions to be
   /// repainted
   /// @param   updateUnion   (out) Rectangle which surrounds all dirty areas
   virtual void buildUpdateUnion(RectI *updateUnion);

   /// This will swap the buffers at the end of renderFrame. It was added for canvas
   /// sub-classes in case they wanted to do some custom code before the buffer
   /// flip occured.
   virtual void swapBuffers();

   /// @}

   /// @name Canvas Content Management
   /// @{

   /// This sets the content control to something different
   /// @param   gui   New content control
   virtual void setContentControl(GuiControl *gui);

   /// Returns the content control
   virtual GuiControl *getContentControl();

   /// Adds a dialog control onto the stack of dialogs
   /// @param   gui   Dialog to add
   /// @param   layer   Layer to put dialog on
   virtual void pushDialogControl(GuiControl *gui, S32 layer = 0);

   /// Removes a specific layer of dialogs
   /// @param   layer   Layer to pop off from
   virtual void popDialogControl(S32 layer = 0);

   /// Removes a specific dialog control
   /// @param   gui   Dialog to remove from the dialog stack
   virtual void popDialogControl(GuiControl *gui);
   ///@}

   /// This turns on/off front-buffer rendering
   /// @param   front   True if all rendering should be done to the front buffer
   virtual void setRenderFront(bool front) { mRenderFront = front; }

   /// @name Cursor commands
   /// A cursor can be on, but not be shown. If a cursor is not on, than it does not
   /// process input.
   /// @{

   /// Sets the cursor for the canvas.
   /// @param   cursor   New cursor to use.
   virtual void setCursor(GuiCursor *cursor);

   virtual bool getUseNativeCursor(void);
   virtual void useNativeCursor(bool useNative);

   /// Returns true if the cursor is on.
   virtual bool isCursorON() {return cursorON; }

   /// Sets the time allowed between clicks to be considered a double click.
   void setDoubleClickTime(S32 time) { mDoubleClickTime = time; };

   /// Sets the amount of movement allowed that won't cancel a double click.
   void setDoubleClickWidth(S32 width) { mDoubleClickWidth = width; };
   void setDoubleClickHeight(S32 height) { mDoubleClickHeight = height; };

   /// Turns the cursor on or off.
   /// @param   onOff   True if the cursor should be on.
   virtual void setCursorON(bool onOff);

   /// Sets the position of the cursor
   /// @param   pt   Point, in screenspace for the cursor
   virtual void setCursorPos(const Point2I &pt);

   /// Returns the point, in screenspace, at which the cursor is located.
   virtual Point2I getCursorPos()                 { return Point2I(S32(cursorPt.x), S32(cursorPt.y)); }

   /// Enable/disable rendering of the cursor.
   /// @param   state    True if we should render cursor
   virtual void showCursor(bool state)            { mShowCursor = state; Input::setCursorState(state); }

   /// Returns true if the cursor is being rendered.
   virtual bool isCursorShown()                   { return(mShowCursor); }
   /// @}
   ///used by the tooltip resource
   Point2I getCursorExtent() { return defaultCursor->getExtent(); }
 
   /// @name Input Processing
   /// @{

   /// Processes an input event
   /// @see InputEvent
   /// @param   event   Input event to process
   virtual bool processInputEvent(const InputEvent *event);

   /// Processes a mouse movement event
   /// @see MouseMoveEvent
   /// @param   event   Mouse move event to process
   virtual void processMouseMoveEvent(const MouseMoveEvent *event);

   virtual void processScreenTouchEvent(const ScreenTouchEvent *event);

   /// @}

   /// @name Mouse Methods
   /// @{

   /// When a control gets the mouse lock this means that that control gets
   /// ALL mouse input and no other control recieves any input.
   /// @param   lockingControl   Control to lock mouse to
   virtual void mouseLock(GuiControl *lockingControl);

   /// Unlocks the mouse from a control
   /// @param   lockingControl   Control to unlock from
   virtual void mouseUnlock(GuiControl *lockingControl);

   /// Returns the control which the mouse is over
   virtual GuiControl* getMouseControl()       { return mMouseControl; }

   /// Returns the control which the mouse is locked to if any
   virtual GuiControl* getMouseLockedControl() { return mMouseCapturedControl; }

   /// Returns true if the left mouse button is down
   virtual bool mouseButtonDown(void) { return mMouseButtonDown; }

   /// Returns true if the right mouse button is down
   virtual bool mouseRightButtonDown(void) { return mMouseRightButtonDown; }

   virtual void checkLockMouseMove(const GuiEvent &event);
   /// @}

   /// @name Mouse input methods
   /// These events process the events before passing them down to the
   /// controls they effect. This allows for things such as the input
   /// locking and such.
   ///
   /// Each of these methods corosponds to the action in it's method name
   /// and processes the GuiEvent passd as a parameter
   /// @{
   virtual void rootMouseUp(const GuiEvent &event);
   virtual void rootMouseDown(const GuiEvent &event);
   virtual void rootMouseMove(const GuiEvent &event);
   virtual void rootMouseDragged(const GuiEvent &event);

   virtual void rootScreenTouchUp(const GuiEvent &event);
   virtual void rootScreenTouchDown(const GuiEvent &event);
   virtual void rootScreenTouchMove(const GuiEvent &event);

   virtual void rootRightMouseDown(const GuiEvent &event);
   virtual void rootRightMouseUp(const GuiEvent &event);
   virtual void rootRightMouseDragged(const GuiEvent &event);

   virtual void rootMiddleMouseDown(const GuiEvent &event);
   virtual void rootMiddleMouseUp(const GuiEvent &event);
   virtual void rootMiddleMouseDragged(const GuiEvent &event);

   virtual void rootMouseWheelUp(const GuiEvent &event);
   virtual void rootMouseWheelDown(const GuiEvent &event);
   /// @}

   /// @name Keyboard input methods
   /// First responders
   ///
   /// A first responder is a the GuiControl which responds first to input events
   /// before passing them off for further processing.
   /// @{

   /// Moves the first responder to the next tabable controle
   virtual bool tabNext(void);

   /// Moves the first responder to the previous tabable control
   virtual bool tabPrev(void);

   /// Setups a keyboard accelerator which maps to a GuiControl.
   ///
   /// @param   ctrl       GuiControl to map to.
   /// @param   index
   /// @param   keyCode    Key code.
   /// @param   modifier   Shift, ctrl, etc.
   virtual void addAcceleratorKey(GuiControl *ctrl, U32 index, U32 keyCode, U32 modifier);

   /// Sets the first responder.
   /// @param   firstResponder    Control to designate as first responder
   virtual void setFirstResponder(GuiControl *firstResponder);
   /// @}
};

extern GuiCanvas *Canvas;

#endif
