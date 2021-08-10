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

if (!isObject(MusicBehavior))
{
    %template = new BehaviorTemplate(MusicBehavior);

    %template.addBehaviorInput(playMusic, "", "Plays the background music");
}

//-----------------------------------------------------------------------------

function MusicBehavior::onAddToScene(%this, %scene)
{
    // Find out if this object has the button behavior.
    %button = %this.owner.getBehavior("ButtonBehavior");

    // If yes, create a behavior connection.
    if (isObject(%button))
        %this.owner.connect(%button, %this, buttonDown, playMusic);

    // Initial value setup.
    %this.owner.Music = "";
    %this.owner.Paused = false;
}

//-----------------------------------------------------------------------------

function MusicBehavior::playMusic(%this, %fromBehavior, %fromOutput)
{
    // Check which image is being used.
    switch$ (%this.owner.Image)
    {
        case "AudioToy:Play":
            // If there is no audio handle, start playing music.
            if (%this.owner.Music $= "")
                %this.owner.Music = alxPlay(AudioToy.Music);

            // If the music is paused, unpause it.
            if (%this.owner.Paused)
            {
                alxUnpause(%this.owner.Music);
                %this.owner.Paused = false;
            }
                
            // Change the image from play to pause.
            %this.owner.Image = "AudioToy:Pause";

        case "AudioToy:Pause":
            // Only pause if we have a valid audio handle.
            if (%this.owner.Music !$= "")
            {
                // Pause the music and set the paused flag to true.
                alxPause(%this.owner.Music);
                %this.owner.Paused = true;
            }
            
            // Change the image from pause to play.
            %this.owner.Image = "AudioToy:Play";
    }
}
