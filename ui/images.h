#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_router;
extern const lv_img_dsc_t img_thermometer;
extern const lv_img_dsc_t img_horizontal_rule;
extern const lv_img_dsc_t img_wi_fi;
extern const lv_img_dsc_t img_back;
extern const lv_img_dsc_t img_info;
extern const lv_img_dsc_t img_ram;
extern const lv_img_dsc_t img_cpu;
extern const lv_img_dsc_t img_sim;
extern const lv_img_dsc_t img_imei;
extern const lv_img_dsc_t img_me;
extern const lv_img_dsc_t img_wi_fi_2_g;
extern const lv_img_dsc_t img_wi_fi_5_g;
extern const lv_img_dsc_t img_start;
extern const lv_img_dsc_t img_wi_fi_device;
extern const lv_img_dsc_t img_network;
extern const lv_img_dsc_t img_download;
extern const lv_img_dsc_t img_upload;
extern const lv_img_dsc_t img_ping;
extern const lv_img_dsc_t img_speed_test;
extern const lv_img_dsc_t img_start_test;
extern const lv_img_dsc_t img_no_sim;
extern const lv_img_dsc_t img_uptime;
extern const lv_img_dsc_t img_combo;
extern const lv_img_dsc_t img_data_usage;
extern const lv_img_dsc_t img_metrics;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[26];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/