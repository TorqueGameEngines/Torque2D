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

function ThemeManager::onAdd(%this)
{
	%this.themeList = new SimSet();

	%constructionVest = new ScriptObject()
	{
		class = "BaseTheme";
	};

	%labCoat = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "LabCoatTheme";
	};

	%forestRobe = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "ForestRobeTheme";
	};

	%torqueSuit = new ScriptObject()
	{
		superclass = "BaseTheme";
		class = "TorqueSuitTheme";
	};

	%this.registerTheme(%constructionVest);
	%this.registerTheme(%labCoat);
	%this.registerTheme(%forestRobe);
	%this.registerTheme(%torqueSuit);
	%this.setTheme(0);
}

function ThemeManager::setTheme(%this, %i)
{
	%i = mClamp(%i, 0, %this.themeList.getCount() - 1);
	%this.curTheme = %i;
	%theme = %this.themeList.getObject(%i);
	%this.activateTheme(%theme);
}

function ThemeManager::nextTheme(%this)
{
	%this.setTheme(%this.curTheme+1);
}

function ThemeManager::prevTheme(%this)
{
	%this.setTheme(%this.curTheme-1);
}

function ThemeManager::registerTheme(%this, %theme)
{
	%this.themeList.add(%theme);
}

function ThemeManager::activateTheme(%this, %theme)
{
	%this.panelProfile = %theme.panelProfile;
	%this.fullPanelProfile = %theme.fullPanelProfile;
	%this.overlayProfile = %theme.overlayProfile;
	%this.tipProfile = %theme.tipProfile;
	%this.buttonProfile = %theme.buttonProfile;
	%this.tabProfileLeft = %theme.tabProfileLeft;
	%this.tabProfileRight = %theme.tabProfileRight;
	%this.tabProfileTop = %theme.tabProfileTop;
	%this.tabProfileBottom = %theme.tabProfileBottom;
	%this.tabBookProfileLeft = %theme.tabBookProfileLeft;
	%this.tabBookProfileRight = %theme.tabBookProfileRight;
	%this.tabBookProfileTop = %theme.tabBookProfileTop;
	%this.tabBookProfileBottom = %theme.tabBookProfileBottom;
	%this.tabPageProfile = %theme.tabPageProfile;
	%this.textEditProfile = %theme.textEditProfile;
	%this.scrollProfile = %theme.scrollProfile;
	%this.thumbProfile = %theme.thumbProfile;
	%this.trackProfile = %theme.trackProfile;
	%this.scrollArrowProfile = %theme.scrollArrowProfile;
	%this.consoleProfile = %theme.consoleProfile;
	%this.scrollingPanelProfile = %theme.scrollingPanelProfile;
	%this.scrollingPanelThumbProfile = %theme.scrollingPanelThumbProfile;
	%this.scrollingPanelTrackProfile = %theme.scrollingPanelTrackProfile;
	%this.scrollingPanelArrowProfile = %theme.scrollingPanelArrowProfile;

	%this.postEvent("ThemeChanged", %this);
}
