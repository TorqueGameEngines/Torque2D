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

function AudioToy::create(%this)
{
    // Load script files.
    exec("./scripts/ButtonBehavior.cs");
    exec("./scripts/AnimationBehavior.cs");
    exec("./scripts/SoundBehavior.cs");
    exec("./scripts/MusicBehavior.cs");
    exec("./scripts/MuteBehavior.cs");

    // Turn on input events for scene objects.
    SandboxWindow.setUseObjectInputEvents(true);

    // Set the camera.
    SandboxWindow.setCameraSize(40, 30);
    
    // Setup default values.
    AudioToy.Music = "ToyAssets:titleMusic";
    
    // Sandbox options
    addSelectionOption("Title Music,Level Music,Win Music,Lose Music", "Background Music", 4, "setupMusic", true, "Choose a music file");
    
    // Reset the toy initially.
    AudioToy.reset();
}

//-----------------------------------------------------------------------------

function AudioToy::destroy(%this)
{
    // Stop all audio when quitting the module.
    alxStopAll();
}

//-----------------------------------------------------------------------------

function AudioToy::reset(%this)
{
    // Clear the scene.
    SandboxScene.clear();
    
    // Stop all music.
    alxStopAll();
    
    // Load scene objects from TAML.
    %background = TamlRead("./objects/Background.taml");
    %ground = TamlRead("./objects/Ground.taml");
    %grass = TamlRead("./objects/Grass.taml");
    %barbarian = TamlRead("./objects/Barbarian.taml");
    %dwarf = TamlRead("./objects/Dwarf.taml");
    %knight = TamlRead("./objects/Knight.taml");
    %wizard = TamlRead("./objects/Wizard.taml");
    %play = TamlRead("./objects/PlayButton.taml");
    %mute = TamlRead("./objects/MuteButton.taml");

    // Add objects to the Scene.
    SandboxScene.add(%background);
    SandboxScene.add(%ground);
    SandboxScene.add(%grass);
    SandboxScene.add(%barbarian);
    SandboxScene.add(%dwarf);
    SandboxScene.add(%knight);
    SandboxScene.add(%wizard);
    SandboxScene.add(%play);
    SandboxScene.add(%mute);
}

//-----------------------------------------------------------------------------

function AudioToy::setupMusic(%this, %value)
{
    // Reset the toy.
    AudioToy.reset();

    // Set the music to the proper audio asset.
    switch$(%value)
    {
        case "Title Music":
        AudioToy.Music = "ToyAssets:titleMusic";
      
        case "Level Music":
        AudioToy.Music = "ToyAssets:level1Music";
      
        case "Win Music":
        AudioToy.Music = "ToyAssets:winMusic";
      
        case "Lose Music":
        AudioToy.Music = "ToyAssets:loseMusic";
    }
}
