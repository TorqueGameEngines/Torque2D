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

function TextSpriteToy::create( %this )
{
    // Reset the toy.
    TextSpriteToy.reset();
}

//-----------------------------------------------------------------------------

function TextSpriteToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function TextSpriteToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();

    //Title
    new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:TrajanProFont";
        FontSize = 6.5;
        Text = "Presenting the TextSprite!";
        Position = "0 30";
        Size = "90 7";
        OverflowModeX = "visible";
        TextAlignment = "center";
        BlendColor = "0.2 0.5 1 1";
    };

    //Opening Description
    new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:ArialFont";
        FontSize = 3;
        Text = "The TextSprite takes a FontAsset which directly loads a bitmap font file, so adding a new font is super easy! The TextSprite let's you align the text to the left, right, center or even justified. It can also be vertically aligned to the top, middle, or bottom. There's multiple overflow options including automatically shrinking text fit the box. Finally, you can control the color, offset, and scale of each character individually! Check out these examples!";
        Position = "0 19";
        Size = "90 15";
        OverflowModeY = "visible";
        BlendColor = "1 1 1 0.8";
    };

    %example1 = new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:OratorBoldFont";
        FontSize = 5;
        Text = "You can make any letter big or small!";
        Position = "-30 -8";
        Size = "24 24";
        BlendColor = "1 1 1 1";
    };

    for(%i = 24; %i <= 26; %i++)
    {
        %example1.setCharacterScale(%i, "1.7 2");
    }

    for(%i = 31; %i <= 35; %i++)
    {
        %example1.setCharacterScale(%i, "0.8 0.6");
    }

    %example2 = new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:TrajanProFont";
        FontSize = 4.3;
        Text = "Don't be boring! Add some COLOR and let it spin!";
        Position = "0 -5";
        Size = "24 24";
        BlendColor = "1 1 1 1";
        AngularVelocity = 30;
        TextAlignment = "center";
        TextVAlignment = "middle";
        autoLineHeight = false;
        customLineHeight = 3.8;
    };
    %example2.setCharacterBlendColor(26, "1 0 0 1");
    %example2.setCharacterBlendColor(27, "1 0.5 0 1");
    %example2.setCharacterBlendColor(28, "1 1 0 1");
    %example2.setCharacterBlendColor(29, "0.2 1 0.2 1");
    %example2.setCharacterBlendColor(30, "0 0.2 1 1");

    %example3 = new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:ArialFont";
        FontSize = 3.5;
        Text = "With a little work you can get your text to jump out at your readers!";
        Position = "30 -8";
        Size = "24 24";
        BlendColor = "1 1 1 1";
        TextAlignment = "right";
        TextVAlignment = "bottom";
        Class = "JumpingText";
        start = 44;
        end = 47;
        jumpSpeed = 0;
        letterHeight = 0;
        UpdateCallback = true;
        autoLineHeight = false;
        customLineHeight = 6;
    };

    %example4 = new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:ArialFont";
        FontSize = 3.7;
        Text = "You could build a dialog box that shows one letter at a time!";
        Position = "-24 -27";
        Size = "45 10";
        OverflowModeY = "visible";
        BlendColor = "1 1 1 0";
        Class = "DialogText";
        pen = 0;
    };
    %example4.showLetter();

    %example5 = new TextSprite()
    {
        Scene = SandboxScene;
        Font = "ToyAssets:OratorBoldFont";
        FontSize = 3.4;
        Text = "AJDIORQNAKMAKENZCBADTWOLFJEI PEPWQFDNWORDFHISUDFBHAKLSBLH RUEBNAJDUIOQPSEARCHUIRDFBUYJ MBPIRATEIDBGVMCODEWHTKEIAVIE";
        Position = "24 -25.5";
        Size = "45 10";
        OverflowModeY = "visible";
        BlendColor = "1 1 1 1";
        Class = "WordSearchText";
    };
    %example5.setCharacterBlendColor(4, "1 1 1 1");
    %example5.setCharacterBlendColor(5, "1 1 1 1");
    for(%i = 10; %i <= 13; %i++)
    {
        %example5.setCharacterBlendColor(%i, "1 1 1 1");
    }
    %example5.setCharacterBlendColor(18, "1 1 1 1");
    for(%i = 37; %i <= 40; %i++)
    {
        %example5.setCharacterBlendColor(%i, "1 1 1 1");
    }
    for(%i = 71; %i <= 76; %i++)
    {
        %example5.setCharacterBlendColor(%i, "1 1 1 1");
    }
    %example5.schedule(6000, "fadeOut");
}

//-----------------------------------------------------------------------------

function JumpingText::onUpdate(%this)
{
    if(%this.jumpSpeed == 0 && %this.letterHeight == 0)
    {
        //jump again!
        %this.jumpSpeed = 0.7;
    }

    %this.letterHeight += %this.jumpSpeed;
    if(%this.jumpSpeed > 0)
    {
        %this.jumpSpeed *= 0.8;
        if(%this.jumpSpeed < 0.1)
        {
            %this.jumpSpeed = -0.1;
        }
    }
    else if(%this.jumpSpeed < 0)
    {
        %this.jumpSpeed *= 1.2;
        if(%this.letterHeight < 0)
        {
            %this.letterHeight = 0;
            %this.jumpSpeed = 0;
            %this.setUpdateCallback(false);
            %this.schedule(1000, "setUpdateCallback", true);
        }
    }

    for(%i = %this.start; %i <= %this.end; %i++)
    {
        %this.setCharacterOffset(%i, "0" SPC %this.letterHeight);
    }
}

//-----------------------------------------------------------------------------

function DialogText::showLetter(%this)
{
    if(%this.pen == 0)
    {
        %this.resetCharacterSettings();
    }
    %this.setCharacterBlendColor(%this.pen, "1 1 1 1");
    %this.pen++;
    if(%this.pen >= strlen(%this.getText()))
    {
        %this.pen = 0;
        %this.schedule(3000, "showLetter");
    }
    else
    {
        %this.schedule(80, "showLetter");
    }
}

//-----------------------------------------------------------------------------

function WordSearchText::fadeOut(%this)
{
    %this.fadeTo("1 1 1 0", 0.6);

    %this.schedule(6000, "fadeIn");
}

//-----------------------------------------------------------------------------

function WordSearchText::fadeIn(%this)
{
    %this.fadeTo("1 1 1 1", 0.6);

    %this.schedule(6000, "fadeOut");
}
