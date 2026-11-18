#define _GNU_SOURCE

#include "speedtest.h"

#include "util.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPEED_TEST_COMMAND "/usr/bin/librespeed-cli --json --concurrent 8 --no-icmp --local-json \"/bin/lvgl/globe-unlimited.json\" 2>/dev/null"
#define SPEED_TEST_TIMEOUT_MS 90000     /* a hung test must not leave "Running" forever */
#define SPEED_JSON_SIZE 4096

static speedtest_state_t state;
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

static bool format_speed_value(const char *json, const char *key,
                               char *destination, size_t size) {
    char value[32];
    char *end;
    double number;

    if (!json_value(json, key, value, sizeof(value))) {
        return false;
    }
    number = strtod(value, &end);
    if (end == value || *end != '\0') {
        return false;
    }
    snprintf(destination, size, "%.0f", number);
    return true;
}

static void *speed_test_worker(void *arg) {
    char json[SPEED_JSON_SIZE];
    char download[24];
    char upload[24];
    char ping[24];
    bool valid;

    (void)arg;
    valid = run_command(SPEED_TEST_COMMAND, json, sizeof(json), SPEED_TEST_TIMEOUT_MS) > 0 &&
            format_speed_value(json, "download", download, sizeof(download)) &&
            format_speed_value(json, "upload", upload, sizeof(upload)) &&
            format_speed_value(json, "ping", ping, sizeof(ping));

    pthread_mutex_lock(&state_mutex);
    state.running = false;
    state.done = true;
    state.failed = !valid;
    if (valid) {
        snprintf(state.download, sizeof(state.download), "%s", download);
        snprintf(state.upload, sizeof(state.upload), "%s", upload);
        snprintf(state.ping, sizeof(state.ping), "%s", ping);
    }
    pthread_mutex_unlock(&state_mutex);
    return NULL;
}

void speedtest_start(void) {
    pthread_t thread;

    pthread_mutex_lock(&state_mutex);
    if (state.running) {
        pthread_mutex_unlock(&state_mutex);
        return;
    }
    memset(&state, 0, sizeof(state));
    state.running = true;
    pthread_mutex_unlock(&state_mutex);

    if (pthread_create(&thread, NULL, speed_test_worker, NULL) != 0) {
        pthread_mutex_lock(&state_mutex);
        state.running = false;
        state.done = true;
        state.failed = true;
        pthread_mutex_unlock(&state_mutex);
        fprintf(stderr, "[speedtest] failed to start thread\n");
        return;
    }
    pthread_detach(thread);
}

void speedtest_get(speedtest_state_t *out) {
    pthread_mutex_lock(&state_mutex);
    *out = state;
    pthread_mutex_unlock(&state_mutex);
}
