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

#include "platform/platformVideo.h"
#include "gui/guiCanvas.h"
#include "console/console.h"
#include "game/gameInterface.h"

#include "platformVideo_ScriptBinding.h"

extern void GameDeactivate( bool noRender );
extern void GameReactivate();
#ifdef TORQUE_OS_ANDROID
extern int _AndroidGetScreenWidth();
extern int _AndroidGetScreenHeight();
#endif
// Static class data:
Vector<DisplayDevice*>  Video::smDeviceList;
DisplayDevice*          Video::smCurrentDevice;
bool					Video::smCritical = false;
bool					Video::smNeedResurrect = false;

Resolution  DisplayDevice::smCurrentRes;
bool        DisplayDevice::smIsFullScreen;

//------------------------------------------------------------------------------
void Video::init()
{
    Con::printSeparator();

    Con::printf("Video initialization:");

    destroy();
}

//------------------------------------------------------------------------------
void Video::destroy()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      smCurrentDevice->shutdown();
      smCritical = false;
   }

   smCurrentDevice = NULL;

   for ( U32 i = 0; i < (U32)smDeviceList.size(); i++ )
      delete smDeviceList[i];

   smDeviceList.clear();
}


//------------------------------------------------------------------------------
bool Video::installDevice( DisplayDevice *dev )
{
   if ( dev )
   {
      smDeviceList.push_back( dev );
      return true;
   }
   return false;
}


//------------------------------------------------------------------------------
bool Video::setDevice( const char *renderName, U32 width, U32 height, U32 bpp, bool fullScreen )
{
   S32 deviceIndex = NO_DEVICE;
    S32 iOpenGL = -1;
    S32 iD3D = -1;

   bool bOpenglRender   = true; //(bool)(dStricmp(renderName,"OpenGL") == 0);
   bool bD3DRender      = false; //(bool)(dStricmp(renderName,"D3D") == 0);
   bool bAllowD3D       = false; //Con::getBoolVariable("$pref::Video::allowD3D");
   bool bAllowOpengl    = true; //Con::getBoolVariable("$pref::Video::allowOpenGL");


   for ( S32 i = 0; i < smDeviceList.size(); i++ )
   {
      if ( dStrcmp( smDeviceList[i]->mDeviceName, renderName ) == 0 )
         deviceIndex = i;

        if ( dStrcmp( smDeviceList[i]->mDeviceName, "OpenGL" ) == 0 )
            iOpenGL = i;
        if ( dStrcmp( smDeviceList[i]->mDeviceName, "D3D" ) == 0 )
            iD3D = i;
   }

   if ( deviceIndex == NO_DEVICE )
   {
      Con::warnf( ConsoleLogEntry::General, "\"%s\" display device not found!", renderName );
      return false;
   }

   // Change the display device:
   if ( smDeviceList[deviceIndex] == NULL )
      return false;


   if (smCurrentDevice && smCurrentDevice != smDeviceList[deviceIndex])
   {
      Con::printf( "Deactivating the previous display device..." );
      Game->textureKill();
      smNeedResurrect = true;
      smCurrentDevice->shutdown();
   }
   if (iOpenGL != -1 && !bAllowOpengl )
   {
      // change to D3D, delete OpenGL in the recursive call
      if ( bOpenglRender )
      {
         U32 w, h, d;

         if( fullScreen )
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("D3D",w,h,d,fullScreen);
      }
      else
      {
         delete smDeviceList[iOpenGL];
         smDeviceList.erase(iOpenGL);
      }
   }
   else if (iD3D != -1 && !bAllowD3D )
   {
      // change to OpenGL, delete D3D in the recursive call
      if ( bD3DRender )
      {
         U32 w, h, d;
         if( fullScreen )
            dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
         else
            dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);

         return setDevice("OpenGL",w,h,d,fullScreen);
      }
      else
      {
         delete smDeviceList[iD3D];
         smDeviceList.erase(iD3D);
      }
   }
   else if (iD3D != -1 && bOpenglRender &&
      !Con::getBoolVariable("$pref::Video::preferOpenGL") &&
      !Con::getBoolVariable("$pref::Video::appliedPref"))
   {
      U32 w, h, d;
      if( fullScreen )
         dSscanf(Con::getVariable("$pref::Video::resolution"), "%d %d %d", &w, &h, &d);
      else
         dSscanf(Con::getVariable("$pref::Video::windowedRes"), "%d %d %d", &w, &h, &d);
      Con::setBoolVariable("$pref::Video::appliedPref", true);

      return setDevice("D3D",w,h,d,fullScreen);
   }
   else
      Con::setBoolVariable("$pref::Video::appliedPref", true);

   Con::printf( "Activating the %s display device...", renderName );
   smCurrentDevice = smDeviceList[deviceIndex];

   smCritical = true;
   bool result = smCurrentDevice->activate( width, height, bpp, fullScreen );
   smCritical = false;

   if ( result )
   {
      if (smNeedResurrect)
      {
         Game->textureResurrect();
         smNeedResurrect = false;
      }
      resetCanvas();
   }

    // Show Maximum Texture Size reported by the graphics hardware.
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if ( maxTextureSize > 0 )
        Con::printf("Max Texture Size reported as: %d", maxTextureSize);
    else
        Con::warnf("Max Texture Size reported as: %d !", maxTextureSize);

   // The video mode activate may have failed above, return that status
   return( result );
}

void Video::resetCanvas()
{
    GuiCanvas* Canvas = Sim::findObject<GuiCanvas>("Canvas");
    if (Canvas != NULL)
    {
        Canvas->paint();
    }
}

extern bool retinaEnabled;

//------------------------------------------------------------------------------
bool Video::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   if ( smCurrentDevice )
   {
       //MIN_RESOLUTION defined in platformWin32/platformGL.h
#ifdef TORQUE_OS_IOS
      if(width == 0)
         width = IOS_DEFAULT_RESOLUTION_X;
      if(height == 0)
         height = IOS_DEFAULT_RESOLUTION_Y;
      if(bpp == 0)
         bpp = IOS_DEFAULT_RESOLUTION_BIT_DEPTH;

      //if(retinaEnabled)
      //{
      //    width *=2;
      //    height *=2;
      //}

#elif TORQUE_OS_ANDROID
      if(width == 0)
	     width = _AndroidGetScreenWidth();
	  if(height == 0)
	     height = _AndroidGetScreenHeight();
	  if(bpp == 0)
	     bpp = ANDROID_DEFAULT_RESOLUTION_BIT_DEPTH;
#else
      if(width == 0)
         width = MIN_RESOLUTION_X;
      if(height == 0)
         height = MIN_RESOLUTION_Y;
      if(bpp == 0)
         bpp = MIN_RESOLUTION_BIT_DEPTH;
#endif// TORQUE_OS_IOS

      smCritical = true;
      bool result = smCurrentDevice->setScreenMode( width, height, bpp, fullScreen );
    smCritical = false;

       return( result );
   }

   return( false );
}


//------------------------------------------------------------------------------
void Video::deactivate( bool force )
{
   if ( smCritical ) return;

   bool doDeactivate = force ? true : DisplayDevice::isFullScreen();

   Game->gameDeactivate( doDeactivate );
   if ( smCurrentDevice && doDeactivate )
   {
       smCritical = true;

      Game->textureKill();
      smCurrentDevice->shutdown();

      Platform::minimizeWindow();
      smCritical = false;
   }
}

//------------------------------------------------------------------------------
void Video::reactivate( bool force )
{
   if ( smCritical ) return;

   bool doReactivate = force ? true : DisplayDevice::isFullScreen();

   if ( smCurrentDevice && doReactivate )
   {
       Resolution res = DisplayDevice::getResolution();

      smCritical = true;
      smCurrentDevice->activate(res.w,res.h,res.bpp,DisplayDevice::isFullScreen());
       Game->textureResurrect();

       smCritical = false;
      if (sgOriginalGamma != -1.0)
        Video::setGammaCorrection(sgOriginalGamma + sgGammaCorrection);
   }
   Game->gameReactivate();
}


//------------------------------------------------------------------------------
bool Video::setResolution( U32 width, U32 height, U32 bpp )
{
   if ( smCurrentDevice )
   {
      if ( bpp == 0 )
         bpp = DisplayDevice::getResolution().bpp;

      smCritical = true;
      bool result = smCurrentDevice->setResolution( width, height, bpp );
      smCritical = false;

      return( result );
   }
   return( false );
}


//------------------------------------------------------------------------------
bool Video::toggleFullScreen()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->toggleFullScreen();
      smCritical = false;

      return( result );
   }
   return( false );
}


//------------------------------------------------------------------------------
DisplayDevice* Video::getDevice( const char* renderName )
{
   for ( S32 i = 0; i < smDeviceList.size(); i++ )
   {
      if ( dStrcmp( smDeviceList[i]->mDeviceName, renderName ) == 0 )
            return( smDeviceList[i] );
   }

    return( NULL );
}


//------------------------------------------------------------------------------
bool Video::prevRes()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->prevRes();
      smCritical = false;

      return( result );
   }
   return( false );
}


//------------------------------------------------------------------------------
bool Video::nextRes()
{
   if ( smCurrentDevice )
   {
      smCritical = true;
      bool result = smCurrentDevice->nextRes();
      smCritical = false;

      return( result );
   }
   return( false );
}


//------------------------------------------------------------------------------
Resolution Video::getResolution()
{
   return DisplayDevice::getResolution();
}


//------------------------------------------------------------------------------
const char* Video::getDeviceList()
{
    U32 deviceCount = smDeviceList.size();
    if ( deviceCount > 0 ) // It better be...
    {
        U32 strLen = 0, i;
        for ( i = 0; i < deviceCount; i++ )
            strLen += ( dStrlen( smDeviceList[i]->mDeviceName ) + 1 );

        char* returnString = Con::getReturnBuffer( strLen );
        dStrcpy( returnString, smDeviceList[0]->mDeviceName );
        for ( i = 1; i < deviceCount; i++ )
        {
            dStrcat( returnString, "\t" );
            dStrcat( returnString, smDeviceList[i]->mDeviceName );
        }

        return( returnString );
    }

    return( NULL );
}


//------------------------------------------------------------------------------
const char* Video::getResolutionList()
{
   if ( smCurrentDevice )
      return smCurrentDevice->getResolutionList();
   else
      return NULL;
}


//------------------------------------------------------------------------------
const char* Video::getDriverInfo()
{
   if ( smCurrentDevice )
      return smCurrentDevice->getDriverInfo();
   else
      return NULL;
}


//------------------------------------------------------------------------------
bool Video::isFullScreen()
{
   return DisplayDevice::isFullScreen();
}


//------------------------------------------------------------------------------
void Video::swapBuffers()
{
   if ( smCurrentDevice )
      smCurrentDevice->swapBuffers();
}


//------------------------------------------------------------------------------
bool Video::getGammaCorrection(F32 &g)
{
   if (smCurrentDevice)
      return smCurrentDevice->getGammaCorrection(g);

   return false;
}


//------------------------------------------------------------------------------
bool Video::setGammaCorrection(F32 g)
{
   if (smCurrentDevice)
      return smCurrentDevice->setGammaCorrection(g);

   return false;
}

//------------------------------------------------------------------------------
bool Video::getVerticalSync()
{
    if (smCurrentDevice)
        return smCurrentDevice->getVerticalSync();

    return( false );
}

//------------------------------------------------------------------------------
bool Video::setVerticalSync( bool on )
{
   if ( smCurrentDevice )
      return( smCurrentDevice->setVerticalSync( on ) );

   return( false );
}

//------------------------------------------------------------------------------
DisplayDevice::DisplayDevice()
{
   mDeviceName = NULL;
}


//------------------------------------------------------------------------------
void DisplayDevice::init()
{
    smCurrentRes = Resolution( 0, 0, 0 );
    smIsFullScreen = false;
}


//------------------------------------------------------------------------------
bool DisplayDevice::prevRes()
{
   U32 resIndex;
   for ( resIndex = mResolutionList.size() - 1; resIndex > 0; resIndex-- )
   {
      if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp
        && mResolutionList[resIndex].w <= smCurrentRes.w
        && mResolutionList[resIndex].h != smCurrentRes.h )
         break;
   }

   if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp )
      return( Video::setResolution( mResolutionList[resIndex].w, mResolutionList[resIndex].h, mResolutionList[resIndex].bpp ) );

    return( false );
}


//------------------------------------------------------------------------------
bool DisplayDevice::nextRes()
{
   U32 resIndex;
   for ( resIndex = 0; resIndex < (U32)mResolutionList.size() - 1; resIndex++ )
   {
      if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp
        && mResolutionList[resIndex].w >= smCurrentRes.w
        && mResolutionList[resIndex].h != smCurrentRes.h )
         break;
   }

   if ( mResolutionList[resIndex].bpp == smCurrentRes.bpp )
      return( Video::setResolution( mResolutionList[resIndex].w, mResolutionList[resIndex].h, mResolutionList[resIndex].bpp ) );

    return( false );
}


//------------------------------------------------------------------------------
// This function returns a string containing all of the available resolutions for this device
// in the format "<bit depth> <width> <height>", separated by tabs.
//
const char* DisplayDevice::getResolutionList()
{
   if (Con::getBoolVariable("$pref::Video::clipHigh", false))
        for (S32 i = mResolutionList.size()-1; i >= 0; --i)
            if (mResolutionList[i].w > 1152 || mResolutionList[i].h > 864)
                mResolutionList.erase(i);

    if (Con::getBoolVariable("$pref::Video::only16", false))
        for (S32 i = mResolutionList.size()-1; i >= 0; --i)
            if (mResolutionList[i].bpp == 32)
                mResolutionList.erase(i);

   U32 resCount = mResolutionList.size();
   if ( resCount > 0 )
   {
      char* tempBuffer = new char[resCount * 15];
      tempBuffer[0] = 0;
      for ( U32 i = 0; i < resCount; i++ )
      {
         char newString[15];
         dSprintf( newString, sizeof( newString ), "%d %d %d\t", mResolutionList[i].w, mResolutionList[i].h, mResolutionList[i].bpp );
         dStrcat( tempBuffer, newString );
      }
      tempBuffer[dStrlen( tempBuffer ) - 1] = 0;

      char* returnString = Con::getReturnBuffer( dStrlen( tempBuffer ) + 1 );
      dStrcpy( returnString, tempBuffer );
      delete [] tempBuffer;

      return returnString;
   }

   return NULL;
}
