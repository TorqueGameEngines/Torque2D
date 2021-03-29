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

#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include "vector.h"
#include "platform/platform.h"
#include "string/stringTable.h"

namespace Hash
{
//-Mat this was copied from StringTable
    void initTolowerTable();
   U32 hash(const char *str);

//-Mat
   inline U32 hash(U32 data)
   {
      return data;
   }

   inline U32 hash(const void *data)
   {
#ifdef TORQUE_CPU_X64
      return (U32)((U64)data);
#else
      return (U32)data;
#endif
   }

   U32 nextPrime(U32);
};

namespace tKeyCompare
{
   template<typename Key>
   inline bool equals( Key keya, Key keyb )
   {
      return ( keya == keyb );
   }

   template<>
   inline bool equals<>( const char *keya, const char *keyb )
   {
       //-Mat make sure this is an accurate compare (do we check case?)
      return ( dStricmp( keya, keyb ) == 0 );
   }
};

/// A HashTable template class.
///
/// The hash table class maps between a key and an associated value. Access
/// using the key is performed using a hash table.  The class provides
/// methods for both unique and equal keys. The global ::hash(Type) function
/// is used for hashing, see util/hash.h
/// @ingroup UtilContainers
template<typename Key, typename Value >
class HashTable
{
public:
   struct Pair {
      Key  key;
      Value value;
      Pair() {}
      Pair(Key k,Value v): key(k), value(v) {}
   };

private:
   struct Node {
      Node* mNext;
      Pair mPair;
      Node(): mNext(0) {}
      Node(Pair p,Node* n): mPair(p),mNext(n) {}
   };

   Node** mTable;                      ///< Hash table
   S32 mTableSize;                     ///< Hash table size
   U32 mSize;                          ///< Number of keys in the table

   U32 _hash(const Key& key) const;
   U32 _index(const Key& key) const;
   Node* _next(U32 index) const;
   void _resize(U32 size);
   void _destroy();

public:
   // iterator support
   template<typename U,typename E, typename M>
   class _Iterator {
      friend class HashTable;
      E* mLink;
      M* mtHashTable;
      operator E*();
   public:
      typedef U  ValueType;
      typedef U* Pointer;
      typedef U& Reference;

      _Iterator()
      {
         mtHashTable = 0;
         mLink = 0;
      }

      _Iterator(M* table,E* ptr)
      {
         mtHashTable = table;
         mLink = ptr;
      }

      _Iterator& operator++()
      {
         mLink = mLink->mNext? mLink->mNext :
            mtHashTable->_next(mtHashTable->_index(mLink->mPair.key) + 1);
         return *this;
      }

      _Iterator operator++(int)
      {
         _Iterator itr(*this);
         ++(*this);
         return itr;
      }
      //-Mat to check if mLink is bad
      Value getValue() {
          if( mLink ) {
              return mLink->mPair.value;
          } 
          return (Value)(0);
      }

      bool operator==(const _Iterator& b) const
      {
         return mtHashTable == b.mtHashTable && mLink == b.mLink;
      }

      bool operator!=(const _Iterator& b) const
      {
         return !(*this == b);
      }

      U* operator->() const
      {
         return &mLink->mPair;
      }

      U& operator*() const
      {
         return mLink->mPair;
      }
   };

   // Types
   typedef Pair        ValueType;
   typedef Pair&       Reference;
   typedef const Pair& ConstReference;
   typedef S32         DifferenceType;
   typedef U32         SizeType;

   typedef _Iterator<Pair,Node,HashTable>  iterator;
   typedef _Iterator<const Pair,const Node,const HashTable>  const_iterator;

   // Initialization
   HashTable();
   ~HashTable();
   HashTable(const HashTable& p);

   // Management
   U32  size() const;                  ///< Return the number of elements
   U32  tableSize() const;             ///< Return the size of the hash bucket table
   void clear();                       ///< Empty the HashTable
   void resize(U32 size);
   bool isEmpty() const;               ///< Returns true if the table is empty
   F32 collisions() const;             ///< Returns the average number of nodes per bucket

   // Insert & erase elements
   iterator insertEqual(const Key& key, const Value&);
   iterator insertUnique(const Key& key, const Value&);
   void erase(iterator);               ///< Erase the given entry
   void erase(const Key& key);         ///< Erase all matching keys from the table

   // HashTable lookup
   iterator findOrInsert(const Key& key);
   iterator find(const Key&);          ///< Find the first entry for the given key
   const_iterator find(const Key&) const;    ///< Find the first entry for the given key
   S32 count(const Key&);              ///< Count the number of matching keys in the table

   // Forward iterator access
   iterator       begin();             ///< iterator to first element
   const_iterator begin() const;        ///< iterator to first element
   iterator       end();               ///< iterator to last element + 1
   const_iterator end() const;          ///< iterator to last element + 1

   void operator=(const HashTable& p);
};


template<typename Key, typename Value> HashTable<Key,Value>::HashTable()
{
   mTableSize = 0;
   mTable = 0;
   mSize = 0;
}

template<typename Key, typename Value> HashTable<Key,Value>::HashTable(const HashTable& p)
{
   mSize = 0;
   mTableSize = 0;
   mTable = 0;
   *this = p;
}

template<typename Key, typename Value> HashTable<Key,Value>::~HashTable()
{
   _destroy();
}


//-----------------------------------------------------------------------------

template<typename Key, typename Value>
inline U32 HashTable<Key,Value>::_hash(const Key& key) const
{
   return Hash::hash(key);
}

template<typename Key, typename Value>
inline U32 HashTable<Key,Value>::_index(const Key& key) const
{
   return _hash(key) % mTableSize;
}

template<typename Key, typename Value>
typename HashTable<Key,Value>::Node* HashTable<Key,Value>::_next(U32 index) const
{
   for (; index < (U32)mTableSize; index++)
      if (Node* node = mTable[index])
         return node;
   return 0;
}

template<typename Key, typename Value>
void HashTable<Key,Value>::_resize(U32 size)
{
   S32 currentSize = mTableSize;
   mTableSize = Hash::nextPrime(size);
   Node** table = new Node*[mTableSize];
   dMemset(table,0,mTableSize * sizeof(Node*));

   for (S32 i = 0; i < currentSize; i++)
      for (Node* node = mTable[i]; node; )
      {
         // Get groups of matching keys
         Node* last = node;
         while (last->mNext && last->mNext->mPair.key == node->mPair.key)
            last = last->mNext;

         // Move the chain to the new table
         Node** link = &table[_index(node->mPair.key)];
         Node* tmp = last->mNext;
         last->mNext = *link;
         *link = node;
         node = tmp;
      }

   delete[] mTable;
   mTable = table;
}

template<typename Key, typename Value>
void HashTable<Key,Value>::_destroy()
{
   for (S32 i = 0; i < mTableSize; i++)
      for (Node* ptr = mTable[i]; ptr; ) 
      {
         Node *tmp = ptr;
         ptr = ptr->mNext;
         delete tmp;
      }
   delete[] mTable;
   mTable = NULL;
}


//-----------------------------------------------------------------------------
// management

template<typename Key, typename Value>
inline U32 HashTable<Key,Value>::size() const
{
   return mSize;
}

template<typename Key, typename Value>
inline U32 HashTable<Key,Value>::tableSize() const
{
   return mTableSize;
}

template<typename Key, typename Value>
inline void HashTable<Key,Value>::clear()
{
   _destroy();
   mTableSize = 0;
   mSize = 0;
}

/// Resize the bucket table for an estimated number of elements.
/// This method will optimize the hash bucket table size to hold the given
/// number of elements.  The size argument is a hint, and will not be the
/// exact size of the table.  If the table is sized down below it's optimal
/// size, the next insert will cause it to be resized again. Normally this
/// function is used to avoid resizes when the number of elements that will
/// be inserted is known in advance.
template<typename Key, typename Value>
inline void HashTable<Key,Value>::resize(U32 size)
{
   _resize(size);
}

template<typename Key, typename Value>
inline bool HashTable<Key,Value>::isEmpty() const
{
   return mSize == 0;
}

template<typename Key, typename Value>
inline F32 HashTable<Key,Value>::collisions() const
{
   S32 chains = 0;
   for (S32 i = 0; i < mTableSize; i++)
      if (mTable[i])
         chains++;
   return F32(mSize) / chains;
}


//-----------------------------------------------------------------------------
// add & remove elements

/// Insert the key value pair but don't insert duplicates.
/// This insert method does not insert duplicate keys. If the key already exists in
/// the table the function will fail and end() is returned.
template<typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::insertUnique(const Key& key, const Value& x)
{
   if (mSize >= (U32)mTableSize)
      _resize(mSize + 1);
   Node** table = &mTable[_index(key)];
   for (Node* itr = *table; itr; itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key) )
         return end();

   mSize++;
   *table = new Node(Pair(key,x),*table);
   return iterator(this,*table);
}

/// Insert the key value pair and allow duplicates.
/// This insert method allows duplicate keys.  Keys are grouped together but
/// are not sorted.
template<typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::insertEqual(const Key& key, const Value& x)
{
   if (mSize >= (U32)mTableSize)
      _resize(mSize + 1);
   // The new key is inserted at the head of any group of matching keys.
   Node** prev = &mTable[_index(key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
         break;
   mSize++;
   *prev = new Node(Pair(key,x),*prev);
   return iterator(this,*prev);
}

template<typename Key, typename Value>
void HashTable<Key,Value>::erase(const Key& key)
{
   if (!mSize)
      return;

   Node** prev = &mTable[_index(key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) ) {
         // Delete matching keys, which should be grouped together.
         do {
            Node* tmp = itr;
            itr = itr->mNext;
            delete tmp;
            mSize--;
         } while (itr && tKeyCompare::equals<Key>( itr->mPair.key, key ) );
         *prev = itr;
         return;
      }
}

template<typename Key, typename Value>
void HashTable<Key,Value>::erase(iterator node)
{
   Node** prev = &mTable[_index(node->key)];
   for (Node* itr = *prev; itr; prev = &itr->mNext, itr = itr->mNext)
      if (itr == node.mLink) {
         *prev = itr->mNext;
         delete itr;
         mSize--;
         return;
      }
}


//-----------------------------------------------------------------------------

/// Find the key, or insert a one if it doesn't exist.
/// Returns the first key in the table that matches, or inserts one if there
/// are none.
template<typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::findOrInsert(const Key& key)
{
   if (mSize >= (U32)mTableSize)
      _resize(mSize + 1);
   Node** table = &mTable[_index(key)];
   for (Node* itr = *table; itr; itr = itr->mNext)
      if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
         return iterator(this,itr);
   mSize++;
   *table = new Node(Pair(key,Value()),*table);
   return iterator(this,*table);
}

template<typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::find(const Key& key)
{
   if (mTableSize)
      for (Node* itr = mTable[_index(key)]; itr; itr = itr->mNext)
         if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) )
            return iterator(this,itr);
   return iterator(this,0);
}

template<typename Key, typename Value>
S32 HashTable<Key,Value>::count(const Key& key)
{
   S32 count = 0;
   if (mTableSize)
      for (Node* itr = mTable[_index(key)]; itr; itr = itr->mNext)
         if ( tKeyCompare::equals<Key>( itr->mPair.key, key ) ) {
            // Matching keys should be grouped together.
            do {
               count++;
               itr = itr->mNext;
            } while (itr && tKeyCompare::equals<Key>( itr->mPair.key, key ) );
            break;
         }
   return count;
}


//-----------------------------------------------------------------------------
// iterator access

template<typename Key, typename Value>
inline typename HashTable<Key,Value>::iterator HashTable<Key,Value>::begin()
{
   return iterator(this,_next(0));
}

template<typename Key, typename Value>
inline typename HashTable<Key,Value>::const_iterator HashTable<Key,Value>::begin() const
{
   return const_iterator(this,_next(0));
}

template<typename Key, typename Value>
inline typename HashTable<Key,Value>::iterator HashTable<Key,Value>::end()
{
   return iterator(this,0);
}

template<typename Key, typename Value>
inline typename HashTable<Key,Value>::const_iterator HashTable<Key,Value>::end() const
{
   return const_iterator(this,0);
}


//-----------------------------------------------------------------------------
// operators

template<typename Key, typename Value>
void HashTable<Key,Value>::operator=(const HashTable& p)
{
   _destroy();
   mTableSize = p.mTableSize;
   mTable = new Node*[mTableSize];
   mSize = p.mSize;
   for (S32 i = 0; i < mTableSize; i++)
      if (Node* itr = p.mTable[i])
      {
         Node** head = &mTable[i];
         do 
         {
            *head = new Node(itr->mPair,0);
            head = &(*head)->mNext;
            itr = itr->mNext;
         } while (itr);
      }
      else
         mTable[i] = 0;
}

//-----------------------------------------------------------------------------
// iterator class

/// A HashMap template class.
/// The map class maps between a key and an associated value. Keys
/// are unique.
/// The hash table class is used as the default implementation so the
/// the key must be hashable, see util/hash.h for details.
/// @ingroup UtilContainers
template<typename Key, typename Value, class Sequence = HashTable<Key,Value> >
class HashMap: private Sequence
{
   typedef HashTable<Key,Value> Parent;

private:
   Sequence mHashMap;

public:
   // types
   typedef typename Parent::Pair Pair;
   typedef Pair        ValueType;
   typedef Pair&       Reference;
   typedef const Pair& ConstReference;

   typedef typename Parent::iterator  iterator;
   typedef typename Parent::const_iterator const_iterator;
   typedef S32         DifferenceType;
   typedef U32         SizeType;

   // initialization
   HashMap() {}
   ~HashMap() {}
   HashMap(const HashMap& p);

   // management
   U32  size() const;                  ///< Return the number of elements
   void clear();                       ///< Empty the HashMap
   bool isEmpty() const;               ///< Returns true if the map is empty

   // insert & erase elements
   iterator insert(const Key& key, const Value&); // Documented below...
   void erase(iterator);               ///< Erase the given entry
   void erase(const Key& key);         ///< Erase the key from the map

   // HashMap lookup
   iterator find(const Key&);          ///< Find entry for the given key
   const_iterator find(const Key&) const;    ///< Find entry for the given key
   bool contains(const Key&a)
   {
      return mHashMap.count(a) > 0;
   }

   // forward iterator access
   iterator       begin();             ///< iterator to first element
   const_iterator begin() const;       ///< iterator to first element
   iterator       end();               ///< IIterator to last element + 1
   const_iterator end() const;         ///< iterator to last element + 1

   // operators
   Value& operator[](const Key&);      ///< Index using the given key. If the key is not currently in the map it is added.
};

template<typename Key, typename Value, class Sequence> HashMap<Key,Value,Sequence>::HashMap(const HashMap& p)
{
   *this = p;
}


//-----------------------------------------------------------------------------
// management

template<typename Key, typename Value, class Sequence>
inline U32 HashMap<Key,Value,Sequence>::size() const
{
   return mHashMap.size();
}

template<typename Key, typename Value, class Sequence>
inline void HashMap<Key,Value,Sequence>::clear()
{
   mHashMap.clear();
}

template<typename Key, typename Value, class Sequence>
inline bool HashMap<Key,Value,Sequence>::isEmpty() const
{
   return mHashMap.isEmpty();
}


//-----------------------------------------------------------------------------
// add & remove elements

/// Insert the key value pair but don't allow duplicates.
/// The map class does not allow duplicates keys. If the key already exists in
/// the map the function will fail and return end().
template<typename Key, typename Value, class Sequence>
typename HashMap<Key,Value,Sequence>::iterator HashMap<Key,Value,Sequence>::insert(const Key& key, const Value& x)
{
   return mHashMap.insertUnique(key,x);
}

template<typename Key, typename Value, class Sequence>
void HashMap<Key,Value,Sequence>::erase(const Key& key)
{
   mHashMap.erase(key);
}

template<typename Key, typename Value, class Sequence>
void HashMap<Key,Value,Sequence>::erase(iterator node)
{
   mHashMap.erase(node);
}


//-----------------------------------------------------------------------------
// Searching

template<typename Key, typename Value, class Sequence>
typename HashMap<Key,Value,Sequence>::iterator HashMap<Key,Value,Sequence>::find(const Key& key)
{
   return mHashMap.find(key);
}

//-----------------------------------------------------------------------------
// iterator access

template<typename Key, typename Value, class Sequence>
inline typename HashMap<Key,Value,Sequence>::iterator HashMap<Key,Value,Sequence>::begin()
{
   return mHashMap.begin();
}

template<typename Key, typename Value, class Sequence>
inline typename HashMap<Key,Value,Sequence>::const_iterator HashMap<Key,Value,Sequence>::begin() const
{
   return mHashMap.begin();
}

template<typename Key, typename Value, class Sequence>
inline typename HashMap<Key,Value,Sequence>::iterator HashMap<Key,Value,Sequence>::end()
{
   return mHashMap.end();
}

template<typename Key, typename Value, class Sequence>
inline typename HashMap<Key,Value,Sequence>::const_iterator HashMap<Key,Value,Sequence>::end() const
{
   return mHashMap.end();
}


//-----------------------------------------------------------------------------
// operators

template<typename Key, typename Value, class Sequence>
inline Value& HashMap<Key,Value,Sequence>::operator[](const Key& key)
{
   return mHashMap.findOrInsert(key)->value;
}








#endif// _HASHTABLE_H
