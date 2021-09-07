@page docs_links_and_tools Links and Third-Party Tools


This is a brief list of useful tools and information. It is not meant to be complete or exhaustive, for a larger list see the @ref awesome_gb "Awesome Game Boy Development" list.

@anchor links_sdcc_docs
# SDCC Compiler Suite User Manual
  - GBDK-2020 uses the SDCC compiler and related tools. The SDCC manual goes into much more detail about available features and how to use them.  
  http://sdcc.sourceforge.net/doc/sdccman.pdf  
  http://sdcc.sourceforge.net


@anchor links_help_and_community
# Getting Help
  - GBDK Discord community:  
  https://github.com/gbdk-2020/gbdk-2020/#discord-servers

  - Game Boy discussion forum:  
  https://gbdev.gg8.se/forums/


@anchor links_gameboy_docs
# Game Boy Documentation
  - @anchor Pandocs
    __Pandocs__  
    Extensive and up-to-date technical documentation about the Game Boy and related hardware.  
    https://gbdev.io/pandocs/

  - @anchor awesome_gb
    __Awesome Game Boy Development list__  
    A list of Game Boy/Color development resources, tools, docs, related projects and homebrew.  
    https://gbdev.io/list.html


@anchor links_gbdk_tutorials
# Tutorials
  - @anchor tutorials_gamingmonsters
    __Gaming Monsters Tutorials__  
    Several video tutorials and code for making games with GBDK/GBDK-2020.  
    https://www.youtube.com/playlist?list=PLeEj4c2zF7PaFv5MPYhNAkBGrkx4iPGJo  
    https://github.com/gingemonster/GamingMonstersGameBoySampleCode

  - @anchor tutorials_typorter
    __Pocket Leage Tutortial__  
    https://blog.ty-porter.dev/development/2021/04/04/writing-a-gameboy-game-in-2021-pt-0.html


@anchor link_examples
# Example code
  - @anchor examples_gbdk_playground
    __Simplified GBDK examples__  
    https://github.com/mrombout/gbdk_playground/commits/master


@anchor links_graphic
# Graphics Tools
  - @anchor gbtd_gbmb
    @anchor gbmb
    @anchor gbtd     
    @anchor toxa_gbtd_gbmb
    __Game Boy Tile Designer and Map Builder (GBTD / GBMB)__  
    Sprite / Tile editor and Map Builder that can export to C that works with GBDK.  
    This is an updated version with const export fixed and other improvments.  
    https://github.com/gbdk-2020/GBTD_GBMB
 
      - A GIMP plugin to read/write GBR/GBM files and do map conversion:  
        https://github.com/bbbbbr/gimp-tilemap-gb
      - Command line version of the above tool that doesn't require GIMP (png2gbtiles):  
        https://github.com/bbbbbr/gimp-tilemap-gb/tree/master/console

  - @anchor Tilemap_Studio
    __Tilemap Studio__  
    A tilemap editor for Game Boy, GBC, GBA, or SNES projects.  
    https://github.com/Rangi42/tilemap-studio/


@anchor tools_music
# Music drivers and tools
  - @anchor gbt-player
    __GBT Player__  
    A .mod converter and music driver that works with GBDK and RGBDS.  
    https://github.com/AntonioND/gbt-player  
    Docs from GBStudio that should mostly apply: https://www.gbstudio.dev/docs/music/


  - @anchor hUGEdriver
    __hUGEdriver__  
    A tracker and music driver that works with GBDK and RGBDS.
    It is smaller, more efficient and more versatile than gbt_player.  
    https://github.com/untoxa/hUGEBuild  
    https://github.com/SuperDisk/hUGEDriver  
    https://github.com/SuperDisk/hUGETracker


@anchor tools_emulators
# Emulators
  - @anchor bgb
    __BGB__  
    Accurate emulator, has useful debugging tools.  
    http://bgb.bircd.org/

  - @anchor emulicious
    __Emulicious__  
    An accurate emulator with extensive tools including source level debugging.  
    https://emulicious.net/


@anchor tools_debug
# Debugging tools
  - @anchor Emulicious_debug
    __Emulicious debug adapter__  
    Provides source-level debugging in VS Code that works with GBDK2020.  
    https://marketplace.visualstudio.com/items?itemName=emulicious.emulicious-debugger

  - @anchor romusage
    __romusage__  
    Calculate used and free space in banks (ROM/RAM) and warn about errors such as bank overflows.  
    https://github.com/bbbbbr/romusage

  - @anchor bgb_symbol_conversion
    __noi file to sym conversion for bgb__  
    Debug information in .noi files can be converted to a symbol format that @ref bgb "BGB" recognizes using:
    - @ref lcc : `-Wm-yS` (with `--debug`, or `-Wl-j` to create the .noi)
    - directly with @ref makebin : `-yS` (with `-j` passed to the linker)

  - @anchor src2sym
    __src2sym.pl__  
    Add line-by-line C source code to the main symbol file in a BGB compatible format. This allows for C source-like debugging in BGB in a limited way.
    https://gbdev.gg8.se/forums/viewtopic.php?id=710


@anchor tools_build_ci_cd
# Continuous Integration and Deployment
  - @anchor GBDK_GitHub_Action_Builder
    __GBDK GitHub Action Builder__
    A Github Action which provides basic CI/CD for building projects based on GBDK (not for building GBDK itself).  
    https://github.com/wujood/gbdk-2020-github-builder
    
