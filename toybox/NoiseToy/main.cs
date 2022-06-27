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

function NoiseToy::create( %this )
{
    // Load scripts.
    exec( "./scripts/terrainMap.cs" );

    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation( pan );
    Sandbox.allowManipulation( pull );

    // Set the manipulation mode.
    Sandbox.useManipulation( pan );

    // Configure the toy.
    NoiseToy.Seed = getRandom(1, 100);
	NoiseToy.OctaveCount = 4;
	NoiseToy.Persistence = 0.5;

	SandboxScene.clear();

	//Create our noise generator
	NoiseToy.generator = new NoiseGenerator();

	//Create our map
	NoiseToy.terrain = new CompositeSprite()
	{
		class = "TerrainMap";
	};

    // Add the configuration options.
    addSelectionOption( "Earth,Moon,Mars,PlanetX", "Color Mode", 4, "setColorMode", false, "Changes the colors used for different values." );
    addNumericOption("Seed", 1, 999999, 1, "setNoiseSeed", NoiseToy.Seed, false, "A given seed always prouduces the same result." );
    addNumericOption("Octave Count", 1, 8, 1, "setOctaveCount", NoiseToy.OctaveCount, false, "Each octave produces a smaller layer of noise added to the result." );
    addNumericOption("Persistence", 0.05, 0.95, 0.05, "setPersistence", NoiseToy.persistence, false, "Higher values of persistence allow each octave to have a greater impact on the result." );

    // Reset the toy.
    %this.reset();
}

function NoiseToy::destroy( %this )
{
}

function NoiseToy::reset(%this)
{
	%this.generator.setSeed(%this.seed);

	%this.terrain.generate(%this.generator, %this.octaveCount, %this.persistence);
}

function NoiseToy::setColorMode(%this, %value)
{
	%this.terrain.colorMode = %value;

	%this.terrain.generate(%this.generator, %this.octaveCount, %this.persistence);
}

function NoiseToy::setNoiseSeed( %this, %value )
{
    NoiseToy.Seed = %value;
	%this.reset();
}

function NoiseToy::setOctaveCount( %this, %value )
{
    NoiseToy.OctaveCount = %value;

	%this.terrain.generate(%this.generator, %this.octaveCount, %this.persistence);
}

function NoiseToy::setPersistence( %this, %value )
{
    NoiseToy.Persistence = %value;

	%this.terrain.generate(%this.generator, %this.octaveCount, %this.persistence);
}
