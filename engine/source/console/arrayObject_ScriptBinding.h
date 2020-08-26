//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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

//	adopted from original code by Daniel Neilsen - 14/10/03
//	dneilsen@iprimus.com.au

ConsoleMethodWithDocs( Array, getIndexFromValue, S32, 3, 3, "(string value)"
              "Search array from current position for the first matching value")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->getIndexFromValue(value);
}

ConsoleMethodWithDocs( Array, getIndexFromKey, S32, 3, 3, "(string key)"
              "Search array from current position for the first matching key")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->getIndexFromKey(value);
}

ConsoleMethodWithDocs( Array, getValue, const char*, 3, 3, "(int index)"  
              "Get the value of the array element at the submitted index")  
{  
   StringTableEntry ret = object->getValueFromIndex(dAtoi(argv[2]));  
   return ret; // oxe 20081103 - no need to use the return buffer mechanism because StringTableEntries are write-once read-many.  
}  
  
ConsoleMethodWithDocs( Array, getKey, const char*, 3, 3, "(int index)"  
              "Get the key of the array element at the submitted index")  
{  
   StringTableEntry ret = object->getKeyFromIndex(dAtoi(argv[2]));  
   return ret; // oxe 20081103 - no need to use the return buffer mechanism because StringTableEntries are write-once read-many.  
}

ConsoleMethodWithDocs( Array, setKey, void, 4, 4, "(string key, int index)"
              "Set the key at the given index")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   S32 index = dAtoi(argv[3]);
   object->setKey(key, index);
}

ConsoleMethodWithDocs( Array, setValue, void, 4, 4, "(string key, int index)"
              "Set the value at the given index")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   S32 index = dAtoi(argv[3]);
   object->setValue(value, index);
}

ConsoleMethodWithDocs( Array, count, S32, 2, 2, "Get the number of elements in the array")
{
	return (S32)object->count();
}

ConsoleMethodWithDocs( Array, countValue, S32, 3, 3, "(string value)"
              "Get the number of times a particular value is found in the array")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->countValue(value);
}

ConsoleMethodWithDocs( Array, countKey, S32, 3, 3, "(string key)"
              "Get the number of times a particular key is found in the array")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->countKey(value);
}

ConsoleMethodWithDocs( Array, add, void, 4, 4, "(string key, string value)"
              "Adds a new element to the end of an array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->push_back(key, value);
}

ConsoleMethodWithDocs( Array, push_back, void, 4, 4, "(string key, string value)"
              "Adds a new element to the end of an array")
{ 
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);

   object->push_back(key, value);
}

ConsoleMethodWithDocs( Array, push_front, void, 4, 4, "(string key, string value)"
              "Adds a new element to the front of an array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->push_front(key, value);
}

ConsoleMethodWithDocs( Array, insert, void, 5, 5, "(string key, string value, int index)"
              "Adds a new element to a specified position in the array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->insert(key, value, dAtoi(argv[4]));
}

ConsoleMethodWithDocs( Array, pop_back, void, 2, 2, "Removes the last element from the array")
{
   object->pop_back();
}

ConsoleMethodWithDocs( Array, pop_front, void, 2, 2, "Removes the first element from the array")
{
   object->pop_front();
}

ConsoleMethodWithDocs( Array, erase, void, 3, 3, "(int index)"
					"Removes an element at a specific position from the array")
{
   object->erase(dAtoi(argv[2]));
}

ConsoleMethodWithDocs( Array, empty, void, 2, 2, "Emptys all elements from an array")
{
   object->empty();
}

ConsoleMethodWithDocs( Array, uniqueValue, void, 2, 2, "Removes any elements that have duplicated values (leaving the first instance)")
{
   object->uniqueValue();
}

ConsoleMethodWithDocs( Array, uniqueKey, void, 2, 2, "Removes any elements that have duplicated keys (leaving the first instance)")
{
   object->uniqueKey();
}

ConsoleMethodWithDocs( Array, duplicate, bool, 3, 3, "(Array target)"
					"Alters array into an exact duplicate of the target array")
{
	Array *target;
	if (Sim::findObject(argv[2],target))
	{
		object->duplicate(target);
		return true;
	}
	return false;
}

ConsoleMethodWithDocs( Array, crop, bool, 3, 3, "(Array target)"
					"Removes elements with matching keys from array")
{
	Array *target;
	if (Sim::findObject(argv[2],target))
	{
		object->crop(target);
		return true;
	}
	return false;
}

ConsoleMethodWithDocs( Array, append, bool, 3, 3, "(Array target)"
					"Appends the target array to the array object")
{
	Array *target;
	if (Sim::findObject(argv[2],target))
	{
		object->append(target);
		return true;
	}
	return false;
}

ConsoleMethodWithDocs( Array, sort, void, 2, 3, "(bool desc)"
					"Alpha sorts the array by value (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(true,descending,false);
}

ConsoleMethodWithDocs( Array, sorta, void, 2, 2, "Alpha sorts the array by value in ascending order")
{
	object->sort(true,false,false);
}

ConsoleMethodWithDocs( Array, sortd, void, 2, 2, "Alpha sorts the array by value in descending order")
{
	object->sort(true,true,false);
}

ConsoleMethodWithDocs( Array, sortk, void, 2, 3, "(bool desc)"
					"Alpha sorts the array by key (default ascending sort)")
{
	bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(false,descending,false);
}

ConsoleMethodWithDocs( Array, sortka, void, 2, 2, "Alpha sorts the array by key in ascending order")
{
	object->sort(false,false,false);
}

ConsoleMethodWithDocs( Array, sortkd, void, 2, 2, "Alpha sorts the array by key in descending order")
{
	object->sort(false,true,false);
}

ConsoleMethodWithDocs( Array, sortn, void, 2, 3, "(bool desc)"
					"Numerically sorts the array by value (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(true,descending,true);
}

ConsoleMethodWithDocs( Array, sortna, void, 2, 2, "Numerically sorts the array by value in ascending order")
{
	object->sort(true,false,true);
}

ConsoleMethodWithDocs( Array, sortnd, void, 2, 2, "Numerically sorts the array by value in descending order")
{
	object->sort(true,true,true);
}

ConsoleMethodWithDocs( Array, sortnk, void, 2, 3, "(bool desc)"
					"Numerically sorts the array by key (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(false,descending,true);
}

ConsoleMethodWithDocs( Array, sortnka, void, 2, 2, "Numerical sorts the array by key in ascending order")
{
	object->sort(false,false,true);
}

ConsoleMethodWithDocs( Array, sortnkd, void, 2, 2, "Numerical sorts the array by key in descending order")
{
	object->sort(false,true,true);
}

ConsoleMethodWithDocs( Array, moveFirst, S32, 2, 2, "Moves array pointer to start of array")
{
	return object->moveFirst();
}

ConsoleMethodWithDocs( Array, moveLast, S32, 2, 2, "Moves array pointer to end of array")
{
	return object->moveLast();
}

ConsoleMethodWithDocs( Array, moveNext, S32, 2, 2, "Moves array pointer to next position (returns -1 if cannot move)")
{
	return object->moveNext();
}

ConsoleMethodWithDocs( Array, movePrev, S32, 2, 2, "Moves array pointer to prev position (returns -1 if cannot move)")
{
	return object->movePrev();
}

ConsoleMethodWithDocs( Array, getCurrent, S32, 2, 2, "Gets the current pointer index")
{
	return object->getCurrent();
}

ConsoleMethodWithDocs( Array, echo, void, 2, 2, "Echos the array in the console")
{
	object->echo();
}