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

#include "math/mMath.h"
#include "math/mathUtils.h"
#include "math/mRandom.h"
#include "2d/core/Vector2.h"

RandomLCG sgRandom(0xdeadbeef); ///< Our random number generator.

//------------------------------------------------------------------------------
// Creates orientation matrix from a direction vector.  Assumes ( 0 0 1 ) is up.
//------------------------------------------------------------------------------
MatrixF createOrientFromDir( Point3F &direction )
{
    Point3F j = direction;
    Point3F k(0.0, 0.0, 1.0);
    Point3F i;
    
    mCross( j, k, &i );

    if( i.magnitudeSafe() == 0.0 )
    {
        i = Point3F( 0.0, -1.0, 0.0 );
    }

    i.normalizeSafe();
    mCross( i, j, &k );

   MatrixF mat( true );
   mat.setColumn( 0, i );
   mat.setColumn( 1, j );
   mat.setColumn( 2, k );

    return mat;
}


//------------------------------------------------------------------------------
// Creates random direction given angle parameters similar to the particle system.
// The angles are relative to the specified axis.
//------------------------------------------------------------------------------
Point3F randomDir( Point3F &axis, F32 thetaAngleMin, F32 thetaAngleMax,
                                 F32 phiAngleMin, F32 phiAngleMax )
{
   MatrixF orient = createOrientFromDir( axis );
   Point3F axisx;
   orient.getColumn( 0, &axisx );

   F32 theta = (thetaAngleMax - thetaAngleMin) * sgRandom.randF() + thetaAngleMin;
   F32 phi = (phiAngleMax - phiAngleMin) * sgRandom.randF() + phiAngleMin;

   // Both phi and theta are in degs.  Create axis angles out of them, and create the
   //  appropriate rotation matrix...
   AngAxisF thetaRot(axisx, theta * ((F32)M_PI / 180.0f));
   AngAxisF phiRot(axis,    phi   * ((F32)M_PI / 180.0f));

   Point3F ejectionAxis = axis;

   MatrixF temp(true);
   thetaRot.setMatrix(&temp);
   temp.mulP(ejectionAxis);
   phiRot.setMatrix(&temp);
   temp.mulP(ejectionAxis);

   return ejectionAxis;
}


//------------------------------------------------------------------------------
// Returns yaw and pitch angles from a given vector.  Angles are in RADIANS.
// Assumes north is (0.0, 1.0, 0.0), the degrees move upwards clockwise.
// The range of yaw is 0 - 2PI.  The range of pitch is -PI/2 - PI/2.
// ASSUMES Z AXIS IS UP
//------------------------------------------------------------------------------
void getAnglesFromVector( VectorF &vec, F32 &yawAng, F32 &pitchAng )
{
   yawAng = mAtan( vec.x, vec.y );

   if( yawAng < 0.0 )
   {
      yawAng += (F32)M_2PI;
   }

   if( fabs(vec.x) > fabs(vec.y) )
   {
      pitchAng = mAtan( fabs(vec.z), fabs(vec.x) );
   }
   else
   {
      pitchAng = mAtan( fabs(vec.z), fabs(vec.y) );
   }

   if( vec.z < 0.0 )
   {
      pitchAng = -pitchAng;
   }

}


//------------------------------------------------------------------------------
// Returns vector from given yaw and pitch angles.  Angles are in RADIANS.
// Assumes north is (0.0, 1.0, 0.0), the degrees move upwards clockwise.
// The range of yaw is 0 - 2PI.  The range of pitch is -PI/2 - PI/2.
// ASSUMES Z AXIS IS UP
//------------------------------------------------------------------------------
void getVectorFromAngles( VectorF &vec, F32 &yawAng, F32 &pitchAng )
{
   VectorF  pnt( 0.0, 1.0, 0.0 );

   EulerF   rot( -pitchAng, 0.0, 0.0 );
   MatrixF  mat( rot );

   rot.set( 0.0, 0.0, yawAng );
   MatrixF   mat2( rot );

   mat.mulV( pnt );
   mat2.mulV( pnt );

   vec = pnt;
}

Point2F rotateAtPoint(Point2F & pivot, Point2F & objPos, F32 & ang)
{
   F32 s = mSin(ang);
   F32 c = mCos(ang);

   Point2F out = objPos;

   out.x -= pivot.x;
   out.y -= pivot.y;

   F32 x = out.x * c - out.y * s;
   F32 y = out.x * s + out.y * c;

   out.x = x + pivot.x;
   out.y = y + pivot.y;

   return out;
}
