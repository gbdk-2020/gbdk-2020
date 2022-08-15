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
 - Support for multiple consoles: Game Boy, Analogue Pocket, Mega Duck, Master System and Game Gear

GBDK is freeware. Most of the tooling code is under the GPL. The runtime libraries should be under the LGPL. Please consider mentioning GBDK in the credits of projects made with it.

- - - - - -

# Historical Info and Links

Work on the original GBDK (pre-2020) was by:

Pascal Felber, Lars Malmborg, Michael Hope, David Galloway (djmips), John Fuge, and others.


The following is from the original GBDK documentation:

Thanks to quang for many of the comments to the gb functions.  Some
of the comments are ripped directly from the Linux Programmers
manual, and some directly from the pan/k00Pa document.

<a href="http://quangdx.com/">quangDX.com</a>

<a href="http://gbdk.sourceforge.net/">The (original) gbdk homepage</a>

<a href="http://www.devrs.com/gb/">Jeff Frohwein's GB development page.</a>
A extensive source of Game Boy related information, including GeeBee's GB faq and the pan/k00Pa document.

