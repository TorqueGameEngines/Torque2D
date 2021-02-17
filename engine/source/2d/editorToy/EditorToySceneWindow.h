#ifndef _EDITORTOYSCENEWINDOW_H_
#define _EDITORTOYSCENEWINDOW_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif // !_GUICONTROL_H_

#ifndef _SCENEWINDOW_H_
#include "2d/gui/SceneWindow.h"
#endif // !_SCENEWINDOW_H_

#ifndef _SCENEOBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif // !_SCENEOBJECT_H_

class SceneObject;
class EditorToyTool;
class EditorToySelection;


class EditorToySceneWindow : public SceneWindow
{
private:
   typedef SceneWindow Parent;

public:

   DECLARE_CONOBJECT(EditorToySceneWindow);

   typedef EditorToySelection Selection;

   // Selection class
   void clearSelection();
   void selectObject(SimObject *obj);
   void selectObject(const char *obj);
   static void markAsSelected(SimObject* obj, bool sel);

   void unselectObject(SimObject *obj);
   void unselectObject(const char *obj);

   S32 getSlectionSize();
   S32 getSelectObj(S32 index);

   const Point2F& getSelectionCenter();
   const RectF& getSelectionBounds();
   Point2F getSelectionExtent();

private:

   // Mouse modes when dragging
   enum {
      Move = 0,
      Rotate,
      Scale
   };

   // Private mouse vars
   SimObjectPtr<SceneObject>  mHitObject;
   SimObjectPtr<SceneObject>  mPossibleHitObject;
   SimObjectPtr< Selection >  mSelected;
   SimObjectPtr< Selection >  mDragSelected;
   bool                       mMouseDown;
   bool                       mDragSelect;
   RectI                      mDragRect;
   Point2I                    mDragStart;
   bool                       mMouseDragged;

   EditorToyTool* mActiveTool;

   bool collide(const GuiEvent & gEvt, SceneObject ** hitObj);

public:

   enum DropType
   {
      DropAtOrigin = 0,
      DropAtCamera,
      DropAtCameraWithRot,
      DropAtScreenCenter
   };

   enum AlignType
   {
      AlignNone = 0,
      AlignPosX,
      AlignPosY,
      AlignNegPosX,
      AlignNegPosY
   };

   S32         mDropType;
   Scene*       mEditorScene;
   // Public Mouse Vars
   GuiEvent    mLastMouseEvent;
   GuiEvent    mLastMouseDownEvent;
   ColorI      mSelectionBoxColor;
   bool        mRenderSelectionBox;
   bool        mSelectionLocked;
   // Mouse Dragging
   bool        mDragGridSnapToggle;
   bool        mNoMouseDrag;
   ColorI      mDragRectColor;
   // Mouse Object vars
   ColorI      mObjSelColor;
   ColorI      mObjMultiSelColor;
   ColorI      mObjMouseOverSelColor;
   ColorI      mObjMouseOverColor;

   //
   bool        mGridSnap;
   bool        mUseGroupCenter;
   bool        mBoundingBoxCollision;

   bool        mObjectsUseBoxCenter;

   U32         mLayerMask;
   U32         mGroupMask;

public:
   EditorToySceneWindow();
   ~EditorToySceneWindow();

   virtual bool onAdd();

   // Input Handling
   //bool mouseHandler(const GuiEvent& gEvt, const MouseStatus::MouseEvt mEvt);
   void onTouchUp(const GuiEvent& gEvt);
   void onTouchDown(const GuiEvent& gEvt);
   void onTouchDragged(const GuiEvent& gEvt);
   void onTouchMove(const GuiEvent& gEvt);
   void onRightMouseUp(const GuiEvent& gEvt);
   void onRightMouseDown(const GuiEvent& gEvt);
   void onRightMouseDragged(const GuiEvent& gEvt);

   // Scene handling
   virtual void setScene(Scene* pScene);
   virtual void resetScene(void);

   // Camera functions
   virtual void setTargetCameraArea(const RectF& camWindow);
   void makeActiveSelectionSet(EditorToySelection * selection);
   virtual void setTargetCameraPosition(Vector2 center, F32 w, F32 h);
   virtual void setCameraArea(const RectF& camWindow);
   virtual void setCameraPosition(Vector2 center, F32 w, F32 h);
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);

   EditorToySelection * getActiveSelectionSet() const;

   void setLayerMask(const U32 lyrMsk);
   void setGroupMask(const U32 grpMask);
   U32 getLayerMask();
   U32 getGroupMask();

   virtual void onRender(Point2I offset, const RectI &updateRect);
   RectI getSelBounds(Selection * sel);
   void renderObjBox(SceneObject * obj, const ColorI & col);
   void renderSelBox(Selection*  sel);

   void setTool(EditorToyTool* tool);
   EditorToyTool* getTool() { return mActiveTool; }

};



#endif // !_EDITORTOYSCENEWINDOW_H_
