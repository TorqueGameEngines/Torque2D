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
#import "platformOSX/osxOpenGLDevice.h"
#import "platformOSX/platformGL.h"
#include "console/console.h"
#include "game/gameInterface.h"
#include "graphics/dgl.h"

//-----------------------------------------------------------------------------

osxOpenGLDevice::osxOpenGLDevice()
{
    mDeviceName = "OpenGL";
    mFullScreenOnly = false;

    // pick a monitor to run on
    enumMonitors();

    platState = [osxPlatState sharedPlatState];

    CGDirectDisplayID display = chooseMonitor();

    [platState setCgDisplay:display];

    enumDisplayModes(display);
}

//------------------------------------------------------------------------------

bool osxOpenGLDevice::enumDisplayModes( CGDirectDisplayID display )
{
    // Clear the resolution list.
    mResolutionList.clear();

    // Fetch a list of all available modes for the specified display.
    CFArrayRef modeArray = CGDisplayCopyAllDisplayModes(display, NULL);

    // Fetch the mode count.
    const S32 modeCount = (const S32)CFArrayGetCount(modeArray);

    // Iterate the modes.
    for( S32 modeIndex = 0; modeIndex < modeCount; modeIndex++ )
    {
        // Fetch the display mode.
        CGDisplayModeRef mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modeArray, modeIndex);

        // Get the mode width.
        const S32 width = (const S32)CGDisplayModeGetWidth(mode);

        // Get the mode height.
        const S32 height = (const S32)CGDisplayModeGetHeight(mode);

        // Get the pixel encoding.
        CFStringRef pixelEncoding = CGDisplayModeCopyPixelEncoding(mode);

        // Is it a 32 bpp?
        S32 bitDepth;
        if ( CFStringCompare( pixelEncoding, CFSTR(IO32BitDirectPixels), 0 ) == kCFCompareEqualTo )
        {
            bitDepth = 32;
        }
        else if ( CFStringCompare( pixelEncoding, CFSTR(IO16BitDirectPixels), 0 ) == kCFCompareEqualTo )
        {
            bitDepth = 16;
        }
        else
        {
            // Skip the mode.
            continue;
        }

        // Prepare the resolution.
        Resolution foundResolution( width, height, bitDepth );

        // Ensure this isn't already in the list.
        bool alreadyPresent = false;
        for( Vector<Resolution>::iterator itr = mResolutionList.begin(); itr != mResolutionList.end(); ++itr )
        {
            if ( *itr == foundResolution )
            {
                alreadyPresent = true;
                break;
            }
        }

        // Skip if already present.
        if ( alreadyPresent )
            continue;

        // Store the resolution.
        mResolutionList.push_back( Resolution( width, height, bitDepth ) );
    }

    return true;
}

//-----------------------------------------------------------------------------
// Unused for new OS X platform. The constructor handles initialization now
void osxOpenGLDevice::initDevice()
{
}

//-----------------------------------------------------------------------------
// This will fully clear the OpenGL context
bool osxOpenGLDevice::cleanUpContext()
{
    bool needResurrect = false;

    platState = [osxPlatState sharedPlatState];

    if ([[platState torqueView] contextInitialized])
    {
        if (!Video::smNeedResurrect)
        {
            Con::printf( "Killing the texture manager..." );
            Game->textureKill();
            needResurrect = true;
        }

        [[platState torqueView] clearContext];
    }

    // clear the Resolution state, so setScreenMode() will know not to early-out.
    smCurrentRes = Resolution(0, 0, 0);

    return needResurrect;
}

//-----------------------------------------------------------------------------
//
bool osxOpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
    Con::printf( " OpenGLDevice activating..." );

    // gets opengl rendering capabilities of the screen pointed to by platState.hDisplay
    // sets up dgl with the capabilities info, & reports opengl status.
    getGLCapabilities();

    // Create the window or capture fullscreen
    if(!setScreenMode(width, height, bpp, fullScreen, true, false))
        return false;

    // set the displayDevice pref to "OpenGL"
    Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

    // set vertical sync now because it doesnt need setting every time we setScreenMode()
    setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ));

    return true;
}

//-----------------------------------------------------------------------------

void osxOpenGLDevice::shutdown()
{
    Con::printf( "Shutting down the OpenGL display device..." );
    cleanUpContext();
}

//-----------------------------------------------------------------------------

NSOpenGLPixelFormat* osxOpenGLDevice::generateValidPixelFormat(bool fullscreen, U32 bpp, U32 samples)
{
    AssertWarn(samples <= 6, "An unusual multisample depth was requested in findValidPixelFormat(). clamping to 0...6");

    samples = samples > 6 ? 6 : samples;

    int i = 0;
    NSOpenGLPixelFormatAttribute attr[64];

    attr[i++] = NSOpenGLPFADoubleBuffer;
    attr[i++] = NSOpenGLPFANoRecovery;
    attr[i++] = NSOpenGLPFAAccelerated;

    if(bpp != 0)
    {
        // native pixel formats are argb 1555 & argb 8888.
        U32 colorbits = 0;
        U32 alphabits = 0;

        if(bpp == 16)
        {
            colorbits = 5;             // ARGB 1555
            alphabits = 1;
        }
        else if(bpp == 32)
            colorbits = alphabits = 8; // ARGB 8888

        attr[i++] = NSOpenGLPFADepthSize;
        attr[i++] = (NSOpenGLPixelFormatAttribute)bpp;
        attr[i++] = NSOpenGLPFAColorSize;
        attr[i++] = (NSOpenGLPixelFormatAttribute)colorbits;
        attr[i++] = NSOpenGLPFAAlphaSize;
        attr[i++] = (NSOpenGLPixelFormatAttribute)alphabits;
    }

    if (samples != 0)
    {
        attr[i++] = NSOpenGLPFAMultisample;
        attr[i++] = (NSOpenGLPixelFormatAttribute)1;
        attr[i++] = NSOpenGLPFASamples;
        attr[i++] = (NSOpenGLPixelFormatAttribute)samples;
    }

    attr[i++] = 0;

    NSOpenGLPixelFormat* format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attr] autorelease];

    return format;
}

//-----------------------------------------------------------------------------

bool osxOpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{
    // Print to the console that we are setting the screen mode
    Con::printf(" set screen mode %i x %i x %i, %s, %s, %s",width, height, bpp,
                fullScreen  ? "fullscreen" : "windowed",
                forceIt     ? "force it" : "dont force it",
                repaint     ? "repaint"  : "dont repaint");

    bool needResurrect = cleanUpContext();

    // Get the global OSX platform state
    osxPlatState * platState = [osxPlatState sharedPlatState];

    // Validation, early outs
    // Sanity check. Some scripts are liable to pass in bad values.
    if (!bpp)
        bpp = [platState desktopBitsPixel];

    if (bpp)
        bpp = bpp > 16 ? 32 : 16;

    Resolution newRes = Resolution(width, height, bpp);

    // If no values changing and we're not forcing a change, kick out. prevents thrashing.
    if (!forceIt && smIsFullScreen == fullScreen && smCurrentRes == newRes)
        return true;

    // Create a pixel format to be used with the context
    NSOpenGLPixelFormat* pixelFormat = generateValidPixelFormat(fullScreen, bpp, 0);

    if (!pixelFormat)
    {
        Con::printf("osxOpenGLDevice::setScreenMode error: No OpenGL pixel format");
        return false;
    }

    [platState setFullScreen:fullScreen];

    if (fullScreen)
    {

        NSRect mainDisplayRect = [[NSScreen mainScreen] frame];

        newRes.w = mainDisplayRect.size.width;
        newRes.h = mainDisplayRect.size.height;

        [[platState window] setStyleMask:NSBorderlessWindowMask];

        [[platState window] setFrame:mainDisplayRect display:YES];

        [[platState window] setLevel:NSMainMenuWindowLevel+1];
    }
    else
    {
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1070
        [[platState window] setStyleMask:NSTitledWindowMask | NSClosableWindowMask] ;

        // Calculate the actual center
        CGFloat x = ([[NSScreen mainScreen] frame].size.width - width) / 2;
        CGFloat y = ([[NSScreen mainScreen] frame].size.height - height) / 2;

        // Create a rect to send to the window
        NSRect newFrame = NSMakeRect(x, y, width, height);

        // Send message to the window to resize/relocate
        [[platState window] setFrame:newFrame display:YES animate:NO];
#else
        [[platState window] setStyleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask];
#endif
    }

    [[platState torqueView] createContextWithPixelFormat:pixelFormat];

    [platState setWindowSize:newRes.w height:newRes.h];

    // clear out garbage from the gl window.
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT );

    // set opengl options & other options ---------------------------------------
    // ensure data is packed tightly in memory. this defaults to 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // TODO: set gl arb multisample enable & hint
    //dglSetFSAASamples(gFSAASamples);

    // update smIsFullScreen and pref
    smIsFullScreen = fullScreen;

    Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );

    // save resolution
    smCurrentRes = newRes;

    // save resolution to prefs
    char buf[32];
    if (fullScreen)
    {
        dSprintf( buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
        Con::setVariable("$pref::Video::resolution", buf);
    }
    else
    {
        dSprintf( buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
        Con::setVariable("$pref::Video::windowedRes", buf);
    }

    if (needResurrect)
    {
        // Reload the textures gl names
        Con::printf( "Resurrecting the texture manager..." );
        Game->textureResurrect();
    }

    if( repaint )
        Video::resetCanvas();

    return true;
}

//-----------------------------------------------------------------------------

void osxOpenGLDevice::swapBuffers()
{
    if ([[platState torqueView] contextInitialized])
        [[platState torqueView] flushBuffer];

#if defined(TORQUE_DEBUG)
    if (gOutlineEnabled)
        glClear(GL_COLOR_BUFFER_BIT);
#endif

}

//-----------------------------------------------------------------------------

const char* osxOpenGLDevice::getDriverInfo()
{
    // Prepare some driver info for the console:
    const char* vendorString   = (const char*) glGetString( GL_VENDOR );
    const char* rendererString = (const char*) glGetString( GL_RENDERER );
    const char* versionString  = (const char*) glGetString( GL_VERSION );
    const char* extensionsString = (const char*) glGetString( GL_EXTENSIONS );

    U32 bufferLen = ( vendorString ? dStrlen( vendorString ) : 0 )
    + ( rendererString ? dStrlen( rendererString ) : 0 )
    + ( versionString  ? dStrlen( versionString ) : 0 )
    + ( extensionsString ? dStrlen( extensionsString ) : 0 )
    + 4;

    char* returnString = Con::getReturnBuffer( bufferLen );
    dSprintf( returnString, bufferLen, "%s\t%s\t%s\t%s",
             ( vendorString ? vendorString : "" ),
             ( rendererString ? rendererString : "" ),
             ( versionString ? versionString : "" ),
             ( extensionsString ? extensionsString : "" ) );

    return( returnString );
}

//-----------------------------------------------------------------------------
// Not yet implemented. Will resolve in the next video update
bool osxOpenGLDevice::getGammaCorrection(F32 &g)
{
    return false;
}

//-----------------------------------------------------------------------------
// Not yet implemented. Will resolve in the next video update
bool osxOpenGLDevice::setGammaCorrection(F32 g)
{
    return false;
}

//-----------------------------------------------------------------------------

bool osxOpenGLDevice::getVerticalSync()
{
    if (!gGLState.suppSwapInterval)
    {
        return false;
    }

    //Note that this returns the number of frames between Swaps.
    //The function returns 0 / false if SwapInterval has not been specified.
    return false;//getVerticalSync();
}

//-----------------------------------------------------------------------------

bool osxOpenGLDevice::setVerticalSync( bool sync )
{
    if ([[platState torqueView] contextInitialized])
    {
        [[platState torqueView] setVerticalSync:sync];
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
//  Fill mMonitorList with list of supported modes
//   Guaranteed to include at least the main device.
//------------------------------------------------------------------------------
bool osxOpenGLDevice::enumMonitors()
{
    mMonitorList.clear();
    nAllDevs = 0;

    CGDirectDisplayID _displayIDs[32];
    uint32_t _displayCount;

    CGGetActiveDisplayList (32, _displayIDs, &_displayCount);

    for (int ii = 0 ; ii < _displayCount ; ii++)
    {
        mMonitorList.push_back(_displayIDs[ii]);
        allDevs[nAllDevs++] = _displayIDs[ii];
    }

    return true;
}

//------------------------------------------------------------------------------
// Chooses a monitor based on $pref, on the results of tors(), & on the
// current window's screen.
//------------------------------------------------------------------------------
CGDirectDisplayID osxOpenGLDevice::chooseMonitor()
{
    // TODO: choose monitor based on which one contains most of the window.
    // NOTE: do not call cleanup before calling choose, or we won't have a window to consider.
    AssertFatal(!mMonitorList.empty(), "Cannot choose a monitor if the list is empty!");

    U32 monNum = Con::getIntVariable("$pref::Video::monitorNum", 0);

    if (monNum >= mMonitorList.size())
    {
        Con::errorf("invalid monitor number %i", monNum);
        monNum = 0;
        Con::setIntVariable("$pref::Video::monitorNum", 0);
    }

    Con::printf("using display 0x%x", mMonitorList[monNum]);

    return mMonitorList[monNum];
}
