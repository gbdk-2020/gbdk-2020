
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "files.h"

#define STR_FFWD_MATCH_ANY NULL

#define BUF_DEFAULT_SIZE 20000
#define BUF_GROW_SIZE    10000


static uint32_t size_compressed = 0;
static uint32_t size_decompressed = 0;


void c_source_set_sizes(uint32_t size_compressed_in, uint32_t size_decompressed_in) {
    size_compressed = size_compressed_in;
    size_decompressed = size_decompressed_in;
}


// Search for a character in a string
//
// Terminate search if:
// * any non-allowed character was found (str_chars_allowed, NULL for allow any chars)
// * end of string is reached (by terminator or length)
//
static char * str_ffwd_to(char * str_in, uint32_t * max_len, char char_match, char * str_chars_allowed) {

    char str_cur[2] = " \0"; // current character to test, for strpbrk

    while ((*max_len) && (*str_in != '\0')) {

        str_cur[0] = *str_in;
        if (*str_in == char_match)
            return str_in;
        else if (str_chars_allowed) {
            if (strpbrk(str_cur, str_chars_allowed) == NULL) {
                // signal failure if any non-allowed character was found
                return NULL;
            }
        } 

        str_in++;
        (*max_len)--;
    }

    return NULL;
}



// Convert an array of comma delimtied numbers into a buffer
//
// If conversion fails: returns NULL, *p_ret_size == 0
//
static uint8_t * str_array_to_buf(char * str_in, uint32_t * p_ret_len) {

    uint32_t  buf_size  = BUF_DEFAULT_SIZE; 
    uint8_t * p_buf     = malloc(buf_size);
    uint8_t * p_buf_last;
    char    * end_ptr;

    // Read value at a time from a string buffer
    // (strtok will replace the , chars with \0 on each split)
    char * str_cur = strtok(str_in,",");

    if (p_buf) {
        
        // Start with size zero
        *p_ret_len = 0;

        // Loop as long as strtok returns comma separated items
        while (str_cur != NULL) {

            // Grow buffer if needed
            if ((*p_ret_len) >= buf_size)  {

                buf_size += BUF_GROW_SIZE;
                p_buf_last = p_buf;
                p_buf = (uint8_t *)realloc(p_buf, buf_size);

                // Exit if realloc failed
                if (p_buf == NULL) {
                    *p_ret_len = 0;
                    free(p_buf_last);
                    return NULL;
                }
            }

            // Store current value
            p_buf[ *p_ret_len ] = (uint8_t)strtol(str_cur, &end_ptr, 0);

            // Only advance data buffer if conversion didn't fail
            if (str_cur != end_ptr)
                (*p_ret_len)++;

            // Read next value (strtok maintains state of last call)
            str_cur = strtok(NULL,",");
        }
    }

    return p_buf;
}



// Read from a file into a buffer, find first C source formatted
// array and parse it into another buffer (will allocate needed memory)
//
// Returns NULL if reading file didn't succeed
//
uint8_t * file_read_c_input_into_buffer(char * filename, uint32_t *ret_size) {

    char * filedata = NULL;
    uint32_t  file_size;

    char * str_c_array = NULL;
    char * str_c_array_start = NULL;
    uint8_t * p_array_data = NULL;

    filedata = file_read_into_buffer_char(filename, &file_size);

    if (filedata) {
        str_c_array = filedata; 

        // Find array opening bracket `[`
        str_c_array = str_ffwd_to(str_c_array, &file_size, '[', STR_FFWD_MATCH_ANY);
        if (str_c_array) {
            // Find Array closing bracket `]`
            str_c_array = str_ffwd_to(str_c_array, &file_size, ']', "[xX0123456789ABCDEFabcdef\t\n\r ");
            if (str_c_array) {
                // Find Start or array
                str_c_array = str_ffwd_to(str_c_array, &file_size, '{', "]\t\n\r =");
                if (str_c_array) {
                    // Save start of array
                    str_c_array_start = str_c_array + 1;
                    
                    // Find end of array
                    str_c_array = str_ffwd_to(str_c_array, &file_size, '}', "{xX0123456789ABCDEFabcdef,\t\n\r ");
                    if (str_c_array) {
                        // Terminate string at end of array
                        *str_c_array = '\0';

                        // If conversion fails: p_array_data == NULL, ret_size == 0
                        p_array_data = str_array_to_buf(str_c_array_start, ret_size);

                        // printf("C array length in = %d", *ret_size);
                    }
                }
            }
        }

        // Free file data if allocated
        free(filedata);
        filedata = NULL;
    }

    return p_array_data;
}



// Writes a buffer to a file in C source format
// Adds a matching .h if possible
//
bool file_write_c_output_from_buffer(char * filename, uint8_t * p_buf, uint32_t data_len, char * var_name, bool var_is_const) {

    bool status = false;
    size_t wrote_bytes;
    FILE * file_out = fopen(filename, "w");
    int i;

    if (file_out) {

        // Array entry with variable name
        fprintf(file_out, "\n\n%s unsigned char %s[] = {", (var_is_const) ? "const" : "", var_name);

        for (i = 0; i < data_len; i++) {

            // Line break every 16 bytes (includes at start. but exclude if on last entry)
            if ((i != data_len -1 ) && ((i % 16) == 0))
                fprintf(file_out, "\n    ");

            // Write current byte
            fprintf(file_out, "0x%.2X", p_buf[i]);
            
            // Add comma after every entry except last one
            if (i != data_len -1)
                fprintf(file_out, ", ");
        }
        fprintf(file_out, "\n};\n\n");
        status = true;

        fclose(file_out);


        // Create matching .h header file output, if file ends in .c or .C
        if (strlen(filename) >= 2) {
            if ((strcmp(&filename[strlen(filename) - 2],".c") == 0) ||
                (strcmp(&filename[strlen(filename) - 2],".C") == 0)) {
         
                // Replace file extension
                filename[strlen(filename) - 2] = '.';
                filename[strlen(filename) - 1] = 'h';

                FILE * file_out = fopen(filename, "w");

                if (file_out) {

                    fprintf(file_out, "\n\n#define %s_sz_comp %d\n", var_name, size_compressed);
                    fprintf(file_out, "#define %s_sz_decomp %d\n", var_name, size_decompressed);

                    // array entry with variable name
                    fprintf(file_out, "\n\nextern %s unsigned char %s[];\n\n", (var_is_const) ? "const" : "", var_name);

                    fclose(file_out);
                }
            }
        }
    }

    return status;
}
