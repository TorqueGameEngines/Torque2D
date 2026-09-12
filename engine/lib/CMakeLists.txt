# -----------------------------------------------------------------------------
# engine/lib/CMakeLists.txt
#
# Vendored third-party libraries, each built as a STATIC library target with
# modern, target-scoped include directories so the engine inherits them via
# target_link_libraries(). Source lists mirror the known-good sets historically
# compiled by the per-library VS projects; the standalone tools and test/example
# programs that live in these trees (e.g. ljpeg/extras, cjpeg/djpeg, pngtest,
# vorbis barkmel/tone) are intentionally excluded.
#
# Warnings in third-party C are silenced (/W0) and CRT-secure warnings disabled.
# The MSVC runtime (static /MT) is inherited from CMAKE_MSVC_RUNTIME_LIBRARY set
# by the root CMakeLists before this directory is added.
# -----------------------------------------------------------------------------

function(torque_thirdparty_lib name)
    if(MSVC)
        target_compile_options(${name} PRIVATE /W0)
        target_compile_definitions(${name} PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE)
    endif()
    set_target_properties(${name} PROPERTIES FOLDER "ThirdParty")
endfunction()

# --- SDL 2 (Linux desktop only) ----------------------------------------------
# Vendored from Torque3D and built by SDL's own CMake, the way Torque3D builds it
# (see sdl/README-TORQUE2D.md) -- except static rather than shared, so the engine
# still ships as one executable. The Web build takes SDL 2 from Emscripten's port
# (-sUSE_SDL=2) instead, and Windows, macOS, iOS and Android don't use SDL.
# EXCLUDE_FROM_ALL: nothing is built unless the engine links SDL2::SDL2-static.
if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
    set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
    set(SDL_TEST OFF CACHE BOOL "Build the SDL2_test library" FORCE)
    set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "The engine has its own main()" FORCE)
    set(SDL2_DISABLE_INSTALL ON CACHE BOOL "Don't install SDL with the engine" FORCE)
    set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "Don't add an uninstall target" FORCE)
    add_subdirectory(sdl ${CMAKE_CURRENT_BINARY_DIR}/sdl2 EXCLUDE_FROM_ALL)
endif()

# --- zlib (built from source on desktop; Android uses the NDK system libz) -----
if(NOT ANDROID)
add_library(zlib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/adler32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/deflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzclose.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/gzwrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/infback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/inffast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/inflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/inftrees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/trees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/uncompr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib/zutil.c
)
target_include_directories(zlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
# On Unix, zlib's gz*.c call read/write/close/lseek, declared in <unistd.h>.
# zconf.h only includes it when Z_HAVE_UNISTD_H is set (the ./configure macro);
# define HAVE_UNISTD_H so it does. Modern clang (macOS) errors on the otherwise
# implicit declarations; on Windows there is no <unistd.h> (uses <io.h>).
if(UNIX)
    target_compile_definitions(zlib PRIVATE HAVE_UNISTD_H)
endif()
torque_thirdparty_lib(zlib)
endif()

# --- lpng (libpng) ------------------------------------------------------------
add_library(lpng STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/png.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngerror.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngget.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngpread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngrio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngrtran.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngrutil.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngtrans.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngwio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngwrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngwtran.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lpng/pngwutil.c
)
# png.h includes zlib.h, so zlib's headers are part of lpng's public interface.
target_include_directories(lpng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lpng)
if(ANDROID)
    target_link_libraries(lpng PUBLIC z)   # NDK system zlib (headers in the sysroot)
else()
    target_link_libraries(lpng PUBLIC zlib)
endif()
torque_thirdparty_lib(lpng)

# --- ljpeg (libjpeg) ----------------------------------------------------------
add_library(ljpeg STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcapimin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcapistd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jccoefct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jccolor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcdctmgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jchuff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcinit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcmainct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcmarker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcmaster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcomapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcparam.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcphuff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcprepct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jcsample.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jctrans.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdapimin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdapistd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdatadst.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdatasrc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdcoefct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdcolor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jddctmgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdhuff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdinput.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdmainct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdmarker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdmaster.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdmerge.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdphuff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdpostct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdsample.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jdtrans.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jerror.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jfdctflt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jfdctfst.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jfdctint.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jidctflt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jidctfst.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jidctint.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jidctred.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jmemmgr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jmemnobs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jquant1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jquant2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg/jutils.c
)
target_include_directories(ljpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ljpeg)
torque_thirdparty_lib(ljpeg)

# --- libogg -------------------------------------------------------------------
# config_types.h is already populated on disk; no configure step required.
add_library(libogg STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libogg/src/bitwise.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libogg/src/framing.c
)
target_include_directories(libogg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libogg/include)
torque_thirdparty_lib(libogg)

# --- libvorbis ----------------------------------------------------------------
add_library(libvorbis STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/analysis.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/bitrate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/block.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/codebook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/envelope.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/floor0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/floor1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/info.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/lookup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/lpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/lsp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/mapping0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/mdct.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/psy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/registry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/res0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/sharedbook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/smallft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/synthesis.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/vorbisenc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/vorbisfile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/window.c
)
target_include_directories(libvorbis
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/include
    # Root holds the internal headers (codec_internal.h, window.h, ...); lib/ holds
    # the mode/codebook trees referenced as "modes/..." and "books/..." (matches VS).
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis
            ${CMAKE_CURRENT_SOURCE_DIR}/libvorbis/lib)
target_link_libraries(libvorbis PUBLIC libogg)
torque_thirdparty_lib(libvorbis)

# --- freetype (built from source for the Web/WASM build only) ------------------
# Desktop platforms use their system font APIs (Win32 GDI / CoreText / X11 Xft) and
# Linux links the system FreeType via find_package; Android links the prebuilt
# static .a. ONLY the Emscripten build needs FreeType compiled from source — the
# browser has no system fonts, so EmscriptenFont rasterizes a bundled .ttf with it.
# The vendored 2.4.12 tree ships one "aggregator" .c per module (each #includes that
# module's translation units), so a minimal TrueType + anti-aliased-raster build is
# just these files. FT2_BUILD_LIBRARY is REQUIRED to compile FreeType's internals
# (its private headers #error without it).
if(EMSCRIPTEN)
    set(_ft ${CMAKE_CURRENT_SOURCE_DIR}/freetype/android/freetype-2.4.12)
    add_library(freetype STATIC
        ${_ft}/src/base/ftbase.c       # core (ftobjs/ftstream/ftoutln/ftcalc/...)
        ${_ft}/src/base/ftinit.c       # FT_Init_FreeType + module registration
        ${_ft}/src/base/ftsystem.c     # ANSI memory/stream layer
        ${_ft}/src/base/ftdebug.c      # debug/trace layer
        ${_ft}/src/sfnt/sfnt.c         # SFNT tables (cmap/hmtx/...) — TrueType needs it
        ${_ft}/src/truetype/truetype.c # TrueType font driver
        ${_ft}/src/smooth/smooth.c     # anti-aliased (grayscale) rasterizer
        ${_ft}/src/raster/raster.c     # monochrome rasterizer (FT_RENDER_MODE_MONO)
        ${_ft}/src/autofit/autofit.c   # auto-hinter (better small-size output)
        ${_ft}/src/psnames/psnames.c   # PostScript glyph-name service
        ${_ft}/src/psaux/psaux.c       # PostScript aux (CFF/Type1 support)
        ${_ft}/src/pshinter/pshinter.c # PostScript hinter
    )
    target_include_directories(freetype PUBLIC ${_ft}/include)
    target_compile_definitions(freetype PRIVATE FT2_BUILD_LIBRARY)
    torque_thirdparty_lib(freetype)
endif()
