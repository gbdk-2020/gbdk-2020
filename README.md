# GBDK-2020
GBDK is a cross-platform development kit for sm83 and z80 based gaming consoles. It includes libraries, toolchain utilities and the [SDCC](http://sdcc.sourceforge.net/) C compiler suite.

__Supported Consoles:__ [(see docs)](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_supported_consoles.html)
- Nintendo Gameboy / Game Boy Color
- Analogue Pocket
- Sega Master System & Game Gear
- Mega Duck / Cougar Boy

Expirimental consoles (not yet fully functional)
  - MSXDOS
  - NES


## Current Release

[![Current Release](https://img.shields.io/github/v/release/gbdk-2020/gbdk-2020?label=Current%20Release)](https://github.com/gbdk-2020/gbdk-2020/releases/latest)
![Downloads](https://img.shields.io/github/downloads/gbdk-2020/gbdk-2020/total?label=Total%20Downloads)

<a href="https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-linux64.tar.gz"><img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="GBDK-2020 Linux Release"></a>
<a href="https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-macos.tar.gz"><img src="https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=apple&logoColor=white" alt="GBDK-2020 MacOS Release"></a>
<a href="https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-win64.zip"><img src="https://img.shields.io/badge/Windows%2064%20Bit-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="GBDK-2020 Windows 64 Bit Release"></a> 
<a href="https://github.com/gbdk-2020/gbdk-2020/releases/latest/download/gbdk-win32.zip"><img src="https://img.shields.io/badge/32%20Bit-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="GBDK-2020 Windows 32 Bit"></a> 

<!-- <a href="https://hub.docker.com"><img src="https://img.shields.io/badge/Docker-2CA5E0?style=for-the-badge&logo=docker&logoColor=white" alt="Docker"></a> -->

Upgrading to a new version? Check the [Migration notes](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_migrating_versions.html). You can find older versions [here](https://github.com/gbdk-2020/gbdk-2020/releases).

For a full list of changes see the [ChangeLog](https://github.com/gbdk-2020/gbdk-2020/blob/master/docs/ChangeLog) file or [online Docs](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_releases.html).


## Build status
[![GBDK Build and Package](https://github.com/gbdk-2020/gbdk-2020/actions/workflows/gbdk_build_and_package.yml/badge.svg?branch=develop)](https://github.com/gbdk-2020/gbdk-2020/actions/workflows/gbdk_build_and_package.yml)


## Docs
GBDK includes extensive [documentation](https://gbdk-2020.github.io/gbdk-2020/docs/api). A good place to begin is the [Getting Started Section](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_getting_started.html).

Check the [Links and Third-Party Tools Section](https://gbdk-2020.github.io/gbdk-2020/docs/api/docs_links_and_tools.html) for a list of recommended emulators, graphics tools, music drivers and more.

For SDCC you can check its [website](http://sdcc.sourceforge.net/) and the [manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)


## Usage
Most users will only need to download and unzip the latest [release](https://github.com/gbdk-2020/gbdk-2020/releases)

Then go to the examples folder and build them (with `compile.bat` on Windows or running `make`). They are a good starting point.

The sources in this repo are only needed if you want to re-compile GBDK-2020 yourself instead of using the release binaries linked above.


## Discord servers
* [gbdk/zgb Discord](https://discord.gg/XCbjCvqnUY) - For help with using GBDK (and ZGB), discussion and development of gbdk-2020

* [gbdev Discord](https://discordapp.com/invite/tKGMPNr) - There is a #gbdk channel and also people with a lot of Game Boy development knowledge
* [SMS Power! Discord](https://discord.gg/h5xrKUK) - Additional SMS & Game Gear discussion and resources.

## Forums
- [The Game Boy Development Forum](https://gbdev.gg8.se/forums/) - A good place to search for Game Boy related answers. 
- [SMS Power! Forum](https://www.smspower.org/) - Additional SMS & Game Gear discussion and resources.


## Current status
- Updated CRT and library that suits better for game development
- SDCC Versions
  - Current GBDK-2020 versions require SDCC patches for z80 SMS/Game Gear support (only `sdldz80` in GBDK-2020 4.1.0). So SDCC nightlies/snapshot builds cannot be used if you want to target SMS/Game Gear. Instead use the [Patched SDCC Builds](https://github.com/gbdk-2020/gbdk-2020-sdcc/releases). 
  - The default calling convention changed in `SDCC 4.2`. This is supported starting with `GBDK-2020 4.1.0`. Older versions of GBDK should use SDCC builds #12539 or older (see per-version GBDK notes).
- The compiler driver **lcc** supports the latest sdcc toolchain.


## Origin
Over the years people have been complaining about all the GBDK issues caused by a very old version of SDCC (the compiler). This is a proper attempt of updating it while also keeping all the old functionallity working, like support for banked code and data and so on.

The last version in the OLD repo is [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) although releases are available until 2.95-3. Version [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) is the starting point of this repo.


# Build instructions
Unless you are interested on recompiling the sources for some reason (like fixing some bugs) **you don't need to build GBDK**

- **Windows only**: Download and install [mingw](http://mingw-w64.org/)
- Clone, download this repo or just get the source form the [releases](https://github.com/gbdk-2020/gbdk-2020/releases)
- Download and install the **PATCHED** [sdcc builds](https://github.com/gbdk-2020/gbdk-2020-sdcc/releases) from the separate repo for that (https://github.com/gbdk-2020/gbdk-2020-sdcc/releases).
- On Linux **don't use package managers** The latest release available won't work, you need to compile or download one of the nightlies
- Create **SDCCDIR** environment variable, that points into the folder, where you installed sdcc
- Open command prompt or a terminal, go to the root directory of the repo and run **make**
