#define _DEFAULT_SOURCE

#include "lvgl.h"
#include "ui.h"
#include "backlight.h"
#include "telemetry.h"
#include "touch_input.h"
#include "fb_display.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <string.h>
#include <time.h>

/* Used when the panel cannot be found by name and ROUTERUI_TOUCH is not set. */
#define DEFAULT_TOUCH_DEVICE "/dev/input/event2"
#define INSTANCE_LOCK_FILE "/tmp/routerui.lock"

/* Labels refresh fast until the first data arrives, then twice a second (the
 * worker polls once per second, the clock changes once per minute). */
#define UI_REFRESH_STARTUP_MS 50
#define UI_REFRESH_MS 500
#define UI_STARTUP_GIVE_UP_MS 5000

/* The touch fd wakes the main loop; this timer is only the fallback that
 * notices a release on panels that do not report one. */
#define TOUCH_READ_PERIOD_MS 30

#define MAX_SLEEP_MS 50

#define WIFI_2G_FILE "/etc/wireless/mediatek/mt7915.1.dat"
#define WIFI_5G_FILE "/etc/wireless/mediatek/mt7915.2.dat"

static int instance_lock_fd = -1;
static volatile sig_atomic_t quit_requested;

static void on_signal(int signal_number) {
    (void)signal_number;
    quit_requested = 1;
}

static void install_signal_handlers(void) {
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = on_signal;      /* no SA_RESTART: interrupts poll() */
    sigemptyset(&action.sa_mask);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    signal(SIGPIPE, SIG_IGN);
}

static bool acquire_instance_lock(void) {
    /* O_CLOEXEC: spawned children (ubus, speed test) must not inherit the
     * lock, or a lingering child would block the next start. */
    instance_lock_fd = open(INSTANCE_LOCK_FILE, O_RDWR | O_CREAT | O_CLOEXEC, 0644);
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
    static bool slowed_down;
    static uint32_t started_ms;

    if (started_ms == 0) {
        started_ms = now_ms();
    }
    telemetry_refresh_ui();
    backlight_update(lv_display_get_inactive_time(NULL));

    if (!slowed_down && (telemetry_first_data_ready() ||
                         now_ms() - started_ms > UI_STARTUP_GIVE_UP_MS)) {
        lv_timer_set_period(timer, UI_REFRESH_MS);
        slowed_down = true;
    }
}

/* ---- navigation ---------------------------------------------------------- */

/* A single touch that is still held down could land on a button of the screen
 * that just appeared (the new screen fades in over ~200 ms) and navigate
 * twice. Ignore further navigation for a short time after each switch so one
 * tap means one navigation. The same window swallows the touch that woke a
 * dimmed screen. */
#define NAV_LOCKOUT_MS 200

static void go_to_screen(enum ScreensEnum screen_id) {
    static uint32_t last_nav_ms;
    static bool have_navigated;
    uint32_t now = now_ms();

    if (backlight_recently_woken(NAV_LOCKOUT_MS)) {
        return;
    }
    if (have_navigated && (uint32_t)(now - last_nav_ms) < NAV_LOCKOUT_MS) {
        return;
    }
    last_nav_ms = now;
    have_navigated = true;
    loadScreen(screen_id);
    telemetry_set_screen((int)screen_id);
}

/* Called on every new touch, before LVGL sees it. */
static void on_touch_press(void) {
    if (backlight_wake()) {
        return;                         /* this touch only wakes the screen */
    }
    if (lv_screen_active() == objects.main) {
        go_to_screen(SCREEN_ID_SETTINGS);
    }
}

/* One handler for every plain "go to screen X" button; X travels in user_data. */
static void nav_cb(lv_event_t *event) {
    go_to_screen((enum ScreensEnum)(intptr_t)lv_event_get_user_data(event));
}

static void start_speed_test_cb(lv_event_t *event) {
    (void)event;
    if (backlight_recently_woken(NAV_LOCKOUT_MS)) {
        return;
    }
    telemetry_start_speed_test();
}

/* ---- wifi QR (cached) ---------------------------------------------------- */

typedef struct {
    const char *path;
    char ssid[96];
    char qr_data[420];
    time_t mtime;           /* mtime of the config file when the cache was built */
    unsigned generation;    /* bumped every time the cache is rebuilt */
    bool valid;
} wifi_cache_t;

static wifi_cache_t wifi_cache[2] = {
    { .path = WIFI_2G_FILE },
    { .path = WIFI_5G_FILE },
};
static int qr_shown_band = -1;          /* which band the QR widget currently holds */
static unsigned qr_shown_generation;    /* cache generation encoded in the widget */

/* Escapes the characters that are special in the WIFI: QR format. */
static void qr_escape(const char *source, char *destination, size_t destination_size) {
    size_t used = 0;

    while (*source && used + 1 < destination_size) {
        if ((*source == '\\' || *source == ';' || *source == ',' ||
             *source == ':' || *source == '"') &&
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
 * Cost when unchanged: one stat(). */
static void refresh_wifi_cache(wifi_cache_t *cache) {
    struct stat st;
    char key[96] = "";
    char escaped_ssid[192];
    char escaped_key[192];
    time_t mtime = (stat(cache->path, &st) == 0) ? st.st_mtime : 0;

    if (cache->valid && cache->mtime == mtime) {
        return;
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
    memset(escaped_key, 0, sizeof(escaped_key));
    cache->mtime = mtime;
    cache->valid = true;
    ++cache->generation;
}

static void open_wifi_qr(int band) {
    wifi_cache_t *cache = &wifi_cache[band];

    if (backlight_recently_woken(NAV_LOCKOUT_MS)) {
        return;
    }
    refresh_wifi_cache(cache);
    /* Re-encoding the QR is the expensive part: skip it when this band's
     * current code is already in the widget. */
    if (qr_shown_band != band || qr_shown_generation != cache->generation) {
        lv_label_set_text(objects.wifi_name_label, cache->ssid);
        lv_qrcode_update(objects.wifi_qr_scan, cache->qr_data, strlen(cache->qr_data));
        qr_shown_band = band;
        qr_shown_generation = cache->generation;
    }
    go_to_screen(SCREEN_ID_QR_WIFI);
}

/* user_data is the band: 0 = 2.4 GHz, 1 = 5 GHz. */
static void wifi_qr_cb(lv_event_t *event) {
    open_wifi_qr((int)(intptr_t)lv_event_get_user_data(event));
}

static void make_pressable(lv_obj_t *obj, int32_t extra_area, lv_event_cb_t cb, void *user_data) {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(obj, extra_area);
    lv_obj_add_event_cb(obj, cb, LV_EVENT_PRESSED, user_data);
}

static void install_screen_navigation(void) {
    const struct {
        lv_obj_t *obj;
        enum ScreensEnum target;
    } nav[] = {
        { objects.wifi_icon,            SCREEN_ID_WIFI },
        { objects.info_icon,            SCREEN_ID_INFO },
        { objects.back_icon,            SCREEN_ID_MAIN },
        { objects.wifi_back_icon,       SCREEN_ID_SETTINGS },
        { objects.qr_back_icon,         SCREEN_ID_WIFI },
        { objects.info_back_icon,       SCREEN_ID_SETTINGS },
        { objects.speed_test_icon,      SCREEN_ID_SPEED_TEST },
        { objects.speed_test_back_icon, SCREEN_ID_SETTINGS },
        { objects.metrics_icon,         SCREEN_ID_METRIC },
        { objects.metrics_back_icon,    SCREEN_ID_SETTINGS },
    };
    const struct {
        lv_obj_t *obj;
        int band;
    } qr[] = {
        { objects.wifi_2g_icon, 0 },
        { objects.wifi_5g_icon, 1 },
    };

    for (size_t i = 0; i < sizeof(nav) / sizeof(nav[0]); ++i) {
        make_pressable(nav[i].obj, 16, nav_cb, (void *)(intptr_t)nav[i].target);
    }
    for (size_t i = 0; i < sizeof(qr) / sizeof(qr[0]); ++i) {
        make_pressable(qr[i].obj, 12, wifi_qr_cb, (void *)(intptr_t)qr[i].band);
    }
    make_pressable(objects.start_test_icon, 12, start_speed_test_cb, NULL);
}

int main(void) {
    lv_timer_t *touch_timer = NULL;
    struct pollfd touch_poll = { .fd = -1, .events = POLLIN, .revents = 0 };

    if (!acquire_instance_lock()) {
        return 1;
    }
    install_signal_handlers();

    (void)setpriority(PRIO_PROCESS, 0, -5);

    lv_init();
    lv_tick_set_cb(lvgl_tick_cb);

    /* Framebuffer */
    lv_display_t *disp = fb_display_create("/dev/fb0");
    if (!disp) {
        fprintf(stderr, "fb create fail\n");
        return 1;
    }
    backlight_init();

    /* Touch (optional: the UI still runs without it; the driver logs why).
     * Device: ROUTERUI_TOUCH, else found by name, else the default path. */
    {
        char found_path[64];
        const char *touch_path = getenv("ROUTERUI_TOUCH");
        lv_indev_t *touch;

        if (!touch_path || touch_path[0] == '\0') {
            touch_path = touch_input_find_device(found_path, sizeof(found_path))
                             ? found_path
                             : DEFAULT_TOUCH_DEVICE;
        }
        touch = touch_input_create(touch_path);
        if (touch) {
            lv_indev_set_display(touch, disp);
            touch_timer = lv_indev_get_read_timer(touch);
            if (touch_timer) {
                lv_timer_set_period(touch_timer, TOUCH_READ_PERIOD_MS);
            }
            touch_poll.fd = touch_input_get_fd(touch);
        }
    }

    /* EEZ Studio UI */
    ui_init();
    install_screen_navigation();
    touch_input_set_press_callback(on_touch_press);
    telemetry_init();
    {
        bool has_sim = telemetry_has_sim();
        /* No fade on the very first screen: show it as soon as it is ready. */
        lv_screen_load(has_sim ? objects.main : objects.no_sim);
        telemetry_set_screen(has_sim ? SCREEN_ID_MAIN : SCREEN_ID_NO_SIM);
    }
    telemetry_refresh_ui();
    telemetry_start();
    lv_timer_create(telemetry_ui_timer, UI_REFRESH_STARTUP_MS, NULL);

    /* Warm both QR caches now so the first tap costs a stat() instead of two
     * file reads. */
    refresh_wifi_cache(&wifi_cache[0]);
    refresh_wifi_cache(&wifi_cache[1]);

    /* Main loop: sleep until LVGL's next timer is due or a touch event
     * arrives, whichever is first. */
    while (!quit_requested) {
        uint32_t next = lv_timer_handler();
        if (next < 1) next = 1;
        if (next > MAX_SLEEP_MS) next = MAX_SLEEP_MS;   /* also covers LV_NO_TIMER_READY */

        /* A negative fd is ignored by poll(), so this is a plain sleep when
         * there is no touch device. */
        if (poll(&touch_poll, 1, (int)next) > 0) {
            if (touch_poll.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                touch_poll.fd = -1;                     /* device gone: stop polling it */
            } else if ((touch_poll.revents & POLLIN) && touch_timer) {
                lv_timer_ready(touch_timer);            /* read the events right away */
            }
        }
    }

    backlight_restore();
    return 0;
}
