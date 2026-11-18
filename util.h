#ifndef RAIN101_UTIL_H
#define RAIN101_UTIL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint32_t monotonic_ms(void);

bool read_int_file(const char *path, int *value);

/* Reads the first line, trailing whitespace trimmed. False if empty/missing. */
bool read_text_file(const char *path, char *value, size_t value_size);

/* Runs `sh -c command`, captures stdout (NUL-terminated, truncated to fit).
 * The child is killed (whole process group) if it runs longer than
 * timeout_ms. Returns bytes captured; 0 on failure, timeout or no output. */
size_t run_command(const char *command, char *buffer, size_t size, unsigned timeout_ms);

/* Minimal JSON lookup: finds "key": value and copies the value (strings are
 * unquoted). Output is truncated to fit. Returns false if the key is missing
 * or its value is empty; the destination is only written on success. */
bool json_value(const char *json, const char *key, char *value, size_t value_size);

#endif
