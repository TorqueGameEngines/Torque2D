#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

class RayList
{
public:

   F32 x;
   F32 y;
   F32 l;
   F32 ang;
   bool operator == (const RayList& t) const { return ((mFabs(t.x - x) < 0.01) && (mFabs(t.y - y) < 0.01)); }

};

class RaysCastCallback : public b2RayCastCallback
{
public:

   RaysCastCallback() : m_fixture(NULL) {}

   float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
      m_fixture = fixture;
      m_point = point;
      m_normal = normal;
      m_fraction = fraction;
      return fraction;
   }

   b2Fixture* m_fixture;
   b2Vec2 m_point;
   b2Vec2 m_normal;
   float32 m_fraction;

};

class ShadowMap : public SceneObject

{
   typedef SceneObject Parent;

protected:

   F32                     mLightRadius;
   U32                     mLightSegments;

public:

   ShadowMap();
   ~ShadowMap();

   static void initPersistFields();

   

   virtual bool onAdd();
   virtual void onRemove();

   virtual void safeDelete(void);
   virtual void sceneRender(const SceneRenderState* sceneRenderState, const SceneRenderRequest* sceneRenderRequest, BatchRender* batchRender);
   //virtual bool validRender(void) const {}
   virtual bool shouldRender(void) const { return true; }

   /// Light segments.
   inline void setLightSegments(const U32 lightSegments) { mLightSegments = lightSegments; };
   inline U32 getLightSegments(void) const { return mLightSegments; }

   /// Light Radius.
   inline void setLightRadius(const F32 lightRadius) { mLightRadius = lightRadius; }
   inline F32 getLightRadius(void) const { return mLightRadius; }

   DECLARE_CONOBJECT(ShadowMap);


protected:

   virtual void OnRegisterScene(Scene* mScene);
   virtual void OnUnregisterScene(Scene* mScene);

protected:

   static bool setLightRadius(void* obj, const char* data) { static_cast<ShadowMap*>(obj)->setLightRadius(dAtof(data)); return false; }
   static bool writeLightRadius(void* obj, StringTableEntry pFieldName) { return static_cast<ShadowMap*>(obj)->getLightRadius() > 0.0f; }

   static bool setLightSegments(void* obj, const char* data) { static_cast<ShadowMap*>(obj)->setLightSegments(dAtoi(data)); return false; }
   static bool writeLightSegments(void* obj, StringTableEntry pFieldName) { return static_cast<ShadowMap*>(obj)->getLightSegments() > 0; }

};

#endif //_SHADOWMAP_H_

S32 QSORT_CALLBACK sortRays(const void * a, const void * b);
