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
	exec("./scripts/GuiEditorControlListWindow.cs");
	exec("./scripts/GuiEditorControlListBox.cs");
	exec("./scripts/GuiEditorInspectorWindow.cs");
	exec("./scripts/GuiEditorExplorerWindow.cs");
    exec("./scripts/GuiEditorExplorerTree.cs");

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

    %this.brain = new GuiEditCtrl()
    {
        Profile = "GuiTextEditProfile";
        Class = "GuiEditorBrain";
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
    };
    %this.guiPage.add(%this.brain);

    %this.ctrlListWindow = new GuiWindowCtrl()
    {
        Class = "GuiEditorControlListWindow";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "360 0";
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

    %this.inspectorWindow = new GuiWindowCtrl()
    {
        Class = "GuiEditorInspectorWindow";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "0 0";
        Extent = "360 380";
        MinExtent = "100 100";
        text = "Gui Inspector";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = false;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.inspectorWindow, "windowProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.guiPage.add(%this.inspectorWindow);
    %this.inspectorWindow.startListening(%this.brain);

    %this.explorerWindow = new GuiWindowCtrl()
    {
        Class = "GuiEditorExplorerWindow";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "610 0";
        Extent = "400  380";
        MinExtent = "100 100";
        text = "Explorer";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.explorerWindow, "windowProfile");
    ThemeManager.setProfile(%this.explorerWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.explorerWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.explorerWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.explorerWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.guiPage.add(%this.explorerWindow);
    %this.explorerWindow.startListening(%this.brain);

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
    %this.explorerWindow.open(%this.blankGui);
}

//MENU FUNCTIONS---------------------------------------------------------------
function GuiEditor::NewGui(%this)
{

}

function GuiEditor::OpenGui(%this)
{
    
}

function GuiEditor::SaveGui(%this)
{
    
}

function GuiEditor::Undo(%this)
{
    %undoManager = %this.brain.getUndoManager();
    %undoManager.undo();
}

function GuiEditor::Redo(%this)
{
    %undoManager = %this.brain.getUndoManager();
    %undoManager.redo();

    %count = %undoManager.getRedoCount();

}

function GuiEditor::Cut(%this)
{
    
}

function GuiEditor::Copy(%this)
{
    
}

function GuiEditor::Paste(%this)
{
    
}

function GuiEditor::changeExtent(%this, %x, %y)
{
    %set = %this.brain.getSelected();
    if(%set.getCount() == 1)
    {
        %obj = %set.getObject(0);
        %ext = %obj.getExtent();
        %obj.setExtent(getWord(%ext, 0) + %x, getWord(%ext, 1) + %y);
    }
}

function GuiEditor::SetGridSize(%this)
{
    
}

function GuiEditor::SnapToGrid(%this, %gridOn)
{
    if(%gridOn)
    {
        %this.brain.setSnapToGrid(10);
    }
    else 
    {
        %this.brain.setSnapToGrid(0);
    }
}

//METHODS-----------------------------------------------------------------
