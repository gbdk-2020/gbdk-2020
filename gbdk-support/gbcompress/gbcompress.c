// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org>
// bbbbbr 2020

#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "gbcompress.h"

const uint8_t len_mask    = 0x3F;
const uint8_t token_mask  = 0xC0;

#define token_byte  0x00
#define token_word  0x40
#define token_str   0x80
#define token_trash 0xC0

const uint8_t EOFMarker  = 0x00;

uint8_t * FinBuf      = NULL;
uint32_t  Fsize_in    = 0;
uint32_t  FinIndex    = 0;

uint8_t ** pp_FoutBuf = NULL;
uint8_t * FoutBuf     = NULL;
uint32_t  Fsize_out   = 0;
uint32_t  FoutIndex   = 0;


static void check_write_size(uint8_t len) {

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


static void write_byte(uint8_t len, uint8_t data) {

    check_write_size(2); // writing 2 bytes

    FoutBuf[FoutIndex++] = ((len - 1) & len_mask);
    FoutBuf[FoutIndex++] = data;
}


static void write_word( uint8_t len, uint16_t data ) {

    check_write_size(3); // writing 3 bytes

    FoutBuf[FoutIndex++] = (((len - 1) & len_mask) | token_word);
    FoutBuf[FoutIndex++] = (uint8_t)((data >> 8) & 0xFF);
    FoutBuf[FoutIndex++] = (uint8_t)(data & 0xFF);
}


static void write_string( uint8_t len, uint16_t data) {

    check_write_size(3); // writing 3 bytes

    // conver's complement does not give the negation, see § Most negative number below. t back-ref offset from positive unsigned to negative signed
    data = (data ^ 0xFFFF) + 1;

    FoutBuf[FoutIndex++] = (((len - 1) & len_mask) | token_str);
    FoutBuf[FoutIndex++] = (uint8_t)(data & 0xFF);
    FoutBuf[FoutIndex++] = (uint8_t)((data >> 8) & 0xFF);
}


static void write_trash( uint8_t len, uint8_t * pos) {

    uint8_t i;

    check_write_size(len); // writing len bytes

    FoutBuf[FoutIndex++] = (((len-1) & len_mask) | token_trash);
    for (i=0; i < len; i++)
        FoutBuf[FoutIndex++] = pos[i];
}


static void write_end(void) {

    check_write_size(1); // writing 1 byte

    FoutBuf[FoutIndex++] = EOFMarker;
}


static int read_uint16_t(uint32_t byte_pos, uint16_t * out_data) {

    if ((byte_pos + 2) < Fsize_in) {
        *out_data = (uint16_t)((FinBuf[byte_pos] << 8) + (uint16_t)FinBuf[byte_pos+1]);
        return true;
    }
    else return false;
}


// Writes out any pending "trash" (non-rle sequence) and resets the counter
static void flush_trash(uint32_t * byte_pos, uint32_t * trash_len) {

    if (*trash_len > 0) {
        write_trash(*trash_len, &FinBuf[*byte_pos - *trash_len]);
        *trash_len = 0;
    }
}


// Convert buffer inBuf to gbcompress rle encoding and write out to outBuf
// Returns converted length
uint32_t gbcompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_out) {

    uint8_t   rle_u8_match;  // x
    uint16_t  rle_u16_match; // y

    uint32_t  rle_u8_len;   // r_rb
    uint32_t  rle_u16_len;  // r_rw
    uint32_t  rle_str_len;  // r_rs
    uint32_t  trash_len;    // tb (by "trash" the original author meant, "non-rle sequence of bytes")

    uint32_t  rle_str_start;       // rr
    uint32_t  rle_str_back_offset; // sr (this is signed in original code, handled differently to be unsigned now)
    uint32_t  rle_str_len_work;    // rl

    FinBuf     = inBuf;
    Fsize_in   = size_in;
    FinIndex   = 0;         // bp

    pp_FoutBuf = pp_outBuf;
    FoutBuf    = *pp_outBuf;
    Fsize_out  = size_out;
    FoutIndex = 0;

    trash_len = 0;

    while (FinIndex < Fsize_in) {

        // Check for u8 RLE run up to 63 bytes max
        rle_u8_match = FinBuf[FinIndex];
        rle_u8_len = 1;
        while ((FinIndex + rle_u8_len) < Fsize_in) {
            // If the current u8 matches, increment the length
            if ((FinBuf[FinIndex + rle_u8_len] == rle_u8_match) &&
                (rle_u8_len < 64)) {
                rle_u8_len++;
            }
            else break;
        }

        // Check for overlapping uint16_t RLE run up to 63 bytes max
        // Read in initial u16 to match against
        if (read_uint16_t(FinIndex, &rle_u16_match)) {
            uint16_t  temp_u16;
            rle_u16_len = 1;
            // If the current u16 matches, increment the length
            while (read_uint16_t(FinIndex + (rle_u16_len * 2), &temp_u16)) {
                if ((temp_u16 == rle_u16_match) &&
                    (rle_u16_len < 64)) {
                    rle_u16_len++;
                }
                else break;
            }
        }

        // Check for matching sequences starting at current position
        // against all previous data beginning at start up to 63 bytes max
        // (back reference "strings")
        rle_str_back_offset = 0;
        rle_str_len = 0;
        // Max string backreference length is 16 bits unsigned
        // adjust search start accordingly
        if (FinIndex > 0xFFFF)
            rle_str_start = FinIndex - 0xFFFF;
        else
            rle_str_start = 0;

        while (rle_str_start < FinIndex) {
            rle_str_len_work = 0;

            // Check for a matching run at rle_str_start against FinIndex
            // and save length to rle_str_len_work
            while ((FinIndex + rle_str_len_work) < Fsize_in) {
                // Test to see if u8 in current sequence matches the u8 for a previous sequence
                // Break out of the current sequence if it would reach 64 bytes
                // or it would reach the current byte pos (and cause an overlap)
                if ((FinBuf[rle_str_start + rle_str_len_work] == FinBuf[FinIndex + rle_str_len_work]) &&
                    ((rle_str_start + rle_str_len_work) < FinIndex) &&
                    (rle_str_len_work < 64)) {

                    rle_str_len_work++;
                }
                else break;
            }

            // If the newly tested sequence length is greater than the previous length
            // then save the length and store an negative offset to it in rle_str_back_offset
            if (rle_str_len_work > rle_str_len) {
                rle_str_back_offset = FinIndex - rle_str_start; // Changed this from neg offset to pos, and gets flipped to negative on writing it out
                rle_str_len = rle_str_len_work;
            }

            rle_str_start++;
        }


        // Write out any rle data if it's ready
        if ((rle_u8_len > 2) &&
            (rle_u8_len > rle_u16_len) &&
            (rle_u8_len > rle_str_len)) {
            flush_trash(&FinIndex, &trash_len);
            write_byte(rle_u8_len, rle_u8_match);
            FinIndex = FinIndex + rle_u8_len;
        }
        else if ((rle_u16_len > 2) &&
                 ((rle_u16_len*2) > rle_str_len)) {
            flush_trash(&FinIndex, &trash_len);
            write_word(rle_u16_len, rle_u16_match);
            FinIndex = FinIndex + rle_u16_len*2;
        }
        else if (rle_str_len > 3) {
            flush_trash(&FinIndex, &trash_len);
            write_string(rle_str_len, rle_str_back_offset);
            FinIndex = FinIndex + rle_str_len;
        }
        else if (trash_len >= 64) {
            write_trash(trash_len, &FinBuf[FinIndex-trash_len]);
            trash_len = 0;
        }
        else {
            trash_len++;
            FinIndex++;
        }

    }

    // Flush any remaining "trash" bytes
    flush_trash(&FinIndex, &trash_len);

    write_end();

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
uint32_t gbdecompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_out) {

    uint8_t  rle_val[2];
    uint8_t  token;
    uint8_t  rle_toggle;
    uint8_t * inBuf_end = inBuf + size_in - 1;
    uint32_t rle_len;
    uint32_t backref_index;

    FinBuf     = inBuf;
    Fsize_in   = size_in;
    FinIndex   = 0;

    pp_FoutBuf = pp_outBuf;
    FoutBuf    = *pp_outBuf;
    Fsize_out  = size_out;
    FoutIndex  = 0;


    while (FinIndex < size_in) {

        token = read_single_byte();

        // Check for EOF token, exit if encountered
        if (token == EOFMarker)
            break;

        // Read a RLE token
        // First byte should always be a token
        rle_len = (token & len_mask) + 1;
        token   =  token & token_mask;

        // Read how to handle ENCODED RLE Value
        switch (token) {
            case token_byte:
                // Load only one byte
                rle_val[0] = read_single_byte();
                break;

            case token_word:
                // If token is Word double the RLE decode length
                rle_len *= 2;
                rle_toggle = 0; // Reset RLE decoded word/byte flipflop

                // Load LS byte then MS Byte
                rle_val[0] = read_single_byte();
                rle_val[1] = read_single_byte();
                break;

            case token_str:
                // This token: copy N bytes from negative offset of current
                // DECODED memory location (back reference)
                backref_index = ((uint16_t)read_single_byte() | (( (uint16_t)read_single_byte() ) << 8));
                // Convert input from from Signed to Unsigned
                backref_index = (backref_index ^ 0xFFFF) + 1;

                // Assign the string back reference relative to the current buffer pointer
                backref_index = (FoutIndex - (uint32_t)backref_index);
                break;

            case token_trash: // AKA "Trash Bytes" in GBTD
                // This token : copy next N bytes directly from ENCODED input
                break;
        }

        while (rle_len) {

            // Copy the decoded byte into VRAM
            switch (token) {
                case token_byte:
                    // Copy from cached repeating RLE value
                    write_single_byte(rle_val[0]);
                    break;

                case token_word:
                    // Copy from cached repeating RLE value
                    // Toggle between MS/LS bytes input values
                    write_single_byte(rle_val[rle_toggle]);
                    rle_toggle ^= 0x01;
                    break;

                case token_str:
                    // Copy byte from the backreferenced VRAM address
                    // Then increment backreference to next VRAM byte
                    write_single_byte(FoutBuf[backref_index++]);
                    break;

                case token_trash:
                    // Copy directly from encoded input
                    write_single_byte(read_single_byte());
                    break;
            }

            rle_len--; // Decrement number of RLE bytes remaining
        }
    }

    return FoutIndex;
}
