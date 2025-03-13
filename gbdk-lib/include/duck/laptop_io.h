#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

/** @file duck/laptop_io.h

    @anchor megaduck_laptop_io_docs
    # MegaDuck Laptop Peripheral IO support

    The MegaDuck Laptop models (Spanish and German) have
    several built-in hardware peripherals which are attached
    via a controller that is communicated with using the
    serial link port.

    @note Using the duck_io_* functions referenced from this
    header file will cause the duck_io_ serial interrupt
    handler to be automatically installed.

    To use any functions here, @ref duck_io_laptop_init()
    must be called first (just once).

    For the present time all of the serial operations are
    blocking, they do not return until completed.
*/

#ifndef _MEGADUCK_LAPTOP_IO_H
#define _MEGADUCK_LAPTOP_IO_H


// Commands sent via serial IO to the Duck laptop peripheral hardware
#define DUCK_IO_CMD_INIT_START                0x00u   /**< Command to request starting the hardware init counter sequence process */
#define DUCK_IO_CMD_GET_KEYS                  0x00u   /**< Command to get hardware keyboard data by receiving a multi-byte packet*/
#define DUCK_IO_CMD_DONE_OR_OK                0x01u
// #define DUCK_IO_CMD_DONE_OR_OK_AND_SOMETHING  0x81u
#define DUCK_IO_CMD_ABORT_OR_FAIL             0x04u
#define DUCK_IO_CMD_PLAY_SPEECH               0x05u
#define DUCK_IO_CMD_RUN_CART_IN_SLOT          0x08u
#define DUCK_IO_CMD_PRINT_INIT_MAYBE_EXT_IO   0x09u   /**< Command to init the printer and return status + model type */
#define DUCK_IO_CMD_SET_RTC                   0x0Bu   /**< Command to set hardware RTC by sending a multi-byte packet */
#define DUCK_IO_CMD_GET_RTC                   0x0Cu   /**< Command to get hardware RTC by receiving a a multi-byte packet */
#define DUCK_IO_CMD_PRINT_SEND_BYTES          0x11u   /**< Send printer data */


// #define FF60_REG_BEFORE_XFER       0x00u
#define DUCK_IO_REPLY_BOOT_UNSET      0x00u
#define DUCK_IO_REPLY_BOOT_FAIL       0x01u
#define DUCK_IO_REPLY_BUFFER_XFER_OK  0x01u
#define DUCK_IO_REPLY_SEND_BUFFER_OK  0x03u
// #define DUCK_IO_REPLY_READ_FAIL_MAYBE    0x00u
#define DUCK_IO_REPLY_BOOT_OK         0x01u

#define DUCK_IO_LEN_KBD_GET           2u /**< Get Keyboard key payload size: 2 bytes Payload (excludes 1 length header byte, 1 byte Checksum) */
#define DUCK_IO_LEN_RTC_GET           8u /**< Get RTC          payload size: 8 bytes Payload (excludes 1 length header byte, 1 byte Checksum) */
#define DUCK_IO_LEN_RTC_SET           8u /**< Set RTC          payload size: 8 bytes Payload (excludes 1 length header byte, 1 byte Checksum) */
#define DUCK_IO_LEN_PLAY_SPEECH       1u /**< Play Speech      payload size: 1 byte  Payload (excludes 1 length header byte, 1 byte Checksum) */

#define DUCK_IO_REPLY_NO_CART_IN_SLOT 06u

// #define MEGADUCK_KBD_BYTE_1_EXPECT   0x0Eu
// #define MEGADUCK_SIO_BOOT_OK         0x01u

#define DUCK_IO_LEN_RX_MAX            14u // 13 data bytes + 1 checksum byte max reply length?
#define DUCK_IO_LEN_TX_MAX            14u // 13 data bytes + 1 checksum byte max reply length?

#define DUCK_IO_TIMEOUT_2_MSEC        2u  // Used for hardware init counter sequence
#define DUCK_IO_TIMEOUT_100_MSEC      100u
#define DUCK_IO_TIMEOUT_200_MSEC      200u


// Pre-recorded Speech Samples for playback
#define DUCK_IO_SPEECH_CMD_MIN 1
#define DUCK_IO_SPEECH_CMD_MAX 6


// RTC packet byte ordering (all in BCD format)
#define DUCK_IO_RTC_YEAR    0u
#define DUCK_IO_RTC_MON     1u
#define DUCK_IO_RTC_DAY     2u
#define DUCK_IO_RTC_WEEKDAY 3u
#define DUCK_IO_RTC_AMPM    4u
#define DUCK_IO_RTC_HOUR    5u
#define DUCK_IO_RTC_MIN     6u
#define DUCK_IO_RTC_SEC     7u


// Keyboard packet byte ordering (all in BCD format)
#define DUCK_IO_KBD_FLAGS   0u
#define DUCK_IO_KBD_KEYCODE 1u


// Printer init reply related
// Init Reply Bit.0
#define DUCK_IO_PRINTER_INIT_MASK   0x01u // Bit.0
#define DUCK_IO_PRINTER_INIT_OK     0x01u
#define DUCK_IO_PRINTER_INIT_FAIL   0x00u

// Init Reply Bit.1
#define DUCK_IO_PRINTER_TYPE_MASK   0x02u // Bit.1
#define DUCK_IO_PRINTER_TYPE_2_PASS 0x00u // Bit.1 = 0  // 13 x 12 byte packets + 1 x 5 or 6 byte packet (with CR and/or LF)
#define DUCK_IO_PRINTER_TYPE_1_PASS 0x02u // Bit.1 = 1  // 3 x 12 byte packets + 118 non-packet bytes



extern volatile bool    duck_io_rx_byte_done;
extern volatile uint8_t duck_io_rx_byte;


// TODO: change these to user supplied buffers?
extern uint8_t duck_io_rx_buf[DUCK_IO_LEN_RX_MAX];
extern uint8_t duck_io_rx_buf_len;

extern uint8_t duck_io_tx_buf[DUCK_IO_LEN_TX_MAX];
extern uint8_t duck_io_tx_buf_len;


// ===== Low level helper IO functions =====


// TODO: No longer in use(?)
//
// Waits for a serial transfer to complete with a timeout
//
//    @param timeout_len_ms   Unit size is in msec (100 is about ~ 103 msec or 6.14 frames)
//
//    Serial ISR populates status var if anything was received
//
//  void duck_io_wait_done_with_timeout(uint8_t timeout_len_ms);


/** Sends a byte over serial to the MegaDuck laptop peripheral

    @param tx_byte  Byte to send

*/
void duck_io_send_byte(uint8_t tx_byte);


/** Reads a byte over serial from the MegaDuck laptop peripheral with NO timeout

    Returns: the received byte

    If there is no reply then it will hang forever
*/
uint8_t duck_io_read_byte_no_timeout(void);


/** Prepares to receive serial data from the MegaDuck laptop peripheral

    \li Sets serial IO to external clock and enables ready state.
    \li Turns on Serial interrupt, clears any pending interrupts and
        then turns interrupts on (state of @ref IE_REG should be
        preserved before calling this and then restored at the end of
        the serial communication being performed).
*/
void duck_io_enable_read_byte(void);


/** Performs init sequence over serial with the MegaDuck laptop peripheral

    Returns `true` if successful, otherwise `false`

    Needs to be done *just once* any time system is powered
    on or a cartridge is booted.

    Sends count up sequence + some commands, then waits for
    a matching count down sequence in reverse.
*/
bool duck_io_laptop_init(void);


/** Returns whether the MegaDuck Printer was detected during duck_io_laptop_init()

    Returns `true` if successful, otherwise `false`

    @ref duck_io_laptop_init() must be called first
*/
bool duck_io_printer_detected(void);


/** Returns which type of MegaDuck Printer was detected during duck_io_laptop_init()

    Return value should be one of the following:
    \li @ref DUCK_IO_PRINTER_TYPE_1_PASS (single pass monochrome)
    \li @ref DUCK_IO_PRINTER_TYPE_2_PASS (two pass possibly supports shades of grey)

    @ref duck_io_laptop_init() must be called first
*/
uint8_t duck_io_printer_type(void);


// ===== Higher level IO functions =====


/** Waits to receive a byte over serial from the MegaDuck laptop peripheral with a timeout

    @param timeout_len_ms   Unit size is in msec (100 is about ~ 103 msec or 6.14 frames)

    Returns:
    \li `true`: Success, received byte will be in `duck_io_rx_byte` global
    \li `false`: Read timed out with no reply
*/
bool duck_io_read_byte_with_msecs_timeout(uint8_t timeout_len_ms);


/** Sends a byte over over serial to the MegaDuck laptop peripheral and waits for a reply with a timeout

    @param tx_byte          Byte to send
    @param timeout_len_ms   Unit size is in msec (100 is about ~ 103 msec or 6.14 frames)
    @param expected_reply   The expected value of the reply byte

    Returns:
    \li `true`: Success
    \li `false`: if timed out or reply byte didn't match expected value
*/
bool duck_io_send_byte_and_check_ack_msecs_timeout(uint8_t tx_byte, uint8_t timeout_len_ms, uint8_t expected_reply);


/** Sends a command and a multi-byte buffer over serial to the MegaDuck laptop peripheral

    @param io_cmd    Command byte to send

    The data should be pre-loaded into these globals:
    \li @ref duck_io_tx_buf : Buffer with data to send
    \li @ref duck_io_tx_buf_len : Number of bytes to send

    Returns: `true` if succeeded

    @see DUCK_IO_CMD_GET_KEYS, DUCK_IO_CMD_SET_RTC
*/
bool duck_io_send_cmd_and_buffer(uint8_t io_cmd);


/** Sends a command and then receives a multi-byte buffer over serial from the MegaDuck laptop peripheral

    @param io_cmd    Command byte to send

    If successful, the received data and length will be in these globals:
    \li @ref duck_io_rx_buf : Buffer with received data
    \li @ref duck_io_rx_buf_len : Number of bytes received

    Returns: `true` if succeeded, `false` if failed (could be no reply, failed checksum, etc)

    @see DUCK_IO_CMD_GET_RTC
*/
bool duck_io_send_cmd_and_receive_buffer(uint8_t io_cmd);

#endif // _MEGADUCK_LAPTOP_IO_H
