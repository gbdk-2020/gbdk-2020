// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#ifndef _BIN_TO_COM_H
#define _BIN_TO_COM_H

#define MAX(a, b)            ((a > b) ? a : b)
#define BANKS_MAX_COUNT  256
#define BANKS_MAX_ID     (BANKS_MAX_COUNT - 1)
#define BANK_SIZE        (0x4000u)
#define BANK_START_ADDR  (0x0000u)
#define BANK_0           0
#define BANK_0_ADDR_OFFSET 0x100

#define BANK_GET_NUM(addr)   ((addr & 0xFFFF0000U) >> 16)
#define WITHOUT_BANK(addr)   (addr & 0x0000FFFFU)
// Converts a symbol bank address such as 0x34000 to a bin/rom address such as 0x12000 (i.e: ~0x4000 * 3)
// Make sure base bank addess doesn't go negative
#define BANK_GET_ROM_ADDR(addr) ( (MAX(WITHOUT_BANK(addr), BANK_SIZE) - BANK_SIZE) + (BANK_SIZE * BANK_GET_NUM(addr)) )

// Apply bank 0 address offset and make sure it doesn't go negative
#define BANK_0_RELOC_ADDR(addr)   (MAX((addr), BANK_0_ADDR_OFFSET) - BANK_0_ADDR_OFFSET)

void copy_data(uint16_t bank_num, uint32_t rom_src_addr, uint32_t bank_out_addr, uint32_t length);
uint8_t * bank_get_ptr(uint16_t bank_num);
void copy_to_bank(uint16_t bank_num, uint32_t rom_src_addr, uint32_t bank_out_addr, uint32_t length);
void banks_cleanup(void);
int bin2com(void);

#endif // _BIN_TO_COM_H