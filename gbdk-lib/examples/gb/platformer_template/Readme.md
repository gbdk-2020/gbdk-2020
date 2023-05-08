# GBDK-2020 Platformer Template

A Basic "Mario-Style" Platformer Template

Mario can walk, run, and jump on the background of 3 different areas. Mario is rendered using metasprites.

Controls:
  - D-pad left/right: Walk Horizontally
  - D-pad up:         Jump (hold to increase jump height)
  - A:                Jump (hold to increase jump height)
  - B:                Sprint

The "res" folder contains some PNGs that are converted to .c and .h files using [png2asset](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_toolchain_settings.html#png2asset-settings).

The level.c file defines a function for changing the level. The code expects all non-solid tiles be first in the tilesets. The amount of non-solid tiles should be specified in "currentLevelNonSolidTileCount" when changing level.