ConsoleMethodGroupBeginWithDocs(EditorToySceneWindow, SceneWindow)

ConsoleMethodWithDocs(EditorToySceneWindow, getLayerMask, ConsoleInt, 2, 2, ())
{

   return object->getLayerMask();
}

ConsoleMethodWithDocs(EditorToySceneWindow, getGroupMask, ConsoleInt, 2, 2, ())
{
   return object->getGroupMask();
}

ConsoleMethodWithDocs(EditorToySceneWindow, setLayerMask, ConsoleVoid, 3, 3, (scene))
{
   object->setLayerMask(dAtoi(argv[2]));
}

ConsoleMethodWithDocs(EditorToySceneWindow, setGroupMask, ConsoleVoid, 3, 3, (scene))
{
   object->setGroupMask(dAtoi(argv[2]));
}

ConsoleMethodWithDocs(EditorToySceneWindow, setTool, ConsoleVoid, 3, 3, (tool))
{
   return object->setTool(dynamic_cast<EditorToyTool*>(Sim::findObject(argv[2])));
}

ConsoleMethodWithDocs(EditorToySceneWindow, clearTool, ConsoleVoid, 2, 2, ())
{
   object->setTool(NULL);
}
