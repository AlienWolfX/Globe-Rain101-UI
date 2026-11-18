#define _DEFAULT_SOURCE

#include "touch_input.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
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
    int slot;                       /* current multitouch slot; only slot 0 is used */
    lv_indev_state_t state;
    uint32_t last_coordinate_tick;
    bool release_events_supported;
    bool press_edge;                /* released -> pressed seen since last read */
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

static void set_state(touch_input_state_t *state, lv_indev_state_t new_state) {
    if (new_state == LV_INDEV_STATE_PRESSED && state->state == LV_INDEV_STATE_RELEASED) {
        state->press_edge = true;
    }
    state->state = new_state;
}

static void touch_input_read(lv_indev_t *indev, lv_indev_data_t *data) {
    touch_input_state_t *state = lv_indev_get_driver_data(indev);
    lv_display_t *disp = lv_indev_get_display(indev);
    struct input_event event;
    bool coordinate_seen = false;
    uint32_t now = lv_tick_get();

    if (!disp) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    while (read(state->fd, &event, sizeof(event)) == (ssize_t)sizeof(event)) {
        if (event.type == EV_ABS) {
            if (event.code == ABS_MT_SLOT) {
                state->slot = event.value;
                continue;
            }
            /* Ignore extra fingers: only slot 0 drives the pointer. */
            if (event.code >= ABS_MT_SLOT && state->slot != 0) {
                continue;
            }
            if (event.code == ABS_X || event.code == ABS_MT_POSITION_X) {
                state->x = event.value;
                coordinate_seen = true;
                state->last_coordinate_tick = now;
            } else if (event.code == ABS_Y || event.code == ABS_MT_POSITION_Y) {
                state->y = event.value;
                coordinate_seen = true;
                state->last_coordinate_tick = now;
            } else if (event.code == ABS_MT_TRACKING_ID) {
                state->release_events_supported = true;
                set_state(state, event.value < 0 ? LV_INDEV_STATE_RELEASED
                                                 : LV_INDEV_STATE_PRESSED);
            }
            /* Without explicit release events, coordinates mean "touching". */
            if (coordinate_seen && !state->release_events_supported) {
                set_state(state, LV_INDEV_STATE_PRESSED);
            }
        } else if (event.type == EV_KEY && event.code == BTN_TOUCH) {
            state->release_events_supported = true;
            set_state(state, event.value ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED);
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
    data->point.x = scale_coordinate(state->x, state->min_x, state->max_x,
                                     lv_display_get_horizontal_resolution(disp) - 1);
    data->point.y = scale_coordinate(state->y, state->min_y, state->max_y,
                                     lv_display_get_vertical_resolution(disp) - 1);

    /* Fires once per touch, whichever event (BTN_TOUCH, tracking id or first
     * coordinate) started it. */
    if (state->press_edge) {
        state->press_edge = false;
        if (press_callback) {
            press_callback();
        }
    }
}

void touch_input_set_press_callback(void (*callback)(void)) {
    press_callback = callback;
}

int touch_input_get_fd(lv_indev_t *indev) {
    touch_input_state_t *state = indev ? lv_indev_get_driver_data(indev) : NULL;
    return state ? state->fd : -1;
}

static bool name_looks_like_touch(const char *name) {
    static const char *const patterns[] = { "touch", "ctp", "goodix", "ft5", "cst", "gt9" };
    char lower[96];
    size_t i;

    for (i = 0; name[i] && i + 1 < sizeof(lower); ++i) {
        lower[i] = (char)tolower((unsigned char)name[i]);
    }
    lower[i] = '\0';
    for (size_t p = 0; p < sizeof(patterns) / sizeof(patterns[0]); ++p) {
        if (strstr(lower, patterns[p])) {
            return true;
        }
    }
    return false;
}

bool touch_input_find_device(char *path, size_t size) {
    FILE *file = fopen("/proc/bus/input/devices", "r");
    char line[256];
    char name[96] = "";
    char event[16] = "";
    bool has_abs = false;
    bool found = false;

    if (!file) {
        return false;
    }
    /* Blocks of "N: Name=", "H: Handlers=... eventN", "B: ABS=..." separated
     * by blank lines (a final block may end at EOF). */
    for (;;) {
        bool have_line = fgets(line, sizeof(line), file) != NULL;

        if (!have_line || line[0] == '\n') {
            if (event[0] && has_abs && name_looks_like_touch(name)) {
                snprintf(path, size, "/dev/input/%s", event);
                found = true;
                break;
            }
            name[0] = event[0] = '\0';
            has_abs = false;
            if (!have_line) break;
        } else if (strncmp(line, "N: Name=", 8) == 0) {
            snprintf(name, sizeof(name), "%s", line + 8);
        } else if (strncmp(line, "H: Handlers=", 12) == 0) {
            const char *p = strstr(line, "event");
            if (p) {
                (void)sscanf(p, "%15[a-z0-9]", event);
            }
        } else if (strncmp(line, "B: ABS=", 7) == 0 && line[7] != '0') {
            has_abs = true;
        }
    }
    fclose(file);
    return found;
}

lv_indev_t *touch_input_create(const char *device_path) {
    touch_input_state_t *state = lv_malloc_zeroed(sizeof(*state));
    struct input_absinfo absinfo;
    lv_indev_t *indev;

    if (!state) return NULL;
    state->fd = open(device_path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (state->fd < 0) {
        fprintf(stderr, "[touch] cannot open %s\n", device_path);
        lv_free(state);
        return NULL;
    }
    if (ioctl(state->fd, EVIOCGABS(ABS_X), &absinfo) == 0 ||
        ioctl(state->fd, EVIOCGABS(ABS_MT_POSITION_X), &absinfo) == 0) {
        state->min_x = absinfo.minimum;
        state->max_x = absinfo.maximum;
    }
    if (ioctl(state->fd, EVIOCGABS(ABS_Y), &absinfo) == 0 ||
        ioctl(state->fd, EVIOCGABS(ABS_MT_POSITION_Y), &absinfo) == 0) {
        state->min_y = absinfo.minimum;
        state->max_y = absinfo.maximum;
    }
    fprintf(stderr, "[touch] opened %s, calibration x=%d..%d y=%d..%d\n", device_path,
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
