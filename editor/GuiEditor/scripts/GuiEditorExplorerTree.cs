
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
        %this.postEvent("ClearInspectAll");
        %this.postEvent("AlsoInspect", %item);
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

function GuiEditorExplorerTree::onPostApply(%this, %obj)
{
    %index = %this.findItemID(%obj.getId());
    if(%index > -1)
    {
        %this.refreshItemText(%index);
    }
}

function GuiEditorExplorerTree::onGetObjectText(%this, %obj)
{
    if(%obj == GuiEditor.rootGui)
    {
        return "Canvas Simulation";
    }
    return "";
}