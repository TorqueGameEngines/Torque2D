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

ConsoleMethodGroupBeginWithDocs(SpineObject, SceneObject)

/*! Sets the spine asset Id to use.
	@param spineAssetId Integer - The spine asset Id to use.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSpineAsset, ConsoleVoid, 3, 3, (int spineAssetId))
{
	object->setSpineAsset(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the spine asset Id.
	@return Integer - The spine asset Id.
*/
ConsoleMethodWithDocs(SpineObject, getSpineAsset, ConsoleString, 2, 2, ())
{
	return object->getSpineAsset();
}

//-----------------------------------------------------------------------------

/*! Sets the animation time scale factor.
	@param timeScale Float - The factor by which to multiply the base time scale as
	set in the animation.
	@return Float - The previous value of the time scale.
*/
ConsoleMethodWithDocs(SpineObject, setTimeScale, ConsoleFloat, 3, 3, (float timeScale))
{
	return object->setTimeScale(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets the animation time scale.
	@return Float - The current animation time factor.
*/
ConsoleMethodWithDocs(SpineObject, getTimeScale, ConsoleFloat, 2, 2, ())
{
	return object->getTimeScale();
}

//-----------------------------------------------------------------------------

/*! Sets the animation for the object.
	@param animationName String - containing animation name to run.
	@param track Int - Optional. Track to run animation in.  Defaults to zero.
	@param loop Bool - Optional.  Determines whether the animation should loop. Defaults to false.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition (or mix) from the
	previously running animation to this one being set.  If not set, it defaults to the value set
	using the setMix() method.  If nothing was defined via setMix, then it defaults to zero, which means
	no transition, an abrupt change.
	@return Return Bool - True on success.
*/
ConsoleMethodWithDocs(SpineObject, setAnimation, ConsoleBool, 3, 6, (char *animationName, [int track, bool loop, float mixDuration]))
{
	// Determine looping
	int track = argc >= 4 ? dAtoi(argv[3]) : 0;
	bool shouldLoop = argc >= 5 ? dAtob(argv[4]) : false;
	F32 mixDuration = argc >= 6 ? dAtof(argv[5]) : -1.0f;
	return object->setAnimation(argv[2], track, shouldLoop, mixDuration);
}

//-----------------------------------------------------------------------------

/*! Sets the empty animation on the object.  This is mainly used to fade into and out of the pose
	position, but can be used for whatever.
	@param track Int - Optional. Track to run animation in.  Defaults to zero.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition from the
	currently running animation to this one.
	@return Return Bool - True on success.
*/
ConsoleMethodWithDocs(SpineObject, setEmptyAnimation, ConsoleBool, 2, 4, ([int track, float mixDuration]))
{
	int track = argc >= 3 ? dAtoi(argv[2]) : 0;
	F32 mixDuration = argc >= 4 ? dAtof(argv[3]) : 0.0f;
	return object->setEmptyAnimation(track, mixDuration);
}

//-----------------------------------------------------------------------------

/*! Queues the animation for the object.  The queued animation will play after the currently
	playing	animation completes.
	@param animationName String - Contains the name of the animation to queue.
	@param track Int - Optional. Track to run in.  Defaults to zero.  Animations can be layered in
	different tracks.
	@param loop Boolean - Optional. Indicates if animation should loop once it's processed.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition from the
	currently running animation to this one.
	@param delay Float - Optional.  Indicates a lag (leading or trailing) between this and
	the currently running animation.  In seconds.
	@return Returns Bool - True on success.
*/
ConsoleMethodWithDocs(SpineObject, queueAnimation, ConsoleBool, 3, 7, (char *animationName, [int track, bool loop, float mixDuration, float delay]))
{
	int track = argc >= 4 ? dAtoi(argv[3]) : 0;
	bool shouldLoop = argc >= 5 ? dAtob(argv[4]) : false;
	F32 mixDuration = argc >= 6 ? dAtof(argv[5]) : -1.0f;
	F32 delay = argc >= 7 ? dAtof(argv[6]) : 0.0f;
	return object->queueAnimation(argv[2], track, shouldLoop, mixDuration, delay);
}

//-----------------------------------------------------------------------------

/*! Queues the empty animation on the object.  Useful for fading and transitions.
	@param track Int - Optional. Track to run animation in.  Defaults to zero.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition from the
	currently running animation to this one.
	@param delay Float - Optional.  Indicates a lag (leading or trailing) between this and
	the currently running animation.  In seconds.  Defaults to zero.
	@return Return Bool - True on success.
*/
ConsoleMethodWithDocs(SpineObject, queueEmptyAnimation, ConsoleBool, 2, 5, ([int track, float mixDuration, float delay]))
{
	int track = argc >= 3 ? dAtoi(argv[2]) : 0;
	F32 mixDuration = argc >= 4 ? dAtof(argv[3]) : 0.0f;
	F32 delay = argc >= 5 ? dAtof(argv[4]) : 0.0f;
	return object->queueEmptyAnimation(track, mixDuration, delay);
}

//-----------------------------------------------------------------------------

/*! Clears animations (running and queued) from the given track.
	@param track Int - Optional. Track to clear animations from.  Defaults to zero.
	@param mixToSetup Bool - Optional. If true the track will fade back to the setup pose
	over the number of seconds specified in the mixDuration argument.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition back to
	the setup pose.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, clearAnimations, ConsoleVoid, 2, 5, ([int track, bool mixToSetup, float mixDuration]))
{
	int track = argc >= 3 ? dAtoi(argv[2]) : 0;
	bool mixToSetup = argc >= 4 ? dAtob(argv[3]) : false;
	F32 mixDuration = argc >= 5 ? dAtof(argv[4]) : 0.0f;
	object->clearAnimations(track, mixToSetup, mixDuration);
}

//-----------------------------------------------------------------------------

/*! Clears animations (running and queued) from all tracks.
	@param mixToSetup Bool - Optional. If true the character will fade back to the setup pose
	over the number of seconds specified in the mixDuration argument.
	@param mixDuration Float - Optional.  The amount of time in seconds to transition back to
	the setup pose.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, clearAllAnimations, ConsoleVoid, 2, 4, ([bool mixToSetup, float mixDuration]))
{
	bool mixToSetup = argc >= 3 ? dAtob(argv[2]) : false;
	F32 mixDuration = argc >= 4 ? dAtof(argv[3]) : 0.0f;
	object->clearAllAnimations(mixToSetup, mixDuration);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the animation currently running in the given track.
	@param track Int - Optional. Track to get name from.  Defaults to zero.
	@return String - Contains the animation name.
*/
ConsoleMethodWithDocs(SpineObject, getAnimationName, ConsoleString, 2, 3, ([int track]))
{
	int track = argc >= 3 ? dAtoi(argv[2]) : 0;
	return object->getAnimationName(track);
}

//-----------------------------------------------------------------------------

/*! Check if the animation currently running on the given track is looping.
	@param track Int - Optional. Track to check.  Defaults to zero.
	@return Bool - True if current animation is looping.  False if there is no animation
	running or there is but it is not looping.
*/
ConsoleMethodWithDocs(SpineObject, getIsLooping, ConsoleBool, 2, 3, ([int track]))
{
	int track = argc >= 3 ? dAtoi(argv[2]) : 0;
	return object->getIsLooping(track);
}

//-----------------------------------------------------------------------------

/*! Sets the animation mix time to be used when transitioning between two
	animations.  This is the amount of time that will be spent morphing
	between the from and to animations.
	@param fromAnimation String - The name of the animation transitioning from.
	@param toAnimation String - The name of the animation transitioning to.
	@param mixDuration Float - The number of seconds to spend in the transition.
	@return Returns Bool - True on success.
*/
ConsoleMethodWithDocs(SpineObject, setMix, ConsoleBool, 5, 5, (char *fromAnimation, char *toAnimation, float mixDuration))
{
	Con::printf("Mixing %s with %s at %f", argv[2], argv[3], dAtof(argv[4]));

	return object->setMix(argv[2], argv[3], dAtof(argv[4]));
}

//-----------------------------------------------------------------------------

/*! Sets the skin for the spine object.
	@param skinName String - Name of the skin (as defined in the animation data) to apply.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSkin, ConsoleVoid, 3, 3, (char *skinName))
{
	object->setSkin(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Gets the name of the skin currently applied to the spine object.
	@return String - The currently applied skin name.
*/
ConsoleMethodWithDocs(SpineObject, getSkinName, ConsoleString, 2, 2, ())
{
	return object->getSkinName();
}

//-----------------------------------------------------------------------------

/*! Sets scaling of the spine object's skeleton.
	@param scaleX Float - X factor.
	@param scaleY Float - Y factor.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setScale, ConsoleVoid, 3, 4, (float scaleX, float scaleY))
{
	F32 scaleX, scaleY;

	const U32 elementCount = Utility::mGetStringElementCount(argv[2]);

	// ("width height")
	if ((elementCount == 2) && (argc == 3))
	{
		scaleX = dAtof(Utility::mGetStringElement(argv[2], 0));
		scaleY = dAtof(Utility::mGetStringElement(argv[2], 1));
	}

	// (width, [height])
	else if (elementCount == 1)
	{
		scaleX = dAtof(argv[2]);

		if (argc > 3)
			scaleY = dAtof(argv[3]);
		else
			scaleY = scaleX;
	}

	// Invalid
	else
	{
		Con::warnf("SpineObject::setScale() - Invalid number of parameters!");
		return;
	}

	// Set Scale.
	object->setScale(Vector2(scaleX, scaleY));
}

//-----------------------------------------------------------------------------

/*! Gets the spine object's skeleton scale.
	@return (float x/y height) The x and y scale of the object's root bone.
*/
ConsoleMethodWithDocs(SpineObject, getScale, ConsoleString, 2, 2, ())
{
	return object->getScale().scriptThis();
}

//-----------------------------------------------------------------------------

/*! Sets the sprite texture flipping for each axis.
	@param flipX Bool - Whether or not to flip the texture along the x (horizontal) axis.
	@param flipY Bool - Whether or not to flip the texture along the y (vertical) axis.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setFlip, ConsoleVoid, 4, 4, (bool flipX, bool flipY))
{
	// Set Flip.
	object->setFlip(dAtob(argv[2]), dAtob(argv[3]));
}

//-----------------------------------------------------------------------------

/*! Gets the flip for each axis.
	@return (bool flipX/bool flipY) Whether or not the texture is flipped along the x and y axes.
*/
ConsoleMethodWithDocs(SpineObject, getFlip, ConsoleString, 2, 2, ())
{
	// Create Returnable Buffer.
	char* pBuffer = Con::getReturnBuffer(32);

	// Format Buffer.
	dSprintf(pBuffer, 32, "%d %d", object->getFlipX(), object->getFlipY());

	// Return Buffer.
	return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped horizontally.
	@param flipX Bool - Whether or not to flip the texture along the x (horizontal) axis.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setFlipX, ConsoleVoid, 3, 3, (bool flipX))
{
	// Set Flip.
	object->setFlipX(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Sets whether or not the texture is flipped vertically.
	@param flipY Bool - Whether or not to flip the texture along the y (vertical) axis.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setFlipY, ConsoleVoid, 3, 3, (bool flipY))
{
	// Set Flip.
	object->setFlipY(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped horizontally.
	@return (bool flipX) Whether or not the texture is flipped along the x axis.
*/
ConsoleMethodWithDocs(SpineObject, getFlipX, ConsoleBool, 2, 2, ())
{
	return object->getFlipX();
}

//-----------------------------------------------------------------------------

/*! Gets whether or not the texture is flipped vertically.
	@return (bool flipY) Whether or not the texture is flipped along the y axis.
*/
ConsoleMethodWithDocs(SpineObject, getFlipY, ConsoleBool, 2, 2, ())
{
	return object->getFlipY();
}

//-----------------------------------------------------------------------------

/*! Enables the Jitter special effect
	@param JitterX Float - A special effect control parameter.
	@param JitterY Float - A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, enableJitter, ConsoleVoid, 4, 4, (float x, float y))
{
	// Enable the special effect
	object->enableJitter(dAtof(argv[2]), dAtof(argv[3]));
}

//-----------------------------------------------------------------------------

/*! Set the Jitter X control value.
	@param X - Float A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setJitterX, ConsoleVoid, 3, 3, (float x))
{
	// Enable the special effect
	object->setJitterX(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Jitter X control value.
	@return The current Jitter effect X control value.  If the Jitter effect is not or
	has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getJitterX, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getJitterX();
}

//-----------------------------------------------------------------------------

/*! Set the Jitter Y control value.
	@param Y Float - A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setJitterY, ConsoleVoid, 3, 3, (float y))
{
	// Enable the special effect
	object->setJitterY(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Jitter Y control value.
	@return The current Jitter effect Y control value.  If the Jitter effect is not
	or has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getJitterY, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getJitterY();
}

//-----------------------------------------------------------------------------

/*! Disables the Jitter special effect.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, disableJitter, ConsoleVoid, 2, 2, ())
{
	return object->disableJitter();
}

//-----------------------------------------------------------------------------

/*! Enables the Swirl special effect
	@param Radius Float - In degrees.  A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, enableSwirl, ConsoleVoid, 3, 3, (float radius))
{
	// Enable the special effect
	object->enableSwirl(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Set the Swirl X control value.
	@param X Float - A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSwirlX, ConsoleVoid, 3, 3, (float x))
{
	// Enable the special effect
	object->setSwirlX(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Swirl X control value.
	@return The current Swirl effect X control value.  If the Swirl effect is not
	or has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getSwirlX, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getSwirlX();
}

//-----------------------------------------------------------------------------

/*! Set the Swirl Y control value.
	@param Y Float - A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSwirlY, ConsoleVoid, 3, 3, (float y))
{
	// Enable the special effect
	object->setSwirlY(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Swirl Y control value.
	@return The current Swirl effect Y control value.  If the Swirl effect is not
	or has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getSwirlY, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getSwirlY();
}

//-----------------------------------------------------------------------------

/*! Set the Swirl Radius control value.
	@param Radius Float - A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSwirlRadius, ConsoleVoid, 3, 3, (float radius))
{
	// Enable the special effect
	object->setSwirlRadius(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Swirl Radius control value.
	@return The current Swirl effect Radius control value.  If the Swirl effect is not
	or has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getSwirlRadius, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getSwirlRadius();
}

//-----------------------------------------------------------------------------

/*! Set the Swirl Angle control value.
	@param Angle Float - In degrees.  A special effect control parameter.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, setSwirlAngle, ConsoleVoid, 3, 3, (float radius))
{
	// Enable the special effect
	object->setSwirlAngle(dAtof(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Get the Swirl Angle control value.
	@return The current Swirl effect Angle control value.  If the Swirl effect is not
	or has not been enabled, it will return zero.
*/
ConsoleMethodWithDocs(SpineObject, getSwirlAngle, ConsoleFloat, 2, 2, ())
{
	// Enable the special effect
	return object->getSwirlAngle();
}

//-----------------------------------------------------------------------------

/*! Disables the Swirl special effect.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, disableSwirl, ConsoleVoid, 2, 2, ())
{
	return object->disableSwirl();
}

//-----------------------------------------------------------------------------

/*! Enables animation event callbacks.
	After calling this method, your SpineObject instance will begin to receive animation lifecycle callbacks.
	This includes events defined within the animation when it was created.  Those embedded events will arrive
	in the 'onAnimationEvent' callback, where '%eventName' will indicate which particular event occurred.

@note The onAnimationEvent callback comes with a large number of arguments.  These correspond in large part to
	  the attributes that can be defined for the event inside Spine's animation editor.  It is safe to ignore
	  them if not needed.

	The following code block shows every available event callback method.  It is okay to define handlers for only
	those events of interest, though enabling callbacks and not defining any would just be a waste.  More likely
	would be to remove the last callback at some future time and forget to disable them.

	@code
	function SpineObject::onAnimationStart(%this, %animationName, %animationTrack){
		echo("SpineObject received onAnimationStart: "@%animationName);
	}

	function SpineObject::onAnimationInterrupt(%this, %animationName, %animationTrack){
		echo("SpineObject received onAnimationInterrupt: "@%animationName);
	}

	function SpineObject::onAnimationEnd(%this, %animationName, %animationTrack){
		echo("SpineObject received onAnimationEnd: "@%animationName);
	}

	function SpineObject::onAnimationComplete(%this, %animationName, %animationTrack){
		echo("SpineObject received onAnimationComplete: "@%animationName);
	}

	function SpineObject::onAnimationDispose(%this, %animationName, %animationTrack){
		echo("SpineObject received onAnimationDispose: "@%animationName);
	}

	function SpineObject::onAnimationEvent(%this, %animationName, %animationTrack, %eventName, %intArg, %floatArg, %stringArg, %time, %volume, %balance){
		echo("SpineObject received onAnimationEvent:" SPC %animationName SPC %eventName);
		if(%eventName $= "footstep"){
			alxPlay("GameAssets:KnightFootstep");
		}
	}
	@endcode

	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, enableAnimationEventCallbacks, ConsoleVoid, 2, 2, ())
{
	object->enableEventCallbacks();
}

//-----------------------------------------------------------------------------

/*! Disables animation event callbacks.
	@return No return value.
*/
ConsoleMethodWithDocs(SpineObject, disableAnimationEventCallbacks, ConsoleVoid, 2, 2, ())
{
	object->disableEventCallbacks();
}

//-----------------------------------------------------------------------------

/*! Retrieve a collision proxy object.  This is a scene object that wraps a collsion box centered on
	a spine attachment.  Since it's a SceneObject, it can then be manipulated/used for collision
	processing in onCollision etc... This first checks if the attachment requested already has a proxy
	and returns that if so.  If not, a new proxy is created and returned.
	@note
	The collision objects never have any velocity.  They are continually positioned (warped) to match
	the location of the attachment they were created on.  This means that they are only useful as
	sensors.  Even if they are not explicitly set to be sensors, since they have no velocity, they
	don't trigger an impulse or other response.

	@note
	The generated collision proxy is assigned to the same group and layer as the parent spine object.
	In addition, its collision mask is configured to disable collisions with all other collision boxes
	in that same group.  The intent is to prevent collisions between attachements on the same spine
	object.  Therefore, for different spine objects to collide, they should be assigned to different
	scene groups.  However, if needed, the settings on the returned proxy may be changed by making the
	appropriate	calls.

	@param AttachmentName String - The spine attachment name that the new proxy should wrap.
	@param SlotName String - The slot name that the new proxy should wrap.  The slot and attachment
	names form a compound key.  Both are required to identify the attachment to get a collision
	box for.
	@param SkinName String Optional - The skin in which to search for the specified attachment name.
	If given, then only that skin will be searched for the attachment, and creation will fail if it is not
	found.  If this parameter is not given (or is set to "default"), then the current skin will be searched
	first and if not found, the default skin will be searched.  Use "default" if you want the default
	behavior while needing to define further optional parameters.
	@param SizerWidth Float Optional - A factor that can be used to arbitrarily shrink or grow the generated
	box's width.  The box remains centered on the attachment.
	@param SizerHeight Float Optional - A factor that can be used to arbitrarily shrink or grow the generated
	box's height.
	@param ObjectName String Optional - Used to set the name of the object returned to the caller.

	@return The requested SpineCollisionProxy object, or nothing if creation failed.
*/
ConsoleMethodWithDocs(SpineObject, getCollisionProxy, ConsoleString, 4, 8, (char *attachmentName, char *slotName, [char *skinName, float sizerWidth, float sizerHeight, char *objectName]))
{
	const char* skinName = argc >= 5 ? argv[4] : "default";
	const F32 sizerWidth = argc >= 6 ? dAtof(argv[5]) : 1.0f;
	const F32 sizerHeight = argc >= 7 ? dAtof(argv[6]) : 1.0f;
	const char* objectName = argc >= 8 ? argv[7] : NULL;

	// Get the collision proxy for the requested spine attachment.
	const SceneObject* pProxy = object->getCollisionProxy(argv[2], argv[3], skinName, sizerWidth, sizerHeight, objectName);

	return pProxy ? pProxy->getIdString() : StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

/*! Delete the given collision proxy.
	Proxies will be deleted automatically at cleanup.  However, this can be used
	to delete one sooner.
	@param proxy SpineCollisionProxy - The proxy to delete.
	@return bool - True if the object was deleted, false otherwise.
*/
ConsoleMethodWithDocs(SpineObject, deleteCollisionProxy, ConsoleBool, 3, 3, (SpineCollsionProxy proxy))
{
	// Enable the special effect
	return object->deleteCollisionProxy(argv[2]);
}


ConsoleMethodGroupEndWithDocs(SpineObject)
