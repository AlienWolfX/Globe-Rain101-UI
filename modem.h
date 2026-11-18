#ifndef RAIN101_MODEM_H
#define RAIN101_MODEM_H

#include <stdbool.h>

#include "telemetry_internal.h"

/* Reads ROUTERUI_MODEM_DIR (default /var/run/ModemInfo); lets the UI be run
 * against fake data on a PC. */
void modem_init(void);

/* Fast path: everything the main screen shows, from the modem daemon's small
 * status files (no process spawn). Always updates signal bars. Returns false
 * if the identity files are missing so the caller can fall back to ubus. */
bool modem_poll_files(telemetry_status_t *out);

/* Slow path (spawns ubus). combo_only refreshes just the combo band, which has
 * no file equivalent; otherwise it also fills what the files would have. */
void modem_poll_ubus(telemetry_status_t *out, bool combo_only);

#endif
