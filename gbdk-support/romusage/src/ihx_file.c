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
#include "banks.h"
#include "ihx_file.h"

// Example data to parse from a .ihx file
// No area names
// : 01 0020 00 E9 F6
// S BB AAAA RR DD CC
//
// S:    Start (":") (1 char)
// BB:   ByteCount   (2 chars, one byte)
// AAAA: Address     (4 chars, two bytes)
// RR:   Record Type (2 chars, one byte. 00=data, 01=EOF, Others)
// DD:   Data        (<ByteCount>  bytes)
// CC:   Checksum    (2 chars, one byte. Sum record bytes, 2's complement of LSByte)
/*
:01002000E9F6
:05002800220D20FCC9BF
:070030001A22130D20FAC98A
.
.
.
:00000001FF (EOF indicator)
*/

/* TODO/WARNING/BUG: 100% full banks
It may not be possible to easily tell the difference between a perfectly filled
bank (100% and no more) and an adjacent partially filled bank that starts at
zero - versus - the first bank overflowed into the second that is empty.

Currently the 100% bank will get merged into the next one and present as overflow
*/


#define IS_NOT_BANK_START(addr)  (!((addr & 0x00003FFFU) == 0x00000000U))
#define IS_NOT_BANK_END(addr)    (!((addr & 0x00003FFFU) == 0x00003FFFU))
#define BANK_NUM(addr)  ((addr & 0xFFFFC000U) >> 14)
#define ADDR_UNSET      0xFFFFFFFEU

#define MAX_STR_LEN     4096
#define IHX_DATA_LEN_MAX 255
#define IHX_REC_LEN_MIN  (1 + 2 + 4 + 2 + 0 + 2) // Start(1), ByteCount(2), Addr(4), Rec(2), Data(0..255x2), Checksum(2)

// IHX record types
#define IHX_REC_DATA     0x00U
#define IHX_REC_EOF      0x01U
#define IHX_REC_EXTSEG   0x02U
#define IHX_REC_STARTSEG 0x03U
#define IHX_REC_EXTLIN   0x04U
#define IHX_REC_STARTLIN 0x05U

typedef struct ihx_record {
    uint16_t length;
    uint32_t byte_count;
    uint32_t address;
    uint32_t address_end;
    uint32_t type;
    uint32_t checksum; // Would prefer this be a uint8_t, but mingw sscanf("%2hhx") has a buffer overflow that corrupts adjacent data
} ihx_record;

uint32_t g_address_upper;

// Converts sequentally stored ihx bank style (ROM0=0x0000, ROM1=0x4000, ROM2=0x8000)
// into map/noi banked style (ROM0=0x0000, ROM1=0x04000, ROM2=0x14000)
uint32_t ihx_bank_2_mapnoi_bank(uint32_t addr) {

    // If above 0x4000, upshift any address bits beyond 0x3FFF
    // into upper 16 bits and force 0x4000 as base address
    if (addr >= 0x4000)
        addr = (addr & 0x3FFF) | ((addr & 0xFFFFC000) << 2) + 0x4000;
    return addr;
}


// Return false if any character isn't a valid hex digit
int check_hex(char * c) {
    while (*c != '\0') {
        if ((*c >= '0') && (*c <= '9'))
            c++;
        if ((*c >= 'A') && (*c <= 'F'))
            c++;
        if ((*c >= 'a') && (*c <= 'f'))
            c++;
        else
            return false;
    }

    return true;
}


// Parse and validate an IHX record
int ihx_parse_and_validate_record(char * p_str, ihx_record * p_rec) {

        int calc_length = 0;
        int c;
        uint32_t ctemp, checksum_calc = 0; // Avoid mingw sscanf("%2hhx") buffer overflow with uint8_t

        // Remove trailing CR and LF
        p_rec->length = strlen(p_str);
        for (c = 0;c < p_rec->length;c++) {
            if (p_str[c] == '\n' || p_str[c] == '\r') {
                p_str[c] = '\0';   // Replace char with string terminator
                p_rec->length = c; // Shrink length to truncated size
                break;             // Exit loop after finding first CR or LF
            }
        }

        // Only parse lines that start with ':' character (Start token for IHX record)
        if (p_str[0] != ':') {
            log_warning("Warning: IHX: Invalid start of line token for line: %s \n", p_str);
            return false;
        }

       // Require minimum length
        if (p_rec->length < IHX_REC_LEN_MIN) {
            log_warning("Warning: IHX: Invalid line, too few characters: %s. Is %d, needs at least %d \n", p_str, p_rec->length, IHX_REC_LEN_MIN);
            return false;
        }

        // Only hex characters are allowed after start token
        p_str++; // Advance past Start code
        if (check_hex(p_str)) {
            log_warning("Warning: IHX: Invalid line, non-hex characters present: %s\n", p_str);
            return false;
        }

        // Read record header: byte count, start address, type
        sscanf(p_str, "%2x%4x%2x", &p_rec->byte_count, &p_rec->address, &p_rec->type);
        p_str += (2 + 4 + 2);

        // Require expected data byte count to fit within record length (at 2 chars per hex byte)
        calc_length = IHX_REC_LEN_MIN + (p_rec->byte_count * 2);
        if (p_rec->length != calc_length) {
            log_warning("Warning: IHX: byte count doesn't match length available in record! Record length = %d, Calc length = %d, bytecount = %d \n", p_rec->length, calc_length, p_rec->byte_count);
            return false;
        }

        // Is this an extended linear address record? Read in offset address if so
        if (p_rec->type == IHX_REC_EXTLIN) {
            sscanf(p_str, "%4x", &g_address_upper);
            g_address_upper <<= 16; // Shift into upper 16 bits of address space
        }
        else if (p_rec->type == IHX_REC_DATA) {

            // Don't process records with zero bytes of length
            if (p_rec->byte_count == 0) {
                log_warning("Warning: IHX: Zero length record starting at %x\n", p_rec->address);
                return false;
            }

            // Apply extended linear address (upper 16 bits of address space)
            // Calculate end address
            p_rec->address |= g_address_upper;
            p_rec->address_end = p_rec->address + p_rec->byte_count - 1;
        }

        // Read data segment and calculate checsum of data + headers
        checksum_calc = p_rec->byte_count + (p_rec->address & 0xFF) + ((p_rec->address >> 8) & 0xFF) + p_rec->type;
        for (c = 0;c < p_rec->byte_count;c++) {
            sscanf(p_str, "%2x", &ctemp);
            p_str += 2;
            checksum_calc += ctemp;
        }

        // Final calculated checeksum is 2's complement of LSByte
        checksum_calc = (((checksum_calc & 0xFF) ^ 0xFF) + 1) & 0xFF;

        // Read checksum from data
        sscanf(p_str, "%2x", &p_rec->checksum);
        p_str += 2;

        if (p_rec->checksum != checksum_calc) {
            log_warning("Warning: IHX: record checksum %x didn't match calculated checksum %x\n", p_rec->checksum, checksum_calc);
            return false;
        }

        // For records that start in banks above the unbanked region (0x000 - 0x3FFF)
        // Warn if they cross the boundary between different banks
        if ((p_rec->address >= 0x00004000U) &&
            ((p_rec->address & 0xFFFFC000U) != (p_rec->address_end & 0xFFFFC000U))) {
            log_warning("Warning: IHX: Write from one bank spans into the next. Bank overflow? %x -> %x (bank %d -> %d)\n",
                   p_rec->address, p_rec->address_end, BANK_NUM(p_rec->address), BANK_NUM(p_rec->address_end));
        }

    return true;
}

void area_convert_and_add(area_item area) {

    area_item t_area;

    // Convert ihx banked addresses to map/noi style
    // End should be calculated relative to start

    // Records which cross from (< 0x4000) into (>= 0x4000)
    // could produce ROM_0 entries that should be in ROM_1.
    // These are not warned about earlier since it could be
    // legit behavior for a 32K unbanked ROM.
    //
    // Split them on the bank boundary to avoid that.
    if ((area.start < 0x00004000U) && (area.end >= 0x00004000U))
    {
        // Copy area and drop part in the lower unbanked bank area
        t_area = area;
        t_area.start = 0x00004000U;
        area_convert_and_add(t_area);
        // Truncate original area at unbanked ROM boundary
        area.end = 0x00003FFFU;
    }

    area.end   = (area.end - area.start) + ihx_bank_2_mapnoi_bank(area.start);
    area.start = ihx_bank_2_mapnoi_bank(area.start);
    banks_check(area);
}


int ihx_file_process_areas(char * filename_in) {

    char cols;
    char strline_in[MAX_STR_LEN] = "";
    FILE * ihx_file = fopen(filename_in, "r");
    area_item area;
    ihx_record ihx_rec;

    set_option_input_source(OPT_INPUT_SRC_IHX);

    // IHX record areas don't have distinct names, so turn
    // off duplicate suppression to avoid any being dropped.
    // They are also never allowed to overlap
    set_option_suppress_duplicates(false);
    set_option_all_areas_exclusive(true);

    // Initialize global upper address modifier
    g_address_upper = 0x0000;

    // Initialize area record
    snprintf(area.name, sizeof(area.name), "ihx record");
    area.exclusive = option_all_areas_exclusive; // Default is false
    area.start = ADDR_UNSET;
    area.end   = ADDR_UNSET;

    if (ihx_file) {

        // Read one line at a time into \0 terminated string
        while (fgets(strline_in, sizeof(strline_in), ihx_file) != NULL) {

            // Parse record, skip if fails validation
            if (!ihx_parse_and_validate_record(strline_in, &ihx_rec))
                continue;

            // Process the pending record and exit if last record (EOF)
            // Also ignore non-default data records (don't seem to occur for gbz80)
            if (ihx_rec.type == IHX_REC_EOF) {
                area_convert_and_add(area);
                continue;
            } else if (ihx_rec.type == IHX_REC_EXTLIN) {
                // printf("Extended linear address changed to %08x %s\n\n\n", g_address_upper, strline_in);
                continue;
            } else if (ihx_rec.type != IHX_REC_DATA) {
                log_warning("Warning: IHX: dropped record %s of type %d\n", strline_in, ihx_rec.type);
                continue;
            }

            // Records are left pending (non-processed) until they don't merge
            // with the current incoming record *or* the final (EOF) record is found.

            // Try to merge with (pending) previous record if it's address-adjacent,
            // except when the new record starts or ends on a bank boundary
            // (this reduces count from 1000's since most are only 32 bytes long)
            if ((ihx_rec.address == area.end + 1) && IS_NOT_BANK_START(ihx_rec.address)) {
                area.end = ihx_rec.address_end;  // append to previous area
            } else if ((ihx_rec.address_end == area.start + 1) && IS_NOT_BANK_END(ihx_rec.address_end)) {
                area.start = ihx_rec.address;    // pre-pend to previous area
            } else {
                // New record was *not* adjacent to last,
                // so process the last/pending record
                if (area.start != ADDR_UNSET) {
                    area_convert_and_add(area);
                }
                // Now queue current record as pending for next loop
                area.start = ihx_rec.address;
                area.end   = ihx_rec.address + ihx_rec.byte_count - 1;
            }

        } // end: while still lines to process

        fclose(ihx_file);

    } // end: if valid file
    else {
        log_error("Error: Failed to open input file %s\n", filename_in);
        return false;
    }

    return true;
}

