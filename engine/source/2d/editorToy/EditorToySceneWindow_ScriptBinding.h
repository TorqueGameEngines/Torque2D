ConsoleMethodGroupBeginWithDocs(EditorToySceneWindow, SceneWindow)

ConsoleMethodWithDocs(EditorToySceneWindow, setEditScene, ConsoleVoid, 2, 3, (Scene))
{
   // No scene specified?
   if (argc < 3)
   {
      // No, so reset the scene.
      object->resetEditScene();
      // Finish here.
      return;
   }

   // Find Scene Object.
   Scene* pScene = (Scene*)(Sim::findObject(argv[2]));

   // Validate Object.
   if (!pScene)
   {
      Con::warnf("SceneWindow::setScene() - Couldn't find object '%s'.", argv[2]);
      return;
   }

   // Set Scene.
   object->setEditScene(pScene);
}