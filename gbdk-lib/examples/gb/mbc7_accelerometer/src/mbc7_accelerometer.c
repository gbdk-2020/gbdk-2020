#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>


// **** IMPORTANT ****
//
// ** DO NOT ** use regular vsync() or wait_vbl_done() for at least 1.2 msec after
//  latching accelerometer data with mbc7_accel_read(). Doing that will cause the
// MBC7 accelerometer sensor data to have errors.
//
// This is because vsync() puts the CPU into HALT (until the next frame)
// which turns off the cartridge PHI clock that the MBC7 accelerometer
// sensor relies on.
//
// This example provides an alternative vsync_no_halt() that is safe to use
// without timing limitations.


// Accelerometer Sensor data ranges
//
// Note: The ranges below are for regular single-speed
// processor mode. In GBC Double-speed mode there appears
// to be an offset of around +440 on the X axis and +480
// on the Y axis when the cart is flat/centered.
//
// Slow rotation, measuring mostly the pull
// earth's gravity when the sensor not flat.
//
// The range is for a full rotation of 90 degrees from flat
//
// X:  Left         Centered          Right
//    0x8230 <------- 0x81BC -------> 0x8148
//     +116              0             -116
//
// Y: Forward        Centered        Backward
//    0x8253 <------- 0x81DE -------> 0x8170
//     +117              0             -110
//
//
// Higher end ranges for fast horizontal movements
//
// X:  Left         Centered          Right
//    0x837E <------- 0x81BC -------> 0x8000
//     +450              0             -444
//
// Y: Forward        Centered        Backward
//    0x837D <------- 0x81DE -------> 0x8020
//     +416              0             -446


// ADXL202 or ADXL210 accelerometer
// https://www.analog.com/media/en/technical-documentation/obsolete-data-sheets/ADXL210.pdf



bool accel_origin_set = false;
uint16_t accel_x;
uint16_t accel_y;
uint16_t accel_x_origin = 0u;
uint16_t accel_y_origin = 0u;
int16_t accel_x_relative = 0;
int16_t accel_y_relative = 0;



// Call this once per frame to update the accelerometer data
//
// Optionally it may be installed as a VBL handler
void mbc7_accel_read(void) {
    // Read the Raw sensor values
    accel_x = ((uint16_t)(rMBC7_ACCEL_X_HI) << 8) | (uint16_t)rMBC7_ACCEL_X_LO;
    accel_y = ((uint16_t)(rMBC7_ACCEL_Y_HI) << 8) | (uint16_t)rMBC7_ACCEL_Y_LO;

    // Latch data that doesn't get read until next frame
    // requires ~1.2 msec of settling time during which the CPU
    // ** MUST NOT ** be put into HALT or STOP (so do not use regular
    // vsync() or wait_vbl_done(), instead use the vsync_no_halt() ).
    rMBC7_LATCH_1 = MBC7_LATCH_ERASE;
    rMBC7_LATCH_2 = MBC7_LATCH_CAPTURE;
}


void mbc7_accel_init(void) {

    // Enable SRAM MVBC7 access (required for accessing accelerometer data)
    rMBC7_SRAM_ENABLE_1 = MBC7_SRAM_ENABLE_KEY_1;
    rMBC7_SRAM_ENABLE_2 = MBC7_SRAM_ENABLE_KEY_2;

    // Do a single read to prime the sensor
    mbc7_accel_read();

    // The data read won't be valid until ~1.2 msec after latching
    // so for the initial one just delay
    delay(2);
}


void mbc7_accel_setorigin(void) {
    accel_x_origin = accel_x;
    accel_y_origin = accel_y;
    accel_origin_set = true;
}


void mbc7_accel_update(bool do_set_origin) {

    mbc7_accel_read();

    if (do_set_origin) {
        mbc7_accel_setorigin();
    }

    if (accel_origin_set) {
        // Calculate relative movement of sensor compared to user prompted origin
        accel_x_relative = (int16_t)(accel_x_origin - accel_x);
        accel_y_relative = (int16_t)(accel_y_origin - accel_y);
    }
}
