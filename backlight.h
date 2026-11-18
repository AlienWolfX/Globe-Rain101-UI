#ifndef RAIN101_BACKLIGHT_H
#define RAIN101_BACKLIGHT_H

#include <stdbool.h>
#include <stdint.h>

/* Dims the panel after a period without touches (ROUTERUI_DIM_SECONDS,
 * default 30, 0 = never) using /sys/class/backlight. Does nothing if the
 * system has no backlight device. */
void backlight_init(void);

/* Call periodically with LVGL's inactivity time. */
void backlight_update(uint32_t inactive_ms);

/* Restores full brightness. Returns true if the panel was dimmed. */
bool backlight_wake(void);

/* True shortly after a wake-up, so that the touch that woke the screen is not
 * also treated as a button press. */
bool backlight_recently_woken(uint32_t window_ms);

void backlight_restore(void);

#endif
