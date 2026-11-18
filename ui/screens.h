#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_BLANK = 1,
    SCREEN_ID_MAIN = 2,
    SCREEN_ID_NO_SIM = 3,
    SCREEN_ID_INFO = 4,
    SCREEN_ID_SETTINGS = 5,
    SCREEN_ID_WIFI = 6,
    SCREEN_ID_QR_WIFI = 7,
    SCREEN_ID_METRIC = 8,
    SCREEN_ID_SPEED_TEST = 9,
    _SCREEN_ID_LAST = 9
};

typedef struct _objects_t {
    lv_obj_t *blank;
    lv_obj_t *main;
    lv_obj_t *no_sim;
    lv_obj_t *info;
    lv_obj_t *settings;
    lv_obj_t *wifi;
    lv_obj_t *qr_wifi;
    lv_obj_t *metric;
    lv_obj_t *speed_test;
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
    lv_obj_t *no_sim_icon;
    lv_obj_t *no_sim_label;
    lv_obj_t *no_sim_info_description;
    lv_obj_t *info_label;
    lv_obj_t *info_back_icon;
    lv_obj_t *model_name;
    lv_obj_t *sn_name;
    lv_obj_t *model_label;
    lv_obj_t *sn_label;
    lv_obj_t *sv_name;
    lv_obj_t *sv_label;
    lv_obj_t *mac_name;
    lv_obj_t *mac_label;
    lv_obj_t *version_name;
    lv_obj_t *version_label;
    lv_obj_t *system_name;
    lv_obj_t *system_label;
    lv_obj_t *cp_no_name;
    lv_obj_t *cp_num_label;
    lv_obj_t *imei_name;
    lv_obj_t *imei_label;
    lv_obj_t *wifi_icon;
    lv_obj_t *settings_label;
    lv_obj_t *back_icon;
    lv_obj_t *info_icon;
    lv_obj_t *speed_test_icon;
    lv_obj_t *metrics_icon;
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
    lv_obj_t *metrics_label;
    lv_obj_t *metrics_back_icon;
    lv_obj_t *temperature_icon;
    lv_obj_t *temperature_label;
    lv_obj_t *cpu_icon;
    lv_obj_t *ram_icon;
    lv_obj_t *cpu_load_label;
    lv_obj_t *ram_load_label;
    lv_obj_t *uptime_icon;
    lv_obj_t *uptime_label;
    lv_obj_t *combo_icon;
    lv_obj_t *data_usage_icon;
    lv_obj_t *data_usage_label;
    lv_obj_t *combo_label;
    lv_obj_t *download_icon;
    lv_obj_t *upload_icon;
    lv_obj_t *ping_icon;
    lv_obj_t *speed_test_label;
    lv_obj_t *download_label;
    lv_obj_t *ping_label;
    lv_obj_t *upload_label;
    lv_obj_t *speed_test_back_icon;
    lv_obj_t *start_test_icon;
    lv_obj_t *status_label;
} objects_t;

extern objects_t objects;

void create_screen_blank();
void tick_screen_blank();

void create_screen_main();
void tick_screen_main();

void create_screen_no_sim();
void tick_screen_no_sim();

void create_screen_info();
void tick_screen_info();

void create_screen_settings();
void tick_screen_settings();

void create_screen_wifi();
void tick_screen_wifi();

void create_screen_qr_wifi();
void tick_screen_qr_wifi();

void create_screen_metric();
void tick_screen_metric();

void create_screen_speed_test();
void tick_screen_speed_test();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/