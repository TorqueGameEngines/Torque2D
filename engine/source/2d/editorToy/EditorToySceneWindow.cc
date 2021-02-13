#include "2d/editorToy/EditorToySceneWindow.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include "platform/event.h"
#include "2d/editorToy/EditorToySelection.h"
#include "2d/editorToy/EditorToyTool.h"

#include "2d/editorToy/EditorToySceneWindow_ScriptBinding.h"

//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToySceneWindow);

//-----------------------------------------------------------------------

EditorToySceneWindow::EditorToySceneWindow()
{
   mMouseDown = false;
   mDragSelect = false;

   mDragSelected = new Selection();
   mDragSelected->registerObject("EEditorToyDragSelection");
   Sim::getRootGroup()->addObject(mDragSelected);

   mGridSnap = false;
   mUseGroupCenter = true;

   mDropType = DropAtScreenCenter;
   mBoundingBoxCollision = true;
   mObjectsUseBoxCenter = true;
   mObjSelColor.set(255, 0, 0, 200);
   mObjMouseOverColor.set(0, 0, 255);
   mObjMouseOverColor.set(0, 255, 0);
   mDragRectColor.set(100, 0, 255);
   mActive = true;

   mActiveTool = nullptr;
}

bool EditorToySceneWindow::onAdd()
{
   if (!Parent::onAdd())
      return false;

   mEditorScene = new Scene();
   mEditorScene->setIsEditorScene(true);

   return true;
}

//-----------------------------------------------------------------------

EditorToySceneWindow::~EditorToySceneWindow()
{
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setScene(Scene* pScene)
{
   Parent::setScene(pScene);
   if (getScene())
      getScene()->setIsEditorScene(true);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::resetScene()
{
   if (getScene())
      getScene()->setIsEditorScene(false);

   Parent::resetScene();
}

//-----------------------------------------------------------------------

bool EditorToySceneWindow::collide(const GuiEvent &gEvt, SceneObject **hitObj)
{
   if (!mEditorScene->getSceneObjectCount() > 0)
      return false;

   if (mBoundingBoxCollision)
   {
      Point2I pt = globalToLocalCoord(gEvt.mousePoint);
      Vector2 scenePt;
      windowToScenePoint(Vector2(F32(gEvt.mousePoint.x),
         F32(gEvt.mousePoint.y)),
         scenePt);

      WorldQuery* mWQuery = mEditorScene->getWorldQuery(true);

      // Create a filter by layer and group, we want everything else
      // objects don't have to be enabled, or visible but do need picking
      // allowed and need to be in the scope of the scene.
      WorldQueryFilter qFilter(mLayerMask, mGroupMask, false, false, true, true);

      mWQuery->setQueryFilter(qFilter);

      if (mWQuery->anyQueryPoint(scenePt) > 0)
      {
         // return the first object hit.
         *hitObj = mWQuery->getQueryResults().at(0).mpSceneObject;
         return true;
      }

   }
   return false;
}


void EditorToySceneWindow::onTouchUp(const GuiEvent& gEvt)
{
   if (mActiveTool != nullptr && mActiveTool->onMouseDown(gEvt))
      return;

   mMouseDown = false;

   if (mDragGridSnapToggle)
   {
      mDragGridSnapToggle = false;
      const bool snapToGrid = !mGridSnap;
   }

   if (mDragSelect)
   {
      mDragSelect = false;
      mPossibleHitObject = NULL;
      const bool addToSelection = (gEvt.modifier &(SI_SHIFT | SI_CTRL));
      if (!addToSelection)
         clearSelection();

      if (mDragSelected->size() > 1)
      {
         for (U32 i = 0; i < mDragSelected->size(); i++)
            mSelected->addObject((*mDragSelected)[i]);

         Con::executef(this, 3, "onMultiSelect", mDragSelected->getIdString(), addToSelection ? "1" : "0");

         mDragSelected->clear();

      }
      else if (mDragSelected->size() == 1)
      {
         mSelected->addObject((*mDragSelected)[0]);
         Con::executef(this, 2, "onSelect", (*mDragSelected)[0]->getIdString());
         mDragSelected->clear();
      }

      mouseUnlock();
      return;
   }
   else if (mPossibleHitObject.notNull())
   {
      if (!mSelectionLocked)
      {
         if (gEvt.modifier &(SI_SHIFT | SI_CTRL))
         {
            mNoMouseDrag = true;
            if (mSelected->objInSet(mPossibleHitObject))
            {
               mSelected->removeObject(mPossibleHitObject);
               mSelected->storeCurCenter();
               Con::executef(this, 2, "onUnSelect", mPossibleHitObject->getIdString());
            }
            else
            {
               mSelected->addObject(mPossibleHitObject);
               mSelected->storeCurCenter();
               Con::executef(this, 2, "onSelect", mPossibleHitObject->getIdString());
            }
         }
         else
         {
            if (bool(mSelected) && !mSelected->objInSet(mPossibleHitObject))
            {
               mNoMouseDrag = true;

               for (S32 i = mSelected->size() - 1; i >= 0; --i)
               {
                  EditorToySceneWindow::markAsSelected((*mSelected)[i], false);
                  Con::executef(this, 2, "onUnSelect", (*mSelected)[i]->getIdString());
               }

               mSelected->clear();
               mSelected->addObject(mPossibleHitObject);
               mSelected->storeCurCenter();
               Con::executef(this, 2, "onSelect", mPossibleHitObject->getIdString());
            }
         }
      }

      if (gEvt.mouseClickCount > 1)
      {
         char buf[16];
         dSprintf(buf, sizeof(buf), "%d", mPossibleHitObject->getId());
      }
      else
      {
         char buf[16];
         dSprintf(buf, sizeof(buf), "%d", mPossibleHitObject->getId());
      }

      mHitObject = mPossibleHitObject;

   }

   if (bool(mSelected) && mSelected->hasCenterChanged())
   {
      Con::executef(this, 1, "onSelectionCenterChanged");
   }

   if (mMouseDragged && bool(mSelected) && mSelected->size())
   {
      if (mSelected->size())
      {
         if (isMethod("onEndDrag"))
         {
            SimObject * obj = 0;
         }
      }
   }

   mouseUnlock();
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchDown(const GuiEvent& gEvt)
{
   Con::printf("left mouse down");

   if (mActiveTool != nullptr && mActiveTool->onMouseDown(gEvt))
      return;

   mMouseDown = true;
   mMouseDragged = false;
   mDragGridSnapToggle = false;
   mLastMouseDownEvent = gEvt;
   mNoMouseDrag = false;

   mouseLock();

   if (bool(mSelected) && mSelected->size() > 0)
   {
      const RectF& selBounds = getActiveSelectionSet()->getBoxBounds();
      const F32 maxDim = getMax(selBounds.len_x(), selBounds.len_y());
      const F32 size = mCeil(maxDim + 10.0f);
      const F32 spacing = mCeil(size / 20.0f);
   }

   SceneObject *hitObj = NULL;
   if (collide(gEvt, &hitObj) && hitObj->getPickingAllowed())
   {
      mPossibleHitObject = hitObj;
      mNoMouseDrag = true;
   }
   else if(!mSelectionLocked)
   {
      if (!(gEvt.modifier &(SI_SHIFT | SI_CTRL)))
         clearSelection();

      mDragSelect = true;
      mDragSelected->clear();
      mDragRect.set(Point2I(gEvt.mousePoint), Point2I(0, 0));
      mDragStart = gEvt.mousePoint;

   }

   mLastMouseEvent = gEvt;

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchDragged(const GuiEvent& gEvt)
{
   if (mActiveTool != nullptr && mActiveTool->onMouseDown(gEvt))
      return;

   if (!mMouseDown)
      return;

   Con::printf("left mouse dragged");

   if (mNoMouseDrag)
   {
      if (mAbs(mLastMouseDownEvent.mousePoint.x - gEvt.mousePoint.x) > 2 || mAbs(mLastMouseDownEvent.mousePoint.y - gEvt.mousePoint.y) > 2)
      {
         if (!(gEvt.modifier &(SI_SHIFT | SI_CTRL)))
            clearSelection();

         mDragSelect = true;
         mDragSelected->clear();
         mDragRect.set(Point2I(mLastMouseDownEvent.mousePoint), Point2I(0, 0));
         mDragStart = mLastMouseDownEvent.mousePoint;

         mNoMouseDrag = false;
         mHitObject = NULL;

      }
      else
      {
         return;
      }
   }

   if (!mMouseDragged)
   {
      if (mHitObject && bool(mSelected) && !mSelected->objInSet(mHitObject))
      {
         if (!mSelectionLocked)
            mSelected->addObject(mHitObject);
      }

      mMouseDragged = true;
   }
   
   if (mDragSelect)
   {
      mDragRect.point.x = (gEvt.mousePoint.x < mDragStart.x) ? gEvt.mousePoint.x : mDragStart.x;
      mDragRect.extent.x = (gEvt.mousePoint.x > mDragStart.x) ? gEvt.mousePoint.x - mDragStart.x : mDragStart.x - gEvt.mousePoint.x;
      mDragRect.point.y = (gEvt.mousePoint.y < mDragStart.y) ? gEvt.mousePoint.y : mDragStart.y;
      mDragRect.extent.y = (gEvt.mousePoint.y > mDragStart.y) ? gEvt.mousePoint.y - mDragStart.y : mDragStart.y - gEvt.mousePoint.y;
      return;
   }

   if ((!mHitObject || !mSelected->objInSet(mHitObject)))
      return;

   for (U32 i = 0; i < mSelected->size(); i++)
   {
      if ((*mSelected)[i]->isLocked())
         return;
   }

   mLastMouseEvent = gEvt;
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onTouchMove(const GuiEvent& gEvt)
{
   mHitObject = NULL;

   if (mActiveTool != nullptr && mActiveTool->onMouseMove(gEvt))
      return;

   if (bool(mSelected) && mSelected->size() > 0)
   {

   }

   if (!mHitObject)
   {
      SceneObject *hitObj = NULL;
      if (collide(gEvt, &hitObj) && hitObj->getPickingAllowed())
      {
         mHitObject = hitObj;
      }
   }

   mLastMouseEvent = gEvt;

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseUp(const GuiEvent& gEvt)
{

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDown(const GuiEvent& gEvt)
{
   Con::printf("right mouse down");
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRightMouseDragged(const GuiEvent& gEvt)
{
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setTargetCameraArea(const RectF& camWindow)
{
   Point2F center = camWindow.point + (camWindow.extent * 0.5);
   F32 w = camWindow.extent.x;
   F32 h = camWindow.extent.y;

   setTargetCameraPosition(center, w, h);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setTargetCameraPosition(Vector2 center, F32 w, F32 h)
{
   F32 winAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = w / h;

   if (sceneAR > winAR)
      h *= (sceneAR / winAR);
   else
      w *= (winAR / sceneAR);

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::setTargetCameraPosition(center);
   Parent::setTargetCameraSize(size);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setCameraArea(const RectF& camWindow)
{
   Point2F center = camWindow.point + (camWindow.extent * 0.5);
   F32 w = camWindow.extent.x;
   F32 h = camWindow.extent.y;

   setCameraPosition(center, w, h);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::setCameraPosition(Vector2 center, F32 w, F32 h)
{
   F32 winAR = (F32)getExtent().x / (F32)getExtent().y;
   F32 sceneAR = w / h;

   if (sceneAR > winAR)
      h *= (sceneAR / winAR);
   else
      w *= (winAR / sceneAR);

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::setCameraPosition(center);
   Parent::setCameraSize(size);
}

//-----------------------------------------------------------------------

void EditorToySceneWindow::resize(const Point2I & newPosition, const Point2I & newExtent)
{
   F32 w = getCameraArea().extent.x;
   F32 h = getCameraArea().extent.y;

   F32 winAR = (F32)newExtent.x / (F32)newExtent.y;
   F32 sceneAR = w / h;

   Point2I oldExtent = getExtent();

   if ((oldExtent.x != newExtent.x) && (oldExtent.y != newExtent.y))
   {
      w *= (sceneAR = winAR);
      h *= (winAR / sceneAR);
   }
   else if (oldExtent.x != newExtent.x)
   {
      w *= (winAR / sceneAR);
   }
   else if (oldExtent.y != newExtent.y)
   {
      h *= (sceneAR / winAR);
   }

   Vector2 size;
   size.x = w;
   size.y = h;

   Parent::resize(newPosition, newExtent);
   Parent::setCameraPosition(getCameraPosition());
   Parent::setCameraSize(size);

}

//-----------------------------------------------------------------------

void EditorToySceneWindow::onRender(Point2I offset, const RectI &updateRect)
{
   if (mActiveTool != nullptr)
      mActiveTool->onRenderScene();

   Selection* sel = getActiveSelectionSet();
   if (sel)
   {
      bool isMultiSel = mSelected->size() > 1;
      for (U32 i = 0; i < mSelected->size(); i++)
      {
         if ((const SceneObject*)mHitObject == (*mSelected)[i])
            continue;
         SceneObject* obj = dynamic_cast<SceneObject*>((*mSelected)[i]);
         if (obj && mRenderSelectionBox)
            renderObjBox(obj, isMultiSel ? mObjMultiSelColor : mObjSelColor);
      }
   }

   for (U32 i = 0; i < mDragSelected->size(); i++)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>((*mDragSelected)[i]);
      if (obj && mRenderSelectionBox)
         renderObjBox(obj, mObjSelColor);
   }

   if (sel)
   {
      if (bool(mHitObject) && mRenderSelectionBox)
      {
         ColorI & col = sel->objInSet(mHitObject) ? mObjMouseOverSelColor : mObjMouseOverColor;
         renderObjBox(mHitObject, col);
      }

      if (sel->size())
      {
         if (mRenderSelectionBox && sel->size() > 1)
            renderSelBox(sel);
      }
   }

   if (mDragSelect)
      mDragSelected->clear();

   if (mDragSelect && mDragRect.extent.x > 1 && mDragRect.extent.y > 1)
      dglDrawRect(mDragRect, mDragRectColor);

   if (mDragSelect && mDragRect.extent.x > 1 && mDragRect.extent.y > 1)
   {
      if (!mEditorScene->getSceneObjectCount() > 0)
         return;

      WorldQuery* mWQuery = mEditorScene->getWorldQuery(true);

      WorldQueryFilter qFilter(mLayerMask, mGroupMask, false, false, true, true);

      mWQuery->clearQuery();

      Vector2 sceneDragPt;
      Vector2 sceneCurPt;

      windowToScenePoint(globalToLocalCoord(mDragStart), sceneDragPt);
      windowToScenePoint(globalToLocalCoord(mDragStart + mDragRect.extent), sceneCurPt);

      if (mWQuery->anyQueryArea(sceneDragPt, sceneCurPt) > 0)
      {
         for (U32 i = 0; i < mWQuery->getQueryResultsCount(); i++)
         {
            SceneObject *obj = mWQuery->getQueryResults().at(0).mpSceneObject;;
            mDragSelected->addObject(obj);
         }
      }

   }

}

void EditorToySceneWindow::renderObjBox(SceneObject * obj, const ColorI & col)
{
   // HAHAHAHAHAHAH This is easier in T2D than T3D :) 
   const b2Vec2* oobb = obj->getRenderOOBB();
   Vector2 strt = oobb[0];
   Vector2 ext(oobb[3].x - oobb[0].x, oobb[3].y - oobb[0].y);
   RectF draw(strt, ext);
   dglDrawRectF(draw, col);
}

void EditorToySceneWindow::renderSelBox(Selection*  sel)
{
   if (!mRenderSelectionBox || !sel->size())
      return;

   const RectF box = sel->getBoxBounds();

   dglDrawRectF(box, mSelectionBoxColor);
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

EditorToySelection* EditorToySceneWindow::getActiveSelectionSet() const
{
   return mSelected;
}


void EditorToySceneWindow::setLayerMask(const U32 lyrMask)
{
   mLayerMask = lyrMask;
}

void EditorToySceneWindow::setGroupMask(const U32 grpMask)
{
   mGroupMask = grpMask;
}

U32 EditorToySceneWindow::getLayerMask()
{
   return mLayerMask;
}

U32 EditorToySceneWindow::getGroupMask()
{
   return mGroupMask;
}

void EditorToySceneWindow::clearSelection()
{
   if (mSelectionLocked || !mSelected)
      return;

   for (S32 i = mSelected->size() - 1; i >= 0; --i)
   {
      EditorToySceneWindow::markAsSelected((*mSelected)[i], false);
      Con::executef(this,2, "onUnSelect", (*mSelected)[i]->getIdString());
   }

   Con::executef(this,1, "onClearSelection");
   mSelected->clear();

}

void EditorToySceneWindow::selectObject(SimObject * obj)
{
   if (mSelectionLocked || !mSelected || !obj)
      return;

   if (!mSelected->objInSet(obj))
   {
      mSelected->addObject(obj);
      Con::executef(this,2, "onSelect", obj->getIdString());
   }
}

void EditorToySceneWindow::selectObject(const char * obj)
{
   SimObject *select;

   if (Sim::findObject(obj, select))
      selectObject(select);
}

void EditorToySceneWindow::markAsSelected(SimObject * obj, bool sel)
{
   obj->setSelected(sel);
}

void EditorToySceneWindow::unselectObject(SimObject * obj)
{
   if (mSelectionLocked || !mSelected || !obj)
      return;

   if (mSelected->objInSet(obj))
   {
      mSelected->removeObject(obj);
      Con::executef(this,2, "onUnSelect", obj->getIdString());
   }
}

void EditorToySceneWindow::unselectObject(const char * obj)
{
   SimObject *select;

   if (Sim::findObject(obj, select))
      unselectObject(select);
}

S32 EditorToySceneWindow::getSlectionSize()
{
   if (!mSelected)
      return 0;
   return mSelected->size();
}

S32 EditorToySceneWindow::getSelectObj(S32 index)
{
   AssertFatal(mSelected != NULL, "EditorToy::getSelectedObj - no active selection");

   return (*mSelected)[index]->getId();
}

const Point2F & EditorToySceneWindow::getSelectionCenter()
{
   if (!mSelected)
      return Point2F::Zero;

   return mObjectsUseBoxCenter ? mSelected->getBoxCenter() : mSelected->getCenter();
}

const RectF & EditorToySceneWindow::getSelectionBounds()
{
   return mSelected->getBoxBounds();
}

Point2F EditorToySceneWindow::getSelectionExtent()
{
   const RectF& bound = getSelectionBounds();
   return bound.extent;
}
