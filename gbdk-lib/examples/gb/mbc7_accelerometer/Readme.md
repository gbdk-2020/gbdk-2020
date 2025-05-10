# GBDK example for using the Accelerometer sensor on the Game Boy MBC7 Cartridge
This example demonstrates using data from the accelerometer sensor on MBC7 cartridges
such as used by Kirby Tilt 'n' Tumble.

# Important hardware information
Do not use regular `vsync()` or `wait_vbl_done()` for at least `1.2 msec` after
triggering a latch of the accelerometer data (in this example with `mbc7_accel_read()`).
Doing that will cause the MBC7 accelerometer sensor to return errors in the data.

This is because `vsync()` puts the CPU into `HALT` (until the next frame)
which turns off the cartridge PHI clock. The MBC7 cartridge relies
on that clock to accurately sample the accelerometer sensor.

This example provides an alternative `vsync_no_halt()` that is safe to use
without concern for timing limitations. It may result in slightly higher
battery usage since the CPU will not enter the `HALT` state.

# Emulators
At present most emulators do not fully emulate the MBC7 cartridge hardware,
in particular the latching behavior mentioned above. In some cases the range
of outputs data is different. So when testing, if possible, include a MBC7
cartridge as part of the process.

Some example ranges for the sensor data:

```
Slow rotation, measuring mostly the pull
earth's gravity when the sensor not flat.
The range is for a full rotation of 90 degrees from flat.

Note: The ranges below are for regular single-speed
processor mode. In GBC Double-speed mode there appears
to be an offset of around +440 on the X axis and +480
on the Y axis when the cart is flat/centered.

X:  Left          Centered          Right
   0x8230 <------- 0x81BC -------> 0x8148
    +116              0             -116

Y: Forward        Centered        Backward
   0x8253 <------- 0x81DE -------> 0x8170
    +117              0             -110


Higher end ranges for fast horizontal movements

X:  Left          Centered          Right
   0x837E <------- 0x81BC -------> 0x8000
    +450              0             -444

Y: Forward        Centered        Backward
   0x837D <------- 0x81DE -------> 0x8020
    +416              0             -446
```
