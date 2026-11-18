#ifndef RAIN101_TELEMETRY_INTERNAL_H
#define RAIN101_TELEMETRY_INTERNAL_H

#include <stdbool.h>

/* Everything the worker collects and the UI displays. Speed test state lives
 * in speedtest.c with its own lock. */
typedef struct {
    int signal_bars;
    int sinr_db;
    bool has_sinr;
    int connected_devices;
    char carrier[32];
    char reg_text[16];          /* "Searching", "Denied"... empty when registered */
    char radio[16];
    char temperature_text[16];
    char cpu_load_text[16];
    char memory_load_text[16];
    char imei_text[32];
    char cnum_text[32];
    char combo_band_text[32];
    char data_usage_text[24];
    char uptime_text[24];
    char board_model_text[64];
    char board_name_text[64];
    char board_system_text[64];
    char board_version_text[32];
    char board_description_text[96];
    char dev_model_text[64];
    char dev_board_sn_text[64];
    char dev_mac_wan_text[32];
    char dev_hardware_text[32];
    char dev_software_text[32];
    char dev_nickname_text[64];
    bool has_sim;
    bool sim_status_known;
} telemetry_status_t;

/* telemetry.c */
void telemetry_get_status(telemetry_status_t *out);

/* ui_update.c (UI thread; needs the LVGL objects to exist) */
void ui_update_init(void);

#endif
