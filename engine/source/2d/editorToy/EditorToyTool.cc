#include "console/consoleInternal.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "2d/editorToy/EditorToyTool.h"


//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToyTool);

//-----------------------------------------------------------------------

EditorToyTool::EditorToyTool()
{

   mEditorToy = NULL;
   mUseMouseDown = true;
   mUseMouseUp = true;
   mUseMouseMove = true;

   mUseRightMouseDown = false;
   mUseRightMouseUp = false;
   mUseRightMouseMove = false;

   mUseMiddleMouseDown = true;
   mUseMiddleMouseUp = true;
   mUseMiddleMouseMove = true;

   mUseKeyInput = true;

}

EditorToyTool::~EditorToyTool()
{

}

bool EditorToyTool::onAdd()
{
   return Parent::onAdd();
}

void EditorToyTool::onRemove()
{
   Parent::onRemove();
}

void EditorToyTool::onActivate(EditorToySceneWindow* sceneWin)
{
   mEditorToy = sceneWin;
   Con::executef(this, 1, "onActivate");
}

void EditorToyTool::onDeactivate()
{
   mEditorToy = NULL;
   Con::executef(this, 1, "onDeactivate");
}

void EditorToyTool::onRelinquishObj(SceneObject * obj)
{
}

bool EditorToyTool::onTouchMove(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onTouchMove", e.mousePoint);
   return true;
}

bool EditorToyTool::onTouchDown(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onTouchDown", e.mousePoint);

   return true;
}

bool EditorToyTool::onTouchDragged(const GuiEvent &e)
{
   Con::executef(this, 2, "onTouchDragged", e.mousePoint);

   return true;
}

bool EditorToyTool::onTouchUp(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onTouchUp", e.mousePoint);

   return true;
}

bool EditorToyTool::onRightMouseDown(const GuiEvent &e)
{
   if (!mUseRightMouseDown)
      return false;

   Con::executef(this,2, "onRightMouseDown", e.mousePoint);
   return true;
}

bool EditorToyTool::onRightMouseDragged(const GuiEvent &e)
{
   Con::executef(this,2, "onRightMouseDragged", e.mousePoint);
   return true;
}

bool EditorToyTool::onRightMouseUp(const GuiEvent &e)
{
   if (!mUseRightMouseDown)
      return false;

   Con::executef(this,2, "onRightMouseUp", e.mousePoint);
   return true;
}

bool EditorToyTool::onMiddleMouseDown(const GuiEvent &e)
{
   if (!mUseMiddleMouseDown)
      return false;

   Con::executef(this,2, "onMiddleMouseDown", e.mousePoint);
   return true;
}

bool EditorToyTool::onMiddleMouseDragged(const GuiEvent &e)
{
   Con::executef(this,2, "onMiddleMouseDragged", e.mousePoint);
   return true;
}

bool EditorToyTool::onMiddleMouseUp(const GuiEvent &e)
{
   if (!mUseMiddleMouseDown)
      return false;

   Con::executef(this,2, "onMiddleMouseUp", e.mousePoint);
   return true;
}

bool EditorToyTool::onInputEvent(const GuiEvent &e)
{
   if (!mUseKeyInput)
      return false;

   Con::executef(this,3, "onKeyPress", e.ascii, e.modifier);
   return true;
}
