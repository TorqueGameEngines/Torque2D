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

ConsoleMethodGroupBeginWithDocs(CompositeSprite, SceneObject)

/*! Adds a sprite at the specified logical position.
    You must specify the correct number of arguments for the selected layout mode.
    The created sprite will be automatically selected.
    @param a b c d e f Logical positions #1 & #2 and four additional and optional arguments.
    @return The batch Id of the added sprite or zero if not successful.
*/
ConsoleMethodWithDocs(CompositeSprite, addSprite, ConsoleInt, 2, 3, ( [a] [b] [c] [d] [e] [f] ))
{
    if ( argc == 2 )
        return object->addSprite( SpriteBatchItem::LogicalPosition() );
    else
        return object->addSprite( SpriteBatchItem::LogicalPosition(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Removes the selected sprite.
    @return Whether the sprite was removed or not.
*/
ConsoleMethodWithDocs(CompositeSprite, removeSprite, ConsoleBool, 2, 2, ())
{
    return object->removeSprite();
}

//-----------------------------------------------------------------------------

/*! Removes all sprites.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, clearSprites, ConsoleVoid, 2, 2, ())
{
    return object->clearSprites();
}

//-----------------------------------------------------------------------------

/*! Gets a count of sprites in the composite.
    @return The count of sprites in the composite.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteCount, ConsoleInt, 2, 2, ())
{
    return object->getSpriteCount();
}


//-----------------------------------------------------------------------------

/*! Sets the batch layout type.
    The render sort mode is used when isolated batch mode is on.
    @param batchLayoutType 'none', 'rect' or 'iso' layout types are valid.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setBatchLayout, ConsoleVoid, 3, 3, (batchLayoutType))
{
    // Fetch the batch layout type/
    CompositeSprite::BatchLayoutType batchLayoutType = CompositeSprite::getBatchLayoutTypeEnum( argv[2] );

    // Sanity!
    if ( batchLayoutType == CompositeSprite::INVALID_LAYOUT )
    {
        // Warn.
        Con::warnf( "CompositeSprite::setBatchLayout() - Unknown batch layout type of '%s'.", argv[2] );
        return;
    }

    object->setBatchLayout( batchLayoutType );
}

//-----------------------------------------------------------------------------

/*! Gets the batch layout type.
    @return The batch layout type.
*/
ConsoleMethodWithDocs(CompositeSprite, getBatchLayout, ConsoleString, 2, 2, ())
{
    return CompositeSprite::getBatchLayoutTypeDescription( object->getBatchLayout() );
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprites are rendered, isolated from other renderings as one batch or not.
    When in batch isolated mode, the sprites can be optionally sorted.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setBatchIsolated, ConsoleVoid, 3, 3, (bool batchIsolated))
{
    // Fetch batch isolated.
    const bool batchIsolated = dAtob(argv[2]);

    object->setBatchIsolated( batchIsolated );
}

//-----------------------------------------------------------------------------

/*! Gets whether the sprites are rendered, isolated from other renderings as one batch or not.
    @return Whether the sprites are rendered, isolated from other renderings as one batch or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getBatchIsolated, ConsoleBool, 2, 2, ())
{
    return object->getBatchIsolated();
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprites are culled.
    For sprites that are off-screen this is considerably faster during render at the expense of memory.
    For small composites with a few sprites, the overhead is probably not worth it.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setBatchCulling, ConsoleVoid, 3, 3, (bool batchCulling))
{
    // Fetch batch culling..
    const bool batchCulling = dAtob(argv[2]);

    STATIC_VOID_CAST_TO(CompositeSprite, SpriteBatch, object)->setBatchCulling( batchCulling );
}

//-----------------------------------------------------------------------------

/*! Gets whether the sprites are render culled or not
    @return Whether the sprites are rendered culled or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getBatchCulling, ConsoleBool, 2, 2, ())
{
    return object->getBatchCulling();
}

//-----------------------------------------------------------------------------

/*! Sets the batch render sort mode.
    The render sort mode is used when isolated batch mode is on.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setBatchSortMode, ConsoleVoid, 3, 3, (renderSortMode))
{
    // Fetch render sort mode.
    SceneRenderQueue::RenderSort batchSortMode = SceneRenderQueue::getRenderSortEnum( argv[2] );

    // Sanity!
    if ( batchSortMode == SceneRenderQueue::RENDER_SORT_INVALID )
    {
        // Warn.
        Con::warnf( "CompositeSprite::setBatchSortMode() - Unknown batch sort mode of '%s'.", argv[2] );
        return;
    }

    object->setBatchSortMode( batchSortMode );
}

//-----------------------------------------------------------------------------

/*! Gets the batch render sort mode.
    @return The render sort mode.
*/
ConsoleMethodWithDocs(CompositeSprite, getBatchSortMode, ConsoleString, 2, 2, ())
{
    return SceneRenderQueue::getRenderSortDescription( object->getBatchSortMode() );
}

//-----------------------------------------------------------------------------

/*! Sets the stride which scales the position at which sprites are created.
    @param strideX The default stride of the local X axis.
    @param strideY The default stride of the local Y axis.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setDefaultSpriteStride, ConsoleVoid, 3, 4, (float strideX, [float strideY]]))
{
    Vector2 stride;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("strideX strideY")
    if ( (elementCount == 2) && (argc == 3) )
    {
        stride.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        stride.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (strideX, [strideY])
    else if (elementCount == 1)
    {
        stride.x = dAtof(argv[2]);

        if (argc > 3)
            stride.y = dAtof(argv[3]);
        else
            stride.y = stride.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setDefaultSpriteStride() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteStride( stride );
}

//-----------------------------------------------------------------------------

/*! Gets the stride which scales the position at which sprites are created.
    @return (float strideX/float strideY) The stride which scales the position at which sprites are created.
*/
ConsoleMethodWithDocs(CompositeSprite, getDefaultSpriteStride, ConsoleString, 2, 2, ())
{
    return object->getDefaultSpriteStride().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the size at which sprites are created.
    @param width The default width of sprites.
    @param height The default height of sprites
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setDefaultSpriteSize, ConsoleVoid, 3, 4, (float width, [float height]))
{
    Vector2 size;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ( (elementCount == 2) && (argc == 3) )
    {
        size.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        size.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (width, [height])
    else if (elementCount == 1)
    {
        size.x = dAtof(argv[2]);

        if (argc > 3)
            size.y = dAtof(argv[3]);
        else
            size.y = size.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setDefaultSpriteSize() - Invalid number of parameters!");
        return;
    }

    object->setDefaultSpriteSize( size );
}

//-----------------------------------------------------------------------------

/*! Gets the size at which sprites are created.
    @return (float width/float height) The size at which sprites are created.
*/
ConsoleMethodWithDocs(CompositeSprite, getDefaultSpriteSize, ConsoleString, 2, 2, ())
{
    return object->getDefaultSpriteSize().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the angle at which sprites are created.
    @param angle The angle at which sprites are created.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setDefaultSpriteAngle, ConsoleVoid, 3, 3, (float angle))
{
    // Fetch angle.
    const F32 angle = mDegToRad( dAtof(argv[2]) );

    static_cast<SpriteBatch*>(object)->setDefaultSpriteAngle( angle );
}

//-----------------------------------------------------------------------------

/*! Gets the angle at which sprites are created.
    @return (float angle) The angle at which sprites are created.
*/
ConsoleMethodWithDocs(CompositeSprite, getDefaultSpriteAngle, ConsoleFloat, 3, 3, ())
{
    return mRadToDeg( static_cast<SpriteBatch*>(object)->getDefaultSpriteAngle() );
}

//-----------------------------------------------------------------------------

/*! Selects a sprite at the specified logical position.
    @param a b c d e f Logical positions #1 & #2 and four additional and optional arguments.
    @return Whether the sprite was selected or not.
*/
ConsoleMethodWithDocs(CompositeSprite, selectSprite, ConsoleBool, 3, 3, ( a b [c] [d] [e] [f] ))
{
    return object->selectSprite( SpriteBatchItem::LogicalPosition(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Selects a sprite with the specified batch Id.
    @param batchId The batch Id of the sprite to select.
    @return Whether the sprite was selected or not.
*/
ConsoleMethodWithDocs(CompositeSprite, selectSpriteId, ConsoleBool, 3, 3, ( int batchId ))
{
    return object->selectSpriteId( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Selects a sprite with the specified name.
    @param name The name of the sprite.
    @return Whether the sprite was selected or not.
*/
ConsoleMethodWithDocs(CompositeSprite, selectSpriteName, ConsoleBool, 3, 3, ( name ))
{
    return object->selectSpriteName( argv[2] );
}


//-----------------------------------------------------------------------------

/*! Deselects any selected sprite.
    This is not required but can be used to stop accidental changes to sprites.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, deselectSprite, ConsoleVoid, 2, 2, ())
{
    return object->deselectSprite();
}

//-----------------------------------------------------------------------------

/*! Checks whether a sprite is selected or not.
    @return Whether a sprite is selected or not.
*/
ConsoleMethodWithDocs(CompositeSprite, isSpriteSelected, ConsoleBool, 2, 2, ())
{
    return object->isSpriteSelected();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite image and optional frame.
    @param imageAssetId The image to set the sprite to.
    @param imageFrame The image frame of the imageAssetId to set the sprite to.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteImage, ConsoleVoid, 3, 4, (imageAssetId, [imageFrame]))
{
    // Was a frame specified?
    if (argc >= 4)
    {
        // Was it a number or a string?
        if (!dIsalpha(*argv[3]))
        {
            // Fetch the numerical frame and set the image
            const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;
            object->setSpriteImage( argv[2], frame );
        }
        else
        {
            // Set the image and pass the named frame string
            object->setSpriteImage( argv[2], argv[3] );
        }
    }
    else
    {
        // Frame was not specified, use default 0 and set the image
        const U32 frame = 0;
        object->setSpriteImage( argv[2], frame );
    }
}

//-----------------------------------------------------------------------------

/*! Gets the sprite image.
    @return The sprite image.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteImage, ConsoleString, 2, 2, ())
{
    return object->getSpriteImage();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite image frame.
    @param imageFrame The image frame to set the sprite to.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteImageFrame, ConsoleVoid, 3, 3, (int imageFrame))
{
    // Fetch frame.
    const U32 frame = dAtoi(argv[2]);

    object->setSpriteImageFrame( frame );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite image frame.
    @return The sprite image frame.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteImageFrame, ConsoleInt, 2, 2, ())
{
    return object->getSpriteImageFrame();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite named image frame.
    @param namedFrame The named image frame to set the sprite to.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteNamedImageFrame, ConsoleVoid, 3, 3, (namedFrame))
{
    object->setSpriteNamedImageFrame( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite named image frame.
    @return The sprite named image frame.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteNamedImageFrame, ConsoleString, 2, 2, ())
{
    return object->getSpriteNamedImageFrame();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite animation.
    @param imageAssetId The animation to set the sprite to.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteAnimation, ConsoleVoid, 3, 4, (animationAssetId))
{
    object->setSpriteAnimation( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite animation.
    @return The sprite animation.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteAnimation, ConsoleString, 2, 2, ())
{
    return object->getSpriteAnimation();
}

//-----------------------------------------------------------------------------

/*! Sets the current animation frame for the selected sprite. IMPORTANT: this is not the image frame number used in the animation!
@param frame Which frame of the animation to display
@return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteAnimationFrame, ConsoleVoid, 3, 3, (int animationFrame))
{
	// Fetch frame.
	const U32 frame = dAtoi(argv[2]);

	object->setSpriteAnimationFrame(frame);
}

//-----------------------------------------------------------------------------

/*! Gets current frame index used in the animation for the selected sprite. IMPORTANT: this is not the image frame number!
@return The current numerical animation frame for the selected sprite
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteAnimationFrame, ConsoleInt, 2, 2, ())
{
	return object->getSpriteAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Clears any image or animation asset from the sprite.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, clearSpriteAsset, ConsoleVoid, 2, 2, ())
{
    return object->clearSpriteAsset();
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprite is visible or not.
    @param visible Whether the sprite is visible or not.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteVisible, ConsoleVoid, 3, 3, (bool visible))
{
    // Fetch visible.
    const bool visible = dAtob(argv[2]);

    object->setSpriteVisible( visible );

}

//-----------------------------------------------------------------------------

/*! Gets whether the sprite is visible or not.
    @return Whether the sprite is visible or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteVisible, ConsoleBool, 2, 2, ())
{
    return object->getSpriteVisible();
}

//-----------------------------------------------------------------------------

/*! Sets the sprites local position.
    @param localX The local position X.
    @param localY The local position Y.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteLocalPosition, ConsoleVoid, 3, 4, (float localX, float localY))
{
    Vector2 localPosition;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ( (elementCount == 2) && (argc == 3) )
    {
        localPosition.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        localPosition.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (x, y)
    else if ( elementCount == 1 && (argc > 3) )
    {
        localPosition.x = dAtof(argv[2]);
        localPosition.y = dAtof(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteLocalPosition() - Invalid number of parameters!");
        return;
    }

    object->setSpriteLocalPosition( localPosition );
    object->setSpatialDirty();
}

//-----------------------------------------------------------------------------

/*! Gets the sprite local position.
    @return The sprite local position.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteLocalPosition, ConsoleString, 2, 2, ())
{
    return object->getSpriteLocalPosition().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Gets the sprite logical position.
    @return The sprite logical position.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteLogicalPosition, ConsoleString, 2, 2, ())
{
    return object->getSpriteLogicalPosition().getString();
}

//-----------------------------------------------------------------------------

/*! Sets the sprites local angle.
    @param localAngle The sprite local angle.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteAngle, ConsoleVoid, 3, 3, (float localAngle))
{
    // Fetch angle.
    const F32 angle = mDegToRad( dAtof(argv[2]) );

    object->setSpriteAngle( angle );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite local angle.
    @return The sprite local angle.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteAngle, ConsoleFloat, 2, 2, ())
{
    return mRadToDeg( object->getSpriteAngle() );
}

//-----------------------------------------------------------------------------

/*! Sets the sprites depth.
    @param depth The sprite depth.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteDepth, ConsoleVoid, 3, 3, (float depth))
{
    // Fetch depth.
    const F32 depth = dAtof(argv[2]);

    object->setSpriteDepth( depth );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite depth.
    @return The sprite depth.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteDepth, ConsoleFloat, 2, 2, ())
{
    return object->getSpriteDepth();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite size.
    @param width The sprite width.
    @param height The sprite height
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteSize, ConsoleVoid, 3, 4, (float width, [float height]))
{
    Vector2 size;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("width height")
    if ( (elementCount == 2) && (argc == 3) )
    {
        size.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        size.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (width, [height])
    else if (elementCount == 1)
    {
        size.x = dAtof(argv[2]);

        if (argc > 3)
            size.y = dAtof(argv[3]);
        else
            size.y = size.x;
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteSize() - Invalid number of parameters!");
        return;
    }

    object->setSpriteSize( size );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite size.
    @return (float width/float height) The sprite size.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteSize, ConsoleString, 2, 2, ())
{
    return object->getSpriteSize().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprite is flipped along its local X axis or not.
    @param flipX Whether the sprite is flipped along its local X axis or not.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteFlipX, ConsoleVoid, 3, 3, (bool flipX))
{
    // Fetch flipX.
    const bool flipX = dAtob(argv[2]);

    object->setSpriteFlipX( flipX );
}

//-----------------------------------------------------------------------------

/*! Gets whether the sprite is flipped along its local X axis or not.
    @return Whether the sprite is flipped along its local X axis or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteFlipX, ConsoleBool, 2, 2, ())
{
    return object->getSpriteFlipX();
}

//-----------------------------------------------------------------------------

/*! Sets whether the sprite is flipped along its local Y axis or not.
    @param flipY Whether the sprite is flipped along its local Y axis or not.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteFlipY, ConsoleVoid, 3, 3, (bool flipY))
{
    const bool flipY = dAtob(argv[2]);

    object->setSpriteFlipY( flipY );
}

//-----------------------------------------------------------------------------

/*! Gets whether the sprite is flipped along its local Y axis or not.
    @return Whether the sprite is flipped along its local Y axis or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteFlipY, ConsoleBool, 2, 2, ())
{
    return object->getSpriteFlipY();
}

//-----------------------------------------------------------------------------

/*! Sets the sprites sort point.
    @param localX The local sort point X.
    @param localY The local sort point Y.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteSortPoint, ConsoleVoid, 3, 4, (float localX, float localY))
{
    Vector2 sortPoint;

    // Fetch element count.
    const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ( (elementCount == 2) && (argc == 3) )
    {
        sortPoint.x = dAtof(Utility::mGetStringElement(argv[2], 0));
        sortPoint.y = dAtof(Utility::mGetStringElement(argv[2], 1));
    }
    // (x, y)
    else if ( elementCount == 1 && (argc > 3) )
    {
        sortPoint.x = dAtof(argv[2]);
        sortPoint.y = dAtof(argv[3]);
    }
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::setSpriteSortPoint() - Invalid number of parameters!");
        return;
    }

    object->setSpriteSortPoint( sortPoint );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite local sort point.
    @return The sprite local sort point.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteSortPoint, ConsoleString, 2, 2, ())
{
    return object->getSpriteSortPoint().scriptThis();
}


//-----------------------------------------------------------------------------

/*! Sets the name of the render group used to sort the sprite during rendering.
    @param renderGroup The name of the render group to use.  Defaults to nothing.
    @return No return value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteRenderGroup, ConsoleVoid, 3, 3, (renderGroup))
{
    object->setSpriteRenderGroup( argv[2] );
} 

//-----------------------------------------------------------------------------

/*! Gets the name of the render group used to sort the sprite during rendering.
    @return The render group used to sort the object during rendering.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteRenderGroup, ConsoleString, 2, 2, ())
{
    return object->getSpriteRenderGroup();
} 

//-----------------------------------------------------------------------------

/*! Sets whether sprite blending is on or not.
    @blendMode Whether sprite blending is on or not.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteBlendMode, ConsoleVoid, 3, 3, (bool blendMode))
{
    // Fetch blend mode.
    const bool blendMode = dAtob(argv[2]);

    object->setSpriteBlendMode( blendMode );
}

//-----------------------------------------------------------------------------

/*! Gets whether sprite blending is on or not.
    @return (bool blendMode) Whether sprite blending is on or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteBlendMode, ConsoleBool, 2, 2, ())
{
   return object->getSpriteBlendMode();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite source blend factor.
    @param srcBlend The sprite source blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteSrcBlendFactor, ConsoleVoid, 3, 3, (srcBlend))
{
    // Fetch source blend factor.
    GLenum blendFactor = SceneObject::getSrcBlendFactorEnum(argv[2]);

    object->setSpriteSrcBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite source blend factor.
    @return (srcBlend) The sprite source blend factor.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteSrcBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getSrcBlendFactorDescription( object->getSpriteSrcBlendFactor() );
}

//-----------------------------------------------------------------------------

/*! Sets the sprite destination blend factor.
    @param dstBlend The sprite destination blend factor.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteDstBlendFactor, ConsoleVoid, 3, 3, (dstBlend))
{
    // Fetch destination blend factor.
    GLenum blendFactor = SceneObject::getDstBlendFactorEnum(argv[2]);

    object->setSpriteDstBlendFactor( blendFactor );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite destination blend factor.
    @return (dstBlend) The sprite destination blend factor.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteDstBlendFactor, ConsoleString, 2, 2, ())
{
   return SceneObject::getDstBlendFactorDescription( object->getSpriteDstBlendFactor() );
}

//-----------------------------------------------------------------------------

/*! Sets whether sprite uses complex colors or not.
	@complexColor Whether sprite complex color is on or not.
	@return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteUseComplexColor, ConsoleVoid, 3, 3, (bool complexColor))
{
	// Fetch blend mode.
	const bool complex = dAtob(argv[2]);

	object->setSpriteUseComplexColor(complex);
}

//-----------------------------------------------------------------------------

/*! Gets whether sprite uses complex colors or not.
	@return (bool complexColor) Whether sprite complex color is on or not.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteUseComplexColor, ConsoleBool, 2, 2, ())
{
	return object->getSpriteUseComplexColor();
}

//-----------------------------------------------------------------------------

/*! or ( stockColorName ) - Sets the sprite blend color.
    @param red The red value.
    @param green The green value.
    @param blue The blue value.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteBlendColor, ConsoleVoid, 3, 6, (float red, float green, float blue, [float alpha = 1.0]))
{
    // The colors.
    F32 red;
    F32 green;
    F32 blue;
    F32 alpha = 1.0f;

    // Space separated.
    if (argc == 3 )
    {
        // Grab the element count.
        const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

        // Has a single argument been specified?
        if ( elementCount == 1 )
        {
            // Is a sprite selected?
            if ( !object->isSpriteSelected() )
            {
                // No, so warn.
                Con::warnf("CompositeSprite::setSpriteBlendColor() - Cannot set sprite blend color as no sprite is selected." );
                return;
            }

            Con::setData( TypeColorF, &const_cast<ColorF&>(object->getSpriteBlendColor()), 0, 1, &(argv[2]) );
            return;
        }

        // ("R G B [A]")
        if ((elementCount == 3) || (elementCount == 4))
        {
            // Extract the color.
            red   = dAtof(Utility::mGetStringElement(argv[2], 0));
            green = dAtof(Utility::mGetStringElement(argv[2], 1));
            blue  = dAtof(Utility::mGetStringElement(argv[2], 2));

            // Grab the alpha if it's there.
            if (elementCount > 3)
                alpha = dAtof(Utility::mGetStringElement(argv[2], 3));
        }

        // Invalid.
        else
        {
            Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
            return;
        }
    }

    // (R, G, B)
    else if (argc >= 5)
    {
        red   = dAtof(argv[2]);
        green = dAtof(argv[3]);
        blue  = dAtof(argv[4]);

        // Grab the alpha if it's there.
        if (argc > 5)
            alpha = dAtof(argv[5]);
    }

    // Invalid.
    else
    {
        Con::warnf("SceneObject::setBlendColor() - Invalid Number of parameters!");
        return;
    }

    // Set blend color.
    object->setSpriteBlendColor(ColorF(red, green, blue, alpha));
}

//-----------------------------------------------------------------------------

/*! Gets the sprite blend color
    @param allowColorNames Whether to allow stock color names to be returned or not.  Optional: Defaults to false.
    @return (float red / float green / float blue / float alpha) The sprite blend color.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteBlendColor, ConsoleString, 2, 3, (allowColorNames))
{
    // Get Blend color.
    ColorF blendColor = object->getSpriteBlendColor();

    // Fetch allow color names flag.
    const bool allowColorNames = (argc > 2) ? dAtob(argv[2] ) : false;

    // Are color names allowed?
    if ( allowColorNames )
    {
        // Yes, so fetch the field value.
        return Con::getData( TypeColorF, &blendColor, 0 );
    }

    // No, so fetch the raw color values.
    return blendColor.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite complex color.
	@param topLeft The topLeft color space separated.
	@param topLeft The topRight color space separated.
	@param topLeft The bottomRight color space separated.
	@param topLeft The bottomLeft color space separated.
	@return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteComplexColor, ConsoleVoid, 6, 6, (topLeft, topRight, bottomRight, bottomLeft))
{
	// The corners.
	ColorF corner0;
	ColorF corner1;
	ColorF corner2;
	ColorF corner3;

	// Space separated.
	if (argc == 6)
	{
		// Grab the element count.
		const U32 elementCount0 = Utility::mGetStringElementCount(argv[2]);
		if (elementCount0 == 3)
		{
			corner0 = ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)), dAtof(Utility::mGetStringElement(argv[2], 1)), dAtof(Utility::mGetStringElement(argv[2], 2)));
		}
		else if (elementCount0 == 4)
		{
			corner0 = ColorF(dAtof(Utility::mGetStringElement(argv[2], 0)), dAtof(Utility::mGetStringElement(argv[2], 1)), dAtof(Utility::mGetStringElement(argv[2], 2)), dAtof(Utility::mGetStringElement(argv[2], 3)));
		}
		else
		{
			Con::warnf("CompositeSprite::setSpriteComplexColor() - Top Left Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount1 = Utility::mGetStringElementCount(argv[3]);
		if (elementCount1 == 3)
		{
			corner1 = ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)), dAtof(Utility::mGetStringElement(argv[3], 2)));
		}
		else if (elementCount1 == 4)
		{
			corner1 = ColorF(dAtof(Utility::mGetStringElement(argv[3], 0)), dAtof(Utility::mGetStringElement(argv[3], 1)), dAtof(Utility::mGetStringElement(argv[3], 2)), dAtof(Utility::mGetStringElement(argv[3], 3)));
		}
		else
		{
			Con::warnf("CompositeSprite::setSpriteComplexColor() - Top Right Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount2 = Utility::mGetStringElementCount(argv[4]);
		if (elementCount2 == 3)
		{
			corner2 = ColorF(dAtof(Utility::mGetStringElement(argv[4], 0)), dAtof(Utility::mGetStringElement(argv[4], 1)), dAtof(Utility::mGetStringElement(argv[4], 2)));
		}
		else if (elementCount2 == 4)
		{
			corner2 = ColorF(dAtof(Utility::mGetStringElement(argv[4], 0)), dAtof(Utility::mGetStringElement(argv[4], 1)), dAtof(Utility::mGetStringElement(argv[4], 2)), dAtof(Utility::mGetStringElement(argv[4], 3)));
		}
		else
		{
			Con::warnf("CompositeSprite::setSpriteComplexColor() - Bottom Right Corner has Invalid Number of arguments!");
			return;
		}

		// Grab the element count.
		const U32 elementCount3 = Utility::mGetStringElementCount(argv[5]);
		if (elementCount3 == 3)
		{
			corner3 = ColorF(dAtof(Utility::mGetStringElement(argv[5], 0)), dAtof(Utility::mGetStringElement(argv[5], 1)), dAtof(Utility::mGetStringElement(argv[5], 2)));
		}
		else if (elementCount3 == 4)
		{
			corner3 = ColorF(dAtof(Utility::mGetStringElement(argv[5], 0)), dAtof(Utility::mGetStringElement(argv[5], 1)), dAtof(Utility::mGetStringElement(argv[5], 2)), dAtof(Utility::mGetStringElement(argv[5], 3)));
		}
		else
		{
			Con::warnf("CompositeSprite::setSpriteComplexColor() - Bottom Left Corner has Invalid Number of arguments!");
			return;
		}

		// Set blend color.
		object->setSpriteComplexColor(corner0, corner1, corner2, corner3);
	}

	// Invalid.
	else
	{
		Con::warnf("CompositeSprite::setSpriteComplexColor() - Invalid Number of parameters!");
		return;
	}
}

//-----------------------------------------------------------------------------

/*! Gets the sprite complex color
	@param cornerID The cornerID whose color will be returned (1 - 4).
	@return (float red / float green / float blue / float alpha) The sprite corner blend color.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteComplexColor, ConsoleString, 3, 3, (cornerID))
{
	S8 cornerID = dAtoi(argv[2]);

	// Get Blend color.
	ColorF blendColor = object->getSpriteComplexColor(cornerID);

	// No, so fetch the raw color values.
	return blendColor.scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite color alpha (transparency).
    The alpha value specifies directly the transparency of the image. A value of 1.0 will not affect the object and a value of 0.0 will make the object completely transparent.
    @param alpha The alpha value.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteBlendAlpha, ConsoleVoid, 3, 3, (float alpha))
{
    object->setSpriteBlendAlpha( dAtof(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite color alpha (transparency).
    @return (float alpha) The alpha value, a range from 0.0 to 1.0.  Less than zero if alpha testing is disabled.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteBlendAlpha, ConsoleFloat, 2, 2, ())
{
    return object->getSpriteBlendAlpha();
}

//-----------------------------------------------------------------------------

/*! Set the sprite alpha test.
    @param value Numeric value of 0.0 to 1.0 to turn on alpha testing. Less than zero to disable alpha testing.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteAlphaTest, ConsoleVoid, 3, 3, (float alpha))
{
    object->setSpriteAlphaTest(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the sprite alpha test.
    @return (S32) A value of 0 to 255 if alpha testing is enabled. <0 represents disabled alpha testing.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteAlphaTest, ConsoleFloat, 2, 2, ())
{
    return object->getSpriteAlphaTest();
}

//-----------------------------------------------------------------------------

/*! Set the sprite data object.
    NOTE: This object will be persisted alongside the composite sprite.
    To clear the object you can pass an empty string.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteDataObject, ConsoleVoid, 3, 3, (object))
{
    object->setSpriteDataObject( Sim::findObject( argv[2] ) );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite data object.
    @return The sprite data object.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteDataObject, ConsoleString, 2, 2, ())
{
    return object->getSpriteDataObject()->getIdString();
}

//-----------------------------------------------------------------------------

/*! Set the sprite user data field.
    @param data A space separated string containing the data you wish to store.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteUserData, ConsoleVoid, 3, 3, (data))
{
    StringTableEntry userData = StringTable->insert(argv[2]);
    object->setUserData( (void*)userData );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite user data.
    @return The sprite user data.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteUserData, ConsoleString, 2, 2, ())
{
    const char* userData = (const char*) object->getUserData();
    return userData;
}

//-----------------------------------------------------------------------------

/*! Set the sprite name.
    This must be unique within this composite sprite instance.  To clear the name you can pass an empty string.
    @return No return Value.
*/
ConsoleMethodWithDocs(CompositeSprite, setSpriteName, ConsoleVoid, 3, 3, (name))
{
    object->setSpriteName( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the sprite name.
    @return The sprite name.
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteName, ConsoleString, 2, 2, ())
{
    return object->getSpriteName();
}

//-----------------------------------------------------------------------------

/*! Gets the SpriteBatchId of the currently selected sprite.
    @returns The SpriteBatchId
*/
ConsoleMethodWithDocs(CompositeSprite, getSpriteId, ConsoleInt, 2, 2, ())
{
    return object->getSpriteId();
}

//-----------------------------------------------------------------------------

/*! Picks sprites intersecting the specified point with optional group/layer masks.
    @param x/y The coordinate of the point as either (\x y\ or (x,y)
    @return Returns list of sprite Ids.
*/
ConsoleMethodWithDocs(CompositeSprite, pickPoint, ConsoleString, 3, 4, (x / y ))
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickPoint() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // The point.
    Vector2 point;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first parameter.
    U32 elementCount = Utility::mGetStringElementCount(argv[2]);

    // ("x y")
    if ((elementCount == 2) && (argc < 8))
    {
        point = Utility::mGetStringElementVector(argv[2]);
        firstArg = 3;
    }
   
    // (x, y)
    else if ((elementCount == 1) && (argc > 3))
    {
        point = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        firstArg = 4;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickPoint() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch the render transform.
    const b2Transform& renderTransform = object->getRenderTransform();

    // Transform into local space.
    point = b2MulT( renderTransform, point );

    // Perform query.
    pSpriteBatchQuery->queryPoint( point, true );

    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add picked sprites.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickPoint() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Picks sprites intersecting the specified area with optional group/layer masks.
    @param startx/y The coordinates of the start point as either (\x y\ or (x,y)
    @param endx/y The coordinates of the end point as either (\x y\ or (x,y)
    @return Returns list of sprite Ids.
*/
ConsoleMethodWithDocs(CompositeSprite, pickArea, ConsoleString, 4, 6, (startx/y, endx/y ))
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickArea() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // Upper left and lower right bound.
    Vector2 v1, v2;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first two parameters.
    U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
    U32 elementCount2 = 1;
    if (argc > 3)
        elementCount2 = Utility::mGetStringElementCount(argv[3]);

    // ("x1 y1 x2 y2")
    if ((elementCount1 == 4) && (argc < 9))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[2], 2);
        firstArg = 3;
    }
   
    // ("x1 y1", "x2 y2")
    else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[3]);
        firstArg = 4;
    }
   
    // (x1, y1, x2, y2)
    else if (argc > 5)
    {
        v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
        firstArg = 6;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickArea() - Invalid number of parameters!");
        return NULL;
    }

    // Calculate normalized AABB.
    b2AABB aabb;
    aabb.lowerBound.x = getMin( v1.x, v2.x );
    aabb.lowerBound.y = getMin( v1.y, v2.y );
    aabb.upperBound.x = getMax( v1.x, v2.x );
    aabb.upperBound.y = getMax( v1.y, v2.y );

    // Calculate local OOBB.
    b2Vec2 localOOBB[4];
    CoreMath::mAABBtoOOBB( aabb, localOOBB );
    CoreMath::mCalculateInverseOOBB( localOOBB, object->getRenderTransform(), localOOBB );

    // Calculate local AABB.
    b2AABB localAABB;
    CoreMath::mOOBBtoAABB( localOOBB, localAABB );
  
    // Convert OOBB to a PolygonShape
    b2PolygonShape oobb_polygon;
    oobb_polygon.Set(localOOBB, 4);

    // Perform query.
    pSpriteBatchQuery->queryOOBB( localAABB, oobb_polygon, true );


    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add picked objects.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickArea() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Picks sprites intersecting the specified ray with optional group/layer masks.
    @param startx/y The coordinates of the start point as either (\x y\ or (x,y)
    @param endx/y The coordinates of the end point as either (\x y\ or (x,y)
    @return Returns list of sprite Ids
*/
ConsoleMethodWithDocs(CompositeSprite, pickRay, ConsoleString, 4, 6, (startx/y, endx/y))
{
    // Fetch sprite batch query and clear results.
    SpriteBatchQuery* pSpriteBatchQuery = object->getSpriteBatchQuery( true );

    // Is the sprite batch query available?
    if ( pSpriteBatchQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "CompositeSprite::pickRay() - Cannot pick sprites if clipping mode is off." );

        // Return nothing.
        return NULL;
    }

    // Upper left and lower right bound.
    Vector2 v1, v2;

    // The index of the first optional parameter.
    U32 firstArg;

    // Grab the number of elements in the first two parameters.
    U32 elementCount1 = Utility::mGetStringElementCount(argv[2]);
    U32 elementCount2 = 1;
    if (argc > 3)
        elementCount2 = Utility::mGetStringElementCount(argv[3]);

    // ("x1 y1 x2 y2")
    if ((elementCount1 == 4) && (argc < 9))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[2], 2);
        firstArg = 3;
    }
   
    // ("x1 y1", "x2 y2")
    else if ((elementCount1 == 2) && (elementCount2 == 2) && (argc > 3) && (argc < 10))
    {
        v1 = Utility::mGetStringElementVector(argv[2]);
        v2 = Utility::mGetStringElementVector(argv[3]);
        firstArg = 4;
    }
   
    // (x1, y1, x2, y2)
    else if (argc > 5)
    {
        v1 = Vector2(dAtof(argv[2]), dAtof(argv[3]));
        v2 = Vector2(dAtof(argv[4]), dAtof(argv[5]));
        firstArg = 6;
    }
   
    // Invalid
    else
    {
        Con::warnf("CompositeSprite::pickRay() - Invalid number of parameters!");
        return NULL;
    }

    // Fetch the render transform.
    const b2Transform& renderTransform = object->getRenderTransform();

    // Transform into local space.
    v1 = b2MulT( renderTransform, v1 );
    v2 = b2MulT( renderTransform, v2 );

    // Perform query.
    pSpriteBatchQuery->queryRay( v1, v2, true );

    // Sanity!
    AssertFatal( pSpriteBatchQuery->getIsRaycastQueryResult(), "Invalid non-ray-cast query result returned." );

    // Fetch result count.
    const U32 resultCount = pSpriteBatchQuery->getQueryResultsCount();

    // Finish if no results.
    if (resultCount == 0 )
        return NULL;

    // Sort ray-cast result.
    pSpriteBatchQuery->sortRaycastQueryResult();

    // Fetch results.
    typeSpriteBatchQueryResultVector& queryResults = pSpriteBatchQuery->getQueryResults();

    // Set Max Buffer Size.
    const U32 maxBufferSize = 4096;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(maxBufferSize);

    // Set Buffer Counter.
    U32 bufferCount = 0;

    // Add Picked Objects to List.
    for ( U32 n = 0; n < resultCount; n++ )
    {
        // Output Object ID.
        bufferCount += dSprintf( pBuffer + bufferCount, maxBufferSize-bufferCount, "%d ", queryResults[n].mpSpriteBatchItem->getBatchId() );

        // Finish early if we run out of buffer space.
        if ( bufferCount >= maxBufferSize )
        {
            // Warn.
            Con::warnf("CompositeSprite::pickRay() - Too many items picked to return to scripts!");
            break;
        }
    }

    // Clear sprite batch query.
    pSpriteBatchQuery->clearQuery();

    // Return buffer.
    return pBuffer;
}

ConsoleMethodGroupEndWithDocs(CompositeSprite)
