This project shows how assets can be managed for multiple different console targets together.

`png2asset` is used to generate assets in the native format for each console at compile-time from separate source PNG images. The Makefile is set to use the source PNG folder which matches the current console being compiled, and the source code uses @ref set_native_tile_data() to load the assets tiles in native format.

Arguments for png2asset are stored in .meta files next to the PNG files of the same name. This allows parameters to be set based on the individual characteristics of the image, and is useful when handling many images (versus defining parameters for each image in the Makefile or elsewhere). Arguments for png2asset can be found in this repository's documentation. In particular, note that -noflip is required for DMG-based systems, and is the default for DMG and CGB examples here, but is less space efficient.

Users who wish to build their own logo ROM should:
* Replace the image files in the res directories, preserving the filename GBDK_2020_logo.png
* Set the PROJECTNAME and TITLE in the Makefile
* Run "make" within the logos directory (use "make clean" before trying again)
* Find the resulting roms in the build directory