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

function EditorConsole::create(%this)
{
	%this.guiPage = EditorCore.RegisterEditor("Console", %this);

	%this.consoleEntry = new GuiConsoleEditCtrl()
	{
		HorizSizing="width";
		VertSizing="top";
		Position="0 738";
		Extent="924 30";
		minExtent="120 20";
		AltCommand="EditorConsole.eval();";
		MaxLength="255";
		active = "1";
	};
	ThemeManager.setProfile(%this.consoleEntry, "textEditProfile");
	%this.guiPage.add(%this.consoleEntry);

	%this.hideLogButton = new GuiButtonCtrl()
	{
		Text="Close";
		command="EditorCore.close();";
		HorizSizing="left";
		VertSizing="top";
		Position="924 738";
		Extent="100 30";
		MinExtent="80 20";
	};
	ThemeManager.setProfile(%this.hideLogButton, "buttonProfile");
	%this.guiPage.add(%this.hideLogButton);

	%this.scroller = new GuiScrollCtrl()
	{
		HorizSizing="width";
		VertSizing="height";
		Position="0 0";
		Extent="1024 738";
		MinExtent="220 200";
		hScrollBar="alwaysOn";
		vScrollBar="alwaysOn";
		constantThumbHeight="0";
		showArrowButtons="1";
	};
	ThemeManager.setProfile(%this.scroller, "scrollProfile");
	ThemeManager.setProfile(%this.scroller, "thumbProfile", ThumbProfile);
	ThemeManager.setProfile(%this.scroller, "trackProfile", TrackProfile);
	ThemeManager.setProfile(%this.scroller, "scrollArrowProfile", ArrowProfile);
	%this.guiPage.add(%this.scroller);

	%this.consoleLog = new GuiConsole()
	{
		Position="0 0";
		Extent="1024 738";
		HorizSizing="width";
		VertSizing="height";
		Visible="1";
	};
	ThemeManager.setProfile(%this.consoleLog, "consoleProfile");
	%this.scroller.add(%this.consoleLog);

	EditorCore.FinishRegistration(%this.guiPage);
}

function EditorConsole::destroy(%this)
{

}

function EditorConsole::open(%this)
{
	%this.scroller.scrollToBottom();
	%this.consoleEntry.setFirstResponder();
}

function EditorConsole::close(%this)
{

}

function EditorConsole::eval(%this)
{
	%text = trim(%this.consoleEntry.getValue());

    if(strpos(%text, "(") == -1 && strpos(%text, "=") == -1 && strpos(%text, " ") == -1 && strpos(%text, "{") == -1 && strpos(%text, "}") == -1)
    {
        %text = %text @ "()";
    }

    %pos = strlen(%text) - 1;

    if(strpos(%text, ";", %pos) == -1 && strpos(%text, "}") == -1)
        %text = %text @ ";";

    echo("==>" @ %text);
    eval(%text);
    %this.consoleEntry.setValue("");
}
