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

#include "sim/simObject.h"
#include "sim/simObjectTimerEvent.h"
#include "console/consoleInternal.h"
#include "console/codeBlock.h"
#include "console/consoleInternal.h"
#include "memory/frameAllocator.h"
#include "io/fileStream.h"
#include "io/fileObject.h"
#include "console/ConsoleTypeValidators.h"

#include "simObject_ScriptBinding.h"
#include <algorithm>

//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(SimObject);

namespace Sim
{
    extern U32 gNextObjectId;
    extern SimIdDictionary *gIdDictionary;
    extern SimManagerNameDictionary *gNameDictionary;
    extern void cancelPendingEvents(SimObject *obj);
}

//-----------------------------------------------------------------------------
bool SimObject::disableNameChanging = false;
//-----------------------------------------------------------------------------

SimObject::SimObject()
{
    mFlags.set( ModStaticFields | ModDynamicFields );
    objectName               = NULL;
    mInternalName            = NULL;
    nextNameObject           = (SimObject*)-1;
    nextManagerNameObject    = (SimObject*)-1;
    nextIdObject             = NULL;  
    mId                      = 0;
    mIdString                = StringTable->EmptyString;
    mGroup                   = 0;
    mNameSpace               = NULL;
    mNotifyList              = NULL;
    mTypeMask                = 0;
    mScriptCallbackGuard     = 0;
    mFieldDictionary         = NULL;
    mCanSaveFieldDictionary  = true;
    mClassName               = NULL;
    mSuperClassName          = NULL;
    mProgenitorFile          = CodeBlock::getCurrentCodeBlockFullPath();
    mPeriodicTimerID         = 0;
    bIsEventRaised           = false;
}

//---------------------------------------------------------------------------

bool SimObject::isMethod( const char* methodName )
{
   if( !methodName || !methodName[0] )
      return false;

   StringTableEntry stname = StringTable->insert( methodName );

   if( getNamespace() )
      return ( getNamespace()->lookup( stname ) != NULL );

   return false;
}

//---------------------------------------------------------------------------

bool SimObject::registerObject()
{
    AssertFatal( !mFlags.test( Added ), "reigsterObject - Object already registered!");
    mFlags.clear(Deleted | Removed);

    if( mId == 0 )
    {
        mId = Sim::gNextObjectId++;

        char idBuffer[64];
        dSprintf(idBuffer, sizeof(idBuffer), "%d", mId);
        mIdString = StringTable->insert( idBuffer );
    }

   AssertFatal(Sim::gIdDictionary && Sim::gNameDictionary, 
      "SimObject::registerObject - tried to register an object before Sim::init()!");

   Sim::gIdDictionary->insert(this);  

   Sim::gNameDictionary->insert(this);

    // Notify object
   bool ret = onAdd();

   if(!ret)
      unregisterObject();

   AssertFatal(!ret || isProperlyAdded(), "Object did not call SimObject::onAdd()");

    if ( isMethod( "onAdd" ) )
        Con::executef( this, 1, "onAdd" );

   return ret;
}

//---------------------------------------------------------------------------

void SimObject::unregisterObject()
{
    // Sanity!
    AssertISV( getScriptCallbackGuard() == 0, "SimObject::unregisterObject: Object is being unregistered whilst performing a script callback!" );

    if ( isMethod( "onRemove" ) )
        Con::executef( this, 1, "onRemove" );

   mFlags.set(Removed);

   // Notify object first
   onRemove();

   // Clear out any pending notifications before
   // we call our own, just in case they delete
   // something that we have referenced.
   clearAllNotifications();

   // Notify all objects that are waiting for delete
   // messages
   if (getGroup())
      getGroup()->removeObject(this);

   processDeleteNotifies();

   // Do removals from the Sim.
   Sim::gNameDictionary->remove(this);
   Sim::gIdDictionary->remove(this);
   Sim::cancelPendingEvents(this);
}

//---------------------------------------------------------------------------

void SimObject::deleteObject()
{
    // Sanity!
    AssertISV( getScriptCallbackGuard() == 0, "SimObject::deleteObject: Object is being deleted whilst performing a script callback!" );

    AssertFatal(mFlags.test(Added),
        "SimObject::deleteObject: Object not registered.");
    AssertFatal(!isDeleted(),"SimManager::deleteObject: "
        "Object has already been deleted");
    AssertFatal(!isRemoved(),"SimManager::deleteObject: "
        "Object in the process of being removed");
    mFlags.set(Deleted);

   unregisterObject();
   delete this;
}

//---------------------------------------------------------------------------


void SimObject::setId(SimObjectId newId)
{
    if(!mFlags.test(Added))
    {
        mId = newId;
    }
    else
    {
       // get this object out of the id dictionary if it's in it
       Sim::gIdDictionary->remove(this);

        // Free current Id.
        // Assign new one.
        mId = newId ? newId : Sim::gNextObjectId++;
        Sim::gIdDictionary->insert(this);
    }

    char idBuffer[64];
    dSprintf(idBuffer, sizeof(idBuffer), "%d", mId);
    mIdString = StringTable->insert( idBuffer );
}

void SimObject::assignName(const char *name)
{
    if( dStricmp( getClassName(), name ) == 0 )
        Con::errorf( "SimObject::assignName - Assigning name '%s' to instance of object with type '%s'."
        " This can cause namespace linking issues.", getClassName(), name  );

    // Is this name already registered?
    if ( Sim::gNameDictionary->find(name) != NULL )
    {
        // Yes, so error,
        Con::errorf( "SimObject::assignName() - Attempted to set object to name '%s' but it is already assigned to another object.", name );
        return;
    }

    StringTableEntry newName = NULL;

    if (name[0])
        newName = StringTable->insert(name);

    if (mGroup)
        mGroup->nameDictionary.remove(this);

    if (isProperlyAdded())
    {
        // Unlink the old namespaces.
        unlinkNamespaces();

        Sim::gNameDictionary->remove(this);
    }

    objectName = newName;

    if (mGroup)
        mGroup->nameDictionary.insert(this);

    if (isProperlyAdded())
    {
        // Link the new namespaces.
        linkNamespaces();

        Sim::gNameDictionary->insert(this);
    }  
}

//---------------------------------------------------------------------------

bool SimObject::registerObject(U32 id)
{
   setId(id);
   return registerObject();
}

bool SimObject::registerObject(const char *name)
{
   assignName(name);
   return registerObject();
}

bool SimObject::registerObject(const char *name, U32 id)
{
   setId(id);
   assignName(name);
   return registerObject();
}

void SimObject::assignDynamicFieldsFrom(SimObject* parent)
{
   if(parent->mFieldDictionary)
   {
      if( mFieldDictionary == NULL )
         mFieldDictionary = new SimFieldDictionary;
      mFieldDictionary->assignFrom(parent->mFieldDictionary);
   }
}

void SimObject::assignFieldsFrom(SimObject *parent)
{
   // only allow field assigns from objects of the same class:
   if(getClassRep() == parent->getClassRep())
   {
      const AbstractClassRep::FieldList &list = getFieldList();

      // copy out all the fields:
      for(U32 i = 0; i < (U32)list.size(); i++)
      {
         const AbstractClassRep::Field* f = &list[i];
         S32 lastField = f->elementCount - 1;
         for(S32 j = 0; j <= lastField; j++)
         {
             const char* fieldVal = (*f->getDataFn)( this,  Con::getData(f->type, (void *) (((const char *)parent) + f->offset), j, f->table, f->flag));
            //if(fieldVal)
            //   Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            if(fieldVal)
            {
               // code copied from SimObject::setDataField().
               // TODO: paxorr: abstract this into a better setData / getData that considers prot fields.
               FrameTemp<char> buffer(2048);
               FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
               ConsoleBaseType *cbt = ConsoleBaseType::getType( f->type );
               const char* szBuffer = cbt->prepData( fieldVal, buffer, 2048 );
               dMemset( bufferSecure, 0, 2048 );
               dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

               if((*f->setDataFn)( this, bufferSecure ) )
                  Con::setData(f->type, (void *) (((const char *)this) + f->offset), j, 1, &fieldVal, f->table);
            }
         }
      }
   }

   assignDynamicFieldsFrom(parent);
}

bool SimObject::writeField(StringTableEntry fieldname, const char* value)
{
   // Don't write empty fields.
   if (!value || !*value)
      return false;

   // Don't write ParentGroup
   if( fieldname == StringTable->insert("parentGroup") )
      return false;


   return true;
}

void SimObject::writeFields(Stream &stream, U32 tabStop)
{
   const AbstractClassRep::FieldList &list = getFieldList();

   for(U32 i = 0; i < (U32)list.size(); i++)
   {
      const AbstractClassRep::Field* f = &list[i];

      if( f->type == AbstractClassRep::DepricatedFieldType ||
          f->type == AbstractClassRep::StartGroupFieldType ||
          f->type == AbstractClassRep::EndGroupFieldType) continue;

      // Fetch fieldname.
      StringTableEntry fieldName = StringTable->insert( f->pFieldname );

      // Fetch element count.
      const S32 elementCount = f->elementCount;

      // Skip if the field should not be written.
      // For now, we only deal with non-array fields.
      if (  elementCount == 1 &&
            f->writeDataFn != NULL &&
            f->writeDataFn( this, fieldName ) == false )
            continue;

      for(U32 j = 0; S32(j) < elementCount; j++)
      {
         char array[8];
         dSprintf( array, 8, "%d", j );
         const char *val = getDataField(fieldName, array );

         // Make a copy for the field check.
         if (!val)
            continue;

         U32 nBufferSize = dStrlen( val ) + 1;
         FrameTemp<char> valCopy( nBufferSize );
         dStrcpy( (char *)valCopy, val );

         if (!writeField(fieldName, valCopy))
            continue;

         val = valCopy;

         U32 expandedBufferSize = ( nBufferSize  * 2 ) + 32;
         FrameTemp<char> expandedBuffer( expandedBufferSize );
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, expandedBufferSize, "%s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, expandedBufferSize, "%s[%d] = \"", f->pFieldname, j);

         // detect and collapse relative path information
         char fnBuf[1024];
         if (f->type == TypeFilename)
         {
            Con::collapsePath(fnBuf, 1024, val);
            val = fnBuf;
         }

         expandEscape((char*)expandedBuffer + dStrlen(expandedBuffer), val);
         dStrcat(expandedBuffer, "\";\r\n");

         stream.writeTabs(tabStop);
         stream.write(dStrlen(expandedBuffer),expandedBuffer);
      }
   }    
   if(mFieldDictionary && mCanSaveFieldDictionary)
      mFieldDictionary->writeFields(this, stream, tabStop);
}

void SimObject::write(Stream &stream, U32 tabStop, U32 flags)
{
   // Only output selected objects if they want that.
   if((flags & SelectedOnly) && !isSelected())
      return;

   stream.writeTabs(tabStop);
   char buffer[1024];
   dSprintf(buffer, sizeof(buffer), "new %s(%s) {\r\n", getClassName(), getName() ? getName() : "");
   stream.write(dStrlen(buffer), buffer);
   writeFields(stream, tabStop + 1);
   stream.writeTabs(tabStop);
   stream.write(4, "};\r\n");
}

bool SimObject::save(const char* pcFileName, bool bOnlySelected)
{
   static const char *beginMessage = "//--- OBJECT WRITE BEGIN ---";
   static const char *endMessage = "//--- OBJECT WRITE END ---";
   FileStream stream;
   FileObject f;
   f.readMemory(pcFileName);

   // check for flags <selected, ...>
   U32 writeFlags = 0;
   if(bOnlySelected)
      writeFlags |= SimObject::SelectedOnly;

   if(!ResourceManager->openFileForWrite(stream, pcFileName)) 
      return false;

   char docRoot[256];
   char modRoot[256];

   dStrcpy(docRoot, pcFileName);
   char *p = dStrrchr(docRoot, '/');
   if (p) *++p = '\0';
   else  docRoot[0] = '\0';

   dStrcpy(modRoot, pcFileName);
   p = dStrchr(modRoot, '/');
   if (p) *++p = '\0';
   else  modRoot[0] = '\0';

   Con::setVariable("$DocRoot", docRoot);
   Con::setVariable("$ModRoot", modRoot);

   const char *buffer;
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, beginMessage))
         break;
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }
   stream.write(dStrlen(beginMessage), beginMessage);
   stream.write(2, "\r\n");
   write(stream, 0, writeFlags);
   stream.write(dStrlen(endMessage), endMessage);
   stream.write(2, "\r\n");
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      if(!dStrcmp(buffer, endMessage))
         break;
   }
   while(!f.isEOF())
   {
      buffer = (const char *) f.readLine();
      stream.write(dStrlen(buffer), buffer);
      stream.write(2, "\r\n");
   }

   Con::setVariable("$DocRoot", NULL);
   Con::setVariable("$ModRoot", NULL);

   return true;

}

void SimObject::setInternalName(const char* newname)
{
   if(newname)
      mInternalName = StringTable->insert(newname);
}

StringTableEntry SimObject::getInternalName()
{
   return mInternalName;
}

void  SimObject::dumpClassHierarchy()
{
   AbstractClassRep* pRep = getClassRep();
   while(pRep)
   {
      Con::warnf("%s ->", pRep->getClassName());
      pRep  = pRep->getParentClass();
   }
}

const char *SimObject::tabComplete(const char *prevText, S32 baseLen, bool fForward)
{
   return mNameSpace->tabComplete(prevText, baseLen, fForward);
}

//-----------------------------------------------------------------------------

void SimObject::setDataField(StringTableEntry slotName, const char *array, const char *value)
{
   // first search the static fields if enabled
   if(mFlags.test(ModStaticFields))
   {
      const AbstractClassRep::Field *fld = findField(slotName);
      if(fld)
      {
         if( fld->type == AbstractClassRep::DepricatedFieldType ||
            fld->type == AbstractClassRep::StartGroupFieldType ||
            fld->type == AbstractClassRep::EndGroupFieldType) 
            return;

         S32 array1 = array ? dAtoi(array) : 0;

         if(array1 >= 0 && array1 < fld->elementCount && fld->elementCount >= 1)
         {
            // If the set data notify callback returns true, then go ahead and
            // set the data, otherwise, assume the set notify callback has either
            // already set the data, or has deemed that the data should not
            // be set at all.
            FrameTemp<char> buffer(2048);
            FrameTemp<char> bufferSecure(2048); // This buffer is used to make a copy of the data 
            // so that if the prep functions or any other functions use the string stack, the data
            // is not corrupted.

            ConsoleBaseType *cbt = ConsoleBaseType::getType( fld->type );
            AssertFatal( cbt != NULL, "Could not resolve Type Id." );

            const char* szBuffer = cbt->prepData( value, buffer, 2048 );
            dMemset( bufferSecure, 0, 2048 );
            dMemcpy( bufferSecure, szBuffer, dStrlen( szBuffer ) );

            if( (*fld->setDataFn)( this, bufferSecure ) )
               Con::setData(fld->type, (void *) (((const char *)this) + fld->offset), array1, 1, &value, fld->table);
         }

         if(fld->validator)
            fld->validator->validateType(this, (void *) (((const char *)this) + fld->offset));

         onStaticModified( slotName, value );
         return;
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         mFieldDictionary = new SimFieldDictionary;

      if(!array)
         mFieldDictionary->setFieldValue(slotName, value);
      else
      {
         char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         mFieldDictionary->setFieldValue(StringTable->insert(buf), value);
      }
   }
}

//-----------------------------------------------------------------------------

const char *SimObject::getDataField(StringTableEntry slotName, const char *array)
{
   if(mFlags.test(ModStaticFields))
   {
      S32 array1 = array ? dAtoi(array) : -1;
      const AbstractClassRep::Field *fld = findField(slotName);
   
      if(fld)
      {
         if(array1 == -1 && fld->elementCount == 1)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), 0, fld->table, fld->flag) );
         if(array1 >= 0 && array1 < fld->elementCount)
            return (*fld->getDataFn)( this, Con::getData(fld->type, (void *) (((const char *)this) + fld->offset), array1, fld->table, fld->flag) );// + typeSizes[fld.type] * array1));
         return "";
      }
   }

   if(mFlags.test(ModDynamicFields))
   {
      if(!mFieldDictionary)
         return "";

      if(!array) 
      {
         if (const char* val = mFieldDictionary->getFieldValue(slotName))
            return val;
      }
      else
      {
         static char buf[256];
         dStrcpy(buf, slotName);
         dStrcat(buf, array);
         if (const char* val = mFieldDictionary->getFieldValue(StringTable->insert(buf)))
            return val;
      }
   }

   return "";
}

//-----------------------------------------------------------------------------

const char *SimObject::getPrefixedDataField(StringTableEntry fieldName, const char *array)
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field value with NULL field name." );

    // Fetch field value.
    const char* pFieldValue = getDataField( fieldName, array );

    // Return without the prefix if there's no value.
    if ( pFieldValue == NULL || *pFieldValue == 0 )
        return StringTable->EmptyString;

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = getDataFieldPrefix( fieldName );

    // Sanity!
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Calculate a buffer size including prefix.
    const U32 valueBufferSize = dStrlen(fieldPrefix) + dStrlen(pFieldValue) + 1;

    // Fetch a buffer.
    char* pValueBuffer = Con::getReturnBuffer( valueBufferSize );

    // Format the value buffer.
    dSprintf( pValueBuffer, valueBufferSize, "%s%s", fieldPrefix, pFieldValue );

    return pValueBuffer;
}

//-----------------------------------------------------------------------------

void SimObject::setPrefixedDataField(StringTableEntry fieldName, const char *array, const char *value)
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot set object field value with NULL field name." );
    AssertFatal( value != NULL, "Field value cannot be NULL." );

    // Set value without prefix if there's no value.
    if ( *value == 0 )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = getDataFieldPrefix( fieldName );

    // Sanity.
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Do we have a field prefix?
    if ( fieldPrefix == StringTable->EmptyString )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so fetch the length of the field prefix.
    const U32 fieldPrefixLength = dStrlen(fieldPrefix);

    // Yes, so does it start with the object field prefix?
    if ( dStrnicmp( value, fieldPrefix, fieldPrefixLength ) != 0 )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so set the data excluding the prefix.
    setDataField( fieldName, NULL, value + fieldPrefixLength );
}

//-----------------------------------------------------------------------------

const char *SimObject::getPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const S32 fieldType )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field value with NULL field name." );

    // Fetch field value.
    const char* pFieldValue = getDataField( fieldName, array );

    // Sanity.
    AssertFatal( pFieldValue != NULL, "Field value cannot be NULL." );

    // Return the field if no field type is specified.
    if ( fieldType == -1 )
        return pFieldValue;

    // Return without the prefix if there's no value.
    if ( *pFieldValue == 0 )
        return StringTable->EmptyString;

    // Fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( fieldType );

    // Did we find the console base type?
    if ( pConsoleBaseType == NULL )
    {
        // No, so warn.
        Con::warnf("getPrefixedDynamicDataField() - Invalid field type '%d' specified for field '%s' with value '%s'.",
            fieldType, fieldName, pFieldValue );
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = pConsoleBaseType->getTypePrefix();

    // Sanity!
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Calculate a buffer size including prefix.
    const U32 valueBufferSize = dStrlen(fieldPrefix) + dStrlen(pFieldValue) + 1;

    // Fetch a buffer.
    char* pValueBuffer = Con::getReturnBuffer( valueBufferSize );

    // Format the value buffer.
    dSprintf( pValueBuffer, valueBufferSize, "%s%s", fieldPrefix, pFieldValue );

    return pValueBuffer;
}

//-----------------------------------------------------------------------------

void SimObject::setPrefixedDynamicDataField(StringTableEntry fieldName, const char *array, const char *value, const S32 fieldType )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot set object field value with NULL field name." );
    AssertFatal( value != NULL, "Field value cannot be NULL." );

    // Set value without prefix if no field type was specified.
    if ( fieldType == -1 )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( fieldType );

    // Did we find the console base type?
    if ( pConsoleBaseType == NULL )
    {
        // No, so warn.
        Con::warnf("setPrefixedDynamicDataField() - Invalid field type '%d' specified for field '%s' with value '%s'.",
            fieldType, fieldName, value );
    }

    // Set value without prefix if there's no value or we didn't find the console base type.
    if ( *value == 0 || pConsoleBaseType == NULL )
    {
        setDataField( fieldName, NULL, value );
        return;
    }

    // Fetch the field prefix.
    StringTableEntry fieldPrefix = pConsoleBaseType->getTypePrefix();

    // Sanity.
    AssertFatal( fieldPrefix != NULL, "Field prefix cannot be NULL." );

    // Do we have a field prefix?
    if ( fieldPrefix == StringTable->EmptyString )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so fetch the length of the field prefix.
    const U32 fieldPrefixLength = dStrlen(fieldPrefix);

    // Yes, so does it start with the object field prefix?
    if ( dStrnicmp( value, fieldPrefix, fieldPrefixLength ) != 0 )
    {
        // No, so set the data field in the usual way.
        setDataField( fieldName, NULL, value );
        return;
    }

    // Yes, so set the data excluding the prefix.
    setDataField( fieldName, NULL, value + fieldPrefixLength );
}

//-----------------------------------------------------------------------------

StringTableEntry SimObject::getDataFieldPrefix( StringTableEntry fieldName )
{
    // Sanity!
    AssertFatal( fieldName != NULL, "Cannot get field prefix with NULL field name." );

    // Find the field.
    const AbstractClassRep::Field* pField = findField( fieldName );

    // Return nothing if field was not found.
    if ( pField == NULL )
        return StringTable->EmptyString;

    // Yes, so fetch the console base type.
    ConsoleBaseType* pConsoleBaseType = ConsoleBaseType::getType( pField->type );

    // Fetch the type prefix.
    return pConsoleBaseType->getTypePrefix();
}

//-----------------------------------------------------------------------------

U32 SimObject::getDataFieldType( StringTableEntry slotName, const char* array )
{
   const AbstractClassRep::Field* field = findField( slotName );
   if( field )
      return field->type;

   return 0;
}

SimObject::~SimObject()
{
   delete mFieldDictionary;

   AssertFatal(nextNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from dictionary: name %s, id %i",
                  objectName, mId));
   AssertFatal(nextManagerNameObject == (SimObject*)-1,avar(
                  "SimObject::~SimObject:  Not removed from manager dictionary: name %s, id %i",
                  objectName,mId));
   AssertFatal(mFlags.test(Added) == 0, "SimObject::object "
               "missing call to SimObject::onRemove");
}

//---------------------------------------------------------------------------


void SimObject::setLocked( bool b = true )
{
   if (b)
      mFlags.set(Locked);
   else
      mFlags.clear(Locked);
}

void SimObject::setHidden(bool b = true)
{
   if (b)
      mFlags.set(Hidden);
   else
      mFlags.clear(Hidden);
}

//---------------------------------------------------------------------------

bool SimObject::onAdd()
{
   mFlags.set(Added);

   linkNamespaces();

   // onAdd() should return FALSE if there was an error
   return true;
}

void SimObject::onRemove()
{
   mFlags.clear(Added);

   unlinkNamespaces();
}

void SimObject::onGroupAdd()
{
}

void SimObject::onGroupRemove()
{
}

void SimObject::onDeleteNotify(SimObject*)
{
}

void SimObject::onNameChange(const char*)
{
}

void SimObject::onStaticModified(const char* slotName, const char* newValue)
{
}

bool SimObject::processArguments(S32 argc, const char**)
{
   return argc == 0;
}

bool SimObject::isChildOfGroup(SimGroup* pGroup)
{
   if(!pGroup)
      return false;

   //if we *are* the group in question,
   //return true:
   if(pGroup == dynamic_cast<SimGroup*>(this))
      return true;

   SimGroup* temp = mGroup;
   while(temp)
   {
      if(temp == pGroup)
         return true;
      temp = temp->mGroup;
   }

   return false;
}

//---------------------------------------------------------------------------

static Chunker<SimObject::Notify> notifyChunker(128000);
SimObject::Notify *SimObject::mNotifyFreeList = NULL;

SimObject::Notify *SimObject::allocNotify()
{
   if(mNotifyFreeList)
   {
      SimObject::Notify *ret = mNotifyFreeList;
      mNotifyFreeList = ret->next;
      return ret;
   }
   return notifyChunker.alloc();
}

void SimObject::freeNotify(SimObject::Notify* note)
{
   AssertFatal(note->type != SimObject::Notify::Invalid, "Invalid notify");
   note->type = SimObject::Notify::Invalid;
   note->next = mNotifyFreeList;
   mNotifyFreeList = note;
}

//------------------------------------------------------------------------------

SimObject::Notify* SimObject::removeNotify(void *ptr, SimObject::Notify::Type type)
{
   Notify **list = &mNotifyList;
   while(*list)
   {
      if((*list)->ptr == ptr && (*list)->type == type)
      {
         SimObject::Notify *ret = *list;
         *list = ret->next;
         return ret;
      }
      list = &((*list)->next);
   }
   return NULL;
}

void SimObject::deleteNotify(SimObject* obj)
{
   AssertFatal(!obj->isDeleted(),
               "SimManager::deleteNotify: Object is being deleted");
   Notify *note = allocNotify();
   note->ptr = (void *) this;
   note->next = obj->mNotifyList;
   note->type = Notify::DeleteNotify;
   obj->mNotifyList = note;

   note = allocNotify();
   note->ptr = (void *) obj;
   note->next = mNotifyList;
   note->type = Notify::ClearNotify;
   mNotifyList = note;

   //obj->deleteNotifyList.pushBack(this);
   //clearNotifyList.pushBack(obj);
}

void SimObject::registerReference(SimObject **ptr)
{
   Notify *note = allocNotify();
   note->ptr = (void *) ptr;
   note->next = mNotifyList;
   note->type = Notify::ObjectRef;
   mNotifyList = note;
}

void SimObject::unregisterReference(SimObject **ptr)
{
   Notify *note = removeNotify((void *) ptr, Notify::ObjectRef);
   if(note)
      freeNotify(note);
}

void SimObject::clearNotify(SimObject* obj)
{
   Notify *note = obj->removeNotify((void *) this, Notify::DeleteNotify);
   if(note)
      freeNotify(note);

   note = removeNotify((void *) obj, Notify::ClearNotify);
   if(note)
      freeNotify(note);
}

void SimObject::processDeleteNotifies()
{
   // clear out any delete notifies and
   // object refs.

   while(mNotifyList)
   {
      Notify *note = mNotifyList;
      mNotifyList = note->next;

      AssertFatal(note->type != Notify::ClearNotify, "Clear notes should be all gone.");

      if(note->type == Notify::DeleteNotify)
      {
         SimObject *obj = (SimObject *) note->ptr;
         Notify *cnote = obj->removeNotify((void *)this, Notify::ClearNotify);
         obj->onDeleteNotify(this);
         freeNotify(cnote);
      }
      else
      {
         // it must be an object ref - a pointer refs this object
         *((SimObject **) note->ptr) = NULL;
      }
      freeNotify(note);
   }
}

void SimObject::clearAllNotifications()
{
   for(Notify **cnote = &mNotifyList; *cnote; )
   {
      Notify *temp = *cnote;
      if(temp->type == Notify::ClearNotify)
      {
         *cnote = temp->next;
         Notify *note = ((SimObject *) temp->ptr)->removeNotify((void *) this, Notify::DeleteNotify);
         freeNotify(temp);
         freeNotify(note);
      }
      else
         cnote = &(temp->next);
   }
}

//---------------------------------------------------------------------------

void SimObject::initPersistFields()
{
   Parent::initPersistFields();

   
   addGroup("SimBase");
   addProtectedField("name", TypeName, Offset(objectName, SimObject), &setProtectedName, &defaultProtectedGetFn, "Name for the object.");
   addField("canSaveDynamicFields",    TypeBool,         Offset(mCanSaveFieldDictionary, SimObject), &writeCanSaveDynamicFields, "");
   addField("internalName",            TypeString,       Offset(mInternalName, SimObject), &writeInternalName, "");   
   addProtectedField("parentGroup",    TypeSimObjectPtr, Offset(mGroup, SimObject), &setParentGroup, &defaultProtectedGetFn, &writeParentGroup, "Group hierarchy parent of the object." );
   endGroup("SimBase");

   // Namespace Linking.
   addGroup("Namespace Linking");
   addProtectedField("superclass", TypeString, Offset(mSuperClassName, SimObject), &setSuperClass, &defaultProtectedGetFn, &writeSuperclass, "Script Class of object.");
   addProtectedField("class",      TypeString, Offset(mClassName,      SimObject), &setClass,      &defaultProtectedGetFn, &writeClass, "Script SuperClass of object.");
   endGroup("Namespace Linking");

   addGroup("Editing");

   addProtectedField("hidden", TypeBool, NULL, &_setHidden, &_getHidden, &_writeHidden, "Whether the object is visible.");
   addProtectedField("locked", TypeBool, NULL, &_setLocked, &_getLocked, &_writeLocked, "Whether the object can be edited.");

   endGroup("Editing");
}

//-----------------------------------------------------------------------------

bool SimObject::setProtectedName(void *obj, const char *data)
{
   if (disableNameChanging)
      return false;
   SimObject *object = static_cast<SimObject*>(obj);

   if (object->isProperlyAdded())
      object->assignName(data);

   // always return false because we assign the name here
   return false;
}


//-----------------------------------------------------------------------------

SimObject* SimObject::clone( const bool copyDynamicFields )
{
    // Craete cloned object.
    SimObject* pCloneObject = dynamic_cast<SimObject*>( ConsoleObject::create(getClassName()) );
    if (!pCloneObject)
    {
        Con::errorf("SimObject::clone() - Unable to create cloned object.");
        return NULL;
    }

    // Register object.
    if ( !pCloneObject->registerObject() )
    {
        Con::warnf("SimObject::clone() - Unable to register cloned object.");
        delete pCloneObject;
        return NULL;
    }

    // Copy object.
    copyTo( pCloneObject );

    // Copy over dynamic fields if requested.
    if ( copyDynamicFields )
        pCloneObject->assignDynamicFieldsFrom( this );

    return pCloneObject;
}


//-----------------------------------------------------------------------------

void SimObject::copyTo(SimObject* object)
{
   object->mClassName = mClassName;
   object->mSuperClassName = mSuperClassName;
   object->mNameSpace = NULL;
   object->linkNamespaces();
}

//-----------------------------------------------------------------------------

bool SimObject::setParentGroup(void* obj, const char* data)
{
   SimGroup *parent = NULL;
   SimObject *object = static_cast<SimObject*>(obj);

   if(Sim::findObject(data, parent))
      parent->addObject(object);
   
   // always return false, because we've set mGroup when we called addObject
   return false;
}


bool SimObject::addToSet(SimObjectId spid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(spid);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectId does not refer to a set object");
      if (sp)
      {
         sp->addObject(this);
         return true;
      }
   }
   return false;
}

bool SimObject::addToSet(const char *ObjectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimObject* ptr = Sim::findObject(ObjectName);
   if (ptr) 
   {
      SimSet* sp = dynamic_cast<SimSet*>(ptr);
      AssertFatal(sp != 0,
                  "SimObject::addToSet: "
                  "ObjectName does not refer to a set object");
      if (sp)
      {
         sp->addObject(this);
         return true;
      }
   }
   return false;
}

bool SimObject::removeFromSet(SimObjectId sid)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(sid, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

bool SimObject::removeFromSet(const char *objectName)
{
   if (mFlags.test(Added) == false)
      return false;

   SimSet *set;
   if(Sim::findObject(objectName, set))
   {
      set->removeObject(this);
      return true;
   }
   return false;
}

void SimObject::inspectPreApply()
{
}

void SimObject::inspectPostApply()
{
}

//-----------------------------------------------------------------------------

void SimObject::linkNamespaces()
{
    // Don't link if we already have a namespace linkage in place.
    AssertWarn(mNameSpace == NULL, "SimObject::linkNamespaces -- Namespace linkage already in place");

    if (mNameSpace)
        return;

    // Start with the C++ Class namespace.
    StringTableEntry parent = StringTable->insert( getClassName() );

    // Link SuperClass to C++ Class.
    if ( mSuperClassName && mSuperClassName[0] )
    {
        if ( Con::linkNamespaces(parent, mSuperClassName) )
            parent = mSuperClassName;
        else
            mSuperClassName = NULL;
    }

    // Link Class to SuperClass or Class to C++ Class.
    if ( mClassName && mClassName[0] )
    {
        if ( Con::linkNamespaces(parent, mClassName) )
            parent = mClassName;
        else
            mClassName = NULL;
    }

    // Get the object's name.
    StringTableEntry objectName = getName();

    // Link Object Name to Class/SuperClass/C++ Class.
    if ( objectName && objectName[0] )
    {
        if ( Con::linkNamespaces(parent, objectName) )
            parent = objectName;
    }

    // Store our namespace.
    mNameSpace = Con::lookupNamespace(parent);
}

//-----------------------------------------------------------------------------

void SimObject::unlinkNamespaces()
{
    // Stop if there is no assigned namespace.
    if (!mNameSpace)
        return;

    // Get the object's name.
    StringTableEntry child = getName();

    // Unlink any possible namespace combination.
    if ( child && child[0] )
    {
        // Object Name/Class
        if ( mClassName && mClassName[0] )
        {
            if( Con::unlinkNamespaces(mClassName, child) )
                child = mClassName;
        }

        // Object Name/SuperClass or Class/SuperClass
        if ( mSuperClassName && mSuperClassName[0] )
        {
            if ( Con::unlinkNamespaces(mSuperClassName, child) )
                child = mSuperClassName;
        }

        // Object Name/C++ Class or SuperClass/C++ Class
        Con::unlinkNamespaces(getClassName(), child);
    }
    else
    {
        // No Object Name, so get the Class namespace.
        child = mClassName;

        // Unlink any possible namespace combination.
        if ( child && child[0] )
        {
            // Class/SuperClass
            if ( mSuperClassName && mSuperClassName[0] )
            {
                if ( Con::unlinkNamespaces(mSuperClassName, child) )
                    child = mSuperClassName;
            }

            // Class/C++ Class or SuperClass/C++ Class
            Con::unlinkNamespaces(getClassName(), child);
        }
        else
        {
            // SuperClass/C++ Class
            if ( mSuperClassName && mSuperClassName[0] )
                Con::unlinkNamespaces(getClassName(), mSuperClassName);
        }
    }

    // Reset the namespace.
    mNameSpace = NULL;
}

//-----------------------------------------------------------------------------

void SimObject::setClassNamespace( const char* classNamespace )
{
    StringTableEntry oldClass = mClassName;
    StringTableEntry newClass = StringTable->insert(classNamespace);

    // Skip if no change.
    if (oldClass == newClass)
        return;

    // Unlink the old namespaces.
    if ( isProperlyAdded() )
        unlinkNamespaces();

    // Assign the new class namespace.
    mClassName = newClass;

    // Link the new namespaces.
    if ( isProperlyAdded() )
        linkNamespaces();
}

//-----------------------------------------------------------------------------

void SimObject::setSuperClassNamespace( const char* superClassNamespace )
{
    StringTableEntry oldSuperClass = mSuperClassName;
    StringTableEntry newSuperClass = StringTable->insert(superClassNamespace);

    // Skip if no change.
    if (oldSuperClass == newSuperClass)
        return;

    // Unlink the old namespaces.
    if ( isProperlyAdded() )
        unlinkNamespaces();

    // Assign the new SuperClass namespace.
    mSuperClassName = newSuperClass;

    // Link the new namespaces.
    if ( isProperlyAdded() )
        linkNamespaces();
}

//-----------------------------------------------------------------------------

void SimObject::addListener(std::string objID)
{
   for (auto iter = mListenerList.begin(); iter != mListenerList.end(); ++iter)
   {
      if (iter->objID == objID)
      {
         iter->doomed = false;
         return;
      }
   }

   OtoListener listener = OtoListener();
   listener.objID = objID;
   listener.doomed = false;
   mListenerList.push_back(listener);
}

//-----------------------------------------------------------------------------

void SimObject::removeListener(std::string objID)
{
   for (auto iter = mListenerList.begin(); iter != mListenerList.end(); ++iter)
   {
      if (iter->objID == objID)
      {
         iter->doomed = true;
      }
   }
   
   if (!bIsEventRaised)
   {
      mListenerList.erase(std::remove_if(mListenerList.begin(), mListenerList.end(), [](OtoListener listener){ return listener.doomed; }), mListenerList.end());
   }
}

//-----------------------------------------------------------------------------

void SimObject::removeAllListeners()
{
   if (bIsEventRaised)
   {
      for (auto iter = mListenerList.begin(); iter != mListenerList.end(); ++iter)
      {
         iter->doomed = true;
      }
   }
   else
   {
      mListenerList.clear();
   }
}

//-----------------------------------------------------------------------------

void SimObject::postEvent(std::string eventName, std::string data)
{
   std::string onEvent = "on" + eventName;
   if (mListenerList.empty())
   {
      return;
   }

   if (bIsEventRaised)
   {
      Con::warnf("SimObject::postEvent() - To avoid circular events, you cannot raise the event '%s' until a previous event has finished.", eventName.c_str());
      return;
   }

   bIsEventRaised = true;
   for (auto iter = mListenerList.begin(); iter != mListenerList.end(); ++iter)
   {
      SimObject* pSimObject = dynamic_cast<SimObject*>(Sim::findObject(iter->objID.c_str()));

      // Did we find the object?
      if (pSimObject)
      {
         if (!iter->doomed && pSimObject->isMethod(onEvent.c_str()))
         {
            Con::executef(pSimObject, 3, onEvent.c_str(), data.c_str());
         }
      }
      else
      {
         //it must have been deleted
         iter->doomed = true;
      }
   }
   bIsEventRaised = false;

   //now to remove all doomed listeners
   mListenerList.erase(std::remove_if(mListenerList.begin(), mListenerList.end(), [](OtoListener listener){ return listener.doomed; }), mListenerList.end());
}

//-----------------------------------------------------------------------------

static S32 QSORT_CALLBACK compareFields(const void* a,const void* b)
{
   const AbstractClassRep::Field* fa = *((const AbstractClassRep::Field**)a);
   const AbstractClassRep::Field* fb = *((const AbstractClassRep::Field**)b);

   return dStricmp(fa->pFieldname, fb->pFieldname);
}

void SimObject::dump()
{
   const AbstractClassRep::FieldList &list = getFieldList();
   char expandedBuffer[4096];

   Con::printf("Static Fields:");
   Vector<const AbstractClassRep::Field *> flist(__FILE__, __LINE__);

   for(U32 i = 0; i < (U32)list.size(); i++)
      flist.push_back(&list[i]);

   dQsort(flist.address(),flist.size(),sizeof(AbstractClassRep::Field *),compareFields);

   for(Vector<const AbstractClassRep::Field *>::iterator itr = flist.begin(); itr != flist.end(); itr++)
   {
      const AbstractClassRep::Field* f = *itr;
      if( f->type == AbstractClassRep::DepricatedFieldType ||
          f->type == AbstractClassRep::StartGroupFieldType ||
          f->type == AbstractClassRep::EndGroupFieldType) continue;

      for(U32 j = 0; S32(j) < f->elementCount; j++)
      {
         // [neo, 07/05/2007 - #3000]
         // Some objects use dummy vars and projected fields so make sure we call the get functions 
         //const char *val = Con::getData(f->type, (void *) (((const char *)object) + f->offset), j, f->table, f->flag);                          
         const char *val = (*f->getDataFn)( this, Con::getData(f->type, (void *) (((const char *)this) + f->offset), j, f->table, f->flag) );// + typeSizes[fld.type] * array1));

         if(!val /*|| !*val*/)
            continue;
         if(f->elementCount == 1)
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s = \"", f->pFieldname);
         else
            dSprintf(expandedBuffer, sizeof(expandedBuffer), "  %s[%d] = \"", f->pFieldname, j);
         expandEscape(expandedBuffer + dStrlen(expandedBuffer), val);
         Con::printf("%s\"", expandedBuffer);
      }
   }

   Con::printf("Dynamic Fields:");
   if(getFieldDictionary())
      getFieldDictionary()->printFields(this);

   Con::printf("Methods:");
   Namespace *ns = getNamespace();
   Vector<Namespace::Entry *> vec(__FILE__, __LINE__);

   if(ns)
      ns->getEntryList(&vec);

   for(Vector<Namespace::Entry *>::iterator j = vec.begin(); j != vec.end(); j++)
      Con::printf("  %s() - %s", (*j)->mFunctionName, (*j)->mUsage ? (*j)->mUsage : "");

}
