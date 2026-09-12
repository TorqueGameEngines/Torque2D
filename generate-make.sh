#!/usr/bin/env bash
# ---------------------------------------------------------------------------
#  Generates a Unix Makefile build for Torque2D from CMake (Linux).
#
#  Usage:  ./generate-make.sh [Debug|Release|Shipping]   (default: Debug)
#  Then:   cmake --build build/make -j
#
#  Requires CMake and the usual dev packages, e.g. on Debian/Ubuntu:
#    sudo apt install build-essential cmake nasm libx11-dev libxext-dev \
#         libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev \
#         libxft-dev libfreetype6-dev libopenal-dev libgl1-mesa-dev
#  (nasm is only needed for the 32-bit build. SDL 2 is vendored in engine/lib/sdl
#  and built by the configure step; the libx*-dev packages are its build deps.)
#
#  For a 32-bit build see cmake/BUILD-PLATFORM-NOTES.md (multilib + :i386 libs and
#  -m32 flags). STATUS: builds & links on Linux (verified 32 & 64-bit).
# ---------------------------------------------------------------------------
set -e
cd "$(dirname "$0")"

BUILD_TYPE="${1:-Debug}"

echo ""
echo "  Torque2D : generating a Unix Makefile build ($BUILD_TYPE) ..."
echo ""

if ! command -v cmake >/dev/null 2>&1; then
  echo "  ERROR: CMake was not found. Install it (e.g. 'sudo apt install cmake')."
  exit 1
fi

cmake -S . -B build/make -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo ""
echo "  Configured. Now build with:"
echo "      cmake --build build/make -j\$(nproc)"
echo ""
echo "  The executable is written to the repository root (Torque2D_DEBUG / Torque2D)."
