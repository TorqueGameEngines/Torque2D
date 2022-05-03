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
	%this.editorKeyMap = new ActionMap();
	if(!isObject(AppCore))
	{
		exec("./scripts/constants.cs");
		exec("./scripts/defaultPreferences.cs");
		exec("./gui/guiProfiles.cs");
		%this.createGuiProfiles();

		exec("./scripts/canvas.cs");
		exec("./scripts/EditorCoreSplash.cs");
		exec("./scripts/EditorProjectSelector.cs");
		exec("./scripts/EditorProjectCard.cs");
		exec("./scripts/NewProjectDialog.cs");

		%this.initializeCanvas("Torque2D: Rocket Edition");
	}
	else
	{
		%this.editorKeyMap.bindCmd( "keyboard", "ctrl tilde", "EditorCore.toggleEditor();", "");
	}
	exec("./Themes/ThemeManager.cs");
	exec("./EditorDialog.cs");
	exec("./EditorForm.cs");
	exec("./EditorIconButton.cs");
	exec("./EditorButtonBar.cs");

	new ScriptObject(ThemeManager);

	%this.initGui();
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
			Text = "Torque2D";

			new GuiMenuItemCtrl() {
				Text = "Close Tools";
				Command = "EditorCore.close();";
			};

			new GuiMenuItemCtrl() {
				Text = "Close Project";
				Command = "restartInstance();";
			};

			new GuiMenuItemCtrl() {
				Text = "Exit";
				Command = "quit();";
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

	%this.torqueBackground = new GuiSpriteCtrl()
	{
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		SingleFrameBitmap = "1";
		ConstrainProportions = "0";
		FullSize = "1";
		ImageColor = "255 255 255 255";
		PositionOffset = "0 0";
		Visible = false;
	};
	ThemeManager.setProfile(%this.torqueBackground, "spriteProfile");
	ThemeManager.setImage(%this.torqueBackground, "editorBG");
	%this.baseGui.add(%this.torqueBackground);

	%this.projectSelector = new GuiControl()
	{
		Class = EditorProjectSelector;
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		Visible = false;
	};
	ThemeManager.setProfile(%this.projectSelector, "emptyProfile");
	%this.baseGui.add(%this.projectSelector);

	%this.splash = new GuiSpriteCtrl()
	{
		Class = EditorCoreSplash;
		HorizSizing = width;
		VertSizing = height;
		Position = "0 0";
		Extent = "1024 768";
		Bitmap = "./images/t2d_rocket_splash.png";
		SingleFrameBitmap = "1";
		constrainProportions = "1";
		FullSize = "0";
		ImageSize = "672 480";
		ImageColor = "255 255 255 0";
		PositionOffset = "0 20";
		Visible = false;
	};
	ThemeManager.setProfile(%this.splash, "spriteProfile");
	%this.baseGui.add(%this.splash);
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
	Canvas.setCursor(ThemeManager.activeTheme.defaultCursor);
}

function EditorCore::close(%this)
{
	if ( $enableDirectInput )
		activateKeyboard();
	Canvas.popDialog(%this.baseGui);

	if(isObject(defaultCursor))
	{
		Canvas.setCursor(defaultCursor);
	}
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

function EditorCore::findModuleOfPath(%this, %pathWithFile)
{
	%pathWithFile = makeFullPath(%pathWithFile);
	%loadedModules = ModuleDatabase.findModules(false);

	%chosenModSig = "";
	for(%i = 0; %i < getWordCount(%loadedModules); %i++)
	{
		%mod = getWord(%loadedModules, %i);
		%modPath = %this.flattenPath(%mod.ModulePath);

		//Now compare the two paths to see if they go together
		if(strPos(%pathWithFile, %modPath) != -1)
		{
			%chosenModSig = %mod.Signature;
			break;
		}
	}

	return %chosenModSig;
}

function EditorCore::flattenPath(%this, %path)
{
	%pen = 0;
	for(%i = 0; %i < getUnitCount(%path, "/"); %i++)
	{
		%token = getUnit(%path, %i, "/");

		if(%token $= "..")
		{
			%pen = mGetMax(%pen - 1, 0);
		}
		else
		{
			%token[%pen] = %token;
			%pen++;
		}
	}

	%result = %token[0];
	for(%i = 1; %i <= %pen; %i++)
	{
		%result = %result @ "/" @ %token[%i];
	}

	return %result;
}

function EditorCore::showProjectSelector(%this)
{
	%this.menuBar.setVisible(false);
	%this.tabBook.setVisible(false);
	%this.projectSelector.setVisible(true);
	%this.splash.show();
	%this.torqueBackground.setVisible(true);
	%this.projectSelector.schedule(2800, "show");
	%this.startListening(%this.projectSelector);
}

function EditorCore::onProjectSelected(%this)
{
	%this.close();
	%this.menuBar.setVisible(true);
	%this.tabBook.setVisible(true);
	%this.projectSelector.setVisible(false);
	%this.torqueBackground.setVisible(false);
	%this.stopListening(%this.projectSelector);
	%this.schedule(500, "finishProjectSelection");
}

function EditorCore::finishProjectSelection(%this)
{
	ModuleDatabase.LoadExplicit("AppCore", 1);
	%this.editorKeyMap.bindCmd( "keyboard", "ctrl tilde", "EditorCore.toggleEditor();", "");
}
