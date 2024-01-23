@page docs_links_and_tools Links, Tools and Debugging


This is a brief list of useful tools and information. It is not meant to be complete or exhaustive, for a larger list see the @ref awesome_gb "Awesome Game Boy Development" list.

@anchor links_sdcc_docs
# SDCC Compiler Suite User Manual
  - GBDK-2020 uses the SDCC compiler and related tools. The SDCC manual goes into much more detail about available features and how to use them.  
  http://sdcc.sourceforge.net/doc/sdccman.pdf  
  http://sdcc.sourceforge.net

  - The SDCC assembler and linker (sdas / asxxxx and aslink) manual.  
  https://sourceforge.net/p/sdcc/code/HEAD/tree/trunk/sdcc/sdas/doc/asmlnk.txt


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
    https://gbdev.io/resources.html


@anchor links_sms_gg_docs
# Sega Master System / Game Gear Documentation
  - @anchor smspower
    __SMS Power!__  
    Community site with technical documentation, reviews and other content related to the Sega 8-bit systems.  
    https://www.smspower.org/


@anchor links_gbdk_tutorials
# Tutorials
  - @anchor tutorials_larolds
    __Larold's Jubilant Junkyard  Tutorials__      
    Several walk throughs about the fundamentals of developing for the Game Boy with GBDK-2020. There are simple examples with source code.  
    https://laroldsjubilantjunkyard.com/tutorials/

  - @anchor tutorials_gamingmonsters
    __Gaming Monsters Tutorials__  
    Several video tutorials and code for making games with GBDK/GBDK-2020.  
    https://www.youtube.com/playlist?list=PLeEj4c2zF7PaFv5MPYhNAkBGrkx4iPGJo  
    https://github.com/gingemonster/GamingMonstersGameBoySampleCode

  - @anchor tutorials_typorter
    __Pocket Leage Tutorial__  
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
    This is an updated version with const export fixed and other improvements.  
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
# Music And Sound Effects
  - @anchor hUGEdriver
    __hUGEtracker__ and __hUGEdriver__  
    A tracker and music driver that work with GBDK and RGBDS.
    It is smaller, more efficient and more versatile than gbt_player.  
    https://github.com/SuperDisk/hUGEDriver  
    https://github.com/SuperDisk/hUGETracker

  - @anchor CBT-FX
    __CBT-FX__  
    A sound effects driver which can play effects created in FX Hammer.
    https://github.com/datguywitha3ds/CBT-FX

  - @anchor VGM2GBSFX
    __VGM2GBSFX__  
    A sound effects converter and driver for DMG VGM files, FX Hammer and PCM WAV files.
    https://github.com/untoxa/VGM2GBSFX

  - @anchor gbt-player
    __GBT Player__  
    A .mod converter and music driver that works with GBDK and RGBDS.  
    https://github.com/AntonioND/gbt-player  
    Docs from GBStudio that should mostly apply: https://www.gbstudio.dev/docs/music/


@anchor tools_emulators
# Emulators
  - @anchor emulicious
    __Emulicious__  
    An accurate emulator with extensive tools including source level debugging.
    https://emulicious.net/

  - @anchor bgb
    __BGB__  
    Accurate emulator, has useful debugging tools.  
    http://bgb.bircd.org/

@anchor code_editors_hinting
Intellisense in VSCode may have trouble identifying some GBDK types or functions, and therefore flag them as warnings or unidentified.

GBDK platform constants can be declared so that header files are parsed more completely in VSCode. The following `c_cpp_properties.json` example may be adapted for your own project.

        {
            "configurations": [
                {
                    "name": "gameboy",
                    "includePath": [
                        "${workspaceFolder}/src/**",
                        "${workspaceFolder}/res/**",
                        "${workspaceFolder}/include/**",
                        "${workspaceFolder}/../../../gbdk/include/**"
                    ],
                    "defines": ["__PORT_sm83", "__TARGET_gb"],
                    "compilerPath": "",
                    "cStandard": "c11",
                    "intelliSenseMode": "${default}",
                    "compilerArgs": [],
                    "browse": {
                        "limitSymbolsToIncludedHeaders": true
                    }
                }
            ],
            "version": 4
        }


@anchor tools_debug
# Debugging tools
  - @anchor Emulicious_debug
    __Emulicious debug adapter__  
    Provides source-level debugging in VS Code and Sublime Text that works with GBDK2020.  
    https://marketplace.visualstudio.com/items?itemName=emulicious.emulicious-debugger
    - If compiler optimization is making the program source hard to step through in the debugger then adding this flag to @ref lcc can help. Note that using this flag will likely reduce code performance and increase code size while enabled, so it is best to only use it temporarily.
        - `-Wf--max-allocs-per-node0`

  - @anchor romusage
    __romusage__  
    Calculate used and free space in banks (ROM/RAM) and warn about errors such as bank overflows.  
    See @ref romusage-settings

  - @anchor bgb_symbol_conversion
    __noi file to sym conversion for bgb__  
    Debug information in .noi files can be converted to a symbol format that @ref bgb "BGB" recognizes using:
    - @ref lcc : `-Wm-yS` (with `--debug`, or `-Wl-j` to create the .noi)
    - directly with @ref makebin : `-yS` (with `-j` passed to the linker)

  - @anchor src2sym
    __src2sym.pl__  
    Add line-by-line C source code to the main symbol file in a BGB compatible format. This allows for C source-like debugging in BGB in a limited way.
    https://gbdev.gg8.se/forums/viewtopic.php?id=710


@anchor links_optimizing
# Optimizing Assembly
  - @anchor optimizing_assembly
    __Optimizing Assembly Code__  
    Pret has a useful guide to optimizing assembly for
    the Game Boy for times when asm using in a project
    in addition to C.
    https://github.com/pret/pokecrystal/wiki/Optimizing-assembly-code


@anchor tools_build_ci_cd
# Continuous Integration and Deployment
  - @anchor GBDK_GitHub_Action_Builder
    __GBDK GitHub Action Builder__  
    A Github Action which provides basic CI/CD for building projects based on GBDK (not for building GBDK itself).  
    https://github.com/wujood/gbdk-2020-github-builder
    
