#define _DEFAULT_SOURCE

#include "lvgl.h"
#include "ui.h"
#include "telemetry.h"
#include "touch_input.h"
#include "fb_display.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <time.h>

#define TOUCH_DEVICE "/dev/input/event2"
#define INSTANCE_LOCK_FILE "/tmp/routerui.lock"

/* The worker polls once per second and the clock changes once per minute, so
 * refreshing the labels twice a second is plenty. */
#define UI_REFRESH_MS 500

#define WIFI_2G_FILE "/etc/wireless/mediatek/mt7915.1.dat"
#define WIFI_5G_FILE "/etc/wireless/mediatek/mt7915.2.dat"

static int instance_lock_fd = -1;

static bool acquire_instance_lock(void) {
    instance_lock_fd = open(INSTANCE_LOCK_FILE, O_RDWR | O_CREAT, 0644);
    if (instance_lock_fd < 0 || flock(instance_lock_fd, LOCK_EX | LOCK_NB) != 0) {
        if (instance_lock_fd >= 0) {
            close(instance_lock_fd);
            instance_lock_fd = -1;
        }
        fprintf(stderr, "[main] another routerui instance is already running\n");
        return false;
    }
    return true;
}

static uint32_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static uint32_t lvgl_tick_cb(void) { return now_ms(); }

static void telemetry_ui_timer(lv_timer_t *timer) {
    (void)timer;
    telemetry_refresh_ui();
}

/* ---- navigation ---------------------------------------------------------- */

/* Screen switches are instant (loadScreen uses LV_SCR_LOAD_ANIM_NONE), so a
 * single touch that is still held down could land on a button of the screen
 * that just appeared and navigate twice. Ignore further navigation for a short
 * time after each switch so one tap means one navigation.
 * Returns false if the request was ignored. */
#define NAV_LOCKOUT_MS 200

static bool go_to_screen(enum ScreensEnum screen_id) {
    static uint32_t last_nav_ms;
    static bool have_navigated;
    uint32_t now = now_ms();

    if (have_navigated && (uint32_t)(now - last_nav_ms) < NAV_LOCKOUT_MS) {
        return false;
    }
    last_nav_ms = now;
    have_navigated = true;
    loadScreen(screen_id);
    return true;
}

static void open_settings_screen(void) {
    if (lv_scr_act() != objects.main) {
        return;
    }
    go_to_screen(SCREEN_ID_SETTINGS);
}

/* One handler for every plain "go to screen X" button; X travels in user_data. */
static void nav_cb(lv_event_t *event) {
    go_to_screen((enum ScreensEnum)(intptr_t)lv_event_get_user_data(event));
}

/* ---- wifi QR (cached) ---------------------------------------------------- */

typedef struct {
    const char *path;
    char ssid[96];
    char qr_data[420];
    time_t mtime;       /* mtime of the config file when the cache was built */
    bool valid;
} wifi_cache_t;

static wifi_cache_t wifi_cache[2] = {
    { .path = WIFI_2G_FILE },
    { .path = WIFI_5G_FILE },
};
static int qr_shown_band = -1;      /* which band the QR widget currently holds */
static bool qr_needs_update = true;

static void qr_escape(const char *source, char *destination, size_t destination_size) {
    size_t used = 0;

    while (*source && used + 1 < destination_size) {
        if ((*source == '\\' || *source == ';' || *source == ',' || *source == ':') &&
            used + 2 < destination_size) {
            destination[used++] = '\\';
        }
        destination[used++] = *source++;
    }
    destination[used] = '\0';
}

static bool read_config_value(const char *path, const char *name,
                              char *value, size_t value_size) {
    char line[256];
    size_t name_length = strlen(name);
    FILE *file = fopen(path, "r");

    if (!file) {
        return false;
    }
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, name, name_length) == 0 && line[name_length] == '=') {
            snprintf(value, value_size, "%s", line + name_length + 1);
            value[strcspn(value, "\r\n")] = '\0';
            fclose(file);
            return value[0] != '\0';
        }
    }
    fclose(file);
    return false;
}

/* Rebuilds the cached SSID/QR string only if the config file changed.
 * Returns true if the cache was rebuilt. Cost when unchanged: one stat(). */
static bool refresh_wifi_cache(wifi_cache_t *cache) {
    struct stat st;
    char key[96] = "";
    char escaped_ssid[192];
    char escaped_key[192];
    time_t mtime = (stat(cache->path, &st) == 0) ? st.st_mtime : 0;

    if (cache->valid && cache->mtime == mtime) {
        return false;
    }
    cache->ssid[0] = '\0';
    if (read_config_value(cache->path, "SSID1", cache->ssid, sizeof(cache->ssid))) {
        (void)read_config_value(cache->path, "WPAPSK1", key, sizeof(key));
    }
    if (cache->ssid[0] == '\0') {
        snprintf(cache->ssid, sizeof(cache->ssid), "WiFi unavailable");
    }
    qr_escape(cache->ssid, escaped_ssid, sizeof(escaped_ssid));
    qr_escape(key, escaped_key, sizeof(escaped_key));
    if (key[0] == '\0') {
        snprintf(cache->qr_data, sizeof(cache->qr_data), "WIFI:T:nopass;S:%s;P:;;", escaped_ssid);
    } else {
        snprintf(cache->qr_data, sizeof(cache->qr_data), "WIFI:T:WPA;S:%s;P:%s;;",
                 escaped_ssid, escaped_key);
    }
    memset(key, 0, sizeof(key));
    cache->mtime = mtime;
    cache->valid = true;
    return true;
}

static void open_wifi_qr(int band) {
    wifi_cache_t *cache = &wifi_cache[band];

    if (refresh_wifi_cache(cache)) {
        qr_needs_update = true;
    }
    /* Re-encoding the QR is the expensive part: skip it when this band's code
     * is already in the widget. */
    if (qr_needs_update || qr_shown_band != band) {
        lv_label_set_text(objects.wifi_name_label, cache->ssid);
        lv_qrcode_update(objects.wifi_qr_scan, cache->qr_data, strlen(cache->qr_data));
        qr_shown_band = band;
        qr_needs_update = false;
    }
    go_to_screen(SCREEN_ID_QR_WIFI);
}

/* user_data is the band: 0 = 2.4 GHz, 1 = 5 GHz. */
static void wifi_qr_cb(lv_event_t *event) {
    open_wifi_qr((int)(intptr_t)lv_event_get_user_data(event));
}

static void install_screen_navigation(void) {
    /* Each icon also gets an invisible extra hit area so a slightly-off finger
     * still hits: the icons are drawn at 1.37x (image scale 350) but their
     * object box stays at the source image size. */
    const struct {
        lv_obj_t *obj;
        enum ScreensEnum target;
    } nav[] = {
        { objects.wifi_icon,      SCREEN_ID_WIFI },
        { objects.info_icon,      SCREEN_ID_INFO },
        { objects.back_icon,      SCREEN_ID_MAIN },
        { objects.wifi_back_icon, SCREEN_ID_SETTINGS },
        { objects.qr_back_icon,   SCREEN_ID_WIFI },
        { objects.info_back_icon, SCREEN_ID_SETTINGS },
    };
    const struct {
        lv_obj_t *obj;
        int band;
    } qr[] = {
        { objects.wifi_2g_icon, 0 },
        { objects.wifi_5g_icon, 1 },
    };

    for (size_t i = 0; i < sizeof(nav) / sizeof(nav[0]); ++i) {
        lv_obj_add_flag(nav[i].obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(nav[i].obj, 16);
        lv_obj_add_event_cb(nav[i].obj, nav_cb, LV_EVENT_PRESSED,
                            (void *)(intptr_t)nav[i].target);
    }
    for (size_t i = 0; i < sizeof(qr) / sizeof(qr[0]); ++i) {
        lv_obj_add_flag(qr[i].obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(qr[i].obj, 12);
        lv_obj_add_event_cb(qr[i].obj, wifi_qr_cb, LV_EVENT_PRESSED,
                            (void *)(intptr_t)qr[i].band);
    }
    /* The SINR arc covers the whole main screen and is clickable by default,
     * so a touch could drag its knob. It is display-only. */
    lv_obj_remove_flag(objects.sinr_arc, LV_OBJ_FLAG_CLICKABLE);
}

int main(void) {
    if (!acquire_instance_lock()) {
        return 1;
    }

    /* Slightly higher priority than the modem/wifi daemons on a busy CPE.
     * Silently ignored if we are not allowed to. */
    (void)setpriority(PRIO_PROCESS, 0, -5);

    lv_init();
    lv_tick_set_cb(lvgl_tick_cb);

    /* Framebuffer */
    lv_display_t *disp = fb_display_create("/dev/fb0");
    if (!disp) { fprintf(stderr, "fb create fail\n"); return 1; }
    lv_indev_t *touch = touch_input_create(TOUCH_DEVICE);
    if (!touch) {
        fprintf(stderr, "touch device unavailable: %s\n", TOUCH_DEVICE);
    } else {
        lv_timer_t *touch_timer;
        lv_indev_set_display(touch, disp);
        touch_timer = lv_indev_get_read_timer(touch);
        if (touch_timer) {
            lv_timer_set_period(touch_timer, 5);
        }
    }

    /* No force-refresh: fb_display.c commits once per frame with
     * FBIOPAN_DISPLAY (~67 ms, one panel refresh) instead of the stock
     * FBIOPUT_VSCREENINFO path (3 commits, ~290 ms, incl. a vblank timeout).
     * The panel itself refreshes at ~15 Hz, so that is the ceiling. */

    /* EEZ Studio UI */
    ui_init();
    install_screen_navigation();
    touch_input_set_press_callback(open_settings_screen);
    telemetry_init();
    telemetry_refresh_ui();
    telemetry_start();
    lv_timer_create(telemetry_ui_timer, UI_REFRESH_MS, NULL);

    /* Warm both QR caches in the background of startup so the first tap is
     * a stat() instead of two file reads + QR generation. */
    (void)refresh_wifi_cache(&wifi_cache[0]);
    (void)refresh_wifi_cache(&wifi_cache[1]);

    /* Main loop */
    while (1) {
        uint32_t next = lv_timer_handler();
        /* Sleep as long as LVGL says it can (clamped), instead of a fixed 2 ms. */
        if (next < 2) next = 2;
        if (next > 10) next = 10;       /* also covers LV_NO_TIMER_READY */
        usleep(next * 1000);
    }
}
