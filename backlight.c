#define _GNU_SOURCE

#include "backlight.h"

#include "util.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKLIGHT_ROOT "/sys/class/backlight"
#define DEFAULT_DIM_AFTER_SECONDS 30
#define DIM_PERCENT 10

static char brightness_path[192];
static int normal_level;
static int dim_level;
static uint32_t dim_after_ms;
static bool enabled;
static bool dimmed;
static bool has_woken;
static uint32_t woken_at_ms;

static bool write_level(int level) {
    FILE *file = fopen(brightness_path, "w");
    if (!file) {
        return false;
    }
    fprintf(file, "%d\n", level);
    return fclose(file) == 0;
}

void backlight_init(void) {
    const char *env = getenv("ROUTERUI_DIM_SECONDS");
    long seconds = env ? strtol(env, NULL, 10) : DEFAULT_DIM_AFTER_SECONDS;
    char directory[192] = "";
    char path[256];
    int maximum, current;
    DIR *root;
    struct dirent *entry;

    if (seconds <= 0) {
        return;
    }
    root = opendir(BACKLIGHT_ROOT);
    if (!root) {
        return;
    }
    while ((entry = readdir(root)) != NULL) {
        if (entry->d_name[0] != '.') {
            snprintf(directory, sizeof(directory), "%s/%s", BACKLIGHT_ROOT, entry->d_name);
            break;
        }
    }
    closedir(root);
    if (directory[0] == '\0') {
        return;
    }

    snprintf(path, sizeof(path), "%s/max_brightness", directory);
    if (!read_int_file(path, &maximum) || maximum <= 1) {
        return;
    }
    snprintf(path, sizeof(path), "%s/brightness", directory);
    if (!read_int_file(path, &current)) {
        return;
    }
    snprintf(brightness_path, sizeof(brightness_path), "%s", path);

    normal_level = current > 0 ? current : maximum;
    dim_level = maximum * DIM_PERCENT / 100;
    if (dim_level < 1) dim_level = 1;
    if (dim_level >= normal_level) {
        return;                         /* already at or below the dim level */
    }
    dim_after_ms = (uint32_t)seconds * 1000u;
    enabled = true;
}

bool backlight_wake(void) {
    if (!enabled || !dimmed) {
        return false;
    }
    (void)write_level(normal_level);
    dimmed = false;
    has_woken = true;
    woken_at_ms = monotonic_ms();
    return true;
}

void backlight_update(uint32_t inactive_ms) {
    if (!enabled) {
        return;
    }
    if (!dimmed && inactive_ms >= dim_after_ms) {
        int current;
        /* Remember whatever brightness is set right now so a change made by
         * the system in the meantime is respected on wake. */
        if (read_int_file(brightness_path, &current) && current > dim_level) {
            normal_level = current;
        }
        if (write_level(dim_level)) {
            dimmed = true;
        }
    } else if (dimmed && inactive_ms < dim_after_ms) {
        (void)backlight_wake();
    }
}

bool backlight_recently_woken(uint32_t window_ms) {
    return has_woken && (uint32_t)(monotonic_ms() - woken_at_ms) < window_ms;
}

void backlight_restore(void) {
    if (enabled && dimmed) {
        (void)write_level(normal_level);
        dimmed = false;
    }
}
