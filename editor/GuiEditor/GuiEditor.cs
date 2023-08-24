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
    exec("./scripts/GuiEditorGridSizeDialog.cs");
    exec("./scripts/GuiEditorColorWindow.cs");

	%this.guiPage = EditorCore.RegisterEditor("Gui Editor", %this);

    %this.content = new GuiFrameSetCtrl() {
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.guiPage.getExtent();
        DividerThickness = 6;
	};
    ThemeManager.setProfile(%this.content, "frameSetProfile");
    ThemeManager.setProfile(%this.content, "dropButtonProfile", "dropButtonProfile");
    ThemeManager.setProfile(%this.content, "frameSetTabBookProfile", "tabBookProfile");
    ThemeManager.setProfile(%this.content, "frameSetTabProfile", "tabProfile");
    ThemeManager.setProfile(%this.content, "frameSetTabPageProfile", "tabPageProfile");
    %this.guiPage.add(%this.content);

    %idList = %this.content.createHorizontalSplit(1);
    %leftID = getWord(%idList, 0);
    %rightID = getWord(%idList, 1);
    %this.content.anchorFrame(%rightID);
    %this.content.setFrameSize(%rightID, 300);
    
    %ids = %this.content.createHorizontalSplit(%leftID);
    %inspectorFrameID = getWord(%ids, 0);
    %centerFrameID = getWord(%ids, 1);
    %this.content.setFrameSize(%inspectorFrameID, 360);
    
    %ids = %this.content.createVerticalSplit(%rightID);
    %toolFrameID = getWord(%ids, 0);
    %explorerFrameID = getWord(%ids, 1);
    %this.content.setFrameSize(%toolFrameID, 380);

    %this.brain = new GuiEditCtrl()
    {
        Class = "GuiEditorBrain";
		HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = "100 100";
    };
    ThemeManager.setProfile(%this.brain, "guiEditorProfile");

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
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.inspectorWindow, "windowProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.inspectorWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.content.add(%this.inspectorWindow);
    %this.inspectorWindow.startListening(%this.brain);

    %this.background = new GuiSpriteCtrl() {
		HorizSizing = "right";
        VertSizing = "bottom";
        Position = "0 0";
        Extent = "100 100";
		imageColor = "255 255 255 255";
        bitmap = "./images/gridTiny2";
		singleFrameBitmap = "1";
		tileImage = "1";
		positionOffset = "0 0";
		imageSize = "128 128";
		fullSize = "0";
		constrainProportions = "1";
	};
    ThemeManager.setProfile(%this.background, "emptyProfile");
    %this.content.add(%this.background);

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
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.ctrlListWindow, "windowProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.ctrlListWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.content.add(%this.ctrlListWindow);

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
    %this.content.add(%this.explorerWindow);
    %this.explorerWindow.startListening(%this.brain);

    %this.rootGui = new GuiControl()
    {
        HorizSizing = "width";
        VertSizing = "height";
        Position = "0 0";
        Extent = %this.background.getExtent();
        Profile = GuiDefaultProfile;
        class = "SimulatedCanvas";
    };
    %this.background.add(%this.rootGui);
    %this.brain.extent = %this.background.getExtent();
    %this.background.add(%this.brain);
    %this.fileName = "";
    %this.filePath = "";
    %this.formatIndex = 0;
    %this.folder = "";
    %this.module = "";
    %this.brain.setRoot(%this.rootGui);
    %this.brain.root = %this.rootGui;
    %this.explorerWindow.inspect(%this.rootGui);

    /* %this.colorWindow = new GuiWindowCtrl()
    {
        Class = "GuiEditorColorWindow";
        HorizSizing = "right";
        VertSizing = "bottom";
        Position = "610 0";
        Extent = "400  380";
        MinExtent = "100 100";
        text = "Color Test";
        canMove = true;
        canClose = false;
        canMinimize = true;
        canMaximize = false;
        resizeWidth = true;
        resizeHeight = true;
    };
    ThemeManager.setProfile(%this.colorWindow, "windowProfile");
    ThemeManager.setProfile(%this.colorWindow, "windowContentProfile", "ContentProfile");
    ThemeManager.setProfile(%this.colorWindow, "windowButtonProfile", "CloseButtonProfile");
    ThemeManager.setProfile(%this.colorWindow, "windowButtonProfile", "MinButtonProfile");
    ThemeManager.setProfile(%this.colorWindow, "windowButtonProfile", "MaxButtonProfile");
    %this.guiPage.add(%this.colorWindow); */

    EditorCore.FinishRegistration(%this.guiPage);
}

//-----------------------------------------------------------------------------

function GuiEditor::destroy( %this )
{

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
    %this.rootGui.clear();
    %this.fileName = "";
    %this.filePath = "";
    %this.formatIndex = 0;
    %this.folder = "";
    %this.module = "";
    %this.brain.clearSelection();
    %this.explorerWindow.tree.refresh();
}

function GuiEditor::OpenGui(%this)
{
    %path = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder());
	%dialog = new OpenFileDialog()
	{
		Filters = "ALL (*.GUI;*.GUI.DSO;*.GUI.TAML)|*.GUI;*.GUI.DSO;*.GUI.TAML|GUI (*.GUI;*.GUI.DSO)|*.GUI;*.GUI.DSO|TAML (*.GUI.TAML)|*.GUI.TAML";
		ChangePath = false;
		MultipleFiles = false;
		DefaultFile = "";
		defaultPath = %path;
		title = "Open Gui File";
	};
	%result = %dialog.execute();

	if ( %result )
	{
        if(fileExt(%dialog.fileName) $= ".taml")
        {
            %guiContent = TAMLRead(%dialog.fileName);
            %includesSimulatedCanvas = (%guiContent.class $= "SimulatedCanvas");
        }
        else 
        {
            exec(%dialog.fileName);
        }
        if(%includesSimulatedCanvas $= "")
        {
            %includesSimulatedCanvas = true;
        }
        if(isObject(%guiContent))
        {
            %this.fileName = fileName(%dialog.fileName);
            %this.filePath = %dialog.fileName;
            %this.formatIndex = 0;
            if(getSubStr(%dialog.fileName, strlen(%dialog.fileName) - 5, 5) $= ".taml")
            {
                %this.formatIndex = 1;
            }
            %this.folder = makeRelativePath(filePath(%dialog.fileName), getMainDotCsDir());
            %this.module = EditorCore.findModuleOfPath(%dialog.fileName);
            %this.DisplayGuiContent(%guiContent, %includesSimulatedCanvas);
        }
        else 
        {
            EditorCore.alert("Something went wrong while opening the Gui File. Gui Files should be structures with the root object assigned to %guiContent. If this file was made outside of the editor, you can change it manually and then open it in the Gui Editor.");
        }
    }
	// Cleanup
	%dialog.delete();
}

function GuiEditor::DisplayGuiContent(%this, %content, %includesSimulatedCanvas)
{
    %this.rootGui.deleteObjects();
    %this.brain.clearSelection();

    if(%includesSimulatedCanvas)
    {
        %count = %content.getCount();
        for(%i = 0; %i < %count; %i++)
        {
            %obj[%i] = %content.getObject(%i);
        }
        for(%i = 0; %i < %count; %i++)
        {
            %this.rootGui.add(%obj[%i]);
        }
        %content.delete();
        %this.explorerWindow.tree.refresh();
        %this.brain.onSelect(%this.rootGui.getObject(0));
    }
    else 
    {
        %this.rootGui.add(%content);
        %this.explorerWindow.tree.refresh();
        %this.brain.onSelect(%content);
    }
}

function GuiEditor::SaveGui(%this)
{
    if(%this.fileName $= "")
    {
        %this.SaveGuiAs();
    }
    else 
    {
        %this.SaveCore(%this.filePath, %this.formatIndex, %this.folder, %this.module);
    }
}

function GuiEditor::SaveGuiAs(%this)
{
    %width = 700;
	%height = 390;
	%dialog = new GuiControl()
	{
		class = "GuiEditorSaveGuiDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "Save Gui";
        defaultFileName = %this.fileName;
        formatIndex = %this.formatIndex;
        defaultFolder = %this.folder;
        defaultModule = %this.module;
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
}

function GuiEditor::SaveCore(%this, %filePath, %formatIndex, %folder, %module)
{
    if(%formatIndex == 0)
    {
        %fo = new FileObject();
        %fo.openForWrite(%filePath);
        %fo.writeLine("//--- Created with the GuiEditor ---//");
        if(%this.rootGui.getCount() == 1)
        {
            //Saved without the simulated canvas
            %fo.writeLine("%includesSimulatedCanvas = false;");
            %fo.writeObject(%this.rootGui.getObject(0), "%guiContent = ");
        }
        else 
        {
            //We have multiple top level objects so include the containing simulated canvas
            %fo.writeLine("%includesSimulatedCanvas = true;");
            %fo.writeObject(%this.rootGui, "%guiContent = ");
        }
        %fo.writeLine("//--- GuiEditor End ---//");
        %fo.close();
        %fo.delete();
    }
    else 
    {
        if(GuiEditor.rootGui.getCount() == 1)
        {
            //Saved without the Simulated Canvas
            TAMLWrite(%this.rootGui.getObject(0), %filePath);
        }
        else 
        {
            TAMLWrite(%this.rootGui, %filePath);
        }
    }
    %this.fileName = fileName(%filePath);
    %this.filePath = %filePath;
    %this.formatIndex = %formatIndex;
    %this.folder = %folder;
    %this.module = %module;
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
    if(%set.getCount() >= 1)
    {
        %obj = %set.getObject(0);
        %ext = %obj.getExtent();
        %obj.setExtent(getWord(%ext, 0) + %x, getWord(%ext, 1) + %y);
    }
}

function GuiEditor::SetGridSize(%this)
{
    %width = 300;
	%height = 140;
	%dialog = new GuiControl()
	{
		class = "GuiEditorGridSizeDialog";
		superclass = "EditorDialog";
		dialogSize = (%width + 8) SPC (%height + 8);
		dialogCanClose = true;
		dialogText = "Grid Size";
	};
	%dialog.init(%width, %height);

	Canvas.pushDialog(%dialog);
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
