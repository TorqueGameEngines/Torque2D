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

#import "platformOSX/platformOSX.h"
#import "platformOSX/osxTorqueView.h"
#import "game/gameInterface.h"
#import "gui/guiCanvas.h"

#pragma mark ---- OSXTorqueView Implementation ----

@interface OSXTorqueView (PrivateMethods)
- (void)windowFinishedLiveResize:(NSNotification *)notification;
- (void)getModifierKey:(U32&)modifiers event:(NSEvent *)event;
- (void)processMouseButton:(NSEvent *)event button:(KeyCodes)button action:(U8)action;
- (void)processKeyEvent:(NSEvent *)event make:(BOOL)make;
@end

@implementation OSXTorqueView

@synthesize contextInitialized = _contextInitialized;

//-----------------------------------------------------------------------------
// Custom initialization method for OSXTorqueView
- (void)initialize
{
    if (self)
    {
        // Make absolutely sure _openGLContext is nil
        _openGLContext = nil;

        NSTrackingAreaOptions trackingOptions = NSTrackingCursorUpdate |
                NSTrackingMouseMoved |
                NSTrackingMouseEnteredAndExited |
                NSTrackingInVisibleRect |
                NSTrackingActiveInActiveApp;

        _trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds] options:trackingOptions owner:self userInfo:nil];

        [self addTrackingArea:_trackingArea];

        inputManager = (osxInputManager *) Input::getManager();
    }
}

//-----------------------------------------------------------------------------
// Default dealloc override
- (void)dealloc
{
    // End notifications
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    // Drop the tracking rectangle for mouse events
    if (_trackingArea != nil)
    {
        [self removeTrackingArea:_trackingArea];
        [_trackingArea release];
    }

    // Custom memory cleanup
    if (_openGLContext != nil)
    {
        [_openGLContext release];
        _openGLContext = nil;
    }

    // "Parent" cleanup
    [super dealloc];
}

//-----------------------------------------------------------------------------
// This view an always be a first responder
- (BOOL)acceptsFirstResponder
{
    return YES;
}

//-----------------------------------------------------------------------------
// Called whent the parent finishes its live resizing
- (void)windowFinishedLiveResize:(NSNotification *)notification
{
    NSSize size = [[self window] frame].size;

    [[osxPlatState sharedPlatState] setWindowSize:(S32)size.width height:(S32)size.height];
    
    NSRect frame = NSMakeRect(0, 0, size.width, size.height);
    
    S32 barHeight = frame.size.height;
    frame = [NSWindow frameRectForContentRect:frame styleMask:NSTitledWindowMask];
    barHeight -= frame.size.height;
    
    NSRect viewFrame = NSMakeRect(0, barHeight, frame.size.width, frame.size.height);
    
    [self setFrame:viewFrame];
    [self updateContext];
}

#pragma mark ---- OSXTorqueView OpenGL Handling ----

//-----------------------------------------------------------------------------
// Allocates a new NSOpenGLContext with the specified pixel format and makes
// it the current OpenGL context automatically
- (void)createContextWithPixelFormat:(NSOpenGLPixelFormat *)pixelFormat
{
    _openGLContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] retain];

    AssertFatal(_openGLContext, "We could not create a valid NSOpenGL rendering context.");

    [_openGLContext setView:self];

    [_openGLContext makeCurrentContext];

    _contextInitialized = YES;
}

//-----------------------------------------------------------------------------
// Clears the current context, releases control from this view, and deallocates
// the NSOpenGLContext
- (void)clearContext
{
    if (_openGLContext != nil)
    {
        [NSOpenGLContext clearCurrentContext];
        [_openGLContext clearDrawable];

        [_openGLContext release];
        _openGLContext = nil;

        _contextInitialized = NO;
    }
}

//-----------------------------------------------------------------------------
// Perform an update on the NSOpenGLContext, which will match the surface
// size to the view's frame
- (void)updateContext
{
    if (_openGLContext != nil)
        [_openGLContext update];
}

//-----------------------------------------------------------------------------
// Perform a swap buffer if the NSOpenGLContext is initialized
- (void)flushBuffer
{
    if (_openGLContext != nil)
        [_openGLContext flushBuffer];
}

//-----------------------------------------------------------------------------
- (int)getVerticalSync
{
    if (_openGLContext != nil)
    {
        GLint swapInterval = 0;
        [_openGLContext getValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
        return swapInterval;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
- (void)setVerticalSync:(bool)sync
{
    if (_openGLContext != nil)
    {
        GLint swapInterval = sync ? 1 : 0;
        [_openGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
    }
}

#pragma mark ---- OSXTorqueView Input Handling ----

//-----------------------------------------------------------------------------
// Fills out the modifiers based on key presses such as shift, alt, etc
- (void)getModifierKey:(U32&)modifiers event:(NSEvent *)event;
{
    /*
     NSAlphaShiftKeyMask = 1 << 16,
     NSShiftKeyMask      = 1 << 17,
     NSControlKeyMask    = 1 << 18,
     NSAlternateKeyMask  = 1 << 19,
     NSCommandKeyMask    = 1 << 20,
     NSNumericPadKeyMask = 1 << 21,
     NSHelpKeyMask       = 1 << 22,
     NSFunctionKeyMask   = 1 << 23,
     NSDeviceIndependentModifierFlagsMask = 0xffff0000U
     */
    
    U32 keyMods = (U32)[event modifierFlags];
    
    if (keyMods & NSShiftKeyMask)
        modifiers |= SI_SHIFT;
    
    if (keyMods & NSCommandKeyMask)
        modifiers |= SI_ALT;
    
    if (keyMods & NSAlternateKeyMask)
        modifiers |= SI_MAC_OPT;
    
    if (keyMods & NSControlKeyMask)
        modifiers |= SI_CTRL;
}

//-----------------------------------------------------------------------------
// Processes mouse up and down events, posts to the event system
- (void)processMouseButton:(NSEvent *)event button:(KeyCodes)button action:(U8)action
{
    // Get the click location
    NSPoint clickLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    
    NSRect bounds = [self bounds];
    
    clickLocation.y = bounds.size.height - clickLocation.y;
    
    // Move the cursor
    Canvas->setCursorPos(Point2I((S32) clickLocation.x, (S32) clickLocation.y));
    
    // Grab any modifiers
    U32 modifiers = 0;
    [self getModifierKey:modifiers event:event];
    
    // Build the input event
    InputEvent torqueEvent;
    
    torqueEvent.deviceType = MouseDeviceType;
    torqueEvent.deviceInst = 0;
    torqueEvent.objType = SI_BUTTON;
    torqueEvent.objInst = button;
    torqueEvent.modifier = modifiers;
    torqueEvent.ascii = 0;
    torqueEvent.action = action;
    if (action == SI_BREAK)
        torqueEvent.fValues[0] = 0.0;
    else
        torqueEvent.fValues[0] = 1.0;
    
    // Post the input event
    Game->postEvent(torqueEvent);
}

//-----------------------------------------------------------------------------
// Processes keyboard up and down events, posts to the event system
- (void)processKeyEvent:(NSEvent *)event make:(BOOL)make
{
    // If input and keyboard are enabled
    if (!Input::isEnabled() && !Input::isKeyboardEnabled())
        return;
    
    unichar chars = [[event charactersIgnoringModifiers] characterAtIndex:0];
    
    // Get the key code for the event
    U32 keyCode = [event keyCode];
    
    U16 objInst = TranslateOSKeyCode(keyCode);
    
    // Grab any modifiers
    U32 modifiers = 0;
    [self getModifierKey:modifiers event:event];
    
    // Build the input event
    InputEvent torqueEvent;
    
    F32 fValue = 1.0f;
    U8 action = SI_MAKE;
    
    if (!make)
    {
        action = SI_BREAK;
        fValue = 0.0f;
    }
    else if(make && [event isARepeat])
    {
        action = SI_REPEAT;
    }
    
    torqueEvent.deviceType = KeyboardDeviceType;
    torqueEvent.deviceInst = 0;
    torqueEvent.objType = SI_KEY;
    torqueEvent.objInst = objInst;
    torqueEvent.modifier = modifiers;
    torqueEvent.ascii = 0;
    torqueEvent.action = action;
    torqueEvent.fValues[0] = fValue;
    torqueEvent.ascii = chars;
    
    // Post the input event
    Game->postEvent(torqueEvent);
}

//-----------------------------------------------------------------------------
// Default mouseDown override
- (void)mouseDown:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON0 action:SI_MAKE];
}

//-----------------------------------------------------------------------------
// Default rightMouseDown override
- (void)rightMouseDown:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON1 action:SI_MAKE];
}

//-----------------------------------------------------------------------------
// Default otherMouseDown override
- (void)otherMouseDown:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON2 action:SI_MAKE];
}

//-----------------------------------------------------------------------------
// Default mouseUp override
- (void)mouseUp:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON0 action:SI_BREAK];
}

//-----------------------------------------------------------------------------
// Default rightMouseUp override
- (void)rightMouseUp:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON1 action:SI_BREAK];
}

//-----------------------------------------------------------------------------
// Default otherMouseUp override
- (void)otherMouseUp:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self processMouseButton:event button:KEY_BUTTON2 action:SI_BREAK];
}

//-----------------------------------------------------------------------------
- (void)mouseEntered:(NSEvent *)event
{
    if (!Canvas->getUseNativeCursor())
    {
        [NSCursor hide];
    }
}

-(void)mouseExited:(NSEvent *)event
{
    [NSCursor unhide];
}
// Default otherMouseDown override
- (void)mouseMoved:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    // Get the mouse location
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    
    // NSViews increase the Y the higher the cursor
    // Torque needs that to be inverted
    NSRect bounds = [self bounds];
    location.y = bounds.size.height - location.y;
    
    // Grab any modifiers
    U32 modifiers = 0;
    [self getModifierKey:modifiers event:event];
    
    
    // Move the cursor
    Canvas->setCursorPos(Point2I((S32) location.x, (S32) location.y));
    
    // Build the mouse event
    MouseMoveEvent TorqueEvent;
    TorqueEvent.xPos = (S32) location.x;
    TorqueEvent.yPos = (S32) location.y;
    TorqueEvent.modifier = modifiers;
    
    // Post the event
    Game->postEvent(TorqueEvent);
}

//-----------------------------------------------------------------------------
// Default mouseDragged override
- (void)mouseDragged:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self mouseMoved:event];
}

//-----------------------------------------------------------------------------
// Default rightMouseDragged override
- (void)rightMouseDragged:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self mouseMoved:event];
}

//-----------------------------------------------------------------------------
// Default otherMouseDragged override
- (void)otherMouseDragged:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    [self mouseMoved:event];
}

//-----------------------------------------------------------------------------
// Default scrollWheel override
- (void)scrollWheel:(NSEvent *)event
{
    if (!Input::isEnabled() && !Input::isMouseEnabled())
        return;
    
    F32 deltaY = [event deltaY];
    
    if (deltaY == 0)
        return;
    
    // Grab any modifiers
    U32 modifiers = 0;
    [self getModifierKey:modifiers event:event];
    
    InputEvent torqueEvent;
    
    torqueEvent.deviceType = MouseDeviceType;
    torqueEvent.deviceInst = 0;
    torqueEvent.objType = SI_ZAXIS;
    torqueEvent.objInst = 0;
    torqueEvent.modifier = modifiers;
    torqueEvent.ascii = 0;
    torqueEvent.action = SI_MOVE;
    torqueEvent.fValues[0] = deltaY;
    Game->postEvent(torqueEvent);
}

//-----------------------------------------------------------------------------
// Default keyDown override
- (void)keyDown:(NSEvent *)event
{
    // If input and keyboard are enabled
    if (!Input::isEnabled() && !Input::isKeyboardEnabled())
        return;
    
    [self processKeyEvent:event make:YES];
}

//-----------------------------------------------------------------------------
// Default keyUp override
- (void)keyUp:(NSEvent *)event
{
    // If input and keyboard are enabled
    if (!Input::isEnabled() && !Input::isKeyboardEnabled())
        return;
    
    [self processKeyEvent:event make:NO];
}

@end
