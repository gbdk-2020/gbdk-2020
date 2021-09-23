// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "rlecompress.h"


static uint8_t * FinBuf      = NULL;
static uint32_t  Fsize_in    = 0;
static uint32_t  FinIndex    = 0;

static uint8_t ** pp_FoutBuf = NULL;
static uint8_t * FoutBuf     = NULL;
static uint32_t  Fsize_out   = 0;
static uint32_t  FoutIndex   = 0;


#define RLE_MASK_LEN    0x7F
#define RLE_MASK_TYPE   0x80
#define RLE_TYPE_RAND   0x00
#define RLE_TYPE_REPEAT 0x80
#define RLE_MAX_LEN     127
#define RLE_CTRL_END    0x00
#define RLE_MAX_BLOCK   127
#define RLE_CHANGE_COST 2


static uint8_t rle_queued[128];
static int rle_queue_idx = 0;
static int run_len = 0;


// Initialize the buffer vars
static void initbufs(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_out) {

    FinBuf     = inBuf;
    Fsize_in   = size_in;
    FinIndex   = 0;

    pp_FoutBuf = pp_outBuf;
    FoutBuf    = *pp_outBuf;
    Fsize_out  = size_out;
    FoutIndex = 0;
}


static void check_write_size(int len) {

    // Grow output buffer if needed
    if ((FoutIndex + len) >= Fsize_out) {
        uint8_t * p_tmp = *pp_FoutBuf;

        // Reallocate to twice as large
        Fsize_out = Fsize_out * 2;
        *pp_FoutBuf = (void *)realloc(*pp_FoutBuf, Fsize_out);

        // If realloc failed, free original buffer before quitting
        if (!(*pp_FoutBuf)) {
            printf("Error: Failed to grow memory for output buffer!\n");
            if (p_tmp) free(p_tmp);
            p_tmp = NULL;
            exit(EXIT_FAILURE);
        } else
            FoutBuf = *pp_FoutBuf; // Update working pointer
    }
}



static void write_end_of_data(void) {

    check_write_size(1);                 // writing 1 control byte
    FoutBuf[FoutIndex++] = RLE_CTRL_END; // Write sequence control byte
}


static void write_run_repeat(int len, uint8_t value) {

    if (len) {
        check_write_size(2); // writing 1 repeated value bytes + 1 control byte
        // Write sequence control byte (length + type)
        // Convert length to twos complement to identify it as repeat run
        FoutBuf[FoutIndex++] = ((len & RLE_MASK_LEN) ^ 0xFFu) + 1u;
        FoutBuf[FoutIndex++] = value;
    }
}


static void rle_commit() {

    int idx = 0;

    if (rle_queue_idx > 0) {
        check_write_size(rle_queue_idx + 1); // writing len bytes + 1 control byte
        // Write sequence control byte (length + type)
        FoutBuf[FoutIndex++] = (rle_queue_idx & RLE_MASK_LEN) | RLE_TYPE_RAND;
        while (rle_queue_idx > 0) {
            FoutBuf[FoutIndex++] = rle_queued[idx++];
            rle_queue_idx--;
        }
    }
    rle_queue_idx = 0; // redundant
}



// Convert buffer inBuf to rlecompress rle encoding and write out to outBuf
// Returns converted length
uint32_t rlecompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_out) {

    uint8_t  last, current;
    initbufs(inBuf, size_in, pp_outBuf, size_out);

    last = 0;
    run_len = 0;
    rle_queue_idx = 0;

    while (FinIndex < Fsize_in) {

        current = FinBuf[FinIndex++];

        if (current != last) {
            // The run stopped, if switching to repeat is worthwhile then
            // flush preceding random data
            if (run_len > RLE_CHANGE_COST) {
                rle_commit();
                write_run_repeat(run_len, last);
            } else {
                // Otherwise treat repeat data as random and queue it
                // with flushing as needed
                while(run_len--) {
                    if (rle_queue_idx >= RLE_MAX_BLOCK) {
                        rle_commit();
                    }
                    rle_queued[rle_queue_idx++] = last;
                }
            }
          run_len = 1;
          last = current;
        }
        else {
            // Flush pending repeat if max length is encountered
            if (run_len >= RLE_MAX_BLOCK) {
                rle_commit();
                write_run_repeat(run_len, last);
                run_len = 0;
            }
            run_len++;
        }

    }

    // If switching to repeat is worthwhile then flush preceding random data
    if (run_len > RLE_CHANGE_COST) {
        rle_commit();
        write_run_repeat(run_len, last);
    } else {
        // Otherwise treat repeat data as random and queue it
        // with flushing as needed
        while(run_len--) {
            if (rle_queue_idx >= RLE_MAX_BLOCK) {
                rle_commit();
            }
            rle_queued[rle_queue_idx++] = last;
        }
    }

    write_end_of_data();

    return FoutIndex;

}



static void write_single_byte(uint8_t data) {

    check_write_size(1);

    FoutBuf[FoutIndex++] = data;
}


static uint8_t read_single_byte(void) {

    if (FinIndex >= Fsize_in) {
        printf("Error: Read past end of input buffer!\n");
        exit(EXIT_FAILURE);
    }

    return (FinBuf[FinIndex++]);
}



// Decompress buffer inBuf from gbcompress rle encoding and write to outBuf
// Returns converted length
uint32_t rledecompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_out) {

    initbufs(inBuf, size_in, pp_outBuf, size_out);

    uint8_t token, rle_len, value;

    while (FinIndex < size_in) {

        token = read_single_byte();

        // Check for EOF token, exit if encountered
        if (token == RLE_CTRL_END)
            break;
        else if ((token & RLE_MASK_TYPE) == RLE_TYPE_RAND) {
            rle_len = token & RLE_MASK_LEN;
            while (rle_len--)
                write_single_byte(read_single_byte());
        }
        else { // if ((token & RLE_MASK_TYPE) == RLE_TYPE_REPEAT) {
            rle_len = ((token ^ 0xFF) + 1) & RLE_MASK_LEN; // length is two's complement
            value = read_single_byte();
            while (rle_len--)
                write_single_byte(value);
        }
    }

    return FoutIndex;
}
