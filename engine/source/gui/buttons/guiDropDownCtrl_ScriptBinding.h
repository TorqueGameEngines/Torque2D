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

ConsoleMethodGroupBeginWithDocs(GuiDropDownCtrl, GuiControl)

/*! Opens the drop down. Does nothing if the drop down is already open.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, openDropDown, ConsoleVoid, 2, 2, "()")
{
	object->openDropDown();
}

/*! Closes the drop down. Does nothing if the drop down is already closed.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, closeDropDown, ConsoleVoid, 2, 2, "()")
{
	object->closeDropDown();
}

/*! Removes all items from the listbox.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, clearItems, ConsoleVoid, 2, 2, "()")
{
	object->getList()->clearItems();
}

/*! Unselects any items that have been selected.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, clearSelection, ConsoleVoid, 2, 2, "()")
{
	object->getList()->clearSelection();
}

/*! Sets an item to selected or unselected using an index.
	@param index The zero-based index of the item that should be selected.
	@param isSelected If true, the item is selected. If false, it is unselected. If omitted, will default to true.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setSelected, ConsoleVoid, 3, 4, "(S32 index, [bool isSelected])")
{
	bool value = true;
	if (argc == 4)
		value = dAtob(argv[3]);

	if (value == true)
		object->getList()->addSelection(dAtoi(argv[2]));
	else
		object->getList()->removeSelection(dAtoi(argv[2]));
}

/*! Sets an item's ID using an index.
	@param index The zero-based index of the item that should have the ID.
	@param ID An interger value that numerically represents this item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setItemID, ConsoleVoid, 4, 4, "(int index, int ID)")
{
	if (argc != 4)
	{
		Con::warnf("GuiDropDownCtrl::setItemID() - Invalid number of parameters! Should be (index, ID).");
	}
	else
	{
		object->getList()->setItemID(dAtoi(argv[2]), dAtoi(argv[3]));
	}
}

/*! Returns an item's ID using an index.
	@param index The zero-based index of the item with the needed ID.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getItemID, ConsoleInt, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiDropDownCtrl::getItemID() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		return object->getList()->getItemID(dAtoi(argv[2]));
	}
	return 0;
}

/*! Finds the first item with the given ID.
	@param ID The item ID to search for.
	@return The first matching item index or -1 if no item is found.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, findItemID, ConsoleInt, 3, 3, "(int ID)")
{
	if (argc != 3)
	{
		Con::warnf("GuiDropDownCtrl::findItemID() - Invalid number of parameters! Should be (ID).");
		return -1;
	}

	return object->getList()->findItemID(dAtoi(argv[2]));
}

/*! Sets an item active using an index.
	@param index The zero-based index of the item that should be active.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setItemActive, ConsoleVoid, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiDropDownCtrl::setItemActive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		object->getList()->setItemActive(dAtoi(argv[2]));
	}
}

/*! Disables an item using an index.
	@param index The zero-based index of the item that should be inactive.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setItemInactive, ConsoleVoid, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiDropDownCtrl::setItemInactive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		object->getList()->setItemInactive(dAtoi(argv[2]));
	}
}

/*! Returns if an item is active or not using an index.
	@param index The zero-based index of the item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getItemActive, ConsoleBool, 3, 3, "(int index)")
{
	if (argc != 3)
	{
		Con::warnf("GuiDropDownCtrl::getItemActive() - Invalid number of parameters! Should be (index).");
	}
	else
	{
		return object->getList()->getItemActive(dAtoi(argv[2]));
	}
	return true;
}

/*! Returns the number of items in the list.
	@return The number of items.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getItemCount, ConsoleInt, 2, 2, "()")
{
	return object->getList()->getItemCount();
}

/*! Returns the index of the selected item.
	@return The index of the selected item or -1 if no item is selected.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getSelectedItem, ConsoleInt, 2, 2, "()")
{
	return object->getList()->getSelectedItem();
}

/*! Finds all items with the given text.
	@param itemText The text to search for.
	@param caseSensitive If true, a case-sensitive search will be performed.
	@return A space-delimited list of matching item indexes or -1 if no items are found.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, findItemText, ConsoleInt, 3, 4, "(string itemText, bool caseSensitive)")
{
	bool bCaseSensitive = false;

	if (argc == 4)
		bCaseSensitive = dAtob(argv[3]);

	return object->getList()->findItemText(argv[2], bCaseSensitive);
}

/*! Selects the item at the given index.
	@param index The zero-based index of the item that should be selected. Passing a -1 clears the selection.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setCurSel, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->getList()->setCurSel(dAtoi(argv[2]));
}

/*! Adds an item to the end of the list with an optional color bullet.
	@param text The text of the new item.
	@param color The optional color of a color bullet that will appear to the left of the text. Values range between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, addItem, ConsoleVoid, 3, 4, "(string text, [color red / green / blue / [alpha]])")
{
	if (argc == 3)
	{
		object->getList()->addItem(argv[2]);
	}
	else if (argc == 4)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiDropDownCtrl::addItem() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			object->getList()->addItem(argv[2]);
			return;
		}

		F32 red, green, blue, alpha;

		red = dAtof(Utility::mGetStringElement(argv[3], 0));
		green = dAtof(Utility::mGetStringElement(argv[3], 1));
		blue = dAtof(Utility::mGetStringElement(argv[3], 2));
		alpha = colorCount > 3 ? dAtof(Utility::mGetStringElement(argv[3], 3)) : 255;

		object->getList()->addItemWithColor(argv[2], ColorI(red, green, blue, alpha));
	}
	else
	{
		Con::warnf("GuiDropDownCtrl::addItem() - Invalid number of parameters!");
	}
}

/*! Adds an item to the end of the list with an ID.
	@param text The text of the new item.
	@param ID The ID to assign to the item.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, addItemWithID, ConsoleVoid, 4, 4, "(string text, int ID")
{
	if (argc == 4)
	{
		object->getList()->addItemWithID(argv[2], dAtoi(argv[3]));
	}
	else
	{
		Con::warnf("GuiDropDownCtrl::addItemWithID() - Invalid number of parameters! Should be (text, ID).");
	}
}

/*! Sets the color of the color bullet at the given index.
	@param index The zero-based index of the item that should have a color bullet.
	@param color The color of a color bullet that will appear to the left of the text. Values range between 0 and 255.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setItemColor, ConsoleVoid, 4, 4, "(S32 index, color red / green / blue / [alpha])")
{
	if (argc == 4)
	{
		const U32 colorCount = Utility::mGetStringElementCount(argv[3]);
		if (colorCount != 4 && colorCount != 3)
		{
			Con::warnf("GuiDropDownCtrl::setItemColor() - Invalid color! Colors require three or four values (red / green / blue / [alpha])!");
			return;
		}

		F32 red, green, blue, alpha;

		red = dAtof(Utility::mGetStringElement(argv[3], 0));
		green = dAtof(Utility::mGetStringElement(argv[3], 1));
		blue = dAtof(Utility::mGetStringElement(argv[3], 2));
		alpha = colorCount > 3 ? dAtof(Utility::mGetStringElement(argv[3], 3)) : 255;

		object->getList()->setItemColor(dAtoi(argv[2]), ColorI(red, green, blue, alpha));
	}
	else
	{
		Con::warnf("GuiDropDownCtrl::setItemColor() - Invalid number of parameters!");
	}
}

/*! Clears the color of the color bullet at the given index.
	@param index The zero-based index of the item's color bullet.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, clearItemColor, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->getList()->clearItemColor(dAtoi(argv[2]));
}

/*! Clears the color bullets on all items.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, clearAllColors, ConsoleVoid, 2, 2, "()")
{
	object->getList()->clearAllColors();
}

/*! Inserts an item into the list at the specified index.
	@param index The zero-based index of the new location of the item.
	@param text The text of the new item.
	@return Returns the index assigned or -1 on error.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, insertItem, ConsoleVoid, 4, 4, "(S32 index, string text)")
{
	object->getList()->insertItem(dAtoi(argv[2]), argv[3]);
}

/*! Removes the item at the given index.
	@param index The zero-based index of the item to delete.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, deleteItem, ConsoleVoid, 3, 3, "(S32 index)")
{
	object->getList()->deleteItem(dAtoi(argv[2]));
}

/*! Returns the text of the given item index.
	@param index The zero-based index of the item to inspect.
	@return The text of the inspected item.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getItemText, ConsoleString, 3, 3, "(S32 index)")
{
	return object->getList()->getItemText(dAtoi(argv[2]));
}

/*! Returns the text of the selected item.
	@return The text of the selected item.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, getText, ConsoleString, 2, 2, "()")
{
	S32 index = object->getList()->getSelectedItem();
	if(index != -1)
	{
		return object->getList()->getItemText(index);
	}
	return "";
}

/*! Sets the text of the item at the given item index.
	@param index The zero-based index of the item that will be updated.
	@param text The text value to update with.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setItemText, ConsoleVoid, 4, 4, "(S32 index, string text)")
{
	object->getList()->setItemText(dAtoi(argv[2]), argv[3]);
}

/*! Sets the currently used BackgroundProfile for the GuiControl
	@param p The BackgroundProfile applies to the the entire screen when the list opens.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setBackgroundProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlBackgroundProfile(profile);
}

/*! Sets the currently used ListBoxProfile for the GuiControl
	@param p The ListBoxProfile is applied to each item in the drop down.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setListBoxProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlListBoxProfile(profile);
}

/*! Sets the currently used ScrollProfile for the GuiControl
	@param p The ScrollProfile is applied the to scrollable area.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setScrollProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlScrollProfile(profile);
}

/*! Sets the currently used ThumbProfile for the GuiControl
	@param p The ThumbProfile is applied to the thumb of the scrollbar.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setThumbProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlThumbProfile(profile);
}

/*! Sets the currently used TrackProfile for the GuiControl
	@param p The TrackProfile is applied to the track area used by the scroll thumb.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setTrackProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlTrackProfile(profile);
}

/*! Sets the currently used ArrowProfile for the GuiControl
	@param p The ArrowProfile is applied to the arrow buttons of the scrollbar if you turn them on.
	@return No return value
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, setArrowProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
	GuiControlProfile* profile;

	if (Sim::findObject(argv[2], profile))
		object->setControlArrowProfile(profile);
}

/*! Sorts the items in the list by their text values.
	@param ascending An optional direction. If true, text will be sorted ascending (A-Z). If false, text will be sorted descending (Z-A). Defaults to Ascending.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, sortByText, ConsoleVoid, 2, 3, "([bool ascending = true])")
{
	bool direction = true;
	if (argc >= 3)
	{
		direction = dAtob(argv[3]);
	}
	object->getList()->sortByText(direction);
}

/*! Sorts the items in the list by their IDs.
	@param ascending An optional direction. If true, IDs will be sorted ascending (1, 2, 3...). If false, IDs will be sorted descending (3, 2, 1...). Defaults to Ascending.
	@return No return value.
*/
ConsoleMethodWithDocs(GuiDropDownCtrl, sortByID, ConsoleVoid, 2, 3, "([bool ascending = true])")
{
	bool direction = true;
	if (argc >= 3)
	{
		direction = dAtob(argv[3]);
	}
	object->getList()->sortByID(direction);
}

ConsoleMethodGroupEndWithDocs(GuiDropDownCtrl)