
function DeclaredAssetForm::getFolderPath(%this, %title, %textEdit)
{
	%modulePath = pathConcat(getMainDotCsDir(), ProjectManager.getProjectFolder(), %this.module.moduleID);
	if(isDirectory(pathConcat(%modulePath, %this.module.versionID)))
	{
		%modulePath = pathConcat(%modulePath, %this.module.versionID);
	}
	%dialog = new OpenFolderDialog()
	{
		Filters = "All Files|*.*";
		ChangePath = false;
		DefaultFile = "";
		defaultPath = %modulePath;
		title = %title;
	};
	%result = %dialog.execute();

	if ( %result )
	{
		%selectedFile = makeRelativePath(%dialog.fileName, %modulePath);
		%textEdit.setText(%selectedFile);
		%textEdit.setCursorPos(999999);//move it to the far right.
	}
	// Cleanup
	%dialog.delete();

	%this.postEvent("FolderOpened", %textEdit);
}
