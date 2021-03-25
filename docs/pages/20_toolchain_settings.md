@page docs_toolchain_settings Toolchain settings
@anchor lcc-settings
# lcc settings
```
./lcc [ option | file ]...
	except for -l, options are processed left-to-right before files
	unrecognized options are taken to be linker options
-A	warn about nonANSI usage; 2nd -A warns more
-b	emit expression-level profiling code; see bprint(1)
-Bdir/	use the compiler named `dir/rcc'
-c	compile only
-dn	set switch statement density to `n'
-debug	turn on --debug for compiler, -y (.cdb) and -j (.noi) for linker
-Dname -Dname=def	define the preprocessor symbol `name'
-E	run only the preprocessor on the named C programs and unsuffixed files
-g	produce symbol table information for debuggers
-help or -?	print this message
-Idir	add `dir' to the beginning of the list of #include directories
-K don't run ihxcheck test on linker ihx output
-lx	search library `x'
-N	do not search the standard directories for #include files
-n	emit code to check for dereferencing zero pointers
-O	is ignored
-o file	leave the output in `file'
-P	print ANSI-style declarations for globals
-p -pg	emit profiling code; see prof(1) and gprof(1)
-S	compile to assembly language
-autobank auto-assign banks set to 255 (bankpack)
-static	specify static libraries (default is dynamic)
-t -tname	emit function tracing calls to printf or to `name'
-target name	is ignored
-tempdir=dir	place temporary files in `dir/'; default=/tmp
-Uname	undefine the preprocessor symbol `name'
-v	show commands as they are executed; 2nd -v suppresses execution
-w	suppress warnings
-Woarg	specify system-specific `arg'
-W[pfablim]arg	pass `arg' to the preprocessor, compiler, assembler, bankpack, linker, ihxcheck, or makebin
```
@anchor sdcc-settings
# sdcc settings
```
SDCC : mcs51/z80/z180/r2k/r2ka/r3ka/gbz80/tlcs90/ez80_z80/z80n/ds390/pic16/pic14/TININative/ds400/hc08/s08/stm8/pdk13/pdk14/pdk15 4.1.0 #12069 (Linux)
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
  -S                        Compile only; do not assemble or link
  -c  --compile-only        Compile and assemble, but do not link
  -E  --preprocessonly      Preprocess only, do not compile
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
      --std-c89             Use ISO C90 (aka ANSI C89) standard (slightly incomplete)
      --std-sdcc89          Use ISO C90 (aka ANSI C89) standard with SDCC extensions
      --std-c95             Use ISO C95 (aka ISO C94) standard (slightly incomplete)
      --std-c99             Use ISO C99 standard (incomplete)
      --std-sdcc99          Use ISO C99 standard with SDCC extensions
      --std-c11             Use ISO C11 standard (incomplete)
      --std-sdcc11          Use ISO C11 standard with SDCC extensions (default)
      --std-c2x             Use ISO C2X standard (incomplete)
      --std-sdcc2x          Use ISO C2X standard with SDCC extensions
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
      --profile             On supported ports, generate extra profiling information
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
      --nooverlay           Disable overlaying leaf function auto variables
      --nogcse              Disable the GCSE optimisation
      --nolabelopt          Disable label optimisation
      --noinvariant         Disable optimisation of invariants
      --noinduction         Disable loop variable induction
      --noloopreverse       Disable the loop reverse optimisation
      --no-peep             Disable the peephole assembly file optimisation
      --no-reg-params       On some ports, disable passing some parameters in registers
      --peep-asm            Enable peephole optimization on inline assembly
      --peep-return         Enable peephole optimization for return instructions
      --no-peep-return      Disable peephole optimization for return instructions
      --peep-file           <file> use this extra peephole file
      --opt-code-speed      Optimize for code speed rather than size
      --opt-code-size       Optimize for code size rather than speed
      --max-allocs-per-node  Maximum number of register assignments considered at each node of the tree decomposition
      --nolospre            Disable lospre
      --allow-unsafe-read   Allow optimizations to read any memory location anytime
      --nostdlibcall        Disable optimization of calls to standard library

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

Special options for the mcs51 port:
      --model-small         internal data space is used (default)
      --model-medium        external paged data space is used
      --model-large         external data space is used
      --model-huge          functions are banked, data in external space
      --stack-size          Tells the linker to allocate this space for stack
      --parms-in-bank1      use Bank1 for parameter passing
      --acall-ajmp          Use acall/ajmp instead of lcall/ljmp
      --no-ret-without-call  Do not use ret independent of acall/lcall

Special options for the z80 port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the z180 port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the r2k port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the r2ka port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the r3ka port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the gbz80 port:
      -bo                   <num> use code bank <num>
      -ba                   <num> use data bank <num>
      --callee-saves-bc     Force a called function to always save BC
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --legacy-banking      Use legacy method to call banked functions

Special options for the tlcs90 port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the ez80_z80 port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the z80n port:
      --callee-saves-bc     Force a called function to always save BC
      --portmode=           Determine PORT I/O mode (z80/z180)
      --asm=                Define assembler name (rgbds/asxxxx/isas/z80asm/gas)
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --dataseg             <name> use this name for the data segment
      --no-std-crt0         For the z80/gbz80 do not link default crt0.rel
      --reserve-regs-iy     Do not use IY (incompatible with --fomit-frame-pointer)
      --oldralloc           Use old register allocator
      --fno-omit-frame-pointer  Do not omit frame pointer
      --emit-externs        Emit externs list in generated asm
      --legacy-banking      Use legacy method to call banked functions
      --nmos-z80            Generate workaround for NMOS Z80 when saving IFF2

Special options for the ds390 port:
      --model-flat24        use the flat24 model for the ds390 (default)
      --stack-8bit          use the 8bit stack for the ds390 (not supported yet)
      --stack-size          Tells the linker to allocate this space for stack
      --stack-10bit         use the 10bit stack for ds390 (default)
      --use-accelerator     generate code for ds390 arithmetic accelerator
      --protect-sp-update   will disable interrupts during ESP:SP updates
      --parms-in-bank1      use Bank1 for parameter passing

Special options for the pic16 port:
      --pstack-model=       use stack model 'small' (default) or 'large'
  -y  --extended            enable Extended Instruction Set/Literal Offset Addressing mode
      --pno-banksel         do not generate BANKSEL assembler directives
      --obanksel=           set banksel optimization level (default=0 no)
      --denable-peeps       explicit enable of peepholes
      --no-optimize-goto    do NOT use (conditional) BRA instead of GOTO
      --optimize-cmp        try to optimize some compares
      --optimize-df         thoroughly analyze data flow (memory and time intensive!)
      --asm=                Use alternative assembler
      --mplab-comp          enable compatibility mode for MPLAB utilities (MPASM/MPLINK)
      --link=               Use alternative linker
      --preplace-udata-with=  Place udata variables at another section: udata_acs, udata_ovr, udata_shr
      --ivt-loc=            Set address of interrupt vector table.
      --nodefaultlibs       do not link default libraries when linking
      --use-crt=            use <crt-o> run-time initialization module
      --no-crt              do not link any default run-time initialization module
      --debug-xtra          show more debug info in assembly output
      --debug-ralloc        dump register allocator debug file *.d
      --pcode-verbose       dump pcode related info
      --calltree            dump call tree in .calltree file
      --gstack              trace stack pointer push/pop to overflow
      --no-warn-non-free    suppress warning on absent --use-non-free option

Special options for the pic14 port:
      --debug-xtra          show more debug info in assembly output
      --no-pcode-opt        disable (slightly faulty) optimization on pCode
      --stack-size          sets the size if the argument passing stack (default: 16, minimum: 4)
      --no-extended-instructions  forbid use of the extended instruction set (e.g., ADDFSR)
      --no-warn-non-free    suppress warning on absent --use-non-free option

Special options for the TININative port:
      --model-flat24        use the flat24 model for the ds390 (default)
      --stack-8bit          use the 8bit stack for the ds390 (not supported yet)
      --stack-size          Tells the linker to allocate this space for stack
      --stack-10bit         use the 10bit stack for ds390 (default)
      --use-accelerator     generate code for ds390 arithmetic accelerator
      --protect-sp-update   will disable interrupts during ESP:SP updates
      --parms-in-bank1      use Bank1 for parameter passing
      --tini-libid          <nnnn> LibraryID used in -mTININative

Special options for the ds400 port:
      --model-flat24        use the flat24 model for the ds400 (default)
      --stack-8bit          use the 8bit stack for the ds400 (not supported yet)
      --stack-size          Tells the linker to allocate this space for stack
      --stack-10bit         use the 10bit stack for ds400 (default)
      --use-accelerator     generate code for ds400 arithmetic accelerator
      --protect-sp-update   will disable interrupts during ESP:SP updates
      --parms-in-bank1      use Bank1 for parameter passing

Special options for the hc08 port:
      --model-small         8-bit address space for data
      --model-large         16-bit address space for data (default)
      --out-fmt-elf         Output executable in ELF format
      --oldralloc           Use old register allocator

Special options for the s08 port:
      --model-small         8-bit address space for data
      --model-large         16-bit address space for data (default)
      --out-fmt-elf         Output executable in ELF format
      --oldralloc           Use old register allocator

Special options for the stm8 port:
      --model-medium        16-bit address space for both data and code (default)
      --model-large         16-bit address space for data, 24-bit for code
      --codeseg             <name> use this name for the code segment
      --constseg            <name> use this name for the const segment
      --out-fmt-elf         Output executable in ELF format
```
@anchor sdasgb-settings
# sdasgb settings
```

sdas Assembler V02.00 + NoICE + SDCC mods  (GameBoy Z80-like CPU)


Copyright (C) 2012  Alan R. Baldwin
This program comes with ABSOLUTELY NO WARRANTY.

Usage: [-Options] file
Usage: [-Options] outfile file1 [file2 file3 ...]
  -d   Decimal listing
  -q   Octal   listing
  -x   Hex     listing (default)
  -g   Undefined symbols made global
  -a   All user symbols made global
  -b   Display .define substitutions in listing
  -bb  and display without .define substitutions
  -c   Disable instruction cycle count in listing
  -j   Enable NoICE Debug Symbols
  -y   Enable SDCC  Debug Symbols
  -l   Create list   file/outfile[.lst]
  -o   Create object file/outfile[.rel]
  -s   Create symbol file/outfile[.sym]
  -p   Disable automatic listing pagination
  -u   Disable .list/.nlist processing
  -w   Wide listing format for symbol table
  -z   Disable case sensitivity for symbols
  -f   Flag relocatable references by  `   in listing file
  -ff  Flag relocatable references by mode in listing file
  -I   Add the named directory to the include file
       search path.  This option may be used more than once.
       Directories are searched in the order given.

removing 
```
@anchor bankpack-settings
# bankpack settings
```
bankalloc [options] objfile1 objfile2 etc
Use: Read .o files and auto-assign areas with bank=255.
     Typically called by Lcc compiler driver before linker.

Options
-h           : Show this help
-yt<hexbyte> : Set MBC type per ROM byte 149 in Hex (see pandocs)
-mbc=N       : Similar to -yt, but sets MBC type directly to N instead
               of by intepreting ROM byte 149
               mbc1 will exclude banks {0x20,0x40,0x60} max=127, 
               mbc2 max=15, mbc3 max=127, mbc5 max=255 (not 511!) 
-min=N       : Min assigned ROM bank is N (default 1)
-max=N       : Max assigned ROM bank is N, error if exceeded
-ext=<.ext>  : Write files out with <.ext> instead of source extension
-path=<path> : Write files out to <path> (<path> *MUST* already exist)
-sym=<prefix>: Add symbols starting with <prefix> to match + update list.
               Default entry is "___bank_" (see below)
-cartsize    : Print min required cart size as "autocartsize:<NNN>"
-v           : Verbose output, show assignments

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

sdld Linker V03.00 + NoICE + sdld

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
```
makebin: convert a Intel IHX file to binary or GameBoy format binary.
Usage: makebin [options] [<in_file> [<out_file>]]
Options:
  -p             pack mode: the binary file size will be truncated to the last occupied byte
  -s romsize     size of the binary file (default: rom banks * 16384)
  -Z             genarate GameBoy format binary file
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
  -yp addr=value Set address in ROM to given value (address 0x100-0x1FE)
Arguments:
  <in_file>      optional IHX input file, '-' means stdin. (default: stdin)
  <out_file>     optional output file, '-' means stdout. (default: stdout)
```
@anchor gbcompress-settings
# gbcompress settings
```
gbcompress [options] infile outfile
Use: Gbcompress a binary file and write it out.

Options
-h : Show this help screen
-d : Decompress (default is compress)
-v : Verbose output
Example: "gbcompress binaryfile.bin compressed.bin"
Example: "gbcompress -d compressedfile.bin decompressed.bin"
```
@anchor png2mtspr-settings
# png2mtspr settings
```
usage: png2mtspr <file>.png [options]
-c            ouput file (default: <png file>.c)
-sw <width>   metasprites width size (default: png width)
-sh <height>  metasprites height size (default: png height)
-px <x coord> metasprites pivot x coordinate (default: metasprites width / 2)
-py <y coord> metasprites pivot y coordinate (default: metasprites height / 2)
-spr8x8       use SPRITES_8x8 (default: SPRITES_8x16)
-spr8x16      use SPRITES_8x16 (default: SPRITES_8x16)
-b <bank>     bank (default 0)
```
