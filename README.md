## Current release for Windows: [WINDOWS Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-win.zip)
## Current release for MacOS: [MacOS Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-macos.zip)
## Current release for Linux: [Linux Binaries](https://github.com/Zal0/gbdk-2020/releases/latest/download/gbdk-linux64.tar.gz)
You don't need the sources, unless you wish to compile GBDK-2020 yourself. Download the current release binaries using the links above.

# gbdk-2020
[GBDK](http://gbdk.sourceforge.net/) A C compiler, assembler, linker and set of libraries for the Z80 like Nintendo Gameboy.

# Current status
- updated CRT and library that suits better for game development
- the latest nightlies of **sdcc** (the compiler and toolchain) are used from [sourceforge](http://sdcc.sourceforge.net). At the moment of writing this the last stable version is 4.0 which has linker issues and won't work if you want to use banks. Please use one of the nightlies available [here](http://sdcc.sourceforge.net/snap.php) (we used 4.0.7 #12016)
- The compiler driver **lcc** supports the latest sdcc toolchain.

For full list of changes see the [ChangeLog](https://github.com/Zal0/gbdk-2020/blob/master/gbdk-support/ChangeLog) file

# Origin

Over the years people have been complaining about all the issues caused by a very old version of SDCC (the compiler). This is a proper attempt of updating it while also keeping all the old functionallity working, like support for banked code and data and so on

The last version in the OLD repo is [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) although releases are available until 2.95-3. Version [2.96](https://sourceforge.net/projects/gbdk/files/gbdk/2.96/) is the starting point of this repo

# Usage
Most users will only need to download and unzip the latest [release](https://github.com/Zal0/gbdk-2020/releases)

Then go to the examples folder and build them (with make.bat on windows or running make). They are a good starting point.

# Docs
Online documentation is avaliable [HERE](https://zal0.github.io/gbdk-2020/)

## Discord servers
* [gbdk/zgb Discord](https://discord.gg/XCbjCvqnUY) - For help with using GBDK (and ZGB), discussion and development of gbdk-2020
* [gbdev Discord](https://discordapp.com/invite/tKGMPNr) - There is a #gbdk channel and also people with a lot of Game Boy development knowledge

For SDCC you can check its [website](http://sdcc.sourceforge.net/) and the [manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)

[The Game Boy Development Forum](https://gbdev.gg8.se/forums/) is a good place to search for answers. 



# Build instructions
Unless you are interested on recompiling the sources for some reason (like fixing some bugs) **you don't need to build GBDK**

- **Windows only**: Download and install [mingw](http://www.mingw.org/)
- Clone, download this repo or just get the source form the [releases](https://github.com/Zal0/gbdk-2020/releases)
- Download and install [**sdcc nightlies from 4.0.7 #12016 onwards**](http://sdcc.sourceforge.net/snap.php) (SDCC is no longer part of GDDK so you need to download it (just the binaries) in the platform you need)
- On Linux **don't use package managers** The latest release available won't work, you need to compile or download one of the nightlies
- Create and environment var **SDCCDIR** pointint to the folder where you installed sdcc
- Open a command prompt or a terminal, go the root directory of the repo and run **make**
