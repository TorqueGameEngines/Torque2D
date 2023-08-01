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

#ifndef _MRECT_H_
#define _MRECT_H_

//Includes
#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

class RectI
{
  public:
   Point2I  point;
   Point2I  extent;

  public:
   RectI() { }
   RectI(const Point2I& in_rMin,
         const Point2I& in_rExtent);
   RectI(const S32 in_left,  const S32 in_top,
         const S32 in_width, const S32 in_height);

   void set(const Point2I& in_rMin, const Point2I& in_rExtent);
   void set(const S32 in_left,  const S32 in_top,
         const S32 in_width, const S32 in_height);

   bool intersect(const RectI& clipRect);
   bool pointInRect(const Point2I& pt) const;
   S32 distanceToRect(const Point2I& pt) const;
   bool contains(const RectI& R) const;
   bool overlaps(RectI R) const;
   void inset(S32 x, S32 y);

   void unionRects(const RectI&);

   Point2I centre( void ) const;
   S32   len_x() const;
   S32   len_y() const;

   bool operator==(const RectI&) const;
   bool operator!=(const RectI&) const;

   bool isValidRect() const { return (extent.x > 0 && extent.y > 0); }
};

class RectF
{
  public:
   Point2F  point;
   Point2F  extent;

  public:
   RectF() { }
   RectF(const Point2F& in_rMin,
         const Point2F& in_rExtent);
   RectF(const F32 in_left,  const F32 in_top,
         const F32 in_width, const F32 in_height);
   void inset(F32 x, F32 y);

   bool intersect(const RectF& clipRect);
   bool overlaps(const RectF&) const;

   Point2F centre( void ) const;
   F32 len_x() const;
   F32 len_y() const;

   bool isValidRect() const { return (extent.x > 0 && extent.y > 0); }
};

class RectD
{
  public:
   Point2D  point;
   Point2D  extent;

  public:
   RectD() { }
   RectD(const Point2D& in_rMin,
         const Point2D& in_rExtent);
   RectD(const F64 in_left,  const F64 in_top,
         const F64 in_width, const F64 in_height);
   void inset(F64 x, F64 y);

   bool intersect(const RectD& clipRect);

   Point2D centre( void ) const;
   F64 len_x() const;
   F64 len_y() const;

   bool isValidRect() const { return (extent.x > 0 && extent.y > 0); }
};

//------------------------------------------------------------------------------
//-------------------------------------- INLINES (RectI)
//
inline
RectI::RectI(const Point2I& in_rMin,
             const Point2I& in_rExtent)
 : point(in_rMin),
   extent(in_rExtent)
{
   //
}

inline
RectI::RectI(const S32 in_left,  const S32 in_top,
             const S32 in_width, const S32 in_height)
 : point(in_left,  in_top),
   extent(in_width, in_height)
{
   //
}

inline void RectI::set(const Point2I& in_rMin, const Point2I& in_rExtent)
{
   point = in_rMin;
   extent = in_rExtent;
}

inline void RectI::set(const S32 in_left,  const S32 in_top,
                      const S32 in_width, const S32 in_height)
{
   point.set(in_left,  in_top);
   extent.set(in_width, in_height);
}

inline bool RectI::intersect(const RectI& clipRect)
{
   Point2I bottomL;
   bottomL.x = getMin(point.x + extent.x - 1, clipRect.point.x + clipRect.extent.x - 1);
   bottomL.y = getMin(point.y + extent.y - 1, clipRect.point.y + clipRect.extent.y - 1);

   point.x = getMax(point.x, clipRect.point.x);
   point.y = getMax(point.y, clipRect.point.y);

   extent.x = bottomL.x - point.x + 1;
   extent.y = bottomL.y - point.y + 1;

   return isValidRect();
}

inline bool RectI::pointInRect(const Point2I &pt) const
{
   return (pt.x >= point.x && pt.x < point.x + extent.x && pt.y >= point.y && pt.y < point.y + extent.y);
}

inline S32 RectI::distanceToRect(const Point2I& pt) const
{
	if (pointInRect(pt))
	{
		return 0;
	}
	S32 x = pt.x < point.x ? point.x - pt.x : pt.x - (point.x + extent.x);
	S32 y = pt.y < point.y ? point.y - pt.y : pt.y - (point.y + extent.y);

	return getMax(x, y);
}

inline bool RectI::contains(const RectI& R) const
{
   if (point.x <= R.point.x && point.y <= R.point.y)
    if (R.point.x + R.extent.x <= point.x + extent.x)
     if (R.point.y + R.extent.y <= point.y + extent.y)
      return true;
   return false;
}

inline bool RectI::overlaps(RectI R) const
{
   return R.intersect (* this);
}

inline void RectI::inset(S32 x, S32 y)
{
   point.x += x;
   point.y += y;
   extent.x -= 2 * x;
   extent.y -= 2 * y;
}

inline void RectF::inset(F32 x, F32 y)
{
   point.x += x;
   point.y += y;
   extent.x -= 2 * x;
   extent.y -= 2 * y;
}

inline void RectD::inset(F64 x, F64 y)
{
   point.x += x;
   point.y += y;
   extent.x -= 2 * x;
   extent.y -= 2 * y;
}


inline void RectI::unionRects(const RectI& u)
{
   S32 minx = point.x < u.point.x ? point.x : u.point.x;
   S32 miny = point.y < u.point.y ? point.y : u.point.y;
   S32 maxx = (point.x + extent.x) > (u.point.x + u.extent.x) ? (point.x + extent.x) : (u.point.x + u.extent.x);
   S32 maxy = (point.y + extent.y) > (u.point.y + u.extent.y) ? (point.y + extent.y) : (u.point.y + u.extent.y);

   point.x  = minx;
   point.y  = miny;
   extent.x = maxx - minx;
   extent.y = maxy - miny;
}

inline Point2I RectI::centre( void ) const
{
    return Point2I( point.x + (S32)(extent.x * 0.5f), point.y + (S32)(extent.y * 0.5f) );
}

inline S32
RectI::len_x() const
{
   return extent.x;
}

inline S32
RectI::len_y() const
{
   return extent.y;
}

inline bool
RectI::operator==(const RectI& in_rCompare) const
{
   return (point == in_rCompare.point) && (extent == in_rCompare.extent);
}

inline bool
RectI::operator!=(const RectI& in_rCompare) const
{
   return (operator==(in_rCompare) == false);
}

//------------------------------------------------------------------------------
//-------------------------------------- INLINES (RectF)
//
inline
RectF::RectF(const Point2F& in_rMin,
             const Point2F& in_rExtent)
 : point(in_rMin),
   extent(in_rExtent)
{
   //
}

inline
RectF::RectF(const F32 in_left,  const F32 in_top,
             const F32 in_width, const F32 in_height)
 : point(in_left,  in_top),
   extent(in_width, in_height)
{
   //
}

inline Point2F RectF::centre( void ) const
{
    return Point2F( point.x + (extent.x * 0.5f), point.y + (extent.y * 0.5f) );
}

inline F32
RectF::len_x() const
{
   return extent.x;
}

inline F32
RectF::len_y() const
{
   return extent.y;
}

inline bool RectF::intersect(const RectF& clipRect)
{
   Point2F bottomL;
   bottomL.x = getMin(point.x + extent.x, clipRect.point.x + clipRect.extent.x);
   bottomL.y = getMin(point.y + extent.y, clipRect.point.y + clipRect.extent.y);

   point.x = getMax(point.x, clipRect.point.x);
   point.y = getMax(point.y, clipRect.point.y);

   extent.x = bottomL.x - point.x;
   extent.y = bottomL.y - point.y;

   return isValidRect();
}


inline bool RectF::overlaps(const RectF& clipRect) const
{
   RectF test = *this;
   return test.intersect(clipRect);
}


//------------------------------------------------------------------------------
//-------------------------------------- INLINES (RectD)
//
inline
RectD::RectD(const Point2D& in_rMin,
             const Point2D& in_rExtent)
 : point(in_rMin),
   extent(in_rExtent)
{
   //
}

inline
RectD::RectD(const F64 in_left,  const F64 in_top,
             const F64 in_width, const F64 in_height)
 : point(in_left,  in_top),
   extent(in_width, in_height)
{
   //
}

inline Point2D RectD::centre( void ) const
{
    return Point2D( point.x + (F64)(extent.x * 0.5), point.y + (F64)(extent.y * 0.5) );
}

inline F64
RectD::len_x() const
{
   return extent.x;
}

inline F64
RectD::len_y() const
{
   return extent.y;
}

inline bool RectD::intersect(const RectD& clipRect)
{
   Point2D bottomL;
   bottomL.x = getMin(point.x + extent.x, clipRect.point.x + clipRect.extent.x);
   bottomL.y = getMin(point.y + extent.y, clipRect.point.y + clipRect.extent.y);

   point.x = getMax(point.x, clipRect.point.x);
   point.y = getMax(point.y, clipRect.point.y);

   extent.x = bottomL.x - point.x;
   extent.y = bottomL.y - point.y;

   return isValidRect();
}

#endif //_RECT_H_
