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

// [tom, 3/9/2007] To avoid having to change pretty much every source file, this
// header is included from simBase.h where the original definitions of SimSet and
// SimGroup were. simSet.h should not be included directly. Include dependencies
// are hell.

#ifndef _SIMSET_H_
#define _SIMSET_H_

#ifndef _SIM_OBJECT_H_
#include "sim/simObject.h"
#endif

#ifndef _SIM_OBJECT_LIST_H_
#include "sim/SimObjectList.h"
#endif

#ifndef _FIND_ITERATOR_H_
#include "collection/findIterator.h"
#endif

#ifndef _SIMDICTIONARY_H_
#include "sim/simDictionary.h"
#endif

#ifndef _TAML_CHILDREN_H_
#include "persistence/taml/tamlChildren.h"
#endif

//---------------------------------------------------------------------------
/// A set of SimObjects.
///
/// It is often necessary to keep track of an arbitrary set of SimObjects.
/// For instance, Torque's networking code needs to not only keep track of
/// the set of objects which need to be ghosted, but also the set of objects
/// which must <i>always</i> be ghosted. It does this by working with two
/// sets. The first of these is the RootGroup (which is actually a SimGroup)
/// and the second is the GhostAlwaysSet, which contains objects which must
/// always be ghosted to the client.
///
/// Some general notes on SimSets:
///     - Membership is not exclusive. A SimObject may be a member of multiple
///       SimSets.
///     - A SimSet does not destroy subobjects when it is destroyed.
///     - A SimSet may hold an arbitrary number of objects.
///
/// Using SimSets, the code to work with these two sets becomes
/// relatively straightforward:
///
/// @code
///        // (Example from netObject.cc)
///        // To iterate over all the objects in the Sim:
///        for (SimSetIterator obj(Sim::getRootGroup()); *obj; ++obj)
///        {
///                  NetObject* nobj = dynamic_cast<NetObject*>(*obj);
///
///                 if (nobj)
///                   {
///                     // ... do things ...
///                 }
///         }
///
///         // (Example from netGhost.cc)
///         // To iterate over the ghostAlways set.
///         SimSet* ghostAlwaysSet = Sim::getGhostAlwaysSet();
///         SimSet::iterator i;
///
///         U32 sz = ghostAlwaysSet->size();
///         S32 j;
///
///         for(i = ghostAlwaysSet->begin(); i != ghostAlwaysSet->end(); i++)
///         {
///             NetObject *obj = (NetObject *)(*i);
///
///             /// ... do things with obj...
///         }
/// @endcode
///

class SimSet: public SimObject, public TamlChildren
{
   typedef SimObject Parent;
   typedef SimObject Children;

protected:
   SimObjectList objectList;
   void *mMutex;

public:
   SimSet() {
      VECTOR_SET_ASSOCIATION(objectList);

      mMutex = Mutex::createMutex();
   }

   ~SimSet()
   {
      lock();
      unlock();
      Mutex::destroyMutex(mMutex);
      mMutex = NULL;
   }

   /// @name STL Interface
   /// @{

   ///
   typedef SimObjectList::iterator iterator;
   typedef SimObjectList::value_type value;
   SimObject* front() { return objectList.front(); }
   SimObject* first() { return objectList.first(); }
   SimObject* last()  { return objectList.last(); }
   bool       empty() { return objectList.empty();   }
   S32        size() const  { return objectList.size(); }
   iterator   begin() { return objectList.begin(); }
   iterator   end()   { return objectList.end(); }
   value operator[] (S32 index) { return objectList[U32(index)]; }

   inline iterator find( iterator first, iterator last, SimObject *obj ) { return ::find(first, last, obj); }
   inline iterator find( SimObject *obj ) { return ::find(begin(), end(), obj); }

   template <typename T> inline bool containsType( void )
   {
       for( iterator itr = begin(); itr != end(); ++itr )
       {
           if ( dynamic_cast<T*>(*itr) != NULL )
               return true;
       }

       return false;
   }

   bool isMember(SimObject* obj);
   virtual bool reOrder( SimObject *obj, SimObject *target=0 );
   SimObject* at(S32 index) const { return objectList.at(index); }

   void deleteObjects( void );

   void clear();
   /// @}

   virtual void onRemove();
   virtual void onDeleteNotify(SimObject *object);

   /// @name Set Management
   /// @{

   virtual void addObject(SimObject*);      ///< Add an object to the set.
   virtual void removeObject(SimObject*);   ///< Remove an object from the set.

   virtual void pushObject(SimObject*);     ///< Add object to end of list.
   ///
   /// It will force the object to the end of the list if it already exists
   /// in the list.

   virtual void popObject();                ///< Remove an object from the end of the list.

   void bringObjectToFront(SimObject* obj) { reOrder(obj, front()); }
   void pushObjectToBack(SimObject* obj) { reOrder(obj, NULL); }

   /// @}

    virtual U32 getTamlChildCount( void ) const
    {
        return (U32)size();
    }

    virtual SimObject* getTamlChild( const U32 childIndex ) const
    {
        // Sanity!
        AssertFatal( childIndex < (U32)size(), "SimSet::getTamlChild() - Child index is out of range." );

        // For when the assert is not used.
        if ( childIndex >= (U32)size() )
            return NULL;

        return at( childIndex );
    }

    virtual void addTamlChild( SimObject* pSimObject )
    {
        // Sanity!
        AssertFatal( pSimObject != NULL, "SimSet::addTamlChild() - Cannot add a NULL child object." );

        addObject( pSimObject );
    }

   void callOnChildren( const char * method, S32 argc, const char *argv[], bool executeOnChildGroups = true );

   virtual void write(Stream &stream, U32 tabStop, U32 flags = 0);

   virtual SimObject *findObject(const char *name);
   SimObject*	findObjectByInternalName(const char* internalName, bool searchChildren = false);

   virtual bool writeObject(Stream *stream);
   virtual bool readObject(Stream *stream);

   inline void lock()
   {
#ifdef TORQUE_MULTITHREAD
      Mutex::lockMutex(mMutex);
#endif
   }

   inline void unlock()
   {
#ifdef TORQUE_MULTITHREAD
      Mutex::unlockMutex(mMutex);
#endif
   }

   DECLARE_CONOBJECT(SimSet);

#ifdef TORQUE_DEBUG
   inline void _setVectorAssoc( const char *file, const U32 line )
   {
      objectList.setFileAssociation( file, line );
   }
#endif
};
#ifdef TORQUE_DEBUG
#  define SIMSET_SET_ASSOCIATION( x ) x._setVectorAssoc( __FILE__, __LINE__ )
#else
#  define SIMSET_SET_ASSOCIATION( x )
#endif

/// Iterator for use with SimSets
///
/// @see SimSet
class SimSetIterator
{
protected:
   struct Entry {
      SimSet* set;
      SimSet::iterator itr;
   };
   class Stack: public Vector<Entry> {
   public:
      void push_back(SimSet*);
   };
   Stack stack;

public:
   SimSetIterator(SimSet*);
   SimObject* operator++();
   SimObject* operator*() {
      return stack.empty()? 0: *stack.last().itr;
   }
};

//---------------------------------------------------------------------------
/// A group of SimObjects.
///
/// A SimGroup is a stricter form of SimSet. SimObjects may only be a member
/// of a single SimGroup at a time.
///
/// The SimGroup will automatically enforce the single-group-membership rule.
///
/// @code
///      // From engine/sim/simPath.cc - getting a pointer to a SimGroup
///      SimGroup* pMissionGroup = dynamic_cast<SimGroup*>(Sim::findObject("MissionGroup"));
///
///      // From game/trigger.cc:46 - iterating over a SimObject's group.
///      SimObject* trigger = ...;
///      SimGroup* pGroup = trigger->getGroup();
///      for (SimGroup::iterator itr = pGroup->begin(); itr != pGroup->end(); itr++)
///      {
///         // do something with *itr
///      }
/// @endcode
class SimGroup: public SimSet
{
private:
   friend class SimManager;
   friend class SimObject;

   typedef SimSet Parent;
   SimNameDictionary nameDictionary;

public:
   ~SimGroup();

   /// Add an object to the group.
   virtual void addObject(SimObject*);
   void addObject(SimObject*, SimObjectId);
   void addObject(SimObject*, const char *name);

   /// Remove an object from the group.
   virtual void removeObject(SimObject*);

   virtual void onRemove();

   virtual void onChildRemoved(SimObject*);

   /// Find an object in the group.
   virtual SimObject* findObject(const char* name);

   bool processArguments(S32 argc, const char **argv);

   DECLARE_CONOBJECT(SimGroup);
};

inline void SimGroup::addObject(SimObject* obj, SimObjectId id)
{
   obj->mId = id;
   addObject( obj );
}

inline void SimGroup::addObject(SimObject *obj, const char *name)
{
   addObject( obj );
   obj->assignName(name);
}

class SimGroupIterator: public SimSetIterator
{
public:
   SimGroupIterator(SimGroup* grp): SimSetIterator(grp) {}
   SimObject* operator++();
};

#endif // _SIMSET_H_
