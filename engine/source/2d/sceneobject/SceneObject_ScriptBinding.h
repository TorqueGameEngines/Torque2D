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

#ifndef _AUDIODESCRIPTION_H_
#include "audio/audioDescriptions.h"
#endif
/*! Gets the system-wide scene-object count.
    @return The system-wide scene-object count.
*/
ConsoleFunctionWithDocs( getGlobalSceneObjectCount, ConsoleInt, 1, 1, ())
{
    return SceneObject::getGlobalSceneObjectCount();
}

//-----------------------------------------------------------------------------

ConsoleMethodGroupBeginWithDocs(SceneObject, BehaviorComponent)

/*! Add the object to a scene.
    @param scene the scene you wish to add this object to.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, addToScene, ConsoleVoid, 3, 3, (Scene scene))
{
    // Find Scene Object.
    Scene* pScene = dynamic_cast<Scene*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pScene )
    {
        Con::warnf("SceneObject::addToScene() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Add to Scene.
    pScene->addToScene( object );
}   

//-----------------------------------------------------------------------------

/*! Remove the object from the scene.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, removeFromScene, ConsoleVoid, 2, 2, ())
{
    // Check we're in a scene.
    if ( !object->getScene() )
    {
        Con::warnf("SceneObject::removeFromScene() - Object is not in Scene! (%s)", object->getIdString());
        return;
    }

    // Remove from Scene.
    object->getScene()->removeFromScene( object );
}  

//-----------------------------------------------------------------------------

/*! Get the scene the object is in.
    @return (Scene scene) The scene this object is currently in.
*/
ConsoleMethodWithDocs( SceneObject, getScene, ConsoleInt, 2, 2, ())
{
   return object->getScene() ? object->getScene()->getId() : 0;
}

//-----------------------------------------------------------------------------

/*! Enables or disables the object.
    @param status Whether to enable or disable the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setEnabled, ConsoleVoid, 3, 3, (bool status))
{
    // Set Enabled.
    object->setEnabled( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Set Objects' Lifetime.
    @param lifetime The amount of time, in seconds, before the object is automatically deleted.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLifetime, ConsoleVoid, 3, 3, (float lifetime))
{
    // Set Lifetime.
    object->setLifetime( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the Objects Lifetime.
    @return (float lifetime) The objects lifetime left before it is automatically deleted.
*/
ConsoleMethodWithDocs(SceneObject, getLifetime, ConsoleFloat, 2, 2, ())
{
    // Return Lifetime.
    return object->getLifetime();
}

//-----------------------------------------------------------------------------

/*! Sets the objects scene layer (0-31).
    The layer on which to place the object.
    @param layer Integer in the range [0-31].
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayer, ConsoleVoid, 3, 3, (integer layer))
{
    // Set Layer.
    object->setSceneLayer( dAtoi(argv[2]) );
}  

//-----------------------------------------------------------------------------

/*! Gets the Objects Layer.
    @return (integer layer) The scene layer the object is on.
*/
ConsoleMethodWithDocs(SceneObject, getSceneLayer, ConsoleInt, 2, 2, ())
{
    // Return Layer.
    return object->getSceneLayer();
}

//-----------------------------------------------------------------------------

/*! Sets the objects scene layer depth.
    @param layerDepth The layer depth can be any value.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayerDepth, ConsoleVoid, 3, 3, (float layerDepth))
{
    // Set scene layer depth.
    object->setSceneLayerDepth( dAtof(argv[2]) );
}  

//-----------------------------------------------------------------------------

/*! Gets the scene layer depth.
    @return (float layerDepth) The scene layer depth.
*/
ConsoleMethodWithDocs(SceneObject, getSceneLayerDepth, ConsoleFloat, 2, 2, ())
{
    // Return Layer.
    return object->getSceneLayerDepth();
}


//-----------------------------------------------------------------------------

/*! Sets the layer depth to be the front-most within the current layer..
    @return Whether the scene layer depth was adjusted or not.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayerDepthFront, ConsoleBool, 2, 2, ())
{
    return object->setSceneLayerDepthFront();
}

//-----------------------------------------------------------------------------

/*! Sets the layer depth to be the back-most within the current layer..
    @return Whether the scene layer depth was adjusted or not.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayerDepthBack, ConsoleBool, 2, 2, ())
{
    return object->setSceneLayerDepthBack();
}

//-----------------------------------------------------------------------------

/*! Sets the layer depth to be in-front of the object currently in-front within the current layer.
    @return Whether the scene layer depth was adjusted or not.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayerDepthForward, ConsoleBool, 2, 2, ())
{
    return object->setSceneLayerDepthForward();
}

//-----------------------------------------------------------------------------

/*! Sets the layer depth to be behind of the object currently behind within the current layer.
    @return Whether the scene layer depth was adjusted or not.
*/
ConsoleMethodWithDocs(SceneObject, setSceneLayerDepthBackward, ConsoleBool, 2, 2, ())
{
    return object->setSceneLayerDepthBackward();
}

//-----------------------------------------------------------------------------

/*! Sets the objects group (0-31).
    The scene group to place the object in.
    @param group Integer in the range [0-31].
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSceneGroup, ConsoleVoid, 3, 3, (integer group))
{
    // Set Group.
    object->setSceneGroup( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the Objects Group.
    @return (integer group) The scene group the object is on.
*/
ConsoleMethodWithDocs(SceneObject, getSceneGroup, ConsoleInt, 2, 2, ())
{
    // Return Group.
    return object->getSceneGroup();
}

//-----------------------------------------------------------------------------

/*! Sets the objects area.
    @param x1 The lower left corner x position.
    @param y1 The lower left corner y position.
    @param x2 The upper right corner x position.
    @param y2 The upper right corner y position.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setArea, ConsoleVoid, 3, 6, (float x1, float y1, float x2, float y2))
{
   // Upper left and lower right bound.
   Vector2 v1, v2;

   // Grab the number of elements in the first two parameters.
   U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
   U32 elementCount2 = 1;
   if (argc > 3)
      elementCount2 = Utility::mGetStringElementCount(argv[3]);

   // ("x1 y1 x2 y2")
   if ((elementCount1 == 4) && (argc == 3))
   {
       v1 = Utility::mGetStringElementVector(argv[2]);
       v2 = Utility::mGetStringElementVector(argv[2], 2);
   }
   
   // ("x1 y1", "x2 y2")
   else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc == 4))
   {
      v1 = Utility::mGetStringElementVector(argv[2]);
      v2 = Utility::mGetStringElementVector(argv[3]);
   }
   
   // (x1, y1, x2, y2)
   else if (argc == 6)
   {
       v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
       v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
   }
   
   // Invalid
   else
   {
      Con::warnf("SceneObject::setArea() - Invalid number of parameters!");
      return;
   }

   // Set Area.
   object->setArea( v1, v2 );
}

//-----------------------------------------------------------------------------

/*! Gets the object's area.
    @return (float x1/float y1/float x2/float y2) A space separated list of the lower left corner x and y position and the upper right corner x and y positions.
*/
ConsoleMethodWithDocs(SceneObject, getArea, ConsoleString, 2, 2, ())
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(64);
    // Format Buffer.
    dSprintf(pBuffer, 64, "%g %g %g %g", position.x-halfSize.x, position.y-halfSize.y, position.x+halfSize.x, position.y+halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets the upper left point of the object.
    @return (float x1/float y1) The lower left corner x and y position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getAreaMin, ConsoleString, 2, 2, ())
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 256, "%g %g", position.x-halfSize.x, position.y-halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets the lower right point of the object.
    @return (float x2/float y2) The upper right corner x and y position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getAreaMax, ConsoleString, 2, 2, (...))
{
    // Fetch position and half-size.
    const Vector2 position = object->getPosition();
    const Vector2 halfSize = object->getHalfSize();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);
    // Format Buffer.
    dSprintf(pBuffer, 32, "%g %g", position.x+halfSize.x, position.y+halfSize.y);
    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets whether the object automatically sizes itself or not.
    You cannot set the size of an object that automatically sizes itself.
    @return Whether the object automatically sizes itself or not.
*/
ConsoleMethodWithDocs(SceneObject, getAutoSizing, ConsoleBool, 2, 2, ())
{
    return object->getAutoSizing();
}

//-----------------------------------------------------------------------------

/*! Sets the objects size.
    @param width The width of the object.
    @param height The height of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSize, ConsoleVoid, 3, 4, (float width, float height))
{
    F32 width, height;

    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ((elementCount == 2) && (argc == 3))
    {
        width = dAtof(Utility::mGetStringElement(argv[2], 0));
        height = dAtof(Utility::mGetStringElement(argv[2], 1));
    }

    // (width, [height])
    else if (elementCount == 1)
    {
        width = dAtof(argv[2]);

        if (argc > 3)
            height = dAtof(argv[3]);
        else
            height = width;
    }

    // Invalid
    else
    {
        Con::warnf("SceneObject::setSize() - Invalid number of parameters!");
        return;
    }

    // Is the object auto-sizing?
    if ( object->getAutoSizing() )
    {
        // Yes, so warn.
        Con::warnf( "Cannot set the size of a type '%s' as it automatically sizes itself.", object->getClassName() );
        return;
    }

    // Set Size.
    object->setSize(Vector2(width, height));
}  

//-----------------------------------------------------------------------------

/*! Gets the objects size.
    @return (float width/float height) The width and height of the object.
*/
ConsoleMethodWithDocs(SceneObject, getSize, ConsoleString, 2, 2, ())
{
    return object->getSize().scriptThis();
} 

//-----------------------------------------------------------------------------

/*! Sets the width of the object.
    @param width The width of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSizeX, ConsoleVoid, 3, 3, (float width))
{
    // Set Size X-Component.
    object->setSize( Vector2( dAtof(argv[2]), object->getSize().y ) );
}

//-----------------------------------------------------------------------------

/*! Sets the height of the object.
    @param height The height of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSizeY, ConsoleVoid, 3, 3, (float height))
{
    // Set Size Y-Component.
    object->setSize( Vector2( object->getSize().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets the width of the object.
    @return (float width) The width of the object.
*/
ConsoleMethodWithDocs(SceneObject, getSizeX, ConsoleFloat, 2, 2, ())
{
    // Get Size X-Component.
    return object->getSize().x;
}

//-----------------------------------------------------------------------------

/*! Gets the height of the object.
    @return (float height) The height of the object.
*/
ConsoleMethodWithDocs(SceneObject, getSizeY, ConsoleFloat, 2, 2, ())
{
    // Get Size Y-Component.
    return object->getSize().y;
}

//-----------------------------------------------------------------------------

/*! Sets the width of the object.
    @param width The width of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setWidth, ConsoleVoid, 3, 3, (float width))
{
    // Set Size Width Component.
    object->setSize( Vector2( dAtof(argv[2]), object->getSize().y ) );
}

//-----------------------------------------------------------------------------

/*! Sets the height of the object.
    @param height The height of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setHeight, ConsoleVoid, 3, 3, (float height))
{
    // Set Size Height Component.
    object->setSize( Vector2( object->getSize().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets the width of the object.
    @return (float width) The width of the object.
*/
ConsoleMethodWithDocs(SceneObject, getWidth, ConsoleFloat, 2, 2, ())
{
    // Get Size Width Component.
    return object->getSize().x;
}

//-----------------------------------------------------------------------------

/*! Gets the height of the object.
    @return (float height) The height of the object.
*/
ConsoleMethodWithDocs(SceneObject, getHeight, ConsoleFloat, 2, 2, ())
{
    // Get Size Height Component.
    return object->getSize().y;
}

//-----------------------------------------------------------------------------

/*! Gets the axis-aligned bounding-box of the object.
    @return (lowerXY upperXY) A space separated list of the lower left and upper right bounds.
*/
ConsoleMethodWithDocs(SceneObject, getAABB, ConsoleString, 2, 2, ())
{
    // Fetch the AABB
    const b2AABB aabb = object->getAABB();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer( 64 );

    // Format Buffer.
    dSprintf( pBuffer, 64, "%g %g %g %g", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y );
    
    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets the objects position.
    @param x The position of the object along the horizontal axis.
    @param y The position of the object along the vertical axis.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setPosition, ConsoleVoid, 3, 4, (float x, float y))
{
    // The new position.
    b2Vec2 position;

    // Elements in the first argument.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc == 3))
        position = Utility::mGetStringElementVector(argv[2]);

    // (x, y)
    else if ((elementCount == 1) && (argc == 4))
        position.Set(dAtof(argv[2]), dAtof(argv[3]));

    // Invalid
    else
    {
        Con::warnf("SceneObject::setPosition() - Invalid number of parameters!");
        return;
    }

    // Set Position.
    object->setPosition(position);
} 

//-----------------------------------------------------------------------------

/*! Sets the objects x position.
    @param x The horizontal position of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setPositionX, ConsoleVoid, 3, 3, (float x))
{
    // Set Position X-Component.
    object->setPosition( b2Vec2( dAtof(argv[2]), object->getPosition().y ) );
}

//-----------------------------------------------------------------------------

/*! Sets the objects y position.
    @param y The vertical position of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setPositionY, ConsoleVoid, 3, 3, (float y))
{
    // Set Position Y-Component.
    object->setPosition( b2Vec2( object->getPosition().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets the object's position.
    @return (float x/float y) The x and y (horizontal and vertical) position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getPosition, ConsoleString, 2, 2, ())
{
    // Get position.
    return object->getPosition().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the object's position.
	@return The x (horizontal) position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getPositionX, ConsoleFloat, 2, 2, ())
{
	// Get position.
	return object->getPosition().x;
}

//-----------------------------------------------------------------------------

/*! Gets the object's position.
	@return The y (vertical) position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getPositionY, ConsoleFloat, 2, 2, ())
{
	// Get position.
	return object->getPosition().y;
}

//-----------------------------------------------------------------------------

/*! Gets the current render position.
    @return (float x/float y) The x and y (horizontal and vertical) render position of the object.
*/
ConsoleMethodWithDocs(SceneObject, getRenderPosition, ConsoleString, 2, 2, ())
{
   // Get render position.
    return object->getRenderPosition().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the objects angle.
    @param angle The angle of the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setAngle, ConsoleVoid, 3, 3, (float angle))
{
    // Set Rotation.
    object->setAngle( mDegToRad( dAtof(argv[2]) ) );
}   

//-----------------------------------------------------------------------------

/*! Gets the object's angle.
    @return (float angle) The object's current angle.
*/
ConsoleMethodWithDocs(SceneObject, getAngle, ConsoleFloat, 2, 2, ())
{
    // Return angle.
    return mRadToDeg( object->getAngle());
}

//-----------------------------------------------------------------------------

/*! Gets the object's render angle.
    @return (float rotation) The object's current render angle.
*/
ConsoleMethodWithDocs(SceneObject, getRenderAngle, ConsoleFloat, 2, 2, ())
{
    // Return Rotation.
    return mRadToDeg( object->getRenderAngle() );
}

//-----------------------------------------------------------------------------

/*! Whether the object angle is fixed or not.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setFixedAngle, ConsoleVoid, 3, 3, (bool status?))
{
    // Set fixed angle.
    object->setFixedAngle( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the angle is fixed or not.
    @return (bool status?) Whether the angle is fixed or not.
*/
ConsoleMethodWithDocs(SceneObject, getFixedAngle, ConsoleBool, 2, 2, ())
{
    // Get fixed angle.
    return object->getFixedAngle();
}

//-----------------------------------------------------------------------------

/*! Gets the local center of mass.
    @return (float localCenter) Gets the local center of mass.
*/
ConsoleMethodWithDocs(SceneObject, getLocalCenter, ConsoleString, 2, 2, ())
{
    // Get local center.
    return object->getLocalCenter().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the world center of mass.
    @return (float worldCenter) Gets the world center of mass.
*/
ConsoleMethodWithDocs(SceneObject, getWorldCenter, ConsoleString, 2, 2, ())
{
    // Get world center.
    return object->getWorldCenter().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns a local point from the world point.
    Converts a point in world space to local space.
    @param worldPointX/Y The world point to convert into a local point.
    @return (float localPointX/Y) The local point that was converted from the world point passed.
*/
ConsoleMethodWithDocs(SceneObject, getLocalPoint, ConsoleString, 3, 4, (float worldPointX/Y))
{
   // The new position.
   Vector2 worldPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      worldPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      worldPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getLocalPoint() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate local coordinate.
    Vector2 localPoint = object->getLocalPoint(worldPoint);

    return localPoint.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns a world point from the local point.
    Converts a point in local space to world space.
    @param localPointX/Y The local point to convert into a world point.
    @return (float worldPointX/Y) The world point that was converted from the local point passed.
*/
ConsoleMethodWithDocs(SceneObject, getWorldPoint, ConsoleString, 3, 4, (float localPointX/Y))
{
   // The new position.
   Vector2 localPoint;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      localPoint = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      localPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
        Con::warnf("SceneObject::getWorldPoint() - Invalid number of parameters!");
        return NULL;
   }

    // Calculate world coordinate.
    Vector2 worldPoint = object->getWorldPoint(localPoint);

    return worldPoint.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns a local vector from the world vector.
    Converts a vector in world space to local space.
    @param worldVectorX/Y The world vector to convert into a vector point.
    @return (float localVectorX/Y) The local vector that was converted from the world vector passed.
*/
ConsoleMethodWithDocs(SceneObject, getLocalVector, ConsoleString, 3, 4, (float worldVectorX/Y))
{
   // The new vector.
   Vector2 worldVector;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      worldVector = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      worldVector = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getLocalVector() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate local vector.
    Vector2 localVector = object->getLocalVector(worldVector);

    return localVector.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns a world vector from the local vector.
    Converts a vector in local space to world space.
    @param localVectorX/Y The local vector to convert into a world vector.
    @return (float worldVectorX/Y) The world vector that was converted from the local vector passed.
*/
ConsoleMethodWithDocs(SceneObject, getWorldVector, ConsoleString, 3, 4, (float localVectorX/Y))
{
   // The new vector.
   Vector2 localVector;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      localVector = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      localVector = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getWorldVector() - Invalid number of parameters!");
      return NULL;
   }

    // Calculate world vector.
    Vector2 worldVector = object->getWorldVector(localVector);

    return worldVector.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Returns whether the world point intersects with the objects OOBB or not.
    @param worldPointX/Y The world point to check.
    @return (bool isInside) Whether the world point intersects with the objects OOBB or not.
*/
ConsoleMethodWithDocs(SceneObject, getIsPointInOOBB, ConsoleBool, 3, 4, (worldPointX/Y))
{
   Vector2 worldPoint;

   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("X Y")
   if ((elementCount == 2) && (argc == 3))
      worldPoint = Utility::mGetStringElementVector(argv[2]);

   // (X, Y)
   else if ((elementCount == 1) && (argc == 4))
      worldPoint = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::getIsPointInOOBB() - Invalid number of parameters!");
      return false;
   }

    // Calculate if point intersects the OOBB.
    return object->getIsPointInOOBB( worldPoint );
}

//-----------------------------------------------------------------------------

/*! Returns whether the world point intersects with the specified collision shape or not.
    @param worldPointX/Y The world point to check.
    @param shapeIndex - The index of the collision shape.
    @return (bool isInside) Whether the world point intersects with the specified collision shape or not.
*/
ConsoleMethodWithDocs(SceneObject, getIsPointInCollisionShape, ConsoleBool, 4, 5, (int shapeIndex, worldPointX/Y))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getIsPointInCollisionShape() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Fetch shape index.
    Vector2 worldPoint;

    U32 elementCount = Utility::mGetStringElementCount(argv[3]);

    // ("X Y")
    if ((elementCount == 2) && (argc == 4))
        worldPoint = Utility::mGetStringElementVector(argv[2]);

    // (X, Y)
    else if ((elementCount == 1) && (argc == 5))
        worldPoint = Vector2(dAtof(argv[3]), dAtof(argv[4]));

    // Invalid
    else
    {
        Con::warnf("SceneObject::getIsPointInCollisionShape() - Invalid number of parameters!");
        return false;
    }

    // Calculate if point intersects the collision shape.
    return object->getIsPointInCollisionShape( shapeIndex, worldPoint );
}

//-----------------------------------------------------------------------------

/*! Sets the body type.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setBodyType, ConsoleVoid, 3, 3, (bodyType type))
{
    // Fetch body type.
    const b2BodyType type = SceneObject::getBodyTypeEnum(argv[2]);

    // Check for error.
    if ( type != b2_staticBody && type != b2_kinematicBody && type != b2_dynamicBody )
        return;

    // Set body type.
    object->setBodyType( type );
}

//-----------------------------------------------------------------------------

/*! Gets the body type.
    @return The body type.
*/
ConsoleMethodWithDocs(SceneObject, getBodyType, ConsoleString, 2, 2, ())
{
    return SceneObject::getBodyTypeDescription( object->getBodyType() );
}

//-----------------------------------------------------------------------------

/*! Sets the body active status.
    @param status - Whether the body should be active or not (defaults to true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setActive, ConsoleVoid, 2, 3, ([bool status?]))
{
    object->setActive( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the body active status.
    @return (bool status) Whether the body is active or not.
*/
ConsoleMethodWithDocs(SceneObject, getActive, ConsoleBool, 2, 2, ())
{
    return object->getActive();
}

//-----------------------------------------------------------------------------

/*! Sets whether the body is awake or not.
    @param status - Whether the body should be awake or not (defaults to true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setAwake, ConsoleVoid, 2, 3, ([bool status?]))
{
    object->setAwake( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets whether the body is awake or not.
    @return (bool status) Whether the body is awake or not.
*/
ConsoleMethodWithDocs(SceneObject, getAwake, ConsoleBool, 2, 2, ())
{
    return object->getAwake();
}

//-----------------------------------------------------------------------------

/*! Sets whether the body is a bullet or not.
    @param status - Whether the body should be a bullet (fast moving body) or not (defaults to true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setBullet, ConsoleVoid, 2, 3, ([bool status?]))
{
    object->setBullet( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets whether the body is a bullet or not.
    @return (bool status) Whether the body is a bullet or not.
*/
ConsoleMethodWithDocs(SceneObject, getBullet, ConsoleBool, 2, 2, ())
{
    return object->getBullet();
}

//-----------------------------------------------------------------------------

/*! Sets whether the body is allowed to sleep or not.
    @param status - Whether sleeping is allowed on the body or not (defaults to true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSleepingAllowed, ConsoleVoid, 2, 3, ([bool status?]))
{
    object->setSleepingAllowed( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets whether the body is allowed to sleep or not.
    @return (bool status) Whether the body is allowed to sleep or not.
*/
ConsoleMethodWithDocs(SceneObject, getSleepingAllowed, ConsoleBool, 2, 2, ())
{
    return object->getSleepingAllowed();
}

//-----------------------------------------------------------------------------

/*! Gets the total mass of the body.
    @return (float mass) The total mass of the body.  If object is not in a scene then mass is always zero.
*/
ConsoleMethodWithDocs(SceneObject, getMass, ConsoleFloat, 2, 2, ())
{
    return object->getMass();
}

//-----------------------------------------------------------------------------

/*! Gets the rotation inertia of the body around the local origin.
    @return (float inertia) rotation inertia of the body around the local origin (Kg-m^2).  If object is not in a scene then mass is always zero.
*/
ConsoleMethodWithDocs(SceneObject, getInertia, ConsoleFloat, 2, 2, ())
{
    return object->getInertia();
}

//-----------------------------------------------------------------------------

/*! Sets the collision suppress status.
    @param status Whether to suppress the collision or not (defaults to true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionSuppress, ConsoleVoid, 2, 3, ([bool status?]))
{
    // Set Collision Suppress.
    object->setCollisionSuppress( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the collision suppress status.
    @return (bool status) Whether to supress the collision or not.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionSuppress, ConsoleBool, 2, 2, ())
{
    // Get Collision Callback.
    return object->getCollisionSuppress();
}

//-----------------------------------------------------------------------------

/*! Sets the one way collision status for chain and edge shapes that belong to this object.
    @param status Whether collisions happen only in one direction or not (defaults to false).
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionOneWay, ConsoleVoid, 2, 3, ([bool status?]))
{
    object->setCollisionOneWay( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the one way collision status for chain and edge shapes that belong to this object.
    @return (bool status) Whether collisions happen only in one direction or not.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionOneWay, ConsoleBool, 2, 2, ())
{
    return object->getCollisionOneWay();
}

//-----------------------------------------------------------------------------

/*! Sets whether to gather contacts or not.
    @param gatherContacts Whether to gather contacts or not.  By default contact gather is off as it can become expensive if a lot of contacts are being processed.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setGatherContacts, ConsoleVoid, 3, 3, (bool gatherContacts))
{
    object->setGatherContacts( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the gathering of contacts is active or not.
    @return Whether the gathering of contacts is active or not.
*/
ConsoleMethodWithDocs(SceneObject, getGatherContacts, ConsoleBool, 2, 2, ())
{
    return object->getGatherContacts();
}

//-----------------------------------------------------------------------------

/*! Gets the current contact count.  This will always be zero if contact gathering is off.
    @return (int count) - Gets the current contact count.
*/
ConsoleMethodWithDocs(SceneObject, getContactCount, ConsoleInt, 2, 2, ())
{
    return object->getCurrentContactCount();
}

//-----------------------------------------------------------------------------

/*! Gets the contact at the specified contact index.
    @param contactIndex The contact at the specified index.
    @return The contact at the specified index.
*/
ConsoleMethodWithDocs(SceneObject, getContact, ConsoleString, 3, 3, (contactIndex))
{
    // Fetch contact index.
    const S32 contactIndex = dAtoi(argv[2]);

    // Is the contact index valid?
    if ( contactIndex < 0 || contactIndex >= (S32)object->getCurrentContactCount() )
    {
        // No, so warn.
        Con::warnf("SceneObject::getContact() - The contact index '%d' is out of bounds.", contactIndex );
        return NULL;
    }

    // Fetch contacts.
    const Scene::typeContactVector* pCurrentContacts = object->getCurrentContacts();

    // Sanity!
    AssertFatal( pCurrentContacts != NULL, "SceneObject::getContact() - Contacts not initialized correctly." );

    // Fetch contact.
    const TickContact& tickContact = (*pCurrentContacts)[contactIndex];

    // Fetch scene objects.
    SceneObject* pSceneObjectThis;
    SceneObject* pSceneObjectCollider;
    S32 shapeIndexThis;
    S32 shapeIndexCollider;

    // Calculate the respective colliding objects.
    if ( tickContact.mpSceneObjectA == object )
    {
        pSceneObjectThis = tickContact.mpSceneObjectA;
        pSceneObjectCollider = tickContact.mpSceneObjectB;
        shapeIndexThis = pSceneObjectThis->getCollisionShapeIndex( tickContact.mpFixtureA );
        shapeIndexCollider = pSceneObjectCollider->getCollisionShapeIndex( tickContact.mpFixtureB );
    }
    else
    {
        pSceneObjectThis = tickContact.mpSceneObjectB;
        pSceneObjectCollider = tickContact.mpSceneObjectA;
        shapeIndexThis = pSceneObjectThis->getCollisionShapeIndex( tickContact.mpFixtureB );
        shapeIndexCollider = pSceneObjectCollider->getCollisionShapeIndex( tickContact.mpFixtureA );
    }

    // Sanity!
    AssertFatal( shapeIndexThis >= 0, "SceneObject::getContact() - Cannot find shape index reported on physics proxy of a fixture." );
    AssertFatal( shapeIndexCollider >= 0, "SceneObject::getContact() - Cannot find shape index reported on physics proxy of a fixture." );

    // Fetch normal and contact points.
    const U32& pointCount = tickContact.mPointCount;
    const b2Vec2& normal = tickContact.mWorldManifold.normal;
    const b2Vec2& point1 = tickContact.mWorldManifold.points[0];
    const b2Vec2& point2 = tickContact.mWorldManifold.points[1];

    // Fetch collision impulse information
    const F32 normalImpulse1 = tickContact.mNormalImpulses[0];
    const F32 normalImpulse2 = tickContact.mNormalImpulses[1];
    const F32 tangentImpulse1 = tickContact.mTangentImpulses[0];
    const F32 tangentImpulse2 = tickContact.mTangentImpulses[1];

    // Format buffer.
    char* pReturnBuffer = Con::getReturnBuffer(128);
    if ( pointCount == 2 )
    {
        dSprintf(pReturnBuffer, 128,
            "%d %d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
            pSceneObjectCollider->getId(),
            shapeIndexThis, shapeIndexCollider,
            normal.x, normal.y,
            point1.x, point1.y,
            normalImpulse1,
            tangentImpulse1,
            point2.x, point2.y,
            normalImpulse2,
            tangentImpulse2 );
    }
    else if ( pointCount == 1 )
    {
        dSprintf(pReturnBuffer, 128,
            "%d %d %d %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
            pSceneObjectCollider->getId(),
            shapeIndexThis, shapeIndexCollider,
            normal.x, normal.y,
            point1.x, point1.y,
            normalImpulse1,
            tangentImpulse1 );
    }
    else
    {
        dSprintf(pReturnBuffer, 64,
            "%d %d %d",
            pSceneObjectCollider->getId(),
            shapeIndexThis, shapeIndexCollider );
    }

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets the collision masks.
    Collision masks limit the objects that are collided with based on their scene group and scene Layer. In order for two objects to collide, the sending object must have the receiving object's Graph Group and Rendering Layer in its masks. The receiving object's masks, however, do not have to contain the sending object's Graph Group and Rendering Layer.
    @param sceneGroupMask The scene group(s) to enable collision with.
    @param sceneLayerMask The scene layer(s) to enable collision with. If sceneLayerMask is not specified then all scene layers will be included.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionMasks, ConsoleVoid, 3, 4, (groupMask, [layerMask]))
{
    // Calculate Group-Mask.
    const U32 groupMask = dAtoi(argv[2]);
    // Calculate Layer-Mask.
    const U32 layerMask = (argc > 3) ? dAtoi(argv[3]) : MASK_ALL;

    // Set Collision Masks.
    object->setCollisionGroupMask( groupMask );
    object->setCollisionLayerMask( layerMask );
}

//-----------------------------------------------------------------------------

/*! Gets the collision group/layer masks.
    @return (groupMask/layerMask) The scene group(s) and scene layer(s) the object is enabled to collide with.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionMasks, ConsoleString, 2, 2, (...))
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getCollisionGroupMask(), (U32)object->getCollisionLayerMask());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets the collision masks against a specific object.
    @param object The SceneObject to set collision masks against
    @param clearMasks Whether or not to clear the collision masks before setting them against the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionAgainst, ConsoleVoid, 3, 4, (SceneObject object, [clearMasks? = false]))
{
    // Find SceneObject Object.
    const SceneObject* pSceneObject = dynamic_cast<SceneObject*>(Sim::findObject(argv[2]));

    // Validate Object.
    if ( !pSceneObject )
    {
        Con::warnf("SceneObject::setCollisionAgainst() - Couldn't find/Invalid object '%s'.", argv[2]);
        return;
    }

    // Fetch ClearMasks Option.
    bool clearMasks = argc >= 4 ? dAtob(argv[3]) : false;

    // Set Collision Against.
    object->setCollisionAgainst( pSceneObject, clearMasks );
}

//-----------------------------------------------------------------------------

/*! Sets the collision layers(s).
    @param layers A list of layers to collide with.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionLayers, ConsoleVoid, 2, 2 + MASK_BITCOUNT, (layers))
{
    // Set to all if no arguments.
    if ( argc == 2 )
    {
        object->setCollisionLayerMask(MASK_ALL);
        return;
    }

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setCollisionLayerMask(MASK_ALL);
        return;
    }
    else if ( elementCount == 1 )
    {
        if ( dStricmp( argv[2], "all" ) == 0 )
        {
            object->setCollisionLayerMask(MASK_ALL);
            return;
        }
        else if ( dStricmp( argv[2], "none" ) == 0 || dStricmp( argv[2], "off" ) == 0 )
        {
            object->setCollisionLayerMask(0);
            return;
        }
    }

    // The mask.
    U32 mask = 0;

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionLayers() - Invalid layer specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set Collision Layers
    object->setCollisionLayerMask(mask);
}

//-----------------------------------------------------------------------------

/*! Sets the collision group(s).
    @param groups A list of collision groups to collide with.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionGroups, ConsoleVoid, 2, 2 + MASK_BITCOUNT, (groups))
{
    // Set to all if no arguments.
    if ( argc == 2 )
    {
        object->setCollisionGroupMask(MASK_ALL);
        return;
    }

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        object->setCollisionGroupMask(MASK_ALL);
        return;
    }
    else if ( elementCount == 1 )
    {
        if ( dStricmp( argv[2], "all" ) == 0 )
        {
            object->setCollisionGroupMask(MASK_ALL);
            return;
        }
        else if ( dStricmp( argv[2], "none" ) == 0 || dStricmp( argv[2], "off" ) == 0 )
        {
            object->setCollisionGroupMask(0);
            return;
        }
    }

    // The mask.
    U32 mask = 0;

    // Space separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            S32 bit = dAtoi(Utility::mGetStringElement(argv[2], i));
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }
         
            mask |= (1 << bit);
        }
    }

    // Comma separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            S32 bit = dAtoi(argv[i]);
         
            // Make sure the group is valid.
            if ((bit < 0) || (bit >= MASK_BITCOUNT))
            {
            Con::warnf("SceneObject::setCollisionGroups() - Invalid group specified (%d); skipped!", bit);
            continue;
            }

            mask |= (1 << bit);
        }
    }
    // Set Collision Groups.
    object->setCollisionGroupMask(mask);
}

//-----------------------------------------------------------------------------

/*! Gets the collision layers.
    @return (collisionLayers) A list of collision layers.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionLayers, ConsoleString, 2, 2, ())
{
    U32 mask = object->getCollisionLayerMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

//-----------------------------------------------------------------------------

/*! Gets the collision groups.
    @return (collisionGroups) A list of collision groups.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionGroups, ConsoleString, 2, 2, ())
{
    U32 mask = object->getCollisionGroupMask();

    bool first = true;
    char* bits = Con::getReturnBuffer(128);
    bits[0] = '\0';
    for (S32 i = 0; i < MASK_BITCOUNT; i++)
    {
        if (mask & BIT(i))
        {
            char bit[4];
            dSprintf(bit, 4, "%s%d", first ? "" : " ", i);
            first = false;
            dStrcat(bits, bit);
        }
    }

    return bits;
}

//-----------------------------------------------------------------------------

/*! Sets the default density to use when creating collision shapes.
    @param density The default density to use when creating collision shapes.
    @param updateShapes Whether to update the density on any existing shapes or not (default is true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setDefaultDensity, ConsoleVoid, 3, 4, (float density, [bool updateShapes]))
{
    // Fetch density.
    const F32 density = dAtof(argv[2]);
    if ( density < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultDensity() - Invalid density of %g.", density);
    }

    // Set default density.
    object->setDefaultDensity( density, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the default density.
    @return (float density) The default density.
*/
ConsoleMethodWithDocs(SceneObject, getDefaultDensity, ConsoleFloat, 2, 2, ())
{
    // Get default density.
    return object->getDefaultDensity();
}

//-----------------------------------------------------------------------------

/*! Sets the default friction to use when creating collision shapes.
    @param friction The default friction to use when creating collision shapes.
    @param updateShapes Whether to update the friction on any existing shapes or not (default is true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setDefaultFriction, ConsoleVoid, 3, 4, (float friction, [bool updateShapes]))
{
    // Fetch friction.
    const F32 friction = dAtof(argv[2]);
    if ( friction < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultFriction() - Invalid friction of %g.", friction);
    }

    // Set default friction.
    object->setDefaultFriction( friction, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the default friction.
    @return (float friction) The default friction.
*/
ConsoleMethodWithDocs(SceneObject, getDefaultFriction, ConsoleFloat, 2, 2, ())
{
    // Get default friction.
    return object->getDefaultFriction();
}

//-----------------------------------------------------------------------------

/*! Sets the default restitution to use when creating collision shapes..
    @param restitution The default restitution to use when creating collision shapes.
    @param updateShapes Whether to update the restitution on any existing shapes or not (default is true).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setDefaultRestitution, ConsoleVoid, 3, 4, (float restitution, [bool updateShapes]))
{
    // Fetch restitution.
    const F32 restitution = dAtof(argv[2]);
    if ( restitution < 0.0f )
    {
        Con::warnf("SceneObject::setDefaultRestitution() - Invalid restitution of %g.", restitution);
    }

    // Set default restitution.
    object->setDefaultRestitution( restitution, argc > 3 ? dAtob(argv[3]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets the default restitution.
    @return (float density) The default restitution.
*/
ConsoleMethodWithDocs(SceneObject, getDefaultRestitution, ConsoleFloat, 2, 2, ())
{
    // Get default restitution.
    return object->getDefaultRestitution();
}

//-----------------------------------------------------------------------------

/*! Sets Objects Linear Velocity.
    Sets the velocity at which the object will move.
    @param velocityX The x component of the velocity.
    @param velocityY The y component of the velocity.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLinearVelocity, ConsoleVoid, 3, 4, (float velocityX, float velocityY))
{
   // The velocity.
   Vector2 velocity;

   // Grab the element count.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // (x, y)
   if ((elementCount == 1) && (argc > 3))
      velocity = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // ("x y")
   else if ((elementCount == 2) && (argc < 4))
      velocity = Utility::mGetStringElementVector(argv[2]);

   // Invalid
   else
   {
      Con::warnf("SceneObject::setLinearVelocity() - Invalid number of parameters!");
      return;
   }

   // Set Linear Velocity.
   object->setLinearVelocity(velocity);
}

//-----------------------------------------------------------------------------

/*! Sets Objects Linear Velocity X-Component.
    @param velocityX The x component of the velocity.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLinearVelocityX, ConsoleVoid, 3, 3, (float velocityX))
{
    // Set Linear Velocity X-Component.
    object->setLinearVelocity( Vector2( dAtof(argv[2]), object->getLinearVelocity().y ) );
}

//-----------------------------------------------------------------------------

/*! Sets Objects Linear Velocity Y-Component.
    @param velocityY The y component of the velocity.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLinearVelocityY, ConsoleVoid, 3, 3, (float velocityY))
{
    // Set Linear Velocity Y-Component.
    object->setLinearVelocity( Vector2( object->getLinearVelocity().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Sets Objects Linear Velocity using Polar-speed.
    Sets the speed and direction at which the object will move.
    This has the same effect as setLinearVelocity(), it just sets the velocity in a different way.
    @param angle The angle of the direction at which the object to move.
    @param speed The speed at which the object will move.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLinearVelocityPolar, ConsoleVoid, 4, 4, (float angle, float speed))
{
    // Renormalize Angle.
    F32 angle = mDegToRad(mFmod(dAtof(argv[2]), 360.0f));

    // Fetch Speed.
    F32 speed = dAtof(argv[3]);

    // Calculate Angle.
    F32 sin, cos;
    mSinCos( angle, sin, cos );

    // Set Gross Linear Velocity.
    object->setLinearVelocity( Vector2( cos*speed, sin*speed ) );
}

//-----------------------------------------------------------------------------

/*! Gets Objects Linear Velocity using Polar angle/speed.
    @return (float angle, float speed) The angle and speed the object is moving at.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocityPolar, ConsoleString, 2, 2, ())
{
    // Get Linear Velocity.
    Vector2 linearVelocity = object->getLinearVelocity();

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%g %g", mRadToDeg(mAtan(linearVelocity.x, linearVelocity.y)), linearVelocity.Length() );

    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets Object Linear Velocity.
    @return (float velocityX, float velocityY) The x and y velocities of the object.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocity, ConsoleString, 2, 2, ())
{
    // Get Linear Velocity.
    Vector2 linearVelocity = object->getLinearVelocity();

    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets Object Linear Velocity X.
    @return (float velocityX) The object's velocity along the x axis.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocityX, ConsoleFloat, 2, 2, ())
{
    // Get Linear Velocity X.
    return object->getLinearVelocity().x;
}

//-----------------------------------------------------------------------------

/*! Gets Object Linear Velocity Y.
    @return (float velocityY) The object's velocity along the y axis.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocityY, ConsoleFloat, 2, 2, ())
{
    // Get Linear Velocity Y.
    return object->getLinearVelocity().y;
}

//-----------------------------------------------------------------------------

/*! Gets the linear velocity from a world point.
    @return (float linearVelocityX/Y) The linear velocity at the world point.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocityFromWorldPoint, ConsoleString, 3, 4, (worldPointX/Y))
{
    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 worldPoint;

    S32 nextArg = 3;
    if ( worldPointElementCount == 1 && argc > 3 )
    {
        worldPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::getLinearVelocityFromWorldPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch linear velocity.
    Vector2 linearVelocity = object->getLinearVelocityFromWorldPoint( worldPoint );
    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the linear velocity from a local point.
    @return (float linearVelocityX/Y) The linear velocity at the local point.
*/
ConsoleMethodWithDocs(SceneObject, getLinearVelocityFromLocalPoint, ConsoleString, 3, 4, (localPointX/Y))
{
    // Local point.
    const U32 localPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 localPoint;

    S32 nextArg = 3;
    if ( localPointElementCount == 1 && argc > 3 )
    {
        localPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( localPointElementCount == 2 )
    {
        localPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::getLinearVelocityFromLocalPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch linear velocity.
    Vector2 linearVelocity = object->getLinearVelocityFromWorldPoint( localPoint );
    return linearVelocity.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets Objects Angular Velocity.
    @param velocity The speed at which the object will rotate.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setAngularVelocity, ConsoleVoid, 3, 3, (float velocity))
{
    // Set Angular Velocity.
    object->setAngularVelocity( mDegToRad( dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets Object Angular Velocity.
    @return (float velocity) The speed at which the object is rotating.
*/
ConsoleMethodWithDocs(SceneObject, getAngularVelocity, ConsoleFloat, 2, 2, ())
{
    // Get Angular Velocity.
    return mRadToDeg( object->getAngularVelocity() );
}

//-----------------------------------------------------------------------------

/*! Sets the linear damping.
    @param damping - The linear damping scale.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setLinearDamping, ConsoleVoid, 3, 3, (float damping))
{
    // Set linear damping.
    object->setLinearDamping( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the linear damping.
    @return (float damping) The linear damping.
*/
ConsoleMethodWithDocs(SceneObject, getLinearDamping, ConsoleFloat, 2, 2, ())
{
    // Get linear damping.
    return object->getLinearDamping();
}

//-----------------------------------------------------------------------------

/*! Sets the angular damping.
    @param damping - The angular damping scale.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setAngularDamping, ConsoleVoid, 3, 3, (float damping))
{
    // Set angular damping.
    object->setAngularDamping( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the angular damping.
    @return (float damping) The angular damping.
*/
ConsoleMethodWithDocs(SceneObject, getAngularDamping, ConsoleFloat, 2, 2, ())
{
    // Get angular damping.
    return object->getAngularDamping();
}

//-----------------------------------------------------------------------------

/*! Moves the object to the specified world point.
    Linear velocity is applied to the object at the given speed in the direction of the target world point.
    The object may never reach the point if other forces act on the target such as collisions.
    If the object moves away from the target the object will stop checking to see if it has arrived at the target.
    @param worldPointX/Y The world point to move the object to.
    @param speed The speed (in m/s) to use to move to the specified point.
    @param autoStop? Whether to automatically set the linear velocity to zero when the object arrives at the target.
    @param snapToTarget? Whether to snap the object to the target point when it is within the margin.
    @param margin? The distance from the target that qualifies as reaching the target.
    @return Whether the move could be started or not.
*/
ConsoleMethodWithDocs(SceneObject, moveTo, ConsoleBool, 4, 7, (worldPoint X/Y, speed, [autoStop = true], [snapToTarget = true], [margin = 0.1]))
{
    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[2]);

    Vector2 worldPoint;

    S32 nextArg = 3;
    if ( worldPointElementCount == 1 && argc > 3 )
    {
        worldPoint.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::moveTo() - Invalid number of parameters!");
        return false;
    }

    // Speed.
    const F32 speed = dAtof(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->moveTo( worldPoint, speed );
    }

    // Auto stop?
    const bool autoStop = dAtob(argv[nextArg++]);

    if ( argc <= nextArg )
    {
        return object->moveTo( worldPoint, speed, autoStop );
    }

    // Snap to target?
    const bool snapToTarget = dAtob(argv[nextArg++]);

    if ( argc <= nextArg )
    {
       return object->moveTo(worldPoint, speed, autoStop, snapToTarget);
    }

    // Margin.
    const F32 margin = dAtof(argv[nextArg++]);

    return object->moveTo( worldPoint, speed, autoStop, snapToTarget, margin );
}

//-----------------------------------------------------------------------------

/*! Rotates the object to the specified angle.
    The angle is rotated to by calculating the initial angular velocity required and applies it.
    The object may never reach the point if it has angular damping applied or collides with another object.
    @param angle The angle to rotate the object to.
    @param speed The speed (in degree/s) to use to rotate to the specified angle.
    @param autoStop? Whether to automatically set the angular velocity to zero when time has elapsed or not
    @param warpToTarget? Whether to rotate instantly to the target angle after the specified time or not in-case the target was not quite reached.
    @return Whether the rotation could be started or not.
*/
ConsoleMethodWithDocs(SceneObject, rotateTo, ConsoleBool, 4, 6, (angle, speed, [autoStop = true], [warpToTarget = true]))
{
    // Fetch angle.
    const F32 angle = mDegToRad(dAtof(argv[2]));

    // Speed.
    const F32 speed = mDegToRad(dAtof(argv[3]));

    if ( argc == 4 )
    {
        return object->rotateTo( angle, speed );
    }

    // Auto stop?
    const bool autoStop = dAtob(argv[4]);

    if ( argc == 5 )
    {
        return object->rotateTo( angle, speed, autoStop );
    }

    // Warp to target.
    const bool warpToTarget = dAtob(argv[5]);

    return object->rotateTo( angle, speed, autoStop, warpToTarget );

}

//-----------------------------------------------------------------------------

/*! Fades the object to the target color.
	The current color of the object will continue to change until it arrives at the target color or the fade is cancelled.
	The change will continue even if the blendColor is set directly.
	@param (red / green / blue / alpha) The target color to fade the object to.
	@param rate The rate per second to change each color value. Must be a number greater than zero.
	@return Whether the fade started or not.
*/
ConsoleMethodWithDocs(SceneObject, fadeTo, ConsoleBool, 4, 4, (targetColor red / green / blue / alpha, rate))
{
	if (argc < 3)
	{
		Con::warnf("SceneObject::fadeTo() - Invalid number of parameters!");
		return false;
	}

	const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
	if (colorCount != 4)
	{
		Con::warnf("SceneObject::fadeTo() - Invalid color! Colors require four values (red / green / blue / alpha)!");
		return false;
	}

	F32 rate = dAtof(argv[3]);
	if (rate <= 0.0f)
	{
		Con::warnf("SceneObject::fadeTo() - Rate must be greater than zero!");
		return false;
	}

	return object->fadeTo(ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)),
								 dAtof(Utility::mGetStringElement(argv[2], 1)),
								 dAtof(Utility::mGetStringElement(argv[2], 2)),
								 dAtof(Utility::mGetStringElement(argv[2], 3))), 
								 rate, rate, rate, rate);
}

//-----------------------------------------------------------------------------

/*! Fades the object to the target color over a period of time.
	The current color of the object will continue to change until it arrives at the target color or the fade is cancelled.
	The change will continue even if the blendColor is set directly which will change the amount of time it takes.
	Unhindered, each of the color values will arrive at the target in approximately the target time.
	@param (red / green / blue / alpha) The target color to fade the object to.
	@param time The amount of time in milliseconds that each color value will take to reach the target. Must be a number greater than zero.
	@return Whether the fade started or not.
*/
ConsoleMethodWithDocs(SceneObject, fadeToTime, ConsoleBool, 4, 4, (targetColor red / green / blue / alpha, time))
{
	if (argc < 3)
	{
		Con::warnf("SceneObject::fadeToTime() - Invalid number of parameters!");
		return false;
	}

	const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
	if (colorCount != 4)
	{
		Con::warnf("SceneObject::fadeToTime() - Invalid color! Colors require four values (red / green / blue / alpha)!");
		return false;
	}

	F32 time = dAtof(argv[3]);
	if (time <= 0.0f)
	{
		Con::warnf("SceneObject::fadeToTime() - Time must be greater than zero!");
		return false;
	}

	// Get the target color values.
	const F32 tRed = dAtof(Utility::mGetStringElement(argv[2], 0));
	const F32 tGreen = dAtof(Utility::mGetStringElement(argv[2], 1));
	const F32 tBlue = dAtof(Utility::mGetStringElement(argv[2], 2));
	const F32 tAlpha = dAtof(Utility::mGetStringElement(argv[2], 3));

	// Get the rate to change each value. The rate will be change per second.
	const ColorF currentColor = object->getBlendColor();
	F32 rRed = (1000.0f * fabs(tRed - currentColor.red)) / time;
	F32 rGreen = (1000.0f * fabs(tGreen - currentColor.green)) / time;
	F32 rBlue = (1000.0f * fabs(tBlue - currentColor.blue)) / time;
	F32 rAlpha = (1000.0f * fabs(tAlpha - currentColor.alpha)) / time;

	return object->fadeTo(ColorF(tRed, tGreen, tBlue, tAlpha), rRed, rGreen, rBlue, rAlpha);
}

//-----------------------------------------------------------------------------

/*! Grows or shrinks the object to the target size.
	The current size of the object will continue to change until it arrives at the target size or the grow is cancelled.
	The change will continue even if the size is set directly.
	@param (width / height) The target size to grow or shrink the object to.
	@param (rateX / rateY) The rate per second to change the size. Must be a number greater than zero even if shrinking. Can be one or two values.
	@return Whether the grow started or not.
*/
ConsoleMethodWithDocs(SceneObject, growTo, ConsoleBool, 4, 4, (targetSize width / height, rate rateX / rateY))
{
	if (argc < 3)
	{
		Con::warnf("SceneObject::growTo() - Invalid number of parameters!");
		return false;
	}

	const U32 targetCount = Utility::mGetStringElementCount(argv[2]);
	if (targetCount != 2)
	{
		Con::warnf("SceneObject::growTo() - Invalid size! Target size requires two values (width / height)!");
		return false;
	}

	Vector2 rate;
	const U32 rateCount = Utility::mGetStringElementCount(argv[3]);
	if (rateCount == 1)
	{
		rate.x = dAtof(Utility::mGetStringElement(argv[3], 0));
		rate.y = rate.x;
	}
	else if (rateCount == 2)
	{
		rate.x = dAtof(Utility::mGetStringElement(argv[3], 0));
		rate.y = dAtof(Utility::mGetStringElement(argv[3], 1));
	}
	else
	{
		Con::warnf("SceneObject::growTo() - Invalid size! Target size requires two values (width / height)!");
		return false;
	}

	if (rate.x <= 0.0f || rate.y <= 0.0f)
	{
		Con::warnf("SceneObject::growTo() - Rate must be greater than zero!");
		return false;
	}

	return object->growTo(Vector2(dAtof(Utility::mGetStringElement(argv[2], 0)),
		dAtof(Utility::mGetStringElement(argv[2], 1))),
		rate);
}

//-----------------------------------------------------------------------------

/*! Grows or shrinks the object to the target size over a period of time.
	The current size of the object will continue to change until it arrives at the target size or the grow is cancelled.
	The change will continue even if the size is set directly which will change the amount of time it takes.
	Unhindered, both size values will arrive at the target in approximately the target time.
	@param (width / height) The target size to grow or shrink the object to.
	@param time The amount of time in milliseconds that both size values will take to reach the target. Must be a number greater than zero.
	@return Whether the fade started or not.
*/
ConsoleMethodWithDocs(SceneObject, growToTime, ConsoleBool, 4, 4, (targetSize width / height, time))
{
	if (argc < 3)
	{
		Con::warnf("SceneObject::growToTime() - Invalid number of parameters!");
		return false;
	}

	const U32 targetCount = Utility::mGetStringElementCount(argv[2]);
	if (targetCount != 2)
	{
		Con::warnf("SceneObject::growToTime() - Invalid size! Target size requires two values (width / height)!");
		return false;
	}

	F32 time = dAtof(argv[3]);
	if (time <= 0.0f)
	{
		Con::warnf("SceneObject::growToTime() - Time must be greater than zero!");
		return false;
	}

	// Get the target size values.
	const F32 tWidth = dAtof(Utility::mGetStringElement(argv[2], 0));
	const F32 tHeight = dAtof(Utility::mGetStringElement(argv[2], 1));

	// Get the rate to change each value. The rate will be change per second.
	const Vector2 currentSize = object->getSize();
	F32 rWidth = (1000.0f * fabs(tWidth - currentSize.x)) / time;
	F32 rHeight = (1000.0f * fabs(tHeight - currentSize.y)) / time;

	return object->growTo(Vector2(tWidth, tHeight), Vector2(rWidth, rHeight));
}

//-----------------------------------------------------------------------------

/*! Stop a previous 'moveTo' command.
    @param autoStop? - Whether to automatically set the linear velocity to zero or not
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, cancelMoveTo, ConsoleVoid, 2, 3, ([autoStop = true]))
{
    if ( argc == 2 )
    {
        object->cancelMoveTo();
    }
    else
    {
        const bool autoStop = dAtob(argv[2]);
        object->cancelMoveTo( autoStop );
    }
}

//-----------------------------------------------------------------------------

/*! Stop a previous 'rotateTo' command.
    @param autoStop? - Whether to automatically set the angular velocity to zero or not
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, cancelRotateTo, ConsoleVoid, 2, 3, ([autoStop = true]))
{
    if ( argc == 2 )
    {
        object->cancelRotateTo();
    }
    else
    {
        const bool autoStop = dAtob(argv[2]);
        object->cancelRotateTo( autoStop );
    }
}

//-----------------------------------------------------------------------------

/*! Stop a previous 'fadeTo' command.
	@return No return value.
*/
ConsoleMethodWithDocs(SceneObject, cancelFadeTo, ConsoleVoid, 2, 2, ())
{
	object->cancelFadeTo();
}

//-----------------------------------------------------------------------------

/*! Stop a previous 'growTo' command.
	@return No return value.
*/
ConsoleMethodWithDocs(SceneObject, cancelGrowTo, ConsoleVoid, 2, 2, ())
{
	object->cancelGrowTo();
}

//-----------------------------------------------------------------------------

/*! Gets whether a previous 'moveTo' command has completed or not.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, isMoveToComplete, ConsoleBool, 2, 2, ())
{
    return object->isMoveToComplete();
}

//-----------------------------------------------------------------------------

/*! Gets whether a previous 'rotateTo' command has completed or not.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, isRotateToComplete, ConsoleBool, 2, 2, ())
{
    return object->isRotateToComplete();
}

//-----------------------------------------------------------------------------

/*! Gets whether a previous 'fadeTo' command has completed or not.
	@return No return value.
*/
ConsoleMethodWithDocs(SceneObject, isFadeToComplete, ConsoleBool, 2, 2, ())
{
	return object->isFadeToComplete();
}

//-----------------------------------------------------------------------------

/*! Gets whether a previous 'growTo' command has completed or not.
	@return No return value.
*/
ConsoleMethodWithDocs(SceneObject, isGrowToComplete, ConsoleBool, 2, 2, ())
{
	return object->isGrowToComplete();
}

//-----------------------------------------------------------------------------

/*! Applies a force at a world point.
    If the force is not applied at the center of mass, it will generate a torque and affect the angular velocity.
    @param worldForceX/Y - The world force vector in Newtons (N).
    @param worldPointX/Y - The world point where the force is applied.  If world point is not specified, the center of mass is used.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, applyForce, ConsoleVoid, 4, 6, (worldForce X/Y, [worldPoint X/Y]))
{
    // World force.
    const U32 worldForceElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 worldForce;

    S32 nextArg = 3;
    if ( worldForceElementCount == 1 && argc > 3 )
    {
        worldForce.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldForceElementCount == 2 )
    {
        worldForce = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::applyForce() - Invalid number of parameters!");
        return;
    }

    if ( argc <= nextArg )
    {
        // Apply force.
        object->applyForce( worldForce );
        return;
    }

    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldPoint;

    if ( worldPointElementCount == 1 && argc > (nextArg+1) )
    {
        worldPoint.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::applyForce() - Invalid number of parameters!");
        return;
    }

    // Apply force.
    object->applyForce( worldForce, worldPoint );
}

//-----------------------------------------------------------------------------

/*! Applies a torque.
    This affects the angular velocity without affecting the linear velocity of the center of mass.
    @param torque - The torque in Newton-metres (N-m).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, applyTorque, ConsoleVoid, 3, 3, (torque))
{
    // Fetch torque.
    const F32 torque = dAtof(argv[2]);

    // Apply torque.
    object->applyTorque( torque );
}

//-----------------------------------------------------------------------------

/*! Applies an impulse at a world point.
    This immediately modifies the linear velocity.  It also modifies the angular velocity if the point of application is not the center of mass.
    @param worldImpulse/Y - The world impulse vector in Newtons (N-seconds) or Kg-m/s.
    @param worldPointX/Y - The world point where the force is applied.  If world point is not specified, the center of mass is used.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, applyLinearImpulse, ConsoleVoid, 4, 6, (worldImpulse X/Y, [worldPoint X/Y]))
{
    // World impulse.
    const U32 worldImpulseElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 worldImpulse;

    S32 nextArg = 3;
    if ( worldImpulseElementCount == 1 && argc > 3 )
    {
        worldImpulse.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( worldImpulseElementCount == 2 )
    {
        worldImpulse = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::applyLinearImpulse() - Invalid number of parameters!");
        return;
    }

    if ( argc <= nextArg )
    {
        // Apply force.
        object->applyForce( worldImpulse );
        return;
    }

    // World point.
    const U32 worldPointElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 worldPoint;

    if ( worldPointElementCount == 1 && argc > (nextArg+1) )
    {
        worldPoint.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( worldPointElementCount == 2 )
    {
        worldPoint = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::applyLinearImpulse() - Invalid number of parameters!");
        return;
    }

    // Apply linear impulse.
    object->applyLinearImpulse( worldImpulse, worldPoint );
}

//-----------------------------------------------------------------------------

/*! Applies an angular impulse.
    @param impulse - The impulse in Kg*m*m/s.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, applyAngularImpulse, ConsoleVoid, 3, 3, (impulse))
{
    // Fetch impulse.
    const F32 impulse = dAtof(argv[2]);

    // Apply impulse.
    object->applyAngularImpulse( impulse );
}

//-----------------------------------------------------------------------------

/*! Sets the scaled world gravity.
    @param scale - The scaled world gravity.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setGravityScale, ConsoleVoid, 3, 3, (float scale))
{
    // Set gravity scale.
    object->setGravityScale( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the gravity scale.
    @return (float scale) The gravity scale.
*/
ConsoleMethodWithDocs(SceneObject, getGravityScale, ConsoleFloat,2, 2, ())
{
    // Get gravity scale.
    return object->getGravityScale();
}

//-----------------------------------------------------------------------------

/*! Deletes the specified collision shape.
    @param shapeIndex - The index of the collision shape.
    @return Whether the collision shape was successfully deleted or not.
*/
ConsoleMethodWithDocs( SceneObject, deleteCollisionShape, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::deleteCollisionShape() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Delete collision shape.
    object->deleteCollisionShape( shapeIndex );

    return true;
}

//-----------------------------------------------------------------------------

/*! Deletes all collision shapes.
    @return No return value.
*/
ConsoleMethodWithDocs( SceneObject, clearCollisionShapes, ConsoleVoid, 2, 2, ())
{
    object->clearCollisionShapes();
}

//-----------------------------------------------------------------------------

/*! Gets the number of collision shapes.
    @return (int count) The number of collision shapes.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionShapeCount, ConsoleInt, 2, 2, ())
{
    // Get collision shape count.
    return object->getCollisionShapeCount();
}

//-----------------------------------------------------------------------------

/*! Gets the type of collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (type) The type of collision shape at the specified index.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionShapeType, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeType() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    return SceneObject::getCollisionShapeTypeDescription( object->getCollisionShapeType( shapeIndex ) );
}

//-----------------------------------------------------------------------------

/*! Gets the area of collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (area) The area of collision shape at the specified index.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionShapeArea, ConsoleFloat, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeArea() - Invalid shape index of %d.", shapeIndex);
        return 0.0f;
    }

    // Calculate area if the shape type is circle
    if (object->getCollisionShapeType( shapeIndex ) == b2Shape::e_circle)
    {
        // Get the radius
        const F32 radius = object->getCircleCollisionShapeRadius( shapeIndex );

        // Return the area
        return b2_pi * radius * radius;
    }

    // Calculate area if the shape type is polygon
    if (object->getCollisionShapeType( shapeIndex ) == b2Shape::e_polygon)
    {
        const b2PolygonShape* pShape = object->getCollisionPolygonShape( shapeIndex );

        // Sanity
        if ( pShape->GetVertexCount() < 3 )
        {
            Con::warnf("SceneObject::getCollisionShapeArea() - Fewer than 3 vertices in polygon shape.");
            return 0.0f;
        }

        // Calculate the sum of cross-products
        F32 crossProductSum = 0.0f;

        // Because the cross-produce will have a different sign for triangles 
        // outside of the polygon this method will work for non-convex polygons
        for ( S32 i = 1; i < pShape->GetVertexCount() - 1; ++i )
        {
            b2Vec2 edge1 = pShape->GetVertex(i) - pShape->GetVertex(0);
            b2Vec2 edge2 = pShape->GetVertex(i+1) - pShape->GetVertex(0);
            crossProductSum += b2Cross(edge1, edge2);
        }

        crossProductSum *= 0.5f;

        // Return the area, which is the magnitude of the sum
        return fabs(crossProductSum);
    }

    // Return area of zero for any other shape type
    return 0.0f;
}

//-----------------------------------------------------------------------------

/*! Sets the density of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @param density - The collision shape density.
    @return No return value.
*/
ConsoleMethodWithDocs( SceneObject, setCollisionShapeDensity, ConsoleVoid, 4, 4, (int shapeIndex, float density))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeDensity() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch density.
    const F32 density = dAtof(argv[3]);
    if ( density < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeDensity() - Invalid density of %g.", density);
    }

    // Set density.
    object->setCollisionShapeDensity( shapeIndex, density );
}

//-----------------------------------------------------------------------------

/*! Gets the density of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return The density of the collision shape at the specified index (-1 if error).
*/
ConsoleMethodWithDocs( SceneObject, getCollisionShapeDensity, ConsoleFloat, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeDensity() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get density.
    return object->getCollisionShapeDensity( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Sets the friction of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @param friction - The collision shape friction.
    @return No return value.
*/
ConsoleMethodWithDocs( SceneObject, setCollisionShapeFriction, ConsoleVoid, 4, 4, (int shapeIndex, float friction))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeFriction() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch friction.
    const F32 friction = dAtof(argv[3]);
    if ( friction < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeFriction() - Invalid friction of %g.", friction);
    }

    // Set friction.
    object->setCollisionShapeFriction( shapeIndex, friction );
}

//-----------------------------------------------------------------------------

/*! Gets the friction of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return The friction of the collision shape at the specified index (-1 if error).
*/
ConsoleMethodWithDocs( SceneObject, getCollisionShapeFriction, ConsoleFloat, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeFriction() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get friction.
    return object->getCollisionShapeFriction( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Sets the restitution of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @param density - The collision shape restitution.
    @return No return value.
*/
ConsoleMethodWithDocs( SceneObject, setCollisionShapeRestitution, ConsoleVoid, 4, 4, (int shapeIndex, float restitution))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeRestitution() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Fetch restitution.
    const F32 restitution = dAtof(argv[3]);
    if ( restitution < 0.0f )
    {
        Con::warnf("SceneObject::setCollisionShapeRestitution() - Invalid restitution of %g.", restitution);
    }

    // Set restitution.
    object->setCollisionShapeRestitution( shapeIndex, restitution );
}

//-----------------------------------------------------------------------------

/*! Gets the restitution of the collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return The restitution of the collision shape at the specified index (-1 if error).
*/
ConsoleMethodWithDocs( SceneObject, getCollisionShapeRestitution, ConsoleFloat, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeRestitution() - Invalid shape index of %d.", shapeIndex);
        return -1.0f;
    }

    // Get restitution.
    return object->getCollisionShapeRestitution( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Sets whether the collision shape at the specified index is a sensor or not.
    @param shapeIndex - The index of the collision shape.
    @param status - Whether the collision shape at the specified index is a sensor or not.
    @return No return value.
*/
ConsoleMethodWithDocs( SceneObject, setCollisionShapeIsSensor, ConsoleVoid, 4, 4, (int shapeIndex, bool status ))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::setCollisionShapeIsSensor() - Invalid shape index of %d.", shapeIndex);
        return;
    }

    // Set is sensor.
    object->setCollisionShapeIsSensor( shapeIndex, dAtob(argv[3]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether the collision shape at the specified index is a sensor or not.
    @param shapeIndex - The index of the collision shape.
    @return Whether the collision shape at the specified index is a sensor or not.
*/
ConsoleMethodWithDocs( SceneObject, getCollisionShapeIsSensor, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Sanity!
    if (  shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCollisionShapeIsSensor() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Get is sensor.
    return object->getCollisionShapeIsSensor( shapeIndex );
}

//-----------------------------------------------------------------------------
/*! Creates a circle collision shape.
    @param radius The radius of the circle.
    @return (int shapeIndex) The index of the collision shape or (-1) if not created.
*/
ConsoleMethodWithDocs( SceneObject, createCircleCollisionShape, ConsoleInt, 3, 5, (radius, [localPositionX, localPositionY]))
{
    const F32 radius = dAtof(argv[2]);

    if ( argc == 3 )
    {
        return object->createCircleCollisionShape( radius );
    }

    b2Vec2 localPosition;

    // Fetch local position element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[3]);

    if ( elementCount == 2 && argc == 4 )
    {
        localPosition = Utility::mGetStringElementVector(argv[3]);
    }
    else if ( elementCount == 1 && argc == 5 )
    {
        localPosition.Set( dAtof(argv[3]), dAtof(argv[4]) );
    }
    else
    {
        Con::warnf("SceneObject::createCircleCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createCircleCollisionShape( radius, localPosition );
}

//-----------------------------------------------------------------------------

/*! Gets the radius of a circle collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (F32 radius) The radius of a circle collision shape at the specified index or (0) if an invalid shape.
*/
ConsoleMethodWithDocs( SceneObject, getCircleCollisionShapeRadius, ConsoleFloat, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeRadius() - Invalid shape index of %d.", shapeIndex);
        return 0.0f;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_circle )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeRadius() - Not a circle shape at index of %d.", shapeIndex);
        return 0.0f;
    }

    return object->getCircleCollisionShapeRadius( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the local position of a circle collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (localPositionXY) The local position of a circle collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getCircleCollisionShapeLocalPosition, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeLocalPosition() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_circle )
    {
        Con::warnf("SceneObject::getCircleCollisionShapeLocalPosition() - Not a circle shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getCircleCollisionShapeLocalPosition( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Creates a convex polygon collision shape.
    @param localPointsX/Y A space separated list of local points for the convex polygon collision shape (must be at least three points).
    @return (int shapeIndex) The index of the collision shape or (-1) if not created.
*/
ConsoleMethodWithDocs( SceneObject, createPolygonCollisionShape, ConsoleInt, 3, 3, (localPointsX/Y))
{
    const U32 pointElements = Utility::mGetStringElementCount(argv[2]);

    // Check even number of elements and at least 6 (3 points) exist.
    if ( ( pointElements % 2 ) != 0 || pointElements < 6 || pointElements > (2*b2_maxPolygonVertices) )
    {
        Con::warnf("SceneObject::createPolygonCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 localPoints[b2_maxPolygonVertices];
    for ( U32 elementIndex = 0, pointIndex = 0; elementIndex < pointElements; elementIndex += 2, pointIndex++ )
    {
        localPoints[pointIndex] = Utility::mGetStringElementVector( argv[2], elementIndex );
    }

    const U32 pointCount = pointElements / 2;
    return object->createPolygonCollisionShape( pointCount, localPoints );
}

//-----------------------------------------------------------------------------

/*! Creates a polygon box collision shape.
    @param width The width of the box.
    @param height The height of the box.
    @param localCentroidX/Y The local position of the box centroid.
    @param angle The angle of the box.
    @return (int shapeIndex) The index of the collision shape or (-1) if not created.
*/
ConsoleMethodWithDocs( SceneObject, createPolygonBoxCollisionShape, ConsoleInt, 2, 7, (width, height, [localCentroidX, localCentroidY], [angle]))
{
    // Were any dimensions specified?
    if( argc == 2 )
    {
        // No, so fetch the objects size.
        const Vector2& size = object->getSize();

        // Create a box surrounding the object.
        return object->createPolygonBoxCollisionShape( size.x, size.y );
    }

    // Width and height.
    const U32 widthHeightElementCount = Utility::mGetStringElementCount(argv[2]);

    F32 width;
    F32 height;

    S32 nextArg = 3;
    if ( widthHeightElementCount == 1 && argc > 3 )
    {
        width = dAtof(argv[2]);
        height = dAtof(argv[3]);
        nextArg = 4;
    }
    else if ( widthHeightElementCount == 2 )
    {
        Vector2 vector = Utility::mGetStringElementVector(argv[2]);
        width = vector.x;
        height = vector.y;
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createPolygonBoxCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createPolygonBoxCollisionShape( width, height );
    }

    // Local centroid.
    const U32 localCentroidElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localCentroid;
    if ( localCentroidElementCount == 1 && argc > (nextArg+1) )
    {
        localCentroid.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( localCentroidElementCount == 2 )
    {
        localCentroid = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createPolygonBoxCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createPolygonBoxCollisionShape( width, height, localCentroid );
    }

    // Angle.
    const F32 angle = mDegToRad(dAtof(argv[nextArg++]));
    
    return object->createPolygonBoxCollisionShape( width, height, localCentroid, angle );
}

//-----------------------------------------------------------------------------

/*! Gets the point count of a polygon collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (int pointCount) The point count of a polygon collision shape at the specified index or (0) if an invalid shape.
*/
ConsoleMethodWithDocs( SceneObject, getPolygonCollisionShapePointCount, ConsoleInt, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapePointCount() - Invalid shape index of %d.", shapeIndex);
        return 0;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_polygon )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapePointCount() - Not a polygon shape at index of %d.", shapeIndex);
        return 0;
    }

    return object->getPolygonCollisionShapePointCount( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the local point of a polygon collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @param pointIndex - The index of the local point.
    @return (localPointXY) The local point of a polygon collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getPolygonCollisionShapeLocalPoint, ConsoleString, 4, 4, (int shapeIndex, int pointIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch point index.
    const U32 pointIndex = dAtoi(argv[3]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_polygon )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Not a polygon shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Fetch point count.
    const U32 pointCount = object->getPolygonCollisionShapePointCount( shapeIndex );

    // Sanity!
    if ( pointIndex >= pointCount )
    {
        Con::warnf("SceneObject::getPolygonCollisionShapeLocalPoint() - Invalid point index of %d (only %d available) on shape index of %d.", pointIndex, pointCount, shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getPolygonCollisionShapeLocalPoint( shapeIndex, pointIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Creates a chain collision shape.
    @param localPointsX/Y A space separated list of local points for the chain collision shape (must be at least two points).
    @param adjacentLocalPositionStartXY The adjacent local position of the start of the edge.
    @param adjacentLocalPositionEndXY The adjacent local position of the end of the edge.
    @return (int shapeIndex) The index of the collision shape or (-1) if not created.
*/
ConsoleMethodWithDocs( SceneObject, createChainCollisionShape, ConsoleInt, 3, 7, (localPointsX/Y, [adjacentLocalPositionStartX, adjacentLocalPositionStartY], [adjacentLocalPositionEndX, adjacentLocalPositionEndY]))
{
    const U32 pointElements = Utility::mGetStringElementCount(argv[2]);

    // Check even number of elements and at least 4 (2 points) exist.
    if ( ( pointElements % 2 ) != 0 || pointElements < 4 )
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 localPoints[b2_maxPolygonVertices];
    for ( U32 elementIndex = 0, pointIndex = 0; elementIndex < pointElements; elementIndex += 2, pointIndex++ )
    {
        localPoints[pointIndex] = Utility::mGetStringElementVector( argv[2], elementIndex );
    }

    const U32 pointCount = pointElements / 2;

    if ( argc == 3 )
    {
        return object->createChainCollisionShape( pointCount, localPoints );
    }

    S32 nextArg = 3;

    // Adjacent local position start.
    const U32 adjacentLocalPositionStartElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 adjacentLocalPositionStart;
    if ( adjacentLocalPositionStartElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionStart.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionStartElementCount == 2 )
    {
        adjacentLocalPositionStart = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 adjacentLocalPositionEnd( 0.0f, 0.0f );

    if ( argc <= nextArg )
    {
        return object->createChainCollisionShape( pointCount, localPoints, true, false, adjacentLocalPositionStart, adjacentLocalPositionEnd );
    }

    // Adjacent local position end.
    const U32 adjacentLocalPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    if ( adjacentLocalPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionEndElementCount == 2 )
    {
        adjacentLocalPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createChainCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createChainCollisionShape( pointCount, localPoints, true, true, adjacentLocalPositionStart, adjacentLocalPositionEnd );
}

//-----------------------------------------------------------------------------

/*! Gets the point count of a chain collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (int pointCount) The point count of a chain collision shape at the specified index or (0) if an invalid shape.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapePointCount, ConsoleInt, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapePointCount() - Invalid shape index of %d.", shapeIndex);
        return 0;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapePointCount() - Not a chain shape at index of %d.", shapeIndex);
        return 0;
    }

    return object->getChainCollisionShapePointCount( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the local point of a polygon collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @param pointIndex - The index of the local point.
    @return (localPointXY) The local point of a polygon collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapeLocalPoint, ConsoleString, 4, 4, (int shapeIndex, int pointIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch point index.
    const U32 pointIndex = dAtoi(argv[3]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeLocalPoint() - Invalid shape index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeLocalPoint() - Not a chain shape at index of %d.", shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    // Fetch point count.
    const U32 pointCount = object->getChainCollisionShapePointCount( shapeIndex );

    // Sanity!
    if ( pointIndex >= pointCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapePointCount() - Invalid point index of %d (only %d available) on shape index of %d.", pointIndex, pointCount, shapeIndex);
        return Vector2::getZero().scriptThis();
    }

    return object->getChainCollisionShapeLocalPoint( shapeIndex, pointIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets whether the chain collision shape at the specified index has an adjacent start point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentStart) Whether the chain collision shape at the specified index has an adjacent start point or not.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapeHasAdjacentStart, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentStart() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeHasAdjacentStart( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets whether the chain collision shape at the specified index has an adjacent end point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentEnd) Whether the chain collision shape at the specified index has an adjacent end point or not.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapeHasAdjacentEnd, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeHasAdjacentEnd() - Not a chain shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getChainCollisionShapeHasAdjacentEnd( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the adjacent start point of the chain collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (adjacentStartPointXY) The adjacent start point of the chain collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapeAdjacentStart, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentStart() - Not a chain shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getChainCollisionShapeAdjacentStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets whether the chain collision shape at the specified index has an adjacent end point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentEnd) Whether the chain collision shape at the specified index has an adjacent end point or not.
*/
ConsoleMethodWithDocs( SceneObject, getChainCollisionShapeAdjacentEnd, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_chain )
    {
        Con::warnf("SceneObject::getChainCollisionShapeAdjacentEnd() - Not a chain shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getChainCollisionShapeAdjacentEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Creates an edge collision shape.
    @param localPositionStartXY The local position of the start of the edge.
    @param localPositionEndXY The local position of the end of the edge.
    @param adjacentLocalPositionStartXY The adjacent local position of the start of the edge.
    @param adjacentLocalPositionEndXY The adjacent local position of the end of the edge.
    @return (int shapeIndex) The index of the collision shape or (-1) if not created.
*/
ConsoleMethodWithDocs(SceneObject, createEdgeCollisionShape, ConsoleInt, 4, 10, (localPositionStartX, localPositionStartY, localPositionEndX, localPositionEndY, [adjacentLocalPositionStartX, adjacentLocalPositionStartY], [adjacentLocalPositionEndX, adjacentLocalPositionEndY]))
{
    // Local position start.
    const U32 localPositionStartElementCount = Utility::mGetStringElementCount(argv[2]);

    b2Vec2 localPositionStart;
    S32 nextArg = 3;
    if ( localPositionStartElementCount == 1 && argc > 3 )
    {
        localPositionStart.Set( dAtof(argv[2]), dAtof(argv[3]) );
        nextArg = 4;
    }
    else if ( localPositionStartElementCount == 2 )
    {
        localPositionStart = Utility::mGetStringElementVector(argv[2]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    // Local position end.
    const U32 localPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 localPositionEnd;
    if ( localPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        localPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( localPositionEndElementCount == 2 )
    {
        localPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    if ( argc <= nextArg )
    {
        return object->createEdgeCollisionShape( localPositionStart, localPositionEnd );
    }

    // Adjacent local position start.
    const U32 adjacentLocalPositionStartElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    b2Vec2 adjacentLocalPositionStart;
    if ( adjacentLocalPositionStartElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionStart.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionStartElementCount == 2 )
    {
        adjacentLocalPositionStart = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    b2Vec2 adjacentLocalPositionEnd(0.0f, 0.0f);

    if ( argc <= nextArg )
    {
        return object->createEdgeCollisionShape( localPositionStart, localPositionEnd, true, false, adjacentLocalPositionStart, adjacentLocalPositionEnd );
    }

    // Adjacent local position end.
    const U32 adjacentLocalPositionEndElementCount = Utility::mGetStringElementCount(argv[nextArg]);

    if ( adjacentLocalPositionEndElementCount == 1 && argc > (nextArg+1) )
    {
        adjacentLocalPositionEnd.Set( dAtof(argv[nextArg]), dAtof(argv[nextArg+1]) );
        nextArg += 2;
    }
    else if ( adjacentLocalPositionEndElementCount == 2 )
    {
        adjacentLocalPositionEnd = Utility::mGetStringElementVector(argv[nextArg++]);
    }
    // Invalid
    else
    {
        Con::warnf("SceneObject::createEdgeCollisionShape() - Invalid number of parameters!");
        return -1;
    }

    return object->createEdgeCollisionShape( localPositionStart, localPositionEnd, true, true, adjacentLocalPositionStart, adjacentLocalPositionEnd );
}

//-----------------------------------------------------------------------------

/*! Gets the local position start of the edge collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (localPositionStartXY) The local position start of the edge collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeLocalPositionStart, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionStart() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionStart() - Not an edge shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getEdgeCollisionShapeLocalPositionStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the local position end of the edge collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (localPositionEndXY) The local position end of the edge collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeLocalPositionEnd, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionEnd() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeLocalPositionEnd() - Not an edge shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getEdgeCollisionShapeLocalPositionEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets whether the chain collision shape at the specified index has an adjacent start point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentStart) Whether the chain collision shape at the specified index has an adjacent start point or not.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeHasAdjacentStart, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentStart() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeHasAdjacentStart( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets whether the edge collision shape at the specified index has an adjacent end point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentEnd) Whether the edge collision shape at the specified index has an adjacent end point or not.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeHasAdjacentEnd, ConsoleBool, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return false;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeHasAdjacentEnd() - Not an edge shape at index of %d.", shapeIndex);
        return false;
    }

    return object->getEdgeCollisionShapeHasAdjacentEnd( shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Gets the adjacent start point of the edge collision shape at the specified index.
    @param shapeIndex - The index of the collision shape.
    @return (adjacentStartPointXY) The adjacent start point of the edge collision shape at the specified index.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeAdjacentStart, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentStart() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentStart() - Not an edge shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getEdgeCollisionShapeAdjacentStart( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets whether the edge collision shape at the specified index has an adjacent end point or not.
    @param shapeIndex - The index of the collision shape.
    @return (bool adjacentEnd) Whether the edge collision shape at the specified index has an adjacent end point or not.
*/
ConsoleMethodWithDocs( SceneObject, getEdgeCollisionShapeAdjacentEnd, ConsoleString, 3, 3, (int shapeIndex))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentEnd() - Invalid shape index of %d.", shapeIndex);
        return NULL;
    }

    // Sanity!
    if ( object->getCollisionShapeType( shapeIndex ) != b2Shape::e_edge )
    {
        Con::warnf("SceneObject::getEdgeCollisionShapeAdjacentEnd() - Not an edge shape at index of %d.", shapeIndex);
        return NULL;
    }

    return object->getEdgeCollisionShapeAdjacentEnd( shapeIndex ).scriptThis();
}

//-----------------------------------------------------------------------------

/*! Copies all collision shapes to the target object.
    @param targetObject - The target object to receive the collision shapes.
    @param clearTargetShapes - Whether to clear the current collision shapes on the target or not.  Optional: Defaults to true.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, copyAllCollisionShapes, ConsoleVoid, 3, 4, (targetObject [, clearTargetShapes?] ))
{
    // Fetch target object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[2] );

    // Sanity!
    if ( pSceneObject == NULL )
    {
        Con::warnf( "SceneObject::copyAllCollisionShapes() - Invalid target object." );
        return;
    }

    // Fetch clear target shapes flag.
    const bool clearTargetShapes = argc >= 4 ? dAtob( argv[3] ) : true;
    
    // Copy collision shapes.
    object->copyCollisionShapes( pSceneObject, clearTargetShapes );
}

//-----------------------------------------------------------------------------

/*! Copies a collision shape at the specified index to the target object.
    @param shapeIndex - The index of the collision shape.
    @param targetObject - The target object to receive the collision shape copy.
    @return The shape index of the copied collision shape on the target object or (-1) if not copied.
*/
ConsoleMethodWithDocs(SceneObject, copyCollisionShape, ConsoleInt, 4, 4, (int shapeIndex, targetObject))
{
    // Fetch shape index.
    const U32 shapeIndex = dAtoi(argv[2]);

    // Fetch shape count.
    const U32 shapeCount = object->getCollisionShapeCount();

    // Sanity!
    if ( shapeIndex >= shapeCount )
    {
        Con::warnf( "SceneObject::copyCollisionShape() - Invalid shape index of %d.", shapeIndex );
        return INVALID_COLLISION_SHAPE_INDEX;
    }

    // Fetch target object.
    SceneObject* pSceneObject = Sim::findObject<SceneObject>( argv[3] );

    // Sanity!
    if ( pSceneObject == NULL )
    {
        Con::warnf( "SceneObject::copyCollisionShape() - Invalid target object." );
        return INVALID_COLLISION_SHAPE_INDEX;
    }
    
    // Copy collision shape.
    return object->copyCollisionShapes( pSceneObject, false, shapeIndex );
}

//-----------------------------------------------------------------------------

/*! Show or hide the object.
    @param status Whether to enable or disable visibility on the object.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setVisible, ConsoleVoid, 3, 3, (bool status))
{
    // Set Visible.
    object->setVisible( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the object's visible status.
    @return (bool status) Whether or not the object is visible.
*/
ConsoleMethodWithDocs(SceneObject, getVisible, ConsoleBool, 2, 2, ())
{
    // Get Visible Status.
    return object->getVisible();
} 

//-----------------------------------------------------------------------------

/*! Sets whether blending is on or not.
    @blendMode Whether blending is on or not.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setBlendMode, ConsoleVoid, 3, 3, (bool blendMode))
{
    // Fetch blend mode.
    const bool blendMode = dAtob(argv[2]);

    object->setBlendMode( blendMode );
}

//-----------------------------------------------------------------------------

/*! Gets whether blending is on or not.
    @return (bool blendMode) Whether blending is on or not.
*/
ConsoleMethodWithDocs(SceneObject, getBlendMode, ConsoleBool, 2, 2, ())
{
   return object->getBlendMode();
}

//-----------------------------------------------------------------------------

/*! Sets the source blend factor.
    @param srcBlend The source blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSrcBlendFactor, ConsoleVoid, 3, 3, (srcBlend))
{
    // Fetch source blend factor.
    GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the source blend factor.
    @return (srcBlend) The source blend factor.
*/
ConsoleMethodWithDocs(SceneObject, getSrcBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getSrcBlendFactorDescription(object->getSrcBlendFactor());
}

//-----------------------------------------------------------------------------

/*! Sets the destination blend factor.
    @param dstBlend The destination blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setDstBlendFactor, ConsoleVoid, 3, 3, (dstBlend))
{
    // Fetch destination blend factor.
    GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the destination blend factor.
    @return (dstBlend) The destination blend factor.
*/
ConsoleMethodWithDocs(SceneObject, getDstBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getDstBlendFactorDescription(object->getDstBlendFactor());
}

//-----------------------------------------------------------------------------

/*! or ( stockColorName ) - Sets the blend color.
    @param red The red value.
    @param green The green value.
    @param blue The blue value.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setBlendColor, ConsoleVoid, 3, 6, (float red, float green, float blue, [float alpha = 1.0]))
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Space separated.
    if (argc == 3 )
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            // Set color.
            Con::setData( TypeColorF, &const_cast<ColorF&>(object->getBlendColor()), 0, 1, &(argv[2]) );
            return;
        }

        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
                alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
            return;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
        return;
    }

    // Set blend color.
    object->setBlendColor(ColorF(red, green, blue, alpha));
}

//-----------------------------------------------------------------------------

/*! Gets the Rendering Blend color.
    @param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.
    @return (float red / float green / float blue / float alpha) The sprite blend color.
*/
ConsoleMethodWithDocs(SceneObject, getBlendColor, ConsoleString, 2, 3, (allowColorNames))
{
    // Get Blend color.
    ColorF blendColor = object->getBlendColor();

    // Fetch allow color names flag.
    const bool allowColorNames = (argc > 2) ? dAtob(argv[2] ) : false;

    // Are color names allowed?
    if ( allowColorNames )
    {
        // Yes, so fetch the field value.
        return Con::getData( TypeColorF, &blendColor, 0 );
    }

    // No, so fetch the raw color values.
    return blendColor.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the Rendering Alpha (transparency).
    The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setBlendAlpha, ConsoleVoid, 3, 3, (float alpha))
{
    // Set Blend Alpha.
    object->setBlendAlpha( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the Rendering Alpha (transparency).
    @return (float alpha) The alpha value, a range from 0.0 to 1.0.  Less than zero if alpha testing is disabled.
*/
ConsoleMethodWithDocs(SceneObject, getBlendAlpha, ConsoleFloat, 2, 2, ())
{
    // Get Blend Alpha.
    return object->getBlendAlpha();
}

//-----------------------------------------------------------------------------

/*! Set the render alpha test threshold.
    @param alpha The alpha test threshold in the range of 0.0 to 1.0.  Less than zero to disable alpha testing.
    @return No return value.

*/
ConsoleMethodWithDocs(SceneObject, setAlphaTest, ConsoleVoid, 3, 3, (float alpha))
{
    object->setAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------


/*! Gets the render alpha test threshold.
	@return The render alpha test threshold in the range of 0.0f to 1.0.  Less than zero represents disabled alpha testing.
*/
ConsoleMethodWithDocs(SceneObject, getAlphaTest, ConsoleFloat, 2, 2, ())
{
	return object->getAlphaTest();
}

//-----------------------------------------------------------------------------

/*! Sets the layer draw order sorting point.
    @param x The x position local to the object of the sort point.
    @param y The y position local to the object of the sort point.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSortPoint, ConsoleVoid, 3, 4, (float x, float y))
{
   // The new sort point.
   Vector2 sortPt;

   // Elements in the first argument.
   U32 elementCount = Utility::mGetStringElementCount(argv[2]);

   // ("x y")
   if ((elementCount == 2) && (argc == 3))
      sortPt = Utility::mGetStringElementVector(argv[2]);

   // (x, y)
   else if ((elementCount == 1) && (argc == 4))
      sortPt = Vector2(dAtof(argv[2]), dAtof(argv[3]));

   // Invalid
   else
   {
      Con::warnf("SceneObject::setSortPoint() - Invalid number of parameters!");
      return;
   }

   // Set Position.
   object->setSortPoint(sortPt);
} 

//-----------------------------------------------------------------------------

/*! Gets the layer draw order sorting point.
    @return (float x/float y) The local x and y position of the sort point.
*/
ConsoleMethodWithDocs(SceneObject, getSortPoint, ConsoleString, 2, 2, ())
{
    // Get sort point.
    return object->getSortPoint().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the x-component of the layer draw order sorting point.
    @param x The x position local to the object of the sort point.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSortPointX, ConsoleVoid, 3, 3, (float x))
{
    // Set sort point X-component.
    object->setSortPoint( Vector2( dAtof(argv[2]), object->getSortPoint().y ) );
}

//-----------------------------------------------------------------------------

/*! Gets the x component of the layer draw order sorting point.
    @return (float x) The local x position of the sort point.
*/
ConsoleMethodWithDocs(SceneObject, getSortPointX, ConsoleFloat, 2, 2, ())
{
    // Get sort point X-component.
    return object->getSortPoint().x;
}

//-----------------------------------------------------------------------------

/*! Sets the y-component of the layer draw order sorting point.
    @param y The y position local to the object of the sort point.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setSortPointY, ConsoleVoid, 3, 3, (float y))
{
    // Set sort point Y-component.
    object->setSortPoint( Vector2( object->getSortPoint().x, dAtof(argv[2]) ) );
}

//-----------------------------------------------------------------------------

/*! Gets the y component of the layer draw order sorting point.
    @return (float y) The local y position of the sort point.
*/
ConsoleMethodWithDocs(SceneObject, getSortPointY, ConsoleFloat, 2, 2, ())
{
    // Get sort point Y-component.
    return object->getSortPoint().y;
}

//-----------------------------------------------------------------------------

/*! Sets the name of the render group used to sort the object during rendering.
    @param renderGroup The name of the render group to use.  Defaults to nothing.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setRenderGroup, ConsoleVoid, 3, 3, (renderGroup))
{
    object->setRenderGroup( argv[2] );
} 

//-----------------------------------------------------------------------------

/*! Gets the name of the render group used to sort the object during rendering.
    @return The render group used to sort the object during rendering.
*/
ConsoleMethodWithDocs(SceneObject, getRenderGroup, ConsoleString, 2, 2, ())
{
    return object->getRenderGroup();
} 

//-----------------------------------------------------------------------------

/*! Sets whether input events are passed from the scene window are monitored by this object or not.
    @param inputStatus Whether input events are passed from the scene window are monitored by this object or not.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setUseInputEvents, ConsoleVoid, 3, 3, (bool inputStatus))
{
    object->setUseInputEvents( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether input events are passed from the scene window are monitored by this object or not.
    @return (bool inputStatus) Whether input events are passed from the scene window are monitored by this object or not.
*/
ConsoleMethodWithDocs(SceneObject, getUseInputEvents, ConsoleBool, 2, 2, ())
{
    return object->getUseInputEvents();
} 

//-----------------------------------------------------------------------------

/*! Sets whether the 'onUpdate' callback is called or not.
    @param status Whether the 'onUpdate' callback is called or not (default is false).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setUpdateCallback, ConsoleVoid, 2, 3, ([bool status?]))
{
   object->setUpdateCallback( argc > 2 ? dAtob(argv[2]) : true);
}

//-----------------------------------------------------------------------------

/*! Gets whether the 'onUpdate' callback is called or not.
    @return (bool status ) Whether the 'onUpdate' callback is called or not.
*/
ConsoleMethodWithDocs(SceneObject, getUpdateCallback, ConsoleBool, 2, 2, ())
{
   return object->getUpdateCallback();
}

//-----------------------------------------------------------------------------

/*! Sets whether the 'onWake' and 'onSleep' callbacks are called or not when the object changes sleep state.
    The object must be able to sleep (setSleepingAllowed) and have a 'dynamic' body mode for this callback to occur.
    @param status Whether the 'onWake' and 'onSleep' callbacks are called or not (default is false).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setSleepingCallback, ConsoleVoid, 2, 3, ([bool status?]))
{
   object->setSleepingCallback( argc > 2 ? dAtob(argv[2]) : true);
}

//-----------------------------------------------------------------------------

/*! Gets whether the 'onWake' and 'onSleep' callbacks are called or not when the object changes sleep state.
    @return (bool status ) Whether the 'onWake' and 'onSleep' callbacks are called or not.
*/
ConsoleMethodWithDocs(SceneObject, getSleepingCallback, ConsoleBool, 2, 2, ())
{
   return object->getSleepingCallback();
}

//-----------------------------------------------------------------------------

/*! Sets whether the 'Scene::onCollision' callback is called or not.
    Sets whether whether the 'Scene::onCollision' callback is called or not.
    @param status Whether the 'Scene::onCollision' callback is called or not (default is false).
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setCollisionCallback, ConsoleVoid, 2, 3, ([bool status?]))
{
    // Set collision callback.
    object->setCollisionCallback( argc > 2 ? dAtob(argv[2]) : true );
}

//-----------------------------------------------------------------------------

/*! Gets whether the 'Scene::onCollision' callback is called or not.
    @return (bool status) the 'Scene::onCollision' callback is called or not.
*/
ConsoleMethodWithDocs(SceneObject, getCollisionCallback, ConsoleBool, 2, 2, ())
{
    // Get collision callback.
    return object->getCollisionCallback();
}

//-----------------------------------------------------------------------------

/*! Sets Debug option(s) on.
    @param debugOptions Either a list of debug modes (comma-separated), or a string with the modes (space-separated)
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setDebugOn, ConsoleVoid, 3, 2 + DEBUG_MODE_COUNT, (debugOptions))
{
    // Reset the mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        Con::warnf( "SceneObject::setDebugOn() - Invalid number of parameters!" );
        return;
    }

    // Space-separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = Utility::mGetStringElement( argv[2], i );
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( pDebugOption );
        
            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }
         
            // Merge into mask.
            mask |= debugOption;
        }
    }
    // Comma-separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = argv[i];
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( argv[i] );

            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOn() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }

            // Merge into mask.
            mask |= debugOption;
        }
    }

    // Set debug mask.
    object->setDebugOn(mask);
} 

//-----------------------------------------------------------------------------

/*! Sets Debug options(s) off.
    @param debugOptions Either a list of debug modes to turn off (comma-separated) or a string (space-separated)
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, setDebugOff, ConsoleVoid, 3, 2 + DEBUG_MODE_COUNT, (debugOptions))
{
    // Reset the mask.
    U32 mask = 0;

    // Grab the element count of the first parameter.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // Make sure we get at least one number.
    if (elementCount < 1)
    {
        Con::warnf( "SceneObject::setDebugOff() - Invalid number of parameters!" );
        return;
    }

    // Space-separated list.
    if (argc == 3)
    {
        // Convert the string to a mask.
        for (U32 i = 0; i < elementCount; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = Utility::mGetStringElement( argv[2], i );
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( pDebugOption );
        
            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }
         
            // Merge into mask.
            mask |= debugOption;
        }
    }
    // Comma-separated list.
    else
    {
        // Convert the list to a mask.
        for (U32 i = 2; i < (U32)argc; i++)
        {
            // Fetch the debug option.
            const char* pDebugOption = argv[i];
            Scene::DebugOption debugOption = Scene::getDebugOptionEnum( argv[i] );

            // Is the option valid?
            if ( debugOption == Scene::SCENE_DEBUG_INVALID )
            {
                // No, so warn.
                Con::warnf( "SceneObject::setDebugOff() - Invalid debug option '%s' specified.", pDebugOption );
                continue;
            }

            // Merge into mask.
            mask |= debugOption;
        }
    }

    // Set debug mask.
    object->setDebugOff(mask);
} 

//-----------------------------------------------------------------------------

/*! Attach a GUI Control to the object.
@param guiObject The GuiControl to attach.
@param window The SceneWindow to bind the GuiControl to.
@param sizeControl Whether or not to size the GuiControl to the size of this object.
@param offset The position offset to apply to the GuiControl.
@return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, attachGui, ConsoleVoid, 4, 7, (guiControl guiObject, SceneWindow window, [sizeControl ? = false], [offset ? = Vector2(0, 0)]))
{
    // Find GuiControl Object.
    GuiControl* pGuiControl = dynamic_cast<GuiControl*>(Sim::findObject(argv[2]));

    // Check for invalid Gui Control.
    if (!pGuiControl)
    {
        Con::warnf("SceneObject::attachGui() - Couldn't find GuiControl %s!", argv[2]);
        return;
    }

    // Find SceneWindow Object.
    SceneWindow* pSceneWindow = dynamic_cast<SceneWindow*>(Sim::findObject(argv[3]));

    // Check for invalid SceneWindow Object.
    if (!pSceneWindow)
    {
        Con::warnf("SceneObject::attachGui() - Couldn't find SceneWindow %s!", argv[3]);
        return;
    }

    // Auto-size control?.
    const bool sizeControl = argc >= 5 ? dAtob(argv[4]) : false;

    // GuiControl position offset.
    Vector2 offset(0, 0);

    // Elements in the first argument.
    U32 elementCount = Utility::mGetStringElementCount(argv[5]);

    // ("x y")
    if ((elementCount == 2) && (argc == 6))
        offset = Utility::mGetStringElementVector(argv[5]);

    // (x, y)
    else if ((elementCount == 1) && (argc == 7))
        offset = Vector2(dAtof(argv[5]), dAtof(argv[6]));

    // Attach GUI Control.
    object->attachGui(pGuiControl, pSceneWindow, sizeControl, offset);
}

//-----------------------------------------------------------------------------

/*! Detach any GUI Control.
@param ctrl The SimObjectId of the GuiControl to detach.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, detachGui, ConsoleVoid, 2, 3, ())
{
    if (argc >= 3)
    {
        GuiControl* pGuiControl = dynamic_cast<GuiControl*>(Sim::findObject(argv[3]));

        if (!pGuiControl)
        {
            // Detach the last attached control.
            object->detachGui();
        }
        else
        {
            // Detach the attached control.
            object->detachGui(pGuiControl);
        }
    }
    else
    {
        // Detach the last attached control.
        object->detachGui();
    }
}

//-----------------------------------------------------------------------------

/*! Detach all GUI Controls.
    @return No return value.
*/
ConsoleMethodWithDocs(SceneObject, detachAllGuiControls, ConsoleVoid, 2, 2, ())
{
    object->detachAllGuiControls();
}

//-----------------------------------------------------------------------------

/*! Copies one scene object from another scene object.
    The object being copied to needs to be of the same class as the object being copied from.
    @param object The SceneObject to copy this object to.
    @param copyDynamicFields Whether the dynamic fields should be copied or not.  Optional: Defaults to false.
    @return (bool) Whether or not the copy was successful.
*/
ConsoleMethodWithDocs(SceneObject, copyFrom, ConsoleBool, 3, 4, (SceneObject object, [copyDynamicFields? = false]))
{
    // Find scene object.
    SceneObject* pSceneObject = dynamic_cast<SceneObject*>( Sim::findObject(argv[2]) );

    // Did we find the object?
    if ( !pSceneObject )
    {
        // No, so warn.
        Con::warnf("SceneObject::copyFrom() - Could not find object '%s'.", argv[2] );
        return false;
    }

    // Fetch copy dynamic fields flag.
    const bool copyDynamicFields = ( argc >= 4 ) ? dAtob( argv[3] ) : false;

    // Copy object.
    object->copyFrom( pSceneObject, copyDynamicFields );

    return true;
}

//-----------------------------------------------------------------------------

/*! Sets whether picking is allowed or not.
    @param pickingAllowed Whether picking is allowed or not.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, setPickingAllowed, ConsoleVoid, 3, 3, (bool pickingAllowed))
{
    // Fetch flag.
    const bool pickingAllowed = dAtob(argv[2]);

    object->setPickingAllowed( pickingAllowed );
}

//-----------------------------------------------------------------------------

/*! Gets whether picking is allowed or not.
    @return Whether picking is allowed or not.
*/
ConsoleMethodWithDocs(SceneObject, getPickingAllowed, ConsoleBool, 2, 2, ())
{
    return object->getPickingAllowed();
}

//-----------------------------------------------------------------------------

/*! Safely deletes object.
    @return No return Value.
*/
ConsoleMethodWithDocs(SceneObject, safeDelete, ConsoleVoid, 2, 2, ())
{
    // Script Delete.
    object->safeDelete();
}

/*! Plays a sound at the object's location
@param AudioAsset - The Audio Asset to play
@param AudioDescription - Sets the audio Description. If omitted,  (optional)
@return returns the audio handle
*/
ConsoleMethodWithDocs(SceneObject, playSound, ConsoleInt, 3, 4, ())
{
    // Fetch asset Id.    
    const char* pAssetId = argv[2];

    // Acquire audio asset.
    AudioAsset* pAudioAsset = AssetDatabase.acquireAsset<AudioAsset>(pAssetId);

    // Did we get the audio asset?
    if (pAudioAsset == NULL)
    {
        // No, so warn.
        Con::warnf("alxPlay() - Could not find audio asset '%s'.", pAssetId);
        return NULL_AUDIOHANDLE;
    }
    AudioDescription* AD;

    if (argc > 3)
    {
        AD = static_cast<AudioDescription*>(Sim::findObject(argv[3]));
    }
    else
    {
        AD = new AudioDescription();
    }

    Vector2 pos = object->getPosition();
    Vector2 vel = object->getLinearVelocity();
    Point3F realpos;
    Point3F velocity;

    realpos.x = pos.x;
    realpos.y = pos.y;
    realpos.z = 0.f;

    MatrixF transform(false);
    transform.setColumn(3, realpos);

    //If the source is not looping and outside of listener range, it is not created, handle = 0
    AUDIOHANDLE handle = alxCreateSource_AD(pAudioAsset, AD, &transform);
    if (!handle) return (0);
    //alxplay returns the same handle as alxCreateSource
    alxPlay(handle);
    alxSource3f(handle, AL_POSITION, realpos.x, realpos.y, 0.f);

    // Release asset.
    AssetDatabase.releaseAsset(pAssetId);
    object->addAudioHandle(handle);
    return handle;
}

/*! Stops a sound attached to a SceneObject
@param Index - The Index of the sound (obtained via getSoundatIndex)
*/
ConsoleMethodWithDocs(SceneObject, stopSound, ConsoleVoid, 3, 3, (S32 index))
{
    const S32 index = dAtoi(argv[2]);
    
    if (!object->getSoundsCount())
    {
        Con::warnf("No sounds on this object. Can't stop. WON't stop.");
        return;
    }    
    
    U32 handle = object->getSound(index);


    if (!handle)
    {
        Con::warnf("Unable to find Sound Handle at index %i", index);
        return;
    }

    alxStop(handle);
    object->refreshsources();
}

/*! gets the amount of sounds currently attached to a SceneObject
@return returns the Amount of sounds currently attached to the SceneObject
*/
ConsoleMethodWithDocs(SceneObject, getSoundsCount, ConsoleInt, 2, 2, ())
{
    return object->getSoundsCount();
}

/*! Gets the handle of a sound at specified index
@param Index - The Index of the sound (obtained via getSoundatIndex)
@return returns the handle of a sound
*/
ConsoleMethodWithDocs(SceneObject, getSoundatIndex, ConsoleInt, 3, 3, ())
{
    U32 size = object->getSoundsCount();
    if (!size)
    {
        Con::printf("No sounds on this object.");
        return 0;
    }

    U32 handle = object->getSound(dAtoi(argv[2]));
    if (!handle)
    {
        Con::printf("Not a valid handle at index %i", dAtoi(argv[2]));
        return 0;
    }

    return handle;
}
ConsoleMethodGroupEndWithDocs(SceneObject)
