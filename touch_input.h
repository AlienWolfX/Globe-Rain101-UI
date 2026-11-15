#ifndef RAIN101_TOUCH_INPUT_H
#define RAIN101_TOUCH_INPUT_H

#include "lvgl.h"

lv_indev_t *touch_input_create(const char *device_path);
void touch_input_set_press_callback(void (*callback)(void));

#endif
