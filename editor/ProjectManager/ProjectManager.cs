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

function ProjectManager::create(%this)
{
	exec("./scripts/ProjectModulePanel.cs");
	exec("./scripts/ProjectGamePanel.cs");
	exec("./scripts/ProjectLibraryPanel.cs");
	exec("./scripts/ProjectModuleCard.cs");
	exec("./scripts/ProjectModuleDependList.cs");
	exec("./scripts/NewDependencyDialog.cs");

	%this.guiPage = EditorCore.RegisterEditor("Project Manager", %this);

	%this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="1024 768";
		hScrollBar="dynamic";
		vScrollBar="dynamic";
		constantThumbHeight="0";
		showArrowButtons="1";
		scrollBarThickness="14";
	};
	ThemeManager.setProfile(%this.scroller, "emptyProfile");
	ThemeManager.setProfile(%this.scroller, "thumbProfile", "ThumbProfile");
	ThemeManager.setProfile(%this.scroller, "trackProfile", "TrackProfile");
	ThemeManager.setProfile(%this.scroller, "scrollArrowProfile", "ArrowProfile");
	%this.guiPage.add(%this.scroller);

	%this.container = new GuiControl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="1024 768";
		MinExtent="1024 500";
	};
	ThemeManager.setProfile(%this.container, "emptyProfile");
	%this.scroller.add(%this.container);

	%this.libraryPanel = new GuiControl()
	{
		Class = "ProjectLibraryPanel";
		superclass = "ProjectModulePanel";
		HorizSizing="relative";
		VertSizing="height";
		Position="0 0";
		Extent="512 768";
	};
	ThemeManager.setProfile(%this.libraryPanel, "emptyProfile");
	%this.container.add(%this.libraryPanel);
	%this.libraryPanel.load();
	%this.startListening(%this.libraryPanel);

	%this.gamePanel = new GuiControl()
	{
		Class = "ProjectGamePanel";
		superclass = "ProjectModulePanel";
		HorizSizing="relative";
		VertSizing="height";
		Position="512 0";
		Extent="512 768";
	};
	ThemeManager.setProfile(%this.gamePanel, "emptyProfile");
	%this.container.add(%this.gamePanel);
	%this.startListening(%this.gamePanel);

	EditorCore.FinishRegistration(%this.guiPage);
}

function ProjectManager::destroy(%this)
{
	%this.postEvent("ShutDown");
}

function ProjectManager::open(%this)
{
	%allModules = ModuleDatabase.findModules(false);
	%this.gamePanel.onOpen(%allModules);
	%this.libraryPanel.onOpen(%allModules);
}

function ProjectManager::close(%this)
{
	%this.libraryPanel.onClose();
	%this.gamePanel.onClose();
}

function ProjectManager::setProjectFolder(%this, %folder)
{
	%this.projectFolder = %folder;
}

function ProjectManager::getProjectFolder(%this)
{
	if(%this.projectFolder $= "")
	{
		%appCore = ModuleDatabase.findModule("AppCore", 1);
		%appCorePath = %appCore.getModulePath();
		%mainCsPath = getMainDotCsDir();
		%mainLen = strLen(%mainCsPath);
		%lastChar = getSubStr(%mainCsPath, %mainLen-1, 1);
		if(%lastChar $= "\\" || %lastChar $= "\/")
		{
			%relativePath = getSubStr(%appCorePath, strlen(%mainCsPath), strlen(%appCorePath) - %mainLen);
		}
		else
		{
			%relativePath = getSubStr(%appCorePath, strlen(%mainCsPath) + 1, strlen(%appCorePath) - (%mainLen + 1));
		}
		%unwantedPortion1 = strchr(%relativePath, "\\");
		%unwantedPortion2 = strchr(%relativePath, "\/");
		if(%unwantedPortion1 !$= "")
		{
			%this.projectFolder = getSubStr(%relativePath, 0, strlen(%relativePath) - strlen(%unwantedPortion1));
		}
		else if(%unwantedPortion2 !$= "")
		{
			%this.projectFolder = getSubStr(%relativePath, 0, strlen(%relativePath) - strlen(%unwantedPortion2));
		}
		else
		{
			%this.projectFolder = %relativePath;
		}
	}
	return pathConcat(getMainDotCsDir(), %this.projectFolder);
}

function ProjectManager::onModuleInstalled(%this, %module)
{
	%allModules = ModuleDatabase.findModules(false);
	%this.gamePanel.onOpen(%allModules);
	%this.libraryPanel.onOpen(%allModules);
}
