@page docs_toolchain_settings Toolchain settings
@anchor lcc-settings
# lcc settings
```
./lcc [ option | file ]...
    except for -l, options are processed left-to-right before files
    unrecognized options are taken to be linker options
-A             warn about nonANSI usage; 2nd -A warns more
-b             emit expression-level profiling code; see bprint(1)
-Bdir/         use the compiler named `dir/rcc'
-c             compile only
-dn            set switch statement density to `n'
-debug         Turns on --debug for compiler, -y (.cdb), -j (.noi), -w (wide .map format) for linker
                       -Wa-l (assembler .lst), -Wl-u (.lst -> .rst address update)
-Dname=def     define the preprocessor symbol `name'
-E             only run preprocessor on named .c and .h files files -> stdout
--save-preproc Use with -E for output to *.i files instead of stdout
-g             produce symbol table information for debuggers
-help or -?    print this message
-Idir          add `dir' to the beginning of the list of #include directories
-K             don't run ihxcheck test on linker ihx output
-lx            search library `x'
-m             select port and platform: "-m[port]:[plat]" ports:sm83,z80,mos6502 plats:ap,duck,gb,sms,gg,nes
-N             do not search the standard directories for #include files
-n             emit code to check for dereferencing zero pointers
-no-crt        do not auto-include the gbdk crt0.o runtime in linker list
-no-libs       do not auto-include the gbdk libs in linker list
-O             is ignored
-o file        leave the output in `file'
-P             print ANSI-style declarations for globals
-p -pg         emit profiling code; see prof(1) and gprof(1)
-S             compile to assembly language
-autobank      auto-assign banks set to 255 (bankpack)
-static        specify static libraries (default is dynamic)
-t -tname      emit function tracing calls to printf or to `name'
-target name   is ignored
-tempdir=dir   place temporary files in `dir/'; default=/tmp
-Uname         undefine the preprocessor symbol `name'
-v             show commands as they are executed; 2nd -v suppresses execution
-w             suppress warnings
-Woarg         specify system-specific `arg'
-W[pfablim]arg pass `arg' to the preprocessor, compiler, assembler, bankpack, linker, ihxcheck, or makebin
```
@anchor sdcc-settings
# sdcc settings
```
SDCC : z80/sm83/mos6502/mos65c02 TD- 4.4.0 #14620 (Linux)
published under GNU General Public License (GPL)
Usage : sdcc [options] filename
Options :-

General options:
      --help                Display this help
  -v  --version             Display sdcc's version
      --verbose             Trace calls to the preprocessor, assembler, and linker
  -V                        Execute verbosely. Show sub commands as they are run
  -d                        Output list of macro definitions in effect. Use with -E
  -D                        Define macro as in -Dmacro
  -I                        Add to the include (*.h) path, as in -Ipath
  -A                        
  -U                        Undefine macro as in -Umacro
  -M                        Preprocessor option
  -W                        Pass through options to the pre-processor (p), assembler (a) or linker (l)
      --include             Pre-include a file during pre-processing
  -E  --preprocessonly      Preprocess only, do not compile
      --syntax-only         Parse and verify syntax only, do not compile
  -S                        Compile only; do not assemble or link
  -c  --compile-only        Compile and assemble, but do not link
      --c1mode              Act in c1 mode.  The standard input is preprocessed code, the output is assembly code.
  -o                        Place the output into the given path resp. file
  -x                        Optional file type override (c, c-header or none), valid until the next -x
      --print-search-dirs   display the directories in the compiler's search path
      --vc                  messages are compatible with Micro$oft visual studio
      --use-stdout          send errors to stdout instead of stderr
      --nostdlib            Do not include the standard library directory in the search path
      --nostdinc            Do not include the standard include directory in the search path
      --less-pedantic       Disable some of the more pedantic warnings
      --disable-warning     <nnnn> Disable specific warning
      --Werror              Treat the warnings as errors
      --debug               Enable debugging symbol output
      --cyclomatic          Display complexity of compiled functions
      --std                 Determine the language standard (c89, c99, c11, c23, sdcc89 etc.)
      --fdollars-in-identifiers  Permit '$' as an identifier character
      --fsigned-char        Make "char" signed by default
      --use-non-free        Search / include non-free licensed libraries and header files

Code generation options:
  -m                        Set the port to use e.g. -mz80.
  -p                        Select port specific processor e.g. -mpic14 -p16f84
      --stack-auto          Stack automatic variables
      --xstack              Use external stack
      --int-long-reent      Use reentrant calls on the int and long support functions
      --float-reent         Use reentrant calls on the float support functions
      --xram-movc           Use movc instead of movx to read xram (xdata)
      --callee-saves        <func[,func,...]> Cause the called function to save registers instead of the caller
      --fomit-frame-pointer  Leave out the frame pointer.
      --all-callee-saves    callee will always save registers used
      --stack-probe         insert call to function __stack_probe at each function prologue
      --no-xinit-opt        don't memcpy initialized xram from code
      --no-c-code-in-asm    don't include c-code as comments in the asm file
      --no-peep-comments    don't include peephole optimizer comments
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment

Optimization options:
      --opt-code-speed      Optimize for code speed rather than size
      --opt-code-size       Optimize for code size rather than speed
      --max-allocs-per-node  Maximum number of register assignments considered at each node of the tree decomposition
      --no-reg-params       On some ports, disable passing some parameters in registers
      --nostdlibcall        Disable optimization of calls to standard library
      --nooverlay           Disable overlaying leaf function auto variables
      --nogcse              Disable the GCSE optimisation
      --nolospre            Disable lospre
      --nogenconstprop      Disable generalized constant propagation
      --nolabelopt          Disable label optimisation
      --noinvariant         Disable optimisation of invariants
      --noinduction         Disable loop variable induction
      --noloopreverse       Disable the loop reverse optimisation
      --no-peep             Disable the peephole assembly file optimisation
      --peep-asm            Enable peephole optimization on inline assembly
      --peep-return         Enable peephole optimization for return instructions
      --no-peep-return      Disable peephole optimization for return instructions
      --peep-file           <file> use this extra peephole file
      --allow-unsafe-read   Allow optimizations to read any memory location anytime

Internal debugging options:
      --dump-ast            Dump front-end AST before generating i-code
      --dump-i-code         Dump the i-code structure at all stages
      --dump-graphs         Dump graphs (control-flow, conflict, etc)
      --i-code-in-asm       Include i-code as comments in the asm file
      --fverbose-asm        Include code generator comments in the asm output

Linker options:
  -l                        Include the given library in the link
  -L                        Add the next field to the library search path
      --lib-path            <path> use this path to search for libraries
      --out-fmt-ihx         Output in Intel hex format
      --out-fmt-s19         Output in S19 hex format
      --xram-loc            <nnnn> External Ram start location
      --xram-size           <nnnn> External Ram size
      --iram-size           <nnnn> Internal Ram size
      --xstack-loc          <nnnn> External Stack start location
      --code-loc            <nnnn> Code Segment Location
      --code-size           <nnnn> Code Segment size
      --stack-loc           <nnnn> Stack pointer initial value
      --data-loc            <nnnn> Direct data start location
      --idata-loc           
      --no-optsdcc-in-asm   Do not emit .optsdcc in asm

Special options for the z80 port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      -bo                   <num> use code bank <num>
      -ba                   <num> use data bank <num>
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         Do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2
      --sdcccall            Set ABI version for default calling convention
      --allow-undocumented-instructions  Allow use of undocumented instructions

Special options for the sm83 port:
      -bo                   <num> use code bank <num>
      -ba                   <num> use data bank <num>
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --callee-saves-bc     Force a called function to always save BC
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         Do not link default crt0.rel
      --legacy-banking      Use legacy method to call banked functions
      --sdcccall            Set ABI version for default calling convention

Special options for the mos6502 port:
      --model-small         8-bit address space for data
      --model-large         16-bit address space for data (default)
      --no-zp-spill         place register spills in 16-bit address space
      --no-std-crt0         Do not link default crt0.rel

Special options for the mos65c02 port:
      --model-small         8-bit address space for data
      --model-large         16-bit address space for data (default)
      --no-zp-spill         place register spills in 16-bit address space
      --no-std-crt0         Do not link default crt0.rel
```
@anchor sdasgb-settings
# sdasgb settings
```

sdas Assembler V02.00 + NoICE + SDCC mods  (GameBoy)


Copyright (C) 2012  Alan R. Baldwin
This program comes with ABSOLUTELY NO WARRANTY.

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
  -h   or NO ARGUMENTS  Show this help list
Input:
  -I   Add the named directory to the include file
       search path.  This option may be used more than once.
       Directories are searched in the order given.
Output:
  -l   Create list   file/outfile[.lst]
  -o   Create object file/outfile[.rel]
  -s   Create symbol file/outfile[.sym]
Listing:
  -d   Decimal listing
  -q   Octal   listing
  -x   Hex     listing (default)
  -b   Display .define substitutions in listing
  -bb  and display without .define substitutions
  -c   Disable instruction cycle count in listing
  -f   Flag relocatable references by  `   in listing file
  -ff  Flag relocatable references by mode in listing file
  -p   Disable automatic listing pagination
  -u   Disable .list/.nlist processing
  -w   Wide listing format for symbol table
Assembly:
  -v   Enable out of range signed / unsigned errors
Symbols:
  -a   All user symbols made global
  -g   Undefined symbols made global
  -n   Don't resolve global assigned value symbols
  -z   Disable case sensitivity for symbols
Debugging:
  -j   Enable NoICE Debug Symbols
  -y   Enable SDCC  Debug Symbols

```
@anchor sdasz80-settings
# sdasz80 settings
```

sdas Assembler V02.00 + NoICE + SDCC mods  (Zilog Z80 / Hitachi HD64180 / ZX-Next / eZ80 / R800)


Copyright (C) 2012  Alan R. Baldwin
This program comes with ABSOLUTELY NO WARRANTY.

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
  -h   or NO ARGUMENTS  Show this help list
Input:
  -I   Add the named directory to the include file
       search path.  This option may be used more than once.
       Directories are searched in the order given.
Output:
  -l   Create list   file/outfile[.lst]
  -o   Create object file/outfile[.rel]
  -s   Create symbol file/outfile[.sym]
Listing:
  -d   Decimal listing
  -q   Octal   listing
  -x   Hex     listing (default)
  -b   Display .define substitutions in listing
  -bb  and display without .define substitutions
  -c   Disable instruction cycle count in listing
  -f   Flag relocatable references by  `   in listing file
  -ff  Flag relocatable references by mode in listing file
  -p   Disable automatic listing pagination
  -u   Disable .list/.nlist processing
  -w   Wide listing format for symbol table
Assembly:
  -v   Enable out of range signed / unsigned errors
Symbols:
  -a   All user symbols made global
  -g   Undefined symbols made global
  -n   Don't resolve global assigned value symbols
  -z   Disable case sensitivity for symbols
Debugging:
  -j   Enable NoICE Debug Symbols
  -y   Enable SDCC  Debug Symbols

```
@anchor sdas6500-settings
# sdas6500 settings
```

sdas Assembler V02.00 + NoICE + SDCC mods  (Rockwell 6502/6510/65C02)


Copyright (C) 2012  Alan R. Baldwin
This program comes with ABSOLUTELY NO WARRANTY.

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
  -h   or NO ARGUMENTS  Show this help list
Input:
  -I   Add the named directory to the include file
       search path.  This option may be used more than once.
       Directories are searched in the order given.
Output:
  -l   Create list   file/outfile[.lst]
  -o   Create object file/outfile[.rel]
  -s   Create symbol file/outfile[.sym]
Listing:
  -d   Decimal listing
  -q   Octal   listing
  -x   Hex     listing (default)
  -b   Display .define substitutions in listing
  -bb  and display without .define substitutions
  -c   Disable instruction cycle count in listing
  -f   Flag relocatable references by  `   in listing file
  -ff  Flag relocatable references by mode in listing file
  -p   Disable automatic listing pagination
  -u   Disable .list/.nlist processing
  -w   Wide listing format for symbol table
Assembly:
  -v   Enable out of range signed / unsigned errors
Symbols:
  -a   All user symbols made global
  -g   Undefined symbols made global
  -n   Don't resolve global assigned value symbols
  -z   Disable case sensitivity for symbols
Debugging:
  -j   Enable NoICE Debug Symbols
  -y   Enable SDCC  Debug Symbols

```
@anchor bankpack-settings
# bankpack settings
```
bankalloc [options] objfile1 objfile2 etc
Use: Read .o files and auto-assign areas with bank=255.
     Typically called by Lcc compiler driver before linker.

Options
-h             : Show this help
-lkin=<file>   : Load object files specified in linker file <file>
-lkout=<file>  : Write list of object files out to linker file <file>
-yt<mbctype>   : Set MBC type per ROM byte 149 in Decimal or Hex (0xNN)
                ([see pandocs](https://gbdev.io/pandocs/The_Cartridge_Header.html#0147---cartridge-type))
-mbc=N         : Similar to -yt, but sets MBC type directly to N instead
                of by intepreting ROM byte 149
                mbc1 will exclude banks {0x20,0x40,0x60} max=127, 
                mbc2 max=15, mbc3 max=127, mbc5 max=255 (not 511!) 
-min=N         : Min assigned ROM bank is N (default 1)
-max=N         : Max assigned ROM bank is N, error if exceeded
-ext=<.ext>    : Write files out with <.ext> instead of source extension
-path=<path>   : Write files out to <path> (<path> *MUST* already exist)
-sym=<prefix>  : Add symbols starting with <prefix> to match + update list
                 Default entry is "___bank_" (see below)
-cartsize      : Print min required cart size as "autocartsize:<NNN>"
-plat=<plat>   : Select platform specific behavior (default:gb) (gb,sms)
-random        : Distribute banks randomly for testing (honors -min/-max)
-reserve=<b:n> : Reserve N bytes (hex) in bank B (decimal)
                 Ex: -reserve=105:30F reserves 0x30F bytes in bank 105
-banktype=<b:t>: Set bank B (decimal) to use type T (CODE or LIT). For sms/gg
                 Ex: -banktype=2:LIT sets bank 2 to type LIT
-v             : Verbose output, show assignments

Example: "bankpack -ext=.rel -path=some/newpath/ file1.o file2.o"
Unless -ext or -path specify otherwise, input files are overwritten.

Default MBC type is not set. It *must* be specified by -mbc= or -yt!

The following will have FF and 255 replaced with the assigned bank:
A _CODE_255 size <size> flags <flags> addr <address>
S b_<function name> Def0000FF
S ___bank_<const name> Def0000FF
    (Above can be made by: const void __at(255) __bank_<const name>;
```
@anchor sdldgb-settings
# sdldgb settings
```

sdld Linker V03.00/V05.40 + sdld

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
Startup:
  -p   Echo commands to stdout (default)
  -n   No echo of commands to stdout
Alternates to Command Line Input:
  -c                   ASlink >> prompt input
  -f   file[.lk]       Command File input
Libraries:
  -k   Library path specification, one per -k
  -l   Library file specification, one per -l
Relocation:
  -b   area base address = expression
  -g   global symbol = expression
  -a   (platform) Select platform specific virtual address translation
Map format:
  -m   Map output generated as (out)file[.map]
  -w   Wide listing format for map file
  -x   Hexadecimal (default)
  -d   Decimal
  -q   Octal
Output:
  -i   Intel Hex as (out)file[.ihx]
  -s   Motorola S Record as (out)file[.s19]
  -j   NoICE Debug output as (out)file[.noi]
  -y   SDCDB Debug output as (out)file[.cdb]
List:
  -u   Update listing file(s) with link data as file(s)[.rst]
Case Sensitivity:
  -z   Disable Case Sensitivity for Symbols
End:
  -e   or null line terminates input

```
@anchor sdldz80-settings
# sdldz80 settings
```

sdld Linker V03.00/V05.40 + sdld

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
Startup:
  -p   Echo commands to stdout (default)
  -n   No echo of commands to stdout
Alternates to Command Line Input:
  -c                   ASlink >> prompt input
  -f   file[.lk]       Command File input
Libraries:
  -k   Library path specification, one per -k
  -l   Library file specification, one per -l
Relocation:
  -b   area base address = expression
  -g   global symbol = expression
  -a   (platform) Select platform specific virtual address translation
Map format:
  -m   Map output generated as (out)file[.map]
  -w   Wide listing format for map file
  -x   Hexadecimal (default)
  -d   Decimal
  -q   Octal
Output:
  -i   Intel Hex as (out)file[.ihx]
  -s   Motorola S Record as (out)file[.s19]
  -j   NoICE Debug output as (out)file[.noi]
  -y   SDCDB Debug output as (out)file[.cdb]
List:
  -u   Update listing file(s) with link data as file(s)[.rst]
Case Sensitivity:
  -z   Disable Case Sensitivity for Symbols
End:
  -e   or null line terminates input

```
@anchor sdld6808-settings
# sdld6808 settings
```

sdld Linker V03.00/V05.40 + sdld

Usage: [-Options] [-Option with arg] file
Usage: [-Options] [-Option with arg] outfile file1 [file2 ...]
Startup:
  -p   Echo commands to stdout (default)
  -n   No echo of commands to stdout
Alternates to Command Line Input:
  -c                   ASlink >> prompt input
  -f   file[.lk]       Command File input
Libraries:
  -k   Library path specification, one per -k
  -l   Library file specification, one per -l
Relocation:
  -b   area base address = expression
  -g   global symbol = expression
  -a   (platform) Select platform specific virtual address translation
Map format:
  -m   Map output generated as (out)file[.map]
  -w   Wide listing format for map file
  -x   Hexadecimal (default)
  -d   Decimal
  -q   Octal
Output:
  -i   Intel Hex as (out)file[.ihx]
  -s   Motorola S Record as (out)file[.s19]
  -j   NoICE Debug output as (out)file[.noi]
  -y   SDCDB Debug output as (out)file[.cdb]
List:
  -u   Update listing file(s) with link data as file(s)[.rst]
Case Sensitivity:
  -z   Disable Case Sensitivity for Symbols
End:
  -e   or null line terminates input

```
@anchor ihxcheck-settings
# ihxcheck settings
```
ihx_check input_file.ihx [options]

Options
-h : Show this help
-e : Treat warnings as errors

Use: Read a .ihx and warn about overlapped areas.
Example: "ihx_check build/MyProject.ihx"
```
@anchor makebin-settings
# makebin settings
Also see @ref setting_mbc_and_rom_ram_banks
```
makebin: convert a Intel IHX file to binary or GameBoy format binary.
Usage: makebin [options] [<in_file> [<out_file>]]
Options:
  -p             pack mode: the binary file size will be truncated to the last occupied byte
  -s romsize     size of the binary file (default: rom banks * 16384)
  -Z             generate GameBoy format binary file
  -S             generate Sega Master System format binary file
  -N             generate Famicom/NES format binary file
  -o bytes       skip amount of bytes in binary file
SMS format options (applicable only with -S option):
  -xo n          header rom size (0xa-0x2) (default: 0xc)
  -xj n          set region code (3-7) (default: 4)
  -xv n          version number (0-15) (default: 0)
  -yo n          number of rom banks (default: 2) (autosize: A)
  -ya n          number of ram banks (default: 0)
GameBoy format options (applicable only with -Z option):
  -yo n          number of rom banks (default: 2) (autosize: A)
  -ya n          number of ram banks (default: 0)
  -yt n          MBC type (default: no MBC)
  -yl n          old licensee code (default: 0x33)
  -yk cc         new licensee string (default: 00)
  -yn name       cartridge name (default: none)
  -yc            GameBoy Color compatible
  -yC            GameBoy Color only
  -ys            Super GameBoy
  -yS            Convert .noi file named like input file to .sym
  -yj            set non-Japanese region flag
  -yN            do not copy big N validation logo into ROM header
  -yp addr=value Set address in ROM to given value (address 0x100-0x1FE)
Arguments:
  <in_file>      optional IHX input file, '-' means stdin. (default: stdin)
  <out_file>     optional output file, '-' means stdout. (default: stdout)
```
@anchor makecom-settings
# makecom settings
```
makecom image.rom image.noi output.com
Use: convert a binary .rom file to .msxdos com format.
```
@anchor gbcompress-settings
# gbcompress settings
```
gbcompress [options] infile outfile
Use: compress a binary file and write it out.

Options
-h       : Show this help screen
-d       : Decompress (default is compress)
-v       : Verbose output
--cin    : Read input as .c source format (8 bit char ONLY, uses first array found)
--cout   : Write output in .c / .h source format (8 bit char ONLY) 
--varname=<NAME> : specify variable name for c source output
--alg=<type>     : specify compression type: 'rle', 'gb' (default)
--bank=<num>     : Add Bank Ref: 1 - 511 (default is none, with --cout only)
Example: "gbcompress binaryfile.bin compressed.bin"
Example: "gbcompress -d compressedfile.bin decompressed.bin"
Example: "gbcompress --alg=rle binaryfile.bin compressed.bin"

The default compression (gb) is the type used by gbtd/gbmb
The rle compression is Amiga IFF style
```
@anchor png2asset-settings
# png2asset settings
```
usage: png2asset    <file>.png [options]
-o <filename>       ouput file (if not used then default is <png file>.c)
-c <filename>       deprecated, same as -o
-sw <width>         metasprites width size (default: png width)
-sh <height>        metasprites height size (default: png height)
-sp <props>         change default for sprite OAM property bytes (in hex) (default: 0x00)
-px <x coord>       metasprites pivot x coordinate (default: metasprites width / 2)
-py <y coord>       metasprites pivot y coordinate (default: metasprites height / 2)
-pw <width>         metasprites collision rect width (default: metasprites width)
-ph <height>        metasprites collision rect height (default: metasprites height)
-spr8x8             use SPRITES_8x8
-spr8x16            use SPRITES_8x16 (this is the default)
-spr16x16msx        use SPRITES_16x16
-sprite_no_optimize keep empty sprite tiles, do not remove duplicate tiles
-b <bank>           bank (default: fixed bank)
-keep_palette_order use png palette
-repair_indexed_pal try to repair indexed tile palettes (implies "-keep_palette_order")
-noflip             disable tile flip
-map                Export as map (tileset + bg) instead of default metasprite output
-use_map_attributes Use CGB BG Map attributes
-use_nes_attributes Use NES BG Map attributes
-use_nes_colors     Convert RGB color values to NES PPU colors
-use_structs        Group the exported info into structs (default: false) (used by ZGB Game Engine)
-bpp                bits per pixel: 1, 2, 4 (default: 2)
-max_palettes       max number of palettes allowed (default: 8)
                    (note: max colors = max_palettes x num colors per palette)
-pack_mode          gb, nes, sgb, sms, 1bpp (default: gb)
-tile_origin        tile index offset for maps (default: 0)
-tiles_only         export tile data only
-maps_only          export map tilemap only
-metasprites_only   export metasprite descriptors only
-source_tileset     use source tileset (image with common tiles)
-entity_tileset     (maps only) mark matching tiles counting from 255 down, entity patterns not exported
-keep_duplicate_tiles   do not remove duplicate tiles (default: not enabled)
-no_palettes        do not export palette data
-bin                export to binary format
-transposed         export transposed (column-by-column instead of row-by-row)
-rel_paths          paths to tilesets are relative to the input file path
decoder error empty input buffer given to decoder. Maybe caused by non-existing file?
```
@anchor png2hicolorgb-settings
# png2hicolorgb settings
```

png2hicolorgb input_image.png [options]
version 1.4.1: bbbbbr. Based on Glen Cook's Windows GUI "hicolour.exe" 1.2
Convert an image to Game Boy Hi-Color format

Options
-h         : Show this help
-v*        : Set log level: "-v" verbose, "-vQ" quiet, "-vE" only errors, "-vD" debug
-o <file>  : Set base output filename (otherwise from input image)
--csource  : Export C source format with incbins for data files
--bank=N   : Set bank number for C source output where N is decimal bank number 1-511
--type=N   : Set conversion type where N is one of below 
              1: Median Cut - No Dither (*Default*)
              2: Median Cut - With Dither
              3: Wu Quantiser
-p         : Show screen attribute pattern options (no processing)
-L=N       : Set Left  screen attribute pattern where N is decimal entry (-p to show patterns)
-R=N       : Set Right screen attribute pattern where N is decimal entry (-p to show patterns)
--vaddrid  : Map uses vram id (128->255->0->127) instead of (*Default*) sequential tile order (0->255)
--nodedupe : Turn off tile pattern deduplication

Example 1: "png2hicolorgb myimage.png"
Example 2: "png2hicolorgb myimage.png --csource -o=my_output_filename"
* Default settings provide good results. Better quality but slower: "--type=3 -L=2 -R=2"

Historical credits and info:
   Original Concept : Icarus Productions
   Original Code : Jeff Frohwein
   Full Screen Modification : Anon
   Adaptive Code : Glen Cook
   Windows Interface : Glen Cook
   Additional Windows Programming : Rob Jones
   Original Quantiser Code : Benny
   Quantiser Conversion : Glen Cook

```
@anchor romusage-settings
# romusage settings
```
romusage input_file.[map|noi|ihx|cdb|.gb[c]|.pocket|.duck|.gg|.sms] [options]
version 1.2.8, by bbbbbr

Options
-h  : Show this help
-p:SMS_GG : Set platform to GBDK SMS/Game Gear (changes memory map templates)

-a  : Show Areas in each Bank. Optional sort by, address:"-aA" or size:"-aS" 
-g  : Show a small usage graph per bank (-gA for ascii style)
-G  : Show a large usage graph per bank (-GA for ascii style)
-B  : Brief (summarized) output for banked regions. Auto scales max bank
      shows [Region]_[Max Used Bank] / [auto-sized Max Bank Num]
-F  : Force Max ROM and SRAM bank num for -B. (0 based) -F:ROM:SRAM (ex: -F:255:15)

-m  : Manually specify an Area -m:NAME:HEXADDR:HEXLENGTH
-e  : Manually specify an Area that should not overlap -e:NAME:HEXADDR:HEXLENGTH
-E  : All areas are exclusive (except HEADERs), warn for any overlaps
-q  : Quiet, no output except warnings and errors
-Q  : Suppress output of warnings and errors
-R  : Return error code for Area warnings and errors

-sR : [Rainbow] Color output (-sRe for Row Ends, -sRd for Center Dimmed, -sRp % based)
-sP : Custom Color Palette. Colon separated entries are decimal VT100 color codes
      -sP:DEFAULT:ROM:VRAM:SRAM:WRAM:HRAM (section based color only)
-sC : Show Compact Output, hide non-essential columns
-sH : Show HEADER Areas (normally hidden)
-smROM  : Show Merged ROM_0  and ROM_1  output (i.e. bare 32K ROM)
-smWRAM : Show Merged WRAM_0 and WRAM_1 output (i.e DMG/MGB not CGB)
          -sm* compatible with banked ROM_x or WRAM_x when used with -B
-sJ : Show JSON output. Some options not applicable. When used, -Q recommended
-nB : Hide warning banner (for .cdb output)
-nA : Hide areas (shown by default in .cdb output)
-z  : Hide areas smaller than SIZE -z:DECSIZE

Use: Read a .map, .noi, .cdb or .ihx file to display area sizes
Example 1: "romusage build/MyProject.map"
Example 2: "romusage build/MyProject.noi -a -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0"
Example 3: "romusage build/MyProject.ihx -g"
Example 4: "romusage build/MyProject.map -q -R"
Example 5: "romusage build/MyProject.noi -sR -sP:90:32:90:35:33:36"
Example 6: "romusage build/MyProject.map -sRp -g -B -F:255:15 -smROM -smWRAM"

Notes:
  * GBDK / RGBDS map file format detection is automatic.
  * Estimates are as close as possible, but may not be complete.
    Unless specified with -m/-e they *do not* factor regions lacking
    complete ranges in the Map/Noi/Ihx file, for example Shadow OAM and Stack.
  * IHX files can only detect overlaps, not detect memory region overflows.
  * CDB file output ONLY counts (most) data from C sources.
    It cannot count functions and data from ASM and LIBs,
    so bank totals may be incorrect/missing.
  * GB/GBC/ROM files are just guessing, no promises.
```
