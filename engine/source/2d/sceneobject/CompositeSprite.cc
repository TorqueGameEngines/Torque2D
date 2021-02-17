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

#ifndef _COMPOSITE_SPRITE_H_
#include "2d/sceneobject/CompositeSprite.h"
#endif

#ifndef _SPRITE_BATCH_QUERY_H_
#include "2d/core/SpriteBatchQuery.h"
#endif

#ifndef _RENDER_PROXY_H_
#include "2d/core/RenderProxy.h"
#endif

// Script bindings.
#include "2d/sceneobject/CompositeSprite_ScriptBinding.h"

//------------------------------------------------------------------------------

static EnumTable::Enums batchLayoutTypeLookup[] =
                {
                    { CompositeSprite::NO_LAYOUT,           "off"    },
                    { CompositeSprite::RECTILINEAR_LAYOUT,  "rect" },
                    { CompositeSprite::ISOMETRIC_LAYOUT,    "iso"   },
                    { CompositeSprite::CUSTOM_LAYOUT,       "custom"   },
                };

EnumTable batchLayoutTypeTable(sizeof(batchLayoutTypeLookup) / sizeof(EnumTable::Enums), &batchLayoutTypeLookup[0]);

//-----------------------------------------------------------------------------

CompositeSprite::BatchLayoutType CompositeSprite::getBatchLayoutTypeEnum(const char* label)
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(batchLayoutTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( dStricmp(batchLayoutTypeLookup[i].label, label) == 0)
            return (BatchLayoutType)batchLayoutTypeLookup[i].index;
    }

    // Warn.
    Con::warnf("CompositeSprite::getBatchLayoutTypeEnum() - Invalid batch layout type of '%s'", label );

    return CompositeSprite::INVALID_LAYOUT;
}

//-----------------------------------------------------------------------------

const char* CompositeSprite::getBatchLayoutTypeDescription(const CompositeSprite::BatchLayoutType batchLayoutType )
{
    // Search for Mnemonic.
    for (U32 i = 0; i < (sizeof(batchLayoutTypeLookup) / sizeof(EnumTable::Enums)); i++)
    {
        if( batchLayoutTypeLookup[i].index == batchLayoutType )
            return batchLayoutTypeLookup[i].label;
    }

    // Warn.
    Con::warnf( "CompositeSprite::getBatchLayoutTypeDescription() - Invalid batch layout type.");

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

CompositeSprite::CompositeSprite() :
    mBatchLayoutType( NO_LAYOUT )
{
    // Set as auto-sizing.
    mAutoSizing = true;
}

//------------------------------------------------------------------------------

CompositeSprite::~CompositeSprite()
{
}

//------------------------------------------------------------------------------

void CompositeSprite::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    /// Defaults.
    addProtectedField( "DefaultSpriteStride", TypeVector2, Offset(mDefaultSpriteStride, CompositeSprite), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeDefaultSpriteStride, "");
    addProtectedField( "DefaultSpriteSize", TypeVector2, Offset(mDefaultSpriteSize, CompositeSprite), &defaultProtectedSetFn, &defaultProtectedGetFn, &writeDefaultSpriteSize, "");
    addProtectedField( "DefaultSpriteAngle", TypeF32, Offset(mDefaultSpriteSize, CompositeSprite), &setDefaultSpriteAngle, &getDefaultSpriteAngle, &writeDefaultSpriteAngle, "");
    addProtectedField( "BatchLayout", TypeEnum, Offset(mBatchLayoutType, CompositeSprite), &setBatchLayout, &defaultProtectedGetFn, &writeBatchLayout, 1, &batchLayoutTypeTable, "");
    addProtectedField( "BatchCulling", TypeBool, Offset(mBatchCulling, CompositeSprite), &setBatchCulling, &defaultProtectedGetFn, &writeBatchCulling, "");
    addField( "BatchIsolated", TypeBool, Offset(mBatchIsolated, CompositeSprite), &writeBatchIsolated, "");
    addField( "BatchSortMode", TypeEnum, Offset(mBatchSortMode, CompositeSprite), &writeBatchSortMode, 1, &SceneRenderQueue::renderSortTable, "");
}

//-----------------------------------------------------------------------------

bool CompositeSprite::onAdd()
{
    // Call parent.
    if ( !Parent::onAdd() )
        return false;

    // Call sprite batch.
    return SpriteBatch::onAdd();
}

//-----------------------------------------------------------------------------

void CompositeSprite::onRemove()
{
    // Call sprite batch.
    SpriteBatch::onRemove();

    // Call parent.
    Parent::onRemove();
}

//-----------------------------------------------------------------------------

void CompositeSprite::preIntegrate( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Are the spatials dirty?
    if ( getSpatialDirty() )
    {
        // Yes, so update the world transform.
        setBatchTransform( getRenderTransform() );
    }

    // Are the render extents dirty?
    if ( getLocalExtentsDirty() )
    {
        // Yes, so set size as local extents.
        setSize( getLocalExtents() );
    }

    // Call parent.
    Parent::preIntegrate( totalTime, elapsedTime, pDebugStats );
}

//-----------------------------------------------------------------------------

void CompositeSprite::integrateObject( const F32 totalTime, const F32 elapsedTime, DebugStats* pDebugStats )
{
    // Call Parent.
    Parent::integrateObject( totalTime, elapsedTime, pDebugStats );

    integrateSprites(totalTime, elapsedTime, pDebugStats);

    // Finish if the spatials are NOT dirty.
    if ( !getSpatialDirty() )
        return;

    // Update the batch world transform.
    setBatchTransform( getRenderTransform() );
}

//-----------------------------------------------------------------------------

void CompositeSprite::interpolateObject( const F32 timeDelta )
{
    // Call parent.
    Parent::interpolateObject( timeDelta );

    // Finish if the spatials are NOT dirty.
    if ( !getSpatialDirty() )
        return;

    // Update the batch world transform.
    setBatchTransform( getRenderTransform() );
}

//-----------------------------------------------------------------------------

void CompositeSprite::scenePrepareRender( const SceneRenderState* pSceneRenderState, SceneRenderQueue* pSceneRenderQueue )
{
    // Prepare render.
    SpriteBatch::prepareRender( this, pSceneRenderState, pSceneRenderQueue );
}

//-----------------------------------------------------------------------------

void CompositeSprite::sceneRender( const SceneRenderState* pSceneRenderState, const SceneRenderRequest* pSceneRenderRequest, BatchRender* pBatchRenderer )
{
    // Render.
    SpriteBatch::render( pSceneRenderState, pSceneRenderRequest, pBatchRenderer );
}

//------------------------------------------------------------------------------

void CompositeSprite::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object); 

    // Fetch object.
    CompositeSprite* pCompositeSprite = dynamic_cast<CompositeSprite*>(object);

    // Sanity!
    AssertFatal(pCompositeSprite != NULL, "CompositeSprite::copyTo() - Object is not the correct type.");

    // Copy batch layout.
    pCompositeSprite->setBatchLayout( getBatchLayout() );

    // Call sprite batch.
    SpriteBatch::copyTo( dynamic_cast<SpriteBatch*>(object) );
}

//------------------------------------------------------------------------------

void CompositeSprite::setBatchLayout( const BatchLayoutType& batchLayoutType )
{
    // Finish if no change.
    if ( mBatchLayoutType == batchLayoutType )
        return;

    // Do we already have some sprites?
    if ( getSpriteCount() > 0 )
    {
        // Yes, so warn.
        Con::warnf( "CompositeSprite::setBatchLayout() - Changing the batch layout with existing sprites is not allowed.  Clear the sprites first." );
        return;
    }

    // Set layout type.
    mBatchLayoutType = batchLayoutType;
}

//------------------------------------------------------------------------------

SpriteBatchItem* CompositeSprite::createSprite( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Handle layout type appropriately.
    switch( mBatchLayoutType )
    {
        // No layout.
        case NO_LAYOUT:
            return SpriteBatch::createSprite( logicalPosition );

        // Rectilinear layout.
        case RECTILINEAR_LAYOUT:
            return createSpriteRectilinearLayout( logicalPosition );

        // Isometric layout.
        case ISOMETRIC_LAYOUT:
            return createSpriteIsometricLayout( logicalPosition );

        case CUSTOM_LAYOUT:
            return createCustomLayout( logicalPosition );

        default:
            // Warn.
            Con::warnf( "CompositeSprite::createSprite() - Unknown layout type encountered." );
            return SpriteBatch::createSprite( logicalPosition );
    }
}

//-----------------------------------------------------------------------------

SpriteBatchItem* CompositeSprite::createSpriteRectilinearLayout( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Do we have a valid logical position?
    if ( logicalPosition.getArgCount() != 2 )
    {
        // No, so warn.
        Con::warnf( "Invalid logical position specified for composite sprite." );
        return NULL;
    }

    // Does the sprite position already exist?
    if ( findSpritePosition( logicalPosition ) != NULL )
    {
        // Yes, so warn.
        Con::warnf( "Cannot add sprite at logical position '%s' as one already exists.", logicalPosition.getString() );
        return NULL;
    }

    // Create the sprite.
    SpriteBatchItem* pSpriteBatchItem = SpriteBatch::createSprite();

    // Set sprite logical position.
    pSpriteBatchItem->setLogicalPosition( logicalPosition );

    // Set the sprite default position.
    pSpriteBatchItem->setLocalPosition( logicalPosition.getAsVector2().mult( getDefaultSpriteStride() ) );

    // Set the sprite default size and angle.
    pSpriteBatchItem->setSize( SpriteBatch::getDefaultSpriteSize() );
    pSpriteBatchItem->setLocalAngle( SpriteBatch::getDefaultSpriteAngle() );

    return pSpriteBatchItem;
}

//-----------------------------------------------------------------------------

SpriteBatchItem* CompositeSprite::createSpriteIsometricLayout( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Do we have a valid logical position?
    if ( logicalPosition.getArgCount() != 2 )
    {
        // No, so warn.
        Con::warnf( "Invalid logical position specified for composite rectilinear sprite." );
        return NULL;
    }

    // Does the sprite position already exist?
    if ( findSpritePosition( logicalPosition ) != NULL )
    {
        // Yes, so warn.
        Con::warnf( "Cannot add sprite at logical position '%s' as one already exists.", logicalPosition.getString() );
        return NULL;
    }

    // Create the sprite.
    SpriteBatchItem* pSpriteBatchItem = SpriteBatch::createSprite();

    // Set sprite logical position.
    pSpriteBatchItem->setLogicalPosition( logicalPosition );

    // Fetch sprite stride.
    const Vector2 spriteStride = getDefaultSpriteStride();

    // Fetch logical coordinates.
    Vector2 position = logicalPosition.getAsVector2();

    // Calculate position, because this is iso, the stride needs to be angled.
    // TODO: the direction offset needs to be calculated to go with the offset of the squish
    // of the sprite batch items instead of just typing arbitrary values =/.
    Vector2 dir;
    dir.setPolar(0.785398f, getDefaultSpriteSize().x);
    position.Set( (position.x * dir.x ) + (position.y * dir.y ), (position.x * (dir.y * 0.3f)) + (position.y * (-dir.y * 0.3f)) );

    // Set the sprite default position.
    pSpriteBatchItem->setLocalPosition(position);

    // Set the sprite default size and angle.
    pSpriteBatchItem->setSize( getDefaultSpriteSize() );
    pSpriteBatchItem->setIsometric(true);
    //45 degrees
    pSpriteBatchItem->setLocalAngle( -0.785398f );

    return pSpriteBatchItem;
}

//-----------------------------------------------------------------------------

SpriteBatchItem* CompositeSprite::createCustomLayout( const SpriteBatchItem::LogicalPosition& logicalPosition )
{
    // Do we have a valid logical position?
    if ( logicalPosition.isValid() )        
    {
        // Does the sprite already exist?
        if ( findSpritePosition( logicalPosition ) != NULL )
        {
            // Yes, so warn.
            Con::warnf( "Cannot add sprite at logical position '%s' as one already exists.", logicalPosition.getString() );
            return NULL;
        }
    }

    // Create the sprite.
    SpriteBatchItem* pSpriteBatchItem = SpriteBatch::createSprite();

    // Set sprite logical position if it's valid.
    if ( logicalPosition.isValid() )
        pSpriteBatchItem->setLogicalPosition( logicalPosition );

    // Retrieve the local position from the custom layout callback.
    const char* pLocalPosition = Con::executef(this, 2, "onCustomLayout", logicalPosition.getString() );

    // Was a local position returned.
    if ( pLocalPosition != NULL && dStrlen(pLocalPosition) != 0 )
    {
        // Fetch the local position.
        Vector2 position(0.0f, 0.0f);
        Con::setData( TypeVector2, &position, 0, 1, &pLocalPosition );

        // Set the sprite default position.
        pSpriteBatchItem->setLocalPosition( position );
    }

    // Set the sprite default size and angle.
    pSpriteBatchItem->setSize( getDefaultSpriteSize() );
    pSpriteBatchItem->setLocalAngle( SpriteBatch::getDefaultSpriteAngle() );

    return pSpriteBatchItem;
}

//-----------------------------------------------------------------------------

void CompositeSprite::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Call parent.
    Parent::onTamlCustomWrite( customNodes );

    // Write node with sprite batch.
    SpriteBatch::onTamlCustomWrite( customNodes );
}

//-----------------------------------------------------------------------------

void CompositeSprite::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Call parent.
    Parent::onTamlCustomRead( customNodes );

    // Read node with sprite batch.
    SpriteBatch::onTamlCustomRead( customNodes );
}

//-----------------------------------------------------------------------------

static void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "CompositeSprite::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "CompositeSprite::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    // Write sprite batch.
    SpriteBatch::WriteCustomTamlSchema( pClassRep, pParentElement );
}

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_SCHEMA(CompositeSprite, WriteCustomTamlSchema);
