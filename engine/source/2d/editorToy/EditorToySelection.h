#ifndef _EDITORTOYSELECTION_H_
#define _EDITORTOYSELECTION_H_

#ifndef _SIMSET_H_
#include "sim/simSet.h"
#endif // !_SIMSET_H_

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

class SceneObject;

class EditorToySelection : public SimSet
{

public:
   typedef SimSet Parent;

private:

   Point2F mCenter;
   Point2F mBoxCenter;

   RectF mBoxBounds;

   bool mCenterValid;
   bool mAutoSelect;

   Point2F mPrevCenter;

   void updateCenter();

public:

   DECLARE_CONOBJECT(EditorToySelection);

   EditorToySelection();
   ~EditorToySelection();

   static void initPersistFields();

   void addObject(SimObject * obj);

   void removeObject(SimObject * obj);

   void offset(const Point2F & offset, F32 gridSnap);

   F32 _snapFloat(const F32 & val, const F32 & snap) const;

   bool objInSet(SimObject* obj);
   void storeCurCenter() { mPrevCenter = getCenter(); }
   bool hasCenterChanged() { return (mPrevCenter != getCenter()); }
   void invalidCenter() { mCenterValid = false; }

   const Point2F& getCenter();
   const Point2F& getBoxCenter();
   const RectF& getBoxBounds();

   void rotate(const F32 & ang, const Point2F & center);

   void rotate(const F32 & ang);

   void orient(const F32 & ang);

   void scale(const Vector2 & scale);

   void scale(const Vector2 & scale, const Point2F & center);
   
};



#endif // !_EDITORTOYSELECTION_H_
