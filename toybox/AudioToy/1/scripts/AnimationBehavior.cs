//-----------------------------------------------------------------------------
// Copyright (c) 2014 GarageGames, LLC
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

if (!isObject(AnimationBehavior))
{
    %template = new BehaviorTemplate(AnimationBehavior);

    %template.addBehaviorField(DeathAnimation, "The animation asset Id", asset, "", AnimationAsset);

    %template.addBehaviorInput(playDeath, "", "Plays the death animation");

    %template.addBehaviorOutput(reset, "", "Called when an animation has reset");
}

//-----------------------------------------------------------------------------

function AnimationBehavior::onAddToScene(%this, %scene)
{
    // Find out if this object has the button behavior.
    %button = %this.owner.getBehavior("ButtonBehavior");

    // If yes, create a behavior connection.
    if (isObject(%button))
        %this.owner.connect(%button, %this, buttonDown, playDeath);

    // Save the starting animation state.
    %this.StartAnimation = %this.owner.Animation;
}

//-----------------------------------------------------------------------------

function AnimationBehavior::playDeath(%this, %fromBehavior, %fromOutput)
{
    // Change to the death animation if it's not already playing.
    if (%this.owner.Animation $= %this.StartAnimation)
        %this.owner.Animation = %this.DeathAnimation;
}

//-----------------------------------------------------------------------------

function AnimationBehavior::onAnimationEnd(%this)
{
    // Get the current animation.
    %animation = %this.owner.getAnimation();

    // If it is the death animation, schedule a reset.
    if (%animation $= %this.DeathAnimation)
        %this.schedule(500, "resetAnimation");
}

//-----------------------------------------------------------------------------

function AnimationBehavior::resetAnimation(%this)
{
    // Change the current animation to the initial state.
    %this.owner.Animation = %this.StartAnimation;

    // Raise a signal to other behaviors that the animation state has reset.
    %this.owner.raise(%this, reset);
}
