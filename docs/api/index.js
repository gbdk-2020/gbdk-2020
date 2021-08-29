var index =
[
    [ "Introduction", "index.html#autotoc_md163", null ],
    [ "About the Documentation", "index.html#autotoc_md164", null ],
    [ "About GBDK", "index.html#autotoc_md165", null ],
    [ "Historical Info and Links", "index.html#autotoc_md166", null ],
    [ "Getting Started", "docs_getting_started.html", [
      [ "1. Download a Release and unzip it", "docs_getting_started.html#autotoc_md6", null ],
      [ "2. Compile Example projects", "docs_getting_started.html#autotoc_md7", null ],
      [ "3. Use a Template", "docs_getting_started.html#autotoc_md8", null ],
      [ "4. If you use GBTD / GBMB, get the fixed version", "docs_getting_started.html#autotoc_md9", null ],
      [ "5. Review Coding Guidelines", "docs_getting_started.html#autotoc_md10", null ],
      [ "6. Hardware and Resources", "docs_getting_started.html#autotoc_md11", null ],
      [ "7. Set up C Source debugging", "docs_getting_started.html#autotoc_md12", null ],
      [ "8. Try a GBDK Tutorial", "docs_getting_started.html#autotoc_md13", null ],
      [ "9. Read up!", "docs_getting_started.html#autotoc_md14", null ],
      [ "10. Need help?", "docs_getting_started.html#autotoc_md15", null ]
    ] ],
    [ "Links and Third-Party Tools", "docs_links_and_tools.html", [
      [ "SDCC Compiler Suite User Manual", "docs_links_and_tools.html#autotoc_md16", null ],
      [ "Getting Help", "docs_links_and_tools.html#autotoc_md17", null ],
      [ "Game Boy Documentation", "docs_links_and_tools.html#autotoc_md18", null ],
      [ "Tutorials", "docs_links_and_tools.html#autotoc_md19", null ],
      [ "Example code", "docs_links_and_tools.html#autotoc_md20", null ],
      [ "Graphics Tools", "docs_links_and_tools.html#autotoc_md21", null ],
      [ "Music drivers and tools", "docs_links_and_tools.html#autotoc_md22", null ],
      [ "Emulators", "docs_links_and_tools.html#autotoc_md23", null ],
      [ "Debugging tools", "docs_links_and_tools.html#autotoc_md24", null ],
      [ "Continuous Integration and Deployment", "docs_links_and_tools.html#autotoc_md25", null ]
    ] ],
    [ "Using GBDK", "docs_using_gbdk.html", [
      [ "Interrupts", "docs_using_gbdk.html#autotoc_md26", [
        [ "Available Interrupts", "docs_using_gbdk.html#autotoc_md27", null ],
        [ "Adding your own interrupt handler", "docs_using_gbdk.html#autotoc_md28", null ],
        [ "Returning from Interrupts and STAT mode", "docs_using_gbdk.html#autotoc_md29", null ]
      ] ],
      [ "What GBDK does automatically and behind the scenes", "docs_using_gbdk.html#autotoc_md30", [
        [ "OAM (VRAM Sprite Attribute Table)", "docs_using_gbdk.html#autotoc_md31", null ],
        [ "Font tiles when using stdio.h", "docs_using_gbdk.html#autotoc_md32", null ],
        [ "Default Interrupt Service Handlers (ISRs)", "docs_using_gbdk.html#autotoc_md33", null ]
      ] ],
      [ "Copying Functions to RAM and HIRAM", "docs_using_gbdk.html#autotoc_md34", null ],
      [ "Mixing C and Assembly", "docs_using_gbdk.html#autotoc_md35", [
        [ "Inline ASM within C source files", "docs_using_gbdk.html#autotoc_md36", null ],
        [ "In Separate ASM files", "docs_using_gbdk.html#autotoc_md37", null ]
      ] ],
      [ "Known Issues and Limitations", "docs_using_gbdk.html#autotoc_md38", [
        [ "SDCC", "docs_using_gbdk.html#autotoc_md39", null ]
      ] ]
    ] ],
    [ "Coding Guidelines", "docs_coding_guidelines.html", [
      [ "Learning C / C fundamentals", "docs_coding_guidelines.html#autotoc_md40", [
        [ "General C tutorials", "docs_coding_guidelines.html#autotoc_md41", null ],
        [ "Embedded C introductions", "docs_coding_guidelines.html#autotoc_md42", null ],
        [ "Game Boy games in C", "docs_coding_guidelines.html#autotoc_md43", null ]
      ] ],
      [ "Understanding the hardware", "docs_coding_guidelines.html#autotoc_md44", null ],
      [ "Writing optimal C code for the Game Boy and SDCC", "docs_coding_guidelines.html#autotoc_md45", [
        [ "Tools", "docs_coding_guidelines.html#autotoc_md46", [
          [ "GBTD / GBMB, Arrays and the \"const\" keyword", "docs_coding_guidelines.html#autotoc_md47", null ]
        ] ],
        [ "Variables", "docs_coding_guidelines.html#autotoc_md48", null ],
        [ "Code structure", "docs_coding_guidelines.html#autotoc_md49", null ],
        [ "GBDK API/Library", "docs_coding_guidelines.html#autotoc_md50", null ],
        [ "Toolchain", "docs_coding_guidelines.html#autotoc_md51", null ],
        [ "chars and vararg functions", "docs_coding_guidelines.html#autotoc_md52", null ]
      ] ],
      [ "When C isn't fast enough", "docs_coding_guidelines.html#autotoc_md53", [
        [ "Calling convention", "docs_coding_guidelines.html#autotoc_md54", null ],
        [ "Variables and registers", "docs_coding_guidelines.html#autotoc_md55", null ],
        [ "Segments", "docs_coding_guidelines.html#autotoc_md56", null ]
      ] ]
    ] ],
    [ "ROM/RAM Banking and MBCs", "docs_rombanking_mbcs.html", [
      [ "ROM/RAM Banking and MBCs (Memory Bank Controllers)", "docs_rombanking_mbcs.html#autotoc_md57", [
        [ "Non-banked cartridges", "docs_rombanking_mbcs.html#autotoc_md58", null ],
        [ "MBC Banked cartridges (Memory Bank Controllers)", "docs_rombanking_mbcs.html#autotoc_md59", null ]
      ] ],
      [ "Working with Banks", "docs_rombanking_mbcs.html#autotoc_md60", [
        [ "Setting the ROM bank for a Source file", "docs_rombanking_mbcs.html#autotoc_md61", null ],
        [ "Setting the RAM bank for a Source file", "docs_rombanking_mbcs.html#autotoc_md62", null ],
        [ "Setting the MBC and number of ROM & RAM banks available", "docs_rombanking_mbcs.html#autotoc_md63", null ],
        [ "Banked Functions", "docs_rombanking_mbcs.html#autotoc_md64", null ],
        [ "Const Data (Variables in ROM)", "docs_rombanking_mbcs.html#autotoc_md65", null ],
        [ "Variables in RAM", "docs_rombanking_mbcs.html#autotoc_md66", null ],
        [ "Far Pointers", "docs_rombanking_mbcs.html#autotoc_md67", null ],
        [ "Bank switching", "docs_rombanking_mbcs.html#autotoc_md68", null ],
        [ "Restoring the current bank (after calling functions which change it without restoring)", "docs_rombanking_mbcs.html#autotoc_md69", null ],
        [ "Currently active bank: _current_bank", "docs_rombanking_mbcs.html#autotoc_md70", null ]
      ] ],
      [ "Auto-Banking", "docs_rombanking_mbcs.html#autotoc_md71", null ],
      [ "Errors related to banking (overflow, multiple writes to same location)", "docs_rombanking_mbcs.html#autotoc_md72", null ],
      [ "Bank space usage", "docs_rombanking_mbcs.html#autotoc_md73", [
        [ "Other important notes", "docs_rombanking_mbcs.html#autotoc_md74", null ]
      ] ],
      [ "Banking example projects", "docs_rombanking_mbcs.html#autotoc_md75", null ]
    ] ],
    [ "GBDK Toolchain", "docs_toolchain.html", [
      [ "Overview", "docs_toolchain.html#autotoc_md76", null ],
      [ "Data Types", "docs_toolchain.html#autotoc_md77", null ],
      [ "Changing Important Addresses", "docs_toolchain.html#autotoc_md78", null ],
      [ "Compiling programs", "docs_toolchain.html#autotoc_md79", [
        [ "Makefiles", "docs_toolchain.html#Makefiles", null ]
      ] ],
      [ "Build Tools", "docs_toolchain.html#autotoc_md80", [
        [ "lcc", "docs_toolchain.html#autotoc_md81", null ],
        [ "sdcc", "docs_toolchain.html#autotoc_md82", null ],
        [ "sdasgb", "docs_toolchain.html#autotoc_md83", null ],
        [ "bankpack", "docs_toolchain.html#autotoc_md84", null ],
        [ "sdldgb", "docs_toolchain.html#autotoc_md85", null ],
        [ "ihxcheck", "docs_toolchain.html#autotoc_md86", null ],
        [ "makebin", "docs_toolchain.html#autotoc_md87", null ]
      ] ],
      [ "GBDK Utilities", "docs_toolchain.html#autotoc_md88", [
        [ "GBCompress", "docs_toolchain.html#autotoc_md89", null ],
        [ "PNG to Metasprite", "docs_toolchain.html#autotoc_md90", [
          [ "Working with png2mtspr", "docs_toolchain.html#autotoc_md91", null ]
        ] ]
      ] ]
    ] ],
    [ "Example Programs", "docs_example_programs.html", [
      [ "banks (various projects)", "docs_example_programs.html#autotoc_md92", null ],
      [ "comm", "docs_example_programs.html#autotoc_md93", null ],
      [ "crash", "docs_example_programs.html#autotoc_md94", null ],
      [ "colorbar", "docs_example_programs.html#autotoc_md95", null ],
      [ "dscan", "docs_example_programs.html#autotoc_md96", null ],
      [ "filltest", "docs_example_programs.html#autotoc_md97", null ],
      [ "fonts", "docs_example_programs.html#autotoc_md98", null ],
      [ "galaxy", "docs_example_programs.html#autotoc_md99", null ],
      [ "gb-dtmf", "docs_example_programs.html#autotoc_md100", null ],
      [ "gbdecompress", "docs_example_programs.html#autotoc_md101", null ],
      [ "irq", "docs_example_programs.html#autotoc_md102", null ],
      [ "large map", "docs_example_programs.html#autotoc_md103", null ],
      [ "metasprites", "docs_example_programs.html#autotoc_md104", null ],
      [ "lcd isr wobble", "docs_example_programs.html#autotoc_md105", null ],
      [ "paint", "docs_example_programs.html#autotoc_md106", null ],
      [ "rand", "docs_example_programs.html#autotoc_md107", null ],
      [ "ram_fn", "docs_example_programs.html#autotoc_md108", null ],
      [ "rpn", "docs_example_programs.html#autotoc_md109", null ],
      [ "samptest", "docs_example_programs.html#autotoc_md110", null ],
      [ "sgb (various)", "docs_example_programs.html#autotoc_md111", null ],
      [ "sound", "docs_example_programs.html#autotoc_md112", null ],
      [ "space", "docs_example_programs.html#autotoc_md113", null ],
      [ "templates", "docs_example_programs.html#autotoc_md114", null ]
    ] ],
    [ "Frequently Asked Questions (FAQ)", "docs_faq.html", [
      [ "General", "docs_faq.html#autotoc_md115", null ],
      [ "ROM Header Settings", "docs_faq.html#autotoc_md116", null ],
      [ "Errors / Compiling / Toolchain", "docs_faq.html#autotoc_md117", null ],
      [ "API / Utilities", "docs_faq.html#autotoc_md118", null ]
    ] ],
    [ "Migrating to new GBDK Versions", "docs_migrating_versions.html", [
      [ "GBDK 2020 versions", "docs_migrating_versions.html#autotoc_md119", [
        [ "Porting to GBDK 2020 4.0.4", "docs_migrating_versions.html#autotoc_md120", null ],
        [ "Porting to GBDK 2020 4.0.3", "docs_migrating_versions.html#autotoc_md121", null ],
        [ "Porting to GBDK 2020 4.0.2", "docs_migrating_versions.html#autotoc_md122", null ],
        [ "Porting to GBDK 2020 4.0.1", "docs_migrating_versions.html#autotoc_md123", null ],
        [ "Porting to GBDK 2020 4.0", "docs_migrating_versions.html#autotoc_md124", null ],
        [ "Porting to GBDK 2020 3.2", "docs_migrating_versions.html#autotoc_md125", null ],
        [ "Porting to GBDK 2020 3.1.1", "docs_migrating_versions.html#autotoc_md126", null ],
        [ "Porting to GBDK 2020 3.1", "docs_migrating_versions.html#autotoc_md127", null ],
        [ "Porting to GBDK 2020 3.0.1", "docs_migrating_versions.html#autotoc_md128", null ]
      ] ],
      [ "Historical GBDK versions", "docs_migrating_versions.html#autotoc_md130", [
        [ "GBDK 1.1 to GBDK 2.0", "docs_migrating_versions.html#autotoc_md131", null ]
      ] ]
    ] ],
    [ "GBDK Releases", "docs_releases.html", [
      [ "GBDK 2020 Release Notes", "docs_releases.html#autotoc_md132", [
        [ "GBDK 2020 4.0.4", "docs_releases.html#autotoc_md133", null ],
        [ "GBDK 2020 4.0.3", "docs_releases.html#autotoc_md134", null ],
        [ "GBDK 2020 4.0.2", "docs_releases.html#autotoc_md135", null ],
        [ "GBDK 2020 4.0.1", "docs_releases.html#autotoc_md136", null ],
        [ "GBDK 2020 4.0", "docs_releases.html#autotoc_md137", null ],
        [ "GBDK 2020 3.2", "docs_releases.html#autotoc_md138", null ],
        [ "GBDK 2020 3.1.1", "docs_releases.html#autotoc_md139", null ],
        [ "GBDK 2020 3.1", "docs_releases.html#autotoc_md140", null ],
        [ "GBDK 2020 3.0.1", "docs_releases.html#autotoc_md141", null ],
        [ "GBDK 2020 3.0", "docs_releases.html#autotoc_md142", null ]
      ] ],
      [ "Historical GBDK Release Notes", "docs_releases.html#autotoc_md143", [
        [ "GBDK 2.96", "docs_releases.html#autotoc_md144", null ],
        [ "GBDK 2.95-3", "docs_releases.html#autotoc_md145", null ],
        [ "GBDK 2.95-2", "docs_releases.html#autotoc_md146", null ],
        [ "GBDK 2.95", "docs_releases.html#autotoc_md147", null ],
        [ "GBDK 2.94", "docs_releases.html#autotoc_md148", null ],
        [ "GBDK 2.93", "docs_releases.html#autotoc_md149", null ],
        [ "GBDK 2.92-2 for win32", "docs_releases.html#autotoc_md150", null ],
        [ "GBDK 2.92", "docs_releases.html#autotoc_md151", null ],
        [ "GBDK 2.91", "docs_releases.html#autotoc_md152", null ],
        [ "GBDK 2.1.5", "docs_releases.html#autotoc_md153", null ]
      ] ]
    ] ],
    [ "Toolchain settings", "docs_toolchain_settings.html", [
      [ "lcc settings", "docs_toolchain_settings.html#autotoc_md154", null ],
      [ "sdcc settings", "docs_toolchain_settings.html#autotoc_md155", null ],
      [ "sdasgb settings", "docs_toolchain_settings.html#autotoc_md156", null ],
      [ "bankpack settings", "docs_toolchain_settings.html#autotoc_md157", null ],
      [ "sdldgb settings", "docs_toolchain_settings.html#autotoc_md158", null ],
      [ "ihxcheck settings", "docs_toolchain_settings.html#autotoc_md159", null ],
      [ "makebin settings", "docs_toolchain_settings.html#autotoc_md160", null ],
      [ "gbcompress settings", "docs_toolchain_settings.html#autotoc_md161", null ],
      [ "png2mtspr settings", "docs_toolchain_settings.html#autotoc_md162", null ]
    ] ]
];