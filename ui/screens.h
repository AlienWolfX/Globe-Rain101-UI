#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SETTINGS = 2,
    SCREEN_ID_WIFI = 3,
    SCREEN_ID_QR_WIFI = 4,
    SCREEN_ID_INFO = 5,
    _SCREEN_ID_LAST = 5
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *settings;
    lv_obj_t *wifi;
    lv_obj_t *qr_wifi;
    lv_obj_t *info;
    lv_obj_t *time_label;
    lv_obj_t *date_label;
    lv_obj_t *bar_1;
    lv_obj_t *bar_2;
    lv_obj_t *bar_3;
    lv_obj_t *bar_4;
    lv_obj_t *bar_5;
    lv_obj_t *telco_label;
    lv_obj_t *network_label;
    lv_obj_t *sinr_arc;
    lv_obj_t *wifi_icon;
    lv_obj_t *settings_label;
    lv_obj_t *back_icon;
    lv_obj_t *info_icon;
    lv_obj_t *wifi_label;
    lv_obj_t *wifi_back_icon;
    lv_obj_t *wifi_2g_icon;
    lv_obj_t *wifi_5g_icon;
    lv_obj_t *connected_device_label;
    lv_obj_t *label_2g;
    lv_obj_t *label_5g;
    lv_obj_t *wifi_qr_scan;
    lv_obj_t *wifi_name_label;
    lv_obj_t *qr_back_icon;
    lv_obj_t *info_label;
    lv_obj_t *info_back_icon;
    lv_obj_t *temperature_icon;
    lv_obj_t *temperature_label;
    lv_obj_t *cpu_icon;
    lv_obj_t *ram_icon;
    lv_obj_t *cpu_load_label;
    lv_obj_t *ram_load_label;
    lv_obj_t *sim_icon;
    lv_obj_t *imei_icon;
    lv_obj_t *phone_number;
    lv_obj_t *imei_label;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_settings();
void tick_screen_settings();

void create_screen_wifi();
void tick_screen_wifi();

void create_screen_qr_wifi();
void tick_screen_qr_wifi();

void create_screen_info();
void tick_screen_info();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/