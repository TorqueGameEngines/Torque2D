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

#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "sim/simBase.h"
#include "gui/guiCanvas.h"
#include "gui/editor/guiEditCtrl.h"
#include "platform/event.h"
#include "io/fileStream.h"
#include "gui/containers/guiScrollCtrl.h"

IMPLEMENT_CONOBJECT(GuiEditCtrl);

GuiEditCtrl::GuiEditCtrl(): mCurrentAddSet(NULL),
                            mContentControl(NULL),
                            mGridSnap(0,0),
                            mDragBeginPoint(-1,-1)                            
{
   VECTOR_SET_ASSOCIATION(mSelectedControls);
   VECTOR_SET_ASSOCIATION(mDragBeginPoints);

   mActive = true;
   mDragBeginPoints.clear();
   mSelectedControls.clear();

   mDefaultCursor    = NULL;
   mLeftRightCursor  = NULL;
   mUpDownCursor     = NULL;
   mNWSECursor       = NULL;
   mNESWCursor       = NULL;
   mMoveCursor       = NULL;
}

bool GuiEditCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   mTrash.registerObject();
   mSelectedSet.registerObject();
   mUndoManager.registerObject();

   return true;
}

void GuiEditCtrl::onRemove()
{
   Parent::onRemove();
   mTrash.unregisterObject();
   mSelectedSet.unregisterObject();
   mUndoManager.unregisterObject();
}

ConsoleMethod( GuiEditCtrl, setRoot, void, 3, 3, "(GuiControl root) Sets the given control as root\n"
              "@return No return value.")
{
   GuiControl *ctrl;
   if(!Sim::findObject(argv[2], ctrl))
      return;
   object->setRoot(ctrl);
}


ConsoleMethod( GuiEditCtrl, addNewCtrl, void, 3, 3, "(GuiControl ctrl) Adds the given control to the control list\n"
              "@return No return value.")
{
   GuiControl *ctrl;
   if(!Sim::findObject(argv[2], ctrl))
      return;
   object->addNewControl(ctrl);
}
ConsoleMethod( GuiEditCtrl, addSelection, void, 3, 3, "(ctrlID) Adds the selected control.\n"
              "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->addSelection(id);
}
ConsoleMethod( GuiEditCtrl, removeSelection, void, 3, 3, "(ctrlID) Removes the selected control from list.\n"
              "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->removeSelection(id);
}

ConsoleMethod( GuiEditCtrl, clearSelection, void, 2, 2, "() Clear selected controls list.\n"
              "@return No return value.")
{
   object->clearSelection();
}
ConsoleMethod( GuiEditCtrl, select, void, 3, 3, "(GuiControl ctrl) Finds and selects given object\n"
              "@return No return value.")
{
   GuiControl *ctrl;

   if(!Sim::findObject(argv[2], ctrl))
      return;

   object->setSelection(ctrl, false);
}

ConsoleMethod( GuiEditCtrl, setCurrentAddSet, void, 3, 3, "(GuiControl ctrl) Set the current control set in which controls are added.\n"
              "@param ctrl The addset\n"
              "@return No return value.")
{
   GuiControl *addSet;

   if (!Sim::findObject(argv[2], addSet))
   {
      Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
      return;
   }
   object->setCurrentAddSet(addSet);
}

ConsoleMethod( GuiEditCtrl, getCurrentAddSet, S32, 2, 2, "()\n @return Returns the set to which new controls will be added")
{
   const GuiControl* add = object->getCurrentAddSet();
   return add ? add->getId() : 0;
}

ConsoleMethod( GuiEditCtrl, toggle, void, 2, 2, "() Toggle activation.\n"
              "@return No return value.")
{
   object->setEditMode(! object->mActive);
}

ConsoleMethod( GuiEditCtrl, justify, void, 3, 3, "(int mode) Sets justification mode of selection\n"
              "@return No return value." )
{
   object->justifySelection((GuiEditCtrl::Justification)dAtoi(argv[2]));
}

ConsoleMethod( GuiEditCtrl, bringToFront, void, 2, 2, "() Brings control to front\n"
              "@return No return value.")
{
   object->bringToFront();
}

ConsoleMethod( GuiEditCtrl, pushToBack, void, 2, 2, "() Sends control to back\n"
              "@return No return value.")
{
   object->pushToBack();
}

ConsoleMethod( GuiEditCtrl, deleteSelection, void, 2, 2, "Delete the selected text.\n"
              "@return No return value.")
{
   object->deleteSelection();
}

ConsoleMethod( GuiEditCtrl, moveSelection, void, 4, 4, "(int deltax, int deltay) Moves selection to given (relative to current position) point\n"
              "@param deltax,deltay The change in coordinates.\n"
              "@return No return value.")
{
   object->moveAndSnapSelection(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
}

ConsoleMethod( GuiEditCtrl, saveSelection, void, 3, 3, "(string fileName) Saves the current selection to given filename\n"
              "@return No return value.")
{
   object->saveSelection(argv[2]);
}

ConsoleMethod( GuiEditCtrl, loadSelection, void, 3, 3, "(string fileName) Loads from given filename\n"
              "@return No return value.")
{
   object->loadSelection(argv[2]);
}

ConsoleMethod( GuiEditCtrl, selectAll, void, 2, 2, "() Selects all controls in list\n"
              "@return No return value.")
{
   object->selectAll();
}


ConsoleMethod( GuiEditCtrl, getSelected, S32, 2, 2, "() Gets the GUI control(s) the editor is currently selecting\n"
              "@return Returns the ID of the control.")
{
   return object->getSelectedSet().getId();
}

ConsoleMethod( GuiEditCtrl, getTrash, S32, 2, 2, "() Gets the GUI controls(s) that are currently in the trash.\n"
              "@return Returns the ID of the control")
{
   return object->getTrash().getId();
}

ConsoleMethod( GuiEditCtrl, getUndoManager, S32, 2, 2, "() Gets the Gui Editor's UndoManager object\n"
              "@return Returns the ID of the object.")
{
   return object->getUndoManager().getId();
}

bool GuiEditCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   // Set GUI Controls to DesignTime mode
   GuiControl::smDesignTime = true;
   GuiControl::smEditorHandle = this;

   setEditMode(true);

   // TODO: paxorr: I'm not sure this is the best way to set these defaults.
   bool snap = Con::getBoolVariable("$pref::GuiEditor::snap2grid",false);
   U32 grid = Con::getIntVariable("$pref::GuiEditor::snap2gridsize",8);
   Con::setBoolVariable("$pref::GuiEditor::snap2grid", snap);
   Con::setIntVariable("$pref::GuiEditor::snap2gridsize",grid);

   setSnapToGrid( snap ? grid : 0);

   return true;
}

void GuiEditCtrl::onSleep()
{
   // Set GUI Controls to run time mode
   GuiControl::smDesignTime = false;
   GuiControl::smEditorHandle = NULL;

   Parent::onSleep();
}
void GuiEditCtrl::setRoot(GuiControl *root)
{
   mContentControl = root;
   if( root != NULL ) root->mIsContainer = true;
    mCurrentAddSet = mContentControl;
   Con::executef(this, 1, "onClearSelected");
    mSelectedControls.clear();
}

enum GuiEditConstants {
   GUI_BLACK = 0,
   GUI_WHITE = 255,
   NUT_SIZE = 4
};

// Sizing Cursors
bool GuiEditCtrl::initCursors()
{
   if (mMoveCursor == NULL || mUpDownCursor == NULL || mLeftRightCursor == NULL || mDefaultCursor == NULL || mNWSECursor == NULL || mNESWCursor == NULL)
   {
      SimObject *obj;
      obj = Sim::findObject("MoveCursor");
      mMoveCursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("UpDownCursor");
      mUpDownCursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("LeftRightCursor");
      mLeftRightCursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("DefaultCursor");
      mDefaultCursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("NESWCursor");
      mNESWCursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("NWSECursor");
      mNWSECursor = dynamic_cast<GuiCursor*>(obj);
      obj = Sim::findObject("MoveCursor");
      mMoveCursor = dynamic_cast<GuiCursor*>(obj);

      return(mMoveCursor != NULL && mUpDownCursor != NULL && mLeftRightCursor != NULL && mDefaultCursor != NULL && mNWSECursor != NULL && mNESWCursor != NULL && mMoveCursor != NULL);
   }
   else
      return(true);
}

void GuiEditCtrl::setEditMode(bool value)
{
   mActive = value;

   Con::executef(this, 1, "onClearSelected");
   mSelectedControls.clear();
   if (mActive && mAwake)
      mCurrentAddSet = NULL;
}

void GuiEditCtrl::setCurrentAddSet(GuiControl *ctrl, bool clearSelection)
{
   if (ctrl != mCurrentAddSet)
   {
      if(clearSelection)
      {
         Con::executef(this, 1, "onClearSelected");
         mSelectedControls.clear();
      }
      mCurrentAddSet = ctrl;
   }
}

const GuiControl* GuiEditCtrl::getCurrentAddSet() const
{
   return mCurrentAddSet ? mCurrentAddSet : mContentControl;
}

void GuiEditCtrl::clearSelection(void)
{
   mSelectedControls.clear();
}
void GuiEditCtrl::setSelection(GuiControl *ctrl, bool inclusive)
{
   //sanity check
   if (! ctrl)
      return;

   if(mContentControl == ctrl)
   {
      mCurrentAddSet = ctrl;
      Con::executef(this, 1, "onClearSelected");
      mSelectedControls.clear();
   }
   else
   {
      // otherwise, we hit a new control...
      GuiControl *newAddSet = ctrl->getParent();

      //see if we should clear the old selection set
      if (newAddSet != mCurrentAddSet || (! inclusive)) {
         Con::executef(this, 1, "onClearSelected");
         mSelectedControls.clear();
      }

      //set the selection
      mCurrentAddSet = newAddSet;
      //if (!(ctrl->isLocked())) {
         mSelectedControls.push_back(ctrl);
      Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
      //}
   }
}

void GuiEditCtrl::addNewControl(GuiControl *ctrl)
{
   if (! mCurrentAddSet)
      mCurrentAddSet = mContentControl;

   mCurrentAddSet->addObject(ctrl);
   Con::executef(this, 1, "onClearSelected");
   mSelectedControls.clear();
   //if (!(ctrl->isLocked())) {
      mSelectedControls.push_back(ctrl);
      Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
   //}
   // undo
   Con::executef(this, 2, "onAddNewCtrl", Con::getIntArg(ctrl->getId()));
}

void GuiEditCtrl::drawNut(const Point2I &nut, ColorI &outlineColor, ColorI &nutColor)
{
   RectI r(nut.x - NUT_SIZE, nut.y - NUT_SIZE, 2 * NUT_SIZE, 2 * NUT_SIZE);
   r.inset(1, 1);
   dglDrawRectFill(r, nutColor);
   r.inset(-1, -1);
   dglDrawRect(r, outlineColor);
}

static inline bool inNut(const Point2I &pt, S32 x, S32 y)
{
   S32 dx = pt.x - x;
   S32 dy = pt.y - y;
   return dx <= NUT_SIZE && dx >= -NUT_SIZE && dy <= NUT_SIZE && dy >= -NUT_SIZE;
}

S32 GuiEditCtrl::getSizingHitKnobs(const Point2I &pt, const RectI &box)
{
   S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
   S32 cx = (lx + rx) >> 1;
   S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
   S32 cy = (ty + by) >> 1;
   
   // adjust nuts, so they dont straddle the controls
   lx -= NUT_SIZE;
   ty -= NUT_SIZE;
   rx += NUT_SIZE;
   by += NUT_SIZE;

   if (inNut(pt, lx, ty))
      return sizingLeft | sizingTop;
   if (inNut(pt, cx, ty))
      return sizingTop;
   if (inNut(pt, rx, ty))
      return sizingRight | sizingTop;
   if (inNut(pt, lx, by))
      return sizingLeft | sizingBottom;
   if (inNut(pt, cx, by))
      return sizingBottom;
   if (inNut(pt, rx, by))
      return sizingRight | sizingBottom;
   if (inNut(pt, lx, cy))
      return sizingLeft;
   if (inNut(pt, rx, cy))
      return sizingRight;
   return sizingNone;
}

void GuiEditCtrl::drawNuts(RectI &box, ColorI &outlineColor, ColorI &nutColor)
{
   S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
   S32 cx = (lx + rx) >> 1;
   S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
   S32 cy = (ty + by) >> 1;
   ColorF greenLine(0.0f, 1.0f, 0.0f ,0.6f);
   ColorF lightGreenLine(0.0f, 1.0f, 0.0f, 0.3f);
   if(lx > 0 && ty > 0)
   {
      dglDrawLine(0, ty, lx, ty, greenLine);
      dglDrawLine(lx, 0, lx, ty, greenLine);
   }
   if(lx > 0 && by > 0)
      dglDrawLine(0, by, lx, by, greenLine);

   if(rx > 0 && ty > 0)
      dglDrawLine(rx, 0, rx, ty, greenLine);

   Point2I extent = localToGlobalCoord(mBounds.extent);

   if(lx < extent.x && by < extent.y)
      dglDrawLine(lx, by, lx, extent.y, lightGreenLine);
   if(rx < extent.x && by < extent.y)
   {
      dglDrawLine(rx, by, rx, extent.y, lightGreenLine);
      dglDrawLine(rx, by, extent.x, by, lightGreenLine);
   }
   if(rx < extent.x && ty < extent.y)
      dglDrawLine(rx, ty, extent.x, ty, lightGreenLine);

   // adjust nuts, so they dont straddle the controls
   lx -= NUT_SIZE;
   ty -= NUT_SIZE;
   rx += NUT_SIZE;
   by += NUT_SIZE;

   drawNut(Point2I(lx, ty), outlineColor, nutColor);
   drawNut(Point2I(lx, cy), outlineColor, nutColor);
   drawNut(Point2I(lx, by), outlineColor, nutColor);
   drawNut(Point2I(rx, ty), outlineColor, nutColor);
   drawNut(Point2I(rx, cy), outlineColor, nutColor);
   drawNut(Point2I(rx, by), outlineColor, nutColor);
   drawNut(Point2I(cx, ty), outlineColor, nutColor);
   drawNut(Point2I(cx, by), outlineColor, nutColor);
}

void GuiEditCtrl::getDragRect(RectI &box)
{
   box.point.x = getMin(mLastMousePos.x, mSelectionAnchor.x);
   box.extent.x = getMax(mLastMousePos.x, mSelectionAnchor.x) - box.point.x + 1;
   box.point.y = getMin(mLastMousePos.y, mSelectionAnchor.y);
   box.extent.y = getMax(mLastMousePos.y, mSelectionAnchor.y) - box.point.y + 1;
}

void GuiEditCtrl::onPreRender()
{
   setUpdate();
}

void GuiEditCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   Point2I ctOffset;
   Point2I cext;
   bool keyFocused = isFirstResponder();

   if (mActive)
   {
      if (mCurrentAddSet)
      {
         // draw a white frame inset around the current add set.
         cext = mCurrentAddSet->getExtent();
         ctOffset = mCurrentAddSet->localToGlobalCoord(Point2I(0,0));
         RectI box(ctOffset.x, ctOffset.y, cext.x, cext.y);

            box.inset(-5, -5);
         dglDrawRect(box, ColorI(0, 101, 0,160));
            box.inset(1,1);
         dglDrawRect(box, ColorI(0, 101, 0,170));
            box.inset(1,1);
         dglDrawRect(box, ColorI(0, 101, 0,180));
            box.inset(1,1);
         dglDrawRect(box, ColorI(0, 101, 0,190));
            box.inset(1,1);
            dglDrawRect(box, ColorI(0, 101, 0,200));
      }
      Vector<GuiControl *>::iterator i;
      bool multisel = mSelectedControls.size() > 1;
      for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
      {
         GuiControl *ctrl = (*i);
         cext = ctrl->getExtent();
         ctOffset = ctrl->localToGlobalCoord(Point2I(0,0));
         RectI box(ctOffset.x,ctOffset.y, cext.x, cext.y);
         ColorI nutColor = multisel ? ColorI(255,255,255) : ColorI(0,0,0);
         ColorI outlineColor = multisel ? ColorI(0,0,0) : ColorI(255,255,255);
         if(!keyFocused)
            nutColor.set(128,128,128);

         drawNuts(box, outlineColor, nutColor);
      }
      if (mMouseDownMode == DragSelecting)
      {
         RectI b;
         getDragRect(b);
         b.point += offset;
         dglDrawRect(b, ColorI(255, 255, 255));
      }
   }

   renderChildControls(offset, mBounds, updateRect);

   if(mActive && mCurrentAddSet && (mGridSnap.x && mGridSnap.y) && 
       (mMouseDownMode == MovingSelection || mMouseDownMode == SizingSelection))
   {
      Point2I cext = mCurrentAddSet->getExtent();
      Point2I coff = mCurrentAddSet->localToGlobalCoord(Point2I(0,0));
      // create point-dots
      Point2I snap = mGridSnap;
      if(snap.x < 6)
         snap *= 2;
      if(snap.x < 6)
         snap *= 2;
      U32 maxdot = (cext.x / snap.x) * (cext.y / snap.y);
      Point2F* dots = new Point2F[maxdot];
      U32 ndot = 0;
      
      for(U32 ix = (U32)snap.x; ix < (U32)cext.x; ix += snap.x)
      { 
         for(U32 iy = snap.y; iy < (U32)cext.y; iy += snap.y)
         {
            dots[ndot].x = (F32)(ix + coff.x);
            dots[ndot++].y = (F32)(iy + coff.y);
         }
      }
      AssertFatal(ndot <= maxdot, "dot overflow");
      
      // draw the points.
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnable( GL_BLEND );
      glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

      glVertexPointer(2, GL_FLOAT, 0, dots);
      glColor4ub(50, 50, 254, 200);
      glDrawArrays( GL_POINTS, 0, ndot);
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisable(GL_BLEND);
      delete[] dots;
   }
}

bool GuiEditCtrl::selectionContains(GuiControl *ctrl)
{
   Vector<GuiControl *>::iterator i;
   for (i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
      if (ctrl == *i) return true;
   return false;
}

void GuiEditCtrl::onRightMouseDown(const GuiEvent &event)
{
   if (! mActive || !mContentControl)
   {
      Parent::onRightMouseDown(event);
      return;
   }
   setFirstResponder();

   //search for the control hit in any layer below the edit layer
   GuiControl *hitCtrl = mContentControl->findHitControl(globalToLocalCoord(event.mousePoint), mLayer - 1);
   if (hitCtrl != mCurrentAddSet)
   {
      Con::executef(this, 1, "onClearSelected");
      mSelectedControls.clear();
      mCurrentAddSet = hitCtrl;
   }
   // select the parent if we right-click on the current add set 
   else if( mCurrentAddSet != mContentControl)
   {
      mCurrentAddSet = hitCtrl->getParent();
      select(hitCtrl);
   }

   //Design time mouse events
   GuiEvent designEvent = event;
   designEvent.mousePoint = mLastMousePos;
   hitCtrl->onRightMouseDownEditor( designEvent, localToGlobalCoord( Point2I(0,0) ) );

}
void GuiEditCtrl::select(GuiControl *ctrl)
{
   Con::executef(this, 1, "onClearSelected");
   mSelectedControls.clear();
   if(ctrl != mContentControl) {
      //if (!(ctrl->isLocked())) {
         mSelectedControls.push_back(ctrl);
         Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
      //}
   }
   else
      mCurrentAddSet = mContentControl;
}

void GuiEditCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
   showCursor = true;
   
   Point2I ctOffset;
   Point2I cext;
   GuiControl *ctrl;

   Point2I mousePos  = globalToLocalCoord(lastGuiEvent.mousePoint);

   // first see if we hit a sizing knob on the currently selected control...
   if (mSelectedControls.size() == 1 && initCursors() == true )
   {
      ctrl = mSelectedControls.first();
      cext = ctrl->getExtent();
      ctOffset = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
      RectI box(ctOffset.x,ctOffset.y,cext.x, cext.y);

      GuiEditCtrl::sizingModes sizeMode = (GuiEditCtrl::sizingModes)getSizingHitKnobs(mousePos, box);

      if( mMouseDownMode == SizingSelection )
      {
         if ( ( mSizingMode == ( sizingBottom | sizingRight ) ) || ( mSizingMode == ( sizingTop | sizingLeft ) ) )
            cursor = mNWSECursor;
         else if (  ( mSizingMode == ( sizingBottom | sizingLeft ) ) || ( mSizingMode == ( sizingTop | sizingRight ) ) )
            cursor = mNESWCursor;
         else if ( mSizingMode == sizingLeft || mSizingMode == sizingRight ) 
            cursor = mLeftRightCursor;
         else if (mSizingMode == sizingTop || mSizingMode == sizingBottom )
            cursor = mUpDownCursor;
         else
            cursor = NULL;
      }
      else
      {
         // Check for current mouse position after checking for actual sizing mode
         if ( ( sizeMode == ( sizingBottom | sizingRight ) ) ||
            ( sizeMode == ( sizingTop | sizingLeft ) ) )
            cursor = mNWSECursor;
         else if ( ( sizeMode == ( sizingBottom | sizingLeft ) ) ||
            ( sizeMode == ( sizingTop | sizingRight ) ) )
            cursor = mNESWCursor;
         else if (sizeMode == sizingLeft || sizeMode == sizingRight )
            cursor = mLeftRightCursor;
         else if (sizeMode == sizingTop || sizeMode == sizingBottom )
            cursor = mUpDownCursor;
         else
            cursor = NULL;
      }
   }
   
   if( mMouseDownMode == MovingSelection && cursor == NULL )
       cursor = mMoveCursor;
}

void GuiEditCtrl::onTouchDown(const GuiEvent &event)
{
   if (! mActive)
   {
      Parent::onTouchDown(event);
      return;
   }
   if(!mContentControl)
      return;

   setFirstResponder();
   //lock the mouse
   mouseLock();

   Point2I ctOffset;
   Point2I cext;
   GuiControl *ctrl;

   mLastMousePos = globalToLocalCoord(event.mousePoint);

   // first see if we hit a sizing knob on the currently selected control...
   if (mSelectedControls.size() == 1)
   {
      ctrl = mSelectedControls.first();
      cext = ctrl->getExtent();
      ctOffset = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
      RectI box(ctOffset.x,ctOffset.y,cext.x, cext.y);

      if ((mSizingMode = (GuiEditCtrl::sizingModes)getSizingHitKnobs(mLastMousePos, box)) != 0)
      {
         mMouseDownMode = SizingSelection;
         // undo
         Con::executef(this, 2, "onPreEdit", Con::getIntArg(getSelectedSet().getId()));
         return;
      }
   }

   if(!mCurrentAddSet)
      mCurrentAddSet = mContentControl;

   //find the control we clicked
   ctrl = mContentControl->findHitControl(mLastMousePos, mCurrentAddSet->mLayer);

   bool handledEvent = ctrl->onMouseDownEditor( event, localToGlobalCoord( Point2I(0,0) ) );
   if( handledEvent == true )
   {
      // The Control handled the event and requested the edit ctrl
      // *NOT* act on it.  The dude abides.
      return;
   }
   else if ( selectionContains(ctrl) )
   {
      //if we're holding shift, de-select the clicked ctrl
      if (event.modifier & SI_SHIFT)
      {
         Vector<GuiControl *>::iterator i;
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
         {
            if (*i == ctrl)
            {
               Con::executef(this, 2, "onRemoveSelected", Con::getIntArg(ctrl->getId()));
               mSelectedControls.erase(i);
               break;
            }
         }

         //set the mode
         mMouseDownMode = Selecting;
      }
      else //else we hit a ctrl we've already selected, so set the mode to moving
      {
         // For calculating mouse delta
         mDragBeginPoint = event.mousePoint;

         // Allocate enough space for our selected controls
         mDragBeginPoints.reserve( mSelectedControls.size() );

         // For snapping to origin
         Vector<GuiControl *>::iterator i;
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            mDragBeginPoints.push_back( (*i)->mBounds.point );

         // Set Mouse Mode
         mMouseDownMode = MovingSelection;
         // undo
         Con::executef(this, 2, "onPreEdit", Con::getIntArg(getSelectedSet().getId()));
      }
   }

   //else we clicked on an unselected control
   else
   {
      //if we clicked in the current add set
      if (ctrl == mCurrentAddSet)
      {
         // start dragging a rectangle
         // if the shift is not down, nuke prior selection
         if (!(event.modifier & SI_SHIFT)) {
            Con::executef(this, 1, "onClearSelected");
            mSelectedControls.clear();
         }
         mSelectionAnchor = mLastMousePos;
         mMouseDownMode = DragSelecting;
      }
      else
      {
         //find the new add set
         GuiControl *newAddSet = ctrl->getParent();

         //if we're holding shift and the ctrl is in the same add set
         if (event.modifier & SI_SHIFT && newAddSet == mCurrentAddSet)
         {            	
            //if (!(ctrl->isLocked())) {
               mSelectedControls.push_back(ctrl);
               Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
            //}
            mMouseDownMode = Selecting;
         }
         else if (ctrl != mContentControl)
         {
            //find and set the new add set
            mCurrentAddSet = ctrl->getParent();

            //clear and set the selected controls
            Con::executef(this, 1, "onClearSelected");
            mSelectedControls.clear();
            //if (!(ctrl->isLocked())) {
               mSelectedControls.push_back(ctrl);
               Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
            //}
            mMouseDownMode = Selecting;
         }
         else
            mMouseDownMode = Selecting;
      }
   }
}
void GuiEditCtrl::addSelection(S32 id)
{
   GuiControl * ctrl;
   if(Sim::findObject(id, ctrl))
      mSelectedControls.push_back(ctrl);

}
void GuiEditCtrl::removeSelection(S32 id)
{
   GuiControl * ctrl;
   if (Sim::findObject(id, ctrl)) {
      Vector<GuiControl *>::iterator i;
      for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
      {
         if (*i == ctrl)
         {
            mSelectedControls.erase(i);
            break;
         }
      }
   }
}
void GuiEditCtrl::onTouchUp(const GuiEvent &event)
{
   if (! mActive || !mContentControl || !mCurrentAddSet )
   {
      Parent::onTouchUp(event);
      return;
   }

   //find the control we clicked
   GuiControl *ctrl = mContentControl->findHitControl(mLastMousePos, mCurrentAddSet->mLayer);

   bool handledEvent = ctrl->onMouseUpEditor( event, localToGlobalCoord( Point2I(0,0) ) );
   if( handledEvent == true )
   {
      // The Control handled the event and requested the edit ctrl
      // *NOT* act on it.  The dude abides.
      return;
   }

   //unlock the mouse
   mouseUnlock();

   // Reset Drag Axis Alignment Information
   mDragBeginPoint.set(-1,-1);
   mDragBeginPoints.clear();

   mLastMousePos = globalToLocalCoord(event.mousePoint);
   if (mMouseDownMode == DragSelecting)
   {
      RectI b;
      getDragRect(b);
      GuiControl::iterator i;
      for(i = mCurrentAddSet->begin(); i != mCurrentAddSet->end(); i++)
      {
         GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
         Point2I upperL = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
         Point2I lowerR = upperL + ctrl->mBounds.extent - Point2I(1, 1);

         if (b.pointInRect(upperL) && b.pointInRect(lowerR) && !selectionContains(ctrl)) {
            //if (!(ctrl->isLocked())) {
               mSelectedControls.push_back(ctrl);
               Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
            //}
         }
      }
   }
   if (mSelectedControls.size() == 1)
      Con::executef(this, 2, "onSelect", Con::getIntArg(mSelectedControls[0]->getId()));


   // deliver post edit event if we've been editing
   // note: paxorr: this may need to be moved earlier, if the selection has changed.
   // undo
   if(mMouseDownMode == SizingSelection || mMouseDownMode == MovingSelection)
      Con::executef(this, 2, "onPostEdit", Con::getIntArg(getSelectedSet().getId()));

   //reset the mouse mode
   setFirstResponder();
   mMouseDownMode = Selecting;
}

void GuiEditCtrl::onTouchDragged(const GuiEvent &event)
{
   if (! mActive || !mContentControl || !mCurrentAddSet)
   {
      Parent::onTouchDragged(event);
      return;
   }

   if(!mCurrentAddSet)
      mCurrentAddSet = mContentControl;

   Point2I mousePoint = globalToLocalCoord(event.mousePoint);

   //find the control we clicked
   GuiControl *ctrl = mContentControl->findHitControl(mousePoint, mCurrentAddSet->mLayer);

   bool handledEvent = ctrl->onMouseDraggedEditor( event, localToGlobalCoord( Point2I(0,0) ) );
   if( handledEvent == true )
   {
      // The Control handled the event and requested the edit ctrl
      // *NOT* act on it.  The dude abides.
      return;
   }

   if (mMouseDownMode == SizingSelection)
   {
      if (mGridSnap.x)
         mousePoint.x -= mousePoint.x % mGridSnap.x;
      if (mGridSnap.y)
         mousePoint.y -= mousePoint.y % mGridSnap.y;

      GuiControl *ctrl = mSelectedControls.first();

      // can't resize a locked control
      if (ctrl && ctrl->isLocked())
         return;

      Point2I ctrlPoint = mCurrentAddSet->globalToLocalCoord(event.mousePoint);
      if (mGridSnap.x)
         ctrlPoint.x -= ctrlPoint.x % mGridSnap.x;
      if (mGridSnap.y)
         ctrlPoint.y -= ctrlPoint.y % mGridSnap.y;

      Point2I newPosition = ctrl->getPosition();
      Point2I newExtent = ctrl->getExtent();
      Point2I minExtent = ctrl->getMinExtent();

      if (mSizingMode & sizingLeft)
      {
         newPosition.x = ctrlPoint.x;
         newExtent.x = ctrl->mBounds.extent.x + ctrl->mBounds.point.x - ctrlPoint.x;

         if(newExtent.x < minExtent.x)
         {
            newPosition.x -= minExtent.x - newExtent.x;
            newExtent.x = minExtent.x;
         }
      }
      else if (mSizingMode & sizingRight)
      {
         newExtent.x = ctrlPoint.x - ctrl->mBounds.point.x;
         if(mGridSnap.x)
            newExtent.x -= newExtent.x % mGridSnap.x;

         if(newExtent.x < minExtent.x)
            newExtent.x = minExtent.x;
      }

      if (mSizingMode & sizingTop)
      {
         newPosition.y = ctrlPoint.y;
         newExtent.y = ctrl->mBounds.extent.y + ctrl->mBounds.point.y - ctrlPoint.y;
         if(newExtent.y < minExtent.y)
         {
            newPosition.y -= minExtent.y - newExtent.y;
            newExtent.y = minExtent.y;
         }
      }
      else if (mSizingMode & sizingBottom)
      {
         newExtent.y = ctrlPoint.y - ctrl->mBounds.point.y;
         if(newExtent.y < minExtent.y)
            newExtent.y = minExtent.y;
      }

      if(mGridSnap.x)
      {
         newPosition.x -= newPosition.x % mGridSnap.x;
         newExtent.x -= newExtent.x % mGridSnap.x;
      }
      if(mGridSnap.y)
      {
         newPosition.y -= newPosition.y % mGridSnap.y;
         newExtent.y -= newExtent.y % mGridSnap.y;
      }

      ctrl->resize(newPosition, newExtent);
      mCurrentAddSet->childResized(ctrl);
      Con::executef(this, 2, "onSelect", Con::getIntArg(mSelectedControls[0]->getId()));
   }
   else if (mMouseDownMode == MovingSelection && mSelectedControls.size())
   {
      Vector<GuiControl *>::iterator i = mSelectedControls.begin();
      //Point2I minPos = (*i)->mBounds.point;
      Point2I minPos (S32_MAX, S32_MAX);

      for(; i != mSelectedControls.end(); i++)
      {
         // skip locked controls
         if ((*i)->isLocked())
            continue;

         if ((*i)->mBounds.point.x < minPos.x)
            minPos.x = (*i)->mBounds.point.x;
         if ((*i)->mBounds.point.y < minPos.y)
            minPos.y = (*i)->mBounds.point.y;
      }
      Point2I delta = mousePoint - mLastMousePos;
      delta += minPos; // find new minPos;

      if (mGridSnap.x)
         delta.x -= delta.x % mGridSnap.x;
      if (mGridSnap.y)
         delta.y -= delta.y % mGridSnap.y;

      delta -= minPos;

      // Do we want to align this drag to the X and Y axes within a certain threshold?
      if( event.modifier & SI_SHIFT )
      {
         Point2I dragTotalDelta = event.mousePoint - mDragBeginPoint;
         if( dragTotalDelta.y < 10 && dragTotalDelta.y > -10 )
         {
            for(S32 i = 0; i < mSelectedControls.size(); i++)
            {
               // skip locked controls
               if (mSelectedControls[i]->isLocked())
                  continue;

               Point2I snapBackPoint( mSelectedControls[i]->mBounds.point.x, mDragBeginPoints[i].y);
               // This is kind of nasty but we need to snap back if we're not at origin point with selection - JDD
               if( mSelectedControls[i]->mBounds.point.y != mDragBeginPoints[i].y )
                  mSelectedControls[i]->resize( snapBackPoint, mSelectedControls[i]->mBounds.extent);
            }
            delta.y = 0;
         }
         if( dragTotalDelta.x < 10 && dragTotalDelta.x > -10 )
         {
            for(S32 i = 0; i < mSelectedControls.size(); i++)
            {
               // skip locked controls
               if (mSelectedControls[i]->isLocked())
                  continue;

               Point2I snapBackPoint( mDragBeginPoints[i].x,mSelectedControls[i]->mBounds.point.y);
               // This is kind of nasty but we need to snap back if we're not at origin point with selection - JDD
               if( mSelectedControls[i]->mBounds.point.x != mDragBeginPoints[i].x )
                  mSelectedControls[i]->resize( snapBackPoint, mSelectedControls[i]->mBounds.extent);
            }
            delta.x = 0;
         }


      }

      moveSelection(delta);

      // find the current control under the mouse but not in the selected set.
      // setting a control invisible makes sure it wont be seen by findHitControl()
      for(int i = 0; i< mSelectedControls.size(); i++)
         mSelectedControls[i]->setVisible(false);
      GuiControl *inCtrl = mContentControl->findHitControl(mousePoint, mCurrentAddSet->mLayer);
      for(int i = 0; i< mSelectedControls.size(); i++)
         mSelectedControls[i]->setVisible(true);

      // find the nearest control up the heirarchy from the control the mouse is in
      // that is flagged as a container.
      while(! inCtrl->mIsContainer)
         inCtrl = inCtrl->getParent();
         
      // if the control under the mouse is not our parent, move the selected controls
      // into the new parent.
      if(mSelectedControls[0]->getParent() != inCtrl && inCtrl->mIsContainer)
      {
         moveSelectionToCtrl(inCtrl);
         setCurrentAddSet(inCtrl,false);
      }

      mLastMousePos += delta;
   }
   else
      mLastMousePos = mousePoint;
}

void GuiEditCtrl::moveSelectionToCtrl(GuiControl *newParent)
{
   for(int i = 0; i < mSelectedControls.size(); i++)
   {
      GuiControl* ctrl = mSelectedControls[i];
      if(ctrl->getParent() == newParent)
         continue;

      // skip locked controls
      if (ctrl->isLocked())
         continue;
   
      Point2I globalpos = ctrl->localToGlobalCoord(Point2I(0,0));
      newParent->addObject(ctrl);
      Point2I newpos = ctrl->globalToLocalCoord(globalpos) + ctrl->mBounds.point;
      ctrl->mBounds.set(newpos, ctrl->mBounds.extent);
   }

   Con::executef(this, 1, "onSelectionParentChange");
}

static Point2I snapPoint(Point2I point, Point2I delta, Point2I gridSnap)
{ 
   S32 snap;
   if(gridSnap.x && delta.x)
   {
      snap = point.x % gridSnap.x;
      point.x -= snap;
      if(delta.x > 0 && snap != 0)
         point.x += gridSnap.x;
   }
   if(gridSnap.y && delta.y)
   {
      snap = point.y % gridSnap.y;
      point.y -= snap;
      if(delta.y > 0 && snap != 0)
         point.y += gridSnap.y;
   }
   return point;
}
//-----------------
void GuiEditCtrl::moveAndSnapSelection(const Point2I &delta)
{
   // move / nudge gets a special callback so that multiple small moves can be
   // coalesced into one large undo action.
   // undo
   Con::executef(this, 2, "onPreSelectionNudged", Con::getIntArg(getSelectedSet().getId()));

   Vector<GuiControl *>::iterator i;
   Point2I newPos;
   for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
   {
      newPos = (*i)->mBounds.point + delta;
      newPos = snapPoint(newPos, delta, mGridSnap);
      (*i)->resize(newPos, (*i)->mBounds.extent);
   }

   // undo
   Con::executef(this, 2, "onPostSelectionNudged", Con::getIntArg(getSelectedSet().getId()));

   // allow script to update the inspector
   if (mSelectedControls.size() == 1)
      Con::executef(this, 2, "onSelectionMoved", Con::getIntArg(mSelectedControls[0]->getId()));
}

void GuiEditCtrl::moveSelection(const Point2I &delta)
{
   Vector<GuiControl *>::iterator i;
   for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
   {
      // skip locked controls
      if ((*i)->isLocked())
         continue;

      (*i)->resize((*i)->mBounds.point + delta, (*i)->mBounds.extent);
   }

   // allow script to update the inspector
   if (mSelectedControls.size() == 1)
      Con::executef(this, 2, "onSelectionMoved", Con::getIntArg(mSelectedControls[0]->getId()));
}

void GuiEditCtrl::justifySelection(Justification j)
{
   S32 minX, maxX;
   S32 minY, maxY;
   S32 extentX, extentY;

   if (mSelectedControls.size() < 2)
      return;

   Vector<GuiControl *>::iterator i = mSelectedControls.begin();
   minX = (*i)->mBounds.point.x;
   maxX = minX + (*i)->mBounds.extent.x;
   minY = (*i)->mBounds.point.y;
   maxY = minY + (*i)->mBounds.extent.y;
   extentX = (*i)->mBounds.extent.x;
   extentY = (*i)->mBounds.extent.y;
   i++;
   for(;i != mSelectedControls.end(); i++)
   {
      minX = getMin(minX, (*i)->mBounds.point.x);
      maxX = getMax(maxX, (*i)->mBounds.point.x + (*i)->mBounds.extent.x);
      minY = getMin(minY, (*i)->mBounds.point.y);
      maxY = getMax(maxY, (*i)->mBounds.point.y + (*i)->mBounds.extent.y);
      extentX += (*i)->mBounds.extent.x;
      extentY += (*i)->mBounds.extent.y;
   }
   S32 deltaX = maxX - minX;
   S32 deltaY = maxY - minY;
   switch(j)
   {
      case JUSTIFY_LEFT:
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            (*i)->resize(Point2I(minX, (*i)->mBounds.point.y), (*i)->mBounds.extent);
         break;
      case JUSTIFY_TOP:
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            (*i)->resize(Point2I((*i)->mBounds.point.x, minY), (*i)->mBounds.extent);
         break;
      case JUSTIFY_RIGHT:
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            (*i)->resize(Point2I(maxX - (*i)->mBounds.extent.x + 1, (*i)->mBounds.point.y), (*i)->mBounds.extent);
         break;
      case JUSTIFY_BOTTOM:
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            (*i)->resize(Point2I((*i)->mBounds.point.x, maxY - (*i)->mBounds.extent.y + 1), (*i)->mBounds.extent);
         break;
      case JUSTIFY_CENTER:
         for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            (*i)->resize(Point2I(minX + ((deltaX - (*i)->mBounds.extent.x) >> 1), (*i)->mBounds.point.y),
                                                                                 (*i)->mBounds.extent);
         break;
      case SPACING_VERTICAL:
         {
            Vector<GuiControl *> sortedList;
            Vector<GuiControl *>::iterator k;
            for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            {
               for(k = sortedList.begin(); k != sortedList.end(); k++)
               {
                  if ((*i)->mBounds.point.y < (*k)->mBounds.point.y)
                     break;
               }
               sortedList.insert(k, *i);
            }
            S32 space = (deltaY - extentY) / (mSelectedControls.size() - 1);
            S32 curY = minY;
            for(k = sortedList.begin(); k != sortedList.end(); k++)
            {
               (*k)->resize(Point2I((*k)->mBounds.point.x, curY), (*k)->mBounds.extent);
               curY += (*k)->mBounds.extent.y + space;
            }
         }
         break;
      case SPACING_HORIZONTAL:
         {
            Vector<GuiControl *> sortedList;
            Vector<GuiControl *>::iterator k;
            for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
            {
               for(k = sortedList.begin(); k != sortedList.end(); k++)
               {
                  if ((*i)->mBounds.point.x < (*k)->mBounds.point.x)
                     break;
               }
               sortedList.insert(k, *i);
            }
            S32 space = (deltaX - extentX) / (mSelectedControls.size() - 1);
            S32 curX = minX;
            for(k = sortedList.begin(); k != sortedList.end(); k++)
            {
               (*k)->resize(Point2I(curX, (*k)->mBounds.point.y), (*k)->mBounds.extent);
               curX += (*k)->mBounds.extent.x + space;
            }
         }
         break;
   }
}

void GuiEditCtrl::deleteSelection(void)
{
   // undo
   Con::executef(this, 2, "onTrashSelection", Con::getIntArg(getSelectedSet().getId()));

   Vector<GuiControl *>::iterator i;
   for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
   {
      mTrash.addObject(*i);
   }
   mSelectedControls.clear();
}

void GuiEditCtrl::loadSelection(const char* filename)
{
   if (! mCurrentAddSet)
      mCurrentAddSet = mContentControl;

   Con::executef(2, "exec", filename);
   SimSet *set;
   if(!Sim::findObject("guiClipboard", set))
      return;

   if(set->size())
   {
      Con::executef(this, 1, "onClearSelected");
      mSelectedControls.clear();
      for(U32 i = 0; i < (U32)set->size(); i++)
      {
         GuiControl *ctrl = dynamic_cast<GuiControl *>((*set)[i]);
         if(ctrl)
         {
            mCurrentAddSet->addObject(ctrl);
            mSelectedControls.push_back(ctrl);
            Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
         }
      }
      // Undo 
      Con::executef(this, 2, "onAddNewCtrlSet", Con::getIntArg(getSelectedSet().getId()));
   }
   set->deleteObject();
}

void GuiEditCtrl::saveSelection(const char* filename)
{
   // if there are no selected objects, then don't save
   if (mSelectedControls.size() == 0)
      return;

   FileStream stream;
   if(!ResourceManager->openFileForWrite(stream, filename))
      return;
   SimSet *clipboardSet = new SimSet;
   clipboardSet->registerObject();
   Sim::getRootGroup()->addObject(clipboardSet, "guiClipboard");

   Vector<GuiControl *>::iterator i;
   for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
      clipboardSet->addObject(*i);

   clipboardSet->write(stream, 0);
   clipboardSet->deleteObject();
}

void GuiEditCtrl::selectAll()
{
   GuiControl::iterator i;
   if (!mCurrentAddSet)
      return;
   Con::executef(this, 1, "onClearSelected");
   mSelectedControls.clear();
   for(i = mCurrentAddSet->begin(); i != mCurrentAddSet->end(); i++)
   {
      GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
      //if (!(ctrl->isLocked())) {
         mSelectedControls.push_back(ctrl);
         Con::executef(this, 2, "onAddSelected", Con::getIntArg(ctrl->getId()));
      //}
   }
}

void GuiEditCtrl::bringToFront()
{
   // undo
   if (mSelectedControls.size() != 1)
      return;

   GuiControl *ctrl = *(mSelectedControls.begin());
   mCurrentAddSet->pushObjectToBack(ctrl);
}

void GuiEditCtrl::pushToBack()
{
   // undo
   if (mSelectedControls.size() != 1)
      return;

   GuiControl *ctrl = *(mSelectedControls.begin());
   mCurrentAddSet->bringObjectToFront(ctrl);
}

bool GuiEditCtrl::onKeyDown(const GuiEvent &event)
{
   if (! mActive)
      return Parent::onKeyDown(event);

   if (!(event.modifier & SI_CTRL))
   {
      switch(event.keyCode)
      {
         case KEY_BACKSPACE:
         case KEY_DELETE:
            deleteSelection();
            Con::executef(this,1,"onDelete");
            return true;
      }
   }
   return false;
}

ConsoleMethod(GuiEditCtrl, setSnapToGrid, void, 3, 3, "(gridsize) Set the size of the snap-to grid.\n"
              "@return No return value.")
{
   U32 gridsize = dAtoi(argv[2]);
   object->setSnapToGrid(gridsize);
}

void GuiEditCtrl::setSnapToGrid(U32 gridsize)
{
   mGridSnap.set(gridsize, gridsize);
}


void GuiEditCtrl::controlInspectPreApply(GuiControl* object)
{
   // undo
   Con::executef(this, 2, "onControlInspectPreApply", Con::getIntArg(object->getId()));
}

void GuiEditCtrl::controlInspectPostApply(GuiControl* object)
{
   // undo
   Con::executef(this, 2, "onControlInspectPostApply", Con::getIntArg(object->getId()));
}


void GuiEditCtrl::updateSelectedSet()
{
   mSelectedSet.clear();
   Vector<GuiControl*>::iterator i;
   for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
   {
      mSelectedSet.addObject(*i);
   }
}

// -----------------------------------------------------------------------------
// GuiEditor Ruler
// -----------------------------------------------------------------------------
class GuiEditorRuler : public GuiControl {
   StringTableEntry refCtrl;
   typedef GuiControl Parent;
public:

   void onPreRender()
   {
      setUpdate();
   }
   void onRender(Point2I offset, const RectI &updateRect)
   {
      dglDrawRectFill(updateRect, ColorF(1,1,1,1));
      GuiScrollCtrl *ref;
      SimObject *o = Sim::findObject(refCtrl);

      //Sim::findObject(refCtrl, &ref);
      ref = dynamic_cast<GuiScrollCtrl *>(o);
      Point2I choffset(0,0);
      //if(ref)
      //   choffset = ref->getChildPos();
      if(mBounds.extent.x > mBounds.extent.y)
      {
         // it's horizontal.
         for(U32 i = 0; i < (U32)mBounds.extent.x; i++)
         {
            S32 x = offset.x + i;
            S32 pos = i - choffset.x;
            if(!(pos % 10))
            {
               S32 start = 6;
               if(!(pos %20))
                  start = 4;
               if(!(pos % 100))
                  start = 1;
               dglDrawLine(x, offset.y + start, x, offset.y + 10, ColorF(0,0,0,1));
            }
         }
      }
      else
      {
         // it's vertical.
         for(U32 i = 0; i < (U32)mBounds.extent.y; i++)
         {
            S32 y = offset.y + i;
            S32 pos = i - choffset.y;
            if(!(pos % 10))
            {
               S32 start = 6;
               if(!(pos %20))
                  start = 4;
               if(!(pos % 100))
                  start = 1;
               dglDrawLine(offset.x + start, y, offset.x + 10, y, ColorF(0,0,0,1));
            }
         }
      }
   }
   static void initPersistFields()
   {
      Parent::initPersistFields();
      addField("refCtrl", TypeString, Offset(refCtrl, GuiEditorRuler));
   }
   DECLARE_CONOBJECT(GuiEditorRuler);
};

IMPLEMENT_CONOBJECT(GuiEditorRuler);
