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

#ifndef _PARTICLE_ASSET_FIELD_COLLECTION_H_
#define _PARTICLE_ASSET_FIELD_COLLECTION_H_

#ifndef _PARTICLE_ASSET_FIELD_H_
#include "2d/assets/ParticleAssetField.h"
#endif

#ifndef _HASHTABLE_H_
#include "collection/hashTable.h"
#endif

#ifndef _TAML_CUSTOM_H_
#include "persistence/taml/tamlCustom.h"
#endif

///-----------------------------------------------------------------------------

class ParticleAssetFieldCollection
{
public:
    typedef HashMap<StringTableEntry, ParticleAssetField*> typeFieldHash;

private:
    typeFieldHash       mFields;
    ParticleAssetField* mpSelectedField;

public:
    ParticleAssetFieldCollection();
    virtual ~ParticleAssetFieldCollection();

    void copyTo( ParticleAssetFieldCollection& fieldCollection );

    void addField( ParticleAssetField& particleAssetField, const char* pFieldName, F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue );

    ParticleAssetField* selectField( const char* pFieldName );
    inline void deselectField( void ) { mpSelectedField = NULL; }
    inline const ParticleAssetField* getSelectedField( void ) const { return mpSelectedField; }

    inline const typeFieldHash& getFields( void ) const { return mFields; }
    ParticleAssetField* findField( const char* pFieldName );

    S32 setSingleDataKey( const F32 value );
    S32 addDataKey( F32 time, F32 value );
    bool removeDataKey( S32 index );
    bool clearDataKeys( void );
    bool setDataKey( S32 index, F32 value );

    F32 getDataKeyValue( S32 index ) const;
    F32 getDataKeyTime( S32 index ) const;
    const ParticleAssetField::DataKey& getDataKey( const U32 index ) const;
    U32 getDataKeyCount( void ) const;
    F32 getMinValue( void ) const;
    F32 getMaxValue( void ) const;
    F32 getMinTime( void ) const;
    F32 getMaxTime( void ) const;
    F32 getFieldValue( F32 time ) const;
    bool setRepeatTime( const F32 repeatTime );
    F32 getRepeatTime( void ) const;
    bool setValueScale( const F32 valueScale );
    F32 getValueScale( void ) const;    

    void onTamlCustomWrite( TamlCustomNodes& customNodes );
    void onTamlCustomRead( const TamlCustomNodes& customNodes );

    void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement ) const;
};

#endif // ParticleAssetFieldCollection
