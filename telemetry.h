#ifndef RAIN101_TELEMETRY_H
#define RAIN101_TELEMETRY_H

#include <stdbool.h>

void telemetry_init(void);
void telemetry_start(void);
void telemetry_refresh_ui(void);
void telemetry_start_speed_test(void);
bool telemetry_has_sim(void);

/* Tell the worker which screen (a SCREEN_ID_* value) is showing, so it only
 * runs the expensive queries that screen displays. Call after every screen
 * switch; wakes the worker immediately. */
void telemetry_set_screen(int screen_id);

/* True once the worker has published its first snapshot. */
bool telemetry_first_data_ready(void);

#endif
