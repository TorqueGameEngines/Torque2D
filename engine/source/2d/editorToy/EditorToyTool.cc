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
    if (mUseMouseDown && isMethod("onTouchMove"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onTouchMove", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onTouchDown(const GuiEvent &e)
{
    if (mUseMouseDown && isMethod("onTouchDown"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onTouchDown", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onTouchDragged(const GuiEvent &e)
{
    if (isMethod("onTouchDragged"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onTouchDragged", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onTouchUp(const GuiEvent &e)
{
    if (mUseMouseDown && isMethod("onTouchUp"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onTouchUp", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onRightMouseDown(const GuiEvent &e)
{
   if (mUseRightMouseDown && isMethod("onRightMouseDown"))
   {
       char* point = Con::getArgBuffer(32);
       dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
       Con::executef(this, 2, "onRightMouseDown", point);
       return true;
   }
   return false;
}

bool EditorToyTool::onRightMouseDragged(const GuiEvent &e)
{
    if (isMethod("onRightMouseDragged"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onRightMouseDragged", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onRightMouseUp(const GuiEvent &e)
{
    if (mUseRightMouseDown && isMethod("onRightMouseUp"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onRightMouseUp", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onMiddleMouseDown(const GuiEvent &e)
{
    if (mUseMiddleMouseDown && isMethod("onMiddleMouseDown"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onMiddleMouseDown", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onMiddleMouseDragged(const GuiEvent &e)
{
    if (isMethod("onMiddleMouseDragged"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onMiddleMouseDragged", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onMiddleMouseUp(const GuiEvent &e)
{
    if (mUseMiddleMouseDown && isMethod("onMiddleMouseUp"))
    {
        char* point = Con::getArgBuffer(32);
        dSprintf(point, 32, "%d %d", e.mousePoint.x, e.mousePoint.y);
        Con::executef(this, 2, "onMiddleMouseUp", point);
        return true;
    }
    return false;
}

bool EditorToyTool::onInputEvent(const GuiEvent &e)
{
   if (!mUseKeyInput)
      return false;

   Con::executef(this,3, "onKeyPress", e.ascii, e.modifier);
   return true;
}
