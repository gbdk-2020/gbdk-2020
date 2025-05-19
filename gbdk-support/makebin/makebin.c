/*
  makebin - turn a .ihx file into a binary image or GameBoy format binaryimage

  Copyright (c) 2000 Michael Hope
  Copyright (c) 2010 Borut Razem
  Copyright (c) 2012 Noel Lemouel
  Copyright (c) 2020-2021 Sebastian 'basxto' Riedel
  Copyright (c) 2020 'bbbbbr'

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#endif


typedef unsigned char BYTE;

#define BANK_SIZE 16384
#define FILL_BYTE 0xff

int
getnibble (FILE *fin)
{
  int ret;
  int c = getc (fin);

  if (feof (fin) || ferror (fin))
    {
      fprintf (stderr, "error: unexpected end of file.\n");
      exit (6);
    }

  ret = c - '0';
  if (ret > 9)
    {
      ret -= 'A' - '9' - 1;
    }

  if (ret > 0xf)
    {
       ret -= 'a' - 'A';
    }

  if (ret < 0 || ret > 0xf)
    {
      fprintf (stderr, "error: character %02x.\n", ret);
      exit (7);
    }
  return ret;
}

int
getbyte (FILE *fin, int *sum)
{
  int b = (getnibble (fin) << 4) | getnibble (fin);
  *sum += b;
  return b;
}

void
usage (void)
{
  fprintf (stderr,
           "makebin: convert a Intel IHX file to binary or GameBoy format binary.\n"
           "Usage: makebin [options] [<in_file> [<out_file>]]\n"
           "Options:\n"
           "  -p             pack mode: the binary file size will be truncated to the last occupied byte\n"
           "  -s romsize     size of the binary file (default: rom banks * 16384)\n"
           "  -Z             generate GameBoy format binary file\n"
           "  -S             generate Sega Master System format binary file\n"
           "  -N             generate Famicom/NES format binary file\n"
           "  -o bytes       skip amount of bytes in binary file\n"

           "SMS format options (applicable only with -S option):\n"
           "  -xo n          header rom size (0xa-0x2) (default: 0xc)\n"
           "  -xj n          set region code (3-7) (default: 4)\n"
           //"  -xc n          product code (0-159999)\n"
           "  -xv n          version number (0-15) (default: 0)\n"
           "  -yo n          number of rom banks (default: 2) (autosize: A)\n"
           "  -ya n          number of ram banks (default: 0)\n"
           //"  -xV n          SDSC version number\n"
           //"  -xd n          SDSC date\n"
           //"  -xA n          SDSC author pointer\n"
           //"  -xn n          SDSC program name pointer\n"
           //"  -xD n          SDSC description pointer\n"

           "GameBoy format options (applicable only with -Z option):\n"
           "  -yo n          number of rom banks (default: 2) (autosize: A)\n"
           "  -ya n          number of ram banks (default: 0)\n"
           "  -yt n          MBC type (default: no MBC)\n"
           "  -yl n          old licensee code (default: 0x33)\n"
           "  -yk cc         new licensee string (default: 00)\n"
           "  -yn name       cartridge name (default: none)\n"
           "  -yc            GameBoy Color compatible\n"
           "  -yC            GameBoy Color only\n"
           "  -ys            Super GameBoy\n"
           "  -yS            Convert .noi file named like input file to .sym\n"
           "  -yj            set non-Japanese region flag\n"
           "  -yN            do not copy big N validation logo into ROM header\n"
           "  -yp addr=value Set address in ROM to given value (address 0x100-0x1FE)\n"
           "Arguments:\n"
           "  <in_file>      optional IHX input file, '-' means stdin. (default: stdin)\n"
           "  <out_file>     optional output file, '-' means stdout. (default: stdout)\n");
}

#define CART_NAME_LEN 16

struct gb_opt_s
{
  char cart_name[CART_NAME_LEN];  /* cartridge name buffer */
  char licensee_str[2];           /* new licensee string */
  BYTE mbc_type;                  /* MBC type (default: no MBC) */
  short nb_rom_banks;             /* Number of rom banks (default: 2) */
  BYTE nb_ram_banks;              /* Number of ram banks (default: 0) */
  BYTE licensee_id;               /* old licensee code */
  BYTE is_gbc;                    /* 1 if GBC compatible, 2 if GBC only, false for all other*/
  BYTE is_sgb;                    /* True if SGB, false for all other*/
  BYTE sym_conversion;            /* True if .noi file should be converted to .sym (default false)*/
  BYTE non_jp;                    /* True if non-Japanese region, false for all other*/
  BYTE rom_banks_autosize;        /* True if rom banks should be auto-sized (default false)*/
  bool do_logo_copy;              /* True if the nintendo logo should be copied into the ROM (default true) */
  BYTE address_overwrite[16];     /* For limited compatibility with very old versions */
};

struct sms_opt_s
{
  uint8_t rom_size;                  /* Doesn't have to be the real size, needed for checksum */
  uint8_t region_code;               /* Region code Japan/Export/International and SMS/GG */
  uint8_t version;                   /* Game version */
  uint8_t nb_ram_banks;              /* Number of ram banks (default: 0) */
};

void
gb_postproc (BYTE * rom, int size, int *real_size, struct gb_opt_s *o)
{
  int i, chk;
  static const BYTE gb_logo[] =
    {
      0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
      0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
      0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
      0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
      0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
      0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e
    };

  /* $0104-$0133: Nintendo logo
   * If missing, an actual Game Boy won't run the ROM.
   */

  if (o->do_logo_copy)
    {
      memcpy (&rom[0x104], gb_logo, sizeof (gb_logo));
    }

  rom[0x144] = o->licensee_str[0];
  rom[0x145] = o->licensee_str[1];

  /*
   * 0134-0142: Title of the game in UPPER CASE ASCII. If it
   * is less than 16 characters then the
   * remaining bytes are filled with 00's.
   */

  /* capitalize cartridge name */
  for (i = 0; i < CART_NAME_LEN; ++i)
    {
      rom[0x134 + i] = toupper (o->cart_name[i]);
    }

  if (o->is_gbc == 1)
    {
      rom[0x143] = 0x80;
    }

  if (o->is_gbc == 2)
    {
      rom[0x143] = 0xC0;
    }

  if (o->is_sgb)
    {
      rom[0x146] = 0x03;
    }

  /*
  0147: Cartridge type:
  
| Hex Code | MBC Type      | SRAM | Battery | RTC | Extra Feature   | Max ROM Size (1)|Max SRAM Size   |
| -------- | ------------- | ---- | ------- | --- | --------------- | --------------- |--------------- |
| 0x00     | ROM ONLY      |      |         |     |                 | 32 K            |0               |
| 0x01     | MBC-1 (2)     |      |         |     |                 | 2 MB            |0               |
| 0x02     | MBC-1 (2)     | SRAM |         |     |                 | 2 MB            |32 K (5)        |
| 0x03     | MBC-1 (2)     | SRAM | BATTERY |     |                 | 2 MB            |32 K (5)        |
| 0x05     | MBC-2         |      |         |     |                 | 256 K           |512 x 4 bits (6)|
| 0x06     | MBC-2         |      | BATTERY |     |                 | 256 K           |512 x 4 bits (6)|
| 0x08     | ROM (3)       | SRAM |         |     |                 | 32 K            |8 K             |
| 0x09     | ROM (3)       | SRAM | BATTERY |     |                 | 32 K            |8 K             |
| 0x0B     | MMM01         |      |         |     |                 | 8 MB / N        |                |
| 0x0C     | MMM01         | SRAM |         |     |                 | 8 MB / N        |128K / N        |
| 0x0D     | MMM01         | SRAM | BATTERY |     |                 | 8 MB / N        |128K / N        |
| 0x0F     | MBC-3         |      | BATTERY | RTC |                 | 2 MB            |                |
| 0x10     | MBC-3 (4)     | SRAM | BATTERY | RTC |                 | 2 MB            |32 K            |
| 0x11     | MBC-3         |      |         |     |                 | 2 MB            |                |
| 0x12     | MBC-3 (4)     | SRAM |         |     |                 | 2 MB            |32 K            |
| 0x13     | MBC-3 (4)     | SRAM | BATTERY |     |                 | 2 MB            |32 K            |
| 0x19     | MBC-5         |      |         |     |                 | 8 MB            |                |
| 0x1A     | MBC-5         | SRAM |         |     |                 | 8 MB            |128 K           |
| 0x1B     | MBC-5         | SRAM | BATTERY |     |                 | 8 MB            |128 K           |
| 0x1C     | MBC-5         |      |         |     | RUMBLE          | 8 MB            |                |
| 0x1D     | MBC-5         | SRAM |         |     | RUMBLE          | 8 MB            |128 K           |
| 0x1E     | MBC-5         | SRAM | BATTERY |     | RUMBLE          | 8 MB            |128 K           |
| 0x20     | MBC-6         |      |         |     |                 | ~2MB            |                |
| 0x22     | MBC-7         |EEPROM|         |     | ACCELEROMETER   | 2MB             |256 byte EEPROM |
| 0xFC     | POCKET CAMERA |      |         |     |                 | 1MB             |128KB RAM       |
| 0xFD     | BANDAI TAMA5  |      |         |     |                 | To Do           |To Do           |
| 0xFE     | HuC3          |      |         | RTC |                 | To Do           |To Do           |
| 0xFF     | HuC1          | SRAM | BATTERY |     | IR              | To Do           |To Do           |
  */
  rom[0x147] = o->mbc_type;

  /*
   * 0148 ROM size:
   * 0 - 256Kbit = 32KByte = 2 banks
   * 1 - 512Kbit = 64KByte = 4 banks
   * 2 - 1Mbit = 128KByte = 8 banks
   * 3 - 2Mbit = 256KByte = 16 banks
   * 4 - 4Mbit = 512KByte = 32 banks
   * 5 - 8Mbit = 1MByte = 64 banks
   * 6 - 16Mbit = 2MByte = 128 banks
   * $52 - 9Mbit = 1.1MByte = 72 banks
   * $53 - 10Mbit = 1.2MByte = 80 banks
   * $54 - 12Mbit = 1.5MByte = 96 banks
   */
  switch (o->nb_rom_banks)
    {
    case 2:
      rom[0x148] = 0;
      break;

    case 4:
      rom[0x148] = 1;
      break;

    case 8:
      rom[0x148] = 2;
      break;

    case 16:
      rom[0x148] = 3;
      break;

    case 32:
      rom[0x148] = 4;
      break;

    case 64:
      rom[0x148] = 5;
      break;

    case 128:
      rom[0x148] = 6;
      break;

    case 256:
      rom[0x148] = 7;
      break;

    case 512:
      rom[0x148] = 8;
      break;

    default:
      fprintf (stderr, "warning: unsupported number of ROM banks (%d)\n", o->nb_rom_banks);
      rom[0x148] = 0;
      break;
    }

  /*
   * 0149 RAM size:
   * 0 - None
   * 1 - 16kBit = 2kB = 1 bank
   * 2 - 64kBit = 8kB = 1 bank
   * 3 - 256kBit = 32kB = 4 banks
   * 4 - 1MBit =128kB =16 banks
   */
  switch (o->nb_ram_banks)
    {
    case 0:
      rom[0x149] = 0;
      break;

    case 1:
      rom[0x149] = 2;
      break;

    case 4:
      rom[0x149] = 3;
      break;

    case 16:
      rom[0x149] = 4;
      break;

    default:
      fprintf (stderr, "warning: unsupported number of RAM banks (%d)\n", o->nb_ram_banks);
      rom[0x149] = 0;
      break;
    }

  rom[0x14A] = o->non_jp;

  rom[0x14B] = o->licensee_id;

  for (i = 0; i < 16; i+=2)
    {
      if(o->address_overwrite[i] != 0xFF)
        {
          rom[0x0100 | o->address_overwrite[i]] = o->address_overwrite[i+1];
          // warnings for builds ported from ancient GBDK
          fprintf (stderr, "caution: -yp0x01%02x=0x%02x is outdated", o->address_overwrite[i], o->address_overwrite[i+1]);
          if(o->address_overwrite[i] == 0x43)
            switch(o->address_overwrite[i+1]&0xC0)
              {
              case 0x80:
                fprintf (stderr, ", please use -yc instead");
                break;
              case 0xC0:
                fprintf (stderr, ", please use -yC instead");
                break;
              default:
                o->address_overwrite[i] = 0xFF;
              }
          if(o->address_overwrite[i] == 0x44 || o->address_overwrite[i] == 0x45)
              fprintf (stderr, ", please use -yk cc instead");
          if(o->address_overwrite[i] == 0x46)
            if(o->address_overwrite[i+1] == 0x03)
              fprintf (stderr, ", please use -ys instead");
            else
              o->address_overwrite[i] = 0xFF;
          if(o->address_overwrite[i] == 0x47)
            fprintf (stderr, ", please use -yt 0x%02x instead", o->address_overwrite[i+1]);
          if(o->address_overwrite[i] == 0x4A)
            fprintf (stderr, ", please use -yl 0x%02x instead", o->address_overwrite[i+1]);
          if(o->address_overwrite[i] == 0x4B && o->address_overwrite[i+1] == 1)
            fprintf (stderr, ", please use -yj instead");
          if(o->address_overwrite[i] == 0xFF)
            fprintf (stderr, ", this setting is the default");
          fprintf (stderr, ".\n");
        }
    }

  /* Update complement checksum */
  chk = 0;
  for (i = 0x134; i < 0x14d; ++i)
    chk += rom[i];
  rom[0x014d] = (unsigned char) (0xe7 - (chk & 0xff));

  /* Update checksum */
  chk = 0;
  rom[0x14e] = 0;
  rom[0x14f] = 0;
  for (i = 0; i < size; ++i)
    chk += rom[i];
  rom[0x14e] = (unsigned char) ((chk >> 8) & 0xff);
  rom[0x14f] = (unsigned char) (chk & 0xff);

  if (*real_size < 0x150)
    *real_size = 0x150;
}


void
sms_preproc (struct gb_opt_s * gb_opt, struct sms_opt_s * sms_opt, int *size) {

    // If auto-banking is enabled, and RAM banks specified, auto-fix up min ROM size for mapper support if needed
    //
    // Emulators use ROM file size (var:size) (64K or greater) to determine whether a mapper is present, and RAM banks only work if a mapper is present.
    // This is separate from the ROM size indicated in the header (var:o->rom_size) which reportedly is _not_ used for that detection.
    if (gb_opt->rom_banks_autosize && (sms_opt->nb_ram_banks > 0) && (*size < 0x10000)) {
        *size = 0x10000; // force size to 64K
    }
}

void
sms_postproc (BYTE * rom, int size, int *real_size, struct sms_opt_s *o)
{
  // based on https://www.smspower.org/Development/ROMHeader
  // 0x1ff0 and 0x3ff0 are also possible, but never used
  static const char tmr_sega[] = "TMR SEGA  ";
  short header_base = 0x7ff0;
  int chk = 0;
  unsigned long i;

  // Emulators use ROM file size (var:size) (64K or greater) to determine whether a mapper is present, and RAM banks only work if a mapper is present.
  // So warn if that criteria is not met.
  // This is separate from the ROM size indicated in the header (var:o->rom_size) which reportedly is _not_ used for that detection.
  if ((o->nb_ram_banks > 0) && (size < 0x10000)) {
    fprintf (stderr, "\nWARNING: SMS/GG ROM size (%d) must be at least 64K to enable mapper support for RAM banks (%d specified) in emulators."
                     "\n         \"-yo 4\" for makebin (or \"-Wm-yo4\" for LCC) can be used to set the size to 64K\n\n", size, o->nb_ram_banks);
  }

  // choose earlier positions for smaller roms
  if (header_base > size)
    header_base = 0x3ff0;
  if (header_base > size)
    header_base = 0x1ff0;

  memcpy (&rom[header_base], tmr_sega, sizeof (tmr_sega) - 1);
  // configure amounts of bytes to check
  switch(o->rom_size)
    {
      case 0xa:
      default:
        i = 0x1FEF;
        break;
      case 0xb:
        i = 0x3FEF;
        break;
      case 0xc:
        i = 0x7FEF;
        break;
      case 0xd:
        i = 0xBFEF;
        break;
      case 0xe:
        i = 0xFFFF;
        break;
      case 0xf:
        i = 0x1FFFF;
        break;
      case 0x0:
        i = 0x3FFFF;
        break;
      case 0x1:
        i = 0x7FFFF;
        break;
      case 0x2:
        i = 0xFFFFF;
        break;
    }
  // calculate checksum
  for(;i > 0; --i)
    {
      chk += rom[i];
      // 0x7FF0 - 0x7FFF is skipped
      if(i == 0x8000)
        i = 0x7FF0;
    }
  // we  skipped index 0
  chk += rom[0];
  // little endian
  rom[header_base + 0xa] = chk & 0xff;
  rom[header_base + 0xb] = (chk>>8) & 0xff;
  // game version
  rom[header_base + 0xe] &= 0xF0;
  rom[header_base + 0xe] |= o->version;
  // rom size
  rom[header_base + 0xf] = (o->region_code << 4) | o->rom_size;
}

int
rom_autosize_grow(BYTE **rom, int test_size, int *size, struct gb_opt_s *o)
{
  int last_size = *size;

  while ((test_size > *size) && (o->nb_rom_banks <= 512))
    {
      o->nb_rom_banks *= 2;
      // banks work differently for mbc6, they have half the size
      // but this in general ignored by -yo
      *size = o->nb_rom_banks * 0x4000;
    }

  if (o->nb_rom_banks > 512)
    {
      fprintf (stderr, "error: auto-size banks exceeded max of 512 banks.\n");
      return 0;
    }
  else
    {
      BYTE * t_rom = *rom;
      *rom = realloc (*rom, *size);
      if (*rom == NULL)
        {
          free(t_rom);
          fprintf (stderr, "error: couldn't re-allocate size for larger rom image.\n");
          return 0;
        }
      memset (*rom + last_size, FILL_BYTE, *size - last_size);
    }

  return 1;
}

int
noi2sym (char *filename)
{
  FILE *noi, *sym;
  char *nname, *sname;
  //ssize_t read;
  int read = ' ';
  // no$gmb's implementation is limited to 32 character labels
  // we can safely throw away the rest
  #define SYM_FILE_NAME_LEN_MAX 32
  char label[SYM_FILE_NAME_LEN_MAX + 1];
  // 0x + 6 digit hex number
  // -> 65536 rom banks is the maximum homebrew cartrideges support (TPP1)
  char value[9];
  int name_len = strlen(filename);
  int i = 0;
  // copy filename's value to nname and sname
  nname = malloc((name_len+1) * sizeof(char));
  strcpy (nname, filename);
  sname = malloc((name_len+1) * sizeof(char));
  strcpy (sname, filename);
  // change the extensions
  nname[name_len-1]='i';
  nname[name_len-2]='o';
  nname[name_len-3]='n';
  sname[name_len-1]='m';
  sname[name_len-2]='y';
  sname[name_len-3]='s';

  if (NULL == (noi = fopen (nname, "r")))
    {
      fprintf (stderr, "error: can't open %s: ", nname);
      perror(NULL);
      return 1;
    }
  if (NULL == (sym = fopen (sname, "w")))
    {
      fprintf (stderr, "error: can't create %s: ", sname);
      perror(NULL);
      return 1;
    }
  // write header
  fprintf (sym, "; no$gmb compatible .sym file\n; Generated automagically by makebin\n");
  // iterate through .noi file
  while (read != EOF && (read = fgetc(noi)) != EOF)
    {
      // just skip line breaks
      if (read == '\r' || read == '\n')
        continue;
      // read first 4 chars
      for (i = 0; i < 4; ++i)
        {
          value[i] = read;
          if ((read = fgetc(noi)) == EOF || read == '\r' || read == '\n')
            {
              // leave for-loop
              break;
            }
        }
      // we left loop early
      if (i != 4)
        continue;
      // only accept if line starts with this
      if (strncmp(value, "DEF ", 4) == 0)
        {
          // read label
          for (i = 0; i < (SYM_FILE_NAME_LEN_MAX - 1); ++i)
            {
              label[i] = read;
              if ((read = fgetc(noi)) == EOF || read == '\r' || read == '\n' || read == ' ')
                {
                  // leave for-loop
                  break;
                }
            }
          // skip rest of the label
          while (read != EOF && read != '\r' && read != '\n' && read != ' ')
            read = fgetc(noi);
          // it has to be end of file or line if it's not space
          if (read != ' ')
            continue;
          // strings have to end with \0
          label[i+1] = '\0';
          // read value
          for (i = 0; i < 8; ++i)
            {
              value[i] = read;
              if ((read = fgetc(noi)) == EOF || read == '\r' || read == '\n')
                {
                  // leave for-loop
                  break;
                }
            }
          // number is too long; ignore
          if (read != EOF && read != '\r' && read != '\n')
            continue;
          value[i+1] = '\0';
          // we successfully read label and value

          // but filter out some invalid symbols
          if (strcmp(label, ".__.ABS.") != 0)
            fprintf (sym, "%02X:%04X %s\n", (unsigned int)(strtoul(value, NULL, 0)>>16), (unsigned int)strtoul(value, NULL, 0)&0xFFFF, label);
        }
      else
        // skip until file/line end
        while ((read = fgetc(noi))!= EOF && read != '\r' && read != '\n');
    }

  // free close files
  fclose (noi);
  fclose (sym);

  fprintf (stderr, "Converted %s to %s.\n", nname, sname);
  return 0;
}

int
read_ihx (FILE *fin, BYTE **rom, int *size, int *real_size, struct gb_opt_s *o)
{
  int record_type;

  int extaddr = 0;
  do
    {
      int nbytes;
      int addr;
      int checksum, sum = 0;

      if (getc (fin) != ':')
        {
          fprintf (stderr, "error: invalid IHX line.\n");
          return 0;
        }
      nbytes = getbyte (fin, &sum);
      addr = getbyte (fin, &sum) << 8 | getbyte (fin, &sum);
      record_type = getbyte (fin, &sum);
      if(record_type == 4)
        {
          extaddr = getbyte (fin, &sum) << 8 | getbyte (fin, &sum);
          extaddr <<= 16; // those are the upper 16 bits
          checksum = getbyte (fin, &sum);
          // move to the next record
          if (0 != (sum & 0xff))
            {
              fprintf (stderr, "error: bad checksum: %02x.\n", checksum);
              return 0;
            }
          while (isspace (sum = getc (fin)))  /* skip all kind of spaces */
            ;
          ungetc (sum, fin);
          if (getc (fin) != ':')
            {
              fprintf (stderr, "error: invalid IHX line.\n");
              return 0;
            }
          // parse real data part
          checksum = sum = 0;
          nbytes = getbyte (fin, &sum);
          // lower 16 bits
          addr = getbyte (fin, &sum) << 8 | getbyte (fin, &sum);
          record_type = getbyte (fin, &sum);
        }
      // add linear address extension
      addr |= extaddr;
      // TODO: warn for unreachable banks according to chosen MBC
      if (record_type > 1)
        {
          fprintf (stderr, "error: unsupported record type: %02x.\n", record_type);
          return 0;
        }

      if (nbytes > 0 && (addr + nbytes > *size || (addr < 0 && nbytes > 0)))
        {
          // If auto-size is enabled, grow rom bank size by power of 2 when needed
          if (o->rom_banks_autosize)
            {
              if (rom_autosize_grow(rom, addr + nbytes, size, o) == 0)
                return 0;
            }
          else
            {
              fprintf (stderr, "error: ROM is too large for number of banks specified\n");
              return 0;
            }
        }
      while (nbytes--)
        {
          if (addr < *size)
            (*rom)[addr++] = getbyte (fin, &sum);
        }

      if (addr > *real_size)
        *real_size = addr;

      checksum = getbyte (fin, &sum);
      if (0 != (sum & 0xff))
        {
          fprintf (stderr, "error: bad checksum: %02x.\n", checksum);
          return 0;
        }

      while (isspace (sum = getc (fin)))  /* skip all kind of spaces */
        ;
      ungetc (sum, fin);
    }
  while (1 != record_type); /* EOF record */

  return 1;
}

int
main (int argc, char **argv)
{
  int size = 32768, offset = 0, pack = 0, real_size = 0, i = 0;
  char *token;
  BYTE *rom;
  FILE *fin, *fout;
  char *filename = NULL;
  int ret;
  int gb = 0;
  int sms = 0;
  int nes = 0;

  struct gb_opt_s gb_opt = {.cart_name="",
                            .licensee_str={'0', '0'},
                            .mbc_type=0,
                            .nb_rom_banks=2,
                            .nb_ram_banks=0,
                            .licensee_id=0x33,
                            .is_gbc=0,
                            .is_sgb=0,
                            .sym_conversion=0,
                            .non_jp=0,
                            .rom_banks_autosize=0,
                            .do_logo_copy=true,
                            .address_overwrite={0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0xFF, 0} };

  // 32KiB, SMS Export, version 0 <- should work with most emulaters (<32K was never used, GG accepts SMS)
  struct sms_opt_s sms_opt = {.rom_size=0xc,
                              .region_code=4,
                              .version=0,
                              .nb_ram_banks=0 };

#if defined(_WIN32)
  setmode (fileno (stdout), O_BINARY);
#endif

  while (*++argv && '-' == argv[0][0] && '\0' != argv[0][1])
    {
      switch (argv[0][1])
        {
        case 's':
          if (!*++argv)
            {
              usage ();
              return 1;
            }
          size = strtoul (*argv, NULL, 0);
          break;

        case 'o':
          if (!*++argv)
            {
              usage ();
              return 1;
            }
          offset = strtoul (*argv, NULL, 0);
          break;

        case 'h':
          usage ();
          return 0;

        case 'p':
          pack = 1;
          break;

        case 'Z':
          /* generate GameBoy binary file */
          gb = 1, sms = 0, nes = 0;
          break;

        case 'y':
          /* GameBoy options:
           * -yo  Number of rom banks (default: 2)
           * -ya  Number of ram banks (default: 0)
           * -yt  MBC type (default: no MBC)
           * -yn  Name of program (default: name of output file)
           */
          switch (argv[0][2])
            {
            case 'o':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              // Use auto-size for rom banks if -yto size param is 'A'
              if ((*argv)[0] == 'A' || (*argv)[0] == 'a')
                  gb_opt.rom_banks_autosize = 1;
              else
                {
                  gb_opt.nb_rom_banks = strtoul (*argv, NULL, 0);
                  size = gb_opt.nb_rom_banks * 0x4000;
                }
              break;

            case 'a':
              if (!++argv)
                {
                  usage ();
                  return 1;
                }
              gb_opt.nb_ram_banks  = strtoul (*argv, NULL, 0);
              sms_opt.nb_ram_banks = strtoul (*argv, NULL, 0);
              break;

            case 't':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              gb_opt.mbc_type = strtoul (*argv, NULL, 0);
              break;

            case 'n':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              strncpy (gb_opt.cart_name, *argv, CART_NAME_LEN-1);
              gb_opt.cart_name[CART_NAME_LEN-1] = '\0';
              break;

            case 'k':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              // we don't need \0
              strncpy (gb_opt.licensee_str, *argv, 2);
              break;

            case 'l':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              gb_opt.licensee_id = strtoul (*argv, NULL, 0);
              break;

            case 'c':
              gb_opt.is_gbc = 1;
              break;

            case 'C':
              gb_opt.is_gbc = 2;
              break;

            case 'N':
              gb_opt.do_logo_copy = false; // when switch is present, turn off logo copy
              break;

            case 's':
              gb_opt.is_sgb = 1;
              break;

            case 'S':
              gb_opt.sym_conversion = 1;
              break;

            case 'j':
              gb_opt.non_jp = 1;
              break;

            // like -yp0x143=0x80
            case 'p':
              // remove "-yp"
              *argv += 3;

              // also support -yp 0x143=0x80
              if (!(*argv)[0])
                if (!*++argv)
                  {
                    usage ();
                    return 1;
                  }

              // effectively split string into argv and token
              strtok(*argv, "=");
              token = strtok(NULL, "=");
              for (i = 0; i < 16; i+=2)
                {
                  if (gb_opt.address_overwrite[i] == 0xFF)
                    {
                      gb_opt.address_overwrite[i] = strtoul (*argv, NULL, 0);
                      gb_opt.address_overwrite[i+1] = strtoul (token, NULL, 0);
                      break;
                    }
                }
              break;

            default:
              usage ();
              return 1;
            }
          break;

        case 'S':
          /* generate SMS binary file */
          gb = 0, sms = 1, nes = 0;
          break;

        case 'N':
          /* generate binary file for NES, with .ihx file banks rotated to turn first bank into last bank */
          gb = 0, sms = 0, nes = 1;
          break;

        case 'x':

          switch (argv[0][2])
            {
            case 'o':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              sms_opt.rom_size = strtoul (*argv, NULL, 0);
              if ( sms_opt.rom_size > 2 && (sms_opt.rom_size < 0xa || sms_opt.rom_size > 0xf ) )
                {
                  fprintf (stderr, "error: invalid rom size (0x%X)", sms_opt.rom_size);
                  perror(NULL);
                  return 1;
                }
              if ( sms_opt.rom_size == 0xd || sms_opt.rom_size == 0x2 )
                {
                  fprintf (stderr, "warning: this rom size (0x%X) is bugged in some BIOSes\n", sms_opt.rom_size);
                }
              break;

            case 'j':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              sms_opt.region_code = strtoul (*argv, NULL, 0);
              if ( sms_opt.region_code < 3 && sms_opt.region_code > 7 )
                {
                  fprintf (stderr, "error: invalid region code (0x%X)", sms_opt.region_code);
                  perror(NULL);
                  return 1;
                }
              break;

            case 'v':
              if (!*++argv)
                {
                  usage ();
                  return 1;
                }
              sms_opt.version = strtoul (*argv, NULL, 0);
              if ( sms_opt.version > 0xf )
                {
                  fprintf (stderr, "error: invalid version (0x%X)", sms_opt.version);
                  perror(NULL);
                  return 1;
                }
              break;

            default:
              usage ();
              return 1;
            }
          break;

        default:
          usage ();
          return 1;
        }
    }

  fin = stdin;
  fout = stdout;
  if (*argv)
    {
      if ('-' != argv[0][0] || '\0' != argv[0][1])
        {
          if (NULL == (fin = fopen (*argv, "r")))
            {
              fprintf (stderr, "error: can't open %s: ", *argv);
              perror(NULL);
              return 1;
            }
          filename = *argv;
        }
      ++argv;
    }

  if (NULL != argv[0] && NULL != argv[1])
    {
      usage ();
      return 1;
    }

  // If auto-banking is enabled, and sms/gg has RAM banks specified, fix up min ROM size if needed
  if (sms) sms_preproc(&gb_opt, &sms_opt, &size);

  rom = malloc (size);
  if (rom == NULL)
    {
      fclose (fin);
      fprintf (stderr, "error: couldn't allocate room for the image.\n");
      return 1;
    }
  memset (rom, FILL_BYTE, size);

  if (gb_opt.sym_conversion == 1)
    {
      if (filename)
        noi2sym(filename);
      else
        {
          fprintf (stderr, "error: .noi to .sym conversion needs an input file.\n");
        }
    }

  ret = read_ihx (fin, &rom, &size, &real_size, &gb_opt);

  fclose (fin);

  if (ret)
    {
      if (gb)
        gb_postproc (rom, size, &real_size, &gb_opt);
      else if (sms)
        sms_postproc (rom, size, &real_size, &sms_opt);

      if (*argv)
        {
          if ('-' != argv[0][0] || '\0' != argv[0][1])
            {
              if (NULL == (fout = fopen (*argv, "wb")))
                {
                  fprintf (stderr, "error: can't create %s: ", *argv);
                  perror(NULL);
                  return 1;
                }
            }
        }
      // skip offset
      if (offset > 0)
        {
          memmove (rom, rom + offset, size - offset);
          memset (rom + size - offset, FILL_BYTE, offset);
        }

      if (nes)
      {
        // .ihx file has fixed bank incorrectly placed as first - we fix this when writing out the .bin file.
        // Write the N-1 switchable banks at .nes file start, skipping the first (fixed) bank
        fwrite (rom + BANK_SIZE, 1, (pack ? real_size : size) - offset - BANK_SIZE, fout);
        // Write the fixed bank to end of .nes file
        fwrite (rom, 1, BANK_SIZE, fout);
      }
      else
      {
        fwrite (rom, 1, (pack ? real_size : size) - offset, fout);
      }

      fclose (fout);

      return 0;
    }
  else
    return 1;
}

