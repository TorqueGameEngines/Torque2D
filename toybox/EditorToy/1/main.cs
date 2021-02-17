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

function EditorToy::create( %this )
{
	%editorSceneWin = new EditorToySceneWindow(EditorSceneWindow);
	
	Canvas.setContent(%editorSceneWin);
    EditorSceneWindow.setCameraPosition( 0, 0 );
    EditorSceneWindow.setCameraSize( 16, 9 );
    EditorSceneWindow.setCameraZoom( 1 );
    EditorSceneWindow.setCameraAngle( 0 );
	
	%editorScene = new Scene(EditorScene);
	
	EditorSceneWindow.setScene(%editorScene);
	if(!EditorSceneWindow.getActiveSelection())
		EditorSceneWindow.setActiveSelection(new EditorToySelection(EEditorSelection));
	
	%this.reset();
}

//-----------------------------------------------------------------------------

function EditorToy::destroy( %this )
{
    
}


//-----------------------------------------------------------------------------

function EditorToy::reset(%this)
{
	
	%obj = new Sprite();
	%obj.setImage("ToyAssets:Blocks");
	%obj.setAngle("10");
	EditorScene.add(%obj);
	
	%obj = new Sprite();
	%obj.setImage("ToyAssets:Blocks");
	%obj.setPosition("5 0");
	EditorScene.add(%obj);
	
}