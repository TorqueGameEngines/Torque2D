ConsoleMethodGroupBeginWithDocs(EditorToySceneWindow, SceneWindow)

ConsoleMethod(EditorToySceneWindow, setScene, void, 3, 3, "EditorToy Scene for the EditorSceneWindow")
{
   object->setScene(dynamic_cast<Scene*>(Sim::findObject(argv[2])));
}

ConsoleMethod(EditorToySceneWindow, getActiveSelection, ConsoleInt, 2, 2, "EditorToy Scene for the EditorSceneWindow")
{
   if(object->getActiveSelectionSet())
      return object->getActiveSelectionSet()->getId();
   
   return 0;
}

ConsoleMethod(EditorToySceneWindow, setActiveSelection, void, 3, 3, "EditorToy Scene for the EditorSceneWindow")
{

   object->makeActiveSelectionSet(dynamic_cast<EditorToySelection*>(Sim::findObject(argv[2])));
}


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
