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

#ifndef _SPRITE_BATCH_ITEM_H_
#include "2d/core/SpriteBatchItem.h"
#endif

#ifndef _SPRITE_BATCH_H_
#include "2d/core/SpriteBatch.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "2d/sceneobject/SceneObject.h"
#endif

//-----------------------------------------------------------------------------

StringTableEntry spritesItemTypeName                = StringTable->insert( "Sprite" );

static StringTableEntry spriteNameName              = StringTable->insert("Name");
static StringTableEntry spriteLogicalPositionName   = StringTable->insert("LogicalPosition");
static StringTableEntry spriteVisibleName           = StringTable->insert("Visible");
static StringTableEntry spriteLocalPositionName     = StringTable->insert("Position");
static StringTableEntry spriteLocalAngleName        = StringTable->insert("Angle");
static StringTableEntry spriteSizeName              = StringTable->insert("Size");
static StringTableEntry spriteDepthName             = StringTable->insert("Depth");
static StringTableEntry spriteFlipXName             = StringTable->insert("FlipX");
static StringTableEntry spriteFlipYName             = StringTable->insert("FlipY");
static StringTableEntry spriteSortPointName         = StringTable->insert("SortPoint");
static StringTableEntry spriteRenderGroupName       = StringTable->insert("RenderGroup");
static StringTableEntry spriteBlendModeName         = StringTable->insert("BlendMode");
static StringTableEntry spriteSrcBlendFactorName    = StringTable->insert("SrcBlendFactor");
static StringTableEntry spriteDstBlendFactorName    = StringTable->insert("DstBlendFactor");
static StringTableEntry spriteBlendColorName        = StringTable->insert("BlendColor");
static StringTableEntry spriteAlphaTestName         = StringTable->insert("AlphaTest");
static StringTableEntry spriteImageName             = StringTable->insert("Image");
static StringTableEntry spriteImageFrameName        = StringTable->insert("Frame");
static StringTableEntry spriteNamedImageFrameName   = StringTable->insert("NamedFrame");
static StringTableEntry spriteAnimationName         = StringTable->insert("Animation");
static StringTableEntry spriteDataObjectName        = StringTable->insert("DataObject");
static StringTableEntry spriteUserDataName          = StringTable->insert("UserData");

//------------------------------------------------------------------------------

SpriteBatchItem::SpriteBatchItem() : mProxyId( SpriteBatch::INVALID_SPRITE_PROXY )
{
    resetState();
}

//------------------------------------------------------------------------------

SpriteBatchItem::~SpriteBatchItem()
{    
    resetState();
}

//------------------------------------------------------------------------------

void SpriteBatchItem::resetState( void )
{
    // Call parent.
    Parent::resetState();

    // Do we have a proxy.
    if ( mProxyId != SpriteBatch::INVALID_SPRITE_PROXY )
    {
        // Sanity!
        AssertFatal( mSpriteBatch != NULL, "Cannot remove proxy with NULL sprite batch." );

        // Destroy proxy.
        mSpriteBatch->destroyQueryProxy( this );
    }

    mSpriteBatch = NULL;
    mBatchId = 0;
    mName = StringTable->EmptyString;
    mLogicalPosition.resetState();

    mVisible = true;
    mExplicitMode = false;
	mTriangleRun = false;

    mLocalPosition.SetZero();
    for (U32 i = 0; i < 4; i++)
        mExplicitVerts[i].SetZero();

    mDepth = 0.0f;
    mLocalAngle = 0.0f;
    setSize( Vector2( 1.0f, 1.0f ) );

    mFlipX = false;
    mFlipY = false;

    mSortPoint.SetZero();
    mRenderGroup = StringTable->EmptyString;

    mBlendMode = true;
    mSrcBlendFactor = GL_SRC_ALPHA;
    mDstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
    mBlendColor = ColorF(1.0f,1.0f,1.0f,1.0f);
    mAlphaTest = -1.0f;

    mDataObject = NULL;

    mLocalTransformDirty = true;
    mLocalAABB.lowerBound.Set( -0.5f, -0.5f );
    mLocalAABB.upperBound.Set( 0.5f, 0.5f );

    mRenderAABB.lowerBound.Set( -0.5f, -0.5f );
    mRenderAABB.upperBound.Set( 0.5f, 0.5f );
    mRenderPosition.setZero();
    mLastBatchTransformId = 0;

    mSpriteBatchQueryKey = 0;

    mUserData = NULL;

    // Only animates if the scene is not paused.
    mSelfTick = false;

    //
    mIsoMetric = false;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::setIsometric(bool val)
{
   mIsoMetric = val;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::setBatchParent( SpriteBatch* pSpriteBatch, const U32 batchId )
{
    // Sanity!
    AssertFatal( pSpriteBatch != NULL, "Cannot assign a NULL batch parent." );
    AssertFatal( mSpriteBatch == NULL, "Cannot assign batch parent as one is already assigned." );
    AssertFatal( batchId != 0, "Cannot assign a zero batch Id." );

    // Assign.
    mSpriteBatch = pSpriteBatch;
    mBatchId = batchId;

    // Create proxy.
    mSpriteBatch->createQueryProxy( this );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::copyTo( SpriteBatchItem* pSpriteBatchItem ) const
{
    // Call parent.
    Parent::copyTo( pSpriteBatchItem );

    // Set sprite batch item.
    pSpriteBatchItem->setLogicalPosition( getLogicalPosition() );
    pSpriteBatchItem->setName( getName() );
    pSpriteBatchItem->setVisible( getVisible() );
    pSpriteBatchItem->setLocalPosition( getLocalPosition() );
    pSpriteBatchItem->setDepth( getDepth() );
    pSpriteBatchItem->setLocalAngle( getLocalAngle() );
    pSpriteBatchItem->setSize( getSize() );
    pSpriteBatchItem->setFlipX( getFlipX() );
    pSpriteBatchItem->setFlipY( getFlipY() );
    pSpriteBatchItem->setSortPoint( getSortPoint() );
    pSpriteBatchItem->setRenderGroup( getRenderGroup() );
    pSpriteBatchItem->setBlendMode( getBlendMode() );
    pSpriteBatchItem->setSrcBlendFactor( getSrcBlendFactor() );
    pSpriteBatchItem->setDstBlendFactor( getDstBlendFactor() );
    pSpriteBatchItem->setBlendColor( getBlendColor() );
    pSpriteBatchItem->setAlphaTest( getAlphaTest() );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::prepareRender( SceneRenderRequest* pSceneRenderRequest, const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_PrepareRender);

    // Sanity!
    AssertFatal( pSceneRenderRequest != NULL, "Cannot prepare a sprite batch with a NULL scene render request." );

    // Update the world transform.
    updateWorldTransform( batchTransformId );

    pSceneRenderRequest->mWorldPosition = mRenderPosition;
    pSceneRenderRequest->mDepth = getDepth();
    pSceneRenderRequest->mSortPoint = getSortPoint();
    pSceneRenderRequest->mSerialId = getBatchId();
    pSceneRenderRequest->mRenderGroup = getRenderGroup();
    pSceneRenderRequest->mBlendMode = getBlendMode();
    pSceneRenderRequest->mSrcBlendFactor = getSrcBlendFactor();
    pSceneRenderRequest->mDstBlendFactor = getDstBlendFactor();
    pSceneRenderRequest->mBlendColor = getBlendColor();
    pSceneRenderRequest->mAlphaTest = getAlphaTest();
}

//------------------------------------------------------------------------------

void SpriteBatchItem::render( BatchRender* pBatchRenderer, const SceneRenderRequest* pSceneRenderRequest, const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_Render);

    // Update the world transform.
    updateWorldTransform( batchTransformId );

    // Set the blend mode.
    pBatchRenderer->setBlendMode( pSceneRenderRequest );

    // Set the alpha test mode.
    pBatchRenderer->setAlphaTestMode( pSceneRenderRequest );

    // Render.
	if (mTriangleRun) {
		Parent::render(
			mDrawData.vertexCount,
			mDrawData.vertexArray.data(),
			mDrawData.textureArray.data(),
			mDrawData.colorArray.data(),
			pBatchRenderer
		);
	}
	else if (mExplicitMode) {
		Parent::render(mFlipX, mFlipY,
			mRenderOOBB[0],
			mRenderOOBB[1],
			mRenderOOBB[2],
			mRenderOOBB[3],
			mExplicitUVs[0],
			mExplicitUVs[1],
			mExplicitUVs[2],
			mExplicitUVs[3],
			pBatchRenderer);
	}
	else {
		Parent::render(mFlipX, mFlipY,
			mRenderOOBB[0],
			mRenderOOBB[1],
			mRenderOOBB[2],
			mRenderOOBB[3],
			pBatchRenderer);
	}
}

//------------------------------------------------------------------------------

// When something else (like an animation runtime) is controlling the vertices and uv's
// just past them through to the render stage.
void SpriteBatchItem::setExplicitVertices( const F32* vertices, const F32* uvs)
{
	mExplicitMode = true;

	mExplicitVerts[0].x = vertices[0];
	mExplicitVerts[0].y = vertices[1];
	mExplicitVerts[1].x = vertices[2];
	mExplicitVerts[1].y = vertices[3];
	mExplicitVerts[2].x = vertices[4];
	mExplicitVerts[2].y = vertices[5];
	mExplicitVerts[3].x = vertices[6];
	mExplicitVerts[3].y = vertices[7];

	if (uvs) {
		mExplicitUVs[0].x = uvs[0];
		mExplicitUVs[0].y = uvs[1];
		mExplicitUVs[1].x = uvs[2];
		mExplicitUVs[1].y = uvs[3];
		mExplicitUVs[2].x = uvs[4];
		mExplicitUVs[2].y = uvs[5];
		mExplicitUVs[3].x = uvs[6];
		mExplicitUVs[3].y = uvs[7];
	}
}

//------------------------------------------------------------------------------

void SpriteBatchItem::updateLocalTransform( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_UpdateLocalTransform);

    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::updateLocalTransform() - Cannot update local transform with a NULL sprite batch." );

    // Finish if local transform is not dirty.
    if ( !mLocalTransformDirty )
        return;

	// Set local transform.
	b2Transform localTransform;
	localTransform.p = mLocalPosition;
	localTransform.q.Set(mLocalAngle);

	// Set local size vertices.
    if (!mExplicitMode)
    {
		// Calculate half size.
		const F32 halfWidth = mSize.x * 0.5f;
		const F32 halfHeight = mSize.y * 0.5f;

		mLocalOOBB[0].Set( -halfWidth, -halfHeight );
        mLocalOOBB[1].Set( +halfWidth, -halfHeight );
        mLocalOOBB[2].Set( +halfWidth, +halfHeight );
        mLocalOOBB[3].Set( -halfWidth, +halfHeight );
    }
    else
    {
        mLocalOOBB[0] = mExplicitVerts[0];
        mLocalOOBB[1] = mExplicitVerts[1];
        mLocalOOBB[2] = mExplicitVerts[2];
        mLocalOOBB[3] = mExplicitVerts[3];
    }

    // Calculate local OOBB.
    CoreMath::mCalculateOOBB( mLocalOOBB, localTransform, mLocalOOBB );

    // Calculate local AABB.
    CoreMath::mOOBBtoAABB( mLocalOOBB, mLocalAABB );

    // Move query proxy.
    mSpriteBatch->moveQueryProxy( this, mLocalAABB );

    // Flag local transform as NOT dirty.
    mLocalTransformDirty = false;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::updateWorldTransform( const U32 batchTransformId )
{
    // Debug Profiling.
    PROFILE_SCOPE(SpriteBatchItem_UpdateWorldTransform);

    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::updateWorldTransform() - Cannot update transform with a NULL sprite batch." );

    // Update the local transform if needed.
    if ( mLocalTransformDirty )
    {
        updateLocalTransform();
    }
    // Finish if the batch transform is up-to-date.
    else if ( batchTransformId == mLastBatchTransformId )
        return;

    // Fetch world transform.
    const b2Transform& worldTransform = mSpriteBatch->getBatchTransform();

    // Calculate world OOBB.
    CoreMath::mCalculateOOBB( mLocalOOBB, worldTransform, mRenderOOBB );
    if (mIsoMetric)
    {
       // verts on the oobb go anti-clockwise 
       //   [0]---[3]
       //    |     |
       //   [1]---[2]
       // at a rotation of 45 degrees anti-clockwise squash 3 and 1

       mRenderOOBB[1].y = mRenderOOBB[1].y + (mSize.y * 0.5);
       mRenderOOBB[3].y = mRenderOOBB[3].y - (mSize.y * 0.5);
    }

    // Calculate render AABB.
    CoreMath::mOOBBtoAABB( mRenderOOBB, mRenderAABB );

    // Calculate the render position.
    mRenderPosition = mRenderAABB.GetCenter();

    // Note the last batch transform Id.
    mLastBatchTransformId = batchTransformId;
}

//------------------------------------------------------------------------------

void SpriteBatchItem::onTamlCustomWrite( TamlCustomNode* pParentNode )
{
    // Add sprite node.
    TamlCustomNode* pSpriteNode = pParentNode->addNode( spritesItemTypeName );

    // Write name.
    if ( getName() != StringTable->EmptyString )
        pSpriteNode->addField( spriteNameName, getName() );

    // Static frame provider?
    if ( isStaticFrameProvider() )
    {
        // Fetch image asset Id.
        StringTableEntry assetId = getImage();

        // Do we have an image?
        if ( assetId != StringTable->EmptyString )
        {
            // Yes, so write image asset Id.
            pSpriteNode->addField( spriteImageName, assetId );

            // Write the image frame.
            if ( isUsingNamedImageFrame() )
                pSpriteNode->addField( spriteNamedImageFrameName, getNamedImageFrame() );
            else
                pSpriteNode->addField( spriteImageFrameName, getImageFrame() );
        }
    }
    else
    {
        // Fetch animation asset Id.
        StringTableEntry assetId = getAnimation();

        // Do we have an animation?
        if ( assetId != StringTable->EmptyString )
        {
            // Yes, so write animation asset Id.
            pSpriteNode->addField( spriteAnimationName, assetId );

        }
    }

    // Write visible.
    if ( !mVisible )
        pSpriteNode->addField( spriteVisibleName, mVisible );

    // Write local position.
    pSpriteNode->addField( spriteLocalPositionName, mLocalPosition );

    // Write local angle.
    if ( mNotZero(mLocalAngle) )
        pSpriteNode->addField( spriteLocalAngleName, mRadToDeg(mLocalAngle) );

    // Write size.
    pSpriteNode->addField( spriteSizeName, mSize );

    // Write depth.
    if ( mNotZero(mDepth) )
        pSpriteNode->addField( spriteDepthName, mDepth );

    // Write flipX
    if ( mFlipX )
        pSpriteNode->addField( spriteFlipXName, mFlipX );

    // Write flipY
    if ( mFlipY )
        pSpriteNode->addField( spriteFlipYName, mFlipY );

    // Write sort point.
    if ( mSortPoint.notZero() )
        pSpriteNode->addField( spriteSortPointName, mSortPoint );

    // Write render group.
    if ( mRenderGroup != StringTable->EmptyString )
        pSpriteNode->addField( spriteRenderGroupName, mRenderGroup );

    // Write blend mode.
    if ( !mBlendMode )
        pSpriteNode->addField( spriteBlendModeName, mBlendMode );

    // Write source blend factor.
    if ( mBlendMode && mSrcBlendFactor != GL_SRC_ALPHA )
        pSpriteNode->addField( spriteSrcBlendFactorName, SceneObject::getSrcBlendFactorDescription(mSrcBlendFactor) );
        
    // Write destination blend factor.
    if ( mBlendMode && mDstBlendFactor != GL_ONE_MINUS_SRC_ALPHA )
        pSpriteNode->addField( spriteDstBlendFactorName, SceneObject::getDstBlendFactorDescription(mDstBlendFactor) );

    // Write blend color.
    if ( mBlendMode && mBlendColor != ColorF(1.0f, 1.0f, 1.0f, 1.0f) )
        pSpriteNode->addField( spriteBlendColorName, mBlendColor );

    // Write alpha test.
    if ( mBlendMode && mAlphaTest >= 0.0f )
        pSpriteNode->addField( spriteAlphaTestName, mAlphaTest );

    // Write logical position.
    if ( getLogicalPosition().isValid() )
        pSpriteNode->addField( spriteLogicalPositionName, getLogicalPosition().getString() );

    // Write data object.
    if ( getDataObject() != NULL )
        pSpriteNode->addNode( getDataObject() );

    if ( getUserData() != NULL)
    {   
        const char* UserDatastr = (const char*) getUserData();
        pSpriteNode->addField( "UserData", UserDatastr );
    }
}

//------------------------------------------------------------------------------

void SpriteBatchItem::onTamlCustomRead( const TamlCustomNode* pSpriteNode )
{
    // Sanity!
    AssertFatal( mSpriteBatch != NULL, "SpriteBatchItem::onTamlCustomRead() - Cannot read sprite batch item with sprite batch." );

    // Fetch sprite fields.
    const TamlCustomFieldVector& spriteField = pSpriteNode->getFields();

    // Iterate property fields.
    for ( TamlCustomFieldVector::const_iterator fieldItr = spriteField.begin(); fieldItr != spriteField.end(); ++fieldItr )
    {
        // Fetch sprite field.
        TamlCustomField* pSpriteField = *fieldItr;

        // Fetch sprite field name.
        StringTableEntry fieldName = pSpriteField->getFieldName();

        // Reset image frame.
        S32 imageFrame = -1;

        if ( fieldName == spriteNameName )
        {
            setName( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteImageName )
        {
            setImage( pSpriteField->getFieldValue() );

            // Set image frame if it's available.
            if ( imageFrame != -1 )
                setImageFrame( imageFrame );

        }
        else if ( fieldName == spriteImageFrameName )
        {
            pSpriteField->getFieldValue( imageFrame );

            // Set image frame if image is available.
            if ( getImage() != StringTable->EmptyString )
                setImageFrame( imageFrame );
        }
        else if ( fieldName == spriteNamedImageFrameName )
        {
            if ( getImage() != StringTable->EmptyString )
                setNamedImageFrame( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteAnimationName )
        {
            setAnimation( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteVisibleName )
        {
            bool visible;
            pSpriteField->getFieldValue( visible );
            setVisible( visible );
        }
        else if ( fieldName == spriteLocalPositionName )
        {
            Vector2 localPosition;
            pSpriteField->getFieldValue( localPosition );
            setLocalPosition( localPosition );
        }
        else if ( fieldName == spriteLocalAngleName )
        {
            F32 localAngle;
            pSpriteField->getFieldValue( localAngle );
            setLocalAngle( mDegToRad( localAngle ) );
        }
        else if ( fieldName == spriteSizeName )
        {
            Vector2 size;
            pSpriteField->getFieldValue( size );
            setSize( size );
        }
        else if ( fieldName == spriteDepthName )
        {
            F32 depth;
            pSpriteField->getFieldValue( depth );
            setDepth( depth );
        }
        else if ( fieldName == spriteFlipXName )
        {
            bool flipX;
            pSpriteField->getFieldValue( flipX );
            setFlipX( flipX );
        }
        else if ( fieldName == spriteFlipYName )
        {
            bool flipY;
            pSpriteField->getFieldValue( flipY );
            setFlipY( flipY );
        }
        else if ( fieldName == spriteSortPointName )
        {
            Vector2 sortPoint;
            pSpriteField->getFieldValue( sortPoint );
            setSortPoint( sortPoint );
        }
        else if ( fieldName == spriteRenderGroupName )
        {
            setRenderGroup( pSpriteField->getFieldValue() );
        }
        else if ( fieldName == spriteBlendModeName )
        {
            bool blendMode;
            pSpriteField->getFieldValue( blendMode );
            setBlendMode( blendMode );
        }
        else if ( fieldName == spriteSrcBlendFactorName )
        {
            setSrcBlendFactor( (GLenum)SceneObject::getSrcBlendFactorEnum( pSpriteField->getFieldValue() ) );
        }
        else if ( fieldName == spriteDstBlendFactorName )
        {
            setDstBlendFactor( (GLenum)SceneObject::getDstBlendFactorEnum( pSpriteField->getFieldValue() ) );
        }
        else if ( fieldName == spriteBlendColorName )
        {
            ColorF blendColor;
            pSpriteField->getFieldValue( blendColor );
            setBlendColor( blendColor );
        }
        else if ( fieldName == spriteAlphaTestName )
        {
            F32 alphaTest;
            pSpriteField->getFieldValue( alphaTest );
            setAlphaTest( alphaTest );
        }
        // Logical position.
        else if ( fieldName == spriteLogicalPositionName )
        {
            // Fetch logical position.
            const char* pLogicalPositionArgs = pSpriteField->getFieldValue();

            // Is there any data?
            if ( dStrlen( pLogicalPositionArgs ) == 0 )
            {
                // No, so warn.
                Con::warnf( "SpriteBatchItem::onTamlCustomRead() - Encountered an empty sprite key.  This sprite will no longer be addressable by logical position." );
                continue;
            }

            // Set logical position.
            setLogicalPosition( LogicalPosition( pLogicalPositionArgs ) );
        }
        else if ( fieldName == spriteUserDataName )
        {
            StringTableEntry UserDatastr = StringTable->insert(pSpriteField->getFieldValue());
            setUserData((void *)UserDatastr);
        }
    }

    // Fetch sprite children.
    const TamlCustomNodeVector& spriteChildren = pSpriteNode->getChildren();

    // Set the data object if a single child exists.
    if ( spriteChildren.size() == 1 )
        setDataObject( spriteChildren[0]->getProxyObject<SimObject>(true) );
}

//------------------------------------------------------------------------------

void SpriteBatchItem::WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "SpriteBatchItem::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "SpriteBatchItem::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create batch item element.
    TiXmlElement* pBatchItemElement = new TiXmlElement( "xs:element" );
    pBatchItemElement->SetAttribute( "name", spritesItemTypeName );
    pBatchItemElement->SetAttribute( "minOccurs", 0 );
    pBatchItemElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pBatchItemElement );

    // Create complex type Element.
    TiXmlElement* pBatchItemComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pBatchItemElement->LinkEndChild( pBatchItemComplexTypeElement );

    // Create "Name" attribute.
    TiXmlElement* pBatchItemName = new TiXmlElement( "xs:attribute" );
    pBatchItemName->SetAttribute( "name", spriteNameName );
    pBatchItemName->SetAttribute( "type", "xs:string" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemName );

    // "Create "Image" attribute.
    TiXmlElement* pBatchItemImage = new TiXmlElement( "xs:attribute" );
    pBatchItemImage->SetAttribute( "name", spriteImageName );
    pBatchItemImage->SetAttribute( "type", "AssetId_ConsoleType" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemImage );

    // "Create "Image Frame" attribute.
    TiXmlElement* pBatchItemImageFrame = new TiXmlElement( "xs:attribute" );
    pBatchItemImageFrame->SetAttribute( "name", spriteImageFrameName );
    pBatchItemImageFrame->SetAttribute( "type", "xs:positiveInteger" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemImageFrame );

    // "Create "Animation" attribute.
    TiXmlElement* pBatchItemAnimation = new TiXmlElement( "xs:attribute" );
    pBatchItemAnimation->SetAttribute( "name", spriteAnimationName );
    pBatchItemAnimation->SetAttribute( "type", "AssetId_ConsoleType" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemAnimation );

    // Create "Visible" attribute.
    TiXmlElement* pBatchItemVisible = new TiXmlElement( "xs:attribute" );
    pBatchItemVisible->SetAttribute( "name", spriteVisibleName );
    pBatchItemVisible->SetAttribute( "type", "xs:boolean" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemVisible );

    // Create "Local Position" attribute.
    TiXmlElement* pBatchItemPosition = new TiXmlElement( "xs:attribute" );
    pBatchItemPosition->SetAttribute( "name", spriteLocalPositionName );
    pBatchItemPosition->SetAttribute( "type", "Vector2_ConsoleType" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemPosition );

    // Create "Size" attribute.
    TiXmlElement* pBatchItemSize = new TiXmlElement( "xs:attribute" );
    pBatchItemSize->SetAttribute( "name", spriteSizeName );
    pBatchItemSize->SetAttribute( "type", "Vector2_ConsoleType" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemSize );

    // Create "Local Angle" attribute.
    TiXmlElement* pBatchItemAngle = new TiXmlElement( "xs:attribute" );
    pBatchItemAngle->SetAttribute( "name", spriteLocalAngleName );
    pBatchItemAngle->SetAttribute( "type", "xs:float" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemAngle );

    // Create "Depth" attribute.
    TiXmlElement* pBatchItemDepth = new TiXmlElement( "xs:attribute" );
    pBatchItemDepth->SetAttribute( "name", spriteDepthName );
    pBatchItemDepth->SetAttribute( "type", "xs:float" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemDepth );

    // Create "FlipX" attribute.
    TiXmlElement* pBatchItemFlipX = new TiXmlElement( "xs:attribute" );
    pBatchItemFlipX->SetAttribute( "name", spriteFlipXName );
    pBatchItemFlipX->SetAttribute( "type", "xs:boolean" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemFlipX );

    // Create "FlipY" attribute.
    TiXmlElement* pBatchItemFlipY = new TiXmlElement( "xs:attribute" );
    pBatchItemFlipY->SetAttribute( "name", spriteFlipYName );
    pBatchItemFlipY->SetAttribute( "type", "xs:boolean" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemFlipY );

    // Create "Sort Point" attribute.
    TiXmlElement* pBatchItemSortPoint = new TiXmlElement( "xs:attribute" );
    pBatchItemSortPoint->SetAttribute( "name", spriteSortPointName );
    pBatchItemSortPoint->SetAttribute( "type", "Vector2_ConsoleType" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemSortPoint );

    // Create "Render Group" attribute.
    TiXmlElement* pBatchItemRenderGroup = new TiXmlElement( "xs:attribute" );
    pBatchItemRenderGroup->SetAttribute( "name", spriteRenderGroupName );
    pBatchItemRenderGroup->SetAttribute( "type", "xs:string" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemRenderGroup );

    // Create "Blend Mode" attribute.
    TiXmlElement* pBatchItemBlendMode = new TiXmlElement( "xs:attribute" );
    pBatchItemBlendMode->SetAttribute( "name", spriteBlendModeName );
    pBatchItemBlendMode->SetAttribute( "type", "xs:boolean" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemBlendMode );

    // Create "Source Blend Factor" attribute.
    TiXmlElement* pBatchItemSrcBlendFactor = new TiXmlElement( "xs:attribute" );
    pBatchItemSrcBlendFactor->SetAttribute( "name", spriteSrcBlendFactorName );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemSrcBlendFactor );
    TiXmlElement* pBatchItemSrcBlendFactorType = new TiXmlElement( "xs:simpleType" );
    pBatchItemSrcBlendFactor->LinkEndChild( pBatchItemSrcBlendFactorType );
    TiXmlElement* pBatchItemSrcBlendFactorTypeRestriction = new TiXmlElement( "xs:restriction" );
    pBatchItemSrcBlendFactorTypeRestriction->SetAttribute( "base", "xs:string" );
    pBatchItemSrcBlendFactorType->LinkEndChild( pBatchItemSrcBlendFactorTypeRestriction );
    const S32 srcBlendFactorEnumsCount = srcBlendFactorTable.size;
    for( S32 index = 0; index < srcBlendFactorEnumsCount; ++index )
    {
        // Add enumeration element.
        TiXmlElement* pSrcBlendFactorEnumeration = new TiXmlElement( "xs:enumeration" );
        pSrcBlendFactorEnumeration->SetAttribute( "value", srcBlendFactorTable.table[index].label );
        pBatchItemSrcBlendFactorTypeRestriction->LinkEndChild( pSrcBlendFactorEnumeration );
    }

    // Create "Destination Blend Factor" attribute.
    TiXmlElement* pBatchItemDstBlendFactor = new TiXmlElement( "xs:attribute" );
    pBatchItemDstBlendFactor->SetAttribute( "name", spriteDstBlendFactorName );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemDstBlendFactor );
    TiXmlElement* pBatchItemDstBlendFactorType = new TiXmlElement( "xs:simpleType" );
    pBatchItemDstBlendFactor->LinkEndChild( pBatchItemDstBlendFactorType );
    TiXmlElement* pBatchItemDstBlendFactorTypeRestriction = new TiXmlElement( "xs:restriction" );
    pBatchItemDstBlendFactorTypeRestriction->SetAttribute( "base", "xs:string" );
    pBatchItemDstBlendFactorType->LinkEndChild( pBatchItemDstBlendFactorTypeRestriction );
    const S32 dstBlendFactorEnumsCount = dstBlendFactorTable.size;
    for( S32 index = 0; index < dstBlendFactorEnumsCount; ++index )
    {
        // Add enumeration element.
        TiXmlElement* pDstBlendFactorEnumeration = new TiXmlElement( "xs:enumeration" );
        pDstBlendFactorEnumeration->SetAttribute( "value", dstBlendFactorTable.table[index].label );
        pBatchItemDstBlendFactorTypeRestriction->LinkEndChild( pDstBlendFactorEnumeration );
    }

    // Create "Blend Color" attribute.
    TiXmlElement* pBatchItemBlendColor = new TiXmlElement( "xs:attribute" );
    pBatchItemBlendColor->SetAttribute( "name", spriteBlendColorName );
    pBatchItemBlendColor->SetAttribute( "type", "Color_Enums" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemBlendColor );

    // Create "Alpha Test" attribute.
    TiXmlElement* pBatchItemAlphaTest = new TiXmlElement( "xs:attribute" );
    pBatchItemAlphaTest->SetAttribute( "name", spriteAlphaTestName );
    pBatchItemAlphaTest->SetAttribute( "type", "xs:float" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemAlphaTest );

    // Create "Logical Position" attribute.
    TiXmlElement* pBatchItemLogicalPosition = new TiXmlElement( "xs:attribute" );
    pBatchItemLogicalPosition->SetAttribute( "name", spriteLogicalPositionName );
    pBatchItemLogicalPosition->SetAttribute( "type", "xs:string" );
    pBatchItemComplexTypeElement->LinkEndChild( pBatchItemLogicalPosition );
}
