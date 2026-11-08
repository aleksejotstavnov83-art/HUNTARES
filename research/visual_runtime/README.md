# Screen geometry reference from supplied dumpzenin.zip

The original `screen_geometry.hpp/.cpp` contains `skeleton_bounds` and projection validation for 16 bone positions. It is kept as an isolated reference and is **not linked** into the executable: the game-specific data layout and live bone addresses have not been confirmed for Standoff 2 1.1.0. In the current ESP the box is still an estimated envelope, not per-bone tight bounds.
