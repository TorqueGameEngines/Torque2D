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
#include "platform/platformVideo.h"
#include "game/gameInterface.h"

#pragma mark ---- OSXPlatState Implementation ----

@interface osxPlatState (PrivateMethods)
- (void)mainTorqueLoop:(NSTimer *)obj;
@end

@implementation osxPlatState

@synthesize window = _window;
@synthesize torqueView = _torqueView;
@synthesize cgDisplay = _cgDisplay;
@synthesize applicationID = _applicationID;
@synthesize alertSemaphore = _alertSemaphore;
@synthesize fullScreen = _fullscreen;
@synthesize argc = _argc;
@synthesize argv = _argv;
@synthesize platformRandom = _platformRandom;
@synthesize desktopBitsPixel = _desktopBitsPixel;
@synthesize desktopWidth = _desktopWidth;
@synthesize desktopHeight = _desktopHeight;
@synthesize currentSimTime = _currentSimTime;
@synthesize lastTimeTick = _lastTimeTick;
@synthesize sleepTicks = _sleepTicks;
@synthesize mainCSDirectory = _mainCSDirectory;
@synthesize backgrounded = _backgrounded;
@synthesize minimized = _minimized;
@synthesize mouseLocked = _mouseLocked;
@synthesize windowTitle = _windowTitle;
@synthesize quit = _quit;
@synthesize osxTimer = _osxTimer;

static osxPlatState * tempSharedPlatState = nil;

//-----------------------------------------------------------------------------

- (id)init
{
    self = [super init];
    
    if (self)
    {
        // Default window behaviors
        _backgrounded = false;
        _minimized = false;
        _mouseLocked = true;
        _fullscreen = false;
        _quit = false;
        
        // Default window resolution
        _desktopBitsPixel = 32;
        _desktopWidth = 1024;
        _desktopHeight = 768;
        _windowSize.x = 1024;
        _windowSize.y = 768;
        
        _windowTitle = [[NSString alloc] initWithString:@"Torque 2D OS X"];
        
        // Default window
        _window = nil;
        
        // Default system variables
        _currentSimTime = 0;
        _sleepTicks = 0;
        _lastTimeTick = 0;
        _argc = 0;
        _alertSemaphore = Semaphore::createSemaphore(0);
        _platformRandom = new RandomLCG();
    }
    
    return self;
}

//-----------------------------------------------------------------------------

- (void)dealloc
{
    if (_window)
        [_window release];
    
    if (_windowTitle)
        [_windowTitle release];
    
    if (_mainCSDirectory)
        [_mainCSDirectory release];
    
    if (_platformRandom)
        delete _platformRandom;
    
    [super dealloc];
}

-(void) updateWindowTitle:(const char*)title
{
    _windowTitle = [NSString stringWithFormat:@"%s", title];
    [_window setTitle:_windowTitle];
}

//-----------------------------------------------------------------------------

- (void)setWindowSize:(int)width height:(int)height
{
    // Store the width and height in the state
    _windowSize.x = width * NSScreen.mainScreen.backingScaleFactor; // TODO: See if there's a better solution than this - WageCrusader
    _windowSize.y = height * NSScreen.mainScreen.backingScaleFactor; // TODO: See if there's a better solution than this - WageCrusader
    // Get the window's current frame
    NSRect frame = NSMakeRect([_window frame].origin.x, [_window frame].origin.y, width, height);
    
    // If we are not going to full screen mode, get a new frame offset that accounts
    // for the title bar height
    if (!_fullscreen)
    {
		frame = [NSWindow frameRectForContentRect:frame styleMask:NSWindowStyleMaskTitled];
        
        // Set the new window frame
        [_window setFrame:frame display:YES];
    }
    else
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
        
        NSRect mainDisplayRect = [[NSScreen mainScreen] frame];
        // Update the frame of the torqueView to match the window
        NSRect viewFrame = NSMakeRect(0, 0, mainDisplayRect.size.width, mainDisplayRect.size.height);
        [_torqueView setFrame:viewFrame];
        [_torqueView updateContext];
        
        // Otherwise, just go straight full screen
        [_torqueView enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
        
#else
        // Otherwise, just go straight full screen
        [_window toggleFullScreen:self];
#endif
    }
    
    // Update the frame of the torqueView to match the window
    NSRect viewFrame = NSMakeRect(0, 0, width, height);
    [_torqueView setFrame:viewFrame];
    
    [_torqueView updateContext];
    
    [_window makeKeyAndOrderFront:NSApp];
    [_window makeFirstResponder:_torqueView];
}

//-----------------------------------------------------------------------------

-(Point2I&) getWindowSize
{
    return _windowSize;
}

//-----------------------------------------------------------------------------

- (U32)windowWidth
{
    return (U32)_windowSize.x;
}

//-----------------------------------------------------------------------------

- (U32)windowHeight
{
   return (U32)_windowSize.y;
}

#pragma mark ---- Singleton Functions ----

//-----------------------------------------------------------------------------

+ (id)sharedPlatState
{
    @synchronized(self)
    {
        if (tempSharedPlatState == nil)
            tempSharedPlatState = (osxPlatState *) [[super allocWithZone:NULL] init];
    }
    
    return tempSharedPlatState;
}

//-----------------------------------------------------------------------------

+ (id)allocWithZone:(NSZone *)zone
{
    return [[self sharedPlatState] retain];
}

//-----------------------------------------------------------------------------

- (id)copyWithZone:(NSZone *)zone
{
    return self;
}

//-----------------------------------------------------------------------------

- (id)retain
{
    return self;
}

//-----------------------------------------------------------------------------

- (NSUInteger)retainCount
{
    // Denotes an object that cannot be released
    return UINT_MAX;
}

//-----------------------------------------------------------------------------

- (oneway void)release
{
    // never release
}

//-----------------------------------------------------------------------------

- (id)autorelease
{
    return self;
}

#pragma mark ---- Torque 2D Functions ----

//-----------------------------------------------------------------------------
// Initializes the Game and any additional inits you need to perform
- (BOOL) initializeTorque2D
{
    return Game->mainInitialize(_argc, _argv);
}

//-----------------------------------------------------------------------------
// Runs the main Game instance and any other looping calls you need to perform
- (void) mainTorqueLoop:(NSTimer *)obj
{
    if(Game->isRunning())
    {
        Game->mainLoop();
    }
    else
    {
        [NSApp terminate:self];
    }
}

//-----------------------------------------------------------------------------
// Entry function for running Torque 2D
- (void) runTorque2D
{
    BOOL initialResult = [self initializeTorque2D];
    
    if (initialResult)
    {
        // Setting the interval to 1ms to handle Box2D and rendering        
        _osxTimer = [NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(mainTorqueLoop:) userInfo:self repeats:YES];
    }
    else
    {
        [NSApp terminate:self];
    }
}

//-----------------------------------------------------------------------------
// Shut down the main Game instancea nd perform any additional cleanup
- (void) shutDownTorque2D
{
    // Shutdown the game
    Game->mainShutdown();
    
    // Perform any platform cleanup
}

@end

#pragma mark ---- Platform Namespace Functions ----

//-----------------------------------------------------------------------------
// Used for initializing the OS X platform code
void Platform::init()
{
    // Set the global script variable $Platform to "macos"
    Con::setVariable("$Platform", "macos");
    
    // Initialize standard libraries (namespaces)
    Video::init();
    Input::init();
    
    // Initialize OS X specific libraries and services
    
    Con::printSeparator();
}

//-----------------------------------------------------------------------------
// OS X processing
void Platform::process()
{
}

//-----------------------------------------------------------------------------
// Shuts down the OS X platform layer code
void Platform::shutdown()
{
    Input::destroy();
    Video::destroy();
}

//-----------------------------------------------------------------------------
// Completely closes and restarts the simulation
void Platform::restartInstance()
{
    // Returns the NSBundle that corresponds to the directory where the current app executable is located.
    NSBundle* mainAppBundle = [NSBundle mainBundle];
    
    // Returns the file URL of the receiver's executable file.
    // Not currently used, but left here for reference
    //NSURL* execURL = [mainAppBundle executableURL];
    
    // Returns the full pathname of the receiver's executable file.
    NSString* execString = [mainAppBundle executablePath];
    
    // Create a mutable string we can build into an executable command
    NSMutableString* mut = [[[NSMutableString alloc] init] autorelease];
    
    // Base string is the executable path
    [mut appendString:execString];
    
    // append ampersand so that we can launch without blocking.
    // encase in quotes so that spaces in the path are accepted.
    [mut insertString:@"\"" atIndex:0];
    [mut appendString:@"\" & "];
    [mut appendString:@"\\0"];
    
    // Convert to a C string
    const char* execCString = [mut UTF8String];
    
    // Echo the command before we run it
    Con::printf("---- %s -----", execCString);
    
    // Run the restart command and hope for the best
    system(execCString);
}

//-----------------------------------------------------------------------------
// Starts the quit process for the main Game instance
void Platform::postQuitMessage(const U32 in_quitVal)
{
    Event quitEvent;
    quitEvent.type = QuitEventType;
    
    Game->postEvent(quitEvent);
}

//-----------------------------------------------------------------------------
// Kicks off the termination process for the NSApp
void Platform::forceShutdown(S32 returnValue)
{
    // Note: AppCode states terminate is deprecated, which is not true
    [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}

//-----------------------------------------------------------------------------
void Platform::debugBreak()
{
    raise(SIGTRAP);
}

//-----------------------------------------------------------------------------

void Platform::outputDebugString(const char *string)
{
    fprintf(stderr, string, NULL );
    fprintf(stderr, "\n" );
    fflush(stderr);
}
