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

ConsoleMethodGroupBeginWithDocs(GuiParticleGraphInspector, GuiControl)

/*! Sets the Particle Asset that will be used to draw graphs.
	@param ParticleAsset The target of the graphs.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiParticleGraphInspector, inspect, ConsoleVoid, 3, 3, "(ParticleAsset)")
{
	ParticleAsset* target = dynamic_cast<ParticleAsset*>(Sim::findObject(argv[2]));
	if (!target)
	{
		if (dAtoi(argv[2]) > 0)
			Con::warnf("%s::inspect(): Object is not a ParticleAsset: %s", argv[0], argv[2]);

		return;
	}

	object->inspectObject(target);
}

/*! Sets the graph field to display.
	@param FieldName The name of the field that should be displayed.
	@param EmitterIndex If the field belongs to an emitter, include the index of the emitter to display.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiParticleGraphInspector, setDisplayField, ConsoleVoid, 3, 4, "(FieldName, [EmitterIndex])")
{
	if (argc > 3)
	{
		object->setDisplayField(argv[2], dAtoi(argv[3]));
	}
	else
	{
		object->setDisplayField(argv[2]);
	}
}

/*! Sets the area that will be displayed on the graph.
	@param Area Four space-deliminated values representing left, bottom, right, top.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiParticleGraphInspector, setDisplayArea, ConsoleVoid, 3, 3, "(area (xMin / yMin / xMax / yMax))")
{
	if (argc < 3)
	{
		Con::warnf("GuiParticleGraphInspector:setDisplayArea - Wrong number of arguments. Should be area(left / bottom / right / top).");
		return;
	}
	
	U32 count = Utility::mGetStringElementCount(argv[2]);
	if (count != 4)
	{
		Con::warnf("GuiParticleGraphInspector:setDisplayArea - Area does not have four values. Should be area(left / bottom / right / top).");
		return;
	}

	StringTableEntry s1 = StringTable->insert(Utility::mGetStringElement(argv[2], 0));
	StringTableEntry s2 = StringTable->insert(Utility::mGetStringElement(argv[2], 1));
	StringTableEntry s3 = StringTable->insert(Utility::mGetStringElement(argv[2], 2));
	StringTableEntry s4 = StringTable->insert(Utility::mGetStringElement(argv[2], 3));

	object->setDisplayArea(s1, s2, s3, s4);
}

/*! Sets the labels to display on the graph.
	@param LabelX The label that appears on the bottom of the graph.
	@param LabelY The label that appears on the left of the graph.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiParticleGraphInspector, setDisplayLabels, ConsoleVoid, 4, 4, "(LabelX, LabelY)")
{
	if (argc < 4)
	{
		Con::warnf("GuiParticleGraphInspector:setDisplayLabels - Wrong number of arguments. Should be LabelX and LabelY.");
		return;
	}

	object->setDisplayLabels(argv[2], argv[3]);
}

ConsoleMethodGroupEndWithDocs(GuiParticleGraphInspector)