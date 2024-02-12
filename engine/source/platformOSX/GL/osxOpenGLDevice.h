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
#ifndef _OSXOPENGLDEVICE_H_
#define _OSXOPENGLDEVICE_H_

#include "platform/platformVideo.h"

class osxOpenGLDevice : public DisplayDevice
{
private:
    /// Gamma value
    F32 mGamma;
            
    /// Handles to all the devices ( monitors ) attached to the system.
    Vector<CGDirectDisplayID> mMonitorList;
    
    CGDirectDisplayID allDevs[32];
    U32 nAllDevs;
    
    /// Fills mResolutionList with a list of valid resolutions for a particular screen
    bool enumDisplayModes(CGDirectDisplayID hDevice);
    
    /// Fills mMonitorList with all available monitors
    bool enumMonitors();
    
    /// Chooses a monitor based on $pref::the results of enumMontors()
    CGDirectDisplayID chooseMonitor();
    
    osxPlatState * platState;

public:
    osxOpenGLDevice();
    
    static DisplayDevice* create();
    
    /// The following are inherited from DisplayDevice
    void initDevice();
    
    bool cleanUpContext();
    
    bool activate( U32 width, U32 height, U32 bpp, bool fullScreen );
    
    void shutdown();
    
    NSOpenGLPixelFormat* generateValidPixelFormat(bool fullscreen, U32 bpp, U32 samples);
    
    bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true );
    
    void swapBuffers();
    
    const char* getDriverInfo();
    
    bool getGammaCorrection(F32 &g);
    
    bool setGammaCorrection(F32 g);
    
    bool getVerticalSync();
    
    bool setVerticalSync( bool sync );
    
};

#endif // _OSXOPENGLDEVICE_H_
