#include "TmxMapSprite.h"

#include "assets/assetManager.h"
#include <string>

//script bindings
#include "TmxMapSprite_ScriptBinding.h"



//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(TmxMapSprite);

//------------------------------------------------------------------------------

TmxMapSprite::TmxMapSprite() : mMapPixelToMeterFactor(0.03f),
	mLastTileAsset(StringTable->EmptyString),
	mLastTileImage(StringTable->EmptyString)
{
	mAutoSizing = true;
	setBodyType(b2_staticBody);
}

//------------------------------------------------------------------------------

TmxMapSprite::~TmxMapSprite()
{
	ClearMap();
}

void TmxMapSprite::initPersistFields()
{
	// Call parent.
	Parent::initPersistFields();

	addProtectedField("Map", TypeTmxMapAssetPtr, Offset(mMapAsset, TmxMapSprite), &setMap, &getMap, &writeMap, "");
	addProtectedField("MapToMeterFactor", TypeF32, Offset(mMapPixelToMeterFactor, TmxMapSprite), &setMapToMeterFactor, &getMapToMeterFactor, &writeMapToMeterFactor, "");
}

bool TmxMapSprite::onAdd()
{
	auto layerIdx = mLayers.begin();
	for (layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{

		(*layerIdx)->onAdd();
	}
	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
		(*objectsIdx)->onAdd();
	}
	return (Parent::onAdd());

}

void TmxMapSprite::onRemove()
{
	auto layerIdx = mLayers.begin();
	for (layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{

	//	(*layerIdx)->onRemove();
	}
	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
	//	(*objectsIdx)->onRemove();
	}
	Parent::onRemove();
}

void TmxMapSprite::OnRegisterScene(Scene* pScene)
{

	Parent::OnRegisterScene(pScene);
	auto layerIdx = mLayers.begin();
	for(layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{

		pScene->addToScene(*layerIdx);
	}
	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
		pScene->addToScene(*objectsIdx);
	}

}

void TmxMapSprite::OnUnregisterScene( Scene* pScene )
{
	Parent::OnUnregisterScene(pScene);

	auto layerIdx = mLayers.begin();
	for(layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{
		//(*layerIdx)->OnUnregisterScene(pScene);
	}
	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
		//(*objectsIdx)->OnUnregisterScene(pScene);
	}
}

void TmxMapSprite::setPosition( const Vector2& position )
{
	Parent::setPosition(position);
	auto layerIdx = mLayers.begin();
	for(layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{
		(*layerIdx)->setPosition(position);
	}

}

void TmxMapSprite::ClearMap()
{
	auto layerIdx = mLayers.begin();
	for(layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{
		//CompositeSprite* sprite = *layerIdx;
		//delete sprite;
	}
	mLayers.clear();
	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
		SceneObject* object = *objectsIdx;
		delete object;
	}
	mObjects.clear();
}
void TmxMapSprite::BuildMap()
{
	// Debug Profiling.
	PROFILE_SCOPE(TmxMapSprite_BuildMap);


	ClearMap();
	auto mapParser = mMapAsset->getParser();

	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	F32 halfTileHeight = static_cast<F32>(tileHeight * 0.5);

	F32 width = (mapParser->GetWidth() * tileWidth);
	F32 height = (mapParser->GetHeight() * tileHeight);

	F32 originY = height / 2 - halfTileHeight;
	F32 originX = 0;

	Vector2 tileSize(tileWidth, tileHeight);
	Vector2 originSize(originX, originY);

	Tmx::MapOrientation orient = mapParser->GetOrientation();

	auto layerItr = mapParser->GetLayers().begin();
	for(layerItr; layerItr != mapParser->GetLayers().end(); ++layerItr)
	{
		Tmx::Layer* layer = *layerItr;

		//use default layer number, unless someone realy wants to override it.
		S32 layerNumber = 31 - layer->GetZOrder();
		if (layer->GetProperties().HasProperty(TMX_MAP_LAYER_ID_PROP))
			layerNumber = layer->GetProperties().GetNumericProperty(TMX_MAP_LAYER_ID_PROP);

		auto compSprite = CreateLayer(layerNumber, orient == Tmx::TMX_MO_ISOMETRIC);

		U32 xTiles = mapParser->GetWidth();
		U32 yTiles = mapParser->GetHeight();
		for(U32 x=0; x < xTiles; ++x)
		{
			for (U32 y=0; y < yTiles; ++y)
			{
				const Tmx::MapTile& tile = layer->GetTile(x, y);
				if (tile.tilesetId == TMX_MAP_NO_TILE) continue; //no tile at this location

				const Tmx::Tileset* tset = mapParser->GetTileset(tile.tilesetId);

				StringTableEntry assetName = GetTilesetAsset(tset);
				if (assetName == StringTable->EmptyString) continue;

				U32 localFrame = tile.id;
				F32 spriteHeight = static_cast<F32>( tset->GetTileHeight() );
				F32 spriteWidth = static_cast<F32>( tset->GetTileWidth() );

				//F32 heightOffset = (spriteHeight - tileHeight) / 2;
				//F32 widthOffset = (spriteWidth - tileWidth) / 2;
				auto thisTilePosition = Vector2(static_cast<F32>(x), static_cast<F32>(yTiles - y));
				auto bId = compSprite->addSprite( SpriteBatchItem::LogicalPosition( thisTilePosition.scriptThis()) );
				compSprite->selectSpriteId(bId);
				compSprite->setSpriteImage(assetName, localFrame);
				auto thisTileSize = Vector2(spriteWidth * mMapPixelToMeterFactor, spriteHeight * mMapPixelToMeterFactor);
				compSprite->setSpriteSize(  thisTileSize);

				compSprite->setSpriteFlipX(tile.flippedHorizontally);
				compSprite->setSpriteFlipY(tile.flippedVertically);
				const Tmx::Tile* tileobj= tset->GetTile(tile.id);
				if (tileobj) {
					if (tileobj->GetObjectGroup().GetNumObjects() > 0) {
						auto thisTileAbsolutePosition = Vector2(thisTilePosition.x * spriteWidth, (yTiles * spriteHeight) - (thisTilePosition.y * spriteHeight));
						addObjectGroup(&tset->GetTile(tile.id)->GetObjectGroup(), &thisTileAbsolutePosition);
					}
				}

			}
		}

	}

	//now do object groups...
	auto groupIdx = mapParser->GetObjectGroups().begin();

	for(groupIdx; groupIdx != mapParser->GetObjectGroups().end(); ++groupIdx)
	{
		auto groupLayer = *groupIdx;
		addObjectGroup(groupLayer, NULL);

	}
}

void TmxMapSprite::addObjectGroup(const Tmx::ObjectGroup* objectGroup, const Vector2* offset ) {
	auto mapParser = mMapAsset->getParser();
	Tmx::MapOrientation orient = mapParser->GetOrientation();
	//use default layer number, unless someone realy wants to override it.
	S32 layerNumber = 31 - objectGroup->GetZOrder();
	if (objectGroup->GetProperties().HasProperty(TMX_MAP_LAYER_ID_PROP))
		layerNumber = objectGroup->GetProperties().GetNumericProperty(TMX_MAP_LAYER_ID_PROP);

	auto compSprite = CreateLayer(layerNumber, orient == Tmx::TMX_MO_ISOMETRIC);

	auto objectIdx = objectGroup->GetObjects().begin();
	for (objectIdx; objectIdx != objectGroup->GetObjects().end(); ++objectIdx)
	{
		auto object = *objectIdx;
		//do a number of things.
		//try it as a script reference
		if (object->GetType() == TMX_MAP_SCRIPT_OBJECT || objectGroup->GetName() == TMX_MAP_SCRIPT_OBJECT) {
			addScriptObject(object, layerNumber, offset);
			continue; //don't allow script refs to have a sprite or physics presence...
		}
		//try it as a tile
		auto gid = object->GetGid();
		auto tileSet = mapParser->FindTileset(gid);
		if (tileSet != NULL) {
			addObjectAsSprite(tileSet, object, mapParser, gid, compSprite, offset);
			continue;
		}
		//try it as a physics object.
		if (object->GetName() == TMX_MAP_COLLISION_OBJECT || object->GetType() == TMX_MAP_COLLISION_OBJECT || objectGroup->GetName() == TMX_MAP_COLLISION_OBJECT) {
			//try to add some physics bodies...

			if (object->GetPolyline() != NULL) {
				addPhysicsPolyLine(object, compSprite, offset);
			}
			else if (object->GetPolygon() != NULL) {
				addPhysicsPolygon(object, compSprite, offset);
			}
			else if (object->GetEllipse() != NULL) {
				addPhysicsEllipse(object, compSprite, offset);
			}
			else {
				//must be a rectangle. 
				addPhysicsRectangle(object, compSprite, offset);
			}
			continue;
		}
	}
}

void TmxMapSprite::addScriptObject(const Tmx::Object* object, S32 layerNumber, const Vector2* offset) {
	std::string result;
	if (object->GetProperties().HasProperty(TMX_MAP_SCRIPT_FUNCTION)) {
		result = Con::evaluatef("%s();", object->GetProperties().GetLiteralProperty(TMX_MAP_SCRIPT_FUNCTION).c_str());
	}
	else if (object->GetName() == TMX_MAP_SCRIPT_FUNCTION) {
		result = Con::evaluatef("%s();", object->GetType().c_str());
	}
	else if (object->GetType() == TMX_MAP_SCRIPT_FUNCTION) {
		result = Con::evaluatef("%s();", object->GetName().c_str());
	}
	//Con::printf("Executed a script and got %s back", result.c_str());

	//set the object's layer...
	char layer[128];
	dItoa(layerNumber, layer);
	Con::evaluatef("%s.SceneLayer = %s;", result.c_str(), layer);
	//set the object's position...
	Vector2 pixLoc = Vector2(
		static_cast<F32>(object->GetX()),
		static_cast<F32>(object->GetY())
		);
	if (offset){
		pixLoc.x += offset->x;
		pixLoc.y += offset->y;
	}
	const char* loc = PixelToCoord(pixLoc).scriptThis();
	Con::evaluatef("%s.position = \"%s\";", result.c_str(), loc);

	//assign the rest of the properties from TMX
	std::map<std::string, std::string> list = object->GetProperties().GetList();
	for (auto iter = list.begin();
	iter != list.end();
		iter++) {
		if (
			iter->first != std::string(TMX_MAP_SCRIPT_FUNCTION)
			&&
			iter->second != std::string(TMX_MAP_SCRIPT_FUNCTION)
			) {
			//Con::printf("generating code to set property %s of %s to value %s", iter->first.c_str(), result.c_str(), iter->second.c_str());
			Con::evaluatef("%s.%s = %s;", result.c_str(), iter->first.c_str(), iter->second.c_str());
		}
	}
}

void TmxMapSprite::addObjectAsSprite(const Tmx::Tileset* tileSet, Tmx::Object* object, Tmx::Map * mapParser, U32 gid, CompositeSprite* compSprite, const Vector2* offset){

	F32 objectWidth = static_cast<F32>(object->GetWidth());
	F32 objectHeight = static_cast<F32>(object->GetHeight());
	Vector2 vTile =
		Vector2
		(
		static_cast<F32>(object->GetX()),
		static_cast<F32>(object->GetY())
		);

	Vector2 pos = PixelToCoord( vTile);

	if (offset) {
		pos.x += offset->x;
		pos.y += offset->y;
	}

	S32 frameNumber = gid - tileSet->GetFirstGid();
	StringTableEntry assetName = GetTilesetAsset(tileSet);

	auto bId = compSprite->addSprite( SpriteBatchItem::LogicalPosition( pos.scriptThis()) );
	compSprite->selectSpriteId(bId);
	compSprite->setSpriteImage(assetName, frameNumber);
	compSprite->setSpriteSize(Vector2( objectWidth * mMapPixelToMeterFactor, objectHeight * mMapPixelToMeterFactor));
	compSprite->setSpriteFlipX(false);
	compSprite->setSpriteFlipY(false);


}

void TmxMapSprite::addPhysicsPolyLine(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset){
    auto mapParser = mMapAsset->getParser();
    F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
    F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
    F32 mapHeight = (mapParser->GetHeight() * tileHeight);
    Vector2 tileSize(tileWidth, tileHeight);
    Tmx::MapOrientation orient = mapParser->GetOrientation();
    b2Vec2 origin = b2Vec2
    (
     static_cast<float32>(object->GetX()),
     static_cast<float32>(object->GetY())
     );
    if (offset){
        origin.x += offset->x;
        origin.y += offset->y;
    }
    
    
    //weird additions and subtractions in this area due to the fact that this engine uses bottom->left as origin, and TMX uses top->right.
    //it's hacky, but it works.
    Vector2 heightoffset = Vector2( origin.x, mapHeight-(origin.y));
    Vector2 tilecoord = CoordToTile(heightoffset, tileSize, orient == Tmx::TMX_MO_ISOMETRIC);
    b2Vec2 nativePoint = tilecoord;
    nativePoint += b2Vec2(-tileWidth/2,tileHeight/2);
   // nativePoint += b2Vec2(object->GetWidth()/2.0f, -(object->GetHeight()/2.0f)); //adjust for tmx defining from bottom left point while t2d defines from center...
   // nativePoint *= mMapPixelToMeterFactor;
    const Tmx::Polyline* line = object->GetPolyline();

	U32 points = line->GetNumPoints();
    if (points > 0){
        //find object height:
        F32 min = line->GetPoint(0).y;
        F32 max = line->GetPoint(0).y;
        for (U32 i = 0; i < points; i++){
            Tmx::Point point = line->GetPoint(i);
            if (point.y > max)
                max = point.y;
            if (point.y < min)
                min = point.y;
        }
            
        F32 polylineHeight = max - min;
        for (U32 i = 0; i < points-1; i++){

            Tmx::Point first = line->GetPoint(i);
            Tmx::Point second = line->GetPoint(i+1);
            
            Vector2 firstvec = Vector2(nativePoint.x + first.x, nativePoint.y - first.y);
            Vector2 secondvec = Vector2(nativePoint.x + second.x, nativePoint.y - second.y);

            compSprite->createEdgeCollisionShape(firstvec * mMapPixelToMeterFactor, secondvec * mMapPixelToMeterFactor, false, false, Vector2(), Vector2());

        }
    }

	

}

void TmxMapSprite::addPhysicsPolygon(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset){
	auto mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	F32 height = (mapParser->GetHeight() * tileHeight);
	Vector2 tileSize(tileWidth, tileHeight);
	Tmx::MapOrientation orient = mapParser->GetOrientation();
	const Tmx::Polygon* line = object->GetPolygon();
	int points = line->GetNumPoints();
	b2Vec2* pointsdata = new b2Vec2[points];
	b2Vec2 origin = b2Vec2
		(
			static_cast<float32>(object->GetX()), 
			static_cast<float32>(object->GetY())
		);
    if (offset){
        origin.x += offset->x;
        origin.y += offset->y;
    }
	for (int i = 0; i < points; i++)
	{
		Tmx::Point tmxPoint = line->GetPoint(i);

		//weird additions and subtractions in this area due to the fact that this engine uses bottom->left as origin, and TMX uses top->right. 
		//it's hacky, but it works.
        Vector2 isocoord = Vector2( tmxPoint.x + origin.x, height-(origin.y+tmxPoint.y));
		Vector2 tilecoord = CoordToTile(isocoord,tileSize,orient == Tmx::TMX_MO_ISOMETRIC);
		b2Vec2 nativePoint = tilecoord;
		nativePoint += b2Vec2(-tileWidth/2,tileHeight/2);
		nativePoint *= mMapPixelToMeterFactor;
		pointsdata[i] = nativePoint;	

	}
		compSprite->createPolygonCollisionShape(points, pointsdata);
		delete[] pointsdata;
}

void TmxMapSprite::addPhysicsEllipse(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset){
	auto mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	F32 mapHeight = (mapParser->GetHeight() * tileHeight);
	Vector2 tileSize(tileWidth, tileHeight);
	Tmx::MapOrientation orient = mapParser->GetOrientation();
	const Tmx::Ellipse* ellipse = object->GetEllipse();
	b2Vec2 origin = b2Vec2
		(
			static_cast<float32>(ellipse->GetCenterX()), 
			static_cast<float32>(ellipse->GetCenterY())
		);
    if (offset){
        origin.x += offset->x;
        origin.y += offset->y;
    }

	F32 ellipseHeight = static_cast<F32>(ellipse->GetRadiusY());
	F32 ellipseWidth = static_cast<F32>(ellipse->GetRadiusX());

	//tmx allows arbitrary ellipses, t2d only lets us use circles. approximate ellipses with a height:width ratio of 1/2 - 2with a circle, otherwise fail.
	F32 ratio = ellipseHeight/ellipseWidth;
	if (ratio < 0.5f || ratio > 2.0f){
		Con::warnf("TMX map has an ellipse collision body with H:W ratio outside of our approximatable bounds. Use a different shape.");
		return; 
	}


		//weird additions and subtractions in this area due to the fact that this engine uses bottom->left as origin, and TMX uses top->right. 
		//it's hacky, but it works.
        Vector2 originOffset = Vector2( origin.x, mapHeight-(origin.y));
		Vector2 tilecoord = CoordToTile(originOffset ,tileSize,orient == Tmx::TMX_MO_ISOMETRIC);
		b2Vec2 nativePoint = tilecoord;
		nativePoint += b2Vec2(-tileWidth/2,tileHeight/2);
		nativePoint *= mMapPixelToMeterFactor;
		compSprite->createCircleCollisionShape( (ellipseHeight > ellipseWidth ? ellipseHeight : ellipseWidth ) * mMapPixelToMeterFactor, nativePoint);


}

void TmxMapSprite::addPhysicsRectangle(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset){
	auto mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	F32 mapHeight = (mapParser->GetHeight() * tileHeight);
	Vector2 tileSize(tileWidth, tileHeight);
	Tmx::MapOrientation orient = mapParser->GetOrientation();
	b2Vec2 origin = b2Vec2
		(
			static_cast<float32>(object->GetX()), 
			static_cast<float32>(object->GetY())
		);
    if (offset){
        origin.x += offset->x;
        origin.y += offset->y;
    }


		//weird additions and subtractions in this area due to the fact that this engine uses bottom->left as origin, and TMX uses top->right. 
		//it's hacky, but it works.
        Vector2 heightoffset = Vector2( origin.x, mapHeight-(origin.y));
		Vector2 tilecoord = CoordToTile(heightoffset, tileSize, orient == Tmx::TMX_MO_ISOMETRIC);
		b2Vec2 nativePoint = tilecoord;
		nativePoint += b2Vec2(-tileWidth/2,tileHeight/2);
		nativePoint += b2Vec2(object->GetWidth()/2.0f, -(object->GetHeight()/2.0f)); //adjust for tmx defining from bottom left point while t2d defines from center...
		nativePoint *= mMapPixelToMeterFactor;
		compSprite->createPolygonBoxCollisionShape(object->GetWidth()*mMapPixelToMeterFactor, object->GetHeight()*mMapPixelToMeterFactor, nativePoint);
		

}

Vector2 TmxMapSprite::CoordToTile(Vector2& pos, Vector2& tileSize, bool isIso)
{

	if (isIso)
	{
		Vector2 newPos(
			pos.x / tileSize.y,
			pos.y / tileSize.y
			);

		return newPos;
	}
	else
	{
		return pos;
	}
}

Vector2 TmxMapSprite::PixelToCoord(Vector2& pixPos){
	auto mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	F32 mapHeight = (mapParser->GetHeight() * tileHeight);
	Vector2 tileSize(tileWidth, tileHeight);
	Tmx::MapOrientation orient = mapParser->GetOrientation();
    Vector2 heightOffset = Vector2(pixPos.x, mapHeight - (pixPos.y));
	Vector2 tilecoord = CoordToTile(heightOffset, tileSize, orient == Tmx::TMX_MO_ISOMETRIC);
	b2Vec2 nativePoint = tilecoord;
	nativePoint += b2Vec2(-tileWidth / 2, tileHeight / 2);
	nativePoint *= mMapPixelToMeterFactor;
		return nativePoint;
}

Vector2 TmxMapSprite::TileToCoord(Vector2& pos, Vector2& tileSize, Vector2& offset, bool isIso)
{
	if (isIso)
	{
		Vector2 newPos(
			static_cast<F32>( (pos.x - pos.y) * tileSize.y),
			static_cast<F32>( offset.y - (pos.x + pos.y) * tileSize.y * 0.5 )
			);

		return newPos;
	}
	else
	{
		Vector2 newpos(pos.x*tileSize.x*mMapPixelToMeterFactor, pos.y*tileSize.y*mMapPixelToMeterFactor);
		return newpos;
	}
}

CompositeSprite* TmxMapSprite::CreateLayer(U32 layerIndex, bool isIso)
{
	CompositeSprite* compSprite = new CompositeSprite();
	mLayers.push_back(compSprite);

	auto scene = this->getScene();
	if (scene)
		scene->addToScene(compSprite);

	if (isIso)
		compSprite->setBatchLayout( CompositeSprite::ISOMETRIC_LAYOUT );
	else
		compSprite->setBatchLayout(CompositeSprite::RECTILINEAR_LAYOUT);

	auto mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	compSprite->setDefaultSpriteSize(Vector2(tileWidth, tileHeight) * mMapPixelToMeterFactor);
	compSprite->setDefaultSpriteStride(Vector2(tileWidth, tileHeight) * mMapPixelToMeterFactor);


	compSprite->setPosition(getPosition());
	compSprite->setSceneLayer(layerIndex);
	compSprite->setBatchSortMode(SceneRenderQueue::RENDER_SORT_ZAXIS);
	compSprite->setBatchIsolated(true);
	compSprite->setBodyType(b2_staticBody);

	return compSprite;
}

const char* TmxMapSprite::getFileName(const char* path)
{
	auto pStr = dStrrchr(path, '\\');
	if (pStr == NULL)
		pStr = dStrrchr(path, '/');
	if (pStr == NULL)
		return NULL;

	pStr = pStr+1;

	auto pDot = dStrrchr(pStr, '.');
	U32 file_len = pDot - pStr;
	if (file_len >= 1024) return NULL;

	char buffer[1024];
	dStrncpy(buffer, pStr, file_len);
	buffer[file_len] = 0;
	return StringTable->insert(buffer);
}

StringTableEntry TmxMapSprite::GetTilesetAsset(const Tmx::Tileset* tileSet)
{
	StringTableEntry assetName = StringTable->insert( tileSet->GetProperties().GetLiteralProperty(TMX_MAP_TILESET_ASSETNAME_PROP).c_str() );
	if (assetName == StringTable->EmptyString)
	{
		//we fall back to using the image filename as an asset name query.
		//if that comes back with any hits, we use the first one.
		//If you don't want to name your asset the same as your art tile set name, 
		//you need to set a custom "AssetName" property on the tile set to override this.

		auto imageSource = tileSet->GetImage()->GetSource().c_str();
		StringTableEntry imageName = getFileName(imageSource);

		if (imageName == mLastTileImage)
		{
			//this is a quick memoize optimization to cut down on asset queries.
			//if we are requesting the same assetName from what we already found, we just return that.
			return mLastTileAsset;
		}

		mLastTileImage = imageName;

		AssetQuery query;
		S32 count = AssetDatabase.findAssetName(&query, imageName);
		if (count > 0)
		{
			assetName = query.first();
			mLastTileAsset = StringTable->insert( assetName );
		}
	}
	
	return assetName;
}

const char* TmxMapSprite::getTileProperty(StringTableEntry lName, StringTableEntry pName, U32 x,U32 y){
	//we'll need a parser and to iterate over the layers
	auto mapParser = mMapAsset->getParser();
	auto layerItr = mapParser->GetLayers().begin();
	for(layerItr; layerItr != mapParser->GetLayers().end(); ++layerItr)
	{
		Tmx::Layer* layer = *layerItr;

		//look for a layer with a name that matches lName
		if (std::string(lName) != layer->GetName())
			continue;
		//we found one, get the tile properties at the xy of that layer
		Tmx::MapTile tile = layer->GetTile(x,y);
		const Tmx::Tileset *tset = mapParser->GetTileset(tile.tilesetId);
		//make sure they're asking for valid x and y
		if (tset == NULL)
			return "";

		const Tmx::PropertySet pset = tset->GetTile(tile.id)->GetProperties();
		if (pset.HasProperty(pName)){

			//we have the result, put it in the string table and return it
			std::string presult = pset.GetLiteralProperty(pName);
			StringTableEntry s = StringTable->insert(presult.c_str());

			return s;
		}
		else
			return "";
	}

	//no layer or property of that name. 
	//return empty
	return "";

}

Vector2 TmxMapSprite::getTileSize(){
	Tmx::Map* mapParser = mMapAsset->getParser();
	F32 tileWidth = static_cast<F32>(mapParser->GetTileWidth());
	F32 tileHeight = static_cast<F32>(mapParser->GetTileHeight());
	return Vector2(tileWidth, tileHeight);

}

bool TmxMapSprite::isIsoMap(){
	Tmx::Map* mapParser = mMapAsset->getParser();
	Tmx::MapOrientation orient = mapParser->GetOrientation();
	return orient == Tmx::TMX_MO_ISOMETRIC;
}

void TmxMapSprite::setBodyType(const b2BodyType type){
	Parent::setBodyType(type);

	auto layerIdx = mLayers.begin();
	for(layerIdx; layerIdx != mLayers.end(); ++layerIdx)
	{

		(*layerIdx)->setBodyType(type);
	}

	auto objectsIdx = mObjects.begin();
	for (objectsIdx; objectsIdx != mObjects.end(); ++objectsIdx){
		(*objectsIdx)->setBodyType(type);
	}


}