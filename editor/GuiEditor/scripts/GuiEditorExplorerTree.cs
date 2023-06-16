
function GuiEditorExplorerTree::onAdd(%this)
{
    %this.endRadioSilence();
}

function GuiEditorExplorerTree::startRadioSilence(%this)
{
    %this.removeAllListeners();
}

function GuiEditorExplorerTree::endRadioSilence(%this)
{
    %this.addListener(GuiEditor.brain);
    %this.addListener(GuiEditor.inspectorWindow);
}

function GuiEditorExplorerTree::onSelect(%this, %index, %text, %item)
{
    if(%this.getSelCount() == 1)
    {
        %this.postEvent("Inspect", %item);
    }
    else 
    {
        %this.postEvent("AlsoInspect", %item);
    }
}

function GuiEditorExplorerTree::onUnselect(%this, %index, %text, %item)
{
    %this.postEvent("ClearInspect", %item);
}

function GuiEditorExplorerTree::onUnselectAll(%this)
{
    %this.postEvent("ClearInspectAll");
}

function GuiEditorExplorerTree::onDeleteKey(%this, %index, %text, %item)
{
	%this.postEvent("ObjectRemoved", %item);
}