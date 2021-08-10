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

function SpawnAreaBehavior::initialize(%this, %object, %count, %spawnTime, %spawnVariance, %autoSpawn, %spawnLocation)
{
    %this.object = %object;
    %this.count = %count;
    %this.spawnTime = %spawnTime;
    %this.spawnVariance = %spawnVariance;
    %this.autoSpawn = %autoSpawn;
    %this.spawnLocation = %spawnLocation;
}

//-----------------------------------------------------------------------------

function SpawnAreaBehavior::onBehaviorAdd(%this, %scenegraph)
{
    %this.spawnCount = 0;
    %this.schedule(%this.spawnStart * 1000, "spawn");
}

//-----------------------------------------------------------------------------

function SpawnAreaBehavior::spawn(%this)
{
    if (!isObject(%this.object) || !%this.owner.enabled)
        return;

    %clone = %this.object.clone();

    SandboxScene.add(%clone);

    %clone.setEnabled(1);

    %xPos = 0;
    %yPos = 0;
    %spawnLocation = %this.spawnLocation;

    %edges = "Top" TAB "Bottom" TAB "Left" TAB "Right";

    if (%spawnLocation $= "Edges")
        %spawnLocation = getField(%edges, getRandom(0, 3));

    switch$ (%spawnLocation)
    {
        case "Area":
            %xPos = getRandom(getWord(%this.owner.getAreaMin(), 0), getWord(%this.owner.getAreaMax(), 0));
            %yPos = getRandom(getWord(%this.owner.getAreaMin(), 1), getWord(%this.owner.getAreaMax(), 1));
        case "Center":
            %xPos = %this.owner.position.x;
            %yPos = %this.owner.position.y;
        case "Top":
            %xPos = getRandom(getWord(%this.owner.getAreaMin(), 0), getWord(%this.owner.getAreaMax(), 0));
            %yPos = getWord(%this.owner.getAreaMin(), 1);
        case "Bottom":
            %xPos = getRandom(getWord(%this.owner.getAreaMin(), 0), getWord(%this.owner.getAreaMax(), 0));
            %yPos = getWord(%this.owner.getAreaMax(), 1);
        case "Left":
            %xPos = getWord(%this.owner.getAreaMin(), 0);
            %yPos = getRandom(getWord(%this.owner.getAreaMin(), 1), getWord(%this.owner.getAreaMax(), 1));
        case "Right":
            %xPos = getWord(%this.owner.getAreaMax(), 0);
            %yPos = getRandom(getWord(%this.owner.getAreaMin(), 1), getWord(%this.owner.getAreaMax(), 1));
    }

    %clone.position = %xPos SPC %yPos;

    %this.spawnCount++;

    if (%this.spawnCount < %this.count || %this.count == -1)
    {
        %minTime = (%this.spawnTime - %this.spawnVariance) * 1000;
        %maxTime = (%this.spawnTime + %this.spawnVariance) * 1000;
        %spawnTime = getRandom(%minTime, %maxTime);

        if( %spawnTime < 55 )
            %spawnTime = 55;

        %this.schedule(%spawnTime, "spawn");
    }
}
