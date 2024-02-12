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

#ifndef __OSX_OUTLINE__
#define __OSX_OUTLINE__

#if defined(TORQUE_DEBUG)
#ifndef __GL_OUTLINE_FUNCS__
#define __GL_OUTLINE_FUNCS__

extern bool gOutlineEnabled;

extern void (* glDrawElementsProcPtr) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
extern void (* glDrawArraysProcPtr) (GLenum mode, GLint first, GLsizei count);

void glOutlineDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glOutlineDrawArrays(GLenum mode, GLint first, GLsizei count);

extern void (* glNormDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
extern void (* glNormDrawArrays) (GLenum mode, GLint first, GLsizei count);

#ifndef NO_REDEFINE_GL_FUNCS
#define glDrawElements glDrawElementsProcPtr
#define glDrawArrays glDrawArraysProcPtr
#else 
//#warning glDrawElements and glDrawArrays not redefined
#endif // NO_REDEFINE_GL_FUNCS
#endif // __GL_OUTLINE_FUNCS__
#endif // TORQUE_DEBUG

#endif // __OSX_OUTLINE__

