#ifndef _TMXMAP_SPRITE_H
#define _TMXMAP_SPRITE_H

#ifndef _COMPOSITE_SPRITE_H_
#include "2d/sceneobject/CompositeSprite.h"
#endif

#ifndef _TMXMAP_ASSET_H_
#include "2d/assets/TmxMapAsset.h"
#endif


#define TMX_MAP_TILESET_ASSETNAME_PROP "AssetName"
#define TMX_MAP_LAYER_ID_PROP "LayerId"
#define TMX_MAP_COLLISION_OBJECT "Collision"
#define TMX_MAP_SCRIPT_OBJECT "Script"
#define TMX_MAP_SCRIPT_FUNCTION "Function"
#define TMX_MAP_NO_TILE -1

class TmxMapSprite : public SceneObject
{
protected:
	typedef SceneObject Parent;

////////CORE///////////////////////////////
public:
	TmxMapSprite();
	virtual ~TmxMapSprite();

	static void initPersistFields();
	virtual bool            onAdd();
	virtual void            onRemove();
	virtual void OnRegisterScene(Scene* scene);
	virtual void OnUnregisterScene( Scene* pScene );
	virtual void            setPosition( const Vector2& position );

	/// Declare Console Object.
	DECLARE_CONOBJECT( TmxMapSprite );

//////////////////////////////////////////

private: //config

	AssetPtr<TmxMapAsset> mMapAsset;
	F32					  mMapPixelToMeterFactor;	//This translates TMX pixel/image coordinate space into T2D scene coordinate space. 
													//The default is to set every pixel equal to 0.03 meters (or about 33 pixels per meter)
													//This should match up with the rest of your asset design resolution.
	Vector<CompositeSprite*> mLayers;
	Vector<SceneObject*> mObjects;

	StringTableEntry  mLastTileAsset;
	StringTableEntry  mLastTileImage;

	void BuildMap();
	void ClearMap();
	CompositeSprite* CreateLayer(U32 layerIndex, bool isIso);
	const char* getFileName(const char* path);
	StringTableEntry GetTilesetAsset(const Tmx::Tileset* tileSet);
	void addObjectGroup(const Tmx::ObjectGroup* objectGroup, const Vector2* offset);
	void addScriptObject(const Tmx::Object* object, S32 layerNumber, const Vector2* offset);
	void addObjectAsSprite(const Tmx::Tileset* tileSet, Tmx::Object* object, Tmx::Map * mapParser, U32 gid, CompositeSprite* compSprite, const Vector2* offset);
	void addPhysicsPolyLine(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset);
	void addPhysicsPolygon(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset);
	void addPhysicsEllipse(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset);
	void addPhysicsRectangle(Tmx::Object* object, CompositeSprite* compSprite, const Vector2* offset);

public:
	inline bool setMap( const char* pMapAssetId ){ if (pMapAssetId == NULL) return false; mMapAsset = pMapAssetId; BuildMap(); return false;}
	inline StringTableEntry getMap( void ) const { return mMapAsset.getAssetId(); }
	inline bool setMapToMeterFactor( F32 factor ) {mMapPixelToMeterFactor = factor; BuildMap(); return false;}
	inline F32 getMapToMeterFactor( void ) const {return mMapPixelToMeterFactor;}
	const char* getTileProperty(StringTableEntry lName, StringTableEntry pName, U32 x,U32 y);
	Vector2 CoordToTile(Vector2& pos, Vector2& tileSize, bool isIso);
	Vector2 TileToCoord(Vector2& pos, Vector2& tileSize, Vector2& offset, bool isIso);
	Vector2 PixelToCoord(Vector2& pixPos);
	Vector2 getTileSize();
	bool isIsoMap();
	void setBodyType(const b2BodyType type);

protected:
	static bool setMap(void* obj, const char* data)                       { return static_cast<TmxMapSprite*>(obj)->setMap(data);}
	static const char* getMap(void* obj, const char* data)                { return static_cast<TmxMapSprite*>(obj)->getMap(); }
	static bool writeMap( void* obj, StringTableEntry pFieldName )        { return static_cast<TmxMapSprite*>(obj)->mMapAsset.notNull(); }

	static bool setMapToMeterFactor(void* obj, const char* data)				{return static_cast<TmxMapSprite*>(obj)->setMapToMeterFactor( dAtof(data) ); }
	static StringTableEntry getMapToMeterFactor(void* obj, const char* data)	{return Con::getFloatArg( static_cast<TmxMapSprite*>(obj)->getMapToMeterFactor() );}
	static bool writeMapToMeterFactor(void* obj, StringTableEntry pFieldName)	{return static_cast<TmxMapSprite*>(obj)->mMapPixelToMeterFactor != 0.1f;}


};


#endif //_TMXMAP_SPRITE_H
