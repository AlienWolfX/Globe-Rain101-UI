#define _DEFAULT_SOURCE

#include "lvgl.h"
#include "ui.h"
#include "telemetry.h"
#include "touch_input.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOUCH_DEVICE "/dev/input/event2"

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

static void open_settings_screen(void) {
    if (lv_scr_act() != objects.main) {
        return;
    }
    fprintf(stderr, "[touch] press detected; opening settings screen\n");
    loadScreen(SCREEN_ID_SETTINGS);
}

static void open_settings_page(lv_event_t *event) {
    (void)event;
    loadScreen(SCREEN_ID_SETTINGS);
}

static void open_wifi_screen(lv_event_t *event) {
    (void)event;
    loadScreen(SCREEN_ID_WIFI);
}

static void open_info_screen(lv_event_t *event) {
    (void)event;
    loadScreen(SCREEN_ID_INFO);
}

static bool read_command_value(const char *command, char *value, size_t value_size) {
    FILE *pipe = popen(command, "r");
    bool found = false;

    if (!pipe) return false;
    if (fgets(value, (int)value_size, pipe)) {
        value[strcspn(value, "\r\n")] = '\0';
        found = value[0] != '\0';
    }
    pclose(pipe);
    return found;
}

static void open_wifi_qr(int interface_index) {
    char command[128];
    char ssid[96] = "";
    char key[96] = "";
    char qr_data[220];

    snprintf(command, sizeof(command),
             "uci -q get wireless.@wifi-iface[%d].ssid", interface_index);
    (void)read_command_value(command, ssid, sizeof(ssid));
    snprintf(command, sizeof(command),
             "uci -q get wireless.@wifi-iface[%d].key", interface_index);
    (void)read_command_value(command, key, sizeof(key));
    if (ssid[0] == '\0') {
        snprintf(ssid, sizeof(ssid), "WiFi unavailable");
    }
    if (key[0] == '\0') {
        snprintf(qr_data, sizeof(qr_data), "WIFI:T:nopass;S:%s;P:;;", ssid);
    } else {
        snprintf(qr_data, sizeof(qr_data), "WIFI:T:WPA;S:%s;P:%s;;", ssid, key);
    }
    lv_label_set_text(objects.wifi_name_label, ssid);
    lv_qrcode_update(objects.wifi_qr_scan, qr_data, strlen(qr_data));
    loadScreen(SCREEN_ID_QR_WIFI);
}

static void open_wifi_24_qr(lv_event_t *event) {
    (void)event;
    open_wifi_qr(0);
}

static void open_wifi_5_qr(lv_event_t *event) {
    (void)event;
    open_wifi_qr(1);
}

static void open_main_screen(lv_event_t *event) {
    (void)event;
    loadScreen(SCREEN_ID_MAIN);
}

static void install_screen_navigation(void) {
    lv_obj_add_flag(objects.wifi_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(objects.info_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(objects.back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(objects.wifi_back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(objects.qr_back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(objects.info_back_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(objects.wifi_icon, open_wifi_screen, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.info_icon, open_info_screen, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.wifi_2ghz, open_wifi_24_qr, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.wifi_5ghz, open_wifi_5_qr, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.back_icon, open_main_screen, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.wifi_back_icon, open_settings_page, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.qr_back_icon, open_wifi_screen, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(objects.info_back_icon, open_settings_page, LV_EVENT_PRESSED, NULL);
    lv_obj_add_flag(objects.information_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(objects.info_label, 7, 10);
    lv_obj_set_size(objects.info_label, 190, 160);
    lv_label_set_long_mode(objects.info_label, LV_LABEL_LONG_WRAP);
}

int main(void) {
    lv_init();
    lv_tick_set_cb(lvgl_tick_cb);

    /* Framebuffer */
    lv_display_t *disp = lv_linux_fbdev_create();
    if (!disp) { fprintf(stderr, "fb create fail\n"); return 1; }
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
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

    /* Force full refresh — works around MTK DRM vblank timeout */
    lv_linux_fbdev_set_force_refresh(disp, true);

    /* EEZ Studio UI */
    ui_init();
    install_screen_navigation();
    touch_input_set_press_callback(open_settings_screen);
    telemetry_init();
    telemetry_poll();
    telemetry_refresh_ui();
    lv_timer_create(telemetry_poll_timer, 1000, NULL);
    lv_timer_create(telemetry_ui_timer, 200, NULL);

    /* Main loop */
    while (1) {
        lv_timer_handler();
        usleep(2000); 
    }
}