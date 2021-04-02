#include "2d/editorToy/EditorToySceneWindow.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "platform/event.h"
#include "2d/editorToy/EditorToyTool.h"

#include "2d/editorToy/EditorToySceneWindow_ScriptBinding.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToySceneWindow);

//-----------------------------------------------------------------------

bool EditorToySceneWindow::inNut(const Point2I &pt, S32 x, S32 y)
{
   //S32 checkSize = mNutSize << 1;
   // check area relative to nut size.
   Point2I cTL(-(mNutSize *2), -(mNutSize * 2));
   Point2I cBR((mNutSize * 2), (mNutSize * 2));
   S32 dx = pt.x - x;
   S32 dy = pt.y - y;
   return dx <= cBR.x && dx >= cTL.x && dy <= cBR.y && dy >= cTL.y;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::updatedSelection()
{
   mSelSet.clear();
   Vector<SceneObject*>::iterator i;
   for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
   {
      mSelSet.addObject(*i);
   }
}

EditorToySceneWindow::EditorToySceneWindow()
   :  mGridSnap(1.0f,1.0f),
      mSnapThreshold(0.2f),
      mSceneCamPos(0.0f, 0.0f),
      mSceneCamSize(16.0f, 9.0f),
      mDragBegin(0,0)
{
   VECTOR_SET_ASSOCIATION(mSelectedObjs);
   VECTOR_SET_ASSOCIATION(mDragBeginPoints);
   
   mActive = true;
   mDragBeginPoints.clear();
   mSelectedObjs.clear();
   mMouseMode = Selecting;
   mSizeMode = SizingNone;
   mDropType = DropAtScreenCenter;
   mActiveTool = nullptr;
   mNutSize = 5;

   mDefaultCursor = NULL;
   mLeftRightCursor = NULL;
   mUpDownCursor = NULL;
   mMoveCursor = NULL;

   mAllCam = true;
   mDrawCam = true;
   mDrawAspectSafety = true;
   mDrawSaftey = true;

}

bool EditorToySceneWindow::onAdd()
{
   if (!Parent::onAdd())
      return false;

   mTrash.registerObject();
   mSelSet.registerObject();
   mUndoManager.registerObject();

   return true;
}

void EditorToySceneWindow::onRemove()
{
   Parent::onRemove();
   mTrash.unregisterObject();
   mSelSet.unregisterObject();
   mUndoManager.unregisterObject();
}

//-----------------------------------------------------------------------

EditorToySceneWindow::~EditorToySceneWindow()
{
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setEditScene(Scene* pScene)
{
   Parent::setScene(pScene);
   if (getScene())
      getScene()->setIsEditorScene(true);

   mCurrentScene = pScene;

   // find out if these vars are set. 
   const char* pos = mCurrentScene->getDataField("CamPos", NULL);

   // if not load some defaults.
   if (Utility::mGetStringElementCount(pos) != 2)
   {
      mCurrentScene->setDataField("CamPos", NULL, "0.0 0.0");
      pos = "0.0 0.0";
   }

   const char* size = mCurrentScene->getDataField("CamSize", NULL);

   if (Utility::mGetStringElementCount(size) != 2)
   {
      mCurrentScene->setDataField("CamSize", NULL, "16.0 9.0");
      size = "16.0 9.0";
   }

   mSceneCamPos = Utility::mGetStringElementVector(pos);
   mSceneCamSize = Utility::mGetStringElementVector(size);

   mSceneCamRect = updateSceneCamera();
}

//-----------------------------------------------------------------------

RectI EditorToySceneWindow::updateSceneCamera()
{
   Vector2 uppLeft = mSceneCamPos - (mSceneCamSize * 0.5f);

   Vector2 lowRight = mSceneCamPos + (mSceneCamSize * 0.5f);

   sceneToWindowPoint(uppLeft, uppLeft);
   sceneToWindowPoint(lowRight, lowRight);
   
   Point2I winUppLeft = localToGlobalCoord(Point2I(uppLeft.x, uppLeft.y));
   Point2I winLowRight = localToGlobalCoord(Point2I(lowRight.x, lowRight.y));

   return RectI((S32)winUppLeft.x, (S32)winUppLeft.y,
                         S32(winLowRight.x - winUppLeft.x),
                         S32(winLowRight.y - winUppLeft.y));

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::resetEditScene()
{
   if (getScene())
      getScene()->setIsEditorScene(false);

   Parent::resetScene();
}

//-----------------------------------------------------------------------



S32 EditorToySceneWindow::getNutHit(const Point2I &pt, const RectI &box)
{
   S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
   S32 cx = (lx + rx) >> 1;
   S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
   S32 cy = (ty + by) >> 1;

   if (inNut(pt, lx, ty))
      return SizingLeft | SizingTop;
   if (inNut(pt, rx, ty))
      return SizingRight | SizingTop;
   if (inNut(pt, lx, by))
      return SizingLeft | SizingBottom;
   if (inNut(pt, rx, by))
      return SizingRight | SizingBottom;
   
   return SizingNone;
}

S32 EditorToySceneWindow::getDirNutHit(const Point2I & pt, SceneObject * obj)
{
   // vars
   Vector2 pos = obj->getPosition();
   Vector2 size = obj->getSize();
   F32 ang = obj->getAngle();

   // right
   Vector2 right = right.getVecFromAng(pos, ang, (size.x*0.5f));
   // top
   Vector2 top = top.getVecFromAng(pos, ang + mDegToRad(90.0f), (size.y*0.5f));
   // left
   Vector2 left = left.getVecFromAng(pos, ang - mDegToRad(180.0f), (size.x*0.5f));
   // bottom
   Vector2 bottom = bottom.getVecFromAng(pos, ang - mDegToRad(90.0f), (size.y*0.5f));

   // cardinal
   Vector2 r;
   Vector2 l;
   Vector2 t;
   Vector2 b;

   // cardinal
   sceneToWindowPoint(right, r);
   sceneToWindowPoint(left, l);
   sceneToWindowPoint(top, t);
   sceneToWindowPoint(bottom, b);

   F32 deg = mRadToDeg(ang);

   if (deg == 90.0f)
   {
      if (inNut(pt, (S32)(l.x), (S32)(l.y)))
         return SizingBottom;
      if (inNut(pt, (S32)(r.x), (S32)(r.y)))
         return SizingTop;
      if (inNut(pt, (S32)(b.x), (S32)(b.y)))
         return SizingRight;
      if (inNut(pt, (S32)(t.x), (S32)(t.y)))
         return SizingLeft;
   }
   else if (deg == 180.0f || deg == -180.0f)
   {
      if (inNut(pt, (S32)(l.x), (S32)(l.y)))
         return SizingRight;
      if (inNut(pt, (S32)(r.x), (S32)(r.y)))
         return SizingLeft;
      if (inNut(pt, (S32)(b.x), (S32)(b.y)))
         return SizingTop;
      if (inNut(pt, (S32)(t.x), (S32)(t.y)))
         return SizingBottom;
   }
   else if (deg == -90.0f)
   {
      if (inNut(pt, (S32)(l.x), (S32)(l.y)))
         return SizingTop;
      if (inNut(pt, (S32)(r.x), (S32)(r.y)))
         return SizingBottom;
      if (inNut(pt, (S32)(b.x), (S32)(b.y)))
         return SizingLeft;
      if (inNut(pt, (S32)(t.x), (S32)(t.y)))
         return SizingRight;
   }
   else
   {
      if (inNut(pt, (S32)(l.x), (S32)(l.y)))
         return SizingLeft;
      if (inNut(pt, (S32)(r.x), (S32)(r.y)))
         return SizingRight;
      if (inNut(pt, (S32)(b.x), (S32)(b.y)))
         return SizingBottom;
      if (inNut(pt, (S32)(t.x), (S32)(t.y)))
         return SizingTop;
   }

   return SizingNone;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::drawNut(const Point2I & nut, ColorI & outlineColor, ColorI & nutColor)
{
   dglDrawCircleFill(nut,(F32)mNutSize, nutColor);
   dglDrawCircle(nut, (F32)mNutSize, outlineColor);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::drawSizingNuts(SceneObject* obj, ColorI &outlineColor, ColorI &nutColor)
{
   // vars
   // there has to be a better way...
   Vector2 pos = obj->getPosition();
   Vector2 size = obj->getSize();
   F32 ang = obj->getAngle();

   // right
   Vector2 right = right.getVecFromAng(pos, ang, (size.x*0.5f));
   Vector2 topRight = topRight.getVecFromAng(right, (ang + 1.5708f), (size.y*0.5f));
   // top
   Vector2 top = top.getVecFromAng(pos, (ang + 1.5708f), (size.y*0.5f));
   Vector2 topLeft = topLeft.getVecFromAng(top, ((ang + 1.5708f) + 1.5708f), (size.x*0.5f));
   // left
   Vector2 left = left.getVecFromAng(pos, ang + 3.14159f, (size.x*0.5f));
   Vector2 bottomLeft = bottomLeft.getVecFromAng(left, (ang - 1.5708f), (size.y*0.5f));
   // bottom
   Vector2 bottom = bottom.getVecFromAng(pos, ang - 1.5708f, (size.y*0.5f));
   Vector2 bottomRight = bottomRight.getVecFromAng(bottom, ang, (size.x*0.5f));

   // cardinal
   Vector2 r;
   Vector2 l;
   Vector2 t;
   Vector2 b;
   // corners
   Vector2 tr;
   Vector2 tl;
   Vector2 bl;
   Vector2 br;

   // cardinal
   sceneToWindowPoint(right, r);
   sceneToWindowPoint(left, l);
   sceneToWindowPoint(top, t);
   sceneToWindowPoint(bottom, b);
   // corners
   sceneToWindowPoint(topRight, tr);
   sceneToWindowPoint(topLeft, tl);
   sceneToWindowPoint(bottomLeft, bl);
   sceneToWindowPoint(bottomRight, br);

   // draw lines
   dglDrawLine((S32)(tr.x), (S32)(tr.y), (S32)(tl.x), (S32)(tl.y), outlineColor );
   dglDrawLine((S32)(tl.x), (S32)(tl.y), (S32)(bl.x), (S32)(bl.y),outlineColor );
   dglDrawLine((S32)(bl.x), (S32)(bl.y), (S32)(br.x), (S32)(br.y),outlineColor );
   dglDrawLine((S32)(br.x), (S32)(br.y), (S32)(tr.x), (S32)(tr.y), outlineColor );

   drawNut(Point2I((S32)(r.x), (S32)r.y), outlineColor, nutColor);
   drawNut(Point2I((S32)(l.x), (S32)l.y), outlineColor, nutColor);
   drawNut(Point2I((S32)(t.x), (S32)(t.y)), outlineColor, nutColor);
   drawNut(Point2I((S32)(b.x), (S32)(b.y)), outlineColor, nutColor);
}

bool EditorToySceneWindow::getRotateNutHit(const Point2I & pt, SceneObject* obj)
{
   // vars
   Vector2 pos = obj->getPosition();
   Vector2 size = obj->getSize();
   F32 ang = obj->getAngle();

   // right
   Vector2 edge = edge.getVecFromAng(pos, ang, (size.x*0.5f));
   Vector2 rotate = rotate.getVecFromAng(edge, ang, (size.x*0.5f));

   Vector2 r;

   sceneToWindowPoint(rotate, r);

   return inNut(pt, (S32)(r.x), (S32)(r.y));

}

void EditorToySceneWindow::drawRotateNuts(SceneObject* obj, ColorI &outlineColor, ColorI &nutColor)
{

   // vars
   Vector2 pos = obj->getPosition();
   Vector2 size = obj->getSize();
   F32 ang = obj->getAngle();

   // right
   Vector2 edge = edge.getVecFromAng(pos,ang,(size.x*0.5f));
   Vector2 rotate = rotate.getVecFromAng(edge,ang,(size.x*0.5f));

   Vector2 e;
   Vector2 r;

   sceneToWindowPoint(edge, e);
   sceneToWindowPoint(rotate, r);

   // draw lines
   dglDrawLine((S32)(e.x), (S32)(e.y), (S32)(r.x), (S32)(r.y), outlineColor);
   drawNut(Point2I((S32)(r.x), (S32)r.y), outlineColor, nutColor);
}

//-----------------------------------------------------------------------

// Sizing Cursors
bool EditorToySceneWindow::initCursors()
{
   if (mMoveCursor == NULL || mUpDownCursor == NULL || mLeftRightCursor == NULL || mDefaultCursor == NULL)
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
      obj = Sim::findObject("MoveCursor");
      mMoveCursor = dynamic_cast<GuiCursor*>(obj);

      return(mMoveCursor != NULL && mUpDownCursor != NULL && mLeftRightCursor != NULL && mDefaultCursor != NULL && mMoveCursor != NULL);
   }
   else
      return(true);
}

void EditorToySceneWindow::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &gEvt)
{
   showCursor = true;

   SceneObject *obj;
   Point2I mousePos = globalToLocalCoord(gEvt.mousePoint);

   // first see if we hit a sizing knob on the currently selected control...
   if (mSelectedObjs.size() == 1 && initCursors() == true)
   {
      obj = mSelectedObjs.first();
      F32 deg = mRadToDeg(obj->getAngle());
      EditorToySceneWindow::SizingMode sizeMode = (EditorToySceneWindow::SizingMode)getDirNutHit(mousePos, obj);

      if (mMouseMode == SizingSelection)
      {
         if (mSizeMode == SizingLeft || mSizeMode == SizingRight)
            cursor = mLeftRightCursor;
         else if (mSizeMode == SizingTop || mSizeMode == SizingBottom)
            cursor = mUpDownCursor;
         else
            cursor = NULL;
      }
      else
      {
         // Check for current mouse position after checking for actual sizing mode
         if (sizeMode == SizingLeft || sizeMode == SizingRight)
            cursor = mLeftRightCursor;
         else if (sizeMode == SizingTop || sizeMode == SizingBottom)
            cursor = mUpDownCursor;
         else
            cursor = NULL;
      }
   }

   if (mMouseMode == MovingSelection && cursor == NULL)
      cursor = mMoveCursor;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::getDragRect(RectI &box)
{
   // draw the box in gui space. worldQuery to convert to scene.
   box.point.x = getMin(mLastMousePt.x, mSelectionAnchor.x);
   box.extent.x = getMax(mLastMousePt.x, mSelectionAnchor.x) - box.point.x + 1;
   box.point.y = getMin(mLastMousePt.y, mSelectionAnchor.y);
   box.extent.y = getMax(mLastMousePt.y, mSelectionAnchor.y) - box.point.y + 1;
}

//-----------------------------------------------------------------------

RectI EditorToySceneWindow::objWindowBounds(const SceneObject* obj)
{
   const RectF objAABBRect = obj->getAABBRectangle();

   Vector2 upLeft = Vector2(objAABBRect.point.x, objAABBRect.point.y + objAABBRect.extent.y);
   Vector2 lowRight = Vector2(objAABBRect.point.x + objAABBRect.extent.x, objAABBRect.point.y);

   Vector2 winUpLeft;
   Vector2 winLowRight;

   sceneToWindowPoint(upLeft, winUpLeft);
   sceneToWindowPoint(lowRight, winLowRight);

   return RectI(S32(winUpLeft.x), S32(winUpLeft.y),
      S32(winLowRight.x - winUpLeft.x),
      S32(winLowRight.y - winUpLeft.y));

}

//-----------------------------------------------------------------------

Point2I EditorToySceneWindow::objWindowPos(const SceneObject * obj)
{
   Vector2 pos = obj->getPosition();
   Vector2 winPos;

   sceneToWindowPoint(pos, winPos);

   return Point2I((S32)winPos.x,(S32)winPos.y);
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::selectionContains(SceneObject *obj)
{
   Vector<SceneObject*>::iterator i;
   for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
   {
      if (obj == *i)
         return true;
   }
   return false;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchDown(const GuiEvent& gEvt)
{

   if (!mCurrentScene)
   {
      //no scene, pass to parent and do nothing!
      Parent::onTouchDown(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onTouchDown(gEvt);
      return;
   }

   mouseLock();
   setFirstResponder();

   Point2I ctOff;
   Point2I oExt;
   SceneObject* obj;

   mLastMousePt = globalToLocalCoord(gEvt.mousePoint);
   Vector2 sceneMousePt;
   windowToScenePoint(mLastMousePt, sceneMousePt);
   
   WorldQuery* mWQ = mCurrentScene->getWorldQuery(true);
   WorldQueryFilter mQF(MASK_ALL, MASK_ALL, true, false, true, true);
   mWQ->setQueryFilter(mQF);
   mWQ->anyQueryPoint(sceneMousePt);
   U32 resCount = mWQ->getQueryResultsCount();
   typeWorldQueryResultVector& queryResults = mWQ->getQueryResults();
   // only apply the obj if we actually clicked something.
   if (resCount > 0)
   {
      obj = queryResults[0].mpSceneObject;
   }

   // If we have an object selected already, test sizing.
   if (mSelectedObjs.size() == 1)
   {
      SceneObject* obj = mSelectedObjs.first();
      RectI objBox = objWindowBounds(obj);
      RectI box(objBox.point.x, objBox.point.y, objBox.extent.x, objBox.extent.y);
      mSizeMode = (EditorToySceneWindow::SizingMode)getDirNutHit(mLastMousePt, obj);
      if (mSizeMode != SizingNone)
      {
         mMouseMode = SizingSelection;
         // for the undo
         Con::executef(this, 2, "onPreEdit", Con::getIntArg(getSelectionSet().getId()));
         return;
      }

      else if (getRotateNutHit(mLastMousePt,obj))
      {
         mMouseMode = RotateSelection;

         mStartAngle = obj->getAngle();
         mStartVector = sceneMousePt - obj->getPosition();
         // for the undo
         Con::executef(this, 2, "onPreEdit", Con::getIntArg(getSelectionSet().getId()));
         return;
      }

   }

   // handle movement for objs in selection.
   if (resCount > 0)
   {
      if (selectionContains(obj))
      {
         if (gEvt.modifier & SI_SHIFT)
         {
            Vector<SceneObject*>::iterator i;
            for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
            {
               if (*i == obj)
               {
                  Con::executef(this, 2, "onRemoveSelected", Con::getIntArg(obj->getId()));
                  mSelectedObjs.erase(i);
                  break;
               }
            }

            mMouseMode = Selecting;

         }
         else
         {
            mDragBegin = gEvt.mousePoint;
            mDragBeginPoints.reserve(mSelectedObjs.size());
            // do this whether its 1 object or 100
            Vector<SceneObject*>::iterator i;
            for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
            {
               mDragBeginPoints.push_back((*i)->getPosition());
            }

            Vector2 pos = obj->getPosition();

            if(mSelectedObjs.size() > 1)
               mOffset = mSelCenter - sceneMousePt;
            else
               mOffset = pos - sceneMousePt;
            
            mMouseMode = MovingSelection;
            //undo
            Con::executef(this, 2, "onPreEdit", Con::getIntArg(getSelectionSet().getId()));

         }
      }
      else if(gEvt.modifier & SI_SHIFT)
      {
         mSelectedObjs.push_back(obj);
         Con::executef(this, 2, "onAddSelected", Con::getIntArg(obj->getId()));
         // update bounds.
         updateSelectionBounds();
      }
      else
      {
         //clear and select the object clicked
         mSelectedObjs.clear();
         mSelectedObjs.push_back(obj);
      }
   }
   else
   {
      // if no modifier clear the selection.
      if (!(gEvt.modifier & SI_SHIFT))
      {
         Con::executef(this, 1, "onClearSelected");
         mSelectedObjs.clear();
      }
      // start drgging
      // save this, we need it later.
      mSelectionAnchor = mLastMousePt;
      mMouseMode = DragSelection;
   }
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchUp(const GuiEvent& gEvt)
{
   if (!mCurrentScene)
   {
      //no scene do nothing!
      Parent::onTouchUp(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onTouchUp(gEvt);
      return;
   }
   mouseUnlock();
   // house keeping
   mDragBegin.set(0, 0);
   mDragBeginPoints.clear();

   mLastMousePt = globalToLocalCoord(gEvt.mousePoint);
   if (mMouseMode == DragSelection)
   {
      RectI drag;
      getDragRect(drag);
      Vector2 dragPoint = drag.point;
      Vector2 dragUp;
      dragUp.x = dragPoint.x + drag.extent.x;
      dragUp.y = dragPoint.y + drag.extent.y;
      Vector2 sPt;
      Vector2 sUp;
      windowToScenePoint(dragPoint, sPt);
      windowToScenePoint(dragUp, sUp);
      // Build our area query
      WorldQuery* mWQ = mCurrentScene->getWorldQuery(true);
      WorldQueryFilter mQF(MASK_ALL, MASK_ALL, true, false, true, true);
      mWQ->setQueryFilter(mQF);
      mWQ->anyQueryArea(sPt,sUp);
      for (U32 i = 0; i < mWQ->getQueryResultsCount(); i++)
      {
         SceneObject *obj = mWQ->getQueryResults().at(i).mpSceneObject;;
         mSelectedObjs.push_back(obj);
      }

      updateSelectionBounds();
   }

   // we selected 1 item
   if(mSelectedObjs.size() == 1)
      Con::executef(this, 2, "onSelect", Con::getIntArg(mSelectedObjs[0]->getId()));

   // undo manager for move, size and rotate
   if(mMouseMode == SizingSelection || mMouseMode == MovingSelection || mMouseMode == RotateSelection)
      Con::executef(this, 2, "onPostEdit", Con::getIntArg(getSelectionSet().getId()));

   setFirstResponder();
   mMouseMode = Selecting;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchDragged(const GuiEvent& gEvt)
{
   if (!mCurrentScene)
   {
      //no scene do nothing!
      Parent::onTouchDragged(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onTouchDragged(gEvt);
      return;
   }
   // some vars
   Point2I curPoint = this->globalToLocalCoord(gEvt.mousePoint);
   Vector2 sceneMousePt;
   windowToScenePoint(curPoint, sceneMousePt);
   SceneObject *obj;
   if (mSelectedObjs.size() > 0)
   {
      obj = mSelectedObjs.first();
      // if our obj is locked, don't do anything!
      if (obj->isLocked())
         return;
   }
   
   if (mMouseMode == SizingSelection)
   {

      Vector2 objPos = obj->getRenderPosition();
      Vector2 objSize = obj->getSize();
      bool ar = true;
      bool fP = false;
      // if shift, no ar kept.
      if ((gEvt.modifier & SI_SHIFT))
         ar = false;
      // if ctrl, scale from center, no pos change, size axis.
      if ((gEvt.modifier & SI_LCTRL))
      {
         fP = true;
         ar = false;
      }
      // if alt, scale from center, no pos change.
      if ((gEvt.modifier & SI_LALT))
      {
         fP = true;
         ar = true;
      }

      scaleObject(objSize, objPos, sceneMousePt, ar, fP);
   }
   else if (mMouseMode == MovingSelection && mSelectedObjs.size())
   {
      moveObject(sceneMousePt + mOffset);
   }
   else if (mMouseMode == RotateSelection)
   {
      rotateObject(sceneMousePt - obj->getPosition(), mStartVector,mStartAngle);
   }
   else
   {
      mLastMousePt = curPoint;
   }
}

void EditorToySceneWindow::rotateObject(Vector2 mousePos, Vector2 origVec, F32 origAngle)
{
   // init vars
   SceneObject* obj = mSelectedObjs.first();

   // some snapping
   F32 snapThresh = mDegToRad(10.0f);
   F32 snapDegrees = mDegToRad(90.0f);

   F32 oldAng = mAtan(origVec.x, origVec.y) + mDegToRad(origAngle);
   F32 ang = mAtan(mousePos.x, mousePos.y);

   F32 closeAng = mFloor((ang / snapDegrees) + 0.5f) * snapDegrees;
   if (mFabs(ang - closeAng) < snapThresh)
   {
      ang = closeAng;
   }

   if (ang > mDegToRad(359.0f))
   {
      ang = 0.0f;
   }

   obj->setAngle(ang);

}

void EditorToySceneWindow::moveObject(Vector2 newPos)
{

   if (mGridSnap.x)
   {
      F32 closeX = mFloor((newPos.x / mGridSnap.x) + 0.5f) * mGridSnap.x;
      if (mFabs(newPos.x - closeX) < mSnapThreshold)
         newPos.x = closeX;
   }

   if (mGridSnap.y)
   {
      F32 closeY = mFloor((newPos.y / mGridSnap.y) + 0.5f) * mGridSnap.y;
      if (mFabs(newPos.y - closeY) < mSnapThreshold)
         newPos.y = closeY;
   }

   // moving multiple objects needs to be offset from center.
   if (mSelectedObjs.size() > 1)
   {
      Vector<SceneObject*>::iterator i;
      for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
      {

         (*i)->setPosition(((*i)->getPosition() - mSelCenter) + newPos);
      }
   }
   else
   {
      Vector<SceneObject*>::iterator i = mSelectedObjs.begin();
      (*i)->setPosition(newPos);
   }
}

void EditorToySceneWindow::scaleObject(Vector2 size, Vector2 pos, Vector2 mousePos, bool maintainAr, bool fixedPos)
{
   // init vars
   SceneObject* obj = mSelectedObjs.first();
   Vector2 sizeDelt = Vector2(0.0, 0.0);
   Vector2 newPos = pos;
   Vector2 newSize = size;
   bool flipX = false;
   bool flipY = false;

   F32 ang = mRadToDeg(obj->getAngle());

   if (ang != 90.0f && ang != -180.0f && ang != 0.0f && ang != 180.0f && ang != -90.0f)
   {
      //maintainAr = true;
      fixedPos = true;
   }

   if (mGridSnap.x)
   {
      F32 closeX = mFloor((mousePos.x / mGridSnap.x) + 0.5f) * mGridSnap.x;
      if (mFabs(mousePos.x - closeX) < mSnapThreshold)
         mousePos.x = closeX;
   }

   if (mGridSnap.y)
   {
      F32 closeY = mFloor((mousePos.y / mGridSnap.y) + 0.5f) * mGridSnap.y;
      if (mFabs(mousePos.y - closeY) < mSnapThreshold)
         mousePos.y = closeY;
   }

   if (ang == 90.0f || ang == -90.0f)
   {
      if (mSizeMode == SizingLeft)
      {
         sizeDelt.x = mousePos.x - (pos.x - (size.y * 0.5f));
         newSize.y -= sizeDelt.x;
         newPos.x += sizeDelt.x * 0.5f;
      }
      else if (mSizeMode == SizingRight)
      {
         sizeDelt.x = mousePos.x - (pos.x + (size.y * 0.5f));
         newSize.y += sizeDelt.x;
         newPos.x += sizeDelt.x * 0.5f;
      }

      if (mSizeMode == SizingTop)
      {
         sizeDelt.y = mousePos.y - (pos.y + (size.x * 0.5f));
         newSize.x += sizeDelt.y;
         newPos.y += sizeDelt.y * 0.5f;
      }
      else if (mSizeMode == SizingBottom)
      {
         sizeDelt.y = mousePos.y - (pos.y - (size.x * 0.5f));
         newSize.x -= sizeDelt.y;
         newPos.y += sizeDelt.y * 0.5f;
      }

      // flip the sizing calcs.
      if (newSize.y < 0.0f)
      {
         if (mSizeMode == SizingLeft)
         {
            mSizeMode = SizingRight;
         }
         else if (mSizeMode == SizingRight)
         {
            mSizeMode = SizingLeft;
         }
         flipX = true;
         newSize.y = -newSize.y;
      }

      if (newSize.x < 0.0f)
      {
         if (mSizeMode == SizingTop)
         {
            mSizeMode = SizingBottom;
         }
         else if (mSizeMode == SizingBottom)
         {
            mSizeMode = SizingTop;
         }
         flipY = true;
         newSize.x = -newSize.x;
      }
   }
   else
   {
      if (mSizeMode == SizingLeft)
      {
         sizeDelt.x = mousePos.x - (pos.x - (size.x * 0.5f));
         newSize.x -= sizeDelt.x;
         newPos.x += sizeDelt.x * 0.5f;
      }
      else if (mSizeMode == SizingRight)
      {
         sizeDelt.x = mousePos.x - (pos.x + (size.x * 0.5f));
         newSize.x += sizeDelt.x;
         newPos.x += sizeDelt.x * 0.5f;
      }

      if (mSizeMode == SizingTop)
      {
         sizeDelt.y = mousePos.y - (pos.y + (size.y * 0.5f));
         newSize.y += sizeDelt.y;
         newPos.y += sizeDelt.y * 0.5f;
      }
      else if (mSizeMode == SizingBottom)
      {
         sizeDelt.y = mousePos.y - (pos.y - (size.y * 0.5f));
         newSize.y -= sizeDelt.y;
         newPos.y += sizeDelt.y * 0.5f;
      }

      // flip the sizing calcs.
      if (newSize.x < 0.0f)
      {
         if (mSizeMode == SizingLeft)
         {
            mSizeMode = SizingRight;
         }
         else if (mSizeMode == SizingRight)
         {
            mSizeMode = SizingLeft;
         }
         flipX = true;
         newSize.x = -newSize.x;
      }

      if (newSize.y < 0.0f)
      {
         if (mSizeMode == SizingTop)
         {
            mSizeMode = SizingBottom;
         }
         else if (mSizeMode == SizingBottom)
         {
            mSizeMode = SizingTop;
         }
         flipY = true;
         newSize.y = -newSize.y;
      }

   }

   

   //-----------------------

   if (maintainAr)
   {
      F32 oldAr = size.x / size.y;
      F32 newAr = newSize.x / newSize.y;
      Vector2 prevResize = newSize;

      if (newAr < oldAr)
      {
         if ((newSize.x < size.x))
         {
            newSize.y *= newAr / oldAr;
         }
         else
         {
            newSize.x *= oldAr / newAr;
         }
      }
      else
      {
         if ((newSize.y < size.y) )
         {
            newSize.x *= oldAr / newAr;
         }
         else
         {
            newSize.y *= newAr / oldAr;
         }
      }

      if (ang == 90.0f || ang == -90.0f)
      {
         if (mSizeMode == SizingLeft)
         {
            newPos.x -= (newSize.y - prevResize.y) * 0.5f;
         }
         else if (mSizeMode == SizingRight)
         {
            newPos.x += (newSize.y - prevResize.y) * 0.5f;
         }

         if (mSizeMode == SizingTop)
         {
            newPos.y -= (newSize.x - prevResize.x) * 0.5f;
         }
         else if (mSizeMode == SizingBottom)
         {
            newPos.y += (newSize.x - prevResize.x) * 0.5f;
         }
      }
      else
      {
         if (mSizeMode == SizingLeft)
         {
            newPos.x -= (newSize.x - prevResize.x) * 0.5f;
         }
         else if (mSizeMode == SizingRight)
         {
            newPos.x += (newSize.x - prevResize.x) * 0.5f;
         }

         if (mSizeMode == SizingTop)
         {
            newPos.y += (newSize.y - prevResize.y) * 0.5f;
         }
         else if (mSizeMode == SizingBottom)
         {
            newPos.y -= (newSize.y - prevResize.y) * 0.5f;
         }

      }
   }

   if (fixedPos)
   {
      obj->setPosition(pos);
   }
   else
   {
      obj->setPosition(newPos);
   }

   if (newSize != size)
   {
      obj->setSize(newSize);
   }
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchMove(const GuiEvent& gEvt)
{

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseUp(const GuiEvent& gEvt)
{
   if (!mCurrentScene)
   {
      //no scene do nothing!
      Parent::onRightMouseUp(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onRightMouseUp(gEvt);
      return;
   }
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDown(const GuiEvent& gEvt)
{
   if (!mCurrentScene)
   {
      //no scene do nothing!
      Parent::onRightMouseDown(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onRightMouseDown(gEvt);
      return;
   }

   // some vars
   Point2I curPoint = this->globalToLocalCoord(gEvt.mousePoint);
   windowToScenePoint(curPoint, mMouseCamDown);



}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDragged(const GuiEvent& gEvt)
{
   if (!mCurrentScene)
   {
      //no scene do nothing!
      Parent::onRightMouseDragged(gEvt);
      return;
   }

   if (mActiveTool)
   {
      // if we have an active tool, give it the touch event.
      mActiveTool->onRightMouseDragged(gEvt);
      return;
   }
   // some vars
   Point2I curPoint = this->globalToLocalCoord(gEvt.mousePoint);
   Vector2 sceneMousePt;
   windowToScenePoint(curPoint, sceneMousePt);

   Vector2 move = mMouseCamDown - sceneMousePt;
   Vector2 newPos = getCameraPosition() + move;

   setCameraPosition(newPos);

}

bool EditorToySceneWindow::onMouseWheelDown(const GuiEvent & gEvt)
{
   // Call Parent.
   Parent::onMouseWheelDown(gEvt);

   F32 zoom = getCameraZoom();

   if (zoom < 0.1f)
      return false;

   F32 amt = -10 * 0.005f * zoom;

   setCameraZoom(zoom + amt);

   return true;
}

bool EditorToySceneWindow::onMouseWheelUp(const GuiEvent & gEvt)
{
   // Call Parent.
   Parent::onMouseWheelUp(gEvt);

   F32 zoom = getCameraZoom();

   F32 amt = 10 * 0.005f * zoom;

   setCameraZoom(zoom + amt);

   return true;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::updateSelectionBounds()
{
   // we want everything to be smaller.
   Point2I point(S32_MAX, S32_MAX);
   // we want everything to be bigger.
   Point2I extentPos(S32_MIN, S32_MIN);

   Vector<SceneObject*>::iterator i;
   for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
   {
      RectI bounds = objWindowBounds((*i));

      //bottom right
      Point2I bottomRight((bounds.point.x + bounds.extent.x), (bounds.point.y + bounds.extent.y));

      if (bounds.point.x < point.x)
         point.x = bounds.point.x;
      if (bounds.point.y < point.y)
         point.y = bounds.point.y;

      if (bottomRight.x > extentPos.x)
         extentPos.x = bottomRight.x;
      if (bottomRight.y > extentPos.y)
         extentPos.y = bottomRight.y;

   }

   Point2I extent = extentPos - point;

   mSelBox = RectI(point.x, point.y, extent.x, extent.y);

   mSelCenter = getSelectionCenter();

}

Vector2 EditorToySceneWindow::getSelectionCenter()
{
   Vector2 center(mSelBox.point.x + (mSelBox.extent.x * 0.5f), mSelBox.point.y + (mSelBox.extent.y * 0.5f));
   Vector2 sceneCenter;
   windowToScenePoint(center, sceneCenter);

   return sceneCenter;
}

void EditorToySceneWindow::onRender(Point2I offset, const RectI &updateRect)
{
   // let parent do its thing or you wont see anything
   Parent::onRender(offset, updateRect);

   if (mCurrentScene)
   {
      if (mAllCam)
      {
            // render camera.
            RectI c;
         c = updateSceneCamera();
         Point2I inset(c.extent.x * 0.1, c.extent.y * 0.1);

         if (mDrawAspectSafety)
         {
            // Aspect ratio percent
            c.inset(-inset.x, -inset.y);
            dglDrawRect(c, ColorI(255, 0, 0, 100));
            // ordinary cam
            c.inset(inset.x, inset.y);
         }

         if (mDrawCam)
         {
            dglDrawRect(c, ColorI(255, 255, 255, 255));
         }

         if (mDrawSaftey)
         {
            // 90% inside safety
            c.inset(inset.x, inset.y);
            dglDrawRect(c, ColorI(0, 255, 0, 100));
         }
      }

      bool multi = mSelectedObjs.size() > 1;
      if (multi)
      {
         //RectI b = windowSelectionBounds();
         updateSelectionBounds();
         mSelBox.point += offset;
         dglDrawRect(mSelBox, ColorI(255, 255, 255, 255));
      }

      Vector<SceneObject*>::iterator i;
      for (i = mSelectedObjs.begin(); i != mSelectedObjs.end(); i++)
      {
         SceneObject* obj = (*i);
         ColorI nutColor = multi ? ColorI(255, 255, 255) : ColorI(100, 100, 200);
         ColorI outlineColor = ColorI(255, 255, 255);
         // only draw rotate when we are not in multi
         // draw rotate first so its under other nuts.
         if (!multi)
         {
            drawRotateNuts(obj, outlineColor, nutColor);
         }

         drawSizingNuts(obj, outlineColor, nutColor);

      }
      
      if (mMouseMode == DragSelection)
      {
         RectI b;
         getDragRect(b);
         mSelBox.point += offset;
         dglDrawRect(b, ColorI(255, 255, 255, 255));
      }
   }

   if (mCurrentScene && (mGridSnap.x && mGridSnap.y) &&
      (mMouseMode == MovingSelection || mMouseMode == SizingSelection))
   {
      RectF area = getCameraRenderArea();
      U32 maxdot = (U32)(area.extent.x / mGridSnap.x) * (U32)(area.extent.y / mGridSnap.y);
      Vector2 curDot;
      Point2F areaEnd(area.point.x + area.extent.x, area.point.y + area.extent.y);
      area.point.x = mGridSnap.x * ((S32)(area.point.x / mGridSnap.x));
      area.point.y = mGridSnap.y * ((S32)(area.point.y / mGridSnap.y));
      //vector<F32> verts;
      for (F32 ix = area.point.x; ix < areaEnd.x; ix += mGridSnap.x)
      {
         for (F32 iy = area.point.y; iy < areaEnd.y; iy += mGridSnap.y)
         {
            curDot.Set(ix, iy);
            sceneToWindowPoint(curDot, curDot);
            curDot = (Vector2)localToGlobalCoord(Point2I(S32(curDot.x), S32(curDot.y)));
            //verts.push_back(curDot.x);
            //verts.push_back(curDot.y);
            // the commented out method for doing this is a lot faster
            // but it crashes if there are too many, slow is better than
            // crashing, especially just in an editor.
            dglDrawDot((Point2F)curDot, ColorI(255, 255, 255, 128));
         }
      }
      //U32 num = verts.size();
      //glEnableClientState(GL_VERTEX_ARRAY);
      //glVertexPointer(2, GL_FLOAT, 0, verts.data());
      //glDrawArrays(GL_POINTS, 0, num);
      //glDisableClientState(GL_VERTEX_ARRAY);
   }

}

void EditorToySceneWindow::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Point2F extent = getCameraArea().extent;

   F32 aR = (F32)newExtent.x / (F32)newExtent.y;
   F32 localAr = extent.x / extent.y;

   extent.x = extent.y * aR;

   Parent::resize(newPosition, newExtent);
   Parent::setCameraPosition(getCameraPosition());
   Parent::setCameraSize(Vector2(extent.x, extent.y));
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setTool(EditorToyTool * tool)
{
   if (mActiveTool)
      mActiveTool->onDeactivate();

   mActiveTool = tool;

   if (mActiveTool)
      mActiveTool->onActivate(this);

}

//-----------------------------------------------------------------------
// Required so editor can re-handle ontouch
void EditorToySceneWindow::deactivateTool()
{
   mActiveTool->onDeactivate();
   mActiveTool = nullptr;

}

//-----------------------------------------------------------------------
