#ifndef RAIN101_TELEMETRY_H
#define RAIN101_TELEMETRY_H

#include <stdbool.h>
#include <stdint.h>

#include "lvgl.h"

typedef struct {
    int rsrp_dbm;
    int signal_bars;
    float rx_mbps;
    float tx_mbps;
    char carrier[32];
    char radio[16];
    char interface_name[16];
    char time_text[16];
    char date_text[32];
    char state[16];
    uint32_t last_poll_tick;
    uint32_t last_ui_tick;
    bool has_rsrp;
} telemetry_status_t;

void telemetry_init(void);
void telemetry_poll(void);
void telemetry_refresh_ui(void);
const telemetry_status_t *telemetry_status(void);

#endif
