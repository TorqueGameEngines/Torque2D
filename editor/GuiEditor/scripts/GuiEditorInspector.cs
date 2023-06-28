function GuiEditorInspector::onPostApply(%this, %obj)
{
    %this.postEvent("PostApply", %obj);
}