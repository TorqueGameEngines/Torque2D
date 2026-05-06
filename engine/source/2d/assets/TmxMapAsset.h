#ifndef _TMXMAP_ASSET_H_
#define _TMXMAP_ASSET_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#include <Tmx.h>

//-----------------------------------------------------------------------------

DefineConsoleType( TypeTmxMapAssetPtr )

//-----------------------------------------------------------------------------

class TmxMapAsset : public AssetBase
{

////T2D SIM/CONSOLE setup////////////////////
private:
	typedef AssetBase Parent;

public: 

	TmxMapAsset();
	virtual ~TmxMapAsset();

	/// Core.
	static void initPersistFields();
	virtual bool onAdd();
	virtual void onRemove();
	
	/// Declare Console Object.
	DECLARE_CONOBJECT(TmxMapAsset);
//////////////////////////////////////////////


private:

	/// Configuration.
	StringTableEntry            mMapFile;

public:

	void                    setMapFile( const char* pMapFile );
	inline StringTableEntry getMapFile( void ) const                      { return mMapFile; };


	StringTableEntry getOrientation();
	int				 getLayerCount();

	Tmx::Map*		 getParser();

private:

	Tmx::Map*					mParser;

	void calculateMap( void );
	virtual bool isAssetValid();

protected:
	virtual void initializeAsset( void );
	virtual void onAssetRefresh( void );

	/// Taml callbacks.
	virtual void onTamlPreWrite( void );
	virtual void onTamlPostWrite( void );


protected:

	static bool setMapFile( void* obj, const char* data )                 { static_cast<TmxMapAsset*>(obj)->setMapFile(data); return false; }
	static const char* getMapFile(void* obj, const char* data)            { return static_cast<TmxMapAsset*>(obj)->getMapFile(); }

};

#endif //_TMXMAP_ASSET_H_