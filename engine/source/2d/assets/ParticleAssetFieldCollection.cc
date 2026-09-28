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

#include "2d/assets/ParticleAssetFieldCollection.h"

//-----------------------------------------------------------------------------

    // Set custom property name.
static StringTableEntry particleAssetFieldNodeName = StringTable->insert("Fields");

//-----------------------------------------------------------------------------

ParticleAssetFieldCollection::ParticleAssetFieldCollection() :
                                    mpSelectedField( NULL )
{
}

//-----------------------------------------------------------------------------

ParticleAssetFieldCollection::~ParticleAssetFieldCollection()
{
}

//------------------------------------------------------------------------------

void ParticleAssetFieldCollection::copyTo( ParticleAssetFieldCollection& fieldCollection )
{
    // Iterate the fields.
    for( typeFieldHash::iterator fieldItr = mFields.begin(); fieldItr != mFields.end(); ++fieldItr )
    {
        // Fetch field.
        ParticleAssetField* pParticleAssetField = fieldItr->value;

        // Find target field.
        ParticleAssetField* pTargetParticleAssetField = fieldCollection.findField( fieldItr->key );

        // Sanity!
        AssertFatal( pTargetParticleAssetField != NULL, "ParticleAssetFieldCollection::copyTo() - Could not find target particle asset field." );

        // Copy field.
        pParticleAssetField->copyTo( *pTargetParticleAssetField );
    }

    // Select the same field name.
    if ( getSelectedField() != NULL )
        fieldCollection.selectField( getSelectedField()->getFieldName() );
}

//------------------------------------------------------------------------------

void ParticleAssetFieldCollection::addField( ParticleAssetField& particleAssetField, const char* pFieldName, F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue )
{
    // Sanity!
    AssertFatal( pFieldName != NULL, "ParticleAssetFieldCollection::addField() - Field name cannot be NULL or empty." );

    // Set the field name.
    particleAssetField.setFieldName( pFieldName );

    // Sanity!
    AssertFatal( !mFields.contains( particleAssetField.getFieldName() ), "ParticleAssetFieldCollection::addField() - The particle field name already exists." );

    // Add to fields.
    mFields.insert( particleAssetField.getFieldName(), &particleAssetField );

    // Initialize the field.
    particleAssetField.initialize( maxTime, minValue, maxValue, defaultValue );
}

//-----------------------------------------------------------------------------

ParticleAssetField* ParticleAssetFieldCollection::selectField( const char* pFieldName )
{
    // Sanity!
    AssertFatal( pFieldName != NULL, "ParticleAssetFieldCollection::selectField() - Field name cannot be NULL or empty." );

    // Has a field-name been specified?
    if ( dStrlen(pFieldName) > 0 )
    {
        // Yes, so find the field.
        mpSelectedField = findField( pFieldName );

        // Did we find the field?
        if ( mpSelectedField == NULL )
        {
            // No, so warn.
            Con::warnf( "ParticleAssetFieldCollection::selectField() - Could not find the field name '%s'.", pFieldName );
        }
    }
    else
    {
        // No, so reset the field.
        mpSelectedField = NULL;
    }

    return mpSelectedField;
}

//-----------------------------------------------------------------------------

ParticleAssetField* ParticleAssetFieldCollection::findField( const char* pFieldName )
{
    // Sanity!
    AssertFatal( pFieldName != NULL, "ParticleAssetFieldCollection::findField() - Cannot find NULL field." );

    // Find the field.
    typeFieldHash::iterator fieldItr = mFields.find( StringTable->insert( pFieldName ) );

    // Return the field if it was found.
    return fieldItr == mFields.end() ? NULL : fieldItr->value;
}

//-----------------------------------------------------------------------------

S32 ParticleAssetFieldCollection::setSingleDataKey( F32 value )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::setSingleKey() - No field selected." );
        return -1;
    }

    // Set single data key.
    return mpSelectedField->setSingleDataKey( value );
}

//-----------------------------------------------------------------------------

S32 ParticleAssetFieldCollection::addDataKey( F32 time, F32 value )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::addDataKey() - No field selected." );
        return -1;
    }

    // Add Data Key.
    return mpSelectedField->addDataKey( time, value );
}

//-----------------------------------------------------------------------------

bool ParticleAssetFieldCollection::removeDataKey( S32 index )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::removeDataKey() - No field selected." );
        return false;
    }

    // Remove Data Key.
    return mpSelectedField->removeDataKey( index );
}

//-----------------------------------------------------------------------------

bool ParticleAssetFieldCollection::clearDataKeys( void )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::clearDataKeys() - No field selected." );
        return false;
    }

    // Clear Data Keys
    mpSelectedField->clearDataKeys();

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ParticleAssetFieldCollection::setDataKey( S32 index, F32 value )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::setDataKey() - No field selected." );
        return false;
    }

    // Set the data-key value.
    return mpSelectedField->setDataKeyValue( index, value );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getDataKeyValue( S32 index ) const
{
// Have we got a valid field selected?
if ( !mpSelectedField )
{
    // No, so warn.
    Con::warnf( "ParticleAssetFieldCollection::getDataKeyValue() - No field selected." );
    return 0.0f;
}

// Get the data-key value.
return mpSelectedField->getDataKeyValue( index );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getDataKeyTime( S32 index ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getDataKeyTime() - No field selected." );
        return 0.0f;
    }

    // Get the data-key time.
    return mpSelectedField->getDataKeyTime( index );
}

//-----------------------------------------------------------------------------

const ParticleAssetField::DataKey& ParticleAssetFieldCollection::getDataKey( const U32 index ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getDataKey() - No field selected." );
        return ParticleAssetField::BadDataKey;
    }

    // Get the data-key.
    return mpSelectedField->getDataKey( index );
}

//-----------------------------------------------------------------------------

U32 ParticleAssetFieldCollection::getDataKeyCount( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getDataKeyCount() - No field selected." );
        return -1;
    }

    // Get the data-key count.
    return mpSelectedField->getDataKeyCount();
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getMinValue( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getMinValue() - No field selected." );
        return 0.0f;
    }

    // Get Min Value.
    return mpSelectedField->getMinValue();
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getMaxValue( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getMaxValue() - No field selected." );
        return 0.0f;
    }

    // Get Max Value.
    return mpSelectedField->getMaxValue();
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getMinTime( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getMinTime() - No field selected." );
        return 0.0f;
    }

    // Get Min Time.
    return mpSelectedField->getMinTime();
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getMaxTime( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getMaxTime() - No field selected." );
        return 0.0f;
    }

    // Get Max Time.
    return mpSelectedField->getMaxTime();
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getFieldValue( F32 time ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getFieldValue() - No field selected." );
        return 0.0f;
    }

    // Get Graph Value.
    return mpSelectedField->getFieldValue( time );
}

//-----------------------------------------------------------------------------

bool ParticleAssetFieldCollection::setRepeatTime( const F32 repeatTime )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::setRepeatTime() - No field selected." );
        return false;
    }

    // Set repeat time.
    return mpSelectedField->setRepeatTime( repeatTime );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getRepeatTime( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getRepeatTime() - No field selected." );
        return 0.0f;
    }

    // Get repeat time.
    return mpSelectedField->getRepeatTime();
}

//-----------------------------------------------------------------------------

bool ParticleAssetFieldCollection::setValueScale( const F32 valueScale )
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::setValueScale() - No field selected." );
        return false;
    }

    // Set Value Scale.
    return mpSelectedField->setValueScale( valueScale );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetFieldCollection::getValueScale( void ) const
{
    // Have we got a valid field selected?
    if ( !mpSelectedField )
    {
        // No, so warn.
        Con::warnf( "ParticleAssetFieldCollection::getValueScale() - No field selected." );
        return 0.0f;
    }

    // Get Value Scale.
    return mpSelectedField->getValueScale();
}

//------------------------------------------------------------------------------

void ParticleAssetFieldCollection::onTamlCustomWrite( TamlCustomNodes& customNodes )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetFieldCollection_OnTamlCustomWrite);

    // Finish if there no fields.
    if ( mFields.size() == 0 )
        return;

    // Add particle asset custom node.
    TamlCustomNode* pParticleAssetCustomNode = customNodes.addNode( particleAssetFieldNodeName );

    // Iterate the fields.
    for( typeFieldHash::iterator fieldItr = mFields.begin(); fieldItr != mFields.end(); ++fieldItr )
    {
        fieldItr->value->onTamlCustomWrite( pParticleAssetCustomNode );
    }
}

//-----------------------------------------------------------------------------

void ParticleAssetFieldCollection::onTamlCustomRead( const TamlCustomNodes& customNodes, const char* pOwnerName )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetFieldCollection_OnTamlCustomRead);

    // Find the particle asset custom node.
    const TamlCustomNode* pParticleAssetCustomNode = customNodes.findNode( particleAssetFieldNodeName );

    // Finish if we don't have a custom node.
    if ( pParticleAssetCustomNode == NULL )
        return;

    // Fetch children.
    const TamlCustomNodeVector& children = pParticleAssetCustomNode->getChildren();

    // Iterate the custom properties.
    for( TamlCustomNodeVector::const_iterator childNodeItr = children.begin(); childNodeItr != children.end(); ++childNodeItr )
    {
        // Fetch child node.
        TamlCustomNode* pChildNode = *childNodeItr;

        // Fetch node name.
        StringTableEntry nodeName = pChildNode->getNodeName();

        // Find the field.
        ParticleAssetField* pParticleAssetField = findField( nodeName );

        // Did we find the field?
        if ( pParticleAssetField == NULL )
        {
            // No, so warn.
            Con::warnf( "ParticleAssetFieldCollection::onTamlCustomRead() - Cannot find data field '%s'.", nodeName );
            continue;
        }

        // Read the alias.
        pParticleAssetField->onTamlCustomRead( pChildNode, pOwnerName );
    }
}

//-----------------------------------------------------------------------------

void ParticleAssetFieldCollection::WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement ) const
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "ParticleAssetFieldCollection::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "ParticleAssetFieldCollection::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    char buffer[1024];

    // Create Fields node element.
    TiXmlElement* pFieldsNodeElement = new TiXmlElement( "xs:element" );
    dSprintf( buffer, sizeof(buffer), "%s.%s", pClassRep->getClassName(), particleAssetFieldNodeName );
    pFieldsNodeElement->SetAttribute( "name", buffer );
    pFieldsNodeElement->SetAttribute( "minOccurs", 0 );
    pFieldsNodeElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pFieldsNodeElement );

    // Create complex type.
    TiXmlElement* pFieldsNodeComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pFieldsNodeElement->LinkEndChild( pFieldsNodeComplexTypeElement );
    
    // Create choice element.
    TiXmlElement* pFieldsNodeChoiceElement = new TiXmlElement( "xs:choice" );
    pFieldsNodeChoiceElement->SetAttribute( "minOccurs", 0 );
    pFieldsNodeChoiceElement->SetAttribute( "maxOccurs", "unbounded" );
    pFieldsNodeComplexTypeElement->LinkEndChild( pFieldsNodeChoiceElement );

    // Iterate the fields.
    for( typeFieldHash::const_iterator fieldItr = mFields.begin(); fieldItr != mFields.end(); ++fieldItr )
    {
        // Write schema for the field.
        fieldItr->value->WriteCustomTamlSchema( pClassRep, pFieldsNodeChoiceElement );
    }
}


