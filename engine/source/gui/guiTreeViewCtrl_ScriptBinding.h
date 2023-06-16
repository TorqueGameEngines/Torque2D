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

ConsoleMethodGroupBeginWithDocs(GuiTreeViewCtrl, GuiListBoxCtrl)

/*! Sets the root object to view.
	@param obj A SimGroup that will be viewed as a tree.
	@return No return value.
*/
ConsoleMethodWithDocs( GuiTreeViewCtrl, open, ConsoleVoid, 3, 3, (SimGroup obj))
{
   SimGroup* treeRoot = NULL;
   SimObject* target = Sim::findObject(argv[2]);

   if (target)
      treeRoot = dynamic_cast<SimGroup*>(target);

   object->inspectObject(treeRoot);
}

/*! Informs the tree that the contents of the tree have changed and should be refreshed but preserves selection if possible.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTreeViewCtrl, refresh, ConsoleVoid, 2, 2, ())
{
	object->refreshTree();
}

/*! Sets if an item is open by the index.
	@param index The zero-based index of the item.
	@param isOpen True if the item should be open and false otherwise.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiTreeViewCtrl, setItemOpen, ConsoleVoid, 4, 4, "(int index, bool isOpen)")
{
	if (argc != 4)
	{
		Con::warnf("GuiTreeViewCtrl::setItemOpen() - Invalid number of parameters! Should be (index, isOpen).");
	}
	else
	{
		object->setItemOpen(dAtoi(argv[2]), dAtob(argv[3]));
	}
}

/*! Returns if an item is open or not using an index.
	@param index The zero-based index of the item.
	@return True if the item is open and false otherwise.
*/
ConsoleMethodWithDocs(GuiTreeViewCtrl, getItemOpen, ConsoleBool, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiTreeViewCtrl::getItemOpen() - Invalid number of parameters! Should be (index).");
		return true;
	}
	return object->getItemOpen(dAtoi(argv[2]));
}

/*! Returns the index of the parent of the item.
	@param index The zero-based index of the child item.
	@return The index of the parent or -1 if the parent is the root.
*/
ConsoleMethodWithDocs(GuiTreeViewCtrl, getItemParent, ConsoleInt, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiTreeViewCtrl::getItemOpen() - Invalid number of parameters! Should be (index).");
		return -1;
	}
	return object->getItemTrunk(dAtoi(argv[2]));
}

ConsoleMethodGroupEndWithDocs(GuiTreeViewCtrl)