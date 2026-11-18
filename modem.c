#define _GNU_SOURCE

#include "modem.h"

#include "carrier.h"
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GET_DEVICE_INFO "ubus -S call mtk.cell get_device_info 2>/dev/null"
#define UBUS_TIMEOUT_MS 5000
#define MODEM_JSON_SIZE 8192

#define DEFAULT_MODEM_DIR "/var/run/ModemInfo"

/* Anything outside these ranges is a placeholder, not a real reading. */
#define RSRP_MIN_VALID (-140)
#define RSRP_MAX_VALID (-40)
#define SINR_MIN_VALID (-30)
#define SINR_MAX_VALID 50

typedef enum {
    SIM_UNKNOWN,
    SIM_ABSENT,
    SIM_PRESENT
} sim_state_t;

static char modem_dir[128] = DEFAULT_MODEM_DIR;

void modem_init(void) {
    const char *dir = getenv("ROUTERUI_MODEM_DIR");
    if (dir && dir[0] != '\0') {
        snprintf(modem_dir, sizeof(modem_dir), "%s", dir);
    }
}

/* ---- file access --------------------------------------------------------- */

static bool read_slot_text(const char *relative, char *value, size_t size) {
    char path[192];
    snprintf(path, sizeof(path), "%s/current_slot/%s", modem_dir, relative);
    return read_text_file(path, value, size);
}

static bool read_slot_int(const char *relative, int *value) {
    char path[192];
    snprintf(path, sizeof(path), "%s/current_slot/%s", modem_dir, relative);
    return read_int_file(path, value);
}

static bool read_root_text(const char *relative, char *value, size_t size) {
    char path[192];
    snprintf(path, sizeof(path), "%s/%s", modem_dir, relative);
    return read_text_file(path, value, size);
}

/* ---- SIM ----------------------------------------------------------------- */

static sim_state_t classify_sim_status(const char *text) {
    char lower[64];
    size_t i;
    bool numeric = true;

    if (!text || text[0] == '\0') {
        return SIM_UNKNOWN;
    }
    for (i = 0; text[i] && i + 1 < sizeof(lower); ++i) {
        lower[i] = (char)tolower((unsigned char)text[i]);
        if (!isdigit((unsigned char)text[i])) {
            numeric = false;
        }
    }
    lower[i] = '\0';

    if (numeric) {
        return strtoul(lower, NULL, 10) != 0 ? SIM_PRESENT : SIM_ABSENT;
    }
    if (strstr(lower, "no sim") || strstr(lower, "nosim") || strstr(lower, "no_sim") ||
        strstr(lower, "absent") || strstr(lower, "removed") || strstr(lower, "missing") ||
        strstr(lower, "invalid") || strstr(lower, "not") || strcmp(lower, "false") == 0) {
        return SIM_ABSENT;
    }
    if (strstr(lower, "ready") || strstr(lower, "inserted") || strstr(lower, "present") ||
        strstr(lower, "valid") || strcmp(lower, "normal") == 0 || strcmp(lower, "true") == 0) {
        return SIM_PRESENT;
    }
    return SIM_UNKNOWN;
}

static bool nonzero_text(const char *text) {
    return text[0] != '\0' && strcmp(text, "0") != 0;
}

/* Decides whether a SIM is inserted. An explicit status (modem JSON, then the
 * status file) wins. ICCID/PLMN are only a fallback when no status is usable.
 * The IMEI is NOT evidence: it identifies the modem and exists without a SIM. */
static sim_state_t detect_sim(const char *json) {
    char state[64] = "";
    char iccid[32] = "";
    char plmn[32] = "";
    sim_state_t result;

    (void)(json_value(json, "sim_status", state, sizeof(state)) ||
           json_value(json, "SIMStatus", state, sizeof(state)) ||
           json_value(json, "sim_state", state, sizeof(state)));
    (void)json_value(json, "iccid", iccid, sizeof(iccid));
    if (!json_value(json, "PLMN", plmn, sizeof(plmn))) {
        (void)json_value(json, "plmn", plmn, sizeof(plmn));
    }

    result = classify_sim_status(state);
    if (result != SIM_UNKNOWN) {
        return result;
    }
    if (read_slot_text("sim_status", state, sizeof(state))) {
        result = classify_sim_status(state);
        if (result != SIM_UNKNOWN) {
            return result;
        }
    }
    if (nonzero_text(iccid) || plmn[0] != '\0') {
        return SIM_PRESENT;
    }
    return SIM_UNKNOWN;
}

/* ---- signal -------------------------------------------------------------- */

static int signal_level(int rsrp) {
    if (rsrp >= -80) return 5;
    if (rsrp >= -90) return 4;
    if (rsrp >= -100) return 3;
    if (rsrp >= -110) return 2;
    if (rsrp >= -120) return 1;
    return 0;
}

static bool read_max_rsrp(int *result) {
    static const char *const files[] = { "5G/SSB_RSRP", "5G/RSRP", "4G/RSRP" };
    bool found = false;
    int maximum = RSRP_MIN_VALID;

    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i) {
        int value;
        if (read_slot_int(files[i], &value) &&
            value >= RSRP_MIN_VALID && value <= RSRP_MAX_VALID &&
            (!found || value > maximum)) {
            maximum = value;
            found = true;
        }
    }
    if (found) {
        *result = maximum;
    }
    return found;
}

static bool read_sinr(bool is_5g, int *result) {
    static const char *const files_5g[] = { "5G/SINR", "5G/SSB_SINR" };
    static const char *const files_4g[] = { "4G/SINR" };
    const char *const *files = is_5g ? files_5g : files_4g;
    size_t count = is_5g ? sizeof(files_5g) / sizeof(files_5g[0])
                         : sizeof(files_4g) / sizeof(files_4g[0]);

    for (size_t i = 0; i < count; ++i) {
        int value;
        if (read_slot_int(files[i], &value) &&
            value >= SINR_MIN_VALID && value <= SINR_MAX_VALID) {
            *result = value;
            return true;
        }
    }
    return false;
}

static const char *radio_for_value(const char *text) {
    if (strstr(text, "NSA") || strstr(text, "ENDC")) return "5G NSA";
    if (strstr(text, "SA")) return "5G SA";
    if (strstr(text, "WCDMA")) return "WCDMA";
    if (strstr(text, "HSPA")) return "HSPA";
    if (strstr(text, "UMTS")) return "UMTS";
    if (strstr(text, "LTE")) return "LTE";
    return "3";
}

/* Shows the registration problem instead of a stale carrier name. */
static void update_registration(telemetry_status_t *out) {
    char state[48] = "";
    const char *label;
    int deny = 0;

    out->reg_text[0] = '\0';
    if (!read_slot_text("reg_state", state, sizeof(state)) || strstr(state, "FULL")) {
        return;
    }
    (void)read_slot_int("deny_reason", &deny);
    if (deny != 0) {
        label = "Denied";
    } else if (strstr(state, "LIMITED")) {
        label = "Limited";
    } else if (strstr(state, "SEARCH")) {
        label = "Searching";
    } else {
        label = "No service";
    }
    snprintf(out->reg_text, sizeof(out->reg_text), "%s", label);
}

/* Unknown carriers show the raw PLMN instead of nothing. */
static void set_carrier(telemetry_status_t *out, const char *plmn) {
    const char *name = carrier_for_plmn(plmn);
    snprintf(out->carrier, sizeof(out->carrier), "%s", name[0] != '\0' ? name : plmn);
}

/* ---- public -------------------------------------------------------------- */

bool modem_poll_files(telemetry_status_t *out) {
    char text[64];
    char identity[32];
    char plmn[32] = "";
    int value;
    bool plmn_ok;
    bool radio_ok;

    out->signal_bars = read_max_rsrp(&value) ? signal_level(value) : 0;

    plmn_ok = read_slot_text("PLMN", plmn, sizeof(plmn));
    radio_ok = read_slot_text("radio_tech", text, sizeof(text));
    if (!plmn_ok && !radio_ok) {
        return false;
    }
    if (radio_ok) {
        snprintf(out->radio, sizeof(out->radio), "%s", radio_for_value(text));
    }
    set_carrier(out, plmn);
    update_registration(out);

    if (read_slot_text("sim_status", text, sizeof(text))) {
        sim_state_t sim = classify_sim_status(text);
        if (sim != SIM_UNKNOWN) {
            out->has_sim = (sim == SIM_PRESENT);
            out->sim_status_known = true;
        }
    }
    if (read_root_text("imei", identity, sizeof(identity))) {
        snprintf(out->imei_text, sizeof(out->imei_text), "%s", identity);
    }
    if (read_slot_text("cnum", identity, sizeof(identity))) {
        snprintf(out->cnum_text, sizeof(out->cnum_text), "%s", identity);
    }
    if (read_sinr(strncmp(out->radio, "5G", 2) == 0, &value)) {
        out->sinr_db = value;
        out->has_sinr = true;
    }
    return true;
}

static void parse_combo_band(const char *json, telemetry_status_t *out) {
    char combo_band[64];
    size_t length = 0;

    if (!json_value(json, "combo_band", combo_band, sizeof(combo_band))) {
        return;
    }
    for (const char *cursor = combo_band;
         *cursor && length + 1 < sizeof(out->combo_band_text); ++cursor) {
        if (!isspace((unsigned char)*cursor)) {
            out->combo_band_text[length++] = (char)toupper((unsigned char)*cursor);
        }
    }
    out->combo_band_text[length] = '\0';
}

void modem_poll_ubus(telemetry_status_t *out, bool combo_only) {
    char json[MODEM_JSON_SIZE];
    char plmn[32] = "";
    char ps_eact[64] = "";
    char sinr_text[32] = "";
    int sinr_db;
    sim_state_t sim;

    if (run_command(GET_DEVICE_INFO, json, sizeof(json), UBUS_TIMEOUT_MS) == 0) {
        return;
    }
    parse_combo_band(json, out);
    if (combo_only) {
        return;
    }

    sim = detect_sim(json);
    if (sim != SIM_UNKNOWN) {
        out->has_sim = (sim == SIM_PRESENT);
        out->sim_status_known = true;
    }

    if (!json_value(json, "PLMN", plmn, sizeof(plmn))) {
        (void)json_value(json, "plmn", plmn, sizeof(plmn));
    }
    (void)json_value(json, "PS_eAct", ps_eact, sizeof(ps_eact));
    (void)json_value(json, "sinr", sinr_text, sizeof(sinr_text));
    (void)json_value(json, "imei", out->imei_text, sizeof(out->imei_text));
    (void)json_value(json, "cnum", out->cnum_text, sizeof(out->cnum_text));

    if (sscanf(sinr_text, "%d", &sinr_db) == 1) {
        out->sinr_db = sinr_db;
        out->has_sinr = true;
    }
    set_carrier(out, plmn);
    snprintf(out->radio, sizeof(out->radio), "%s", radio_for_value(ps_eact));
}
