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

#ifndef _SIM_OBJECT_H_
#define _SIM_OBJECT_H_

#ifndef _CONSOLEOBJECT_H_
#include "console/consoleObject.h"
#endif

#ifndef _SIM_FIELD_DICTIONARY_H_
#include "sim/simFieldDictionary.h"
#endif

#ifndef _TAML_CALLBACKS_H_
#include "persistence/taml/tamlCallbacks.h"
#endif

//-----------------------------------------------------------------------------

using namespace std;
#include <vector>
#include <string>
typedef U32 SimObjectId;
class SimGroup;

//---------------------------------------------------------------------------
/// Base class for objects involved in the simulation.
///
/// @section simobject_intro Introduction
///
/// SimObject is a base class for most of the classes you'll encounter
/// working in Torque. It provides fundamental services allowing "smart"
/// object referencing, creation, destruction, organization, and location.
/// Along with SimEvent, it gives you a flexible event-scheduling system,
/// as well as laying the foundation for the in-game editors, GUI system,
/// and other vital subsystems.
///
/// @section simobject_subclassing Subclassing
///
/// You will spend a lot of your time in Torque subclassing, or working
/// with subclasses of, SimObject. SimObject is designed to be easy to
/// subclass.
///
/// You should not need to override anything in a subclass except:
///     - The constructor/destructor.
///     - processArguments()
///     - onAdd()/onRemove()
///     - onGroupAdd()/onGroupRemove()
///     - onNameChange()
///     - onStaticModified()
///     - onDeleteNotify()
///     - onEditorEnable()/onEditorDisable()
///     - inspectPreApply()/inspectPostApply()
///     - things from ConsoleObject (see ConsoleObject docs for specifics)
///
/// Of course, if you know what you're doing, go nuts! But in most cases, you
/// shouldn't need to touch things not on that list.
///
/// When you subclass, you should define a typedef in the class, called Parent,
/// that references the class you're inheriting from.
///
/// @code
/// class mySubClass : public SimObject {
///     typedef SimObject Parent;
///     ...
/// @endcode
///
/// Then, when you override a method, put in:
///
/// @code
/// bool mySubClass::onAdd()
/// {
///     if(!Parent::onAdd())
///         return false;
///
///     // ... do other things ...
/// }
/// @endcode
///
/// Of course, you want to replace onAdd with the appropriate method call.
///
/// @section simobject_lifecycle A SimObject's Life Cycle
///
/// SimObjects do not live apart. One of the primary benefits of using a
/// SimObject is that you can uniquely identify it and easily find it (using
/// its ID). Torque does this by keeping a global hierarchy of SimGroups -
/// a tree - containing every registered SimObject. You can then query
/// for a given object using Sim::findObject() (or SimSet::findObject() if
/// you want to search only a specific set).
///
/// @code
///        // Three examples of registering an object.
///
///        // Method 1:
///        AIClient *aiPlayer = new AIClient();
///        aiPlayer->registerObject();
///
///        // Method 2:
///        ActionMap* globalMap = new ActionMap;
///        globalMap->registerObject("GlobalActionMap");
///
///        // Method 3:
///        bool reg = mObj->registerObject(id);
/// @endcode
///
/// Registering a SimObject performs these tasks:
///     - Marks the object as not cleared and not removed.
///     - Assigns the object a unique SimObjectID if it does not have one already.
///     - Adds the object to the global name and ID dictionaries so it can be found
///       again.
///     - Calls the object's onAdd() method. <b>Note:</b> SimObject::onAdd() performs
///       some important initialization steps. See @ref simobject_subclassing "here
///       for details" on how to properly subclass SimObject.
///     - If onAdd() fails (returns false), it calls unregisterObject().
///     - Checks to make sure that the SimObject was properly initialized (and asserts
///       if not).
///
/// Calling registerObject() and passing an ID or a name will cause the object to be
/// assigned that name and/or ID before it is registered.
///
/// Congratulations, you have now registered your object! What now?
///
/// Well, hopefully, the SimObject will have a long, useful life. But eventually,
/// it must die.
///
/// There are a two ways a SimObject can die.
///         - First, the game can be shut down. This causes the root SimGroup
///           to be unregistered and deleted. When a SimGroup is unregistered,
///           it unregisters all of its member SimObjects; this results in everything
///           that has been registered with Sim being unregistered, as everything
///           registered with Sim is in the root group.
///         - Second, you can manually kill it off, either by calling unregisterObject()
///           or by calling deleteObject().
///
/// When you unregister a SimObject, the following tasks are performed:
///     - The object is flagged as removed.
///     - Notifications are cleaned up.
///     - If the object is in a group, then it removes itself from the group.
///     - Delete notifications are sent out.
///     - Finally, the object removes itself from the Sim globals, and tells
///       Sim to get rid of any pending events for it.
///
/// If you call deleteObject(), all of the above tasks are performed, in addition
/// to some sanity checking to make sure the object was previously added properly,
/// and isn't in the process of being deleted. After the object is unregistered, it
/// de-allocates itself.
///
/// @section simobject_editor Torque Editors
///
/// SimObjects are one of the building blocks for the in-game editors. They
/// provide a basic interface for the editor to be able to list the fields
/// of the object, update them safely and reliably, and inform the object
/// things have changed.
///
/// This interface is implemented in the following areas:
///     - onNameChange() is called when the object is renamed.
///     - onStaticModified() is called whenever a static field is modified.
///     - inspectPreApply() is called before the object's fields are updated,
///                     when changes are being applied.
///     - inspectPostApply() is called after the object's fields are updated.
///     - onEditorEnable() is called whenever an editor is enabled (for instance,
///                     when you hit F11 to bring up the world editor).
///     - onEditorDisable() is called whenever the editor is disabled (for instance,
///                     when you hit F11 again to close the world editor).
///
/// (Note: you can check the variable gEditingMission to see if the mission editor
/// is running; if so, you may want to render special indicators. For instance, the
/// fxFoliageReplicator renders inner and outer radii when the mission editor is
/// running.)
///
/// @section simobject_console The Console
///
/// SimObject extends ConsoleObject by allowing you to
/// to set arbitrary dynamic fields on the object, as well as
/// statically defined fields. This is done through two methods,
/// setDataField and getDataField, which deal with the complexities of
/// allowing access to two different types of object fields.
///
/// Static fields take priority over dynamic fields. This is to be
/// expected, as the role of dynamic fields is to allow data to be
/// stored in addition to the predefined fields.
///
/// The fields in a SimObject are like properties (or fields) in a class.
///
/// Some fields may be arrays, which is what the array parameter is for; if it's non-null,
/// then it is parsed with dAtoI and used as an index into the array. If you access something
/// as an array which isn't, then you get an empty string.
///
/// <b>You don't need to read any further than this.</b> Right now,
/// set/getDataField are called a total of 6 times through the entire
/// Torque codebase. Therefore, you probably don't need to be familiar
/// with the details of accessing them. You may want to look at Con::setData
/// instead. Most of the time you will probably be accessing fields directly,
/// or using the scripting language, which in either case means you don't
/// need to do anything special.
///
/// The functions to get/set these fields are very straightforward:
///
/// @code
///  setDataField(StringTable->insert("locked", false), NULL, b ? "true" : "false" );
///  curObject->setDataField(curField, curFieldArray, STR.getStringValue());
///  setDataField(slotName, array, value);
/// @endcode
///
/// <i>For advanced users:</i> There are two flags which control the behavior
/// of these functions. The first is ModStaticFields, which controls whether
/// or not the DataField functions look through the static fields (defined
/// with addField; see ConsoleObject for details) of the class. The second
/// is ModDynamicFields, which controls dynamically defined fields. They are
/// set automatically by the console constructor code.
///
/// @nosubgrouping
class SimObject: public ConsoleObject, public TamlCallbacks
{
    typedef ConsoleObject Parent;

    friend class SimManager;
    friend class SimGroup;
    friend class SimNameDictionary;
    friend class SimManagerNameDictionary;
    friend class SimIdDictionary;

    //-------------------------------------- Structures and enumerations
private:

    /// Flags for use in mFlags
    enum {
        Deleted   = BIT(0),   ///< This object is marked for deletion.
        Removed   = BIT(1),   ///< This object has been unregistered from the object system.
        Added     = BIT(3),   ///< This object has been registered with the object system.
        Selected  = BIT(4),   ///< This object has been marked as selected. (in editor)
        Expanded  = BIT(5),   ///< This object has been marked as expanded. (in editor)
        ModStaticFields  = BIT(6),    ///< The object allows you to read/modify static fields
        ModDynamicFields = BIT(7),    ///< The object allows you to read/modify dynamic fields
        Hidden = BIT(8),   ///< Object is hidden in editors.
        Locked = BIT(9)   ///< Object is locked in editors.
    };

    static const char* _getHidden(void* object, const char* data)
    {
       if (static_cast<SimObject*>(object)->isHidden()) return "1"; return "0";
    }
    static const char* _getLocked(void* object, const char* data)
    {
       if (static_cast<SimObject*>(object)->isLocked()) return "1"; return "0";
    }
    static bool _setHidden(void* object, const char* data)
    {
       static_cast<SimObject*>(object)->setHidden(dAtob(data)); return false;
    }
    static bool _setLocked(void* object, const char* data)
    {
       static_cast<SimObject*>(object)->setLocked(dAtob(data)); return false;
    }
	static bool _writeHidden(void* object, const char* data)
	{
		return static_cast<SimObject*>(object)->isHidden();
	}
	static bool _writeLocked(void* object, const char* data)
	{
		return static_cast<SimObject*>(object)->isLocked();
	}

public:
    /// @name Notification
    /// @{
    struct Notify {
        enum Type {
            ClearNotify,   ///< Notified when the object is cleared.
            DeleteNotify,  ///< Notified when the object is deleted.
            ObjectRef,     ///< Cleverness to allow tracking of references.
            Invalid        ///< Mark this notification as unused (used in freeNotify).
        } type;
        void *ptr;        ///< Data (typically referencing or interested object).
        Notify *next;     ///< Next notification in the linked list.
    };

    /// @}

    enum WriteFlags {
        SelectedOnly = BIT(0) ///< Passed to SimObject::write to indicate that only objects
                            ///  marked as selected should be outputted. Used in SimSet.
    };

private:
    // dictionary information stored on the object
    StringTableEntry objectName;
    SimObject*       nextNameObject;
    SimObject*       nextManagerNameObject;
    SimObject*       nextIdObject;

    SimGroup*   mGroup;  ///< SimGroup we're contained in, if any.
    BitSet32    mFlags;

    StringTableEntry    mProgenitorFile;

    S32 mPeriodicTimerID;


    /// @name Notification
    /// @{
    Notify*     mNotifyList;
    /// @}

    Vector<StringTableEntry> mFieldFilter;

protected:
    SimObjectId mId;         ///< Id number for this object.
    StringTableEntry mIdString;
    Namespace*  mNameSpace;
    U32         mTypeMask;

    S32 mScriptCallbackGuard; ///< Whether the object is executing a script callback.

protected:
    /// @name Notification
    /// Helper functions for notification code.
    /// @{

    static SimObject::Notify *mNotifyFreeList;
    static SimObject::Notify *allocNotify();     ///< Get a free Notify structure.
    static void freeNotify(SimObject::Notify*);  ///< Mark a Notify structure as free.

    /// @}

    private:
    SimFieldDictionary *mFieldDictionary;    ///< Storage for dynamic fields.

protected:
    /// Taml callbacks.
    virtual void onTamlPreWrite( void ) {}
    virtual void onTamlPostWrite( void ) {}
    virtual void onTamlPreRead( void ) {}
    virtual void onTamlPostRead( const TamlCustomNodes& customNodes ) {}
    virtual void onTamlAddParent( SimObject* pParentObject ) {}
    virtual void onTamlCustomWrite( TamlCustomNodes& customNodes ) {}
    virtual void onTamlCustomRead( const TamlCustomNodes& customNodes ) {}
    
protected:
    bool    mCanSaveFieldDictionary; ///< true if dynamic fields (added at runtime) should be saved, defaults to true
    StringTableEntry mInternalName; ///< Stores object Internal Name

    // Namespace linking
    StringTableEntry mClassName;     ///< Stores the class name to link script class namespaces
    StringTableEntry mSuperClassName;   ///< Stores super class name to link script class namespaces

    static bool setClass(void* obj, const char* data)                                { static_cast<SimObject*>(obj)->setClassNamespace(data); return false; };
    static bool setSuperClass(void* obj, const char* data)                           { static_cast<SimObject*>(obj)->setSuperClassNamespace(data); return false; };
    static bool writeObjectName(void* obj, StringTableEntry pFieldName)              { SimObject* simObject = static_cast<SimObject*>(obj); return simObject->objectName != NULL && simObject->objectName != StringTable->EmptyString; }
    static bool writeCanSaveDynamicFields( void* obj, StringTableEntry pFieldName )  { return static_cast<SimObject*>(obj)->mCanSaveFieldDictionary == false; }
    static bool writeInternalName( void* obj, StringTableEntry pFieldName )          { SimObject* simObject = static_cast<SimObject*>(obj); return simObject->mInternalName != NULL && simObject->mInternalName != StringTable->EmptyString; }
    static bool setParentGroup(void* obj, const char* data);
    static bool writeParentGroup( void* obj, StringTableEntry pFieldName )           { return static_cast<SimObject*>(obj)->mGroup != NULL; }
    static bool writeSuperclass( void* obj, StringTableEntry pFieldName )            { SimObject* simObject = static_cast<SimObject*>(obj); return simObject->mSuperClassName != NULL && simObject->mSuperClassName != StringTable->EmptyString; }
    static bool writeClass( void* obj, StringTableEntry pFieldName )                 { SimObject* simObject = static_cast<SimObject*>(obj); return simObject->mClassName != NULL && simObject->mClassName != StringTable->EmptyString; }
    static bool setProtectedName(void * obj, const char * data);
    // Accessors
    public:
    StringTableEntry getClassNamespace() const { return mClassName; };
    StringTableEntry getSuperClassNamespace() const { return mSuperClassName; };
    void setClassNamespace( const char* classNamespace );
    void setSuperClassNamespace( const char* superClassNamespace );

    // Script callback deletion guard.
    inline void pushScriptCallbackGuard( void )  { mScriptCallbackGuard++; }
    inline void popScriptCallbackGuard( void )   { mScriptCallbackGuard--; AssertFatal( mScriptCallbackGuard >= 0, "Invalid script callback guard." ); }
    inline S32 getScriptCallbackGuard( void )    { return mScriptCallbackGuard; }

protected:
    void linkNamespaces();
    void unlinkNamespaces();

public:
    /// @name Accessors
    /// @{

    /// Get the value of a field on the object.
    ///
    /// See @ref simobject_console "here" for a detailed discussion of what this
    /// function does.
    ///
    /// @param   slotName    Field to access.
    /// @param   array       String containing index into array
    ///                      (if field is an array); if NULL, it is ignored.
    static bool disableNameChanging;
    const char *getDataField(StringTableEntry slotName, const char *array);

    /// Set the value of a field on the object.
    ///
    /// See @ref simobject_console "here" for a detailed discussion of what this
    /// function does.
    ///
    /// @param   slotName    Field to access.
    /// @param   array       String containing index into array; if NULL, it is ignored.
    /// @param   value       Value to store.
    void setDataField(StringTableEntry slotName, const char *array, const char *value);

    const char *getPrefixedDataField(StringTableEntry fieldName, const char *array);

    void setPrefixedDataField(StringTableEntry fieldName, const char *array, const char *value);

    const char *getPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const S32 fieldType = -1);

    void setPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const char *value, const S32 fieldType = -1);

    StringTableEntry getDataFieldPrefix( StringTableEntry fieldName );

    /// Get the type of a field on the object.
    ///
    /// @param   slotName    Field to access.
    /// @param   array       String containing index into array
    ///                      (if field is an array); if NULL, it is ignored.
    U32 getDataFieldType(StringTableEntry slotName, const char *array);

    /// Get reference to the dictionary containing dynamic fields.
    ///
    /// See @ref simobject_console "here" for a detailed discussion of what this
    /// function does.
    ///
    /// This dictionary can be iterated over using a SimFieldDictionaryIterator.
    SimFieldDictionary * getFieldDictionary() {return(mFieldDictionary);}

    /// Clear all dynamic fields.
    inline void clearDynamicFields( void ) { if ( mFieldDictionary != NULL ) { delete mFieldDictionary; mFieldDictionary = new SimFieldDictionary; } }

    /// Set whether fields created at runtime should be saved. Default is true.
    void        setCanSaveDynamicFields(bool bCanSave){ mCanSaveFieldDictionary =   bCanSave;}
    /// Get whether fields created at runtime should be saved. Default is true.
    inline bool getCanSaveDynamicFields(void) const { return    mCanSaveFieldDictionary;}

    /// These functions support internal naming that is not namespace
    /// bound for locating child controls in a generic way.
    ///
    /// Set the internal name of this control (Not linked to a namespace)
    void setInternalName(const char* newname);
    /// Get the internal of of this control
    StringTableEntry getInternalName();

    /// Save object as a TorqueScript File.
    virtual bool        save(const char* pcFilePath, bool bOnlySelected=false);

    /// Check if a method exists in the objects current namespace.
    virtual bool isMethod( const char* methodName );
    /// @}

    /// @name Initialization
    /// @{

    ///
    SimObject();
    virtual ~SimObject();

    virtual bool processArguments(S32 argc, const char **argv);  ///< Process constructor options. (ie, new SimObject(1,2,3))

    /// @}

    /// @name Events
    /// @{
    virtual bool onAdd();                                ///< Called when the object is added to the sim.
    virtual void onRemove();                             ///< Called when the object is removed from the sim.
    virtual void onGroupAdd();                           ///< Called when the object is added to a SimGroup.
    virtual void onGroupRemove();                        ///< Called when the object is removed from a SimGroup.
    virtual void onNameChange(const char *name);         ///< Called when the object's name is changed.
    virtual void onStaticModified(const char* slotName, const char*newValue = NULL); ///< Called when a static field is modified.
                                                        ///
                                                        ///  Specifically, this is called by setDataField
                                                        ///  when a static field is modified, see
                                                        ///  @ref simobject_console "the console details".

    /// Called before any property of the object is changed in the world editor.
    ///
    /// The calling order here is:
    ///  - inspectPreApply()
    ///  - ...
    ///  - calls to setDataField()
    ///  - ...
    ///  - inspectPostApply()
    virtual void inspectPreApply();

    /// Called after any property of the object is changed in the world editor.
    ///
    /// @see inspectPreApply
    virtual void inspectPostApply();

    /// Called when a SimObject is deleted.
    ///
    /// When you are on the notification list for another object
    /// and it is deleted, this method is called.
    virtual void onDeleteNotify(SimObject *object);

    /// Called when the editor is activated.
    virtual void onEditorEnable(){};

    /// Called when the editor is deactivated.
    virtual void onEditorDisable(){};

    /// @}

    /// Find a named sub-object of this object.
    ///
    /// This is subclassed in the SimGroup and SimSet classes.
    ///
    /// For a single object, it just returns NULL, as normal objects cannot have children.
    virtual SimObject *findObject(const char *name);

    /// @name Notification
    /// @{
    Notify *removeNotify(void *ptr, Notify::Type);   ///< Remove a notification from the list.
    void deleteNotify(SimObject* obj);               ///< Notify an object when we are deleted.
    void clearNotify(SimObject* obj);                ///< Notify an object when we are cleared.
    void clearAllNotifications();                    ///< Remove all notifications for this object.
    void processDeleteNotifies();                    ///< Send out deletion notifications.

    /// Register a reference to this object.
    ///
    /// You pass a pointer to your reference to this object.
    ///
    /// When the object is deleted, it will null your
    /// pointer, ensuring you don't access old memory.
    ///
    /// @param obj   Pointer to your reference to the object.
    void registerReference(SimObject **obj);

    /// Unregister a reference to this object.
    ///
    /// Remove a reference from the list, so that it won't
    /// get nulled inappropriately.
    ///
    /// Call this when you're done with your reference to
    /// the object, especially if you're going to free the
    /// memory. Otherwise, you may erroneously get something
    /// overwritten.
    ///
    /// @see registerReference
    void unregisterReference(SimObject **obj);

    /// @}

    /// @name Registration
    ///
    /// SimObjects must be registered with the object system.
    /// @{


    /// Register an object with the object system.
    ///
    /// This must be called if you want to keep the object around.
    /// In the rare case that you will delete the object immediately, or
    /// don't want to be able to use Sim::findObject to locate it, then
    /// you don't need to register it.
    ///
    /// registerObject adds the object to the global ID and name dictionaries,
    /// after first assigning it a new ID number. It calls onAdd(). If onAdd fails,
    /// it unregisters the object and returns false.
    ///
    /// If a subclass's onAdd doesn't eventually call SimObject::onAdd(), it will
    /// cause an assertion.
    bool registerObject();

    /// Register the object, forcing the id.
    ///
    /// @see registerObject()
    /// @param   id  ID to assign to the object.
    bool registerObject(U32 id);

    /// Register the object, assigning the name.
    ///
    /// @see registerObject()
    /// @param   name  Name to assign to the object.
    bool registerObject(const char *name);

    /// Register the object, assigning a name and ID.
    ///
    /// @see registerObject()
    /// @param   name  Name to assign to the object.
    /// @param   id  ID to assign to the object.
    bool registerObject(const char *name, U32 id);

    /// Unregister the object from Sim.
    ///
    /// This performs several operations:
    ///  - Sets the removed flag.
    ///  - Call onRemove()
    ///  - Clear out notifications.
    ///  - Remove the object from...
    ///      - its group, if any. (via getGroup)
    ///      - Sim::gNameDictionary
    ///      - Sim::gIDDictionary
    ///  - Finally, cancel any pending events for this object (as it can't receive them now).
    void unregisterObject();

    void deleteObject();     ///< Unregister, mark as deleted, and free the object.
                            ///
                            /// This helper function can be used when you're done with the object
                            /// and don't want to be bothered with the details of cleaning it up.

    /// @}

    /// @name Accessors
    /// @{
    inline SimObjectId getId( void ) const { return mId; }
    inline StringTableEntry getIdString( void ) const { return mIdString; }
    U32 getType() const  { return mTypeMask; }
    const StringTableEntry getName( void ) const { return objectName; };

    void setId(SimObjectId id);
    void assignName(const char* name);
    SimGroup* getGroup() const { return mGroup; }
    bool isChildOfGroup(SimGroup* pGroup);
    bool isProperlyAdded() const { return mFlags.test(Added); }
    bool isDeleted() const { return mFlags.test(Deleted); }
    bool isRemoved() const { return mFlags.test(Deleted | Removed); }
    bool isLocked() const { return mFlags.test(Locked); }
    void setLocked( bool b );
    bool isHidden()const { return mFlags.test(Hidden); }
    void setHidden(bool b);

    inline void setProgenitorFile( const char* pFile ) { mProgenitorFile = StringTable->insert( pFile ); }
    inline StringTableEntry getProgenitorFile( void ) const { return mProgenitorFile; }

    inline void setPeriodicTimerID( const S32 timerID )     { mPeriodicTimerID = timerID; }
    inline S32 getPeriodicTimerID( void ) const             { return mPeriodicTimerID; }
    inline bool isPeriodicTimerActive( void ) const         { return mPeriodicTimerID != 0; }

    /// @}

    /// @name Sets
    ///
    /// The object must be properly registered before you can add/remove it to/from a set.
    ///
    /// All these functions accept either a name or ID to identify the set you wish
    /// to operate on. Then they call addObject or removeObject on the set, which
    /// sets up appropriate notifications.
    ///
    /// An object may be in multiple sets at a time.
    /// @{
    bool addToSet(SimObjectId);
    bool addToSet(const char *);
    bool removeFromSet(SimObjectId);
    bool removeFromSet(const char *);

    /// @}

    /// @name Serialization
    /// @{

    /// Determine whether or not a field should be written.
    ///
    /// @param   fieldname The name of the field being written.
    /// @param   value The value of the field.
    virtual bool writeField(StringTableEntry fieldname, const char* value);

    /// Output the TorqueScript to recreate this object.
    ///
    /// This calls writeFields internally.
    /// @param   stream  Stream to output to.
    /// @param   tabStop Indentation level for this object.
    /// @param   flags   If SelectedOnly is passed here, then
    ///                  only objects marked as selected (using setSelected)
    ///                  will output themselves.
    virtual void write(Stream &stream, U32 tabStop, U32 flags = 0);

    /// Write the fields of this object in TorqueScript.
    ///
    /// @param   stream  Stream for output.
    /// @param   tabStop Indentation level for the fields.
    virtual void writeFields(Stream &stream, U32 tabStop);

    virtual bool writeObject(Stream *stream);
    virtual bool readObject(Stream *stream);
    virtual void buildFilterList();

    void addFieldFilter(const char *fieldName);
    void removeFieldFilter(const char *fieldName);
    void clearFieldFilters();
    bool isFiltered(const char *fieldName);

    /// Copy fields from another object onto this one.
    ///
    /// Objects must be of same type. Everything from obj
    /// will overwrite what's in this object; extra fields
    /// in this object will remain. This includes dynamic
    /// fields.
    ///
    /// @param   obj Object to copy from.
    void assignFieldsFrom(SimObject *obj);

    /// Copy dynamic fields from another object onto this one.
    ///
    /// Everything from obj will overwrite what's in this
    /// object.
    ///
    /// @param   obj Object to copy from.
    void assignDynamicFieldsFrom(SimObject *obj);

    /// @}

    /// Return the object's namespace.
    Namespace* getNamespace() { return mNameSpace; }

    /// Get next matching item in namespace.
    ///
    /// This wraps a call to Namespace::tabComplete; it gets the
    /// next thing in the namespace, given a starting value
    /// and a base length of the string. See
    /// Namespace::tabComplete for details.
    const char *tabComplete(const char *prevText, S32 baseLen, bool);

    /// @name Accessors
    /// @{
    bool isSelected() const { return mFlags.test(Selected); }
    bool isExpanded() const { return mFlags.test(Expanded); }
    void setSelected(bool sel) { if(sel) mFlags.set(Selected); else mFlags.clear(Selected); }
    void setExpanded(bool exp) { if(exp) mFlags.set(Expanded); else mFlags.clear(Expanded); }
    void setModDynamicFields(bool dyn) { if(dyn) mFlags.set(ModDynamicFields); else mFlags.clear(ModDynamicFields); }
    void setModStaticFields(bool sta) { if(sta) mFlags.set(ModStaticFields); else mFlags.clear(ModStaticFields); }

    /// @}

    /// @Object to Object Events
    /// @{
private:
    struct OtoListener {
        bool doomed;
        std::string objID;
    };
    std::vector<OtoListener> mListenerList;
    bool bIsEventRaised;
public:
    void addListener(std::string objID);
    void removeListener(std::string objID);
    void removeAllListeners();
    void postEvent(std::string eventName, std::string data);

    /// @}

public:
    virtual void            dump();
    virtual void            dumpClassHierarchy();

    static void initPersistFields();
    
    SimObject* clone( const bool copyDynamicFields );
    virtual void copyTo(SimObject* object);

    template<typename T> bool isType(void) { return dynamic_cast<T>(this) != NULL; }

    // Component Console Overrides
    virtual bool handlesConsoleMethod(const char * fname, S32 * routingId) { return false; }
    DECLARE_CONOBJECT(SimObject);
};

#endif // _SIM_OBJECT_H_
