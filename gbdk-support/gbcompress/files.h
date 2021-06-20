#ifndef _FILES_H
#define _FILES_H

uint8_t * file_read_into_buffer(char * filename, uint32_t *ret_size);
bool file_write_from_buffer(char * filename, uint8_t * p_buf, uint32_t data_len);

char * file_read_into_buffer_char(char * filename, uint32_t *ret_size);
bool file_write_from_buffer_char(char * filename, char * p_buf, uint32_t data_len);

#endif // _FILES_H
