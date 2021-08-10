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
	exec("./assets/gui/LBEditorGui.gui");
	EditorToy.blankScene = new Scene();
	
	// Init menu
	EditorToy.initMenus();
	
    // Reset the toy initially.
    EditorToy.reset();
	
}

function EditorToy::initMenus( %this )
{
   if( isObject( %this.menuGroup ) )
      %this.menuGroup.delete();
      
   %this.menuGroup = new SimGroup();
   
   //set up %cmdctrl variable so that it matches OS standards
   %cmdCtrl = $platform $= "macos" ? "Cmd" : "Ctrl";

   %filemenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 0;
      barName = "File";
      
      item[0] = "New Scene..." TAB %cmdCtrl SPC "N" TAB "";
   };

   %layoutmenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 1;
      barName = "Layout";
      
      item[0] = "Align Left" TAB %cmdCtrl SPC "L" TAB "";
   };
   
   %movemenu = new PopupMenu()
   {
      superClass = "MenuBuilder";
      barPosition = 2;
      barName = "Move";
      
      item[0] = "Nudge Up" TAB "Up" TAB "";
   };
   
   // add menus to a group
   %this.menuGroup.add(%fileMenu);
   %this.menuGroup.add(%layoutmenu);
   %this.menuGroup.add(%movemenu);
   
   for( %i = 0; %i < %this.menuGroup.getCount(); %i++ )
     %this.menuGroup.getObject( %i ).attachToMenuBar();
   
}

//-----------------------------------------------------------------------------

function EditorToy::destroy( %this )
{   
}

function EditorToy::reset(%this)
{
	Canvas.setContent(LBEditorGui);
	LBEditorGuiSceneWindow.setEditScene(EditorToy.blankScene);
	
	%tireRear = new Sprite(Tyre1);
    %tireRear.setPosition( -2, 0 );
    %tireRear.setImage( "ToyAssets:tires" );
    %tireRear.setSize( 1, 1 );
	EditorToy.blankScene.add(%tireRear);
	%tireRear = new Sprite(Tyre2);
    %tireRear.setPosition( 2, 0 );
    %tireRear.setImage( "ToyAssets:tires" );
    %tireRear.setSize( 1, 1 );
	EditorToy.blankScene.add(%tireRear);
	
	LBEditorGuiSceneWindow.setCameraSize(16.0 , 9.0);
	
}

