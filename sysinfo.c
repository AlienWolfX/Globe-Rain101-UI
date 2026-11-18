#define _GNU_SOURCE

#include "sysinfo.h"

#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_BOARD_INFO "ubus call system board 2>/dev/null"
#define GET_DEVICE_DETAILS "ubus -S call jytl_api devinfo_get 2>/dev/null"
#define GET_CONNECTED_DEVICES "ubus -S call jytl_api ue_get 2>/dev/null"
#define GET_TRAFFIC "ubus -S call cell.traffic get_traffic_cfg 2>/dev/null"
#define UBUS_TIMEOUT_MS 5000

#define DEVICE_TEMP "/sys/class/thermal/thermal_zone0/temp"
#define DEVICE_CPU_STAT "/proc/stat"
#define DEVICE_MEM_INFO "/proc/meminfo"
#define UPTIME_FILE "/proc/uptime"

static uint64_t previous_cpu_total;
static uint64_t previous_cpu_idle;
static bool have_previous_cpu_sample;

static void read_uptime(telemetry_status_t *out) {
    FILE *file = fopen(UPTIME_FILE, "r");
    double uptime_seconds;
    unsigned long total, days, hours, minutes;

    if (!file) return;
    if (fscanf(file, "%lf", &uptime_seconds) != 1) {
        fclose(file);
        return;
    }
    fclose(file);

    total = (unsigned long)uptime_seconds;
    days = total / 86400;
    hours = (total % 86400) / 3600;
    minutes = (total % 3600) / 60;
    if (days > 0) {
        snprintf(out->uptime_text, sizeof(out->uptime_text),
                 "%lud %02lu:%02lu", days, hours, minutes);
    } else {
        snprintf(out->uptime_text, sizeof(out->uptime_text),
                 "%02lu:%02lu", hours, minutes);
    }
}

void sysinfo_poll_fast(telemetry_status_t *out) {
    int millidegrees;
    unsigned long long total_kb = 0;
    unsigned long long available_kb = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    FILE *file;

    if (read_int_file(DEVICE_TEMP, &millidegrees)) {
        snprintf(out->temperature_text, sizeof(out->temperature_text),
                 "%.1fC", millidegrees / 1000.0);
    }

    file = fopen(DEVICE_CPU_STAT, "r");
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

    file = fopen(DEVICE_MEM_INFO, "r");
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

    read_uptime(out);
}

/* month_traffic looks like "<bytes>-<something>"; strtod stops at the dash. */
void sysinfo_poll_data_usage(telemetry_status_t *out) {
    char json[4096];
    char value[48];
    char *end;
    double bytes;

    if (run_command(GET_TRAFFIC, json, sizeof(json), UBUS_TIMEOUT_MS) == 0 ||
        !json_value(json, "month_traffic", value, sizeof(value))) {
        return;
    }
    bytes = strtod(value, &end);
    if (end == value) {
        return;
    }
    snprintf(out->data_usage_text, sizeof(out->data_usage_text),
             "%.2f GB", bytes / 1073741824.0);
}

void sysinfo_poll_board(telemetry_status_t *out) {
    char json[8192];

    if (run_command(GET_BOARD_INFO, json, sizeof(json), UBUS_TIMEOUT_MS) > 0) {
        (void)json_value(json, "model", out->board_model_text, sizeof(out->board_model_text));
        (void)json_value(json, "board_name", out->board_name_text, sizeof(out->board_name_text));
        (void)json_value(json, "system", out->board_system_text, sizeof(out->board_system_text));
        (void)json_value(json, "version", out->board_version_text, sizeof(out->board_version_text));
        (void)json_value(json, "description", out->board_description_text,
                         sizeof(out->board_description_text));
    }
    if (run_command(GET_DEVICE_DETAILS, json, sizeof(json), UBUS_TIMEOUT_MS) > 0) {
        (void)json_value(json, "ModelName", out->dev_model_text, sizeof(out->dev_model_text));
        (void)json_value(json, "boardSN", out->dev_board_sn_text, sizeof(out->dev_board_sn_text));
        (void)json_value(json, "MAC_WAN", out->dev_mac_wan_text, sizeof(out->dev_mac_wan_text));
        (void)json_value(json, "HardwareVersion", out->dev_hardware_text,
                         sizeof(out->dev_hardware_text));
        (void)json_value(json, "SoftwareVersion", out->dev_software_text,
                         sizeof(out->dev_software_text));
        (void)json_value(json, "NickName", out->dev_nickname_text,
                         sizeof(out->dev_nickname_text));
    }
}

/* Counts "mac" entries of the online list (everything before "offline_list"). */
void sysinfo_poll_devices(int *count) {
    static char json[16384];            /* worker thread only */
    const char *end;
    const char *cursor = json;
    int found = 0;

    if (run_command(GET_CONNECTED_DEVICES, json, sizeof(json), UBUS_TIMEOUT_MS) == 0) {
        return;
    }
    end = strstr(json, "\"offline_list\"");
    if (!end) {
        end = json + strlen(json);
    }
    while ((cursor = strstr(cursor, "\"mac\"")) != NULL && cursor < end) {
        ++found;
        cursor += 5;
    }
    *count = found;
}
