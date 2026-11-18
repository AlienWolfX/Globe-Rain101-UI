#define _GNU_SOURCE

#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

extern char **environ;

uint32_t monotonic_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

bool read_int_file(const char *path, int *value) {
    FILE *file = fopen(path, "r");
    int parsed;
    bool valid = false;

    if (!file) {
        return false;
    }
    if (fscanf(file, "%d", &parsed) == 1) {
        *value = parsed;
        valid = true;
    }
    fclose(file);
    return valid;
}

bool read_text_file(const char *path, char *value, size_t value_size) {
    FILE *file = fopen(path, "r");
    size_t length;

    if (!file) {
        return false;
    }
    if (!fgets(value, (int)value_size, file)) {
        fclose(file);
        value[0] = '\0';
        return false;
    }
    fclose(file);
    length = strlen(value);
    while (length > 0 && isspace((unsigned char)value[length - 1])) {
        value[--length] = '\0';
    }
    return length > 0;
}

size_t run_command(const char *command, char *buffer, size_t size, unsigned timeout_ms) {
    int fds[2];
    pid_t pid;
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attr;
    char *const argv[] = { "sh", "-c", (char *)command, NULL };
    size_t used = 0;
    uint32_t start;
    bool timed_out = false;
    int rc;

    buffer[0] = '\0';
    if (pipe2(fds, O_CLOEXEC) != 0) {
        return 0;
    }

    posix_spawn_file_actions_init(&actions);
    posix_spawnattr_init(&attr);
    posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
    posix_spawnattr_setpgroup(&attr, 0);                 /* own group: killable as a unit */
    posix_spawn_file_actions_adddup2(&actions, fds[1], STDOUT_FILENO);
    rc = posix_spawn(&pid, "/bin/sh", &actions, &attr, argv, environ);
    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    close(fds[1]);
    if (rc != 0) {
        close(fds[0]);
        return 0;
    }

    start = monotonic_ms();
    while (used + 1 < size) {
        uint32_t elapsed = monotonic_ms() - start;
        struct pollfd pfd = { .fd = fds[0], .events = POLLIN, .revents = 0 };
        ssize_t count;
        int ready;

        if (elapsed >= timeout_ms) {
            timed_out = true;
            break;
        }
        ready = poll(&pfd, 1, (int)(timeout_ms - elapsed));
        if (ready < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ready == 0) {
            timed_out = true;
            break;
        }
        count = read(fds[0], buffer + used, size - used - 1);
        if (count < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (count == 0) {
            break;                                       /* EOF: child finished writing */
        }
        used += (size_t)count;
    }

    /* Timed out, or the buffer filled and the child may block on a full pipe. */
    if (timed_out || used + 1 >= size) {
        kill(-pid, SIGKILL);
    }
    close(fds[0]);
    while (waitpid(pid, NULL, 0) < 0 && errno == EINTR) {
    }

    if (timed_out) {
        used = 0;
    }
    buffer[used] = '\0';
    return used;
}

bool json_value(const char *json, const char *key, char *value, size_t value_size) {
    char needle[64];
    const char *cursor = json;

    if (snprintf(needle, sizeof(needle), "\"%s\"", key) >= (int)sizeof(needle)) {
        return false;
    }
    while ((cursor = strstr(cursor, needle)) != NULL) {
        const char *start;
        size_t length;

        cursor += strlen(needle);
        while (isspace((unsigned char)*cursor)) ++cursor;
        if (*cursor != ':') {
            continue;               /* matched a string value, not a key */
        }
        ++cursor;
        while (isspace((unsigned char)*cursor)) ++cursor;

        if (*cursor == '"') {
            start = ++cursor;
            while (*cursor && *cursor != '"') {
                if (*cursor == '\\' && cursor[1]) {
                    ++cursor;
                }
                ++cursor;
            }
        } else {
            start = cursor;
            while (*cursor && *cursor != ',' && *cursor != '}' && *cursor != ']' &&
                   !isspace((unsigned char)*cursor)) {
                ++cursor;
            }
        }
        length = (size_t)(cursor - start);
        if (length == 0) {
            return false;
        }
        if (length >= value_size) {
            length = value_size - 1;
        }
        memcpy(value, start, length);
        value[length] = '\0';
        return true;
    }
    return false;
}
