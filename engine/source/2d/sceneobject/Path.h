#ifndef _PATH_H_
#define _PATH_H_

#include "2d/sceneobject/SceneObject.h"

class Path;

class PathObject
{
private:
   friend class Path;
   void setCurrNode(S32 node);
   void setNextNode(S32 node);
   void setPrevNode(S32 node);

   Path* mPath;
   SimObjectPtr<SceneObject> mObj;
   SimObjectId mObjId;
   F32 mMaxSpeed;
   bool mOrient;
   F32 mMaxForce;
   F32 mAngOff;
   
   S32 mCurrNode;
   S32 mPrevNode;
   S32 mNextNode;
   bool mLoop; 
   
   bool mSnapToNode;
   S32 mLoopCount;
   S32 mMaxLoop;

public:
   PathObject();
   ~PathObject() {};

};

class Path : public SceneObject
{
   typedef SceneObject Parent;

public:
   struct Node
   {
      Node(Vector2 pos, F32 dst, F32 wght)
      {
         position = pos;
         distance = dst;
         weight = wght;
      };

      Vector2 position;
      F32 distance;
      F32 weight;
   };

   Path();
   ~Path();
   virtual void onDeleteNotify(SimObject* object);
   static void initPersistFields();

   virtual void preIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats);
   virtual void integrateObject(const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats);

   S32 addNode(Vector2 pos, F32 distance, F32 weight);

   S32 getNodeCount() const { return mNodes.size(); }

   inline Node& getNode(S32 index)
   {
      if (isValidNode(index)) return mNodes[index];
      return mNodes[0];
   }

   inline bool isValidNode(S32 index)
   {
      if (mNodes.empty()) return false;
      if((index >= 0) && (index < mNodes.size())) return true;
   }

   void attachObject(SceneObject* object, F32 speed, F32 force, bool orientToPath, F32 angleOff, bool snapToNode, S32 startNode, bool loop, S32 maxLoop);

   void detachObject(SceneObject* object);

   S32 getAttachedObjectCount() { return mObjs.size(); }

   SceneObject* getPathObject(U32 index) { if (index < mObjs.size()) return mObjs[index]->mObj; return NULL; }
   
   inline PathObject* getAttachedObject(const SceneObject* obj)
   {
      if (obj == NULL)
         return NULL;

      Vector<PathObject*>::iterator i;
      for (i = mObjs.begin(); i != mObjs.end(); i++)
         if ((*i)->mObj == obj) return *i;
   }

   DECLARE_CONOBJECT(Path);

private:

   void moveObject(PathObject& obj);
   Vector2 truncate(Vector2 vec, F32 max);
   Vector2 seek(Vector2 target, Vector2 objPos, F32 max, Vector2 curr, F32 slowRad);

   Vector<PathObject*> mObjs;
   Vector<Node> mNodes;

};

#endif