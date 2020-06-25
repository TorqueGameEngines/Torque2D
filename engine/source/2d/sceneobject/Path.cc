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

   mMaxSpeed = 1.0f;
   mMaxForce = 3.0f;
   mOrient = false;
   mAngOff = 0.0f;
   mSnapToNode = false;
   mLoop = true;
   mMaxLoop = -1;
   mLoopCount = 0;
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

      if (distance < (pObj.mMaxSpeed * elapsedTime) || distance < cNode.distance )
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
                  stop = true;
               }
               else
               {
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

void Path::attachObject(SceneObject * object, F32 speed, F32 force, bool orientToPath, F32 angleOff, bool snapToNode, S32 startNode, bool loop, S32 maxLoop)
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
   pObj->mMaxForce   = force;
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
   F32 slowRad = mNodes[obj.mNextNode].distance;
   Vector2 oPos = obj.mObj->getPosition();
   Vector2 dir = cDest - oPos;
   Vector2 currVel = obj.mObj->getLinearVelocity();
   dir.Normalize();

   F32 maxSpeed = obj.mMaxSpeed;
   F32 maxForce = obj.mMaxForce;

   Vector2 steer = seek(cDest, oPos, maxSpeed, currVel, slowRad);
   steer = truncate(steer, maxForce);
   steer = steer.scale(0.5f);
   currVel = currVel.add(steer);
   currVel = truncate(currVel.add(steer), maxSpeed);
   Vector2 pos = oPos.add(currVel);

   //Steering Behavior
   obj.mObj->applyForce(pos, obj.mObj->getWorldCenter());

   //Simple direct move.
   //obj.mObj->setLinearVelocity(dir * obj.mMaxSpeed);

   if (obj.mOrient)
   {
      F32 rot = mRadToDeg(mAtan(dir.x, dir.y));
      rot = rot - obj.mAngOff;
      F32 ang = mDegToRad(rot);
      F32 speed = obj.mMaxSpeed;
      obj.mObj->rotateTo(ang, speed);
   }

}

Vector2 Path::truncate(Vector2 vec, F32 max)
{
   F32 i = max;
   i = i < 1.0f ? 1.0f : i;
   vec = vec.scale(max);
   return vec;
}

Vector2 Path::seek(Vector2 target, Vector2 objPos, F32 max, Vector2 curr, F32 slowRad)
{
   Vector2 des = target.sub(objPos);
   F32 dist = des.Length();
   des.Normalize();

   if (dist < slowRad)
   {
      des = des.scale(max * dist / slowRad);
   }
   else
   {
      des = des.scale(max);
   }

   Vector2 force = des.sub(curr);

   return force;
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
