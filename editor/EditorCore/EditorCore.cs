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

function EditorCore::create( %this )
{
	exec("./Themes/ThemeManager.cs");

	new ScriptObject(ThemeManager);

	%this.initGui();

	%this.editorKeyMap = new ActionMap();
    %this.editorKeyMap.bindCmd( "keyboard", "ctrl tilde", "EditorCore.toggleEditor();", "");
	%this.editorKeyMap.push();
}

//-----------------------------------------------------------------------------

function EditorCore::destroy( %this )
{
}

function EditorCore::initGui(%this)
{
	%this.baseGui = new GuiControl()
	{
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
	};
	ThemeManager.setProfile(%this.baseGui, "emptyProfile");

	%this.menuBar = new GuiMenuBarCtrl()
	{
		new GuiMenuItemCtrl() {
			Text = "Tools";

			new GuiMenuItemCtrl() {
				Text = "Close Tools";
				Command = "EditorCore.close();";
			};
		};
		new GuiMenuItemCtrl() {
			Text = "Theme";

			new GuiMenuItemCtrl() {
				Text = "Construction Vest";
				Radio = true;
				IsOn = true;
				Command = "ThemeManager.setTheme(0);";
				Accelerator = "Ctrl 1";
			};

			new GuiMenuItemCtrl() {
				Text = "Lab Coat";
				Radio = true;
				Command = "ThemeManager.setTheme(1);";
				Accelerator = "Ctrl 2";
			};

			new GuiMenuItemCtrl() {
				Text = "Forest Robe";
				Radio = true;
				Command = "ThemeManager.setTheme(2);";
				Accelerator = "Ctrl 3";
			};

			new GuiMenuItemCtrl() {
				Text = "Torque Suit";
				Radio = true;
				Command = "ThemeManager.setTheme(3);";
				Accelerator = "Ctrl 4";
			};
		};
	};
	ThemeManager.setProfile(%this.menuBar, "menuBarProfile");
	ThemeManager.setProfile(%this.menuBar, "menuProfile", "MenuProfile");
	ThemeManager.setProfile(%this.menuBar, "menuItemProfile", "MenuItemProfile");
	ThemeManager.setProfile(%this.menuBar, "menuContentProfile", "MenuContentProfile");
	ThemeManager.setProfile(%this.menuBar, "scrollingPanelThumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.menuBar, "scrollingPanelArrowProfile", "ArrowProfile");
	ThemeManager.setProfile(%this.menuBar, "scrollingPanelTrackProfile", "TrackProfile");

	%this.baseGui.add(%this.menuBar);

	%this.tabBook = new GuiTabBookCtrl()
	{
		Class = EditorCoreTabBook;
		HorizSizing = width;
		VertSizing = height;
		Position = "0 26";
		Extent = "1024 742";
		TabPosition = top;
		Core = %this;
	};
	ThemeManager.setProfile(%this.tabBook, "tabBookProfileTop");
	ThemeManager.setProfile(%this.tabBook, "tabProfileTop", "TabProfile");

	%this.baseGui.add(%this.tabBook);
}

function EditorCore::toggleEditor(%this)
{
    // Is the console awake?
    if ( %this.baseGui.isAwake() )
    {
        // Yes, so deactivate it.
        %this.close();
        return;
    }

    // Activate it.
    %this.open();
}

function EditorCore::open(%this)
{
	if ( $enableDirectInput )
        deactivateKeyboard();

    Canvas.pushDialog(%this.baseGui);

	%this.tabBook.selectPage(0);
}

function EditorCore::close(%this)
{
	if ( $enableDirectInput )
		activateKeyboard();
	Canvas.popDialog(%this.baseGui);
}

function EditorCore::RegisterEditor(%this, %name, %editor)
{
	%this.page[%name] = new GuiTabPageCtrl()
	{
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		Text = %name;
		Editor = %editor;
	};
	ThemeManager.setProfile(%this.page[%name], "tabPageProfile");

	return %this.page[%name];
}

function EditorCore::FinishRegistration(%this, %page)
{
	%this.tabBook.add(%page);
}

function EditorCoreTabBook::onTabSelected(%this, %tabText)
{
	if(isObject(%this.openEditor))
	{
		%this.openEditor.close();
	}
	%this.Core.page[%tabText].Editor.open();
	%this.openEditor = %this.Core.page[%tabText].Editor;
}
