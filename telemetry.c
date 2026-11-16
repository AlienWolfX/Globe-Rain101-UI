#define _DEFAULT_SOURCE

#include "telemetry.h"

#include "lvgl.h"
#include "ui/screens.h"

#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MODEM_COMMAND "ubus -S call mtk.cell get_device_info 2>/dev/null"
#define MODEM_JSON_SIZE 8192
#define DISPLAY_UTC_OFFSET_SECONDS (8 * 60 * 60)

/* The worker wakes every second (cheap: a few small /proc and sysfs reads).
 * The expensive fork+exec of ubus runs once every MODEM_POLL_DIVIDER wakeups. */
#define MODEM_POLL_DIVIDER 3

/* Connected devices: one "mac" entry per client in the ue_get reply. */
#define DEVICE_COMMAND "ubus call jytl_api ue_get 2>/dev/null"
#define DEVICE_POLL_DIVIDER 5

/* time() earlier than this means the router has not synced its clock yet. */
#define CLOCK_VALID_AFTER 1577836800    /* 2020-01-01 */

/* Only what the UI actually shows. */
typedef struct {
    int signal_bars;
    int sinr_db;
    bool has_sinr;
    int connected_devices;      /* -1 until the first successful poll */
    char carrier[32];
    char radio[16];
    char temperature_text[16];
    char cpu_load_text[16];
    char memory_load_text[16];
    char imei_text[32];
    char cnum_text[32];
} telemetry_status_t;

/* Written by the worker thread, read by the UI thread; guarded by status_mutex. */
static telemetry_status_t status;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

/* UI-thread-only state. */
static bool has_displayed_sinr;
static int displayed_sinr;
static int displayed_signal_bars = -1;
static int displayed_connected_devices = -1;

/* Worker-thread-only state (no lock needed). */
static uint64_t previous_cpu_total;
static uint64_t previous_cpu_idle;
static bool have_previous_cpu_sample;
static unsigned poll_counter;

static bool read_int_file(const char *path, int *value) {
    FILE *file = fopen(path, "r");
    int parsed;
    bool valid = false;

    if (!file) {
        return false;
    }
    if (fscanf(file, "%d", &parsed) == 1) {
        *value = parsed;
        valid = true;
    }
    fclose(file);
    return valid;
}

/* ---- system metrics ------------------------------------------------------ */

static void read_system_metrics(telemetry_status_t *out) {
    int millidegrees;
    unsigned long long total_kb = 0;
    unsigned long long available_kb = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    FILE *file;

    if (read_int_file("/sys/class/thermal/thermal_zone0/temp", &millidegrees)) {
        snprintf(out->temperature_text, sizeof(out->temperature_text),
                 "%.1fC", millidegrees / 1000.0);
    }

    file = fopen("/proc/stat", "r");
    if (file) {
        char cpu[8];
        if (fscanf(file, "%7s %llu %llu %llu %llu %llu %llu %llu %llu",
                   cpu, &user, &nice, &system, &idle, &iowait, &irq,
                   &softirq, &steal) == 9) {
            uint64_t total = user + nice + system + idle + iowait +
                             irq + softirq + steal;
            uint64_t idle_total = idle + iowait;

            if (have_previous_cpu_sample && total > previous_cpu_total &&
                idle_total >= previous_cpu_idle) {
                uint64_t total_delta = total - previous_cpu_total;
                uint64_t idle_delta = idle_total - previous_cpu_idle;
                unsigned long busy_percent = idle_delta >= total_delta
                    ? 0
                    : (unsigned long)(((total_delta - idle_delta) * 100) / total_delta);
                snprintf(out->cpu_load_text, sizeof(out->cpu_load_text),
                         "%lu%%", busy_percent);
            }
            previous_cpu_total = total;
            previous_cpu_idle = idle_total;
            have_previous_cpu_sample = true;
        }
        fclose(file);
    }

    /* MemTotal and MemAvailable are within the first few lines: stop reading
     * as soon as both are found instead of parsing the whole file. */
    file = fopen("/proc/meminfo", "r");
    if (file) {
        char line[96];
        while ((total_kb == 0 || available_kb == 0) && fgets(line, sizeof(line), file)) {
            if (sscanf(line, "MemTotal: %llu", &total_kb) == 1) continue;
            (void)sscanf(line, "MemAvailable: %llu", &available_kb);
        }
        fclose(file);
    }
    if (total_kb > 0 && available_kb <= total_kb) {
        snprintf(out->memory_load_text, sizeof(out->memory_load_text),
                 "%.0f%%", (double)(total_kb - available_kb) * 100.0 / total_kb);
    }
}

/* ---- modem --------------------------------------------------------------- */

static int signal_level(int rsrp) {
    if (rsrp >= -80) return 5;
    if (rsrp >= -90) return 4;
    if (rsrp >= -100) return 3;
    if (rsrp >= -110) return 2;
    if (rsrp >= -120) return 1;
    return 0;
}

static bool read_max_rsrp(int *result) {
    static const char *const paths[] = {
        "/var/run/ModemInfo/current_slot/5G/SSB_RSRP",
        "/var/run/ModemInfo/current_slot/5G/RSRP",
        "/var/run/ModemInfo/current_slot/4G/RSRP"
    };
    bool found = false;
    int maximum = -1000;

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        int value;
        if (read_int_file(paths[i], &value) && (!found || value > maximum)) {
            maximum = value;
            found = true;
        }
    }
    if (found) {
        *result = maximum;
    }
    return found;
}

static bool json_value(const char *json, const char *key, char *value, size_t value_size) {
    char needle[64];
    const char *cursor;
    const char *start;
    size_t length;

    if (snprintf(needle, sizeof(needle), "\"%s\"", key) >= (int)sizeof(needle)) {
        return false;
    }
    cursor = json;
    while ((cursor = strstr(cursor, needle)) != NULL) {
        cursor += strlen(needle);
        while (*cursor && (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' ||
                           *cursor == '\n' || *cursor == ':')) {
            ++cursor;
        }
        if (*cursor == '"') {
            start = ++cursor;
            while (*cursor && *cursor != '"') {
                if (*cursor == '\\' && cursor[1]) {
                    ++cursor;
                }
                ++cursor;
            }
        } else {
            start = cursor;
            while (*cursor && *cursor != ',' && *cursor != '}' && !isspace((unsigned char)*cursor)) {
                ++cursor;
            }
        }
        length = (size_t)(cursor - start);
        if (length == 0 || length >= value_size) {
            return false;
        }
        memcpy(value, start, length);
        value[length] = '\0';
        return true;
    }
    return false;
}

static void copy_json_field(const char *json, const char *key, char *destination, size_t size) {
    char value[64];
    if (json_value(json, key, value, sizeof(value))) {
        snprintf(destination, size, "%s", value);
    }
}

static const char *carrier_for_plmn(const char *plmn) {
    static const struct {
        const char *code;
        const char *name;
    } codes[] = {
        {"51501", "Globe"},   /* Innove Communications / Islacom (Globe) */
        {"51502", "Globe"},   /* Globe Telecom (Primary) */
        {"51503", "Smart"},   /* Smart Communications (PLDT) */
        {"51505", "Sun"},     /* Digitel / Sun Cellular (merged into Smart) */
        {"51511", "PLDT"},    /* PLDT via ACeS Philippines */
        {"51518", "Cure"},    /* Connectivity Unlimited Resources (Smart/PLDT) */
        {"51566", "DITO"},    /* DITO Telecommunity */
        {"51588", "Next"}     /* Next Mobile Inc. */
    };

    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); ++i) {
        if (strcmp(plmn, codes[i].code) == 0) {
            return codes[i].name;
        }
    }
    return "";
}

static const char *radio_for_values(const char *ps_eact, const char *combo_band) {
    const char *values[] = {ps_eact, combo_band};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        if (strstr(values[i], "NSA") || strstr(values[i], "ENDC")) return "5G NSA";
        if (strstr(values[i], "SA")) return "5G SA";
        if (strstr(values[i], "WCDMA")) return "WCDMA";
        if (strstr(values[i], "HSPA")) return "HSPA";
        if (strstr(values[i], "UMTS")) return "UMTS";
        if (strstr(values[i], "LTE")) return "LTE";
    }
    return "3";
}

/* Runs WITHOUT status_mutex held; writes into the caller's private copy. */
static void poll_modem(telemetry_status_t *out) {
    char json[MODEM_JSON_SIZE];
    char plmn[32] = "";
    char combo_band[64] = "";
    char ps_eact[64] = "";
    char sinr_text[32] = "";
    char imei[32] = "";
    char cnum[32] = "";
    int sinr_db;
    size_t used = 0;
    FILE *pipe = popen(MODEM_COMMAND, "r");

    if (!pipe) {
        return;
    }
    while (used + 1 < sizeof(json)) {
        size_t count = fread(json + used, 1, sizeof(json) - used - 1, pipe);
        used += count;
        if (count == 0) break;
    }
    json[used] = '\0';
    pclose(pipe);

    /* No output (ubus busy / modem restarting): keep the last good values
     * instead of flashing "Unknown" on screen. */
    if (used == 0) {
        return;
    }

    copy_json_field(json, "PLMN", plmn, sizeof(plmn));
    if (plmn[0] == '\0') {
        copy_json_field(json, "plmn", plmn, sizeof(plmn));
    }
    copy_json_field(json, "combo_band", combo_band, sizeof(combo_band));
    copy_json_field(json, "PS_eAct", ps_eact, sizeof(ps_eact));
    copy_json_field(json, "sinr", sinr_text, sizeof(sinr_text));
    copy_json_field(json, "imei", imei, sizeof(imei));
    copy_json_field(json, "cnum", cnum, sizeof(cnum));
    if (imei[0] != '\0') {
        snprintf(out->imei_text, sizeof(out->imei_text), "%s", imei);
    }
    if (cnum[0] != '\0') {
        snprintf(out->cnum_text, sizeof(out->cnum_text), "%s", cnum);
    }
    if (sscanf(sinr_text, "%d", &sinr_db) == 1) {
        out->sinr_db = sinr_db;
        out->has_sinr = true;
    }
    if (plmn[0] != '\0') {
        snprintf(out->carrier, sizeof(out->carrier), "%s", carrier_for_plmn(plmn));
    }
    snprintf(out->radio, sizeof(out->radio), "%s", radio_for_values(ps_eact, combo_band));
}

/* Counts the "mac" keys in `ubus call jytl_api ue_get` (the same number as
 * `... | grep -c '"mac"'`). Streams the output line by line, so the reply size
 * does not matter, and counts every occurrence so compact JSON works too.
 * Leaves *count untouched if ubus produced no output (busy / not ready). */
static void poll_devices(int *count) {
    char line[256];
    int found = 0;
    bool got_output = false;
    FILE *pipe = popen(DEVICE_COMMAND, "r");

    if (!pipe) {
        return;
    }
    while (fgets(line, sizeof(line), pipe)) {
        const char *cursor = line;
        got_output = true;
        while ((cursor = strstr(cursor, "\"mac\"")) != NULL) {
            ++found;
            cursor += 5;
        }
    }
    pclose(pipe);
    if (got_output) {
        *count = found;
    }
}

/* ---- UI helpers (UI thread only) ----------------------------------------- */

/* lv_label_set_text() always invalidates; every invalidation costs a panel
 * commit, so only touch LVGL when the text really changed. */
static void set_label_if_changed(lv_obj_t *label, const char *text) {
    const char *current = lv_label_get_text(label);
    if (!current || strcmp(current, text) != 0) {
        lv_label_set_text(label, text);
    }
}

static void set_signal_bars(int level) {
    lv_obj_t *const bars[] = {
        objects.bar_1, objects.bar_2, objects.bar_3, objects.bar_4, objects.bar_5
    };
    for (int i = 0; i < 5; ++i) {
        lv_bar_set_value(bars[i], level > i ? 100 : 0, LV_ANIM_OFF);
    }
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
        time_t display_time = now + DISPLAY_UTC_OFFSET_SECONDS;
        struct tm local_time;
        gmtime_r(&display_time, &local_time);
        strftime(time_text, sizeof(time_text), "%I:%M %p", &local_time);
        strftime(date_text, sizeof(date_text), "%b %d, %Y", &local_time);
    }
    set_label_if_changed(objects.time_label, time_text);
    set_label_if_changed(objects.date_label, date_text);
}

/* ---- public API ---------------------------------------------------------- */

void telemetry_init(void) {
    memset(&status, 0, sizeof(status));
    status.connected_devices = -1;
    snprintf(status.radio, sizeof(status.radio), "3");
    set_signal_bars(0);
    lv_arc_set_range(objects.sinr_arc, -20, 30);   /* once, not every refresh */
    displayed_signal_bars = -1;
}

static void telemetry_poll(void) {
    telemetry_status_t local;
    int rsrp;

    pthread_mutex_lock(&status_mutex);
    local = status;
    pthread_mutex_unlock(&status_mutex);

    if (poll_counter % MODEM_POLL_DIVIDER == 0) {
        poll_modem(&local);
    }
    if (poll_counter % DEVICE_POLL_DIVIDER == 0) {
        poll_devices(&local.connected_devices);
    }
    ++poll_counter;
    if (read_max_rsrp(&rsrp)) {
        local.signal_bars = signal_level(rsrp);
    }
    read_system_metrics(&local);

    pthread_mutex_lock(&status_mutex);
    status = local;
    pthread_mutex_unlock(&status_mutex);
}

void telemetry_refresh_ui(void) {
    telemetry_status_t snapshot;

    pthread_mutex_lock(&status_mutex);
    snapshot = status;
    pthread_mutex_unlock(&status_mutex);

    /* All LVGL calls below run without the lock. */
    refresh_clock();
    set_label_if_changed(objects.telco_label, snapshot.carrier);
    set_label_if_changed(objects.network_label, snapshot.radio);
    if (snapshot.temperature_text[0] != '\0') {
        set_label_if_changed(objects.temperature_label, snapshot.temperature_text);
    }
    if (snapshot.cpu_load_text[0] != '\0') {
        set_label_if_changed(objects.cpu_load_label, snapshot.cpu_load_text);
    }
    if (snapshot.memory_load_text[0] != '\0') {
        set_label_if_changed(objects.ram_load_label, snapshot.memory_load_text);
    }
    if (snapshot.cnum_text[0] != '\0') {
        set_label_if_changed(objects.phone_number, snapshot.cnum_text);
    }
    if (snapshot.imei_text[0] != '\0') {
        set_label_if_changed(objects.imei_label, snapshot.imei_text);
    }
    if (snapshot.connected_devices >= 0 &&
        snapshot.connected_devices != displayed_connected_devices) {
        char text[16];
        snprintf(text, sizeof(text), "%d", snapshot.connected_devices);
        set_label_if_changed(objects.connected_device_label, text);
        displayed_connected_devices = snapshot.connected_devices;
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

static void *telemetry_worker(void *arg) {
    (void)arg;
    for (;;) {
        telemetry_poll();
        sleep(1);
    }
    return NULL;
}

void telemetry_start(void) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, telemetry_worker, NULL) != 0) {
        fprintf(stderr, "[telemetry] failed to start polling thread\n");
    } else {
        pthread_detach(thread);
    }
}
