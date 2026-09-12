#!/usr/bin/env bash
# ---------------------------------------------------------------------------
#  Builds Torque2D on Linux from CMake — one shot: configure + compile.
#
#  This is the Linux counterpart to generate-vs2022.bat / generate-vs2026.bat.
#  Those open an IDE that does the build for you; on Linux there is no IDE in
#  the loop, so this script configures CMake AND runs the compile, leaving a
#  runnable executable at the repository root.
#
#  Usage:   ./build-linux.sh [Debug|Release|Shipping]      (default: Debug)
#  Example: ./build-linux.sh Release
#
#  If you only want to CONFIGURE (e.g. to build from an editor or by hand),
#  use generate-make.sh instead.
#
#  Requires CMake, a C/C++ toolchain, and the dev packages. On Debian/Ubuntu:
#    sudo apt install build-essential cmake \
#         libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev \
#         libxi-dev libxss-dev libxft-dev libfreetype6-dev \
#         libopenal-dev libgl1-mesa-dev
#  SDL 2 is not among them: it is vendored in engine/lib/sdl and built by the
#  configure step, and the libx*-dev packages are what it needs to build.
#  For a 32-bit build see cmake/BUILD-PLATFORM-NOTES.md.
# ---------------------------------------------------------------------------
set -e

# Run from the folder this script lives in, regardless of where it was launched.
cd "$(dirname "$0")"

BUILD_TYPE="${1:-Debug}"
BUILD_DIR="build/make"

echo ""
echo "  ==================================================="
echo "    Torque2D : building on Linux ($BUILD_TYPE)"
echo "  ==================================================="
echo ""

if [ ! -f "CMakeLists.txt" ]; then
  echo "  ERROR: CMakeLists.txt was not found next to this script."
  echo "  Please keep build-linux.sh in the root of the Torque2D repository."
  exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "  ERROR: CMake was not found."
  echo "  Install it (e.g. 'sudo apt install cmake'), then run this script again."
  exit 1
fi

# Bound parallelism to the core count. A bare 'make -j' (no number) launches
# every translation unit at once — hundreds of g++ processes — which can
# exhaust RAM and get the build OOM-killed (see the CI note in PR-builds.yml).
JOBS="$(nproc 2>/dev/null || echo 4)"

echo "  Configuring into: $BUILD_DIR"
echo ""
cmake -S . -B "$BUILD_DIR" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo ""
echo "  Compiling with $JOBS parallel jobs ..."
echo ""
cmake --build "$BUILD_DIR" --parallel "$JOBS"

# The executable is dropped at the repo root: Torque2D_DEBUG (Debug) or
# Torque2D (Release/Shipping). It MUST run from the repo root so it can find
# main.cs and the script/asset trees.
if [ "$BUILD_TYPE" = "Debug" ]; then
  EXE="./Torque2D_DEBUG"
else
  EXE="./Torque2D"
fi

echo ""
echo "  ==================================================="
echo "    Success."
echo "  ==================================================="
echo ""
if [ -x "$EXE" ]; then
  echo "  Built: $EXE"
else
  echo "  Built (executable expected at $EXE)."
fi
echo ""
echo "  Run it from the repository root:"
echo "      $EXE"
echo ""
echo "  (It must run from the repo root to find main.cs and the asset trees.)"
echo ""
