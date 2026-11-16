#ifndef RAIN101_FB_DISPLAY_H
#define RAIN101_FB_DISPLAY_H

#include "lvgl.h"

/* Minimal LVGL display driver for the MTK DRM fbdev (DBI panel).
 *
 * Replaces lv_linux_fbdev_create() + set_force_refresh(). The stock driver
 * commits with FBIOPUT_VSCREENINFO|FB_ACTIVATE_FORCE, which in the kernel is
 * set_par + pan_display + set_cmap = three atomic commits (~290 ms here, one of
 * them a 150 ms vblank timeout). This driver renders into a normal malloc'd
 * buffer, copies the dirty areas into the mmap'd framebuffer, and issues ONE
 * FBIOPAN_DISPLAY commit per rendered frame.
 *
 * Commit method can be switched at runtime without rebuilding:
 *   ROUTERUI_FB_COMMIT=pan   (default) FBIOPAN_DISPLAY only
 *   ROUTERUI_FB_COMMIT=put   old behaviour (FBIOPUT_VSCREENINFO + FORCE)
 *   ROUTERUI_FB_COMMIT=none  write to fb memory only, no commit (diagnostics)
 */
lv_display_t *fb_display_create(const char *fb_path);

#endif
