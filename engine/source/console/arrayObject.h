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

#ifndef _ARRAYOBJECT_H_
#define _ARRAYOBJECT_H_

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

//	adopted from original code by Daniel Neilsen - 14/10/03
//	dneilsen@iprimus.com.au

static bool sIncreasing;

/// A data structure holding indexed sequences of key/value pairs for script use.
class Array : public SimObject
{
   typedef SimObject Parent;

private:



public:
	struct Element
	{
		StringTableEntry key;
		StringTableEntry value;
	};
	
	S32 mCurrentIndex;
	Vector<Element> mArray;

	Array();
	
	void onRemove();

	//  add()				- (string key, string value)Adds a new element to the end of an array
	bool onAdd();
	
	//  append()			- (Array target)Appends the target array to the array object
	void append(Array* obj);
	
	//  count()				- Get the number of elements in the array
	S32	count();
	
	//  countKey()			- (string key)Get the number of times a particular key is found in the array
	S32	countKey(StringTableEntry key);
	
	//  countValue()		- (string value)Get the number of times a particular value is found in the array
	S32	countValue(StringTableEntry value);
	
	//  crop()				- (Array target)Removes elements with matching keys from array
	void crop(Array* obj);
	
	//  duplicate()			- (Array target)Alters array into an exact duplicate of the target array
	void duplicate(Array* obj);
	
	//  echo()				- Echos the array in the console
	void echo();
	
	//  empty()				- Emptys all elements from an array
	void empty();
	
	//  erase()				- (int index)Removes an element at a specific position from the array
	void erase(S32 index);
	
	//  getCurrent()		- Gets the current pointer index
	S32	getCurrent();

	// setCurrent()			- Sets the current pointer index
	void setCurrent( S32 idx );
	
	//  getIndexFromKey()	- (string key)Search array from current position for the first matching key
	S32	getIndexFromKey(StringTableEntry key);

	//  getIndexFromValue() - (string value)Search array from current position for the first matching value
	S32	getIndexFromValue(StringTableEntry value);
	
	//  getIndexFromKeyValue()		- Search array and return the index from the key and value
	S32	getIndexFromKeyValue( StringTableEntry key, StringTableEntry value);
	
	//  getKeyFromIndex()			- (int index)Get the key of the array element at the submitted index
	StringTableEntry getKeyFromIndex(S32 index);
	
	//  getValueFromIndex()			- (int index)Get the value of the array element at the submitted index
	StringTableEntry getValueFromIndex(S32 index);
	
	//  setKey()			- (string key, int index)Set the key at the given index
	void setKey(StringTableEntry key, S32 index);
	
	//  setValue()			- (string key, int index)Set the value at the given index
	void setValue(StringTableEntry value, S32 index);
		
	//  insert()			- (string key, string value, int index)Adds a new element to a specified position in the array
	void insert(StringTableEntry key, StringTableEntry value, S32 index);
	
	//  moveFirst()			- Moves array pointer to start of array
	S32	moveFirst();
	
	//  moveLast()			- Moves array pointer to end of array
	S32	moveLast();
	
	//  moveNext()			- Moves array pointer to next position (returns -1 if cannot move)
	S32	moveNext();
	
	//  movePrev()			- Moves array pointer to prev position (returns -1 if cannot move)
	S32	movePrev();
	
	// moveIndex()			- Moves a key and value from one index location to another.
	void moveIndex(S32 prev, S32 index);
	
	//  pop_back()			- Removes the last element from the array
	void pop_back();
	
	//  pop_front()			- Removes the first element from the array
	void pop_front();
	
	//  push_back()			- (string key, string value)Adds a new element to the end of an array
	void push_back(StringTableEntry key, StringTableEntry value);
	
	//  push_front()		- (string key, string value)Adds a new element to the front of an array
	void push_front(StringTableEntry key, StringTableEntry value);
	
	//  sort()				- (bool desc)Sorts the array by value (default ascending sort)
	void sort(bool valtest, bool desc, bool numeric);
	
	//  uniqueKey()			- Removes any elements that have duplicated keys (leaving the first instance)
	void uniqueKey();
	
	//  uniqueValue()		- Removes any elements that have duplicated values (leaving the first instance)
	void uniqueValue();
//-----------------------------------------------------------------------------

DECLARE_CONOBJECT(Array);

};
#endif // _ARRAYOBJECT_H_