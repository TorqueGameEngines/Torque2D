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

// These are a bunch of auxilary functions for logging and computing performance.
// They don't really have anything to do with GL loading, so let's put them
// in a serperate file.

#include "platformWin32/platformWin32.h"
#include "platform/platformGL.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include <time.h>

static bool loggingEnabled = false;
static bool outlineEnabled = false;
static bool perfEnabled = false;

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) extern fn_type (APIENTRY * dll##fn_name)fn_args;
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) extern fn_type (APIENTRY * dlld##fn_name)fn_args;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) extern fn_type (APIENTRY * dlldwgl##fn_name)fn_args;

//includes...
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#include "platformWin32/GLWinFunc.h"

//undefs...
#undef GL_FUNCTION
#undef WGL_FUNCTION
#undef WGLD3D_FUNCTION

static const char * BooleanToString( GLboolean b )
{
   if ( b == GL_FALSE )
      return "GL_FALSE";
   else if ( b == GL_TRUE )
      return "GL_TRUE";
   else
      return "OUT OF RANGE FOR BOOLEAN";
}

static const char * FuncToString( GLenum f )
{
   switch ( f )
   {
   case GL_ALWAYS:
      return "GL_ALWAYS";
   case GL_NEVER:
      return "GL_NEVER";
   case GL_LEQUAL:
      return "GL_LEQUAL";
   case GL_LESS:
      return "GL_LESS";
   case GL_EQUAL:
      return "GL_EQUAL";
   case GL_GREATER:
      return "GL_GREATER";
   case GL_GEQUAL:
      return "GL_GEQUAL";
   case GL_NOTEQUAL:
      return "GL_NOTEQUAL";
   default:
      return "!!! UNKNOWN !!!";
   }
}

static const char* CoordToString( GLenum coord)
{
   if (coord == GL_S)
      return "GL_S";
   else if (coord == GL_T)
      return "GL_T";
   else
      return "INVALID_COORD";
}

static const char * PrimToString( GLenum mode )
{
   static char prim[1024];

   if ( mode == GL_TRIANGLES )
      strcpy( prim, "GL_TRIANGLES" );
   else if ( mode == GL_TRIANGLE_STRIP )
      strcpy( prim, "GL_TRIANGLE_STRIP" );
   else if ( mode == GL_TRIANGLE_FAN )
      strcpy( prim, "GL_TRIANGLE_FAN" );
   else if ( mode == GL_QUADS )
      strcpy( prim, "GL_QUADS" );
   else if ( mode == GL_QUAD_STRIP )
      strcpy( prim, "GL_QUAD_STRIP" );
   else if ( mode == GL_POLYGON )
      strcpy( prim, "GL_POLYGON" );
   else if ( mode == GL_POINTS )
      strcpy( prim, "GL_POINTS" );
   else if ( mode == GL_LINES )
      strcpy( prim, "GL_LINES" );
   else if ( mode == GL_LINE_STRIP )
      strcpy( prim, "GL_LINE_STRIP" );
   else if ( mode == GL_LINE_LOOP )
      strcpy( prim, "GL_LINE_LOOP" );
   else
      sprintf( prim, "0x%x", mode );

   return prim;
}

static const char * CapToString( GLenum cap )
{
   static char buffer[1024];

   switch ( cap )
   {
   case GL_TEXTURE_2D:
      return "GL_TEXTURE_2D";
   case GL_BLEND:
      return "GL_BLEND";
   case GL_DEPTH_TEST:
      return "GL_DEPTH_TEST";
   case GL_CULL_FACE:
      return "GL_CULL_FACE";
   case GL_CLIP_PLANE0:
      return "GL_CLIP_PLANE0";
   case GL_COLOR_ARRAY:
      return "GL_COLOR_ARRAY";
   case GL_TEXTURE_COORD_ARRAY:
      return "GL_TEXTURE_COORD_ARRAY";
   case GL_VERTEX_ARRAY:
      return "GL_VERTEX_ARRAY";
   case GL_ALPHA_TEST:
      return "GL_ALPHA_TEST";
   case GL_STENCIL_TEST:
      return "GL_STENCIL_TEST";
   case GL_TEXTURE_GEN_S:
      return "GL_TEXTURE_GEN_S";
   case GL_TEXTURE_GEN_T:
      return "GL_TEXTURE_GEN_T";
   default:
      sprintf( buffer, "0x%x", cap );
   }

   return buffer;
}

static const char * TypeToString( GLenum t )
{
   switch ( t )
   {
   case GL_BYTE:
      return "GL_BYTE";
   case GL_UNSIGNED_BYTE:
      return "GL_UNSIGNED_BYTE";
   case GL_SHORT:
      return "GL_SHORT";
   case GL_UNSIGNED_SHORT:
      return "GL_UNSIGNED_SHORT";
   case GL_INT:
      return "GL_INT";
   case GL_UNSIGNED_INT:
      return "GL_UNSIGNED_INT";
   case GL_FLOAT:
      return "GL_FLOAT";
   case GL_DOUBLE:
      return "GL_DOUBLE";
   default:
      return "!!! UNKNOWN !!!";
   }
}

//------------------------------------------------------------------------------
// GLU Log Functions
static void APIENTRY loggluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
   fprintf(winState.log_fp, "gluOrtho2D( %f, %f, %f, %f )\n", left, right, bottom, top);
   fflush(winState.log_fp);
   dllgluOrtho2D(left, right, bottom, top);
}

static void APIENTRY loggluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
   fprintf(winState.log_fp, "gluPerspective( %f, %f, %f, %f )\n", fovy, aspect, zNear, zFar);
   fflush(winState.log_fp);
   dllgluPerspective(fovy, aspect, zNear, zFar);
}

static void APIENTRY loggluPickMatrix(GLdouble x, GLdouble y, GLdouble width, GLdouble height, GLint viewport[4])
{
   fprintf(winState.log_fp, "gluPickMatrix(%f, %f, %f, %f, VIEW)\n", x, y, width, height);
   fflush(winState.log_fp);
   dllgluPickMatrix(x, y, width, height, viewport);
}

static void APIENTRY loggluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
{
   fprintf(winState.log_fp, "gluLookAt(%f, %f, %f, %f, %f, %f, %f, %f, %f)\n",eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
   fflush(winState.log_fp);
   dllgluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
}

static int APIENTRY loggluProject(GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *winx, GLdouble *winy, GLdouble *winz)
{
   fprintf(winState.log_fp, "gluProject\n");
   fflush(winState.log_fp);
   return dllgluProject(objx, objy, objz, modelMatrix, projMatrix, viewport, winx, winy, winz);
}

static int APIENTRY loggluUnProject(GLdouble winx, GLdouble winy, GLdouble winz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
   fprintf(winState.log_fp, "gluUnProject\n");
   fflush(winState.log_fp);
   return dllgluUnProject(winx, winy, winz, modelMatrix, projMatrix, viewport, objx, objy, objz);
}

static int APIENTRY loggluScaleImage(GLenum format, GLint widthin, GLint heightin, GLenum typein, const void *datain, GLint widthout, GLint heightout, GLenum typeout, void *dataout)
{
   fprintf(winState.log_fp, "gluScaleImage\n");
   fflush(winState.log_fp);
   return dllgluScaleImage(format, widthin, heightin, typein, datain, widthout, heightout, typeout, dataout);
}

static int APIENTRY loggluBuild1DMipmaps(GLenum target, GLint components, GLint width, GLenum format, GLenum type, const void *data)
{
   fprintf(winState.log_fp, "gluBuild1DMipmaps\n");
   fflush(winState.log_fp);
   return dllgluBuild1DMipmaps(target, components, width, format, type, data);
}

static int APIENTRY loggluBuild2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void *data)
{
   fprintf(winState.log_fp, "gluBuild2DMipmaps\n");
   fflush(winState.log_fp);
   return dllgluBuild2DMipmaps(target, components, width, height, format, type, data);
}


//------------------------------------------------------------------------------
// GL LOG Functions
static void APIENTRY logglAccum(GLenum op, GLfloat value)
{
   fprintf( winState.log_fp, "glAccum\n" );
   fflush(winState.log_fp);
   dllglAccum( op, value );
}

static void APIENTRY logglAlphaFunc(GLenum func, GLclampf ref)
{
   fprintf( winState.log_fp, "glAlphaFunc( 0x%x, %g )\n", func, ref );
   fflush(winState.log_fp);
   dllglAlphaFunc( func, ref );
}

static GLboolean APIENTRY logglAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences)
{
   fprintf( winState.log_fp, "glAreTexturesResident\n" );
   fflush(winState.log_fp);
   return dllglAreTexturesResident( n, textures, residences );
}

static void APIENTRY logglArrayElement(GLint i)
{
   fprintf( winState.log_fp, "glArrayElement\n" );
   fflush(winState.log_fp);
   dllglArrayElement( i );
}

static void APIENTRY logglBegin(GLenum mode)
{
   fprintf( winState.log_fp, "glBegin( %s )\n", PrimToString( mode ));
   fflush(winState.log_fp);
   dllglBegin( mode );
}

static void APIENTRY logglBindTexture(GLenum target, GLuint texture)
{
   fprintf( winState.log_fp, "glBindTexture( 0x%x, %u )\n", target, texture );
   fflush(winState.log_fp);
   dllglBindTexture( target, texture );
}

static void APIENTRY logglBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
   fprintf( winState.log_fp, "glBitmap\n" );
   fflush(winState.log_fp);
   dllglBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
}

static void BlendToName( char *n, GLenum f )
{
   switch ( f )
   {
   case GL_ONE:
      strcpy( n, "GL_ONE" );
      break;
   case GL_ZERO:
      strcpy( n, "GL_ZERO" );
      break;
   case GL_SRC_ALPHA:
      strcpy( n, "GL_SRC_ALPHA" );
      break;
   case GL_ONE_MINUS_SRC_ALPHA:
      strcpy( n, "GL_ONE_MINUS_SRC_ALPHA" );
      break;
   case GL_SRC_COLOR:
      strcpy( n, "GL_SRC_COLOR" );
      break;
   case GL_ONE_MINUS_SRC_COLOR:
      strcpy( n, "GL_ONE_MINUS_SRC_COLOR" );
      break;
   case GL_DST_COLOR:
      strcpy( n, "GL_DST_COLOR" );
      break;
   case GL_ONE_MINUS_DST_COLOR:
      strcpy( n, "GL_ONE_MINUS_DST_COLOR" );
      break;
   case GL_DST_ALPHA:
      strcpy( n, "GL_DST_ALPHA" );
      break;
   default:
      sprintf( n, "0x%x", f );
   }
}
static void APIENTRY logglBlendFunc(GLenum sfactor, GLenum dfactor)
{
   char sf[128], df[128];

   BlendToName( sf, sfactor );
   BlendToName( df, dfactor );

   fprintf( winState.log_fp, "glBlendFunc( %s, %s )\n", sf, df );
   fflush(winState.log_fp);
   dllglBlendFunc( sfactor, dfactor );
}

static void APIENTRY logglCallList(GLuint list)
{
   fprintf( winState.log_fp, "glCallList( %u )\n", list );
   fflush(winState.log_fp);
   dllglCallList( list );
}

static void APIENTRY logglCallLists(GLsizei n, GLenum type, const void *lists)
{
   fprintf( winState.log_fp, "glCallLists\n" );
   fflush(winState.log_fp);
   dllglCallLists( n, type, lists );
}

static void APIENTRY logglClear(GLbitfield mask)
{
   fprintf( winState.log_fp, "glClear( 0x%x = ", mask );

   if ( mask & GL_COLOR_BUFFER_BIT )
      fprintf( winState.log_fp, "GL_COLOR_BUFFER_BIT " );
   if ( mask & GL_DEPTH_BUFFER_BIT )
      fprintf( winState.log_fp, "GL_DEPTH_BUFFER_BIT " );
   if ( mask & GL_STENCIL_BUFFER_BIT )
      fprintf( winState.log_fp, "GL_STENCIL_BUFFER_BIT " );
   if ( mask & GL_ACCUM_BUFFER_BIT )
      fprintf( winState.log_fp, "GL_ACCUM_BUFFER_BIT " );

   fprintf( winState.log_fp, ")\n" );
   fflush(winState.log_fp);
   dllglClear( mask );
}

static void APIENTRY logglClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
   fprintf( winState.log_fp, "glClearAccum\n" );
   fflush(winState.log_fp);
   dllglClearAccum( red, green, blue, alpha );
}

static void APIENTRY logglClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
   fprintf( winState.log_fp, "glClearColor\n" );
   fflush(winState.log_fp);
   dllglClearColor( red, green, blue, alpha );
}

static void APIENTRY logglClearDepth(GLclampd depth)
{
   fprintf( winState.log_fp, "glClearDepth( %g )\n", ( F32 ) depth );
   fflush(winState.log_fp);
   dllglClearDepth( depth );
}

static void APIENTRY logglClearIndex(GLfloat c)
{
   fprintf( winState.log_fp, "glClearIndex\n" );
   fflush(winState.log_fp);
   dllglClearIndex( c );
}

static void APIENTRY logglClearStencil(GLint s)
{
   fprintf( winState.log_fp, "glClearStencil( %d )\n", s );
   fflush(winState.log_fp);
   dllglClearStencil( s );
}

static void APIENTRY logglClipPlane(GLenum plane, const GLdouble *equation)
{
   fprintf( winState.log_fp, "glClipPlane\n" );
   fflush(winState.log_fp);
   dllglClipPlane( plane, equation );
}

static void APIENTRY logglColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
   fprintf( winState.log_fp, "glColor3b\n" );
   fflush(winState.log_fp);
   dllglColor3b( red, green, blue );
}

static void APIENTRY logglColor3bv(const GLbyte *v)
{
   fprintf( winState.log_fp, "glColor3bv\n" );
   fflush(winState.log_fp);
   dllglColor3bv( v );
}

static void APIENTRY logglColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
   fprintf( winState.log_fp, "glColor3d\n" );
   fflush(winState.log_fp);
   dllglColor3d( red, green, blue );
}

static void APIENTRY logglColor3dv(const GLdouble *v)
{
   fprintf( winState.log_fp, "glColor3dv\n" );
   fflush(winState.log_fp);
   dllglColor3dv( v );
}

static void APIENTRY logglColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
   fprintf( winState.log_fp, "glColor3f\n" );
   fflush(winState.log_fp);
   dllglColor3f( red, green, blue );
}

static void APIENTRY logglColor3fv(const GLfloat *v)
{
   fprintf( winState.log_fp, "glColor3fv\n" );
   fflush(winState.log_fp);
   dllglColor3fv( v );
}

static void APIENTRY logglColor3i(GLint red, GLint green, GLint blue)
{
   fprintf( winState.log_fp, "glColor3i\n" );
   fflush(winState.log_fp);
   dllglColor3i( red, green, blue );
}

static void APIENTRY logglColor3iv(const GLint *v)
{
   fprintf( winState.log_fp, "glColor3iv\n" );
   fflush(winState.log_fp);
   dllglColor3iv( v );
}

static void APIENTRY logglColor3s(GLshort red, GLshort green, GLshort blue)
{
   fprintf( winState.log_fp, "glColor3s\n" );
   fflush(winState.log_fp);
   dllglColor3s( red, green, blue );
}

static void APIENTRY logglColor3sv(const GLshort *v)
{
   fprintf( winState.log_fp, "glColor3sv\n" );
   fflush(winState.log_fp);
   dllglColor3sv( v );
}

static void APIENTRY logglColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
   fprintf( winState.log_fp, "glColor3ub\n" );
   fflush(winState.log_fp);
   dllglColor3ub( red, green, blue );
}

static void APIENTRY logglColor3ubv(const GLubyte *v)
{
   fprintf( winState.log_fp, "glColor3ubv\n" );
   fflush(winState.log_fp);
   dllglColor3ubv( v );
}

#define SIG( x ) fprintf( winState.log_fp, x "\n" ); fflush(winState.log_fp)

static void APIENTRY logglColor3ui(GLuint red, GLuint green, GLuint blue)
{
   SIG( "glColor3ui" );
   dllglColor3ui( red, green, blue );
}

static void APIENTRY logglColor3uiv(const GLuint *v)
{
   SIG( "glColor3uiv" );
   dllglColor3uiv( v );
}

static void APIENTRY logglColor3us(GLushort red, GLushort green, GLushort blue)
{
   SIG( "glColor3us" );
   dllglColor3us( red, green, blue );
}

static void APIENTRY logglColor3usv(const GLushort *v)
{
   SIG( "glColor3usv" );
   dllglColor3usv( v );
}

static void APIENTRY logglColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
   SIG( "glColor4b" );
   dllglColor4b( red, green, blue, alpha );
}

static void APIENTRY logglColor4bv(const GLbyte *v)
{
   SIG( "glColor4bv" );
   dllglColor4bv( v );
}

static void APIENTRY logglColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
   SIG( "glColor4d" );
   dllglColor4d( red, green, blue, alpha );
}
static void APIENTRY logglColor4dv(const GLdouble *v)
{
   SIG( "glColor4dv" );
   dllglColor4dv( v );
}
static void APIENTRY logglColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
   fprintf( winState.log_fp, "glColor4f( %g,%g,%g,%g )\n", red, green, blue, alpha );
   fflush(winState.log_fp);
   dllglColor4f( red, green, blue, alpha );
}
static void APIENTRY logglColor4fv(const GLfloat *v)
{
   fprintf( winState.log_fp, "glColor4fv( %g,%g,%g,%g )\n", v[0], v[1], v[2], v[3] );
   fflush(winState.log_fp);
   dllglColor4fv( v );
}
static void APIENTRY logglColor4i(GLint red, GLint green, GLint blue, GLint alpha)
{
   SIG( "glColor4i" );
   dllglColor4i( red, green, blue, alpha );
}
static void APIENTRY logglColor4iv(const GLint *v)
{
   SIG( "glColor4iv" );
   dllglColor4iv( v );
}
static void APIENTRY logglColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
   SIG( "glColor4s" );
   dllglColor4s( red, green, blue, alpha );
}
static void APIENTRY logglColor4sv(const GLshort *v)
{
   SIG( "glColor4sv" );
   dllglColor4sv( v );
}
static void APIENTRY logglColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
   SIG( "glColor4ub" );
   dllglColor4b( red, green, blue, alpha );
}
static void APIENTRY logglColor4ubv(const GLubyte *v)
{
   SIG( "glColor4ubv" );
   dllglColor4ubv( v );
}
static void APIENTRY logglColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
   SIG( "glColor4ui" );
   dllglColor4ui( red, green, blue, alpha );
}
static void APIENTRY logglColor4uiv(const GLuint *v)
{
   SIG( "glColor4uiv" );
   dllglColor4uiv( v );
}
static void APIENTRY logglColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
   SIG( "glColor4us" );
   dllglColor4us( red, green, blue, alpha );
}
static void APIENTRY logglColor4usv(const GLushort *v)
{
   SIG( "glColor4usv" );
   dllglColor4usv( v );
}
static void APIENTRY logglColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
   SIG( "glColorMask" );
   dllglColorMask( red, green, blue, alpha );
}
static void APIENTRY logglColorMaterial(GLenum face, GLenum mode)
{
   SIG( "glColorMaterial" );
   dllglColorMaterial( face, mode );
}

static void APIENTRY logglColorPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
   fprintf( winState.log_fp, "glColorPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
   fflush(winState.log_fp);
   dllglColorPointer( size, type, stride, pointer );
}

static void APIENTRY logglCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
   SIG( "glCopyPixels" );
   dllglCopyPixels( x, y, width, height, type );
}

static void APIENTRY logglCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
   SIG( "glCopyTexImage1D" );
   dllglCopyTexImage1D( target, level, internalFormat, x, y, width, border );
}

static void APIENTRY logglCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
   SIG( "glCopyTexImage2D" );
   dllglCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
}

static void APIENTRY logglCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
   SIG( "glCopyTexSubImage1D" );
   dllglCopyTexSubImage1D( target, level, xoffset, x, y, width );
}

static void APIENTRY logglCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
   SIG( "glCopyTexSubImage2D" );
   dllglCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
}

static void APIENTRY logglCullFace(GLenum mode)
{
   fprintf( winState.log_fp, "glCullFace( %s )\n", ( mode == GL_FRONT ) ? "GL_FRONT" : "GL_BACK" );
   fflush(winState.log_fp);
   dllglCullFace( mode );
}

static void APIENTRY logglDeleteLists(GLuint list, GLsizei range)
{
   SIG( "glDeleteLists" );
   dllglDeleteLists( list, range );
}

static void APIENTRY logglDeleteTextures(GLsizei n, const GLuint *textures)
{
   SIG( "glDeleteTextures" );
   dllglDeleteTextures( n, textures );
}

static void APIENTRY logglDepthFunc(GLenum func)
{
   fprintf( winState.log_fp, "glDepthFunc( %s )\n", FuncToString( func ) );
   fflush(winState.log_fp);
   dllglDepthFunc( func );
}

static void APIENTRY logglDepthMask(GLboolean flag)
{
   fprintf( winState.log_fp, "glDepthMask( %s )\n", BooleanToString( flag ) );
   fflush(winState.log_fp);
   dllglDepthMask( flag );
}

static void APIENTRY logglDepthRange(GLclampd zNear, GLclampd zFar)
{
   fprintf( winState.log_fp, "glDepthRange( %g, %g )\n", ( F32 ) zNear, ( F32 ) zFar );
   fflush(winState.log_fp);
   dllglDepthRange( zNear, zFar );
}

static void APIENTRY logglDisable(GLenum cap)
{
   fprintf( winState.log_fp, "glDisable( %s )\n", CapToString( cap ) );
   fflush(winState.log_fp);
   dllglDisable( cap );
}

static void APIENTRY logglDisableClientState(GLenum array)
{
   fprintf( winState.log_fp, "glDisableClientState( %s )\n", CapToString( array ) );
   fflush(winState.log_fp);
   dllglDisableClientState( array );
}

static void APIENTRY logglDrawArrays(GLenum mode, GLint first, GLsizei count)
{
   SIG( "glDrawArrays" );
   dllglDrawArrays( mode, first, count );
}

static void APIENTRY logglDrawBuffer(GLenum mode)
{
   SIG( "glDrawBuffer" );
   dllglDrawBuffer( mode );
}

static void APIENTRY logglDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
   fprintf( winState.log_fp, "glDrawElements( %s, %d, %s, MEM )\n", PrimToString( mode ), count, TypeToString( type ) );
   fflush(winState.log_fp);
   dllglDrawElements( mode, count, type, indices );
}

static void APIENTRY logglDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
   SIG( "glDrawPixels" );
   dllglDrawPixels( width, height, format, type, pixels );
}

static void APIENTRY logglEdgeFlag(GLboolean flag)
{
   SIG( "glEdgeFlag" );
   dllglEdgeFlag( flag );
}

static void APIENTRY logglEdgeFlagPointer(GLsizei stride, const GLvoid *pointer)
{
   SIG( "glEdgeFlagPointer" );
   dllglEdgeFlagPointer( stride, pointer );
}

static void APIENTRY logglEdgeFlagv(const GLboolean *flag)
{
   SIG( "glEdgeFlagv" );
   dllglEdgeFlagv( flag );
}

static void APIENTRY logglEnable(GLenum cap)
{
   fprintf( winState.log_fp, "glEnable( %s )\n", CapToString( cap ) );
   fflush(winState.log_fp);
   dllglEnable( cap );
}

static void APIENTRY logglEnableClientState(GLenum array)
{
   fprintf( winState.log_fp, "glEnableClientState( %s )\n", CapToString( array ) );
   fflush(winState.log_fp);
   dllglEnableClientState( array );
}

static void APIENTRY logglEnd(void)
{
   SIG( "glEnd" );
   dllglEnd();
}

static void APIENTRY logglEndList(void)
{
   SIG( "glEndList" );
   dllglEndList();
}

static void APIENTRY logglEvalCoord1d(GLdouble u)
{
   SIG( "glEvalCoord1d" );
   dllglEvalCoord1d( u );
}

static void APIENTRY logglEvalCoord1dv(const GLdouble *u)
{
   SIG( "glEvalCoord1dv" );
   dllglEvalCoord1dv( u );
}

static void APIENTRY logglEvalCoord1f(GLfloat u)
{
   SIG( "glEvalCoord1f" );
   dllglEvalCoord1f( u );
}

static void APIENTRY logglEvalCoord1fv(const GLfloat *u)
{
   SIG( "glEvalCoord1fv" );
   dllglEvalCoord1fv( u );
}
static void APIENTRY logglEvalCoord2d(GLdouble u, GLdouble v)
{
   SIG( "glEvalCoord2d" );
   dllglEvalCoord2d( u, v );
}
static void APIENTRY logglEvalCoord2dv(const GLdouble *u)
{
   SIG( "glEvalCoord2dv" );
   dllglEvalCoord2dv( u );
}
static void APIENTRY logglEvalCoord2f(GLfloat u, GLfloat v)
{
   SIG( "glEvalCoord2f" );
   dllglEvalCoord2f( u, v );
}
static void APIENTRY logglEvalCoord2fv(const GLfloat *u)
{
   SIG( "glEvalCoord2fv" );
   dllglEvalCoord2fv( u );
}

static void APIENTRY logglEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
   SIG( "glEvalMesh1" );
   dllglEvalMesh1( mode, i1, i2 );
}
static void APIENTRY logglEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
   SIG( "glEvalMesh2" );
   dllglEvalMesh2( mode, i1, i2, j1, j2 );
}
static void APIENTRY logglEvalPoint1(GLint i)
{
   SIG( "glEvalPoint1" );
   dllglEvalPoint1( i );
}
static void APIENTRY logglEvalPoint2(GLint i, GLint j)
{
   SIG( "glEvalPoint2" );
   dllglEvalPoint2( i, j );
}

static void APIENTRY logglFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer)
{
   SIG( "glFeedbackBuffer" );
   dllglFeedbackBuffer( size, type, buffer );
}

static void APIENTRY logglFinish(void)
{
   SIG( "glFinish" );
   dllglFinish();
}

static void APIENTRY logglFlush(void)
{
   SIG( "glFlush" );
   dllglFlush();
}

static void APIENTRY logglFogf(GLenum pname, GLfloat param)
{
   SIG( "glFogf" );
   dllglFogf( pname, param );
}

static void APIENTRY logglFogfv(GLenum pname, const GLfloat *params)
{
   SIG( "glFogfv" );
   dllglFogfv( pname, params );
}

static void APIENTRY logglFogi(GLenum pname, GLint param)
{
   SIG( "glFogi" );
   dllglFogi( pname, param );
}

static void APIENTRY logglFogiv(GLenum pname, const GLint *params)
{
   SIG( "glFogiv" );
   dllglFogiv( pname, params );
}

static void APIENTRY logglFrontFace(GLenum mode)
{
   SIG( "glFrontFace" );
   dllglFrontFace( mode );
}

static void APIENTRY logglFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
   SIG( "glFrustum" );
   dllglFrustum( left, right, bottom, top, zNear, zFar );
}

static GLuint APIENTRY logglGenLists(GLsizei range)
{
   SIG( "glGenLists" );
   return dllglGenLists( range );
}

static void APIENTRY logglGenTextures(GLsizei n, GLuint *textures)
{
   SIG( "glGenTextures" );
   dllglGenTextures( n, textures );
}

static void APIENTRY logglGetBooleanv(GLenum pname, GLboolean *params)
{
   SIG( "glGetBooleanv" );
   dllglGetBooleanv( pname, params );
}

static void APIENTRY logglGetClipPlane(GLenum plane, GLdouble *equation)
{
   SIG( "glGetClipPlane" );
   dllglGetClipPlane( plane, equation );
}

static void APIENTRY logglGetDoublev(GLenum pname, GLdouble *params)
{
   SIG( "glGetDoublev" );
   dllglGetDoublev( pname, params );
}

static GLenum APIENTRY logglGetError(void)
{
   SIG( "glGetError" );
   return dllglGetError();
}

static void APIENTRY logglGetFloatv(GLenum pname, GLfloat *params)
{
   SIG( "glGetFloatv" );
   dllglGetFloatv( pname, params );
}

static void APIENTRY logglGetIntegerv(GLenum pname, GLint *params)
{
   SIG( "glGetIntegerv" );
   dllglGetIntegerv( pname, params );
}

static void APIENTRY logglGetLightfv(GLenum light, GLenum pname, GLfloat *params)
{
   SIG( "glGetLightfv" );
   dllglGetLightfv( light, pname, params );
}

static void APIENTRY logglGetLightiv(GLenum light, GLenum pname, GLint *params)
{
   SIG( "glGetLightiv" );
   dllglGetLightiv( light, pname, params );
}

static void APIENTRY logglGetMapdv(GLenum target, GLenum query, GLdouble *v)
{
   SIG( "glGetMapdv" );
   dllglGetMapdv( target, query, v );
}

static void APIENTRY logglGetMapfv(GLenum target, GLenum query, GLfloat *v)
{
   SIG( "glGetMapfv" );
   dllglGetMapfv( target, query, v );
}

static void APIENTRY logglGetMapiv(GLenum target, GLenum query, GLint *v)
{
   SIG( "glGetMapiv" );
   dllglGetMapiv( target, query, v );
}

static void APIENTRY logglGetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{
   SIG( "glGetMaterialfv" );
   dllglGetMaterialfv( face, pname, params );
}

static void APIENTRY logglGetMaterialiv(GLenum face, GLenum pname, GLint *params)
{
   SIG( "glGetMaterialiv" );
   dllglGetMaterialiv( face, pname, params );
}

static void APIENTRY logglGetPixelMapfv(GLenum map, GLfloat *values)
{
   SIG( "glGetPixelMapfv" );
   dllglGetPixelMapfv( map, values );
}

static void APIENTRY logglGetPixelMapuiv(GLenum map, GLuint *values)
{
   SIG( "glGetPixelMapuiv" );
   dllglGetPixelMapuiv( map, values );
}

static void APIENTRY logglGetPixelMapusv(GLenum map, GLushort *values)
{
   SIG( "glGetPixelMapusv" );
   dllglGetPixelMapusv( map, values );
}

static void APIENTRY logglGetPointerv(GLenum pname, GLvoid* *params)
{
   SIG( "glGetPointerv" );
   dllglGetPointerv( pname, params );
}

static void APIENTRY logglGetPolygonStipple(GLubyte *mask)
{
   SIG( "glGetPolygonStipple" );
   dllglGetPolygonStipple( mask );
}

static const GLubyte * APIENTRY logglGetString(GLenum name)
{
   SIG( "glGetString" );
   return dllglGetString( name );
}

static void APIENTRY logglGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params)
{
   SIG( "glGetTexEnvfv" );
   dllglGetTexEnvfv( target, pname, params );
}

static void APIENTRY logglGetTexEnviv(GLenum target, GLenum pname, GLint *params)
{
   SIG( "glGetTexEnviv" );
   dllglGetTexEnviv( target, pname, params );
}

static void APIENTRY logglGetTexGendv(GLenum coord, GLenum pname, GLdouble *params)
{
   SIG( "glGetTexGendv" );
   dllglGetTexGendv( coord, pname, params );
}

static void APIENTRY logglGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params)
{
   SIG( "glGetTexGenfv" );
   dllglGetTexGenfv( coord, pname, params );
}

static void APIENTRY logglGetTexGeniv(GLenum coord, GLenum pname, GLint *params)
{
   SIG( "glGetTexGeniv" );
   dllglGetTexGeniv( coord, pname, params );
}

static void APIENTRY logglGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void *pixels)
{
   SIG( "glGetTexImage" );
   dllglGetTexImage( target, level, format, type, pixels );
}
static void APIENTRY logglGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params )
{
   SIG( "glGetTexLevelParameterfv" );
   dllglGetTexLevelParameterfv( target, level, pname, params );
}

static void APIENTRY logglGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
   SIG( "glGetTexLevelParameteriv" );
   dllglGetTexLevelParameteriv( target, level, pname, params );
}

static void APIENTRY logglGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params)
{
   SIG( "glGetTexParameterfv" );
   dllglGetTexParameterfv( target, pname, params );
}

static void APIENTRY logglGetTexParameteriv(GLenum target, GLenum pname, GLint *params)
{
   SIG( "glGetTexParameteriv" );
   dllglGetTexParameteriv( target, pname, params );
}

static void APIENTRY logglHint(GLenum target, GLenum mode)
{
   fprintf( winState.log_fp, "glHint( 0x%x, 0x%x )\n", target, mode );
   fflush(winState.log_fp);
   dllglHint( target, mode );
}

static void APIENTRY logglIndexMask(GLuint mask)
{
   SIG( "glIndexMask" );
   dllglIndexMask( mask );
}

static void APIENTRY logglIndexPointer(GLenum type, GLsizei stride, const void *pointer)
{
   SIG( "glIndexPointer" );
   dllglIndexPointer( type, stride, pointer );
}

static void APIENTRY logglIndexd(GLdouble c)
{
   SIG( "glIndexd" );
   dllglIndexd( c );
}

static void APIENTRY logglIndexdv(const GLdouble *c)
{
   SIG( "glIndexdv" );
   dllglIndexdv( c );
}

static void APIENTRY logglIndexf(GLfloat c)
{
   SIG( "glIndexf" );
   dllglIndexf( c );
}

static void APIENTRY logglIndexfv(const GLfloat *c)
{
   SIG( "glIndexfv" );
   dllglIndexfv( c );
}

static void APIENTRY logglIndexi(GLint c)
{
   SIG( "glIndexi" );
   dllglIndexi( c );
}

static void APIENTRY logglIndexiv(const GLint *c)
{
   SIG( "glIndexiv" );
   dllglIndexiv( c );
}

static void APIENTRY logglIndexs(GLshort c)
{
   SIG( "glIndexs" );
   dllglIndexs( c );
}

static void APIENTRY logglIndexsv(const GLshort *c)
{
   SIG( "glIndexsv" );
   dllglIndexsv( c );
}

static void APIENTRY logglIndexub(GLubyte c)
{
   SIG( "glIndexub" );
   dllglIndexub( c );
}

static void APIENTRY logglIndexubv(const GLubyte *c)
{
   SIG( "glIndexubv" );
   dllglIndexubv( c );
}

static void APIENTRY logglInitNames(void)
{
   SIG( "glInitNames" );
   dllglInitNames();
}

static void APIENTRY logglInterleavedArrays(GLenum format, GLsizei stride, const void *pointer)
{
   SIG( "glInterleavedArrays" );
   dllglInterleavedArrays( format, stride, pointer );
}

static GLboolean APIENTRY logglIsEnabled(GLenum cap)
{
   SIG( "glIsEnabled" );
   return dllglIsEnabled( cap );
}
static GLboolean APIENTRY logglIsList(GLuint list)
{
   SIG( "glIsList" );
   return dllglIsList( list );
}
static GLboolean APIENTRY logglIsTexture(GLuint texture)
{
   SIG( "glIsTexture" );
   return dllglIsTexture( texture );
}

static void APIENTRY logglLightModelf(GLenum pname, GLfloat param)
{
   SIG( "glLightModelf" );
   dllglLightModelf( pname, param );
}

static void APIENTRY logglLightModelfv(GLenum pname, const GLfloat *params)
{
   SIG( "glLightModelfv" );
   dllglLightModelfv( pname, params );
}

static void APIENTRY logglLightModeli(GLenum pname, GLint param)
{
   SIG( "glLightModeli" );
   dllglLightModeli( pname, param );

}

static void APIENTRY logglLightModeliv(GLenum pname, const GLint *params)
{
   SIG( "glLightModeliv" );
   dllglLightModeliv( pname, params );
}

static void APIENTRY logglLightf(GLenum light, GLenum pname, GLfloat param)
{
   SIG( "glLightf" );
   dllglLightf( light, pname, param );
}

static void APIENTRY logglLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
   SIG( "glLightfv" );
   dllglLightfv( light, pname, params );
}

static void APIENTRY logglLighti(GLenum light, GLenum pname, GLint param)
{
   SIG( "glLighti" );
   dllglLighti( light, pname, param );
}

static void APIENTRY logglLightiv(GLenum light, GLenum pname, const GLint *params)
{
   SIG( "glLightiv" );
   dllglLightiv( light, pname, params );
}

static void APIENTRY logglLineStipple(GLint factor, GLushort pattern)
{
   SIG( "glLineStipple" );
   dllglLineStipple( factor, pattern );
}

static void APIENTRY logglLineWidth(GLfloat width)
{
   SIG( "glLineWidth" );
   dllglLineWidth( width );
}

static void APIENTRY logglListBase(GLuint base)
{
   SIG( "glListBase" );
   dllglListBase( base );
}

static void APIENTRY logglLoadIdentity(void)
{
   SIG( "glLoadIdentity" );
   dllglLoadIdentity();
}

static void APIENTRY logglLoadMatrixd(const GLdouble *m)
{
   SIG( "glLoadMatrixd" );
   dllglLoadMatrixd( m );
}

static void APIENTRY logglLoadMatrixf(const GLfloat *m)
{
   SIG( "glLoadMatrixf" );
   dllglLoadMatrixf( m );
}

static void APIENTRY logglLoadName(GLuint name)
{
   SIG( "glLoadName" );
   dllglLoadName( name );
}

static void APIENTRY logglLogicOp(GLenum opcode)
{
   SIG( "glLogicOp" );
   dllglLogicOp( opcode );
}

static void APIENTRY logglMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{
   SIG( "glMap1d" );
   dllglMap1d( target, u1, u2, stride, order, points );
}

static void APIENTRY logglMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
   SIG( "glMap1f" );
   dllglMap1f( target, u1, u2, stride, order, points );
}

static void APIENTRY logglMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{
   SIG( "glMap2d" );
   dllglMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

static void APIENTRY logglMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{
   SIG( "glMap2f" );
   dllglMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

static void APIENTRY logglMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
   SIG( "glMapGrid1d" );
   dllglMapGrid1d( un, u1, u2 );
}

static void APIENTRY logglMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
   SIG( "glMapGrid1f" );
   dllglMapGrid1f( un, u1, u2 );
}

static void APIENTRY logglMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
   SIG( "glMapGrid2d" );
   dllglMapGrid2d( un, u1, u2, vn, v1, v2 );
}
static void APIENTRY logglMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
   SIG( "glMapGrid2f" );
   dllglMapGrid2f( un, u1, u2, vn, v1, v2 );
}
static void APIENTRY logglMaterialf(GLenum face, GLenum pname, GLfloat param)
{
   SIG( "glMaterialf" );
   dllglMaterialf( face, pname, param );
}
static void APIENTRY logglMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
   SIG( "glMaterialfv" );
   dllglMaterialfv( face, pname, params );
}

static void APIENTRY logglMateriali(GLenum face, GLenum pname, GLint param)
{
   SIG( "glMateriali" );
   dllglMateriali( face, pname, param );
}

static void APIENTRY logglMaterialiv(GLenum face, GLenum pname, const GLint *params)
{
   SIG( "glMaterialiv" );
   dllglMaterialiv( face, pname, params );
}

static void APIENTRY logglMatrixMode(GLenum mode)
{
   SIG( "glMatrixMode" );
   dllglMatrixMode( mode );
}

static void APIENTRY logglMultMatrixd(const GLdouble *m)
{
   SIG( "glMultMatrixd" );
   dllglMultMatrixd( m );
}

static void APIENTRY logglMultMatrixf(const GLfloat *m)
{
   SIG( "glMultMatrixf" );
   dllglMultMatrixf( m );
}

static void APIENTRY logglNewList(GLuint list, GLenum mode)
{
   SIG( "glNewList" );
   dllglNewList( list, mode );
}

static void APIENTRY logglNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
   SIG ("glNormal3b" );
   dllglNormal3b( nx, ny, nz );
}

static void APIENTRY logglNormal3bv(const GLbyte *v)
{
   SIG( "glNormal3bv" );
   dllglNormal3bv( v );
}

static void APIENTRY logglNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
   SIG( "glNormal3d" );
   dllglNormal3d( nx, ny, nz );
}

static void APIENTRY logglNormal3dv(const GLdouble *v)
{
   SIG( "glNormal3dv" );
   dllglNormal3dv( v );
}

static void APIENTRY logglNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
   SIG( "glNormal3f" );
   dllglNormal3f( nx, ny, nz );
}

static void APIENTRY logglNormal3fv(const GLfloat *v)
{
   SIG( "glNormal3fv" );
   dllglNormal3fv( v );
}
static void APIENTRY logglNormal3i(GLint nx, GLint ny, GLint nz)
{
   SIG( "glNormal3i" );
   dllglNormal3i( nx, ny, nz );
}
static void APIENTRY logglNormal3iv(const GLint *v)
{
   SIG( "glNormal3iv" );
   dllglNormal3iv( v );
}
static void APIENTRY logglNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
   SIG( "glNormal3s" );
   dllglNormal3s( nx, ny, nz );
}
static void APIENTRY logglNormal3sv(const GLshort *v)
{
   SIG( "glNormal3sv" );
   dllglNormal3sv( v );
}
static void APIENTRY logglNormalPointer(GLenum type, GLsizei stride, const void *pointer)
{
   SIG( "glNormalPointer" );
   dllglNormalPointer( type, stride, pointer );
}
static void APIENTRY logglOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
   SIG( "glOrtho" );
   dllglOrtho( left, right, bottom, top, zNear, zFar );
}

static void APIENTRY logglPassThrough(GLfloat token)
{
   SIG( "glPassThrough" );
   dllglPassThrough( token );
}

static void APIENTRY logglPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values)
{
   SIG( "glPixelMapfv" );
   dllglPixelMapfv( map, mapsize, values );
}

static void APIENTRY logglPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values)
{
   SIG( "glPixelMapuiv" );
   dllglPixelMapuiv( map, mapsize, values );
}

static void APIENTRY logglPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values)
{
   SIG( "glPixelMapusv" );
   dllglPixelMapusv( map, mapsize, values );
}
static void APIENTRY logglPixelStoref(GLenum pname, GLfloat param)
{
   SIG( "glPixelStoref" );
   dllglPixelStoref( pname, param );
}
static void APIENTRY logglPixelStorei(GLenum pname, GLint param)
{
   SIG( "glPixelStorei" );
   dllglPixelStorei( pname, param );
}
static void APIENTRY logglPixelTransferf(GLenum pname, GLfloat param)
{
   SIG( "glPixelTransferf" );
   dllglPixelTransferf( pname, param );
}

static void APIENTRY logglPixelTransferi(GLenum pname, GLint param)
{
   SIG( "glPixelTransferi" );
   dllglPixelTransferi( pname, param );
}

static void APIENTRY logglPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
   SIG( "glPixelZoom" );
   dllglPixelZoom( xfactor, yfactor );
}

static void APIENTRY logglPointSize(GLfloat size)
{
   SIG( "glPointSize" );
   dllglPointSize( size );
}

static void APIENTRY logglPolygonMode(GLenum face, GLenum mode)
{
   fprintf( winState.log_fp, "glPolygonMode( 0x%x, 0x%x )\n", face, mode );
   fflush(winState.log_fp);
   dllglPolygonMode( face, mode );
}

static void APIENTRY logglPolygonOffset(GLfloat factor, GLfloat units)
{
   SIG( "glPolygonOffset" );
   dllglPolygonOffset( factor, units );
}
static void APIENTRY logglPolygonStipple(const GLubyte *mask )
{
   SIG( "glPolygonStipple" );
   dllglPolygonStipple( mask );
}
static void APIENTRY logglPopAttrib(void)
{
   SIG( "glPopAttrib" );
   dllglPopAttrib();
}

static void APIENTRY logglPopClientAttrib(void)
{
   SIG( "glPopClientAttrib" );
   dllglPopClientAttrib();
}

static void APIENTRY logglPopMatrix(void)
{
   SIG( "glPopMatrix" );
   dllglPopMatrix();
}

static void APIENTRY logglPopName(void)
{
   SIG( "glPopName" );
   dllglPopName();
}

static void APIENTRY logglPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
   SIG( "glPrioritizeTextures" );
   dllglPrioritizeTextures( n, textures, priorities );
}

static void APIENTRY logglPushAttrib(GLbitfield mask)
{
   SIG( "glPushAttrib" );
   dllglPushAttrib( mask );
}

static void APIENTRY logglPushClientAttrib(GLbitfield mask)
{
   SIG( "glPushClientAttrib" );
   dllglPushClientAttrib( mask );
}

static void APIENTRY logglPushMatrix(void)
{
   SIG( "glPushMatrix" );
   dllglPushMatrix();
}

static void APIENTRY logglPushName(GLuint name)
{
   SIG( "glPushName" );
   dllglPushName( name );
}

static void APIENTRY logglRasterPos2d(GLdouble x, GLdouble y)
{
   SIG ("glRasterPot2d" );
   dllglRasterPos2d( x, y );
}

static void APIENTRY logglRasterPos2dv(const GLdouble *v)
{
   SIG( "glRasterPos2dv" );
   dllglRasterPos2dv( v );
}

static void APIENTRY logglRasterPos2f(GLfloat x, GLfloat y)
{
   SIG( "glRasterPos2f" );
   dllglRasterPos2f( x, y );
}
static void APIENTRY logglRasterPos2fv(const GLfloat *v)
{
   SIG( "glRasterPos2dv" );
   dllglRasterPos2fv( v );
}
static void APIENTRY logglRasterPos2i(GLint x, GLint y)
{
   SIG( "glRasterPos2if" );
   dllglRasterPos2i( x, y );
}
static void APIENTRY logglRasterPos2iv(const GLint *v)
{
   SIG( "glRasterPos2iv" );
   dllglRasterPos2iv( v );
}
static void APIENTRY logglRasterPos2s(GLshort x, GLshort y)
{
   SIG( "glRasterPos2s" );
   dllglRasterPos2s( x, y );
}
static void APIENTRY logglRasterPos2sv(const GLshort *v)
{
   SIG( "glRasterPos2sv" );
   dllglRasterPos2sv( v );
}
static void APIENTRY logglRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
   SIG( "glRasterPos3d" );
   dllglRasterPos3d( x, y, z );
}
static void APIENTRY logglRasterPos3dv(const GLdouble *v)
{
   SIG( "glRasterPos3dv" );
   dllglRasterPos3dv( v );
}
static void APIENTRY logglRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
   SIG( "glRasterPos3f" );
   dllglRasterPos3f( x, y, z );
}
static void APIENTRY logglRasterPos3fv(const GLfloat *v)
{
   SIG( "glRasterPos3fv" );
   dllglRasterPos3fv( v );
}
static void APIENTRY logglRasterPos3i(GLint x, GLint y, GLint z)
{
   SIG( "glRasterPos3i" );
   dllglRasterPos3i( x, y, z );
}
static void APIENTRY logglRasterPos3iv(const GLint *v)
{
   SIG( "glRasterPos3iv" );
   dllglRasterPos3iv( v );
}
static void APIENTRY logglRasterPos3s(GLshort x, GLshort y, GLshort z)
{
   SIG( "glRasterPos3s" );
   dllglRasterPos3s( x, y, z );
}
static void APIENTRY logglRasterPos3sv(const GLshort *v)
{
   SIG( "glRasterPos3sv" );
   dllglRasterPos3sv( v );
}
static void APIENTRY logglRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
   SIG( "glRasterPos4d" );
   dllglRasterPos4d( x, y, z, w );
}
static void APIENTRY logglRasterPos4dv(const GLdouble *v)
{
   SIG( "glRasterPos4dv" );
   dllglRasterPos4dv( v );
}
static void APIENTRY logglRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
   SIG( "glRasterPos4f" );
   dllglRasterPos4f( x, y, z, w );
}
static void APIENTRY logglRasterPos4fv(const GLfloat *v)
{
   SIG( "glRasterPos4fv" );
   dllglRasterPos4fv( v );
}
static void APIENTRY logglRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
   SIG( "glRasterPos4i" );
   dllglRasterPos4i( x, y, z, w );
}
static void APIENTRY logglRasterPos4iv(const GLint *v)
{
   SIG( "glRasterPos4iv" );
   dllglRasterPos4iv( v );
}
static void APIENTRY logglRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
   SIG( "glRasterPos4s" );
   dllglRasterPos4s( x, y, z, w );
}
static void APIENTRY logglRasterPos4sv(const GLshort *v)
{
   SIG( "glRasterPos4sv" );
   dllglRasterPos4sv( v );
}
static void APIENTRY logglReadBuffer(GLenum mode)
{
   SIG( "glReadBuffer" );
   dllglReadBuffer( mode );
}
static void APIENTRY logglReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
{
   SIG( "glReadPixels" );
   dllglReadPixels( x, y, width, height, format, type, pixels );
}

static void APIENTRY logglRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
   SIG( "glRectd" );
   dllglRectd( x1, y1, x2, y2 );
}

static void APIENTRY logglRectdv(const GLdouble *v1, const GLdouble *v2)
{
   SIG( "glRectdv" );
   dllglRectdv( v1, v2 );
}

static void APIENTRY logglRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
   SIG( "glRectf" );
   dllglRectf( x1, y1, x2, y2 );
}

static void APIENTRY logglRectfv(const GLfloat *v1, const GLfloat *v2)
{
   SIG( "glRectfv" );
   dllglRectfv( v1, v2 );
}
static void APIENTRY logglRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
   SIG( "glRecti" );
   dllglRecti( x1, y1, x2, y2 );
}
static void APIENTRY logglRectiv(const GLint *v1, const GLint *v2)
{
   SIG( "glRectiv" );
   dllglRectiv( v1, v2 );
}
static void APIENTRY logglRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
   SIG( "glRects" );
   dllglRects( x1, y1, x2, y2 );
}
static void APIENTRY logglRectsv(const GLshort *v1, const GLshort *v2)
{
   SIG( "glRectsv" );
   dllglRectsv( v1, v2 );
}
static GLint APIENTRY logglRenderMode(GLenum mode)
{
   SIG( "glRenderMode" );
   return dllglRenderMode( mode );
}
static void APIENTRY logglRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
   SIG( "glRotated" );
   dllglRotated( angle, x, y, z );
}

static void APIENTRY logglRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   SIG( "glRotatef" );
   dllglRotatef( angle, x, y, z );
}

static void APIENTRY logglScaled(GLdouble x, GLdouble y, GLdouble z)
{
   SIG( "glScaled" );
   dllglScaled( x, y, z );
}

static void APIENTRY logglScalef(GLfloat x, GLfloat y, GLfloat z)
{
   SIG( "glScalef" );
   dllglScalef( x, y, z );
}

static void APIENTRY logglScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
   fprintf( winState.log_fp, "glScissor( %d, %d, %d, %d )\n", x, y, width, height );
   fflush(winState.log_fp);
   dllglScissor( x, y, width, height );
}

static void APIENTRY logglSelectBuffer(GLsizei size, GLuint *buffer)
{
   SIG( "glSelectBuffer" );
   dllglSelectBuffer( size, buffer );
}

static void APIENTRY logglShadeModel(GLenum mode)
{
   SIG( "glShadeModel" );
   dllglShadeModel( mode );
}

static void APIENTRY logglStencilFunc(GLenum func, GLint ref, GLuint mask)
{
   SIG( "glStencilFunc" );
   dllglStencilFunc( func, ref, mask );
}

static void APIENTRY logglStencilMask(GLuint mask)
{
   SIG( "glStencilMask" );
   dllglStencilMask( mask );
}

static void APIENTRY logglStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
   SIG( "glStencilOp" );
   dllglStencilOp( fail, zfail, zpass );
}

static void APIENTRY logglTexCoord1d(GLdouble s)
{
   SIG( "glTexCoord1d" );
   dllglTexCoord1d( s );
}

static void APIENTRY logglTexCoord1dv(const GLdouble *v)
{
   SIG( "glTexCoord1dv" );
   dllglTexCoord1dv( v );
}

static void APIENTRY logglTexCoord1f(GLfloat s)
{
   SIG( "glTexCoord1f" );
   dllglTexCoord1f( s );
}
static void APIENTRY logglTexCoord1fv(const GLfloat *v)
{
   SIG( "glTexCoord1fv" );
   dllglTexCoord1fv( v );
}
static void APIENTRY logglTexCoord1i(GLint s)
{
   SIG( "glTexCoord1i" );
   dllglTexCoord1i( s );
}
static void APIENTRY logglTexCoord1iv(const GLint *v)
{
   SIG( "glTexCoord1iv" );
   dllglTexCoord1iv( v );
}
static void APIENTRY logglTexCoord1s(GLshort s)
{
   SIG( "glTexCoord1s" );
   dllglTexCoord1s( s );
}
static void APIENTRY logglTexCoord1sv(const GLshort *v)
{
   SIG( "glTexCoord1sv" );
   dllglTexCoord1sv( v );
}
static void APIENTRY logglTexCoord2d(GLdouble s, GLdouble t)
{
   SIG( "glTexCoord2d" );
   dllglTexCoord2d( s, t );
}

static void APIENTRY logglTexCoord2dv(const GLdouble *v)
{
   SIG( "glTexCoord2dv" );
   dllglTexCoord2dv( v );
}
static void APIENTRY logglTexCoord2f(GLfloat s, GLfloat t)
{
   SIG( "glTexCoord2f" );
   dllglTexCoord2f( s, t );
}
static void APIENTRY logglTexCoord2fv(const GLfloat *v)
{
   SIG( "glTexCoord2fv" );
   dllglTexCoord2fv( v );
}
static void APIENTRY logglTexCoord2i(GLint s, GLint t)
{
   SIG( "glTexCoord2i" );
   dllglTexCoord2i( s, t );
}
static void APIENTRY logglTexCoord2iv(const GLint *v)
{
   SIG( "glTexCoord2iv" );
   dllglTexCoord2iv( v );
}
static void APIENTRY logglTexCoord2s(GLshort s, GLshort t)
{
   SIG( "glTexCoord2s" );
   dllglTexCoord2s( s, t );
}
static void APIENTRY logglTexCoord2sv(const GLshort *v)
{
   SIG( "glTexCoord2sv" );
   dllglTexCoord2sv( v );
}
static void APIENTRY logglTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
   SIG( "glTexCoord3d" );
   dllglTexCoord3d( s, t, r );
}
static void APIENTRY logglTexCoord3dv(const GLdouble *v)
{
   SIG( "glTexCoord3dv" );
   dllglTexCoord3dv( v );
}
static void APIENTRY logglTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
   SIG( "glTexCoord3f" );
   dllglTexCoord3f( s, t, r );
}
static void APIENTRY logglTexCoord3fv(const GLfloat *v)
{
   SIG( "glTexCoord3fv" );
   dllglTexCoord3fv( v );
}
static void APIENTRY logglTexCoord3i(GLint s, GLint t, GLint r)
{
   SIG( "glTexCoord3i" );
   dllglTexCoord3i( s, t, r );
}
static void APIENTRY logglTexCoord3iv(const GLint *v)
{
   SIG( "glTexCoord3iv" );
   dllglTexCoord3iv( v );
}
static void APIENTRY logglTexCoord3s(GLshort s, GLshort t, GLshort r)
{
   SIG( "glTexCoord3s" );
   dllglTexCoord3s( s, t, r );
}
static void APIENTRY logglTexCoord3sv(const GLshort *v)
{
   SIG( "glTexCoord3sv" );
   dllglTexCoord3sv( v );
}
static void APIENTRY logglTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
   SIG( "glTexCoord4d" );
   dllglTexCoord4d( s, t, r, q );
}
static void APIENTRY logglTexCoord4dv(const GLdouble *v)
{
   SIG( "glTexCoord4dv" );
   dllglTexCoord4dv( v );
}
static void APIENTRY logglTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   SIG( "glTexCoord4f" );
   dllglTexCoord4f( s, t, r, q );
}
static void APIENTRY logglTexCoord4fv(const GLfloat *v)
{
   SIG( "glTexCoord4fv" );
   dllglTexCoord4fv( v );
}
static void APIENTRY logglTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
   SIG( "glTexCoord4i" );
   dllglTexCoord4i( s, t, r, q );
}
static void APIENTRY logglTexCoord4iv(const GLint *v)
{
   SIG( "glTexCoord4iv" );
   dllglTexCoord4iv( v );
}
static void APIENTRY logglTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
   SIG( "glTexCoord4s" );
   dllglTexCoord4s( s, t, r, q );
}
static void APIENTRY logglTexCoord4sv(const GLshort *v)
{
   SIG( "glTexCoord4sv" );
   dllglTexCoord4sv( v );
}
static void APIENTRY logglTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
   fprintf( winState.log_fp, "glTexCoordPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
   fflush(winState.log_fp);
   dllglTexCoordPointer( size, type, stride, pointer );
}

static void APIENTRY logglTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
   fprintf( winState.log_fp, "glTexEnvf( 0x%x, 0x%x, %g )\n", target, pname, param );
   fflush(winState.log_fp);
   dllglTexEnvf( target, pname, param );
}

static void APIENTRY logglTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
   SIG( "glTexEnvfv" );
   dllglTexEnvfv( target, pname, params );
}

static void APIENTRY logglTexEnvi(GLenum target, GLenum pname, GLint param)
{
   fprintf( winState.log_fp, "glTexEnvi( 0x%x, 0x%x, 0x%x )\n", target, pname, param );
   fflush(winState.log_fp);
   dllglTexEnvi( target, pname, param );
}
static void APIENTRY logglTexEnviv(GLenum target, GLenum pname, const GLint *params)
{
   SIG( "glTexEnviv" );
   dllglTexEnviv( target, pname, params );
}

static void APIENTRY logglTexGend(GLenum coord, GLenum pname, GLdouble param)
{
   SIG( "glTexGend" );
   dllglTexGend( coord, pname, param );
}

static void APIENTRY logglTexGendv(GLenum coord, GLenum pname, const GLdouble *params)
{
   SIG( "glTexGendv" );
   dllglTexGendv( coord, pname, params );
}

static void APIENTRY logglTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
   SIG( "glTexGenf" );
   dllglTexGenf( coord, pname, param );
}
static void APIENTRY logglTexGenfv(GLenum coord, GLenum pname, const GLfloat *params)
{
//   fprintf( winState.log_fp, "glTexGenfv( %s, (%g, %g, %g, %g) )\n", CoordToString( coord ), params[0], params[1], params[2], params[3]);
   fflush(winState.log_fp);
   fprintf( winState.log_fp, "glTexGenfv( %s, MEM )\n", CoordToString( coord ));
   fflush(winState.log_fp);
   dllglTexGenfv( coord, pname, params );
}
static void APIENTRY logglTexGeni(GLenum coord, GLenum pname, GLint param)
{
   SIG( "glTexGeni" );
   dllglTexGeni( coord, pname, param );
}
static void APIENTRY logglTexGeniv(GLenum coord, GLenum pname, const GLint *params)
{
   SIG( "glTexGeniv" );
   dllglTexGeniv( coord, pname, params );
}
static void APIENTRY logglTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels)
{
   SIG( "glTexImage1D" );
   dllglTexImage1D( target, level, internalformat, width, border, format, type, pixels );
}
static void APIENTRY logglTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
{
   SIG( "glTexImage2D" );
   dllglTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}

static void APIENTRY logglTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
   fprintf( winState.log_fp, "glTexParameterf( 0x%x, 0x%x, %g )\n", target, pname, param );
   fflush(winState.log_fp);
   dllglTexParameterf( target, pname, param );
}

static void APIENTRY logglTexParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
   SIG( "glTexParameterfv" );
   dllglTexParameterfv( target, pname, params );
}
static void APIENTRY logglTexParameteri(GLenum target, GLenum pname, GLint param)
{
   fprintf( winState.log_fp, "glTexParameteri( 0x%x, 0x%x, 0x%x )\n", target, pname, param );
   fflush(winState.log_fp);
   dllglTexParameteri( target, pname, param );
}
static void APIENTRY logglTexParameteriv(GLenum target, GLenum pname, const GLint *params)
{
   SIG( "glTexParameteriv" );
   dllglTexParameteriv( target, pname, params );
}
static void APIENTRY logglTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels)
{
   SIG( "glTexSubImage1D" );
   dllglTexSubImage1D( target, level, xoffset, width, format, type, pixels );
}
static void APIENTRY logglTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
   SIG( "glTexSubImage2D" );
   dllglTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}
static void APIENTRY logglTranslated(GLdouble x, GLdouble y, GLdouble z)
{
   SIG( "glTranslated" );
   dllglTranslated( x, y, z );
}

static void APIENTRY logglTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
   SIG( "glTranslatef" );
   dllglTranslatef( x, y, z );
}

static void APIENTRY logglVertex2d(GLdouble x, GLdouble y)
{
   SIG( "glVertex2d" );
   dllglVertex2d( x, y );
}

static void APIENTRY logglVertex2dv(const GLdouble *v)
{
   SIG( "glVertex2dv" );
   dllglVertex2dv( v );
}
static void APIENTRY logglVertex2f(GLfloat x, GLfloat y)
{
   SIG( "glVertex2f" );
   dllglVertex2f( x, y );
}
static void APIENTRY logglVertex2fv(const GLfloat *v)
{
   SIG( "glVertex2fv" );
   dllglVertex2fv( v );
}
static void APIENTRY logglVertex2i(GLint x, GLint y)
{
   SIG( "glVertex2i" );
   dllglVertex2i( x, y );
}
static void APIENTRY logglVertex2iv(const GLint *v)
{
   SIG( "glVertex2iv" );
   dllglVertex2iv( v );
}
static void APIENTRY logglVertex2s(GLshort x, GLshort y)
{
   SIG( "glVertex2s" );
   dllglVertex2s( x, y );
}
static void APIENTRY logglVertex2sv(const GLshort *v)
{
   SIG( "glVertex2sv" );
   dllglVertex2sv( v );
}
static void APIENTRY logglVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
   SIG( "glVertex3d" );
   dllglVertex3d( x, y, z );
}
static void APIENTRY logglVertex3dv(const GLdouble *v)
{
   SIG( "glVertex3dv" );
   dllglVertex3dv( v );
}
static void APIENTRY logglVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
   SIG( "glVertex3f" );
   dllglVertex3f( x, y, z );
}
static void APIENTRY logglVertex3fv(const GLfloat *v)
{
   SIG( "glVertex3fv" );
   dllglVertex3fv( v );
}
static void APIENTRY logglVertex3i(GLint x, GLint y, GLint z)
{
   SIG( "glVertex3i" );
   dllglVertex3i( x, y, z );
}
static void APIENTRY logglVertex3iv(const GLint *v)
{
   SIG( "glVertex3iv" );
   dllglVertex3iv( v );
}
static void APIENTRY logglVertex3s(GLshort x, GLshort y, GLshort z)
{
   SIG( "glVertex3s" );
   dllglVertex3s( x, y, z );
}
static void APIENTRY logglVertex3sv(const GLshort *v)
{
   SIG( "glVertex3sv" );
   dllglVertex3sv( v );
}
static void APIENTRY logglVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
   SIG( "glVertex4d" );
   dllglVertex4d( x, y, z, w );
}
static void APIENTRY logglVertex4dv(const GLdouble *v)
{
   SIG( "glVertex4dv" );
   dllglVertex4dv( v );
}
static void APIENTRY logglVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
   SIG( "glVertex4f" );
   dllglVertex4f( x, y, z, w );
}
static void APIENTRY logglVertex4fv(const GLfloat *v)
{
   SIG( "glVertex4fv" );
   dllglVertex4fv( v );
}
static void APIENTRY logglVertex4i(GLint x, GLint y, GLint z, GLint w)
{
   SIG( "glVertex4i" );
   dllglVertex4i( x, y, z, w );
}
static void APIENTRY logglVertex4iv(const GLint *v)
{
   SIG( "glVertex4iv" );
   dllglVertex4iv( v );
}
static void APIENTRY logglVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
   SIG( "glVertex4s" );
   dllglVertex4s( x, y, z, w );
}
static void APIENTRY logglVertex4sv(const GLshort *v)
{
   SIG( "glVertex4sv" );
   dllglVertex4sv( v );
}
static void APIENTRY logglVertexPointer(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
   fprintf( winState.log_fp, "glVertexPointer( %d, %s, %d, MEM )\n", size, TypeToString( type ), stride );
   fflush(winState.log_fp);
   dllglVertexPointer( size, type, stride, pointer );
}
static void APIENTRY logglViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
   fprintf( winState.log_fp, "glViewport( %d, %d, %d, %d )\n", x, y, width, height );
   fflush(winState.log_fp);
   dllglViewport( x, y, width, height );
}

static void APIENTRY logglColorTableEXT(GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* data)
{
   AssertFatal(dllglColorTableEXT != NULL, "Error, shouldn't have called unsupported paletted_texture extension");

   fprintf(winState.log_fp, "glColorTableEXT(%d, %d, %d, %d, %d, <data>)\n",
           target,
           internalFormat,
           width,
           format,
           type);
   fflush(winState.log_fp);
   dllglColorTableEXT(target, internalFormat, width, format, type, data);
}

static void APIENTRY logglLockArraysEXT(GLint first, GLsizei count)
{
   AssertFatal(dllglLockArraysEXT != NULL, "Error, shouldn't have called unsupported compiled_vertex_array extension");

   fprintf( winState.log_fp, "glLockArraysEXT( %d, %d )\n", first, count);
   fflush(winState.log_fp);
   dllglLockArraysEXT(first, count);
}

static void APIENTRY logglUnlockArraysEXT()
{
   AssertFatal(dllglLockArraysEXT != NULL, "Error, shouldn't have called unsupported compiled_vertex_array extension");

   SIG("glUnlockArraysEXT");
   dllglUnlockArraysEXT();
}

/* ARB_multitexture */

static const char* gARBMTenums[] = {
   "GL_TEXTURE0_ARB",  "GL_TEXTURE1_ARB",  "GL_TEXTURE2_ARB",
   "GL_TEXTURE3_ARB",  "GL_TEXTURE4_ARB",  "GL_TEXTURE5_ARB",
   "GL_TEXTURE6_ARB",  "GL_TEXTURE7_ARB",  "GL_TEXTURE8_ARB",
   "GL_TEXTURE9_ARB",  "GL_TEXTURE10_ARB", "GL_TEXTURE11_ARB",
   "GL_TEXTURE12_ARB", "GL_TEXTURE13_ARB", "GL_TEXTURE14_ARB",
   "GL_TEXTURE15_ARB", "GL_TEXTURE16_ARB", "GL_TEXTURE17_ARB",
   "GL_TEXTURE18_ARB", "GL_TEXTURE19_ARB", "GL_TEXTURE20_ARB",
   "GL_TEXTURE21_ARB", "GL_TEXTURE22_ARB", "GL_TEXTURE23_ARB",
   "GL_TEXTURE24_ARB", "GL_TEXTURE25_ARB", "GL_TEXTURE26_ARB",
   "GL_TEXTURE27_ARB", "GL_TEXTURE28_ARB", "GL_TEXTURE29_ARB",
   "GL_TEXTURE30_ARB", "GL_TEXTURE31_ARB"
};

static void APIENTRY logglActiveTextureARB(GLenum target)
{
   U32 index = target - GL_TEXTURE0_ARB;

   fprintf( winState.log_fp, "glActiveTexturesARB( %s )\n", gARBMTenums[index]);
   fflush(winState.log_fp);
   dllglActiveTextureARB(target);
}

static void APIENTRY logglClientActiveTextureARB(GLenum target)
{
   U32 index = target - GL_TEXTURE0_ARB;

   fprintf( winState.log_fp, "glClientActiveTexturesARB( %s )\n", gARBMTenums[index]);
   fflush(winState.log_fp);
   dllglClientActiveTextureARB(target);
}

static void APIENTRY logglMultiTexCoord2fARB(GLenum texture, GLfloat x, GLfloat y)
{
   U32 index = texture - GL_TEXTURE0_ARB;

   fprintf( winState.log_fp, "glMultiTexCoord2fARB( %s, %g, %g )\n", gARBMTenums[index], x, y);
   fflush(winState.log_fp);
   dllglMultiTexCoord2fARB(texture, x, y);
}

static void APIENTRY logglMultiTexCoord2fvARB(GLenum texture, const GLfloat* p)
{
   U32 index = texture - GL_TEXTURE0_ARB;

   fprintf( winState.log_fp, "glMultiTexCoord2fARB( %s, [%g, %g] )\n", gARBMTenums[index], p[0], p[1]);
   fflush(winState.log_fp);
   dllglMultiTexCoord2fvARB(texture, p);
}

/* NV_vertex_array_range */

static void APIENTRY logglVertexArrayRangeNV(GLsizei length, void* pointer)
{
   fprintf(winState.log_fp, "glVertexArrayRangeNV( %d, MEMORY )", length);
   fflush(winState.log_fp);
   dllglVertexArrayRangeNV(length, pointer);
}

static void APIENTRY logglFlushVertexArrayRangeNV()
{
   SIG("glFlushVertexArrayRangeNV");
   dllglFlushVertexArrayRangeNV();
}

static void* APIENTRY logwglAllocateMemoryNV(GLsizei length, GLfloat read, GLfloat write, GLfloat priority)
{
   fprintf(winState.log_fp, "wglAllocateMemoryNV( %d, %g, %g, %g)", length, read, write, priority);
   fflush(winState.log_fp);
   return dllwglAllocateMemoryNV(length, read, write, priority);
}

static void APIENTRY logwglFreeMemoryNV(void* pointer)
{
   SIG("glFreeMemoryNV(MEM)");
   dllwglFreeMemoryNV(pointer);
}

/* EXT_fog_coord */

static void APIENTRY logglFogCoordfEXT(GLfloat coord)
{
   fprintf( winState.log_fp, "glFogCoordEXT(%g)\n", coord);
   fflush(winState.log_fp);
   dllglFogCoordfEXT(coord);
}

static void APIENTRY logglFogCoordPointerEXT(GLenum type, GLsizei stride, void *pointer)
{
   fprintf( winState.log_fp, "glFogCoordPointerEXT(%s, %d, MEMORY)\n", TypeToString(type), stride);
   fflush(winState.log_fp);
   dllglFogCoordPointerEXT(type, stride, pointer);
}

/* ARB_texture_compression */

static void APIENTRY logglCompressedTexImage3DARB(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexImage3DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexImage3DARB(target, level, internalformat, width, height, depth, border, imageSize, data);
}

static void APIENTRY logglCompressedTexImage2DARB(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexImage3DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexImage2DARB(target, level, internalformat, width, height, border, imageSize, data);
}

static void APIENTRY logglCompressedTexImage1DARB(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexImage3DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexImage1DARB(target, level, internalformat, width, border, imageSize, data);
}

static void APIENTRY logglCompressedTexSubImage3DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexSubImage3DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexSubImage3DARB(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

static void APIENTRY logglCompressedTexSubImage2DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexSubImage2DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexSubImage2DARB(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

static void APIENTRY logglCompressedTexSubImage1DARB(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)
{
   fprintf( winState.log_fp, "glCompressedTexSubImage1DARB(...)\n");
   fflush(winState.log_fp);
   dllglCompressedTexSubImage1DARB(target, level, xoffset, width, format, imageSize, data);
}

static void APIENTRY logglGetCompressedTexImageARB(GLenum target, GLint lod, void* img)
{
   fprintf( winState.log_fp, "glGetCompressedTexImage3DARB(...)\n");
   fflush(winState.log_fp);
   dllglGetCompressedTexImageARB(target, lod, img);
}

/* EXT_vertex_buffer */

static GLboolean APIENTRY logglAvailableVertexBufferEXT()
{
    fprintf( winState.log_fp, "glAvailableVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    return dllglAvailableVertexBufferEXT();
}

static GLint APIENTRY logglAllocateVertexBufferEXT(GLsizei size, GLint format, GLboolean preserve)
{
    fprintf( winState.log_fp, "glAllocateVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    return dllglAllocateVertexBufferEXT(size, format, preserve);
}

static void * APIENTRY logglLockVertexBufferEXT(GLint handle, GLsizei size)
{
    fprintf( winState.log_fp, "glLockVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    return dllglLockVertexBufferEXT(handle, size);
}

static void APIENTRY logglUnlockVertexBufferEXT(GLint handle)
{
    fprintf( winState.log_fp, "glUnlockVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    dllglUnlockVertexBufferEXT(handle);
}

static void APIENTRY logglSetVertexBufferEXT(GLint handle)
{
    fprintf( winState.log_fp, "glSetVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    dllglSetVertexBufferEXT(handle);
}

static void APIENTRY logglOffsetVertexBufferEXT(GLint handle, GLuint offset)
{
    fprintf( winState.log_fp, "glOffsetVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    dllglOffsetVertexBufferEXT(handle, offset);
}

static void APIENTRY logglFillVertexBufferEXT(GLint handle, GLint first, GLsizei count)
{
    fprintf( winState.log_fp, "glFillVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    dllglFillVertexBufferEXT(handle, first, count);
}

static void APIENTRY logglFreeVertexBufferEXT(GLint handle)
{
    fprintf( winState.log_fp, "glFreeVertexBufferEXT(...)\n");
    fflush(winState.log_fp);
    dllglFreeVertexBufferEXT(handle);
}

static void APIENTRY logglBlendColorEXT(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
   fprintf( winState.log_fp, "glBlendColorEXT( %g, %g, %g, %g )\n", red, green, blue, alpha );
   fflush(winState.log_fp);
   dllglBlendColorEXT(red, green, blue, alpha);
}

static void APIENTRY logglBlendEquationEXT(GLenum mode)
{
   fprintf( winState.log_fp, "glBlendEquationEXT( %d )\n", mode );
   fflush(winState.log_fp);
   dllglBlendEquationEXT(mode);
}

//-------------------------------------------------------
static U32 getIndex(GLenum type, const void *indices, U32 i)
{
   if(type == GL_UNSIGNED_BYTE)
      return ((U8 *) indices)[i];
   else if(type == GL_UNSIGNED_SHORT)
      return ((U16 *) indices)[i];
   else
      return ((U32 *) indices)[i];
}

static BOOL WINAPI outlineSwapBuffers(HDC dc)
{
   bool ret = dlldwglSwapBuffers(dc);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   return ret;
}

static void APIENTRY outlineDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
   if(mode == GL_POLYGON)
      mode = GL_LINE_LOOP;

   if(mode == GL_POINTS || mode == GL_LINE_STRIP || mode == GL_LINE_LOOP || mode == GL_LINES)
      dllglDrawElements( mode, count, type, indices );
   else
   {
      glBegin(GL_LINES);
      if(mode == GL_TRIANGLE_STRIP)
      {
         U32 i;
         for(i = 0; i < (U32)(count - 1); i++)
         {
            dllglArrayElement(getIndex(type, indices, i));
            dllglArrayElement(getIndex(type, indices, i + 1));
            if(i + 2 != count)
            {
               dllglArrayElement(getIndex(type, indices, i));
               dllglArrayElement(getIndex(type, indices, i + 2));
            }
         }
      }
      else if(mode == GL_TRIANGLE_FAN)
      {
         for(U32 i = 1; i < (U32)count; i ++)
         {
            dllglArrayElement(getIndex(type, indices, 0));
            dllglArrayElement(getIndex(type, indices, i));
            if(i != count - 1)
            {
               dllglArrayElement(getIndex(type, indices, i));
               dllglArrayElement(getIndex(type, indices, i + 1));
            }
         }
      }
      else if(mode == GL_TRIANGLES)
      {
         for(U32 i = 3; i <= (U32)count; i += 3)
         {
            dllglArrayElement(getIndex(type, indices, i - 3));
            dllglArrayElement(getIndex(type, indices, i - 2));
            dllglArrayElement(getIndex(type, indices, i - 2));
            dllglArrayElement(getIndex(type, indices, i - 1));
            dllglArrayElement(getIndex(type, indices, i - 3));
            dllglArrayElement(getIndex(type, indices, i - 1));
         }
      }
      else if(mode == GL_QUADS)
      {
         for(U32 i = 4; i <= (U32)count; i += 4)
         {
            dllglArrayElement(getIndex(type, indices, i - 4));
            dllglArrayElement(getIndex(type, indices, i - 3));
            dllglArrayElement(getIndex(type, indices, i - 3));
            dllglArrayElement(getIndex(type, indices, i - 2));
            dllglArrayElement(getIndex(type, indices, i - 2));
            dllglArrayElement(getIndex(type, indices, i - 1));
            dllglArrayElement(getIndex(type, indices, i - 4));
            dllglArrayElement(getIndex(type, indices, i - 1));
         }
      }
      else if(mode == GL_QUAD_STRIP)
      {
         if(count < 4)
            return;
         dllglArrayElement(getIndex(type, indices, 0));
         dllglArrayElement(getIndex(type, indices, 1));
         for(U32 i = 4; i <= (U32)count; i += 2)
         {
            dllglArrayElement(getIndex(type, indices, i - 4));
            dllglArrayElement(getIndex(type, indices, i - 2));

            dllglArrayElement(getIndex(type, indices, i - 3));
            dllglArrayElement(getIndex(type, indices, i - 1));

            dllglArrayElement(getIndex(type, indices, i - 2));
            dllglArrayElement(getIndex(type, indices, i - 1));
         }
      }
      glEnd();
   }
}

static void APIENTRY outlineDrawArrays(GLenum mode, GLint first, GLsizei count)
{
   if(mode == GL_POLYGON)
      mode = GL_LINE_LOOP;

   if(mode == GL_POINTS || mode == GL_LINE_STRIP || mode == GL_LINE_LOOP || mode == GL_LINES)
      dllglDrawArrays( mode, first, count );
   else
   {
      glBegin(GL_LINES);
      if(mode == GL_TRIANGLE_STRIP)
      {
         U32 i;
         for(i = 0; i < (U32)count - 1; i++)
         {
            dllglArrayElement(first + i);
            dllglArrayElement(first + i + 1);
            if(i + 2 != count)
            {
               dllglArrayElement(first + i);
               dllglArrayElement(first + i + 2);
            }
         }
      }
      else if(mode == GL_TRIANGLE_FAN)
      {
         for(U32 i = 1; i < (U32)count; i ++)
         {
            dllglArrayElement(first);
            dllglArrayElement(first + i);
            if(i != count - 1)
            {
               dllglArrayElement(first + i);
               dllglArrayElement(first + i + 1);
            }
         }
      }
      else if(mode == GL_TRIANGLES)
      {
         for(U32 i = 3; i <= (U32)count; i += 3)
         {
            dllglArrayElement(first + i - 3);
            dllglArrayElement(first + i - 2);
            dllglArrayElement(first + i - 2);
            dllglArrayElement(first + i - 1);
            dllglArrayElement(first + i - 3);
            dllglArrayElement(first + i - 1);
         }
      }
      else if(mode == GL_QUADS)
      {
         for(U32 i = 4; i <= (U32)count; i += 4)
         {
            dllglArrayElement(first + i - 4);
            dllglArrayElement(first + i - 3);
            dllglArrayElement(first + i - 3);
            dllglArrayElement(first + i - 2);
            dllglArrayElement(first + i - 2);
            dllglArrayElement(first + i - 1);
            dllglArrayElement(first + i - 4);
            dllglArrayElement(first + i - 1);
         }
      }
      else if(mode == GL_QUAD_STRIP)
      {
         if(count < 4)
            return;
         dllglArrayElement(first + 0);
         dllglArrayElement(first + 1);
         for(U32 i = 4; i <= (U32)count; i += 2)
         {
            dllglArrayElement(first + i - 4);
            dllglArrayElement(first + i - 2);

            dllglArrayElement(first + i - 3);
            dllglArrayElement(first + i - 1);

            dllglArrayElement(first + i - 2);
            dllglArrayElement(first + i - 1);
         }
      }
      glEnd();
   }
}

static void APIENTRY perfDrawArrays(GLenum mode, GLint first, GLsizei count)
{
   gGLState.primCount[gGLState.primMode]++;
   U32 tc = 0;

   if(mode == GL_TRIANGLES)
      tc = count / 3;
   else if(mode == GL_TRIANGLE_FAN || mode == GL_TRIANGLE_STRIP)
      tc = count - 2;

   gGLState.triCount[gGLState.primMode] += tc;
   dllglDrawArrays( mode, first, count );
}

static void APIENTRY perfDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
   gGLState.primCount[gGLState.primMode]++;
   U32 tc = 0;

   if(mode == GL_TRIANGLES)
      tc = count / 3;
   else if(mode == GL_TRIANGLE_FAN || mode == GL_TRIANGLE_STRIP)
      tc = count - 2;

   gGLState.triCount[gGLState.primMode] += tc;
   dllglDrawElements( mode, count, type, indices );
}

#include "winGLSpecial_ScriptBinding.h"
