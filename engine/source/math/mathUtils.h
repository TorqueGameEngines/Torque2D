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

#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

class Point3F;
class MatrixF;
struct Vector2;

#ifndef _MPOINT_H_
#include "mPoint.h"
#endif

/// Creates orientation matrix from a direction vector.  Assumes ( 0 0 1 ) is up.
MatrixF createOrientFromDir( Point3F &direction );

/// Creates random direction given angle parameters similar to the particle system.
///
/// The angles are relative to the specified axis. Both phi and theta are in degrees.
Point3F randomDir( Point3F &axis, F32 thetaAngleMin, F32 thetaAngleMax, F32 phiAngleMin = 0.0, F32 phiAngleMax = 360.0 );

/// Returns yaw and pitch angles from a given vector.
///
/// Angles are in RADIANS.
///
/// Assumes north is (0.0, 1.0, 0.0), the degrees move upwards clockwise.
///
/// The range of yaw is 0 - 2PI.  The range of pitch is -PI/2 - PI/2.
///
/// <b>ASSUMES Z AXIS IS UP</b>
void    getAnglesFromVector( VectorF &vec, F32 &yawAng, F32 &pitchAng );

/// Returns vector from given yaw and pitch angles.
///
/// Angles are in RADIANS.
///
/// Assumes north is (0.0, 1.0, 0.0), the degrees move upwards clockwise.
///
/// The range of yaw is 0 - 2PI.  The range of pitch is -PI/2 - PI/2.
///
/// <b>ASSUMES Z AXIS IS UP</b>
void    getVectorFromAngles( VectorF &vec, F32 &yawAng, F32 &pitchAng );

/// Returns a point on the given line ab that is closest to 'point'.  2D version.
/// @param a The start of the line.
/// @param b The end of the line.
/// @param point The point to test with
/// @return A Point2F of the nearest point that lies on the line
Point2F getClosestPointOnLine( Point2F &a, Point2F &b, Point2F &point);

/// Returns an objects position when rotated around a pivot point.
Point2F rotateAtPoint(Point2F &pivot, Point2F &objPos, F32 &ang);

#endif // _MATHUTILS_H_
