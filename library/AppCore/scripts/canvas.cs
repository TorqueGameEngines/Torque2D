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

//------------------------------------------------------------------------------
// initializeCanvas
// Constructs and initializes the default canvas window.
//------------------------------------------------------------------------------
function AppCore::initializeCanvas(%this, %windowName)
{
    // Don't duplicate the canvas.
    if(!isObject(Canvas))
    {
	    videoSetGammaCorrection($pref::OpenGL::gammaCorrection);

	    if ( !createCanvas(%windowName) )
	    {
	        error("Canvas creation failed. Shutting down.");
	        quit();
	    }

	    if ($platform $= "iOS")
	    {
	        %resolution = $pref::iOS::Width SPC $pref::iOS::Height SPC $pref::iOS::ScreenDepth;
	    }
	    else if ($platform $= "Android")
	    {
	    	%resolution = GetAndroidResolution();
	    }
	    else
	    {
	        if ( $pref::Video::windowedRes !$= "" )
	            %resolution = $pref::Video::windowedRes;
	        else
	            %resolution = $pref::Video::defaultResolution;
	    }

	    if ($platform $= "windows" || $platform $= "macos")
	    {
	        setScreenMode( %resolution._0, %resolution._1, %resolution._2, $pref::Video::fullScreen );
	    }
	    else
	    {
	        setScreenMode( %resolution._0, %resolution._1, %resolution._2, false );
	    }
	}
	else
	{
		Canvas.setCanvasTitle(%windowName);
		Canvas.repaint();
	}
    Canvas.UseBackgroundColor = true;
    Canvas.BackgroundColor = "Black";
}

//------------------------------------------------------------------------------
// iOSResolutionFromSetting
// Helper function that grabs resolution strings based on device type
//------------------------------------------------------------------------------
function AppCore::iOSResolutionFromSetting( %this, %deviceType, %deviceScreenOrientation )
{
    // A helper function to get a string based resolution from the settings given.
    %x = 0;
    %y = 0;

    %scaleFactor = $pref::iOS::RetinaEnabled ? 2 : 1;

    switch(%deviceType)
    {
        case $iOS::constant::iPhone:
            if(%deviceScreenOrientation == $iOS::constant::Landscape)
            {
                %x =  $iOS::constant::iPhoneWidth * %scaleFactor;
                %y =  $iOS::constant::iPhoneHeight * %scaleFactor;
            }
            else
            {
                %x =  $iOS::constant::iPhoneHeight * %scaleFactor;
                %y =  $iOS::constant::iPhoneWidth * %scaleFactor;
            }

        case $iOS::constant::iPad:
            if(%deviceScreenOrientation == $iOS::constant::Landscape)
            {
                %x =  $iOS::constant::iPadWidth * %scaleFactor;
                %y =  $iOS::constant::iPadHeight * %scaleFactor;
            }
            else
            {
                %x =  $iOS::constant::iPadHeight * %scaleFactor;
                %y =  $iOS::constant::iPadWidth * %scaleFactor;
            }

        case $iOS::constant::iPhone5:
            if(%deviceScreenOrientation == $iOS::constant::Landscape)
            {
                %x =  $iOS::constant::iPhone5Width;
                %y =  $iOS::constant::iPhone5Height;
            }
            else
            {
                %x =  $iOS::constant::iPhone5Height;
                %y =  $iOS::constant::iPhone5Width;
            }
    }

    return %x @ " " @ %y;
}
