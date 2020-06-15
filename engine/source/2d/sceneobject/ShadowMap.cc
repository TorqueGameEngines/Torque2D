#include "graphics/dgl.h"
#include "console/consoleTypes.h"
#include "2d/core/Utility.h"
#include "ShadowMap.h"

// Script bindings.
#include "ShadowMap_ScriptBinding.h"

IMPLEMENT_CONOBJECT(ShadowMap);

ShadowMap::ShadowMap()
{
   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;
}

ShadowMap::~ShadowMap()
{

}

bool ShadowMap::onAdd()
{
   // Call Parent.
   if (!Parent::onAdd())
      return false;

   // Return Okay.
   return true;
}

//----------------------------------------------------------------------------

void ShadowMap::onRemove()
{
   // Call Parent.
   Parent::onRemove();
}

void ShadowMap::initPersistFields()
{
   Parent::initPersistFields();
}

void ShadowMap::safeDelete(void)
{
   Parent::safeDelete();
}

void ShadowMap::sceneRender(const SceneRenderState * sceneRenderState, const SceneRenderRequest * sceneRenderRequest, BatchRender * batchRender)
{
   Vector2 worldPos = getPosition();
   Vector<Vector2> verts;
   Vector<RayList> bList;


   F32 radius = getSize().x * 0.5f;
   Scene* scene = getScene();
   b2World* mWorld = scene->getWorld();

   glEnable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glPushMatrix();
   glTranslatef(worldPos.x, worldPos.y, 0);
   glPolygonMode(GL_FRONT, GL_FILL);

   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   // Creates the fading dark region.
   glBegin(GL_TRIANGLE_FAN);
   glColor4f(1, 1, 1, 0.5);
   glVertex2f(0, 0);
   //check scene objects
   U32 objCount = scene->getSceneObjectCount();
   for (U32 i = 0; i < objCount; i++)
   {
      SceneObject *tObj = scene->getSceneObject(i);
      Vector2 dist = worldPos - tObj->getPosition();
      const F32 distSqr = dist.LengthSquared();
      const F32 radSqr = radius * radius;
      //within radius?
      if (distSqr < radSqr || distSqr == radSqr)
      {
         U32 shapeCount = tObj->getCollisionShapeCount();
         for (U32 j = 0; j < shapeCount; j++)
         {
            //All vertices from collision shape
            if (tObj->getCollisionShapeType(j) == b2Shape::e_polygon)
            {
               U32 pCount = tObj->getPolygonCollisionShapePointCount(j);

               for (U32 k = 0; k < pCount; k++)
               {
                  Vector2 locPoint = tObj->getPolygonCollisionShapeLocalPoint(j, k);
                  Vector2 wPoint = tObj->getWorldPoint(locPoint);
                  verts.push_back(wPoint);
               }
            }
         }
      }
   }

   //cast ray to vertices
   for (S32 l = 0; l < verts.size(); l++)
   {
      F32 rayLength = radius;
      Vector2 p1 = worldPos;
      Vector2 p2 = verts[l];
      F32 baseAng = mAtan(p2.x, p2.y);
      F32 cAng = 0;
      for (int m = 0; m < 3; m++)
      {
         if (m == 0)cAng = baseAng - 0.0001f;
         if (m == 1)cAng = baseAng;
         if (m == 2)cAng = baseAng + 0.0001f;

         p2.x = rayLength * mCos(cAng);
         p2.y = rayLength * mSin(cAng);

         RaysCastCallback callback;
         mWorld->RayCast(&callback, p1, p2);
         if (callback.m_fixture)
         {

            F32 ang = mAtan(callback.m_point.x - p1.x, callback.m_point.y - p1.y);
            Vector2 intersection = p1 + callback.m_fraction * (p2 - p1);
            //Point3F intersectionPoint(intersection.x, intersection.y, callback.m_fraction);
            RayList intersectionPoint;
            intersectionPoint.ang = ang;
            intersectionPoint.x = intersection.x;
            intersectionPoint.y = intersection.y;
            intersectionPoint.l = callback.m_fraction;

            bList.push_back(intersectionPoint);

         }
      }
   }
   //create a list of arrays for ordinary circle
   //these need to stop at a collision.
   for (U32 n = 0; n <= 36; n++)
   {
      F32 rayLength = radius;
      b2Vec2 p1 = worldPos;
      b2Vec2 p2 = p1 + rayLength * b2Vec2(mCos(mDegToRad((F32)10 * n)), mSin(mDegToRad((F32)10*n)));
      RaysCastCallback callback;
      mWorld->RayCast(&callback, p1, p2);
      if (callback.m_fixture)
      {
         F32 ang = mAtan(callback.m_point.x - p1.x, callback.m_point.y - p1.y);
         Vector2 intersection = p1 + callback.m_fraction * (p2 - p1);
         //Point3F intersectionPoint(intersection.x, intersection.y, callback.m_fraction);
         RayList intersectionPoint;
         intersectionPoint.ang = ang;
         intersectionPoint.x = intersection.x;
         intersectionPoint.y = intersection.y;
         intersectionPoint.l = callback.m_fraction;

         bList.push_back(intersectionPoint);
      }
      else
      {
         F32 ang = mAtan(p2.x - p1.x, p2.y - p1.y);

         RayList intersectionPoint;
         intersectionPoint.ang = ang;
         intersectionPoint.x = p2.x;
         intersectionPoint.y = p2.y;
         intersectionPoint.l = 1.0;

         bList.push_back(intersectionPoint);
      }
   }
   //sort the list
   if (bList.size() > 1)
   {
      dQsort(bList.address(), bList.size(), sizeof(RayList), sortRays);
   }
   //triangle fan
   for (S32 m = 0; m < bList.size(); m++)
   {
      glColor4f(1.0 - bList[m].l , 1.0 - bList[m].l , 1.0 - bList[m].l , 0.5);
      glVertex2f(bList[m].x, bList[m].y);

   }
   //close off the circle
   glColor4f(1.0 - bList[0].l, 1.0 - bList[0].l, 1.0 - bList[0].l, 0.5);
   glVertex2f(bList[0].x, bList[0].y);

   glEnd();


   /*U32 objCount = scene->getSceneObjectCount();
   for (U32 i = 0; i < objCount; i++)
   {
      SceneObject *tObj = scene->getSceneObject(i);
      Vector2 distancePos = worldPos - tObj->getPosition();

      const F32 distanceSqr = distancePos.LengthSquared();
      const F32 radiusSqr = radius * radius;
      if (distanceSqr < radiusSqr || distanceSqr == radiusSqr)
      {
         processObject(tObj);
      }
   }*/

   glPopMatrix();

}

void ShadowMap::OnRegisterScene(Scene* mScene)
{
   Parent::OnRegisterScene(mScene);
   mScene->getWorldQuery()->addAlwaysInScope(this);

}

void ShadowMap::OnUnregisterScene(Scene* mScene)
{
   mScene->getWorldQuery()->removeAlwaysInScope(this);
   Parent::OnUnregisterScene(mScene);
}

void ShadowMap::processObject(SceneObject *obj)
{
   Vector2 pos = getPosition();
   Vector<Vector2> verts;
   U32 shapeCount = obj->getCollisionShapeCount();
   for (U32 i = 0; i < shapeCount; i++)
   {
      if (obj->getCollisionShapeType(i) == b2Shape::e_polygon)
      {
         U32 pointCount = obj->getPolygonCollisionShapePointCount(i);
         for (U32 n = 0; n < pointCount; n++)
         {
            Vector2 locPoint = obj->getPolygonCollisionShapeLocalPoint(i, n);
            Vector2 wPoint = obj->getWorldPoint(locPoint);

            verts.push_back(wPoint);
         }

         renderShadow(verts, pos);
      }
   }
}

void ShadowMap::renderShadow(const Vector<Vector2>& verts, const Vector2& lightPos)
{

}

S32 QSORT_CALLBACK sortRays(const void* a, const void* b)
{
   RayList* ang_a = (RayList*) a;
   RayList* ang_b = (RayList*) b;

   if (ang_a->ang < ang_b->ang)
   {
      return -1;
   }
   else if (ang_a->ang > ang_b->ang)
   {
      return 1;
   }
  
   return 0;

}