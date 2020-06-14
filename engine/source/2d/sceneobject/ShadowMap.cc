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
   F32 radius = getSize().x * 0.5f;
   glDisable(GL_TEXTURE_2D);
   glPushMatrix();
   glTranslatef(worldPos.x, worldPos.y, 0);
   glPolygonMode(GL_FRONT, GL_FILL);

   glBlendFunc(GL_DST_COLOR, GL_ZERO);
   // Creates the fading dark region.
   glBegin(GL_TRIANGLE_FAN);
   glColor4f(1, 1, 1, 1);
   glVertex2f(0, 0);
   glColor4f(0, 0, 0, 1);
   glEnd();

   glBegin(GL_TRIANGLE_STRIP);
   glColor4f(0, 0, 0, 1);
   glVertex2f(10, 10);
   glEnd();
   Scene* scene = getScene();

   U32 objCount = scene->getSceneObjectCount();
   for (U32 i = 0; i < objCount; i++)
   {
      SceneObject *tObj = scene->getSceneObject(i);
      Vector2 distancePos = worldPos - tObj->getPosition();

      const F32 distanceSqr = distancePos.LengthSquared();
      const F32 radiusSqr = radius * radius;
      if (distanceSqr < radiusSqr || distanceSqr == radiusSqr)
      {
         Con::printf("Found object to shadow");
         processObject(tObj);
      }
   }

   glPopMatrix();

}

void ShadowMap::OnRegisterScene(Scene* mScene)
{
   Parent::OnRegisterScene(mScene);
   Con::printf("we are atleast here");
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
         Con::printf("We have a polygonshape on this sceneobject");
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
   Con::printf("renderShadow");
   Con::printf("glBegin in rendershadow");
   S32 startVert = 0;
   S32 endVert = 0;
   for(S32 i = 1; i < verts.size() + 1; i++)
   {
      S32 vert0 = (i - 1) % verts.size();
      S32 vert1 = (i) % verts.size();
      S32 vert2 = (i + 1) % verts.size();
      Vector2 normal0(verts[vert0] - verts[vert1]);
      normal0.perp();
      Vector2 normal1(verts[vert1] - verts[vert2]);
      normal1.perp();
      Vector2 ray(verts[vert1] - lightPos);
      F32 n0 = ray.dot(normal0);
      F32 n1 = ray.dot(normal1);
      if (n0 <= 0 && n1 > 0)
      {
         startVert = vert1;
      }
      if (n0 > 0 && n1 <= 0)
      {
         endVert = vert1;
      }
   }
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   //glDisable(GL_TEXTURE_2D);
   glColor4f(0, 0, 0, 1);
   glBegin(GL_TRIANGLE_STRIP);
   for (S32 i = 0; i < verts.size(); i++)
   {
      S32 vert = (startVert + i) % verts.size();
      Vector2 ray(verts[vert] - lightPos); ray.Normalize(600.0f);
      glVertex2f(verts[vert].x, verts[vert].y);
      glVertex2f(verts[vert].x + ray.x, verts[vert].y + ray.y);
      if (vert == endVert) break;
   }
   //glVertex2f(verts[0].x, verts[0].y);
   //glVertex2f(verts[1].x, verts[1].y);
   //glVertex2f(lightPos.x, lightPos.y);
   //glVertex2f(verts[3].x, verts[3].y);
   glEnd();
}
