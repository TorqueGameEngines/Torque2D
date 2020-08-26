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
//
//	adopted from original code by Daniel Neilsen - 14/10/03
//	dneilsen@iprimus.com.au

#include "platform/platform.h"
#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "console/arrayObject.h"
#include "math/mMathFn.h"
#include "string/stringTable.h"

// Script bindings.
#include "arrayObject_ScriptBinding.h"



IMPLEMENT_CONOBJECT(Array);


static S32 QSORT_CALLBACK valueCompare( const void* a, const void* b )
{
   Array::Element *ea = (Array::Element *) (a);
   Array::Element *eb = (Array::Element *) (b);
   S32 result = dStricmp(ea->value, eb->value);
   return ( sIncreasing ? result : -result );
}

static S32 QSORT_CALLBACK valueNumCompare( const void* a, const void* b )
{
   Array::Element *ea = (Array::Element *) (a);
   Array::Element *eb = (Array::Element *) (b);
   F32 aCol = dAtof(ea->value);
   F32 bCol = dAtof(eb->value);
   F32 result = aCol - bCol;
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return ( sIncreasing ? res : -res );
}

static S32 QSORT_CALLBACK keyCompare( const void* a, const void* b )
{
   Array::Element *ea = (Array::Element *) (a);
   Array::Element *eb = (Array::Element *) (b);
   S32 result = dStricmp(ea->key, eb->key);
   return ( sIncreasing ? result : -result );
}

static S32 QSORT_CALLBACK keyNumCompare( const void* a, const void* b )
{
   Array::Element *ea = (Array::Element *) (a);
   Array::Element *eb = (Array::Element *) (b);
   const char* aCol = ea->key;
   const char* bCol = eb->key;
   F32 result = dAtof(aCol) - dAtof(bCol);
   S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);
   return ( sIncreasing ? res : -res );
}

Array::Array()
{
	mCurrentIndex = 0;
}

bool Array::onAdd()
{
   if(!Parent::onAdd())
      return false;

   return true;
}

void Array::onRemove()
{
	//mArray.empty();
	empty(); // Fix per forums (.empty() returns a boolean check if the vector
	         // is empty or not, but doesn't actually remove it)
	Parent::onRemove();
}





S32 Array::getIndexFromValue( StringTableEntry value)  
{  
    for(S32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
  
    for(S32 i=0; i<mCurrentIndex; i++)  
    {  
        if(mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
    return -1;  
}  
  
S32 Array::getIndexFromKey( StringTableEntry key)  
{  
    for(S32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].key == key)  
        {  
            return i;  
        }  
    }  
    for(S32 i=0; i<mCurrentIndex; i++)  
    {  
        if(mArray[i].key == key)  
        {  
            return i;  
        }  
    }  
    return -1;  
}  
  
S32 Array::getIndexFromKeyValue( StringTableEntry key, StringTableEntry value)  
{  
    for(S32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].key == key && mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
    for(S32 i=0; i<mCurrentIndex; i++)  
    {  
        if(mArray[i].key == key && mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
    return -1;  
}

//	Returns the key for a given index.
//	Will return a null value for an invalid index
StringTableEntry Array::getKeyFromIndex(S32 index)
{
	if(index >= mArray.size() || index < 0)
		return NULL;
	return mArray[index].key;
}

//	Returns the value for a given index.
//	Will return a null value for an invalid index
StringTableEntry Array::getValueFromIndex(S32 index)
{
	if(index >= mArray.size() || index < 0)
		return NULL;
	return mArray[index].value;
}

//	Counts the number of elements in the array
S32 Array::count()
{
	return mArray.size();
}

//	Counts the number of instances of a particular value in the array
S32 Array::countValue(StringTableEntry value)
{
	S32 count = 0;
	for(S32 i=0; i<mArray.size(); i++)
	{
		if(mArray[i].value == value)
			count++;
	}
	return count;
}

//	Counts the number of instances of a particular key in the array
S32 Array::countKey(StringTableEntry key)
{
	S32 count = 0;
	for(S32 i=0; i<mArray.size(); i++)
	{
		if(mArray[i].key == key)
			count++;
	}
	return count;
}



//---------------------------------------------------------------------
//	Basic Data Adding / Removing Functions

//	Adds a new array item to the end of the array
void Array::push_back(StringTableEntry key, StringTableEntry value)
{
	Element temp;
	temp.value = value;
	temp.key = key;
	mArray.push_back(temp);
}

//	Adds a new array item to the front of the array
void Array::push_front(StringTableEntry key, StringTableEntry value)
{
	Element temp;
	temp.value = value;
	temp.key = key;
	mArray.push_front(temp);
}

//	Adds a new array item to a particular index of the array
void Array::insert(StringTableEntry key, StringTableEntry value, S32 index)
{
	index = mClampF(index, 0, mArray.size()-1);

	S32 size = mArray.size() - 1;
	for(S32 i=size; i>=index; i--)
	{
		moveIndex(i, i+1);
	}

	Element temp;
	temp.value = value;
	temp.key = key;
	mArray[index] = temp;
}

//	Removes an array item from the end of the array
void Array::pop_back()
{
	if(mArray.size() <= 0)
		return;
	mArray.pop_back();
	if(mCurrentIndex >= mArray.size())
		mCurrentIndex = mArray.size()-1;
}

//	Removes an array item from the end of the array
void Array::pop_front()
{
	if(mArray.size() <= 0)
		return;
	mArray.pop_front();
	if(mCurrentIndex >= mArray.size())
		mCurrentIndex = mArray.size()-1;
}

//	Removes an array item from a particular index of the array
void Array::erase(S32 index)
{
	if(index < 0 || index >= mArray.size())
		return;
	mArray.erase(index);
}

//	Clears an array
void Array::empty()
{
	S32 size = mArray.size();
	for(S32 i=0; i<size; i++)
		mArray.pop_front();
	mCurrentIndex = 0;
}


//	Moves a key and value from one index location to another.
void Array::moveIndex(S32 prev, S32 index)
{
	if(index >= mArray.size())
		push_back(mArray[prev].key, mArray[prev].value);
	else
		mArray[index] = mArray[prev];
	mArray[prev].value = NULL;
	mArray[prev].key = NULL;
}


//---------------------------------------------------------------------
//	Complex Data Alteration Functions


//	Removes any duplicate values from the array
//	(keeps the first instance only)
void Array::uniqueValue()
{
	for(S32 i=0; i<mArray.size(); i++)
	{
		for(S32 j=i+1; j<mArray.size(); j++)
		{
			if(mArray[i].value == mArray[j].value)
			{
				erase(j);
				j--;
			}
		}
	}
}

//	Removes any duplicate keys from the array
//	(keeps the first instance only)
void Array::uniqueKey()
{
	for(S32 i=0; i<mArray.size(); i++)
	{
		for(S32 j=i+1; j<mArray.size(); j++)
		{
			if(mArray[i].key == mArray[j].key)
			{
				erase(j);
				j--;
			}
		}
	}
}

//	Makes this array an exact duplicate of another array
void Array::duplicate(Array* obj)
{
	empty();
	for(S32 i=0; i<obj->count(); i++)
	{
		StringTableEntry tempval = obj->getValueFromIndex(i);
		StringTableEntry tempkey = obj->getKeyFromIndex(i);
		push_back(tempkey, tempval);
	}
	mCurrentIndex = obj->getCurrent();
}

//	Crops the keys that exists in the target array from our current array
void Array::crop(Array* obj)
{
	for(S32 i=0; i<obj->count(); i++)
	{
		StringTableEntry tempkey = obj->getKeyFromIndex(i);
		for(S32 j=0; j<mArray.size(); j++)
		{
			if(mArray[j].key == tempkey)
			{
				mArray.erase(j);
				j--;
			}
		}
  	}
}

//	Appends the target array to our current array
void Array::append(Array* obj)
{
	for(S32 i=0; i<obj->count(); i++)
	{
		StringTableEntry tempval = obj->getValueFromIndex(i);
		StringTableEntry tempkey = obj->getKeyFromIndex(i);
		push_back(tempkey, tempval);
  	}
}


//	Sets the key at the given index
void Array::setKey(StringTableEntry key, S32 index)
{
	if(index >= mArray.size())
		return;
	mArray[index].key = key;
}

//	Sets the key at the given index
void Array::setValue(StringTableEntry value, S32 index)
{
	if(index >= mArray.size())
		return;
	mArray[index].value = value;
}




//---------------------------------------------------------------------
//	Sorting

//	Sorts the array
//	First variable determines whether sorting by value or key
//	Second variable determines if sorting ascending or descending
//	Third variable determines if alpha or numeric search
void Array::sort(bool valsort, bool desc, bool numeric)
{
	sIncreasing = desc ? false : true;

	if(numeric)
	{
		if(valsort)
			dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), valueNumCompare);
		else
			dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), keyNumCompare);
	}
	else
	{
		if(valsort)
			dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), valueCompare);
		else
			dQsort((void *)&(mArray[0]), mArray.size(), sizeof(Element), keyCompare);
	}
}

S32 Array::moveFirst()
{
	mCurrentIndex = 0;
	return mCurrentIndex;
}

S32 Array::moveLast()
{
	mCurrentIndex = mArray.size()-1;
	return mCurrentIndex;
}

S32 Array::moveNext()
{
	if(mCurrentIndex >= mArray.size()-1)
		return -1;
	mCurrentIndex++;
	return mCurrentIndex;
}

S32 Array::movePrev()
{
	if(mCurrentIndex <= 0)
		return -1;
	mCurrentIndex--;
	return mCurrentIndex;
}

S32 Array::getCurrent()
{
	return mCurrentIndex;
}

void Array::setCurrent( S32 idx )
{
   if ( idx < 0 || idx >= mArray.size() )
   {
      Con::errorf( "Array::setCurrent( %d ) is out of the array bounds!", idx );
      return;
   }

   mCurrentIndex = idx;
}

void Array::echo()
{
	Con::printf("Array Listing:");
	Con::printf("Index   Key       Value");
	for(U32 i=0; i < mArray.size(); i++)
	{
		StringTableEntry key = mArray[i].key;
		StringTableEntry val = mArray[i].value;
		Con::printf("%d    [%s]    =>    %s",(U32)i, key, val);
	}
}
