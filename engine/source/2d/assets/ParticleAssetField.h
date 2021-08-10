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

#ifndef _PARTICLE_ASSET_FIELD_H_
#define _PARTICLE_ASSET_FIELD_H_

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _TAML_CUSTOM_H_
#include "persistence/taml/tamlCustom.h"
#endif

///-----------------------------------------------------------------------------

class ParticleAssetField
{
public:
    /// Data Key Node.
    struct DataKey
    {
        DataKey() {}
        DataKey( const F32 time, const F32 value ) : mTime( time ), mValue( value ) {}

        bool operator==( const DataKey& dataKey ) const
        {
            return mIsEqual( mTime, dataKey.mTime ) && mIsEqual( mValue, dataKey.mValue );
        }

        bool operator!=( const DataKey& dataKey ) const
        {
            return mNotEqual( mTime, dataKey.mTime ) || mNotEqual( mValue, dataKey.mValue );
        }

        F32     mTime;
        F32     mValue;
    };

    static ParticleAssetField::DataKey BadDataKey;

private:
    StringTableEntry mFieldName;
    F32 mRepeatTime;
    F32 mMaxTime;
    F32 mMinValue;
    F32 mMaxValue;
    F32 mValueScale;
    F32 mDefaultValue;

    bool mValueBoundsDirty;

    Vector<DataKey> mDataKeys;

public:
    ParticleAssetField();
    virtual ~ParticleAssetField();

    void copyTo( ParticleAssetField& field );

    void initialize( const F32 maxTime, const F32 minValue, const F32 maxValue, const F32 defaultValue );
    void setValueBounds( F32 maxTime, F32 minValue, F32 maxValue, F32 defaultValue );

    void setFieldName( const char* pFieldName );
    inline StringTableEntry getFieldName( void ) const { return mFieldName; }
    bool setValueScale( const F32 valueScale );
    inline F32 getValueScale( void ) { return mValueScale; }
    bool setRepeatTime( const F32 repeatTime );
    inline F32 getRepeatTime( void ) const { return mRepeatTime; }
    inline F32 getMinValue( void ) const { return mMinValue; };
    inline F32 getMaxValue( void ) const { return mMaxValue; };
    inline F32 getMinTime( void ) const { return 0.0f; }
    inline F32 getMaxTime( void ) const { return mMaxTime; };
    inline F32 getValueScale( void ) const { return mValueScale; };
    inline F32 getDefaultValue( void ) const { return mDefaultValue; }

    void resetDataKeys( void );
    S32 setSingleDataKey( const F32 value );
	bool doesKeyExist(const F32 time);
    S32 addDataKey( const F32 time, const F32 value );
    bool removeDataKey( const U32 index );
    void clearDataKeys( void );
    bool setDataKeyValue( const U32 index, const F32 value );
    F32 getDataKeyValue( const U32 index ) const;
    F32 getDataKeyTime( const U32 index ) const;
    inline U32 getDataKeyCount( void ) const { return (U32)mDataKeys.size(); }
    const DataKey& getDataKey( const U32 index ) const;
    F32 getFieldValue( F32 time ) const;

    static F32 calculateFieldBV( const ParticleAssetField& base, const ParticleAssetField& variation, const F32 effectAge, const bool modulate = false, const F32 modulo = 0.0f );
    static F32 calculateFieldBVE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& effect, const F32 effectAge, const bool modulate = false, const F32 modulo = 0.0f );
    static F32 calculateFieldBVLE( const ParticleAssetField& base, const ParticleAssetField& variation, const ParticleAssetField& overlife, const ParticleAssetField& effect, const F32 effectTime, const F32 particleAge, const bool modulate = false, const F32 modulo = 0.0f );

    void onTamlCustomWrite( TamlCustomNode* pCustomNode  );
    void onTamlCustomRead( const TamlCustomNode* pCustomNode );

    void WriteCustomTamlSchema( const AbstractClassRep* pClassRep, TiXmlElement* pParentElement );
};

//-----------------------------------------------------------------------------

/// Base field.
class ParticleAssetFieldBase
{
private:
    ParticleAssetField mBase;

public:
    inline ParticleAssetField& getBase( void ) { return mBase; }

    virtual void copyTo( ParticleAssetFieldBase& particleField )
    {
        mBase.copyTo( particleField.getBase() );
    };
};

//-----------------------------------------------------------------------------

/// Life field.
class ParticleAssetFieldLife
{
private:
    ParticleAssetField mLife;

public:
    inline ParticleAssetField& getLife( void ) { return mLife; }

    void copyTo( ParticleAssetFieldLife& particleField )
    {
        mLife.copyTo( particleField.getLife() );
    };
};

//-----------------------------------------------------------------------------

/// Variation field.
class ParticleAssetFieldVariation
{
private:
    ParticleAssetField mVariation;

public:
    inline ParticleAssetField& getVariation( void ) { return mVariation; }

    virtual void copyTo( ParticleAssetFieldVariation& particleField )
    {
        mVariation.copyTo( particleField.getVariation() );
    };
};

//-----------------------------------------------------------------------------

/// Base and variation fields.
class ParticleAssetFieldBaseVariation :
    public ParticleAssetFieldBase,
    public ParticleAssetFieldVariation
{
public:
    void copyTo( ParticleAssetFieldBaseVariation& particleField )
    {
        getBase().copyTo( particleField.getBase() );
        getVariation().copyTo( particleField.getVariation() );
    };
};

//-----------------------------------------------------------------------------

/// Base, variation and life fields.
class ParticleAssetFieldBaseVariationLife :
    public ParticleAssetFieldBase,
    public ParticleAssetFieldVariation,
    public ParticleAssetFieldLife
{
public:
    void copyTo( ParticleAssetFieldBaseVariationLife& particleField )
    {
        getBase().copyTo( particleField.getBase() );
        getVariation().copyTo( particleField.getVariation() );
        getLife().copyTo( particleField.getLife() );
    };
};

#endif // _PARTICLE_ASSET_FIELD_H_
