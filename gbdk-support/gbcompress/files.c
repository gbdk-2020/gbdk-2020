#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
uint8_t * file_read_into_buffer(char * filename, uint32_t *ret_size) {

    long fsize;
    FILE * file_in = fopen(filename, "rb");
    uint8_t * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = malloc(fsize);
            if (filedata) {
                if (fsize != fread(filedata, 1, fsize, file_in)) {
                    printf("gbcompress: Warning: File read size didn't match expected for %s\n", filename);
                    filedata = NULL;
                }
                // Read was successful, set return size
                *ret_size = fsize;
            } else printf("gbcompress: ERROR: Failed to allocate memory to read file %s\n", filename);

        } else printf("gbcompress: ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else printf("gbcompress: ERROR: Failed to open input file %s\n", filename);

    return filedata;
}



// Writes a buffer to a file
bool file_write_from_buffer(char * filename, uint8_t * p_buf, uint32_t data_len) {

    bool status = false;
    size_t wrote_bytes;
    FILE * file_out = fopen(filename, "wb");

    if (file_out) {
        if (data_len == fwrite(p_buf, 1, data_len, file_out))
            status = true;
        else
            printf("gbcompress: Warning: File write size didn't match expected for %s\n", filename);

        fclose(file_out);
    }
    return status;
}




// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
char * file_read_into_buffer_char(char * filename, uint32_t *ret_size) {

    long fsize;
    FILE * file_in = fopen(filename, "r");
    char * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = malloc(fsize);
            if (filedata) {
                if (fsize != fread(filedata, 1, fsize, file_in)) {
                    printf("gbcompress: Warning: File read size didn't match expected for %s\n", filename);
                    filedata = NULL;
                }
                // Read was successful, set return size
                *ret_size = fsize;
            } else printf("gbcompress: ERROR: Failed to allocate memory to read file %s\n", filename);

        } else printf("gbcompress: ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else printf("gbcompress: ERROR: Failed to open input file %s\n", filename);

    return filedata;
}



// Writes a buffer to a file
bool file_write_from_buffer_char(char * filename, char * p_buf, uint32_t data_len) {

    bool status = false;
    size_t wrote_bytes;
    FILE * file_out = fopen(filename, "w");

    if (file_out) {
        if (data_len == fwrite(p_buf, 1, data_len, file_out))
            status = true;
        else
            printf("gbcompress: Warning: File write size didn't match expected for %s\n", filename);

        fclose(file_out);
    }
    return status;
}
