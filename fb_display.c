#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L

#include "fb_display.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

typedef enum {
    COMMIT_PAN,
    COMMIT_PUT,
    COMMIT_NONE
} commit_mode_t;

static int fb_fd = -1;
static uint8_t *fb_mem;
static size_t fb_mem_size;
static uint32_t fb_stride;            /* bytes per framebuffer line */
static uint32_t fb_bytes_per_pixel;
static struct fb_var_screeninfo vinfo;
static commit_mode_t commit_mode = COMMIT_PAN;
static void *draw_buf;

static void fb_cleanup(void) {
    free(draw_buf);
    draw_buf = NULL;
    if (fb_mem && fb_mem != MAP_FAILED) {
        munmap(fb_mem, fb_mem_size);
    }
    fb_mem = NULL;
    if (fb_fd >= 0) {
        close(fb_fd);
        fb_fd = -1;
    }
}

static void commit_frame(void) {
    switch (commit_mode) {
    case COMMIT_PAN:
        /* Single plane commit; no modeset, no cmap commit, no vblank timeout. */
        vinfo.xoffset = 0;
        vinfo.yoffset = 0;
        if (ioctl(fb_fd, FBIOPAN_DISPLAY, &vinfo) != 0) {
            perror("[fb] FBIOPAN_DISPLAY");
        }
        break;
    case COMMIT_PUT:
        vinfo.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
        if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo) != 0) {
            perror("[fb] FBIOPUT_VSCREENINFO");
        }
        break;
    case COMMIT_NONE:
    default:
        break;
    }
}

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int32_t width = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);
    size_t row_bytes = (size_t)width * fb_bytes_per_pixel;
    uint8_t *dst = fb_mem + (size_t)area->y1 * fb_stride +
                   (size_t)area->x1 * fb_bytes_per_pixel;
    const uint8_t *src = px_map;

    for (int32_t y = 0; y < height; ++y) {
        memcpy(dst, src, row_bytes);
        dst += fb_stride;
        src += row_bytes;     /* partial-mode buffers are tightly packed */
    }

    /* One commit per rendered frame, not one per dirty rectangle. */
    if (lv_display_flush_is_last(disp)) {
        commit_frame();
    }
    lv_display_flush_ready(disp);
}

lv_display_t *fb_display_create(const char *fb_path) {
    struct fb_fix_screeninfo finfo;
    const char *mode_env = getenv("ROUTERUI_FB_COMMIT");
    lv_color_format_t color_format;
    size_t buf_size;
    lv_display_t *disp;

    if (mode_env) {
        if (strcmp(mode_env, "put") == 0) commit_mode = COMMIT_PUT;
        else if (strcmp(mode_env, "none") == 0) commit_mode = COMMIT_NONE;
        else commit_mode = COMMIT_PAN;
    }

    fb_fd = open(fb_path, O_RDWR | O_CLOEXEC);
    if (fb_fd < 0) {
        perror("[fb] open");
        return NULL;
    }
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) != 0 ||
        ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) != 0) {
        perror("[fb] get screen info");
        fb_cleanup();
        return NULL;
    }

    fb_bytes_per_pixel = vinfo.bits_per_pixel / 8;
    fb_stride = finfo.line_length;
    fb_mem_size = finfo.smem_len ? finfo.smem_len
                                 : (size_t)fb_stride * vinfo.yres_virtual;

    if (vinfo.bits_per_pixel == 32) {
        color_format = LV_COLOR_FORMAT_XRGB8888;
    } else if (vinfo.bits_per_pixel == 16) {
        color_format = LV_COLOR_FORMAT_RGB565;
    } else {
        fprintf(stderr, "[fb] unsupported bpp %u\n", vinfo.bits_per_pixel);
        fb_cleanup();
        return NULL;
    }

    fb_mem = mmap(NULL, fb_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (fb_mem == MAP_FAILED) {
        perror("[fb] mmap");
        fb_mem = NULL;
        fb_cleanup();
        return NULL;
    }

    fprintf(stderr, "[fb] %ux%u (virtual %ux%u) %u bpp stride=%u commit=%s\n",
            vinfo.xres, vinfo.yres, vinfo.xres_virtual, vinfo.yres_virtual,
            vinfo.bits_per_pixel, fb_stride,
            commit_mode == COMMIT_PAN ? "pan" :
            commit_mode == COMMIT_PUT ? "put" : "none");

    /* Render into ordinary cached RAM. Framebuffer memory is typically
     * uncached, so blending directly into it is very slow. A full-screen
     * buffer in PARTIAL mode still only renders the invalidated areas. */
    buf_size = (size_t)vinfo.xres * vinfo.yres * fb_bytes_per_pixel;
    buf_size = (buf_size + 63) & ~(size_t)63;
    if (posix_memalign(&draw_buf, 64, buf_size) != 0) {
        draw_buf = NULL;
        fprintf(stderr, "[fb] draw buffer alloc failed\n");
        fb_cleanup();
        return NULL;
    }

    disp = lv_display_create((int32_t)vinfo.xres, (int32_t)vinfo.yres);
    if (!disp) {
        fb_cleanup();
        return NULL;
    }
    lv_display_set_color_format(disp, color_format);
    lv_display_set_buffers(disp, draw_buf, NULL, (uint32_t)buf_size,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, flush_cb);
    return disp;
}
