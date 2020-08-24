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

#ifndef _SPINE_ASSET_H_
#define _SPINE_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

#ifndef SPINE_SPINE_H_
#include "spine/spine.h"
#endif

//-----------------------------------------------------------------------------

DefineConsoleType(TypeSpineAssetPtr)

//-----------------------------------------------------------------------------

class SpineAsset : public AssetBase
{
private:
	typedef AssetBase Parent;
	bool                            mAtlasDirty;
	bool							mPreMultipliedAlpha;

public:
	StringTableEntry                mSpineFile;
	StringTableEntry                mAtlasFile;
	AssetPtr<ImageAsset>            mImageAsset;
	spAtlas*                        mAtlas;
	spSkeletonData*                 mSkeletonData;
	spAnimationStateData*           mAnimationStateData;

public:
	SpineAsset();
	virtual ~SpineAsset();

	/// Core.
	static void initPersistFields();
	virtual bool onAdd();
	virtual void onRemove();
	virtual void copyTo(SimObject* object);

	void                    setSpineFile(const char* pSpineFile);
	inline StringTableEntry getSpineFile(void) const { return mSpineFile; }

	void                    setAtlasFile(const char* pAtlasFile);
	inline StringTableEntry getAtlasFile(void) const { return mAtlasFile; }

	virtual bool            isAssetValid(void) const;

	inline void				setPreMultipliedAlpha(const bool usePMA) { mPreMultipliedAlpha = usePMA; }
	inline bool				getPreMultipliedAlpha(void) const { return mPreMultipliedAlpha; }

	/// Declare Console Object.
	DECLARE_CONOBJECT(SpineAsset);

private:
	void buildAtlasData(void);
	void buildSpineData(void);

protected:
	virtual void initializeAsset(void);
	virtual void onAssetRefresh(void);

	/// Taml callbacks.
	virtual void onTamlPreWrite(void);
	virtual void onTamlPostWrite(void);
	virtual void onTamlCustomWrite(TamlCustomNodes& customNodes);
	virtual void onTamlCustomRead(const TamlCustomNodes& customNodes);


protected:
	static bool setSpineFile(void* obj, const char* data) { static_cast<SpineAsset*>(obj)->setSpineFile(data); return false; }
	static bool writeSpineFile(void* obj, StringTableEntry pFieldName) { return static_cast<SpineAsset*>(obj)->getSpineFile() != StringTable->EmptyString; }
	static bool setAtlasFile(void* obj, const char* data) { static_cast<SpineAsset*>(obj)->setAtlasFile(data); return false; }
	static bool writeAtlasFile(void* obj, StringTableEntry pFieldName) { return static_cast<SpineAsset*>(obj)->getAtlasFile() != StringTable->EmptyString; }
	static bool setPreMultipliedAlpha(void* obj, const char* data) { static_cast<SpineAsset*>(obj)->setPreMultipliedAlpha(dAtob(data)); return false; }
	static bool writePreMultipliedAlpha(void* obj, StringTableEntry pFieldName) { return static_cast<SpineAsset*>(obj)->getPreMultipliedAlpha(); }
};

#endif // _SPINE_ASSET_H_
