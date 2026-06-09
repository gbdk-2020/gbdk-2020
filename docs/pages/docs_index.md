@mainpage General Documentation

@anchor docs_index
- @subpage docs_getting_started
- @subpage docs_links_and_tools
- @subpage docs_using_gbdk
- @subpage docs_coding_guidelines
- @subpage docs_rombanking_mbcs
- @subpage docs_supported_consoles
- @subpage docs_toolchain
- @subpage docs_example_programs
- @subpage docs_faq
- @subpage docs_migrating_versions
- @subpage docs_releases
- @subpage docs_toolchain_settings



# Introduction
Welcome to GBDK-2020! The best thing to do is head over to the @ref docs_getting_started "Getting Started" section to get up and running.

If you are upgrading please check @ref docs_releases and @ref docs_migrating_versions


# About the Documentation
This documentation is partially based on material written by the original GBDK authors in 1999 and updated for GBDK-2020. The API docs are automatically generated from the C header files using Doxygen.

GBDK-2020 is an updated version of the original GBDK with a modernized SDCC toolchain and many API improvements and fixes. It can be found at: https://github.com/gbdk-2020/gbdk-2020/.

The original GBDK sources, documentation and website are at: http://gbdk.sourceforge.net/


# About GBDK
The GameBoy Developer's Kit (GBDK, GBDK-2020) is used to develop games and programs for the Nintendo Game Boy (and some other consoles) in C and assembly. GBDK includes a set of libraries for the most common requirements and generates image files for use with a real GameBoy or emulators.

GBDK features:
 - C and ASM toolchain based on SDCC with some support utilities
 - A set of libraries with source code
 - Example programs in ASM and in C
 - Support for multiple ROM bank images and auto-banking
 - Support for multiple consoles: Game Boy, Analogue Pocket, Mega Duck, Master System and Game Gear and NES

GBDK is freeware. Most of the tooling code is under the GPL. The runtime libraries should be under the LGPL. Please consider mentioning GBDK in the credits of projects made with it.

- - - - - -

# Historical Info and Links

## History
Some notable events and releases in GBDK's history.

Classic GBDK
- 1996-06: `GBDK 1.0`: Pascal Felber releases the first GBDK version
- 1999-06: `GBDK 2.1`: Pascal Felber's last personal release
- 2000-02: `GBDK 2.9.0`: Michael Hope switches GBDK to the SDCC compiler after adding Game Boy support to it
- 2001-11: `GBDK 2.9.6`: The last release from this era

GBDK-2020
- 2020-04: `GBDK 3.0.1`: Zal0 revives GBDK and releases a new version. It updates SDCC to a modern build along with other improvements
- 2020-05: Toxa joins as a project contributor
- 2020-07: `GBDK 4.0.0`: Basxto contributes linker patches to SDCC adding Game Boy banking support. GBDK-2020 changes to use the SDCC linker
- 2020-11: bbbbbr joins as a project contributor
- 2021-09: `GBDK 4.0.5`: Toxa adds support for the Sega Master System, Game Gear and Analogue Pocket consoles
- 2022-02: `GBDK 4.0.6`: Toxa adds support for the Mega Duck console
- 2022-06: Michel joins as a project contributor
- 2023-08: `GBDK 4.2.0`: Michel adds support for the NES console


## Original GBDK (pre-2020) was by:

Pascal Felber, Lars Malmborg, Michael Hope, David Galloway (djmips), John Fuge, and others.


The following is from the classic GBDK documentation:

Thanks to quang for many of the comments to the gb functions.  Some
of the comments are ripped directly from the Linux Programmers
manual, and some directly from the pan/k00Pa document.

<a href="http://quangdx.com/">quangDX.com</a>

<a href="http://gbdk.sourceforge.net/">The (classic) gbdk homepage</a>

<a href="http://www.devrs.com/gb/">Jeff Frohwein's GB development page.</a>
A extensive source of Game Boy related information, including GeeBee's GB faq and the pan/k00Pa document.

