#ifndef _FILES_H
#define _FILES_H

char * file_read_into_buffer(char * filename, uint32_t *ret_size);
bool file_write_from_buffer(char * filename, uint8_t * p_buf, uint32_t data_len);

#endif // _FILES_H
