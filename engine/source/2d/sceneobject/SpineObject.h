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

#ifndef _SPINE_OBJECT_H_
#define _SPINE_OBJECT_H_

#ifndef _FRAMEALLOCATOR_H_
#include "memory/frameAllocator.h"
#endif

#ifndef _SPRITE_BATCH_H_
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _SPINE_ASSET_H_
#include "2d/assets/SpineAsset.h"
#endif

#ifndef _SPINE_COLLISION_PROXY_H_
#include "2d/sceneobject/SpineCollisionProxy.h"
#endif

//------------------------------------------------------------------------------
// Spine events support
static void spineAnimationEventCallbackHandler(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event);

//------------------------------------------------------------------------------

static const b2Transform B2_IDENTITY_TRANSFORM = b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f));

class SpineObject : public SceneObject, public SpriteBatch
{
protected:
	typedef SceneObject Parent;

public:
	// Smart pointer support
	struct SkeletonDeleter { void operator()(spSkeleton* p) { spSkeleton_dispose(p); } };
	struct AnimationStateDeleter { void operator()(spAnimationState* p) { p->listener = NULL; spAnimationState_disposeStatics(); spAnimationState_dispose(p); } };
	struct SkeletonClippingDeleter { void operator()(spSkeletonClipping* p) { spSkeletonClipping_dispose(p); } };
	struct SkeletonBoundsDeleter { void operator()(spSkeletonBounds* p) { spSkeletonBounds_dispose(p); } };
	struct JitterVertexDeleter { void operator()(spJitterVertexEffect* p) { spJitterVertexEffect_dispose(p); } };
	struct SwirlVertexDeleter { void operator()(spSwirlVertexEffect* p) { spSwirlVertexEffect_dispose(p); } };

	using skeleton_ptr = unique_ptr<spSkeleton, SkeletonDeleter>;
	using animationState_ptr = unique_ptr<spAnimationState, AnimationStateDeleter>;
	using skeletonClipping_ptr = unique_ptr<spSkeletonClipping, SkeletonClippingDeleter>;
	using skeletonBounds_ptr = unique_ptr<spSkeletonBounds, SkeletonBoundsDeleter>;
	using jitterEffect_ptr = unique_ptr<spJitterVertexEffect, JitterVertexDeleter>;
	using swirlEffect_ptr = unique_ptr<spSwirlVertexEffect, SwirlVertexDeleter>;

	// Can't use smart pointers for this because HashTable calls delete on the value members in its destructor.
	using SpineCollisionProxyMapType = HashMap<spAttachment*, SpineCollisionProxy*>;

	enum VertexEffect {
		INVALID_VERTEX_EFFECT,

		NONE,
		JITTER,
		SWIRL
	};
private:
	AssetPtr<SpineAsset>			mSpineAsset; /* SpineObject holds instance data, SpineAsset holds template data. */

	skeleton_ptr					mSkeleton;
	animationState_ptr				mAnimationState;
	skeletonClipping_ptr			mSkeletonClipping;
	skeletonBounds_ptr				mSkeletonBounds;

	// Special vertex effects provided by Spine
	spVertexEffect					*mVertexEffect;  // Only one effect can be active at a time. This points to it. Null if none active.
	VertexEffect					mActiveEffect;
	jitterEffect_ptr				mJitterControl;
	swirlEffect_ptr					mSwirlControl;

	F32               mPreTickTime;
	F32               mPostTickTime;
	F32               mLastFrameTime;

	bool              mFlipX;
	bool              mFlipY;

	// OOBB calculation support
	F32				  mPriorRootBoneWorldX;
	F32				  mPriorRootBoneWorldY;
	bool			  mPriorFlipX;
	bool			  mPriorFlipY;

	// Auto-Center support
	Vector2			  mAutoCenterOffset;

	// Collision support
	SpineCollisionProxyMapType mCollisionProxies;

public:
	SpineObject() { resetState(); };

	// Use CopyTo() if need to replicate.
	SpineObject(const SpineObject&) = delete;
	SpineObject& SpineObject::operator=(const SpineObject&) = delete;

	// Add 'move' support if/when needed.
	SpineObject(SpineObject&& other) = delete;
	SpineObject& SpineObject::operator=(SpineObject&&) = delete;

	virtual void copyTo(SimObject* object);

	bool setSpineAsset(const char* pSpineAssetId);
	inline StringTableEntry getSpineAsset(void) const { return mSpineAsset.getAssetId(); }

	// Render flipping.
	void setFlip(const bool flipX, const bool flipY);
	void setFlipX(const bool flipX) { setFlip(flipX, mFlipY); }
	void setFlipY(const bool flipY) { setFlip(mFlipX, flipY); }
	inline bool getFlipX(void) const { return mFlipX; }
	inline bool getFlipY(void) const { return mFlipY; }

	// Render special effects
	inline VertexEffect getActiveEffect(void) const { return mActiveEffect; }

	// -- Jitter
	void enableJitter(const F32 x, const F32 y);
	void disableJitter(void);
	// -- These can be used by script at run time to vary the effect.
	inline F32 getJitterX(void) const { return mJitterControl ? mJitterControl->jitterX : 0.0f; }
	inline void setJitterX(const F32 x) { if (mJitterControl) mJitterControl->jitterX = x; }
	inline F32 getJitterY(void) const { return mJitterControl ? mJitterControl->jitterY : 0.0f; }
	inline void setJitterY(const F32 y) { if (mJitterControl) mJitterControl->jitterY = y; }

	// -- Swirl
	void enableSwirl(const F32 radius);
	void disableSwirl(void);
	// -- These can be used by script at run time to vary the effect.
	inline F32 getSwirlX(void) const { return mSwirlControl ? mSwirlControl->centerX : 0.0f; }
	inline void setSwirlX(const F32 x) { if (mSwirlControl) mSwirlControl->centerX = x; }
	inline F32 getSwirlY(void) const { return mSwirlControl ? mSwirlControl->centerY : 0.0f; }
	inline void setSwirlY(const F32 y) { if (mSwirlControl) mSwirlControl->centerY = y; }
	inline F32 getSwirlRadius(void) const { return mSwirlControl ? mSwirlControl->radius : 0.0f; }
	inline void setSwirlRadius(const F32 r) { if (mSwirlControl) mSwirlControl->radius = r; }
	inline F32 getSwirlAngle(void) const { return mSwirlControl ? mSwirlControl->angle : 0.0f; }
	inline void setSwirlAngle(const F32 a) { if (mSwirlControl) mSwirlControl->angle = a; }

	// Appearance
	void setScale(const Vector2& scale);
	inline void setScale(const F32 x, const F32 y) { setScale(Vector2(x, y)); }
	inline Vector2 getScale(void) const { return mSkeleton ? Vector2(mSkeleton->scaleX, mSkeleton->scaleY) : Vector2(0.0f, 0.0f); }

	bool setSkin(const char* pSkin);
	inline StringTableEntry getSkinName(void) const;

	// Animation
	inline F32 setTimeScale(const F32 timeScale);
	inline F32 getTimeScale(void) const { return mAnimationState ? mAnimationState->timeScale : 0.0f; }

	bool setAnimation(const char* pName, const int track = 0, const bool shouldLoop = false, const F32 mixDuration = -1.0f);
	bool setEmptyAnimation(const int track = 0, const F32 mixDuration = 0.0f);
	bool queueAnimation(const char* pName, const int track = 0, const bool shouldLoop = false, const F32 mixDuration = -1.0f, const F32 delay = 0.0f);
	bool queueEmptyAnimation(const int track = 0, const F32 mixDuration = 0.0f, const F32 delay = 0.0f);
	void clearAnimations(const int track, const bool mixToSetupPose = false, const F32 mixDuration = 0.0f);
	void clearAllAnimations(const bool mixToSetupPose = false, const F32 mixDuration = 0.0f);
	StringTableEntry getAnimationName(const int track = 0) const;
	bool getIsLooping(const int track = 0) const;
	bool setMix(const char* pFromName, const char* pToName, const F32 mixDuration);

	// Events
	void SpineObject::enableEventCallbacks(void);
	void SpineObject::disableEventCallbacks(void);

	// Collision Support
	const SpineCollisionProxy* getCollisionProxy(
		const char* anAttachmentName,
		const char* aSlotName,
		const char* aSkinName = "default",
		const F32 sizerWidth = 1.0f,
		const F32 sizerHeight = 1.0f,
		const char* objectName = NULL);
	bool deleteCollisionProxy(const char *proxyId);

	// This needs to be available to the Console for use during object instantiation.
	static void initPersistFields();

	/// Declare Console Object.
	DECLARE_CONOBJECT(SpineObject);

protected:
	// Render suport
	void updateSpine(const F32 time);
	void prepareSpineForRender();
	void calculateSpineOOBB(const vector<Vector2> pointSoup);

	virtual void preIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats);
	virtual void interpolateObject(const F32 timeDelta);

	virtual bool canPrepareRender(void) const { return true; }
	virtual bool validRender(void) const { return mSpineAsset.notNull(); }
	virtual bool shouldRender(void) const { return true; }
	virtual void scenePrepareRender(const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue);
	virtual void sceneRender(const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer);

	// Render special effects - support
	// Set indirectly via enable methods.  However, need this to support TAML reading of object.
	void setActiveEffect(const VertexEffect e);

	static VertexEffect getVertexEffectTypeEnum(const char* label);
	static const char* getVertexEffectTypeDescription(const VertexEffect vertexEffectType);

	// Internal management
	void SpineObject::resetState();

	virtual void OnRegisterScene(Scene *scene);
	virtual void OnUnregisterScene(Scene *scene);

private:
	// Utility 

	// Cope with spine reflecting about an axis to accomplish a flip, while the SceneObject doesn't support such flipping.
	inline F32 setPerFlipState(const F32 value) { return mFlipY ? (mFlipX ? value : -value) : (mFlipX ? -value : value); }

	// Object Persistence Support
	//
	// This determines if there is any animation data to write and returns true if there is.
	bool writeAnimationData(void) const;
	bool writeCollisionData(void) const;

	// This encodes and returns the information required to restart the currently running animations.  It is returned
	// in a string for writing to the TAML file.
	const char *getAnimationData(void) const;

	// This encodes and returns the information required to recreate the Spine object's collision boxes.  
	const char *getCollisionData(void) const;

	// This attempts to start the animation(s) defined in the animation data string passed in.
	void setAnimationData(const char *animationData);

	// This attempts to create the collision boxes defined in the data passed in.
	void setCollisionData(const char *collisionData);

protected:
	static bool setSpineAsset(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSpineAsset(data); return false; }
	static bool writeSpineAsset(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->mSpineAsset.notNull(); }

	static bool setAnimationData(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setAnimationData(data); return false; }
	static const char* getAnimationData(void* obj, const char* data) { return static_cast<SpineObject*>(obj)->getAnimationData(); }
	static bool writeAnimationData(void*obj, const char* data) { return static_cast<SpineObject*>(obj)->writeAnimationData(); }

	static bool setCollisionData(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setCollisionData(data); return false; }
	static const char* getCollisionData(void* obj, const char* data) { return static_cast<SpineObject*>(obj)->getCollisionData(); }
	static bool writeCollisionData(void*obj, const char* data) { return static_cast<SpineObject*>(obj)->writeCollisionData(); }

	static bool setSkin(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSkin(data); return false; }
	static const char* getSkinName(void* obj, const char* data) { return static_cast<SpineObject*>(obj)->getSkinName(); }
	static bool writeCurrentSkin(void*obj, StringTableEntry pSkin) { return true; }

	static bool setScale(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setScale(Vector2(data)); return false; }
	static const char* getScale(void* obj, const char* data) { return static_cast<SpineObject*>(obj)->getScale().scriptThis(); }
	static bool writeScale(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getScale().notZero(); }

	static bool setFlipX(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setFlipX(dAtob(data)); return false; }
	static bool writeFlipX(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getFlipX() == true; }

	static bool setFlipY(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setFlipY(dAtob(data)); return false; }
	static bool writeFlipY(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getFlipY() == true; }

	static bool setTimeScale(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setTimeScale(dAtof(data)); return false; }
	static const char* getTimeScale(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getTimeScale()); }
	static bool writeTimeScale(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getTimeScale() != 1.0f; }

	static bool setActiveEffectType(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setActiveEffect(getVertexEffectTypeEnum(data)); return false; }
	static bool writeActiveEffectType(void* obj, StringTableEntry pFieldName) { VertexEffect ve = static_cast<SpineObject*>(obj)->getActiveEffect(); return ve != NONE && ve != INVALID_VERTEX_EFFECT; }

	static bool setJitterX(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setJitterX(dAtof(data)); return false; }
	static const char* getJitterX(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getJitterX()); }
	static bool setJitterY(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setJitterY(dAtof(data)); return false; }
	static const char* getJitterY(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getJitterY()); }
	static bool writeJitterEffectValues(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getActiveEffect() == VertexEffect::JITTER; }

	static bool setSwirlX(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSwirlX(dAtof(data)); return false; }
	static const char* getSwirlX(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getSwirlX()); }
	static bool setSwirlY(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSwirlY(dAtof(data)); return false; }
	static const char* getSwirlY(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getSwirlY()); }
	static bool setSwirlRadius(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSwirlRadius(dAtof(data)); return false; }
	static const char* getSwirlRadius(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getSwirlRadius()); }
	static bool setSwirlAngle(void* obj, const char* data) { static_cast<SpineObject*>(obj)->setSwirlAngle(dAtof(data)); return false; }
	static const char* getSwirlAngle(void* obj, const char* data) { return Con::getFloatArg(static_cast<SpineObject*>(obj)->getSwirlAngle()); }
	static bool writeSwirlEffectValues(void* obj, StringTableEntry pFieldName) { return static_cast<SpineObject*>(obj)->getActiveEffect() == VertexEffect::SWIRL; }

	static bool setEventCallbacksEnabled(void* obj, const char* data) { if (dAtob(data)) { static_cast<SpineObject*>(obj)->enableEventCallbacks(); } else { static_cast<SpineObject*>(obj)->disableEventCallbacks(); } return false; }
	static bool writeEventCallbacksEnabled(void* obj, StringTableEntry pFieldName) { SpineObject *me = static_cast<SpineObject*>(obj); return me->mAnimationState && me->mAnimationState->listener; }

};

#endif // _SPINE_OBJECT_H_
