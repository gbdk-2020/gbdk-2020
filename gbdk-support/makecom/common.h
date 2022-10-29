#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>

#define ARRAY_LEN(A)  (sizeof(A) / sizeof(A[0]))

#define MAX_STR_LEN     4096
#define MAX_FILE_STR    (MAX_STR_LEN)
#define SYM_MAX_STR_LEN 1024

#define BANK_FNAME_LEN  (8+1+3 +1) // 8.3 filename style + terminator, ex: MYCOMFIL.001 (from "mycomfile.com")
#define COM_OVERLAY_NAME_LEN (8 + 1) // 8 chars for overlay string + terminator "MYCOMFIL"


extern char filename_in_bin[];
extern char filename_in_noi[];
extern char filename_out_com[];
extern char filename_banks_base[];
extern char filename_overlay[];

extern uint8_t * p_rom_buf_in;
extern size_t    rom_buf_in_len;

#endif // _COMMON_H