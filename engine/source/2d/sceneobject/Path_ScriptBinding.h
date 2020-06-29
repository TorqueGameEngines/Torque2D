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

/*! Attaches a sceneObject to the path.
@param SceneObject The object that will be attached to the path.
@param speed The rate that the sceneObject will follow the path.
@param orient If true, the object will automatically rate to face the direction that it is moving.
@param angOff The offset that will be applied to the rotation if orient is true.
@param snapToNode If true, the object will have to get close to a node instead of reaching it exactly. The distance is based on the distance given a node when it is created.
@param startNode A zero-based integer that represents the node that the object should start at.
@param loop If true, the object will start the path again when it reaches the last node. The object will travel from the last node to the first.
@param maxLoop An integer value describing the number of times the object will travel the path if looping is turned on.
@return No return value.
*/
ConsoleMethodWithDocs(Path, attachObject, ConsoleVoid, 4, 11, (sceneObject, float speed,[float force], [bool orient], [float angOff], [bool snapToNode],[integer startNode],[bool loop],[integer maxLoop]))
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

   F32 force = 3.0f;

   if (argc > 4)
   {
      force = dAtof(argv[4]);
   }

   bool orient = false;
   if (argc > 5)
   {
      orient = dAtob(argv[5]);
   }

   F32 angleOff = 0.0f;
   if (argc > 6)
   {
      angleOff = dAtof(argv[6]);
   }

   bool snapToNode = false;
   if (argc > 7)
   {
      snapToNode = dAtob(argv[7]);
   }

   S32 startNode = 0;
   if (argc > 8)
   {
      startNode = dAtoi(argv[8]);
   }

   bool loop = true;
   if (argc > 9)
   {
      loop = dAtob(argv[9]);
   }

   S32 maxLoop = 0;
   if (argc > 10)
   {
      maxLoop = dAtoi(argv[10]);
   }

   object->attachObject(pSceneObject, speed, force, orient, angleOff, snapToNode, startNode, loop, maxLoop);

}

/*! Removes a sceneObject from a path.
@param SceneObject The object that will be detached from the path.
@return No return value.
*/
ConsoleMethodWithDocs(Path, detachObject, ConsoleVoid, 3, 3, (sceneObject))
{
   // Set Group.
   SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

   if (pSceneObject)
      object->detachObject(pSceneObject);
   else
      Con::warnf("Path::detachObject() - Could not find the specified object '%s'.", argv[2]);
}

/*! Adds a node to a path.
@param x The horizontal position of the node in world units.
@param y The vertical position of the node in world units.
@param distance The distance a from a node that it object must reach before it is considered to have reached the node, if snapToNode is set true for the object.
@return No return value.
*/
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
