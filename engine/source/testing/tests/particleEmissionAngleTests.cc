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

#ifndef _PARTICLE_PLAYER_H_
#include "2d/sceneobject/ParticlePlayer.h"
#endif

//-----------------------------------------------------------------------------
// Where an emitter's emission angle meets the player's own rotation.
//
// Two angles in two different units meet on this line. An emitter's EmissionAngle
// is authored in degrees -- 90 means "straight up" in the editor -- and every step
// of the emission math downstream of it keeps degrees until the final mDegToRad
// that turns it into a velocity. A SceneObject's angle is radians: setAngle() takes
// radians, getAngle() hands back what Box2D holds, and only the script binding
// converts on the way in and out.
//
// So the sum has to convert, and for the life of the engine it did not: it added
// radians to degrees, which scaled every linked rotation by pi/180 and left
// LinkEmissionRotation looking like it did nothing at all. These tests are here to
// keep the conversion in place -- the arithmetic is trivial, being wrong about it
// was not.
//
// Tested through the static rather than through a real player: emitting a particle
// wants a scene, a particle asset and an image behind it, none of which this suite
// has. What is left here is the part that was wrong.
//-----------------------------------------------------------------------------

TEST( ParticleEmissionAngleTests, UnlinkedIgnoresThePlayerRotation )
{
    // A quarter turn on the player, which an unlinked emitter must not see.
    const F32 playerAngle = mDegToRad( 90.0f );

    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 90.0f, playerAngle, false ), 90.0f );
    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 0.0f, playerAngle, false ), 0.0f );

    SUCCEED();
}

TEST( ParticleEmissionAngleTests, LinkedAddsThePlayerRotationInDegrees )
{
    // An emitter pointing straight up on a player turned a quarter turn: the two
    // add as degrees, so the particles leave pointing left, not 91.57 degrees.
    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 90.0f, mDegToRad( 90.0f ), true ), 180.0f );

    // The bug this replaced: radians added raw would have landed here.
    ASSERT_NE( ParticlePlayer::calculateEmissionAngle( 90.0f, mDegToRad( 90.0f ), true ), 90.0f + mDegToRad( 90.0f ) );

    SUCCEED();
}

TEST( ParticleEmissionAngleTests, LinkedAtRestChangesNothing )
{
    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 90.0f, 0.0f, true ), 90.0f );

    SUCCEED();
}

TEST( ParticleEmissionAngleTests, LinkedFollowsANegativeRotation )
{
    // Turning the player the other way turns the emission the other way with it.
    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 90.0f, mDegToRad( -45.0f ), true ), 45.0f );

    // A full turn comes back to where it started. The wrap to 0..360 happens after
    // the arc is applied, so this deliberately reads as 450 rather than 90.
    ASSERT_FLOAT_EQ( ParticlePlayer::calculateEmissionAngle( 90.0f, mDegToRad( 360.0f ), true ), 450.0f );

    SUCCEED();
}

#endif // TORQUE_SHIPPING
