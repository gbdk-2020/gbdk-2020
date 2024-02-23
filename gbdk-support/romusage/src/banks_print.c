// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"
#include "list.h"
#include "banks.h"
#include "banks_print.h"
#include "banks_color.h"


#ifdef _WIN32
  #ifndef _WIN32
     #define __WIN32__
  #endif
#endif



// Ascii art style characters
// Print a block character to stdout based on the percentage used (int 0 - 100)
static void print_graph_char_asciistyle(uint32_t perc_used) {

    #ifdef __WIN32__
        // https://en.wikipedia.org/wiki/Code_page_437
        // https://sourceforge.net/p/mingw/mailman/message/14065664/
        // Code Page 437 (appears to be default for windows console,
        if      (perc_used >= 95) fprintf(stdout, "%c", 219u); // Full  Shade Block
        else if (perc_used >= 75) fprintf(stdout, "%c", 178u); // Dark  Shade Block
        else if (perc_used >= 50) fprintf(stdout, "%c", 177u); // Med   Shade Block
        else if (perc_used >= 25) fprintf(stdout, "%c", 176u); // Light Shade Block
        else                      fprintf(stdout, "_"); // ".");

        // All Block characters
        // if      (perc_used >= 99) fprintf(stdout, "%c", 219u); // Full  Shade Block
        // else if (perc_used >= 75) fprintf(stdout, "%c", 178u); // Dark  Shade Block
        // else if (perc_used >= 25) fprintf(stdout, "%c", 177u); // Med   Shade Block
        // else                      fprintf(stdout, "%c", 176u); // Light Shade Block


    #else // Non-Windows
        // https://www.fileformat.info/info/unicode/block/block_elements/utf8test.htm
        // https://www.fileformat.info/info/unicode/char/2588/index.htm
        if      (perc_used >= 95) fprintf(stdout, "%s", u8"\xE2\x96\x88"); // Full         Block in UTF-8 0xE2 0x96 0x88 (e29688)
        else if (perc_used >= 75) fprintf(stdout, "%s", u8"\xE2\x96\x93"); // Dark Shade   Block in UTF-8 0xE2 0x96 0x93 (e29693)
        else if (perc_used >= 50) fprintf(stdout, "%s", u8"\xE2\x96\x92"); // Med Shade    Block in UTF-8 0xE2 0x96 0x92 (e29692)
        else if (perc_used >= 25) fprintf(stdout, "%s", u8"\xE2\x96\x91"); // Light Shade  Block in UTF-8 0xE2 0x96 0x91 (e29691)
        else                      fprintf(stdout, "%s", u8"_"); // u8".");

        // All Blocks characters
        // if      (perc_used >= 95) fprintf(stdout, "%s", u8"\xE2\x96\x88"); // Full         Block in UTF-8 0xE2 0x96 0x88 (e29688)
        // else if (perc_used >= 75) fprintf(stdout, "%s", u8"\xE2\x96\x93"); // Dark Shade   Block in UTF-8 0xE2 0x96 0x93 (e29693)
        // else if (perc_used >= 25) fprintf(stdout, "%s", u8"\xE2\x96\x92"); // Med Shade    Block in UTF-8 0xE2 0x96 0x92 (e29692)
        // else                      fprintf(stdout, "%s", u8"\xE2\x96\x91"); // Light Shade  Block in UTF-8 0xE2 0x96 0x91 (e29691)
    #endif
}


// Standard character style
// Print a block character to stdout based on the percentage used (int 0 - 100)
static void print_graph_char_standard(uint32_t perc_used) {

    char ch;

    if (perc_used > 95)      ch = '#';
    else if (perc_used > 25) ch = '-';
    else                     ch = '.';
    fprintf(stdout, "%c", ch);
}


// Prints a graph for the bank
// Each character represents an arbitrary amount based on the bank size
static void bank_print_graph(bank_item * p_bank, uint32_t num_chars) {

    float bytes_per_char = p_bank->size_total / num_chars;
    unsigned int perc_used;
    uint32_t bucket_start, bucket_end;
    uint32_t bucket_id;
    uint32_t bucket_buf_size = num_chars * sizeof(uint32_t);

    uint32_t * p_buckets = (uint32_t *)malloc(bucket_buf_size);
    if (p_buckets == NULL) {
        log_error("Error: Failed to allocate buffer for graph!\n");
        return;
    }
    memset(p_buckets, 0, bucket_buf_size);

    bank_areas_split_to_buckets(p_bank, p_bank->start, p_bank->size_total, num_chars, p_buckets);

    for (bucket_id = 0; bucket_id <= (num_chars - 1); bucket_id++) {

        // Calculate range size this way so it matches the slightly variable bucket size.
        // See bank_areas_split_to_buckets() for details
        bucket_start = (uint32_t)(bytes_per_char * (float)bucket_id) + p_bank->start;
        bucket_end   = (uint32_t)((bytes_per_char * ((float)bucket_id + 1.0)) - 1.0) + p_bank->start;

        perc_used = (uint32_t)((float)p_buckets[bucket_id] * 100.0) / ((bucket_end - bucket_start) + 1);

        // Non-ascii style output
        if (!get_option_display_asciistyle())
            print_graph_char_standard(perc_used);
        else
            print_graph_char_asciistyle(perc_used);

        // Periodic line break if needed (for multi-line large graphs)
        if (((bucket_id + 1) % 64) == 0)
            fprintf(stdout, "\n");
    }

    if (p_buckets)
        free(p_buckets);
}


// Show a large usage graph for each bank
// Currently 16 bytes per character
static void banklist_print_large_graph(list_type * p_bank_list) {

    bank_item * banks = (bank_item *)p_bank_list->p_array;
    int c;

        for (c = 0; c < p_bank_list->count; c++) {

            fprintf(stdout,"\n\nStart: %s  ",banks[c].name); // Name
            fprintf(stdout,"0x%04X -> 0x%04X",banks[c].start,
                                              banks[c].end); // Address Start -> End
            fprintf(stdout,"\n"); // Name

            uint32_t bytes_per_char = LARGEGRAPH_BYTES_PER_CHAR;

            // Scale large graph unit size by number of banks it uses for banked items
            // (factoring in whether bank start is 0 or 1 based)
            if (option_summarized_mode)
                bytes_per_char *= ((banks[c].bank_num - banks[c].base_bank_num) + 1);

            bank_print_graph(&banks[c], banks[c].size_total / bytes_per_char);

            fprintf(stdout,"End: %s\n",banks[c].name); // Name
        }
}


// Display all areas for a bank
static void bank_print_area(bank_item *p_bank) {

    area_item * areas;
    int b;
    int hidden_count = 0;
    uint32_t hidden_total = 0;

    for(b = 0; b < p_bank->area_list.count; b++) {

        // Load the area list for the bank
        areas = (area_item *)p_bank->area_list.p_array;

        if (b == 0) {
            fprintf(stdout,"|\n");

            // Only show sub column headers for CDB output since there are a lot more areas
            if (get_option_input_source() == OPT_INPUT_SRC_CDB)
                fprintf(stdout,
                           "| Name                            Start  -> End      Size \n"
                           "| ---------------------           ----------------   -----\n");
        }

        // Don't display headers unless requested
        if ((banks_display_headers) || !(strstr(areas[b].name,"HEADER"))) {

            // Optionally hide areas below a given size
            if (areas[b].length >= get_option_area_hide_size()) {

                fprintf(stdout,"+ %-32s", areas[b].name);           // Name
                fprintf(stdout,"0x%04X -> 0x%04X",areas[b].start,
                                                  areas[b].end); // Address Start -> End
                fprintf(stdout,"%8d", areas[b].length);
                fprintf(stdout,"\n");
            } else {
                hidden_count++;
                hidden_total += areas[b].length;
            }
        }
    }
    if (hidden_count > 0)
        fprintf(stdout,"+ (%d items < %d hidden = %d total bytes)\n",
                        hidden_count, get_option_area_hide_size(), hidden_total);

    fprintf(stdout,"\n");
}


static void bank_print_info(bank_item *p_bank) {

    bank_render_color(p_bank, PRINT_REGION_ROW_START);
    fprintf(stdout,"%-13s",p_bank->name); // Name

    bank_render_color(p_bank, PRINT_REGION_ROW_MIDDLE_START);
    // Skip some info if compact mode is enabled.
    if (!option_compact_mode) {
        fprintf(stdout,"0x%04X -> 0x%04X",p_bank->start, p_bank->end); // Address Start -> End
        fprintf(stdout,"%9d", p_bank->size_total);      // Total size
    }
    fprintf(stdout,"%9d", p_bank->size_used); // Used

    if (!option_compact_mode) {
        fprintf(stdout,"  %4d%%", bank_calc_percent_used(p_bank)); // Percent Used
    }

    bank_render_color(p_bank, PRINT_REGION_ROW_MIDDLE_END);
    fprintf(stdout,"%9d", (int32_t)p_bank->size_total - (int32_t)p_bank->size_used); // Free
    fprintf(stdout,"   %3d%%", bank_calc_percent_free(p_bank)); // Percent Free

    // Print a small bar graph if requested
    if (banks_display_minigraph) {
        fprintf(stdout, " |");
        bank_print_graph(p_bank, MINIGRAPH_SIZE);
        fprintf(stdout, "|");
    }

    bank_render_color(p_bank, PRINT_REGION_ROW_END);
}


// Display all banks along with space used.
// Optionally show areas.
void banklist_printall(list_type * p_bank_list) {

    bank_item * banks = (bank_item *)p_bank_list->p_array;
    int c;
    int b;

    #ifdef __WIN32__
        if (get_option_color_mode() != OPT_PRINT_COLOR_OFF)
            if (!colors_try_windows_enable_virtual_term_for_vt_codes())
                log_warning("Warning: Failed to enable Windows virtual terminal sequences for color!\n");
    #endif

    fprintf(stdout, "\n");
    if (option_compact_mode) {
        fprintf(stdout,"Bank              Used     Free  Free%% \n"
                       "--------       -------  -------  -----\n");
    } else {
        fprintf(stdout,"Bank         Range                Size     Used  Used%%     Free  Free%% \n"
                       "--------     ----------------  -------  -------  -----  -------  -----\n");
    }

    // Print all banks
    for (c = 0; c < p_bank_list->count; c++) {

        bank_print_info(&banks[c]);
        fprintf(stdout,"\n");

        if (get_option_area_sort() != OPT_AREA_SORT_HIDE) // This is a hack-workaround, TODO:fixme
            if (banks_display_areas)
                bank_print_area(&banks[c]);

    } // End: Print all banks loop

        // Print a large graph per-bank if requested
    if (banks_display_largegraph)
        banklist_print_large_graph(p_bank_list);
}


// ====== JSON OUTPUT ======

static void bank_print_info_json(bank_item *p_bank) {

    fprintf(stdout,"    {\n");
    fprintf(stdout,"    \"name\":         \"%s\",\n",p_bank->name);

    fprintf(stdout,"    \"type\":         \"%d\",\n",p_bank->bank_mem_type);
    fprintf(stdout,"    \"baseBankNum\":  \"%d\",\n",p_bank->base_bank_num);
    fprintf(stdout,"    \"isBanked\":     \"%d\",\n",p_bank->is_banked);
    fprintf(stdout,"    \"isMergedBank\": \"%d\",\n",p_bank->is_merged_bank);

    fprintf(stdout,"    \"rangeStart\":   \"%d\",\n",p_bank->start);
    fprintf(stdout,"    \"rangeEnd\":     \"%d\",\n",p_bank->end);
    fprintf(stdout,"    \"size\":         \"%d\",\n",p_bank->size_total);
    fprintf(stdout,"    \"used\":         \"%d\",\n",p_bank->size_used);
    fprintf(stdout,"    \"free\":         \"%d\",\n",(int32_t)p_bank->size_total - (int32_t)p_bank->size_used);
    fprintf(stdout,"    \"usedPercent\":  \"%d\",\n",bank_calc_percent_used(p_bank));
    fprintf(stdout,"    \"freePercent\":  \"%d\",\n",bank_calc_percent_free(p_bank));

    fprintf(stdout,"    \"miniGraph\":    \"");
        bank_print_graph(p_bank, MINIGRAPH_SIZE);
    fprintf(stdout,"\"\n");

    fprintf(stdout,"    }\n");
}


// Render all banks and space used as json format
// Example:
//
// {"banks":
//   [
//     {
//     "name":         "ROM_2/3",
//     "type":         "0",
//     "baseBankNum":  "0",
//     "isBanked":     "1",
//     "isMergedBank": "1",
//     "rangeStart":   "0",
//     "rangeEnd":     "32767",
//     "size":         "65536",
//     "used":         "24740",
//     "free":         "40796",
//     "usedPercent":  "38",
//     "freePercent":  "62",
//     "miniGraph":    "-##-...#######.............."
//     }
//     ,
//     ...
//   ]
// }
void banklist_printall_json(list_type * p_bank_list) {

    bank_item * banks = (bank_item *)p_bank_list->p_array;
    int c;
    int b;

    // JSON array header
    fprintf(stdout,"{\"banks\":\n"
                   "  [\n");

    // Print each bank as an array object item
    for (c = 0; c < p_bank_list->count; c++) {
        // Comma separator between array items
        if (c > 0) fprintf(stdout,"    ,\n");

        bank_print_info_json(&banks[c]);
    }

    // JSON array footer
    fprintf(stdout,"  ]\n"
                   "}\n");
}
