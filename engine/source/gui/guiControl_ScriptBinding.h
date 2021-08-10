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

ConsoleMethodGroupBeginWithDocs(GuiControl, SimGroup)

/*! Sets whether this control can serialize itself to the hard disk
    @param Flag setting
    @return No Return Value
*/
ConsoleMethodWithDocs( GuiControl, setCanSave, ConsoleVoid, 3,3, (bool canSave))
{
   object->setCanSave( dAtob( argv[2] ) );
}

/*! Check if point id in the control
    @param x Point x coordinate in parent coords
    @param y Point y coordinate in parent coords
    @return Returns true if the point is in the control, false otherwise
*/
ConsoleMethodWithDocs(GuiControl, pointInControl, ConsoleBool, 4,4, (int x, int y))
{
   Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
   return object->pointInControl(kPoint);
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
/*! Adds the gui control
    @param controlId integer ID of the control to add
    @return No Return value
*/
ConsoleMethodWithDocs( GuiControl, addGuiControl, ConsoleVoid, 3, 3, (int controlId))
{

   GuiControl *ctrl = dynamic_cast<GuiControl *>(Sim::findObject(argv[2]));
   if(ctrl)
   {
      object->addObject(ctrl);
   }

}

//-----------------------------------------------------------------------------
//	Make Sure Child 1 is Ordered Just Under Child 2.
//-----------------------------------------------------------------------------
/*! uses simset reorder to push child 1 after child 2 - both must already be child controls of this control
*/
ConsoleMethodWithDocs(GuiControl, reorderChild, ConsoleVoid, 4,4,  (child1, child2))
{
   GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[2])));
   GuiControl* pTarget	 = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[3])));

   if(pControl && pTarget)
   {
      object->reOrder(pControl,pTarget);
   }
}

/*! @return Returns the Id of the parent control
*/
ConsoleMethodWithDocs( GuiControl, getParent, ConsoleInt, 2, 2, ())
{

   GuiControl* pParent		= object->getParent();
   if(pParent)
   {
      return pParent->getId();
   }

   return 0;

}

/*! Use the setValue method to set the control specific value to value. Purpose and type varies by control type.
    @param value Some control specific value.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setValue, ConsoleVoid, 3, 3, ( value ))
{
   object->setScriptValue(argv[2]);
}

/*! Use the getValue method to get the control-specific 'value' for this control.
    @return Returns a control-specific specific value. Varies by control
*/
ConsoleMethodWithDocs( GuiControl, getValue, ConsoleString, 2, 2, ())
{
   return object->getScriptValue();
}

/*! Use the setActive method to (de)activate this control. Once active, a control can accept inputs. Controls automatically re-shade/skin themselves to reflect their active/inactive state.
    @param isActive A boolean value. If isActive is true, this control is activated, else it is set to inactive.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setActive, ConsoleVoid, 3, 3, ( isActive ))
{
   object->setActive(dAtob(argv[2]));
}

/*! Use the isActive method to determine if this control is active.
    An inactive control may be visible, but will not accept inputs. It will also normally re-shade or re-skin itself to reflect its inactive state
    @return Returns true if this control is active.
*/
ConsoleMethodWithDocs( GuiControl, isActive, ConsoleBool, 2, 2, ())
{
   return object->isActive();
}

/*! Use the setVisible method to (un)hide this control.
    @param isVisible A boolean value. If true, the control will be made visible, otherwise the control will be hidden.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setVisible, ConsoleVoid, 3, 3, ( isVisible ))
{
   object->setVisible(dAtob(argv[2]));
}

/*! Use the makeFirstResponder method to force this control to become the first responder.
    @param isFirst A boolean value. If true, then this control become first reponder and at captures inputs before all other controls, excluding dialogs above this control.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, makeFirstResponder, ConsoleVoid, 3, 3, ( isFirst ))
{
   object->makeFirstResponder(dAtob(argv[2]));
}

/*! Use the isVisible method to determine if this control is visible.
    This can return true, even if the entire control covered by another. This merely means that the control will render if not covered
    @return Returns true if the control is visible.
*/
ConsoleMethodWithDocs( GuiControl, isVisible, ConsoleBool, 2, 2, ())
{
   return object->isVisible();
}

/*! Use the isAwake method to determine if this control is awake.
    @return Returns true if this control is awake and ready to display
*/
ConsoleMethodWithDocs( GuiControl, isAwake, ConsoleBool, 2, 2, ())
{
   return object->isAwake();
}

/*! Sets the currently used from for the GuiControl
    @param p The profile you wish to set the control to use
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
   GuiControlProfile * profile;

   if(Sim::findObject(argv[2], profile))
      object->setControlProfile(profile);
}

/*! Resizes the control to the given dimensions
*/
ConsoleMethodWithDocs( GuiControl, resize, ConsoleVoid, 6, 6, (int x, int y, int w, int h))
{
   Point2I newPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I newExt(dAtoi(argv[4]), dAtoi(argv[5]));
   object->resize(newPos, newExt);
}

/*! @return A string set up as \<pos.x> <pos.y>\
*/
ConsoleMethodWithDocs( GuiControl, getPosition, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &pos = object->getPosition();
   dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
   return retBuffer;
}
/*! @return Returns center of control, as space seperated ints
*/
ConsoleMethodWithDocs( GuiControl, getCenter, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos = object->getPosition();
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

/*! Sets control position, by center - coords are local not global
    @return No Return value.
*/
ConsoleMethodWithDocs( GuiControl, setCenter, ConsoleVoid, 4, 4, (int x, int y))
{
   const Point2I ext = object->getExtent();
   Point2I newpos(dAtoi(argv[2])-ext.x/2, dAtoi(argv[3])-ext.y/2);
   object->setPosition(newpos);
}

/*! @return Returns center of control, as space seperated ints
*/
ConsoleMethodWithDocs( GuiControl, getGlobalCenter, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I tl(0,0);
   Point2I pos		 =	object->localToGlobalCoord(tl);
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

/*! @return Returns the control's position converted to global coordinates (position as space-separted integers)
*/
ConsoleMethodWithDocs( GuiControl, getGlobalPosition, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos(0,0);
   Point2I gPos	=	object->localToGlobalCoord(pos);

   dSprintf(retBuffer, 64, "%d %d", gPos.x, gPos.y);
   return retBuffer;
}
/*! Sets the control's position in global space
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setPositionGlobal, ConsoleVoid, 4, 4, (int x, int y))
{
   //see if we can turn the x/y into ints directly, 
   Point2I gPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I lPosOffset	=	object->globalToLocalCoord(gPos);
   lPosOffset.x += object->mBounds.point.x;
   lPosOffset.y += object->mBounds.point.y;

   object->mBounds.set(lPosOffset,object->mBounds.extent);
}

/*! Sets the current control position in local space
    @return No Return Value.
*/
ConsoleMethodWithDocs( GuiControl, setPosition, ConsoleVoid, 4, 4, (int x, int y))
{
   //see if we can turn the x/y into ints directly, 
   Point2I lPos(dAtoi(argv[2]), dAtoi(argv[3]));
   object->mBounds.set(lPos,object->mBounds.extent);
}

/*! Get the width and height of the control.
    @return The height and width as a string with space-separated integers
*/
ConsoleMethodWithDocs( GuiControl, getExtent, ConsoleString, 2, 2, (...))
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &ext = object->getExtent();
   dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
   return retBuffer;
}

/*! Sets the width & height of the control.
    @return No Return Value.
*/
ConsoleMethodWithDocs( GuiControl, setExtent, ConsoleVoid, 4, 4, (int width, int height))
{
   Point2I kExt(dAtoi(argv[2]), dAtoi(argv[3]));
   object->setExtent(kExt);
   object->resetStoredExtent();
}

/*! Get the minimum allowed size of the control.
    @return Returns the minimum extent as a string with space separated point values <width> <height>
*/
ConsoleMethodWithDocs( GuiControl, getMinExtent, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &minExt = object->getMinExtent();
   dSprintf(retBuffer, 64, "%d %d", minExt.x, minExt.y);
   return retBuffer;
}

/*! Searches for the control at the given point 
    @return Returns the Id of the control at the point
*/
ConsoleMethodWithDocs( GuiControl, findHitControl, ConsoleInt, 4, 4, (int x, int y))
{
   Point2I pos(dAtoi(argv[2]), dAtoi(argv[3]));
   GuiControl *hit = object->findHitControl(pos);
   return hit ? hit->getId() : 0;
}

/*! Sets this control as the first responder
*/
ConsoleMethodWithDocs(GuiControl, setFirstResponder, ConsoleVoid, 2, 2, ())
{
   object->setFirstResponder();
}

/*! Simulates a click on the control.
*/
ConsoleMethodWithDocs(GuiControl, performClick, ConsoleVoid, 2, 2, ())
{
	object->onAction();
}

/*! Sets the text of the control to the string.
*/
ConsoleMethodWithDocs(GuiControl, setText, ConsoleVoid, 3, 3, (text))
{
	object->setText(argv[2]);
}

/*! Sets the text of the control to the localized string.
*/
ConsoleMethodWithDocs(GuiControl, setTextID, ConsoleVoid, 3, 3, (id))
{
	object->setTextID(argv[2]);
}

/*! returns the text of the control.
*/
ConsoleMethodWithDocs(GuiControl, getText, ConsoleString, 2, 2, ())
{
	return object->getText();
}

/*! Turns on or off text wrap.
	@param setting True turns on text wrap.
	@return No return value
*/
ConsoleMethodWithDocs(GuiControl, setTextWrap, ConsoleVoid, 3, 3, (setting))
{
	object->setTextWrap(dAtob(argv[2]));
}

/*! Returns if text wrap is on.
	@return Returns the state of text wrap.
*/
ConsoleMethodWithDocs(GuiControl, getTextWrap, ConsoleBool, 2, 2, ())
{
	return object->getTextWrap();
}

ConsoleMethodGroupEndWithDocs(GuiControl)
