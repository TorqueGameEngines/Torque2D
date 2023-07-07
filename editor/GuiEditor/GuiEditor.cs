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
    exec("./scripts/GuiEditorInspector.cs");
	exec("./scripts/GuiEditorExplorerWindow.cs");
    exec("./scripts/GuiEditorExplorerTree.cs");
    exec("./scripts/GuiEditorSaveGuiDialog.cs");

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
        Class = "GuiEditorBrain";
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
    };
    ThemeManager.setProfile(%this.brain, "guiEditorProfile");
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

    %this.NewGui();

    EditorCore.FinishRegistration(%this.guiPage);
}

//-----------------------------------------------------------------------------

function GuiEditor::destroy( %this )
{
    if(isObject(%this.rootGui))
    {
        %this.content.removeIfMember(%this.rootGui);
    }
}

function GuiEditor::open(%this, %content)
{
    EditorCore.menuBar.setMenuActive("File", true);
    EditorCore.menuBar.setMenuActive("Edit", true);
    EditorCore.menuBar.setMenuActive("Layout", true);
    EditorCore.menuBar.setMenuActive("Select", true);
}

function GuiEditor::close(%this)
{
    EditorCore.menuBar.setMenuActive("File", false);
    EditorCore.menuBar.setMenuActive("Edit", false);
    EditorCore.menuBar.setMenuActive("Layout", false);
    EditorCore.menuBar.setMenuActive("Select", false);
}

//MENU FUNCTIONS---------------------------------------------------------------
function GuiEditor::NewGui(%this)
{
    %content = new GuiControl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
        Profile = GuiDefaultProfile;
    };
    %this.DisplayGuiContent(%content);
}

function GuiEditor::OpenGui(%this)
{
    %path = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder());
	%dialog = new OpenFileDialog()
	{
		Filters = "GUI (*.GUI;*.GUI.DSO)|*.GUI;*.GUI.DSO";
		ChangePath = false;
		MultipleFiles = false;
		DefaultFile = "";
		defaultPath = %path;
		title = "Open Gui File";
	};
	%result = %dialog.execute();

	if ( %result )
	{
        exec(%dialog.fileName);
        if(isObject(%guiContent))
        {
            %this.DisplayGuiContent(%guiContent, %dialog.fileName);
        }
        else 
        {
            EditorCore.alert("Something went wrong while opening the Gui File. Gui Files should be structures with the root object assigned to %guiContent. If this file was made outside of the editor, you can change it manually and then open it in the Gui Editor.");
        }
	}
	// Cleanup
	%dialog.delete();
}

function GuiEditor::DisplayGuiContent(%this, %content, %filePath)
{
    if(isObject(%this.rootGui))
    {
        %this.filePath = "";
        %this.content.removeIfMember(%this.rootGui);
        %this.explorerWindow.tree.uninspect();
        %this.prevGui = %this.rootGui;
    }

    %this.filePath = %filePath;
    %this.rootGui = %content;
    %this.content.add(%content);
    %this.brain.setRoot(%content);
    %this.brain.root = %content;
    %this.explorerWindow.inspect(%content);

    %this.brain.onSelect(%content);
}

function GuiEditor::SaveGui(%this)
{
    %width = 700;
	%height = 340;
	%dialog = new GuiControl()
	{
		class = "GuiEditorSaveGuiDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "Save Gui";
	};
	%dialog.init(%width, %height);
	%this.startListening(%dialog);

	Canvas.pushDialog(%dialog);
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
