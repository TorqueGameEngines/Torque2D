#include "console/consoleInternal.h"
#include "console/console.h"
#include "graphics/dgl.h"
#include "2d/editorToy/EditorToyTool.h"


//-----------------------------------------------------------------------

IMPLEMENT_CONOBJECT(EditorToyTool);

//-----------------------------------------------------------------------

EditorToyTool::EditorToyTool() //: mNutCol(128, 128, 128, 128),
                               //  mNutOutCol(128, 128, 128, 255),
                               //  mNutSize(5)
{
   //VECTOR_SET_ASSOCIATION(mNuts);
   //mTool = StringTable->insert("EditorToy Tool");

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

bool EditorToyTool::onMouseMove(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onMouseMove", e.mousePoint);
   return true;
}

bool EditorToyTool::onMouseDown(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onMouseDown", e.mousePoint);

   return true;
}

bool EditorToyTool::onMouseDragged(const GuiEvent &e)
{
   Con::executef(this, 2, "onMouseDragged", e.mousePoint);

   return true;
}

bool EditorToyTool::onMouseUp(const GuiEvent &e)
{
   if (!mUseMouseDown)
      return false;

   Con::executef(this, 2, "onMouseUp", e.mousePoint);

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
/*
void EditorToyTool::drawNut(Point2I pos)
{
   RectI rect(pos.x - mNutSize, pos.y - mNutSize, 2 * mNutSize + 1, 2 * mNutSize + 1);
   rect.inset(1, 1);
   dglDrawRectFill(rect, mNutCol);
   rect.inset(-1, -1);
   dglDrawRect(rect, mNutOutCol);
}

void EditorToyTool::drawArrow(Point2I pos)
{
   RectI rect((F32)(pos.x - (F32)mNutSize), (F32)(pos.y - (F32)mNutSize), (F32)(2.0f * mNutSize), (F32)(2.0f * mNutSize));

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ubv(&mNutCol.blue);

   glBegin(GL_TRIANGLE_STRIP);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x - (rect.extent.x / 2.0f) + 0.5f, rect.point.y + rect.extent.y + (rect.extent.y / 2.0f) + 0.5f);
   glEnd();

   glColor4ubv(&mNutOutCol.blue);

   glBegin(GL_LINE_STRIP);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + rect.extent.x - (rect.extent.x / 2.0f) + 0.5f, rect.point.y + rect.extent.y + (rect.extent.y / 2.0f) + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + rect.extent.y + 0.5f);
   glVertex2f(rect.point.x + 0.5f, rect.point.y + 0.5f);
   glEnd();

}

bool EditorToyTool::inNut(Point2I pt, S32 x, S32 y)
{
   S32 size = mNutSize << 1;

   S32 px = pt.x - x;
   S32 py = pt.y - y;

   return px <= size && px >= -size && py <= size && py >= -size;
}
*/