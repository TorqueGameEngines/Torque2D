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


#include "platformiOS/platformiOS.h"
#include "platform/platformVideo.h"
#include "platformiOS/iOSOGLVideo.h"
#include "platformiOS/iOSConsole.h"
#include "platform/platformInput.h"
#include "game/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformiOS/iOSEvents.h"
#include "platform/threads/thread.h"
#include "platformiOS/iOSOutlineGL.h"

#include "platformiOS/iOSWindow.h"
#import <OpenGLES/EAGLDrawable.h>

#include "platformiOS/platformGL.h"
#import <GLKit/GLKit.h>

bool setScreenOrientation(bool, bool);
bool getStatusBarHidden();
bool setStatusBarHidden(bool);
void setStatusBarType(S32);

//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
iOSPlatState platState;

iOSPlatState::iOSPlatState()
{


    captureDisplay = true;
    fadeWindows = true;
    backgrounded = false;
    minimized = false;

    quit = false;

    portrait = true;//-Mat iOS is in portrait mode by default


    // start with something reasonable.
    desktopBitsPixel = IOS_DEFAULT_RESOLUTION_BIT_DEPTH;
    desktopWidth = IOS_DEFAULT_RESOLUTION_X;
    desktopHeight = IOS_DEFAULT_RESOLUTION_Y;
    fullscreen = true;

    osVersion = 0;

    dStrcpy(appWindowTitle, "iOS Torque Game Engine");

    // Semaphore for alerts. We put the app in a modal state by blocking the main
    alertSemaphore = Semaphore::createSemaphore(0);

    // directory that contains main.cs . This will help us detect whether we are
    // running with the scripts in the bundle or not.
    mainDotCsDir = NULL;

    mainLoopTimer = NULL;
}


//------------------------------------------------------------------------------
// DGL, the Gui, and TS use this for various purposes.
const Point2I &Platform::getWindowSize()
{
    return platState.windowSize;
}


//------------------------------------------------------------------------------
// save the window size, for DGL's use
void Platform::setWindowSize(U32 newWidth, U32 newHeight)
{
    platState.windowSize.set(newWidth, newHeight);
}

//------------------------------------------------------------------------------
// Issue a minimize event. The standard handler will handle it.
void Platform::minimizeWindow()
{
    //no minimizing on iOS
}

void Platform::restoreWindow()
{
    //no minimizing on iOS
}

//------------------------------------------------------------------------------
void Platform::setWindowTitle(const char *title)
{
    //no window titles on iOS
}



#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
    // set the Platform to iOS
    Con::setVariable("$platform", "iOS");
    
    // Calculate the size of the screen
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    CGFloat screenScale = [[UIScreen mainScreen] scale];
    
    // Set the screen size to a variable
    Con::setFloatVariable("$pref::iOS::Width", screenBounds.size.width * screenScale);
    Con::setFloatVariable("$pref::iOS::Height", screenBounds.size.height * screenScale);
    
    // Set RetinaEnabled and the Scale
    // NOTE: I think we could get rid of RetinaEnabled through out and just multiply directly by the scale or use RetinaScale > 1 when needed because with iPhone 6 Plus the screenScale is 3.
    Con::setBoolVariable("$pref::iOS::RetinaEnabled", false);
    Con::setBoolVariable("$pref::iOS::RetinaScale", screenScale);
    
    // Determine the Aspect Ratio and set the device type from that
    float resolutionWidth = Con::getFloatVariable("$pref::iOS::Width");
    float resolutionHeight = Con::getFloatVariable("$pref::iOS::Height");
    
    // NOTE: This does not work properly for portrate orientation yet, but I think the way portrate is handled over all needs a rework.
    float Ratio = resolutionWidth/resolutionHeight;
    
    if (Ratio <= 1.4)// 1: iPad = 4:3
        Con::setIntVariable("$pref::iOS::DeviceType", 1);
    else if (Ratio > 1.4 && Ratio <= 1.6)// 0: iPhone = 3:2
        Con::setIntVariable("$pref::iOS::DeviceType", 0);
    else if (Ratio > 1.6)// 2: iPhone 5 and up = 16:9
        Con::setIntVariable("$pref::iOS::DeviceType", 2);
    // Add more aspcet ratios if needed for future devices

    iOSConsole::create();

    //if ( !iOSConsole::isEnabled() )
    Input::init();

    // allow users to specify whether to capture the display or not when going fullscreen
    Con::addVariable("pref::mac::captureDisplay", TypeBool, &platState.captureDisplay);
    Con::addVariable("pref::mac::fadeWindows", TypeBool, &platState.fadeWindows);

    // create the opengl display device
    DisplayDevice *dev = NULL;
    Con::printf("Video Init:");
    Video::init();
    dev = OpenGLDevice::create();
    if (dev)
        Con::printf("   Accelerated OpenGL display device detected.");
    else
        Con::printf("   Accelerated OpenGL display device not detected.");

    // and now we can install the device.
    Video::installDevice(dev);
    Con::printf("");
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
    setMouseLock(false);
    Video::destroy();
    Input::destroy();
    iOSConsole::destroy();
}


//Hidden by Default. 1 Black Opaque, 2 Black Translucent
S32 gStatusBarType = 0;
bool gStatusBarHidden = true;

//Landscape by default. 0 Landscape, 1 Portrait
S32 gScreenOrientation = 0;
bool gScreenUpsideDown = true;


//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
    // First fetch the values from the prefs.
    U32 iDeviceOrientation = (U32) Con::getIntVariable("$pref::iOS::ScreenOrientation");
    S32 resolutionWidth = Con::getIntVariable("$pref::iOS::Width");
    S32 resolutionHeight = Con::getIntVariable("$pref::iOS::Height");
    
    Point2I startRes;

    if (!iDeviceOrientation)
    {
        startRes.x = resolutionWidth;
        startRes.y = resolutionHeight;
    }
    else
    {
        //portrait, swap width height.
        startRes.x = resolutionHeight;
        startRes.y = resolutionWidth;
    }

    dSprintf(platState.appWindowTitle, sizeof(platState.appWindowTitle), name);

    platState.windowSize.x = startRes.x;
    platState.windowSize.y = startRes.y;

    //Get screen orientation prefs //Based on 0 Landscape, 1 Portrait
    gScreenOrientation = iDeviceOrientation;
    gScreenUpsideDown = Con::getBoolVariable("$pref::iOS::ScreenUpsideDown");

    //Default to landscape, and run into portrait if requested.
    platState.portrait = false;

    if (gScreenOrientation != 0) //fuzzytodo :add a constant
    {
        //Could handle other options here, later.
        platState.portrait = true;
    }

    //We should now have a good windowSize, it will be default if initial size was bad
    T2DView * glView;
    CGRect rect;

    rect.origin.x = 0;
    rect.origin.y = 0;

    rect.size.width = platState.windowSize.x;
    rect.size.height = platState.windowSize.y;

    glView = (T2DView *) platState.Window;
    
    if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] && [[UIScreen mainScreen] scale] == 2)
        glView.contentScaleFactor = [[UIScreen mainScreen] scale];
    
    platState.ctx = glView;
    
    //get status bar pref // 0 Hidden , 1 BlackOpaque , 2 BlackTranslucent
    
    S32 tempType = Con::getIntVariable("$pref::iOS::StatusBarType");
    setStatusBarType(tempType);
    
    //set screen orientation
    setScreenOrientation(platState.portrait, gScreenUpsideDown);
    
    bool fullScreen;
    U32 bpp = Con::getIntVariable("$pref::iOS::ScreenDepth"); //iOS_DEFAULT_RESOLUTION_BIT_DEPTH;
    if (!bpp)
    {
        Con::printf("Default BPP Chosen , $pref::iOS::ScreenDepth was not found.");
        bpp = IOS_DEFAULT_RESOLUTION_BIT_DEPTH;
    }
    
    fullScreen = true;
    //
    DisplayDevice::init();
    
    // this will create a rendering context & window
    bool ok = Video::setDevice("OpenGL", platState.windowSize.x, platState.windowSize.y, bpp, fullScreen);
    if (!ok)
    {
        AssertFatal( false, "Could not find a compatible display device!" );
    }
    
    //Luma:	Clear frame buffer to BLACK to start with
    //NOTE:	This should probably be set by the user to be the color closest to Default.png in order to minimize any popping effect... $pref:: anyone? Are $pref::s even valid at this point in the Init process?
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//--------------------------------------
// run app function: not applicable to iOS
//--------------------------------------
// run other apps
int runApp(const char *fileName, const char *batchArgs, bool blocking = false)
{
    return 0;
}

bool appIsRunning(int batchId)
{
    return false;
}

bool Platform::openWebBrowser(const char *webAddress)
{
    NSString *string = [[NSString alloc] initWithUTF8String:webAddress];
    NSURL *url = [[NSURL alloc] initWithString:string];
    bool ret = [platState.application openURL:url];

    return ret;// this bails on the application, switching to Safari
}

bool isStatusBarHidden()
{
    if (platState.application.statusBarHidden == YES)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool setStatusBarHidden(bool hidden)
{
    if (hidden)
    {
        platState.application.statusBarHidden = YES;
        gStatusBarHidden = true;

        return true;
    }
    else
    {
        platState.application.statusBarHidden = NO;
        gStatusBarHidden = false;

        return false;
    }
}

void setStatusBarType(S32 type)
{
    switch (type)
    {
        case 0: //Hidden
            setStatusBarHidden(true);
            break;
        case 1: //Black Opaque
            platState.application.statusBarStyle = UIStatusBarStyleBlackOpaque;
            setStatusBarHidden(false);
            break;
        case 2: //Black Transparent
            platState.application.statusBarStyle = UIStatusBarStyleBlackTranslucent;
            setStatusBarHidden(false);
            break;
        default:
            platState.application.statusBarStyle = UIStatusBarStyleDefault;
    }

    gStatusBarType = type;
}


bool setScreenOrientation(bool portrait, bool upsidedown)
{
    bool success = false;

    CGPoint point;
    
    // Is the iOS version less than 8?
    if( [[[UIDevice currentDevice] systemVersion] compare:@"8.0" options:NSNumericSearch] == NSOrderedAscending )
    {
        
        if (platState.portrait)
        {
            point.x = platState.windowSize.x / 2;
            point.y = platState.windowSize.y / 2;
        }
        else
        {
            point.x = platState.windowSize.y / 2;
            point.y = platState.windowSize.x / 2;
        }
        
        
        [platState.ctx centerOnPoint:point];
        
        if (portrait)
        {//normal upright
            if (upsidedown)
            {//button on top
                [platState.ctx rotateToAngle:M_PI + (M_PI / 2.0)];//rotate to 90 degrees
                platState.application.statusBarOrientation = UIInterfaceOrientationPortraitUpsideDown;
                success = true;
            } else
            {//button on bottom
                [platState.ctx rotateToAngle:(M_PI / 2.0)];//rotate to 270 degrees
                platState.application.statusBarOrientation = UIInterfaceOrientationPortrait;
                success = true;
            }
        } else
        {//landscape/ sideways
            if (upsidedown)
            {//button on left
                [platState.ctx rotateToAngle:0];//rotate to -180 (0) degrees
                platState.application.statusBarOrientation = UIInterfaceOrientationLandscapeLeft;
                success = true;
            } else
            {//button on right
                [platState.ctx rotateToAngle:(M_PI)];//rotate to 180 degrees
                platState.application.statusBarOrientation = UIInterfaceOrientationLandscapeRight;
                success = true;
            }
        }
    }
    //Set the screen for iOS 8 and latter
    else
    {
        point.x = platState.windowSize.x / 2;
        point.y = platState.windowSize.y / 2;
    }
    
    return success;
}

ConsoleFunction(setScreenOrientation, bool, 3, 3, "Sets the orientation of the screen ( portrait/landscape, upside down or right-side up )\n"
        "@(bool portrait, bool upside_down)"){
    return setScreenOrientation(dAtob(argv[1]), dAtob(argv[2]));
}


ConsoleFunction(getStatusBarHidden, bool, 1, 1, " Checks whether the status bar is hidden\n"
        "@return Returns true if hidden and false if not"){
    return isStatusBarHidden();
}

ConsoleFunction(setStatusBarHidden, bool, 2, 2, " Hides/unhides the iOS status bar \n"
        "@return true == status bar is hidden, false == status bar is visible"){
    return setStatusBarHidden(dAtob(argv[1]));
}

ConsoleFunction(setStatusBarType, void, 2, 2, " Set the status bar type. 0 hidden, 1 Black Opaque, 2 Black Translucent \n"){
    return setStatusBarType(dAtoi(argv[1]));
}
