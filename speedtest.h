#ifndef RAIN101_SPEEDTEST_H
#define RAIN101_SPEEDTEST_H

#include <stdbool.h>

typedef struct {
    bool running;
    bool done;
    bool failed;
    char download[24];
    char upload[24];
    char ping[24];
} speedtest_state_t;

/* Starts a test in the background; ignored while one is already running. */
void speedtest_start(void);
void speedtest_get(speedtest_state_t *out);

#endif
