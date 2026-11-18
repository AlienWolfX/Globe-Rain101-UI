#ifndef RAIN101_TOUCH_INPUT_H
#define RAIN101_TOUCH_INPUT_H

#include <stdbool.h>
#include <stddef.h>

#include "lvgl.h"

/* Looks in /proc/bus/input/devices for the touch panel by name, so the code
 * does not depend on the /dev/input/eventN number staying the same. */
bool touch_input_find_device(char *path, size_t size);

lv_indev_t *touch_input_create(const char *device_path);
void touch_input_set_press_callback(void (*callback)(void));

/* File descriptor of the opened device, for poll(); -1 if none. */
int touch_input_get_fd(lv_indev_t *indev);

#endif
