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

if (!isObject(SoundBehavior))
{
    %template = new BehaviorTemplate(SoundBehavior);

    %template.addBehaviorField(DeathSound, "The audio asset Id", asset, "", AudioAsset);

    %template.addBehaviorInput(playSound, "", "Plays the death sound");
    %template.addBehaviorInput(removeBlock, "", "Removes a block preventing a sound from playing");
}

//-----------------------------------------------------------------------------

function SoundBehavior::onAddToScene(%this, %scene)
{
    // Find out if this object has the button behavior.
    %button = %this.owner.getBehavior("ButtonBehavior");

    // If yes, create a behavior connection.
    if (isObject(%button))
        %this.owner.connect(%button, %this, buttonDown, playSound);

    // Find out if this object has an animation behavior.
    %animation = %this.owner.getBehavior("AnimationBehavior");

    // If yes, create a behavior connection.
    if (isObject(%animation))
        %this.owner.connect(%animation, %this, reset, removeBlock);

    // Set the sound block state to false.
    %this.Block = false;
}

//-----------------------------------------------------------------------------

function SoundBehavior::playSound(%this, %fromBehavior, %fromOutput)
{
    // Skip playing a sound if the object's animation state is not yet reset.
    if (%this.Block)
        return;

    // Play a sound effect if nothing is currently playing.
    if (!alxIsPlaying(%this.owner.Sound))
        %this.owner.Sound = alxPlay(%this.DeathSound);

    // Prevent the sound from playing again until a reset signal arrives.
    %this.Block = true;
}

//-----------------------------------------------------------------------------

function SoundBehavior::removeBlock(%this, %fromBehavior, %fromOutput)
{
    // Reset the sound block state.
    %this.Block = false;
}
