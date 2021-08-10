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

function ShapeVectorToy::create( %this )
{
    // Set the sandbox drag mode availability.
    Sandbox.allowManipulation(pan);
    
    // Set the manipulation mode.
    Sandbox.useManipulation(pan);

    // Set the toy properties

    // Shape determines the number of vertices for the ShapeVector
    ShapeVectorToy.shape = 4;
    
    // Default shape is a polygon instead of a circle
    ShapeVectorToy.circle = false;

    // Toggles filling the shape with color or leaving as an outline
    ShapeVectorToy.fillMode = true;

    // Color of the filling
    ShapeVectorToy.fillColor = "0.5 1 1 0.5";

    // Color of the border lines
    ShapeVectorToy.lineColor = "0.5 1 1 1";

    // Add custom controls for toy
    addNumericOption("Vertices Count", 3, 20, 1, "setShape", ShapeVectorToy.shape, true, "Selects the shape to add to the scene based on the number of vertices");
    addFlagOption("Make a Circle", "setCircle", ShapeVectorToy.circle, true, "Override the number of vertices to make a circle");
    addFlagOption("Fill mode", "setFillMode", ShapeVectorToy.fillMode, true, "Whether new shapes are filled in or not");

    // Reset the toy.
    ShapeVectorToy.reset();
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::destroy( %this )
{
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::reset( %this )
{
    // Clear the scene.
    SandboxScene.clear();

    // Add a single shape
    %this.addShape();
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::setShape( %this, %value )
{
    %this.shape = %value;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::setCircle(%this, %value)
{
    %this.circle = %value;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::setFillMode( %this, %value)
{
    %this.fillMode = %value;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::generateShape( %this )
{
    // Create the shape vector
    %shape = new ShapeVector();
    %shape.setPosition("0 0");
    %shape.setSize(20);
    %shape.setLineColor(%this.lineColor);
    %shape.setFillColor(%this.fillColor);
    %shape.setFillMode(%this.fillMode);

    // Check if circle, if not make an equiangular convex polygon with n number of sides
    if (%this.circle)
    {
        %shape.setIsCircle(true);
        %shape.setCircleRadius(20);
    }else
    {
        %shape.setPolyPrimitive(%this.shape);
    }

    // Return the shape to be added to the scene
    return %shape;
}

//-----------------------------------------------------------------------------

function ShapeVectorToy::addShape( %this )
{
    // Create the shape.
    %object = %this.generateShape();

    // Add the sprite to the scene.
    SandboxScene.add( %object );    
}
