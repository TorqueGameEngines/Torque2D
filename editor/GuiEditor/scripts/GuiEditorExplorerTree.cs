
function GuiEditorExplorerTree::onAdd(%this)
{
    %this.addListener(GuiEditor.brain);
    %this.addListener(GuiEditor.inspectorWindow);
}

function GuiEditorExplorerTree::onClick(%this, %target)
{
    %this.postEvent("Inspect", %target);
}