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

ConsoleMethodGroupBeginWithDocs(GuiListBoxCtrl, GuiControl)

/*! If true, allows the user to select multiple items.
	@param allowMultiSelection True to allow multi-selection. False to allow single selection.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setMultiSelection, ConsoleVoid, 3, 3, "(bool allowMultiSelection)")
{
	object->setMultipleSelection(dAtob(argv[2]));
}

/*! Reports if multi-selection is enabled.
	@return Returns true if multi-selection is enabled. False otherwise.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getMultiSelection, ConsoleVoid, 2, 2, "()")
{
	object->getMultipleSelection();
}

/*! Removes all items from the listbox.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, clearItems, ConsoleVoid, 2, 2, "()")
{
	object->clearItems();
}

/*! Unselects any items that have been selected.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, clearSelection, ConsoleVoid, 2, 2, "()")
{
	object->clearSelection();
}

/*! Sets an item to selected or unselected using an index.
	@param index The zero-based index of the item that should be selected.
	@param isSelected If true, the item is selected. If false, it is unselected. If omitted, will default to true.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setSelected, ConsoleVoid, 3, 4, "(int index, [bool isSelected])")
{
	bool value = true;
	if (argc == 4)
		value = dAtob(argv[3]);

	if (value == true)
		object->addSelection(dAtoi(argv[2]));
	else
		object->removeSelection(dAtoi(argv[2]));
}

/*! Sets an item's ID using an index.
	@param index The zero-based index of the item that should have the ID.
	@param ID An interger value that numerically represents this item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setItemID, ConsoleVoid, 4, 4, "(int index, int ID)")
{
	if(argc != 4)
	{
		Con::warnf("GuiListBoxCtrl::setItemID() - Invalid number of parameters! Should be (index, ID).");
	}
	else
	{
		object->setItemID(dAtoi(argv[2]), dAtoi(argv[3]));
	}
}

/*! Returns an item's ID using an index.
	@param index The zero-based index of the item with the needed ID.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getItemID, ConsoleInt, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiListBoxCtrl::getItemID() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		return object->getItemID(dAtoi(argv[2]));
	}
	return 0;
}

/*! Finds the first item with the given ID.
	@param ID The item ID to search for.
	@return The first matching item index or -1 if no item is found.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, findItemID, ConsoleInt, 3, 3, "(int ID)")
{
	if (argc != 3)
	{
		Con::warnf("GuiListBoxCtrl::findItemID() - Invalid number of parameters! Should be (ID).");
		return -1;
	}
	
	return object->findItemID(dAtoi(argv[2]));
}

/*! Sets an item active using an index.
	@param index The zero-based index of the item that should be active.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setItemActive, ConsoleVoid, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiListBoxCtrl::setItemActive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		object->setItemActive(dAtoi(argv[2]));
	}
}

/*! Disables an item using an index.
	@param index The zero-based index of the item that should be inactive.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setItemInactive, ConsoleVoid, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiListBoxCtrl::setItemInactive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		object->setItemInactive(dAtoi(argv[2]));
	}
}

/*! Returns if an item is active or not using an index.
	@param index The zero-based index of the item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getItemActive, ConsoleBool, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiListBoxCtrl::getItemActive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		return object->getItemActive(dAtoi(argv[2]));
	}
	return true;
}

/*! Returns the number of items in the list.
	@return The number of items.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getItemCount, ConsoleInt, 2, 2, "()")
{
	return object->getItemCount();
}

/*! Returns the number of selected items in the list.
	@return The number of items currently selected.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getSelCount, ConsoleInt, 2, 2, "()")
{
	return object->getSelCount();
}

/*! Returns the index of the first selected item.
	@return The index of the first selected item or -1 if no item is selected.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getSelectedItem, ConsoleInt, 2, 2, "()")
{
	return object->getSelectedItem();
}

/*! Returns a list of selected item indexes.
	@return A space-delimited list of selected item indexes or -1 if no items are selected.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getSelectedItems, ConsoleString, 2, 2, "()")
{
	S32 selCount = object->getSelCount();
	if (selCount == -1 || selCount == 0)
		return StringTable->lookup("-1");
	else if (selCount == 1)
		return Con::getIntArg(object->getSelectedItem());

	Vector<S32> selItems;
	object->getSelectedItems(selItems);

	if (selItems.empty())
		return StringTable->lookup("-1");

	UTF8 *retBuffer = Con::getReturnBuffer(selItems.size() * 4);
	dMemset(retBuffer, 0, selItems.size() * 4);
	Vector<S32>::iterator i = selItems.begin();
	for (; i != selItems.end(); i++)
	{
		UTF8 retFormat[12];
		dSprintf(retFormat, 12, "%d ", (*i));
		dStrcat(retBuffer, retFormat);
	}

	return retBuffer;
}

/*! Finds all items with the given text.
	@param itemText The text to search for.
	@param caseSensitive If true, a case-sensitive search will be performed.
	@return A space-delimited list of matching item indexes or -1 if no items are found.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, findItemText, ConsoleInt, 3, 4, "(string itemText, bool caseSensitive)")
{
	bool bCaseSensitive = false;

	if (argc == 4)
		bCaseSensitive = dAtob(argv[3]);

	return object->findItemText(argv[2], bCaseSensitive);
}

/*! Selects the item at the given index.
	@param index The zero-based index of the item that should be selected. Passing a -1 clears the selection.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setCurSel, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->setCurSel(dAtoi(argv[2]));
}

/*! Selects a range of items from a start index to the end or an ending index.
	@param start The zero-based index of the start of the selected range.
	@param stop The optional, zero-based index of the end of the selected range. If no stop is given, the selection will go to the end of the list.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setCurSelRange, ConsoleVoid, 3, 4, "(S32 start, [S32 stop])")
{
	if (argc == 4)
		object->setCurSelRange(dAtoi(argv[2]), dAtoi(argv[3]));
	else
		object->setCurSelRange(dAtoi(argv[2]), 999999);
}

/*! Adds an item to the end of the list with an optional color bullet.
	@param text The text of the new item.
	@param color The optional color of a color bullet that will appear to the left of the text. Values range between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, addItem, ConsoleVoid, 3, 4, "(string text, [color red / green / blue / [alpha]])")
{
	if (argc == 3)
	{
		object->addItem(argv[2]);
	}
	else if (argc == 4)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiListBoxCtrl::addItem() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			object->addItem(argv[2]);
			return;
		}

		F32 red, green, blue, alpha;

		red = dAtof(Utility::mGetStringElement(argv[3], 0));
		green = dAtof(Utility::mGetStringElement(argv[3], 1));
		blue = dAtof(Utility::mGetStringElement(argv[3], 2));
		alpha = colorCount > 3 ? dAtof(Utility::mGetStringElement(argv[3], 3))  : 255;

		object->addItemWithColor(argv[2], ColorI(red, green, blue, alpha));
	}
	else
	{
		Con::warnf("GuiListBoxCtrl::addItem() - Invalid number of parameters!");
	}
}

/*! Adds an item to the end of the list with an ID.
	@param text The text of the new item.
	@param ID The ID to assign to the item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, addItemWithID, ConsoleVoid, 4, 4, "(string text, int ID")
{
	if (argc == 4)
	{
		object->addItemWithID(argv[2], dAtoi(argv[3]));
	}
	else
	{
		Con::warnf("GuiListBoxCtrl::addItemWithID() - Invalid number of parameters! Should be (text, ID).");
	}
}

/*! Sets the color of the color bullet at the given index.
	@param index The zero-based index of the item that should have a color bullet.
	@param color The color of a color bullet that will appear to the left of the text. Values range between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setItemColor, ConsoleVoid, 4, 4, "(S32 index, color red / green / blue / [alpha])")
{
	if (argc == 4)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiListBoxCtrl::setItemColor() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			return;
		}

		F32 red, green, blue, alpha;

		red = dAtof(Utility::mGetStringElement(argv[3], 0));
		green = dAtof(Utility::mGetStringElement(argv[3], 1));
		blue = dAtof(Utility::mGetStringElement(argv[3], 2));
		alpha = colorCount > 3 ? dAtof(Utility::mGetStringElement(argv[3], 3)) : 255;

		object->setItemColor(dAtoi(argv[2]), ColorI(red, green, blue, alpha));
	}
	else
	{
		Con::warnf("GuiListBoxCtrl::setItemColor() - Invalid number of parameters!");
	}
}

/*! Clears the color of the color bullet at the given index.
	@param index The zero-based index of the item's color bullet.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, clearItemColor, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->clearItemColor(dAtoi(argv[2]));
}

/*! Clears the color bullets on all items.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, clearAllColors, ConsoleVoid, 2, 2, "()")
{
	object->clearAllColors();
}

/*! Inserts an item into the list at the specified index.
	@param index The zero-based index of the new location of the item.
	@param text The text of the new item.
	@return Returns the index assigned or -1 on error.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, insertItem, ConsoleVoid, 4, 4, "(S32 index, string text)")
{
	object->insertItem(dAtoi(argv[2]), argv[3]);
}

/*! Removes the item at the given index.
	@param index The zero-based index of the item to delete.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, deleteItem, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->deleteItem(dAtoi(argv[2]));
}

/*! Returns the text of the given item index.
	@param index The zero-based index of the item to inspect.
	@return The text of the inspected item.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, getItemText, ConsoleString, 3, 3, "(S32 index)")
{
	return object->getItemText(dAtoi(argv[2]));
}

/*! Sets the text of the item at the given item index.
	@param index The zero-based index of the item that will be updated.
	@param text The text value to update with.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, setItemText, ConsoleVoid, 4, 4, "(S32 index, string text)")
{
	object->setItemText(dAtoi(argv[2]), argv[3]);
}

/*! Sorts the items in the list by their text values.
	@param ascending An optional direction. If true, text will be sorted ascending (A-Z). If false, text will be sorted descending (Z-A). Defaults to Ascending.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, sortByText, ConsoleVoid, 2, 3, "([bool ascending = true])")
{
	bool direction = true;
	if (argc >= 3)
	{
		direction = dAtob(argv[3]);
	}
	object->sortByText(direction);
}

/*! Sorts the items in the list by their IDs.
	@param ascending An optional direction. If true, IDs will be sorted ascending (1, 2, 3...). If false, IDs will be sorted descending (3, 2, 1...). Defaults to Ascending.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiListBoxCtrl, sortByID, ConsoleVoid, 2, 3, "([bool ascending = true])")
{
	bool direction = true;
	if (argc >= 3)
	{
		direction = dAtob(argv[3]);
	}
	object->sortByID(direction);
}

ConsoleMethodGroupEndWithDocs(GuiListBoxCtrl)