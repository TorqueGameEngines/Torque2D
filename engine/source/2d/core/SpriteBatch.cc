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

#ifndef _SPRITE_BATCH_H_
#include "SpriteBatch.h"
#endif

#ifndef _SPRITE_BATCH_QUERY_H_
#include "2d/core/SpriteBatchQuery.h"
#endif

#ifndef _SCENE_RENDER_OBJECT_H_
#include "2d/scene/SceneRenderObject.h"
#endif

//------------------------------------------------------------------------------

static StringTableEntry spritesNodeName = StringTable->insert( "Sprites" );

//------------------------------------------------------------------------------

SpriteBatch::SpriteBatch() :
    mMasterBatchId( 0 ),
    mSelectedSprite( NULL ),
    mBatchSortMode( SceneRenderQueue::RENDER_SORT_OFF ),
    mDefaultSpriteStride( 1.0f, 1.0f),
    mDefaultSpriteSize( 1.0f, 1.0f ),
    mDefaultSpriteAngle( 0.0f ),
    mpSpriteBatchQuery( NULL ),
    mBatchCulling( true )
{
    // Reset batch transform.
    mBatchTransform.SetIdentity();
    mBatchTransformDirty = true;
    mBatchTransformId = 0;

    // Reset local extents.
	mLocalAABB.lowerBound.SetZero();
	mLocalAABB.upperBound.SetZero();
	mLocalExtents.SetZero();
    mLocalExtentsDirty = true;
}

//------------------------------------------------------------------------------

SpriteBatch::~SpriteBatch()
{
}

//-----------------------------------------------------------------------------

bool SpriteBatch::onAdd()
{
    // Create the sprite batch query if required.
    if ( mBatchCulling )
        createSpriteBatchQuery();

    return true;
}

//-----------------------------------------------------------------------------

void SpriteBatch::onRemove()
{
    // Clear the sprites.
    clearSprites();

    // Delete the sprite batch query.
    destroySpriteBatchQuery();
}

//-----------------------------------------------------------------------------

void SpriteBatch::prepareRender( SceneRenderObject* pSceneRenderObject, const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_PrepareRender);

    // Set the sort mode.
    pSceneRenderQueue->setSortMode( getBatchSortMode() );

    // Do we have a sprite batch query?
    if ( mpSpriteBatchQuery != NULL )
    {
        // Yes, so debug Profiling.
        PROFILE_START(SpriteBatch_PrepareRenderQuery);

        // Yes, so fetch sprite batch query and clear results.
        SpriteBatchQuery* pSpriteBatchQuery = getSpriteBatchQuery( true );

		// Calculate local AABB.
		const b2AABB localAABB = calculateLocalAABB(pSceneRenderState->mRenderAABB);

		// Perform query.
		pSpriteBatchQuery->queryArea( localAABB, false );

        // Debug Profiling.
        PROFILE_END(); // SpriteBatch_PrepareRenderQuery

        // Fetch result count.
        const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

        // Finish if no results.
        if ( resultCount == 0 )
            return;

        // Fetch results.
        typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

        // Add picked sprites.
        for ( U32 n = 0; n < resultCount; n++ )
        {
            // Fetch sprite batch Item.
            SpriteBatchItem* pSpriteBatchItem = queryResults[n].mpSpriteBatchItem;

            // Skip if not visible.
            if ( !pSpriteBatchItem->getVisible() )
                continue;

            // Create a render request.
            SceneRenderRequest* pSceneRenderRequest = pSceneRenderQueue->createRenderRequest();

            // Prepare batch item.
            pSpriteBatchItem->prepareRender( pSceneRenderRequest, mBatchTransformId );

            // Set identity.
            pSceneRenderRequest->mpSceneRenderObject = pSceneRenderObject;

            // Set custom data.
            pSceneRenderRequest->mpCustomData1 = pSpriteBatchItem;
        }

        // Clear sprite batch query.
        pSpriteBatchQuery->clearQuery();
    }
    else
    {
        // No, so perform a render request for all the sprites.
        for( typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
        {
            // Fetch sprite batch Item.
            SpriteBatchItem* pSpriteBatchItem = spriteItr->value;

            // Skip if not visible.
            if ( !pSpriteBatchItem->getVisible() )
                continue;

            // Create a render request.
            SceneRenderRequest* pSceneRenderRequest = pSceneRenderQueue->createRenderRequest();

            // Prepare batch item.
            pSpriteBatchItem->prepareRender( pSceneRenderRequest, mBatchTransformId );

            // Set identity.
            pSceneRenderRequest->mpSceneRenderObject = pSceneRenderObject;

            // Set custom data.
            pSceneRenderRequest->mpCustomData1 = pSpriteBatchItem;
        }
    }
}

//------------------------------------------------------------------------------

void SpriteBatch::render( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Fetch sprite batch Item.
    SpriteBatchItem* pSpriteBatchItem = (SpriteBatchItem*)pSceneRenderRequest->mpCustomData1;

    // Batch render.
    pSpriteBatchItem->render( pBatchRenderer, pSceneRenderRequest, mBatchTransformId );
}

//------------------------------------------------------------------------------

void SpriteBatch::createQueryProxy( SpriteBatchItem* pSpriteBatchItem )
{
    // Sanity!
    AssertFatal( pSpriteBatchItem != NULL, "SpriteBatch:createQueryProxy() - Cannot create query proxy on NULL sprite batch item." );

    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CreateTreeProxy);

    // Finish if the batch query is not available.
    if ( mpSpriteBatchQuery == NULL )
        return;

    // Create proxy.
    pSpriteBatchItem->mProxyId = mpSpriteBatchQuery->add( pSpriteBatchItem );    
}

//------------------------------------------------------------------------------

void SpriteBatch::destroyQueryProxy( SpriteBatchItem* pSpriteBatchItem )
{
    // Sanity!
    AssertFatal( pSpriteBatchItem != NULL, "SpriteBatch:destroyQueryProxy() - Cannot destroy query proxy on NULL sprite batch item." );

    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_DestroyTreeProxy);

    // Finish if the batch query is not available.
    if ( mpSpriteBatchQuery == NULL )
        return;

    // Destroy proxy.
    mpSpriteBatchQuery->remove( pSpriteBatchItem );

    // Remove proxy reference.
    pSpriteBatchItem->mProxyId = INVALID_SPRITE_PROXY;
}

//------------------------------------------------------------------------------

void SpriteBatch::moveQueryProxy( SpriteBatchItem* pSpriteBatchItem, const b2AABB& localAABB )
{
    // Sanity!
    AssertFatal( pSpriteBatchItem != NULL, "SpriteBatch:moveQueryProxy() - Cannot move query proxy on NULL sprite batch item." );

    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_MoveTreeProxy);

    // Finish if the batch query is not available.
    if ( mpSpriteBatchQuery == NULL || pSpriteBatchItem->getProxyId() == INVALID_SPRITE_PROXY )
        return;

    // Move proxy.
    mpSpriteBatchQuery->update( pSpriteBatchItem, localAABB, b2Vec2(0.0f, 0.0f) );
}

//------------------------------------------------------------------------------

SpriteBatchQuery* SpriteBatch::getSpriteBatchQuery( const bool clearQuery )
{
    if ( mpSpriteBatchQuery == NULL )
        return NULL;

    // Clear the query if specified.
    if ( clearQuery )
        mpSpriteBatchQuery->clearQuery();
    
    return mpSpriteBatchQuery;
}

//------------------------------------------------------------------------------

void SpriteBatch::copyTo( SpriteBatch* pSpriteBatch ) const
{
    // Clear any existing sprites.
    pSpriteBatch->clearSprites();

    // Set master sprite Id.
    pSpriteBatch->mMasterBatchId = mMasterBatchId;

    // Set batch sort mode.
    pSpriteBatch->setBatchSortMode( getBatchSortMode() );

    // Set batch culling.
    pSpriteBatch->setBatchCulling( getBatchCulling() );

    // Set sprite default size and angle.
    pSpriteBatch->setDefaultSpriteStride( getDefaultSpriteStride() );
    pSpriteBatch->setDefaultSpriteSize( getDefaultSpriteSize() );
    pSpriteBatch->setDefaultSpriteAngle( getDefaultSpriteAngle() );

    // Copy sprites.   
    for( typeSpriteBatchHash::const_iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
    {        
        // Fetch sprite.
        SpriteBatchItem* pSpriteBatchItem = spriteItr->value;

        // Add a sprite.
        const U32 spriteBatchId = pSpriteBatch->addSprite( pSpriteBatchItem->getLogicalPosition() );
        
        // Fetch new sprite.
        SpriteBatchItem* pNewSpriteBatchItem = pSpriteBatch->findSpriteId( spriteBatchId );

        // Push a copy to it.
        pSpriteBatchItem->copyTo( pNewSpriteBatchItem );
    }
}

//------------------------------------------------------------------------------

U32 SpriteBatch::addSprite( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_AddSprite);

    // Create sprite layout.
    mSelectedSprite = createSprite( logicalPosition );

    // Finish if no sprite created.
    if ( mSelectedSprite == NULL )
        return 0;

    // Insert logical position into sprite positions if it's valid.
    if ( logicalPosition.isValid() )
        mSpritePositions.insert( logicalPosition, mSelectedSprite );

    // Flag local extents as dirty.
    setLocalExtentsDirty();

    return mSelectedSprite->getBatchId();
}

//------------------------------------------------------------------------------

bool SpriteBatch::removeSprite( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_RemoveSprite);

    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return false;

    // Remove the sprite logical position if it's valid.
    const SpriteBatchItem::LogicalPosition& logicalPosition = mSelectedSprite->getLogicalPosition();
    if ( logicalPosition.isValid() )
        mSpritePositions.erase( mSelectedSprite->getLogicalPosition() );

    // Fetch and remove any sprite name.
    StringTableEntry spriteName = mSelectedSprite->getName();
    if ( spriteName != StringTable->EmptyString )
        mSpriteNames.erase( spriteName );

    // Destroy the sprite.
    destroySprite( mSelectedSprite->getBatchId() );

    // Reset the selected sprite.
    mSelectedSprite = NULL;

    // Flag local extents as dirty.
    setLocalExtentsDirty();

    return true;
}

//------------------------------------------------------------------------------

void SpriteBatch::clearSprites( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_ClearSprites);

    // Deselect any sprite.
    deselectSprite();

    // Clear sprite positions.
    mSpritePositions.clear();

    // Clear sprite names.
    mSpriteNames.clear();

    // Cache all sprites.
    for( typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
    {
        SpriteBatchItemFactory.cacheObject( spriteItr->value );
    }
    mSprites.clear();
    mMasterBatchId = 0;

    // Flag local extents as dirty.
    setLocalExtentsDirty();
}

//------------------------------------------------------------------------------

void SpriteBatch::setBatchCulling( const bool batchCulling )
{
    // Finish if no change.
    if ( mBatchCulling == batchCulling )
        return;

    // Set batch culling.
    mBatchCulling = batchCulling;

    // Create/destroy sprite batch query appropriately.
    if ( mBatchCulling )
        createSpriteBatchQuery();
    else
        destroySpriteBatchQuery();
}

//------------------------------------------------------------------------------

bool SpriteBatch::selectSprite( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Select sprite.
    mSelectedSprite = findSpritePosition( logicalPosition );

    // Finish if we selected the sprite.
    if ( mSelectedSprite != NULL )
        return true;

    // Not selected so warn.
    Con::warnf( "Cannot select sprite at logical position '%s' as one does not exist.", logicalPosition.getString() );

    return false;
}

//------------------------------------------------------------------------------

bool SpriteBatch::selectSpriteId( const U32 batchId )
{
    // Select sprite.
    mSelectedSprite = findSpriteId( batchId );

    // Finish if we selected the sprite.
    if ( mSelectedSprite != NULL )
        return true;

    // Not selected so warn.
    Con::warnf( "Cannot select sprite Id '%d' as it does not exist.", batchId );

    return false;
}

//------------------------------------------------------------------------------

bool SpriteBatch::selectSpriteName( const char* pName )
{
    // Fail if no name specified.
    if ( pName == NULL || pName == StringTable->EmptyString )
        return false;

    // Select sprite.
    mSelectedSprite = findSpriteName( pName );

    // Finish if we selected the sprite.
    if ( mSelectedSprite != NULL )
        return true;

    // Not selected so warn.
    Con::warnf( "Cannot select sprite name '%s' as it does not exist.", pName );

    return false;
}

//------------------------------------------------------------------------------

U32 SpriteBatch::getSpriteId( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return 0;

    // Get sprite id.
    return mSelectedSprite->getBatchId();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteImage( const char* pAssetId, const U32 imageFrame )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_SetSpriteImage);

    // Sanity!
    AssertFatal( pAssetId, "Cannot set sprite image using a NULL asset Id." );

    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set image and frame.
    mSelectedSprite->setImage( pAssetId, imageFrame );
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteImage( const char* pAssetId, const char* namedFrame )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_SetSpriteImage);

    // Sanity!
    AssertFatal( pAssetId, "Cannot set sprite image using a NULL asset Id." );

    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set image and frame.
    mSelectedSprite->setImage( pAssetId, namedFrame );
}

//------------------------------------------------------------------------------

StringTableEntry SpriteBatch::getSpriteImage( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return StringTable->EmptyString;

    // Get sprite image.
    return mSelectedSprite->getImage();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteImageFrame( const U32 imageFrame )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set image frame.
    mSelectedSprite->setImageFrame( imageFrame );
}

//------------------------------------------------------------------------------

U32 SpriteBatch::getSpriteImageFrame( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return 0;

    // Get image frame.
    return mSelectedSprite->getImageFrame();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteNamedImageFrame( const char* namedFrame )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set image frame.
    mSelectedSprite->setNamedImageFrame( namedFrame );
}

//------------------------------------------------------------------------------

StringTableEntry SpriteBatch::getSpriteNamedImageFrame( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return NULL;

    // Get image frame.
    return mSelectedSprite->getNamedImageFrame();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteAnimation( const char* pAssetId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_SetSpriteAnimation);

    // Sanity!
    AssertFatal( pAssetId, "Cannot set sprite animation using a NULL asset Id." );

    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set animation.
    mSelectedSprite->setAnimation( pAssetId );
}

//------------------------------------------------------------------------------

StringTableEntry SpriteBatch::getSpriteAnimation( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return StringTable->EmptyString;

    // Get animation.
    return mSelectedSprite->getAnimation();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteAnimationFrame(const U32 animationFrame)
{
	// Finish if a sprite is not selected.
	if (!checkSpriteSelected())
		return;

	// Set image frame.
	mSelectedSprite->setAnimationFrame(animationFrame);
}

//------------------------------------------------------------------------------

U32 SpriteBatch::getSpriteAnimationFrame(void) const
{
	// Finish if a sprite is not selected.
	if (!checkSpriteSelected())
		return 0;

	// Get image frame.
	return mSelectedSprite->getAnimationFrame();
}

//------------------------------------------------------------------------------

void SpriteBatch::clearSpriteAsset( void )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Clear the asset.
    mSelectedSprite->clearAssets();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteVisible( const bool visible )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set visibility.
    mSelectedSprite->setVisible( visible );
}

//------------------------------------------------------------------------------

bool SpriteBatch::getSpriteVisible( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return false;

    // Get visibility.
    return mSelectedSprite->getVisible();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteLocalPosition( const Vector2& localPosition )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set local position.
    mSelectedSprite->setLocalPosition( localPosition );

    // Flag local extents as dirty.
    setLocalExtentsDirty();
}

//------------------------------------------------------------------------------

Vector2 SpriteBatch::getSpriteLocalPosition( void )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return Vector2::getZero();

    // Get local position.
    return mSelectedSprite->getLocalPosition();
}

//------------------------------------------------------------------------------

const SpriteBatchItem::LogicalPosition SpriteBatch::getSpriteLogicalPosition( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return NULL;
    
    // Get logical position.
    return mSelectedSprite->getLogicalPosition();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteAngle( const F32 localAngle )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set local angle.
    mSelectedSprite->setLocalAngle( localAngle );

    // Flag local extents as dirty.
    setLocalExtentsDirty();
}

//------------------------------------------------------------------------------

F32 SpriteBatch::getSpriteAngle( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return 0.0f;

    // Get local angle.
    return mSelectedSprite->getLocalAngle();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteDepth( const F32 depth )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set depth.
    mSelectedSprite->setDepth( depth );
}

//------------------------------------------------------------------------------

F32 SpriteBatch::getSpriteDepth( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return 0.0f;

    // Get depth.
    return mSelectedSprite->getDepth();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteSize( const Vector2& size )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set size.
    mSelectedSprite->setSize( size );

    // Flag local extents as dirty.
    setLocalExtentsDirty();
}

//------------------------------------------------------------------------------

Vector2 SpriteBatch::getSpriteSize( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return Vector2::getZero();

    // Get size.
    return mSelectedSprite->getSize();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteFlipX( const bool flipX )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set flip X.
    mSelectedSprite->setFlipX( flipX );
}

//------------------------------------------------------------------------------

bool SpriteBatch::getSpriteFlipX( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return false;

    // Get flip X.
    return mSelectedSprite->getFlipX();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteFlipY( const bool flipY )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set flip Y.
    mSelectedSprite->setFlipY( flipY );
}

//------------------------------------------------------------------------------

bool SpriteBatch::getSpriteFlipY( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return false;

    // Get flip Y.
    return mSelectedSprite->getFlipY();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteSortPoint( const Vector2& sortPoint )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set sort point.
    mSelectedSprite->setSortPoint( sortPoint );
}

//------------------------------------------------------------------------------

Vector2 SpriteBatch::getSpriteSortPoint( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return Vector2::getZero();

    // Get sort point.
    return mSelectedSprite->getSortPoint();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteRenderGroup( const char* pRenderGroup )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set render group.
    mSelectedSprite->setRenderGroup( pRenderGroup );
}

//------------------------------------------------------------------------------

StringTableEntry SpriteBatch::getSpriteRenderGroup( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return NULL;

    // Get render group.
    return mSelectedSprite->getRenderGroup();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteBlendMode( const bool blendMode )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set blend mode.
    mSelectedSprite->setBlendMode( blendMode );
}

//------------------------------------------------------------------------------

bool SpriteBatch::getSpriteBlendMode( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return true;

    // Get blend mode.
    return mSelectedSprite->getBlendMode();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteSrcBlendFactor( GLenum srcBlendFactor )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set source blend factor.
    mSelectedSprite->setSrcBlendFactor( srcBlendFactor );
}

//------------------------------------------------------------------------------

GLenum SpriteBatch::getSpriteSrcBlendFactor( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return GL_SRC_ALPHA;

    // Get source blend factor.
    return mSelectedSprite->getSrcBlendFactor();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteDstBlendFactor( GLenum dstBlendFactor )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return ;

    // Set destination blend factor.
    mSelectedSprite->setDstBlendFactor( dstBlendFactor );
}

//------------------------------------------------------------------------------

GLenum SpriteBatch::getSpriteDstBlendFactor( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return GL_ONE_MINUS_SRC_ALPHA;

    // Get destination blend factor.
    return mSelectedSprite->getDstBlendFactor();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteBlendColor( const ColorF& blendColor )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set blend color.
    mSelectedSprite->setBlendColor( blendColor );
}

//------------------------------------------------------------------------------

const ColorF& SpriteBatch::getSpriteBlendColor( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return ColorF::StockColor("White");

    // Get blend color.
    return mSelectedSprite->getBlendColor();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteBlendAlpha( const F32 alpha )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set blend alpha.
    mSelectedSprite->setBlendAlpha( alpha );
}

//------------------------------------------------------------------------------

F32 SpriteBatch::getSpriteBlendAlpha( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return 0.0f;

    // Get blend alpha.
    return mSelectedSprite->getBlendAlpha();
}
   
//------------------------------------------------------------------------------

void SpriteBatch::setSpriteAlphaTest( const F32 alphaTestMode )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set alpha-test mode.
    mSelectedSprite->setAlphaTest( alphaTestMode );
}

//------------------------------------------------------------------------------

F32 SpriteBatch::getSpriteAlphaTest( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return -1.0f;

    // Get alpha-test mode.
    return mSelectedSprite->getAlphaTest();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteDataObject( SimObject* pDataObject )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set data object.
    mSelectedSprite->setDataObject( pDataObject );
}

//------------------------------------------------------------------------------

SimObject* SpriteBatch::getSpriteDataObject( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return NULL;

    // Get data object.
    return mSelectedSprite->getDataObject();
}

//------------------------------------------------------------------------------

void SpriteBatch::setUserData( void* pUserData )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Set user data.
    mSelectedSprite->setUserData( pUserData );
}

//------------------------------------------------------------------------------

void* SpriteBatch::getUserData( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return NULL;

    // Get user data.
    return mSelectedSprite->getUserData();
}

//------------------------------------------------------------------------------

void SpriteBatch::setSpriteName( const char* pName )
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return;

    // Finish if the sprite name already exists.
    if ( findSpriteName( pName ) )
        return;

    // Insert sprite name.
    mSpriteNames.insert( StringTable->insert( pName ), mSelectedSprite );

    // Set name.
    mSelectedSprite->setName( pName );
}

//------------------------------------------------------------------------------

StringTableEntry SpriteBatch::getSpriteName( void ) const
{
    // Finish if a sprite is not selected.
    if ( !checkSpriteSelected() )
        return StringTable->EmptyString;

    // Get name.
    return mSelectedSprite->getName();
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::createSprite( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CreateSprite);

    // Allocate batch Id.
    const U32 batchId = ++mMasterBatchId;

    // Create sprite batch item,
    SpriteBatchItem* pSpriteBatchItem = SpriteBatchItemFactory.createObject();

    // Set batch parent.
    pSpriteBatchItem->setBatchParent( this, batchId );

    // Create sprite batch item,
    mSprites.insert( batchId, pSpriteBatchItem );

    return pSpriteBatchItem;
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::createSprite( const Vector2* explicitVertices  )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CreateSprite);

    // Allocate batch Id.
    const U32 batchId = ++mMasterBatchId;

    // Create sprite batch item,
    SpriteBatchItem* pSpriteBatchItem = SpriteBatchItemFactory.createObject();

    // Set batch parent.
    pSpriteBatchItem->setBatchParent( this, batchId );

    // Set explicit mode.
    pSpriteBatchItem->setExplicitMode( true );

    // Set explicit vertices.


    // Create sprite batch item,
    mSprites.insert( batchId, pSpriteBatchItem );

    return pSpriteBatchItem;
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::findSpritePosition( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_FindSpritePosition);

    // Invalid logical positions are not stored.
    if ( !logicalPosition.isValid() )
        return NULL;

    // Find sprite.
    typeSpritePositionHash::iterator spriteItr = mSpritePositions.find( logicalPosition );

    return spriteItr == mSpritePositions.end() ? NULL : spriteItr->value;
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::findSpriteId( const U32 batchId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_FindSpriteId);

    // Find sprite.
    typeSpriteBatchHash::iterator spriteItr = mSprites.find( batchId );

    return spriteItr != mSprites.end() ? spriteItr->value : NULL;
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::findSpriteName( const char* pName )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_FindSpriteName);

    // Finish if no name specified.
    if ( pName == NULL || pName == StringTable->EmptyString )
        return NULL;

    // Find sprite.
    typeSpriteNameHash::iterator spriteItr = mSpriteNames.find( StringTable->insert(pName) );

    return spriteItr != mSpriteNames.end() ? spriteItr->value : NULL;
}

//------------------------------------------------------------------------------

SpriteBatchItem* SpriteBatch::createSprite( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CreateSpriteAtLogicalPosition);

    // Reset sprite batch item.
    SpriteBatchItem* pSpriteBatchItem = NULL;

    // Do we have a valid logical position?
    if ( logicalPosition.isValid() )        
    {
        // Does it have the correct argument count?
        if ( logicalPosition.getArgCount() != 2 )
        {
            // No, so warn.
            Con::warnf( "Invalid logical position specified for composite sprite." );
            return NULL;
        }

        // Does the sprite already exist?
        if ( findSpritePosition( logicalPosition ) != NULL )
        {
            // Yes, so warn.
            Con::warnf( "Cannot add sprite at logical position '%s' as one already exists.", logicalPosition.getString() );
            return NULL;
        }

        // Create the sprite.
        pSpriteBatchItem = createSprite();

        // Set the logical position.
        pSpriteBatchItem->setLogicalPosition( logicalPosition );

        // Set the sprite default local position.
        pSpriteBatchItem->setLocalPosition( logicalPosition.getAsVector2() );
    }
    else
    {
        // Create the sprite.
        pSpriteBatchItem = createSprite();
    }

    // Set the sprite default size and angle.
    pSpriteBatchItem->setSize( getDefaultSpriteSize() );
    pSpriteBatchItem->setLocalAngle( getDefaultSpriteAngle() );

    return pSpriteBatchItem;
}

//------------------------------------------------------------------------------

void SpriteBatch::integrateSprites(const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats)
{
   //process the elapsed time for all sprites
   for (typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr)
   {
      // Update image frame provider.
      spriteItr->value->ImageFrameProvider::update(elapsedTime);
   }
}

//------------------------------------------------------------------------------

void SpriteBatch::setBatchTransform( const b2Transform& batchTransform )
{
    // Update world transform.
    mBatchTransform = batchTransform;

    // Flag the batch transform as dirty.
    setBatchTransformDirty();
}

//------------------------------------------------------------------------------

void SpriteBatch::updateLocalExtents(const b2AABB *precalculatedLocalAABB)
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_UpdateLocalExtents);

	if (precalculatedLocalAABB && precalculatedLocalAABB->IsValid()) {
		// We are being given our AABB.  Nothing further is needed.
		mLocalAABB = *precalculatedLocalAABB;
		mLocalExtents = mLocalAABB.upperBound - mLocalAABB.lowerBound;

		mLocalExtentsDirty = false;

		return;
	}

	// Finish if the local extents are not dirty.
    if ( !mLocalExtentsDirty )
        return;

    // Flag as NOT dirty.
    mLocalExtentsDirty = false;

    // Do we have any sprites?
    if ( mSprites.size() == 0 )
    {
        // No, so reset local extents.
        mLocalExtents.setOne();

        return;
    }

    // Fetch first sprite.
    typeSpriteBatchHash::iterator spriteItr = mSprites.begin();

    // Set render AABB to this sprite.
    mLocalAABB = spriteItr->value->getLocalAABB();

	// Combine with the rest of the sprites.
    for( ; spriteItr != mSprites.end(); ++spriteItr )
    {
		mLocalAABB.Combine( spriteItr->value->getLocalAABB() );
    }
	
	float xSize = mLocalAABB.upperBound.x > mLocalAABB.lowerBound.x
		? mLocalAABB.upperBound.x - mLocalAABB.lowerBound.x
		: mLocalAABB.lowerBound.x - mLocalAABB.upperBound.x;
	float ySize = mLocalAABB.upperBound.y > mLocalAABB.lowerBound.y
		? mLocalAABB.upperBound.y - mLocalAABB.lowerBound.y
		: mLocalAABB.lowerBound.y - mLocalAABB.upperBound.y;

	mLocalExtents.Set(xSize, ySize);
}

//------------------------------------------------------------------------------

void SpriteBatch::createSpriteBatchQuery( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CreateSpriteBatchQuery);

    // Finish if batch culling is off or there is already a sprite batch query.
    if ( !mBatchCulling || mpSpriteBatchQuery != NULL )
        return;

    // Set the sprite batch query appropriately.
    mpSpriteBatchQuery = new SpriteBatchQuery( this );

    // Finish if there are no sprites.
    if ( mSprites.size() == 0 )
        return;

    // Add proxies for all the sprites.
    for( typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
    {
        // Fetch sprite batch item.
        SpriteBatchItem* pSpriteBatchItem = spriteItr->value;

        // Create query proxy for sprite.
        createQueryProxy( pSpriteBatchItem );
    }
}

//------------------------------------------------------------------------------

void SpriteBatch::destroySpriteBatchQuery( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_DestroySpriteBatchQuery);

    // Finish if there is no sprite batch query.
    if ( mpSpriteBatchQuery == NULL )
        return;

    // Are there any sprites?
    if ( mSprites.size() > 0 )
    {
        // Yes, so destroy proxies of all the sprites.
        for( typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
        {
            // Destroy query proxy for sprite.
            destroyQueryProxy( spriteItr->value );
        }
    }

    // Finish if sprite clipping 
    delete mpSpriteBatchQuery;
    mpSpriteBatchQuery = NULL;
}

//------------------------------------------------------------------------------

bool SpriteBatch::destroySprite( const U32 batchId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_DestroySprite);

    // Find sprite.
    typeSpriteBatchHash::iterator spriteItr = mSprites.find( batchId );

    // Finish if sprite not found.
    if ( spriteItr == mSprites.end() )
        return false;

    // Find sprite.    
    SpriteBatchItem* pSpriteBatchItem = spriteItr->value;

    // Sanity!
    AssertFatal( pSpriteBatchItem != NULL, "SpriteBatch::destroySprite() - Found sprite but it was NULL." );

    // Cache sprite.
    SpriteBatchItemFactory.cacheObject( pSpriteBatchItem );

    // Remove from sprites.
    mSprites.erase( batchId );

    return true;
}

//------------------------------------------------------------------------------

bool SpriteBatch::checkSpriteSelected( void ) const
{
    // Finish if a sprite is selected.
    if ( mSelectedSprite != NULL )
        return true;

    // No, so warn,
    Con::warnf( "Cannot perform sprite operation no sprite is selected." );

    return false;
}

//------------------------------------------------------------------------------

b2AABB SpriteBatch::calculateLocalAABB( const b2AABB& renderAABB )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_CalculateLocalAABB);

    // Calculate local OOBB.
    b2Vec2 localOOBB[4];
    CoreMath::mAABBtoOOBB( renderAABB, localOOBB );
    CoreMath::mCalculateInverseOOBB( localOOBB, mBatchTransform, localOOBB );

    // Calculate local AABB.
    b2AABB localAABB;
    CoreMath::mOOBBtoAABB( localOOBB, localAABB );
    
    return localAABB;
}

//------------------------------------------------------------------------------

void SpriteBatch::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_TamlCustomWrite);

    // Finish if no sprites.
    if ( getSpriteCount() == 0 )
        return;

    // Add sprites node.
    TamlCustomNode* pSpritesNode = customNodes.addNode( spritesNodeName );

    // Write all sprites.
    for( typeSpriteBatchHash::iterator spriteItr = mSprites.begin(); spriteItr != mSprites.end(); ++spriteItr )
    {      
        // Write type with sprite item.
        spriteItr->value->onTamlCustomWrite( pSpritesNode );
    }
}

//------------------------------------------------------------------------------

void SpriteBatch::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatch_TamlCustomRead);

    // Find sprites custom node.
    const TamlCustomNode* pSpritesNode = customNodes.findNode( spritesNodeName );

    // Finish if we don't have the node.
    if ( pSpritesNode == NULL )
        return;

    // Fetch children nodes.
    const TamlCustomNodeVector& spriteNodes = pSpritesNode->getChildren();

    // Iterate sprite item types.
    for( TamlCustomNodeVector::const_iterator spriteItr = spriteNodes.begin(); spriteItr != spriteNodes.end(); ++spriteItr )
    {
        // Fetch sprite node.
        TamlCustomNode* pNode = *spriteItr;

        // Fetch alias name.
        StringTableEntry nodeName = pNode->getNodeName();

        // Is this a known node name?
        if ( nodeName != spritesItemTypeName )
        {
            // No, so warn.
            Con::warnf( "SpriteBatch - Unknown custom type '%s'.", nodeName );
            continue;
        }

        // Create sprite.
        SpriteBatchItem* pSpriteBatchItem = createSprite();

        // Read type with sprite item.
        pSpriteBatchItem->onTamlCustomRead( pNode );

        // Fetch logical position.
        const SpriteBatchItem::LogicalPosition& logicalPosition = pSpriteBatchItem->getLogicalPosition();

        // Did we get a logical position?
        if ( logicalPosition.isValid() )
        {
            // Yes, so insert into sprite positions.
            mSpritePositions.insert( logicalPosition, pSpriteBatchItem );
        }

        // Fetch sprite name.
        StringTableEntry spriteName = pSpriteBatchItem->getName();

        // Did we get a sprite name?
        if ( spriteName != StringTable->EmptyString )
        {
            // Yes, so insert into sprite names if it doesn't already exist.
            if ( mSpriteNames.find( spriteName ) != mSpriteNames.end() ) 
                mSpriteNames.insert( spriteName, mSelectedSprite );
        }
    }
}

//------------------------------------------------------------------------------

void SpriteBatch::WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SpriteBatch::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SpriteBatch::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create sprite batch node element.
    TiXmlElement* pBatchNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), spritesNodeName );
    pBatchNodeElement->SetAttribute( "name", buffer );
    pBatchNodeElement->SetAttribute( "minOccurs", 0 );
    pBatchNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pBatchNodeElement );

    // Create complex type.
    TiXmlElement* pBatchNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pBatchNodeElement->LinkEndChild( pBatchNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pBatchNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pBatchNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pBatchNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pBatchNodeComplexTypeElement->LinkEndChild( pBatchNodeChoiceElement );

    // Write sprite batch item.
    SpriteBatchItem::WriteCustomTamlSchema( pClassRep, pBatchNodeChoiceElement );
}
