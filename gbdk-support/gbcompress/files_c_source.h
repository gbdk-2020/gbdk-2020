#ifndef _FILES_C_SOURCE_H
#define _FILES_C_SOURCE_H

void c_source_set_sizes(uint32_t, uint32_t);

bool file_write_c_output_from_buffer(char *, uint8_t *, uint32_t, char *, bool);
uint8_t * file_read_c_input_into_buffer(char * filename, uint32_t *ret_size);

#endif // _FILES_C_SOURCE_H
