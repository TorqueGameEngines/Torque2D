# -----------------------------------------------------------------------------
# Torque2D — root CMake build
#
# This is the authoritative, modern (target-based) build for the engine. Engine
# sources are listed EXPLICITLY (see cmake/EngineSources.cmake and
# cmake/PlatformSources.cmake) rather than globbed, so that CMake — not the
# filesystem or a hand-maintained .sln — is the single source of truth. Add a
# source file there once, then regenerate the per-platform project files.
#
# Windows (VS), macOS (arm64), Linux (32/64-bit), iOS (arm64 simulator), Android,
# and Emscripten (Web/WASM) are all wired. See cmake/PlatformSources.cmake and
# cmake/BUILD-PLATFORM-NOTES.md for per-platform status and recipes.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)

# CMP0091: select the MSVC runtime via CMAKE_MSVC_RUNTIME_LIBRARY (abstraction)
# rather than editing CMAKE_CXX_FLAGS by hand.
cmake_policy(SET CMP0091 NEW)

project(Torque2D VERSION 4.0.0 LANGUAGES CXX C)

# ----------------------------------------------------------------------------
# Build configurations — mirror the historical VS solution: Debug/Release/Shipping
# ----------------------------------------------------------------------------
get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_isMultiConfig)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Shipping" CACHE STRING "" FORCE)
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;Shipping")
endif()

# Shipping is an optimized build: inherit Release's compiler/linker flags.
foreach(_flagVar
        CMAKE_C_FLAGS CMAKE_CXX_FLAGS
        CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS CMAKE_STATIC_LINKER_FLAGS)
    set(${_flagVar}_SHIPPING "${${_flagVar}_RELEASE}" CACHE STRING "" FORCE)
endforeach()

# Static, NON-debug CRT (/MT) for every config — including Debug — matching the
# maintained VS solution (all 6 configs use MultiThreaded). This is load-bearing:
# the debug CRT would define _DEBUG, which makes tinyXML (persistence/tinyXML/
# tinyxml.h) do `#define DEBUG` (empty), which in turn breaks Box2D's
# `#if DEBUG && ...` (b2Settings.h) with C1017. Must be set before any target
# (libs, gtest, engine) is created.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

# Treat wchar_t as a non-built-in type (== unsigned short), matching the VS
# solution (TreatWChar_tAsBuiltInType=false). The engine's UTF16 type is
# `unsigned short`, and the Win32 W-APIs take wchar_t*; this makes them
# compatible. Applied globally (before any target) so wchar_t has a consistent
# ABI across the engine, the third-party libs, and gtest.
if(MSVC)
    add_compile_options(/Zc:wchar_t-)
endif()

# ----------------------------------------------------------------------------
# Paths / output — the executable lands at the repo root next to main.cs so it
# can find the script + asset trees (editor/, library/, toybox/, tools/).
# ----------------------------------------------------------------------------
set(TORQUE_SRC "${CMAKE_SOURCE_DIR}/engine/source")
set(TORQUE_LIB "${CMAKE_SOURCE_DIR}/engine/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
foreach(_cfg Debug Release Shipping)
    string(TOUPPER "${_cfg}" _CFG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_CFG} "${CMAKE_SOURCE_DIR}")
endforeach()

# Emscripten: define EMSCRIPTEN=1 GLOBALLY (before any target). emcc only
# predefines __EMSCRIPTEN__, but the engine (platform/types.gcc.h) AND several
# vendored libs (e.g. ljpeg/jconfig.h's platform dispatcher) key off bare
# EMSCRIPTEN — without it ljpeg fails with "No jconfig.h was included". (The
# retired hand-maintained Emscripten recipe set this the same way, globally.)
if(EMSCRIPTEN)
    add_compile_definitions(EMSCRIPTEN=1)
endif()

# ----------------------------------------------------------------------------
# Third-party static libraries (libogg, libvorbis, lpng, ljpeg, zlib)
# ----------------------------------------------------------------------------
add_subdirectory(engine/lib)

# ----------------------------------------------------------------------------
# GoogleTest — build the vendored distribution and link gtest into the engine so
# the in-engine unit tests (runAllUnitTests) actually have a framework to run.
# Not built on Android or Emscripten (no on-device/in-browser test framework; the
# testing/* sources are also excluded from those builds below).
# ----------------------------------------------------------------------------
if(NOT ANDROID AND NOT EMSCRIPTEN)
    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)  # use the static CRT we selected
    set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    # This is the inner googletest library dir (no monorepo root above it), so the
    # version normally provided by the distribution's parent must be supplied here.
    set(GOOGLETEST_VERSION 1.17.0)
    add_subdirectory(engine/source/testing/googleTest)
endif()

# ----------------------------------------------------------------------------
# Engine target
# ----------------------------------------------------------------------------
include(cmake/EngineSources.cmake)      # -> TORQUE_ENGINE_SOURCES
include(cmake/PlatformSources.cmake)    # -> TORQUE_PLATFORM_SOURCES_{WINDOWS,MACOS,LINUX}

# iOS must be detected explicitly: APPLE is also true on iOS, so it has to be
# distinguished from the macOS desktop build (set via -DCMAKE_SYSTEM_NAME=iOS).
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(TORQUE_IOS TRUE)
endif()

# Select the active platform's OS-specific back-end sources. Note ANDROID and
# EMSCRIPTEN are also UNIX, so they must be matched before the UNIX (Linux) branch.
if(WIN32)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_WINDOWS})
elseif(ANDROID)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_ANDROID})
elseif(EMSCRIPTEN)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_EMSCRIPTEN})
elseif(TORQUE_IOS)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_IOS})
elseif(APPLE)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_MACOS})
elseif(UNIX)
    set(TORQUE_PLATFORM_SOURCES ${TORQUE_PLATFORM_SOURCES_LINUX})
else()
    message(FATAL_ERROR "Unsupported platform: no PlatformSources list for this OS.")
endif()

# The in-engine unit tests need gtest, which we don't build on Android/Emscripten.
if(ANDROID OR EMSCRIPTEN)
    list(FILTER TORQUE_ENGINE_SOURCES EXCLUDE REGEX "/testing/")
endif()

# Emscripten uses a dedicated networking back-end (platformNet_Emscripten.cpp, all
# stubs — browsers can't open raw sockets) INSTEAD of the desktop BSD-socket
# implementation. Swap it into the engine source list (the platform back-end list
# in PlatformSources.cmake deliberately omits it). platformNet_ScriptBinding.cc
# (the script API) is shared and stays.
if(EMSCRIPTEN)
    list(FILTER TORQUE_ENGINE_SOURCES EXCLUDE REGEX "platformNet(Async)?\\.cpp$")
    list(APPEND TORQUE_ENGINE_SOURCES ${TORQUE_SRC}/platform/platformNet_Emscripten.cpp)
endif()

# Android builds a shared library (libtorque2d.so) loaded by the NativeActivity;
# every other platform builds an executable. (WIN32 only affects Windows.)
if(ANDROID)
    add_library(Torque2D SHARED)
else()
    add_executable(Torque2D WIN32)
endif()
target_sources(Torque2D PRIVATE ${TORQUE_ENGINE_SOURCES} ${TORQUE_PLATFORM_SOURCES})

# C++17 across the engine (replaces the old -std=c++11 / unset MSVC default).
target_compile_features(Torque2D PRIVATE cxx_std_17)

target_include_directories(Torque2D PRIVATE
    ${TORQUE_SRC}
    ${TORQUE_LIB}
    ${TORQUE_SRC}/persistence/rapidjson
    ${TORQUE_SRC}/persistence/rapidjson/include
)
if(WIN32)
    # OpenAL headers (OpenAL itself is loaded dynamically at runtime — not linked).
    target_include_directories(Torque2D PRIVATE ${TORQUE_LIB}/openal/win32)

    # Stage the architecture-matching OpenAL Soft runtime next to the executable.
    # winOpenAL.cc loads OpenAL via LoadLibrary("OpenAL32.dll") from the app
    # directory, so the DLL must sit beside Torque2D.exe. A 64-bit process cannot
    # load a 32-bit DLL (and vice versa), so pick by the target's pointer size.
    # The staged root-level OpenAL32.dll is a build artifact (git-ignored), same
    # as Torque2D.exe. Sources live in engine/lib/openal/{win32,win64}.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(TORQUE_OPENAL_DLL "${TORQUE_LIB}/openal/win64/OpenAL32.dll")
    else()
        set(TORQUE_OPENAL_DLL "${TORQUE_LIB}/openal/win32/OpenAL32.dll")
    endif()
    add_custom_command(TARGET Torque2D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TORQUE_OPENAL_DLL}" "$<TARGET_FILE_DIR:Torque2D>/OpenAL32.dll"
        VERBATIM
        COMMENT "Staging OpenAL Soft runtime (OpenAL32.dll) next to Torque2D")
endif()
if(ANDROID)
    # platformAndroid vendors android_native_app_glue.h (included as <...> by
    # T2DActivity.h); matches the old Android.mk LOCAL_C_INCLUDES.
    target_include_directories(Torque2D PRIVATE ${TORQUE_SRC}/platformAndroid)
endif()

# Preprocessor definitions.
#  * The spurious unconditional DEBUG=1 from the old CMake is intentionally dropped
#    (the engine keys off TORQUE_DEBUG; MSVC defines _DEBUG for debug runtimes).
target_compile_definitions(Torque2D PRIVATE
    $<$<CONFIG:Debug>:TORQUE_DEBUG>
    $<$<CONFIG:Shipping>:TORQUE_SHIPPING>
)
if(ANDROID)
    # From the ndk-build recipe. __ANDROID__ is defined by the NDK toolchain.
    # HAVE_NEON applies to arm64-v8a (the only ABI we target for now).
    target_compile_definitions(Torque2D PRIVATE
        TORQUE_OS_ANDROID GL_GLEXT_PROTOTYPES ENABLE_CONSOLE_MSGS HAVE_NEON=1)
    target_compile_options(Torque2D PRIVATE -fsigned-char)
else()
    target_compile_definitions(Torque2D PRIVATE
        UNICODE _UNICODE TORQUE_UNICODE TORQUE_DEBUG_GUARD TORQUE_NET_STATS)
endif()
if(MSVC)
    # _HAS_STD_BYTE=0 is REQUIRED under C++17/MSVC (std::byte vs the engine's byte).
    target_compile_definitions(Torque2D PRIVATE
        _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE _HAS_STD_BYTE=0)
endif()
if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    target_compile_definitions(Torque2D PRIVATE LINUX)
endif()

if(MSVC)
    target_compile_options(Torque2D PRIVATE
        /MP                                   # parallel compilation
        /EHsc                                 # standard C++ exception model
        /bigobj                               # large TUs (compiledEval.cc etc.)
        /wd4800 /wd4100 /wd4127 /wd4512        # noisy warnings (matches old build)
    )
else()
    # The engine uses the (C++17-removed) `register` keyword in ~35 files. GCC
    # tolerates it; Clang (Android NDK, and macOS/iOS) makes -Wregister an error.
    target_compile_options(Torque2D PRIVATE -Wno-register)
endif()

# Link: third-party libs built from source (transitively bring their includes).
target_link_libraries(Torque2D PRIVATE libogg libvorbis lpng ljpeg)
if(NOT ANDROID)
    # zlib is built from source everywhere except Android (which uses the NDK's
    # system libz). Emscripten compiles the vendored zlib to wasm just fine.
    target_link_libraries(Torque2D PRIVATE zlib)
endif()
if(NOT ANDROID AND NOT EMSCRIPTEN)
    # gtest provides the in-engine unit-test framework (desktop only).
    target_link_libraries(Torque2D PRIVATE gtest)
endif()

if(WIN32)
    # Windows system libraries (matches the VS solution's AdditionalDependencies).
    # OpenGL is linked via #pragma comment in the platform sources, as before.
    target_link_libraries(Torque2D PRIVATE
        comctl32 comdlg32 user32 advapi32 gdi32 rpcrt4 winmm
        ws2_32 vfw32 imm32 shell32 shlwapi ole32
    )
endif()

if(APPLE AND NOT TORQUE_IOS)
    # Apple Silicon (arm64) build. The legacy Xcode project hard-coded x86_64;
    # we target arm64 natively. Set explicitly so the architecture is reproducible
    # regardless of the host (and overridable via -DCMAKE_OSX_ARCHITECTURES for a
    # universal/Intel build).
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS build architecture" FORCE)
    endif()

    # Minimum macOS. Without this the binary inherits the build host's SDK default
    # (e.g. 14.6), needlessly excluding older Macs. 11.0 (Big Sur) is the floor for
    # arm64 and is the lowest that still lets users on older systems run the game.
    # It does NOT constrain a future Metal renderer: Metal/MetalKit have shipped
    # since 10.11, so 11.0 fully supports Metal (only the Metal 3 feature set would
    # need 13.0+, which a 2D engine won't require). Overridable on the command line.
    if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)
    endif()

    # Build a proper .app bundle, NOT a bare command-line tool. Without this CMake
    # emits a `com.apple.product-type.tool` target; when Xcode runs a tool that then
    # turns itself into a GUI app (NSApplication / setActivationPolicy:Regular),
    # macOS relaunches it to grant a GUI session — and relaunching a non-bundle
    # executable goes through LaunchServices -> Terminal, which spawned EXTRA copies
    # of the app (the mysterious "3 windows" only under Xcode). A real .app has a
    # stable LaunchServices identity and is launched exactly once.
    #
    # The bundle lands at the repo root (CMAKE_RUNTIME_OUTPUT_DIRECTORY) next to
    # main.cs. The engine's getExecutablePath() (osxFileIO.mm) searches inside the
    # bundle (Contents/Resources) and then the bundle's PARENT dir for main.cs, so
    # the script/asset trees resolve with no extra packaging — same as the legacy
    # .app build.
    set_target_properties(Torque2D PROPERTIES
        MACOSX_BUNDLE                      TRUE
        MACOSX_BUNDLE_BUNDLE_NAME          "Torque2D"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "org.torque2d.Torque2D"
        MACOSX_BUNDLE_BUNDLE_VERSION       "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")

    # Ad-hoc code signing ("Sign to Run Locally"). On Apple Silicon a .app must be
    # signed to run, but we don't want to require an Apple developer team/identity
    # for a local build. "-" is the ad-hoc identity; Manual style stops Xcode's
    # automatic signing from trying (and failing) to use a real team. Override these
    # for a distributable, notarized build.
    set_target_properties(Torque2D PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY  "-"
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE     "Manual"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")

    # The platformOSX .mm back-end uses AppKit/Foundation types at file scope and
    # relied on the legacy Xcode prefix header to import Cocoa. Reproduce that by
    # force-including a CMake-owned prefix header (no-op for C/C++ TUs via __OBJC__).
    target_compile_options(Torque2D PRIVATE
        -include "${CMAKE_SOURCE_DIR}/tools/CMake/macOS-Prefix.h")

    # macOS desktop frameworks (from the historical build recipe). The on-platform
    # (Mac) session should confirm this links and the app starts; OpenAL is a
    # framework here (not dynamically loaded as on Windows).
    target_link_libraries(Torque2D PRIVATE
        "-framework Cocoa" "-framework OpenGL" "-framework CoreData"
        "-framework CoreFoundation" "-framework Foundation" "-framework AppKit"
        "-framework AVFoundation" "-framework OpenAL")
endif()

if(TORQUE_IOS)
    # iOS frameworks, derived from the maintained engine/compilers/Xcode_iOS
    # project. Configure with the Xcode generator and -DCMAKE_SYSTEM_NAME=iOS.
    # iOS uses OpenGL ES (not desktop OpenGL).

    # Simulator vs. device. The simulator (-DCMAKE_OSX_SYSROOT=iphonesimulator)
    # needs NO code-signing — the easiest first target. A physical device needs a
    # real signing identity + provisioning profile, which means automatic signing
    # and an Apple Development Team. Detect from the SDK: anything not a *simulator*
    # SDK (incl. the default empty -> iphoneos) is a device build.
    if(CMAKE_OSX_SYSROOT MATCHES "[Ss]imulator")
        set(TORQUE_IOS_SIMULATOR TRUE)
    else()
        set(TORQUE_IOS_SIMULATOR FALSE)
    endif()

    # Overridable identity. Bundle id should be unique to YOUR Apple account for
    # device provisioning (default uses the project's reverse-domain; override with
    # e.g. -DTORQUE_IOS_BUNDLE_ID=com.yourcompany.torque2d). TORQUE_IOS_TEAM is your
    # 10-char Apple Development Team ID — leave empty to pick the team in Xcode's
    # Signing & Capabilities tab instead.
    set(TORQUE_IOS_BUNDLE_ID "org.torque2d.Torque2D" CACHE STRING "iOS app bundle identifier")
    set(TORQUE_IOS_TEAM "" CACHE STRING "Apple Development Team ID for iOS device code-signing")

    set_target_properties(Torque2D PROPERTIES
        MACOSX_BUNDLE TRUE
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")

    if(TORQUE_IOS_SIMULATOR)
        # Simulator: skip signing entirely.
        set_target_properties(Torque2D PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
    else()
        # Device: automatic signing. Xcode creates/uses a development provisioning
        # profile for TORQUE_IOS_BUNDLE_ID under the selected team. If TORQUE_IOS_TEAM
        # is empty, Xcode shows "Signing requires a development team" until you pick
        # one in the target's Signing & Capabilities tab (a free Apple ID works).
        set_target_properties(Torque2D PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic")
        if(TORQUE_IOS_TEAM)
            set_target_properties(Torque2D PROPERTIES XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${TORQUE_IOS_TEAM}")
        endif()
    endif()

    # The iOS app's window is NOT created programmatically — T2DAppDelegate relies
    # on a Main storyboard (UIMainStoryboardFile) to instantiate T2DViewController
    # (a GLKViewController) hosting T2DView (the GLKView the engine renders into).
    # CMake's auto-generated Info.plist has none of the iOS keys (no bundle id, no
    # storyboard, no orientations), so the bare bundle launches the delegate but
    # never gets a window -> black screen. Supply a real iOS Info.plist template
    # and fill it from these MACOSX_BUNDLE_* properties (CFBundleExecutable is
    # filled per-config, so Debug -> Torque2D_DEBUG / Release -> Torque2D).
    set_target_properties(Torque2D PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST           "${CMAKE_SOURCE_DIR}/tools/CMake/iOS-Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME          "Torque2D"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "${TORQUE_IOS_BUNDLE_ID}"
        MACOSX_BUNDLE_BUNDLE_VERSION       "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        # Xcode prefers these build settings over the matching Info.plist keys and
        # warns on a mismatch. Set them so the bundle id used at install time is
        # ours and the device family (1=iPhone, 2=iPad) is authoritative. (The
        # plist's CFBundleIdentifier is kept as a literal for non-Xcode tooling;
        # UIDeviceFamily is intentionally omitted from the plist in favor of this.)
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${TORQUE_IOS_BUNDLE_ID}"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY    "1,2")

    # Bundle the storyboards as compiled resources (Xcode runs ibtool on them).
    # iPhone/iPad: the GLKit UI referenced by UIMainStoryboardFile. LaunchScreen:
    # a plain static screen referenced by UILaunchStoryboardName (modern iOS needs
    # one for a full-screen drawable; it may not contain custom classes, so it's
    # separate from the GL storyboards). The two GL storyboards were copied from
    # engine/compilers/Xcode_iOS into tools/CMake so the CMake build is self-
    # contained (independent of the legacy project tree).
    set(TORQUE_IOS_STORYBOARDS
        ${CMAKE_SOURCE_DIR}/tools/CMake/iPhoneStoryboard.storyboard
        ${CMAKE_SOURCE_DIR}/tools/CMake/iPadStoryboard.storyboard
        ${CMAKE_SOURCE_DIR}/tools/CMake/LaunchScreen.storyboard)
    target_sources(Torque2D PRIVATE ${TORQUE_IOS_STORYBOARDS})
    set_source_files_properties(${TORQUE_IOS_STORYBOARDS} PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)

    # The engine's OS detection (platform/types.gcc.h) gates the iOS branch on
    # TORQUE_OS_IOS but only DEFINES it inside that branch — so the build system
    # must predefine it (the legacy Xcode_iOS project did). Without it, __APPLE__
    # makes the engine select the macOS/desktop-GL back-end and fail.
    target_compile_definitions(Torque2D PRIVATE TORQUE_OS_IOS)

    # The debug-only "outline GL" feature (iOSOutlineGL.h) does
    # `#define glDrawArrays glDrawArraysProcPtr` to route draws through a swappable
    # function pointer. On a modern iOS SDK the prefix header drags GLES gl.h in a
    # second time (UIKit -> CoreImage), and the macro rewrites the SDK's
    # glDrawArrays *function* declaration into glDrawArraysProcPtr, colliding with
    # the engine's same-named *variable*. NO_REDEFINE_GL_FUNCS disables the macro
    # (the engine's own escape hatch; each platform's *OutlineGL source already
    # defines it locally) — outline/wireframe debug draw becomes a no-op on iOS.
    target_compile_definitions(Torque2D PRIVATE NO_REDEFINE_GL_FUNCS)

    # graphics/bitmapPvr.cc (PVR textures) is required on iOS but excluded from
    # desktop builds in EngineSources.cmake — add it back here.
    target_sources(Torque2D PRIVATE ${TORQUE_SRC}/graphics/bitmapPvr.cc)

    # The platformiOS .mm back-end uses UIKit/Foundation types at file scope and
    # relied on the legacy Xcode prefix header. Force-include the CMake-owned one
    # (no-op for C/C++ TUs via __OBJC__).
    target_compile_options(Torque2D PRIVATE
        -include "${CMAKE_SOURCE_DIR}/tools/CMake/iOS-Prefix.h")

    target_link_libraries(Torque2D PRIVATE
        "-framework UIKit" "-framework OpenGLES" "-framework GLKit"
        "-framework QuartzCore" "-framework CoreGraphics" "-framework CoreText"
        "-framework CoreMotion" "-framework CoreMedia" "-framework CoreVideo"
        "-framework CoreAudio" "-framework CoreImage" "-framework AudioToolbox"
        "-framework AVFoundation" "-framework MediaPlayer" "-framework Foundation"
        "-framework CoreFoundation" "-framework OpenAL"
        "-framework GameKit")  # platformiOS/GameCenter.mm

    # Bundle the script/asset trees INTO the .app. Unlike the desktop build (which
    # runs from the repo root and finds main.cs + the trees via the cwd), an
    # installed iOS app is sandboxed: getExecutablePath() (iOSFileio.mm) resolves
    # to the bundle, so the content must live inside it. The engine boots the
    # editor via main.cs (exec "./editor/main.cs"), which needs the library/ and
    # toybox/ trees too. iOS bundles are FLAT, so everything goes to the bundle
    # ROOT ($<TARGET_BUNDLE_DIR>), matching the legacy Xcode_iOS folder references.
    # (POST_BUILD so it lands in whatever build dir Xcode produces, before install;
    # copy_directory re-copies each build — acceptable for a dev/smoke-test bundle.)
    set(TORQUE_IOS_CONTENT_DIRS editor library toybox tools)
    add_custom_command(TARGET Torque2D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/main.cs" "$<TARGET_BUNDLE_DIR:Torque2D>/main.cs"
        COMMENT "Bundling Torque2D script/asset trees into the iOS .app")
    foreach(_dir ${TORQUE_IOS_CONTENT_DIRS})
        add_custom_command(TARGET Torque2D POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/${_dir}" "$<TARGET_BUNDLE_DIR:Torque2D>/${_dir}")
    endforeach()
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    # Linux desktop build: platformX86UNIX on SDL 2, with platformSDL -- the code
    # it shares with the other back-ends on SDL 2. NOTES for the on-platform
    # session:
    #   * SDL 2 is vendored (engine/lib/sdl, the same 2.32.10 Torque3D builds) and
    #     built by engine/lib/CMakeLists.txt as a static library, so there is no
    #     SDL to install and the engine still ships as one executable. SDL's CMake
    #     needs the X11 development headers to build its X11 driver, which is the
    #     driver the back-end runs on (InitSDL in x86UNIXWindow.cc).
    #   * `rt` is a no-op on modern glibc but harmless.
    #   * OpenGL/FreeType are resolved via find_package (more robust than the old
    #     hard-coded /usr/include/freetype2 paths). Install dev packages, e.g. on
    #     Debian/Ubuntu: libx11-dev libxext-dev libxrandr-dev libxcursor-dev
    #     libxfixes-dev libxi-dev libxss-dev libxft-dev libfontconfig1-dev
    #     libfreetype6-dev libopenal-dev libgl1-mesa-dev (and nasm for the 32-bit
    #     build).
    #   * X11 is linked directly as well as through SDL: the fonts (x86UNIXFont.cc)
    #     use Xft on a display connection of their own, and the back-end reads the
    #     window manager's _NET_WM_STATE through SDL's (x86UNIXOGLVideo.cc).
    #   * fontconfig is linked explicitly, not left to Xft: x86UNIXFont.cc calls
    #     Fc* directly for PlatformFont::enumeratePlatformFonts.
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(Freetype REQUIRED)

    target_link_libraries(Torque2D PRIVATE
        Threads::Threads
        OpenGL::GL
        Freetype::Freetype
        m dl rt
        X11 Xft fontconfig
        SDL2::SDL2-static
        openal)

    # TORQUE_SDL: the platform layer is built on SDL 2 and compiles
    # engine/source/platformSDL -- Torque3D's name for the same switch. The unit
    # tests of that shared code are compiled only where it is.
    target_compile_definitions(Torque2D PRIVATE TORQUE_SDL)

    # CPU / bitness. detectX86CPUInfo() (platform/platformCPUInfo.asm — 32-bit
    # NASM, it does NOT assemble for elf64) is referenced only when TORQUE_64 is
    # undefined (engine/source/platformX86UNIX/x86UNIXCPUInfo.cc).
    #   * 64-bit: define TORQUE_64 so the asm is never referenced (and never built).
    #   * 32-bit (-m32): assemble the asm with NASM (elf32) and define `i386` —
    #     bare `i386` isn't predefined under standard C++, and the engine's CPU
    #     detection (types.gcc.h) keys off it. LINUX is already a target define,
    #     so NASM picks it up too (its export macro is `%ifdef LINUX`).
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_compile_definitions(Torque2D PRIVATE TORQUE_64)
    else()
        set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
        enable_language(ASM_NASM)
        target_sources(Torque2D PRIVATE ${TORQUE_SRC}/platform/platformCPUInfo.asm)
        target_compile_definitions(Torque2D PRIVATE i386)
    endif()
endif()

if(ANDROID)
    # Prebuilt third-party libs (arm64-v8a only — the other ABIs have no prebuilts).
    # FreeType is an ancient (2.4.12) static .a; OpenAL is a shared .so that must
    # also be packaged into the APK (see the Gradle jniLibs config).
    add_library(android_freetype STATIC IMPORTED)
    set_target_properties(android_freetype PROPERTIES
        IMPORTED_LOCATION "${TORQUE_LIB}/freetype/android/lib/${ANDROID_ABI}/libfreetype.a"
        INTERFACE_INCLUDE_DIRECTORIES "${TORQUE_LIB}/freetype/android/include;${TORQUE_LIB}/freetype/android/include/freetype2")

    add_library(android_openal SHARED IMPORTED)
    set_target_properties(android_openal PROPERTIES
        IMPORTED_LOCATION "${TORQUE_LIB}/openal/Android/${ANDROID_ABI}/libopenal.so"
        INTERFACE_INCLUDE_DIRECTORIES "${TORQUE_LIB}/openal/Android/openal-soft-master/jni/OpenAL/include")

    # Android system libraries (z = NDK libz; GLESv1_CM matches the engine GL path).
    target_link_libraries(Torque2D PRIVATE
        android_freetype android_openal
        log android EGL GLESv1_CM OpenSLES z)
endif()

if(EMSCRIPTEN)
    # WebAssembly build (configure via `emcmake cmake`, which points CMake at the
    # Emscripten toolchain and sets EMSCRIPTEN=1). See cmake/BUILD-PLATFORM-NOTES.md.

    # (EMSCRIPTEN=1 is defined GLOBALLY above, before the third-party libs — the
    # engine's types.gcc.h and some vendored libs both key off bare EMSCRIPTEN.
    # That's what makes the engine select TORQUE_OS_EMSCRIPTEN / platformEmscripten.)

    # SDL 2 is Emscripten's own port -- 2.32.10, the version the Linux build
    # vendors -- which emcc fetches and builds the first time it is asked for.
    # It makes the canvas and its WebGL context, and delivers every key,
    # character, mouse and wheel event. -sUSE_SDL=2 is needed when compiling
    # as well as when linking: it puts the port's headers on the include path,
    # so that <SDL.h> is found -- the form platformSDL and the Linux back-end
    # include. TORQUE_SDL: the platform layer is built on SDL 2 and compiles
    # engine/source/platformSDL, as on Linux.
    target_compile_options(Torque2D PRIVATE "-sUSE_SDL=2")
    target_compile_definitions(Torque2D PRIVATE TORQUE_SDL)

    # FreeType (built from source in engine/lib for the web build only) — the
    # rasterizer EmscriptenFont uses to synthesize glyphs from a bundled .ttf when a
    # font isn't in the .uft cache. Its PUBLIC include dir propagates here, so
    # EmscriptenFont.cpp sees <ft2build.h> / FT_FREETYPE_H.
    target_link_libraries(Torque2D PRIVATE freetype)

    # Keep the web bundle (.html/.js/.wasm/.data) in the build tree rather than the
    # repo root — unlike a desktop exe it isn't run from the cwd (assets come from
    # the MEMFS preload below), so there's no reason to drop four files at the root.
    set_target_properties(Torque2D PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY          "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG    "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_SHIPPING "${CMAKE_BINARY_DIR}")

    # Script/asset trees packaged into the MEMFS virtual filesystem by the emcc file
    # packager (--preload-file SRC@DST -> a .data sidecar). The engine runs from cwd
    # "/" in the browser, so main.cs lands at /main.cs and the trees under /editor,
    # /library, /toybox. Everything a web build can touch must be preloaded (there's
    # no host FS to lazy-load from), so the runtime content trees are all bundled.
    # NOTE: tools/ is deliberately EXCLUDED — unlike the iOS bundle it is not shipped,
    # because it is build-time-only tooling (TexturePacker, and ~90 MB of generated
    # doxygen HTML under tools/doxygen/output) that the engine never loads at runtime.
    # (SHELL: keeps each option+path pair together and unde-duplicated by CMake.)
    set(_emPreload
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/main.cs@/main.cs"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/editor@/editor"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/library@/library"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/toybox@/toybox"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/PlanetX@/PlanetX")

    target_link_options(Torque2D PRIVATE
        "-sUSE_SDL=2"
        # Fixed-function GL over WebGL 1. It needs the context made by SDL (see
        # CreateGLWindow in EmscriptenOGLVideo.cpp), and cannot be combined with
        # FULL_ES2/FULL_ES3, which is why the build stays on WebGL 1.
        "-sLEGACY_GL_EMULATION=1"
        # Fixed 512 MB heap: with ALLOW_MEMORY_GROWTH the WASM heap ArrayBuffer is
        # resizable, and Chrome's texImage2D rejects typed-array views into a
        # resizable buffer (seen with emscripten 6.x + LEGACY_GL_EMULATION).
        "-sINITIAL_MEMORY=536870912"
        "-sALLOW_MEMORY_GROWTH=0"
        "-sEXIT_RUNTIME=0"
        "-sFORCE_FILESYSTEM=1"
        "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
        "SHELL:--js-library ${TORQUE_SRC}/platformEmscripten/platform.js"
        ${_emPreload})

    # emcc picks the output format from the target suffix: .html emits the HTML
    # shell + .js + .wasm (+ .data from the preload). -> Torque2D_DEBUG.html etc.
    set_target_properties(Torque2D PROPERTIES SUFFIX ".html")
endif()

# Output name. On Android the NativeActivity loads "torque2d" -> libtorque2d.so.
# On desktop, match the VS solution (Debug -> Torque2D_DEBUG.exe).
if(ANDROID)
    set_target_properties(Torque2D PROPERTIES OUTPUT_NAME "torque2d")
else()
    set_target_properties(Torque2D PROPERTIES
        OUTPUT_NAME        "Torque2D"
        OUTPUT_NAME_DEBUG  "Torque2D_DEBUG"
    )
endif()

# ----------------------------------------------------------------------------
# Windows application resource (icon + version info) from the .rc template.
# Assembled in the build tree so resource.h and the .ico resolve next to the rc.
# ----------------------------------------------------------------------------
if(WIN32)
    set(_resDir "${CMAKE_BINARY_DIR}/resources")
    configure_file("${CMAKE_SOURCE_DIR}/tools/CMake/Torque 2D.rc.in" "${_resDir}/Torque2D.rc" COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/tools/CMake/resource.h"      "${_resDir}/resource.h"   COPYONLY)
    configure_file("${CMAKE_SOURCE_DIR}/tools/CMake/Torque 2D.ico"   "${_resDir}/Torque 2D.ico" COPYONLY)
    target_sources(Torque2D PRIVATE "${_resDir}/Torque2D.rc")
endif()

# ----------------------------------------------------------------------------
# IDE niceties (Visual Studio + Xcode).
# ----------------------------------------------------------------------------
source_group(TREE "${TORQUE_SRC}" PREFIX "engine"
    FILES ${TORQUE_ENGINE_SOURCES} ${TORQUE_PLATFORM_SOURCES})

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Torque2D)
set_target_properties(Torque2D PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# On the macOS desktop build, make Xcode's Run/Debug scheme launch the app with
# the repo root as its working directory — the exe loads main.cs and the script/
# asset trees relative to the cwd, so without this "Run" from Xcode starts in the
# wrong directory and the engine can't find main.cs. (iOS runs from a bundle in
# the simulator/device, so this doesn't apply there.)
if(APPLE AND NOT TORQUE_IOS)
    set_target_properties(Torque2D PROPERTIES
        XCODE_GENERATE_SCHEME        TRUE
        XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
