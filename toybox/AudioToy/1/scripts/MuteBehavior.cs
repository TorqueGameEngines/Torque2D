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

if (!isObject(MuteBehavior))
{
    %template = new BehaviorTemplate(MuteBehavior);

    %template.addBehaviorInput(muteAudio, "", "Puts all audio on mute");
}

//-----------------------------------------------------------------------------

function MuteBehavior::onAddToScene(%this, %scene)
{
    // Find out if this object has the button behavior.
    %button = %this.owner.getBehavior("ButtonBehavior");

    // If yes, create a behavior connection.
    if (isObject(%button))
        %this.owner.connect(%button, %this, buttonDown, muteAudio);
}

//-----------------------------------------------------------------------------

function MuteBehavior::muteAudio(%this, %fromBehavior, %fromOutput)
{
    // Check which image is being used.
    switch$ (%this.owner.Image)
    {
        case "AudioToy:SoundOn":
            // Set the volume on channel 0 to 0%.
            alxSetChannelVolume(0, 0.0);
                
            // Change the image from unmute to mute.
            %this.owner.Image = "AudioToy:SoundOff";

        case "AudioToy:SoundOff":
            // Set the volume on channel 0 to 100%.
            alxSetChannelVolume(0, 1.0);

            // Change the image from mute to unmute.
            %this.owner.Image = "AudioToy:SoundOn";
    }
}
