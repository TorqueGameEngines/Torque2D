# -----------------------------------------------------------------------------
# PlatformSources.cmake
#
# Per-platform translation units. The root CMakeLists selects the active
# platform's list and adds it to the Torque2D target. Windows, macOS, Linux, iOS,
# Android and Emscripten (Web/WASM) are all populated.
#
# The cross-platform engine sources live in EngineSources.cmake. The generic
# `platform/` abstraction (compiled on every platform) is part of that list;
# only the OS-specific back-ends live here.
# -----------------------------------------------------------------------------

# === Windows (platformWin32) =================================================
set(TORQUE_PLATFORM_SOURCES_WINDOWS
    # ---- platformWin32 ----
    ${TORQUE_SRC}/platformWin32/cardProfile.cpp
    ${TORQUE_SRC}/platformWin32/winAsmBlit.cc
    ${TORQUE_SRC}/platformWin32/winCPUInfo.cc
    ${TORQUE_SRC}/platformWin32/winConsole.cc
    ${TORQUE_SRC}/platformWin32/winDInputDevice.cc
    ${TORQUE_SRC}/platformWin32/winDirectInput.cc
    ${TORQUE_SRC}/platformWin32/winExec.cc
    ${TORQUE_SRC}/platformWin32/winFileio.cc
    ${TORQUE_SRC}/platformWin32/winFont.cc
    ${TORQUE_SRC}/platformWin32/winGL.cc
    ${TORQUE_SRC}/platformWin32/winGLSpecial.cc
    ${TORQUE_SRC}/platformWin32/winInput.cc
    ${TORQUE_SRC}/platformWin32/winMath.cc
    ${TORQUE_SRC}/platformWin32/winMath_ASM.cc
    ${TORQUE_SRC}/platformWin32/winMemory.cc
    ${TORQUE_SRC}/platformWin32/winOGLVideo.cc
    ${TORQUE_SRC}/platformWin32/winOpenAL.cc
    ${TORQUE_SRC}/platformWin32/winProcessControl.cc
    ${TORQUE_SRC}/platformWin32/winSemaphore.cc
    ${TORQUE_SRC}/platformWin32/winStrings.cc
    ${TORQUE_SRC}/platformWin32/winTLS.cc
    ${TORQUE_SRC}/platformWin32/winTime.cc
    ${TORQUE_SRC}/platformWin32/winUser.cc
    ${TORQUE_SRC}/platformWin32/winVFS.cc
    ${TORQUE_SRC}/platformWin32/winVideo.cc
    ${TORQUE_SRC}/platformWin32/winWindow.cc
    # ---- platformWin32/menus ----
    ${TORQUE_SRC}/platformWin32/menus/popupMenuWin32.cc
    # ---- platformWin32/nativeDialogs ----
    ${TORQUE_SRC}/platformWin32/nativeDialogs/win32DirectoryResolver.cpp
    ${TORQUE_SRC}/platformWin32/nativeDialogs/win32FileDialog.cc
    ${TORQUE_SRC}/platformWin32/nativeDialogs/win32MsgBox.cpp
    # ---- platformWin32/threads ----
    ${TORQUE_SRC}/platformWin32/threads/mutex.cc
    ${TORQUE_SRC}/platformWin32/threads/thread.cc
)

# === macOS (platformOSX) =====================================================
# Objective-C++ (.mm) back-end. Builds & links on Apple Silicon (arm64) with both
# the Makefiles and Xcode generators; the root CMakeLists force-includes
# tools/CMake/macOS-Prefix.h (Cocoa) and pins
# CMAKE_OSX_ARCHITECTURES=arm64. The exe builds as a plain binary that runs from
# the repo root (so it finds main.cs) — no MACOSX_BUNDLE on desktop.
set(TORQUE_PLATFORM_SOURCES_MACOS
    # ---- platformOSX ----
    ${TORQUE_SRC}/platformOSX/AppDelegate.mm
    ${TORQUE_SRC}/platformOSX/main.mm
    ${TORQUE_SRC}/platformOSX/osxAudio.mm
    ${TORQUE_SRC}/platformOSX/osxCPU.mm
    ${TORQUE_SRC}/platformOSX/osxCocoaUtilities.mm
    ${TORQUE_SRC}/platformOSX/osxEvents.mm
    ${TORQUE_SRC}/platformOSX/osxFileDialogs.mm
    ${TORQUE_SRC}/platformOSX/osxFileIO.mm
    ${TORQUE_SRC}/platformOSX/osxFont.mm
    ${TORQUE_SRC}/platformOSX/osxGL.mm
    ${TORQUE_SRC}/platformOSX/osxInput.mm
    ${TORQUE_SRC}/platformOSX/osxInputManager.mm
    ${TORQUE_SRC}/platformOSX/osxMath.mm
    ${TORQUE_SRC}/platformOSX/osxMemory.mm
    ${TORQUE_SRC}/platformOSX/osxMutex.mm
    ${TORQUE_SRC}/platformOSX/osxOpenGLDevice.mm
    ${TORQUE_SRC}/platformOSX/osxOutlineGL.cc
    ${TORQUE_SRC}/platformOSX/osxPopupMenu.mm
    ${TORQUE_SRC}/platformOSX/osxSemaphore.mm
    ${TORQUE_SRC}/platformOSX/osxString.mm
    ${TORQUE_SRC}/platformOSX/osxThread.mm
    ${TORQUE_SRC}/platformOSX/osxTime.mm
    ${TORQUE_SRC}/platformOSX/osxTorqueView.mm
    ${TORQUE_SRC}/platformOSX/osxVideo.mm
    ${TORQUE_SRC}/platformOSX/osxWindow.mm
    ${TORQUE_SRC}/platformOSX/platformOSX.mm
)

# === Linux (platformX86UNIX + platformSDL, on SDL 2's X11 driver) =============
set(TORQUE_PLATFORM_SOURCES_LINUX
    # ---- platformX86UNIX ----
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXAsmBlit.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXCPUInfo.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXConsole.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXDedicatedStub.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXDialogs.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXFileio.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXFont.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXGL.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXIO.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXInput.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXInputManager.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXMath.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXMath_ASM.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXMemory.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXMutex.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXOGLVideo.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXOpenAL.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXPopupMenu.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXProcessControl.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXSemaphore.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXStrings.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXThread.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXTime.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXUtils.cc
    ${TORQUE_SRC}/platformX86UNIX/x86UNIXWindow.cc
    # ---- platformSDL: shared by every back-end on SDL 2 (the web build next) ----
    ${TORQUE_SRC}/platformSDL/sdlInput.cpp
    ${TORQUE_SRC}/platformSDL/sdlMsgBox.cpp
    ${TORQUE_SRC}/platformSDL/sdlTextInput.cpp
)

# === iOS (platformiOS) =======================================================
# UIKit/OpenGL-ES back-end. SEPARATE from macOS (distinct sources + frameworks).
# Was never supported by the old CMake; recipe derived from the Xcode_iOS project.
# Builds & links for the arm64 simulator (iOS 18.2 SDK) -> Torque2D_DEBUG.app.
# Requires full Xcode + `-DCMAKE_SYSTEM_NAME=iOS`; the root CMakeLists' TORQUE_IOS
# block adds bitmapPvr.cc, defines TORQUE_OS_IOS + NO_REDEFINE_GL_FUNCS, and
# force-includes tools/CMake/iOS-Prefix.h. See cmake/BUILD-PLATFORM-NOTES.md.
set(TORQUE_PLATFORM_SOURCES_IOS
    # ---- platformiOS ----
    ${TORQUE_SRC}/platformiOS/GameCenter.mm
    ${TORQUE_SRC}/platformiOS/SoundEngine.mm
    ${TORQUE_SRC}/platformiOS/T2DAppDelegate.mm
    ${TORQUE_SRC}/platformiOS/T2DView.mm
    ${TORQUE_SRC}/platformiOS/T2DViewController.mm
    ${TORQUE_SRC}/platformiOS/iOSAlerts.mm
    ${TORQUE_SRC}/platformiOS/iOSAudio.mm
    ${TORQUE_SRC}/platformiOS/iOSCPUInfo.mm
    ${TORQUE_SRC}/platformiOS/iOSConsole.mm
    ${TORQUE_SRC}/platformiOS/iOSDialogs.mm
    ${TORQUE_SRC}/platformiOS/iOSEvents.mm
    ${TORQUE_SRC}/platformiOS/iOSFileio.mm
    ${TORQUE_SRC}/platformiOS/iOSFont.mm
    ${TORQUE_SRC}/platformiOS/iOSGL.mm
    ${TORQUE_SRC}/platformiOS/iOSGL2ES.mm
    ${TORQUE_SRC}/platformiOS/iOSInput.mm
    ${TORQUE_SRC}/platformiOS/iOSMath.mm
    ${TORQUE_SRC}/platformiOS/iOSMemory.mm
    ${TORQUE_SRC}/platformiOS/iOSMotionManager.mm
    ${TORQUE_SRC}/platformiOS/iOSMoviePlayback.mm
    ${TORQUE_SRC}/platformiOS/iOSMutex.mm
    ${TORQUE_SRC}/platformiOS/iOSOGLVideo.mm
    ${TORQUE_SRC}/platformiOS/iOSOutlineGL.mm
    ${TORQUE_SRC}/platformiOS/iOSPlatform.mm
    ${TORQUE_SRC}/platformiOS/iOSProcessControl.mm
    ${TORQUE_SRC}/platformiOS/iOSProfiler.mm
    ${TORQUE_SRC}/platformiOS/iOSSemaphore.mm
    ${TORQUE_SRC}/platformiOS/iOSStreamSource.cc
    ${TORQUE_SRC}/platformiOS/iOSStrings.mm
    ${TORQUE_SRC}/platformiOS/iOSThread.mm
    ${TORQUE_SRC}/platformiOS/iOSTime.mm
    ${TORQUE_SRC}/platformiOS/iOSUserMusicLibrary.mm
    ${TORQUE_SRC}/platformiOS/iOSUtil.mm
    ${TORQUE_SRC}/platformiOS/iOSWindow.mm
    ${TORQUE_SRC}/platformiOS/main.mm
    # ---- platformiOS/menus ----
    ${TORQUE_SRC}/platformiOS/menus/popupMenu.mm
)

# === Android (platformAndroid) ===============================================
# NativeActivity / OpenGL-ES back-end built into libtorque2d.so by the NDK.
# Recipe derived from the (now-deleted) ndk-build Android.mk; arm64-v8a target.
set(TORQUE_PLATFORM_SOURCES_ANDROID
    # ---- platformAndroid ----
    ${TORQUE_SRC}/platformAndroid/AndroidAlerts.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidAudio.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidCPUInfo.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidConsole.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidDialogs.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidEvents.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidFileio.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidFont.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidGL.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidGL2ES.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidInput.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidMath.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidMemory.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidMutex.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidOGLVideo.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidOutlineGL.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidPlatform.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidProcessControl.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidProfiler.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidSemaphore.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidStreamSource.cc
    ${TORQUE_SRC}/platformAndroid/AndroidStrings.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidThread.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidTime.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidUtil.cpp
    ${TORQUE_SRC}/platformAndroid/AndroidWindow.cpp
    ${TORQUE_SRC}/platformAndroid/T2DActivity.cpp
    ${TORQUE_SRC}/platformAndroid/android_native_app_glue.c
    ${TORQUE_SRC}/platformAndroid/main.cpp
    # ---- platformAndroid/menus ----
    ${TORQUE_SRC}/platformAndroid/menus/popupMenu.cpp
)

# === Emscripten / Web (platformEmscripten) ==================================
# WebAssembly back-end built by emcc (configure via `emcmake cmake`). The browser
# owns the event loop, so main.cpp drives the engine through
# emscripten_set_main_loop(_EmscriptenGameInnerLoop, ...) -> Game->mainLoop() once
# per animation frame (same callback model as iOS/Android). GL is GLES via the
# EmscriptenGL2ES fixed-function shim over WebGL. The root CMakeLists' EMSCRIPTEN
# block defines EMSCRIPTEN=1 (the engine's types.gcc.h keys TORQUE_OS_EMSCRIPTEN
# off it), swaps in platformNet_Emscripten.cpp, and sets the emcc link flags.
# Networking back-end (platformNet_Emscripten.cpp) is swapped in from the engine
# list in the root CMakeLists, not listed here. See cmake/BUILD-PLATFORM-NOTES.md.
set(TORQUE_PLATFORM_SOURCES_EMSCRIPTEN
    # ---- platformEmscripten ----
    ${TORQUE_SRC}/platformEmscripten/EmscriptenAlerts.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenAudio.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenConsole.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenCPUInfo.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenDialogs.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenEvents.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenFileio.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenFont.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenGL.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenGL2ES.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenInput.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenInputManager.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenMath.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenMemory.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenMutex.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenOGLVideo.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenOutlineGL.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenPlatform.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenProcessControl.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenSemaphore.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenStrings.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenThread.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenTime.cpp
    ${TORQUE_SRC}/platformEmscripten/EmscriptenWindow.cpp
    ${TORQUE_SRC}/platformEmscripten/main.cpp
    # ---- platformEmscripten/menus ----
    ${TORQUE_SRC}/platformEmscripten/menus/popupMenu.cpp
)
