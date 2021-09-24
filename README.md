## Current release for Windows: [WINDOWS Binaries](https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-win.zip)
## Current release for MacOS: [MacOS Binaries](https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-macos.zip)
## Current release for Linux: [Linux Binaries](https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-linux64.tar.gz)
You don't need the sources, unless you wish to compile GBDK-2020 yourself. Download the current release binaries using the links above.

Upgrading to a new version? Check the [Migration notes](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_migrating_versions.html).


# Docs
Online documentation is avaliable [HERE](https://gbdk-2020.github.io/gbdk-2020/docs/api)

A good place to start is the [Getting Started Section](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_getting_started.html).

Check the [Links and Third-Party Tools Section](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_links_and_tools.html) for a list of recommended emulators, graphics tools, music drivers and more.

# gbdk-2020
GBDK is a cross-platform development kit for sm83 and z80 based consoles. It includes libraries, toolchain utilities and the [SDCC](http://sdcc.sourceforge.net/) C compiler suite.

__Supported Consoles:__
- Nintendo Gameboy
- Analogue Pocket
- Sega Master System & Game Gear

# Current status
- Updated CRT and library that suits better for game development
- SDCC Versions
  - GBDK-2020 4.0.5 requires SDCC patches for z80 SMS/Game Gear support. So SDCC nightlies/snapshot builds cannot be used if you want to target SMS/Game Gear. Instead use the [Patched SDCC Builds](https://github.com/gbdk-2020/gbdk-2020-sdcc/releases/tag/sdcc-12539-patched). 
  - In addition, recent SDCC nightlies/snapshot builds have switched to a new calling convention which is not well tested with GBDK-2020 and there may be bugs. It's recommended to not use versions after build #12539 at this time.
- The compiler driver **lcc** supports the latest sdcc toolchain.

For full list of changes see the [ChangeLog](https://github.com/gbdk-2020/gbdk-2020/blob/master/gbdk-support/ChangeLog) file

# Origin

Over the years people have been complaining about all the issues caused by a very old version of SDCC (the compiler). This is a proper attempt of updating it while also keeping all the old functionallity working, like support for banked code and data and so on

The last version in the OLD repo is [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) although releases are available until 2.95-3. Version [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) is the starting point of this repo

# Usage
Most users will only need to download and unzip the latest [release](https://github.com/gbdk-2020/gbdk-2020/releases)

Then go to the examples folder and build them (with compile.bat on windows or running make). They are a good starting point.

## Discord servers
* [gbdk/zgb Discord](https://discord.gg/XCbjCvqnUY) - For help with using GBDK (and ZGB), discussion and development of gbdk-2020

* [gbdev Discord](https://discordapp.com/invite/tKGMPNr) - There is a #gbdk channel and also people with a lot of Game Boy development knowledge
* [SMS Power! Discord](https://discord.gg/h5xrKUK) - Additional SMS & Game Gear discussion and resources.

## Forums
- [The Game Boy Development Forum](https://gbdev.gg8.se/forums/) - A good place to search for Game Boy related answers. 
- [SMS Power! Forum](https://www.smspower.org/) - Additional SMS & Game Gear discussion and resources.

For SDCC you can check its [website](http://sdcc.sourceforge.net/) and the [manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)

# Build instructions
Unless you are interested on recompiling the sources for some reason (like fixing some bugs) **you don't need to build GBDK**

- **Windows only**: Download and install [mingw](http://mingw-w64.org/)
- Clone, download this repo or just get the source form the [releases](https://github.com/gbdk-2020/gbdk-2020/releases)
- Download and install the **PATCHED** [sdcc builds](https://github.com/gbdk-2020/gbdk-2020-sdcc/releases/tag/sdcc-12539-patched) from the separate repo for that (https://github.com/gbdk-2020/gbdk-2020-sdcc/releases/tag/sdcc-12539-patched).
- On Linux **don't use package managers** The latest release available won't work, you need to compile or download one of the nightlies
- Create **SDCCDIR** environment variable, that points into the folder, where you installed sdcc
- Open command prompt or a terminal, go to the root directory of the repo and run **make**
