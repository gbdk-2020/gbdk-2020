// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "common.h"
#include "path_ops.h"
#include "list.h"
#include "files.h"
#include "obj_data.h"

static void files_set_output_name(void);
static char * file_read_to_buffer(char *);

list_type filelist;

char g_out_ext[MAX_FILE_STR];
char g_out_path[MAX_FILE_STR];

void files_set_out_ext(char * ext_str) {
    if (snprintf(g_out_ext, sizeof(g_out_ext), "%s", ext_str) > sizeof(g_out_ext))
        printf("Bankpack: Warning: truncated output extension to:%s\n",g_out_ext);
}

void files_set_out_path(char * path_str) {
    if (snprintf(g_out_path, sizeof(g_out_path), "%s", path_str) > sizeof(g_out_path))
        printf("Bankpack: Warning: truncated output path to:%s\n",g_out_path);
}


void files_init(void) {
    list_init(&filelist, sizeof(file_item));
    g_out_ext[0]  = '\0';
    g_out_path[0] = '\0';
}

void files_cleanup(void) {
    list_cleanup(&filelist);
}


void files_add(char * filename) {

    file_item newfile;

    if (snprintf(newfile.name_in, sizeof(newfile.name_in), "%s", filename) > sizeof(newfile.name_in))
        printf("Warning: truncated input filename to:%s\n",newfile.name_in);

    newfile.name_out[0] = '\0';
    newfile.rewrite_needed = false;
    newfile.bank_num = BANK_NUM_UNASSIGNED;

    list_additem(&filelist, &newfile);
}


char * file_get_name_in_by_id(uint32_t file_id) {

    file_item * files = (file_item *)filelist.p_array;

    if ((file_id >= 0) && (file_id < filelist.count))
        return files[file_id].name_in;
    else
        return (char *)"\0";
}


char * file_get_name_out_by_id(uint32_t file_id) {

    file_item * files = (file_item *)filelist.p_array;

    if ((file_id >= 0) && (file_id < filelist.count))
        return files[file_id].name_out;
    else
        return (char *)"\0";
}


// Update output names based on -path= and -ext= option params
static void files_set_output_name(void) {

    uint32_t c;
    file_item * files = (file_item *)filelist.p_array;

    // Process stored file names
    for (c = 0; c < filelist.count; c++) {

        snprintf(files[c].name_out, sizeof(files[c].name_out), "%s", files[c].name_in);

        if (g_out_ext[0])
            filename_replace_extension(files[c].name_out, g_out_ext, sizeof(files[c].name_out));
        if (g_out_path[0])
            filename_replace_path(files[c].name_out, g_out_path, sizeof(files[c].name_out));
    }
}


// Read from a file into a buffer (will allocate needed memory)
// Returns NULL if reading file didn't succeed
static char * file_read_to_buffer(char * filename) {

    long fsize;
    FILE * file_in = fopen(filename, "rb");
    char * filedata = NULL;

    if (file_in) {
        // Get file size
        fseek(file_in, 0, SEEK_END);
        fsize = ftell(file_in);
        if (fsize != -1L) {
            fseek(file_in, 0, SEEK_SET);

            filedata = malloc(fsize + 1); // One extra byte to add string null terminator
            if (filedata) {
                if (fsize != fread(filedata, 1, fsize, file_in))
                    printf("BankPack: Warning: File read size didn't match expected for %s\n", filename);
                filedata[fsize] = '\0'; // Add null string terminator at end
            } else printf("BankPack: ERROR: Failed to allocate memory to read file %s\n", filename);

        } else printf("BankPack: ERROR: Failed to read size of file %s\n", filename);

        fclose(file_in);
    } else printf("BankPack: ERROR: Failed to open input file %s\n", filename);

    return filedata;
}


// Extract areas from files, then collected assign them to banks
void files_extract(void) {
    uint32_t c;
    char strline_in[OBJ_NAME_MAX_STR_LEN] = "";
    file_item * files = (file_item *)filelist.p_array;
    FILE * obj_file;

    // Process stored file names
    for (c = 0; c < filelist.count; c++) {

        // Open each object file and try to process all the lines
        obj_file = fopen(files[c].name_in, "r");
        if (obj_file) {

            // Read one line at a time into \0 terminated string
            while ( fgets(strline_in, sizeof(strline_in), obj_file) != NULL) {
                if (strline_in[0] == 'A')
                    areas_add(strline_in, c);
                else if (strline_in[0] == 'S')
                    symbols_add(strline_in, c);
            }
            fclose(obj_file);

        } // end: if valid file
        else {
            printf("BankPack: ERROR: failed to open file %s\n", files[c].name_in);
            exit(EXIT_FAILURE);
        }
    }

    obj_data_process(&filelist);
    files_set_output_name();
}


void files_rewrite(void) {

    uint32_t c;
    char * in_file_buf = NULL;
    char * strline_in  = NULL;
    FILE * out_file    = NULL;
    file_item * files  = (file_item *)filelist.p_array;

    // Process stored file names - (including unchanged ones since output may get new extensions or path)
    for (c = 0; c < filelist.count; c++) {

        in_file_buf = file_read_to_buffer(files[c].name_in);
        if (!in_file_buf)
            exit(EXIT_FAILURE);

        out_file = fopen(files[c].name_out, "w");
        if (!out_file) {
            printf("BankPack: ERROR: failed to open output file %s\n", files[c].name_out);
            exit(EXIT_FAILURE);
        }

        // Read one line at a time from a string buffer
        // Note: strtok will replace the \n chars with \0 on each split, so be sure to add those back
        strline_in = strtok(in_file_buf,"\n");
        while (strline_in != NULL) {

            // Only modify lines in flagged files
            if (files[c].rewrite_needed) {

                if (!area_modify_and_write_to_file(strline_in, out_file, files[c].bank_num)) {
                    if (!symbol_modify_and_write_to_file(strline_in, out_file, files[c].bank_num, c)) {
                        // Default is to write line with no changes
                        fprintf(out_file, "%s\n", strline_in);
                    }
                }
            } else
                fprintf(out_file, "%s\n", strline_in);

            // Read next line
            strline_in = strtok(NULL,"\n");
        }

        if (in_file_buf)
            free(in_file_buf);
        fclose(out_file);
    }

}
