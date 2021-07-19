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

#ifndef _GUICONTROL_H_
#define _GUICONTROL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif
#ifndef _MRECT_H_
#include "math/mRect.h"
#endif
#ifndef _COLOR_H_
#include "graphics/gColor.h"
#endif
#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif
#ifndef _GUITYPES_H_
#include "gui/guiTypes.h"
#endif
#ifndef _EVENT_H_
#include "platform/event.h"
#endif
#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

#ifndef _LANG_H_
#include "gui/language/lang.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _MFLUID_H_
#include "math/mFluid.h"
#endif

class GuiCanvas;
class GuiEditCtrl;

//-----------------------------------------------------------------------------

/// @defgroup gui_group Gui System
/// The GUI system in Torque provides a powerful way of creating
/// WYSIWYG User Interfaces for your Game or Application written
/// in Torque.  
///
/// The GUI Provides a range of different controls that you may use
/// to arrange and layout your GUI's, including Buttons, Lists, Bitmaps
/// Windows, Containers, and HUD elements.
///
/// The Base Control Class GuiControl provides a basis upon which to 
/// write GuiControl's that may be specific to your particular type
/// of game.  


/// @addtogroup gui_core_group Core
/// @section GuiControl_Intro Introduction
///
/// GuiControl is the base class for GUI controls in Torque. It provides these
/// basic areas of functionality:
///      - Inherits from SimGroup, so that controls can have children.
///      - Interfacing with a GuiControlProfile.
///      - An abstraction from the details of handling user input
///        and so forth, providing friendly hooks like onMouseEnter(), onMouseMove(),
///        and onMouseLeave(), onKeyDown(), and so forth.
///      - An abstraction from the details of rendering and resizing.
///      - Helper functions to manipulate the mouse (mouseLock and
///        mouseUnlock), and convert coordinates (localToGlobalCoord() and
///        globalToLocalCoord()).
///
/// @ref GUI has an overview of the GUI system.
///
/// Tickable Information - taken from guiTickCtrl now retired.
/// This Gui Control is designed to recieve update ticks at a constant interval.
/// It was created to be the Parent class of a control which used a DynamicTexture
/// along with a VectorField to create warping effects much like the ones found
/// in visualization displays for iTunes or Winamp. Those displays are updated
/// at the framerate frequency. This works fine for those effects, however for
/// an application of the same type of effects for things like Gui transitions
/// the framerate-driven update frequency is not desireable because it does not
/// allow the developer to be able to have any idea of a consistant user-experience.
///
/// Enter the Tickable interface. This lets the Gui control, in this case, update
/// the dynamic texture at a constant rate of once per tick, even though it gets
/// rendered every frame, thus creating a framerate-independant update frequency
/// so that the effects are at a consistant speed regardless of the specifics
/// of the system the user is on. This means that the screen-transitions will
/// occur in the same time on a machine getting 300fps in the Gui shell as a
/// machine which gets 150fps in the Gui shell.
/// @see Tickable
///
/// @ingroup gui_group Gui System
/// @{
class GuiControl : public SimGroup, public virtual Tickable
{
private:
   typedef SimGroup Parent;
   typedef GuiControl Children;

public:

    /// @name Control State
    /// @{

   GuiControlProfile *mProfile;

    GuiControlProfile	*mTooltipProfile; 
    S32					mTipHoverTime;
    S32					mTooltipWidth;

    bool    mVisible;
    bool    mActive;
    bool    mAwake;
    bool    mSetFirstResponder;
    bool    mCanSave;
    bool    mIsContainer; ///< if true, then the GuiEditor can drag other controls into this one.

    S32     mLayer;
    static S32     smCursorChanged; ///< Has this control modified the cursor? -1 or type
    RectI   mBounds;
    Point2I mMinExtent;
	Point2I mRenderInsetLT;			///Add this to the mBounds and parent offset to get the true render location of the control
	Point2I mRenderInsetRB;			///The actual rendered inset for the right and bottom sides.
    StringTableEntry mLangTableName;
    LangTable *mLangTable;

    static bool smDesignTime; ///< static GuiControl boolean that specifies if the GUI Editor is active
    /// @}

    /// @name Design Time Editor Access
    /// @{
    static GuiEditCtrl *smEditorHandle; ///< static GuiEditCtrl pointer that gives controls access to editor-NULL if editor is closed
    /// @}

    /// @name Keyboard Input
    /// @{
    SimObjectPtr<GuiControl> mFirstResponder;
    static GuiControl *smPrevResponder;
    static GuiControl *smCurResponder;
    /// @}

    enum horizSizingOptions
    {
        horizResizeRight = 0,   ///< fixed on the left and width
        horizResizeWidth,       ///< fixed on the left and right
        horizResizeLeft,        ///< fixed on the right and width
        horizResizeCenter,
        horizResizeRelative     ///< resize relative
    };
    enum vertSizingOptions
    {
        vertResizeBottom = 0,   ///< fixed on the top and in height
        vertResizeHeight,       ///< fixed on the top and bottom
        vertResizeTop,          ///< fixed in height and on the bottom
        vertResizeCenter,
        vertResizeRelative      ///< resize relative
    };
	enum TextRotationOptions
	{
		tRotateNone = 0,
		tRotateLeft, 
		tRotateRight
	};

protected:
    /// @name Control State
    /// @{

    S32 mHorizSizing;      ///< Set from horizSizingOptions.
    S32 mVertSizing;       ///< Set from vertSizingOptions.

	Point2I mStoredExtent; //Used in conjunction with the min extent.

    StringTableEntry	mConsoleVariable;
    StringTableEntry	mConsoleCommand;
    StringTableEntry	mAltConsoleCommand;
    StringTableEntry	mAcceleratorKey;

    StringTableEntry	mTooltip;

	StringTableEntry    mText;
	StringTableEntry    mTextID;
	bool				mTextWrap;

    /// @}

    /// @name Console
    /// The console variable collection of functions allows a console variable to be bound to the GUI control.
    ///
    /// This allows, say, an edit field to be bound to '$foo'. The value of the console
    /// variable '$foo' would then be equal to the text inside the text field. Changing
    /// either changes the other.
    /// @{

    /// Sets the value of the console variable bound to this control
    /// @param   value   String value to assign to control's console variable
    void setVariable(const char *value);

    /// Sets the value of the console variable bound to this control
    /// @param   value   Integer value to assign to control's console variable
    void setIntVariable(S32 value);

    /// Sets the value of the console variable bound to this control
    /// @param   value   Float value to assign to control's console variable
    void setFloatVariable(F32 value);

    const char* getVariable(); ///< Returns value of control's bound variable as a string
    S32 getIntVariable();      ///< Returns value of control's bound variable as a integer
    F32 getFloatVariable();    ///< Returns value of control's bound variable as a float

public:
    /// Set the name of the console variable which this GuiObject is bound to
    /// @param   variable   Variable name
    void setConsoleVariable(const char *variable);

    /// Set the name of the console function bound to, such as a script function
    /// a button calls when clicked.
    /// @param   newCmd   Console function to attach to this GuiControl
    void setConsoleCommand(const char *newCmd);
    const char * getConsoleCommand(); ///< Returns the name of the function bound to this GuiControl
    LangTable *getGUILangTable(void);
    const UTF8 *getGUIString(S32 id);
    
    /// @}
protected:
    /// @name Callbacks
    /// @{
    /// Executes a console command, and returns the result.
    ///
    /// The global console variable $ThisControl is set to the id of the calling
    /// control. WARNING: because multiple controls may set $ThisControl, at any time,
    /// the value of $ThisControl should be stored in a local variable by the
    /// callback code. The use of the $ThisControl variable is not thread safe.

    /// Executes mConsoleCommand, and returns the result.
    const char* execConsoleCallback();
    /// Executes mAltConsoleCommand, and returns the result.
    const char* execAltConsoleCallback();
    /// @}
public:

    /// @name Editor
    /// These functions are used by the GUI Editor
    /// @{

    /// Sets the size of the GuiControl
    /// @param   horz   Width of the control
    /// @param   vert   Height of the control
    void setSizing(S32 horz, S32 vert);
  
    ///   Overrides Parent Serialization to allow specific controls to not be saved (Dynamic Controls, etc)
    void write(Stream &stream, U32 tabStop, U32 flags);
    /// Returns boolean specifying if a control can be serialized
    bool getCanSave();
    /// Set serialization flag
    void setCanSave(bool bCanSave);
    /// Returns boolean as to whether any parent of this control has the 'no serialization' flag set.
    bool getCanSaveParent();

    /// @}
public:
    /// @name Initialization
    /// @{

    DECLARE_CONOBJECT(GuiControl);
    GuiControl();
    virtual ~GuiControl();
    static void initPersistFields();
    /// @}

    /// @name Accessors
    /// @{

    const Point2I&   getPosition() { return mBounds.point; } ///< Returns position of the control
    const Point2I&   getExtent() { return mBounds.extent; } ///< Returns extents of the control
    const RectI&     getBounds()    { return mBounds; }           ///< Returns the bounds of the control
    const Point2I&   getMinExtent() { return mMinExtent; } ///< Returns minimum size the control can be
    const S32        getLeft() { return mBounds.point.x; } ///< Returns the X position of the control
    const S32        getTop() { return mBounds.point.y; } ///< Returns the Y position of the control
    const S32        getWidth() { return mBounds.extent.x; } ///< Returns the width of the control
    const S32        getHeight() { return mBounds.extent.y; } ///< Returns the height of the control
	
	virtual void             setText(const char *text);
	virtual void             setTextID(S32 id);
	virtual void             setTextID(const char *id);
	virtual const char*      getText();
	inline void				 setTextWrap(const bool wrap) { mTextWrap = wrap; }
	inline bool				 getTextWrap() { return mTextWrap; }

	// Text Property Accessors
	static bool setTextProperty(void* obj, const char* data) { static_cast<GuiControl*>(obj)->setText(data); return false; }
	static const char* getTextProperty(void* obj, const char* data) { return static_cast<GuiControl*>(obj)->getText(); }
	static bool writeTextWrapFn(void* obj, const char* data) { return static_cast<GuiControl*>(obj)->getTextWrap(); }

	static bool setExtentFn(void* obj, const char* data) { GuiControl* ctrl = static_cast<GuiControl*>(obj); Vector2 v = Vector2(data); ctrl->setExtent(Point2I(v.x, v.y)); ctrl->resetStoredExtent(); return false; }
	static bool setMinExtentFn(void* obj, const char* data) { GuiControl* ctrl = static_cast<GuiControl*>(obj); Vector2 v = Vector2(data); ctrl->mMinExtent.set(v.x, v.y); ctrl->resetStoredExtent(); return false; }
	static bool writeMinExtentFn(void* obj,  const char* data) { GuiControl* ctrl = static_cast<GuiControl*>(obj); return ctrl->mMinExtent.x != 0 || ctrl->mMinExtent.y != 0; }

    /// @}

    /// @name Flags
    /// @{

    /// Sets the visibility of the control
    /// @param   value   True if object should be visible
    virtual void setVisible(bool value);
    inline bool isVisible() { return mVisible; } ///< Returns true if the object is visible

    /// Sets the status of this control as active and responding or inactive
    /// @param   value   True if this is active
    void setActive(bool value);
    bool isActive() { return mActive; } ///< Returns true if this control is active

    bool isAwake() { return mAwake; } ///< Returns true if this control is awake

    /// @}

    /// Get information about the size of a scroll line.
    ///
    /// @param   rowHeight   The height, in pixels, of a row
    /// @param   columnWidth The width, in pixels, of a column
    virtual void getScrollLineSizes(U32 *rowHeight, U32 *columnWidth);

    /// Get information about the cursor.
    /// @param   cursor   Cursor information will be stored here
    /// @param   showCursor Will be set to true if the cursor is visible
    /// @param   lastGuiEvent GuiEvent containing cursor position and modifyer keys (ie ctrl, shift, alt etc)
    virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);

    /// @name Children
    /// @{

    /// Adds an object as a child of this object.
    /// @param   obj   New child object of this control
    virtual void addObject(SimObject *obj);

    /// Removes a child object from this control.
    /// @param   obj Object to remove from this control
    void removeObject(SimObject *obj);

    GuiControl *getParent();  ///< Returns the control which owns this one.
    GuiCanvas *getRoot();     ///< Returns the root canvas of this control.
    /// @}

    /// @name Coordinates
    /// @{

    /// Translates local coordinates (wrt this object) into global coordinates
    ///
    /// @param   src   Local coordinates to translate
    virtual Point2I localToGlobalCoord(const Point2I &src);

    /// Returns global coordinates translated into local space
    ///
    /// @param   src   Global coordinates to translate
    virtual Point2I globalToLocalCoord(const Point2I &src);
    /// @}

    /// @name Resizing
    /// @{

    /// Changes the size and/or position of this control
    /// @param   newPosition   New position of this control
    /// @param   newExtent   New size of this control
    virtual void resize(const Point2I &newPosition, const Point2I &newExtent);

    /// Changes the position of this control
    /// @param   newPosition   New position of this control
    virtual void setPosition( const Point2I &newPosition );

    /// Changes the size of this control
    /// @param   newExtent   New size of this control
    virtual void setExtent( const Point2I &newExtent );

    /// Changes the bounds of this control
    /// @param   newBounds   New bounds of this control
    virtual void setBounds( const RectI &newBounds );

    /// Changes the X position of this control
    /// @param   newXPosition   New X Position of this control
    virtual void setLeft( S32 newLeft );

    /// Changes the Y position of this control
    /// @param   newYPosition   New Y Position of this control
    virtual void setTop( S32 newTop );

    /// Changes the width of this control
    /// @param   newWidth   New width of this control
    virtual void setWidth( S32 newWidth );

    /// Changes the height of this control
    /// @param   newHeight   New Height of this control
    virtual void setHeight( S32 newHeight );

    /// Called when a child control of the object is resized
    /// @param   child   Child object
    virtual void childResized(GuiControl *child);

    /// Called when this objects parent is resized
    /// @param   oldParentExtent   The old size of the parent object
    /// @param   newParentExtent   The new size of the parent object
    virtual void parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
    /// @}

    /// @name Rendering
    /// @{

    /// Called when this control is to render itself
    /// @param   offset   The location this control is to begin rendering
    /// @param   updateRect   The screen area this control has drawing access to
    virtual void onRender(Point2I offset, const RectI &updateRect);
    
    /// Render a tooltip at the specified cursor position for this control
    /// @param   cursorPos   position of cursor to display the tip near
    /// @param   tipText     optional alternate tip to be rendered
    virtual bool renderTooltip(Point2I &cursorPos, const char* tipText = NULL );

    /// Called when this control should render its children
    /// @param   offset   The top left of the parent control
    /// @param   contentOffset   The top left of the parent's content
    /// @param   updateRect   The screen area this control has drawing access to
    virtual void renderChildControls(Point2I offset, RectI content, const RectI &updateRect);

    /// Sets the area (local coordinates) this control wants refreshed each frame
    /// @param   pos   UpperLeft point on rectangle of refresh area
    /// @param   ext   Extent of update rect
    void setUpdateRegion(Point2I pos, Point2I ext);

    /// Sets the update area of the control to encompass the whole control
    virtual void setUpdate();
    /// @}

    //child hierarchy calls
    void awaken();          ///< Called when this control and its children have been wired up.
    void sleep();           ///< Called when this control is no more.
    void preRender();       ///< Prerender this control and all its children.

    /// @name Events
    ///
    /// If you subclass these, make sure to call the Parent::'s versions.
    ///
    /// @{

    /// Called when this object is asked to wake up returns true if it's actually awake at the end
    virtual bool onWake();

    /// Called when this object is asked to sleep
    virtual void onSleep();

    /// Do special pre-render proecessing
    virtual void onPreRender();

    /// Called when this object is removed using delete.
    virtual void onRemove();

	/// Called when this object is removed using delete or parent.remove().
	virtual void onGroupRemove();

    /// Called when this object is added to the scene
    bool onAdd();

    /// Called when this object has a new child. Congratulations!
    virtual void onChildAdded( GuiControl *child );

	/// Called when a child is removed.
	virtual inline void onChildRemoved(GuiControl* child) {};

    /// @}

    /// @name Console
    /// @{

    /// Returns the value of the variable bound to this object
    virtual const char *getScriptValue();

    /// Sets the value of the variable bound to this object
    virtual void setScriptValue(const char *value);
    /// @}

    /// @name Input (Keyboard/Mouse)
    /// @{

    /// This function will return true if the provided coordinates (wrt parent object) are
    /// within the bounds of this control
    /// @param   parentCoordPoint   Coordinates to test
    virtual bool pointInControl(const Point2I& parentCoordPoint);

    /// Returns true if the global cursor is inside this control
    bool cursorInControl();

    /// Returns the control which the provided point is under, with layering
    /// @param   pt   Point to test
    /// @param   initialLayer  Layer of gui objects to begin the search
    virtual GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1);

    /// Lock the mouse within the provided control
    /// @param   lockingControl   Control to lock the mouse within
    virtual void mouseLock(GuiControl *lockingControl);

    /// Turn on mouse locking with last used lock control
    virtual void mouseLock();

    /// Unlock the mouse
    virtual void mouseUnlock();

    /// Returns true if the mouse is locked
    virtual bool isMouseLocked();
    /// @}


    /// General input handler.
    virtual bool onInputEvent(const InputEvent &event);

    /// @name Touch/Mouse Events
    /// These functions are called when the input event which is
    /// in the name of the function occurs.
    /// @{
    virtual void onTouchUp(const GuiEvent &event);
    virtual void onTouchDown(const GuiEvent &event);
    virtual void onTouchMove(const GuiEvent &event);
    virtual void onTouchDragged(const GuiEvent &event);
    virtual void onTouchEnter(const GuiEvent &event);
    virtual void onTouchLeave(const GuiEvent &event);

    virtual bool onMouseWheelUp(const GuiEvent &event);
    virtual bool onMouseWheelDown(const GuiEvent &event);

    virtual void onRightMouseDown(const GuiEvent &event);
    virtual void onRightMouseUp(const GuiEvent &event);
    virtual void onRightMouseDragged(const GuiEvent &event);

    virtual void onMiddleMouseDown(const GuiEvent &event);
    virtual void onMiddleMouseUp(const GuiEvent &event);
    virtual void onMiddleMouseDragged(const GuiEvent &event);

    /// @}

	//Called just before onTouch down for the hit control. The focus should then bubble up through the 
	//controls allowing windows to move to the front.
	virtual void onFocus();
    
    /// @name Editor Mouse Events
    ///
    /// These functions are called when the input event which is
    /// in the name of the function occurs.  Conversly from normal
    /// mouse events, these have a boolean return value that, if 
    /// they return true, the editor will NOT act on them or be able
    /// to respond to this particular event.
    ///
    /// This is particularly useful for when writing controls so that
    /// they may become aware of the editor and allow customization
    /// of their data or appearance as if they were actually in use.
    /// For example, the GuiTabBookCtrl catches on mouse down to select
    /// a tab and NOT let the editor do any instant group manipulation.
    /// 
    /// @{

    /// Called when a mouseDown event occurs on a control and the GUI editor is active
    /// @param   event   the GuiEvent which caused the call to this function
    /// @param   offset   the offset which is representative of the units x and y that the editor takes up on screen
    virtual bool onMouseDownEditor(const GuiEvent &event, Point2I offset) { return false; };

    /// Called when a mouseUp event occurs on a control and the GUI editor is active
    /// @param   event   the GuiEvent which caused the call to this function
    /// @param   offset   the offset which is representative of the units x and y that the editor takes up on screen
    virtual bool onMouseUpEditor(const GuiEvent &event, Point2I offset) { return false; };

    /// Called when a rightMouseDown event occurs on a control and the GUI editor is active
    /// @param   event   the GuiEvent which caused the call to this function
    /// @param   offset   the offset which is representative of the units x and y that the editor takes up on screen
    virtual bool onRightMouseDownEditor(const GuiEvent &event, Point2I offset) { return false; };

    /// Called when a mouseDragged event occurs on a control and the GUI editor is active
    /// @param   event   the GuiEvent which caused the call to this function
    /// @param   offset   the offset which is representative of the units x and y that the editor takes up on screen
    virtual bool onMouseDraggedEditor(const GuiEvent &event, Point2I offset) { return false; };

    /// @}

    /// @name Tabs
    /// @{

    /// Find the first tab-accessable child of this control
    virtual GuiControl* findFirstTabable();

    /// Find the last tab-accessable child of this control
    /// @param   firstCall   Set to true to clear the global previous responder
    virtual GuiControl* findLastTabable(bool firstCall = true);

    /// Find previous tab-accessable control with respect to the provided one
    /// @param   curResponder   Current control
    /// @param   firstCall   Set to true to clear the global previous responder
    virtual GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

    /// Find next tab-accessable control with regards to the provided control.
    ///
    /// @param   curResponder   Current control
    /// @param   firstCall   Set to true to clear the global current responder
    virtual GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
    /// @}

    /// Returns true if the provided control is a child (grandchild, or greatgrandchild) of this one.
    ///
    /// @param   child   Control to test
    virtual bool ControlIsChild(GuiControl *child);

    /// @name First Responder
    /// A first responder is the control which reacts first, in it's responder chain, to keyboard events
    /// The responder chain is set for each parent and so there is only one first responder amongst it's
    /// children.
    /// @{

    /// Sets the first responder for child controls
    /// @param   firstResponder   First responder for this chain
    virtual void setFirstResponder(GuiControl *firstResponder);

    /// Sets up this control to be the first in it's group to respond to an input event
    /// @param   value   True if this should be a first responder
    virtual void makeFirstResponder(bool value);

    /// Returns true if this control is a first responder
    bool isFirstResponder();

    /// Sets this object to be a first responder
    virtual void setFirstResponder();

    /// Clears the first responder for this chain
    void clearFirstResponder();

    /// Returns the first responder for this chain
    GuiControl *getFirstResponder() { return mFirstResponder; }

    /// Occurs when the first responder for this chain is lost
    virtual void onLoseFirstResponder();
    /// @}

    /// @name Keyboard Events
    /// @{

    /// Adds the accelerator key for this object to the canvas
    void addAcceleratorKey();

    /// Adds this control's accelerator key to the accelerator map, and
    /// recursively tells all children to do the same.
    virtual void buildAcceleratorMap();

    /// Occurs when the accelerator key for this control is pressed
    ///
    /// @param   index   Index in the acclerator map of the key
    virtual void acceleratorKeyPress(U32 index);

    /// Occurs when the accelerator key for this control is released
    ///
    /// @param   index   Index in the acclerator map of the key
    virtual void acceleratorKeyRelease(U32 index);

    /// Happens when a key is depressed
    /// @param   event   Event descriptor (which contains the key)
    virtual bool onKeyDown(const GuiEvent &event);

    /// Happens when a key is released
    /// @param   event   Event descriptor (which contains the key)
    virtual bool onKeyUp(const GuiEvent &event);

    /// Happens when a key is held down, resulting in repeated keystrokes.
    /// @param   event   Event descriptor (which contains the key)
    virtual bool onKeyRepeat(const GuiEvent &event);
    /// @}

    /// Sets the control profile for this control.
    ///
    /// @see GuiControlProfile
    /// @param   prof   Control profile to apply
    virtual void setControlProfile(GuiControlProfile *prof);

    /// Occurs when this control performs its "action"
    virtual void onAction();

    /// @name Peer Messaging
    /// Used to send a message to other GUIControls which are children of the same parent.
    ///
    /// This is mostly used by radio controls.
    /// @{
    void messageSiblings(S32 message);                      ///< Send a message to all siblings
    virtual void onMessage(GuiControl *sender, S32 msg);    ///< Receive a message from another control
    /// @}

    /// @name Canvas Events
    /// Functions called by the canvas
    /// @{

    /// Called if this object is a dialog, when it is added to the visible layers
    virtual void onDialogPush();

    /// Called if this object is a dialog, when it is removed from the visible layers
    virtual void onDialogPop();
    /// @}

    /// Renders justified text using the profile.
    ///
    /// @note This should move into the graphics library at some point
    void renderText(Point2I &offset, Point2I &extent, const char *text, GuiControlProfile *profile, TextRotationOptions rot = tRotateNone);

	/// Returns a new rect based on the margins.
	RectI applyMargins(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile);

	/// Returns the bounds of the rect after considering the borders.
	RectI applyBorders(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile);

	/// Returns the bounds of the rect this time with padding.
	RectI applyPadding(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile);

	/// Returns the bounds of the rect with margin, borders, and padding applied.
	RectI getInnerRect(Point2I &offset, Point2I &extent, GuiControlState currentState, GuiControlProfile *profile);

	/// Returns the extent of the outer rect given the extent of the inner rect.
	Point2I getOuterExtent(Point2I &innerExtent, GuiControlState currentState, GuiControlProfile *profile);

    virtual void inspectPostApply();
    virtual void inspectPreApply();

	//Stores or spends stored extent
	Point2I extentBattery(Point2I &newExtent);

	//Expells all stored extent
	inline void resetStoredExtent() { mStoredExtent.set(0,0); }

protected:
	virtual void interpolateTick(F32 delta) {};
	virtual void processTick() {};
	virtual void advanceTime(F32 timeDelta) {};

	S32 getTextHorizontalOffset(S32 textWidth, S32 totalWidth, GuiControlProfile::AlignmentType align);
	S32 getTextVerticalOffset(S32 textHeight, S32 totalHeight, GuiControlProfile::VertAlignmentType align);
};
/// @}

#endif
