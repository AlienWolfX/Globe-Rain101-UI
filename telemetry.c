#define _DEFAULT_SOURCE

#include "telemetry.h"

#include "ui/screens.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MODEM_COMMAND "ubus -S call mtk.cell get_device_info 2>/dev/null"
#define MODEM_JSON_SIZE 8192

static telemetry_status_t status;
static uint32_t boot_epoch;
static uint64_t previous_rx;
static uint64_t previous_tx;
static uint32_t previous_net_tick;
static bool have_previous_net_sample;

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
    struct carrier_code {
        const char *code;
        const char *name;
    };
    static const struct carrier_code codes[] = {
        {"51501", "Globe"},   // Innove Communications / Islacom (Globe)
        {"51502", "Globe"},   // Globe Telecom (Primary)
        {"51503", "Smart"},   // Smart Communications (PLDT)
        {"51505", "Sun"},     // Digitel / Sun Cellular (merged into Smart)
        {"51511", "PLDT"},    // PLDT via ACeS Philippines
        {"51518", "Cure"},    // Connectivity Unlimited Resources (Smart/PLDT)
        {"51566", "DITO"},    // DITO Telecommunity
        {"51588", "Next"}     // Next Mobile Inc.
    };

    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); ++i) {
        if (strcmp(plmn, codes[i].code) == 0) {
            return codes[i].name;
        }
    }
    return "Unknown";
}

static const char *radio_for_values(const char *ps_eact, const char *combo_band) {
    const char *values[] = {ps_eact, combo_band};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        if (strstr(values[i], "5G NSA") || strstr(values[i], "NSA") ||
            strstr(values[i], "ENDC")) return "5G NSA";
        if (strstr(values[i], "5G SA") || strstr(values[i], "SA")) return "5G SA";
        if (strstr(values[i], "WCDMA")) return "WCDMA";
        if (strstr(values[i], "HSPA")) return "HSPA";
        if (strstr(values[i], "UMTS")) return "UMTS";
        if (strstr(values[i], "LTE")) return "LTE";
    }
    return "3";
}

static void poll_modem(void) {
    char json[MODEM_JSON_SIZE];
    char plmn[32] = "";
    char combo_band[64] = "";
    char ps_eact[64] = "";
    FILE *pipe = popen(MODEM_COMMAND, "r");
    size_t used = 0;

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

    copy_json_field(json, "PLMN", plmn, sizeof(plmn));
    if (plmn[0] == '\0') {
        copy_json_field(json, "plmn", plmn, sizeof(plmn));
    }
    copy_json_field(json, "combo_band", combo_band, sizeof(combo_band));
    copy_json_field(json, "PS_eAct", ps_eact, sizeof(ps_eact));
    snprintf(status.carrier, sizeof(status.carrier), "%s", carrier_for_plmn(plmn));
    snprintf(status.radio, sizeof(status.radio), "%s", radio_for_values(ps_eact, combo_band));
}

static void poll_network(uint32_t now) {
    char line[128];
    char interface_name[sizeof(status.interface_name)] = "";
    unsigned long long rx_value = 0;
    unsigned long long tx_value = 0;
    uint64_t rx;
    uint64_t tx;
    uint32_t delta_tick = now - previous_net_tick;
    FILE *pipe = popen("ip -o addr show up 2>/dev/null", "r");

    if (pipe) {
        while (fgets(line, sizeof(line), pipe)) {
            char candidate[sizeof(interface_name)];
            if (sscanf(line, "%*u: %15[^ ]", candidate) == 1 && strcmp(candidate, "lo") != 0) {
                snprintf(interface_name, sizeof(interface_name), "%s", candidate);
                break;
            }
        }
        pclose(pipe);
    }
    if (interface_name[0] == '\0') {
        return;
    }
    {
        char path[128];
        snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", interface_name);
        FILE *file = fopen(path, "r");
        if (file) {
            (void)fscanf(file, "%llu", &rx_value);
            fclose(file);
        }
        snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", interface_name);
        file = fopen(path, "r");
        if (file) {
            (void)fscanf(file, "%llu", &tx_value);
            fclose(file);
        }
    }
    rx = (uint64_t)rx_value;
    tx = (uint64_t)tx_value;
    snprintf(status.interface_name, sizeof(status.interface_name), "%s", interface_name);
    if (have_previous_net_sample && delta_tick > 0 && rx >= previous_rx && tx >= previous_tx) {
        status.rx_mbps = (float)((rx - previous_rx) * 8.0 / delta_tick / 1000.0);
        status.tx_mbps = (float)((tx - previous_tx) * 8.0 / delta_tick / 1000.0);
    }
    previous_rx = rx;
    previous_tx = tx;
    previous_net_tick = now;
    have_previous_net_sample = true;
}

static void update_clock(uint32_t now) {
    time_t current = (time_t)boot_epoch + now / 1000;
    struct tm local_time;
    localtime_r(&current, &local_time);
    strftime(status.time_text, sizeof(status.time_text), "%I:%M %p", &local_time);
    strftime(status.date_text, sizeof(status.date_text), "%b %d, %Y", &local_time);
}

void telemetry_init(void) {
    memset(&status, 0, sizeof(status));
    status.state[0] = '\0';
    boot_epoch = (uint32_t)time(NULL) - lv_tick_get() / 1000;
    snprintf(status.carrier, sizeof(status.carrier), "PLMN Unknown");
    snprintf(status.radio, sizeof(status.radio), "3");
    snprintf(status.state, sizeof(status.state), "Ready");
    update_clock(lv_tick_get());
}

void telemetry_poll(void) {
    uint32_t now = lv_tick_get();
    int rsrp;

    poll_modem();
    if (read_max_rsrp(&rsrp)) {
        status.rsrp_dbm = rsrp;
        status.signal_bars = signal_level(rsrp);
        status.has_rsrp = true;
    }
    poll_network(now);
    status.last_poll_tick = now;
    snprintf(status.state, sizeof(status.state), "Ready");
}

void telemetry_refresh_ui(void) {
    uint32_t now = lv_tick_get();
    char text[64];

    update_clock(now);
    lv_label_set_text(objects.time_label, status.time_text);
    lv_label_set_text(objects.date_label, status.date_text);
    lv_label_set_text(objects.telco_label, status.carrier);
    lv_label_set_text(objects.network_label, status.radio);
    lv_bar_set_value(objects.bar_1, status.signal_bars >= 1 ? 100 : 0, LV_ANIM_OFF);
    lv_bar_set_value(objects.bar_2, status.signal_bars >= 2 ? 100 : 0, LV_ANIM_OFF);
    lv_bar_set_value(objects.bar_3, status.signal_bars >= 3 ? 100 : 0, LV_ANIM_OFF);
    lv_bar_set_value(objects.bar_4, status.signal_bars >= 4 ? 100 : 0, LV_ANIM_OFF);
    lv_bar_set_value(objects.bar_5, status.signal_bars >= 5 ? 100 : 0, LV_ANIM_OFF);
    status.last_ui_tick = now;
}

const telemetry_status_t *telemetry_status(void) {
    return &status;
}
