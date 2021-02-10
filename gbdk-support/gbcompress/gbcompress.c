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

const uint8_t token_byte  = 0x00;
const uint8_t token_word  = 0x40;
const uint8_t token_str   = 0x80;
const uint8_t token_trash = 0xC0;

const uint8_t EOFMarker  = 0x00;

uint8_t * FinBuf    = 0;
uint8_t * FoutBuf   = 0;
uint32_t  Fsize_in   = 0;
uint32_t  FOutIndex = 0;
uint32_t  Fsize_max  = 0;

static void check_write_size(uint8_t len) {

    if ((FOutIndex + len) >= Fsize_max) {
        printf("Error: OutBuf too small");
        exit(EXIT_FAILURE);
    }
}


static void write_byte(uint8_t len, uint8_t data) {

    check_write_size(2); // writing 2 bytes

    FoutBuf[FOutIndex++] = ((len - 1) & len_mask);
    FoutBuf[FOutIndex++] = data;
}


static void write_word( uint8_t len, uint16_t data ) {

    check_write_size(3); // writing 3 bytes

    FoutBuf[FOutIndex++] = (((len - 1) & len_mask) | token_word);
    FoutBuf[FOutIndex++] = (uint8_t)((data >> 8) & 0xFF);
    FoutBuf[FOutIndex++] = (uint8_t)(data & 0xFF);
}


static void write_string( uint8_t len, uint16_t data) {

    check_write_size(3); // writing 3 bytes

    // conver's complement does not give the negation, see § Most negative number below. t back-ref offset from positive unsigned to negative signed
    data = (data ^ 0xFFFF) + 1;
    
    FoutBuf[FOutIndex++] = (((len - 1) & len_mask) | token_str);
    FoutBuf[FOutIndex++] = (uint8_t)(data & 0xFF);
    FoutBuf[FOutIndex++] = (uint8_t)((data >> 8) & 0xFF);
}


static void write_trash( uint8_t len, uint8_t * pos) {

    uint8_t i;

    check_write_size(len); // writing len bytes

    FoutBuf[FOutIndex++] = (((len-1) & len_mask) | token_trash);
    for (i=0; i < len; i++)
        FoutBuf[FOutIndex++] = pos[i];
}


static void write_end(void) {

    check_write_size(1); // writing 1 byte

    FoutBuf[FOutIndex++] = EOFMarker;
}


static int read_uint16_t(uint16_t byte_pos, uint16_t * out_data) {

    if ((byte_pos + 2) < Fsize_in) {
        *out_data = (uint16_t)((FinBuf[byte_pos] << 8) + (uint32_t)FinBuf[byte_pos+1]);
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
uint32_t gbcompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t * outBuf, uint32_t size_max) {

    uint32_t  byte_pos = 0; // bp

    uint8_t   rle_u8_match;  // x
    uint16_t  rle_u16_match; // y

    uint32_t  rle_u8_len;   // r_rb
    uint32_t  rle_u16_len;  // r_rw
    uint32_t  rle_str_len;  // r_rs
    uint32_t  trash_len;    // tb (by "trash" the original author meant, "non-rle sequence of bytes")

    uint32_t  rle_str_start;       // rr
    uint32_t  rle_str_back_offset; // sr (this is signed in original code, handled differently to be unsigned now)
    uint32_t  rle_str_len_work;    // rl

    FinBuf   = inBuf;
    FoutBuf  = outBuf;
    Fsize_in  = size_in;
    Fsize_max = size_max;

    byte_pos = 0;
    trash_len = 0;
    FOutIndex = 0;

    while (byte_pos < Fsize_in) {

        // Check for u8 RLE run up to 63 bytes max
        rle_u8_match = FinBuf[byte_pos];
        rle_u8_len = 1;
        while ((byte_pos + rle_u8_len) < Fsize_in) {
            // If the current u8 matches, increment the length
            if ((FinBuf[byte_pos + rle_u8_len] == rle_u8_match) &&
                (rle_u8_len < 64)) {
                rle_u8_len++;
            } 
            else break;
        }
  
        // Check for overlapping uint16_t RLE run up to 63 bytes max
        // Read in initial u16 to match against
        if (read_uint16_t(byte_pos, &rle_u16_match)) {
            uint16_t  temp_u16;
            rle_u16_len = 1;
            // If the current u16 matches, increment the length
            while (read_uint16_t(byte_pos + (rle_u16_len * 2), &temp_u16)) {
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
        rle_str_start = 0; // 
        rle_str_back_offset = 0;
        rle_str_len = 0;
        while (rle_str_start < byte_pos) {
            rle_str_len_work = 0;

            // Check for a matching run at rle_str_start against byte_pos 
            // and save length to rle_str_len_work
            while ((byte_pos + rle_str_len_work) < Fsize_in) {
                // Test to see if u8 in current sequence matches the u8 for a previous sequence
                // Break out of the current sequence if it would reach 64 bytes
                // or it would reach the current byte pos (and cause an overlap)
                if ((FinBuf[rle_str_start + rle_str_len_work] == FinBuf[byte_pos + rle_str_len_work]) &&
                    ((rle_str_start + rle_str_len_work) < byte_pos) &&
                    (rle_str_len_work < 64)) {

                    rle_str_len_work++;
                }
                else break;
            }

            // If the newly tested sequence length is greater than the previous length
            // then save the length and store an negative offset to it in rle_str_back_offset
            if (rle_str_len_work > rle_str_len) {
                rle_str_back_offset = byte_pos - rle_str_start; // Changed this from neg offset to pos, and gets flipped to negative on writing it out
                rle_str_len = rle_str_len_work;
            }

            rle_str_start++;
        }


        // Write out any rle data if it's ready
        if ((rle_u8_len > 2) && 
            (rle_u8_len > rle_u16_len) && 
            (rle_u8_len > rle_str_len)) {
            flush_trash(&byte_pos, &trash_len);
            write_byte(rle_u8_len, rle_u8_match);
            byte_pos = byte_pos + rle_u8_len;
        } 
        else if ((rle_u16_len > 2) && 
                 ((rle_u16_len*2) > rle_str_len)) {
            flush_trash(&byte_pos, &trash_len);
            write_word(rle_u16_len, rle_u16_match);
            byte_pos = byte_pos + rle_u16_len*2;
        }
        else if (rle_str_len > 3) {
            flush_trash(&byte_pos, &trash_len);
            write_string(rle_str_len, rle_str_back_offset);
            byte_pos = byte_pos + rle_str_len;
        }
        else if (trash_len >= 64) {
            write_trash(trash_len, &FinBuf[byte_pos-trash_len]);
            trash_len = 0;
        } 
        else {
            trash_len++;
            byte_pos++;
        }

    }

    // Flush any remaining "trash" bytes
    flush_trash(&byte_pos, &trash_len);

    write_end();

    return FOutIndex;
}



// Decompress buffer inBuf from gbcompress rle encoding and write to outBuf
// Returns converted length
uint32_t gbdecompress_buf(uint8_t * inBuf, uint32_t size_in, uint8_t ** pp_outBuf, uint32_t size_max) {

    uint8_t  rle_val[2];
    uint8_t  token;
    uint8_t  rle_toggle;
    uint8_t * p_rle_backref;
    uint8_t * inBuf_end = inBuf + size_in - 1;
    uint8_t * p_out_buf = *pp_outBuf; // Make a working copy of output buffer pointer

    uint32_t rle_len;
    uint32_t rle_offset;
    uint32_t size_out;

    size_out = 0;
    rle_len = 0;

    while (inBuf <= inBuf_end) {


        // Load the next RLE token if needed
        if (rle_len == 0) {

            // Check for EOF token
            if (*inBuf == EOFMarker)
                break;

            // Read a RLE token
            // First byte should always be a token
            token   = (*inBuf) & token_mask;
            rle_len = ((*inBuf) & len_mask) + 1;

            inBuf++; // Move to next ENCODED byte

            rle_toggle = 0; // Reset RLE decoded byte flipflop

            // Reading a RLE Length zero = END of encoded data
            // Then break out of loop and quit
            if (rle_len == 0) {
                break;
            }

            // Read how to handle ENCODED RLE Value
            switch (token) {
                case token_byte:
                    // Load only one byte
                    rle_val[0] = *inBuf;
                    inBuf++;
                    break;

                case token_word:
                    // If token is Word double the RLE decode length
                    rle_len *= 2;

                    // Load LS byte then MS Byte
                    rle_val[0] = *inBuf;
                    inBuf++;
                    rle_val[1] = *inBuf;
                    inBuf++;
                    break;

                case token_str:
                    // This token: copy N bytes from negative offset of current
                    // DECODED memory location (back reference)
                    rle_offset = (uint16_t)(*inBuf);
                    inBuf++;
                    rle_offset |= (uint16_t)((*inBuf)<<8);
                    inBuf++;
                    // Convert input from from Signed to Unsigned
                    rle_offset = (rle_offset ^ 0xFFFF) + 1;

                    // Assign the string back reference relative to the current buffer pointer
                    p_rle_backref = p_out_buf - rle_offset;
                    break;

                case token_trash: // AKA "Trash Bytes" in GBTD
                    // This token : copy next N bytes directly from ENCODED input
                    break;
            }
        }
        else // implied: if (rle_len > 0) 
        {

            // Grow output buffer if needed
            if (size_out == size_max) {
                uint8_t * p_tmp = *pp_outBuf;
                size_max = size_max * 2;
                *pp_outBuf = (void *)realloc(*pp_outBuf, size_max);
                // If realloc failed, free original buffer before quitting
                if (!(*pp_outBuf)) {
                    printf("Error: Failed to reallocate memory for output buffer!\n");
                    if (p_tmp) free(p_tmp);
                    exit(EXIT_FAILURE);
                }
                // Remap working output pointers based on their offset from the previous one
                p_out_buf = *pp_outBuf + (p_out_buf - p_tmp);
                p_rle_backref = *pp_outBuf + (p_rle_backref - p_tmp);
            }

            // Copy the decoded byte into VRAM
            switch (token) {
                case token_byte:
                    // Copy from cached repeating RLE value
                    *p_out_buf = rle_val[0];
                    break;

                case token_word:
                    // Copy from cached repeating RLE value
                    // Toggle between MS/LS bytes input values
                    *p_out_buf = rle_val[rle_toggle];
                    rle_toggle ^= 0x01;
                    break;

                case token_str:
                    // Copy byte from the backreferenced VRAM address
                    // Then increment backreference to next VRAM byte
                    *p_out_buf = *p_rle_backref;
                    p_rle_backref++;
                    break;

                case token_trash:
                    // Copy directly from encoded input
                    *p_out_buf = *inBuf;
                    inBuf++;
                    break;
            }

            p_out_buf++; // Move to next output byte
            size_out++;
            rle_len--; // Decrement number of RLE bytes remaining
        }
    }

    return size_out;
}
