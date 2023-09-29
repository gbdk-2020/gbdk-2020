// targets.c

#include "gb.h"
#include "targets.h"

// Default Linker list arguments
//
// These get processed in Fixllist()
//
// NOTE: Only -g and -b entries are supported by Fixllist() right now

// Game Boy / Analogue Pocket / Megaduck
arg_entry llist0_defaults_gb[] = {
    {.searchkey= "_shadow_OAM=", .addflag= "-g",.addvalue= "_shadow_OAM=0xC000", .found= false},
    {.searchkey= ".STACK=",      .addflag= "-g",.addvalue= ".STACK=0xE000",      .found= false},
    {.searchkey= ".refresh_OAM=",.addflag= "-g",.addvalue= ".refresh_OAM=0xFF80",.found= false},
    {.searchkey= "_DATA=",       .addflag= "-b",.addvalue= "_DATA=0xC0A0",       .found= false},
    {.searchkey= "_CODE=",       .addflag= "-b",.addvalue= "_CODE=0x0200",       .found= false},
};

// SMS / GG
arg_entry llist0_defaults_sms[] = {
    {.searchkey= "_shadow_OAM=", .addflag= "-g",.addvalue= "_shadow_OAM=0xC000", .found= false},
    {.searchkey= ".STACK=",      .addflag= "-g",.addvalue= ".STACK=0xDFF0",      .found= false},
    {.searchkey= "_DATA=",       .addflag= "-b",.addvalue= "_DATA=0xC0C0",       .found= false},
    {.searchkey= "_CODE=",       .addflag= "-b",.addvalue= "_CODE=0x0100",       .found= false},
};

// MSX
arg_entry llist0_defaults_msxdos[] = {
    {.searchkey= "_DATA=",       .addflag= "-b",.addvalue= "_DATA=0x8000",       .found= false},
    {.searchkey= "_CODE=",       .addflag= "-b",.addvalue= "_CODE=0x0100",       .found= false},
};

// NES
arg_entry llist0_defaults_nes[] = {
    {.searchkey= "_DATA=",       .addflag= "-b",.addvalue= "_DATA=0x0300",       .found = false},
    {.searchkey= "_CODE=",       .addflag= "-b",.addvalue= "_CODE=0xC000",       .found = false},
    {.searchkey= "VECTORS=",     .addflag= "-b",.addvalue= "VECTORS=0xFFFA",     .found = false},
    {.searchkey= "OSEG=",        .addflag= "-b",.addvalue= "OSEG=0x00",          .found = false},
    {.searchkey= "GBDKOVR=",     .addflag= "-b",.addvalue= "GBDKOVR=0x10",       .found = false},
    {.searchkey= "_ZP=",         .addflag= "-b",.addvalue= "_ZP=0x20",           .found = false},
};

// Port/Platform specific settings
//
// $1 are extra parameters passed using -W
// $2 is the list of objects passed as parameters
// $3 is the output file
CLASS classes[] = {
    // GB
    { .port         = "sm83",
      .plat         = "gb",
      .default_plat = "gb",
      .rom_extension=  EXT_GB,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_gb% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% $1 $2",
      .ld           = "%ld_gb% -n -i $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -yN -Z $1 $2 $3",  // -yN: Don't paste in the Nintendo logo bytes for gameboy and clones (-Z)
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_gb,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_gb),
    },
    // Analogue Pocket
    { .port         = "sm83",
      .plat         = "ap",
      .default_plat = "ap",
      .rom_extension=  EXT_AP,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_gb% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% $1 $2",
      .ld           = "%ld_gb% -n -i $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -yN -Z $1 $2 $3",  // -yN: Don't paste in the Nintendo logo bytes for gameboy and clones (-Z)
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_gb,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_gb), // Use GB linker list defaults
    },
    // Megaduck
    { .port         = "sm83",
      .plat         = "duck",
      .default_plat = "duck",
      .rom_extension=  EXT_DUCK,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_gb% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% $1 $2",
      .ld           = "%ld_gb% -n -i $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -yN -Z $1 $2 $3",  // -yN: Don't paste in the Nintendo logo bytes for gameboy and clones (-Z)
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_gb,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_gb), // Use GB linker list defaults
    },

    // SMS
    { .port         = "z80",
      .plat         = "sms",
      .default_plat = "sms",
      .rom_extension=  EXT_SMS,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_z80% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% -plat=sms $1 $2",
      .ld           = "%ld_z80% -a sms -n -i $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -S -xj 4 $1 $2 $3",  // -xj 4 is SS_EXPORT region code
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_sms,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_sms),
    },
    // GG
    { .port         = "z80",
      .plat         = "gg",
      .default_plat = "gg",
      .rom_extension=  EXT_GG,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_z80% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% -plat=sms $1 $2",
      .ld           = "%ld_z80% -a sms -n -i $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -S -xj 6 $1 $2 $3",  // -xj 6 is GG_EXPORT region code, for better compatibility with some emulators
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_sms,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_sms),  // Use SMS linker list defaults
    },

    // MSX
    { .port         = "z80",
      .plat         = "msxdos",
      .default_plat = "msxdos",
      .rom_extension=  EXT_MSXDOS,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_z80% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% -plat=sms $1 $2",
      .ld           = "%ld_z80% -a sms -n -i -j $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% $1 $2 $3",
      .postproc     = "%mkcom% $1 $2",
      .llist0_defaults    = llist0_defaults_msxdos,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_msxdos),
    },

    // NES
    { .port         = "mos6502",
      .plat         = "nes",
      .default_plat = "nes",
      .rom_extension=  EXT_NES,
      .cpp          = "%cpp% %cppdefault% $1 $2 $3",
      .include      = "%includedefault%",
      .com          = "%com% %comdefault% -Wa%asdefault% $1 %comflag% $2 -o $3",
      .as           = "%as_6500% %asdefault% $1 $3 $2",
      .bankpack     = "%bankpack% -plat=nes -mapper=30 $1 $2",
      .ld           = "%ld_6808% -a nes -n -i -j $1 %libs_include% $3 %crt0dir% $2",
      .ihxcheck     = "%ihxcheck% $2 $1",
      .mkbin        = "%mkbin% -N -yo A -yS $2 $3",
      .postproc     = "",
      .llist0_defaults    = llist0_defaults_nes,
      .llist0_defaults_len= ARRAY_LEN(llist0_defaults_nes),
    }

};

int classes_count = ARRAY_LEN(classes);

