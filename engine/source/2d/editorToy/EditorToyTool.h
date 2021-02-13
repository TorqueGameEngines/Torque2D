#ifndef _EDITORTOYTOOL_H_
#define _EDITORTOYTOOL_H_

#include "sim/simBase.h"

#include "2d/editorToy/EditorToySceneWindow.h"

#include "collection/undo.h"

#include "debug/profiler.h"

#include "2d/scene/Scene.h"

#include "2d/sceneobject/SceneObject.h"


class EditorToyTool : public SimObject
{
   typedef SimObject Parent;

protected:

   EditorToySceneWindow* mEditorToy;

   bool mUseMouseDown;
   bool mUseMouseUp;
   bool mUseMouseMove;

   bool mUseRightMouseDown;
   bool mUseRightMouseUp;
   bool mUseRightMouseMove;

   bool mUseMiddleMouseDown;
   bool mUseMiddleMouseUp;
   bool mUseMiddleMouseMove;

   bool mUseKeyInput;

   /*StringTableEntry mTool;
   StringTableEntry mTexture;

   Vector<Point2F>   mNuts;
   ColorI            mNutCol;
   ColorI            mNutOutCol;
   S32               mNutSize;

   UndoManager       mUndo;

   void drawNut(Point2I pos);
   void drawArrow(Point2I pos);
   bool inNut(Point2I pt, S32 x, S32 y);
   */

public:
   EditorToyTool();
   ~EditorToyTool();
   bool onAdd();
   void onRemove();
   virtual void onActivate(EditorToySceneWindow* sceneWin);
   virtual void onDeactivate();


   /*
   virtual bool hasUndoManager() { return false; };
   const UndoManager& getUndoManager() { return mUndo; };
   virtual bool undo() { return false; }
   virtual bool redo() { return false; }
   */

   virtual void onRelinquishObj(SceneObject* obj);

   //
   virtual bool onMouseMove(const GuiEvent &);
   virtual bool onMouseDown(const GuiEvent &);
   virtual bool onMouseDragged(const GuiEvent &);
   virtual bool onMouseUp(const GuiEvent &);

   //
   virtual bool onRightMouseDown(const GuiEvent &);
   virtual bool onRightMouseDragged(const GuiEvent &);
   virtual bool onRightMouseUp(const GuiEvent &);

   //
   virtual bool onMiddleMouseDown(const GuiEvent &);
   virtual bool onMiddleMouseDragged(const GuiEvent &);
   virtual bool onMiddleMouseUp(const GuiEvent &);

   virtual bool onInputEvent(const GuiEvent &);

   virtual void onRenderScene() {};

   DECLARE_CONOBJECT(EditorToyTool);


};


#endif // !_EDITORTOYTOOL_H_
