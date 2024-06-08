// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#define CDB_MAX_STR_LEN     4096
// #define CDB_MAX_STR_AREALEN 100
#define CDB_MAX_SPLIT_WORDS 12

#define CDB_REC_L_COUNT_MATCH 6
#define CDB_REC_S_COUNT_MATCH 12

#define ERR_NO_AREAS_LEFT -1
#define AREA_VAL_UNSET   0xFFFFFFFF

#define CDB_REC_START_LEN 2 // length of "DEF l__"
#define CDB_REC_L   'L'  // Linker (address) Record
#define CDB_REC_S   'S'  // Symbol (size) Record


int cdb_file_process_symbols(char * filename_in);

void cdb_init(void);
void cdb_cleanup(void);
