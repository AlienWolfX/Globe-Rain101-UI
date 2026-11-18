#ifndef LV_CONF_H
#define LV_CONF_H

/* LVGL 9.x. The framebuffer and touch drivers are custom (fb_display.c,
 * touch_input.c), so the stock fbdev/evdev drivers are not enabled. */

#define LV_COLOR_DEPTH 32

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

#define LV_MEM_SIZE (128 * 1024)

#define LV_DEF_REFR_PERIOD 16

#define LV_DRAW_SW_SHADOW_CACHE_SIZE  0
#define LV_DRAW_SW_CIRCLE_CACHE_SIZE  4

/* Widgets used by the UI: arc, bar, label, image, QR code.
 * (The QR code widget is built on the canvas widget, so canvas must stay on.)
 * Label, arc, bar and image are on by default; everything else is off. */
#define LV_USE_ARC      1
#define LV_USE_BAR      1
#define LV_USE_LABEL    1
#define LV_USE_IMAGE    1
#define LV_USE_CANVAS   1
#define LV_USE_QRCODE   1

#define LV_USE_ANIMIMG      0
#define LV_USE_BUTTON       0
#define LV_USE_BUTTONMATRIX 0
#define LV_USE_CALENDAR     0
#define LV_USE_CHART        0
#define LV_USE_CHECKBOX     0
#define LV_USE_DROPDOWN     0
#define LV_USE_IMAGEBUTTON  0
#define LV_USE_KEYBOARD     0
#define LV_USE_LED          0
#define LV_USE_LINE         0
#define LV_USE_LIST         0
#define LV_USE_MENU         0
#define LV_USE_MSGBOX       0
#define LV_USE_ROLLER       0
#define LV_USE_SCALE        0
#define LV_USE_SLIDER       0
#define LV_USE_SPAN         0
#define LV_USE_SPINBOX      0
#define LV_USE_SPINNER      0
#define LV_USE_SWITCH       0
#define LV_USE_TABLE        0
#define LV_USE_TABVIEW      0
#define LV_USE_TEXTAREA     0
#define LV_USE_TILEVIEW     0
#define LV_USE_WIN          0

/* Fonts in use: 10, 12, 18, 20, plus 14 (LVGL's default font / theme). */
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1

#endif
