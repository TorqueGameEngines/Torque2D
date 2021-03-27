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

ConsoleMethodRootGroupBeginWithDocs(SimObject)

/*! @class SimObject
	SimObject is the base class for all other scripted classes.
	This means that all other "simulation" classes -- be they SceneObjects, Scenes, or plain-old SimObjects
	-- can use the methods and fields of SimObject.
	
	@par Identity

	When we create a SimObject with `::new`, it is given a unique id which is returned by `::new`.
	We usually save the id in our own variables.  Alternatively, we can give the SimObject a name which we can
	use to directly manipulate it.  This name can be set with the `::new` operator or it can be added later.

	If we give an object a name, then we can write script methods that are "scoped" to run on this object only.
	For instance, if we have a SimObject named `MyObject`, and if we call `MyObject.myMethod()`, then this
	call will be handled by a method we named `MyObject::myMethod` if one exists.

	See getId(), getName(), setName()

	@par Static and Dynamic Fields

	Each subclass of SimObject will provide important fields.  For instance, a SceneObject will have a position,
	scale, etc.  These are known as "static" or "built-in" fields.  Additionally, you can add any number of your
	own fields, for example `myField` or `hitPoints`.  These are known as "dynamic" or "add-on" fields.
	To do so only requires you to set the field directly with `%%myObject.myField = myValue;`.  Attempting to
	retrieve a field that does not exist (yet) returns nothing.  There is no error or warning.

	Note that static fields exist for every object of a class, while dynamic fields are unique to any one instance.  Adding
	`myField` to one SceneObject does not add it to all SceneObjects.

	For working with fields more programmatically, see *Reflection* below.

	@par Script Namespace

	We can attach a `Namespace` to an object.  Then calls to this object
	will be handled by the script functions in that Namespace.  For instance, if we set `%%myObject.class = MyScope` then
	the call `%%myObject.myMethod` will be handled with a method named `MyScope::myMethod()`.  (If we also named the object `MyObject`, then
	if there is a `MyObject::myMethod()` it will run.  Otherwise, Torque2D will look for `MyScope::myMethod()` and run that
	if found.)
	
	Finally there is also a *secondary* `Namespace` that will receive the call if neither the `Name` nor the *primary* `Namespace`
	had a method to handle the call.

	Unfortunately, these `Namespaces` are called `Classes` in this context.  You set the `class` or `superclass`.  But this
	should not be confused with the SimObject's "real" class which is SimObject or Scene as examples.

	See getClassNamespace(), setClassNamespace(), getSuperClassNamespace(), setSuperClassNamespace()

	@par Reflection

	SimObject supports "reflection" -- the run-time inspection of an object's methods, fields, etc.  For instance,
	we can ask an object what class it instantiates, what dynamic fields it has, etc.  We can also use this feature to
	call a method on an object even if we only know the string name of the method.

	See getClassName(), isMemberOfClass(), isMethod(), dump(), dumpClassHierarchy(), call()

	See getFieldValue(), setFieldValue(), getFieldCount(), getField(), getDynamicFieldCount(), getDynamicField()

	@par Scheduling Callbacks

	We can set a SimObject to regularly call its own onTimer() callback.  Additionally, we can schedule a single call to
	any method at any time in the future.

	See startTimer(), stopTimer(), isTimerActive(), schedule()

	@par Groups
	TBD

	@par Persistence
	canSaveDynamicFields bool = "1" - Whether a save() shall save the object's Dynamic Fields (member fields created by TorqueScript)

*/

/*! @name Identity
	Reference an object
	@{
*/

/*! get the unique numeric ID -- or "handle" -- of this object.

	@return Returns the numeric ID.

	The id is provided for you by the simulator upon object creation.  You can not change it
	and it likely will not be reused by any other object after this object is deleted.

	@par Example
	@code
	new SimObject(example);
	echo(example.getId());
	> 1752
	@endcode

	@par Caveat
	You can not access the id directly.  That is, you can not access `%%object.id`.
	If you do set `%%object.id` you will only succeed in creating a dynamic field named
	`id` -- an unrelated field to the actual object's id.

	@par Example
	@code
	%example = SimObject();
	echo(%example.getId());
	> 1753

	// warning! this will fail to change the id!
	// it will also not warn you as it is legal syntax
	%example.id = 50;
	echo(%example.getId());
	> 1753

	echo(%example.id);
	> 50
	@endcode

	@sa getName, setName
*/
ConsoleMethodWithDocs(SimObject, getId, ConsoleInt, 2, 2, ())
{
   return object->getId();
}

/*! Set the objects name field.
	@param newName name for objects
	@return no return value

	Now the object can be invoked by this name.
	This is different than tracking an object by a variable, such as `%%myObject` or `$myObject`.
	
	Only one object can have a specific name.  Setting a second object
	with this name will remove the name from the former object.

	Note not to confuse this with the `internalName` which is a name for grouping purposes.

	@par Example
	@code
		%obj = new SimObject();
		%obj.setName("MyName");

		// these are now equivalent
		%obj.save();
		MyName.save();
	@endcode

	@par Caveat
	You can not access the name directly.  That is, you can not access `%%object.name`.
	If you do set `%%object.name` you will only succeed in creating a dynamic field named
	`name` -- an unrelated field to the actual object's name.

	@par Example
	@code
	SimObject("example");
	echo(example.getName());
	> example

	// warning! the field `name` does not exist yet
	echo(example.name);
	>

	// warning! this will fail to change the name!
	// it will also not warn you as it is legal syntax
	%example.name = "newExample";
	echo(%example.getName());
	> example

	echo(%example.name);
	> newExample
	@endcode

	@see setName, getId
*/
ConsoleMethodWithDocs(SimObject, setName, ConsoleVoid, 3, 3, (newName))
{
   object->assignName(argv[2]);
}

/*! Returns the name of the object
	@return the "global" name

	See setName() for a description of the name field.

	Note not to confuse this with the `internalName` which is a name for grouping purposes.

	@par Example
	@code
	%example = new SimObject();
	%example.setName("myObject");

	// now we can reference our object with variables and with its name
	%example.getId();
	> 160

	myObject.getId();
	> 160
	@endcode

	@Caveats
	See setName() for caveats.

	@see setName, getId
*/
ConsoleMethodWithDocs(SimObject, getName, ConsoleString, 2, 2, ())
{
   const char *ret = object->getName();
   return ret ? ret : "";
}

/*! @} */ // member group Identity

/*! @name Scoping
	Manipulate the object's script-defined `Namespace`
	@{
*/

/*! Returns the `Namespace` of this object as set by the user.
	@return The Namespace as set in the object's `class` field.

	The class namespace is a a scripting concept that provides a "namespace" in which the engine looks
	to find user-defined scripting functions.	It can be set, and reset, by the user
	by using setClassNamespace().  Alternatively, it can be set directly using the `class` field of the object.

	Note that this can easily be confused with getClassName(), which is unrelated, and returns the "true"
	engine class name of an object, such as `SimObject`.

	See setClassNamespace() for examples.

	@see setClassNamespace
*/
ConsoleMethodWithDocs(SimObject, getClassNamespace, ConsoleString, 2, 2, ())
{
   return object->getClassNamespace();
}

/*! Return the superclass `Namespace` of this object as set by the user.

	An object can have a primary and secondary `Namespace` also known as its
	`class` and `superclass`.  If a user-defined function is not found in the `class`
	then the `superclass` is searched.

	@see getClassNamespace
*/
ConsoleMethodWithDocs(SimObject, getSuperClassNamespace, ConsoleString, 2, 2, ())
{
   return object->getSuperClassNamespace();
}

/*! Sets the `Namespace` of this object.
	@return no return value

	The class namespace is a a scripting concept that provides a "namespace" in which the engine looks
	to find user-defined scripting functions.	It can be set, and reset, by the user using setClassNamespace().
	Alternatively, it can be set directly using the `class` field of the object.

	The `Namespace` or `class` can then be returned with getClassNamespace().  Note that this can easily be
	confused with getClassName(), which is unrelated, and returns the "true" engine class name of an object,
	such as `SimObject`.

	@par Example
	@code
	%example = new SimObject()
	{
		class = MyScope;
	};

	echo(%example.class);
	> MyScope

	// set the namespace using setNamespace()
	%example.setClassNamespace(DifferentScope);
	echo(%example.class);
	> DifferentScope

	// set the namespace directly using the field 'class'
	%example.class = YetAnotherScope;
	echo(%example.getClassNamespace());
	> YetAnotherScope
	@endcode

	@see getClassNamespace
*/
ConsoleMethodWithDocs(SimObject, setClassNamespace, ConsoleVoid, 2, 3, (nameSpace))
{
   object->setClassNamespace(argv[2]);
}

/*! Sets the superclass `Namespace` of this object.

	An object can have a primary and secondary `Namespace` also known as its
	`class` and `superclass`.  If a user-defined function is not found in the `class`
	then the `superclass` is searched.

	@see setClassNamespace
*/
ConsoleMethodWithDocs(SimObject, setSuperClassNamespace, ConsoleVoid, 2, 3, ())
{
   object->setSuperClassNamespace(argv[2]);
}

/*! @} */ // member group Scoping

/*! @name Reflection
	Methods to query and manipulate the object's class, methods, and fields.
	@{
*/

/*! Returns wether the method exists for this object.

	@returns true if the method exists; false otherwise

	The method must be a "built-in" method, or one that is not user-defined in script.
	It must also be a direct method on the object, and not a behavior defined in a Behavior.
*/
ConsoleMethodWithDocs(SimObject, isMethod, ConsoleBool, 3, 3, (string methodName))
{
   return object->isMethod( argv[2] );
}

/*! Dynamically call a method by a string name

	Normally you would call a method in the form `%object.myMethod(param1, param2)`.
	Alternatively, you can use `%object.call(myMethod, param1, param2)`.  This can be
	useful if, for instance, you don't know which method to call in advance.

	@par Example
	@code
	%method = "setClassNamespace";
	%newNamespace = "myNamespace";

	%object.call(%method, %newNamespace);
	@endcode
*/
ConsoleMethodWithDocs( SimObject, call, ConsoleString, 2, 0, ( methodName, [args]* ))
{
   argv[1] = argv[2];
   return Con::execute( object, argc - 1, argv + 1 );
}

/*! Write the class hierarchy of an object to the console.

	@return no return value

	@par Example
	@code
	new SimGroup(sg);
	echo(sg.dumpClassHierarchy());
	> SimGroup ->
	> SimSet ->
	> SimObject
	@endcode
*/
ConsoleMethodWithDocs(SimObject, dumpClassHierarchy, ConsoleVoid, 2, 2, ())
{
   object->dumpClassHierarchy();
}

/*! dump the object to  the console.

	Use the dump method to display the following information about this object:
	+ All static and dynamic fields that are non-null
	+ All engine and script-registered console methods (including superclass methods) for this object
    @return No return value
*/
ConsoleMethodWithDocs(SimObject,dump, ConsoleVoid, 2, 2, ())
{
	object->dump();
}

/*! returns true if this object is of the specified class or a subclass of the specified class
	@return true if a class or subclass of the given class

	@par Example
	@code
	%example = new SceneObject();

	echo(%example.isMemberOfClass(SimObject);
	> 1

	echo(%example.isMemberOfClass(SimSet);
	> 0
	@endcode
*/
ConsoleMethodWithDocs(SimObject, isMemberOfClass, ConsoleBool, 3, 3,  (string classname))
{

   AbstractClassRep* pRep = object->getClassRep();
   while(pRep)
   {
      if(!dStricmp(pRep->getClassName(), argv[2]))
      {
         //matches
         return true;
      }

      pRep	=	pRep->getParentClass();
   }

   return false;
}

/*! Returns the engine class of this object such as `SimObject` or `SceneObject`
	@return class name

	Note that this method is defined in SimObject but is inherited by subclasses of SimObject.
	Subclasses will return the correct subclass name.

	Note also, getClassName() is not related to an object's `class` field!  The `class` field
	is a scripting concept that provides a "namespace" to look for user-defined functions (see getClassNamespace()).

	@par Example
	@code
	%example = new SimObject()
	{
		class = MyScope;
	};

	echo(%example.getClassName());
	> SimObject
	echo(%example.class);
	> MyScope
	@endcode
*/
ConsoleMethodWithDocs(SimObject, getClassName, ConsoleString, 2, 2, ())
{
   const char *ret = object->getClassName();
   return ret ? ret : "";
}

/*! Return the value of any field.
	This can be a static ("built-in") field or a dynamic ("add-on") field.

	Normally, you would get a field directly as `%%object.field`.
	However, in some cases you may want to use getFieldValue().  For instance,
	suppose you allow the field name to be passed into a function.  You can still
	get that field with `%%object.getFieldValue(%%field)`.

	@param fieldName the name of the field
	@return the value of the field

	@par Example
	@code
	// create a SimObject and set its 'class' field for our example
	%example = new SimObject()
	{
		class = "MyClass";
	}

	// 'class' is a static "built-in" field.  retrieve it directly and with getFieldValue()
	echo(%example.class);
	> MyClass

	echo(%example.getFieldValue(class));
	> MyClass

	// set a dynamic "add-on" field
	%example.myField = "myValue";
	echo(%example.myField);
	> myValue

	echo(%example.getFieldValue(myField));
	> myValue
	@endcode
*/
ConsoleMethodWithDocs(SimObject, getFieldValue, ConsoleString, 3, 3, (fieldName))
{
   const char *fieldName = StringTable->insert( argv[2] );
   return object->getDataField( fieldName, NULL );
}

/*! Set the value of any field.
	This can be a static ("built-in") field or a dynamic ("add-on") field.

	Normally, you would set a field directly as `%%object.field = value`.
	However, in some cases you may want to use setFieldValue().  For instance,
	suppose you allow the field name to be passed into a function.  You can still
	set that field with `%%object.setFieldValue(%field, "myValue")`.

	@param fieldName the name of the field to set
	@param value the value to set
	@return always returns true

	@par Example
	@code
	// create a SimObject
	%example = new SimObject();

	// 'class' is a static "built-in" field.  set it directly and with setFieldValue()
	echo(%example.class);
	>

	%example.class = "MyClass";
	echo(%example.class);
	> MyClass

	%example.setFieldValue(class, "AnotherClass");
	echo(%example.class);
	> AnotherClass

	// set a dynamic "add-on" field
	echo(%example.myField);
	>

	%example.myField = "myValue";
	echo(%example.myField);
	> myValue

	%example.setFieldValue(anotherField, "anotherValue");
	echo(%example.anotherField);
	> anotherValue
	@endcode
*/
ConsoleMethodWithDocs(SimObject, setFieldValue, ConsoleBool, 4, 4, (fieldName,value))
{
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];

   object->setDataField( fieldName, NULL, value );

   return true;

}

ConsoleMethodWithDocs(SimObject, setEditFieldValue, ConsoleBool, 4, 4, (fieldName, value))
{
   const char *fieldName = StringTable->insert(argv[2]);
   const char *value = argv[3];
   object->inspectPreApply();
   object->setDataField(fieldName, NULL, value);
   object->inspectPostApply();

   return true;

}

/*! return the number of dynamic ("add-on") fields.
	@return the number of dynamic fields

	Note that static (or "built-in") fields are not counted.  For instance,
	`SimObject.class` will not count.

	See getDynamicField() for an explanation and examples.

	@see getDynamicField, getField, getFieldCount
*/
ConsoleMethodWithDocs(SimObject, getDynamicFieldCount, ConsoleInt, 2, 2, ())
{
   S32 count = 0;
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
      count++;

   return count;
}

/*! Return the field name of a specific dynamic ("add-on") field by index.
	@param index the dynamic field for which to retrieve the name
	@return the name of the field

	You would normally access dynamic fields directly `%%object.field` or
	indirectly `%%object.getFieldValue(%%field)`.  However, you may not know the
	field's names or otherwise need to iterate over the fields.  Use getDynamicFieldCount()
	to get the number of dynamic fields, and then iterate over them with this function.

	Note that only dynamic ("add-on") fields will be surfaced.  Static ("built-in") fields
	like `SimSet.class` will not be counted or listed.

	While static and dynamic fields have separate functions to get their counts and names, they
	share getFieldValue() and setFieldValue() to read and set any field by name.

	Also note that the order of the fields by an index has no meaning.  It is not alphabetical,
	the order created, or otherwise.

	@par Example
	@code
	%count = %example.getDynamicFieldCount();
	for (%i = 0; %i < %count; %i++)
	{
		%fieldName = %example.getDynamicField(%i);
		%fieldValue = %example.getFieldValue(%fieldName);
		echo(%fieldName @ " = " @ %fieldValue);
	}
	@endcode

	@see getDynamicFieldCount, getField, getFieldCount
*/
ConsoleMethodWithDocs(SimObject, getDynamicField, ConsoleString, 3, 3, (index))
{
   SimFieldDictionary* fieldDictionary = object->getFieldDictionary();
   SimFieldDictionaryIterator itr(fieldDictionary);
   S32 index = dAtoi(argv[2]);
   for (S32 i = 0; i < index; i++)
   {
      if (!(*itr))
      {
         Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
         return NULL;
      }
      ++itr;
   }

   char* buffer = Con::getReturnBuffer(256);
   if (*itr)
   {
      SimFieldDictionary::Entry* entry = *itr;
      dSprintf(buffer, 256, "%s", entry->slotName);
      return buffer;
   }

   Con::warnf("Invalid dynamic field index passed to SimObject::getDynamicField!");
   return NULL;
}

/*! return the number of static ("built-in") fields.
	@return the number of dynamic fields

	Note that dynamic (or "add-on") fields are not counted.  For instance,
	`%%object.class` will count, but `%%object.myField` will not.

	See getField() for an explanation and examples.

	@see getDynamicField, getDynamicFieldCount, getField
*/
ConsoleMethodWithDocs( SimObject, getFieldCount, ConsoleInt, 2, 2, ())
{
   const AbstractClassRep::FieldList &list = object->getFieldList();
   const AbstractClassRep::Field* f;
   U32 numDummyEntries = 0;
   for(int i = 0; i < list.size(); i++)
   {
      f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType )
      {
         numDummyEntries++;
      }
   }

   return list.size() - numDummyEntries;
}

/*! Return the field name of a specific static ("built-in") field by index.
	@param index the static field for which to retrieve the name
	@return the name of the field

	You would normally access static fields directly `%%object.class` or
	indirectly `%%object.getFieldValue(%%field)`.  However, you may not know the
	field's names or otherwise need to iterate over the fields.  Use getFieldCount()
	to get the number of static fields, and then iterate over them with this function.

	Note that only static ("built-in") fields will be surfaced.  Dynamic ("add-on") fields
	like `%%SimSet.myField` will not be counted or listed.

	While static and dynamic fields have separate functions to get their counts and names, they
	share getFieldValue() and setFieldValue() to read and set any field by name.

	Also note that the order of the fields by an index has no meaning.  It is not alphabetical,
	the order created, or otherwise.

	@par Example
	@code
	%count = %example.getFieldCount();
	for (%i = 0; %i < %count; %i++)
	{
		%fieldName = %example.getField(%i);
		%fieldValue = %example.getFieldValue(%fieldName);
		echo(%fieldName @ " = " @ %fieldValue);
	}
	@endcode

	@see getDynamicField, getDynamicFieldCount, getFieldCount

*/
ConsoleMethodWithDocs( SimObject, getField, ConsoleString, 3, 3, (int index))
{
   S32 index = dAtoi( argv[2] );
   const AbstractClassRep::FieldList &list = object->getFieldList();
   if( ( index < 0 ) || ( index >= list.size() ) )
      return "";

   const AbstractClassRep::Field* f;
   S32 currentField = 0;
   for(int i = 0; i < list.size() && currentField <= index; i++)
   {
      f = &list[i];
      
      // skip any dummy fields   
      if(f->type == AbstractClassRep::DepricatedFieldType ||
         f->type == AbstractClassRep::StartGroupFieldType ||
         f->type == AbstractClassRep::EndGroupFieldType)
      {
         continue;
      }

      if(currentField == index)
         return f->pFieldname;
      
      currentField++;
   }  

   // if we found nada, return nada.
   return "";
}

/*! Sets the progenitor file responsible for this instances creation.
    @param file The progenitor file responsible for this instances creation.
    @return No return value.
*/
ConsoleMethodWithDocs(SimObject, setProgenitorFile, ConsoleVoid, 3, 3, (file))
{
    object->setProgenitorFile( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the progenitor file responsible for this instances creation.
    @return The progenitor file responsible for this instances creation.
*/
ConsoleMethodWithDocs(SimObject, getProgenitorFile, ConsoleString, 2, 2, ())
{
    return object->getProgenitorFile();
}

/*! Use the getType method to get the type for this object.

    @return Returns a bit mask containing one or more set bits.

	This is here for legacy purposes.
	
	This type is an integer value composed of bitmasks. For simplicity, these bitmasks
	are defined in the engine and exposed for our use as global variables.
    To simplify the writing of scripts, a set of globals has been provided containing
	the bit setting for each class corresponding to a particular type.
    @sa getClassName
*/
ConsoleMethodWithDocs(SimObject, getType, ConsoleInt, 2, 2, ())
{
   return((S32)object->getType());
}

/*! return the type of a field, such as "int" for an Integer
	@param fieldName field of the object to get the type of
	@return string name of the type; or nothing if the field isn't found

	No warning will be shown if the field isn't found.

	@par Example
	@code
	new sprite(s);
	echo(s.getFieldType(frame));
	> int

	echo(s.getFieldType(blendcolor));
	> ColorF

	echo(s.getFieldType(angle));
	> float

	echo(s.getFieldType(position));
	> Vector2

	echo(s.getFieldType(class));
	> string

	echo(s.getFieldType(notAField));
	>
	@endcode
*/
ConsoleMethodWithDocs(SimObject, getFieldType, ConsoleString, 3, 3, (fieldName))
{
   const char *fieldName = StringTable->insert( argv[2] );
   U32 typeID = object->getDataFieldType( fieldName, NULL );
   ConsoleBaseType* type = ConsoleBaseType::getType( typeID );
   
   if( type )
      return type->getTypeClassName();

   return "";
}

/*! @} */ // member group Reflection

/*! @name Grouping
	Manipulate the (singular) group for this object.
	@{
*/

//-----------------------------------------------------------------------------
//	Set the internal name, can be used to find child objects
//	in a meaningful way, usually from script, while keeping
//	common script functionality together using the controls "Name" field.
//-----------------------------------------------------------------------------
/*! sets the objects "internal" name
	@param internalName the name used for group access
	@return nothing returned

	Not to be confused with the object's `Name`, the internal name is used to
	find this object within a group.  Each object may be in one group, ultimately
	forming a tree (usually for GUI related classes).  See SimGroup for more information.

	@see SimGroup, getInternalName, isChildOfGroup, getGroup
*/
ConsoleMethodWithDocs( SimObject, setInternalName, ConsoleVoid, 3, 3, (string InternalName))
{
   object->setInternalName(argv[2]);
}

/*! returns the objects "internal" name
	@return the internalName used for group access

	Not to be confused with the object's `Name`, the internal name is used to
	find this object within a group.  Each object may be in one group, ultimately
	forming a tree (usually for GUI related classes).  See SimGroup for more information.

	@see SimGroup, setInternalName, isChildOfGroup, getGroup
*/
ConsoleMethodWithDocs( SimObject, getInternalName, ConsoleString, 2, 2, ())
{
   return object->getInternalName();
}

/*! test if this object is in a specified group (or subgroup of it)
	@param groupID the ID of the group being tested
	@returns true if we are in the specified simgroup or a subgroup of it; false otherwise

	@see SimGroup, getInternalName, setInternalName, getGroup
*/
ConsoleMethodWithDocs(SimObject, isChildOfGroup, ConsoleBool, 3,3, (groupID))
{
   SimGroup* pGroup = dynamic_cast<SimGroup*>(Sim::findObject(dAtoi(argv[2])));
   if(pGroup)
   {
      return object->isChildOfGroup(pGroup);
   }

   return false;
}

/*! determines if this object is contained in a SimGroup and if so, which one.
    @return Returns the ID of the SimGroup this shape is in or zero if the shape is not contained in a SimGroup


	@see SimGroup, getInternalName, setInternalName, isChildOfGroup
*/
ConsoleMethodWithDocs(SimObject, getGroup, ConsoleInt, 2, 2, ())
{
   SimGroup *grp = object->getGroup();
   if(!grp)
      return -1;
   return grp->getId();
}

/*! @} */ // member group Grouping

/*! Use the delete method to delete this object.
    When an object is deleted, it automatically
	+ Unregisters its ID and name (if it has one) with the engine.
	+ Removes itself from any SimGroup or SimSet it may be a member of.
	+ (eventually) returns the memory associated with itself and its non-dynamic members.
	+ Cancels all pending %obj.schedule() events.

	For objects in the GameBase, ScriptObject, or GUIControl hierarchies, an object will first: Call the onRemove() method for the object's namespace
    @return No return value.
*/
ConsoleMethodWithDocs(SimObject, delete, ConsoleVoid, 2, 2, ())
{
   object->deleteObject();
}

/*! Clones the object.
    @param copyDynamicFields Whether the dynamic fields should be copied to the cloned object or not.  Optional: Defaults to false.
    @return (newObjectID) The newly cloned object's id if successful, otherwise a 0.
*/
ConsoleMethodWithDocs(SimObject, clone, ConsoleInt, 2, 3, ([copyDynamicFields = false]?))
{
    // Fetch copy dynamic fields flag.
    const bool copyDynamicFields = ( argc >= 3 ) ? dAtob( argv[2] ) : false;

    // Clone Object.
    SimObject* pClonedObject = object->clone( copyDynamicFields );

    // Finish if object was not cloned.
    if ( pClonedObject == NULL )
        return 0;

    return pClonedObject->getId();
}

/*! @name Timer/Scheduled Events
	Perform timed callbacks on the object.
	@{
*/

/*! Starts a periodic timer for this object.
    Sets a timer on the object that, when it expires, will cause the object to execute the given callback function.
    The timer event will continue to occur at regular intervals until stopTimer() is called or the timer expires.
    @param callbackFunction The name of the callback function to call for each timer repetition.
    @param timePeriod The period of time (in milliseconds) between each callback.
    @param repeat The number of times the timer should repeat.  If not specified or zero then it will run infinitely.
    @return No return Value.
*/
ConsoleMethodWithDocs(SimObject, startTimer, ConsoleBool, 4, 5, (callbackFunction, float timePeriod, [repeat]?))
{
    // Is the periodic timer running?
    if ( object->getPeriodicTimerID() != 0 )
    {
        // Yes, so cancel it.
        Sim::cancelEvent( object->getPeriodicTimerID() );

        // Reset Timer ID.
        object->setPeriodicTimerID( 0 );
    }

    // Fetch the callback function.
    StringTableEntry callbackFunction = StringTable->insert( argv[2] );

    // Does the function exist?
    if ( !object->isMethod( callbackFunction ) )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The callback function of '%s' does not exist.", callbackFunction );
        return false;
    }

    // Fetch the time period.
    const S32 timePeriod = dAtoi(argv[3]);

    // Is the time period valid?
    if ( timePeriod < 1 )
    {
        // No, so warn.
        Con::warnf("SimObject::startTimer() - The time period of '%d' is invalid.", timePeriod );
        return false;
    }        

    // Fetch the repeat count.
    const S32 repeat = argc >= 5 ? dAtoi(argv[4]) : 0;

    // Create Timer Event.
    SimObjectTimerEvent* pEvent = new SimObjectTimerEvent( callbackFunction, (U32)timePeriod, (U32)repeat );

    // Post Event.
    object->setPeriodicTimerID( Sim::postEvent( object, pEvent, Sim::getCurrentTime() + timePeriod ) );

    return true;
}

//-----------------------------------------------------------------------------

/*! Stops the periodic timer for this object.
    @return No return Value.
*/
ConsoleMethodWithDocs(SimObject, stopTimer, ConsoleVoid, 2, 2, ())
{
    // Finish if the periodic timer isn't running.
    if ( object->getPeriodicTimerID() == 0 )
        return;

    // Cancel It.
    Sim::cancelEvent( object->getPeriodicTimerID() );

    // Reset Timer ID.
    object->setPeriodicTimerID( 0 );
}

//-----------------------------------------------------------------------------

/*! Checks whether the periodic timer is active for this object or not.
    @return Whether the periodic timer is active for this object or not.
*/
ConsoleMethodWithDocs(SimObject, isTimerActive, ConsoleBool, 2, 2, ())
{
    return object->isPeriodicTimerActive();
}


/*! schedule an action to be executed upon this object in the future.

    @param time Time in milliseconds till action is scheduled to occur.
    @param command Name of the command to execute. This command must be scoped to this object
	(i.e. It must exist in the namespace of the object), otherwise the schedule call will fail.
    @param arg1...argN These are optional arguments which will be passed to the command.
	This version of schedule automatically passes the ID of %obj as arg0 to command.
    @return Returns an integer schedule ID.

	The major difference between this and the ::schedule() console function is that if this object is deleted prior
	to the scheduled event, the event is automatically canceled. Times should not be treated as exact since some
	'simulation delay' is to be expected. The minimum resolution for a scheduled event is 32 ms, or one tick.

	The existence of command is not validated. If you pass an invalid console method name, the
	schedule() method will still return a schedule ID, but the subsequent event will fail silently.

	To manipulate the scheduled event, use the id returned with the system schedule functions.

	@see ::schedule
*/
ConsoleMethodWithDocs(SimObject,schedule, ConsoleInt, 4, 0, (time , command , [arg]* ))
{
   U32 timeDelta = U32(dAtof(argv[2]));
   argv[2] = argv[3];
   argv[3] = argv[1];
   SimConsoleEvent *evt = new SimConsoleEvent(argc - 2, argv + 2, true);
   S32 ret = Sim::postEvent(object, evt, Sim::getCurrentTime() + timeDelta);
// #ifdef DEBUG
//    Con::printf("obj %s schedule(%s) = %d", argv[3], argv[2], ret);
//    Con::executef(1, "backtrace");
// #endif
   return ret;
}

/*! @} */ // member group Timer Events

/*! @name member group Object to Object Events
Raise events for listening objects to consume.
@{
*/

/*! Starts listening to another object.
   @param SimObject The object that will be posting events.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, startListening, ConsoleVoid, 3, 3, (SimObject))
{
   // Find the specified object.
   SimObject* pSimObject = dynamic_cast<SimObject*>(Sim::findObject(argv[2]));

   // Did we find the object?
   if (!pSimObject)
   {
      // No, so warn.
      Con::warnf("SimObject::startListening() - Could not find the specified object '%s'.", argv[2]);
      return;
   }

   // Start Listening
   pSimObject->addListener(object->getIdString());
}

/*! Stops listening to another object.
@param SimObject The object that will be posting events.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, stopListening, ConsoleVoid, 3, 3, (SimObject))
{
   // Find the specified object.
   SimObject* pSimObject = dynamic_cast<SimObject*>(Sim::findObject(argv[2]));

   // Did we find the object?
   if (!pSimObject)
   {
      // No, so warn.
      Con::warnf("SimObject::stopListening() - Could not find the specified object '%s'.", argv[2]);
      return;
   }

   // Stop Listening
   pSimObject->removeListener(object->getIdString());
}

/*! Adds an object so that it receives events from this object.
@param SimObject The object that will be listening to events.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, addListener, ConsoleVoid, 3, 3, (SimObject))
{
   // Find the specified object.
   SimObject* pSimObject = dynamic_cast<SimObject*>(Sim::findObject(argv[2]));

   // Did we find the object?
   if (!pSimObject)
   {
      // No, so warn.
      Con::warnf("SimObject::addListener() - Could not find the specified object '%s'.", argv[2]);
      return;
   }

   // Start Listening
   object->addListener(pSimObject->getIdString());
}

/*! Removes an object so that it no longer receives events from this object.
@param SimObject The object that will stop listening to events.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, removeListener, ConsoleVoid, 3, 3, (SimObject))
{
   // Find the specified object.
   SimObject* pSimObject = dynamic_cast<SimObject*>(Sim::findObject(argv[2]));

   // Did we find the object?
   if (!pSimObject)
   {
      // No, so warn.
      Con::warnf("SimObject::removeListener() - Could not find the specified object '%s'.", argv[2]);
      return;
   }

   // Start Listening
   object->removeListener(pSimObject->getIdString());
}

/*! Removes all listeners from this object.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, removeAllListeners, ConsoleVoid, 2, 2, ())
{
   // Start Listening
   object->removeAllListeners();
}

/*! Raises an event on all listening objects.
    @param eventName The name of the event to raise. The actual function called on listeners will begin with "on" followed by the event name.
    @param data Any data that should be passed on to the listeners.
@return No return value.
*/
ConsoleMethodWithDocs(SimObject, postEvent, ConsoleVoid, 3, 4, (String eventName, String data))
{
   if (argc < 3)
   {
      Con::warnf("SimObject::postEvent() - Invalid number of parameters. You must include an Event Name.");
      return;
   }

   // Start Listening
   object->postEvent(argv[2], argc > 3 ? argv[3] : "");
}

/*! @} */ // member group Object to Object Events

ConsoleMethodRootGroupEndWithDocs(SimObject)
