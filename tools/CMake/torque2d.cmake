project(${TORQUE_APP_NAME})

if(UNIX)
    if(NOT CXX_FLAG32)
        set(CXX_FLAG32 "")
    endif()
    #set(CXX_FLAG32 "-m32") #uncomment for build x32 on OSx64

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -Wno-return-type-c-linkage -Wno-unused-local-typedef ${TORQUE_ADDITIONAL_LINKER_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -Wno-return-type-c-linkage -Wno-unused-local-typedef ${TORQUE_ADDITIONAL_LINKER_FLAGS}")
    else()
    # default compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -no-pie ${TORQUE_ADDITIONAL_LINKER_FLAGS} -Wl,-rpath,'$$ORIGIN'")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors ${TORQUE_ADDITIONAL_LINKER_FLAGS} -Wl,-rpath,'$$ORIGIN'")

   endif()

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
endif()

if(UNIX AND NOT APPLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX")
endif()

# TODO: fmod support

###############################################################################
# modules
###############################################################################

option(TORQUE_SFX_VORBIS "Vorbis Sound" ON)
mark_as_advanced(TORQUE_SFX_VORBIS)
option(TORQUE_SFX_OPENAL "OpenAL Sound" ON)

if(TORQUE_SFX_OPENAL)
    #Hide some unnecessary fields as advanced
    mark_as_advanced(ALSOFT_AMBDEC_PRESETS)
    mark_as_advanced(ALSOFT_BACKEND_DSOUND)
    mark_as_advanced(ALSOFT_BACKEND_MMDEVAPI)
    mark_as_advanced(ALSOFT_BACKEND_WAVE)
    mark_as_advanced(ALSOFT_BACKEND_WINMM)
    mark_as_advanced(ALSOFT_CONFIG)
    mark_as_advanced(ALSOFT_CPUEXT_SSE)
    mark_as_advanced(ALSOFT_CPUEXT_SSE2)
    mark_as_advanced(ALSOFT_CPUEXT_SSE3)
    mark_as_advanced(ALSOFT_CPUEXT_SSE4_1)
    mark_as_advanced(ALSOFT_DLOPEN)
	set(ALSOFT_DLOPEN ON CACHE BOOL "Test for open Dll" FORCE)
    mark_as_advanced(ALSOFT_EMBED_HRTF_DATA)
    mark_as_advanced(ALSOFT_EXAMPLES)
    mark_as_advanced(ALSOFT_HRTF_DEFS)
    mark_as_advanced(ALSOFT_INSTALL)
    mark_as_advanced(ALSOFT_NO_CONFIG_UTIL)
    mark_as_advanced(ALSOFT_NO_UID_DEFS)
    mark_as_advanced(ALSOFT_REQUIRE_ALSA)
    mark_as_advanced(ALSOFT_REQUIRE_COREAUDIO)
    mark_as_advanced(ALSOFT_REQUIRE_DSOUND)
    mark_as_advanced(ALSOFT_REQUIRE_JACK)
    mark_as_advanced(ALSOFT_REQUIRE_MMDEVAPI)
    mark_as_advanced(ALSOFT_REQUIRE_NEON)
    mark_as_advanced(ALSOFT_REQUIRE_OPENSL)
    mark_as_advanced(ALSOFT_REQUIRE_OSS)
    mark_as_advanced(ALSOFT_REQUIRE_PORTAUDIO)
    mark_as_advanced(ALSOFT_REQUIRE_PULSEAUDIO)
    mark_as_advanced(ALSOFT_REQUIRE_QSA)
    mark_as_advanced(ALSOFT_REQUIRE_SNDIO)
    mark_as_advanced(ALSOFT_REQUIRE_SOLARIS)
    mark_as_advanced(ALSOFT_REQUIRE_SSE)
    mark_as_advanced(ALSOFT_REQUIRE_SSE2)
    mark_as_advanced(ALSOFT_REQUIRE_SSE4_1)
    mark_as_advanced(ALSOFT_REQUIRE_WINMM)
    mark_as_advanced(ALSOFT_TESTS)
    mark_as_advanced(ALSOFT_UTILS)
    mark_as_advanced(ALSOFT_WERROR)
    mark_as_advanced(COREAUDIO_FRAMEWORK)
    mark_as_advanced(CMAKE_DEBUG_POSTFIX)
    mark_as_advanced(FORCE_STATIC_VCRT)
    mark_as_advanced(ALSOFT_BACKEND_WASAPI)
    mark_as_advanced(ALSOFT_BUILD_ROUTER)
    mark_as_advanced(ALSOFT_REQUIRE_SDL2)
	set(ALSOFT_REQUIRE_SDL2 OFF CACHE BOOL "T2D Does not have SDL" FORCE)
    mark_as_advanced(ALSOFT_REQUIRE_WASAPI)
    #the following is from openal-soft
    mark_as_advanced(SDL2MAIN_LIBRARY)
    mark_as_advanced(SDL2_CORE_LIBRARY)
    mark_as_advanced(SDL2_INCLUDE_DIR)
endif()

mark_as_advanced(TORQUE_SFX_OPENAL)

###############################################################################
# options
###############################################################################
if(UNIX AND NOT APPLE) # handle single-configuration generator
    set(TORQUE_BUILD_TYPE "Debug" CACHE STRING "Select one of Debug, Release and RelWithDebInfo")
    set_property(CACHE TORQUE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

    set(TORQUE_ADDITIONAL_LINKER_FLAGS "" CACHE STRING "Additional linker flags")
    mark_as_advanced(TORQUE_ADDITIONAL_LINKER_FLAGS)
endif()

if(WIN32)
    # warning C4800: 'XXX' : forcing value to bool 'true' or 'false' (performance warning)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4800")
    # warning C4018: '<' : signed/unsigned mismatch
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4018")
    # warning C4244: 'initializing' : conversion from 'XXX' to 'XXX', possible loss of data
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4244")

endif()

# build types
if(UNIX AND NOT APPLE) # handle single-configuration generator
	set(CMAKE_BUILD_TYPE ${TORQUE_BUILD_TYPE})
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(TORQUE_DEBUG TRUE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(TORQUE_RELEASE TRUE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(TORQUE_RELEASE TRUE)
    else()
		message(FATAL_ERROR "Please select Debug, Release or RelWithDebInfo for TORQUE_BUILD_TYPE")
	endif()
endif()

###############################################################################
# Always enabled paths first
###############################################################################
addPath("${srcDir}/") # must come first :)
addPath("${srcDir}/algorithm")
addPath("${srcDir}/assets")
addPath("${srcDir}/audio")
addPath("${srcDir}/bitmapFont")
addPathRec("${srcDir}/Box2D")
addPath("${srcDir}/collection")
addPath("${srcDir}/component")
addPath("${srcDir}/component/behaviors")
addPath("${srcDir}/console")
addPath("${srcDir}/debug")
addPath("${srcDir}/debug/remote")
addPath("${srcDir}/delegates")
addPath("${srcDir}/game")
addPath("${srcDir}/sfx")
set(BLACKLIST "bitmapPvr.cc" )
addPath("${srcDir}/graphics")
if(WIN32)
set(BLACKLIST "")
endif()
addPath("${srcDir}/gui")
addPath("${srcDir}/gui/buttons")
addPath("${srcDir}/gui/containers")
addPath("${srcDir}/gui/editor")
addPath("${srcDir}/gui/language")
addPath("${srcDir}/input")
addPath("${srcDir}/io")
addPath("${srcDir}/io/resource")
addPath("${srcDir}/io/zip")
addPath("${srcDir}/math")
addPath("${srcDir}/memory")
addPath("${srcDir}/messaging")
addPath("${srcDir}/module")
addPath("${srcDir}/network")
addPath("${srcDir}/persistence")
addPath("${srcDir}/persistence/rapidjson/include/rapidjson")
addPath("${srcDir}/persistence/rapidjson/include/rapidjson/internal")
addPath("${srcDir}/persistence/taml")
addPath("${srcDir}/persistence/taml/binary")
addPath("${srcDir}/persistence/taml/json")
addPath("${srcDir}/persistence/taml/xml")
addPath("${srcDir}/persistence/tinyXML")
set(BLACKLIST "platformNet_Emscripten.cpp")
addPath("${srcDir}/platform")
set(BLACKLIST "")
addPath("${srcDir}/platform/menus")
addPath("${srcDir}/platform/nativeDialogs")
addPath("${srcDir}/platform/threads")
addPath("${srcDir}/sim")
addPath("${srcDir}/spine")
addPath("${srcDir}/string")
addPath("${srcDir}/testing")
addPath("${srcDir}/testing/tests")
addPath("${srcDir}/2d")
addPath("${srcDir}/2d/assets")
addPath("${srcDir}/2d/controllers")
addPath("${srcDir}/2d/controllers/core")
addPath("${srcDir}/2d/core")
addPath("${srcDir}/2d/experimental")
addPath("${srcDir}/2d/experimental/composites")
addPath("${srcDir}/2d/gui")
addPath("${srcDir}/2d/scene")
addPath("${srcDir}/2d/sceneobject")

if(TORQUE_SFX_OPENAL)
	if(WIN32)
		option(AL_ALEXT_PROTOTYPES "Use Extended OpenAL options" ON)
		addInclude("${libDir}/openal/win32")
	endif()
	if(UNIX AND NOT APPLE)
		option(AL_ALEXT_PROTOTYPES "Use Extended OpenAL options" ON)
		#addPath("${srcDir}/sfx/linux")
	endif()
	if(APPLE)
		option(AL_ALEXT_PROTOTYPES "Use Extended OpenAL options" OFF)
		addFramework("OpenAL")
	endif()
endif()

mark_as_advanced(AL_ALEXT_PROTOTYPES)
if(AL_ALEXT_PROTOTYPES)
	addDef( "AL_ALEXT_PROTOTYPES" )
endif()

if(UNIX AND NOT APPLE)
       #set(CMAKE_SIZEOF_VOID_P 4) #force 32 bit
       set(ENV{CFLAGS} "${CXX_FLAG32} -g -O3")
       if("${TORQUE_ADDITIONAL_LINKER_FLAGS}" STREQUAL "")
         set(ENV{LDFLAGS} "${CXX_FLAG32}")
       else()
         set(ENV{LDFLAGS} "${CXX_FLAG32} ${TORQUE_ADDITIONAL_LINKER_FLAGS}")
       endif()
endif()

# Vorbis
addInclude(${libDir}/libvorbis/include)
addLib(libvorbis)
addInclude(${libDir}/libogg/include)
addLib(libogg)

###############################################################################
# platform specific things
###############################################################################

if(WIN32)
    addPath("${srcDir}/platformWin32")
    addPath("${srcDir}/platformWin32/nativeDialogs")
    addPath("${srcDir}/platformWin32/menus")
    addPath("${srcDir}/platformWin32/threads")
    # add windows rc file for the icon
    addFile("${cmakeDir}/Torque 2D.rc")
endif()

if(APPLE)
    addPath("${srcDir}/platformOSX")
endif()

if(UNIX AND NOT APPLE)
    addPath("${srcDir}/platformX86UNIX")
endif()

###############################################################################
###############################################################################
finishExecutable()
###############################################################################
###############################################################################

# Set Visual Studio startup project
if(MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TORQUE_APP_NAME})
    set_target_properties(${TORQUE_APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${TORQUE_APP_NAME})
endif()

if(NOT EXISTS "${TORQUE_APP_NAME}/Torque 2D.ico")
      CONFIGURE_FILE("${cmakeDir}/Torque 2D.ico" "${TORQUE_APP_NAME}/Torque 2D.ico" COPYONLY)
endif()
if(WIN32)
    if(NOT EXISTS "${projectSrcDir}/Torque 2D.rc")
        CONFIGURE_FILE("${cmakeDir}/Torque 2D.rc.in" "${TORQUE_APP_NAME}/Torque 2D.rc")
    endif()
	if(NOT EXISTS "${projectSrcDir}/main.cs")
        CONFIGURE_FILE("${cmakeDir}/main.cs.in" "${TORQUE_APP_NAME}/main.cs")
    endif()
    if(NOT EXISTS "${TORQUE_APP_NAME}/cleandso.bat")
        CONFIGURE_FILE("${cmakeDir}/cleandso.bat.in" "${TORQUE_APP_NAME}/cleandso.bat")
    endif()
endif()

###############################################################################
# Common Libraries
###############################################################################
addLib(lpng)
addLib(ljpeg)
addLib(zlib)

if (APPLE)
	addFramework("Cocoa")
	addFramework("OpenGL")
	addFramework("CoreData")
	addFramework("CoreFoundation")
	addFramework("Foundation")
	addFramework("AppKit")
	addFramework("AVFoundation")
	#set a few arch defaults
	set(CMAKE_OSX_ARCHITECTURES "${CMAKE_HOST_SYSTEM_PROCESSOR}" CACHE STRING "OSX Architecture" FORCE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "OSX Deployment target" FORCE)
endif()

if(WIN32)
	set(TORQUE_EXTERNAL_LIBS "COMCTL32.LIB;COMDLG32.LIB;USER32.LIB;ADVAPI32.LIB;GDI32.LIB;RPCRT4.LIB;WINMM.LIB;WS2_32.LIB;vfw32.lib;Imm32.lib;shell32.lib;shlwapi.lib;ole32.lib" CACHE STRING "external libs to link against")
	mark_as_advanced(TORQUE_EXTERNAL_LIBS)
	addLib("${TORQUE_EXTERNAL_LIBS}")
endif()

if(UNIX AND NOT APPLE)
    # copy pasted from T3D build system, some might not be needed
	set(TORQUE_EXTERNAL_LIBS "stdc++ m dl pthread rt X11 Xft SDL2main SDL2 openal" CACHE STRING "external libs to link against")
	mark_as_advanced(TORQUE_EXTERNAL_LIBS)

    string(REPLACE " " ";" TORQUE_EXTERNAL_LIBS_LIST ${TORQUE_EXTERNAL_LIBS})
    addLib( "${TORQUE_EXTERNAL_LIBS_LIST}" )
endif()


###############################################################################
# Always enabled Definitions
###############################################################################
addDef(TORQUE_DEBUG Debug)
addDef(TORQUE_DEBUG_GUARD)
addDef(TORQUE_NET_STATS)
addDef(_CRT_SECURE_NO_WARNINGS)
addDef(_CRT_SECURE_NO_DEPRECATE)
addDef(UNICODE)
addDef(_UNICODE) # for VS
addDef(TORQUE_UNICODE)
addDef(DEBUG=1)

##TORQUE_DEBUG;TORQUE_DEBUG_GUARD;_CRT_SECURE_NO_DEPRECATE;UNICODE;

if(UNIX AND NOT APPLE)
	addDef(LINUX)
endif()

###############################################################################
# Include Paths
###############################################################################
addInclude("${srcDir}/")
addInclude("${libDir}/")
addInclude("${libDir}/lpng")
addInclude("${libDir}/ljpeg")
addInclude("${libDir}/zlib")
addInclude("${libDir}/libogg/include")
addInclude("${srcDir}/persistence/rapidjson")
addInclude("${srcDir}/persistence/rapidjson/include")
addInclude("${srcDir}/testing/googleTest")
addInclude("${srcDir}/testing/googleTest/include")

if(UNIX AND NOT APPLE)
	addInclude("/usr/include/freetype2/freetype")
	addInclude("/usr/include/freetype2")
endif()

if(MSVC)
    # Match projectGenerator naming for executables
    set(OUTPUT_CONFIG DEBUG RELWITHDEBINFO)
    set(OUTPUT_SUFFIX DEBUG OPTIMIZEDDEBUG)
    foreach(INDEX RANGE 1)
        list(GET OUTPUT_CONFIG ${INDEX} CONF)
        list(GET OUTPUT_SUFFIX ${INDEX} SUFFIX)
        set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_${CONF} ${PROJECT_NAME}_${SUFFIX})
    endforeach()
endif()

