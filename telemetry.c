#define _GNU_SOURCE

#include "telemetry.h"

#include "modem.h"
#include "speedtest.h"
#include "sysinfo.h"
#include "telemetry_internal.h"
#include "ui/screens.h"
#include "util.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* The worker ticks once a second (or immediately on a screen change). The
 * process-spawning queries only run while the screen that shows their data
 * is visible. */
#define TICK_MS 1000
#define MODEM_UBUS_FALLBACK_MS 3000     /* only when the modem files are missing */
#define COMBO_BAND_MS 5000              /* metrics screen */
#define DEVICES_MS 5000                 /* wifi screen */
#define BOARD_MS 15000                  /* info screen */
#define DATA_USAGE_MS 10000             /* metrics screen */

typedef struct {
    uint32_t last;
    bool ran;
} schedule_t;

static telemetry_status_t status;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

static int active_screen = SCREEN_ID_MAIN;
static int first_data_ready;

static pthread_mutex_t wake_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t wake_cond;        /* monotonic clock, set up in telemetry_init */
static bool wake_pending;

/* ---- helpers ------------------------------------------------------------- */

static bool due(schedule_t *schedule, uint32_t interval_ms, uint32_t now) {
    if (schedule->ran && now - schedule->last < interval_ms) {
        return false;
    }
    schedule->ran = true;
    schedule->last = now;
    return true;
}

static void publish_status(const telemetry_status_t *local) {
    pthread_mutex_lock(&status_mutex);
    status = *local;
    pthread_mutex_unlock(&status_mutex);
    __atomic_store_n(&first_data_ready, 1, __ATOMIC_RELEASE);
}

static void worker_wait(unsigned timeout_ms) {
    struct timespec deadline;

    clock_gettime(CLOCK_MONOTONIC, &deadline);
    deadline.tv_sec += timeout_ms / 1000;
    deadline.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec += 1;
        deadline.tv_nsec -= 1000000000L;
    }

    pthread_mutex_lock(&wake_mutex);
    while (!wake_pending) {
        if (pthread_cond_timedwait(&wake_cond, &wake_mutex, &deadline) == ETIMEDOUT) {
            break;
        }
    }
    wake_pending = false;
    pthread_mutex_unlock(&wake_mutex);
}

/* ---- polling ------------------------------------------------------------- */

static void telemetry_poll(void) {
    static schedule_t modem_schedule, devices_schedule, board_schedule, usage_schedule;
    static int last_screen = -1;
    static bool board_prefetched;
    telemetry_status_t local;
    int screen = __atomic_load_n(&active_screen, __ATOMIC_RELAXED);
    uint32_t now = monotonic_ms();
    bool files_ok;

    if (screen != last_screen) {
        /* New screen: whatever it shows is refreshed right away. */
        modem_schedule.ran = devices_schedule.ran = false;
        board_schedule.ran = usage_schedule.ran = false;
        last_screen = screen;
    }

    telemetry_get_status(&local);

    /* Fast part: file reads only, published at once. */
    files_ok = modem_poll_files(&local);
    sysinfo_poll_fast(&local);
    publish_status(&local);

    /* Fetch the static device info once at startup so the Info screen is
     * instant the first time it is opened. */
    if (!board_prefetched) {
        sysinfo_poll_board(&local);
        publish_status(&local);
        board_prefetched = true;
        board_schedule.ran = true;
        board_schedule.last = now;
    }

    if (files_ok ? (screen == SCREEN_ID_METRIC && due(&modem_schedule, COMBO_BAND_MS, now))
                 : due(&modem_schedule, MODEM_UBUS_FALLBACK_MS, now)) {
        modem_poll_ubus(&local, files_ok);
        publish_status(&local);
    }
    if (screen == SCREEN_ID_WIFI && due(&devices_schedule, DEVICES_MS, now)) {
        sysinfo_poll_devices(&local.connected_devices);
        publish_status(&local);
    }
    if (screen == SCREEN_ID_INFO && due(&board_schedule, BOARD_MS, now)) {
        sysinfo_poll_board(&local);
        publish_status(&local);
    }
    if (screen == SCREEN_ID_METRIC && due(&usage_schedule, DATA_USAGE_MS, now)) {
        sysinfo_poll_data_usage(&local);
        publish_status(&local);
    }
}

static void *telemetry_worker(void *arg) {
    (void)arg;
    for (;;) {
        telemetry_poll();
        worker_wait(TICK_MS);
    }
    return NULL;
}

/* ---- public API ---------------------------------------------------------- */

void telemetry_get_status(telemetry_status_t *out) {
    pthread_mutex_lock(&status_mutex);
    *out = status;
    pthread_mutex_unlock(&status_mutex);
}

bool telemetry_has_sim(void) {
    telemetry_status_t snapshot;
    telemetry_get_status(&snapshot);
    return snapshot.has_sim;
}

bool telemetry_first_data_ready(void) {
    return __atomic_load_n(&first_data_ready, __ATOMIC_ACQUIRE) != 0;
}

void telemetry_set_screen(int screen_id) {
    if (__atomic_exchange_n(&active_screen, screen_id, __ATOMIC_RELAXED) == screen_id) {
        return;
    }
    pthread_mutex_lock(&wake_mutex);
    wake_pending = true;
    pthread_cond_signal(&wake_cond);
    pthread_mutex_unlock(&wake_mutex);
}

void telemetry_init(void) {
    pthread_condattr_t attr;

    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&wake_cond, &attr);
    pthread_condattr_destroy(&attr);

    memset(&status, 0, sizeof(status));
    status.connected_devices = -1;
    snprintf(status.radio, sizeof(status.radio), "3");

    modem_init();
    /* First screen must not wait on ubus: files first, ubus only as fallback. */
    if (!modem_poll_files(&status)) {
        modem_poll_ubus(&status, false);
    }
    ui_update_init();
}

void telemetry_start(void) {
    pthread_t thread;

    if (pthread_create(&thread, NULL, telemetry_worker, NULL) != 0) {
        fprintf(stderr, "[telemetry] failed to start polling thread\n");
    } else {
        pthread_detach(thread);
    }
}

void telemetry_start_speed_test(void) {
    speedtest_start();
}
