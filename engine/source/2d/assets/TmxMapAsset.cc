#include "TmxMapAsset.h"

// Debug Profiling.
#include "debug/profiler.h"

#include "TmxMapAsset_ScriptBinding.h"

//------------------------------------------------------------------------------

ConsoleType( tmxMapAssetPtr, TypeTmxMapAssetPtr, sizeof(AssetPtr<TmxMapAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeTmxMapAssetPtr )
{
	// Fetch asset Id.
	return (*((AssetPtr<TmxMapAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeTmxMapAssetPtr )
{
	// Was a single argument specified?
	if( argc == 1 )
	{
		// Yes, so fetch field value.
		const char* pFieldValue = argv[0];

		// Fetch asset pointer.
		AssetPtr<TmxMapAsset>* pAssetPtr = dynamic_cast<AssetPtr<TmxMapAsset>*>((AssetPtrBase*)(dptr));

		// Is the asset pointer the correct type?
		if ( pAssetPtr == NULL )
		{
			// No, so fail.
			Con::warnf( "(TypeTmxMapAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
			return;
		}

		// Set asset.
		pAssetPtr->setAssetId( pFieldValue );

		return;
	}

	// Warn.
	Con::warnf( "(TypeTmxMapAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(TmxMapAsset);

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

TmxMapAsset::TmxMapAsset() :  mMapFile(StringTable->EmptyString),
	mParser(NULL)

{
}

//------------------------------------------------------------------------------

TmxMapAsset::~TmxMapAsset()
{
	if (mParser)
	{
		delete mParser;
	}
}

//------------------------------------------------------------------------------

void TmxMapAsset::initPersistFields()
{
	// Call parent.
	Parent::initPersistFields();

	// Fields.
	addProtectedField("MapFile", TypeAssetLooseFilePath, Offset(mMapFile, TmxMapAsset), &setMapFile, &getMapFile, &defaultProtectedWriteFn, "");

}

//------------------------------------------------------------------------------

bool TmxMapAsset::onAdd()
{
	// Call Parent.
	if (!Parent::onAdd())
		return false;

	return true;
}

//------------------------------------------------------------------------------

void TmxMapAsset::onRemove()
{
	// Call Parent.
	Parent::onRemove();
}


//------------------------------------------------------------------------------

void TmxMapAsset::setMapFile( const char* pMapFile )
{
	// Sanity!
	AssertFatal( pMapFile != NULL, "Cannot use a NULL map file." );

	// Fetch image file.
	pMapFile = StringTable->insert( pMapFile );

	// Ignore no change,
	if ( pMapFile == mMapFile )
		return;

	// Update.
	mMapFile = getOwned() ? expandAssetFilePath( pMapFile ) : StringTable->insert( pMapFile );

	// Refresh the asset.
	refreshAsset();
}

//------------------------------------------------------------------------------

void TmxMapAsset::initializeAsset( void )
{
	// Call parent.
	Parent::initializeAsset();

	// Ensure the image-file is expanded.
	mMapFile = expandAssetFilePath( mMapFile );

	calculateMap();
}

//------------------------------------------------------------------------------

void TmxMapAsset::onAssetRefresh( void ) 
{
	// Ignore if not yet added to the sim.
	if ( !isProperlyAdded() )
		return;

	// Call parent.
	Parent::onAssetRefresh();

	calculateMap();
}	

//-----------------------------------------------------------------------------

void TmxMapAsset::onTamlPreWrite( void )
{
	// Call parent.
	Parent::onTamlPreWrite();

	// Ensure the image-file is collapsed.
	mMapFile = collapseAssetFilePath( mMapFile );
}

//-----------------------------------------------------------------------------

void TmxMapAsset::onTamlPostWrite( void )
{
	// Call parent.
	Parent::onTamlPostWrite();

	// Ensure the image-file is expanded.
	mMapFile = expandAssetFilePath( mMapFile );
}

//----------------------------------------------------------------------------

void TmxMapAsset::calculateMap()
{
	if (mParser)
	{
		delete mParser;
		mParser = NULL;
	}

	mParser = new Tmx::Map();
	mParser->ParseFile( mMapFile );

	if (mParser->HasError())
	{
		// No, so warn.
		Con::warnf( "Map '%' could not be parsed: error code (%d) - %s.", getAssetId(), mParser->GetErrorCode(), mParser->GetErrorText().c_str() );
		delete mParser;
		mParser = NULL;
		return;
	}
}

bool TmxMapAsset::isAssetValid()
{
	return (mParser != NULL);
}

StringTableEntry TmxMapAsset::getOrientation()
{
	if (!isAssetValid()) return StringTable->EmptyString;

	switch(mParser->GetOrientation())
	{
	case Tmx::TMX_MO_ORTHOGONAL:
		{
			return StringTable->insert("ortho");
		}
	case Tmx::TMX_MO_ISOMETRIC:
		{
			return StringTable->insert("iso");
		}
	case Tmx::TMX_MO_STAGGERED:
		{
			return StringTable->insert("stag");
		}
	default:
		{
			return StringTable->EmptyString;
		}
	}
}

int TmxMapAsset::getLayerCount()
{
	if (!isAssetValid()) return 0;

	return mParser->GetNumLayers();
}

Tmx::Map* TmxMapAsset::getParser()
{
	if (!isAssetValid()) return NULL;

	return mParser;
}