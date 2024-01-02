// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#define MAX_STR_LEN     4096
// #define MAX_STR_AREALEN 100
#define MAX_SPLIT_WORDS 6

#define NOI_REC_COUNT_MATCH 4

#define ERR_NO_AREAS_LEFT -1
#define AREA_VAL_UNSET   0xFFFFFFFF

#define NOI_REC_START_LEN 7 // length of "DEF l__"
#define NOI_REC_START   's'
#define NOI_REC_LENGTH  'l'


int noi_file_process_areas(char * filename_in);

void noi_init(void);
void noi_cleanup(void);