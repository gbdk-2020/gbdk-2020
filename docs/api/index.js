var index =
[
    [ "About the Documentation", "index.html#autotoc_md35", null ],
    [ "About GBDK", "index.html#autotoc_md36", null ],
    [ "Historical Info and Links", "index.html#autotoc_md37", null ],
    [ "Links and Third-Party Tools", "docs_links_and_tools.html", [
      [ "SDCC Compiler Suite User Manual", "docs_links_and_tools.html#autotoc_md38", null ],
      [ "Game Boy Documentation", "docs_links_and_tools.html#autotoc_md39", null ],
      [ "Tutorials", "docs_links_and_tools.html#autotoc_md40", null ],
      [ "Graphics Tools", "docs_links_and_tools.html#autotoc_md41", null ],
      [ "Music drivers and tools", "docs_links_and_tools.html#autotoc_md42", null ],
      [ "Emulators", "docs_links_and_tools.html#autotoc_md43", null ],
      [ "Debugging tools", "docs_links_and_tools.html#autotoc_md44", null ]
    ] ],
    [ "Using GBDK", "docs_using_gbdk.html", [
      [ "Interrupts", "docs_using_gbdk.html#autotoc_md110", [
        [ "Available Interrupts", "docs_using_gbdk.html#autotoc_md111", null ],
        [ "Adding your own interrupt handler", "docs_using_gbdk.html#autotoc_md112", null ],
        [ "Returning from Interrupts and STAT mode", "docs_using_gbdk.html#autotoc_md113", null ]
      ] ],
      [ "What GBDK does automatically and behind the scenes", "docs_using_gbdk.html#autotoc_md114", [
        [ "OAM (VRAM Sprite Attribute Table)", "docs_using_gbdk.html#autotoc_md115", null ],
        [ "Font tiles when using stdio.h", "docs_using_gbdk.html#autotoc_md116", null ],
        [ "Default Interrupt Service Handlers (ISRs)", "docs_using_gbdk.html#autotoc_md117", null ]
      ] ],
      [ "Copying Functions to RAM and HIRAM", "docs_using_gbdk.html#autotoc_md118", null ],
      [ "Mixing C and Assembly", "docs_using_gbdk.html#autotoc_md119", [
        [ "Inline ASM within C source files", "docs_using_gbdk.html#autotoc_md120", null ],
        [ "In Separate ASM files", "docs_using_gbdk.html#autotoc_md121", null ]
      ] ],
      [ "Known Issues and Limitations", "docs_using_gbdk.html#autotoc_md122", [
        [ "SDCC", "docs_using_gbdk.html#autotoc_md123", null ]
      ] ]
    ] ],
    [ "Coding Guidelines", "docs_coding_guidelines.html", [
      [ "Learning C / C fundamentals", "docs_coding_guidelines.html#autotoc_md20", [
        [ "General C tutorials", "docs_coding_guidelines.html#autotoc_md21", null ],
        [ "Embedded C introductions", "docs_coding_guidelines.html#autotoc_md22", null ],
        [ "Game Boy games in C", "docs_coding_guidelines.html#autotoc_md23", null ]
      ] ],
      [ "Understanding the hardware", "docs_coding_guidelines.html#autotoc_md24", null ],
      [ "Writing optimal C code for the Game Boy and SDCC", "docs_coding_guidelines.html#autotoc_md25", [
        [ "Variables", "docs_coding_guidelines.html#autotoc_md26", null ],
        [ "Code structure", "docs_coding_guidelines.html#autotoc_md27", null ],
        [ "GBDK API/Library", "docs_coding_guidelines.html#autotoc_md28", null ],
        [ "Toolchain", "docs_coding_guidelines.html#autotoc_md29", null ],
        [ "chars and vararg functions", "docs_coding_guidelines.html#autotoc_md30", null ]
      ] ],
      [ "When C isn't fast enough", "docs_coding_guidelines.html#autotoc_md31", [
        [ "Calling convention", "docs_coding_guidelines.html#autotoc_md32", null ],
        [ "Variables and registers", "docs_coding_guidelines.html#autotoc_md33", null ],
        [ "Segments", "docs_coding_guidelines.html#autotoc_md34", null ]
      ] ]
    ] ],
    [ "ROM/RAM Banking and MBCs", "docs_rombanking_mbcs.html", [
      [ "ROM/RAM Banking and MBCs (Memory Bank Controllers)", "docs_rombanking_mbcs.html#autotoc_md1", [
        [ "Unbanked cartridges", "docs_rombanking_mbcs.html#autotoc_md2", null ],
        [ "MBC Banked cartridges (Memory Bank Controllers)", "docs_rombanking_mbcs.html#autotoc_md3", null ]
      ] ],
      [ "Working with Banks", "docs_rombanking_mbcs.html#autotoc_md4", [
        [ "Setting the ROM bank for a Source file", "docs_rombanking_mbcs.html#autotoc_md5", null ],
        [ "Setting the RAM bank for a Source file", "docs_rombanking_mbcs.html#autotoc_md6", null ],
        [ "Setting the MBC and number of ROM & RAM banks available", "docs_rombanking_mbcs.html#autotoc_md7", null ],
        [ "Banked Functions", "docs_rombanking_mbcs.html#autotoc_md8", null ],
        [ "Const Data (Variables in ROM)", "docs_rombanking_mbcs.html#autotoc_md9", null ],
        [ "Variables in RAM", "docs_rombanking_mbcs.html#autotoc_md10", null ],
        [ "Far Pointers", "docs_rombanking_mbcs.html#autotoc_md11", null ],
        [ "Bank switching", "docs_rombanking_mbcs.html#autotoc_md12", null ],
        [ "Bank switching inside an Interrupt Service Routine (ISR)", "docs_rombanking_mbcs.html#autotoc_md13", null ],
        [ "Currently active bank: _current_bank", "docs_rombanking_mbcs.html#autotoc_md14", null ]
      ] ],
      [ "Auto-Banking", "docs_rombanking_mbcs.html#autotoc_md15", null ],
      [ "Errors related to banking (overflow, multiple writes to same location)", "docs_rombanking_mbcs.html#autotoc_md16", null ],
      [ "Bank space usage", "docs_rombanking_mbcs.html#autotoc_md17", [
        [ "Other important notes", "docs_rombanking_mbcs.html#autotoc_md18", null ]
      ] ],
      [ "Banking example projects", "docs_rombanking_mbcs.html#autotoc_md19", null ]
    ] ],
    [ "GBDK Toolchain", "docs_toolchain.html", [
      [ "Overview", "docs_toolchain.html#autotoc_md95", null ],
      [ "Frequently Asked Questions", "docs_toolchain.html#autotoc_md96", null ],
      [ "Changing Important Addresses", "docs_toolchain.html#autotoc_md97", null ],
      [ "Compiling programs", "docs_toolchain.html#autotoc_md98", [
        [ "Makefiles", "docs_toolchain.html#Makefiles", null ]
      ] ],
      [ "Build Tools", "docs_toolchain.html#autotoc_md99", [
        [ "lcc", "docs_toolchain.html#autotoc_md100", null ],
        [ "sdcc", "docs_toolchain.html#autotoc_md101", null ],
        [ "sdasgb", "docs_toolchain.html#autotoc_md102", null ],
        [ "bankpack", "docs_toolchain.html#autotoc_md103", null ],
        [ "sdldgb", "docs_toolchain.html#autotoc_md104", null ],
        [ "ihxcheck", "docs_toolchain.html#autotoc_md105", null ],
        [ "makebin", "docs_toolchain.html#autotoc_md106", null ]
      ] ],
      [ "GBDK Utilities", "docs_toolchain.html#autotoc_md107", [
        [ "GBCompress", "docs_toolchain.html#autotoc_md108", null ],
        [ "PNG to Metasprite", "docs_toolchain.html#autotoc_md109", null ]
      ] ]
    ] ],
    [ "GBDK Sample Programs", "docs_sample_programs.html", [
      [ "banks (various projects)", "docs_sample_programs.html#autotoc_md76", null ],
      [ "comm", "docs_sample_programs.html#autotoc_md77", null ],
      [ "crash", "docs_sample_programs.html#autotoc_md78", null ],
      [ "colorbar", "docs_sample_programs.html#autotoc_md79", null ],
      [ "dscan", "docs_sample_programs.html#autotoc_md80", null ],
      [ "filltest", "docs_sample_programs.html#autotoc_md81", null ],
      [ "fonts", "docs_sample_programs.html#autotoc_md82", null ],
      [ "galaxy", "docs_sample_programs.html#autotoc_md83", null ],
      [ "gb-dtmf", "docs_sample_programs.html#autotoc_md84", null ],
      [ "irq", "docs_sample_programs.html#autotoc_md85", null ],
      [ "lcd isr wobble", "docs_sample_programs.html#autotoc_md86", null ],
      [ "paint", "docs_sample_programs.html#autotoc_md87", null ],
      [ "rand", "docs_sample_programs.html#autotoc_md88", null ],
      [ "ram_fn", "docs_sample_programs.html#autotoc_md89", null ],
      [ "rpn", "docs_sample_programs.html#autotoc_md90", null ],
      [ "samptest", "docs_sample_programs.html#autotoc_md91", null ],
      [ "sgb (various)", "docs_sample_programs.html#autotoc_md92", null ],
      [ "sound", "docs_sample_programs.html#autotoc_md93", null ],
      [ "space", "docs_sample_programs.html#autotoc_md94", null ]
    ] ],
    [ "Migrating to new GBDK Versions", "docs_migrating_versions.html", [
      [ "GBDK 2020 versions", "docs_migrating_versions.html#autotoc_md45", [
        [ "Porting to GBDK 2020 4.0.2", "docs_migrating_versions.html#autotoc_md46", null ],
        [ "Porting to GBDK 2020 4.0.1", "docs_migrating_versions.html#autotoc_md47", null ],
        [ "Porting to GBDK 2020 4.0", "docs_migrating_versions.html#autotoc_md48", null ],
        [ "Porting to GBDK 2020 3.2", "docs_migrating_versions.html#autotoc_md49", null ],
        [ "Porting to GBDK 2020 3.1.1", "docs_migrating_versions.html#autotoc_md50", null ],
        [ "Porting to GBDK 2020 3.1", "docs_migrating_versions.html#autotoc_md51", null ],
        [ "Porting to GBDK 2020 3.0.1", "docs_migrating_versions.html#autotoc_md52", null ]
      ] ],
      [ "Historical GBDK versions", "docs_migrating_versions.html#autotoc_md54", [
        [ "GBDK 1.1 to GBDK 2.0", "docs_migrating_versions.html#autotoc_md55", null ]
      ] ]
    ] ],
    [ "GBDK Releases", "docs_releases.html", [
      [ "GBDK 2020 Release Notes", "docs_releases.html#autotoc_md56", [
        [ "GBDK 2020 4.0.2", "docs_releases.html#autotoc_md57", null ],
        [ "GBDK 2020 4.0.1", "docs_releases.html#autotoc_md58", null ],
        [ "GBDK 2020 4.0", "docs_releases.html#autotoc_md59", null ],
        [ "GBDK 2020 3.2", "docs_releases.html#autotoc_md60", null ],
        [ "GBDK 2020 3.1.1", "docs_releases.html#autotoc_md61", null ],
        [ "GBDK 2020 3.1", "docs_releases.html#autotoc_md62", null ],
        [ "GBDK 2020 3.0.1", "docs_releases.html#autotoc_md63", null ],
        [ "GBDK 2020 3.0", "docs_releases.html#autotoc_md64", null ]
      ] ],
      [ "Historical GBDK Release Notes", "docs_releases.html#autotoc_md65", [
        [ "GBDK 2.96", "docs_releases.html#autotoc_md66", null ],
        [ "GBDK 2.95-3", "docs_releases.html#autotoc_md67", null ],
        [ "GBDK 2.95-2", "docs_releases.html#autotoc_md68", null ],
        [ "GBDK 2.95", "docs_releases.html#autotoc_md69", null ],
        [ "GBDK 2.94", "docs_releases.html#autotoc_md70", null ],
        [ "GBDK 2.93", "docs_releases.html#autotoc_md71", null ],
        [ "GBDK 2.92-2 for win32", "docs_releases.html#autotoc_md72", null ],
        [ "GBDK 2.92", "docs_releases.html#autotoc_md73", null ],
        [ "GBDK 2.91", "docs_releases.html#autotoc_md74", null ],
        [ "GBDK 2.1.5", "docs_releases.html#autotoc_md75", null ]
      ] ]
    ] ]
];