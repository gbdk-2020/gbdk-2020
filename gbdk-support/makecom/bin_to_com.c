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
#include "list.h"
#include "files.h"
#include "noi_file.h"
#include "bin_to_com.h"


uint8_t * banks[BANKS_MAX_COUNT] = {NULL};
uint16_t  banks_len[BANKS_MAX_COUNT] = {0};
uint16_t  banks_count = 0;


void banks_cleanup(void) {
    for (int c = 0; c <= BANKS_MAX_ID; c++) {
        if (banks[c] != NULL) {

            free(banks[c]);
            banks[c] = NULL;
        }
    }
}


// Allocate memory for banks as banks are requested
// If preceding banks are not yet allocated then do those too
uint8_t * bank_get_ptr(uint16_t bank_num) {

    if (bank_num > BANKS_MAX_ID) {
        printf("makecom: ERROR: Requested bank %d is larger than max bank num %d!\n", bank_num, BANKS_MAX_ID);
        exit(EXIT_FAILURE);
    }

    if (banks[bank_num] == NULL) {

        // Make sure all banks leading up to requested are also allocated
        for (int c = 0; c <= bank_num; c++) {
            if (banks[c] == NULL) {

                banks[c] = malloc(BANK_SIZE);

                if (!banks[c]) {
                    printf("makecom: ERROR: Failed to allocate memory for bank %d!\n", c);
                    exit(EXIT_FAILURE);
                }

                // zero out buffer (though unused space will not get written out)
                memset(banks[c], 0x00u, BANK_SIZE);
            }
        }
    }

    return banks[bank_num];
}



// Copy data from source bin file at offset address into requested bank buffer
void copy_to_bank(uint16_t bank_num, uint32_t rom_src_addr, uint32_t bank_out_addr, uint32_t length) {

    // TODO: make sure bin src addr doesn't exceed source file buffer length
    if ((rom_src_addr + length) > rom_buf_in_len) {
        printf("makecom: ERROR: Can't copy %d bytes from %x for bank %d, would overflow past end of ROM source buffer of size %d!\n", length, rom_src_addr, bank_num, (uint32_t)rom_buf_in_len);
        exit(EXIT_FAILURE);
    }

    if ((bank_out_addr + length) > BANK_SIZE) {
        printf("makecom: ERROR: Can't copy %d bytes to %04x for bank %d, would overflow past end of bank buffer of size %d!\n",length, bank_out_addr, bank_num, BANK_SIZE);
        exit(EXIT_FAILURE);
    }

    if (bank_num > banks_count)
        banks_count = bank_num;

    // Update length of bank data buffers (to allow for truncating output later)
    if ((bank_out_addr + length) > banks_len[bank_num])
        banks_len[bank_num] = (bank_out_addr + length);

    // printf("* copying... bank:%d from:%x to:%x len:%d\n",
    //         bank_num, rom_src_addr, bank_out_addr, length);

    memcpy(bank_get_ptr(bank_num) + bank_out_addr, p_rom_buf_in + rom_src_addr, length);
}


void banks_write_out(void) {

    // If any of below fails it will trigger exit() and cleanup() will be called

    // Write first bank out, handled differently than rest
    if ((banks[0] != NULL) && (banks_len[0] > 0)) {
        // printf("Bank %d: Writing %d bytes to %s\n",0, banks_len[0], filename_out_com);
        file_write_from_buffer(filename_out_com, banks[0], banks_len[0]);
    } else {
        printf("makecom: Error: Bank 0 was empty, no data written\n");
        exit;
    }

    // Write out remaining banks if applicable
    char bank_fname[MAX_STR_LEN] = "";
    for (int c = 1; c <= BANKS_MAX_ID; c++) {
        if ((banks[c] != NULL) && (banks_len[0] > 0)) {
            // Format to 8.3 filename with bank num as zero padded extension
            sprintf(bank_fname, "%s.%03d", filename_banks_base, c);
            // printf("Bank %d: Writing %d bytes to %s\n",c, banks_len[c], bank_fname);
            file_write_from_buffer(bank_fname, banks[c], banks_len[c]);
        } else
            break; // Exit loop on first unpopulated bank
    }

}



int bin2com(void) {

    symbol_item * symbols = (symbol_item *)symbol_list.p_array;

    for(int c = 0; c < symbol_list.count; c++) {

        // Only process completed symbols (start and length both set, lenth not zero)
        if ((symbols[c].addr_start != SYM_VAL_UNSET) &&
            (symbols[c].length != SYM_VAL_UNSET) &&
            (symbols[c].length > 0)) {

            // If symbol is for a bank > 0
            if ((strncmp(symbols[c].name, "_CODE_", (sizeof("_CODE_") - 1)) == 0) && (symbols[c].bank_num > 0)) {
                // Copy data from bank addr in source ROM to start of separate bank buffer
                copy_to_bank(symbols[c].bank_num, symbols[c].src_rom_addr, BANK_START_ADDR, symbols[c].length);
            } else {
                // For remaining symbols (assume bank 0), relocate them by -100 when possible (addr -> addr - 100)
                copy_to_bank(BANK_0, symbols[c].addr_start, BANK_0_RELOC_ADDR(symbols[c].addr_start), symbols[c].length);
            }
        }
    }

    // Patch in updated bank / overlay count and bank filename
    if ((banks_count > 0) && (overlay_count_addr != SYM_VAL_UNSET) && (overlay_name_addr != SYM_VAL_UNSET)) {

        if (overlay_count_addr > BANK_0_ADDR_OFFSET)
            *(banks[0] + (overlay_count_addr - BANK_0_ADDR_OFFSET)) = banks_count;

        if (overlay_name_addr > BANK_0_ADDR_OFFSET)
            memcpy(banks[0] + (overlay_name_addr - BANK_0_ADDR_OFFSET), filename_overlay, COM_OVERLAY_NAME_LEN-1);
    }

    // No write the data out
    banks_write_out();

    return EXIT_SUCCESS;
}



