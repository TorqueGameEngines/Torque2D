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

#ifndef _UNDO_H_
#include "collection/undo.h"
#endif

class SceneObject;
class EditorToyTool;
class EditorToySelection;


class EditorToySceneWindow : public SceneWindow
{
private:
   typedef SceneWindow Parent;

   U32                  mNutSize;

   // We really only want to work on sceneObjects
   Vector<SceneObject*> mSelectedObjs;
   UndoManager          mUndoManager;
   SimGroup             mTrash;
   SimSet               mSelSet;
   Point2I              mLastMousePt;
   Point2I              mSelectionAnchor;
   Vector2              mGridSnap;
   Point2I              mDragBegin;
   Vector<Vector2>      mDragBeginPoints;
   // Store the current scene.
   Scene*               mCurrentScene;
   Vector2              mOffset;
   RectI                mSelBox;
   Vector2              mSelCenter;
   F32                  mSnapThreshold;
   // rotation vars
   F32                  mStartAngle;
   Vector2              mStartVector;
   // Sizing Cursors
   GuiCursor*           mDefaultCursor;
   GuiCursor*           mLeftRightCursor;
   GuiCursor*           mUpDownCursor;
   GuiCursor*           mMoveCursor;

   // cam vars
   Vector2              mMouseCamDown;
   Vector2              mSceneCamPos;
   Vector2              mSceneCamSize;
   RectI                mSceneCamRect;

   bool                 mAllCam;
   bool                 mDrawCam;
   bool                 mDrawAspectSafety;
   bool                 mDrawSaftey;

   enum MouseMode
   {
      Selecting = 0,
      MovingSelection,
      SizingSelection,
      RotateSelection,
      DragSelection
   };

   enum SizingMode
   {
      SizingNone = 0,
      SizingLeft = 1,
      SizingRight = 2,
      SizingTop = 4,
      SizingBottom = 8
   };

   enum DropType
   {
      DropAtOrigin = 0,
      DropAtCamera,
      DropAtCameraWithRot,
      DropAtScreenCenter
   };

   EditorToyTool* mActiveTool;

   MouseMode      mMouseMode;
   SizingMode     mSizeMode;
   DropType       mDropType;

   bool inNut(const Point2I & pt, S32 x, S32 y);

   void updatedSelection();

public:

   DECLARE_CONOBJECT(EditorToySceneWindow);

   EditorToySceneWindow();
   ~EditorToySceneWindow();

   virtual bool onAdd();
   virtual void onRemove();

   // handle sizing nuts
   S32 getNutHit(const Point2I &pt, const RectI &box);
   S32 getDirNutHit(const Point2I &pt, SceneObject* obj);

   // draw visuals
   void drawNut(const Point2I &nut, ColorI &outlineColor, ColorI &nutColor);
   void drawSizingNuts(SceneObject* obj, ColorI &outlineColor, ColorI &nutColor);
   bool getRotateNutHit(const Point2I & pt, SceneObject * obj );
   void drawRotateNuts(SceneObject* obj, ColorI &outlineColor, ColorI &nutColor);

   // object scene to window functions
   void getDragRect(RectI &box);
   RectI objWindowBounds(const SceneObject* obj);
   Point2I objWindowPos(const SceneObject* obj);

   // Input Handling
   void onTouchUp(const GuiEvent& gEvt);
   void onTouchDown(const GuiEvent& gEvt);
   void onTouchDragged(const GuiEvent& gEvt);
   void onTouchMove(const GuiEvent& gEvt);
   void onRightMouseUp(const GuiEvent& gEvt);
   void onRightMouseDown(const GuiEvent& gEvt);
   void onRightMouseDragged(const GuiEvent& gEvt);
   bool onMouseWheelUp(const GuiEvent &gEvt);
   bool onMouseWheelDown(const GuiEvent &gEvt);

   // do something to objects
   void rotateObject(Vector2 mousePos, Vector2 origVec, F32 origAngle);
   void moveObject(Vector2 newPos);
   void scaleObject(Vector2 size, Vector2 pos, Vector2 mousePos, bool maintainAr, bool fixedPos);

   // Scene handling
   void setEditScene(Scene* pScene);
   RectI updateSceneCamera();
   void resetEditScene(void);

   // Find out stuff about selection.
   void updateSelectionBounds();
   Vector2 getSelectionCenter();
   bool selectionContains(SceneObject * obj);

   // render
   virtual void onRender(Point2I offset, const RectI &updateRect);

   // camera
   virtual void resize(const Point2I &newPosition, const Point2I &newExtent);

   // tool system.
   void setTool(EditorToyTool* tool);
   void deactivateTool();
   EditorToyTool* getTool() { return mActiveTool; }

   const SimSet& getSelectionSet() { updatedSelection(); return mSelSet; }

   // Sizing Cursors
   bool initCursors();
   void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &gEvt);

};



#endif // !_EDITORTOYSCENEWINDOW_H_
