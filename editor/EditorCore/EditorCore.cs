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
	exec("./Themes/BaseTheme.cs");
	exec("./Themes/LabCoatTheme.cs");
	exec("./Themes/ForestRobeTheme.cs");
	exec("./Themes/TorqueSuitTheme.cs");

	%this.themes = new ScriptObject()
	{
		class = "ThemeManager";
	};
	%this.startListening(%this.themes);

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
	%this.tabBook = new GuiTabBookCtrl()
	{
		Profile = %this.themes.tabBookProfileTop;
		TabProfile = %this.themes.tabProfileTop;
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		TabPosition = top;
	};
}

function EditorCore::toggleEditor(%this)
{
    // Is the console awake?
    if ( %this.tabBook.isAwake() )
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

    Canvas.pushDialog(%this.tabBook);

	%this.tabBook.selectPage(0);
	EditorConsole.open();
}

function EditorCore::close(%this)
{
	if ( $enableDirectInput )
		activateKeyboard();
	Canvas.popDialog(%this.tabBook);
}

function EditorCore::RegisterEditor(%this, %name, %editor)
{
	%this.page[%name] = new GuiTabPageCtrl()
	{
		Profile = %this.themes.tabPageProfile;
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		Text = %name;
	};

	%editor.startListening(%this.themes);

	return %this.page[%name];
}

function EditorCore::FinishRegistration(%this, %page)
{
	%this.tabBook.add(%page);
}

function EditorCore::onThemeChanged(%this, %theme)
{
	%this.tabBook.setProfile(%theme.tabBookProfileTop);
	%this.tabBook.setTabProfile(%theme.tabProfileTop);

	for(%i = 0; %i < %this.tabBook.getCount(); %i++)
	{
		%page = %this.tabBook.getObject(%i);
		%page.setProfile(%theme.tabPageProfile);
	}
}

function EditorCore::setTheme(%this, %i)
{
	%this.themes.setTheme(%i);
}

function EditorCore::nextTheme(%this)
{
	%this.themes.nextTheme();
}

function EditorCore::prevTheme(%this)
{
	%this.themes.prevTheme();
}
