#define _DEFAULT_SOURCE

#include "lvgl.h"
#include "ui.h"
#include "telemetry.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

static uint32_t now_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static uint32_t lvgl_tick_cb(void) { return now_ms(); }

static void telemetry_poll_timer(lv_timer_t *timer) {
    (void)timer;
    telemetry_poll();
}

static void telemetry_ui_timer(lv_timer_t *timer) {
    (void)timer;
    telemetry_refresh_ui();
}

int main(void) {
    lv_init();
    lv_tick_set_cb(lvgl_tick_cb);

    /* Framebuffer */
    lv_display_t *disp = lv_linux_fbdev_create();
    if (!disp) { fprintf(stderr, "fb create fail\n"); return 1; }
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    /* Force full refresh — works around MTK DRM vblank timeout */
    lv_linux_fbdev_set_force_refresh(disp, true);

    /* EEZ Studio UI */
    ui_init();
    telemetry_init();
    telemetry_poll();
    telemetry_refresh_ui();
    lv_timer_create(telemetry_poll_timer, 2000, NULL);
    lv_timer_create(telemetry_ui_timer, 500, NULL);

    /* Main loop */
    while (1) {
        lv_timer_handler();
        usleep(2000); 
    }
}