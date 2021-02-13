#include "2d/editorToy/EditorToySelection.h"
#include "2d/editorToy/EditorToySceneWindow.h"

#ifndef _MATHUTILS_H_
#include "math/mathUtils.h"
#endif

//-----------------------------------------------------------------------------

EditorToySelection::EditorToySelection()
   :  mCenterValid(false),
      mAutoSelect(false),
      mPrevCenter(0.0f, 0.0f)
{
}

//-----------------------------------------------------------------------------

EditorToySelection::~EditorToySelection()
{
}

//-----------------------------------------------------------------------------

void EditorToySelection::initPersistFields()
{
   Parent::initPersistFields();
}

void EditorToySelection::addObject(SimObject* obj)
{
   // If object is in set, or set locked return.
   if (objInSet(obj))
      return;

   if (isLocked())
      return;

   // dont add us
   if (obj == this)
      return;

   // if obj is itself a selection set dont add it.

   EditorToySelection* sel = dynamic_cast<EditorToySelection*>(obj);
   if (sel && !sel->objInSet(this))
      return;

   for (SimGroup* group = getGroup(); group != NULL; group = group->getGroup())
      if (obj == group)
         return;

   invalidCenter();

   Parent::addObject(obj);

   return;

}

void EditorToySelection::removeObject(SimObject* obj)
{
   if (!objInSet(obj))
      return;

   if (isLocked())
      return;

   invalidCenter();

   Parent::removeObject(obj);

   return;
}

//-----------------------------------------------------------------------------

bool EditorToySelection::objInSet(SimObject * obj)
{
   // Lock the sim and scan through for the object.
   lock();

   bool res = false;
   for (iterator iter = begin(); iter != end(); ++iter)
   {
      if (obj == *iter)
      {
         res = true;
         break;
      }

      EditorToySelection* set = dynamic_cast< EditorToySelection* >( *iter );
      if (set && set->objInSet(obj))
      {
         res = true;
         break;
      }
   }

   unlock();

   return res;
}

//-----------------------------------------------------------------------------

void EditorToySelection::updateCenter()
{
   if (mCenterValid)
      return;

   mCenterValid = true;

   mCenter.set(0.0f, 0.0f);
   mBoxCenter = mCenter;
   // Find the center based on positions of all objects in set.
   for (SimSet::iterator iter = begin(); iter != end(); ++iter)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
      if (!obj)
         continue;
      Vector2 mPos = obj->getRenderPosition();
      mCenter += mPos;

      // Worked out by taking the right bottom and minusing
      // the left top
      const b2Vec2* oobb = obj->getRenderOOBB();
      Vector2 strt = oobb[0];
      Vector2 end(oobb[3].x - strt.x, oobb[3].y - strt.y);
      RectF bound(strt, end);

      mBoxBounds.point.setMin(bound.point);
      mBoxBounds.extent.setMax(bound.extent);
   }

   mCenter /= (F32)size();
   mBoxCenter = mBoxBounds.centre();
}

//-----------------------------------------------------------------------------

void EditorToySelection::offset(const Point2F& offset, F32 gridSnap)
{
   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
      if (!obj)
         continue;

      Point2F mPos = obj->getRenderPosition();

      mPos += offset;

      if (gridSnap != 0.0f)
      {
         mPos.x = _snapFloat(mPos.x, gridSnap);
         mPos.y = _snapFloat(mPos.y, gridSnap);
      }

      obj->setPosition(mPos);
   }

}

F32 EditorToySelection::_snapFloat(const F32 &val, const F32 &snap) const
{
   // SNAPPPPP!!!!
   if (snap == 0.0f)
      return val;

   F32 a = mFmod(val, snap);
   F32 temp = val;

   if (mFabs(a) > (snap / 2))
      val < 0.0f ? temp -= snap : temp += snap;

   return(temp - a);
}

//-----------------------------------------------------------------------------

const Point2F & EditorToySelection::getCenter()
{
   // Refind center of selection.
   updateCenter();
   return(mCenter);
}

const Point2F & EditorToySelection::getBoxCenter()
{
   // Refind center of selection.
   updateCenter();
   return(mBoxCenter);
}

const RectF & EditorToySelection::getBoxBounds()
{
   // Refind center of selection.
   updateCenter();
   return(mBoxBounds);
}


//-----------------------------------------------------------------------------

void EditorToySelection::rotate(const F32 & ang, const Point2F & center)
{
   if (size() == 1)
   {
      // Rotate single object
      SceneObject* obj = dynamic_cast<SceneObject*>(at(0));
      if (obj)
      {
         obj->setAngle(ang);
      }
   }
   else
   {
      for (iterator iter = begin(); iter != end(); ++iter)
      {
         // Rotate objects selected around center point
         SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
         if (!obj)
            continue;

         Point2F mPos = obj->getRenderPosition();
         Point2F point = center;

         F32 s = mSin(ang);
         F32 c = mCos(ang);

         F32 x = mPos.x * c - mPos.y * s;
         F32 y = mPos.x * s + mPos.y * c;

         mPos.x = x + center.x;
         mPos.y = y + center.y;

         obj->setPosition(mPos);
         obj->setAngle(ang);

      }
   }

   mCenterValid = false;
}

//-----------------------------------------------------------------------------

void EditorToySelection::rotate(const F32 & ang)
{
   // simple rotate because some people dont say orient.
   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
      if (!obj)
         continue;

      obj->setAngle(ang);
   }

   mCenterValid = false;
}

//-----------------------------------------------------------------------------

void EditorToySelection::orient(const F32 & ang)
{
   // Orient all objects selected to the given angle.
   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
      if (!obj)
         continue;

      obj->setAngle(ang);
   }

   mCenterValid = false;
}

void EditorToySelection::scale(const Vector2 & scale)
{
   for (iterator iter = begin(); iter != end(); ++iter)
   {
      SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
      if (!obj)
         continue;
      Vector2 cur = obj->getSize();
      cur.scale(scale);
      cur.clampMin(Vector2(0.01f, 0.01f));
      cur.clampMax(Vector2(1000.0f, 1000.0f));
      obj->setSize(cur);
   }

   mCenterValid = false;
}

void EditorToySelection::scale(const Vector2 & scale, const Point2F & center)
{
   if (size() == 1)
   {
      // single object selected, simple scale.
      SceneObject* obj = dynamic_cast<SceneObject*>(at(0));
      if (obj)
      {
         Vector2 cur = obj->getSize();
         cur.scale(scale);
         cur.clampMin(Vector2(0.01f, 0.01f));
         cur.clampMax(Vector2(1000.0f, 1000.0f));

         obj->setSize(cur);
      }
   }
   else
   {
      for (iterator iter = begin(); iter != end(); ++iter)
      {
         // scale and offset position.
         SceneObject* obj = dynamic_cast<SceneObject*>(*iter);
         if (!obj)
            continue;
         Vector2 cur = obj->getSize();
         Vector2 prev = obj->getSize();
         cur.scale(scale);
         cur.clampMin(Vector2(0.01f, 0.01f));
         cur.clampMax(Vector2(1000.0f, 1000.0f));

         obj->setSize(cur);
         if (!obj->getSize().isEqual(cur))
            continue;

         Vector2 adj = cur.div(prev);

         Point2F mPos = obj->getRenderPosition();
         Point2F off = mPos - center;
         off *= adj;

         obj->setPosition(off + center);

      }
   }
   // We are scaling from the center so shouldnt have to refind it.
}

//-----------------------------------------------------------------------------

