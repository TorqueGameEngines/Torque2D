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

ConsoleMethodGroupBeginWithDocs(ImageAsset, AssetBase)

/*! Sets the image file (bitmap file).
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setImageFile, ConsoleVoid, 3, 3, (ImageFile))
{
    object->setImageFile( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the image file.
    @return Returns the bitmap image file.
*/
ConsoleMethodWithDocs(ImageAsset, getImageFile, ConsoleString, 2, 2, ())
{
    return object->getImageFile();
}

//------------------------------------------------------------------------------

/*! Sets the filter mode.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setFilterMode, ConsoleVoid, 3, 3, (mode))
{
    // Fetch Texture Filter Mode.
    const ImageAsset::TextureFilterMode filterMode = ImageAsset::getFilterModeEnum( argv[2] );

    // Valid Filter?
    if ( filterMode == ImageAsset::FILTER_INVALID )
    {
        // Warn.
        Con::warnf("ImageAsset::setFilterMode() - Invalid Filter Mode Specified! (%s)", argv[2] );
        // Finish Here.
        return;
    }

    // Set Filter Mode.
    object->setFilterMode( filterMode );
}

//------------------------------------------------------------------------------

/*! Gets the filter mode.
    @return The filter mode.
*/
ConsoleMethodWithDocs(ImageAsset, getFilterMode, ConsoleString, 2, 2, ())
{
    return ImageAsset::getFilterModeDescription( object->getFilterMode() );
}

//-----------------------------------------------------------------------------

/*! Sets whether 16-bit image is forced or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setForce16Bit, ConsoleVoid, 3, 3, (force16Bit?))
{
    object->setForce16Bit( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether 16-bit image is forced or not.
    @return Whether 16-bit image is forced or not.
*/
ConsoleMethodWithDocs(ImageAsset, getForce16Bit, ConsoleBool, 2, 2, ())
{
    return object->getForce16Bit();
}

//-----------------------------------------------------------------------------

/*! Sets whether CELL row order should be used or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellRowOrder, ConsoleVoid, 3, 3, (rowOrder?))
{
    object->setCellRowOrder( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether CELL row order should be used or not.
    @return Whether CELL row order should be used or not.
*/
ConsoleMethodWithDocs(ImageAsset, getCellRowOrder, ConsoleBool, 2, 2, ())
{
    return object->getCellRowOrder();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL offset X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellOffsetX, ConsoleVoid, 3, 3, (offsetX))
{
    object->setCellOffsetX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset X.
    @return The CELL offset X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellOffsetX, ConsoleInt, 2, 2, ())
{
    return object->getCellOffsetX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL offset Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellOffsetY, ConsoleVoid, 3, 3, (offsetY))
{
    object->setCellOffsetY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset Y.
    @return The CELL offset Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellOffsetY, ConsoleInt, 2, 2, ())
{
    return object->getCellOffsetY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL stride X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellStrideX, ConsoleVoid, 3, 3, (strideX))
{
    object->setCellStrideX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL stride X.
    @return The CELL stride X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellStrideX, ConsoleInt, 2, 2, ())
{
    return object->getCellStrideX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL stride Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellStrideY, ConsoleVoid, 3, 3, (strideY))
{
    object->setCellStrideY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL stride Y.
    @return The CELL stride Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellStrideY, ConsoleInt, 2, 2, ())
{
    return object->getCellStrideY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL count X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellCountX, ConsoleVoid, 3, 3, (countX))
{
    object->setCellCountX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL count X.
    @return The CELL count X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellCountX, ConsoleInt, 2, 2, ())
{
    return object->getCellCountX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL count Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellCountY, ConsoleVoid, 3, 3, (countY))
{
    object->setCellCountY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL count Y.
    @return The CELL count Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellCountY, ConsoleInt, 2, 2, ())
{
    return object->getCellCountY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL width.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellWidth, ConsoleVoid, 3, 3, (Width))
{
    object->setCellWidth( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL width.
    @return The CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getCellWidth, ConsoleInt, 2, 2, ())
{
    return object->getCellWidth();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL height.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellHeight, ConsoleVoid, 3, 3, (Height))
{
    object->setCellHeight( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL height.
    @return The CELL height.
*/
ConsoleMethodWithDocs(ImageAsset, getCellHeight, ConsoleInt, 2, 2, ())
{
    return object->getCellHeight();
}

//-----------------------------------------------------------------------------

/*! Gets the image width.
    @return The image width.
*/
ConsoleMethodWithDocs(ImageAsset, getImageWidth, ConsoleInt, 2, 2, ())
{
    return object->getImageWidth();
}

//-----------------------------------------------------------------------------

/*! Gets the image height.
    @return The image width.
*/
ConsoleMethodWithDocs(ImageAsset, getImageHeight, ConsoleInt, 2, 2, ())
{
    return object->getImageHeight();
}

//-----------------------------------------------------------------------------

/*! Gets the image size.
    @return The image size.
*/
ConsoleMethodWithDocs(ImageAsset, getImageSize, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getImageWidth(), object->getImageHeight());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets whether the image has a power-of-two dimensions or not.
    @return Whether the image has a power-of-two dimensions or not.
*/
ConsoleMethodWithDocs(ImageAsset, getIsImagePOT, ConsoleBool, 2, 2, ())
{
    return isPow2( object->getImageWidth() ) && isPow2( object->getImageHeight() );
}

//-----------------------------------------------------------------------------

/*! Gets the frame count.
    @return The frame count.
*/
ConsoleMethodWithDocs(ImageAsset, getFrameCount, ConsoleInt, 2, 2, ())
{
    return object->getFrameCount(); 
}

//-----------------------------------------------------------------------------

/*! Gets the specified frames size.
    @param frame The frame number to use.
    @return The specified frames size.
*/
ConsoleMethodWithDocs(ImageAsset, getFrameSize, ConsoleString, 3, 3, (int frame))
{
    // Fetch Frame.
    const S32 frame = dAtoi(argv[2]);

    // Check Frame.
    if ( frame < 0 || frame >= (S32)object->getFrameCount() )
    {
        // Warn.
        Con::warnf("ImageAsset::getFrameSize() - Invalid Frame; Allowed range is 0 to %d", object->getFrameCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Fetch Selected Frame Pixel Area.
    const ImageAsset::FrameArea::PixelArea& framePixelArea = object->getImageFrameArea( frame ).mPixelArea;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", framePixelArea.mPixelWidth, framePixelArea.mPixelHeight );

    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Clears all explicit cells.
    The image asset stays in explicit mode however with no explicit cells a single full-frame cell becomes default.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, clearExplicitCells, ConsoleBool, 2, 2, ())
{
    return object->clearExplicitCells();
}

//-----------------------------------------------------------------------------

/*! Add an explicit cell.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, addExplicitCell, ConsoleBool, 7, 7, (int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[2] );
    const S32 cellOffsetY = dAtoi( argv[3] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[4] );
    const S32 cellHeight = dAtoi (argv[5] );

    return object->addExplicitCell( cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[6] );
}

//-----------------------------------------------------------------------------

/*! Insert an explicit cell at the specified index.
    @param cellIndex The zero-based index to insert the cell.  This will work when no cells are present.  If the index is beyond the cell count then the cell is simply added.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, insertExplicitCell, ConsoleBool, 8, 8, (int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[3] );
    const S32 cellOffsetY = dAtoi( argv[4] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[5] );
    const S32 cellHeight = dAtoi (argv[6] );

    return object->insertExplicitCell( cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[7] );
}


//-----------------------------------------------------------------------------

/*! Remove an explicit cell from the specified index.
    @param cellIndex The zero-based index to remove the cell from.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, removeExplicitCell, ConsoleBool, 3, 3, (int cellIndex))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    return object->removeExplicitCell( cellIndex );
}

//-----------------------------------------------------------------------------

/*! Set an explicit cell at the specified index.
    @param cellIndex The zero-based index to set the cell.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, setExplicitCell, ConsoleBool, 8, 8, (int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[3] );
    const S32 cellOffsetY = dAtoi( argv[4] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[5] );
    const S32 cellHeight = dAtoi (argv[6] );

    return object->setExplicitCell( cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[7] );
}

//-----------------------------------------------------------------------------

/*! Gets the explicit cell count.
    @return The explicit cell count.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellCount, ConsoleInt, 2, 2, ())
{
    return object->getExplicitCellCount();
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific offset.
    @return The specified CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellOffset, ConsoleString, 3, 3, (cell))
{
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        const S32 cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellOffset(cellIndex).scriptThis();
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        const Vector2 offset = frameRegion.mPixelArea.mPixelOffset;
        
        return offset.scriptThis();
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL width in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific width.
    @return The specified CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellWidth, ConsoleInt, 3, 3, (cell))
{
    S32 cellIndex;
    
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellWidth(cellIndex);
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        return frameRegion.mPixelArea.mPixelWidth;
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL height in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific height.
    @return The specified CELL height.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellHeight, ConsoleInt, 3, 3, (cell))
{
    S32 cellIndex;
    
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellHeight(cellIndex);
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        return frameRegion.mPixelArea.mPixelHeight;
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL region name in Explicit Mode.
    @param cell The cell index to use to find the specific name.
    @return The specified CELL region name.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellName, ConsoleString, 3, 3, (cell))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi(argv[2]);

    return object->getExplicitCellName(cellIndex);
}

//-----------------------------------------------------------------------------

/*! Gets the CELL index number in Explicit Mode.
    @param cellName The cell name to use to find the specific index.
    @return The specified CELL index number or -1 if no such cell exists.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellIndex, ConsoleInt, 3, 3, (cellName))
{
    return object->getExplicitCellIndex( argv[2] );
}

/*! Gets the status of Explicit mode
@return Returns true if ExplicitMode is on, false otherwise.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitMode, ConsoleBool, 2, 2, ())
{
    return object->getExplicitMode();
}

/*! Sets Explicit Mode
@param explicitMode Whether to set Explicit Mode or disable it
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setExplicitMode, ConsoleVoid, 3, 3, (explicitMode))
{
    object->setExplicitMode(dAtob(argv[2]));
}

/*! Adds a layer to the ImageAsset
@param imageFile The file for the image to layer over the texture.
@param position The position for the layer to be at with (0, 0) in the top left corner.
@param blendColor The blending color to be applied to this new layer.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, addLayer, ConsoleVoid, 5, 6, (imageFile, position, blendColor, [doRedraw]))
{
    if (argc < 5)
    {
        Con::warnf("ImageAsset::addLayer() - Invalid number of parameters!");
        return;
    }

    F32 x, y;
    x = 0.0f;
    y = 0.0f;
    if (Utility::mGetStringElementCount(argv[3]) >= 2)
    {
        x = dAtof(Utility::mGetStringElement(argv[3], 0));
        y = dAtof(Utility::mGetStringElement(argv[3], 1));
    }
    Point2I pos(x, y);

    const U32 colorCount = Utility::mGetStringElementCount(argv[4]);
    if (colorCount != 4)
    {
        Con::warnf("Scene::addLayer() - Invalid color! Colors require four values (red / green / blue / alpha)!");
        return;
    }
    ColorF blendColor = ColorF(dAtof(Utility::mGetStringElement(argv[4], 0)),
        dAtof(Utility::mGetStringElement(argv[4], 1)),
        dAtof(Utility::mGetStringElement(argv[4], 2)),
        dAtof(Utility::mGetStringElement(argv[4], 3)));

    bool redraw = true;
    if (argc == 6)
    {
        redraw = dAtob(argv[5]);
    }

    object->addLayer(argv[2], pos, blendColor, redraw);
}

/*! Inserts a layer into the stack of layers on a texture at a given index.
@param index The one-based index to insert the new layer at. Must be less or equal to the number of layers.
@param imageFile The file for the image to layer over the texture.
@param position The position for the layer to be at with (0, 0) in the top left corner.
@param blendColor The blending color to be applied to this new layer.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, insertLayer, ConsoleVoid, 6, 7, (index, imageFile, position, blendColor, [doRedraw]))
{
    if (argc < 6)
    {
        Con::warnf("ImageAsset::insertLayer() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    F32 x, y;
    x = 0.0f;
    y = 0.0f;
    if (Utility::mGetStringElementCount(argv[4]) >= 2)
    {
        x = dAtof(Utility::mGetStringElement(argv[4], 0));
        y = dAtof(Utility::mGetStringElement(argv[4], 1));
    }
    Point2I pos(x, y);

    const U32 colorCount = Utility::mGetStringElementCount(argv[5]);
    if (colorCount != 4)
    {
        Con::warnf("Scene::insertLayer() - Invalid color! Colors require four values (red / green / blue / alpha)!");
        return;
    }
    ColorF blendColor = ColorF(dAtof(Utility::mGetStringElement(argv[5], 0)),
        dAtof(Utility::mGetStringElement(argv[5], 1)),
        dAtof(Utility::mGetStringElement(argv[5], 2)),
        dAtof(Utility::mGetStringElement(argv[5], 3)));

    bool redraw = true;
    if (argc == 7)
    {
        redraw = dAtob(argv[6]);
    }

    object->insertLayer(index, argv[3], pos, blendColor, redraw);
}

/*! Removes a layer from a texture at a given index.
@param index The one-based index to remove the new layer from. Must be less or equal to the number of layers.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, removeLayer, ConsoleVoid, 3, 4, (index, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::removeLayer() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->removeLayer(index, redraw);
}

/*! Moves the layer at the index forward.
@param index The one-based index to move forward. Must be less or equal to the number of layers.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, moveLayerForward, ConsoleVoid, 3, 4, (index, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::moveLayerForward() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->moveLayerForward(index, redraw);
}

/*! Moves the layer at the index backward.
@param index The one-based index to move backward. Must be less or equal to the number of layers.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, moveLayerBackward, ConsoleVoid, 3, 4, (index, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::moveLayerBackward() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->moveLayerBackward(index, redraw);
}

/*! Moves the layer at the index to the front.
@param index The one-based index to move to the front. Must be less or equal to the number of layers.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, moveLayerToFront, ConsoleVoid, 3, 4, (index, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::moveLayerToFront() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->moveLayerToFront(index, redraw);
}

/*! Moves the layer at the index to the back.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, moveLayerToBack, ConsoleVoid, 3, 4, (index, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::moveLayerToBack() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->moveLayerToBack(index, redraw);
}

/*! Sets the image for a layer at the given index.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@param image The path to an image to be used by the layer.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setLayerImage, ConsoleVoid, 4, 5, (index, image, [doRedraw]))
{
    if (argc < 4)
    {
        Con::warnf("ImageAsset::setLayerImage() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    bool redraw = true;
    if (argc == 5)
    {
        redraw = dAtob(argv[4]);
    }

    object->setLayerImage(index, argv[3], redraw);
}

/*! Sets the position for a layer at the given index.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@param position The space-deliminated (x, y) position to place the image with the top left corner as (0, 0). The layer can safely overflow the bounds of the original image.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setLayerPosition, ConsoleVoid, 4, 5, (index, position, [doRedraw]))
{
    if (argc < 4)
    {
        Con::warnf("ImageAsset::setLayerPosition() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    F32 x, y;
    x = 0.0f;
    y = 0.0f;
    if (Utility::mGetStringElementCount(argv[3]) >= 2)
    {
        x = dAtof(Utility::mGetStringElement(argv[3], 0));
        y = dAtof(Utility::mGetStringElement(argv[3], 1));
    }
    Point2I pos(x, y);

    bool redraw = true;
    if (argc == 5)
    {
        redraw = dAtob(argv[4]);
    }

    object->setLayerPosition(index, pos, redraw);
}

/*! Sets the blend color for a layer at the given index.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@param blendColor The space-deliminated color used to blend the layer. Requires four decimal values between 0 and 1 that represent red, green, blue, and alpha.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setLayerBlendColor, ConsoleVoid, 4, 5, (index, blendColor, [doRedraw]))
{
    if (argc < 4)
    {
        Con::warnf("ImageAsset::setLayerBlendColor() - Invalid number of parameters!");
        return;
    }

    U32 index = dAtoi(argv[2]);

    const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
    if (colorCount != 4)
    {
        Con::warnf("Scene::setLayerBlendColor() - Invalid color! Colors require four values (red / green / blue / alpha)!");
        return;
    }
    ColorF blendColor = ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)),
        dAtof(Utility::mGetStringElement(argv[3], 1)),
        dAtof(Utility::mGetStringElement(argv[3], 2)),
        dAtof(Utility::mGetStringElement(argv[3], 3)));

    bool redraw = true;
    if (argc == 5)
    {
        redraw = dAtob(argv[4]);
    }

    object->setLayerBlendColor(index, blendColor, redraw);
}

/*! Returns the number of layers.
@return The number of layers used by the texture. Zero if only the base texture is used.
*/
ConsoleMethodWithDocs(ImageAsset, getLayerCount, ConsoleInt, 2, 2, ())
{
    return object->getLayerCount();
}

/*! Returns image used for a given layer.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@return The path to the image file.
*/
ConsoleMethodWithDocs(ImageAsset, getLayerImage, ConsoleString, 3, 3, (index))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::getLayerImage() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    U32 index = dAtoi(argv[2]);

    return object->getLayerImage(index);
}

/*! Returns position for a given layer.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@return (int y, int x) The layer position.
*/
ConsoleMethodWithDocs(ImageAsset, getLayerPosition, ConsoleString, 3, 3, (index))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::getLayerPosition() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    U32 index = dAtoi(argv[2]);

    char* retBuffer = Con::getReturnBuffer(64);
    const Point2I& pos = object->getLayerPosition(index);
    dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
    return retBuffer;
}

/*! Returns the blend color for a given layer.
@param index The one-based index to move to the back. Must be less or equal to the number of layers.
@return (float red / float green / float blue / float alpha) The layer blend color.
*/
ConsoleMethodWithDocs(ImageAsset, getLayerBlendColor, ConsoleString, 3, 3, (index))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::getLayerBlendColor() - Invalid number of parameters!");
        return StringTable->EmptyString;
    }

    U32 index = dAtoi(argv[2]);

    return object->getLayerBlendColor(index).scriptThis();
}

/*! Forces the texture to redraw its layers. Redrawing can be expensive and is automatically done on every change to a layer unless prevented.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, forceRedraw, ConsoleVoid, 2, 2, ())
{
    if (object->getLayerCount() > 0)
    {
        object->forceRedrawImage();
    }
}

/*! Sets the blend color for the base image when using layers.
@param blendColor The space-deliminated color used to blend the layer. Requires four decimal values between 0 and 1 that represent red, green, blue, and alpha.
@param doRedraw Optional value that prevents a redraw of the texture when false. Defaults to true.
@return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setBlendColor, ConsoleVoid, 3, 4, (blendColor, [doRedraw]))
{
    if (argc < 3)
    {
        Con::warnf("ImageAsset::setBlendColor() - Invalid number of parameters!");
        return;
    }

    const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
    if (colorCount != 4)
    {
        Con::warnf("Scene::setBlendColor() - Invalid color! Colors require four values (red / green / blue / alpha)!");
        return;
    }
    ColorF blendColor = ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)),
        dAtof(Utility::mGetStringElement(argv[2], 1)),
        dAtof(Utility::mGetStringElement(argv[2], 2)),
        dAtof(Utility::mGetStringElement(argv[2], 3)));

    bool redraw = true;
    if (argc == 4)
    {
        redraw = dAtob(argv[3]);
    }

    object->setBlendColor(blendColor, redraw);
}

/*! Returns the blend color for the base texture when using layers.
@return (float red / float green / float blue / float alpha) The base blend color.
*/
ConsoleMethodWithDocs(ImageAsset, getBlendColor, ConsoleString, 2, 2, ())
{
    return object->getBlendColor().scriptThis();
}

ConsoleMethodGroupEndWithDocs(ImageAsset)
