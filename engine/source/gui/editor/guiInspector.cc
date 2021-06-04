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

#pragma region GuiInspector
//////////////////////////////////////////////////////////////////////////
// GuiInspector
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspector);

GuiInspector::GuiInspector()
{
   mGroups.clear();
   mTarget = NULL;

   mGroupPanelProfile = NULL;
   setField("GroupPanelProfile", "GuiPanelProfile");

   mGroupGridProfile = NULL;
   setField("GroupGridProfile", "GuiDefaultProfile");

   mLabelProfile = NULL;
   setField("LabelProfile", "GuiDefaultProfile");

   mTextEditProfile = NULL;
   setField("TextEditProfile", "GuiTextEditProfile");

   mDropDownProfile = NULL;
   setField("DropDownProfile", "GuiDropDownProfile");

   mDropDownItemProfile = NULL;
   setField("DropDownItemProfile", "GuiListBoxProfile");

   mScrollProfile = NULL;
   setField("ScrollProfile", "GuiScrollProfile");

   mBackgroundProfile = NULL;
   setField("BackgroundProfile", "GuiDefaultProfile");

   mThumbProfile = NULL;
   setField("ThumbProfile", "GuiScrollThumbProfile");

   mArrowProfile = NULL;
   setField("ArrowProfile", "GuiScrollArrowProfile");

   mTrackProfile = NULL;
   setField("TrackProfile", "GuiScrollTrackProfile");

   mCheckboxProfile = NULL;
   setField("CheckboxProfile", "GuiCheckboxProfile");

   mButtonProfile = NULL;
   setField("ButtonProfile", "GuiButtonProfile");

   mUseConstantHeightThumb = false;
   mScrollBarThickness = 12;
   mShowArrowButtons = true;

   mFieldCellSize.set(300, 30);
   mControlOffset.set(10, 16);
}

GuiInspector::~GuiInspector()
{
   clearGroups();
}

void GuiInspector::initPersistFields()
{
	Parent::initPersistFields();

	addField("GroupPanelProfile", TypeGuiProfile, Offset(mGroupPanelProfile, GuiInspector));
	addField("GroupGridProfile", TypeGuiProfile, Offset(mGroupGridProfile, GuiInspector));
	addField("LabelProfile", TypeGuiProfile, Offset(mLabelProfile, GuiInspector));
	addField("TextEditProfile", TypeGuiProfile, Offset(mTextEditProfile, GuiInspector));
	addField("DropDownProfile", TypeGuiProfile, Offset(mDropDownProfile, GuiInspector));
	addField("DropDownItemProfile", TypeGuiProfile, Offset(mDropDownItemProfile, GuiInspector));
	addField("ScrollProfile", TypeGuiProfile, Offset(mScrollProfile, GuiInspector));
	addField("backgroundProfile", TypeGuiProfile, Offset(mBackgroundProfile, GuiInspector));
	addField("thumbProfile", TypeGuiProfile, Offset(mThumbProfile, GuiInspector));
	addField("trackProfile", TypeGuiProfile, Offset(mTrackProfile, GuiInspector));
	addField("arrowProfile", TypeGuiProfile, Offset(mArrowProfile, GuiInspector));
	addField("CheckboxProfile", TypeGuiProfile, Offset(mCheckboxProfile, GuiInspector));
	addField("ButtonProfile", TypeGuiProfile, Offset(mButtonProfile, GuiInspector));

	addField("constantThumbHeight", TypeBool, Offset(mUseConstantHeightThumb, GuiInspector));
	addField("scrollBarThickness", TypeS32, Offset(mScrollBarThickness, GuiInspector));
	addField("showArrowButtons", TypeBool, Offset(mShowArrowButtons, GuiInspector));

	addField("FieldCellSize", TypePoint2I, Offset(mFieldCellSize, GuiInspector));
	addField("ControlOffset", TypePoint2I, Offset(mControlOffset, GuiInspector));
}

bool GuiInspector::onWake()
{
	if (!Parent::onWake())
		return false;

	if (mGroupPanelProfile != NULL)
		mGroupPanelProfile->incRefCount();

	if (mGroupGridProfile != NULL)
		mGroupGridProfile->incRefCount();

	if (mLabelProfile != NULL)
		mLabelProfile->incRefCount();

	if (mTextEditProfile != NULL)
		mTextEditProfile->incRefCount();

	if (mDropDownProfile != NULL)
		mDropDownProfile->incRefCount();

	if (mDropDownItemProfile != NULL)
		mDropDownItemProfile->incRefCount();

	if (mScrollProfile != NULL)
		mScrollProfile->incRefCount();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->incRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->incRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->incRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->incRefCount();

	if (mCheckboxProfile != NULL)
		mCheckboxProfile->incRefCount();

	if (mButtonProfile != NULL)
		mButtonProfile->incRefCount();

	return true;
}

void GuiInspector::onSleep()
{
	Parent::onSleep();

	if (mGroupPanelProfile != NULL)
		mGroupPanelProfile->decRefCount();

	if (mGroupGridProfile != NULL)
		mGroupGridProfile->decRefCount();

	if (mLabelProfile != NULL)
		mLabelProfile->decRefCount();

	if (mTextEditProfile != NULL)
		mTextEditProfile->decRefCount();

	if (mDropDownProfile != NULL)
		mDropDownProfile->decRefCount();

	if (mDropDownItemProfile != NULL)
		mDropDownItemProfile->decRefCount();

	if (mScrollProfile != NULL)
		mScrollProfile->decRefCount();

	if (mBackgroundProfile != NULL)
		mBackgroundProfile->decRefCount();

	if (mThumbProfile != NULL)
		mThumbProfile->decRefCount();

	if (mTrackProfile != NULL)
		mTrackProfile->decRefCount();

	if (mArrowProfile != NULL)
		mArrowProfile->decRefCount();

	if (mCheckboxProfile != NULL)
		mCheckboxProfile->decRefCount();

	if (mButtonProfile != NULL)
		mButtonProfile->decRefCount();
}

void GuiInspector::inspectPostApply()
{
	Parent::inspectPostApply();

	if (mTarget)
	{
		SimObjectPtr<SimObject> oldTarget = mTarget;
		mTarget = NULL;
		inspectObject(oldTarget);
	}
}

void GuiInspector::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	Parent::resize(Point2I(0, 0), newExtent);
}

void GuiInspector::parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent)
{
   GuiControl *parent = getParent();
   if( parent && dynamic_cast<GuiScrollCtrl*>(parent) != NULL )
   {
	   // Handle Parent Sizing (We constrain ourself to our parent's width)
      GuiScrollCtrl *scroll = dynamic_cast<GuiScrollCtrl*>(parent);
      setWidth(newParentExtent.x - scroll->scrollBarThickness());
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
      addObject( dynGroup );
   }

   // If any group is still empty at this point, kill it.
   for(S32 i=0; i<mGroups.size(); i++)
   {
      if(mGroups[i]->mGrid->size() == 0)
      {
		 mGroups[i]->deleteObject();
         mGroups.erase(i);
		 i--;
      }
   }
   resize(getPosition(), getExtent());

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

ConsoleMethod(GuiInspector, clearHiddenFields, void, 2, 2, "() Clears the list of hidden fields\n"
	"@return No return value.")
{
	object->clearHiddenField();
}

ConsoleMethod(GuiInspector, addHiddenField, void, 3, 3, "() Adds a new field to the list of hidden fields\n"
	"@return No return value.")
{
	object->addHiddenField(argv[2]);
}
#pragma endregion

#pragma region GuiInspectorField
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

GuiInspectorField::GuiInspectorField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, AbstractClassRep::Field* field )
{
   if( field != NULL )
      mText    = StringTable->insert( field->pFieldname );
   else
      mText    = StringTable->EmptyString;

   mGroup     = parent;
   mTarget     = target;
   mField      = field;
   mCanSave    = false;
   mFieldArrayIndex = NULL;
}

GuiInspectorField::GuiInspectorField()
{
   mText       = StringTable->EmptyString;
   mGroup        = NULL;
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

      mText = StringTable->insert( valCopy ); 
   }
   else
	   mText = StringTable->insert( field->pFieldname );
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
GuiControl* GuiInspectorField::constructEditControl(S32 width)
{
   GuiControl* retCtrl = new GuiTextEditCtrl();
   
   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setControlProfile(mGroup->mInspector->mTextEditProfile);

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(), retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->mBounds.set(mGroup->mInspector->mControlOffset, Point2I(width - mGroup->mInspector->mControlOffset.x, 30));

   return retCtrl;
}

void GuiInspectorField::registerEditControl( GuiControl *ctrl )
{
   if(!mTarget)
      return;
        
   char szName[512];
   dSprintf( szName, 512, "IE_%s_%d_%s_Field", ctrl->getClassName(), mTarget->getId(),mText);

   // Register the object
   ctrl->registerObject( szName );
}

bool GuiInspectorField::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	if( !mTarget )
		return false;

	setControlProfile(mGroup->mInspector->mLabelProfile);

	//Find the target width
	RectI innerRect = getInnerRect(Point2I(0,0), mGroup->mInspector->mFieldCellSize, NormalState, mProfile);
	mEdit = constructEditControl(innerRect.extent.x);

	if( mEdit == NULL )
		return false;

	innerRect.extent.y = mGroup->mInspector->mControlOffset.y + mEdit->getExtent().y;
	Point2I outerExt = getOuterExtent(innerRect.extent, NormalState, mProfile);
	mBounds.extent.y = outerExt.y;

	//Set the tool tip if possible
	if (mField != NULL && mField->pFieldDocs != NULL)
	{
		mEdit->setField("tooltip", mField->pFieldDocs);
		if (mGroup->mInspector->mTooltipProfile != NULL)
		{
			char buffer[256];
			dSprintf(buffer, sizeof(buffer), "%d", mGroup->mInspector->mTooltipProfile->getId());
			mEdit->setField("tooltipProfile", buffer);
		}
	}

	addObject( mEdit );

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
#pragma endregion

#pragma region GuiInspectorGroup
//////////////////////////////////////////////////////////////////////////
// GuiInspectorGroup
//////////////////////////////////////////////////////////////////////////
//
// The GuiInspectorGroup control is a helper control that the inspector
// makes use of which houses a collapsible panel type control for separating
// inspected objects fields into groups.  The content of the inspector is 
// made up of zero or more GuiInspectorGroup controls inside of a GuiChainControl
//
//
//
IMPLEMENT_CONOBJECT(GuiInspectorGroup);

GuiInspectorGroup::GuiInspectorGroup()
{
   mChildren.clear();

   mTarget = NULL;
   mInspector = NULL;
   mCanSave = false;

   mHorizSizing = horizSizingOptions::horizResizeWidth;
   mVertSizing = vertSizingOptions::vertResizeTop;
   mBounds.set(10, 0, (mInspector->getExtent().x - 20), 24);
}

GuiInspectorGroup::GuiInspectorGroup( SimObjectPtr<SimObject> target, StringTableEntry groupName, SimObjectPtr<GuiInspector> inspector )
{
   mChildren.clear();

   mText = StringTable->insert(groupName);
   mTarget = target;
   mInspector = inspector;
   mCanSave = false;

   mHorizSizing = horizSizingOptions::horizResizeWidth;
   mVertSizing = vertSizingOptions::vertResizeTop;
   mBounds.set(10, 0, (mInspector->getExtent().x - 20), 24);
}

bool GuiInspectorGroup::onAdd()
{
   if( !Parent::onAdd() )
      return false;

	setControlProfile(mInspector->mGroupPanelProfile);

   // Create our inner controls. Allow subclasses to provide other content.
   if(!createContent())
      return false;

   inspectGroup();

   return true;
}

bool GuiInspectorGroup::createContent()
{
   // Create our field stack control
   mGrid = new GuiGridCtrl();
   if( !mGrid)
      return false;


   mGrid->setControlProfile(mInspector->mGroupGridProfile);
   mGrid->setCellSize(mInspector->mFieldCellSize.x, mInspector->mFieldCellSize.y);
   mGrid->setCellSpacing(0,0);
   mGrid->setCellModeX(GuiGridCtrl::CellMode::Variable);
   mGrid->setCellModeY(GuiGridCtrl::CellMode::Variable);
   mGrid->setMaxColCount(0);
   mGrid->mIsExtentDynamic = true;
   mGrid->mOrderMode = GuiGridCtrl::OrderMode::LRTB;
   mGrid->setWidth(getExtent().x - 20);
   mGrid->setPosition(Point2I(10, getExtent().y));
   mGrid->setField("horizSizing", "width");
   mGrid->setField("vertSizing", "bottom");
   mGrid->registerObject();
   addObject( mGrid );

   return true;
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
         if( bNoGroup == true || ( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mText ) == 0 ))
            bGrabItems = true;
         continue;
      }
      else if ( itr->type == AbstractClassRep::EndGroupFieldType )
      {
         // If we're dealing with general fields, always set grabItems to false (to grab them)
         if( bNoGroup == true || ( itr->pGroupname != NULL && dStricmp( itr->pGroupname, mText ) == 0 ))
            bGrabItems = false;
         continue;
      }

      if((bGrabItems == true || (bNoGroup == true && bGrabItems == false)) && itr->type != AbstractClassRep::DepricatedFieldType )
      { 
		  //We are inside a group and looking for items that don't have a group. Move on.
		  if( bNoGroup == true && bGrabItems == true )
				continue; 

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
                  field->setInspectorGroup( this );
                  field->setInspectorField( itr, intToStr );
               }
               field->registerObject();
               mChildren.push_back( field );
               mGrid->addObject( field );
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

			//check the hidden field list
			if (mInspector->hideField(itr->pFieldname))
			{
				continue;
			}
			//Time to create a new field
            field = constructField( itr->type );
            if( field == NULL )
               field = new GuiInspectorField( this, mTarget, itr );
            else
            {
               field->setTarget( mTarget );
               field->setInspectorGroup( this );
               field->setInspectorField( itr );
            }
            field->setExtent(Point2I(this->getExtent().x, 30));
            field->registerObject();
            mChildren.push_back( field );
            mGrid->addObject( field );

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
#pragma endregion

#pragma region GuiInspectorDynamicGroup
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
   mGrid->addObject(shell);

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
         mGrid->addObject( field );
      }
   }
   
   mGrid->pushObjectToBack(mAddCtrl);

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
   mGrid->clear();
   // clear the mChildren list.
   mChildren.clear();
   // and restore.
   mGrid->addObject(mAddCtrl);
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
   if( !mTarget || !mGrid)
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
   //animateToContents();
}

ConsoleMethod( GuiInspectorDynamicGroup, addDynamicField, void, 2, 2, "obj.addDynamicField();" )
{
   object->addDynamicField();
}
#pragma endregion

#pragma region GuiInspectorDynamicField
//////////////////////////////////////////////////////////////////////////
// GuiInspectorDynamicField - Child class of GuiInspectorField 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorDynamicField);

GuiInspectorDynamicField::GuiInspectorDynamicField( GuiInspectorGroup* parent, SimObjectPtr<SimObject> target, SimFieldDictionary::Entry* field )
{
   mGroup     = parent;
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
   if( mTarget == NULL || mDynField == NULL || mGroup == NULL || mEdit == NULL )
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
   GuiInspectorDynamicGroup *group = dynamic_cast<GuiInspectorDynamicGroup*>(mGroup);

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

   // Lastly we need to reassign our AltCommand fields for our value edit control
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();",mTarget->getId(), getFieldName(), mEdit->getId() );
   mEdit->setExtent(Point2I((getExtent().x / 2) - 20, 30));
   mEdit->setField("AltCommand", szBuffer );
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
   addObject( retCtrl );


   // Finally, add a delete button for this field
   GuiButtonCtrl * delButt = new GuiButtonCtrl();
   if( delButt != NULL )
   {
      dSprintf(szBuffer, 512, "%d.%s = \"\";%d.inspectGroup();", mTarget->getId(), getFieldName(), mGroup->getId());

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
#pragma endregion

#pragma region GuiInspectorDatablockField
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
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();%d.inspect(%d);",mTarget->getId(), mField->pFieldname, menu->getId(), mGroup->mInspector->getId(), mTarget->getId() );
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
#pragma endregion
