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

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _GBITMAP_H_
#include "graphics/gBitmap.h"
#endif

#ifndef _UTILITY_H_
#include "2d/core/Utility.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

#ifndef _SPINE_ASSET_H_
#include "2d/assets/SpineAsset.h"
#endif

// Script bindings.
#include "SpineAsset_ScriptBinding.h"

#include "spine/extension.h"

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SpineAsset);

//------------------------------------------------------------------------------

ConsoleType(spineAssetPtr, TypeSpineAssetPtr, sizeof(AssetPtr<SpineAsset>), ASSET_ID_FIELD_PREFIX)

//-----------------------------------------------------------------------------

ConsoleGetType(TypeSpineAssetPtr)
{
	// Fetch asset Id.
	return (*((AssetPtr<SpineAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType(TypeSpineAssetPtr)
{
	// Was a single argument specified?
	if (argc == 1)
	{
		// Yes, so fetch field value.
		const char* pFieldValue = argv[0];

		// Fetch asset pointer.
		AssetPtr<SpineAsset>* pAssetPtr = dynamic_cast<AssetPtr<SpineAsset>*>((AssetPtrBase*)(dptr));

		// Is the asset pointer the correct type?
		if (pAssetPtr == NULL)
		{
			// No, so fail.
			Con::warnf("(TypeSpineAssetPtr) - Failed to set asset Id '%d'.", pFieldValue);
			return;
		}

		// Set asset.
		pAssetPtr->setAssetId(pFieldValue);

		return;
	}

	// Warn.
	Con::warnf("(TypeSpineAssetPtr) - Cannot set multiple args to a single asset.");
}


//-----------------------------------------------------------------------------
// 
// The following three methods are hooks required to be defined by the spine runtime.
//
//-----------------------------------------------------------------------------
void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) {

	// Allocate a new ImageAsset. 
	ImageAsset* pImageAsset = new ImageAsset();

	SpineAsset *pSpine = (SpineAsset *)self->atlas->rendererObject;
	const char* imageFilePath = pSpine->expandAssetFilePath(self->name);

	// Point to the raw file (png or jpg)
	pImageAsset->setImageFile(imageFilePath);

	// Enable Explicit Mode so we can use region coordinates
	pImageAsset->setExplicitMode(true);

	// Add it to the AssetDatabase, making it accessible everywhere
	pSpine->mImageAsset = AssetDatabase.addPrivateAsset(pImageAsset);

	// Attach texture info for the page.
	self->rendererObject = pSpine->mImageAsset;
	self->width = pImageAsset->getImageWidth();
	self->height = pImageAsset->getImageHeight();
}

void _spAtlasPage_disposeTexture(spAtlasPage* self) {
}

char* _spUtil_readFile(const char* path, int* length) {
	return _spReadFile(path, length);
}

//------------------------------------------------------------------------------

SpineAsset::SpineAsset() : mSpineFile(StringTable->EmptyString),
mAtlasFile(StringTable->EmptyString),
mAtlasDirty(true),
mAtlas(NULL),
mSkeletonData(NULL),
mAnimationStateData(NULL),
mPreMultipliedAlpha(false)

{
}

//------------------------------------------------------------------------------

SpineAsset::~SpineAsset()
{
	spAnimationStateData_dispose(mAnimationStateData);
	spSkeletonData_dispose(mSkeletonData);
	spAtlas_dispose(mAtlas);
}

//------------------------------------------------------------------------------

void SpineAsset::initPersistFields()
{
	// Call parent.
	Parent::initPersistFields();

	// Fields.
	addProtectedField("AtlasFile", TypeAssetLooseFilePath, Offset(mAtlasFile, SpineAsset), &setAtlasFile, &defaultProtectedGetFn, &writeAtlasFile, "The loose file pointing to the .atlas file used for skinning");
	addProtectedField("SpineFile", TypeAssetLooseFilePath, Offset(mSpineFile, SpineAsset), &setSpineFile, &defaultProtectedGetFn, &writeSpineFile, "The loose file produced by the editor, which is fed into this asset");
	addProtectedField("PreMultipliedAlpha", TypeBool, Offset(mPreMultipliedAlpha, SpineAsset), &setPreMultipliedAlpha, &defaultProtectedGetFn, &writePreMultipliedAlpha, "Whether texture is built with pre-multiplied alpha values.");
}

//------------------------------------------------------------------------------

bool SpineAsset::onAdd()
{
	// Call Parent.
	if (!Parent::onAdd())
		return false;

	return true;
}

//------------------------------------------------------------------------------

void SpineAsset::onRemove()
{
	// Call Parent.
	Parent::onRemove();
}

//------------------------------------------------------------------------------

void SpineAsset::setSpineFile(const char* pSpineFile)
{
	// Sanity!
	AssertFatal(pSpineFile != NULL, "Cannot use a NULL spine file.");

	// Fetch spine file.
	pSpineFile = StringTable->insert(pSpineFile);

	// Ignore no change.
	if (pSpineFile == mSpineFile)
		return;

	// Update.
	mSpineFile = getOwned() ? expandAssetFilePath(pSpineFile) : StringTable->insert(pSpineFile);

	// Refresh the asset.
	refreshAsset();
}

//------------------------------------------------------------------------------

void SpineAsset::setAtlasFile(const char* pAtlasFile)
{
	// Sanity!
	AssertFatal(pAtlasFile != NULL, "Cannot use a NULL atlas file.");

	// Fetch atlas file.
	pAtlasFile = StringTable->insert(pAtlasFile);

	// Ignore no change.
	if (pAtlasFile == mAtlasFile)
		return;

	// Update.
	mAtlasFile = getOwned() ? expandAssetFilePath(pAtlasFile) : StringTable->insert(pAtlasFile);
	mAtlasDirty = true;

	// Refresh the asset.
	refreshAsset();
}

//------------------------------------------------------------------------------

void SpineAsset::copyTo(SimObject* object)
{
	// Call to parent.
	Parent::copyTo(object);

	// Cast to asset.
	SpineAsset* pAsset = static_cast<SpineAsset*>(object);

	// Sanity!
	AssertFatal(pAsset != NULL, "SpineAsset::copyTo() - Object is not the correct type.");

	// Copy state.
	pAsset->setAtlasFile(getAtlasFile());
	pAsset->setSpineFile(getSpineFile());
}

//------------------------------------------------------------------------------

void SpineAsset::initializeAsset(void)
{
	// Call parent.
	Parent::initializeAsset();

	// Ensure the spine file is expanded.
	mSpineFile = expandAssetFilePath(mSpineFile);

	// Ensure the spine file is expanded.
	mAtlasFile = expandAssetFilePath(mAtlasFile);

	// Build the atlas data
	if (mAtlasDirty)
		buildAtlasData();

	// Build the spine data
	buildSpineData();
}

//------------------------------------------------------------------------------

void SpineAsset::onAssetRefresh(void)
{
	// Ignore if not yet added to the sim.
	if (!isProperlyAdded())
		return;

	// Call parent.
	Parent::onAssetRefresh();

	// Reset any states or data
	if (mAtlasDirty)
		buildAtlasData();

	buildSpineData();
}

//-----------------------------------------------------------------------------

void SpineAsset::buildAtlasData(void)
{
	// If the atlas data was previously created, need to release it
	if (mAtlas)
		spAtlas_dispose(mAtlas);

	// If we are using a .atlas file
	if (mAtlasFile != StringTable->EmptyString)
		mAtlas = spAtlas_createFromFile(mAtlasFile, this);

	// Atlas load failure
	AssertFatal(mAtlas != NULL, "SpineAsset::buildAtlasData() - Atlas was not loaded.");

	spAtlasPage* currentPage = mAtlas->pages;

	while (currentPage != NULL)
	{
		ImageAsset* pImageAsset = (ImageAsset *)currentPage->rendererObject;

		spAtlasRegion* currentRegion = mAtlas->regions;

		// Loop through the Atlas information to create cell regions
		while (currentRegion != NULL)
		{
			if (currentRegion->rotate) {
				pImageAsset->addExplicitCell(currentRegion->x, currentRegion->y, currentRegion->height, currentRegion->width, currentRegion->name);
			}
			else {
				pImageAsset->addExplicitCell(currentRegion->x, currentRegion->y, currentRegion->width, currentRegion->height, currentRegion->name);
			}

			currentRegion = currentRegion->next;
		}

		mImageAsset->forceCalculation();
		currentPage = currentPage->next;
	}

	mAtlasDirty = false;
}

//-----------------------------------------------------------------------------

void SpineAsset::buildSpineData(void)
{
	// Atlas load failure
	AssertFatal(mAtlas != NULL, "SpineAsset::buildSpineData() - Atlas was not loaded.");

	// Clear state data
	if (mAnimationStateData)
		spAnimationStateData_dispose(mAnimationStateData);

	// Clear skeleton data
	if (mSkeletonData)
		spSkeletonData_dispose(mSkeletonData);

	// Determine if we have a json or binary file to process.
	AssertFatal(mSpineFile != NULL, "SpineAsset::buildSpineData() - Spine data file name is not defined.");

	if (Platform::hasExtension(mSpineFile, "skel")) {
		spSkeletonBinary* skel = spSkeletonBinary_create(mAtlas);
		skel->scale = 0.01f; // Adapt to box2d coordinate size - where 1 unit = 1 meter.
		mSkeletonData = spSkeletonBinary_readSkeletonDataFile(skel, mSpineFile);
		spSkeletonBinary_dispose(skel);
	}
	else if (Platform::hasExtension(mSpineFile, "json")) {
		spSkeletonJson* json = spSkeletonJson_create(mAtlas);
		json->scale = 0.01f;
		mSkeletonData = spSkeletonJson_readSkeletonDataFile(json, mSpineFile);
		spSkeletonJson_dispose(json);
	}
	else {
		AssertFatal(mSkeletonData != NULL,
			"SpineAsset::buildSpineData: Unrecognized spine data file extension recieved.  Expecting either '.json' or '.skel'.");
	}

	if (!mSkeletonData)
	{
		spAtlas_dispose(mAtlas);
		mAtlas = 0;

		// Report json->error message
		AssertFatal(mSkeletonData != NULL, "SpineAsset::buildSpineData() - Spine data was not valid.");
	}

	mAnimationStateData = spAnimationStateData_create(mSkeletonData);
}

//-----------------------------------------------------------------------------

bool SpineAsset::isAssetValid(void) const
{
	return ((mAtlas != NULL) && (mSkeletonData != NULL) && (mAnimationStateData != NULL) && mImageAsset.notNull());
}

//-----------------------------------------------------------------------------

void SpineAsset::onTamlPreWrite(void)
{
	// Call parent.
	Parent::onTamlPreWrite();

	// Ensure the spine file is collapsed.
	mSpineFile = collapseAssetFilePath(mSpineFile);

	// Ensure the atlas file is collapsed.
	mAtlasFile = collapseAssetFilePath(mAtlasFile);
}

//-----------------------------------------------------------------------------

void SpineAsset::onTamlPostWrite(void)
{
	// Call parent.
	Parent::onTamlPostWrite();

	// Ensure the spine file is expanded.
	mSpineFile = expandAssetFilePath(mSpineFile);

	// Ensure the atlas file is expanded.
	mAtlasFile = expandAssetFilePath(mAtlasFile);
}

//------------------------------------------------------------------------------

void SpineAsset::onTamlCustomWrite(TamlCustomNodes& customNodes)
{
	// Debug Profiling.
	PROFILE_SCOPE(SpineAsset_OnTamlCustomWrite);

	// Call parent.
	Parent::onTamlCustomWrite(customNodes);
}

//-----------------------------------------------------------------------------

void SpineAsset::onTamlCustomRead(const TamlCustomNodes& customNodes)
{
	// Debug Profiling.
	PROFILE_SCOPE(SpineAsset_OnTamlCustomRead);

	// Call parent.
	Parent::onTamlCustomRead(customNodes);
}
