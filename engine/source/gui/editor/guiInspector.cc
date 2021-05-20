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
#include "gui/editor/guiInspector.h"
#include "gui/buttons/guiButtonCtrl.h"
#include "memory/frameAllocator.h"

//////////////////////////////////////////////////////////////////////////
// GuiInspector
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspector);

GuiInspector::GuiInspector()
{
   mGroups.clear();
   mTarget = NULL;
   setField("ChildSpacing", "4");
}


GuiInspector::~GuiInspector()
{
   clearGroups();
}

bool GuiInspector::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   return true;
}

//////////////////////////////////////////////////////////////////////////
// Handle Parent Sizing (We constrain ourself to our parents width)
//////////////////////////////////////////////////////////////////////////
void GuiInspector::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   GuiControl *parent = getParent();
   if( parent && dynamic_cast<GuiScrollCtrl*>(parent) != NULL )
   {
      GuiScrollCtrl *scroll = dynamic_cast<GuiScrollCtrl*>(parent);
      setWidth( ( newParentExtent.x - ( scroll->scrollBarThickness() + 4  ) ) );
   }
   else
      Parent::parentResized(oldParentExtent,newParentExtent);
}

bool GuiInspector::findExistentGroup( StringTableEntry groupName )
{
   // If we have no groups, it couldn't possibly exist
   if( mGroups.empty() )
      return false;

   // Attempt to find it in the group list
   Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();

   for( ; i != mGroups.end(); i++ )
   {
      if( dStricmp( (*i)->getGroupName(), groupName ) == 0 )
         return true;
   }

   return false;
}

void GuiInspector::clearGroups()
{
   // If we're clearing the groups, we want to clear our target too.
   mTarget = NULL;

   // If we have no groups, there's nothing to clear!
   if( mGroups.empty() )
      return;

   // Attempt to find it in the group list
   Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();

   for( ; i != mGroups.end(); i++ )
      if( (*i)->isProperlyAdded() )
         (*i)->deleteObject();

   mGroups.clear();
}

void GuiInspector::inspectObject( SimObject *object )
{  
   GuiCanvas  *guiCanvas = getRoot();
   if( !guiCanvas )
      return;

   SimObjectPtr<GuiControl> currResponder = guiCanvas->getFirstResponder();

   // If our target is the same as our current target, just update the groups.
   if( mTarget == object )
   {
      Vector<GuiInspectorGroup*>::iterator i = mGroups.begin();
      for ( ; i != mGroups.end(); i++ )
         (*i)->inspectGroup();

      // Don't steal first responder
      if( !currResponder.isNull() )
         guiCanvas->setFirstResponder( currResponder );

      return;
   }

   // Clear our current groups
   clearGroups();

   // Set Target
   mTarget = object;

   // Always create the 'general' group (for un-grouped fields)
   GuiInspectorGroup* general = new GuiInspectorGroup( mTarget, "General", this );
   if( general != NULL )
   {
      general->registerObject();
      mGroups.push_back( general );
      general->setPosition(Point2I(10, 0));
      general->setExtent(Point2I((getExtent().x - 20), 20));
      addObject( general );
      
   }

   // Grab this objects field list
   AbstractClassRep::FieldList &fieldList = mTarget->getModifiableFieldList();
   AbstractClassRep::FieldList::iterator itr;

   // Iterate through, identifying the groups and create necessary GuiInspectorGroups
   for(itr = fieldList.begin(); itr != fieldList.end(); itr++)
   {
      if(itr->type == AbstractClassRep::StartGroupFieldType && !findExistentGroup( itr->pGroupname ) )
      {
         GuiInspectorGroup *group = new GuiInspectorGroup( mTarget, itr->pGroupname, this );
         if( group != NULL )
         {
            group->registerObject();
            mGroups.push_back( group );
            group->setPosition(Point2I(10, 0));
            group->setExtent(Point2I((getExtent().x - 20), 20));
            addObject( group );
         }            
      }
   }

   // Deal with dynamic fields
   GuiInspectorGroup *dynGroup = new GuiInspectorDynamicGroup( mTarget, "Dynamic Fields", this);
   if( dynGroup != NULL )
   {
      dynGroup->registerObject();
      mGroups.push_back( dynGroup );
      dynGroup->setPosition(Point2I(10, 0));
      dynGroup->setExtent(Point2I((getExtent().x - 20), 20));
      addObject( dynGroup );
      
   }

   // If the general group is still empty at this point, kill it.
   for(S32 i=0; i<mGroups.size(); i++)
   {
      if(mGroups[i] == general && general->mStack->size() == 0)
      {
         mGroups.erase(i);
         general->deleteObject();
         resize(getPosition(), getExtent());
      }
   }

   // Don't steal first responder
   if( !currResponder.isNull() )
      guiCanvas->setFirstResponder( currResponder );

}

ConsoleMethod( GuiInspector, inspect, void, 3, 3, "(obj) Goes through the object's fields and autogenerates editor boxes\n"
              "@return No return value.")
{
   SimObject * target = Sim::findObject(argv[2]);
   if(!target)
   {
      if(dAtoi(argv[2]) > 0)
         Con::warnf("%s::inspect(): invalid object: %s", argv[0], argv[2]);
      
      object->clearGroups();
      return;
   }

   object->inspectObject(target);
}


ConsoleMethod( GuiInspector, getInspectObject, const char*, 2, 2, "() - Returns currently inspected object\n"
              "@return The Object's ID as a string.")
{
   SimObject *pSimObject = object->getInspectObject();
   if( pSimObject != NULL )
      return pSimObject->getIdString();
   
   return "";
}

void GuiInspector::setName( const char* newName )
{
   if( mTarget == NULL )
      return;

   // Only assign a new name if we provide one
   mTarget->assignName(newName);

}

ConsoleMethod( GuiInspector, setName, void, 3, 3, "(NewObjectName) Set object name.\n"
              "@return No return value.")
{
   object->setName(argv[2]);
}


//////////////////////////////////////////////////////////////////////////
// GuiInspectorField
//////////////////////////////////////////////////////////////////////////
// The GuiInspectorField control is a representation of a single abstract
// field for a given ConsoleObject derived object.  It handles creation
// getting and setting of it's fields data and editing control.  
//
// Creation of custom edit controls is done through this class and is
// dependent upon the dynamic console type, which may be defined to be
// custom for different types.
//
// Note : GuiInspectorField controls must have a GuiInspectorGroup as their
//        parent.  
IMPLEMENT_CONOBJECT(GuiInspectorField);

// Caption width is in percentage of total width
S32 GuiInspectorField::smCaptionWidth = 50;

GuiInspectorField::GuiInspectorField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, AbstractClassRep::Field* field )
{
   if( field != NULL )
      mCaption    = StringTable->insert( field->pFieldname );
   else
      mCaption    = StringTable->EmptyString;

   mParent     = parent;
   mTarget     = target;
   mField      = field;
   mCanSave    = false;
   mFieldArrayIndex = NULL;
}

GuiInspectorField::GuiInspectorField()
{
   mCaption       = StringTable->EmptyString;
   mParent        = NULL;
   mTarget        = NULL;
   mField         = NULL;
   mFieldArrayIndex = NULL;
   mCanSave       = false;
}

GuiInspectorField::~GuiInspectorField()
{
}

//////////////////////////////////////////////////////////////////////////
// Get/Set Data Functions
//////////////////////////////////////////////////////////////////////////
void GuiInspectorField::setData( const char* data )
{
   if( mField == NULL || mTarget == NULL )
      return;

   mTarget->inspectPreApply();

   mTarget->setDataField( mField->pFieldname, mFieldArrayIndex, data );

   // Force our edit to update
   updateValue( data );

   mTarget->inspectPostApply();
}

const char* GuiInspectorField::getData()
{
   if( mField == NULL || mTarget == NULL )
      return "";

   return mTarget->getDataField( mField->pFieldname, mFieldArrayIndex );
}

void GuiInspectorField::setInspectorField( AbstractClassRep::Field *field, const char*arrayIndex ) 
{
   mField = field; 

   if( arrayIndex != NULL )
   {
      mFieldArrayIndex = StringTable->insert( arrayIndex );

      S32 frameTempSize = dStrlen( field->pFieldname ) + 32;
      FrameTemp<char> valCopy( frameTempSize );
      dSprintf( (char *)valCopy, frameTempSize, "%s%s", field->pFieldname, arrayIndex );

      mCaption = StringTable->insert( valCopy ); 
   }
   else
      mCaption = StringTable->insert( field->pFieldname );
}


StringTableEntry GuiInspectorField::getFieldName() 
{ 
   // Sanity
   if ( mField == NULL )
      return StringTable->EmptyString;

   // Array element?
   if( mFieldArrayIndex != NULL )
   {
      S32 frameTempSize = dStrlen( mField->pFieldname ) + 32;
      FrameTemp<char> valCopy( frameTempSize );
      dSprintf( (char *)valCopy, frameTempSize, "%s%s:", mField->pFieldname, mFieldArrayIndex );

      // Return formatted element
      return StringTable->insert( valCopy );
   }

   // Plain ole field name.
   return mField->pFieldname; 
};
//////////////////////////////////////////////////////////////////////////
// Overrideables for custom edit fields
//////////////////////////////////////////////////////////////////////////
GuiControl* GuiInspectorField::constructEditControl()
{
   GuiControl* retCtrl = new GuiTextEditCtrl();
   
   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiTextEditProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(), retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );
   retCtrl->setExtent(Point2I((getExtent().x / 2) - 40, 30));

   return retCtrl;
}

void GuiInspectorField::registerEditControl( GuiControl *ctrl )
{
   if(!mTarget)
      return;
        
   char szName[512];
   dSprintf( szName, 512, "IE_%s_%d_%s_Field", ctrl->getClassName(), mTarget->getId(),mCaption);

   // Register the object
   ctrl->registerObject( szName );
}

void GuiInspectorField::onRender(Point2I offset, const RectI &updateRect)
{

   Parent::onRender( offset, updateRect );
}

bool GuiInspectorField::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   if( !mTarget )
      return false;

   mEdit = constructEditControl();

   if( mEdit == NULL )
      return false;

   GuiControl* capCtrl = new GuiControl();
   capCtrl->setField("position", "0 0");
   capCtrl->setExtent(Point2I((getExtent().x / 2) - 10, 30));
   capCtrl->setField("profile", "GuiTextProfile");
   capCtrl->setField("text", mCaption);

   mEdit->setPosition(Point2I((getExtent().x / 2), 0));
   addObject(capCtrl);
   setExtent(Point2I(getExtent().x, (mEdit->getExtent().y) + 10));
   // Add our edit as a child
   addObject( mEdit );

   //
   setField( "profile", "GuiDefaultProfile" );


   // Force our editField to set it's value
   updateValue( getData() );

   return true;
}

void GuiInspectorField::updateValue( const char* newValue )
{
   GuiTextEditCtrl *ctrl = dynamic_cast<GuiTextEditCtrl*>( mEdit );
   if( ctrl != NULL )
      ctrl->setText( newValue );
}

ConsoleMethod( GuiInspectorField, apply, void, 3,3, "(newValue) Applies the given value to the field\n"
              "@return No return value." )
{
   object->setData( argv[2] );
}

void GuiInspectorField::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   
   Parent::resize(newPosition, newExtent);

}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorGroup
//////////////////////////////////////////////////////////////////////////
//
// The GuiInspectorGroup control is a helper control that the inspector
// makes use of which houses a collapsible pane type control for separating
// inspected objects fields into groups.  The content of the inspector is 
// made up of zero or more GuiInspectorGroup controls inside of a GuiStackControl
//
//
//
IMPLEMENT_CONOBJECT(GuiInspectorGroup);

GuiInspectorGroup::GuiInspectorGroup()
{

   mChildren.clear();

   mTarget              = NULL;
   mParent              = NULL;
   mCanSave             = false;

   // Make sure we receive our ticks.
   setProcessTicks();
}

GuiInspectorGroup::GuiInspectorGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> parent )
{

   mChildren.clear();

   mText                = StringTable->insert(groupName);
   mTarget              = target;
   mParent              = parent;
   mCanSave             = false;
}

GuiInspectorGroup::~GuiInspectorGroup()
{
   if( !mChildren.empty() )
   {
      Vector<GuiInspectorField*>::iterator i = mChildren.begin();
      for( ; i != mChildren.end(); i++ );
         
   } 
}

//////////////////////////////////////////////////////////////////////////
// Scene Events
//////////////////////////////////////////////////////////////////////////
bool GuiInspectorGroup::onAdd()
{
   setField( "profile", "GuiPanelProfile" );

   if( !Parent::onAdd() )
      return false;

   // Create our inner controls. Allow subclasses to provide other content.
   if(!createContent())
      return false;

   inspectGroup();

   return true;
}

bool GuiInspectorGroup::createContent()
{
   // Create our field stack control
   mStack = new GuiChainCtrl();
   if( !mStack )
      return false;

   setWidth(mParent->getExtent().x);

   // Prefer GuiTransperantProfile for the stack.
   mStack->setField( "profile", "GuiDefaultProfile" );
   mStack->registerObject();
   mStack->setExtent(Point2I(getExtent().x - 20, 10));
   mStack->setField("position", "0 30");
   addObject( mStack );
   

   return true;
}

//////////////////////////////////////////////////////////////////////////
// Control Sizing Animation Functions
//////////////////////////////////////////////////////////////////////////
void GuiInspectorGroup::animateToContents()
{
   setExpanded(0);
   setExpanded(1);
}

GuiInspectorField* GuiInspectorGroup::constructField( S32 fieldType )
{
   ConsoleBaseType *cbt = ConsoleBaseType::getType(fieldType);
   AssertFatal(cbt, "GuiInspectorGroup::constructField - could not resolve field type!");

   // Alright, is it a datablock?
   if(cbt->isDatablock())
   {
      // This is fairly straightforward to deal with.
      GuiInspectorDatablockField *dbFieldClass = new GuiInspectorDatablockField( cbt->getTypeClassName() );
      if( dbFieldClass != NULL )
      {
         // return our new datablock field with correct datablock type enumeration info
         return dbFieldClass;
      }
   }

   // Nope, not a datablock. So maybe it has a valid inspector field override we can use?
   if(!cbt->getInspectorFieldType())
      // Nothing, so bail.
      return NULL;

   // Otherwise try to make it!
   ConsoleObject *co = create(cbt->getInspectorFieldType());
   GuiInspectorField *gif = dynamic_cast<GuiInspectorField*>(co);

   if(!gif)
   {
      // Wasn't appropriate type, bail.
      delete co;
      return NULL;
   }

   return gif;
}

GuiInspectorField *GuiInspectorGroup::findField( StringTableEntry fieldName )
{
   // If we don't have any field children we can't very well find one then can we?
   if( mChildren.empty() )
      return NULL;

   Vector<GuiInspectorField*>::iterator i = mChildren.begin();

   for( ; i != mChildren.end(); i++ )
   {
      if( (*i)->getFieldName() != NULL && dStricmp( (*i)->getFieldName(), fieldName ) == 0 )
         return (*i);
   }

   return NULL;
}

bool GuiInspectorGroup::inspectGroup()
{
   // We can't inspect a group without a target!
   if( !mTarget )
      return false;

   // to prevent crazy resizing, we'll just freeze our stack for a sec..
   //mStack->freeze(true);

   bool bNoGroup = false;

   // Un-grouped fields are all sorted into the 'general' group
   if ( dStricmp( mText, "General" ) == 0 )
      bNoGroup = true;

   AbstractClassRep::FieldList &fieldList = mTarget->getModifiableFieldList();
   AbstractClassRep::FieldList::iterator itr;

   bool bGrabItems = false;
   bool bNewItems = false;

   for(itr = fieldList.begin(); itr != fieldList.end(); itr++)
   {
      if( itr->type == AbstractClassRep::StartGroupFieldType )
      {
         // If we're dealing with general fields, always set grabItems to true (to skip them)
         if( bNoGroup == true )
            bGrabItems = true;
         else if( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mText ) == 0 )
            bGrabItems = true;
         continue;
      }
      else if ( itr->type == AbstractClassRep::EndGroupFieldType )
      {
         // If we're dealing with general fields, always set grabItems to false (to grab them)
         if( bNoGroup == true )
            bGrabItems = false;
         else if( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mText ) == 0 )
            bGrabItems = false;
         continue;
      }

      if( ( bGrabItems == true || ( bNoGroup == true && bGrabItems == false ) ) && itr->type != AbstractClassRep::DepricatedFieldType )
      {
         if( bNoGroup == true && bGrabItems == true )
            continue; 
           // This is weird, but it should work for now. - JDD
           // We are going to check to see if this item is an array
           // if so, we're going to construct a field for each array element
         if( itr->elementCount > 1 )
         {
            for(S32 nI = 0; nI < itr->elementCount; nI++)
            {
               FrameTemp<char> intToStr( 64 );
               dSprintf( intToStr, 64, "%d", nI );

               const char *val = mTarget->getDataField( itr->pFieldname, intToStr );
               if (!val)
                  val = StringTable->EmptyString;


               // Copy Val and construct proper ValueName[nI] format 
               //      which is "ValueName0" for index 0, etc.
               S32 frameTempSize = dStrlen( val ) + 32;
               FrameTemp<char> valCopy( frameTempSize );
               dSprintf( (char *)valCopy, frameTempSize, "%s%d", itr->pFieldname, nI );

               // If the field already exists, just update it
               GuiInspectorField *field = findField( valCopy );
               if( field != NULL )
               {
                  field->updateValue( field->getData() );
                  continue;
               }

               bNewItems = true;

               field = constructField( itr->type );
               if( field == NULL )
               {
                  field = new GuiInspectorField( this, mTarget, itr );
                  field->setExtent(Point2I(this->getExtent().x,30));
                  field->setInspectorField( itr, intToStr );
               }
               else
               {
                  field->setExtent(Point2I(this->getExtent().x, 30));
                  field->setTarget( mTarget );
                  field->setParent( this );
                  field->setInspectorField( itr, intToStr );
               }
               field->registerObject();
               mChildren.push_back( field );
               mStack->addObject( field );
            }
         }
         else
         {
            // If the field already exists, just update it
            GuiInspectorField *field = findField( itr->pFieldname );
            if( field != NULL )
            {
               field->updateValue( field->getData() );
               continue;
            }

            bNewItems = true;

            field = constructField( itr->type );
            if( field == NULL )
               field = new GuiInspectorField( this, mTarget, itr );
            else
            {
               field->setTarget( mTarget );
               field->setParent( this );
               field->setInspectorField( itr );
            }
            field->setExtent(Point2I(this->getExtent().x, 30));
            field->registerObject();
            mChildren.push_back( field );
            mStack->addObject( field );

         }       
      }
   }

   // If we've no new items, there's no need to resize anything!
   if( bNewItems == false && !mChildren.empty() )
      return true;

   //sizeToContents();

   setUpdate();

   return true;
}


IMPLEMENT_CONOBJECT(GuiInspectorDynamicGroup);

//////////////////////////////////////////////////////////////////////////
// GuiInspectorDynamicGroup - add custom controls
//////////////////////////////////////////////////////////////////////////
bool GuiInspectorDynamicGroup::createContent()
{
   if(!Parent::createContent())
      return false;

   // add a button that lets us add new dynamic fields.
   GuiButtonCtrl* addFieldBtn = new GuiButtonCtrl();
   {
      SimObject* profilePtr = Sim::findObject("EditorButton");
      if( profilePtr != NULL )
         addFieldBtn->setControlProfile( dynamic_cast<GuiControlProfile*>(profilePtr) );

      char commandBuf[64];
      dSprintf(commandBuf, 64, "%d.addDynamicField();", this->getId());
      addFieldBtn->setField("profile", "GuiButtonDynProfile");
      addFieldBtn->setField("command", commandBuf);
      addFieldBtn->setField("text", "+");
      addFieldBtn->setExtent(Point2I(30, 30));
      addFieldBtn->registerObject("zAddButton");
   }

   // encapsulate the button in a dummy control.
   GuiControl* shell = new GuiControl();
   shell->setField( "profile", "GuiTextProfile" );
   shell->registerObject();
   shell->setField("text", "Add Field");

   shell->setExtent(Point2I(getExtent().x - 30,30 + 10));
   addFieldBtn->setPosition(Point2I((shell->getExtent().x) - 30, 0));
   shell->addObject(addFieldBtn);
   // save off the shell control, so we can push it to the bottom of the stack in inspectGroup()
   mAddCtrl = shell;
   mStack->addObject(shell);

   return true;
}

static S32 QSORT_CALLBACK compareEntries(const void* a,const void* b)
{
   SimFieldDictionary::Entry *fa = *((SimFieldDictionary::Entry **)a);
   SimFieldDictionary::Entry *fb = *((SimFieldDictionary::Entry **)b);
   return dStricmp(fa->slotName, fb->slotName);
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorDynamicGroup - inspectGroup override
//////////////////////////////////////////////////////////////////////////
bool GuiInspectorDynamicGroup::inspectGroup()
{
   // We can't inspect a group without a target!
   if( !mTarget )
      return false;

   // Clearing the fields and recreating them will more than likely be more
   // efficient than looking up existent fields, updating them, and then iterating
   // over existent fields and making sure they still exist, if not, deleting them.
   clearFields();
   
   // Create a vector of the fields
   Vector<SimFieldDictionary::Entry *> flist;

   // Then populate with fields
   SimFieldDictionary * fieldDictionary = mTarget->getFieldDictionary();
   for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
   {
      flist.push_back(*ditr);
   }
   dQsort(flist.address(),flist.size(),sizeof(SimFieldDictionary::Entry *),compareEntries);
   
   for(U32 i = 0; i < (U32)flist.size(); i++)
   {
      SimFieldDictionary::Entry * entry = flist[i];
      GuiInspectorField *field = new GuiInspectorDynamicField( this, mTarget, entry );
      if( field != NULL )
      {
         field->setExtent(Point2I(this->getExtent().x, 30));
         field->registerObject();
         mChildren.push_back( field );
         mStack->addObject( field );
      }
   }
   
   mStack->pushObjectToBack(mAddCtrl);

   setUpdate();

   return true;
}
ConsoleMethod(GuiInspectorDynamicGroup, inspectGroup, bool, 2, 2, "() Refreshes the dynamic fields in the inspector.\n"
              "@return Returns true on success.")
{
   return object->inspectGroup();
}

void GuiInspectorDynamicGroup::clearFields()
{
   // save mAddCtrl
   Sim::getGuiGroup()->addObject(mAddCtrl);
   // delete everything else
   mStack->clear();
   // clear the mChildren list.
   mChildren.clear();
   // and restore.
   mStack->addObject(mAddCtrl);
}

SimFieldDictionary::Entry* GuiInspectorDynamicGroup::findDynamicFieldInDictionary( StringTableEntry fieldName )
{
   if( !mTarget )
      return NULL;

   SimFieldDictionary * fieldDictionary = mTarget->getFieldDictionary();

   for(SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
   {
      SimFieldDictionary::Entry * entry = (*ditr);
      
      if( dStricmp( entry->slotName, fieldName ) == 0 )
         return entry;
   }

   return NULL;
}

void GuiInspectorDynamicGroup::addDynamicField()
{
   // We can't add a field without a target
   if( !mTarget || !mStack )
   {
      Con::warnf("GuiInspectorDynamicGroup::addDynamicField - no target SimObject to add a dynamic field to.");
      return;
   }

   // find a field name that is not in use. 
   // But we wont try more than 100 times to find an available field.
   U32 uid = 1;
   char buf[64] = "dynamicField";
   SimFieldDictionary::Entry* entry = findDynamicFieldInDictionary(buf);
   while(entry != NULL && uid < 100)
   {
      dSprintf(buf, sizeof(buf), "dynamicField%03d", uid++);
      entry = findDynamicFieldInDictionary(buf);
   }

   //Con::evaluatef( "%d.%s = \"defaultValue\";", mTarget->getId(), buf );
   mTarget->setDataField(StringTable->insert(buf), NULL, "defaultValue");

   // now we simply re-inspect the object, to see the new field.
   this->inspectGroup();
   animateToContents();
}

ConsoleMethod( GuiInspectorDynamicGroup, addDynamicField, void, 2, 2, "obj.addDynamicField();" )
{
   object->addDynamicField();
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorDynamicField - Child class of GuiInspectorField 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorDynamicField);

GuiInspectorDynamicField::GuiInspectorDynamicField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, SimFieldDictionary::Entry* field )
{
   mCaption    = NULL;

   mParent     = parent;
   mTarget     = target;
   mDynField   = field;
   mRenameCtrl = NULL;
}

void GuiInspectorDynamicField::setData( const char* data )
{
   if( mTarget == NULL || mDynField == NULL )
      return;

   char buf[1024];
   const char * newValue = mEdit->getScriptValue();
   dStrcpy( buf, newValue ? newValue : "" );
   collapseEscape(buf);

   mTarget->getFieldDictionary()->setFieldValue(mDynField->slotName, buf);

   // Force our edit to update
   updateValue( data );

}

const char* GuiInspectorDynamicField::getData()
{
   if( mTarget == NULL || mDynField == NULL )
      return "";

   return mTarget->getFieldDictionary()->getFieldValue( mDynField->slotName );
}

void GuiInspectorDynamicField::renameField( StringTableEntry newFieldName )
{
   if( mTarget == NULL || mDynField == NULL || mParent == NULL || mEdit == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - No target object or dynamic field data found!" );
      return;
   }

   if( !newFieldName )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Invalid field name specified!" );
      return;
   }

   // Only proceed if the name has changed
   if( dStricmp( newFieldName, getFieldName() ) == 0 )
      return;

   // Grab a pointer to our parent and cast it to GuiInspectorDynamicGroup
   GuiInspectorDynamicGroup *group = dynamic_cast<GuiInspectorDynamicGroup*>(mParent);

   if( group == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Unable to locate GuiInspectorDynamicGroup parent!" );
      return;
   }

   // Grab our current dynamic field value
   const char* currentValue = getData();

   // Create our new field with the value of our old field and the new fields name!
   mTarget->setDataField( newFieldName, NULL, currentValue );

   // Configure our field to grab data from the new dynamic field
   SimFieldDictionary::Entry *newEntry = group->findDynamicFieldInDictionary( newFieldName );

   if( newEntry == NULL )
   {
      Con::warnf("GuiInspectorDynamicField::renameField - Unable to find new field!" );
      return;
   }

   // Set our old fields data to "" (which will effectively erase the field)
   mTarget->setDataField( getFieldName(), NULL, "" );
   
   // Assign our dynamic field pointer (where we retrieve field information from) to our new field pointer
   mDynField = newEntry;

   // Lastly we need to reassign our Command and AltCommand fields for our value edit control
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();",mTarget->getId(), getFieldName(), mEdit->getId() );
   mEdit->setExtent(Point2I((getExtent().x / 2) - 20, 30));
   mEdit->setField("AltCommand", szBuffer );
   mEdit->setField("Validate", szBuffer );
}

ConsoleMethod( GuiInspectorDynamicField, renameField, void, 3,3, "field.renameField(newDynamicFieldName);" )
{
   object->renameField( StringTable->insert(argv[2]) );
}

bool GuiInspectorDynamicField::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   mRenameCtrl = constructRenameControl();
   pushObjectToBack(mEdit);

   return true;
}

GuiControl* GuiInspectorDynamicField::constructRenameControl()
{
   // Create our renaming field
   GuiControl* retCtrl = new GuiTextEditCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiTextEditProfile" );

   // Don't forget to register ourselves
   char szName[512];
   dSprintf( szName, 512, "IE_%s_%d_%s_Rename", retCtrl->getClassName(), mTarget->getId(), getFieldName() );
   retCtrl->registerObject( szName );


   // Our command will evaluate to :
   //
   //    if( (editCtrl).getText() !$= "" )
   //       (field).renameField((editCtrl).getText());
   //
   char szBuffer[512];
   dSprintf( szBuffer, 512, "if( %d.getText() !$= \"\" ) %d.renameField(%d.getText());",retCtrl->getId(), getId(), retCtrl->getId() );
   dynamic_cast<GuiTextEditCtrl*>(retCtrl)->setText( getFieldName() );
   retCtrl->setExtent(Point2I((getExtent().x / 2) - 20, 30));
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer);
   addObject( retCtrl );


   // Finally, add a delete button for this field
   GuiButtonCtrl * delButt = new GuiButtonCtrl();
   if( delButt != NULL )
   {
      dSprintf(szBuffer, 512, "%d.%s = \"\";%d.inspectGroup();", mTarget->getId(), getFieldName(), mParent->getId());

      delButt->setField("profile", "GuiButtonDynProfile");
      delButt->setField("Text", "X");
      delButt->setPosition(Point2I((getExtent().x - 40), 0));
      delButt->setField("extent", "30 30");
      delButt->setField("Command", szBuffer);
      delButt->registerObject();

      addObject(delButt);
   }

   return retCtrl;
}

void GuiInspectorDynamicField::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   Parent::resize( newPosition, newExtent );
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorDatablockField 
// Field construction for datablock types
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorDatablockField);

static S32 QSORT_CALLBACK stringCompare(const void *a,const void *b)
{
   StringTableEntry sa = *(StringTableEntry*)a;
   StringTableEntry sb = *(StringTableEntry*)b;
   return(dStricmp(sb, sa));
}

GuiInspectorDatablockField::GuiInspectorDatablockField( StringTableEntry className )
{
   setClassName(className);
};

void GuiInspectorDatablockField::setClassName( StringTableEntry className )
{
   // Walk the ACR list and find a matching class if any.
   AbstractClassRep *walk = AbstractClassRep::getClassList();
   while(walk)
   {
      if(!dStricmp(walk->getClassName(), className))
      {
         // Match!
         mDesiredClass = walk;
         return;
      }

      walk = walk->getNextClass();
   }

   // No dice.
   Con::warnf("GuiInspectorDatablockField::setClassName - no class '%s' found!", className);
   return;
}

GuiControl* GuiInspectorDatablockField::constructEditControl()
{
   GuiControl* retCtrl = new GuiDropDownCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiDropDownCtrl *menu = dynamic_cast<GuiDropDownCtrl*>(retCtrl);

   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the dropdown is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();%d.inspect(%d);",mTarget->getId(), mField->pFieldname, menu->getId(), mParent->mParent->getId(), mTarget->getId() );
   menu->setField("Command", szBuffer );

   Vector<StringTableEntry> entries;

   SimDataBlockGroup * grp = Sim::getDataBlockGroup();
   for(SimDataBlockGroup::iterator i = grp->begin(); i != grp->end(); i++)
   {
      SimDataBlock * datablock = dynamic_cast<SimDataBlock*>(*i);

      // Skip non-datablocks if we somehow encounter them.
      if(!datablock)
         continue;

      // Ok, now we have to figure inheritance info.
      if( datablock && datablock->getClassRep()->isClass(mDesiredClass) )
         entries.push_back(datablock->getName());
   }

   // sort the entries
   dQsort(entries.address(), entries.size(), sizeof(StringTableEntry), stringCompare);

   // add them to our enum
   for(U32 j = 0; j < (U32)entries.size(); j++)
      menu->getList()->addItem(entries[j]);

   return retCtrl;
}
