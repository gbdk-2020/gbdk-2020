#ifndef _MBC7_ACCELEROMETER_H
#define _MBC7_ACCELEROMETER_H

extern bool accel_origin_set;
extern uint16_t accel_x;
extern uint16_t accel_y;
extern uint16_t accel_x_origin;
extern uint16_t accel_y_origin;
extern int16_t accel_x_relative;
extern int16_t accel_y_relative;


void mbc7_accel_read(void);
void mbc7_accel_init(void);
void mbc7_accel_setorigin(void);

void mbc7_accel_update(bool do_set_origin);

#endif
