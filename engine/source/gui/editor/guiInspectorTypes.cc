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
#include "gui/editor/guiInspectorTypes.h"

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeEnum 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeEnum);

GuiControl* GuiInspectorTypeEnum::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiPopUpMenuProfile2" );
   retCtrl->setExtent(Point2I((mParent->getWidth() / 2) - 20, 18));
   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.%s = %d.getText();%d.inspect(%d);",mTarget->getId(), mField->pFieldname, menu->getId(), mParent->mParent->getId(), mTarget->getId() );
   menu->setField("Command", szBuffer );

   //now add the entries
   for(S32 i = 0; i < mField->table->size; i++)
      menu->addEntry(mField->table->table[i].label, mField->table->table[i].index);

   return retCtrl;
}

void GuiInspectorTypeEnum::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeEnum)->setInspectorFieldType("GuiInspectorTypeEnum");
}

void GuiInspectorTypeEnum::updateValue( StringTableEntry newValue )
{
   GuiPopUpMenuCtrl *ctrl = dynamic_cast<GuiPopUpMenuCtrl*>( mEdit );
   if( ctrl != NULL )
      ctrl->setText( newValue );
}

void GuiInspectorTypeEnum::setData( StringTableEntry data )
{
   if( mField == NULL || mTarget == NULL )
      return;

   mTarget->setDataField( mField->pFieldname, NULL, data );

   // Force our edit to update
   updateValue( data );
}

StringTableEntry  GuiInspectorTypeEnum::getData()
{
   if( mField == NULL || mTarget == NULL )
      return "";

   return mTarget->getDataField( mField->pFieldname, NULL );
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeCheckBox 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeCheckBox);

GuiControl* GuiInspectorTypeCheckBox::constructEditControl()
{
   GuiControl* retCtrl = new GuiCheckBoxCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiCheckBoxCtrl *check = dynamic_cast<GuiCheckBoxCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiCheckBoxProfile" );
   retCtrl->setField( "text", "" );
   retCtrl->setExtent(Point2I(20, 30));

   //check->mIndent = 4;

   retCtrl->setScriptValue( getData() );

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getValue());",getId(),check->getId() );
   check->setField("Command", szBuffer );

   return retCtrl;
}


void GuiInspectorTypeCheckBox::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeBool)->setInspectorFieldType("GuiInspectorTypeCheckBox");
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiProfile 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeGuiProfile);

void GuiInspectorTypeGuiProfile::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeGuiProfile)->setInspectorFieldType("GuiInspectorTypeGuiProfile");
}

static S32 QSORT_CALLBACK stringCompare(const void *a,const void *b)
{
   StringTableEntry sa = *(StringTableEntry*)a;
   StringTableEntry sb = *(StringTableEntry*)b;
   return(dStricmp(sa, sb));
}

GuiControl* GuiInspectorTypeGuiProfile::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiPopUpMenuProfile2" );
   retCtrl->setExtent(Point2I((mParent->getWidth() / 2) - 20, 18));
   menu->setField("text", getData());

   registerEditControl( retCtrl );

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),menu->getId() );
   menu->setField("Command", szBuffer );

   Vector<StringTableEntry> entries;

   SimGroup * grp = Sim::getGuiDataGroup();
   for(SimGroup::iterator i = grp->begin(); i != grp->end(); i++)
   {
      GuiControlProfile * profile = dynamic_cast<GuiControlProfile *>(*i);
      if(profile)
      {
         entries.push_back(profile->getName());
      }
   }

   menu->sort();
   // sort the entries
   //dQsort(entries.address(), entries.size(), sizeof(StringTableEntry), stringCompare);
   for(U32 j = 0; j < (U32)entries.size(); j++)
      menu->addEntry(entries[j], 0);

   return retCtrl;
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeGuiBorderProfile 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeGuiBorderProfile);

void GuiInspectorTypeGuiBorderProfile::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeGuiBorderProfile)->setInspectorFieldType("GuiInspectorTypeGuiBorderProfile");
}

GuiControl* GuiInspectorTypeGuiBorderProfile::constructEditControl()
{
   GuiControl* retCtrl = new GuiPopUpMenuCtrl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   GuiPopUpMenuCtrl *menu = dynamic_cast<GuiPopUpMenuCtrl*>(retCtrl);

   // Let's make it look pretty.
   retCtrl->setField("profile", "GuiPopUpMenuProfile2");
   retCtrl->setExtent(Point2I((mParent->getWidth() / 2) - 20, 18));
   menu->setField("text", getData());

   registerEditControl(retCtrl);

   // Configure it to update our value when the popup is closed
   char szBuffer[512];
   dSprintf(szBuffer, 512, "%d.apply(%d.getText());", getId(), menu->getId());
   menu->setField("Command", szBuffer);

   Vector<StringTableEntry> entries;

   SimGroup * grp = Sim::getGuiDataGroup();
   for (SimGroup::iterator i = grp->begin(); i != grp->end(); i++)
   {
      GuiBorderProfile * profile = dynamic_cast<GuiBorderProfile *>(*i);
      if (profile)
      {
         entries.push_back(profile->getName());
      }
   }

   menu->sort();
   for (U32 j = 0; j < (U32)entries.size(); j++)
      menu->addEntry(entries[j], 0);

   return retCtrl;
}


//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeFileName 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeFileName);

void GuiInspectorTypeFileName::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeFilename)->setInspectorFieldType("GuiInspectorTypeFileName");
}

GuiControl* GuiInspectorTypeFileName::constructEditControl()
{
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField( "profile", "GuiTextProfile" );

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   char szBuffer[512];
   dSprintf( szBuffer, 512, "%d.apply(%d.getText());",getId(),retCtrl->getId() );
   retCtrl->setField("AltCommand", szBuffer );
   retCtrl->setField("Validate", szBuffer );
   retCtrl->setExtent(Point2I((mParent->getExtent().x / 2) - 20, 25));

   mBrowseButton = new GuiButtonCtrl();

   if( mBrowseButton != NULL )
   {
      
      char szBuffer[512];
      dSprintf( szBuffer, 512, "getLoadFilename(\"*.*\", \"%d.apply\", \"%s\");",getId(), getData());
      mBrowseButton->setField( "Command", szBuffer );
      mBrowseButton->setField( "text", "..." );
      mBrowseButton->setField( "Profile", "GuiButtonDynProfile" );
      mBrowseButton->registerObject();
      mBrowseButton->setExtent(Point2I(30, 25));
      mBrowseButton->setPosition(Point2I(((mParent->getExtent().x / 2) - 20) - 30, 0));
      retCtrl->addObject( mBrowseButton );

   }

   return retCtrl;
}

void GuiInspectorTypeFileName::resize( const Point2I &newPosition, const Point2I &newExtent )
{
   Parent::resize( newPosition, newExtent );
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColor (Base for ColorI/ColorF) 
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColor);

GuiControl* GuiInspectorTypeColor::constructEditControl()
{
   GuiControl* retCtrl = new GuiColorPickerCtrl();

   // If we couldn't construct the control, bail!
   if( retCtrl == NULL )
      return retCtrl;

   // Don't forget to register ourselves
   registerEditControl( retCtrl );

   retCtrl->setExtent(Point2I((getExtent().x / 2) - 30, 30));
   const char* mCol = getData();
   retCtrl->setField("BaseColor", mCol);
   char szBuffer[512];
   dSprintf(szBuffer, 512, "%s(\"%s\", \"%d.apply\");", mColorFunction, getData(), getId());
   retCtrl->setField("Command", szBuffer);

   return retCtrl;
}

void GuiInspectorTypeColor::updateValue(StringTableEntry newValue)
{
   GuiColorPickerCtrl *ctrl = dynamic_cast<GuiColorPickerCtrl*>(mEdit);
   if (ctrl != NULL)
      ctrl->setField("BaseColor", newValue);
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColorI
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorI);

void GuiInspectorTypeColorI::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorI)->setInspectorFieldType("GuiInspectorTypeColorI");
}

GuiInspectorTypeColorI::GuiInspectorTypeColorI()
{
   mColorFunction = StringTable->insert("getColorI");
}

//////////////////////////////////////////////////////////////////////////
// GuiInspectorTypeColorF
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeColorF);

void GuiInspectorTypeColorF::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeColorF)->setInspectorFieldType("GuiInspectorTypeColorF");
}

GuiInspectorTypeColorF::GuiInspectorTypeColorF()
{
   mColorFunction = StringTable->insert("getColorF");
}


//////////////////////////////////////////////////////////////////////////
// TypeSimObjectPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeSimObjectPtr);

void GuiInspectorTypeSimObjectPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeSimObjectPtr)->setInspectorFieldType("GuiInspectorTypeSimObjectPtr");
}

GuiControl * GuiInspectorTypeSimObjectPtr::constructEditControl()
{
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   retCtrl->setField("profile", "GuiTextProfile");
   retCtrl->setExtent(Point2I((mParent->getWidth() / 2) - 20, 25));
   retCtrl->setField("text", getData());

   return retCtrl;

}

StringTableEntry GuiInspectorTypeSimObjectPtr::getData()
{
   if (mField == NULL || mTarget == NULL)
      return "";

   return mTarget->getDataField(mField->pFieldname, NULL);
}

//////////////////////////////////////////////////////////////////////////
// TypeS32 GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeS32);

void GuiInspectorTypeS32::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeS32)->setInspectorFieldType("GuiInspectorTypeS32");
}

GuiControl * GuiInspectorTypeS32::constructEditControl()
{
   
   GuiControl* retCtrl;
   const char* mData = getData();
   const char* mName = mField->pFieldname; 
   U32 mCount = 1;
   if (mName == StringTable->insert("CollisionGroups") || mName == StringTable->insert("CollisionLayers"))
   {
      mCount = 32;
   }

   // this is just here for Collision layers and groups, needs to be swapped over to a different type.
   if (mCount == 32)
   {
      retCtrl = new GuiGridCtrl();
      registerEditControl(retCtrl);
      retCtrl->setField("CellSizeX","35");
      retCtrl->setField("CellSizeY","30");
      retCtrl->setExtent(Point2I(mParent->getExtent().x / 2, 10));

      for (U32 i = 0; i < mCount; i++)
      {
         char szName[32];
         dSprintf(szName, 32, "%d", i);
         // create our elemnts.
         GuiControl* mCont = new GuiControl;
         registerEditControl(mCont);
         GuiControl* mLabel = new GuiControl;
         registerEditControl(mLabel);
         GuiCheckBoxCtrl* mCheck = new GuiCheckBoxCtrl;
         registerEditControl(mCheck);
         // collision group and layer default to on.
         const char* val = StringUnit::getUnit(mData, i, " ");
         if (StringTable->insert(szName) == StringTable->insert(val))
         {
            mCheck->setStateOn(true);
         }
         // say hello to my really big friend.
         char szBuffer[512];
         dSprintf(szBuffer, 512, "CollisionArrayUpdate(%d,\"%s\", \"%s\", %d, \"%d.apply\");",
            mTarget->getId(), mName,szName,mCheck->getId(),getId());

         mCheck->setField("Command", szBuffer);
         // minimum size required for 2 numbers to show up and a checkbox.
         mCont->setExtent(Point2I(35, 30));
         mLabel->setText(szName);
         mLabel->setExtent(Point2I(15, 30));
         mCheck->setExtent(Point2I(20, 30));
         mCont->addObject(mLabel);
         mLabel->setPosition(Point2I(0, 0));
         mCont->addObject(mCheck);
         mCheck->setPosition(Point2I(15, 0));

         retCtrl->addObject(mCont);
      }

      GuiButtonCtrl* mAll = new GuiButtonCtrl;
      registerEditControl(mAll);
      GuiButtonCtrl* mNone = new GuiButtonCtrl;
      registerEditControl(mNone);

      mAll->setText("All");
      mNone->setText("None");

      char szBufferAll[512];
      dSprintf(szBufferAll, 512, "%d.apply(\"all\");", getId());
      mAll->setField("Command", szBufferAll);
      mAll->setField("profile", "GuiButtonDynProfile");
      dSprintf(szBufferAll, 512, "%d.apply(\"none\");", getId());
      mNone->setField("Command", szBufferAll);
      mNone->setField("profile", "GuiButtonDynProfile");

      retCtrl->addObject(mAll);
      retCtrl->addObject(mNone);

   }
   else
   {
      // everything else should do fine under an ordinary text edit.
      retCtrl = new GuiTextEditCtrl();

      // If we couldn't construct the control, bail!
      if (retCtrl == NULL)
         return retCtrl;

      // Let's make it look pretty.
      retCtrl->setField("profile", "GuiNumberEditProfile");

      // Don't forget to register ourselves
      registerEditControl(retCtrl);

      char szBuffer[512];
      dSprintf(szBuffer, 512, "%d.apply(%d.getText());", getId(), retCtrl->getId());
      retCtrl->setField("AltCommand", szBuffer);
      retCtrl->setField("Validate", szBuffer);
      retCtrl->setExtent(Point2I((getExtent().x / 2) - 40, 30));

   }

   

   return retCtrl;
}

const char* GuiInspectorTypeS32::getData()
{
   if (mField == NULL || mTarget == NULL)
      return "";

   return mTarget->getDataField(mField->pFieldname, NULL);
}

//////////////////////////////////////////////////////////////////////////
// TypePoint2I GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypePoint2I);

void GuiInspectorTypePoint2I::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypePoint2I)->setInspectorFieldType("GuiInspectorTypePoint2I");
}

GuiControl * GuiInspectorTypePoint2I::constructEditControl()
{
   // Do everything in a simple way first.
   // then complicate it.
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   // get our data.
   const char* mData = mTarget->getDataField(mField->pFieldname, NULL);
   // Don't forget to register ourselves
   retCtrl->setCanSaveDynamicFields(true);
   registerEditControl(retCtrl);
   // we want 2 controls in this one.
   retCtrl->setExtent(Point2I(mParent->getExtent().x / 2, 60));
   // make our x fields.
   GuiTextEditCtrl* mXctrl = new GuiTextEditCtrl();
   GuiControl* mXLabel = new GuiControl();
   // make our y fields.
   GuiTextEditCtrl* mYctrl = new GuiTextEditCtrl();
   GuiControl* mYLabel = new GuiControl();

   // Don't forget to register ourselves
   registerEditControl(mXctrl);
   registerEditControl(mYctrl);

   mXctrl->setField("profile", "GuiNumberEditProfile");
   mYctrl->setField("profile", "GuiNumberEditProfile");

   U32 mCount = StringUnit::getUnitCount(mData, " ");

   for (U32 i = 0; i < mCount; i++)
   {
      const char* mVal = StringUnit::getUnit(mData, i, " ");

      if (i == 0)
      {
         mXctrl->setText(mVal);
      }
      else
      {
         mYctrl->setText(mVal);
      }
   }

   // make and position our controls.
   mXLabel->setExtent(Point2I(15, 30));
   mYLabel->setExtent(Point2I(15, 30));
   mXLabel->setField("text","X:");
   mYLabel->setField("text", "Y:");
   mYLabel->setPosition(Point2I(0, 30));

   mXLabel->setField("profile", "GuiTextProfile");
   mYLabel->setField("profile", "GuiTextProfile");
   
   mXctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));
   mYctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));

   char szCommand[512];

   dSprintf(szCommand, 512, "ApplyPoint2Value(\"%d.apply\",%d,%d);", getId(), mXctrl->getId(), mYctrl->getId());

   mXctrl->setPosition(Point2I(10, 0));
   mYctrl->setPosition(Point2I(10, 30));

   // they can share the same command.
   // set altCommand instead.
   mXctrl->setField("AltCommand", szCommand);
   mYctrl->setField("AltCommand", szCommand);

   retCtrl->addObject(mXLabel);
   retCtrl->addObject(mYLabel);
   retCtrl->addObject(mXctrl);
   retCtrl->addObject(mYctrl);
   char szBuffX[512];
   char szBuffY[512];
   dSprintf(szBuffX, 512, "%d", mXctrl->getId());
   dSprintf(szBuffY, 512, "%d", mYctrl->getId());
   retCtrl->setDataField(StringTable->insert("fieldX"),NULL, StringTable->insert(szBuffX));
   retCtrl->setDataField(StringTable->insert("fieldY"),NULL, StringTable->insert(szBuffY));

   return retCtrl;
}

void GuiInspectorTypePoint2I::updateValue(StringTableEntry newValue)
{
   GuiControl *ctrl = dynamic_cast<GuiControl*>(mEdit);
   if (ctrl != NULL)
   {
      const char* xField = ctrl->getDataField(StringTable->insert("fieldX"), NULL);
      const char* yField = ctrl->getDataField(StringTable->insert("fieldY"), NULL);
      Con::evaluatef("UpdatePoint2Value(\"%s\",\"%s\",\"%s\");", newValue, xField, yField);

   }

}

//////////////////////////////////////////////////////////////////////////
// TypePoint2F GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypePoint2F);

void GuiInspectorTypePoint2F::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypePoint2F)->setInspectorFieldType("GuiInspectorTypePoint2F");
}

GuiControl * GuiInspectorTypePoint2F::constructEditControl()
{
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   // get our data.
   const char* mData = mTarget->getDataField(mField->pFieldname, NULL);
   // Don't forget to register ourselves
   retCtrl->setCanSaveDynamicFields(true);
   registerEditControl(retCtrl);
   // we want 2 controls in this one.
   retCtrl->setExtent(Point2I(mParent->getExtent().x / 2, 60));
   // make our x fields.
   GuiTextEditCtrl* mXctrl = new GuiTextEditCtrl();
   GuiControl* mXLabel = new GuiControl();
   // make our y fields.
   GuiTextEditCtrl* mYctrl = new GuiTextEditCtrl();
   GuiControl* mYLabel = new GuiControl();

   // Don't forget to register ourselves
   registerEditControl(mXctrl);
   registerEditControl(mYctrl);

   // the only difference between point2i and 2f
   // keeping them separate just incase changes to textEdit.
   mXctrl->setField("profile", "GuiTextEditProfile");
   mYctrl->setField("profile", "GuiTextEditProfile");

   U32 mCount = StringUnit::getUnitCount(mData, " ");

   for (U32 i = 0; i < mCount; i++)
   {
      const char* mVal = StringUnit::getUnit(mData, i, " ");

      if (i == 0)
      {
         mXctrl->setText(mVal);
      }
      else
      {
         mYctrl->setText(mVal);
      }
   }

   // make and position our controls.
   mXLabel->setExtent(Point2I(15, 30));
   mYLabel->setExtent(Point2I(15, 30));
   mXLabel->setField("text", "X:");
   mYLabel->setField("text", "Y:");
   mYLabel->setPosition(Point2I(0, 30));

   mXLabel->setField("profile", "GuiTextProfile");
   mYLabel->setField("profile", "GuiTextProfile");

   mXctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));
   mYctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));

   char szCommand[512];

   dSprintf(szCommand, 512, "ApplyPoint2Value(\"%d.apply\",%d,%d);", getId(), mXctrl->getId(), mYctrl->getId());

   mXctrl->setPosition(Point2I(10, 0));
   mYctrl->setPosition(Point2I(10, 30));

   // they can share the same command.
   // set altCommand instead.
   mXctrl->setField("AltCommand", szCommand);
   mYctrl->setField("AltCommand", szCommand);

   retCtrl->addObject(mXLabel);
   retCtrl->addObject(mYLabel);
   retCtrl->addObject(mXctrl);
   retCtrl->addObject(mYctrl);
   char szBuffX[512];
   char szBuffY[512];
   dSprintf(szBuffX, 512, "%d", mXctrl->getId());
   dSprintf(szBuffY, 512, "%d", mYctrl->getId());
   retCtrl->setDataField(StringTable->insert("fieldX"), NULL, StringTable->insert(szBuffX));
   retCtrl->setDataField(StringTable->insert("fieldY"), NULL, StringTable->insert(szBuffY));

   return retCtrl;
}

void GuiInspectorTypePoint2F::updateValue(StringTableEntry newValue)
{
   GuiControl *ctrl = dynamic_cast<GuiControl*>(mEdit);
   if (ctrl != NULL)
   {
      const char* xField = ctrl->getDataField(StringTable->insert("fieldX"), NULL);
      const char* yField = ctrl->getDataField(StringTable->insert("fieldY"), NULL);
      Con::evaluatef("UpdatePoint2Value(\"%s\",\"%s\",\"%s\");", newValue, xField, yField);

   }

}

//////////////////////////////////////////////////////////////////////////
// TypeVector2 GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeVector2);

void GuiInspectorTypeVector2::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeVector2)->setInspectorFieldType("GuiInspectorTypeVector2");
}

GuiControl * GuiInspectorTypeVector2::constructEditControl()
{
   // yes vector2 is the same as point2f as well. 
   // all of these could be done similarly to color. but testing need test.
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   // get our data.
   const char* mData = mTarget->getDataField(mField->pFieldname, NULL);
   // Don't forget to register ourselves
   retCtrl->setCanSaveDynamicFields(true);
   registerEditControl(retCtrl);
   // we want 2 controls in this one.
   retCtrl->setExtent(Point2I(mParent->getExtent().x / 2, 60));
   // make our x fields.
   GuiTextEditCtrl* mXctrl = new GuiTextEditCtrl();
   GuiControl* mXLabel = new GuiControl();
   // make our y fields.
   GuiTextEditCtrl* mYctrl = new GuiTextEditCtrl();
   GuiControl* mYLabel = new GuiControl();

   // Don't forget to register ourselves
   registerEditControl(mXctrl);
   registerEditControl(mYctrl);

   // the only difference between point2i and 2f
   // keeping them separate just incase changes to textEdit.
   mXctrl->setField("profile", "GuiTextEditProfile");
   mYctrl->setField("profile", "GuiTextEditProfile");

   U32 mCount = StringUnit::getUnitCount(mData, " ");

   for (U32 i = 0; i < mCount; i++)
   {
      const char* mVal = StringUnit::getUnit(mData, i, " ");

      if (i == 0)
      {
         mXctrl->setText(mVal);
      }
      else
      {
         mYctrl->setText(mVal);
      }
   }

   // make and position our controls.
   mXLabel->setExtent(Point2I(15, 30));
   mYLabel->setExtent(Point2I(15, 30));
   mXLabel->setField("text", "X:");
   mYLabel->setField("text", "Y:");
   mYLabel->setPosition(Point2I(0, 30));

   mXLabel->setField("profile", "GuiTextProfile");
   mYLabel->setField("profile", "GuiTextProfile");

   mXctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));
   mYctrl->setExtent(Point2I((mParent->getExtent().x / 2) - 25 - 20, 30));

   char szCommand[512];

   dSprintf(szCommand, 512, "ApplyPoint2Value(\"%d.apply\",%d,%d);", getId(), mXctrl->getId(), mYctrl->getId());

   mXctrl->setPosition(Point2I(10, 0));
   mYctrl->setPosition(Point2I(10, 30));

   // they can share the same command.
   // set altCommand instead.
   mXctrl->setField("AltCommand", szCommand);
   mYctrl->setField("AltCommand", szCommand);

   retCtrl->addObject(mXLabel);
   retCtrl->addObject(mYLabel);
   retCtrl->addObject(mXctrl);
   retCtrl->addObject(mYctrl);
   char szBuffX[512];
   char szBuffY[512];
   dSprintf(szBuffX, 512, "%d", mXctrl->getId());
   dSprintf(szBuffY, 512, "%d", mYctrl->getId());
   retCtrl->setDataField(StringTable->insert("fieldX"), NULL, StringTable->insert(szBuffX));
   retCtrl->setDataField(StringTable->insert("fieldY"), NULL, StringTable->insert(szBuffY));

   return retCtrl;
}

void GuiInspectorTypeVector2::updateValue(StringTableEntry newValue)
{
   GuiControl *ctrl = dynamic_cast<GuiControl*>(mEdit);
   if (ctrl != NULL)
   {
      const char* xField = ctrl->getDataField(StringTable->insert("fieldX"), NULL);
      const char* yField = ctrl->getDataField(StringTable->insert("fieldY"), NULL);
      Con::evaluatef("UpdatePoint2Value(\"%s\",\"%s\",\"%s\");", newValue, xField, yField);

   }

}

//////////////////////////////////////////////////////////////////////////
// TypeAsset GuiInspectorField Class Base for Assets
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeAsset);

GuiControl* GuiInspectorTypeAsset::constructEditControl()
{
   // we don't need a custom update for this.
   // the object is a guicontrol and therefore 
   // just needs the text value of the asset.
   GuiControl* retCtrl = new GuiControl();

   // If we couldn't construct the control, bail!
   if (retCtrl == NULL)
      return retCtrl;

   // Let's make it look pretty.
   retCtrl->setField("profile", "GuiTextProfile");

   // Don't forget to register ourselves
   registerEditControl(retCtrl);

   retCtrl->setExtent(Point2I((getExtent().x / 2), 30));
   retCtrl->setText(getData());

   mBrowseButton = new GuiButtonCtrl();

   if (mBrowseButton != NULL)
   {

      char szBuffer[512];
      dSprintf(szBuffer, 512, "getAsset(\"%d.apply\", \"%s\", \"%s\");", getId(), getData(), mAssetType);
      mBrowseButton->setField("Command", szBuffer);
      mBrowseButton->setField("text", "...");
      mBrowseButton->setField("Profile", "GuiButtonDynProfile");
      mBrowseButton->registerObject();
      mBrowseButton->setExtent(Point2I(30, 25));
      mBrowseButton->setPosition(Point2I(((getExtent().x / 2) - 30) - 30, 0));
      retCtrl->addObject(mBrowseButton);

   }

   return retCtrl;
}

//////////////////////////////////////////////////////////////////////////
// TypeImageAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeImageAssetPtr);

GuiInspectorTypeImageAssetPtr::GuiInspectorTypeImageAssetPtr()
{
   mAssetType = StringTable->insert("ImageAsset");
}

void GuiInspectorTypeImageAssetPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeImageAssetPtr)->setInspectorFieldType("GuiInspectorTypeImageAssetPtr");
}

//////////////////////////////////////////////////////////////////////////
// TypeAnimationAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeAnimationAssetPtr);

GuiInspectorTypeAnimationAssetPtr::GuiInspectorTypeAnimationAssetPtr()
{
   mAssetType = StringTable->insert("AnimationAsset");
}

void GuiInspectorTypeAnimationAssetPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeAnimationAssetPtr)->setInspectorFieldType("GuiInspectorTypeAnimationAssetPtr");
}

//////////////////////////////////////////////////////////////////////////
// TypeAudioAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeAudioAssetPtr);

GuiInspectorTypeAudioAssetPtr::GuiInspectorTypeAudioAssetPtr()
{
   mAssetType = StringTable->insert("AudioAsset");
}

void GuiInspectorTypeAudioAssetPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeAudioAssetPtr)->setInspectorFieldType("GuiInspectorTypeAudioAssetPtr");
}

//////////////////////////////////////////////////////////////////////////
// TypeFontAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeFontAssetPtr);

GuiInspectorTypeFontAssetPtr::GuiInspectorTypeFontAssetPtr()
{
   mAssetType = StringTable->insert("FontAsset");
}

void GuiInspectorTypeFontAssetPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeFontAssetPtr)->setInspectorFieldType("GuiInspectorTypeFontAssetPtr");
}

//////////////////////////////////////////////////////////////////////////
// TypeParticleAssetPtr GuiInspectorField Class
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_CONOBJECT(GuiInspectorTypeParticleAssetPtr);

GuiInspectorTypeParticleAssetPtr::GuiInspectorTypeParticleAssetPtr()
{
   mAssetType = StringTable->insert("ParticleAsset");
}

void GuiInspectorTypeParticleAssetPtr::consoleInit()
{
   Parent::consoleInit();

   ConsoleBaseType::getType(TypeParticleAssetPtr)->setInspectorFieldType("GuiInspectorTypeParticleAssetPtr");
}

