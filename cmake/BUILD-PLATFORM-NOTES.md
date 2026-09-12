# Build platform notes (CMake source-of-truth migration)

Status board and handoff notes for finishing the per-platform CMake builds. The
goal is to make CMake the single source of truth and generate the per-platform
project files from it.

## Status

| Platform | CMake wiring | Configured | Built | Runtime verified |
|----------|--------------|------------|-------|------------------|
| Windows (VS2022) | ✅ | ✅ | ✅ Debug+Release | ✅ (GUI launches) |
| Windows (VS2026) | ✅ | — | — | generator supported by CMake 4.x; needs VS2026 installed |
| macOS (arm64) | ✅ | ✅ | ✅ Debug (.app, signed) | ✅ (single window; editor renders + animates) |
| Linux x86_64 (Make) | ✅ | ✅ | ✅ Debug+Release | ✅ (GUI launches under WSLg) |
| Linux x86 32-bit (Make, -m32) | ✅ | ✅ | ✅ Debug+Release | ✅ (boots+GL init under WSLg via llvmpipe) |
| iOS (arm64 simulator) | ✅ | ✅ | ✅ Debug (.app, full bundle) | ✅ editor renders + touch works (user-confirmed) |
| iOS (arm64 device) | ✅ | ✅ | ✅ Debug (.app, code-signed) | ✅ runs on a real iPad — perfect FPS, touch good (user-confirmed) |
| Android (Gradle+CMake) | ✅ | ✅ (CI) | ✅ APK (CI) | ✅ editor boots, renders & runs on a real Pixel 7 Pro via Firebase Test Lab — main UI (Roboto) text renders; only un-baked decorative faces (e.g. "black ops one") stay blank. See Android round |
| Web (Emscripten/WASM) | ✅ | ✅ | ✅ Debug+Release (.html/.js/.wasm/.data) | ✅ editor renders in-browser — Project Manager UI with full TEXT (.uft cache + a FreeType-rasterized Roboto fallback for any uncached face/size) + sprites; toys render incl. blended/lit draws (PyramidToy light); stable, no crash. On SDL 2 since the SDL 2 round: typing (accented letters too), the mouse wheel and browser resizes verified headless |

**Linux (32 & 64-bit) builds and links** (verified in WSL/Ubuntu 22.04). The
**64-bit Debug GUI runtime is verified under WSLg** (`./build-linux.sh` →
`./Torque2D_DEBUG`): the Project Manager window launches, OpenGL initializes via
WSLg's D3D12/Mesa GL, and it shuts down cleanly. The **32-bit Debug runtime is also
verified** under WSLg — same boot/GUI, but on llvmpipe (software GL), since WSLg's
hardware-GL passthrough is 64-bit only. See the WSL caveat below.
**macOS (arm64) is DONE and RUNTIME-VERIFIED** (Apple Silicon, Xcode 16.2). It
builds + code-signs a `Torque2D_DEBUG.app`, launches as a single window from Xcode,
boots, and the Project Manager / editor renders and animates correctly. Getting
there past "it builds" took six runtime fixes — see the macOS round below. **iOS
(arm64) is DONE and RUNTIME-VERIFIED on both the simulator AND a real iPad
(iOS 26.2.1, Xcode 26.x).** The editor renders, point-based GUI scale is correct,
and touch input works; on hardware the frame rate is perfect (the simulator's poor
FPS was its GLES translation layer, not the engine). Getting from "builds" to "runs"
took four runtime fixes (frozen clock, frame-allocator size, point-vs-pixel scale,
touch release) — see the iOS round below. The device build is code-signed with a free
Apple ID (7-day profile; no paid account).

**Legacy projects retired.** With Windows, macOS, Linux (32 & 64-bit), and iOS all
CMake-runtime-verified, the hand-maintained project files were deleted from
`engine/compilers/` (the VS 2019/2022 solutions, the macOS `Xcode` project, the
`Make-32bit`/`Make-64bit` Makefiles, and the `Xcode_iOS` project) — CMake is now their
single source of truth. What remains under `engine/compilers/` is intentionally kept:
`android-studio` (the Gradle shell that *drives* CMake via the NDK) and `emscripten`
(the legacy reference recipe — now superseded since the Web target is CMake-runtime-verified
via `emcmake` + the shared `PlatformSources.cmake`; kept for reference and a candidate for
retirement). (`cmake-modules` is retained because `emscripten/CMakeLists.txt` includes
`CopyFiles` from it.)

## How the build is structured

- `CMakeLists.txt` (root) — modern, target-based. Selects the active platform's
  back-end source list and applies platform link libs/frameworks/defs.
- `cmake/EngineSources.cmake` — explicit cross-platform engine sources (the
  `platform/` abstraction is here; it compiles on every OS).
- `cmake/PlatformSources.cmake` — OS-specific back-ends: `..._WINDOWS`,
  `..._MACOS` (Objective-C++ `.mm`), `..._LINUX`, `..._IOS`, `..._ANDROID`,
  `..._EMSCRIPTEN`.
- `engine/lib/CMakeLists.txt` — third-party static libs (platform-neutral; MSVC
  flags are guarded by `if(MSVC)`).
- Generator scripts at repo root: `generate-vs2022.bat`, `generate-vs2026.bat`,
  `generate-xcode.command` (macOS), `generate-xcode-ios.command` (iOS simulator),
  `generate-xcode-ios-device.command` (iOS device, code-signed), `generate-make.sh`,
  `generate-emscripten.sh` (Web/WASM via `emcmake`).

## macOS round (run on a Mac) — DONE (builds, signs, runs; arm64)

Verified on Apple Silicon (Xcode 16.2): builds with both generators (0 errors),
code-signs, launches as a single window from Xcode, boots, and the editor renders +
animates. Configure + build:
- Xcode (recommended): `./generate-xcode.command` (or `cmake -S . -B build/xcode -G
  Xcode`), then in Xcode pick the `Torque2D` scheme and Cmd-R. The Run scheme is
  preconfigured with the repo root as its working directory.
- Makefiles: `cmake -S . -B build/macos -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug`
  then `cmake --build build/macos -j8`.
- The product is a real `.app` bundle at the repo root (`Torque2D_DEBUG.app`). The
  engine's `getExecutablePath()` finds `main.cs` in the bundle's PARENT dir, so no
  asset repackaging is needed. NOTE: a `.app` must be launched via LaunchServices
  (Xcode / `open` / double-click) — running the inner binary directly, or `open`ing
  it from a headless/non-GUI session, won't deliver the launch event (launchd error
  153), so GUI runtime can only be checked from an interactive desktop.

Resolved RUNTIME issues (these only appear once the app actually runs; six of them
stood between "it builds" and "the editor works"):
- **No window** — the bare executable installed no app delegate, so the engine never
  booted. Fixed by bootstrapping AppKit programmatically in `platformOSX/main.mm`
  (create NSApplication, set Regular activation policy, install AppDelegate, run) —
  no nib required; the legacy `NSApplicationMain`/nib path is preserved for a bundle
  that ships one.
- **Crash on launch** — `GuiListBoxCtrl`'s `std::sort` comparators weren't a strict
  weak ordering (`<=` for equality, negation for descending); modern libc++ aborts
  on that. Fixed in `gui/guiListBoxCtrl.h`.
- **Frozen UI / no scheduled events** — `getRealMilliseconds()` cast an out-of-range
  `double` to `U32`, which SATURATES to a constant on arm64 (`fcvtzu`), so the sim
  clock never advanced. Fixed via `U64` in `platformOSX/osxTime.mm`. (See the
  recurring-bug note at the end of this section.)
- **Duplicate windows (only under Xcode)** — the target was a `com.apple.product-
  type.tool`, and Xcode relaunches a *tool* that becomes a GUI app via LaunchServices
  → Terminal, spawning extra copies. Fixed with `MACOSX_BUNDLE` (a real `.app` has a
  stable LaunchServices identity, launched once).
- **CodeSign failure** — a `.app` must be signed to run on arm64; also the repo-root
  `Torque2D_DEBUG.app` got contaminated by stale legacy + iOS build artifacts (iOS
  writes a *flat* `.app` to the same path), which breaks codesign ("unsealed contents
  in the bundle root"). Fixed with ad-hoc signing (`CODE_SIGN_IDENTITY="-"`, Manual
  style) + cleaning the stale `.app`. **Delete the repo-root `.app` when switching
  between the iOS and macOS builds in one checkout.**
- **Editor UI hidden behind the background** — every fade-OUT was frozen.
  `FluidColorI::processValue` did `(U8)mRound((target-start)*progress)`; for a fade
  DOWN, `(target-start)` is negative and `(U8)(negative float)` saturates to 0 on
  arm64, so alpha never left the start value. The Project Manager rendered fine but
  sat under a `torqueCurtain` that never faded. Fixed in `math/mFluid.h` (round in
  signed space, cast only the final sum to U8).

**Recurring arm64 trap:** three of the above (clock, fades) are the SAME bug class —
converting an out-of-range or negative float to an unsigned int **saturates** on
arm64 where x86 silently **wrapped**. For any Apple-Silicon "value won't change"
runtime bug, suspect this first; grep for `(U8)`/`(U32)` casts of float/time/`mRound`
results. Casting a *positive in-range* value is fine (e.g. font metrics).

Resolved BUILD issues (were latent in the scaffold):
- **zlib needs `<unistd.h>`.** `engine/lib/zlib/gz*.c` call `read/write/close/lseek`;
  zconf.h only includes `<unistd.h>` when `Z_HAVE_UNISTD_H` is set. Modern clang
  errors on the otherwise-implicit declarations. Fixed by defining `HAVE_UNISTD_H`
  on the zlib target for `UNIX` (`engine/lib/CMakeLists.txt`).
- **Classic-Mac-OS landmines in the vendored libs (`TARGET_OS_MAC`).** Several of
  the old vendored C libs gate code on `MACOS`/`TARGET_OS_MAC` assuming it means
  *Classic* Mac OS. But `TARGET_OS_MAC` is 1 on ALL modern Apple platforms (macOS
  *and* iOS), so those branches wrongly fire and reference headers/behaviour that
  no longer exist. They are latent on older SDKs (which don't define
  `TARGET_OS_MAC` until late) and fire on newer ones (Xcode 16.4 / macOS SDK 15.5 /
  iPhoneOS 18.5 on the CI runners). Each surfaces only once the prior is fixed,
  since the libs build in sequence. Fixed in place by excluding modern Apple
  (`!defined(__APPLE__)`):
    - `zlib/zutil.h` — `#define fdopen(fd,mode) NULL` clobbered the SDK's
      `<stdio.h>` `fdopen` (`HAVE_UNISTD_H` above pulls `<unistd.h>` early, which
      defines `TARGET_OS_MAC` before the branch).
    - `lpng/pngpriv.h` — included the dead Classic-Mac `<fp.h>` instead of `<math.h>`.
  The other Apple-compiled libs are clean: `ljpeg/jconfig.h` already handles
  `__APPLE__`; `libogg`/`libvorbis` only branch on `_WIN32`. (The `TARGET_OS_MAC`
  hits under `engine/lib/openal/*` and `engine/lib/freetype/android/*` are for
  Windows/Android/iPhone-framework headers NOT compiled by the Apple CMake build —
  macOS/iOS link the system OpenAL.framework and use Cocoa/UIKit fonts.)
- **Cocoa prefix header.** The `platformOSX` `.mm` back-end uses AppKit/Foundation
  types at file scope (NSApplicationMain, NSEvent, NSCursor, NSAutoreleasePool,
  NSTask, NSString, ...) and relied on the legacy Xcode prefix header. Reproduced
  by force-including `tools/CMake/macOS-Prefix.h` (`-include`, guarded by `__OBJC__`
  so C/C++ TUs are unaffected).
- **One stray include.** `osxCocoaUtilities.mm` used `#import "fileDialog.h"`; fixed
  to the canonical `"platform/nativeDialogs/fileDialog.h"` (matches every other
  reference; the header dir was never on the search path).
- **Architecture:** `CMAKE_OSX_ARCHITECTURES` is set to `arm64` (Apple Silicon) in
  the `APPLE` block, overridable on the command line for Intel/universal builds. The
  old build hard-coded `x86_64`.
- **Deployment target:** `CMAKE_OSX_DEPLOYMENT_TARGET` is pinned to `11.0` (Big Sur,
  the arm64 floor). Without it the binary inherited the host SDK default (14.6 here),
  needlessly excluding older Macs. 11.0 does not block a future Metal renderer
  (Metal/MetalKit ship since 10.11; only the Metal 3 feature set would need 13.0+).
  The legacy Xcode project used 10.13 (an Intel-era value; predates arm64).

Comparison against the legacy `engine/compilers/Xcode` project (now **retired** — see
it in git history before the CMake migration; differences below are deliberate or
benign, not bugs):
- **C++17** (vs legacy C++14) and the **modern C standard** (vs legacy `gnu89`) are
  intentional. `gnu89` is precisely what masked the zlib implicit-declaration error;
  it's fixed properly via `HAVE_UNISTD_H` rather than by loosening the C dialect.
- The legacy project linked `ApplicationServices` and `QD` (QuickDraw) frameworks
  and put `QD.framework` on the header path. The CMake build compiles and links
  without them (QuickDraw is long dead; no active code includes it). Add them back
  only if a runtime/link symbol turns up missing.
- Source membership: the CMake build is a SUPERSET of the legacy project's compiled
  sources — nothing is dropped. Third-party libs are separate static-lib targets
  (`engine/lib/`); libjpeg uses `jmemmgr.c`+`jmemnobs.c` where legacy used the
  equivalent `jmemansi.c`. CMake additionally compiles the editorToy sources,
  `arrayObject`, `b2ParticleAssembly`, the (arm64-inert) x86 SIMD math TUs, and the
  GoogleTest suite.

## iOS round (run on a Mac) — DONE (runtime-verified; arm64 simulator AND real device)

iOS is a **separate** platform from macOS (distinct `platformiOS/` sources and a
UIKit/OpenGL-ES framework stack), and was **never supported by the old CMake** —
the recipe here was derived fresh from the maintained `engine/compilers/Xcode_iOS`
project. `CMakeLists.txt` distinguishes it via `TORQUE_IOS` (since `APPLE` is true
on iOS too). Verified building/linking a `Torque2D_DEBUG.app` against the iOS 18.2
simulator SDK.

Configure (arm64 simulator — no code-signing needed for a first pass):
```
cmake -S . -B build/ios -G Xcode -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0
cmake --build build/ios --config Debug
```
Use full Xcode, not just the Command Line Tools — point at it with
`DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer` (or `sudo xcode-select -s`).

**Device build (code-signed):** `./generate-xcode-ios-device.command` (configures into
`build/ios-device` with `-DCMAKE_OSX_SYSROOT=iphoneos`). The root `CMakeLists` detects
simulator-vs-device from the SDK: a `[Ss]imulator` sysroot keeps `CODE_SIGNING_ALLOWED=NO`;
anything else (incl. the default empty → iphoneos) switches to `CODE_SIGN_STYLE=Automatic`.
Supply the signing identity one of two ways: pass `-DTORQUE_IOS_TEAM=<10-char team id>`
(the device script forwards `$TORQUE_IOS_TEAM`/`$TORQUE_IOS_BUNDLE_ID` from the env), or
leave it empty and pick the team in Xcode's target → Signing & Capabilities. The bundle id
(`-DTORQUE_IOS_BUNDLE_ID`, default `org.torque2d.Torque2D`) must be unique to the Apple
account; a free Apple ID works for running on your own device (7-day profile). On the device:
enable Developer Mode (Settings → Privacy & Security), and trust the cert after first install
(Settings → General → VPN & Device Management). A device `.app` is a thin/arm64 bundle; the
same POST_BUILD content copy applies, so `main.cs` + the trees ship inside it.

**Verified on real hardware** (an iPad on iOS 26.2.1, Xcode 26.x) — the editor runs, the
point-based GUI scale is correct, touch input works, and the frame rate is perfect. Toolchain
note: an iOS 26 device needs Xcode 26.x (hence a recent macOS — this required upgrading off
Sonoma 14.6.1). Free-account first-launch gotcha that cost real time: the device showed
"Unable to Verify App — An Internet connection is required" / "Developer App Certificate is
not trusted" *even while online*. With a free Personal Team, iOS must reach Apple's cert server
(`ppq.apple.com`) on first launch, and that check fails silently on clock skew or a stuck network
state. **What fixed it: restart the iPad, then Verify** (with Date & Time on Automatic). Toggling
airplane mode and dropping to cellular-only did NOT help. A paid Developer Program account would
remove the online-verify step (and the 7-day expiry) entirely, but it isn't needed for spot-testing.

Resolved issues:
- **`TORQUE_OS_IOS` must be predefined by the build.** `platform/types.gcc.h` gates
  the iOS branch on `TORQUE_OS_IOS` but only *defines* it inside that branch — a
  chicken-and-egg. Without it, `__APPLE__` selects the macOS/desktop-GL back-end and
  fails on `<OpenGL/gl.h>`. The `TORQUE_IOS` block now defines `TORQUE_OS_IOS` (as
  the legacy Xcode_iOS project did).
- **UIKit prefix header.** Same mechanism as macOS: force-include
  `tools/CMake/iOS-Prefix.h` (imports UIKit + Foundation, `__OBJC__`-guarded).
- **`glDrawArraysProcPtr` collision (modern SDK).** The debug-only "outline GL"
  feature does `#define glDrawArrays glDrawArraysProcPtr`. On the modern SDK the
  prefix header drags GLES `gl.h` in a second time (UIKit → CoreImage), and the
  macro rewrites the SDK's `glDrawArrays` *function* decl into `glDrawArraysProcPtr`,
  colliding with the engine's same-named *variable*. Fixed by defining
  `NO_REDEFINE_GL_FUNCS` for the iOS target (the engine's own escape hatch) — the
  outline/wireframe debug draw becomes a no-op on iOS. (macOS is unaffected because
  the Cocoa prefix header doesn't pull GL.)
- **`graphics/bitmapPvr.cc` (PVR textures)** is required on iOS but excluded from
  desktop builds in `EngineSources.cmake`; the `TORQUE_IOS` block adds it back.
- **GameKit:** `"-framework GameKit"` is linked for `platformiOS/GameCenter.mm`.
- **Output location:** like every target, the `.app` lands at the repo root
  (`Torque2D_DEBUG.app`). A stale macOS bundle of the same name can leave a leftover
  `Contents/` subdir inside it — harmless; delete it if it bothers you.

Comparison against the legacy `engine/compilers/Xcode_iOS` project (now **retired** —
see git history): same as macOS,
the CMake source set is a superset (it also compiles the GoogleTest suite, which the
legacy iOS project omitted). Deployment target is set to 12.0 to match legacy
(`XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET` + `CMAKE_OSX_DEPLOYMENT_TARGET`);
note the arm64 *simulator* slice reports minos 14.0 (simulators have their own floor)
— a device build honors 12.0. Legacy used C++14; we use C++17 (engine-wide).

Runnable bundle — DONE. The first simulator run showed the app launching but
**nothing rendering**, because the CMake-generated `.app` was bundle-incomplete: it
had no `CFBundleIdentifier`, no storyboard, and no game content. The iOS app's
window is NOT created programmatically — `T2DAppDelegate` (`@synthesize window`, no
`UIWindow alloc`) relies on a **Main storyboard** (`UIMainStoryboardFile`) to
instantiate `T2DViewController` (a `GLKViewController`) hosting `T2DView` (the
`GLKView` the engine renders into). The bare CMake plist had none of that, so the
delegate launched but no window/GL view ever existed. Fixed by reproducing the
legacy `engine/compilers/Xcode_iOS` bundle in CMake (`TORQUE_IOS` block):
- **`tools/CMake/iOS-Info.plist.in`** (wired via `MACOSX_BUNDLE_INFO_PLIST`):
  bundle id, `UIMainStoryboardFile`(+`~ipad`), `UILaunchStoryboardName`,
  `LSRequiresIPhoneOS`, landscape orientations, `UIRequiresFullScreen`. Device
  family + bundle id are ALSO set as build settings (`TARGETED_DEVICE_FAMILY`,
  `PRODUCT_BUNDLE_IDENTIFIER`) because Xcode overrides the matching plist keys and
  warns otherwise.
- **Storyboards** bundled as compiled resources (ibtool): `iPhoneStoryboard` /
  `iPadStoryboard` (copied from the legacy project into `tools/CMake` so the build
  is self-contained — the GLKit UI) + a new minimal static **`LaunchScreen`**
  (modern iOS needs a launch storyboard for a full-screen drawable; it may contain
  no custom classes, so it's separate from the GL storyboards).
- **Game content** copied into the flat `.app` root via POST_BUILD: `main.cs` +
  `editor/`, `library/`, `toybox/`, `tools/`. An installed iOS app is sandboxed, so
  `getExecutablePath()` resolves to the bundle and the content must live inside it
  (the desktop build instead runs from the repo-root cwd). `main.cs` boots the
  editor (`exec "./editor/main.cs"`), same as desktop.

Verified: `xcodebuild ... -sdk iphonesimulator -arch arm64` BUILD SUCCEEDED with the
storyboards compiled/linked and all content present in `Torque2D_DEBUG.app`. Generate
with `./generate-xcode-ios.command`.

Runtime — DONE (the editor renders; user-confirmed from Xcode on an iPad Pro 11" M4
iOS 18.3 simulator). Three fixes stood between "launches" and "renders":
- **Black screen #1 — frozen clock.** `platformiOS/iOSTime.mm` `getRealMilliseconds()`
  was the **arm64 float→unsigned saturation** bug (see the macOS round), in iOS's own
  time file that the `osxTime.mm` fix never touched: `mach_absolute_time() *
  absolute_to_millis` (a huge double) was stored in a Carbon `Duration` (SInt32) and
  saturated to INT_MAX on arm64 → constant time → sim clock never advanced → the
  editor's fade-in curtain stayed opaque → black. Fixed via U64 (mirrors `osxTime.mm`).
  `mFluid.h`/`guiListBoxCtrl.h` are shared headers, already applied to iOS.
- **Black screen #2 — FrameAllocator assert.** `game/defaultGame.cc` initialized the iOS
  frame allocator at 256/512KB (a 2013, ~256MB-RAM-era value), but `main.cs` boots the
  full desktop editor; a boot-time allocation overran the buffer and tripped the fatal
  `frameAllocator.h:102` "alloc too large" assert → halt. Bumped iOS to the desktop 3MB
  (negligible on modern devices; Android/Emscripten keep the small budget).
- **Half-size GUI + broken scene picking — pixel-vs-point coordinate mismatch.**
  The engine ran in PIXEL resolution (`$pref::iOS::Width = points * scale`) while the GL
  backing was built at POINT resolution (`createFramebuffer` runs in `viewDidLoad` and the
  old `contentScaleFactor` set in `Platform::initWindow` ran later and only for `scale==2`).
  The decision (user direction) is to render in **points**, not pixels: a Retina display
  packs 2-3x the pixels into the same physical area, so a GUI laid out in raw pixels is
  half/third size. The fix makes logical resolution, GL backing, and touch input all share
  ONE point-space coordinate system:
    * `iOSWindow.mm` `Platform::init`: `$pref::iOS::Width/Height` = point bounds (dropped the
      `* screenScale`).
    * `T2DViewController.mm`: force `view.contentScaleFactor = 1` BEFORE `createFramebuffer`
      (a UIView otherwise defaults it to the screen scale), and `retinaEnabled = false` so
      touch points are not scaled up. `iOSWindow.mm` `initWindow` also pins it to 1 (and no
      longer special-cases `scale==2`).
  Trade-off: rendering is at point resolution (UIKit upscales the layer to the physical
  screen → slightly soft on Retina), but GUI sizing and scene picking are correct. A future
  improvement could decouple a points logical space from a pixel backing for crisp Retina,
  but that needs the GL viewport to use `backingWidth` while the projection stays in points.
  (Confirmed correct on the simulator and on a real iPad.)
- **Touch never released (press stuck) — two layers.** (1) `iOSInput.mm` `createMouseUpEvent`
  posted `SI_BREAK` only on an EXACT coordinate match with the stored slot; the simulator's
  mouse-up lands a pixel off → event dropped. Also it never freed the slot (leak → eventually
  no downs). Fixed: fall back to the first occupied slot, set the cursor to the release point,
  clear the slot. (2) The REAL stick was in `gui/guiCanvas.cc` `rootScreenTouchUp`: unlike the
  desktop `rootMouseUp`, it ignored `mMouseCapturedControl` and only dispatched to the control
  under the release point. A `GuiButtonCtrl` calls `mouseLock()` in `onTouchDown`, so if the
  release didn't re-hit the button it stayed locked + depressed with no way to release. Fixed
  by routing the up to the captured control first (mirrors `rootMouseUp`); touch-only, so
  desktop is unaffected.

All four fixes are confirmed working in the simulator AND on a real iPad. The simulator's
**poor frame rate did NOT carry to hardware** — on the device the frame rate is perfect, so it
was the simulator's GLES translation layer, not an engine problem. Note that on touch there is
no hover/move without a finger down, so the editor cursor only tracks during a press — that is
inherent to touch, not a bug.

Heads-up on the shared output path: iOS and macOS both emit `Torque2D_DEBUG.app` to
the repo root but with INCOMPATIBLE layouts (iOS = flat; macOS = `Contents/`).
Building one then the other in the same checkout corrupts the bundle and breaks
codesign — `rm -rf Torque2D_DEBUG.app` when switching platforms.

## Linux round (run in WSL or on Linux) — DONE (builds & links, 32 & 64-bit)

1. Install deps (Debian/Ubuntu):
   `sudo apt install build-essential cmake nasm libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxft-dev libfontconfig1-dev libfreetype6-dev libopenal-dev libgl1-mesa-dev`
   For 32-bit add the multilib toolchain + `:i386` libs:
   `sudo dpkg --add-architecture i386 && sudo apt update && sudo apt install gcc-multilib g++-multilib libx11-dev:i386 libxext-dev:i386 libxrandr-dev:i386 libxcursor-dev:i386 libxfixes-dev:i386 libxi-dev:i386 libxss-dev:i386 libxft-dev:i386 libfontconfig1-dev:i386 libfreetype6-dev:i386 libopenal-dev:i386 libgl1-mesa-dev:i386`
   **SDL 2 is not a package to install.** It is vendored in `engine/lib/sdl`
   (SDL 2.32.10, the tree Torque3D builds; see its `README-TORQUE2D.md`) and
   compiled by the configure step as a static library. The `libx*-dev` packages
   are its X11 build dependencies: `libxext-dev` is required -- SDL's configure
   stops with "Missing Xext.h" without it -- and each of the others turns on an
   SDL feature (display modes through XRandR, cursors, XInput2 relative mouse,
   screensaver inhibit) that is silently left out when its headers are missing.
   **fontconfig is a direct dependency, not just Xft's:** `x86UNIXFont.cc` calls
   `Fc*` itself for `PlatformFont::enumeratePlatformFonts` (the installed-font list
   the GUI tools offer), so the Linux link list carries `fontconfig` explicitly.
2. 64-bit one-shot: `./build-linux.sh [Debug|Release|Shipping]` (configures **and**
   compiles, bounding `--parallel` to `nproc`, leaving the exe at the repo root).
   Configure-only: `./generate-make.sh Debug` then `cmake --build build/make -j$(nproc)`.
   32-bit (verified building, linking, and running): configure with
   `-DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_EXE_LINKER_FLAGS=-m32`
   (and `PKG_CONFIG_PATH=/usr/lib/i386-linux-gnu/pkgconfig`), then build. `-m32` makes CMake
   auto-detect `CMAKE_LIBRARY_ARCHITECTURE=i386-linux-gnu`, so OpenGL/FreeType/etc.
   resolve to `/usr/lib/i386-linux-gnu`; the root picks the bitness code path from
   `CMAKE_SIZEOF_VOID_P`. Note both builds emit `Torque2D_DEBUG` at the repo root,
   so they overwrite each other — use separate build dirs (`build/make`, `build/make32`)
   and rebuild whichever bitness you want at the root.
3. Resolved issues (the original scaffold's wrong assumptions):
   - **SDL 2, vendored.** The back-end was written against SDL 1.2 and ran on
     either a genuine 1.2.15 or the SDL2-backed `sdl12-compat` shim that Ubuntu
     24.04+ and Arch ship as SDL 1.2 -- two implementations that disagreed about
     windows, events and fullscreen. It now uses the vendored SDL 2 on SDL's X11
     driver (through Xwayland on a Wayland desktop); `SDL_VIDEODRIVER` overrides
     the driver.
   - **`detectX86CPUInfo`** comes from `platform/platformCPUInfo.asm`, 32-bit-only
     NASM (does not assemble for elf64). It's referenced only `#ifndef TORQUE_64`,
     so 64-bit defines `TORQUE_64` (asm unneeded); 32-bit assembles it via NASM.
   - **Bitness macros:** 64-bit defines `TORQUE_64` (`__amd64__` is auto); 32-bit
     defines `i386` (bare `i386` isn't predefined under standard C++, and
     `types.gcc.h`'s CPU detection keys off it).
   - **OpenGL/FreeType** are resolved via `find_package`; SDL is the vendored
     `SDL2::SDL2-static` target from `engine/lib`.
4. **WSL runtime — VERIFIED under WSLg (64-bit Debug).** On a WSL2 box with WSLg
   up (`DISPLAY=:0`, `WAYLAND_DISPLAY=wayland-0`, `/mnt/wslg/.X11-unix/X0`),
   `./Torque2D_DEBUG` launches the Project Manager GUI: OpenGL initializes through
   WSLg's GL stack (`Renderer: D3D12 (...) Mesa`), screen mode sets, editor modules
   load, and it exits 0 on close. Without WSLg/an X server the build/link still
   verifies but the window won't appear.
   **32-bit also boots under WSLg**, but falls back to **llvmpipe (software GL)** —
   `Renderer: llvmpipe (...)` rather than the 64-bit `D3D12 (NVIDIA ...)`, because
   WSLg's hardware-GL passthrough (the d3d12 Mesa driver) is 64-bit only. It still
   renders; on real 32-bit hardware it would use the native GL driver.

## Android round (build via CI or Android Studio + NDK)

Android builds a **shared library** (`libtorque2d.so`) via Gradle → `externalNativeBuild { cmake }`
(the root `CMakeLists.txt`), loaded by a NativeActivity; the script/asset tree is copied into the
APK assets by the `copyGame` Gradle task. The old `Android.mk` (stale, referenced deleted files)
and its `.cxx` cache were removed; the Gradle project was modernized to AGP 8.6 / Gradle 8.7 /
`compileSdk 34` / `namespace`. Target is **arm64-v8a only** (the only ABI with prebuilt
freetype/openal). Build it with the CI job (`./gradlew assembleDebug`) or open
`engine/compilers/android-studio` in Android Studio.

**DONE — the Android CI job builds a working APK (arm64-v8a).** Getting there took
(all are legit cross-platform correctness fixes):
- `settings.gradle`: `plugins{}` must follow `pluginManagement{}` (before `dependencyResolutionManagement`).
- Committed the vendored prebuilt arm64 `libfreetype.a` / `libopenal.so` (the global `*.a`/`*.so`
  ignore was hiding them, so CI had nothing to link).
- `types.gcc.h`: detect `__aarch64__` (NDK/Linux) as 64-bit ARM, not just Apple's `__arm64__`
  (fixed CPU/endian + `TORQUE_CPU_X64`).
- `-Wno-register` for non-MSVC (the engine uses the C++17-removed `register` in ~35 files; clang errors).
- `mMathSSE.cc`: gate the x86 SSE inline asm on `TORQUE_CPU_X86_64`, not `TORQUE_CPU_X64`
  (which is now also set for arm64).
- Added `platformAndroid` to the Android include path (`T2DActivity.h` includes the vendored
  `<android_native_app_glue.h>`).

**Runtime — DONE (verified on a real Pixel 7 Pro via Firebase Test Lab).** With no local
arm64 device, the APK is run on a **real Pixel 7 / 7 Pro** through Firebase Test Lab (Robo
test, free Spark tier). The local x86_64 emulator *can* run the arm64 APK via NDK ARM
translation, but a crash there lands in an anonymous translated-code region that can't be
symbolicated — so use a real device. Debug loop: build → FTL Robo run → download the logcat →
`ndk-stack -sym <app/build/intermediates/cxx/Debug/.../obj/arm64-v8a> -dump <logcat>`
(the unstripped lib keeps full symbols + line numbers). The logcat tag for the engine's own
`Con::printf` output is **`Torque2D`** (filter on it to read the boot narrative).

What works: the editor **boots, renders, and runs**. The logcat shows the full boot —
EGL + GLESv1/v2 + OpenAL init, GL up on **Mali-G710 / OpenGL ES-CM 1.1** (screen mode
2232×1080×32, Max Texture Size 16383), **all five editor modules register** (EditorCore,
EditorConsole, ProjectManager, AssetAdmin, GuiEditor), Android logs
`Displayed …MyNativeActivity: +297ms` (first frame drawn), the app runs ~35 s and Robo
crawls the live window — **zero native crashes**. Main UI text (Roboto) renders.

Fixed to get there (each crash surfaced the next, the same crash-by-crash bring-up the
other platforms went through):
- **Empty main.cs dir (the module-registration crash).** Android's process cwd is `/`, so
  `defaultGame.cc` resolved `main.cs` to `/main.cs`, then chopped the filename at the
  *leading* slash, leaving an **empty** main.cs dir. That empty string became the `cwd`
  for every `Platform::makeFullPathName`, so `endptr = buffer + strlen("")-1 = buffer-1`
  (before the buffer) → out-of-bounds path math → SIGSEGV in `catPath`. Fixed in
  `defaultGame.cc`: when the script is at the filesystem root, keep `/` as the directory
  rather than truncating to `""`. Also hardened `platformFileIO.cc` `makeFullPathName` /
  `catPath` (signed remaining-length via `getMax(...,0)` + a `len<3` guard) so a
  degenerate cwd can't overrun the buffer again — a latent cross-platform bug.
- **Font init crash #1 (`AndroidFont::getCharInfo`, null deref, fault 0x98).** Two layers,
  both fixed earlier: the editor's font failed to load (`FT_New_Face` errored), AND
  `AndroidFont::create()` returned `true` even on failure so `getCharInfo` dereferenced an
  invalid `FT_Face`. Now `create()` propagates the real result (→ `createPlatformFont()`
  returns NULL, which `GFont::create`/`GuiControlProfile::getFont` already tolerate) and
  `getCharInfo` guards `!fontFaceCreated || face == NULL`. This stopped the crash *inside*
  AndroidFont but exposed the next one — the font still wasn't loading.
- **Font init crash #2 — the font never resolved (`GFont::isValidChar` null-`this`, fault
  0x1e0 ← `GuiMenuBarCtrl::calculateMenus`).** Root cause was the **Java** side:
  `FontManager.TTFAnalyzer.getTtfFontName()` only read Macintosh (`platformID == 1`) name
  records, but the bundled `Roboto-Regular.ttf` *and* the Pixel's own system fonts ship
  **only** Windows (`platformID == 3`, UTF-16BE) records. So the enumerated font map came up
  empty, `getFont("Roboto")` returned null, `AndroidFont` failed, `GFont::create` returned
  NULL, and `GuiMenuBarCtrl` (one of ~30 GUI sites that deref `getFont()` unguarded)
  crashed. Fixed by accepting `platformID` 1/3/0 and decoding UTF-16BE for 3/0
  (`FontManager.java`). Roboto now resolves → the main UI renders text. (We chose the
  root-cause fix over hardening all ~30 `getFont()->` deref sites; those stay reliant on the
  font loading, which now holds. The unguarded sites remain a latent cross-platform
  robustness gap if a font is ever genuinely missing.)
- **Frame-allocator overflow (`FrameAllocator::alloc` SEGV ← `GFont::read` of a `.uft` ←
  `GuiListBoxCtrl::updateSize`).** Once fonts actually loaded, reading a cached `.uft` glyph
  table allocates a `FrameTemp` larger than Android's **512 KB** frame allocator and
  overran it. This is the SAME class already fixed for iOS/Emscripten (the desktop-class
  editor boot needs 3 MB); Android was the last platform left on the small budget. Collapsed
  `defaultGame.cc` to give **every** platform the 3 MB buffer (negligible on any modern
  device). This was the last boot crash.

**Checked, NOT a bug on Android: the arm64 float→unsigned saturation class** (frozen clock /
frozen fades; fixed on macOS/iOS in `osxTime.mm` / `iOSTime.mm` / `mFluid.h`). `mFluid.h` is
shared (already applied), and `AndroidTime::getRealMilliseconds` is **safe** — unlike the
mac/iOS code that cast a huge raw time, it subtracts a startup baseline (`android_StartupTime()`
at boot, `T2DActivity.cpp:1194`), so the value stays in U32 range. No frozen clock.

Cosmetic follow-up (NOT a crash, DEFERRED): un-baked **decorative faces with no `.uft`
cache** (e.g. the editor title font **`black ops one`** 21/28) render blank — `getFont` for
them falls back to `Helvetica`, which doesn't exist on Android, so `createSafePlatformFont`
"utterly fails". This is the exact gap the web build had before its per-face FreeType
fallback. The intended fix is a per-face → bundled-Roboto fallback (so any unresolved face
rasterizes from Roboto instead of disappearing), mirroring the Emscripten `EmscriptenFont`
approach.

**Two fallback attempts were made and BOTH reverted — deferred pending better runtime
evidence.** (1) In `AndroidFont::create` (C++): on a failed face, call `getFontPath("Roboto")`
and retry — this adds a SECOND `getFontPath` JNI call (extra `AttachCurrentThread`/`Detach`
on the engine thread). (2) In `FontManager.getFont` (Java): return the Roboto path instead of
null when no face matches (no new JNI). **Both** FTL runs showed the engine thread going
**silent right after the `FileWalker` "time in dir java" log** — no crash, no further GL/frame
activity, and **zero `Torque2D`-tagged lines for the whole run** (even ones that precede
`FileWalker`, like `Input Init`, which DID appear in the good `ewrz` run). No `logd` "chatty"
drop markers tie to the app, so it's **ambiguous**: either FTL's logcat capture silently lost
the high-volume `Torque2D` tag (plausible — `ewrz` captured it, these didn't) OR boot really
hangs there. The two attempts use entirely different mechanisms (C++ JNI vs pure-Java map
lookup), and code review finds **no plausible deterministic hang** in the Java path, which
points at FTL flakiness — but it couldn't be proven from lossy n=1 FTL logs. Since this is
purely cosmetic and the verified-working state (`ewrz`: full boot, all modules, no crash) is
already shipped, the fallback was reverted. **Re-attempt on a local arm64 device with reliable
`adb logcat`** (not FTL) to disambiguate before committing; if it's confirmed flaky, the Java
`getFont` → Roboto fallback is the cleaner of the two (single JNI call).

Remaining iteration notes:
- **Engine source set under GLES/NDK:** the unified `EngineSources.cmake` will surface files that
  need `TORQUE_OS_ANDROID`/GLES guards (the old `Android.mk` compiled a smaller, divergent subset).
  Fix by guarding the code, not by forking the list.
- **OpenAL `.so` packaging:** shipped via `jniLibs.srcDirs = ['../../../lib/openal/Android']`; confirm
  it lands in the APK and loads.
- **Java glue** (`MyNativeActivity`, helpers) may need minor AndroidX/API updates under AGP 8.
- **Gradle wrapper jar** is old (5.4.1-era) but should bootstrap 8.7; regenerate with
  `gradle wrapper --gradle-version 8.7` if it doesn't.
- Other ABIs (armeabi-v7a/x86/x86_64) are a later round — need freetype/openal built from source.

## Emscripten / Web round (build with emsdk) — DONE: editor + toys render in-browser

Emscripten builds the engine to **WebAssembly** (`emcc`), emitting
`Torque2D_DEBUG.{html,js,wasm,data}`. The browser owns the event loop, so
`platformEmscripten/main.cpp` drives the engine with
`emscripten_set_main_loop(_EmscriptenGameInnerLoop, 60, false)` → `Game->mainLoop()`
once per animation frame — the SAME callback model as iOS/Android (no blocking
`while`). The back-end (`engine/source/platformEmscripten/*`) existed but was years
bit-rotted and had never been CMake-built; the legacy `engine/compilers/emscripten`
recipe was a hand-maintained flat source list (it still compiled the dead `spine/*`
tree). This round wires Emscripten into the **shared** `EngineSources.cmake` (the
Android pattern: one list + guards), NOT the legacy list.

### Build (Windows host; works from any host with emsdk)
1. Install the Emscripten SDK once and activate it:
   `git clone https://github.com/emscripten-core/emsdk && cd emsdk && ./emsdk install latest && ./emsdk activate latest`,
   then `source ./emsdk_env.sh` (or `emsdk_env.bat`). Verified with **emsdk 6.0.1**.
   - **Windows/Git-Bash gotcha:** `emsdk_env.sh` shells out to `python`, which on
     Windows hits the Microsoft-Store `python` stub and silently fails to export
     `EMSDK`/PATH. Work around it by driving the tools directly:
     `export EM_CONFIG=/c/Users/<you>/emsdk/.emscripten` and prepend
     `.../emsdk/upstream/emscripten` and `.../emsdk/node/<ver>/bin` to `PATH`. Then
     `emcc`/`emcmake` work.
2. Configure + build (a `make` program must be on PATH — `mingw32-make`, MSYS make,
   or the chocolatey `make` all work):
   `./generate-emscripten.sh` (= `emcmake cmake -S . -B build/emscripten -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug`)
   then `cmake --build build/emscripten -j`. `emcmake` injects the Emscripten
   toolchain and sets the CMake `EMSCRIPTEN` variable, which selects the
   `platformEmscripten` back-end.
3. **Run:** serve over HTTP (NOT `file://`) and open the page:
   `cd build/emscripten && python -m http.server 8000` → `http://localhost:8000/Torque2D_DEBUG.html`.

### How it's wired (root `CMakeLists.txt` + `PlatformSources.cmake`)
- `TORQUE_PLATFORM_SOURCES_EMSCRIPTEN` lists the `platformEmscripten/*.cpp` back-end
  (incl. `EmscriptenGL2ES.cpp`, the fixed-function→GLES immediate-mode shim, which
  the legacy list wrongly omitted). Its `glBegin`/`glEnd`/`glVertex*` are dead code,
  though: they are declared as C++ overloads (`glBegin(GLint)`, not GL's C
  `glBegin(GLenum)`), which nothing calls — `llvm-nm` shows only the mangled
  `_Z7glBegini` and no caller of it. Immediate mode on the web is emscripten's
  `LEGACY_GL_EMULATION`.
- The `EMSCRIPTEN` branch is matched **before** `UNIX` in the platform dispatch (the
  Emscripten toolchain sets `UNIX=1`, exactly like Android).
- **`EMSCRIPTEN=1` is defined GLOBALLY** (before `add_subdirectory(engine/lib)`) — emcc
  only predefines `__EMSCRIPTEN__`, but the engine (`types.gcc.h`, which then defines
  `TORQUE_OS_EMSCRIPTEN`) AND vendored libs (`ljpeg/jconfig.h`) key off bare
  `EMSCRIPTEN`. Without it ljpeg fails with "No jconfig.h was included". The legacy
  recipe did the same via a global `ADD_DEFINITIONS`.
- **Net swap:** the engine list filters out `platformNet.cpp`/`platformNetAsync.cpp`
  and adds `platform/platformNet_Emscripten.cpp` (all stubs — browsers can't open raw
  sockets). `platformNet_ScriptBinding.cc` (the script API) is shared and stays.
- **emcc flags:** `-sUSE_SDL=2` at compile *and* link (Emscripten's SDL 2.32.10 port,
  the version Linux vendors; at compile time it puts the port's headers on the include
  path, so `<SDL.h>` resolves as it does on Linux), `TORQUE_SDL` defined as on Linux,
  `-sLEGACY_GL_EMULATION=1` (fixed-function GL over WebGL 1.0 — works on emsdk 6.0.x;
  it needs the context SDL makes, and can't be combined with `FULL_ES2`/`FULL_ES3`),
  a fixed `-sINITIAL_MEMORY=512MB` with `-sALLOW_MEMORY_GROWTH=0` (a growable heap is a
  resizable ArrayBuffer, which Chrome's `texImage2D` rejects), `-sEXIT_RUNTIME=0`
  `-sFORCE_FILESYSTEM=1`, `-sASSERTIONS=1` in Debug, and
  `--js-library platformEmscripten/platform.js`. See the SDL 2 round below.
- **Assets** are packaged into MEMFS with `--preload-file SRC@/DST` (no host FS to
  lazy-load from in a browser → a `.data` sidecar). `main.cs` + `editor/`, `library/`,
  `toybox/` are bundled at the VFS root (engine cwd is `/`). **`tools/` is deliberately
  NOT bundled** — it's build-time-only (TexturePacker + ~90 MB of generated doxygen
  HTML under `tools/doxygen/output`), which cut the `.data` from 272 MB to ~186 MB.
  (Heads-up: the iOS bundle still ships `tools/` and carries that same dead ~90 MB —
  worth trimming there too.)
- No gtest / no `testing/*` on Emscripten (like Android). zlib is still built from
  source (compiles fine to wasm). Output suffix is `.html` (drives emcc to emit the
  HTML shell); the bundle is kept in `build/emscripten`, NOT the repo root (it's a web
  bundle, not run from cwd).

### Compile/link fixes (the back-end had drifted from the engine interfaces)
All were bit-rot in `platformEmscripten/*`, surfaced once it compiled against current
headers:
- **ljpeg** "No jconfig.h" — the global `EMSCRIPTEN=1` (above).
- `EmscriptenStrings.cpp` — `dStrlen`/`dStrspn`/`dStrcspn` returned `dsize_t`, but
  `platformString.h` (and every other platform) declares them `U32`; on wasm32
  `dsize_t != U32` → "functions differ only in return type". Changed to `U32`.
- `EmscriptenGL.cpp` — dropped a stale `AssertFatal(platState.engine, ...)`
  (`EmscriptenPlatState` has no `engine` member; macOS-derived leftover).
- `EmscriptenOutlineGL.cpp` — fixed include typo `platformEmscriptenplatformGL.h` →
  `platformEmscripten/platformGL.h`.
- `EmscriptenOGLVideo.{h,cpp}` — added the missing `getVerticalSync()` override
  (`DisplayDevice` declares it pure-virtual → the class was abstract → `new
  OpenGLDevice()` failed). Stub mirrors x86UNIX.
- `platformGL.h` — `#include "platform/types.h"` so the `gluProject`/`gluUnProject`
  prototypes' `F64` resolves regardless of include order.
- `platform/platformNet_Emscripten.cpp` — the whole file was guarded
  `#if defined(TORQUE_OS_EMSCRIPTEN)` but `#include "platformNet.h"` (which *defines*
  that macro) was the NEXT line, INSIDE the guard → the file compiled to nothing and
  every `Net::` symbol was undefined at link. Guard now keys on bare `EMSCRIPTEN`
  (a build-level define available before any include). Same chicken-and-egg class as
  the iOS `TORQUE_OS_IOS` predefine.
- `EmscriptenOutlineGL.cpp` — `glArrayElement` (legacy immediate-mode indexed draw)
  is undefined: emscripten's `LEGACY_GL_EMULATION` provides `glBegin/glEnd/glDrawArrays`
  but NOT `glArrayElement`. Added an `extern "C"` no-op (this debug-only wireframe
  overlay is off by default — same graceful-degradation choice as iOS's
  `NO_REDEFINE_GL_FUNCS`).

### Runtime fixes (browser-driven via Playwright + `python -m http.server`)
Debugged headless by driving Chromium at the page, reading the JS console, and
screenshotting. Boot reaches: all subsystems init → **WebGL 1.0 context up**
(`Renderer: WebKit WebGL`, extensions, `Max Texture Size 16384`, screen mode
1024×768) → **EditorCore module loads** → main loop runs. Fixes:
- **Directory scan dropped the first char of each entry** — module registration
  reported scanning `/editor/ssetAdmin` (should be `AssetAdmin`), so editor modules
  never loaded. `EmscriptenFileio.cpp recurseDumpDirectories()` built the child
  `subPath` WITHOUT a leading slash when `basePath` ended in `/`, but the path-assembly
  unconditionally did `&subPath[1]` (assuming a leading slash to strip) → dropped a
  real filename char. Fixed to join with exactly one `/`, only stripping `subPath`'s
  leading slash when it actually has one. (Same FAMILY as the Android leading-slash
  path off-by-one — Emscripten runs from cwd `/` too, but has its own Fileio.)
- **`platform.js` showed pointer addresses, not messages** — `js_AlertOK` etc. did
  `alert(message)` where `message` is the raw wasm heap POINTER (e.g. "363333"), not a
  JS string. Decode with `UTF8ToString()`. Also routed informational `AlertOK` to
  `console.error` instead of a blocking native `alert()`: the engine's assert handler
  calls a Platform alert per failed assert, and a blocking dialog wedges the browser
  tab in an un-dismissable storm (a web game must never block on `alert()`). Decision
  dialogs (OKCancel/Retry/YesNo) keep a real `confirm()` since the engine needs the
  boolean.
- **`GFont::create` hard-crashed on a missing font** — `gFont.cc` did
  `AssertFatal(platFont, ...)` then dereferenced the null `platFont` anyway
  (`getFontHeight()`), which TRAPS the wasm runtime ("memory access out of bounds").
  Its sole caller `GuiControlProfile::addFont()` ALREADY null-checks the return, so
  `GFont::create` now returns a null `Resource<GFont>` on a missing font instead of
  crashing. Cross-platform robustness fix; directly analogous to the open Android
  font-robustness bug (`AndroidFont::create` returning true on failure + unguarded
  deref).

### Fonts round — DONE: the editor renders TEXT on the web (cache-first, no FreeType)
The web build has **no font backend** (`EmscriptenFont::createPlatformFont()` is stubbed),
so it can't synthesize glyphs at runtime. But the engine's `.uft` files are **fully
self-contained** (glyph bitmaps + metrics + texture sheets via `GFont::read`) and load
with no platform font — and the editor already ships ~125 MB of them. So fonts were wired
**cache-first** (the `.uft` already in the preload), no FreeType, no extra download. Result:
the Project Manager UI renders with real text ("TORQUE2D", version, project-tile labels)
plus sprites, stable, no crash. This was also the first end-to-end exercise of the GL
**draw** path on web (text quads + sprite batches under `LEGACY_GL_EMULATION`) — it works.

Fixes (most are cross-platform robustness; the asset-path ones are web-specific):
- **Blocking assert dialogs wedged the tab.** `PlatformAssert::process` showed a native
  `AlertRetry`/`AlertOKCancel` (→ blocking `confirm()`) for every non-Warning assert and
  `forceShutdown(1)` on Cancel — fatal inside the rAF main loop, and a per-frame assert
  produced an un-dismissable dialog storm. On `TORQUE_OS_EMSCRIPTEN` the assert is now
  logged-and-continued (no modal, no shutdown) — the only sane web behavior. This is what
  unblocked boot past the (still-present, non-fatal) `Con::init should only be called once`
  double-init assert.
- **Frame allocator too small.** Emscripten was grouped with Android at 512 KB, but it
  boots the SAME desktop-class editor iOS needed 3 MB for — moved Emscripten to the 3 MB
  branch (`defaultGame.cc`); only Android keeps the small budget.
- **`GuiControlProfile::getFont()` hard-asserted on a missing font** (`guiTypes.cc:768`),
  and text-render sites deref the result. Now it falls back to another loaded size in the
  profile and returns NULL only if the profile has no usable font (cross-platform).
- **Web font selection + cache dir.** `AppCore`/`EditorCore` `SetProfileFont` picked
  "monaco" on web (`$platform=="x86UNIX"`, no `.uft`, no system font). Added a web branch
  (`$platformUnixType=="emscripten"`) → "share tech mono". The base `GuiDefaultProfile`
  also hardcoded a **non-existent** `^EditorCore/gui/fonts` dir (desktop only survived via
  `createPlatformFont`); pointed it at an **expanded** real dir under the registered
  `^EditorCore` expando that actually ships the face — `^EditorCore/Themes/LabCoat/fonts`
  (the resource manager does NOT resolve the `^Module` expando for cache lookups, and
  `^AppCore` isn't even loaded at editor boot, so the path must be pre-`expandPath`'d).
- **`GFont::getTextureHandle(index)` did an unguarded `mTextureSheets[index]`** — an
  out-of-range sheet index returned a garbage `TextureHandle` whose non-NULL `object` was
  then dereferenced by `lock()` → fatal wasm "memory access out of bounds" in `dglDrawText`.
  Bounds-checked to return a NULL handle (lock-safe), so an unrenderable glyph is skipped
  (cross-platform robustness; gFont.h).

**SCRIPT-CHANGE GOTCHA (build):** the `--preload-file` asset trees are NOT tracked as CMake
dependencies (same as `--js-library platform.js`), so editing a `.cs`/asset does NOT trigger
a repackage. After a script edit, force it: `rm build/emscripten/Torque2D_DEBUG.{html,js,wasm,data}`
then rebuild. (TODO: add a CMake custom-command dependency on the preload trees, or a clean
target, so script edits repackage automatically.)

Residual after the cache-first round (since RESOLVED by the FreeType round below):
- The un-baked editor title sizes (`black ops one` 21/28) that logged a non-fatal per-frame
  `Vector out of bounds` now rasterize via FreeType — gone.
- **`Con::init should only be called once`** still fires at boot (console double-init); now
  harmless (logged-and-continued). Root cause not yet chased.

### FreeType round — DONE: a real rasterizer (web behaves like the desktop)
The cache-first round rendered only the pre-baked `.uft` sizes; anything else degraded to a
near size or nothing (Windows' GDI is a universal backstop — the web had none). This round
restores that backstop: **FreeType (the vendored 2.4.12) is compiled to wasm** and
`EmscriptenFont` rasterizes a bundled **Roboto** `.ttf` for any face/size not in the `.uft`
cache. Result: the editor renders ALL its text (the previously-blank "New Project" heading /
body now draw), the per-frame OOB is gone, and web matches the desktop font behavior. `.uft`
is kept (designed faces still use it at baked sizes; FreeType only fills gaps).

- **FreeType static lib** (`engine/lib/CMakeLists.txt`, gated `if(EMSCRIPTEN)` — desktop uses
  system fonts / `find_package(Freetype)`, Android the prebuilt `.a`): the vendored
  `freetype/android/freetype-2.4.12/` tree built from its per-module **aggregator** `.c`
  (`ftbase/ftinit/ftsystem/ftdebug`, `sfnt`, `truetype`, `smooth`, `raster`, `autofit`,
  `psnames/psaux/pshinter`), `FT2_BUILD_LIBRARY` defined, `torque_thirdparty_lib`. Linked in
  the root EMSCRIPTEN block (its PUBLIC include propagates `<ft2build.h>`). +~1 MB wasm.
- **Trimmed `ftmodule.h`** — `ftinit.c` registers every driver listed there, so the default
  full list produced undefined-symbol link errors (`bdf/pcf/t42/winfnt/type1/cff/cid/pfr`).
  Trimmed to the TrueType path we compile (the file's whole purpose is to list built-in
  modules). Edited in the vendored tree — safe, since that source is ONLY consumed by this
  from-source Emscripten build (Android links the prebuilt `.a`).
- **`EmscriptenFont`** (`platformEmscripten/EmscriptenFont.{h,cpp}`) — mirrors `AndroidFont`:
  `FT_Init_FreeType`; `create` resolves the `.ttf` from `$pref::Web::fallbackFont`,
  `FT_New_Face(path)` (the path is a preloaded MEMFS file — FreeType's ANSI stdio reads it),
  `FT_Set_Pixel_Sizes`, metrics from `face->size->metrics`; `getCharInfo` does
  `FT_Load_Char(FT_LOAD_RENDER)` and copies the 8-bit alpha bitmap. Uses the rendered
  **bitmap** dims (`slot->bitmap.width/rows`, stride `bitmap.pitch`) for the CharInfo — keeps
  alloc/copy/size consistent (metrics width can be a pixel narrower → would overrun).
- **Fallback resolution + app/editor separation.** `EmscriptenFont` only gets the face NAME,
  so the `.ttf` path comes from `$pref::Web::fallbackFont`, which each core registers to its
  OWN copy: `library/AppCore/scripts/defaultPreferences.cs` →
  `^AppCore/fonts/Roboto-Regular.ttf` (ships with games; self-contained), and
  `editor/EditorCore/scripts/defaultPreferences.cs` → `^EditorCore/gui/fonts/Roboto-Regular.ttf`
  (overrides while the editor is loaded). Removing `editor/` falls back to AppCore's — the
  editor never reaches into the app. `Roboto-Regular.ttf` (SIL OFL) is bundled in both dirs
  (preloaded into the web `.data`) and in `android-studio/.../assets/fonts/`.
- **Android (wired, NOT tested this round).** `guiProfiles.cs` (both cores) request `"Roboto"`
  instead of the long-gone `"Droid"`, and `Roboto-Regular.ttf` is in `assets/fonts/` so
  `FontManager` resolves it → `AndroidFont` (already FreeType, failure now propagated)
  rasterizes it. No new Android code; APK run deferred.

Residual (minor): the `Con::init` double-init still logs (harmless); a single non-fatal OOB
remains at boot (down from thousands). FOLLOW-UPS: per-face `.ttf` resolution
(`<dir>/<face>.ttf` before the generic fallback) for exact typography without baking `.uft`;
then drop most `.uft` for a much smaller web download; and run the Android APK to confirm text.

### Interaction round — DONE: navigate the editor, open toys, type in the console
With text rendering, the next step was making the UI actually usable (find/open a project,
keyboard input). Four fixes:
- **Project selector found no projects** (only the "New Project" placeholder). `Platform::
  dumpDirectories` started recursion at `currentDepth 0`, but the child-recursion guard is
  `currentDepth < recurseDepth`, so the common `getDirectoryList()` (depth 0) call evaluated
  `0 < 0` == false and descended into NO children → empty list. The editor enumerates
  `getMainDotCsDir()` to find project folders, so the Toy Box (the only default project) never
  listed. Fixed by starting recursion at `-1`, matching Win32/x86UNIX (the SAME fix was made
  to x86UNIX in the Linux round but never propagated here). `EmscriptenFileio.cpp`.
- **Typing crashed the tab (hard wasm trap).** `_StringTable::hashString/hashStringn` indexed
  the 256-entry hash table with a signed `char`; any byte ≥ 0x80 → negative index → read
  before the array. Harmless wrong-hash on desktop, a hard "memory access out of bounds" on
  wasm. Reproduced by pressing Ctrl with a text field focused (the bogus high ascii got
  inserted, then hashed on `StringTable::insert`). Cast the index to `(U8)` in both hashers
  (`string/stringTable.cc`) — a latent CROSS-PLATFORM bug; fixes all high-bit/accented input.
- **Phantom glyphs from modifier keys.** `EmscriptenInputManager::MapKey` assigned the raw SDL
  keysym as each key's `ascii`, so modifiers/function/arrow/keypad keys all carried a bogus
  non-zero ascii and got inserted as (unrenderable) characters. Desktop x86UNIX avoids this by
  reading the X keymap (which yields nothing for a non-character key), but a browser canvas has
  no keymap to read. Filtered the default assignment: only printable ASCII (0x20-0x7E)
  carries a character ascii; SDL specials (≥0x100), 0x7F-0xFF, and control keys (<0x20) map to
  0 and stay handled by keycode (`EmscriptenInputManager.cpp`).
- **Event-list re-entrancy OOB.** `ProcessMessages()` cached the size of the shared
  `gPlatState.eventList` then indexed it in the loop, but handling `SDL_USEREVENT`
  (SETVIDEOMODE) → `SetAppState` → `Input::reactivate` re-enters `ProcessMessages` and
  clears+refills that same list → the outer loop read past the now-smaller vector (the two red
  `vector.h:578` fatals on every toybox load). Iterate a LOCAL copy of the frame's events so
  re-entrancy can't corrupt iteration (`EmscriptenWindow.cpp`). Root-caused with a temporary
  `emscripten_log(EM_LOG_C_STACK)` in the assert path.

*Since the SDL 2 round (below), the last two are moot: the SDL keysym table, its ASCII filter,
the copied event list and the `SDL_USEREVENT` that re-entered it are gone. Keys arrive by
scancode and characters from `SDL_TEXTINPUT`, through `platformSDL`, and SDL's queue is
emptied in one loop.*

### Blending / immediate-mode round — DONE: toys render correctly (e.g. PyramidToy light)
First exercise of the toys' raw `glBegin`/`glEnd` draw path on the web (the editor uses the
batched array path; toys reach further into legacy GL). The PyramidToy `LightObject` rendered
as an opaque dark "umbrella" fading to BLACK instead of a soft light fading OUT.

- **Root cause — GL state changed *inside* glBegin/glEnd.** `LightObject::sceneRender`
  (`2d/sceneobject/LightObject.cc`) called `glDisable(GL_BLEND)` before `glEnd()`. On desktop
  GL that call is illegal between glBegin/glEnd (`GL_INVALID_OPERATION`) and is silently
  IGNORED, so the fan still draws with the additive blend it set up — fades out correctly.
  But the web's immediate mode (emscripten's `LEGACY_GL_EMULATION`; `EmscriptenGL2ES.cpp`'s
  own `glBegin`/`glEnd` turned out to be uncalled C++ overloads, see above) only BUFFERS the
  vertices and defers the real draw to `glEnd()`. So the `glDisable(GL_BLEND)` runs
  immediately and the deferred draw happens with blending OFF → opaque fan whose per-vertex
  colors fade to black. Fixed by moving the disable AFTER `glEnd()`.
- **General rule (web immediate mode):** only emit vertex/color/texcoord between glBegin/glEnd;
  do every enable/disable/blendfunc OUTSIDE the block. A sweep of `engine/source/2d` found
  `LightObject` was the only offender — `DebugDraw.cc` already disables blend after `glEnd()`,
  and BatchRender/SceneWindow/SceneObject use direct vertex arrays (no deferral). Watch for
  this when porting any other legacy-GL toy/sample to the web.

### SDL 2 round — DONE: the web on SDL 2, sharing its input code with Linux
The back-end left Emscripten's SDL 1.2 port (a JavaScript reimplementation of 1.2) for its
SDL 2 port — 2.32.10, the version the Linux build vendors — and for `engine/source/platformSDL`,
the keyboard and text code the Linux port took from Torque3D. Torque3D has no web target, so
that shared code is the reuse.

- **Build.** `-sUSE_SDL=2` at compile and link, `TORQUE_SDL` defined. Both back-ends include
  `<SDL.h>`: compiling with `-sUSE_SDL=2` adds `-isystem <sysroot>/include/SDL2`, so no include
  directory of our own is needed (`<SDL2/SDL.h>` resolves too, but the vendored Linux SDL only
  offers `<SDL.h>`). The Emscripten list gains `platformSDL/sdlInput.cpp` and
  `sdlTextInput.cpp`, not `sdlMsgBox.cpp` (below); `EmscriptenEvents.cpp`, which held only the
  keyboard-translation stubs `sdlTextInput.cpp` now provides, is deleted.
- **One canvas, one context** (`CreateGLWindow`, `EmscriptenOGLVideo.cpp`):
  `SDL_CreateWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)`, `SDL_GL_CreateContext`,
  `SDL_GL_MakeCurrent`, once. The context must come from SDL: its driver makes it through
  Emscripten's EGL, which is what starts `LEGACY_GL_EMULATION`'s immediate mode (the phase-0
  prototype showed this); `emscripten_webgl_create_context` skips that. No version is asked
  for, so it is WebGL 1 (GLES 2.0), now RGB 8/8/8 where SDL 1.2 reported 3/3/2. Mode changes
  resize around the context, so the texture kill/resurrect on every mode change is gone.
- **Canvas size.** If the page sizes `#canvas` with CSS (tested the way SDL tests it, before the
  window exists), the page decides: SDL resizes the drawing buffer on every browser resize and
  sends `SDL_WINDOWEVENT_SIZE_CHANGED`, which the event loop hands to
  `OpenGLDevice::followWindow` → `Platform::setWindowSize` and `$pref::Video::resolution`, and
  `setScreenMode` leaves the size alone (it would only change the buffer, which the page then
  stretches). Otherwise — emcc's default shell — the canvas takes the game's size,
  `$pref::Video::windowedRes` or else `defaultResolution`, as on Linux. Not
  `SDL_WINDOW_ALLOW_HIGHDPI`: the buffer would outgrow the page size by the pixel ratio while
  the GUI and the mouse stay in page pixels. The desktop is SDL's one display mode,
  `screen.width × screen.height` (it was a made-up 1024×768 in `platform.js`), and no size is
  refused any more.
- **Fullscreen** is `SDL_WINDOW_FULLSCREEN_DESKTOP`. The browser grants it only in answer to a
  click or a key, so the request waits for the next one, and `$pref::Video::fullScreen` is not
  applied at page load. Leaving with Esc is followed.
- **Event loop.** One `SDL_PollEvent` loop a frame in `EmscriptenWindow.cpp`, as on Linux: window
  events there, keys, text, mouse and wheel to `UInputManager::processEvent`. Gone: the copied
  `eventList` and its re-entrancy workaround, `SDL_USEREVENT`/`TORQUE_SETVIDEOMODE`,
  `SDL_ACTIVEEVENT`/`SDL_GetAppState`, and the unused `NumEventsPending`,
  `AlertDisable/EnableVideo`, `SendQuitEvent` and `DisplayErrorAlert`.
- **Input.** `EmscriptenInputManager.cpp` is now the Linux input manager with `gPlatState` for
  `x86UNIXState`: keys by scancode through platformSDL's table (SDL reads `KeyboardEvent.code`),
  characters from `SDL_TEXTINPUT` (text input is on only while a text field has the keyboard,
  which is also when SDL lets a keydown through so that the browser sends the keypress SDL reads
  the character from), modifiers per side, the side buttons, and the wheel as `SDL_MOUSEWHEEL` —
  it was commented out. SDL counts 100 px or 3 lines of a browser wheel event as a notch and adds
  a trackpad's small movements up. `Input::getAscii`/`getKeyCode` answer for a US layout, since a
  browser does not tell a page its layout.
- **Focus.** SDL 2 reports the page gaining and losing the keyboard, which SDL 1.2 never did.
  That woke two old sleeps — `Platform::process` and `TimeManager::process` waited out
  `Pref::backgroundSleepTime` (200 ms) while backgrounded, and a sleep in a page is a busy wait —
  so both are gone; the browser slows a hidden page by itself.
- **Pointer.** `SDL_ShowCursor(SDL_DISABLE)` now really hides the pointer (CSS `cursor: none` on
  the canvas). The input manager no longer hides it on activate, as the desktop ones do;
  `Input::setCursorState` — the canvas's choice between drawing its own cursor and the native
  one — does, so the pointer is hidden exactly while the canvas draws one. `Input::setCursorPos`
  is a no-op (a page cannot move the pointer; SDL's `WarpMouse` is unsupported there), and a
  locked mouse is pointer lock, granted at the next click.
- **Clipboard** is SDL's. Its Emscripten driver has no system clipboard, so copy and paste work
  within the engine but not with other programs; before, the clipboard was always empty.
- **Alerts stay in `platform.js`.** SDL 2 has no `SDL_ShowMessageBox` in a browser ("No message
  system available"), only `SDL_ShowSimpleMessageBox` as an `alert()`, so `sdlMsgBox.cpp` would
  print every alert and answer every question no. `AlertOK` still goes to the console and the
  others to `confirm()`.
- **Removed as no-ops:** the gamma ramp (get/set return false), iconify, and the accumulation
  buffer attributes. Vertical sync is left alone: on the web it is the main loop's timing
  (`emscripten_set_main_loop_timing`), which cannot be set before `main.cpp` starts the loop, so
  `suppSwapInterval` is false and the loop keeps its 60 Hz timer.
- **Sizes** (emsdk 6.0.9; the `.data` is 137,207,314 bytes either way):

  | Build | wasm | wasm gzipped | js |
  |-------|------|--------------|----|
  | Debug, SDL 1.2 | 28,327,627 | 9,264,967 | 717,600 |
  | Debug, SDL 2 | 30,713,305 | 10,095,514 | 825,270 |
  | Release, SDL 1.2 | 3,161,157 | 1,150,249 | 395,271 |
  | Release, SDL 2 | 3,527,632 | 1,253,607 | 462,122 |

  In Release, SDL 2 costs 366 KB of wasm (103 KB gzipped) and 67 KB of JavaScript — about
  what the phase-0 prototype measured for the port alone.

- **Verified headless** (Chromium 151 on SwiftShader, driven over the DevTools protocol with real
  key, mouse and wheel events). The editor boots to the Project Manager in emcc's shell and in a
  page whose CSS fills the window, logging nothing SDL 1.2 did not already log (`Con::init`
  twice, two `AssetWindow.cs` script errors, emscripten's rAF and GL-emulation warnings); SDL
  1.2's four joystick errors are gone. Typed into the editor console,
  `echo("ascii-typed-OK " @ 6 * 7);` prints `ascii-typed-OK 42`, and `café naïve` arrives
  intact (SDL 1.2 typed `caf nave`). The wheel over a `GuiScrollCtrl` scrolls it 0 → 90 with
  three notches down, back to 60 with one up, and one notch further with four 30 px
  trackpad-sized movements. Resizing the browser window from 1280×813 to 1000×613 and then
  1440×873 brings the canvas's buffer and the GUI along. The Release build boots to the same
  Project Manager.

## Cross-cutting notes

- Single-config generators (Make/Ninja) use `-DCMAKE_BUILD_TYPE=`; multi-config
  (VS/Xcode) use `--config`. The root handles both, incl. the `Shipping` config
  (Release flags + `TORQUE_SHIPPING`) and per-config exe names (`Torque2D_DEBUG`).
- `bitmapPvr.cc` (PVR/mobile) is excluded on all desktop platforms — correct.
- Windows-only details (static `/MT`, `/Zc:wchar_t-`, `_HAS_STD_BYTE=0`) are all
  guarded by `if(MSVC)` and won't affect mac/linux.
- **Coordination:** the root `CMakeLists.txt`, `PlatformSources.cmake`, and
  `engine/lib/CMakeLists.txt` are shared. Do macOS and Linux on **separate
  branches off `cmake-do-over`** (or one at a time) to avoid merge conflicts.
