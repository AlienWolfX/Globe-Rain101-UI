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

void create_screen_blank() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.blank = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    
    tick_screen_blank();
}

void tick_screen_blank() {
}

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
            lv_label_set_text_static(obj, "");
        }
        {
            // date_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.date_label = obj;
            lv_obj_set_pos(obj, 70, 50);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // bar_1
            lv_obj_t *obj = lv_bar_create(parent_obj);
            objects.bar_1 = obj;
            lv_obj_set_pos(obj, 68, 149);
            lv_obj_set_size(obj, 10, 20);
            lv_bar_set_value(obj, 100, LV_ANIM_OFF);
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
            lv_bar_set_value(obj, 100, LV_ANIM_OFF);
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
            lv_bar_set_value(obj, 100, LV_ANIM_OFF);
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
            lv_bar_set_value(obj, 100, LV_ANIM_OFF);
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
            lv_bar_set_value(obj, 100, LV_ANIM_OFF);
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
            lv_label_set_text_static(obj, "");
        }
        {
            // network_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.network_label = obj;
            lv_obj_set_pos(obj, 70, 177);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
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

void create_screen_no_sim() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.no_sim = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // no_sim_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.no_sim_icon = obj;
            lv_obj_set_pos(obj, 108, 67);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_no_sim);
            lv_image_set_scale(obj, 250);
        }
        {
            // no_sim_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.no_sim_label = obj;
            lv_obj_set_pos(obj, 80, 100);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "NO SIM");
        }
        {
            // no_sim_info_description
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.no_sim_info_description = obj;
            lv_obj_set_pos(obj, 70, 131);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_width(obj, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "INSERT A SIM CARD TO STAY CONNECTED");
        }
    }
    
    tick_screen_no_sim();
}

void tick_screen_no_sim() {
}

void create_screen_info() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.info = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // info_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.info_label = obj;
            lv_obj_set_pos(obj, 45, 30);
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
            lv_obj_set_pos(obj, 108, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // model_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.model_name = obj;
            lv_obj_set_pos(obj, 27, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "MODEL:");
        }
        {
            // sn_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.sn_name = obj;
            lv_obj_set_pos(obj, 27, 78);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "S/N:");
        }
        {
            // model_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.model_label = obj;
            lv_obj_set_pos(obj, 92, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // sn_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.sn_label = obj;
            lv_obj_set_pos(obj, 92, 78);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // sv_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.sv_name = obj;
            lv_obj_set_pos(obj, 27, 96);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "S/V:");
        }
        {
            // sv_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.sv_label = obj;
            lv_obj_set_pos(obj, 92, 96);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // mac_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.mac_name = obj;
            lv_obj_set_pos(obj, 27, 114);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "MAC:");
        }
        {
            // mac_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.mac_label = obj;
            lv_obj_set_pos(obj, 92, 115);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // version_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.version_name = obj;
            lv_obj_set_pos(obj, 27, 134);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "VERSION:");
        }
        {
            // version_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.version_label = obj;
            lv_obj_set_pos(obj, 92, 134);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // system_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.system_name = obj;
            lv_obj_set_pos(obj, 27, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SYSTEM:");
        }
        {
            // system_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.system_label = obj;
            lv_obj_set_pos(obj, 92, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // cp_no_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.cp_no_name = obj;
            lv_obj_set_pos(obj, 27, 170);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "CP NO:");
        }
        {
            // cp_num_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.cp_num_label = obj;
            lv_obj_set_pos(obj, 92, 170);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // imei_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.imei_name = obj;
            lv_obj_set_pos(obj, 27, 188);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "IMEI:");
        }
        {
            // imei_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.imei_label = obj;
            lv_obj_set_pos(obj, 92, 188);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
    }
    
    tick_screen_info();
}

void tick_screen_info() {
}

void create_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_icon = obj;
            lv_obj_set_pos(obj, 51, 69);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_wi_fi);
            lv_image_set_scale(obj, 250);
        }
        {
            // settings_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_label = obj;
            lv_obj_set_pos(obj, 75, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SETTINGS");
        }
        {
            // back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.back_icon = obj;
            lv_obj_set_pos(obj, 108, 200);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // info_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.info_icon = obj;
            lv_obj_set_pos(obj, 166, 128);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_info);
            lv_image_set_scale(obj, 250);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 180);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // speed_test_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.speed_test_icon = obj;
            lv_obj_set_pos(obj, 51, 128);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_speed_test);
            lv_image_set_scale(obj, 250);
        }
        {
            // metrics_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.metrics_icon = obj;
            lv_obj_set_pos(obj, 166, 69);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_metrics);
            lv_image_set_scale(obj, 250);
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
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x15171a), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_label = obj;
            lv_obj_set_pos(obj, 73, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "CONNECT");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 180);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // wifi_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_back_icon = obj;
            lv_obj_set_pos(obj, 108, 200);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // wifi_2g_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_2g_icon = obj;
            lv_obj_set_pos(obj, 61, 75);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_wi_fi_2_g);
            lv_image_set_scale(obj, 250);
        }
        {
            // wifi_5g_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.wifi_5g_icon = obj;
            lv_obj_set_pos(obj, 155, 75);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_wi_fi_5_g);
            lv_image_set_scale(obj, 250);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 141);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_wi_fi_device);
            lv_image_set_scale(obj, 250);
        }
        {
            // connected_device_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.connected_device_label = obj;
            lv_obj_set_pos(obj, 107, 165);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "0");
        }
        {
            // label_2g
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_2g = obj;
            lv_obj_set_pos(obj, 48, 99);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "2.4Ghz");
        }
        {
            // label_5g
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_5g = obj;
            lv_obj_set_pos(obj, 142, 99);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "5Ghz");
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
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_qr_scan
            lv_obj_t *obj = lv_qrcode_create(parent_obj);
            objects.wifi_qr_scan = obj;
            lv_obj_set_pos(obj, 49, 53);
            lv_obj_set_size(obj, 143, 130);
            lv_qrcode_set_size(obj, 130);
            lv_qrcode_set_dark_color(obj, lv_color_hex(0xc0374e));
            lv_qrcode_set_light_color(obj, lv_color_hex(0xe2f5fe));
            lv_qrcode_update(obj, "oten", 4);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        {
            // wifi_name_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_name_label = obj;
            lv_obj_set_pos(obj, 45, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
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
            lv_obj_set_pos(obj, 108, 200);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_qr_wifi();
}

void tick_screen_qr_wifi() {
}

void create_screen_metric() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.metric = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // metrics_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.metrics_label = obj;
            lv_obj_set_pos(obj, 45, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Metrics");
        }
        {
            // metrics_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.metrics_back_icon = obj;
            lv_obj_set_pos(obj, 108, 200);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // temperature_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.temperature_icon = obj;
            lv_obj_set_pos(obj, 45, 68);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_thermometer);
            lv_image_set_scale(obj, 250);
        }
        {
            // temperature_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temperature_label = obj;
            lv_obj_set_pos(obj, 40, 92);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 35, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // cpu_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.cpu_icon = obj;
            lv_obj_set_pos(obj, 108, 68);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_cpu);
            lv_image_set_scale(obj, 250);
        }
        {
            // ram_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.ram_icon = obj;
            lv_obj_set_pos(obj, 171, 68);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ram);
            lv_image_set_scale(obj, 250);
        }
        {
            // cpu_load_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.cpu_load_label = obj;
            lv_obj_set_pos(obj, 105, 92);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // ram_load_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ram_load_label = obj;
            lv_obj_set_pos(obj, 168, 92);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 180);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // uptime_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.uptime_icon = obj;
            lv_obj_set_pos(obj, 108, 120);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_uptime);
            lv_image_set_scale(obj, 250);
        }
        {
            // uptime_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.uptime_label = obj;
            lv_obj_set_pos(obj, 90, 144);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // combo_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.combo_icon = obj;
            lv_obj_set_pos(obj, 45, 120);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_network);
            lv_image_set_scale(obj, 250);
        }
        {
            // data_usage_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.data_usage_icon = obj;
            lv_obj_set_pos(obj, 171, 120);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_data_usage);
            lv_image_set_scale(obj, 250);
        }
        {
            // data_usage_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.data_usage_label = obj;
            lv_obj_set_pos(obj, 153, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // combo_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.combo_label = obj;
            lv_obj_set_pos(obj, 27, 145);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
    }
    
    tick_screen_metric();
}

void tick_screen_metric() {
}

void create_screen_speed_test() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.speed_test = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 240);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    {
        lv_obj_t *parent_obj = obj;
        {
            // download_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.download_icon = obj;
            lv_obj_set_pos(obj, 45, 96);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_download);
            lv_image_set_scale(obj, 250);
        }
        {
            // upload_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.upload_icon = obj;
            lv_obj_set_pos(obj, 171, 96);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_upload);
            lv_image_set_scale(obj, 250);
        }
        {
            // ping_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.ping_icon = obj;
            lv_obj_set_pos(obj, 108, 96);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_ping);
            lv_image_set_scale(obj, 250);
        }
        {
            // speed_test_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.speed_test_label = obj;
            lv_obj_set_pos(obj, 45, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Speed Test");
        }
        {
            // download_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.download_label = obj;
            lv_obj_set_pos(obj, 32, 80);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // ping_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.ping_label = obj;
            lv_obj_set_pos(obj, 95, 80);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // upload_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.upload_label = obj;
            lv_obj_set_pos(obj, 158, 80);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_width(obj, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
        {
            // speed_test_back_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.speed_test_back_icon = obj;
            lv_obj_set_pos(obj, 108, 200);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_back);
            lv_image_set_scale(obj, 250);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // start_test_icon
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.start_test_icon = obj;
            lv_obj_set_pos(obj, 108, 154);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_start_test);
            lv_image_set_scale(obj, 250);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 108, 180);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_horizontal_rule);
            lv_image_set_scale(obj, 500);
        }
        {
            // status_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.status_label = obj;
            lv_obj_set_pos(obj, 85, 130);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_width(obj, 70, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "");
        }
    }
    
    tick_screen_speed_test();
}

void tick_screen_speed_test() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_blank,
    tick_screen_main,
    tick_screen_no_sim,
    tick_screen_info,
    tick_screen_settings,
    tick_screen_wifi,
    tick_screen_qr_wifi,
    tick_screen_metric,
    tick_screen_speed_test,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 9) {
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
    create_screen_blank();
    create_screen_main();
    create_screen_no_sim();
    create_screen_info();
    create_screen_settings();
    create_screen_wifi();
    create_screen_qr_wifi();
    create_screen_metric();
    create_screen_speed_test();
}