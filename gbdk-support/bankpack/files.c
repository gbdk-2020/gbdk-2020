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
char g_out_linkerfile_name[MAX_FILE_STR] = {'\0'};

static unsigned int get_obj_file_format(FILE * obj_file, const char * str_filename);

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


// Reads a list of object files from a linkerfile
// (one filename per line) and adds them
void files_read_linkerfile(char * filename_in) {
    char strline_in[MAX_FILE_STR] = "";

    FILE * in_file;

    // Open each object file and try to process all the lines
    in_file = fopen(filename_in, "r");
    if (in_file) {

        // Read one line at a time into \0 terminated string
        // Skip empty lines
        // Strip newlines and carraige returns from end of filename
        while ( fgets(strline_in, sizeof(strline_in), in_file) != NULL) {
            if ((strline_in[0] != '\n') || (strline_in[0] != '\r')) {
                strline_in[strcspn(strline_in, "\r\n")] = '\0';
                files_add(strline_in);
            }
        }
        fclose(in_file);

    } // end: if valid file
    else {
        printf("BankPack: ERROR: failed to open input linkerfile: %s\n", filename_in);
        exit(EXIT_FAILURE);
    }
}


void files_set_linkerfile_outname(char * filename) {

    if (snprintf(g_out_linkerfile_name, sizeof(g_out_linkerfile_name), "%s", filename) > sizeof(g_out_linkerfile_name))
        printf("Warning: truncated output linkerfile name to:%s\n",g_out_linkerfile_name);
}


// qsort compare rule function for linkerfile_order
static int linkerfile_order_compare(const void* a, const void* b) {

    // Sort by linkerfile_order [asc]
    if (((file_order_t *)a)->linkerfile_order != ((file_order_t *)b)->linkerfile_order)
        return (((file_order_t *)a)->linkerfile_order < ((file_order_t *)b)->linkerfile_order) ? QSORT_A_FIRST : QSORT_A_AFTER;
    else
        return QSORT_A_SAME;
}

// Set linkerfile order
//
// Helps ensure fixed bank areas (hopefully) get linked/placed before autobank.
// Intended to facilitate alignment assumptions for size padded object files.
// - Files that only have non-banked data ("_CODE", "_HOME") will always be first in the order
// - See area_item_compare() for full details of how linkerfile_order gets set
static void linkerfile_output_order_sort(const file_item * p_files, file_order_t * p_filelist_order, const uint32_t count) {

    uint32_t c;

    if (p_filelist_order && p_files) {
        // Create sort-able list from list of files and their assigned linkerfile order
        for (c = 0; c < filelist.count; c++) {
            p_filelist_order[c].file_id = c;
            p_filelist_order[c].linkerfile_order = p_files[c].linkerfile_order;
        }

        // Sort the new list by linkerfile order
        qsort (p_filelist_order, count, sizeof(file_order_t), linkerfile_order_compare);
    } else {
        printf("BankPack: ERROR: failed to allocate memory for sorting linkerfile output\n");
        exit(EXIT_FAILURE);
    }
}


// Writes a list of loaded object filenames to
// a linkerfile (one filename per line)
void files_write_linkerfile(void) {
    uint32_t c;
    file_item * files = (file_item *)filelist.p_array;
    FILE * out_file;

    // Instead of sorting the file array itself, which would break references
    // (i.e. "files[ areas[c].file_id ]."), create a sorted reference array to order the file output
    file_order_t * filelist_order = malloc(filelist.count * sizeof(file_order_t));
    linkerfile_output_order_sort(files, filelist_order, filelist.count);

    // Open the linkerfile output and write all object filenames
    out_file = fopen(g_out_linkerfile_name, "w");
    if (out_file) {

        // Process stored file names
        for (c = 0; c < filelist.count; c++) {
            // printf("linkerfile out: c = %d : file_id = %d linkerfile_order = %d, name = %s\n", c, filelist_order[c].file_id,  filelist_order[c].linkerfile_order, files[ filelist_order[c].file_id ].name_out);
            fprintf(out_file, "%s\n", files[ filelist_order[c].file_id ].name_out);
        }

        fclose(out_file);

    } // end: if valid file
    else {
        printf("BankPack: ERROR: failed to open output linkerfile: %s\n", g_out_linkerfile_name);
        exit(EXIT_FAILURE);
    }

    if (filelist_order)
        free(filelist_order);
}


void files_add(char * filename) {

    file_item newfile;

    if (snprintf(newfile.name_in, sizeof(newfile.name_in), "%s", filename) > sizeof(newfile.name_in))
        printf("Warning: truncated input filename to:%s\n",newfile.name_in);

    newfile.name_out[0] = '\0';
    newfile.rewrite_needed = false;
    newfile.bank_num = BANK_NUM_UNASSIGNED;
    newfile.linkerfile_order = LINKERFILE_ORDER_FIRST;  // This won't get changed for non-banked object files

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


// Scan the file looking for lines that start with XL3 or XL4
// which indicate object file version type
static unsigned int get_obj_file_format(FILE * obj_file, const char * str_filename) {

    char strline_in[OBJ_NAME_MAX_STR_LEN] = "";

    if (obj_file) {

        // Rewind file and read one line at a time into \0 terminated string
        rewind(obj_file);
        while ( fgets(strline_in, sizeof(strline_in), obj_file) != NULL) {

            // Check if a version version indicator was found
            if (strncmp(strline_in, OBJ_FILE_XL3_ID, strlen(OBJ_FILE_XL3_ID)) == 0) {
                return (OBJ_FILE_XL3_24BIT_ADDR);
            }
            else if (strncmp(strline_in, OBJ_FILE_XL4_ID, strlen(OBJ_FILE_XL4_ID)) == 0) {
                return (OBJ_FILE_XL4_32BIT_ADDR);
            }
        }
    } else {
        printf("BankPack: ERROR: invalid file pointer for file: \"%s\"\n", str_filename);
        exit(EXIT_FAILURE);
    }

    printf("BankPack: ERROR: unable to determine Object File version for file: \"%s\"\n", str_filename);
    exit(EXIT_FAILURE);
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
            files[c].obj_file_format = get_obj_file_format(obj_file, files[c].name_in);

            // Rewind file (due to format scan above)
            // Then read one line at a time into \0 terminated string
            rewind(obj_file);
            while ( fgets(strline_in, sizeof(strline_in), obj_file) != NULL) {
                if (strline_in[0] == 'A')
                    areas_add(strline_in, c);
                else if (strline_in[0] == 'S')
                    symbols_add(strline_in, c, files[c].obj_file_format);
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

    // If linkerfile output is enabled, write it
    if (g_out_linkerfile_name[0] != '\0')
        files_write_linkerfile();

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
                    if (!symbol_modify_and_write_to_file(strline_in, out_file, files[c].bank_num, c, files[c].obj_file_format)) {
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
