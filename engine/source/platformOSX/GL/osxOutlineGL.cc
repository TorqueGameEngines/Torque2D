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

// The debug render modes are only built in a debug build, 
// partially because a release build should not need them
// and partially because using aglMacro.h or cglMacro.h would prevent us from
// playing this little function-pointer-hijacking trick
#if defined(TORQUE_DEBUG)

#define NO_REDEFINE_GL_FUNCS

#include "platformGL.h"
#include "console/console.h"

bool gOutlineEnabled = false;

void (* glDrawElementsProcPtr) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) = glDrawElements;
void (* glDrawArraysProcPtr) (GLenum mode, GLint first, GLsizei count) = glDrawArrays;
void (* glNormDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) = glDrawElements;
void (* glNormDrawArrays) (GLenum mode, GLint first, GLsizei count) = glDrawArrays;      


/// A utility for the outline drawing routines
static U32 getIndex(GLenum type, const void *indices, U32 i)
{
   if(type == GL_UNSIGNED_BYTE)
      return ((U8 *) indices)[i];
   else if(type == GL_UNSIGNED_SHORT)
      return ((U16 *) indices)[i];
   else
      return ((U32 *) indices)[i];
}

void glOutlineDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{ 
   if(mode == GL_POLYGON)
      mode = GL_LINE_LOOP;

   if(mode == GL_POINTS || mode == GL_LINE_STRIP || mode == GL_LINE_LOOP || mode == GL_LINES)   
      glDrawElements( mode, count, type, indices );
   else
   {
      glBegin(GL_LINES);
      if(mode == GL_TRIANGLE_STRIP)
      {
         U32 i;
         for(i = 0; i < count - 1; i++)
         {
            glArrayElement(getIndex(type, indices, i));
            glArrayElement(getIndex(type, indices, i + 1));
            if(i + 2 != count)
            {
               glArrayElement(getIndex(type, indices, i));
               glArrayElement(getIndex(type, indices, i + 2));
            }
         }
      }
      else if(mode == GL_TRIANGLE_FAN)
      {
         for(U32 i = 1; i < count; i ++)
         {
            glArrayElement(getIndex(type, indices, 0));
            glArrayElement(getIndex(type, indices, i));
            if(i != count - 1)
            {
               glArrayElement(getIndex(type, indices, i));
               glArrayElement(getIndex(type, indices, i + 1));
            }
         }
      }
      else if(mode == GL_TRIANGLES)
      {
         for(U32 i = 3; i <= count; i += 3)
         {
            glArrayElement(getIndex(type, indices, i - 3));
            glArrayElement(getIndex(type, indices, i - 2));
            glArrayElement(getIndex(type, indices, i - 2));
            glArrayElement(getIndex(type, indices, i - 1));
            glArrayElement(getIndex(type, indices, i - 3));
            glArrayElement(getIndex(type, indices, i - 1));
         }
      }
      else if(mode == GL_QUADS)
      {
         for(U32 i = 4; i <= count; i += 4)
         {
            glArrayElement(getIndex(type, indices, i - 4));
            glArrayElement(getIndex(type, indices, i - 3));
            glArrayElement(getIndex(type, indices, i - 3));
            glArrayElement(getIndex(type, indices, i - 2));
            glArrayElement(getIndex(type, indices, i - 2));
            glArrayElement(getIndex(type, indices, i - 1));
            glArrayElement(getIndex(type, indices, i - 4));
            glArrayElement(getIndex(type, indices, i - 1));
         }
      }
      else if(mode == GL_QUAD_STRIP)
      {
         if(count < 4)
            return;
         glArrayElement(getIndex(type, indices, 0));
         glArrayElement(getIndex(type, indices, 1));
         for(U32 i = 4; i <= count; i += 2)
         {
            glArrayElement(getIndex(type, indices, i - 4));
            glArrayElement(getIndex(type, indices, i - 2));

            glArrayElement(getIndex(type, indices, i - 3));
            glArrayElement(getIndex(type, indices, i - 1));

            glArrayElement(getIndex(type, indices, i - 2));
            glArrayElement(getIndex(type, indices, i - 1));
         }
      }
      glEnd();
   }
}

void glOutlineDrawArrays(GLenum mode, GLint first, GLsizei count) 
{ 
   if(mode == GL_POLYGON)
      mode = GL_LINE_LOOP;

   if(mode == GL_POINTS || mode == GL_LINE_STRIP || mode == GL_LINE_LOOP || mode == GL_LINES)   
      glDrawArrays( mode, first, count );
   else
   {
      glBegin(GL_LINES);
      if(mode == GL_TRIANGLE_STRIP)
      {
         U32 i;
         for(i = 0; i < count - 1; i++) 
         {
            glArrayElement(first + i);
            glArrayElement(first + i + 1);
            if(i + 2 != count)
            {
               glArrayElement(first + i);
               glArrayElement(first + i + 2);
            }
         }
      }
      else if(mode == GL_TRIANGLE_FAN)
      {
         for(U32 i = 1; i < count; i ++)
         {
            glArrayElement(first);
            glArrayElement(first + i);
            if(i != count - 1)
            {
               glArrayElement(first + i);
               glArrayElement(first + i + 1);
            }
         }
      }
      else if(mode == GL_TRIANGLES)
      {
         for(U32 i = 3; i <= count; i += 3)
         {
            glArrayElement(first + i - 3);
            glArrayElement(first + i - 2);
            glArrayElement(first + i - 2);
            glArrayElement(first + i - 1);
            glArrayElement(first + i - 3);
            glArrayElement(first + i - 1);
         }
      }
      else if(mode == GL_QUADS)
      {
         for(U32 i = 4; i <= count; i += 4)
         {
            glArrayElement(first + i - 4);
            glArrayElement(first + i - 3);
            glArrayElement(first + i - 3);
            glArrayElement(first + i - 2);
            glArrayElement(first + i - 2);
            glArrayElement(first + i - 1);
            glArrayElement(first + i - 4);
            glArrayElement(first + i - 1);
         }
      }
      else if(mode == GL_QUAD_STRIP)
      {
         if(count < 4)
            return;
         glArrayElement(first + 0);
         glArrayElement(first + 1);
         for(U32 i = 4; i <= count; i += 2)
         {
            glArrayElement(first + i - 4);
            glArrayElement(first + i - 2);

            glArrayElement(first + i - 3);
            glArrayElement(first + i - 1);

            glArrayElement(first + i - 2);
            glArrayElement(first + i - 1);
         }
      }
      glEnd();
   }
}

ConsoleFunction(GLEnableOutline,void,2,2,"GLEnableOutline( true | false ) - sets whether to render wireframe") 
{
   gOutlineEnabled = dAtob(argv[1]);
   if(gOutlineEnabled)
   {
      glDrawElementsProcPtr = glOutlineDrawElements;
      glDrawArraysProcPtr = glOutlineDrawArrays;
      Con::printf("swapped to outline mode funcs");
   } else {
      glDrawElementsProcPtr = glDrawElements;
      glDrawArraysProcPtr = glDrawArrays;
      Con::printf("swapped to normal funcs");
   }
}
#endif
