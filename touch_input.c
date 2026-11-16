#define _DEFAULT_SOURCE

#include "touch_input.h"

#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Only used when the panel never reports BTN_TOUCH / tracking-id releases.
 * If the device does report them, real release events are used and this
 * timeout is ignored (so a held-still finger is no longer cut off). */
#define TOUCH_RELEASE_TIMEOUT_MS 80

typedef struct {
    int fd;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int x;
    int y;
    lv_indev_state_t state;
    uint32_t last_coordinate_tick;
    bool release_events_supported;
} touch_input_state_t;

static void (*press_callback)(void);

static int scale_coordinate(int value, int input_min, int input_max, int output_max) {
    if (input_max > input_min) {
        value = (int)(((int64_t)(value - input_min) * output_max) / (input_max - input_min));
    }
    if (value < 0) return 0;
    if (value > output_max) return output_max;
    return value;
}

static void touch_input_read(lv_indev_t *indev, lv_indev_data_t *data) {
    touch_input_state_t *state = lv_indev_get_driver_data(indev);
    struct input_event event;
    bool coordinate_seen = false;
    uint32_t now = lv_tick_get();

    while (read(state->fd, &event, sizeof(event)) == (ssize_t)sizeof(event)) {
        if (event.type == EV_ABS) {
            if (event.code == ABS_X || event.code == ABS_MT_POSITION_X) {
                bool was_pressed = state->state == LV_INDEV_STATE_PRESSED;
                state->x = event.value;
                coordinate_seen = true;
                state->state = LV_INDEV_STATE_PRESSED;
                state->last_coordinate_tick = now;
                if (!was_pressed && press_callback) {
                    press_callback();
                }
            } else if (event.code == ABS_Y || event.code == ABS_MT_POSITION_Y) {
                state->y = event.value;
                coordinate_seen = true;
                state->state = LV_INDEV_STATE_PRESSED;
                state->last_coordinate_tick = now;
            } else if (event.code == ABS_MT_TRACKING_ID) {
                state->release_events_supported = true;
                state->state = event.value < 0 ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;
            }
        } else if (event.type == EV_KEY && event.code == BTN_TOUCH) {
            state->release_events_supported = true;
            state->state = event.value ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        }
    }

    /* Fallback for panels that only send coordinates while the finger moves.
     * No logging here: stderr writes can block the UI thread. */
    if (!state->release_events_supported && !coordinate_seen &&
        state->state == LV_INDEV_STATE_PRESSED &&
        now - state->last_coordinate_tick > TOUCH_RELEASE_TIMEOUT_MS) {
        state->state = LV_INDEV_STATE_RELEASED;
    }

    data->state = state->state;
    data->point.x = scale_coordinate(
        state->x, state->min_x, state->max_x,
        lv_display_get_horizontal_resolution(lv_indev_get_display(indev)) - 1);
    data->point.y = scale_coordinate(
        state->y, state->min_y, state->max_y,
        lv_display_get_vertical_resolution(lv_indev_get_display(indev)) - 1);
}

void touch_input_set_press_callback(void (*callback)(void)) {
    press_callback = callback;
}

lv_indev_t *touch_input_create(const char *device_path) {
    touch_input_state_t *state = lv_malloc_zeroed(sizeof(*state));
    struct input_absinfo absinfo;
    lv_indev_t *indev;

    if (!state) return NULL;
    state->fd = open(device_path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (state->fd < 0) {
        fprintf(stderr, "touch device unavailable: %s\n", device_path);
        lv_free(state);
        return NULL;
    }
    fprintf(stderr, "[touch] opened %s\n", device_path);
    if (ioctl(state->fd, EVIOCGABS(ABS_X), &absinfo) == 0) {
        state->min_x = absinfo.minimum;
        state->max_x = absinfo.maximum;
    } else if (ioctl(state->fd, EVIOCGABS(ABS_MT_POSITION_X), &absinfo) == 0) {
        state->min_x = absinfo.minimum;
        state->max_x = absinfo.maximum;
    }
    if (ioctl(state->fd, EVIOCGABS(ABS_Y), &absinfo) == 0) {
        state->min_y = absinfo.minimum;
        state->max_y = absinfo.maximum;
    } else if (ioctl(state->fd, EVIOCGABS(ABS_MT_POSITION_Y), &absinfo) == 0) {
        state->min_y = absinfo.minimum;
        state->max_y = absinfo.maximum;
    }
    fprintf(stderr, "[touch] calibration x=%d..%d y=%d..%d\n",
            state->min_x, state->max_x, state->min_y, state->max_y);
    indev = lv_indev_create();
    if (!indev) {
        close(state->fd);
        lv_free(state);
        return NULL;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_input_read);
    lv_indev_set_driver_data(indev, state);
    return indev;
}
