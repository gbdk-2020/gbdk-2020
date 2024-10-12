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

#define EMPTY_RUN_TOO_SHORT(length, threshold) ((length > 0) && (length < threshold))

#define EMPTY_VALUE_MAX_COUNT               256
#define EMPTY_DEFAULT_CONSECUTIVE_THRESHOLD 17
#define EMPTY_0x00_CONSECUTIVE_THRESHOLD    128  // Larger threshold for 0x00 empty values due to possible sparse arrays
bool     empty_values[EMPTY_VALUE_MAX_COUNT];
uint32_t empty_consecutive_thresholds[EMPTY_VALUE_MAX_COUNT];

#define BANK_SIZE 0x4000
#define BANK_ADDR_MASK 0x00003FFF
#define BANK_NUM_MASK  0xFFFFC000
#define BANK_NUM_UPSHIFT 2

#define ROM_ADDR_TO_BANKED(addr) ((addr & BANK_ADDR_MASK) | ((addr & BANK_NUM_MASK) << BANK_NUM_UPSHIFT))



// Clear the empty values table to all values disabled
// should be called to remove defaults
void romfile_empty_value_table_clear(void) {
    for (int c = 0; c < EMPTY_VALUE_MAX_COUNT; c++)
        empty_values[c] = false;
}


// Set a byte value in the empty values table to true, range is 0-255 (byte)
void romfile_empty_value_table_add_entry(uint8_t value) {
    empty_values[value] = true;
}


// Call this before processing option arguments
void romfile_init_defaults(void) {

    // Default is: only value considered empty is 0xFF
    romfile_empty_value_table_clear();
    empty_values[0xFF] = true;

    // "Empty" 0x00 byte values use a longer run length threshold than other values due to possible sparse arrays
    for (int c = 0; c < EMPTY_VALUE_MAX_COUNT; c++) {
        empty_consecutive_thresholds[c] = EMPTY_DEFAULT_CONSECUTIVE_THRESHOLD;
    }
    empty_consecutive_thresholds[0x00] = EMPTY_0x00_CONSECUTIVE_THRESHOLD;
}


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
            } else log_error("Error: Failed to allocate memory to read file %s\n", filename);

        } else log_error("Error: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else log_error("Error: Failed to open input file %s\n", filename);

    return filedata;
}


// Calculate a range, adjust it's bank num and add try adding to banks if valid
static void rom_add_range(area_item range, bool romsize_32K_or_less) {

    // If active range ended, add to areas
    if (range.start != ADDR_UNSET) {

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
    uint32_t empty_run_length;
    uint32_t empty_run_length_threshold;
    uint8_t  empty_run_value;
    uint32_t bank_bytes = 0;
    area_item used_rom_range;
    bool romsize_32K_or_less;

    set_option_input_source(OPT_INPUT_SRC_ROM);

    // Read in ROM image
    p_buf = file_read_into_buffer(filename_in, &buf_length);
    romsize_32K_or_less = (buf_length <= 0x8000);

    used_rom_range.name[0] = '\0';  // Rom file ranges don't have names, set string to empty

    if (p_buf) {

        // Loop through all ROM bytes
        while (buf_idx < buf_length) {

            // This is looking for "Used" ranges broken up by non-"Empty" ranges
            //
            // Process each bank (0x4000 bytes in a row) separately
            // and close out any ranges that might span between them

            if (buf_length > BANK_SIZE) bank_bytes = BANK_SIZE;
            else bank_bytes = buf_length;

            // Reset range state values for each bank pass
            used_rom_range.start = ADDR_UNSET;
            empty_run_length = 0;

            while (bank_bytes) {

                // Split buffer up into potential runs of "Used" bytes with a
                // threshold of N non-empty same value bytes in a row to split them up

                uint8_t cur_byte_value = p_buf[buf_idx];

                // Continue an existing run if the value matches the current runs value (0x00 or 0xFF)
                if ((empty_run_length > 0) && (cur_byte_value == empty_run_value)) {
                    empty_run_length++;

                    // If the current "Empty" range is over the threshold it means
                    // any existing "Used" data range needs to be closed out and submitted.
                    //
                    // Otherwise the "Empty" values may be part of data and can be ignored
                    if ((empty_run_length >= empty_run_length_threshold) &&
                        (used_rom_range.start != ADDR_UNSET)) {

                        // Add range and back-calculate last "Used" range address using start of "empty" address
                        used_rom_range.end = buf_idx - empty_run_length;
                        rom_add_range(used_rom_range, romsize_32K_or_less);
                        // Clear as ready for new range
                        used_rom_range.start = ADDR_UNSET;
                    }
                }
                else {
                    // Close out pending failed (too short) "Empty" run
                    if (EMPTY_RUN_TOO_SHORT(empty_run_length, empty_run_length_threshold)) {
                        // If no range started, then convert it to a "Used" range since the bytes aren't actually empty
                        if (used_rom_range.start == ADDR_UNSET) used_rom_range.start = buf_idx - empty_run_length;
                    }

                    // Start a potential "Empty" new run
                    if (empty_values[cur_byte_value] == true) {
                        empty_run_length = 1;
                        empty_run_value = cur_byte_value;
                        // "Empty" 0x00 byte values use a longer run length threshold due to possible sparse arrays
                        empty_run_length_threshold = empty_consecutive_thresholds[cur_byte_value];
                    }
                    // Not "Empty", so reset "Empty" length
                    else {
                        empty_run_length = 0;
                        // Start a potential "Used" (non-empty) data range if one isn't active.
                        if (used_rom_range.start == ADDR_UNSET) used_rom_range.start = buf_idx;
                    }

                }

                buf_idx++;
                bank_bytes--;
            } // end: while still _bank_ bytes to process

            // End of Bank Cleanup

            // Potentially Empty bytes at the end of a bank that don't meet threshold... might be empty?
            //
            // // Convert "Empty" run to "Used" if it didn't cross the "Empty" threshold
            // if (EMPTY_RUN_TOO_SHORT(empty_run_length, empty_run_length_threshold)) {
            //     if (used_rom_range.start == ADDR_UNSET) used_rom_range.start = buf_idx - empty_run_length;
            // }

            // Close pending "Used" run if needed (at last byte which is -1 of current)
            if (used_rom_range.start != ADDR_UNSET) {
                used_rom_range.end = (buf_idx - 1);
                rom_add_range(used_rom_range, romsize_32K_or_less);
            }

        } // End main buffer loop

        if (p_buf) {
            free(p_buf);
            p_buf = NULL;
        }

    } // end: if valid file
    else {
        log_error("Error: Failed to open input file %s\n", filename_in);
        return false;
    }

   return true;
}
