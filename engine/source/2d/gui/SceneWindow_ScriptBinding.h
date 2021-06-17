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

ConsoleMethodGroupBeginWithDocs(SceneWindow, GuiControl)

/*! Returns the windows position and extent.
    @return Returns the window dimensions as a string formatted as follows: <position.x> <position.y> <width> <height>
*/
ConsoleMethodWithDocs(SceneWindow, getWindowArea, ConsoleString, 2, 2, ())
{
    // Get Size Argument Buffer.
    char* pExtentsBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf( pExtentsBuffer, 64, "%d %d %d %d", object->getPosition().x, object->getPosition().y, object->getExtent().x, object->getExtent().y );

    // Return Buffer.
    return pExtentsBuffer;
}

//-----------------------------------------------------------------------------

/*! Returns the Scene associated with this window.
    @return Returns the scene ID as a string
*/
ConsoleMethodWithDocs(SceneWindow, getScene, ConsoleString, 2, 2, ())
{
   Scene* pScene = object->getScene();

   char* id = Con::getReturnBuffer(8);
   if (pScene)
   {
        dItoa(pScene->getId(), id );
   }
   else
   {
      id[0] = '\0';
   }

   return id;
}

//-----------------------------------------------------------------------------

/*! Associates Scene Object.
    @param Scene The scene ID or name.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setScene, ConsoleVoid, 2, 3, (Scene))
{
    // No scene specified?
    if ( argc < 3 )
    {
        // No, so reset the scene.
        object->resetScene();
        // Finish here.
        return;
    }

    // Find Scene Object.
    Scene* pScene = (Scene*)(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pScene )
    {
        Con::warnf("SceneWindow::setScene() - Couldn't find object '%s'.", argv[2]);
        return;
    }

    // Set Scene.
    object->setScene( pScene );
}

//-----------------------------------------------------------------------------

/*! Detaches the window from any Scene Object.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, resetScene, ConsoleVoid, 2, 2, ())
{
    // Reset Scene.
    object->resetScene();
}

//-----------------------------------------------------------------------------

/*! Set the current camera position.
    @param X Position along the X axis.
    @param Y Position along the Y axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraPosition, ConsoleVoid, 3, 4, (x , y))
{
   if ( argc == 3 )
   {
       object->setCameraPosition( Vector2(argv[2]) );
       return;
   }

   object->setCameraPosition( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

/*! Get the current camera position.
    @return The current camera position.
*/
ConsoleMethodWithDocs(SceneWindow, getCameraPosition, ConsoleString, 2, 2, ())
{
    return object->getCameraPosition().scriptThis();
}   

//-----------------------------------------------------------------------------

/*! Set the current camera position.
    @param width Size along the X axis.
    @param height Size along the Y axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraSize, ConsoleVoid, 3, 4, (width , height))
{
   if ( argc == 3 )
   {
       object->setCameraSize( Vector2(argv[2]) );
       return;
   }

   object->setCameraSize( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

/*! Get the current camera size.
    @return The current camera width and height.
*/
ConsoleMethodWithDocs(SceneWindow, getCameraSize, ConsoleString, 2, 2, ())
{
    return object->getCameraSize().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Set the current camera area.
    @param x1,y1,x2,y2 The coordinates of the minimum and maximum points (top left, bottom right)
    The input can be formatted as either \x1 y1 x2 y2\, \x1 y1, x2 y2\, \x1, y1, x2, y2\
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraArea, ConsoleVoid, 3, 6, (x1 / y1 / x2 / y2))
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = Utility::mGetStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = Utility::mGetStringElementVector(argv[2]);
       v2 = Utility::mGetStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = Utility::mGetStringElementVector(argv[2]);
      v2 = Utility::mGetStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
       v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("SceneWindow::setCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    Vector2 topLeft( (v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y );
    Vector2 bottomRight( (v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y );

    // Set Current Camera Area.
    object->setCameraArea( RectF(topLeft.x, topLeft.y, bottomRight.x-topLeft.x, bottomRight.y-topLeft.y) );
}

//-----------------------------------------------------------------------------

/*! Get the current camera Area.
    @return The camera area formatted as \x1 y1 x2 y2\
*/
ConsoleMethodWithDocs(SceneWindow, getCameraArea, ConsoleString, 2, 2, ())
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%.5g %.5g %.5g %.5g", cameraWindow.point.x, cameraWindow.point.y, cameraWindow.point.x+cameraWindow.extent.x, cameraWindow.point.y+cameraWindow.extent.y);

    // Return Buffer.
    return pBuffer;
}     

//-----------------------------------------------------------------------------

/*! Set the current camera Zoom Factor.
    @param zoomFactor A float value representing the zoom factor
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraZoom, ConsoleVoid, 3, 3, (zoomFactor))
{
    object->setCameraZoom( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Get the current camera Zoom.
    @return The current camera zoom.
*/
ConsoleMethodWithDocs(SceneWindow, getCameraZoom, ConsoleFloat, 2, 2, ())
{
    return object->getCameraZoom();
} 

//-----------------------------------------------------------------------------

/*! Sets the current camera angle.
    @param angle The current camera angle in degrees.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraAngle, ConsoleVoid, 3, 3, (angle))
{
    object->setCameraAngle( mDegToRad(dAtof(argv[2])) );
}

//-----------------------------------------------------------------------------

/*! Gets the current camera angle.
    @return The current camera angle in degrees.
*/
ConsoleMethodWithDocs(SceneWindow, getCameraAngle, ConsoleFloat, 2, 2, ())
{
    return object->getCameraAngle();
}

//-----------------------------------------------------------------------------

/*! Get current camera scale to world.
    @return Returns the cameras window width/height scale to world as a string formatted as \widthScale heightScale\
*/
ConsoleMethodWithDocs(SceneWindow, getCameraWorldScale, ConsoleString, 2, 2, ())
{
    // Fetch camera window
    const Vector2 cameraWindowScale = object->getCameraWindowScale();

    return cameraWindowScale.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Get current camera position post-view-limit clamping.
    @return The current camera render position.
*/
ConsoleMethodWithDocs(SceneWindow, getCameraRenderPosition, ConsoleString, 2, 2, ())
{
    return object->getCameraRenderPosition().scriptThis();
} 

//-----------------------------------------------------------------------------

/*! Get current camera scale to render.
    @return Returns the cameras window width/height scale to render as a string formatted as \widthScale heightScale\
*/
ConsoleMethodWithDocs(SceneWindow, getCameraRenderScale, ConsoleString, 2, 2, ())
{
    // Fetch camera window scale.
    Vector2 cameraWindowScale = object->getCameraWindowScale();

    // Inverse scale.
    cameraWindowScale.receiprocate();

    return cameraWindowScale.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Set the target camera position.
    @param X Position along the X axis.
    @param Y Position along the Y axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setTargetCameraPosition, ConsoleVoid, 3, 4, (x , y))
{
   if ( argc == 3 )
   {
       object->setTargetCameraPosition( Vector2(argv[2]) );
       return;
   }

   object->setTargetCameraPosition( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}


//-----------------------------------------------------------------------------

/*! Get the target camera position.
    @return The target camera position.
*/
ConsoleMethodWithDocs(SceneWindow, getTargetCameraPosition, ConsoleString, 2, 2, ())
{
    return object->getTargetCameraPosition().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Set the target camera position.
    @param width Size along the X axis.
    @param height Size along the Y axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setTargetCameraSize, ConsoleVoid, 3, 4, (width , height))
{
   if ( argc == 3 )
   {
       object->setTargetCameraSize( Vector2(argv[2]) );
       return;
   }

   object->setTargetCameraSize( Vector2(dAtof(argv[2]), dAtof(argv[3])) );
}

//-----------------------------------------------------------------------------

/*! Get the target camera size.
    @return The target camera width and height.
*/
ConsoleMethodWithDocs(SceneWindow, getTargetCameraSize, ConsoleString, 2, 2, ())
{
    return object->getTargetCameraSize().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Set the target camera area.
    @param x1 / y1 Coordinates of the lower left corner of the target area.
    @param x2 / y2 Coordinates of the upper right corner of the target area.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setTargetCameraArea, ConsoleVoid, 3, 6, (x1 / y1 / x2 / y2))
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = Utility::mGetStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = Utility::mGetStringElementVector(argv[2]);
       v2 = Utility::mGetStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = Utility::mGetStringElementVector(argv[2]);
      v2 = Utility::mGetStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
       v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("SceneWindow::setTargetCameraArea() - Invalid number of parameters!");
      return;
   }

    // Calculate Normalised Rectangle.
    Vector2 topLeft( (v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y );
    Vector2 bottomRight( (v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y );

    // Set Target Camera Area.
    object->setTargetCameraArea( RectF(topLeft.x, topLeft.y, bottomRight.x-topLeft.x+1, bottomRight.y-topLeft.y+1) );
}

//-----------------------------------------------------------------------------

/*! Get the target camera Area.
    @return The camera area formatted as \x1 y1 x2 y2\
*/
ConsoleMethodWithDocs(SceneWindow, getTargetCameraArea, ConsoleString, 2, 2, ())
{
    // Fetch Camera Window.
    const RectF cameraWindow = object->getTargetCameraArea();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);

    // Format Buffer.
    dSprintf(pBuffer, 64, "%.5g %.5g %.5g %.5g", cameraWindow.point.x, cameraWindow.point.y, cameraWindow.point.x+cameraWindow.extent.x, cameraWindow.point.y+cameraWindow.extent.y);

    // Return Buffer.
    return pBuffer;
} 

//-----------------------------------------------------------------------------

/*! Set the target camera Zoom Factor.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setTargetCameraZoom, ConsoleVoid, 3, 3, (zoomFactor))
{
    object->setTargetCameraZoom( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Get the target camera Zoom.
    @return The current camera zoom.
*/
ConsoleMethodWithDocs(SceneWindow, getTargetCameraZoom, ConsoleFloat, 2, 2, ())
{
    return object->getTargetCameraZoom();
}

//-----------------------------------------------------------------------------

/*! Sets the target camera angle.
    @param angle The target camera angle in degrees.
     @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setTargetCameraAngle, ConsoleVoid, 3, 3, (angle))
{
    object->setTargetCameraAngle( mDegToRad(dAtof(argv[2])) );
}

//-----------------------------------------------------------------------------

/*! Gets the target camera angle.
    @return The target camera angle in degrees.
*/
ConsoleMethodWithDocs(SceneWindow, getTargetCameraAngle, ConsoleFloat, 2, 2, ())
{
    return object->getTargetCameraAngle();
}

//-----------------------------------------------------------------------------

/*! Set camera interpolation time.
    @param interpolationTime Time to animate given in seconds.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setCameraInterpolationTime, ConsoleVoid, 3, 3, (interpolationTime))
{
    // Set Camera Interpolation Time.
    object->setCameraInterpolationTime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Set camera interpolation mode.
    @param interpolationMode Posible values are Linear and Sigmoid.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setCameraInterpolationMode, ConsoleVoid, 3, 3, (interpolationMode))
{
    // Set Camera Interpolation Mode.
    object->setCameraInterpolationMode( SceneWindow::getInterpolationModeEnum(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Start Camera Move.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, startCameraMove, ConsoleVoid, 2, 3, ([interpolationTime]))
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Start Camera Move.
    object->startCameraMove( interpolationTime );
}

//-----------------------------------------------------------------------------

/*! Stops current camera movement
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, stopCameraMove, ConsoleVoid, 2, 2, ())
{
    // Stop Camera Move.
    object->stopCameraMove();
}

//-----------------------------------------------------------------------------

/*! Moves camera directly to target.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, completeCameraMove, ConsoleVoid, 2, 2, ())
{
    // Complete Camera Move.
    object->completeCameraMove();
}

//-----------------------------------------------------------------------------

/*! Reverses previous camera movement.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, undoCameraMove, ConsoleVoid, 2, 3, ([interpolationTime]))
{
    F32 interpolationTime;

    // Interpolation Time?
    if ( argc >= 3 )
        interpolationTime = dAtof(argv[2]);
    else
        interpolationTime = object->getCameraInterpolationTime();

    // Undo Camera Move.
    object->undoCameraMove( interpolationTime );
}

//-----------------------------------------------------------------------------

/*! Check the camera moving status.
    @return Returns a boolean value as to whether or not the camera is moving.
*/
ConsoleMethodWithDocs(SceneWindow, getIsCameraMoving, ConsoleBool, 2, 2, ())
{
    // Is Camera Moving?
    return object->isCameraMoving();
}

//-----------------------------------------------------------------------------

/*! Check the camera mounted status.
    @return Returns a boolean value as to whether or not the camera is mounted.
*/
ConsoleMethodWithDocs(SceneWindow, getIsCameraMounted, ConsoleBool, 2, 2, ())
{
    // Is Camera Mounted.
    return object->isCameraMounted();
}

//-----------------------------------------------------------------------------

/*! Starts the camera shaking.
    @param shakeMagnitude The intensity of the shaking
    @param time The length of the shake
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, startCameraShake, ConsoleVoid, 4, 4, (shakeMagnitude, time))
{
    // Start Camera Shake.
    object->startCameraShake( dAtof(argv[2]), dAtof(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Stops the camera shaking.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, stopCameraShake, ConsoleVoid, 2, 2, ())
{
    // Stop Camera Shake.
    object->stopCameraShake();
}

//-----------------------------------------------------------------------------

/*! Mounts the camera onto the specified object.
    @param sceneObject The scene object to mount the camera to.
    @param offsetX / offsetY The offset from the objects position to mount the camera to.  Optional: Defaults to no offset.
    @param mountForce The force to use to keep the camera mounted to the object.  Zero is a rigid mount.  Optional: Defaults to zero.
    @param sendToMount Whether to immediately move the camera to the objects position or not.  Optional: Defaults to true.
    @param mountAngle Whether to mount the cameras angle to the objects angle or not.  Optional: Defaults to false.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, mount, ConsoleVoid, 3, 8, (sceneObject, [offsetX / offsetY], [mountForce], [sendToMount?], [mountAngle?]))
{
    // Grab the object. Always specified.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if (!pSceneObject)
    {
        Con::warnf("SceneWindow::mount() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Set defaults.
    Vector2 mountOffset(0.0f, 0.0f);
    F32 mountForce = 0.0f;
    bool sendToMount = true;
    bool mountAngle = false;
    
    U32 nextArg = 3;
    if ( (U32)argc > nextArg )
    {
        // Fetch Element Count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[nextArg]);

        // (object, "offsetX offsetY", ...)
        if ( elementCount == 2 )
        {
            mountOffset = Utility::mGetStringElementVector(argv[nextArg++]);
        }
        // (object, offsetX, offsetY, ...)
        else if ( elementCount == 1 && (U32)argc >= nextArg+1 )
        {
            mountOffset = Vector2(dAtof(argv[nextArg]), dAtof(argv[nextArg+1]));
            nextArg += 2;
        }
        // Invalid.
        else
        {
            Con::warnf("SceneWindow::mount() - Invalid number of parameters!");
            return;
        }
    }

    // Grab the mount force - if it's specified.
    if ( (U32)argc > nextArg )
        mountForce = dAtof(argv[nextArg++]);

    // Grab the send to mount flag.

    if ( (U32)argc > nextArg )
        sendToMount = dAtob(argv[nextArg++]);


    if ( (U32)argc > nextArg )
        mountAngle = dAtob(argv[nextArg++]);

    // Mount Object.
    object->mount( pSceneObject, mountOffset, mountForce, sendToMount, mountAngle );
}

//-----------------------------------------------------------------------------

/*! Dismounts Camera from object.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, dismount, ConsoleVoid, 2, 2, ())
{
    // Dismount Object.
    object->dismount();
}

//-----------------------------------------------------------------------------

/*! Set View Limit On.
	@param [minX / minY / maxX / maxY] Optionally supplies the limits for the view.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setViewLimitOn, ConsoleVoid, 3, 6, ([minX / minY / maxX / maxY]))
{
   // Upper left bound.
   Vector2 v1;
   // Lower right bound.
   Vector2 v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = Utility::mGetStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = Utility::mGetStringElementVector(argv[2]);
       v2 = Utility::mGetStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = Utility::mGetStringElementVector(argv[2]);
      v2 = Utility::mGetStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
       v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("SceneWindow::setViewLimitOn() - Invalid number of parameters!");
      return;
   }

   // Calculate Normalised Rectangle.
   Vector2 topLeft((v1.x <= v2.x) ? v1.x : v2.x, (v1.y <= v2.y) ? v1.y : v2.y);
   Vector2 bottomRight((v1.x > v2.x) ? v1.x : v2.x, (v1.y > v2.y) ? v1.y : v2.y);

    // Set the View Limit On.
    object->setViewLimitOn(topLeft, bottomRight);
}

//-----------------------------------------------------------------------------

/*! Set View Limit Off.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setViewLimitOff, ConsoleVoid, 2, 2, ())
{
    // Set View Limit Off.
    object->setViewLimitOff();
}

//-----------------------------------------------------------------------------

/*! Clamps the current camera to the current view limit.
    Nothing will happen if the view-limit is not active or the camera is moving.
    @return No return value
*/
ConsoleMethodWithDocs(SceneWindow, clampCameraViewLimit, ConsoleVoid, 2, 2, ())
{
    object->clampCameraViewLimit();
}

//-----------------------------------------------------------------------------

/*! Sets the scroll bars on. View Limits should be set prior to turning on scroll bars. 
	@param setting If true, scroll bars will be on. 
	@return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setShowScrollBar, ConsoleVoid, 3, 3, (setting))
{
	object->setShowScrollBar(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! When true, allows the mouse wheel to move the scroll bar when it's turned on.
	@param setting If true, the scroll bars will consume the mouse wheel events instead of being passed to the scene. Holding shift will reverse this behavior.
	@return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setMouseWheelScrolls, ConsoleVoid, 3, 3, (setting))
{
	object->setMouseWheelScrolls(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Sets the render group(s).
    @param groups The list of groups you wish to set.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setRenderGroups, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (groups$))
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setRenderGroups() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderGroups(mask);
}

//-----------------------------------------------------------------------------

/*! Sets the render layers(s).
    @param The layer numbers you wish to set.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setRenderLayers, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (layers$))
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setRenderLayers() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setRenderLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }
   // Set Collision Groups.
   object->setRenderLayers(mask);
}

//-----------------------------------------------------------------------------

/*! Sets the layer/group mask which control what is rendered.
    @param layermask The bitmask for setting the layers to render
    @param groupmask The bitmask for setting the groups to render
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setRenderMasks, ConsoleVoid, 3, 4, (layerMask, groupMask))
{
    // Set Render Masks.
   if( argc < 4 )
      object->setRenderLayers( dAtoi( argv[2] ) );
   else
      object->setRenderMasks( dAtoi(argv[2]), dAtoi(argv[3]) );
}   

//-----------------------------------------------------------------------------

/*! Gets the layer mask which controls what is rendered.
    @returns The bit mask corresponding to the layers which are to be rendered
*/
ConsoleMethodWithDocs(SceneWindow, getRenderLayerMask, ConsoleInt, 2, 2, ())
{
   return object->getRenderLayerMask();
}   

//-----------------------------------------------------------------------------

/*! Gets the group mask which controls what is rendered.
    @returns The bit mask corresponding to the groups which are to be rendered
*/
ConsoleMethodWithDocs(SceneWindow, getRenderGroupMask, ConsoleInt, 2, 2, ())
{
   return object->getRenderGroupMask();
}

//-----------------------------------------------------------------------------

/*! or ( stockColorName )  - Sets the background color for the scene.
    @param red The red value as floating point value between 0 and 1.
    @param green The green value as floating point value between 0 and 1.
    @param blue The blue value as floating point value between 0 and 1.
    @param alpha The alpha value as floating point value between 0 and 1.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneWindow, setBackgroundColor, ConsoleVoid, 3, 6, (float red, float green, float blue, [float alpha = 1.0]))
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
            Con::warnf("SceneWindow::setBackgroundColor() - Invalid Number of parameters!");
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
        Con::warnf("SceneWindow::setBackgroundColor() - Invalid Number of parameters!");
        return;
    }

    // Set background color.
    object->setBackgroundColor(ColorF(red, green, blue, alpha) );
}

//-----------------------------------------------------------------------------

/*! Gets the background color for the scene.
    @return (float red / float green / float blue / float alpha) The background color for the scene.
*/
ConsoleMethodWithDocs(SceneWindow, getBackgroundColor, ConsoleString, 2, 2, (...))
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

/*! Sets whether to use the scene background color or not.
    @param useBackgroundColor Whether to use the scene background color or not.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setUseBackgroundColor, ConsoleVoid, 3, 3, (...))
{
    // Fetch flag.
    const bool useBackgroundColor = dAtob(argv[2]);

    // Set the flag.
    object->setUseBackgroundColor( useBackgroundColor );
}

//-----------------------------------------------------------------------------

/*! Gets whether the scene background color is in use or not.
    @return Whether the scene background color is in use or not.
*/
ConsoleMethodWithDocs(SceneWindow, getUseBackgroundColor, ConsoleBool, 2, 2, (...))
{
    // Get the flag.
    return object->getUseBackgroundColor();
}

//-----------------------------------------------------------------------------

/*! Sets whether input events are monitored by the window or not.
    @param inputStatus Whether input events are processed by the window or not.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setUseWindowInputEvents, ConsoleVoid, 3, 3, (inputStatus))
{
    object->setUseWindowInputEvents( dAtob(argv[2]) );
}   

//-----------------------------------------------------------------------------

/*! Gets whether input events are monitored by the window or not.
    @return Whether input events are monitored by the window or not.
*/
ConsoleMethodWithDocs(SceneWindow, getUseWindowInputEvents, ConsoleBool, 2, 2, ())
{
    return object->getUseWindowInputEvents();
}   

//-----------------------------------------------------------------------------

/*! Sets whether input events are passed onto scene objects or not.
    @param mouseStatus Whether input events are passed onto scene objects or not.
*/
ConsoleMethodWithDocs(SceneWindow, setUseObjectInputEvents, ConsoleVoid, 3, 3, (inputStatus))
{
    object->setUseObjectInputEvents( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether input events are passed onto scene objects or not.
    @return Whether input events are passed onto scene objects or not..
*/
ConsoleMethodWithDocs(SceneWindow, getUseObjectInputEvents, ConsoleBool, 2, 2, ())
{
    return object->getUseObjectInputEvents();
}

//-----------------------------------------------------------------------------

/*! Sets the input events group filter.
    @param List of groups to filter input events with.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setObjectInputEventGroupFilter, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (groups$))
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventGroupFilter() - Invalid group specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }

   // Set group filter.
   object->setObjectInputEventGroupFilter(mask);
}

//-----------------------------------------------------------------------------

/*! Sets the input events layer filter.
    @param The list of layers to filter input events with.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setObjectInputEventLayerFilter, ConsoleVoid, 3, 2 + MASK_BITCOUNT, (layers$))
{
   // The mask.
   U32 mask = 0;

   // Grab the element count of the first parameter.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // Make sure we get at least one number.
   if (elementCount < 1)
   {
      Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid number of parameters!");
      return;
   }

   // Space separated list.
   if (argc == 3)
   {
      // Convert the string to a mask.
      for (U32 i = 0; i < elementCount; i++)
      {
         S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }
         
         mask |= (1 << bit);
      }
   }

   // Comma separated list.
   else
   {
      // Convert the list to a mask.
      for ( U32 i = 2; i < (U32)argc; i++ )
      {
         S32 bit = dAtoi(argv[i]);
         
         // Make sure the group is valid.
         if ((bit < 0) || (bit >= MASK_BITCOUNT))
         {
            Con::warnf("SceneWindow::setObjectInputEventLayerFilter() - Invalid layer specified (%d); skipped!", bit);
            continue;
         }

         mask |= (1 << bit);
      }
   }

   // Set layer filter.
   object->setObjectInputEventLayerFilter(mask);
}

//-----------------------------------------------------------------------------

/*! Sets input filter for input events.
*/
ConsoleMethodWithDocs(SceneWindow, setObjectInputEventFilter, ConsoleVoid, 4, 5, (groupMask, layerMask, [useInvisibleFilter?]))
{
    // Calculate Use Invisible Flag.
    bool useInvisible = argc >= 5 ? dAtob(argv[4]) : true;

    // Set input event filter.
    object->setObjectInputEventFilter( dAtoi(argv[2]), dAtoi(argv[3]), useInvisible );
}

//-----------------------------------------------------------------------------

/*! Sets whether invisible objects should be filtered for input events or not.
*/
ConsoleMethodWithDocs(SceneWindow, setObjectInputEventInvisibleFilter, ConsoleVoid, 3, 3, (bool useInvisibleFilter))
{
   object->setObjectInputEventInvisibleFilter(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Adds an object as an input listener.
    @param inputListener The object to add as an input listener.
    @return Whether the object was added as an input event listener or not.
*/
ConsoleMethodWithDocs(SceneWindow, addInputListener, ConsoleBool, 3, 3, (inputListener))
{
    // Find the SimObject
    SimObject* pSimObject = Sim::findObject( argv[2] );

    // Did we find the SimObject?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "SceneWindow::addInputListener() - Could not find the object '%s' to add as an input event listener.", argv[2] );
        return false;
    }

    // Add input listener.
    object->addInputListener( pSimObject );

    return true;
}

//-----------------------------------------------------------------------------

/*! Removes an object from being an input listener.
    @param inputListener The object to remove as an input listener.
    @return Whether the object was removed as an input event listener or not.
*/
ConsoleMethodWithDocs(SceneWindow, removeInputListener, ConsoleBool, 3, 3, (inputListener))
{
    // Find the SimObject
    SimObject* pSimObject = Sim::findObject( argv[2] );

    // Did we find the SimObject?
    if ( pSimObject == NULL )
    {
        // No, so warn.
        Con::warnf( "SceneWindow::removeInputListener() - Could not find the object '%s' to remove from being an input event listener.", argv[2] );
        return false;
    }

    // Remove input listener.
    object->removeInputListener( pSimObject );

    return true;
}

//-----------------------------------------------------------------------------

/*! Sets the window mouse-lock status.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneWindow, setLockMouse, ConsoleVoid, 3, 3, (bool lockSet))
{
    // Set Lock Mouse.
    object->setLockMouse( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the window mouse-lock status.
*/
ConsoleMethodWithDocs(SceneWindow, getLockMouse, ConsoleBool, 2, 2, ())
{
    // Get Lock Mouse.
    return object->getLockMouse();
}

//-----------------------------------------------------------------------------

/*! Sets Current Mouse Position.
    @param x,y The coordinates to set the mouse cursor. Accepts either (x,y) or (\x y\
*/
ConsoleMethodWithDocs(SceneWindow, setMousePosition, ConsoleVoid, 3, 4, (x/y))
{
   // The new position.
   Vector2 position;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      position = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      position = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::setMousePosition() - Invalid number of parameters!");
      return;
   }

    // Set Mouse Position.
    object->setMousePosition( position );
}

//-----------------------------------------------------------------------------

/*! Gets Current Mouse Position.
    @return Returns a string with the current mouse cursor coordinates formatted as \x y\
*/
ConsoleMethodWithDocs(SceneWindow, getMousePosition, ConsoleString, 2, 2, ())
{
    // Fetch Mouse Position.
    Vector2 worldMousePoint = object->getMousePosition();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Generate Script Parameters.
    dSprintf(pBuffer, 32, "%g %g", worldMousePoint.x, worldMousePoint.y);

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Returns World coordinate of Window coordinate.
    @param x,y The coordinates in window coordinates you wish to convert to world coordinates. Accepts either (x,y) or (\x y\
    @return Returns the desired world coordinates as a string formatted as \x y\
*/
ConsoleMethodWithDocs(SceneWindow, getWorldPoint, ConsoleString, 3, 4, (X / Y))
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getWorldPoint() - Invalid number of parameters!");
      return NULL;
   }
   
   // Destination Point.
    Vector2 dstPoint;

    // Do Conversion.
    object->windowToScenePoint( srcPoint, dstPoint );

    return dstPoint.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns Window coordinate of World coordinate.
    @param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\x y\
    @return Returns the desired window coordinates as a string formatted as \x y\
*/
ConsoleMethodWithDocs(SceneWindow, getWindowPoint, ConsoleString, 3, 4, (X / Y))
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getWindowPoint() - Invalid number of parameters!");
      return NULL;
   }
   
   // Destination Point.
    Vector2 dstPoint;

    // Do Conversion.
    object->sceneToWindowPoint( srcPoint, dstPoint );

    return dstPoint.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns Canvas coordinate of Window coordinate.
    @param x,y The coordinates in world coordinates you wish to convert to window coordinates. Accepts either (x,y) or (\x y\
    @return Returns the desired canvas coordinates as a string formatted as \x y\
*/
ConsoleMethodWithDocs(SceneWindow, getCanvasPoint, ConsoleString, 3, 4, (X / Y))
{
   // The new position.
   Point2I srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
   {
      srcPoint.x = dAtoi(Utility::mGetStringElement(argv[2], 0));
      srcPoint.y = dAtoi(Utility::mGetStringElement(argv[2], 1));
   }

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Point2I(dAtoi(argv[2]), dAtoi(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getCanvasPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Do Conversion.
    Point2I dstPoint = object->localToGlobalCoord( srcPoint );

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", dstPoint.x, dstPoint.y);
    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Checks if World coordinate is inside Window.
    @param x,y The coordinates in world coordinates you wish to check. Accepts either (x,y) or (\x y\
    @return Returns true if the coordinates are within the window, and false otherwise.
*/
ConsoleMethodWithDocs(SceneWindow, getIsWindowPoint, ConsoleBool, 3, 4, (X / Y))
{
   // The new position.
   Vector2 srcPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      srcPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      srcPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneWindow::getIsWindowPoint() - Invalid number of parameters!");
      return false;
   }

   // Destination Point.
    Vector2 dstPoint;
    // Do Conversion.
    object->sceneToWindowPoint( srcPoint, dstPoint );

    // Check if point is in window bounds.
    return object->mBounds.pointInRect( Point2I( S32(mFloor(dstPoint.x)+object->mBounds.point.x), S32(mFloor(dstPoint.y)+object->mBounds.point.y )) );
}

/*! Sets this Scenewindow to process the AudioListener's position and velocity
    WARNING : The code does not verify if multiple SceneWindows have this function enabled.
              User must make sure to manage this manually. Multiple listeners may lead to crashes/bugs/unexpected behaviors.
    @param status - Whether to enable or disable AudioListener processing (optional, default to true)
*/
ConsoleMethodWithDocs(SceneWindow, setAudioListener, ConsoleVoid, 2, 3, ())
{
    bool onoff = true;
    if ((argc > 2) && !dAtob(argv[2])) onoff = false;
    object->setProcessAudioListener(onoff);
}

/*! Sets the currently used ThumbProfile for the GuiControl
	@param p The Thumbprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setThumbProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlThumbProfile(profile);
}

/*! Sets the currently used TrackProfile for the GuiControl
	@param p The Trackprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setTrackProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlTrackProfile(profile);
}

/*! Sets the currently used ArrowProfile for the GuiControl
	@param p The Arrowprofile you wish to set the control to use
	@return No return value
*/
ConsoleMethodWithDocs(SceneWindow, setArrowProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlArrowProfile(profile);
}

ConsoleMethodGroupEndWithDocs(SceneWindow)
