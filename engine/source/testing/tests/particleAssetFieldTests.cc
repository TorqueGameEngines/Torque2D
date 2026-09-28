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

// We don't want tests in a shipping version.
#ifndef TORQUE_SHIPPING

#ifndef _UNIT_TESTING_H_
#include "testing/unitTesting.h"
#endif

#ifndef _PARTICLE_ASSET_FIELD_H_
#include "2d/assets/ParticleAssetField.h"
#endif

#ifndef _TAML_CUSTOM_H_
#include "persistence/taml/tamlCustom.h"
#endif

//-----------------------------------------------------------------------------
// A particle field's keys, as a file hands them over
//
// ParticleAssetField::getFieldValue finds the two keys either side of a time by
// walking the keys in order, and takes the first key to be at time 0: a time
// before it has no key on its left, and the search stepped off the front of the
// array. The editor never writes such a field, but nothing stopped a file from
// saying one -- a curve starting at 0.6, or its keys out of order -- and the
// asset loaded without complaint and crashed the first time it was sampled
// early.
//-----------------------------------------------------------------------------

// A field with room on its time axis for keys past 1, as an emitter's base
// fields have.
static void initializeField( ParticleAssetField& field )
{
    field.setFieldName( "SizeX" );
    field.initialize( 10.0f, 0.0f, 100.0f, 1.0f );
}

static void addKey( TamlCustomNode* pFieldNode, const F32 time, const F32 value )
{
    TamlCustomNode* pKeyNode = pFieldNode->addNode( "Key" );
    pKeyNode->addField( "Time", time );
    pKeyNode->addField( "Value", value );
}

TEST( ParticleAssetFieldTests, AFieldWithNoKeyAtTimeZeroGainsOne )
{
    TamlCustomNodes nodes;
    TamlCustomNode* pFieldNode = nodes.addNode( "SizeX" );
    addKey( pFieldNode, 0.6f, 0.4f );
    addKey( pFieldNode, 1.5f, 1.9f );

    ParticleAssetField field;
    initializeField( field );
    field.onTamlCustomRead( pFieldNode, "Unit test asset" );

    ASSERT_EQ( field.getDataKeyCount(), 3 );
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 0 ), 0.0f );
    ASSERT_FLOAT_EQ( field.getDataKeyValue( 0 ), 0.4f ) << "The curve is flat up to its first key.";
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 1 ), 0.6f );
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 2 ), 1.5f );

    SUCCEED();
}

TEST( ParticleAssetFieldTests, ATimeBeforeTheFirstKeyReadsTheFirstKeysValue )
{
    TamlCustomNodes nodes;
    TamlCustomNode* pFieldNode = nodes.addNode( "SizeX" );
    addKey( pFieldNode, 0.6f, 0.4f );
    addKey( pFieldNode, 1.5f, 1.9f );

    ParticleAssetField field;
    initializeField( field );
    field.onTamlCustomRead( pFieldNode, "Unit test asset" );

    ASSERT_FLOAT_EQ( field.getFieldValue( 0.3f ), 0.4f );

    SUCCEED();
}

TEST( ParticleAssetFieldTests, KeysReadOutOfOrderAreSorted )
{
    TamlCustomNodes nodes;
    TamlCustomNode* pFieldNode = nodes.addNode( "SizeX" );
    addKey( pFieldNode, 1.0f, 5.0f );
    addKey( pFieldNode, 0.0f, 1.0f );
    addKey( pFieldNode, 0.5f, 3.0f );

    ParticleAssetField field;
    initializeField( field );
    field.onTamlCustomRead( pFieldNode, "Unit test asset" );

    ASSERT_EQ( field.getDataKeyCount(), 3 );
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 0 ), 0.0f );
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 1 ), 0.5f );
    ASSERT_FLOAT_EQ( field.getDataKeyTime( 2 ), 1.0f );
    ASSERT_FLOAT_EQ( field.getFieldValue( 0.25f ), 2.0f ) << "Halfway between the first two keys, once they are in order.";

    SUCCEED();
}

#endif // TORQUE_SHIPPING
