#ifndef _FILES_H
#define _FILES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint8_t * file_read_into_buffer(char * filename, size_t *ret_size);
bool file_write_from_buffer(char * filename, uint8_t * p_buf, size_t data_len);

char * file_read_into_buffer_char(char * filename, size_t *ret_size);
bool file_write_from_buffer_char(char * filename, char * p_buf, size_t data_len);

#endif // _FILES_H
