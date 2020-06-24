//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
ConsoleMethodWithDocs(Path, attachObject, ConsoleVoid, 4, 10, (sceneObject, float speed, [bool orient], [float angOff], [bool snapToNode],[integer startNode],[bool loop],[integer maxLoop]))
{
   // Set Group.
   SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));
   // Did we find the object?
   if (!pSceneObject)
   {
      // No, so warn.
      Con::warnf("Path::attachObject() - Could not find the specified object '%s'.", argv[2]);
      return;
   }

   F32 speed = dAtof(argv[3]);

   bool orient = false;
   if (argc > 4)
   {
      orient = dAtob(argv[4]);
   }

   F32 angleOff = 0.0f;
   if (argc > 5)
   {
      angleOff = dAtof(argv[5]);
   }

   bool snapToNode = false;
   if (argc > 6)
   {
      snapToNode = dAtob(argv[6]);
   }

   S32 startNode = 0;
   if (argc > 7)
   {
      startNode = dAtoi(argv[7]);
   }

   bool loop = true;
   if (argc > 8)
   {
      loop = dAtob(argv[8]);
   }

   S32 maxLoop = 0;
   if (argc > 9)
   {
      maxLoop = dAtoi(argv[9]);
   }

   object->attachObject(pSceneObject, speed, orient, angleOff, snapToNode, startNode, loop, maxLoop);

}

ConsoleMethodWithDocs(Path, detachObject, ConsoleVoid, 3, 3, (sceneObject))
{
   // Set Group.
   SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

   if (pSceneObject)
      object->detachObject(pSceneObject);
   else
      Con::warnf("Path::detachObject() - Could not find the specified object '%s'.", argv[2]);
}

ConsoleMethodWithDocs(Path, addNode, ConsoleVoid, 3, 6, (float x, float y, [float distance], [float weight]))
{
   Vector2 position;
   position.Set(dAtof(argv[2]), dAtof(argv[3]));

   F32 distance = 0.0f;
   if (argc > 4)
   {
      distance = dAtof(argv[4]);
   }

   F32 weight = 0.0f;
   if(argc > 5)
   {
      weight = dAtof(argv[5]);
   }

   object->addNode(position, distance, weight);
}