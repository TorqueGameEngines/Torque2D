#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

struct RayList
{
   F32 ang;
   F32 x;
   F32 y;
   F32 l;
};

class RaysCastCallback : public b2RayCastCallback
{
public:
   RaysCastCallback() : m_fixture(NULL) {
   }

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
   F32            mLightRadius;

public:
   ShadowMap();
   ~ShadowMap();

   static void initPersistFields();

   inline void setLightRadius(const F32 lightRadius) { mLightRadius = lightRadius; }
   inline F32 getLightRadius(void) const { return mLightRadius; }

   virtual bool onAdd();
   virtual void onRemove();

   virtual void safeDelete(void);
   virtual void sceneRender(const SceneRenderState* sceneRenderState, const SceneRenderRequest* sceneRenderRequest, BatchRender* batchRender);
   //virtual bool validRender(void) const {}
   virtual bool shouldRender(void) const { return true; }

   void processObject(SceneObject *obj);
   void renderShadow(const Vector<Vector2>& verts, const Vector2& lightPos);

   DECLARE_CONOBJECT(ShadowMap);


   protected:
      virtual void OnRegisterScene(Scene* mScene);
      virtual void OnUnregisterScene(Scene* mScene);

private:
   F32 theSinTable[361];
   F32 theCosTable[361];
};


#endif //_SHADOWMAP_H_

S32 QSORT_CALLBACK sortRays(const void * a, const void * b);
