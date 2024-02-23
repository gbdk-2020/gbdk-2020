// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"
#include "list.h"
#include "banks.h"
#include "rom_file.h"


#define ADDR_UNSET 0xFFFFFFFF
#define EMPTY_CONSECUTIVE_THRESHOLD 16
#define EMPTY_VAL 0xFF
#define BANK_SIZE 0x4000
#define BANK_ADDR_MASK 0x00003FFF
#define BANK_NUM_MASK  0xFFFFC000
#define BANK_NUM_UPSHIFT 2

#define ROM_ADDR_TO_BANKED(addr) ((addr & BANK_ADDR_MASK) | ((addr & BANK_NUM_MASK) << BANK_NUM_UPSHIFT))


// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
uint8_t * file_read_into_buffer(char * filename, uint32_t *ret_size) {

    long fsize;
    FILE * file_in = fopen(filename, "rb");
    uint8_t * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = (uint8_t *)malloc(fsize);
            if (filedata) {
                if (fsize != fread(filedata, 1, fsize, file_in)) {
                    log_warning("Warning: File read size didn't match expected for %s\n", filename);
                    filedata = NULL;
                }
                // Read was successful, set return size
                *ret_size = fsize;
            } else log_error("ERROR: Failed to allocate memory to read file %s\n", filename);

        } else log_error("ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else log_error("ERROR: Failed to open input file %s\n", filename);

    return filedata;
}


// Calculate a range, adjust it's bank num and add try adding to banks if valid
static void rom_add_range(area_item range, uint32_t cur_idx, uint32_t empty_run, bool romsize_32K_or_less) {

    if (range.start != ADDR_UNSET) {

        // Range ended, add to areas
        range.end = cur_idx - empty_run;

        // Don't try to virtual translate address for binary ROM
        // files 32K or smaller, most likely they're unbanked.
        if (!romsize_32K_or_less) {
            // convert from ROM banked format (linear, bits in .14+)
            // to expected banked format (bank in bits .16+)
            range.start = ROM_ADDR_TO_BANKED(range.start);
            range.end = ROM_ADDR_TO_BANKED(range.end);

            // Banks 1+ all start at 0x4000
            if (BANK_GET_NUM(range.start) >= 1) {
                range.start += BANK_SIZE;
                range.end += BANK_SIZE;
            }
        }

        // Calc length and add to banks
        if (range.end >= range.start) {
            range.length = range.end - range.start + 1;
            // printf("ROM ADD Range: %8x -> %8x, %d (at %8x)\n", range.start, range.end, range.length, cur_idx);
            banks_check(range);
        }
    }
}



int rom_file_process(char * filename_in) {

    char cols;
    uint8_t * p_buf = NULL;
    uint32_t buf_idx = 0;
    uint32_t buf_length = 0;
    uint32_t empty_run = 0;
    uint32_t bank_bytes = 0;
    area_item rom_range;
    bool romsize_32K_or_less;

    set_option_input_source(OPT_INPUT_SRC_ROM);

    // Read in ROM image
    p_buf = file_read_into_buffer(filename_in, &buf_length);
    romsize_32K_or_less = (buf_length <= 0x8000);

    rom_range.name[0] = '\0';  // Rom file ranges don't have names, set string to empty
    rom_range.start = ADDR_UNSET;

    if (p_buf) {

        // Loop through all ROM bytes
        while (buf_idx < buf_length) {

            // Process each bank (0x4000 bytes in a row) separately
            // and close out any ranges that might span between them

            if (buf_length > BANK_SIZE) bank_bytes = BANK_SIZE;
            else bank_bytes = buf_length;

            while (bank_bytes) {

                // Split buffer up into runs of non-zero bytes
                // with a threshold of N zero bytes in a row to split them
                if (p_buf[buf_idx] != EMPTY_VAL) {
                    if (rom_range.start == ADDR_UNSET)
                        rom_range.start = buf_idx;
                    empty_run = 0;
                } else {
                    empty_run++;

                    // Handle current run/range if present and over threshold.
                    // Otherwise ignore the "empty" vals since it may be data of that value
                    if ((empty_run > EMPTY_CONSECUTIVE_THRESHOLD) &&
                        (rom_range.start != ADDR_UNSET)) {

                        // Add range then flag as cleared and prep for a new range
                        rom_add_range(rom_range, buf_idx, empty_run, romsize_32K_or_less);
                        rom_range.start = ADDR_UNSET;
                    }
                }

                buf_idx++;
                bank_bytes--;
            } // end: while still _bank_ bytes to process

            // Close out a remaining run if one exists (at last byte which is -1 of current)
            // Flag as cleared and prep for a new range
            rom_add_range(rom_range, buf_idx - 1, empty_run, romsize_32K_or_less);
            rom_range.start = ADDR_UNSET;

        } // End main buffer loop

        if (p_buf) {
            free(p_buf);
            p_buf = NULL;
        }

    } // end: if valid file
    else return (false);

   return true;
}
