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


function FishClass::onCollision(%this, %object, %collisionDetails)
{
    if (%object.class $= "AquariumBoundary")
        %this.recycle(%object.side);
}

//-----------------------------------------------------------------------------

function FishClass::onAdd(%this)
{
    // Set a random speed for the fish
    %this.setSpeed();
   
    if (getRandom(0, 10) > 5)
    {
        %this.setLinearVelocityX(%this.speed);
        %this.setFlipX(false);
    }
    else
    {
        %this.setLinearVelocityX(-%this.speed);
        %this.setFlipX(true);
    }
}

//-----------------------------------------------------------------------------

function FishClass::recycle(%this, %side)
{
    // Fish has turned around, so set a new random speed
    %this.setSpeed();
    %layer = getRandom(0, 5);
    %this.setLinearVelocityY(getRandom(-3, 3));
    %this.setPositionY(getRandom(-15, 15));
    %this.setSceneLayer(%layer);

    if (%side $= "left")
    {
        %this.setLinearVelocityX(%this.speed);
        %this.setFlipX(false);
    }
    else if (%side $= "right")
    {
        %this.setLinearVelocityX(-%this.speed);
        %this.setFlipX(true);
    }
}

//-----------------------------------------------------------------------------

function FishClass::setSpeed(%this)
{
   // Speed is a dynamic variable created when this function is first called
   // Every other time after the first call will simply modify the variable
   // .minSpeed and .maxSpeed are declared in the Dynamic Fields rollout of the editor
   %this.speed = getRandom(%this.minSpeed, %this.maxSpeed);
}