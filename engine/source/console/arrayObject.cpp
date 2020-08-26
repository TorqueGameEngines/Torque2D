//-----------------------------------------------------------------------------
//
//	Torque Script Array Object
//
//	by Daniel Neilsen - 14/10/03
//	dneilsen@iprimus.com.au
//
//-----------------------------------------------------------------------------
//
//	Installation:
//		1) BACKUP EVERYTHING
//		2) Copy this file into the /engine/console dir and add to your project
//		3) Compile
//
//
//	Usage:
//
//	In script you should create an array object like so
//		%myarray = new array();
//
//	With this object you can then do a variety of functions.  Some examples are:
//  add()				- (string key, string value)Adds a new element to the end of an array
//  append()			- (Array target)Appends the target array to the array object
//  count()				- Get the number of elements in the array
//  countKey()			- (string key)Get the number of times a particular key is found in the array
//  countValue()		- (string value)Get the number of times a particular value is found in the array
//  crop()				- (Array target)Removes elements with matching keys from array
//  duplicate()			- (Array target)Alters array into an exact duplicate of the target array
//  echo()				- Echos the array in the console
//  empty()				- Emptys all elements from an array
//  erase()				- (int index)Removes an element at a specific position from the array
//  getCurrent()		- Gets the current pointer index
//  getIndexFromKey()	- (string key)Search array from current position for the first matching key
//  getIndexFromValue() - (string value)Search array from current position for the first matching value
//  getKey()			- (int index)Get the key of the array element at the submitted index
//  getValue()			- (int index)Get the value of the array element at the submitted index
//  setKey()			- (string key, int index)Set the key at the given index
//  setValue()			- (string key, int index)Set the value at the given index
//  insert()			- (string key, string value, int index)Adds a new element to a specified position in the array
//  moveFirst()			- Moves array pointer to start of array
//  moveLast()			- Moves array pointer to end of array
//  moveNext()			- Moves array pointer to next position (returns -1 if cannot move)
//  movePrev()			- Moves array pointer to prev position (returns -1 if cannot move)
//  pop_back()			- Removes the last element from the array
//  pop_front()			- Removes the first element from the array
//  push_back()			- (string key, string value)Adds a new element to the end of an array
//  push_front()		- (string key, string value)Adds a new element to the front of an array
//  sort()				- (bool desc)Sorts the array by value (default ascending sort)
//  sorta()				- Alpha sorts the array by value in ascending order
//  sortd()				- Alpha sorts the array by value in descending order
//  sortkd()			- Alpha sorts the array by key in descending order
//  sortka()			- (bool desc)Sorts the array by key (default ascending sort)
//  sortn()				- (bool desc)Sorts the array numerically by value (default ascending sort)
//  sortna()			- Numerical sorts the array by value in ascending order
//  sortnd()			- Numerical sorts the array by value in descending order
//  sortnka()			- Numerical sorts the array by key in ascending order
//  sortnkd()			- Numerical sorts the array by key in descending order
//  uniqueKey()			- Removes any elements that have duplicated keys (leaving the first instance)
//  uniqueValue()		- Removes any elements that have duplicated values (leaving the first instance)
//
//
//-----------------------------------------------------------------------------


#include "platform/platform.h"
#include "sim/simBase.h"
#include "console/consoleTypes.h"
#include "math/mMathFn.h"
#include "string/stringTable.h"

static bool sIncreasing;


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

	U32 mCurrentIndex;
	Vector<Element> mArray;

   Array();
   bool onAdd();
   void onRemove();

   DECLARE_CONOBJECT(Array);

	S32					getIndexFromValue(StringTableEntry value);
	S32					getIndexFromKey(StringTableEntry key);
	StringTableEntry	getKeyFromIndex(U32 index);
	StringTableEntry	getValueFromIndex(U32 index);
	U32					count();
	U32					countValue(StringTableEntry value);
	U32					countKey(StringTableEntry key);

	void 		push_back(StringTableEntry key, StringTableEntry value);
	void		push_front(StringTableEntry key, StringTableEntry value);
	void 		insert(StringTableEntry key, StringTableEntry value, U32 index);
	void		pop_back();
	void 		pop_front();
	void 		erase(U32 index);
	void		empty();

	void		uniqueValue();
	void		uniqueKey();
	void		duplicate(Array* obj);
	void		crop(Array* obj);
	void		append(Array* obj);
	void 		setKey(StringTableEntry key, U32 index);
	void 		setValue(StringTableEntry value, U32 index);

	void		sort(bool valtest, bool desc, bool numeric);

	U32			moveFirst();
	U32			moveLast();
	U32			moveNext();
	U32			movePrev();
	U32			getCurrent();

	void		echo();

	void		moveIndex(U32 prev, U32 index);
	S32		getIndexFromKeyValue( StringTableEntry key, StringTableEntry value);

};

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
//-----------------------------------------------------------------------
//	Object Init

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



//---------------------------------------------------------------------
//	Data query functions

S32 Array::getIndexFromValue( StringTableEntry value)  
{  
    for(U32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
  
    for(U32 i=0; i<mCurrentIndex; i++)  
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
    for(U32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].key == key)  
        {  
            return i;  
        }  
    }  
    for(U32 i=0; i<mCurrentIndex; i++)  
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
    for(U32 i=mCurrentIndex; i<mArray.size(); i++)  
    {  
        if(mArray[i].key == key && mArray[i].value == value)  
        {  
            return i;  
        }  
    }  
    for(U32 i=0; i<mCurrentIndex; i++)  
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
StringTableEntry Array::getKeyFromIndex(U32 index)
{
	if(index >= mArray.size() || index < 0)
		return NULL;
	return mArray[index].key;
}

//	Returns the value for a given index.
//	Will return a null value for an invalid index
StringTableEntry Array::getValueFromIndex(U32 index)
{
	if(index >= mArray.size() || index < 0)
		return NULL;
	return mArray[index].value;
}

//	Counts the number of elements in the array
U32 Array::count()
{
	return mArray.size();
}

//	Counts the number of instances of a particular value in the array
U32 Array::countValue(StringTableEntry value)
{
	U32 count = 0;
	for(U32 i=0; i<mArray.size(); i++)
	{
		if(mArray[i].value == value)
			count++;
	}
	return count;
}

//	Counts the number of instances of a particular key in the array
U32 Array::countKey(StringTableEntry key)
{
	U32 count = 0;
	for(U32 i=0; i<mArray.size(); i++)
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
void Array::insert(StringTableEntry key, StringTableEntry value, U32 index)
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
void Array::erase(U32 index)
{
	if(index < 0 || index >= mArray.size())
		return;
	mArray.erase(index);
}

//	Clears an array
void Array::empty()
{
	U32 size = mArray.size();
	for(U32 i=0; i<size; i++)
		mArray.pop_front();
	mCurrentIndex = 0;
}


//	Moves a key and value from one index location to another.
void Array::moveIndex(U32 prev, U32 index)
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
	for(U32 i=0; i<mArray.size(); i++)
	{
		for(U32 j=i+1; j<mArray.size(); j++)
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
	for(U32 i=0; i<mArray.size(); i++)
	{
		for(U32 j=i+1; j<mArray.size(); j++)
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
	for(U32 i=0; i<obj->count(); i++)
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
	for(U32 i=0; i<obj->count(); i++)
	{
		StringTableEntry tempkey = obj->getKeyFromIndex(i);
		for(U32 j=0; j<mArray.size(); j++)
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
	for(U32 i=0; i<obj->count(); i++)
	{
		StringTableEntry tempval = obj->getValueFromIndex(i);
		StringTableEntry tempkey = obj->getKeyFromIndex(i);
		push_back(tempkey, tempval);
  	}
}


//	Sets the key at the given index
void Array::setKey(StringTableEntry key, U32 index)
{
	if(index >= mArray.size())
		return;
	mArray[index].key = key;
}

//	Sets the key at the given index
void Array::setValue(StringTableEntry value, U32 index)
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


//---------------------------------------------------------------------
//	Pointer Manipulation Functions

//	Moves pointer to arrays first position
U32 Array::moveFirst()
{
	mCurrentIndex = 0;
	return mCurrentIndex;
}

//	Moves pointer to arrays last position
U32 Array::moveLast()
{
	mCurrentIndex = mArray.size()-1;
	return mCurrentIndex;
}

//	Moves pointer to arrays next position
//	If last position it returns -1 and no move occurs;
U32 Array::moveNext()
{
	if(mCurrentIndex >= mArray.size()-1)
		return -1;
	mCurrentIndex++;
	return mCurrentIndex;
}

//	Moves pointer to arrays prev position
//	If first position it returns -1 and no move occurs;
U32 Array::movePrev()
{
	if(mCurrentIndex <= 0)
		return -1;
	mCurrentIndex--;
	return mCurrentIndex;
}

//	Returns current pointer index
U32 Array::getCurrent()
{
	return mCurrentIndex;
}


//---------------------------------------------------------------------
//	Data Listing Functions


//	Echos the array to console
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




//---------------------------------------------------------------------
//	Console Functions

ConsoleMethod( Array, getIndexFromValue, S32, 3, 3, "(string value)"
              "Search array from current position for the first matching value")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->getIndexFromValue(value);
}

ConsoleMethod( Array, getIndexFromKey, S32, 3, 3, "(string key)"
              "Search array from current position for the first matching key")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->getIndexFromKey(value);
}

ConsoleMethod( Array, getValue, const char*, 3, 3, "(int index)"  
              "Get the value of the array element at the submitted index")  
{  
   StringTableEntry ret = object->getValueFromIndex(dAtoi(argv[2]));  
   return ret; // oxe 20081103 - no need to use the return buffer mechanism because StringTableEntries are write-once read-many.  
}  
  
ConsoleMethod( Array, getKey, const char*, 3, 3, "(int index)"  
              "Get the key of the array element at the submitted index")  
{  
   StringTableEntry ret = object->getKeyFromIndex(dAtoi(argv[2]));  
   return ret; // oxe 20081103 - no need to use the return buffer mechanism because StringTableEntries are write-once read-many.  
}

ConsoleMethod( Array, setKey, void, 4, 4, "(string key, int index)"
              "Set the key at the given index")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   U32 index = dAtoi(argv[3]);
   object->setKey(key, index);
}

ConsoleMethod( Array, setValue, void, 4, 4, "(string key, int index)"
              "Set the value at the given index")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   U32 index = dAtoi(argv[3]);
   object->setValue(value, index);
}

ConsoleMethod( Array, count, S32, 2, 2, "Get the number of elements in the array")
{
	return (S32)object->count();
}

ConsoleMethod( Array, countValue, S32, 3, 3, "(string value)"
              "Get the number of times a particular value is found in the array")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->countValue(value);
}

ConsoleMethod( Array, countKey, S32, 3, 3, "(string key)"
              "Get the number of times a particular key is found in the array")
{
   StringTableEntry value = StringTable->insert(argv[2]);
   return (S32)object->countKey(value);
}

ConsoleMethod( Array, add, void, 4, 4, "(string key, string value)"
              "Adds a new element to the end of an array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->push_back(key, value);
}

ConsoleMethod( Array, push_back, void, 4, 4, "(string key, string value)"
              "Adds a new element to the end of an array")
{ 
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);

   object->push_back(key, value);
}

ConsoleMethod( Array, push_front, void, 4, 4, "(string key, string value)"
              "Adds a new element to the front of an array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->push_front(key, value);
}

ConsoleMethod( Array, insert, void, 5, 5, "(string key, string value, int index)"
              "Adds a new element to a specified position in the array")
{
   StringTableEntry key = StringTable->insert(argv[2]);
   StringTableEntry value = StringTable->insert(argv[3]);
   object->insert(key, value, dAtoi(argv[4]));
}

ConsoleMethod( Array, pop_back, void, 2, 2, "Removes the last element from the array")
{
   object->pop_back();
}

ConsoleMethod( Array, pop_front, void, 2, 2, "Removes the first element from the array")
{
   object->pop_front();
}

ConsoleMethod( Array, erase, void, 3, 3, "(int index)"
					"Removes an element at a specific position from the array")
{
   object->erase(dAtoi(argv[2]));
}

ConsoleMethod( Array, empty, void, 2, 2, "Emptys all elements from an array")
{
   object->empty();
}

ConsoleMethod( Array, uniqueValue, void, 2, 2, "Removes any elements that have duplicated values (leaving the first instance)")
{
   object->uniqueValue();
}

ConsoleMethod( Array, uniqueKey, void, 2, 2, "Removes any elements that have duplicated keys (leaving the first instance)")
{
   object->uniqueKey();
}

ConsoleMethod( Array, duplicate, bool, 3, 3, "(Array target)"
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

ConsoleMethod( Array, crop, bool, 3, 3, "(Array target)"
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

ConsoleMethod( Array, append, bool, 3, 3, "(Array target)"
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

ConsoleMethod( Array, sort, void, 2, 3, "(bool desc)"
					"Alpha sorts the array by value (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(true,descending,false);
}

ConsoleMethod( Array, sorta, void, 2, 2, "Alpha sorts the array by value in ascending order")
{
	object->sort(true,false,false);
}

ConsoleMethod( Array, sortd, void, 2, 2, "Alpha sorts the array by value in descending order")
{
	object->sort(true,true,false);
}

ConsoleMethod( Array, sortk, void, 2, 3, "(bool desc)"
					"Alpha sorts the array by key (default ascending sort)")
{
	bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(false,descending,false);
}

ConsoleMethod( Array, sortka, void, 2, 2, "Alpha sorts the array by key in ascending order")
{
	object->sort(false,false,false);
}

ConsoleMethod( Array, sortkd, void, 2, 2, "Alpha sorts the array by key in descending order")
{
	object->sort(false,true,false);
}

ConsoleMethod( Array, sortn, void, 2, 3, "(bool desc)"
					"Numerically sorts the array by value (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(true,descending,true);
}

ConsoleMethod( Array, sortna, void, 2, 2, "Numerically sorts the array by value in ascending order")
{
	object->sort(true,false,true);
}

ConsoleMethod( Array, sortnd, void, 2, 2, "Numerically sorts the array by value in descending order")
{
	object->sort(true,true,true);
}

ConsoleMethod( Array, sortnk, void, 2, 3, "(bool desc)"
					"Numerically sorts the array by key (default ascending sort)")
{
    bool descending = argc == 3 ? dAtob(argv[2]) : false;
	object->sort(false,descending,true);
}

ConsoleMethod( Array, sortnka, void, 2, 2, "Numerical sorts the array by key in ascending order")
{
	object->sort(false,false,true);
}

ConsoleMethod( Array, sortnkd, void, 2, 2, "Numerical sorts the array by key in descending order")
{
	object->sort(false,true,true);
}

ConsoleMethod( Array, moveFirst, S32, 2, 2, "Moves array pointer to start of array")
{
	return object->moveFirst();
}

ConsoleMethod( Array, moveLast, S32, 2, 2, "Moves array pointer to end of array")
{
	return object->moveLast();
}

ConsoleMethod( Array, moveNext, S32, 2, 2, "Moves array pointer to next position (returns -1 if cannot move)")
{
	return object->moveNext();
}

ConsoleMethod( Array, movePrev, S32, 2, 2, "Moves array pointer to prev position (returns -1 if cannot move)")
{
	return object->movePrev();
}

ConsoleMethod( Array, getCurrent, S32, 2, 2, "Gets the current pointer index")
{
	return object->getCurrent();
}

ConsoleMethod( Array, echo, void, 2, 2, "Echos the array in the console")
{
	object->echo();
}
