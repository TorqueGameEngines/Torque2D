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

ConsoleMethodGroupBeginWithDocs(GuiCanvas, GuiControl)

/*! Use the getContent method to get the ID of the control which is being used as the current canvas content.
    @return Returns the ID of the current canvas content (a control), or 0 meaning the canvas is empty
*/
ConsoleMethodWithDocs( GuiCanvas, getContent, ConsoleInt, 2, 2, ())
{
   GuiControl *ctrl = object->getContentControl();
   if(ctrl)
      return ctrl->getId();
   return -1;
}

/*! Use the setContent method to set the control identified by handle as the current canvas content.
    @param handle The numeric ID or name of the control to be made the canvas contents.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, setContent, ConsoleVoid, 3, 3, ( handle ))
{
   GuiControl *gui = NULL;
   if(argv[2][0])
   {
      if (!Sim::findObject(argv[2], gui))
      {
         Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
         return;
      }
   }

   //set the new content control
   Canvas->setContentControl(gui);
}

/*! Use the pushDialog method to open a dialog on a specific canvas layer, or in the same layer the last openned dialog. Newly placed dialogs placed in a layer with another dialog(s) will overlap the prior dialog(s).
    @param handle The numeric ID or name of the dialog to be opened.
    @param layer A integer value in the range [ 0 , inf ) specifying the canvas layer to place the dialog in.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, pushDialog, ConsoleVoid, 3, 4, ( handle [ , layer ] ))
{
   GuiControl *gui;

   if (!	Sim::findObject(argv[2], gui))
   {
      Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
      return;
   }

   //find the layer
   S32 layer = 0;
   if (argc == 4)
      layer = dAtoi(argv[3]);

   //set the new content control
   Canvas->pushDialogControl(gui, layer);
}

/*! Use the popDialog method to remove a currently showing dialog. If no handle is provided, the top most dialog is popped.
    @param handle The ID or a previously pushed dialog.
    @return No return value.
    @sa pushDialog, popLayer
*/
ConsoleMethodWithDocs( GuiCanvas, popDialog, ConsoleVoid, 2, 3, ( handle ))
{
   // Must initialize this to NULL to avoid crash on the if (gui) statement further down [KNM | 07/28/11 | ITGB-120]
   //GuiControl * gui;
   GuiControl * gui = NULL;
   if (argc == 3)
   {
      if (!Sim::findObject(argv[2], gui))
      {
         Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
         return;
      }
   }

   if (gui)
      Canvas->popDialogControl(gui);
   else
      Canvas->popDialogControl();
}

/*! Use the popLayer method to remove (close) all dialogs in the specified canvas ???layer???.
    @param layer A integer value in the range [ 0 , inf ) specifying the canvas layer to clear.
    @return No return value.
    @sa pushDialog, popDialog
*/
ConsoleMethodWithDocs( GuiCanvas, popLayer, ConsoleVoid, 2, 3, ( layer ))
{
   S32 layer = 0;
   if (argc == 3)
      layer = dAtoi(argv[2]);

   Canvas->popDialogControl(layer);
}

/*! Use the cursorOn method to enable the cursor.
    @return No return value
*/
ConsoleMethodWithDocs(GuiCanvas, cursorOn, ConsoleVoid, 2, 2, ())
{
   Canvas->setCursorON(true);
}

/*! Use the cursorOff method to disable the cursor.
    @return No return value
*/
ConsoleMethodWithDocs(GuiCanvas, cursorOff, ConsoleVoid, 2, 2, ())
{
   Canvas->setCursorON(false);
}

/*! Use the setCursor method to select the current cursor.
    @param cursorHandle The ID of a previously defined GuiCursor object.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, setCursor, ConsoleVoid, 3, 3, ( cursorHandle ))
{
   GuiCursor *curs = NULL;
   if(argv[2][0])
   {
      if(!Sim::findObject(argv[2], curs))
      {
         Con::printf("%s is not a valid cursor.", argv[2]);
         return;
      }
   }
   Canvas->useNativeCursor(false);
   Canvas->setCursor(curs);
   Canvas->showCursor(true);
}

/*! Returns the cursor to the system default.
    @return No return value
*/
ConsoleMethodWithDocs(GuiCanvas, resetCursor, void, 2, 2, ())
{
   Canvas->useNativeCursor(true);
   Canvas->showCursor(false);
   Input::refreshCursor();
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, renderFront, ConsoleVoid, 3, 3, (bool enable))
{
   Canvas->setRenderFront(dAtob(argv[2]));
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, showCursor, ConsoleVoid, 2, 2, ())
{
   Canvas->showCursor(true);
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, hideCursor, ConsoleVoid, 2, 2, ())
{
   Canvas->showCursor(false);
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, isCursorOn, ConsoleBool, 2, 2, ())
{
   return Canvas->isCursorON();
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, setDoubleClickDelay, ConsoleVoid, 3, 3, ())
{
   Canvas->setDoubleClickTime(dAtoi(argv[2]));
}

/*! 
*/
ConsoleMethodWithDocs( GuiCanvas, setDoubleClickMoveBuffer, ConsoleVoid, 4, 4, ())
{
   Canvas->setDoubleClickWidth(dAtoi(argv[2]));
   Canvas->setDoubleClickHeight(dAtoi(argv[3]));
}

/*! Use the repaint method to force the canvas to redraw all elements.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, repaint, ConsoleVoid, 2, 2, ())
{
   Canvas->paint();
}

/*! Use the reset method to reset the current canvas update region.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, reset, ConsoleVoid, 2, 2, ())
{
   Canvas->resetUpdateRegions();
}

/*! Use the getCursorPos method to retrieve the current position of the mouse pointer.
    @return Returns a vector containing the ???x y??? coordinates of the cursor in the canvas
*/
ConsoleMethodWithDocs( GuiCanvas, getCursorPos, ConsoleString, 2, 2, ())
{
   Point2I pos = Canvas->getCursorPos();
   char * ret = Con::getReturnBuffer(32);
   dSprintf(ret, 32, "%d %d", pos.x, pos.y);
   return(ret);
}

/*! Use the setCursorPos method to set the position of the cursor in the cavas.
    @param position An \x y\ position vector specifying the new location of the cursor.
    @return No return value
*/
ConsoleMethodWithDocs( GuiCanvas, setCursorPos, ConsoleVoid, 3, 4, ( ))
{
   Point2I pos(0,0);

   if(argc == 4)
      pos.set(dAtoi(argv[2]), dAtoi(argv[3]));
   else
      dSscanf(argv[2], "%d %d", &pos.x, &pos.y);

   Canvas->setCursorPos(pos);
}

/*! Gets the gui control under the mouse.
*/
ConsoleMethodWithDocs( GuiCanvas, getMouseControl, ConsoleInt, 2, 2, ())
{
   GuiControl* control = object->getMouseControl();
   if (control)
      return control->getId();
   
   return 0;
}

//-----------------------------------------------------------------------------

/*! Sets the background color for the canvas.
    @param red The red value.
    @param green The green value.
    @param blue The blue value.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(GuiCanvas, setBackgroundColor, ConsoleVoid, 3, 6, (float red, float green, float blue, [float alpha = 1.0]))
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Space separated.
    if (argc == 3)
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            object->setDataField( StringTable->insert("BackgroundColor"), NULL, argv[2] );
            return;
        }

        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
                alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("GuiCanvas::setBackgroundColor() - Invalid Number of parameters!");
            return;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("GuiCanvas::setBackgroundColor() - Invalid Number of parameters!");
        return;
    }

    // Set background color.
    object->setBackgroundColor(ColorF(red, green, blue, alpha) );
}

//-----------------------------------------------------------------------------

/*! Gets the background color for the canvas.
    @return (float red / float green / float blue / float alpha) The background color for the canvas.
*/
ConsoleMethodWithDocs(GuiCanvas, getBackgroundColor, ConsoleString, 2, 2, (...))
{
    // Get the background color.
    const ColorF& color = object->getBackgroundColor();

    // Fetch color name.
    StringTableEntry colorName = StockColor::name( color );

    // Return the color name if it's valid.
    if ( colorName != StringTable->EmptyString )
        return colorName;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%g %g %g %g", color.red, color.green, color.blue, color.alpha );

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets whether to use the canvas background color or not.
    @param useBackgroundColor Whether to use the canvas background color or not.
    @return No return value.
*/
ConsoleMethodWithDocs(GuiCanvas, setUseBackgroundColor, ConsoleVoid, 3, 3, (...))
{
    // Fetch flag.
    const bool useBackgroundColor = dAtob(argv[2]);

    // Set the flag.
    object->setUseBackgroundColor( useBackgroundColor );
}

//-----------------------------------------------------------------------------

/*! Gets whether the canvas background color is in use or not.
    @return Whether the canvas background color is in use or not.
*/
ConsoleMethodWithDocs(GuiCanvas, getUseBackgroundColor, ConsoleBool, 2, 2, (...))
{
    // Get the flag.
    return object->getUseBackgroundColor();
}

ConsoleMethodGroupEndWithDocs(GuiCanvas)

/*! Use the createCanvas function to initialize the canvas.
    @return Returns true on success, false on failure.
    @sa createEffectCanvas
*/
ConsoleFunctionWithDocs( createCanvas, ConsoleBool, 2, 2, ( WindowTitle ))
{
    AssertISV(!Canvas, "CreateCanvas: canvas has already been instantiated");

    Platform::initWindow(Point2I(MIN_RESOLUTION_X, MIN_RESOLUTION_Y), argv[1]);


    if (!Video::getResolutionList())
        return false;

    // create the canvas, and add it to the manager
    Canvas = new GuiCanvas();
    Canvas->registerObject("Canvas"); // automatically adds to GuiGroup
    return true;
}

/*! Sets the title to the provided string 
    @param windowTitle The desired title
    @return No Return Value
*/
ConsoleFunctionWithDocs( setCanvasTitle, ConsoleVoid, 2, 2, (string windowTitle))
{
   Platform::setWindowTitle( argv[1] );
}

/*! 
    Take a screenshot.
    @param format One of JPEG or PNG.
*/
ConsoleFunctionWithDocs(screenShot, ConsoleVoid, 3, 3, (string file, string format))
{
#if !defined(TORQUE_OS_IOS) && !defined(TORQUE_OS_ANDROID)
// PUAP -Mat no screenshots on iPhone can do it from Xcode
    FileStream fStream;
   if(!fStream.open(argv[1], FileStream::Write))
   {   
      Con::printf("Failed to open file '%s'.", argv[1]);
      return;
   }
    
   glReadBuffer(GL_FRONT);
   
   Point2I extent = Canvas->getExtent();
   U8 * pixels = new U8[extent.x * extent.y * 4];
   glReadPixels(0, 0, extent.x, extent.y, GL_RGB, GL_UNSIGNED_BYTE, pixels);
   
   GBitmap * bitmap = new GBitmap;
   bitmap->allocateBitmap(U32(extent.x), U32(extent.y));
   
   // flip the rows
   for(U32 y = 0; y < (U32)extent.y; y++)
      dMemcpy(bitmap->getAddress(0, extent.y - y - 1), pixels + y * extent.x * 3, U32(extent.x * 3));

   if ( dStrcmp( argv[2], "JPEG" ) == 0 )
      bitmap->writeJPEG(fStream);
   else if( dStrcmp( argv[2], "PNG" ) == 0)
      bitmap->writePNG(fStream);
   else
      bitmap->writePNG(fStream);

   fStream.close();
   delete [] pixels;
   delete bitmap;
#endif
}
