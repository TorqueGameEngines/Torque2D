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

#ifndef _PLATFORM_MEMORY_H_
#define _PLATFORM_MEMORY_H_

//------------------------------------------------------------------------------

#define placenew(x) new(x)
#define dMalloc(x) dMalloc_r(x, __FILE__, __LINE__)
#define dRealloc(x, y) dRealloc_r(x, y, __FILE__, __LINE__)

//------------------------------------------------------------------------------

#if defined(TORQUE_OS_WIN)
extern void* FN_CDECL operator new(dsize_t size, void* ptr);
#endif

//------------------------------------------------------------------------------

template <class T> inline T* constructInPlace(T* p)
{
   return new(p) T;
}

//------------------------------------------------------------------------------

template <class T> inline T* constructInPlace(T* p, const T* copy)
{
   return new(p) T(*copy);
}


template< class T >
inline T* constructArrayInPlace( T* p, U32 num )
{
    return new ( p ) T[ num ];
}

//------------------------------------------------------------------------------

template <class T> inline void destructInPlace(T* p)
{
   p->~T();
}

//------------------------------------------------------------------------------

extern void* dMalloc_r(dsize_t in_size, const char*, const dsize_t);
extern void  dFree(void* in_pFree);
extern void* dRealloc_r(void* in_pResize, dsize_t in_size, const char*, const dsize_t);
extern void* dRealMalloc(dsize_t);
extern void  dRealFree(void*);

extern void* dMemcpy(void *dst, const void *src, dsize_t size);
extern void* dMemmove(void *dst, const void *src, dsize_t size);
extern void* dMemset(void *dst, int c, dsize_t size);
extern int   dMemcmp(const void *ptr1, const void *ptr2, dsize_t size);

#endif // _PLATFORM_MEMORY_H_
