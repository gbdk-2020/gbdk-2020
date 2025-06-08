/*
  makenes - Prepend an iNES header to a binary ROM image

  Copyright (c) 2025 Michel Iwaniec

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

#define BANK_SIZE 16384
uint8_t bank_data[BANK_SIZE];   // Static work ram for copying banks from input to output file

void usage(void)
{
  fprintf (stderr,
           "makenes: Prepend an iNES header to a binary ROM image file.\n"
           "Usage: makenes [options] [<in_file> [<out_file>]]\n"
           "Options:\n"
           "  -m   Mapper number (default: 0)\n"
           "  -n   Nametable arrangement:\n"
           "                              0: vertical arrangement / horizontal mirroring\n"
           "                              1: horizontal arrangement / vertical mirroring\n"
           "  -b   Battery bit set (default: 0)\n"
           "  -a   Alternative nametable layout (default: 0)\n"

           "Arguments:\n"
           "  <in_file>      optional binary input file, '-' means stdin. (default: stdin)\n"
           "  <out_file>     optional .nes output file, '-' means stdout. (default: stdout)\n");
}

struct nes_opt_s
{
    uint8_t mapper;
    uint8_t num_prg_banks;
    uint8_t num_chr_banks;
    bool nametable_layout;
    bool alternative_nametables;
    bool battery;
};

void write_ines_header(FILE* fout, struct nes_opt_s* nes_opt)
{
    char id_string[] = { 0x4E, 0x45, 0x53, 0x1A };  
    uint8_t flags6 = ((nes_opt->mapper & 0xF) << 4) | 
                      (nes_opt->alternative_nametables << 3) |
                      (nes_opt->battery << 1) |
                      (nes_opt->nametable_layout << 0);
    uint8_t flags7 = (nes_opt->mapper & 0xF0);
    uint8_t flags8 = 0;
    uint8_t flags9 = 0;
    uint8_t flags10 = 0;
    uint8_t padding[5] = { 0, 0, 0, 0, 0, };
    // "NES" + end-of-file
    fwrite(&id_string, sizeof(char), 4, fout);
    fwrite(&nes_opt->num_prg_banks, sizeof(uint8_t), 1, fout);
    fwrite(&nes_opt->num_chr_banks, sizeof(uint8_t), 1, fout);
    fwrite(&flags6, sizeof(uint8_t), 1, fout);
    fwrite(&flags7, sizeof(uint8_t), 1, fout);
    fwrite(&flags8, sizeof(uint8_t), 1, fout);
    fwrite(&flags9, sizeof(uint8_t), 1, fout);
    fwrite(&flags10, sizeof(uint8_t), 1, fout);
    fwrite(padding, sizeof(uint8_t), 5, fout);
}

void parse_cmd_line(int argc, const char** argv, struct nes_opt_s* nes_opt, const char** input_filename, const char** output_filename)
{
    int i = 1;
    while(i < argc)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'm':
                    nes_opt->mapper = strtoul(argv[i+1], NULL, 0);
                    break;
                case 'n':
                    nes_opt->nametable_layout = strtoul(argv[i+1], NULL, 0);
                    break;
                case 'b':
                    nes_opt->battery = strtoul(argv[i+1], NULL, 0);
                    break;
                case 'a':
                    nes_opt->alternative_nametables = strtoul(argv[i+1], NULL, 0);
                    break;
                case 'h':
                    usage();
                    exit(0);
                default:
                    usage();
                    exit(1);
            }
            i++;
        }
        else
        {
            if(*input_filename == NULL)
            {
                *input_filename = argv[i];
            }
            else if(*output_filename == NULL)
            {
                *output_filename = argv[i];
            }
            else
            {
                usage();
                exit(1);
            }
        }
        i++;
    }
}

int main (int argc, char** argv)
{
    int i = 0;
    size_t input_bin_size;
    uint8_t num_banks;
    FILE* f_input_bin;
    FILE* f_output_nes;
    char* input_filename = NULL;
    char* output_filename = NULL;
    struct nes_opt_s nes_opt = {
                              .mapper = 0,
                              .num_prg_banks = 0,
                              .num_chr_banks = 0,
                              .nametable_layout = 0,
                              .alternative_nametables = 0,
                              .battery = 0
    };
#if defined(_WIN32)
  setmode (fileno (stdout), O_BINARY);
#endif
    parse_cmd_line(argc, argv, &nes_opt, &input_filename, &output_filename);
    // Open input .bin file
    f_input_bin = stdin;
    if(input_filename)
    {
        f_input_bin = fopen(input_filename, "rb");
        if(f_input_bin == NULL)
        {
            fprintf(stderr, "Error: Failed to open input file `%s`\n", input_filename);
        }
    }
    // Get file size and number of banks
    fseek(f_input_bin, 0, SEEK_END);
    input_bin_size = ftell(f_input_bin);
    fseek(f_input_bin, 0, SEEK_SET);
    if(input_bin_size % BANK_SIZE != 0)
    {
        fprintf(stderr, "Error: input file size not a multiple of bank size %d\n", BANK_SIZE);
        exit(1);
    }
    nes_opt.num_prg_banks = (uint8_t)(input_bin_size / BANK_SIZE);
    // Open output .nes file
    f_output_nes = stdout;
    if(output_filename)
    {
        f_output_nes = fopen(output_filename, "wb");
        if(f_output_nes == NULL)
        {
            fprintf(stderr, "Error: Failed to open output file `%s`\n", output_filename);
            exit(1);
        }
    }
    // Write iNES header to output file
    write_ines_header(f_output_nes, &nes_opt);
    // Copy input file to output file, one 16kB bank at a time 
    for(uint8_t i = 0; i < nes_opt.num_prg_banks; i++)
    {
        size_t num_bytes = fread(bank_data, 1, BANK_SIZE, f_input_bin);
        fwrite(bank_data, 1, BANK_SIZE, f_output_nes);
    }
    fclose(f_input_bin);
    fclose(f_output_nes);
    return 0;
}
