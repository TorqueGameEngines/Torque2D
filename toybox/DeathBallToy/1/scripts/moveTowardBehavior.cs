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

function MoveTowardBehavior::initialize(%this, %target, %speed)
{
    %this.target = %target;
    %this.speed = %speed;
}

//-----------------------------------------------------------------------------

function MoveTowardBehavior::onBehaviorAdd(%this)
{
    %this.schedule(500, updateMoveToward);
}

//-----------------------------------------------------------------------------

function MoveTowardBehavior::updateMoveToward(%this)
{
    if (!isObject(%this.target))
        return;

    %relativePosition = VectorSub(%this.target.getPosition(), %this.owner.getPosition());
    
    %scaledVelocity = VectorScale(%relativePosition, %this.speed);

    %this.owner.setLinearVelocity( getWord(%scaledVelocity, 0), getWord(%scaledVelocity, 1) );
    
    %this.schedule(500, updateMoveToward);
}
