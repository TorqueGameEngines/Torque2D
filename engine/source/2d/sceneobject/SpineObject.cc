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
#include "2d/sceneobject/SpineObject.h"
#endif

// Script bindings.
#include "2d/sceneobject/SpineObject_ScriptBinding.h"

//------------------------------------------------------------------------------

void spineAnimationEventCallbackHandler(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event) {

	SpineObject *targetSpineObject = (SpineObject *)state->userData;
	if (!targetSpineObject) {
		Con::warnf("spineAnimationEventCallbackHandler - Event ('%s') received with no spine object in spAnimationState->userData.  Discarding event.",
			event->data->name);
	}

	const char* animationName = StringTable->insert(entry && entry->animation ? entry->animation->name : 0, true);

	switch (type) {
	case SP_ANIMATION_START:
		Con::executef(targetSpineObject, 3, "onAnimationStart", animationName, Con::getIntArg(entry->trackIndex));
		break;
	case SP_ANIMATION_INTERRUPT:
		Con::executef(targetSpineObject, 3, "onAnimationInterrupt", animationName, Con::getIntArg(entry->trackIndex));
		break;
	case SP_ANIMATION_END:
		Con::executef(targetSpineObject, 3, "onAnimationEnd", animationName, Con::getIntArg(entry->trackIndex));
		break;
	case SP_ANIMATION_COMPLETE:
		Con::executef(targetSpineObject, 3, "onAnimationComplete", animationName, Con::getIntArg(entry->trackIndex));
		break;
	case SP_ANIMATION_DISPOSE:
		Con::executef(targetSpineObject, 3, "onAnimationDispose", animationName, Con::getIntArg(entry->trackIndex));
		break;
	case SP_ANIMATION_EVENT:
		Con::executef(targetSpineObject, 10
			, "onAnimationEvent"
			, animationName
			, Con::getIntArg(entry->trackIndex)
			, event->data->name ? event->data->name : ""
			, Con::getIntArg(event->intValue)
			, Con::getFloatArg(event->floatValue)
			, event->stringValue ? event->stringValue : ""
			, Con::getFloatArg(event->time)
			, Con::getFloatArg(event->volume)
			, Con::getFloatArg(event->balance));
		break;
	}
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SpineObject);

//------------------------------------------------------------------------------

// Spine vertex effect support
static EnumTable::Enums vertexEffectLookup[] =
{
	{ SpineObject::NONE,     "None"   },
	{ SpineObject::JITTER,   "Jitter" },
	{ SpineObject::SWIRL,    "Swirl"  }
};

static EnumTable VertexEffectTable(3, &vertexEffectLookup[0]);

//------------------------------------------------------------------------------

SpineObject::VertexEffect SpineObject::getVertexEffectTypeEnum(const char* label)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(vertexEffectLookup) / sizeof(EnumTable::Enums)); i++)
		if (dStricmp(vertexEffectLookup[i].label, label) == 0)
			return((SpineObject::VertexEffect)vertexEffectLookup[i].index);

	// Warn.
	Con::warnf("SpineObject::getVertexEffectTypeEnum() - Invalid vertex effect type '%s'.", label);

	return VertexEffect::INVALID_VERTEX_EFFECT;
}

//-----------------------------------------------------------------------------

const char* SpineObject::getVertexEffectTypeDescription(const VertexEffect vertexEffectType)
{
	// Search for Mnemonic.
	for (U32 i = 0; i < (sizeof(vertexEffectLookup) / sizeof(EnumTable::Enums)); i++)
	{
		if (vertexEffectLookup[i].index == (S32)vertexEffectType)
			return vertexEffectLookup[i].label;
	}

	// Warn.
	Con::warnf("SpineObject::getVertexEffectTypeDescription() - Invalid vertex effect type.");

	return StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

void SpineObject::initPersistFields()
{
	// Call parent.
	Parent::initPersistFields();

	addProtectedField("Asset", TypeSpineAssetPtr, Offset(mSpineAsset, SpineObject), &setSpineAsset, &defaultProtectedGetFn, &writeSpineAsset, "The spine asset ID used for the spine.");
	addProtectedField("Skin", TypeString, 0, &setSkin, &getSkinName, &writeCurrentSkin, "The skin to use.");
	addProtectedField("Scale", TypeVector2, 0, &setScale, &getScale, &writeScale, "Scaling of the skeleton geometry.");
	addProtectedField("AnimationData", TypeString, 0, &setAnimationData, &getAnimationData, &writeAnimationData, "String encoding of the running animations.  It's a tilde separated list of animation entries.  Within each entry, each attribute is separated by a semicolon.  The attributes are, in this order:  1) Name - String: Name of animation as defined in Spine.  2) Track - Integer: Track the animation is running on.  3) Is Looping - Boolean: 1 or 0.  4) Mix Duration - Float: Can be set to -1.0 to request default mix duration.");
	addProtectedField("TimeScale", TypeF32, 0, &setTimeScale, &getTimeScale, &writeTimeScale, "Time scale (animation speed) adjustment factor.");
	addProtectedField("FlipX", TypeBool, Offset(mFlipX, SpineObject), &setFlipX, &defaultProtectedGetFn, &writeFlipX, "Whether to invert the image horizontally.");
	addProtectedField("FlipY", TypeBool, Offset(mFlipY, SpineObject), &setFlipY, &defaultProtectedGetFn, &writeFlipY, "Whether image should be inverted vertically.");
	addGroup("Vertex Effects");
	addProtectedField("ActiveEffect", TypeEnum, Offset(mActiveEffect, SpineObject), &setActiveEffectType, &defaultProtectedGetFn, &writeActiveEffectType, 1, &VertexEffectTable, "The name of the vertex effect assigned, or None.");
	addProtectedField("JitterX", TypeF32, 0, &setJitterX, &getJitterX, &writeJitterEffectValues, "A 'Jitter' vertex special effect setting.  Note: Play around with the various settings.  They can be modified on the fly to vary the effect the displayed item.");
	addProtectedField("JitterY", TypeF32, 0, &setJitterY, &getJitterY, &writeJitterEffectValues, "A 'Jitter' vertex special effect setting.");
	addProtectedField("SwirlX", TypeF32, 0, &setSwirlX, &getSwirlX, &writeSwirlEffectValues, "A 'Swirl' vertex special effect setting.");
	addProtectedField("SwirlY", TypeF32, 0, &setSwirlY, &getSwirlY, &writeSwirlEffectValues, "A 'Swirl' vertex special effect setting.");
	addProtectedField("SwirlRadius", TypeF32, 0, &setSwirlRadius, &getSwirlRadius, &writeSwirlEffectValues, "A 'Swirl' vertex special effect setting.");
	addProtectedField("SwirlAngle", TypeF32, 0, &setSwirlAngle, &getSwirlAngle, &writeSwirlEffectValues, "A 'Swirl' vertex special effect setting.");
	endGroup("Vertex Effects");
	addProtectedField("EventCallbacksEnabled", TypeBool, 0, &setEventCallbacksEnabled, &defaultProtectedGetFn, &writeEventCallbacksEnabled, "Whether the SpineObject should receive spine animation event callbacks.");
	addProtectedField("CollisionData", TypeString, 0, &setCollisionData, &getCollisionData, &writeCollisionData, "String encoding of the Spine object's collision boxes.  It's a tilde separated list of collision proxy definitions.  Within each entry, each attribute is separated by a semicolon.  The attributes are, in this order:  1) Attachment Name - String: Name of attachment that box belongs to.  2) Slot Name - String: Slot that owns the attachment.  3) Skin Name - String: Skin that defines the attachment.  4) Width Sizer - Float: Factor useful to tweak the width of the box.  5) Height Sizer - Float: Factor useful to tweak the height of the box.  6) SimObject Name - String: Name assigned to the collision proxy instance.  May be NULL if no name was assigned.");

}

//------------------------------------------------------------------------------

void SpineObject::resetState() {

	mPreTickTime = 0.0f;
	mPostTickTime = 0.0f;
	mLastFrameTime = 0.0f;
	mFlipX = mFlipY = false;

	mSkeleton.reset();
	mAnimationState.reset();
	mSkeletonClipping.reset();
	mSkeletonBounds.reset();

	mAutoCenterOffset.SetZero();

	// Don't want b2DynamicTree supported search (for culling reasons) being performed 
	// on our couple-o-sprites lashup.
	setBatchCulling(false);

	mVertexEffect = NULL;
	mActiveEffect = VertexEffect::NONE;

	mPriorRootBoneWorldX = 0.0f;
	mPriorRootBoneWorldY = 0.0f;
	mPriorFlipX = mFlipX;
	mPriorFlipY = mFlipY;

	mCollisionProxies.clear();
}

//------------------------------------------------------------------------------

// NOTE: Update this if new state members are added to the object.
void SpineObject::copyTo(SimObject* object)
{
	// Call to parent.
	Parent::copyTo(object);

	// Fetch object.
	SpineObject* pSpine = dynamic_cast<SpineObject*>(object);

	// Sanity!
	AssertFatal(pSpine != NULL, "SpineObject::copyTo() - Object is not the correct type.");

	// Copy state.
	pSpine->setSpineAsset(getSpineAsset());
	pSpine->setFlipX(getFlipX());
	pSpine->setFlipY(getFlipY());
	pSpine->setPosition(getPosition());
	pSpine->setAngle(getAngle());
	pSpine->setAnimation(getAnimationName(), getIsLooping());
	pSpine->setSkin(getSkinName());
	pSpine->setScale(getScale());
	pSpine->mAutoCenterOffset = mAutoCenterOffset;

	copyCollisionShapes(pSpine);
}

//-----------------------------------------------------------------------------

bool SpineObject::setSpineAsset(const char* pSpineAssetId)
{
	// Sanity!
	if (pSpineAssetId == NULL)
	{
		Con::errorf("SpineObject::setSpineAsset() - Cannot use a NULL asset Id.");
		return false;
	}

	// Fetch the asset.
	AssetPtr<SpineAsset> temp = StringTable->insert(pSpineAssetId, true);

	if (temp->mImageAsset.isNull())
	{
		Con::errorf("SpineObject::setSpineAsset() - Image asset is undefined.");
		return false;
	}

	// Clear away existing. 
	clearSprites();
	resetState();

	// Reflect asset definition
	mSpineAsset = std::move(temp);
	mSkeleton = skeleton_ptr(spSkeleton_create(mSpineAsset->mSkeletonData));
	mAnimationState = animationState_ptr(spAnimationState_create(mSpineAsset->mAnimationStateData));
	mSkeletonBounds = skeletonBounds_ptr(spSkeletonBounds_create());
	mSkeletonClipping = skeletonClipping_ptr(spSkeletonClipping_create());

	spSkeleton_setToSetupPose(mSkeleton.get());

	// Needed by the events system to route callbacks.
	mAnimationState->userData = this;

	// Prepare for animation.
	updateSpine(0.0f);

	return true;
}

//-----------------------------------------------------------------------------

void SpineObject::setFlip(const bool flipX, const bool flipY) {

	mFlipX = flipX;
	mSkeleton->scaleX = flipX
		? mSkeleton->scaleX < 0 ? mSkeleton->scaleX : -mSkeleton->scaleX
		: mSkeleton->scaleX >= 0 ? mSkeleton->scaleX : -mSkeleton->scaleX;

	mFlipY = flipY;
	mSkeleton->scaleY = flipY
		? mSkeleton->scaleY < 0 ? mSkeleton->scaleY : -mSkeleton->scaleY
		: mSkeleton->scaleY >= 0 ? mSkeleton->scaleY : -mSkeleton->scaleY;
}

//-----------------------------------------------------------------------------

F32 SpineObject::setTimeScale(const F32 timeScale) {
	if (!mAnimationState)
		return 0.0f;

	F32 previousValue = mAnimationState->timeScale;

	mAnimationState->timeScale = timeScale;

	return previousValue;
}

//-----------------------------------------------------------------------------

bool SpineObject::setAnimation(const char* pName, const int track, const bool shouldLoop, const F32 mixDuration)
{
	if (!mAnimationState)
		return false;

	// Check to see if the requested animation is valid
	auto animation = spSkeletonData_findAnimation(mSkeleton->data, StringTable->insert(pName, true));

	if (!animation)
	{
		Con::warnf("SpineObject::setAnimation() - Animation '%s' does not exist.", StringTable->insert(pName, true));
		return false;
	}

	// Set the animation.
	spTrackEntry* entry = spAnimationState_setAnimation(mAnimationState.get(), track, animation, shouldLoop);

	if (mixDuration < 0.0f) {
		// Use default mix duration
		return entry != NULL;
	}
	else {
		// Use given mix duration.
		entry->mixDuration = mixDuration;
		return true;
	}
}

//-----------------------------------------------------------------------------

bool SpineObject::setEmptyAnimation(const int track, const F32 mixDuration) {
	if (!mAnimationState)
		return false;

	return spAnimationState_setEmptyAnimation(mAnimationState.get(), track, mixDuration) != NULL;
}

//-----------------------------------------------------------------------------

bool SpineObject::queueAnimation(const char* pName, const int track, const bool shouldLoop, const F32 mixDuration, const F32 delay)
{
	if (!mAnimationState)
		return false;

	// Check to see if the requested animation is valid
	auto animation = spSkeletonData_findAnimation(mSkeleton->data, StringTable->insert(pName, true));

	if (!animation)
	{
		Con::warnf("SpineObject::queueAnimation() - Animation '%s' does not exist.", StringTable->insert(pName, true));
		return false;
	}

	// Set the animation.
	spTrackEntry* entry = spAnimationState_addAnimation(mAnimationState.get(), track, animation, shouldLoop, delay);

	if (mixDuration < 0.0f) {
		// Use default mix duration
		return entry != NULL;
	}
	else {
		// Use given mix duration.
		entry->mixDuration = mixDuration;
		return true;
	}
}

//-----------------------------------------------------------------------------

bool SpineObject::queueEmptyAnimation(const int track, const F32 mixDuration, const F32 delay) {
	if (!mAnimationState)
		return false;

	return spAnimationState_addEmptyAnimation(mAnimationState.get(), track, mixDuration, delay) != NULL;
}

//-----------------------------------------------------------------------------

void SpineObject::clearAnimations(const int track, const bool mixToSetupPose, const F32 mixDuration) {
	if (!mAnimationState)
		return;

	spAnimationState_clearTrack(mAnimationState.get(), track);

	if (mixToSetupPose)
		setEmptyAnimation(track, mixDuration);
}

//-----------------------------------------------------------------------------

void SpineObject::clearAllAnimations(const bool mixToSetupPose, const F32 mixDuration) {
	if (!mAnimationState)
		return;

	spAnimationState_clearTracks(mAnimationState.get());

	if (mixToSetupPose)
		spAnimationState_setEmptyAnimations(mAnimationState.get(), mixDuration);
}

//-----------------------------------------------------------------------------

StringTableEntry SpineObject::getAnimationName(const int track) const {
	if (!mAnimationState)
		return StringTable->EmptyString;

	spTrackEntry* entry = spAnimationState_getCurrent(mAnimationState.get(), track);

	if (!entry)
		return StringTable->EmptyString;

	return StringTable->insert(entry->animation->name, true);
}

//-----------------------------------------------------------------------------

bool SpineObject::getIsLooping(const int track) const {
	if (!mAnimationState)
		return StringTable->EmptyString;

	spTrackEntry* entry = spAnimationState_getCurrent(mAnimationState.get(), track);

	if (!entry)
		return StringTable->EmptyString;

	return entry->loop;
}

//-----------------------------------------------------------------------------

bool SpineObject::setMix(const char* pFromAnimation, const char* pToAnimation, const F32 time)
{
	if (mSpineAsset.isNull())
	{
		Con::warnf("SpineObject::setMix() - Cannot mix. No asset assigned");
		return false;
	}

	// Check for valid animation state data
	AssertFatal(mSpineAsset->mAnimationStateData != NULL, "SpineObject::setMix() - Animation state data invalid");

	// Check to see if the "from animation" is valid
	auto from = spSkeletonData_findAnimation(mSkeleton->data, StringTable->insert(pFromAnimation, true));

	if (!from)
	{
		Con::warnf("SpineObject::setMix() - Animation '%s' does not exist.", StringTable->insert(pFromAnimation, true));
		return false;
	}

	// Check to see if the "to animation" is valid
	auto to = spSkeletonData_findAnimation(mSkeleton->data, StringTable->insert(pToAnimation, true));

	if (!to)
	{
		Con::warnf("SpineObject::setMix() - Animation '%s' does not exist.", StringTable->insert(pToAnimation, true));
		return false;
	}

	// Check to see if a valid mix time was passsed
	if (time < 0.0f)
	{
		Con::warnf("SpineObject::setMix() - Invalid time set, '%f'", time);
		return false;
	}

	spAnimationStateData_setMix(mSpineAsset->mAnimationStateData, from, to, time);
	return true;
}

//-----------------------------------------------------------------------------

bool SpineObject::setSkin(const char* pSkin)
{
	if (mSpineAsset.isNull() || !mSkeleton)
	{
		Con::errorf("SpineObject::setSkin() - Spine asset was null or skeleton was not built.");
		return false;
	}

	auto to = spSkeletonData_findSkin(mSkeleton->data, StringTable->insert(pSkin, true));
	if (!to)
	{
		Con::warnf("SpineObject::setSkin() - Skin '%s' does not exist.", StringTable->insert(pSkin, true));
		return false;
	}

	spSkeleton_setSkin(mSkeleton.get(), to);
	spSkeleton_setSlotsToSetupPose(mSkeleton.get());
	spAnimationState_apply(mAnimationState.get(), mSkeleton.get());

	return true;
}

//-----------------------------------------------------------------------------

inline StringTableEntry SpineObject::getSkinName(void) const {
	if (!mAnimationState || !mSkeleton)
		return StringTable->EmptyString;

	spSkin* pSkin = mSkeleton->skin;

	if (!pSkin) {
		AssertFatal(mSkeleton->data->defaultSkin, avar("SpineObject::getSkinName() - Skin name is undefined in '%s'.  Is file valid?", mSpineAsset->mSpineFile));

		// Using default skin.
		return StringTable->insert(mSkeleton->data->defaultSkin->name, true);
	}
	else {
		return StringTable->insert(pSkin->name, true);
	}
}

//-----------------------------------------------------------------------------

void SpineObject::setScale(const Vector2& scale)
{
	if (!mSkeleton)
		return;

	// Set scale, but keep orientation.  This is used by spine to flip the character
	// as well as set its size.
	mSkeleton->scaleX = mSkeleton->scaleX < 0.0f ? -scale.x : scale.x;
	mSkeleton->scaleY = mSkeleton->scaleY < 0.0f ? -scale.y : scale.y;
}

//------------------------------------------------------------------------------

void SpineObject::setActiveEffect(const VertexEffect requestedEffect) {
	if (mActiveEffect == requestedEffect)
		return;

	switch (requestedEffect) {
	case VertexEffect::JITTER:
		if (!mJitterControl) {
			mJitterControl = jitterEffect_ptr(spJitterVertexEffect_create(0, 0));
		}
		mVertexEffect = &mJitterControl->super;
		mActiveEffect = VertexEffect::JITTER;
		break;
	case VertexEffect::SWIRL:
		if (!mSwirlControl) {
			mSwirlControl = swirlEffect_ptr(spSwirlVertexEffect_create(0));
		}
		mVertexEffect = &mSwirlControl->super;
		mActiveEffect = VertexEffect::SWIRL;
		break;
	default:
		Con::warnf("SpineObject::setActiveEffect - Unrecognized vertex special effect requested: '%s'.",
			getVertexEffectTypeDescription(requestedEffect));
	}
}

//------------------------------------------------------------------------------

void SpineObject::enableJitter(const F32 x, const F32 y) {
	if (mJitterControl) {
		mJitterControl->jitterX = x;
		mJitterControl->jitterY = y;
	}
	else {
		mJitterControl = jitterEffect_ptr(spJitterVertexEffect_create(x, y));
	}
	mVertexEffect = &mJitterControl->super;
	mActiveEffect = VertexEffect::JITTER;
}

//------------------------------------------------------------------------------

void SpineObject::disableJitter() {
	if (mActiveEffect == VertexEffect::JITTER) {
		mVertexEffect = NULL;
		mActiveEffect = VertexEffect::NONE;
	}
}

//------------------------------------------------------------------------------

void SpineObject::enableSwirl(const F32 radius) {
	if (mSwirlControl) {
		mSwirlControl->radius = radius;
	}
	else {
		mSwirlControl = swirlEffect_ptr(spSwirlVertexEffect_create(radius));
	}
	mVertexEffect = &mSwirlControl->super;
	mActiveEffect = VertexEffect::SWIRL;
}

//------------------------------------------------------------------------------

void SpineObject::disableSwirl() {
	if (mActiveEffect == VertexEffect::SWIRL) {
		mVertexEffect = NULL;
		mActiveEffect = VertexEffect::NONE;
	}
}

//------------------------------------------------------------------------------

void SpineObject::enableEventCallbacks(void) {
	// No animation state
	if (!mAnimationState)
		return;

	// Already listening
	if (mAnimationState->listener)
		return;

	mAnimationState->listener = spineAnimationEventCallbackHandler;
}

//-----------------------------------------------------------------------------

void SpineObject::disableEventCallbacks(void) {
	// No animation state
	if (!mAnimationState)
		return;

	// Not listening anyway.
	if (!mAnimationState->listener)
		return;

	mAnimationState->listener = NULL;
}

//------------------------------------------------------------------------------

void SpineObject::preIntegrate(const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats)
{
	Parent::preIntegrate(totalTime, elapsedTime, pDebugStats);

	// Note tick times.
	mPreTickTime = mPostTickTime;
	mPostTickTime = totalTime;

	// Update at pre-tick time.
	updateSpine(mPreTickTime);
	prepareSpineForRender();
}

//-----------------------------------------------------------------------------

void SpineObject::interpolateObject(const F32 timeDelta)
{
	Parent::interpolateObject(timeDelta);

	// Update time (interpolated).
	F32 timeInterp = (timeDelta * mPreTickTime) + ((1.0f - timeDelta) * mPostTickTime);
	updateSpine(timeInterp);
	prepareSpineForRender();
}

//-----------------------------------------------------------------------------

void SpineObject::scenePrepareRender(const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue)
{
	// Set batch transform to identity because the skeleton is responsible for 
	// its geometry's position
	setBatchTransform(B2_IDENTITY_TRANSFORM);

	SpriteBatch::prepareRender(this, pSceneRenderState, pSceneRenderQueue);
}

//-----------------------------------------------------------------------------

void SpineObject::sceneRender(const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer)
{
	// Render.
	SpriteBatch::render(pSceneRenderState, pSceneRenderRequest, pBatchRenderer);
}

//-----------------------------------------------------------------------------

void SpineObject::updateSpine(const F32 time)
{
	// Update the skeleton's position/rotation.
	Vector2 p = getPosition();
	mSkeleton->x = p.x - mAutoCenterOffset.x;
	mSkeleton->y = p.y - mAutoCenterOffset.y;
	mSkeleton->root->rotation = setPerFlipState(mRadToDeg(getAngle()));

	// Advance time
	F32 delta = (time - mLastFrameTime);
	mLastFrameTime = time;

	spSkeleton_update(mSkeleton.get(), delta);
	spAnimationState_update(mAnimationState.get(), delta);
	spAnimationState_apply(mAnimationState.get(), mSkeleton.get());
	spSkeleton_updateWorldTransform(mSkeleton.get());
}

//-----------------------------------------------------------------------------

void SpineObject::prepareSpineForRender()
{
	// Early out if skeleton is invisible
	if (mSkeleton->color.a == 0) {
		for (const auto& i : mCollisionProxies) {
			i.value->deActivate();
		}
		return;
	}

	if (mVertexEffect)
		mVertexEffect->begin(mVertexEffect, mSkeleton.get());

	// Get the ImageAsset used by the sprites
	StringTableEntry assetId = mSpineAsset->mImageAsset.getAssetId();

	clearSprites();

	b2AABB spriteAABB;
	U16 quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
	FrameTemp<F32> VertexBuffer(BatchRender::maxVertexCount);

	vector<Vector2> pointSoup;

	for (int i = 0; i < mSkeleton->slotsCount; ++i)
	{
		auto slot = mSkeleton->drawOrder[i];
		if (!slot)
			continue;

		auto attachment = slot->attachment;
		if (!attachment)
			continue;

		SpineCollisionProxy* pCollisionProxy = NULL;
		auto itr = mCollisionProxies.find(attachment);
		if (itr != mCollisionProxies.end()) {
			pCollisionProxy = itr->value;
		}

		if (slot->color.a == 0 || !slot->bone->active) {
			spSkeletonClipping_clipEnd(mSkeletonClipping.get(), slot);

			if (pCollisionProxy && pCollisionProxy->mActive) {
				pCollisionProxy->deActivate();
			}
			continue;
		}

		F32	*uvs = 0;
		U16	*indices = 0;
		int	indicesCount = 0;
		F32 *vertices = VertexBuffer; // It will be redirected to a different buffer if clipping is performed.
		int	verticesCount = 0;
		spColor* attachmentColor = NULL;
		string attachmentName;

		if (attachment->type == SP_ATTACHMENT_REGION) {
			auto regionAttachment = (spRegionAttachment*)attachment;
			attachmentName = StringTable->insert(regionAttachment->path ? regionAttachment->path : attachment->name, true);
			attachmentColor = &regionAttachment->color;

			// Is slot invisible?
			if (attachmentColor->a == 0) {
				spSkeletonClipping_clipEnd(mSkeletonClipping.get(), slot);

				if (pCollisionProxy && pCollisionProxy->mActive) {
					pCollisionProxy->deActivate();
				}
				continue;
			}

			if (pCollisionProxy && !pCollisionProxy->mActive) {
				pCollisionProxy->activate();
			}

			auto currentRegion = (spAtlasRegion *)regionAttachment->rendererObject;

			spRegionAttachment_updateOffset(regionAttachment);
			spRegionAttachment_setUVs(regionAttachment, currentRegion->u, currentRegion->v, currentRegion->u2, currentRegion->v2, currentRegion->rotate);
			spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, vertices, 0, 2);

			verticesCount = 4;
			uvs = regionAttachment->uvs;
			indices = quadIndices;
			indicesCount = 6;
		}
		else if (attachment->type == SP_ATTACHMENT_MESH) {
			auto meshAttachment = (spMeshAttachment*)attachment;
			attachmentName = StringTable->insert(meshAttachment->path ? meshAttachment->path : attachment->name, true);

			attachmentColor = &meshAttachment->color;

			// Is slot invisible?
			if (attachmentColor->a == 0) {
				spSkeletonClipping_clipEnd(mSkeletonClipping.get(), slot);

				if (pCollisionProxy && pCollisionProxy->mActive) {
					pCollisionProxy->deActivate();
				}
				continue;
			}

			// Vertex buffer overrun?
			if (meshAttachment->super.worldVerticesLength > BatchRender::maxVertexCount) {
				Con::warnf("Mesh attachment '%s' exceeded vertex buffer size. Mesh count was %d. Buffer size is %d.\n",
					attachmentName, meshAttachment->super.worldVerticesLength, BatchRender::maxVertexCount);
				continue;
			}

			if (pCollisionProxy && !pCollisionProxy->mActive) {
				pCollisionProxy->activate();
			}

			// Compute updated render info.
			spVertexAttachment_computeWorldVertices(&meshAttachment->super, slot, 0, meshAttachment->super.worldVerticesLength, vertices, 0, 2);

			verticesCount = meshAttachment->super.worldVerticesLength >> 1;
			uvs = meshAttachment->uvs;
			indices = meshAttachment->triangles;
			indicesCount = meshAttachment->trianglesCount;
		}
		else if (attachment->type == SP_ATTACHMENT_CLIPPING) {
			auto clippingAttachment = (spClippingAttachment*)attachment;
			spSkeletonClipping_clipStart(mSkeletonClipping.get(), slot, clippingAttachment);
			continue;
		}
		else continue;

		// Perform clipping if active
		if (spSkeletonClipping_isClipping(mSkeletonClipping.get())) {
			spSkeletonClipping_clipTriangles(mSkeletonClipping.get(), vertices, verticesCount << 1, indices, indicesCount, uvs, 2);
			vertices = mSkeletonClipping->clippedVertices->items;
			verticesCount = mSkeletonClipping->clippedVertices->size >> 1;
			uvs = mSkeletonClipping->clippedUVs->items;
			indices = mSkeletonClipping->clippedTriangles->items;
			indicesCount = mSkeletonClipping->clippedTriangles->size;
		}

		if (!verticesCount) {
			// No geometry to display.
			if (pCollisionProxy && pCollisionProxy->mActive) {
				pCollisionProxy->deActivate();
			}
			continue;
		}

		if (!mSpineAsset->getPreMultipliedAlpha()) {
			// Not using Premultiplied Alpha
			switch (slot->data->blendMode) {
			case SP_BLEND_MODE_ADDITIVE:
				setSrcBlendFactor(GL_SRC_ALPHA);
				setDstBlendFactor(GL_ONE);
				break;
			case SP_BLEND_MODE_MULTIPLY:
				setSrcBlendFactor(GL_DST_COLOR);
				setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);
				break;
			case SP_BLEND_MODE_SCREEN:
				setSrcBlendFactor(GL_ONE);
				setDstBlendFactor(GL_ONE_MINUS_SRC_COLOR);
				break;
			case SP_BLEND_MODE_NORMAL:
			default:
				setSrcBlendFactor(GL_SRC_ALPHA);
				setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);
				break;
			}
		}
		else {
			// Setup for Premultiplied Alpha
			switch (slot->data->blendMode) {
			case SP_BLEND_MODE_ADDITIVE:
				setSrcBlendFactor(GL_ONE);
				setDstBlendFactor(GL_ONE);
				break;
			case SP_BLEND_MODE_MULTIPLY:
				setSrcBlendFactor(GL_DST_COLOR);
				setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);
				break;
			case SP_BLEND_MODE_SCREEN:
				setSrcBlendFactor(GL_ONE);
				setDstBlendFactor(GL_ONE_MINUS_SRC_COLOR);
				break;
			case SP_BLEND_MODE_NORMAL:
			default:
				setSrcBlendFactor(GL_ONE);
				setDstBlendFactor(GL_ONE_MINUS_SRC_ALPHA);
				break;
			}
		}

		// Define sprite carrier object.  NOTE: This isn't a Sprite. It's a SpriteBatchItem, which is completely different
		// than a Sprite.  It's more like a render request headed for the batch renderer.
		SpriteBatchItem* pSprite = SpriteBatch::createSprite();

		pSprite->setDepth(mSceneLayerDepth);

		pSprite->setSrcBlendFactor(mSrcBlendFactor);
		pSprite->setDstBlendFactor(mDstBlendFactor);

		F32 r = mSkeleton->color.r * slot->color.r * attachmentColor->r;
		F32 g = mSkeleton->color.g * slot->color.g * attachmentColor->g;
		F32 b = mSkeleton->color.b * slot->color.b * attachmentColor->b;
		F32 a = mSkeleton->color.a * slot->color.a * attachmentColor->a;

		spColor light;
		light.r = r; light.g = g; light.b = b; light.a = a;

		SpriteBatchItem::drawData *pDrawData = pSprite->getDrawData();
		pDrawData->size(indicesCount);

		spriteAABB.lowerBound.x = spriteAABB.upperBound.x = vertices[0];
		spriteAABB.lowerBound.y = spriteAABB.upperBound.y = vertices[1];
		if (mVertexEffect != NULL) {
			vector<F32> vertexEffectUVs;
			vector<spColor> vertexEffectColors;
			for (int j = 0; j < verticesCount; j++) {
				spColor vertexColor = light;
				spColor dark;
				dark.r = dark.g = dark.b = dark.a = 0;
				int index = j << 1;
				float x = vertices[index];
				float y = vertices[index + 1];
				float u = uvs[index];
				float v = uvs[index + 1];
				mVertexEffect->transform(mVertexEffect, &x, &y, &u, &v, &vertexColor, &dark);
				vertices[index] = x;
				vertices[index + 1] = y;
				vertexEffectUVs.push_back(u);
				vertexEffectUVs.push_back(v);
				vertexEffectColors.push_back(vertexColor);
			}

			for (int j = 0; j < indicesCount; ++j) {
				int index = indices[j] << 1;
				F32 x = vertices[index];
				F32 y = vertices[index + 1];
				spColor vertexColor = vertexEffectColors[index >> 1];

				pDrawData->vertexArray[j].Set(x, y);
				pDrawData->textureArray[j].Set(vertexEffectUVs[index], vertexEffectUVs[index + 1]);
				pDrawData->colorArray[j].red = vertexColor.r;
				pDrawData->colorArray[j].green = vertexColor.g;
				pDrawData->colorArray[j].blue = vertexColor.b;
				pDrawData->colorArray[j].alpha = vertexColor.a;

				spriteAABB.lowerBound.x = x < spriteAABB.lowerBound.x ? x : spriteAABB.lowerBound.x;
				spriteAABB.lowerBound.y = y < spriteAABB.lowerBound.y ? y : spriteAABB.lowerBound.y;
				spriteAABB.upperBound.x = x > spriteAABB.upperBound.x ? x : spriteAABB.upperBound.x;
				spriteAABB.upperBound.y = y > spriteAABB.upperBound.y ? y : spriteAABB.upperBound.y;
			}
		}
		else {
			for (int j = 0; j < indicesCount; ++j) {
				int index = indices[j] << 1;
				F32 x = vertices[index];
				F32 y = vertices[index + 1];

				pDrawData->vertexArray[j].Set(vertices[index], vertices[index + 1]);
				pDrawData->textureArray[j].Set(uvs[index], uvs[index + 1]);
				pDrawData->colorArray[j].red = r;
				pDrawData->colorArray[j].green = g;
				pDrawData->colorArray[j].blue = b;
				pDrawData->colorArray[j].alpha = a;

				spriteAABB.lowerBound.x = x < spriteAABB.lowerBound.x ? x : spriteAABB.lowerBound.x;
				spriteAABB.lowerBound.y = y < spriteAABB.lowerBound.y ? y : spriteAABB.lowerBound.y;
				spriteAABB.upperBound.x = x > spriteAABB.upperBound.x ? x : spriteAABB.upperBound.x;
				spriteAABB.upperBound.y = y > spriteAABB.upperBound.y ? y : spriteAABB.upperBound.y;
			}
		}

		// Save the sprite's aabb on the sprite.
		F32 ev[]{
			spriteAABB.upperBound.x, spriteAABB.upperBound.y,	//LL
			spriteAABB.lowerBound.x, spriteAABB.upperBound.y,	//LR
			spriteAABB.lowerBound.x, spriteAABB.lowerBound.y,	//UR
			spriteAABB.upperBound.x, spriteAABB.lowerBound.y };	//UL
		pSprite->setExplicitVertices(ev);

		pSprite->setTriangleRun(true);
		pSprite->setImage(assetId, attachmentName.c_str());

		spSkeletonClipping_clipEnd(mSkeletonClipping.get(), slot);

		if (pCollisionProxy) {
			// Center collision box on attachment.
			pCollisionProxy->setPosition(
				Vector2((spriteAABB.lowerBound.x + spriteAABB.upperBound.x) * 0.5f,
				(spriteAABB.lowerBound.y + spriteAABB.upperBound.y) * 0.5f)
			);
			pCollisionProxy->setAngle(
				mDegToRad(spBone_localToWorldRotation(slot->bone, slot->bone->rotation + pCollisionProxy->mRotation))
			);
		}

		// Capture this sprite's vertices for OOBB calculation later.
		pointSoup.insert(pointSoup.end(), pDrawData->vertexArray.begin(), pDrawData->vertexArray.end());
	}

	spSkeletonClipping_clipEnd2(mSkeletonClipping.get());

	if (mVertexEffect)
		mVertexEffect->end(mVertexEffect);

	calculateSpineOOBB(pointSoup);
}


//-----------------------------------------------------------------------------
// This spins through all of the spine object's drawn points and calculates its OOBB.
// It then updates the location of the SceneObject to center it over the spine object.
void SpineObject::calculateSpineOOBB(const vector<Vector2> pointSoup) {

	if (!pointSoup.size()) {
		setSize(Vector2(0.0f, 0.0f));
		setLocalExtentsDirty();
		updateLocalExtents();
		return;
	}

	// First step is to get the AABB.
	b2AABB oobb = getLocalAABB();

	if (getAngle() == 0.0f) {
		// The OOBB coincides with the AABB so we are done.
		setSize(oobb.upperBound - oobb.lowerBound);
	}
	else {
		// Second step is to rotate the soup about its center to axis align it and
		// grab its AABB.
		b2Rot rotation(-getAngle());
		b2Transform t(oobb.GetCenter(), rotation);

		b2Vec2 pp = CoreMath::mRotateAboutArbitraryPoint(t, pointSoup[0]);
		oobb.lowerBound.Set(pp.x, pp.y);
		oobb.upperBound.Set(pp.x, pp.y);
		for (const auto& p : pointSoup) {
			pp = CoreMath::mRotateAboutArbitraryPoint(t, p);
			oobb.lowerBound.x = pp.x < oobb.lowerBound.x ? pp.x : oobb.lowerBound.x;
			oobb.lowerBound.y = pp.y < oobb.lowerBound.y ? pp.y : oobb.lowerBound.y;
			oobb.upperBound.x = pp.x > oobb.upperBound.x ? pp.x : oobb.upperBound.x;
			oobb.upperBound.y = pp.y > oobb.upperBound.y ? pp.y : oobb.upperBound.y;
		}

		// We now have the updated AABB.  This is what the SceneObject needs, so 
		// pass it in.
		updateLocalExtents(&oobb);
		setSize(oobb.upperBound - oobb.lowerBound);

		// Third step is to complete calculation of the OOBB by aligning it back to 
		// the spine object.
		t.q.Set(getAngle());
		oobb.lowerBound = CoreMath::mRotateAboutArbitraryPoint(t, oobb.lowerBound);
		oobb.upperBound = CoreMath::mRotateAboutArbitraryPoint(t, oobb.upperBound);
	}

	// Reposition the SceneObject so that it is centered over the spine object.
	Vector2 actualCenter = oobb.GetCenter();
	Vector2 sceneObjectCenter = getPosition();

	Vector2 delta = actualCenter - sceneObjectCenter;

	// Nullify the reposition of the SceneObject by incorporating its inverse 
	// into the spine object's position.
	mAutoCenterOffset.x += delta.x;
	mAutoCenterOffset.y += delta.y;

	// Handle special case of the spine object being 'flipped' in either axis.  When
	// that happens, we want to keep the location of the root bone at roughly the
	// same position on screen as it was prior to the flip.  This helps to reduce 
	// a jarring reposition of the root bone.
	bool doFlipRecurse = false;

	if (mPriorFlipX == mFlipX) {
		mPriorRootBoneWorldX = mSkeleton->root->worldX;
	}
	else {
		// Compensate for the flip in X (about the y axis).  This is an approximation
		// that will fail if the velocity of the spine object is large.  If that 
		// becomes a problem, incorporate the velocity delta into the 'prior world x'
		// to have the compensated position reflect the underlying movement.
		mPriorFlipX = mFlipX;
		doFlipRecurse = true;

		delta.x += mPriorRootBoneWorldX - mSkeleton->root->worldX;
	}
	if (mPriorFlipY == mFlipY) {
		mPriorRootBoneWorldY = mSkeleton->root->worldY;
	}
	else {
		mPriorFlipY = mFlipY;
		doFlipRecurse = true;

		delta.y += mPriorRootBoneWorldY - mSkeleton->root->worldY;
	}

	setPosition(sceneObjectCenter + delta);

	if (doFlipRecurse) {
		// Trash the current render data and recalculate it.  This avoids a flash
		// artifact due to possibly large changes coming from the flip.
		this->clearSprites();
		updateSpine(mLastFrameTime);
		prepareSpineForRender();
	}
}

//-----------------------------------------------------------------------------

const SpineCollisionProxy* SpineObject::getCollisionProxy(
	const char* anAttachmentName,
	const char* aSlotName,
	const char* aSkinName,
	const F32 sizerWidth,
	const F32 sizerHeight,
	const char* anObjectName) {

	// First, locate the requested attachment.
	spAttachment* pAttachment = NULL;
	auto attachmentName = StringTable->insert(anAttachmentName, true);

	auto slotName = StringTable->insert(aSlotName, true);
	auto pSlotData = spSkeletonData_findSlot(mSkeleton->data, slotName);
	if (!pSlotData) {
		Con::warnf("SpineObject::getCollisionProxy - Slot not found: '%s'.", slotName);
		return NULL;
	}

	auto skinName = StringTable->insert(aSkinName, true);
	if (skinName == StringTable->EmptyString || dStricmp(skinName, "default") == 0) {
		// Look in the currently active skin and then the default skin if needed.
		pAttachment = spSkeleton_getAttachmentForSlotIndex(mSkeleton.get(), pSlotData->index, attachmentName);
		if (!pAttachment) {
			Con::warnf("SpineObject::getCollisionProxy - Attachment not found: '%s'.", attachmentName);
			return NULL;
		}
	}
	else {
		// Attachment must exist in the skin name given or else it's an error.
		auto pSkin = spSkeletonData_findSkin(mSkeleton->data, skinName);
		if (!pSkin) {
			Con::warnf("SpineObject::getCollisionProxy - Skin not found: '%s'.", skinName);
			return NULL;
		}

		pAttachment = spSkin_getAttachment(pSkin, pSlotData->index, attachmentName);
		if (!pAttachment) {
			Con::warnf("SpineObject::getCollisionProxy - Attachment not found in requested skin.\nskin: '%s'.  attachment: '%s'.", skinName, attachmentName);
			return NULL;
		}
	}

	// Check that the attachment is a supported type.
	if (pAttachment->type != spAttachmentType::SP_ATTACHMENT_REGION && pAttachment->type != spAttachmentType::SP_ATTACHMENT_MESH) {
		Con::warnf("SpineObject::getCollisionProxy - Attachment requested is of an unsupported type.  Only Region and Mesh attachments are supported.");
		return NULL;
	}

	// objectName is not considered here.  Multiple proxies for the same attachment is 
	// not supported.
	auto itr = mCollisionProxies.find(pAttachment);
	if (itr != mCollisionProxies.end()) {
		return itr->value;
	}

	auto pBone = spSkeleton_findBone(mSkeleton.get(), pSlotData->boneData->name);
	if (!pBone) {
		Con::warnf("SpineObject::getCollisionProxy - Unable to find bone required by slot data: '%s'.", pSlotData->boneData->name);
		return NULL;
	}

	// Create new proxy object.
	const char *objectName = anObjectName ? StringTable->insert(anObjectName, true) : NULL;
	auto pProxy = new SpineCollisionProxy(attachmentName, slotName, skinName, sizerWidth, sizerHeight, objectName);

	// Add to the internal collection
	mCollisionProxies.insert(pAttachment, pProxy);

	// Initialize the proxy
	F32 width, height = 0.0f;
	if (pAttachment->type == SP_ATTACHMENT_REGION) {
		auto regionAttachment = (spRegionAttachment*)pAttachment;

		width = regionAttachment->width * regionAttachment->scaleX * spBone_getWorldScaleX(pBone) * sizerWidth;
		height = regionAttachment->height * regionAttachment->scaleY * spBone_getWorldScaleY(pBone) * sizerHeight;

		pProxy->mRotation = regionAttachment->rotation;
	}
	else {
		auto meshAttachment = (spMeshAttachment*)pAttachment;

		width = meshAttachment->width * spBone_getWorldScaleX(pBone) * sizerWidth;
		height = meshAttachment->height * spBone_getWorldScaleY(pBone) * sizerHeight;

		auto currentRegion = (spAtlasRegion *)meshAttachment->rendererObject;
		pProxy->mRotation = currentRegion->rotate ? 90.0f : 0.0f;
	}

	// Set box location to a galaxy far far away... 
	// NOTE: The box is auto-centered over the attachment at render.  
	pProxy->setPosition(Vector2(5000, CoreMath::mGetRandomF(-5000.0, 5000.0)));

	// Set layer and group membership to that of the owning spine object.
	pProxy->setSceneGroup(getSceneGroup());
	pProxy->setSceneLayer(getSceneLayer());

	// Register the proxy object.  This adds it to the runtime environment.
	if (objectName) {
		if (!pProxy->registerObject(objectName)) {
			return NULL;
		}
	}
	else {
		if (!pProxy->registerObject()) {
			return NULL;
		}
	}

	// Create collision fixture and enable collision callback support.
	auto fixtureIndex = pProxy->createPolygonBoxCollisionShape(width * abs(mSkeleton->scaleX), height * abs(mSkeleton->scaleY));
	pProxy->setCollisionShapeIsSensor(fixtureIndex, true);
	pProxy->setCollisionCallback(true);

	pProxy->setSleepingAllowed(false);

	// Disable collision box from colliding against other collision boxes in the spine object's scene group.
	// The intent is to stop a character's own collision boxes from setting each other off.  Different characters
	// should be in different groups.  That's the idea.  Maybe, should just let all the contacts go through and let
	// them be handled/ignored on the script side.
	auto groupMask = pProxy->getCollisionGroupMask();
	groupMask &= ~(1 << getSceneGroup()); // Clears bit in mask corresponding to the spine object's group.
	pProxy->setCollisionGroupMask(groupMask);

	// Deactivate.  If should be active, it will be made so at render time.
	pProxy->deActivate();

	// If Spine object is already in the scene, add the proxy too.
	if (getScene()) {
		getScene()->addToScene(pProxy);
	}

	return pProxy;
}

//-----------------------------------------------------------------------------

bool SpineObject::deleteCollisionProxy(const char *proxyId) {

	AssertFatal(proxyId != NULL, "SpineObject::deleteCollisionProxy() - Recieved NULL collision proxy object id.");

	SimObjectId idToDelete = dAtoi(proxyId);

	// Locate the requested object
	for (const auto& i : mCollisionProxies) {
		if (i.value->getId() == idToDelete) {
			// Delete it
			getScene()->removeFromScene(i.value);
			i.value->deleteObject();

			mCollisionProxies.erase(i.key);
			return true;
		}
	}

	Con::warnf("SpineObject::deleteCollisionProxy() - Unable to locate proxy with given id: '%s'.", proxyId);
	return false;
}

//-----------------------------------------------------------------------------

bool SpineObject::writeAnimationData(void) const {
	// According to the API, a track entry might be null.  So we need to make sure there is at least
	// one valid entry before returning true.
	for (auto i = 0; i < mAnimationState->tracksCount; ++i) {
		if (mAnimationState->tracks[i] != NULL) {
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------

const char *SpineObject::getAnimationData(void) const {
	string result;
	
	for (auto i = 0; i < mAnimationState->tracksCount; ++i) {
		spTrackEntry *track = mAnimationState->tracks[i];
		if (track) {
			// Encode the track entry.
			result
				.append(track->animation->name).append(";")
				.append(Con::getIntArg(track->trackIndex)).append(";")
				.append(Con::getBoolArg(track->loop)).append(";")
				.append(Con::getFloatArg(track->mixDuration)).append(";~");
		}
	}

	// Could throw it in the StringTable, but don't want to pollute that with this crud.
	unique_ptr<char> retVal = unique_ptr<char>(new char[result.size() + 1]);
	dStrcpy(retVal.get(), result.c_str());

	return retVal.get();
}

//-----------------------------------------------------------------------------

void SpineObject::setAnimationData(const char *animationData) {
	if (!animationData) {
		Con::warnf("SpineObject::setAnimationData() - Ignoring empty animation data string.");
		return;
	}

	// Break into list of entries.
	vector<char *> entries;
	char *entry = dStrtok(const_cast<char *>(animationData), "~");
	while (entry) {
		entries.push_back(entry);
		entry = dStrtok(NULL, "~");
	}

	// Process each entry
	for (auto entry : entries) {
		const char *name = dStrtok(entry, ";");
		int track = dAtoi(dStrtok(NULL, ";"));
		bool loop = dAtob(dStrtok(NULL, ";"));
		F32 mixDuration = dAtof(dStrtok(NULL, ";"));

		setAnimation(name, track, loop, mixDuration);
	}
}

//-----------------------------------------------------------------------------

bool SpineObject::writeCollisionData(void) const {
	return mCollisionProxies.size() > 0;
}

//-----------------------------------------------------------------------------

const char *SpineObject::getCollisionData(void) const {
	string result;

	for (auto pair : mCollisionProxies) {
		auto proxy = pair.value;

		// Encode the proxy.
		result
			.append(proxy->mAttachmentName).append(";")
			.append(proxy->mSlotName).append(";")
			.append(proxy->mSkinName).append(";")
			.append(Con::getFloatArg(proxy->mWidthSizer)).append(";")
			.append(Con::getFloatArg(proxy->mHeightSizer)).append(";");

		if (proxy->mObjectName) {
			result.append(proxy->mObjectName).append(";~");
		}
		else {
			result.append("~");
		}
	}

	unique_ptr<char> retVal = unique_ptr<char>(new char[result.size() + 1]);
	dStrcpy(retVal.get(), result.c_str());

	return retVal.get();
}

//-----------------------------------------------------------------------------

void SpineObject::setCollisionData(const char *collisionData) {
	if (!collisionData) {
		Con::warnf("SpineObject::setCollisionData() - Ignoring empty collision setup string.");
		return;
	}

	// Break into list of entries.
	vector<char *> entries;
	char *entry = dStrtok(const_cast<char *>(collisionData), "~");
	while (entry) {
		entries.push_back(entry);
		entry = dStrtok(NULL, "~");
	}
	
	// Process each entry
	for (auto entry : entries) {
		const char *attachmentName = dStrtok(entry, ";");
		const char *slotName = dStrtok(NULL, ";");
		const char *skinName = dStrtok(NULL, ";");
		F32 wSizer = dAtof(dStrtok(NULL, ";"));
		F32 hSizer = dAtof(dStrtok(NULL, ";"));
		const char *objectName = dStrtok(NULL, ";");

		getCollisionProxy(attachmentName, slotName, skinName, wSizer, hSizer, objectName);
	}
}

//-----------------------------------------------------------------------------

void SpineObject::OnRegisterScene(Scene *scene) {
	Parent::OnRegisterScene(scene);

	for (auto i : mCollisionProxies) {
		scene->addToScene(i.value);
	}
}

//-----------------------------------------------------------------------------

void SpineObject::OnUnregisterScene(Scene *scene) {
	Parent::OnUnregisterScene(scene);

	for (auto i : mCollisionProxies) {
		if(i.value->getScene())
			i.value->getScene()->removeFromScene(i.value);
	}
}