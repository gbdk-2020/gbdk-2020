@page docs_getting_started Getting Started


Follow the steps in this section to start using GBDK-2020.

# 1. Download a Release and unzip it
You can get the latest releases from here: https://github.com/gbdk-2020/gbdk-2020/releases


# 2. Compile Example projects
Make sure your GBDK-2020 installation is working correctly by compiling some of the included @ref docs_example_programs "example projects".

If everything in works in the steps below and there are no errors reported then each project that was build should have it's on .gb ROM file (or suitable extension for the other supported targets).

## Windows (without Make installed):
Navigate to a project within the example projects folder (`"examples\gb\"` under your GBDK-2020 install folder) and open a command line. Then type:

    compile

or

    compile.bat

This should build the example project. You can also navigate into other example project folders and build in the same way.


## Linux / MacOS / Windows with Make installed:
Navigate to the example projects folder (`"examples/gb/"` under your GBDK-2020 install folder) and open a command line. Then type:

    make

This should build all of the examples sequentially. You can also navigate into an individual example project's folder and build it by typing `make`.


# 3. Use a Template
__To create a new project use a template!__ 

There are template projects included in the @ref docs_example_programs "GBDK example projects" to help you get up and running. Their folder names start with `template_`.

1. Copy one of the template folders to a new folder name

2. If you moved the folder out of the GBDK examples then you __must__ update the `GBDK` path variable and/or the path to `LCC` in the `Makefile` or `compile.bat` so that it will still build correctly.

3. Type `make` on the command line in that folder to verify it still builds.

4. Open main.c to start making changes.


# 4. If you use GBTD / GBMB, get the fixed version
If you plan to use GBTD / GBMB for making graphics, make sure to get the version with the `const` fix and other improvements. See @ref const_gbtd_gbmb.


# 5. Review Coding Guidelines
Take a look at the @ref docs_coding_guidelines "coding guidelines", even if you have experience writing software for other platforms. There is important information to help you get good results and performance on the Game Boy.

If you haven't written programs in C before, check the @ref docs_c_tutorials "C tutorials section".


# 6. Hardware and Resources
If you have a specific project in mind, consider what hardware want to target. It isn't something that has to be decided up front, but it can influence design and implementation.

What size will your game or program be?
  - 32K Cart (no-MBC required)
  - Larger than 32K (MBC required)
  - See more details about @ref docs_rombanking_mbcs "ROM Banking and MBCs".

What console platform(s) will it run on?
  - Game Boy (GB/GBC)
  - Analogue Pocket (AP)
  - Sega Master System (SMS)
  - Game Gear (GG)
  - Mega Duck (DUCK)
  - See @ref docs_supported_consoles

If targeting the Game Boy, what hardware will it run on?
  - Game Boy (& Game Boy Color)
  - Game Boy Color only
  - Game Boy & Super Game Boy
  - See how to @ref faq_gb_type_header_setting "set the compatibility type in the cartridge header". Read more about hardware differences in the @ref Pandocs


# 7. Set up C Source debugging
Tracking down problems in code is easier with a debugger. Emulicious has a @ref Emulicious_debug "debug adapter" that provides C source debugging with GBDK-2020.


# 8. Try a GBDK Tutorial
You might want to start off with a guided GBDK tutorial from the @ref links_gbdk_tutorials "GBDK Tutorials section".

  - __Note:__ Tutorials (or parts of them) may be based on the older GBDK from the 2000's before it was updated to be GBDK-2020. The general principals are all the same, but the setup and parts of the @ref docs_toolchain "toolchain" (compiler/etc) may be somewhat different and some links may be outdated (pointing to the old GBDK or old tools).


# 9. Read up!
- It is strongly encouraged to read more @ref docs_index "GBDK-2020 General Documentation".
- Learn about the Game Boy hardware by reading through the @ref Pandocs technical reference.


# 10. Need help?
Check out the links for @ref links_help_and_community "online community and support" and read the @ref docs_faq "FAQ".

