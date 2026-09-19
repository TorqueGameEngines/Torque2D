# SDL 2 in Torque2D

This is SDL 2.32.10, copied from Torque3D's vendored tree
(`Engine/lib/sdl` in TorqueGameEngines/Torque3D, `development` at `d3d253d1`)
so that the two engines build the same SDL. The Emscripten port that the Web
build uses (`-sUSE_SDL=2`) is also 2.32.10; keep the three in step.

The tree is Torque3D's, byte for byte, except that `test/` has been left out:
it is 51 MB of SDL's own test programs, which the engine never builds
(`SDL_TEST` is off).

To upgrade: replace this directory with Torque3D's `Engine/lib/sdl`, delete
`test/`, keep this file, and update the version above. SDL is zlib-licensed;
see `LICENSE.txt`.
