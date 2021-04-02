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

public:
   EditorToyTool();
   ~EditorToyTool();
   bool onAdd();
   void onRemove();
   virtual void onActivate(EditorToySceneWindow* sceneWin);
   virtual void onDeactivate();

   virtual void onRelinquishObj(SceneObject* obj);

   //
   virtual bool onTouchMove(const GuiEvent &);
   virtual bool onTouchDown(const GuiEvent &);
   virtual bool onTouchDragged(const GuiEvent &);
   virtual bool onTouchUp(const GuiEvent &);

   //
   virtual bool onRightMouseDown(const GuiEvent &);
   virtual bool onRightMouseDragged(const GuiEvent &);
   virtual bool onRightMouseUp(const GuiEvent &);

   //
   virtual bool onMiddleMouseDown(const GuiEvent &);
   virtual bool onMiddleMouseDragged(const GuiEvent &);
   virtual bool onMiddleMouseUp(const GuiEvent &);

   virtual bool onInputEvent(const GuiEvent &);

   virtual void onRender() {};

   DECLARE_CONOBJECT(EditorToyTool);


};


#endif // !_EDITORTOYTOOL_H_
