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

function GuiEditor::create( %this )
{
	exec("./scripts/GuiEditorBrain.cs");
	exec("./scripts/GuiEditorControlList.cs");
	exec("./scripts/GuiEditorControlListBox.cs");

	%this.guiPage = EditorCore.RegisterEditor("Gui Editor", %this);

    %this.content = new GuiSpriteCtrl() {
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
		imageColor = "255 255 255 255";
        bitmap = "./images/gridTiny2";
		singleFrameBitmap = "1";
		tileImage = "1";
		positionOffset = "0 0";
		imageSize = "128 128";
		fullSize = "0";
		constrainProportions = "1";
        isContainer = true;
	};
    ThemeManager.setProfile(%this.content, "emptyProfile");
    %this.guiPage.add(%this.content);

    %this.brain = new GuiEditCtrl(GuiEditorBrain)
    {
        Profile = "GuiTextEditProfile";
        //Class = "GuiEditorBrain";
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
    };
    %this.guiPage.add(%this.brain);

    %this.ctrlListWindow = new GuiWindowCtrl()
    {
        Class = "GuiEditorControlList";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "0 0";
        Extent = "250 380";
        MinExtent = "100 100";
        text = "Control List";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = false;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.ctrlListWindow, "windowProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.guiPage.add(%this.ctrlListWindow);

    %this.onNewGui();

    EditorCore.FinishRegistration(%this.guiPage);
}

//-----------------------------------------------------------------------------

function GuiEditor::destroy( %this )
{
}

function GuiEditor::open(%this)
{
}

function GuiEditor::close(%this)
{
}

function GuiEditor::onNewGui(%this)
{
    %this.blankGui = new GuiControl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
        Profile = GuiDefaultProfile;
    };
    %this.content.add(%this.blankGui);
    %this.brain.setRoot(%this.blankGui);
    %this.brain.root = %this.blankGui;
}