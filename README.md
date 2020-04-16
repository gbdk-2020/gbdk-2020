# gbdk-2020
Over the years people have been complaining about all the issues caused by a very old version of SDCC (the compiler). This is a proper attempt of updating it while also keeping all the old functionallity working, like banks support

The last version in the old repo is [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) although releases are available until 2.95-3.

Version [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) is the starting point of this repo

# Current status
Although **sdcc** already includes a library supporting Game Boy development, the linker is broken and does not support banks (more info [here](http://zalods.blogspot.com/2017/07/bitbitjam-4-velcro-sheep.html)). Years ago it was discovered that it is still posible to link with the old linker **link-gbz80** while also updating **sdcc**. Here is what has been updated
- **sdcc** (the compiler), fully removed the old one and using the latest version from [sourceforge](http://sdcc.sourceforge.net)
- **sdccp** (the preprocessor) also from the same repo
- **sdasgb** (the assembler) has also been updated but the old one as-gbz80 is still used to compile the sources because it fixes some issues caused by .org directive in drawing.s
- **link-gbz80** (the linker) is kept as it was, because it works and has banking support
- **lcc** (the retargetable compiler) to support the new versions of sdcc, sdcpp and sdasgb 

# Usage
Download and unzip the latest [release](https://github.com/Zal0/gbdk-2020/releases)

You can still refer to the documentation of GBDK [html](http://gbdk.sourceforge.net/doc/html/book01.html) or [pdf](http://gbdk.sourceforge.net/doc/gbdk-doc.pdf)

The original [website](http://gbdk.sourceforge.net/) also contains a lot of imformation

For SDCC you can check its [website](http://sdcc.sourceforge.net/) and the [manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)

# Build instructions
SDCC is no longer part of GDDK so you need to download it (just the binaries) in the platform you need
## Windows
- Download and install [mingw](http://www.mingw.org/)

## Windows, Linux, Mac OS
- Clone or download this repo
- Download and install [sdcc](http://sdcc.sourceforge.net/)
- Create and environment var SDCCDIR pointint to the folder where you installed sdcc
- Open a command prompt or a terminal, go the root directory of the repo and run make

