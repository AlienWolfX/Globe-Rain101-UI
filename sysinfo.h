#ifndef RAIN101_SYSINFO_H
#define RAIN101_SYSINFO_H

#include "telemetry_internal.h"

/* Cheap file reads (temperature, CPU, memory, uptime). CPU load is a delta, so
 * call this on every tick. */
void sysinfo_poll_fast(telemetry_status_t *out);

/* Each of these spawns a process: call only when the data is on screen. */
void sysinfo_poll_data_usage(telemetry_status_t *out);
void sysinfo_poll_board(telemetry_status_t *out);   /* board + device details */
void sysinfo_poll_devices(int *count);

#endif
