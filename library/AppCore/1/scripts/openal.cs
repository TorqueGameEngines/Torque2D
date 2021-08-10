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

//------------------------------------------------------------------------------
// Audio channel descriptions.
//------------------------------------------------------------------------------
$musicAudioType = 1;
$effectsAudioType = 2;

//------------------------------------------------------------------------------
// initializeOpenAL
// Starts up the OpenAL driver.
//------------------------------------------------------------------------------
function initializeOpenAL()
{
    // Just in case it is already started.
    shutdownOpenAL();

    echo("OpenAL Driver Init");

    if (!OpenALInitDriver())
    {
        echo("OpenALInitDriver() failed");
        $Audio::initFailed = true;
    }
    else
    {
        // Set the master volume.
        alxListenerf(AL_GAIN_LINEAR, $pref::Audio::masterVolume);

        // Set the channel volumes.
        for (%channel = 1; %channel <= 3; %channel++)
            alxSetChannelVolume(%channel, $pref::Audio::channelVolume[%channel]);

        echo("OpenAL Driver Init Success");
    }
}

//------------------------------------------------------------------------------
// shutdownOpenAL
//------------------------------------------------------------------------------
function shutdownOpenAL()
{
    OpenALShutdownDriver();
}