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

function TerrainMap::onAdd(%this)
{
	%tileSize = 1.4;
	%this.mapSize = 75;
	%center = (%tileSize / 2) - ((%this.mapSize * %tileSize) / 2);
	%this.BatchCulling = true;
	%this.BatchLayout = "rect";
	%this.BatchIsolated = false;
	%this.BatchSortMode = "Batch";
	%this.DefaultSpriteSize = %tileSize;
	%this.DefaultSpriteStride = %tileSize;
	%this.Position = %center SPC %center;
	%this.BodyType = static;
	%this.Enabled = true;

	//Create the tile
	for(%y = 0; %y < %this.mapSize; %y++)
	{
		for(%x = 0; %x < %this.mapSize; %x++)
		{
			%this.addSprite(%x SPC %y);
			%this.setSpriteImage("ToyAssets:Blank");
		}
	}

	SandboxScene.add(%this);
	%this.colorMode = "Earth";
}

function TerrainMap::generate(%this, %generator, %octaveCount, %persistence)
{
	for(%y = 0; %y < %this.mapSize; %y++)
	{
		for(%x = 0; %x < %this.mapSize; %x++)
		{
			%this.selectSprite(%x SPC %y);
			%this.setSpriteBlendColor(%this.getColor(%x, %y, %generator, %octaveCount, %persistence));
		}
	}
}

function TerrainMap::getColor(%this, %x, %y, %generator, %octaveCount, %persistence)
{
	%zoomFactor = 0.1;
	%value = %generator.getComplexNoise(%x * %zoomFactor, %y * %zoomFactor, %octaveCount, %persistence);

	return %this.call("get" @ %this.colorMode @ "Color", %value);
}

function TerrainMap::getMoonColor(%this, %value)
{
	return %value SPC %value SPC %value SPC "1";
}

function TerrainMap::getEarthColor(%this, %value)
{
	if(%value > 0.7)
	{
		return (%value + 0.2) SPC (%value + 0.2) SPC (%value + 0.2) SPC "1";
	}
	else if(%value > 0.6)
	{
		%value = 0.4 + (%value / 10);
		return %value SPC %value SPC (%value + 0.05) SPC "1";
	}
	else if(%value > 0.5)
	{
		return (%value / 5) SPC (%value * 1.2) SPC (%value / 5) SPC "1";
	}
	else if(%value > 0.48)
	{
		return "1" SPC "0.9" SPC %value SPC "1";
	}
	else
	{
		%factor = %value / 0.48;
		%red = 0.1 * %factor;
		%green = 0.1 + (0.2 * %factor);
		%blue = 0.4 + (0.5 * %factor);
		return %red SPC %green SPC %blue SPC "1";
	}
}

function TerrainMap::getMarsColor(%this, %value)
{
	return %value SPC (%value / 4) SPC "0" SPC "1";
}

function TerrainMap::getPlanetXColor(%this, %value)
{
	if(%value > 0.45)
	{
		%factor = ((%value - 0.45) / 0.55);
		%red = 0.15 + (0.29 * %factor);
		%green = 0.1 - (0.05 * (1 - %factor));
		%blue = 0.23 + (0.45 * %factor);
		return %red SPC %green SPC %blue SPC "1";
	}
	else
	{
		%factor = %value / 0.45;
		%red = 0.1 * %factor;
		%green = 0.2 + (0.3 * %factor);
		%blue = 0.05 + (0.1 * %factor);
		return %red SPC %green SPC %blue SPC "1";
	}
}
