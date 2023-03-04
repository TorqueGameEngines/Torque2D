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

/*! @addtogroup WindowsPlatform Windows Platform
	@ingroup TorqueScriptFunctions
	@{
*/

#if defined (TORQUE_DEBUG) || defined(INTERNAL_RELEASE)

/*! Use the GLEnableLogging function to enable/disable the gathering of OpenGL metrics.
    For this to work, the engine must have been compiled with either TORQUE_DEBUG or INTERNAL_RELEASE defined. Always be sure to do a disable after an enable to flush the last log data to the log file.
    @param enable A boolean value. If set to true, the engine will gather various OpenGL metrics and dump them to a file named gl_log.txt. If set to false, logging is stopped, the last writes to the log are flushed, and the file is closed.
    @return No return value.
    @sa GLEnableMetrics, metrics
*/
ConsoleFunctionWithDocs(GLEnableLogging, ConsoleVoid, 2, 2, ( enable ))
{
   argc;
   bool enable = dAtob(argv[1]);

   if(loggingEnabled == enable)
      return;

   if(enable && (outlineEnabled || perfEnabled))
      return;

   loggingEnabled = enable;

   if ( enable )
   {
      if ( !winState.log_fp )
      {
         struct tm *newtime;
         time_t aclock;

         time( &aclock );
         newtime = localtime( &aclock );

         winState.log_fp = fopen( "gl_log.txt", "wt" );

         fprintf( winState.log_fp, "%s\n", asctime( newtime ) );
         fflush(winState.log_fp);
      }

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_name = log##fn_name;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#undef GL_FUNCTION
   }
   else
   {
      if ( winState.log_fp )
      {
         fprintf( winState.log_fp, "*** CLOSING LOG ***\n" );
         fflush(winState.log_fp);
         fclose( winState.log_fp );
         winState.log_fp = NULL;
      }

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_name = dll##fn_name;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#undef GL_FUNCTION
   }
}

#endif

/*! Enables outlines with OpenGL
    @param enable Boolean value representing whether or not outlines should be enabled.
    @return No Return Value.
*/
ConsoleFunctionWithDocs(GLEnableOutline, ConsoleVoid, 2, 2, (bool))
{
   argc;
   bool enable = dAtob(argv[1]);
   if(outlineEnabled == enable)
      return;

   if(enable && (loggingEnabled || perfEnabled))
      return;

   outlineEnabled = enable;

   if ( enable )
   {
      glDrawElements = outlineDrawElements;
      glDrawArrays = outlineDrawArrays;
      dwglSwapBuffers = outlineSwapBuffers;
   }
   else
   {
      glDrawElements = dllglDrawElements;
      glDrawArrays = dllglDrawArrays;
      dwglSwapBuffers = dlldwglSwapBuffers;
   }
}

/*! Use the GLEnableMetrics function to enable or disable logging of OpenGL texture and video metrics.
    For this to work, the engine must have been compiled with either TORQUE_DEBUG or INTERNAL_RELEASE defined. Use the metrics function to get at this information. Also, once this feature is enabled, the following globals will be available for inspection/examination: <OpenGL::triCount0 ? Terrain triangles, OpenGL::triCount1? DIF triangles, OpenGL::triCount2 ? DTS triangles, OpenGL::triCount3 ? Uncategorized triangles, OpenGL::primCount0 - Terrain primitives, OpenGL::primCount1 ? DIF primitives, OpenGL::primCount2 ? DTS primitives, OpenGL::primCount3 ? Uncategorized primitives>
    @param enable A boolean value. When this is set to true, texture and video (triangles and primitives) logging is enabled and dumped as part of calls to certain metrics.
    @return No return value.
    @sa GLEnableLogging, metrics
*/
ConsoleFunctionWithDocs(GLEnableMetrics, ConsoleVoid, 2, 2, ( enable ))
{
   argc;
   static bool varsAdded = false;

   if(!varsAdded)
   {
      Con::addVariable("OpenGL::triCount0", TypeS32, &gGLState.triCount[0]);
      Con::addVariable("OpenGL::triCount1", TypeS32, &gGLState.triCount[1]);
      Con::addVariable("OpenGL::triCount2", TypeS32, &gGLState.triCount[2]);
      Con::addVariable("OpenGL::triCount3", TypeS32, &gGLState.triCount[3]);

      Con::addVariable("OpenGL::primCount0", TypeS32, &gGLState.primCount[0]);
      Con::addVariable("OpenGL::primCount1", TypeS32, &gGLState.primCount[1]);
      Con::addVariable("OpenGL::primCount2", TypeS32, &gGLState.primCount[2]);
      Con::addVariable("OpenGL::primCount3", TypeS32, &gGLState.primCount[3]);
      varsAdded = true;
   }

   bool enable = dAtob(argv[1]);
   if(perfEnabled == enable)
      return;

   if(enable && (loggingEnabled || outlineEnabled))
      return;

   perfEnabled = enable;

   if ( enable )
   {
      glDrawElements = perfDrawElements;
      glDrawArrays = perfDrawArrays;
   }
   else
   {
      glDrawElements = dllglDrawElements;
      glDrawArrays = dllglDrawArrays;
   }
}

/*! @} */ // end group WindowsPlatform
