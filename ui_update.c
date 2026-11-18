#define _GNU_SOURCE

#include "telemetry.h"

#include "lvgl.h"
#include "speedtest.h"
#include "telemetry_internal.h"
#include "ui/screens.h"
#include "ui/ui.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Used when the router has no time zone configured (POSIX syntax: UTC-8 is
 * UTC+8). */
#define DEFAULT_TZ "UTC-8"

/* UI thread only. */
static bool has_displayed_sinr;
static int displayed_sinr;
static int displayed_signal_bars = -1;

/* Clocks earlier than this are treated as "not set yet" (an unsynced router
 * reports 1970 or its firmware build date). 2026-01-01 00:00 UTC. */
#define CLOCK_VALID_AFTER 1767225600

/* ---- helpers ------------------------------------------------------------- */

static void set_label_if_changed(lv_obj_t *label, const char *text) {
    const char *current = lv_label_get_text(label);
    if (!current || strcmp(current, text) != 0) {
        lv_label_set_text(label, text);
    }
}

/* Keeps the previous text when there is nothing new to show. */
static void set_label_if_set(lv_obj_t *label, const char *text) {
    if (text[0] != '\0') {
        set_label_if_changed(label, text);
    }
}

static const char *first_non_empty(const char *preferred, const char *fallback) {
    return preferred[0] != '\0' ? preferred : fallback;
}

static void set_signal_bars(int level) {
    lv_obj_t *const bars[] = {
        objects.bar_1, objects.bar_2, objects.bar_3, objects.bar_4, objects.bar_5
    };
    for (int i = 0; i < 5; ++i) {
        lv_bar_set_value(bars[i], level > i ? 100 : 0, LV_ANIM_OFF);
    }
}

/* ---- clock --------------------------------------------------------------- */

/* Uses the router's own time zone (/etc/TZ on OpenWrt). */
static void clock_init(void) {
    char tz[64];

    if (!getenv("TZ")) {
        if (read_text_file("/etc/TZ", tz, sizeof(tz)) ||
            read_text_file("/tmp/TZ", tz, sizeof(tz))) {
            setenv("TZ", tz, 1);
        } else {
            setenv("TZ", DEFAULT_TZ, 1);
        }
    }
    tzset();
}

static void refresh_clock(void) {
    static time_t shown_minute = -1;
    time_t now = time(NULL);
    char time_text[16];
    char date_text[32];

    if (now / 60 == shown_minute) {
        return;
    }
    shown_minute = now / 60;

    if (now < CLOCK_VALID_AFTER) {
        snprintf(time_text, sizeof(time_text), "--:--");
        date_text[0] = '\0';
    } else {
        struct tm local_time;
        localtime_r(&now, &local_time);
        strftime(time_text, sizeof(time_text), "%I:%M %p", &local_time);
        strftime(date_text, sizeof(date_text), "%b %d, %Y", &local_time);
    }
    set_label_if_changed(objects.time_label, time_text);
    set_label_if_changed(objects.date_label, date_text);
}

/* ---- public -------------------------------------------------------------- */

void ui_update_init(void) {
    clock_init();
    set_signal_bars(0);
    lv_arc_set_range(objects.sinr_arc, -20, 30);
    displayed_signal_bars = -1;
}

void telemetry_refresh_ui(void) {
    telemetry_status_t snapshot;
    speedtest_state_t speed;
    bool show_results;

    telemetry_get_status(&snapshot);
    speedtest_get(&speed);

    refresh_clock();

    if (snapshot.sim_status_known) {
        bool on_no_sim = lv_screen_active() == objects.no_sim;
        if (!snapshot.has_sim && !on_no_sim) {
            loadScreen(SCREEN_ID_NO_SIM);
            telemetry_set_screen(SCREEN_ID_NO_SIM);
        } else if (snapshot.has_sim && on_no_sim) {
            loadScreen(SCREEN_ID_MAIN);
            telemetry_set_screen(SCREEN_ID_MAIN);
        }
    }

    set_label_if_changed(objects.telco_label,
                         snapshot.reg_text[0] != '\0' ? snapshot.reg_text : snapshot.carrier);
    set_label_if_changed(objects.network_label, snapshot.radio);

    set_label_if_set(objects.model_label,
                     first_non_empty(snapshot.dev_model_text, snapshot.board_model_text));
    set_label_if_set(objects.sn_label,
                     first_non_empty(snapshot.dev_board_sn_text, snapshot.board_name_text));
    set_label_if_set(objects.mac_label, snapshot.dev_mac_wan_text);
    set_label_if_set(objects.sv_label,
                     first_non_empty(snapshot.dev_hardware_text, snapshot.board_system_text));
    set_label_if_set(objects.version_label,
                     first_non_empty(snapshot.dev_software_text, snapshot.board_version_text));
    set_label_if_set(objects.system_label,
                     first_non_empty(snapshot.dev_nickname_text, snapshot.board_description_text));
    set_label_if_set(objects.cp_num_label, snapshot.cnum_text);
    set_label_if_set(objects.imei_label, snapshot.imei_text);

    set_label_if_set(objects.temperature_label, snapshot.temperature_text);
    set_label_if_set(objects.cpu_load_label, snapshot.cpu_load_text);
    set_label_if_set(objects.ram_load_label, snapshot.memory_load_text);
    set_label_if_set(objects.uptime_label, snapshot.uptime_text);
    set_label_if_set(objects.combo_label, snapshot.combo_band_text);
    set_label_if_set(objects.data_usage_label, snapshot.data_usage_text);

    /* Results are cleared while running or after a failure so stale numbers
     * from a previous run are never shown next to the new status. */
    show_results = speed.done && !speed.failed;
    if (speed.running) {
        set_label_if_changed(objects.status_label, "Running");
    } else if (speed.done) {
        set_label_if_changed(objects.status_label, speed.failed ? "Error" : "Done");
    }
    set_label_if_changed(objects.download_label, show_results ? speed.download : "");
    set_label_if_changed(objects.upload_label, show_results ? speed.upload : "");
    set_label_if_changed(objects.ping_label, show_results ? speed.ping : "");

    if (snapshot.connected_devices >= 0) {
        char text[16];
        snprintf(text, sizeof(text), "%d", snapshot.connected_devices);
        set_label_if_changed(objects.connected_device_label, text);
    }

    if (snapshot.has_sinr && (!has_displayed_sinr || displayed_sinr != snapshot.sinr_db)) {
        int arc_value = snapshot.sinr_db;
        if (arc_value < -20) arc_value = -20;
        if (arc_value > 30) arc_value = 30;
        lv_arc_set_value(objects.sinr_arc, arc_value);
        displayed_sinr = snapshot.sinr_db;
        has_displayed_sinr = true;
    }
    if (displayed_signal_bars != snapshot.signal_bars) {
        set_signal_bars(snapshot.signal_bars);
        displayed_signal_bars = snapshot.signal_bars;
    }
}
