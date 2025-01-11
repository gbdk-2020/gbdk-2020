#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
uint8_t * file_read_into_buffer(char * filename, size_t *ret_size) {

    long fsize;
    FILE * file_in = fopen(filename, "rb");
    uint8_t * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = (uint8_t *)malloc(fsize);
            if (filedata) {
                if (fsize != (long)fread(filedata, 1, fsize, file_in)) {
                    printf("Warning: File read size didn't match expected for %s\n", filename);
                    filedata = NULL;
                }
                // Read was successful, set return size
                *ret_size = (size_t)fsize;
            } else printf("ERROR: Failed to allocate memory to read file %s\n", filename);

        } else printf("ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else printf("ERROR: Failed to open input file %s\n", filename);

    return filedata;
}



// Writes a buffer to a file
bool file_write_from_buffer(char * filename, uint8_t * p_buf, size_t data_len) {

    bool status = false;
    FILE * file_out = fopen(filename, "wb");

    if (file_out) {
        if (data_len == fwrite(p_buf, 1, data_len, file_out))
            status = true;
        else
            printf("Warning: File write size didn't match expected for %s\n", filename);

        fclose(file_out);
    } else
      printf("Warning: Unable to open file: %s\n", filename);
    return status;
}




// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
char * file_read_into_buffer_char(char * filename, size_t *ret_size) {

    long fsize;
    // On windows windows fread() will auto-translate CRLF to just LF and
    // report it as one byte read, messing up the size check
    FILE * file_in = fopen(filename, "rb");
    char * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = (char *)malloc(fsize);
            if (filedata) {
                if (fsize != (long)fread(filedata, 1, fsize, file_in)) {
                    printf("Warning: File read size didn't match expected for %s\n", filename);
                    filedata = NULL;
                }
                // Read was successful, set return size
                *ret_size = (size_t)fsize;
            } else printf("ERROR: Failed to allocate memory to read file %s\n", filename);

        } else printf("ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else printf("ERROR: Failed to open input file %s\n", filename);

    return filedata;
}



// Writes a buffer to a file
bool file_write_from_buffer_char(char * filename, char * p_buf, size_t data_len) {

    bool status = false;
    FILE * file_out = fopen(filename, "w");

    if (file_out) {
        if (data_len == fwrite(p_buf, 1, data_len, file_out))
            status = true;
        else
            printf("Warning: File write size didn't match expected for %s\n", filename);

        fclose(file_out);
    }
    return status;
}
