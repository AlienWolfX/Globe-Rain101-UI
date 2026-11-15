#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // time_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.time_label = obj;
            lv_obj_set_pos(obj, 70, 34);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "01:30 PM");
        }
        {
            // date_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.date_label = obj;
            lv_obj_set_pos(obj, 70, 50);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Nov 14, 2026");
        }
        {
            // bar_1
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_1 = obj;
            lv_obj_set_pos(obj, 68, 149);
            lv_obj_set_size(obj, 10, 20);
            lv_bar_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xa52e42), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x9fa1a4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0x000000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // bar_2
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_2 = obj;
            lv_obj_set_pos(obj, 91, 130);
            lv_obj_set_size(obj, 10, 40);
            lv_bar_set_value(obj, 40, LV_ANIM_OFF);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xa52e42), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x9fa1a4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x2f3237), LV_PART_MAIN | LV_STATE_HOVERED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // bar_3
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_3 = obj;
            lv_obj_set_pos(obj, 115, 110);
            lv_obj_set_size(obj, 10, 60);
            lv_bar_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xa52e42), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x9fa1a4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // bar_4
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_4 = obj;
            lv_obj_set_pos(obj, 139, 90);
            lv_obj_set_size(obj, 10, 80);
            lv_bar_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xa52e42), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x9fa1a4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // bar_5
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_5 = obj;
            lv_obj_set_pos(obj, 164, 70);
            lv_obj_set_size(obj, 10, 100);
            lv_bar_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xa52e42), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x9fa1a4), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // telco_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.telco_label = obj;
            lv_obj_set_pos(obj, 70, 204);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Smart");
        }
        {
            // network_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.network_label = obj;
            lv_obj_set_pos(obj, 70, 177);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "5G NSA");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 188);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // sinr_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.sinr_arc = obj;
            lv_obj_set_pos(obj, 7, 10);
            lv_obj_set_size(obj, 224, 255);
            lv_arc_set_range(obj, -20, 30);
            lv_arc_set_value(obj, 3);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfafafa), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xa52e42), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_opa(obj, 200, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xa52e42), LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xc0374e), LV_PART_KNOB | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_icon = obj;
            lv_obj_set_pos(obj, 51, 108);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_wi_fi);
            lv_image_set_scale(obj, 350);
        }
        {
            // settings_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_label = obj;
            lv_obj_set_pos(obj, 75, 28);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SETTINGS");
        }
        {
            // back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.back_icon = obj;
            lv_obj_set_pos(obj, 114, 199);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 350);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // info_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.info_icon = obj;
            lv_obj_set_pos(obj, 166, 108);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_info);
            lv_image_set_scale(obj, 350);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 175);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
    }
    
    tick_screen_settings();
}

void tick_screen_settings() {
}

void create_screen_wifi() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.wifi = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x15171a), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_label = obj;
            lv_obj_set_pos(obj, 73, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "CONNECT");
        }
        {
            // wifi_2ghz
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.wifi_2ghz = obj;
            lv_obj_set_pos(obj, 45, 99);
            lv_obj_set_size(obj, 57, 42);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xc0374e), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // wifi_5ghz
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.wifi_5ghz = obj;
            lv_obj_set_pos(obj, 139, 99);
            lv_obj_set_size(obj, 57, 42);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xc0374e), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_2g
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_2g = obj;
            lv_obj_set_pos(obj, 49, 112);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "2.4Ghz");
        }
        {
            // label_5g
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_5g = obj;
            lv_obj_set_pos(obj, 149, 112);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text_static(obj, "5Ghz");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 179);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // wifi_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_back_icon = obj;
            lv_obj_set_pos(obj, 114, 199);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 350);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_wifi();
}

void tick_screen_wifi() {
}

void create_screen_qr_wifi() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.qr_wifi = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_qr_scan
            lv_obj_t *obj = lv_qrcode_create(parent_obj);
            objects.wifi_qr_scan = obj;
            lv_obj_set_pos(obj, 52, 73);
            lv_obj_set_size(obj, 136, 110);
            lv_qrcode_set_size(obj, 110);
            lv_qrcode_set_dark_color(obj, lv_color_hex(0xc0374e));
            lv_qrcode_set_light_color(obj, lv_color_hex(0xe2f5fe));
            lv_qrcode_update(obj, "", 0);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // wifi_name_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_name_label = obj;
            lv_obj_set_pos(obj, 45, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "WIFI_NAME");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 180);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // qr_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.qr_back_icon = obj;
            lv_obj_set_pos(obj, 114, 199);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 350);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_qr_wifi();
}

void tick_screen_qr_wifi() {
}

void create_screen_info() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.info = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // information_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.information_panel = obj;
            lv_obj_set_pos(obj, 18, 65);
            lv_obj_set_size(obj, 205, 218);
        }
        {
            // info_label
            lv_obj_t *obj = lv_label_create(objects.information_panel);
            objects.info_label = obj;
            lv_obj_set_pos(obj, 7, 10);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Device Info");
        }
        {
            // info_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.info_back_icon = obj;
            lv_obj_set_pos(obj, 114, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 350);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_info();
}

void tick_screen_info() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_settings,
    tick_screen_wifi,
    tick_screen_qr_wifi,
    tick_screen_info,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 5) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_main();
    create_screen_settings();
    create_screen_wifi();
    create_screen_qr_wifi();
    create_screen_info();
}