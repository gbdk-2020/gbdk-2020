#include <gbdk/platform.h>
#include <gb/isr.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/laptop_io.h>

// volatile SFR __at(0xFF60) FF60_REG;

// TODO: namespace to megaduck
volatile bool    duck_io_rx_byte_done;
volatile uint8_t duck_io_rx_byte;

 uint8_t duck_io_rx_buf[DUCK_IO_LEN_RX_MAX];
 uint8_t duck_io_rx_buf_len;

 uint8_t duck_io_tx_buf[DUCK_IO_LEN_TX_MAX];
 uint8_t duck_io_tx_buf_len;

 uint8_t duck_io_priniter_init_result = DUCK_IO_PRINTER_FAIL;  // Default printer to not connected


static void _delay_1_msec(void);
static bool duck_io_controller_init(void);

/** Serial link handler for receiving data send by the MegaDuck laptop peripheral

*/
void duck_io_sio_isr(void) CRITICAL INTERRUPT {

    // Save received data and update status flag
    // Turn Serial ISR back off
    duck_io_rx_byte = SB_REG;
    duck_io_rx_byte_done = true;
    set_interrupts(IE_REG & ~SIO_IFLAG);
}

ISR_VECTOR(VECTOR_SERIAL, duck_io_sio_isr)



static void _delay_1_msec(void) {
    volatile uint8_t c = 75u;
    while (c--);
}


// TODO: No longer in use(?)
//
// void duck_io_wait_done_with_timeout(uint8_t timeout_len_ms) {
//     while (timeout_len_ms--) {
//         _delay_1_msec();
//         if (duck_io_rx_byte_done)
//             return;
//     }
// }


void duck_io_send_byte(uint8_t tx_byte) {

    // FF60_REG = FF60_REG_BEFORE_XFER;  // Seems optional in testing so far
    SB_REG = tx_byte;
    SC_REG = SIOF_XFER_START | SIOF_CLOCK_INT;

    // TODO: the delay here seems inefficient, but need to find out actual timing on the wire first
    _delay_1_msec();

    // Restore to SIO input and clear pending interrupt
    IF_REG = 0x00;
    SC_REG = SIOF_XFER_START | SIOF_CLOCK_EXT;
}


void duck_io_enable_read_byte(void) {
    // FF60_REG = FF60_REG_BEFORE_XFER;
    SC_REG = (SIOF_XFER_START | SIOF_CLOCK_EXT);
    IE_REG |= SIO_IFLAG;
    IF_REG = 0x00;
    enable_interrupts();
}


uint8_t duck_io_read_byte_no_timeout(void) {
    CRITICAL {
        duck_io_rx_byte_done = false;
    }

    duck_io_enable_read_byte();
    while (!duck_io_rx_byte_done);
    return duck_io_rx_byte;
}


bool duck_io_read_byte_with_msecs_timeout(uint8_t timeout_len_ms) {
    uint8_t msec_counter;
    CRITICAL {
        duck_io_rx_byte_done = false;
    }

    duck_io_enable_read_byte();

    while (timeout_len_ms--) {
        // Each full run of the inner loop is ~ 1msec
        msec_counter = 75u;
        while (msec_counter--) {
            if (duck_io_rx_byte_done)
                return true;
        }
    }

    return duck_io_rx_byte_done;
}


bool duck_io_send_byte_and_check_ack_msecs_timeout(uint8_t tx_byte, uint8_t timeout_len_ms, uint8_t expected_reply) {

    duck_io_send_byte(tx_byte);

    // A reply for the byte sent above should be incoming, fail if there is no reply
    if (!duck_io_read_byte_with_msecs_timeout(timeout_len_ms)) return false;

    // Then check reply byte vs expected reply
    return (duck_io_rx_byte == expected_reply);
}


bool duck_io_send_cmd_and_buffer(uint8_t io_cmd) {

    // Send buffer length + 2 (for length header and checksum bytes)
    uint8_t packet_length = duck_io_tx_buf_len + 2;
    uint8_t checksum_calc = packet_length; // Use total tx length (byte) as initial checksum

    // Save interrupt enables and then set only Serial to ON
    uint8_t int_enables_saved = IE_REG;
    IE_REG = SIO_IFLAG;

    // Send command to initiate buffer transfer, then check for reply
    if (!duck_io_send_byte_and_check_ack_msecs_timeout(io_cmd, DUCK_IO_TIMEOUT_200_MSEC, DUCK_IO_REPLY_SEND_BUFFER_OK)) {
        IE_REG = int_enables_saved;
        return false;
    }

    // Send buffer length + 2 (for length header and checksum bytes)
    _delay_1_msec;  // Delay, perhaps for byte serial transfer time? (present in system rom)
    if (!duck_io_send_byte_and_check_ack_msecs_timeout(packet_length, DUCK_IO_TIMEOUT_200_MSEC, DUCK_IO_REPLY_SEND_BUFFER_OK)) {
        IE_REG = int_enables_saved;
        return false;
    }

    // Send the buffer contents
    uint8_t buffer_bytes_to_send = duck_io_tx_buf_len;
    for (uint8_t idx = 0; idx < duck_io_tx_buf_len; idx++) {

        // Update checksum with next byte
        checksum_calc += duck_io_tx_buf[idx];

        // Send a byte from the buffer
        if (!duck_io_send_byte_and_check_ack_msecs_timeout(duck_io_tx_buf[idx], DUCK_IO_TIMEOUT_200_MSEC, DUCK_IO_REPLY_SEND_BUFFER_OK)) {
            IE_REG = int_enables_saved;
            return false;
        }
    }

    // Done sending buffer bytes, last byte to send is checksum 
    // Tx Checksum Byte should == (((sum of all bytes except checksum) XOR 0xFF) + 1) [two's complement]
    checksum_calc = ~checksum_calc + 1u;  // 2's complement
    // Note different expected reply value versus previous reply checks
    if (!duck_io_send_byte_and_check_ack_msecs_timeout(checksum_calc, DUCK_IO_TIMEOUT_200_MSEC, DUCK_IO_REPLY_BUFFER_XFER_OK)) {
        IE_REG = int_enables_saved;
        return false;
    }

    // Success
    IE_REG = int_enables_saved;
    return true;
}


bool duck_io_send_cmd_and_receive_buffer(uint8_t io_cmd) {

    uint8_t packet_length  = 0u;
    uint8_t checksum_calc  = 0x00u;

    // Reset global rx buffer length
    duck_io_rx_buf_len      = 0u;

    // Save interrupt enables and then set only Serial to ON
    uint8_t int_enables_saved = IE_REG;
    IE_REG = SIO_IFLAG;

    // _delay_1_msec()  // Another mystery, ignore it for now
    duck_io_send_byte(io_cmd);

    // Fail if first rx byte timed out
    if (duck_io_read_byte_with_msecs_timeout(DUCK_IO_TIMEOUT_100_MSEC)) {

        // First rx byte will be length of all incoming bytes
        if (duck_io_rx_byte <= DUCK_IO_LEN_RX_MAX) {

            // Save rx byte as length and use to initialize checksum
            // Reduce length by 1 (since it includes length byte already received)
            checksum_calc = duck_io_rx_byte;
            packet_length = duck_io_rx_byte - 1u;

            while (packet_length--) {
                // Wait for next rx byte
                if (duck_io_read_byte_with_msecs_timeout(DUCK_IO_TIMEOUT_100_MSEC)) {
                    // Save rx byte to buffer and add to checksum
                    checksum_calc                     += duck_io_rx_byte;
                    duck_io_rx_buf[duck_io_rx_buf_len++] = duck_io_rx_byte;
                } else {
                    // Error: Break out and set checksum so it fails test below (causing return with failure)
                    checksum_calc = 0xFFu;
                    break;
                }
            }

            // Done receiving buffer bytes, last rx byte should be checksum 
            // Rx Checksum Byte should == (((sum of all bytes except checksum) XOR 0xFF) + 1) [two's complement]
            // so ((sum of received bytes including checksum byte) should == -> unsigned 8 bit overflow -> 0x00
            if (checksum_calc == 0x00u) {
                // Return success
                duck_io_send_byte(DUCK_IO_CMD_DONE_OR_OK);

                // Reduce number of received bytes by 1 to strip off trailing checksum byte
                duck_io_rx_buf_len--;
                IE_REG = int_enables_saved;
                return true;
            }
        }
    }

    // Something went wrong, error out
    duck_io_send_byte(DUCK_IO_CMD_ABORT_OR_FAIL);
    IE_REG = int_enables_saved;
    return false;
}


static bool duck_io_controller_init(void) {
    uint8_t counter;

    IE_REG = SIO_IFLAG;
    bool serial_system_init_is_ok = true;

    // Send a count up sequence through the serial IO (0,1,2,3...255)
    // Exit on 8 bit unsigned wraparound to 0x00
    counter = 0u;
    do {
        duck_io_send_byte(counter++);
    } while (counter != 0u);

    // Then wait for a response
    // Fail if reply back timed out or was not expected response
    if (duck_io_read_byte_with_msecs_timeout(DUCK_IO_TIMEOUT_2_MSEC)) {
        if (duck_io_rx_byte != DUCK_IO_REPLY_BOOT_OK) serial_system_init_is_ok = false;
    } else
        serial_system_init_is_ok = false;

    // Send a command that seems to request a 255..0 countdown sequence from the external controller
    if (serial_system_init_is_ok)  {
        duck_io_send_byte(DUCK_IO_CMD_INIT_START);

        // Expects a reply sequence through the serial IO of (255,254,253...0)
        counter = 255u;

        // Exit on 8 bit unsigned wraparound to 0xFFu
        do {
            // Fail if reply back timed out or did not match expected counter
            // TODO: OEM approach doesn't break out once a failure occurs, 
            //       but maybe that's possible + sending the abort command early?
            if (duck_io_read_byte_with_msecs_timeout(DUCK_IO_TIMEOUT_2_MSEC)) {
                if (counter != duck_io_rx_byte) serial_system_init_is_ok = false;
            } else
                serial_system_init_is_ok = false;
            counter--;
        } while (counter != 255u);

        // Check for failures during the reply sequence
        // and send reply byte based on that
        if (serial_system_init_is_ok)
            duck_io_send_byte(DUCK_IO_CMD_DONE_OR_OK);
        else
            duck_io_send_byte(DUCK_IO_CMD_ABORT_OR_FAIL);
    }

    return serial_system_init_is_ok;
}


bool duck_io_laptop_init(void) {

    uint8_t int_enables_saved;
    bool duck_io_init_ok = true;

    disable_interrupts();
    int_enables_saved = IE_REG;
    SC_REG = 0x00u;
    SB_REG = 0x00u;

    // Initialize Serially attached peripheral
    duck_io_init_ok = duck_io_controller_init();
    if (duck_io_init_ok) {

        // Save response from some command
        // (so far not seen being used in System ROM 32K Bank 0)
        duck_io_send_byte(DUCK_IO_CMD_PRINT_INIT_EXT_IO);

        // TODO: This wait with no timeout is how the System ROM does it,
        //       but it can probably be changed to a long delay and
        //       attempt to fail somewhat gracefully.
        duck_io_read_byte_no_timeout();
        duck_io_priniter_init_result = duck_io_rx_byte;
    }

    // Ignore the RTC init check for now

    IE_REG = int_enables_saved;
    enable_interrupts();

    return (duck_io_init_ok);
}


uint8_t duck_io_printer_last_status(void) {
    return (duck_io_priniter_init_result);
}


uint8_t duck_io_printer_query(void) {

    // Query three times in a row, per System ROM behavior
    // Might be doing some kind of initialization/test
    for (uint8_t c = 0u; c < 3u; c++) {
        // Delay per system rom behavior
        delay(50);
        duck_io_send_byte(DUCK_IO_CMD_PRINT_INIT_EXT_IO);
        duck_io_read_byte_with_msecs_timeout(200);

        // Cache result
        duck_io_priniter_init_result = duck_io_rx_byte;
        // Fail if the printer is not detected and/or not available
        if (duck_io_rx_byte == 0) return duck_io_rx_byte;
    }
    return duck_io_rx_byte;
}
