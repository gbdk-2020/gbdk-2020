#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <stdint.h>
#include <gbdk/platform.h>

extern uint16_t camera_x, camera_y;

void InitialCameraDraw();

void UpdateCamera();

#endif