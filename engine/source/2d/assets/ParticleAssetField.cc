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

#include "2d/assets/ParticleAssetField.h"

#ifndef _CORE_MATH_H_
#include "2d/core/CoreMath.h"
#endif

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif

#ifndef _SIMBASE_H_
#include "sim/simBase.h"
#endif

#ifndef _STRINGUNIT_H_
#include "string/stringUnit.h"
#endif

//-----------------------------------------------------------------------------

static StringTableEntry particleAssetFieldRepeatTimeName   = StringTable->insert( "RepeatTime" );
static StringTableEntry particleAssetFieldMaxTimeName      = StringTable->insert( "MaxTime" );
static StringTableEntry particleAssetFieldMinValueName     = StringTable->insert( "MinValue" );
static StringTableEntry particleAssetFieldMaxValueName     = StringTable->insert( "MaxValue" );
static StringTableEntry particleAssetFieldDefaultValueName = StringTable->insert( "DefaultValue" );
static StringTableEntry particleAssetFieldValueScaleName   = StringTable->insert( "ValueScale" );
static StringTableEntry particleAssetFieldDataKeysName     = StringTable->insert( "Keys" );

static StringTableEntry particleAssetFieldDataKeyName      = StringTable->insert( "Key" );
static StringTableEntry particleAssetFieldDataKeyTimeName  = StringTable->insert( "Time" );
static StringTableEntry particleAssetFieldDataKeyValueName = StringTable->insert( "Value" );

ParticleAssetField::DataKey ParticleAssetField::BadDataKey( -1.0f, 0.0f );

//-----------------------------------------------------------------------------

ParticleAssetField::ParticleAssetField() :
                        mFieldName( StringTable->EmptyString ),
                        mRepeatTime( 1.0f ),
                        mMaxTime( 1.0f ),
                        mMinValue( 0.0f ),
                        mMaxValue( 0.0f ),
                        mDefaultValue( 1.0f ),
                        mValueScale( 1.0f ),
                        mValueBoundsDirty( true )
{
    // Set Vector Associations.
    VECTOR_SET_ASSOCIATION( mDataKeys );
}

//-----------------------------------------------------------------------------

ParticleAssetField::~ParticleAssetField()
{

}

//-----------------------------------------------------------------------------

void ParticleAssetField::copyTo( ParticleAssetField& field )
{
    field.mFieldName = mFieldName;
    field.mRepeatTime = mRepeatTime;
    field.mMaxTime = mMaxTime;
    field.mMinValue = mMinValue;
    field.mMaxValue = mMaxValue;
    field.mDefaultValue = mDefaultValue;
    field.mValueScale = mValueScale;

    // Copy data keys.    
    field.clearDataKeys();
    for ( S32 i = 0; i < mDataKeys.size(); i++ )
    {
        DataKey key = mDataKeys[i];
        field.addDataKey(key.mTime, key.mValue);
    }
}

//-----------------------------------------------------------------------------

void ParticleAssetField::initialize( const F32 maxTime, const F32 minValue, const F32 maxValue, const F32 defaultValue )
{
    // Set the value bounds.
    setValueBounds( maxTime, minValue, maxValue, defaultValue );

    // Reset the value bounds dirty flag.
    mValueBoundsDirty = false;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::setValueBounds( F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue )
{
    // Check Max Time.
    if ( maxTime <= 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Max-time '%f' is invalid", maxTime );

        // Set Default Max Time.
        maxTime = 1.0f;
    }

    // Set Max Time.
    mMaxTime = maxTime;

    // Check Value Range Normalisation.
    if ( minValue > maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Value Range is not normalised! (minValue:%f / maxValue:%f)", minValue, maxValue );

        // Normalise Y-Axis.
        F32 temp = minValue;
        minValue = maxValue;
        maxValue = temp;
    }
    // Check Value Range Scale.
    else if ( minValue == maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Value Range has no scale! (minValue:%f / maxValue:%f)", minValue, maxValue );

        // Insert some Y-Axis Scale.
        maxValue = minValue + 0.001f;
    }

    // Set Bounds.
    mMinValue = minValue;
    mMaxValue = maxValue;

    // Check Default Value.
    if ( defaultValue < minValue || defaultValue > maxValue )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueBounds() - Default Value is out of range! (minValue:%f / maxValue:%f / defaultValue:%f)", minValue, maxValue, defaultValue );

        // Clamp at lower value.
        defaultValue = minValue;
    }

    // Set Default Value.
    mDefaultValue = defaultValue;

    // Reset the data keys if none are present.
    if ( mDataKeys.size() == 0 )
        resetDataKeys();

    // Flag the value bounds as dirty.
    mValueBoundsDirty = true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::setFieldName( const char* pFieldName )
{
    // Sanity!
    AssertFatal( mFieldName == StringTable->EmptyString, "ParticleAssetField::setFieldName() - Cannot set particle asset field name once it has been set." );

    mFieldName = StringTable->insert( pFieldName );

    // Sanity!
    AssertFatal( mFieldName != StringTable->EmptyString, "ParticleAssetField::setFieldName() - Field name cannot be empty." );
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setRepeatTime( const F32 repeatTime )
{
    // Check repeat time.
    if ( repeatTime < 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setRepeatTime() - Repeat time''%f'' is invalid.", repeatTime );

        // Return Error.
        return false;
    }

    // Set repeat time.
    mRepeatTime = repeatTime;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setValueScale( const F32 valueScale )
{
    // Check Value Scale.
    if ( valueScale < 0.0f )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setValueScale() - Invalid Value Scale! (%f)", valueScale );

        // Return Error.
        return false;
    }

    // Set Value Scale/
    mValueScale = valueScale;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::resetDataKeys(void)
{
    // Clear Data Keys.
    mDataKeys.clear();

    // Add default value Data-Key.
    addDataKey( 0.0f, mDefaultValue );
}

//-----------------------------------------------------------------------------

S32 ParticleAssetField::setSingleDataKey( const F32 value )
{
    // Clear Data Keys.
    mDataKeys.clear();

    // Add a single key with the specified value.
    return addDataKey( 0.0f, value );
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::doesKeyExist(const F32 time)
{
	U32 index = 0;
	for (index = 0; index < getDataKeyCount(); index++)
	{
		// Found Time?
		if (mDataKeys[index].mTime == time)
		{
			return true;
		}
		// Past Time?
		else if (mDataKeys[index].mTime > time)
			// Finish search.
			break;
	}
	return false;
}

//-----------------------------------------------------------------------------

S32 ParticleAssetField::addDataKey( const F32 time, const F32 value )
{
    // Check Max Time.
    if ( time > mMaxTime )
    {
        // Warn.
        Con::warnf("ParticleAssetField::addDataKey() - Time is out of bounds! (time:%f)", time );

        // Return Error.
        return -1;
    }

    // If data key exists already then set it and return the key index.
    U32 index = 0;
    for ( index = 0; index < getDataKeyCount(); index++ )
    {
        // Found Time?
        if ( mDataKeys[index].mTime == time )
        {
            // Yes, so set time.
            mDataKeys[index].mValue = value;

            // Return Index.
            return index;
        }
        // Past Time?
        else if ( mDataKeys[index].mTime > time )
            // Finish search.
            break;
    }

    // Insert Data-Key.
    mDataKeys.insert( index );

    // Set Data-Key.
    mDataKeys[index].mTime = time;
    mDataKeys[index].mValue = value;

    // Return Index.
    return index;
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::removeDataKey( const U32 index )
{
    // Cannot Remove First Node!
    if ( index == 0 )
    {
        // Warn.
        Con::warnf("rParticleAssetField::emoveDataKey() - Cannot remove first Data-Key!");
        return false;
    }

    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("rParticleAssetField::emoveDataKey() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return false;
    }

    // Remove Index.
    mDataKeys.erase(index);

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::clearDataKeys( void )
{
    // Reset Data Keys.
    resetDataKeys();
}

//-----------------------------------------------------------------------------

bool ParticleAssetField::setDataKeyValue( const U32 index, const F32 value )
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::setDataKeyValue() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return false;
    }

    // Set Data Key Value.
    mDataKeys[index].mValue = value;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getDataKeyValue( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKeyValue() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return 0.0f;
    }

    // Return Data Key Value.
    return mDataKeys[index].mValue;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getDataKeyTime( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKeyTime() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return 0.0f;
    }

    // Return Data Key Time.
    return mDataKeys[index].mTime;
}

//-----------------------------------------------------------------------------

const ParticleAssetField::DataKey& ParticleAssetField::getDataKey( const U32 index ) const
{
    // Check Index.
    if ( index >= getDataKeyCount() )
    {
        // Warn.
        Con::warnf("ParticleAssetField::getDataKey() - Index out of range! (%d of %d)", index, getDataKeyCount()-1);
        return BadDataKey;
    }

    // Return Data-Key.
    return mDataKeys[index];
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::getFieldValue( F32 time ) const
{
    // Return First Entry if it's the only one or we're using zero time.
    if ( mIsZero(time) || getDataKeyCount() < 2)
        return mDataKeys[0].mValue * mValueScale;

    // Clamp Key-Time.
    time = getMin(getMax( 0.0f, time ), mMaxTime);

    // Repeat Time.
    time = mFmod( time * mRepeatTime, mMaxTime + FLT_EPSILON );

    // Fetch Max Key Index.
    const U32 maxKeyIndex = getDataKeyCount()-1;

    // Return Last Value if we're on/past the last time.
    if ( time >= mDataKeys[maxKeyIndex].mTime )
        return mDataKeys[maxKeyIndex].mValue * mValueScale;

    // Find Data-Key Indexes.
    U32 index1;
    U32 index2;
    for ( index1 = 0; index1 < getDataKeyCount(); index1++ )
        if ( mDataKeys[index1].mTime >= time )
            break;

    // If we're exactly on a Data-Key then return that key.
    if ( mIsEqual( mDataKeys[index1].mTime, time) )
        return mDataKeys[index1].mValue * mValueScale;

    // Set Adjacent Indexes.
    index2 = index1--;

    // Fetch Index Times.
    const F32 time1 = mDataKeys[index1].mTime;
    const F32 time2 = mDataKeys[index2].mTime;
    // Calculate Time Differential.
    const F32 dTime = (time-time1)/(time2-time1);

    // Return lerped Value.
    return ((mDataKeys[index1].mValue * (1.0f-dTime)) + (mDataKeys[index2].mValue * dTime)) * mValueScale;
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBV( const ParticleAssetField& base, const ParticleAssetField& variation, const F32 effectAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( baseValue + CoreMath::mGetRandomF(-varValue, varValue), modulo );
    else
        // Return Clamped Calculation.
        return mClampF( baseValue + CoreMath::mGetRandomF(-varValue, varValue), base.getMinValue(), base.getMaxValue() );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBVE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& effect, const F32 effectAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;
    const F32 effectValue = effect.getFieldValue( effectAge );

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue, modulo );
    else
        // Return Clamped Calculation.
        return mClampF( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue, base.getMinValue(), base.getMaxValue() );
}

//-----------------------------------------------------------------------------

F32 ParticleAssetField::calculateFieldBVLE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& overlife, const ParticleAssetField& effect, const F32 effectAge, const F32 particleAge, const bool modulate, const F32 modulo )
{
    // Fetch Graph Components.
    const F32 baseValue   = base.getFieldValue( effectAge );
    const F32 varValue    = variation.getFieldValue( effectAge ) * 0.5f;
    const F32 effectValue = effect.getFieldValue( effectAge );
    const F32 lifeValue   = overlife.getFieldValue( particleAge );

    // Modulate?
    if ( modulate )
        // Return Modulo Calculation.
        return mFmod( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue * lifeValue, modulo );
    else
        // Return Clamped Calculation.
        return mClampF( (baseValue + CoreMath::mGetRandomF(-varValue, varValue)) * effectValue * lifeValue, base.getMinValue(), base.getMaxValue() );
}

//------------------------------------------------------------------------------

void ParticleAssetField::onTamlCustomWrite( TamlCustomNode* pCustomNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetField_OnTamlCustomWrite);

    // Add a child (ignore it if there ends up being no children).
    TamlCustomNode* pAssetField = pCustomNode->addNode( getFieldName(), true );

    // Sanity!
    AssertFatal( pAssetField != NULL, "ParticleAssetField::onTamlCustomWrite() - Could not create field." );

    if ( mValueBoundsDirty && (mNotEqual( getMinValue(), 0.0f ) || mNotEqual( getMaxValue(), 0.0f )) )
    {
        pAssetField->addField( particleAssetFieldMinValueName, getMinValue() );
        pAssetField->addField( particleAssetFieldMaxValueName, getMaxValue() );
    }
    
    if ( mValueBoundsDirty && mNotEqual( getMaxTime(), 1.0f ) )
        pAssetField->addField( particleAssetFieldMaxTimeName, getMaxTime() );

    if ( mValueBoundsDirty && mNotEqual( getDefaultValue(), 1.0f ) )
        pAssetField->addField( particleAssetFieldDefaultValueName, getDefaultValue() );

    if ( mNotEqual( getValueScale(), 1.0f ) )
        pAssetField->addField( particleAssetFieldValueScaleName, getValueScale() );

    if ( mNotEqual( getRepeatTime(), 1.0f ) )
        pAssetField->addField( particleAssetFieldRepeatTimeName, getRepeatTime() );

    // Fetch key count.
    const U32 keyCount = getDataKeyCount();

    // Finish if no data keys.
    if ( keyCount == 0 )
        return;

    // Finish if there's only one key and it's the default one.
    if ( keyCount == 1 && mIsEqual(mDataKeys[0].mTime, 0.0f) && mIsEqual(mDataKeys[0].mValue, mDefaultValue) )
        return;

    // Iterate the keys.
    for( U32 index = 0; index < keyCount; ++index )
    {
        // Fetch the data key.
        const DataKey& dataKey = mDataKeys[index];

        // Add a key node.
        TamlCustomNode* pKeyNode = pAssetField->addNode( particleAssetFieldDataKeyName );

        // Add key fields.
        pKeyNode->addField( particleAssetFieldDataKeyTimeName, dataKey.mTime );
        pKeyNode->addField( particleAssetFieldDataKeyValueName, dataKey.mValue );
    }
}

//-----------------------------------------------------------------------------

void ParticleAssetField::onTamlCustomRead( const TamlCustomNode* pCustomNode )
{
    // Debug Profiling.
    PROFILE_SCOPE(ParticleAssetField_OnTamlCustomRead);

    // Fetch existing values.
    F32 repeatTime = getRepeatTime();
    F32 maxTime = getMaxTime();
    F32 minValue = getMinValue();
    F32 maxValue = getMaxValue();
    F32 defaultValue = getDefaultValue();
    F32 valueScale = getValueScale();

    // Set-up a temporary set of keys.
    Vector<DataKey> keys;

    // Clear the existing keys.
    mDataKeys.clear();

    // Fetch fields.
    const TamlCustomFieldVector& fields = pCustomNode->getFields();

    // Iterate fields.
    for ( TamlCustomFieldVector::const_iterator fieldItr = fields.begin(); fieldItr != fields.end(); ++fieldItr )
    {
        // Fetch field.
        TamlCustomField* pField = *fieldItr;

        // Fetch property field name.
        StringTableEntry fieldName = pField->getFieldName();

        if ( fieldName == particleAssetFieldRepeatTimeName )
        {
            pField->getFieldValue( repeatTime );
        }
        else if ( fieldName == particleAssetFieldMaxTimeName )
        {
            pField->getFieldValue( maxTime );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldMinValueName )
        {
            pField->getFieldValue( minValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldMaxValueName )
        {
            pField->getFieldValue( maxValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldDefaultValueName )
        {
            pField->getFieldValue( defaultValue );
            mValueBoundsDirty = true;
        }
        else if ( fieldName == particleAssetFieldValueScaleName )
        {
            pField->getFieldValue( valueScale );
        }
        else if ( fieldName == particleAssetFieldDataKeysName )
        {
            const char* pDataKeys = pField->getFieldValue();
            const S32 elementCount = StringUnit::getUnitCount( pDataKeys, " ,\t" );

            // Are there a valid number of elements?
            if ( elementCount < 2 || (elementCount % 2 ) != 0 )
            {
                // No, so warn.
                Con::warnf( "ParticleAssetField::onTamlCustomRead() - An invalid set of data keys was found." );
            }
            else
            {
                // Iterate the elements.
                for( S32 elementIndex = 0; elementIndex <= (elementCount-2); elementIndex += 2 )
                {
                    DataKey key;
                    key.mTime = dAtof( StringUnit::getUnit( pDataKeys, elementIndex, " ,\t" ) );
                    key.mValue = dAtof( StringUnit::getUnit( pDataKeys, elementIndex+1, " ,\t" ) );
                    keys.push_back( key );
                }
            }            
        }
    }

    // Fetch any children.
    const TamlCustomNodeVector& children = pCustomNode->getChildren();

    // Iterate node children.
    for( TamlCustomNodeVector::const_iterator childItr = children.begin(); childItr != children.end(); ++childItr )
    {
        // Fetch node.
        TamlCustomNode* pKeyNode = *childItr;

        // Ignore anything that isn't a key.
        if ( pKeyNode->getNodeName() != particleAssetFieldDataKeyName )
            continue;

        // Fetch the fields.
        const TamlCustomField* pTimeField = pKeyNode->findField( particleAssetFieldDataKeyTimeName );
        const TamlCustomField* pValueField = pKeyNode->findField( particleAssetFieldDataKeyValueName );

        // Did we find the fields?
        if ( pTimeField == NULL || pValueField == NULL )
        {
            // No, so warn.
            Con::warnf("ParticleAssetField::onTamlCustomRead() - Found a key but it did not have a time and value field." );

            continue;
        }

        // Read key.
        DataKey key;
        pTimeField->getFieldValue( key.mTime );
        pValueField->getFieldValue( key.mValue );
        keys.push_back( key );
    }

    // If value bounds are present but no keys, assign the field its default values.
    if ( !keys.size() )
    {
        DataKey key;
        key.mTime = getMinTime();
        key.mValue = getDefaultValue();
        keys.push_back( key );
    }

    // Did we read in any value bounds?
    if ( mValueBoundsDirty )
    {
        // Set the value bounds.
        setValueBounds( maxTime, minValue, maxValue, defaultValue );
    }

    // Set the value scale.
    setValueScale( valueScale );

    // Set the repeat time.
    setRepeatTime( repeatTime );

    // Set the data keys.
    mDataKeys = keys;
}

//-----------------------------------------------------------------------------

void ParticleAssetField::WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement )
{
    // Sanity!
    AssertFatal( pClassRep != NULL,  "ParticleAssetField::WriteCustomTamlSchema() - ClassRep cannot be NULL." );
    AssertFatal( pParentElement != NULL,  "ParticleAssetField::WriteCustomTamlSchema() - Parent Element cannot be NULL." );

    // Create Field element.
    TiXmlElement* pFieldElement = new TiXmlElement( "xs:element" );
    pFieldElement->SetAttribute( "name", getFieldName() );
    pFieldElement->SetAttribute( "minOccurs", 0 );
    pFieldElement->SetAttribute( "maxOccurs", 1 );
    pParentElement->LinkEndChild( pFieldElement );

    // Create complex type Element.
    TiXmlElement* pFieldComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pFieldElement->LinkEndChild( pFieldComplexTypeElement );

    // Create choice element.
    TiXmlElement* pFieldChoiceElement = new TiXmlElement( "xs:choice" );
    pFieldChoiceElement->SetAttribute( "minOccurs", 0 );
    pFieldChoiceElement->SetAttribute( "maxOccurs", 1 );
    pFieldComplexTypeElement->LinkEndChild( pFieldChoiceElement );

    // Create key element.
    TiXmlElement* pKeyElement = new TiXmlElement( "xs:element" );
    pKeyElement->SetAttribute( "name", particleAssetFieldDataKeyName );
    pKeyElement->SetAttribute( "minOccurs", 0 );
    pKeyElement->SetAttribute( "maxOccurs", "unbounded" );
    pFieldChoiceElement->LinkEndChild( pKeyElement );

    // Create complex type Element.
    TiXmlElement* pKeyComplexTypeElement = new TiXmlElement( "xs:complexType" );
    pKeyElement->LinkEndChild( pKeyComplexTypeElement );

    // Create "Time" attribute.
    TiXmlElement* pKeyTimeAttribute = new TiXmlElement( "xs:attribute" );
    pKeyTimeAttribute->SetAttribute( "name", particleAssetFieldDataKeyTimeName );
    pKeyComplexTypeElement->LinkEndChild( pKeyTimeAttribute );
    TiXmlElement* pKeyTimeSimpleType = new TiXmlElement( "xs:simpleType" );
    pKeyTimeAttribute->LinkEndChild( pKeyTimeSimpleType );
    TiXmlElement* pKeyTimeRestriction = new TiXmlElement( "xs:restriction" );
    pKeyTimeRestriction->SetAttribute( "base", "xs:float" );
    pKeyTimeSimpleType->LinkEndChild( pKeyTimeRestriction );
    TiXmlElement* pKeyTimeMinRestriction = new TiXmlElement( "xs:minInclusive" );
    pKeyTimeMinRestriction->SetAttribute( "value", "0" );
    pKeyTimeRestriction->LinkEndChild( pKeyTimeMinRestriction );

    // Create "Value" attribute.
    TiXmlElement* pKeyValueAttribute = new TiXmlElement( "xs:attribute" );
    pKeyValueAttribute->SetAttribute( "name", particleAssetFieldDataKeyValueName );
    pKeyValueAttribute->SetAttribute( "type", "xs:float" );
    pKeyComplexTypeElement->LinkEndChild( pKeyValueAttribute );

    // Create "Min Value" attribute.
    TiXmlElement* pFieldMinValue = new TiXmlElement( "xs:attribute" );
    pFieldMinValue->SetAttribute( "name", particleAssetFieldMinValueName );
    pFieldMinValue->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldMinValue );

    // Create "Max Value" attribute.
    TiXmlElement* pFieldMaxValue = new TiXmlElement( "xs:attribute" );
    pFieldMaxValue->SetAttribute( "name", particleAssetFieldMaxValueName );
    pFieldMaxValue->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldMaxValue );

    // Create "Max Time" attribute.
    TiXmlElement* pFieldMaxTime = new TiXmlElement( "xs:attribute" );
    pFieldMaxTime->SetAttribute( "name", particleAssetFieldMaxTimeName );
    pFieldMaxTime->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldMaxTime );

    // Create "Default Value" attribute.
    TiXmlElement* pFieldDefaultValue = new TiXmlElement( "xs:attribute" );
    pFieldDefaultValue->SetAttribute( "name", particleAssetFieldDefaultValueName );
    pFieldDefaultValue->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldDefaultValue );

    // Create "Value Scale" attribute.
    TiXmlElement* pFieldValueScale = new TiXmlElement( "xs:attribute" );
    pFieldValueScale->SetAttribute( "name", particleAssetFieldValueScaleName );
    pFieldValueScale->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldValueScale );

    // Create "Repeat Time" attribute.
    TiXmlElement* pFieldRepeatTime = new TiXmlElement( "xs:attribute" );
    pFieldRepeatTime->SetAttribute( "name", particleAssetFieldRepeatTimeName );
    pFieldRepeatTime->SetAttribute( "type", "xs:float" );
    pFieldComplexTypeElement->LinkEndChild( pFieldRepeatTime );
}
