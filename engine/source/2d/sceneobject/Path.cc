#include "graphics/dgl.h"
#include "2d/sceneobject/Path.h"

#include "2d/sceneobject/Path_ScriptBinding.h"

IMPLEMENT_CONOBJECT(Path);

PathObject::PathObject():
   mObj(NULL),
   mPath(NULL)
{
   mCurrNode = 0;
   mPrevNode = 0;
   mLoop = true;
   mOrient = false;
   mSnapToNode = false;
   mLoopCount = 0;
   mMaxLoop = -1;
   mMaxSpeed = 1.0f;
   mAngOff = 0.0f;
}

void PathObject::setCurrNode(S32 node)
{
}

void PathObject::setNextNode(S32 node)
{
}

void PathObject::setPrevNode(S32 node)
{
}


Path::Path()
{

   // Use a static body by default.
   mBodyDefinition.type = b2_staticBody;

   VECTOR_SET_ASSOCIATION(mObjs);
   VECTOR_SET_ASSOCIATION(mNodes);
}

Path::~Path()
{

}

void Path::initPersistFields()
{
   Parent::initPersistFields();
}

void Path::preIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats * pDebugStats)
{
   Parent::preIntegrate(totalTime, elapsedTime, pDebugStats);

   Vector<PathObject*>::iterator i;
   for (i = mObjs.begin(); i != mObjs.end(); i++)
   {
      bool stop = false;
      PathObject &pObj = *(*i);

      Vector2 cPos = pObj.mObj->getPosition();
      Node &cNode = mNodes[pObj.mCurrNode];
      Vector2 cDst = mNodes[pObj.mCurrNode].position;
      F32 distance = (cDst - cPos).Length();

      if (distance < (pObj.mMaxSpeed * elapsedTime) || distance <= cNode.distance)
      {
         S32 nCount = mNodes.size();
         S32 end = nCount - 1;
         if (pObj.mCurrNode == end)
         {
            if (pObj.mLoop)
            {
               pObj.mLoopCount++;
               if ((pObj.mMaxLoop > 0) && (pObj.mLoopCount >= pObj.mMaxLoop))
               {
                  Con::printf("why we stopping?");
                  stop = true;
               }
               else
               {
                  Con::printf("back to 0");
                  pObj.mPrevNode = pObj.mCurrNode;
                  pObj.mCurrNode = 0;
                  pObj.mNextNode = pObj.mCurrNode;
               }
            }
            else
            {
               stop = true;
            }
         }
         else
         {
            pObj.mPrevNode = pObj.mCurrNode;
            pObj.mCurrNode = pObj.mCurrNode + 1;
            pObj.mNextNode = pObj.mCurrNode;
         }

         if (pObj.mCurrNode >= nCount)
         {
            pObj.mCurrNode = 0;
            pObj.mNextNode = pObj.mCurrNode;
         }
         else if (pObj.mCurrNode < 0)
         {
            pObj.mCurrNode = 0;
            pObj.mNextNode = pObj.mCurrNode;
         }

      }

      if (!stop)
      {
         moveObject(pObj);
      }

      else
      {
         pObj.mObj->setLinearVelocity(Vector2(0.0f, 0.0f));
      }

   }

}

void Path::integrateObject(const F32 totalTime, const F32 elapsedTime, DebugStats * pDebugStats)
{
   Parent::integrateObject(totalTime, elapsedTime, pDebugStats);
}

S32 Path::addNode(Vector2 pos, F32 distance, F32 weight)
{
   S32 nodeCount = mNodes.size();

   mNodes.push_back(Node(pos, distance, weight));

   return nodeCount;
}

void Path::attachObject(SceneObject * object, F32 speed, bool orientToPath, F32 angleOff, bool snapToNode, S32 startNode, bool loop, S32 maxLoop)
{
   if (snapToNode)
   {
      if ((startNode >= 0) && (startNode < mNodes.size()))
         object->setPosition(mNodes[startNode].position);
   }

   object->setLinearVelocity(Vector2::getZero());

   deleteNotify(object);

   PathObject *pObj  = new PathObject();
   pObj->mPath       = this;
   pObj->mObj        = object;
   pObj->mObjId      = object->getId();
   pObj->mOrient     = orientToPath;
   pObj->mAngOff     = angleOff;
   pObj->mLoop       = loop;
   pObj->mMaxLoop    = maxLoop;
   pObj->mMaxSpeed   = speed;
   pObj->mCurrNode   = startNode;
   pObj->mNextNode   = startNode;
   

   mObjs.push_back(pObj);
}

void Path::detachObject(SceneObject * object)
{
   if (!object)
      return;

   Vector<PathObject*>::iterator i;

   for (i = mObjs.begin(); i != mObjs.end(); i++)
   {
      PathObject *pObj = (*i);
      if(object == pObj->mObj)
      {
         if (!pObj->mObj.isNull())
         {
            pObj->mObj->setLinearVelocity(Vector2(0, 0));
            clearNotify(pObj->mObj);
         }

         delete pObj;

         mObjs.erase_fast(i);
         
         break;
      }
   }
}

void Path::moveObject(PathObject& obj)
{
   Vector2 cDest = mNodes[obj.mNextNode].position;
   Vector2 oPos = obj.mObj->getPosition();
   Vector2 dir = cDest - oPos;
   dir.Normalize();

   obj.mObj->setLinearVelocity(dir * obj.mMaxSpeed);

   if (obj.mOrient)
   {
      F32 rot = mRadToDeg(mAtan(dir.x, dir.y));
      rot = rot - obj.mAngOff;
      F32 ang = mDegToRad(rot);
      F32 speed = obj.mMaxSpeed;
      obj.mObj->rotateTo(ang, speed);
   }

}

void Path::onDeleteNotify(SimObject* object)
{
   Vector<PathObject*>::iterator i;

   SimObjectId objId = object->getId();

   for (i = mObjs.begin(); i != mObjs.end(); i++)
   {
      if ((*i)->mObjId == objId)
      {
         delete (*i);

         mObjs.erase_fast(i);

         break;
      }
   }
}
