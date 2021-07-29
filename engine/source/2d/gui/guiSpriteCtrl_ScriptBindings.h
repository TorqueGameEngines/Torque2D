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

ConsoleMethodGroupBeginWithDocs(GuiSpriteCtrl, GuiControl)

/*! Gets whether the control is in static or dynamic (animated) mode.
    @return Returns whether the control is in static or dynamic (animated) mode.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, isStaticFrameProvider, ConsoleBool, 2, 2, ())
{
    return object->isStaticFrameProvider();
}

//------------------------------------------------------------------------------

/*! Gets whether the control is using a numerical or named image frame.
    @return Returns true when using a named frame, false when using a numerical index.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, isUsingNamedImageFrame, ConsoleBool, 2, 2, ())
{
    return object->isUsingNamedImageFrame();
}

//-----------------------------------------------------------------------------

/*! Sets the control image and optionally the frame.
    @param imageAssetId The image asset Id to display
    @param frame The numerical or named frame of the image to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImage, ConsoleBool, 3, 4, (imageAssetId, [frame]))
{
    // Was a frame specified?
    if (argc >= 4)
    {
        // Was it a number or a string?
        if (!dIsalpha(*argv[3]))
        {
            // Fetch the numerical frame and set the image
            const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;
            return object->setImage( argv[2], frame );
        }
        else
        {
            // Set the image and pass the named frame string
            return object->setImage( argv[2], argv[3] );
        }
    }
    else
    {
        // Frame was not specified, use default 0 and set the image
        const U32 frame = 0;
        return object->setImage( argv[2], frame );
    }
}

//------------------------------------------------------------------------------

/*! Gets current image asset Id.
    @return (string imageAssetId) The image being displayed.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImage, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getImage() - Method invalid, not in static mode.");
        return StringTable->EmptyString;
    }

    // Get image.
    return object->getImage();
}

//-----------------------------------------------------------------------------

/*! Sets the image frame to use as the image.
    @param imageFrame The image frame to use as the image.
    @return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImageFrame, ConsoleBool, 3, 3, (int imageFrame))
{
    // Are we in static mode?
    if ( object->usesAsset() && !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::setImageFrame() - Method invalid, not in static mode.");
        return false;
    }

    return object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets current numerical image frame.
    @return (int frame) The numerical frame currently being displayed.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if (object->usesAsset() && !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getImageFrame() - Method invalid, not in static mode.");
        return -1;
    }

    // Are we using a named image frame?
    if (object->usesAsset() && object->isUsingNamedImageFrame() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getImageFrame() - Method invalid, using a named image frame.");
        return -1;
    }

    // Get image frame.
    return object->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the image frame using a named string.
    @param frame The named frame to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setNamedImageFrame, ConsoleBool, 3, 3, (frame))
{
    // Are we in static mode?
    if ( object->usesAsset() && !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::setNamedImageFrame() - Method invalid, not in static mode.");
        return false;
    }

	// Are we using a bitmap?
	if (!object->usesAsset())
	{
		// No, so warn.
		Con::warnf("GuiSpriteCtrl::setNamedImageFrame() - Method invalid, using bitmaps.");
		return false;
	}

    // Set the numerical frame
    return object->setNamedImageFrame( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the current named image frame.
    @return The current named image frame.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getNamedImageFrame, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( object->usesAsset() &&  !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getNamedImageFrame() - Method invalid, not in static mode.");
        return NULL;
    }

    // Are we using a named image frame?
    if (object->usesAsset() && !object->isUsingNamedImageFrame() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getNamedImageFrame() - Method invalid, not using a named image frame.");
        return NULL;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getNamedImageFrame() - Method invalid, using a bitmap.");
		return NULL;
	}

    return object->getNamedImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the animation asset Id to display.
    @param animationAssetId The animation asset Id to play
    @return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setAnimation, ConsoleVoid, 3, 3, (string animationAssetId))
{
    // Set animation.
    object->setAnimation( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the current animation asset Id.
    @return (string ianimationAssetId) The animation being displayed.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimation, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimation() - Method invalid, in static mode.");
        return StringTable->EmptyString;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimation() - Method invalid, using a bitmap.");
		return StringTable->EmptyString;
	}

    // Get animation.
    return object->getAnimation();
}

//-----------------------------------------------------------------------------

/*! Pause the current animation
    @param enable If true, pause the animation. If false, continue animating
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, pauseAnimation, ConsoleVoid, 3, 3, (bool enable))
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::pauseAnimation() - Method invalid, not in dynamic (animated) mode.");
        return;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::pauseAnimation() - Method invalid, using a bitmap.");
		return;
	}

    return static_cast<ImageFrameProvider*>(object)->pauseAnimation(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Stop the current animation
    @return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, stopAnimation, ConsoleVoid, 2, 2, ())
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::stopAnimation() - Method invalid, not in dynamic (animated) mode.");
        return;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::stopAnimation() - Method invalid, using a bitmap.");
		return;
	}

    object->stopAnimation();
}

//-----------------------------------------------------------------------------

/*! Sets the current animation frame. IMPORTANT: this is not the image frame number used in the animation!
    @param frame Which frame of the animation to display
    @return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setAnimationFrame, ConsoleVoid, 3, 3, (int frame))
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::setAnimationFrame() - Method invalid, not in dynamic (animated) mode.");
        return;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::setAnimationFrame() - Method invalid, using a bitmap.");
		return;
	}

    // Set Animation Frame
    object->setAnimationFrame( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets current frame index used in the animation. IMPORTANT: this is not the image frame number!
    @return The current numerical animation frame
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimationFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationFrame() - Method invalid, not in dynamic (animated) mode.");
        return -1;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationFrame() - Method invalid, using a bitmap.");
		return NULL;
	}

    // Get Animation Frame.
    return object->getAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Gets current numerical image frame used in the animation.
    @return The current numerical animation frame
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimationImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( object->usesAsset() && object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationImageFrame() - Method invalid, not in dynamic (animated) mode.");
        return -1;
    }

	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationImageFrame() - Method invalid, using a bitmap.");
		return -1;
	}

    // Get the current animation asset
    const AnimationAsset* asset = object->getCurrentAnimation();
    
    // Are we using named animation frames?
    if (asset->getNamedCellsMode())
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationImageFrame() - Method invalid, animation is in named cells mode.");
        return -1;
    }

    // Get Image Frame.
    return object->getCurrentAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Gets current named image frame used in the animation.
    @return The current named animation frame
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimationNamedImageFrame, ConsoleString, 2, 2, ())
{
	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationNamedImageFrame() - Method invalid, using a bitmap.");
		return NULL;
	}

    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationNamedImageFrame() - Method invalid, not in dynamic (animated) mode.");
        return NULL;
    }
    
    // Get the current animation asset
    const AnimationAsset* asset = object->getCurrentAnimation();

    // Are we using named animation frames?
    if (!asset->getNamedCellsMode())
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationNamedImageFrame() - Method invalid, animation not in named cells mode.");
        return NULL;
    }

    // Get Image Frame.
    return object->getCurrentNamedAnimationFrame();
}

//-----------------------------------------------------------------------------

/*! Gets current animation time.
    @return (float time) The current animation time
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimationTime, ConsoleFloat, 2, 2, ())
{
	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationTime() - Method invalid, using a bitmap.");
		return 0.0f;
	}

    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationTime() - Method invalid, not in dynamic (animated) mode.");
        return 0.0f;
    }


    // Get Animation Time.
    return object->getCurrentAnimationTime();
}

//-----------------------------------------------------------------------------

/*! Checks animation status.
    @return (bool finished) Whether or not the animation is finished
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getIsAnimationFinished, ConsoleBool, 2, 2, ())
{
	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationFinished() - Method invalid, using a bitmap.");
		return true;
	}

    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getIsAnimationFinished() - Method invalid, not in dynamic (animated) mode.");
        return true;
    }

    // Return Animation Finished Status.
    return object->isAnimationFinished();
}

//-----------------------------------------------------------------------------

/*! Change the rate of animation.
    @param timeScale Value which will scale the frame animation speed. 1 by default.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setAnimationTimeScale, ConsoleVoid, 3, 3, (float timeScale))
{
	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::setAnimationTimeScale() - Method invalid, using a bitmap.");
		return;
	}

    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::setAnimationTimeScale() - Method invalid, not in dynamic (animated) mode.");
        return;
    }

    object->setAnimationTimeScale(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the animation time scale for this control.
    @return (float) Returns the animation time scale for this control.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getAnimationTimeScale, ConsoleFloat, 2, 2, ())
{
	//Are we using a bitmap?
	if (!object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getAnimationTimeScale() - Method invalid, using a bitmap.");
		return -1;
	}

    // Are we in static mode?
    if ( object->isStaticFrameProvider() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getAnimationTimeScale() - Method invalid, not in dynamic (animated) mode.");
        return 1.0f;
    }

    return object->getAnimationTimeScale();
}

/*! Sets the amount to offset the image from its actual position. The image will not exceed the bounds of the content area.
	@param x/y The space-delimited x and y values to offset the image by.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setPositionOffset, ConsoleVoid, 3, 3, "(Vector2 x y)")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setPositionOffset() - Invalid number of parameters!");
		return;
	}

	const U32 posCount = Utility::mGetStringElementCount(argv[2]);
	if (posCount != 2)
	{
		Con::warnf("GuiSpriteCtrl::setPositionOffset() - Position requires x and y");
		return;
	}

	object->setPositionOffset(dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(Utility::mGetStringElement(argv[2], 0)));
}

/*! Gets the amount to offset the image from its actual position. The offset may be greater than the movement of the image becuase of the bounds of the content area.
	@return The space-delimited x and y values to offset the image by.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getPositionOffset, ConsoleString, 2, 2, "()")
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &off = object->getPositionOffset();
	dSprintf(retBuffer, 64, "%d %d", off.x, off.y);
	return retBuffer;
}

/*! Sets the size of the image. The image will not exceed the bounds of the content area.
	@param x/y The space-delimited x and y values of the image size.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImageSize, ConsoleVoid, 3, 3, "(Vector2 x y)")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setImageSize() - Invalid number of parameters!");
		return;
	}

	const U32 posCount = Utility::mGetStringElementCount(argv[2]);
	if (posCount != 2)
	{
		Con::warnf("GuiSpriteCtrl::setImageSize() - Size requires x and y");
		return;
	}

	object->setImageSize(Point2I(dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(Utility::mGetStringElement(argv[2], 0))));
}

/*! Gets the size of the image.
	@return The space-delimited x and y values of the image size.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImageSize, ConsoleString, 2, 2, "()")
{
	char *retBuffer = Con::getReturnBuffer(64);
	const Point2I &size = object->getImageSize();
	dSprintf(retBuffer, 64, "%d %d", size.x, size.y);
	return retBuffer;
}

/*! Sets the color that should be used to display the image.
	@param r/g/b/a The space-delimited red, green, blue, and alpha values between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImageColor, ConsoleVoid, 3, 3, "(color red/green/blue/[alpha])")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setImageColor() - Invalid number of parameters!");
		return;
	}

	const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
	if (colorCount == 1)
	{
		// Is this a stock color name?
		if (!StockColor::isColor(argv[2]))
		{
			// No, so warn.
			Con::warnf("GuiSpriteCtrl::setImageColor() - Invalid single argument of '%s' could not be interpreted as a stock color name.", argv[2]);
			return;
		}

		// Set stock color (if it's invalid we'll get the default.
		object->setImageColor(ColorI(argv[2]));

		return;
	}

	if (colorCount != 3 && colorCount != 4)
	{
		Con::warnf("GuiSpriteCtrl::setImageColor() - Colors require 4 values between 0 and 255 (red, green, blue, alpha)!");
		return;
	}

	U8 red, green, blue, alpha;
	red = dAtoi(Utility::mGetStringElement(argv[2], 0));
	green = dAtoi(Utility::mGetStringElement(argv[2], 1));
	blue = dAtoi(Utility::mGetStringElement(argv[2], 2));
	alpha = colorCount == 4 ? dAtoi(Utility::mGetStringElement(argv[2], 3)) : 255;
	

	object->setImageColor(ColorI(red, green, blue, alpha));
}

/*! Gets the current image color.
	@return The space-delimited red, green, blue, and alpha values of the color.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImageColor, ConsoleString, 2, 2, "()")
{
	char *retBuffer = Con::getReturnBuffer(64);
	const ColorI &color = object->getImageColor();
	dSprintf(retBuffer, 64, "%d %d %d %d", color.red, color.green, color.blue, color.alpha);
	return retBuffer;
}

/*! Sets if the bitmap should be broken into multiple frames based on the color of the first pixel.
	@param isSingle If true, the bitmap has one and only one frame. If false, the top left pixel color will be used to divide the bitmap into frames.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setSingleFrameBitmap, ConsoleVoid, 3, 3, "(bool isSingle)")
{
	//Are we using an asset?
	if (object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::setSingleFrameBitmap() - Method invalid, using an asset.");
		return;
	}

	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setSingleFrameBitmap() - Invalid number of parameters!");
		return;
	}

	object->setSingleFrameBitmap(dAtob(argv[2]));
}

/*! Gets if the bitmap should be broken into multiple frames based on the color of the first pixel.
	@return Returns true if there's only one frame for the bitmap and false otherwise.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getSingleFrameBitmap, ConsoleBool, 2, 2, "()")
{
	//Are we using a asset?
	if (object->usesAsset())
	{
		// Yes, so warn.
		Con::warnf("GuiSpriteCtrl::getSingleFrameBitmap() - Method invalid, using an asset.");
		return true;
	}

	return object->getSingleFrameBitmap();
}

/*! Sets if the image should tile across the content area of the control.
	@param tileImage If true, the image will tile. If false, it will display a single copy of the image.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setTileImage, ConsoleVoid, 3, 3, "(bool tileImage)")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setTileImage() - Invalid number of parameters!");
		return;
	}

	object->setTileImage(dAtob(argv[2]));
}

/*! Gets if the image will tile across the content area of the control.
	@return True if tiling is on and false otherwise.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getTileImage, ConsoleBool, 2, 2, "()")
{
	return object->getTileImage();
}

/*! Sets if the image should take the entire content area.
	@param fullSize If true, the image will take the entire content area. False will cause the image to use the image size.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setFullSize, ConsoleVoid, 3, 3, "(bool fullSize)")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setFullSize() - Invalid number of parameters!");
		return;
	}

	object->setFullSize(dAtob(argv[2]));
}

/*! Gets if the image will take the entire content area.
	@return True if using the entire content area and false otherwise.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getFullSize, ConsoleBool, 2, 2, "()")
{
	return object->getFullSize();
}

/*! Sets if the image should keep its aspect ratio.
	@param constrain If true, maintain its aspect ratio. False otherwise.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setConstrainProportions, ConsoleVoid, 3, 3, "(bool constrain)")
{
	if (argc != 3)
	{
		Con::warnf("GuiSpriteCtrl::setConstrainProportions() - Invalid number of parameters!");
		return;
	}

	object->setConstrainProportions(dAtob(argv[2]));
}

/*! Gets if the image will maintain its aspect ratio.
	@return True it will constrain proportions and false otherwise.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getConstrainProportions, ConsoleBool, 2, 2, "()")
{
	return object->getConstrainProportions();
}

/*! Animates the position offset from its current value to a target value over time.
	@param x/y The space-delimited x and y values to change the image offset to.
	@param time The time in miliseconds it should take to complete the transformation.
	@param ease The optional easing function that should be applied to the the animation. Defaults to linear.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, moveTo, ConsoleVoid, 4, 5, "(Vector2 x y, int time, [EasingFunction ease])")
{
	if (argc != 4 && argc != 5)
	{
		Con::warnf("GuiSpriteCtrl::moveTo() - Invalid number of parameters!");
		return;
	}

	const U32 posCount = Utility::mGetStringElementCount(argv[2]);
	if (posCount != 2)
	{
		Con::warnf("GuiSpriteCtrl::moveTo() - Position requires x and y");
		return;
	}

	EasingFunction ease = Linear;
	if (argc == 5)
	{
		for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
		{
			if (dStricmp(easingEnums[i].label, argv[4]) == 0)
				ease = (EasingFunction)easingEnums[i].index;
		}
	}

	object->moveTo(dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(argv[3]), ease);
}

/*! Animates the image size from its current value to a target value over time.
	@param x/y The space-delimited x and y values to change the image size to. Note that the image will not exceed the content area of the control.
	@param time The time in miliseconds it should take to complete the transformation.
	@param ease The optional easing function that should be applied to the the animation. Defaults to linear.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, growTo, ConsoleVoid, 4, 5, "(Vector2 x y, int time, [EasingFunction ease])")
{
	if (argc != 4 && argc != 5)
	{
		Con::warnf("GuiSpriteCtrl::growTo() - Invalid number of parameters!");
		return;
	}

	const U32 posCount = Utility::mGetStringElementCount(argv[2]);
	if (posCount != 2)
	{
		Con::warnf("GuiSpriteCtrl::growTo() - Position requires x and y");
		return;
	}

	EasingFunction ease = Linear;
	if (argc == 5)
	{
		for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
		{
			if (dStricmp(easingEnums[i].label, argv[4]) == 0)
				ease = (EasingFunction)easingEnums[i].index;
		}
	}

	object->growTo(dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(Utility::mGetStringElement(argv[2], 0)),
		dAtoi(argv[3]), ease);
}

/*! Animates the image color from its current value to a target value over time.
	@param red/green/blue/alpha The space-delimited color values to change the image color to. Color values should be between 0 and 255.
	@param time The time in miliseconds it should take to complete the transformation.
	@param ease The optional easing function that should be applied to the the animation. Defaults to linear.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, fadeTo, ConsoleVoid, 4, 5, "(Vector2 x y, int time, [EasingFunction ease])")
{
	if (argc != 4 && argc != 5)
	{
		Con::warnf("GuiSpriteCtrl::fadeTo() - Invalid number of parameters!");
		return;
	}

	EasingFunction ease = Linear;
	if (argc == 5)
	{
		for (U32 i = 0; i < (sizeof(easingEnums) / sizeof(EnumTable::Enums)); i++)
		{
			if (dStricmp(easingEnums[i].label, argv[4]) == 0)
				ease = (EasingFunction)easingEnums[i].index;
		}
	}

	const U32 colorCount = Utility::mGetStringElementCount(argv[2]);
	if (colorCount == 1)
	{
		// Is this a stock color name?
		if (!StockColor::isColor(argv[2]))
		{
			// No, so warn.
			Con::warnf("GuiSpriteCtrl::fadeTo() - Invalid single argument of '%s' could not be interpreted as a stock color name.", argv[2]);
			return;
		}

		// Set stock color (if it's invalid we'll get the default).
		object->fadeTo(ColorI(argv[2]), dAtoi(argv[3]), ease);

		return;
	}

	if (colorCount != 4)
	{
		Con::warnf("GuiSpriteCtrl::fadeTo() - Color requires 4 values between 0 and 255 (red, green, blue, alpha)");
		return;
	}

	object->fadeTo(ColorI(dAtof(Utility::mGetStringElement(argv[2], 0)),
		dAtof(Utility::mGetStringElement(argv[2], 1)),
		dAtof(Utility::mGetStringElement(argv[2], 2)),
		dAtof(Utility::mGetStringElement(argv[2], 3))),
		dAtoi(argv[3]), ease);
}

ConsoleMethodGroupEndWithDocs(GuiSpriteCtrl)
