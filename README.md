## Current release for Windows: [WIN32 Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-win.zip)
## Current release for MacOS: [MacOS Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-macos.zip)
## Current release for Linux: [Linux Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-linux64.tar.gz)
You don't need the sources, unless you wish to compile GBDK-2020 yourself. Download the current release binaries using the links above.

# gbdk-2020
[GBDK](http://gbdk.sourceforge.net/) A C compiler, assembler, linker and set of libraries for the Z80 like Nintendo Gameboy.

# Current status
- updated CRT and library that suits better for game development
- the latest version of **sdcc** (the compiler and toolchain) is used from [sourceforge](http://sdcc.sourceforge.net)
- **lcc** (the retargetable compiler) supports the latest version of sdcc compiler and toolchain. 

For full list of changes see the [ChangeLog](https://github.com/Zal0/gbdk-2020/blob/master/gbdk-support/ChangeLog) file

# Origin

Over the years people have been complaining about all the issues caused by a very old version of SDCC (the compiler). This is a proper attempt of updating it while also keeping all the old functionallity working, like support for banked code and data and so on

The last version in the OLD repo is [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) although releases are available until 2.95-3. Version [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) is the starting point of this repo

# Usage
Most users will only need to download and unzip the latest [release](https://github.com/Zal0/gbdk-2020/releases)

Then go to the examples folder and build them (with make.bat on windows or running make). They are a good starting point.

# Docs
You can still refer to the documentation of GBDK [html](http://gbdk.sourceforge.net/doc/html/book01.html) or [pdf](http://gbdk.sourceforge.net/doc/gbdk-doc.pdf)

The original [website](http://gbdk.sourceforge.net/) also contains a lot of imformation

For SDCC you can check its [website](http://sdcc.sourceforge.net/) and the [manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)

[The Game Boy Development Forum](https://gbdev.gg8.se/forums/) is a good place to search for answers. 

There is also a #gbdk channel in the [gbdev Discord community](https://discord.gg/gpBxq85)

# Build instructions
Unless you are interested on recompiling the sources for some reason (like fixing some bugs) **you don't need to build GBDK**

- **Windows only**: Download and install [mingw](http://www.mingw.org/)
- Clone, download this repo or just get the source form the [releases](https://github.com/Zal0/gbdk-2020/releases)
- Download and install [**sdcc**](http://sdcc.sourceforge.net/) (SDCC is no longer part of GDDK so you need to download it (just the binaries) in the platform you need)
- Create and environment var **SDCCDIR** pointint to the folder where you installed sdcc
- Open a command prompt or a terminal, go the root directory of the repo and run **make**
