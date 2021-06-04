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

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _PLATFORM_H_
#include "platform/platform.h"
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

#ifndef _IMAGE_ASSET_H_
#include "2d/assets/ImageAsset.h"
#endif

// Script bindings.
#include "ImageAsset_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

//------------------------------------------------------------------------------

ImageAsset::FrameArea BadFrameArea(0, 0, 0, 0, 0.0f, 0.0f);

//------------------------------------------------------------------------------

ConsoleType( imageAssetPtr, TypeImageAssetPtr, sizeof(AssetPtr<ImageAsset>), ASSET_ID_FIELD_PREFIX )

//-----------------------------------------------------------------------------

ConsoleGetType( TypeImageAssetPtr )
{
    // Fetch asset Id.
    return (*((AssetPtr<ImageAsset>*)dptr)).getAssetId();
}

//-----------------------------------------------------------------------------

ConsoleSetType( TypeImageAssetPtr )
{
    // Was a single argument specified?
    if( argc == 1 )
    {
        // Yes, so fetch field value.
        const char* pFieldValue = argv[0];

        // Fetch asset pointer.
        AssetPtr<ImageAsset>* pAssetPtr = dynamic_cast<AssetPtr<ImageAsset>*>((AssetPtrBase*)(dptr));

        // Is the asset pointer the correct type?
        if ( pAssetPtr == NULL )
        {
            // No, so fail.
            Con::warnf( "(TypeImageAssetPtr) - Failed to set asset Id '%d'.", pFieldValue );
            return;
        }

        // Set asset.
        pAssetPtr->setAssetId( pFieldValue );

        return;
   }

    // Warn.
    Con::warnf( "(TypeImageAssetPtr) - Cannot set multiple args to a single asset." );
}

//------------------------------------------------------------------------------

static StringTableEntry cellCustomNodeCellsName     = StringTable->insert( "Cells" );
static StringTableEntry cellNodeName                = StringTable->insert( "Cell" );
static StringTableEntry cellRegionName              = StringTable->insert( "RegionName" );
static StringTableEntry cellOffsetName              = StringTable->insert( "Offset" );
static StringTableEntry cellOffsetXName             = StringTable->insert( "OffsetX" );
static StringTableEntry cellOffsetYName             = StringTable->insert( "OffsetY" );
static StringTableEntry cellWidthName               = StringTable->insert( "Width" );
static StringTableEntry cellHeightName              = StringTable->insert( "Height" );
static StringTableEntry cellNameEntryName           = StringTable->insert( "Name" );

//------------------------------------------------------------------------------

static EnumTable::Enums textureFilterLookup[] =
                {
                { ImageAsset::FILTER_NEAREST,     "NEAREST"     },
				{ ImageAsset::FILTER_BILINEAR,    "BILINEAR"    },
				{ ImageAsset::FILTER_INVALID,    "DEFAULT"    }
                };

EnumTable textureFilterTable(sizeof(textureFilterLookup) / sizeof(EnumTable::Enums), &textureFilterLookup[0]);

//------------------------------------------------------------------------------

ImageAsset::TextureFilterMode ImageAsset::getFilterModeEnum(const char* label)
{
    // Search for Mnemonic.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( dStricmp(textureFilterLookup[i].label, label) == 0)
            return((ImageAsset::TextureFilterMode)textureFilterLookup[i].index);

    // Warn.
    Con::warnf( "ImageAsset::getFilterModeEnum() - Invalid filter-mode '%s'", label );

    return ImageAsset::FILTER_INVALID;
}

//------------------------------------------------------------------------------

const char* ImageAsset::getFilterModeDescription( ImageAsset::TextureFilterMode filterMode )
{
    // Search for Mode.
    for(U32 i = 0; i < (sizeof(textureFilterLookup) / sizeof(EnumTable::Enums)); i++)
        if( textureFilterLookup[i].index == filterMode )
            return textureFilterLookup[i].label;

    // Warn.
    Con::warnf( "ImageAsset::getFilterModeDescription() - Invalid filter-mode." );

    return StringTable->EmptyString;
}

//------------------------------------------------------------------------------

ImageAsset::ImageAsset() :  mImageFile(StringTable->EmptyString),
                            mForce16Bit(false),
                            mLocalFilterMode(FILTER_INVALID),
                            mExplicitMode(false),
                            mCellRowOrder(true),
                            mCellOffsetX(0),
                            mCellOffsetY(0),
                            mCellStrideX(0),
                            mCellStrideY(0),
                            mCellCountX(0),
                            mCellCountY(0),
                            mCellWidth(0),
                            mCellHeight(0),

                            mImageTextureHandle(NULL)
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mFrames );
    VECTOR_SET_ASSOCIATION( mExplicitFrames );
}

//------------------------------------------------------------------------------

ImageAsset::~ImageAsset()
{
}

//------------------------------------------------------------------------------

void ImageAsset::initPersistFields()
{
    // Call parent.
    Parent::initPersistFields();

    // Fields.
    addProtectedField("ImageFile", TypeAssetLooseFilePath, Offset(mImageFile, ImageAsset), &setImageFile, &getImageFile, &defaultProtectedWriteFn, "");
    addProtectedField("Force16bit", TypeBool, Offset(mForce16Bit, ImageAsset), &setForce16Bit, &defaultProtectedGetFn, &writeForce16Bit, "Forces the image into 16 bit mode.");
    addProtectedField("FilterMode", TypeEnum, Offset(mLocalFilterMode, ImageAsset), &setFilterMode, &defaultProtectedGetFn, &writeFilterMode, 1, &textureFilterTable);   
    addProtectedField("ExplicitMode", TypeBool, Offset(mExplicitMode, ImageAsset), &setExplicitMode, &defaultProtectedGetFn, &writeExplicitMode, "");

    addProtectedField("CellRowOrder", TypeBool, Offset(mCellRowOrder, ImageAsset), &setCellRowOrder, &defaultProtectedGetFn, &writeCellRowOrder, "");
    addProtectedField("CellOffsetX", TypeS32, Offset(mCellOffsetX, ImageAsset), &setCellOffsetX, &defaultProtectedGetFn, &writeCellOffsetX, "");
    addProtectedField("CellOffsetY", TypeS32, Offset(mCellOffsetY, ImageAsset), &setCellOffsetY, &defaultProtectedGetFn, &writeCellOffsetY, "");
    addProtectedField("CellStrideX", TypeS32, Offset(mCellStrideX, ImageAsset), &setCellStrideX, &defaultProtectedGetFn, &writeCellStrideX, "");
    addProtectedField("CellStrideY", TypeS32, Offset(mCellStrideY, ImageAsset), &setCellStrideY, &defaultProtectedGetFn, &writeCellStrideY, "");
    addProtectedField("CellCountX", TypeS32, Offset(mCellCountX, ImageAsset), &setCellCountX, &defaultProtectedGetFn, &writeCellCountX, "");
    addProtectedField("CellCountY", TypeS32, Offset(mCellCountY, ImageAsset), &setCellCountY, &defaultProtectedGetFn, &writeCellCountY, "");
    addProtectedField("CellWidth", TypeS32, Offset(mCellWidth, ImageAsset), &setCellWidth, &defaultProtectedGetFn, &writeCellWidth, "");
    addProtectedField("CellHeight", TypeS32, Offset(mCellHeight, ImageAsset), &setCellHeight, &defaultProtectedGetFn, &writeCellHeight, "");
}

//------------------------------------------------------------------------------

bool ImageAsset::onAdd()
{
    // Call Parent.
    if (!Parent::onAdd())
       return false;

    return true;
}

//------------------------------------------------------------------------------

void ImageAsset::onRemove()
{
    // Call Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ImageAsset::setImageFile( const char* pImageFile )
{
    // Sanity!
    AssertFatal( pImageFile != NULL, "Cannot use a NULL image file." );

    // Fetch image file.
    pImageFile = StringTable->insert( pImageFile );

    // Ignore no change,
    if ( pImageFile == mImageFile )
        return;

    // Update.
    mImageFile = getOwned() ? expandAssetFilePath( pImageFile ) : StringTable->insert( pImageFile );

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::copyTo(SimObject* object)
{
    // Call to parent.
    Parent::copyTo(object);

    // Cast to asset.
    ImageAsset* pAsset = static_cast<ImageAsset*>(object);

    // Sanity!
    AssertFatal(pAsset != NULL, "ImageAsset::copyTo() - Object is not the correct type.");

    // Copy state.
    pAsset->setImageFile( getImageFile() );
    pAsset->setForce16Bit( getForce16Bit() );
    pAsset->setFilterMode( getFilterMode() );
    pAsset->setExplicitMode( getExplicitMode() );
    pAsset->setCellRowOrder( getCellRowOrder() );
    pAsset->setCellOffsetX( getCellCountX() );
    pAsset->setCellOffsetY( getCellCountY() );
    pAsset->setCellStrideX( getCellStrideX() );
    pAsset->setCellStrideY( getCellStrideY() );
    pAsset->setCellCountX( getCellCountX() );
    pAsset->setCellCountY( getCellCountY() );
    pAsset->setCellWidth( getCellWidth() );
    pAsset->setCellHeight( getCellHeight() );

    // Finish if not in explicit mode.
    if ( !getExplicitMode() )
        return;

    // Fetch the explicit cell count.
    const S32 explicitCellCount = getExplicitCellCount();

    // Finish if no explicit cells exist.
    if ( explicitCellCount == 0 )
        return;

    // Copy explicit cells.
    pAsset->clearExplicitCells();
    for( S32 index = 0; index < explicitCellCount; ++index )
    {
        // Fetch the cell pixel area.
        const FrameArea::PixelArea& pixelArea = getImageFrameArea( index ).mPixelArea;

        // Add the explicit cell.
        pAsset->addExplicitCell( pixelArea.mPixelOffset.x, pixelArea.mPixelOffset.y, pixelArea.mPixelWidth, pixelArea.mPixelHeight, pixelArea.mRegionName );
    }
}

//------------------------------------------------------------------------------

void ImageAsset::setForce16Bit( const bool force16Bit )
{
    // Ignore no change,
    if ( force16Bit == mForce16Bit )
        return;

    // Update.
    mForce16Bit = force16Bit;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setFilterMode( const ImageAsset::TextureFilterMode filterMode )
{
    // Ignore no change,
    if ( filterMode == mLocalFilterMode )
        return;

    // Update.
    mLocalFilterMode = filterMode;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setExplicitMode( const bool explicitMode )
{
    // Ignore no change,
    if ( explicitMode == mExplicitMode )
        return;

    // Update.
    mExplicitMode = explicitMode;

    // Refresh the asset.
    refreshAsset();
}    

//------------------------------------------------------------------------------

void ImageAsset::setCellRowOrder( const bool cellRowOrder )
{
    // Ignore no change.
    if ( cellRowOrder == mCellRowOrder )
        return;

    // Update.
    mCellRowOrder = cellRowOrder;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellOffsetX( const S32 cellOffsetX )
{
    // Ignore no change.
    if ( cellOffsetX == mCellOffsetX )
        return;

    // Valid?
    if ( cellOffsetX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL offset X '%d'.", cellOffsetX );
        return;
    }

    // Update.
    mCellOffsetX = cellOffsetX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellOffsetY( const S32 cellOffsetY )
{
    // Ignore no change.
    if ( cellOffsetY == mCellOffsetY )
        return;

    // Valid?
    if ( cellOffsetY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL offset Y '%d'.", cellOffsetY );
        return;
    }

    // Update.
    mCellOffsetY = cellOffsetY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellStrideX( const S32 cellStrideX )
{
    // Ignore no change.
    if ( cellStrideX == mCellStrideX )
        return;

    // Valid?
    if ( cellStrideX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL stride X '%d'.", cellStrideX );
        return;
    }

    // Update.
    mCellStrideX = cellStrideX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellStrideY( const S32 cellStrideY )
{
    // Ignore no change.
    if ( cellStrideY == mCellStrideY )
        return;

    // Valid?
    if ( cellStrideY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL stride Y '%d'.", cellStrideY );
        return;
    }

    // Update.
    mCellStrideY = cellStrideY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellCountX( const S32 cellCountX )
{
    // Ignore no change.
    if ( cellCountX == mCellCountX )
        return;

    // Valid?
    if ( cellCountX < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL count X '%d'.", cellCountX );
        return;
    }

    // Update.
    mCellCountX = cellCountX;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellCountY( const S32 cellCountY )
{
    // Ignore no change.
    if ( cellCountY == mCellCountY )
        return;

    // Valid?
    if ( cellCountY < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL count Y '%d'.", cellCountY );
        return;
    }

    // Update.
    mCellCountY = cellCountY;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellWidth( const S32 cellWidth )
{
    // Ignore no change.
    if ( cellWidth == mCellWidth )
        return;

    // Valid?
    if ( cellWidth < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL width '%d'.", cellWidth );
        return;
    }

    // Update.
    mCellWidth = cellWidth;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

void ImageAsset::setCellHeight( const S32 cellheight )
{
    // Ignore no change.
    if ( cellheight == mCellHeight )
        return;

    // Valid?
    if ( cellheight < 0 )
    {
        // No, so warn.
        Con::warnf( "Invalid CELL height '%d'.", cellheight );
        return;
    }

    // Update.
    mCellHeight = cellheight;

    // Refresh the asset.
    refreshAsset();
}

//------------------------------------------------------------------------------

Vector2 ImageAsset::getExplicitCellOffset(const S32 cellIndex)
{
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return NULL;
    }
    
    ImageAsset::FrameArea::PixelArea thisCell = mExplicitFrames.at(cellIndex);
    return(thisCell.mPixelOffset);

}

//------------------------------------------------------------------------------

S32 ImageAsset::getExplicitCellWidth(const S32 cellIndex)
{
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return (0);
    }
    
    ImageAsset::FrameArea::PixelArea thisCell = mExplicitFrames.at(cellIndex);
    return(thisCell.mPixelWidth);

}

//------------------------------------------------------------------------------

S32 ImageAsset::getExplicitCellHeight(const S32 cellIndex)
{
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return (0);
    }
    
    ImageAsset::FrameArea::PixelArea thisCell = mExplicitFrames.at(cellIndex);
    return(thisCell.mPixelHeight);

}

//------------------------------------------------------------------------------

StringTableEntry ImageAsset::getExplicitCellName(const S32 cellIndex)
{
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return NULL;
    }
    
    ImageAsset::FrameArea::PixelArea thisCell = mExplicitFrames.at(cellIndex);
    return(thisCell.mRegionName);

}

//------------------------------------------------------------------------------

S32 ImageAsset::getExplicitCellIndex(const char* regionName)
{
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return NULL;
    }
    
    // Set up a frame counter
    S32 frameCounter = 0;
    
    // Interate through the vector
    for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
    {
        // Grab the current pixelArea
        const FrameArea::PixelArea& pixelArea = *frameItr;
        
        // Check to see if the name matches the argument
        if (!dStrcmp(pixelArea.mRegionName, regionName))
        {
            // Found it, so return the frame
            return frameCounter;
        }
        else
        {
            ++frameCounter;
        }
    }
    
    // Didn't find it, so warn
    Con::warnf( "ImageAsset::getExplicitCellIndex() - Cannot find %s cell.", regionName );
    return NULL;
    
}

//------------------------------------------------------------------------------

bool ImageAsset::containsNamedRegion(const char* regionName)
{
    for( typeFrameAreaVector::iterator frameItr = mFrames.begin(); frameItr != mFrames.end(); ++frameItr )
    {
        // Grab the current pixelArea
        const FrameArea::PixelArea& pixelArea = frameItr->mPixelArea;
            
        // Check to see if the name matches the argument
        if (!dStrcmp(pixelArea.mRegionName, regionName))
        {
            // Found it, so erase it and return success
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------

bool ImageAsset::clearExplicitCells( void )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Clear explicit frames.
    mExplicitFrames.clear();

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::addExplicitCell( const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight, const char* regionName )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::addExplicitCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();
    
    // The region name cannot be empty
    if ( regionName == StringTable->EmptyString )
    {
        Con::warnf( "ImageAsset::addExplicitCell() - Cell name of '%s' is invalid or was not set.", regionName );
        U32 currentIndex = mExplicitFrames.size();
        Con::warnf( "- Setting to the next index in the frame list: '%i'", currentIndex );
        dSscanf(regionName, "%i", currentIndex);
    }

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::addExplicitCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::addExplicitCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::addExplicitCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::addExplicitCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Store frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight, regionName );
    mExplicitFrames.push_back( pixelArea );

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::insertExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight, const char* regionName )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Fetch the explicit frame count.
    const S32 explicitFramelCount = mExplicitFrames.size();
    
    // Region cannot be empty
    if ( regionName == StringTable->EmptyString )
    {
        Con::warnf( "ImageAsset::insertExplicitCell() - Cell name of '%s' is invalid or was not set.", regionName );
        Con::warnf( "- Setting to the next index in the frame list: '%i'", explicitFramelCount );
        dSscanf(regionName, "%i", explicitFramelCount);
    }

    // The cell index needs to be in range.
    if ( cellIndex < 0 )
    {
        // Warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::insertExplicitCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Configure frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight, regionName );

    // Insert frame appropriately.
    if ( cellIndex >= explicitFramelCount )
    {
        mExplicitFrames.push_back( pixelArea );
    }
    else
    {
        mExplicitFrames.insert( cellIndex );
        mExplicitFrames[cellIndex] = pixelArea;
    }

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::setExplicitCell( const S32 cellIndex, const S32 cellOffsetX, const S32 cellOffsetY, const S32 cellWidth, const S32 cellHeight, const char* regionName )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Fetch the explicit frame count.
    const S32 explicitFrameCount = mExplicitFrames.size();
    
    // Region cannot be empty
    if ( regionName == StringTable->EmptyString )
    {
        Con::warnf( "ImageAsset::setExplicitCell() - Cell name of '%s' is invalid or was not set.", regionName );
        Con::warnf( "- Setting to the next index in the frame list: '%i'", explicitFrameCount );
        dSscanf(regionName, "%i", explicitFrameCount);
    }

    // The cell index needs to be in range.
    if ( cellIndex < 0 || cellIndex >= explicitFrameCount )
    {
        // Warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // The Cell Offset X needs to be within the image.
    if ( cellOffsetX < 0 || cellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Invalid Cell OffsetX of %d.", cellOffsetX );
        return false;
    }

    // The Cell Offset Y needs to be within the image.
    if ( cellOffsetY < 0 || cellOffsetY >= imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Invalid Cell OffsetY of %d.", cellOffsetY );
        return false;
    }

    // The Cell Width needs to be within the image.
    if ( cellWidth <= 0 || (cellOffsetX+cellWidth) > imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Invalid Cell Width of %d.", cellWidth );
        return false;
    }

    // The Cell Height needs to be within the image.
    if ( cellHeight <= 0 || (cellOffsetY+cellHeight) > imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::setExplicitCell() - Invalid Cell Width of %d.", cellHeight );
        return false;
    }

    // Configure frame.
    FrameArea::PixelArea pixelArea( cellOffsetX, cellOffsetY, cellWidth, cellHeight, regionName );

    // Set cell.
    mExplicitFrames[cellIndex] = pixelArea;

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::removeExplicitCell( const S32 cellIndex )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::removeExplicitCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Fetch the explicit frame count.
    const S32 explicitFrameCount = mExplicitFrames.size();

    // The cell index needs to be in range.
    if ( cellIndex < 0 || cellIndex >= explicitFrameCount )
    {
        // Warn.
        Con::warnf( "ImageAsset::removeExplicitCell() - Invalid Cell Index of %d.", cellIndex );
        return false;
    }

    // Remove cell.
    mExplicitFrames.erase(cellIndex);

    // Refresh the asset.
    refreshAsset();

    return true;
}

//------------------------------------------------------------------------------

bool ImageAsset::removeExplicitCell( const char* regionName )
{
    // Are we in explicit mode?
    if ( !getExplicitMode() )
    {
        // No, so warn.
        Con::warnf( "ImageAsset::removeExplicitCell() - Cannot perform explicit cell operation when not in explicit mode." );
        return false;
    }

    // Interate through the vector
    for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
    {
        // Grab the current pixelArea
        const FrameArea::PixelArea& pixelArea = *frameItr;
        
        // Check to see if the name matches the argument
        if (!dStrcmp(pixelArea.mRegionName, regionName))
        {
            // Found it, so erase it and return success
            mExplicitFrames.erase(frameItr);
            return true;
        }
    }

    // Didn't find it, so warn
    Con::warnf( "ImageAsset::removeExplicitCell() - Cannot find %s cell to remove.", regionName );
    return false;
}

//------------------------------------------------------------------------------

ImageAsset::FrameArea& ImageAsset::getCellByName( const char* cellName)
{
    // If the cellName was empty
    if (cellName == StringTable->EmptyString)
    {
        // Warn and return a bad frame
        Con::warnf( "ImageAsset::getCellByName() - Empty cell name was passed." );
        return BadFrameArea;
    }
    
    for( typeFrameAreaVector::iterator frameItr = mFrames.begin(); frameItr != mFrames.end(); ++frameItr )
    {
        // Grab the current pixelArea
        const FrameArea::PixelArea& pixelArea = frameItr->mPixelArea;
        
        // Check to see if the name matches the argument
        if (!dStrcmp(pixelArea.mRegionName, cellName))
        {
            // Found it, so erase it and return success
            return *frameItr;
        }
    }

    // Didn't find it, so warn and return a bad frame
    Con::warnf( "ImageAsset::getCellByName() - Cannot find %s cell.", cellName );
    return BadFrameArea;
}

//------------------------------------------------------------------------------

void ImageAsset::setTextureFilter( const TextureFilterMode filterMode )
{
    // Finish if no texture.
    if ( mImageTextureHandle.IsNull() )
        return;

    // Select Hardware Filter Mode.
    GLint glFilterMode;

    switch( filterMode )
    {
        // Nearest ("none").
        case FILTER_NEAREST:
        {
            glFilterMode = GL_NEAREST;

        } break;

        // Bilinear ("smooth").
        case FILTER_BILINEAR:
        {
            glFilterMode = GL_LINEAR;

        } break;

        // Huh?
        default:
            // Oh well...
            glFilterMode = GL_LINEAR;
    };

    // Set the texture objects filter mode.
    mImageTextureHandle.setFilter( glFilterMode );
}

//------------------------------------------------------------------------------

void ImageAsset::initializeAsset( void )
{
    // Call parent.
    Parent::initializeAsset();

    // Ensure the image-file is expanded.
    mImageFile = expandAssetFilePath( mImageFile );

    // Calculate the image.
    calculateImage();
}

//------------------------------------------------------------------------------

void ImageAsset::onAssetRefresh( void ) 
{
    // Ignore if not yet added to the sim.
    if ( !isProperlyAdded() )
        return;

    // Call parent.
    Parent::onAssetRefresh();
    
    // Compile image.
    calculateImage();
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlPreWrite( void )
{
    // Call parent.
    Parent::onTamlPreWrite();

    // Ensure the image-file is collapsed.
    mImageFile = collapseAssetFilePath( mImageFile );
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlPostWrite( void )
{
    // Call parent.
    Parent::onTamlPostWrite();

    // Ensure the image-file is expanded.
    mImageFile = expandAssetFilePath( mImageFile );
}

//------------------------------------------------------------------------------

void ImageAsset::calculateImage( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateImage);

    // Clear frames.
    mFrames.clear();

    // If we have an existing texture and we're setting to the same bitmap then force the texture manager
    // to refresh the texture itself.
    if ( !mImageTextureHandle.IsNull() && dStricmp(mImageTextureHandle.getTextureKey(), mImageFile) == 0 )
        TextureManager::refresh( mImageFile );

    // Get image texture.
    mImageTextureHandle.set( mImageFile, TextureHandle::BitmapTexture, true, getForce16Bit() );

    // Is the texture valid?
    if ( mImageTextureHandle.IsNull() )
    {
        // No, so warn.
        Con::warnf( "Image '%s' could not load texture '%s'.", getAssetId(), mImageFile );
        return;
    }

    // Is the local filter mode specified?
    if ( mLocalFilterMode != FILTER_INVALID )
    {
        // Yes, so set filter mode.
        setTextureFilter( mLocalFilterMode );
    }
    else
    {
        TextureFilterMode filterMode = FILTER_NEAREST;

        // No, so fetch the global filter.
        const char* pGlobalFilter = Con::getVariable( "$pref::T2D::imageAssetGlobalFilterMode" );

        // Fetch the global filter mode.
        if ( pGlobalFilter != NULL && dStrlen(pGlobalFilter) > 0 )
            filterMode = getFilterModeEnum( pGlobalFilter );

        // If global filter mode is invalid then use local filter mode.
        if ( filterMode == FILTER_INVALID )
            filterMode = FILTER_NEAREST;

        // Set filter mode.
        setTextureFilter( filterMode );
    }

    // Calculate according to mode.
    if ( mExplicitMode )
    {
        calculateExplicitMode();
    }
    else
    {
        calculateImplicitMode();
    }
}

//------------------------------------------------------------------------------

void ImageAsset::calculateImplicitMode( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateImplicitMode);

    // Sanity!
    AssertFatal( !mExplicitMode, "Cannot calculate implicit cells when in explicit mode." );

    // Fetch the texture object.
    TextureObject* pTextureObject = ((TextureObject*)mImageTextureHandle);
 
    // Calculate texel scales.
    const F32 texelWidthScale = 1.0f / (F32)pTextureObject->getTextureWidth();
    const F32 texelHeightScale = 1.0f / (F32)pTextureObject->getTextureHeight();

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Set full-frame as default.
    FrameArea frameArea( 0, 0, imageWidth, imageHeight, texelWidthScale, texelHeightScale );
    mFrames.push_back( frameArea );

    // Finish if no cell counts are specified.  This is how we default to full-frame mode.
    if ( mCellCountX < 1 || mCellCountY < 1 )
        return;

    // The cell width needs to be at maximum the image width!
    if ( mCellWidth < 1 || mCellWidth > imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell Width of %d.", mCellWidth );
        return;
    }

    // The cell height needs to be at maximum the image height!
    if ( mCellHeight < 1 || mCellHeight > imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell Height of %d.", mCellHeight );
        return;
    }

    // The Cell Offset X needs to be within the image.
    if ( mCellOffsetX < 0 || mCellOffsetX >= imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetX of %d.", mCellOffsetX );
        return;
    }

    // The Cell Offset Y needs to be within the image.
    if ( mCellOffsetY < 0 || mCellOffsetY >= imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetY of %d.", mCellOffsetY );
        return;
    }

    // Are we using Cell-StrideX?
    S32 cellStepX;
    if ( mCellStrideX != 0 )
    {
        // Yes, so set stepX to be StrideX.
        cellStepX = mCellStrideX;
    }
    else
    {
        // No, so set stepY to be Cell Width.
        cellStepX = mCellWidth;
    }

    // Are we using Cell-StrideY?
    S32 cellStepY;
    if ( mCellStrideY != 0 )
    {
        // Yes, so set stepY to be StrideY.
        cellStepY = mCellStrideY;
    }
    else
    {
        // No, so set stepY to be Cell Height.
        cellStepY = mCellHeight;
    }

    // Calculate Final Cell Position X.
    S32 cellFinalPositionX = mCellOffsetX + ((mCellCountX-((cellStepX<0)?1:0))*cellStepX);
    // Off Left?
    if ( cellFinalPositionX < 0 )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off image left-hand-side.", mCellOffsetX, mCellWidth, mCellCountX );
        return;
    }
    // Off Right?
    else if ( cellFinalPositionX > imageWidth )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetX(%d)/Width(%d)/CountX(%d); off image right-hand-side.", mCellOffsetX, mCellWidth, mCellCountX );
        return;
    }

    // Calculate Final Cell Position Y.
    S32 cellFinalPositionY = mCellOffsetY + ((mCellCountY-((cellStepY<0)?1:0))*cellStepY);
    // Off Top?
    if ( cellFinalPositionY < 0 )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off image top-side.", mCellOffsetY, mCellHeight, mCellCountY );
        return;
    }
    // Off Bottom?
    else if ( cellFinalPositionY > imageHeight )
    {
        // Warn.
        Con::warnf( "ImageAsset::calculateImage() - Invalid Cell OffsetY(%d)/Height(%d)/CountY(%d); off image bottom-side.", mCellOffsetY, mCellHeight, mCellCountY );
        return;
    }

    // Clear default frame.
    mFrames.clear();

    // Cell Row Order?
    if ( mCellRowOrder )
    {
        // Yes, so RowRow Order.
        for ( S32 y = 0, cellPositionY = mCellOffsetY; y < mCellCountY; y++, cellPositionY+=cellStepY )
        {
            for ( S32 x = 0, cellPositionX = mCellOffsetX; x < mCellCountX; x++, cellPositionX+=cellStepX )
            {
                // Set frame area.
                frameArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight, texelWidthScale, texelHeightScale );

                // Store fame.
                mFrames.push_back( frameArea );
            }
        }

        return;
    }

    // No, so Column Order.
    for ( S32 x = 0, cellPositionX = mCellOffsetX; x < mCellCountX; x++, cellPositionX+=cellStepX )
    {
        for ( S32 y = 0, cellPositionY = mCellOffsetY; y < mCellCountY; y++, cellPositionY+=cellStepY )
        {
            // Set frame area.
            frameArea.setArea( cellPositionX, cellPositionY, mCellWidth, mCellHeight, texelWidthScale, texelHeightScale );

            // Store fame.
            mFrames.push_back( frameArea );
        }
    }
}

//------------------------------------------------------------------------------

void ImageAsset::calculateExplicitMode( void )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_CalculateExplicitMode);

    // Sanity!
    AssertFatal( mExplicitMode, "Cannot calculate explicit cells when not in explicit mode." );

    // Fetch the texture object.
    TextureObject* pTextureObject = ((TextureObject*)mImageTextureHandle);
 
    // Calculate texel scales.
    const F32 texelWidthScale = 1.0f / (F32)pTextureObject->getTextureWidth();
    const F32 texelHeightScale = 1.0f / (F32)pTextureObject->getTextureHeight();

    // Fetch the original image dimensions.
    const S32 imageWidth = getImageWidth();
    const S32 imageHeight = getImageHeight();

    // Clear default frame.
    mFrames.clear();

    // Are any explicit frames set.
    if ( mExplicitFrames.size() == 0 )
    {
        // No, so set full-frame as default.
        FrameArea frameArea( 0, 0, imageWidth, imageHeight, texelWidthScale, texelHeightScale );
        mFrames.push_back( frameArea );

        return;
    }

    // Iterate explicit frames.
    for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
    {
        // Fetch pixel area.
        const FrameArea::PixelArea& pixelArea = *frameItr;

        // Set frame area.
        FrameArea frameArea( pixelArea.mPixelOffset.x, pixelArea.mPixelOffset.y, pixelArea.mPixelWidth, pixelArea.mPixelHeight, texelWidthScale, texelHeightScale, pixelArea.mRegionName );

        // Store frame.
        mFrames.push_back( frameArea );
    }
}

//------------------------------------------------------------------------------

bool ImageAsset::setFilterMode( void* obj, const char* data )
{
    static_cast<ImageAsset*>(obj)->setFilterMode(getFilterModeEnum(data));
    return false;
}

//------------------------------------------------------------------------------

void ImageAsset::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_OnTamlCustomWrite);

    // Call parent.
    Parent::onTamlCustomWrite( customNodes );

    // Finish if not in explicit mode.
    if ( !mExplicitMode )
        return;

    if (mExplicitFrames.size() > 0)
    {
        // Add cell custom node.
        TamlCustomNode* pCustomCellNodes = customNodes.addNode( cellCustomNodeCellsName );

        // Iterate explicit frames.
        for( typeExplicitFrameAreaVector::iterator frameItr = mExplicitFrames.begin(); frameItr != mExplicitFrames.end(); ++frameItr )
        {
            // Fetch pixel area.
            const FrameArea::PixelArea& pixelArea = *frameItr;

            // Add cell alias.
            TamlCustomNode* pCellNode = pCustomCellNodes->addNode( cellNodeName );

            // Add cell properties.
            pCellNode->addField( cellRegionName, pixelArea.mRegionName );
            pCellNode->addField( cellOffsetName, pixelArea.mPixelOffset );
            pCellNode->addField( cellWidthName, pixelArea.mPixelWidth );
            pCellNode->addField( cellHeightName, pixelArea.mPixelHeight );
        }
    }
}

//-----------------------------------------------------------------------------

void ImageAsset::onTamlCustomRead( const TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(ImageAsset_OnTamlCustomRead);
    
    // Call parent.
    Parent::onTamlCustomRead( customNodes );
    
    // Find cell custom node.
    const TamlCustomNode* pCustomCellNodes = customNodes.findNode( cellCustomNodeCellsName );
    
    // Continue if we have explicit cells.
    if ( pCustomCellNodes != NULL )
    {
        // Set explicit mode.
        mExplicitMode = true;
        
        // Fetch children cell nodes.
        const TamlCustomNodeVector& cellNodes = pCustomCellNodes->getChildren();
        
        // Iterate cells.
        for( TamlCustomNodeVector::const_iterator cellNodeItr = cellNodes.begin(); cellNodeItr != cellNodes.end(); ++cellNodeItr )
        {
            // Fetch cell node.
            TamlCustomNode* pCellNode = *cellNodeItr;
            
            // Fetch node name.
            StringTableEntry nodeName = pCellNode->getNodeName();
            
            // Is this a valid alias?
            if ( nodeName != cellNodeName )
            {
                // No, so warn.
                Con::warnf( "ImageAsset::onTamlCustomRead() - Encountered an unknown custom name of '%s'.  Only '%s' is valid.", nodeName, cellNodeName );
                continue;
            }
            
            Point2I cellOffset(-1, -1);
            S32 cellWidth = 0;
            S32 cellHeight = 0;
            const char* regionName = StringTable->EmptyString;
            
            // Fetch fields.
            const TamlCustomFieldVector& fields = pCellNode->getFields();
            
            // Iterate property fields.
            for ( TamlCustomFieldVector::const_iterator fieldItr = fields.begin(); fieldItr != fields.end(); ++fieldItr )
            {
                // Fetch field.
                const TamlCustomField* pField = *fieldItr;
                
                // Fetch field name.
                StringTableEntry fieldName = pField->getFieldName();
                
                // Check common fields.
                if ( fieldName == cellRegionName )
                {
                    regionName = pField->getFieldValue();
                }
                else if ( fieldName == cellOffsetName )
                {
                    pField->getFieldValue( cellOffset );
                }
                else if ( fieldName == cellOffsetXName )
                {
                    pField->getFieldValue( cellOffset.x );
                }
                else if ( fieldName == cellOffsetYName )
                {
                    pField->getFieldValue( cellOffset.y );
                }
                else if ( fieldName == cellWidthName )
                {
                    pField->getFieldValue( cellWidth );
                }
                else if ( fieldName == cellHeightName )
                {
                    pField->getFieldValue( cellHeight );
                }
                else
                {
                    // Unknown name so warn.
                    Con::warnf( "ImageAsset::onTamlCustomRead() - Encountered an unknown custom field name of '%s'.", fieldName );
                    continue;
                }
            }
            
            // Does the region have a name
            if ( regionName == StringTable->EmptyString )
            {
                // No, so warn and set it to the next index
                Con::warnf( "ImageAsset::onTamlCustomRead() - Cell name of '%s' is invalid or was not set.", regionName );
                
                U32 currentIndex = mExplicitFrames.size();
                Con::warnf( "- Setting to the next index in the frame list: '%i'", currentIndex );
                
                dSscanf(regionName, "%i", currentIndex);
            }
            
            // Is cell offset valid?
            if ( cellOffset.x < 0 || cellOffset.y < 0 )
            {
                // No, so warn.
                Con::warnf( "ImageAsset::onTamlCustomRead() - Cell offset of '(%d,%d)' is invalid or was not set.", cellOffset.x, cellOffset.y );
                continue;
            }
            
            // Is cell width valid?
            if ( cellWidth <= 0 )
            {
                // No, so warn.
                Con::warnf( "ImageAsset::onTamlCustomRead() - Cell width of '%d' is invalid or was not set.", cellWidth );
                continue;
            }
            
            // Is cell height valid?
            if ( cellHeight <= 0 )
            {
                // No, so warn.
                Con::warnf( "ImageAsset::onTamlCustomRead() - Cell height of '%d' is invalid or was not set.", cellHeight );
                continue;
            }
            
            // Add explicit frame.
            FrameArea::PixelArea pixelArea( cellOffset.x, cellOffset.y, cellWidth, cellHeight, regionName );
            mExplicitFrames.push_back( pixelArea );
        }
    }
}

//-----------------------------------------------------------------------------

static void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "ImageAsset::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "ImageAsset::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create ImageAsset node element.
    TiXmlElement* pImageAssetNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), cellCustomNodeCellsName );
    pImageAssetNodeElement->SetAttribute( "name", buffer );
    pImageAssetNodeElement->SetAttribute( "minOccurs", 0 );
    pImageAssetNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pImageAssetNodeElement );

    // Create complex type.
    TiXmlElement* pImageAssetNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pImageAssetNodeElement->LinkEndChild( pImageAssetNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pImageAssetNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pImageAssetNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pImageAssetNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pImageAssetNodeComplexTypeElement->LinkEndChild( pImageAssetNodeChoiceElement );

    // Create ImageAsset element.
    TiXmlElement* pImageAssetElement = new TiXmlElement( "xs:element" );
    pImageAssetElement->SetAttribute( "name", cellNodeName );
    pImageAssetElement->SetAttribute( "minOccurs", 0 );
    pImageAssetElement->SetAttribute( "maxOccurs", 1 );
    pImageAssetNodeChoiceElement->LinkEndChild( pImageAssetElement );

    // Create complex type Element.
    TiXmlElement* pImageAssetComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pImageAssetElement->LinkEndChild( pImageAssetComplexTypeElement );

    // Create "RegionName" attribute.
    TiXmlElement* pImageRegionName = new TiXmlElement( "xs:attribute" );
    pImageRegionName->SetAttribute( "name", cellRegionName );
    pImageRegionName->SetAttribute( "type", "xs:string" );
    pImageAssetComplexTypeElement->LinkEndChild( pImageRegionName );
    
    // Create "Offset" attribute.
    TiXmlElement* pImageAssetOffset = new TiXmlElement( "xs:attribute" );
    pImageAssetOffset->SetAttribute( "name", cellOffsetName );
    pImageAssetOffset->SetAttribute( "type", "Point2I_ConsoleType" );
    pImageAssetComplexTypeElement->LinkEndChild( pImageAssetOffset );

    // Create "Width" attribute.
    TiXmlElement* pImageAssetWidth = new TiXmlElement( "xs:attribute" );
    pImageAssetWidth->SetAttribute( "name", cellWidthName );
    pImageAssetWidth->SetAttribute( "type", "xs:unsignedInt" );
    pImageAssetComplexTypeElement->LinkEndChild( pImageAssetWidth );

    // Create "Height" attribute.
    TiXmlElement* pImageAssetHeight = new TiXmlElement( "xs:attribute" );
    pImageAssetHeight->SetAttribute( "name", cellHeightName );
    pImageAssetHeight->SetAttribute( "type", "xs:unsignedInt" );
    pImageAssetComplexTypeElement->LinkEndChild( pImageAssetHeight );
}

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT_SCHEMA(ImageAsset, WriteCustomTamlSchema);
