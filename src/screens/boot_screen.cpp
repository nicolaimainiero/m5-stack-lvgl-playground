#include "boot_screen.h"
#include <lvgl.h>

static lv_anim_t anim_template;
static lv_anim_t *running_anim;


lv_obj_t *boot_screen()
{

    lv_obj_t *screen = lv_obj_create(NULL);
    LV_IMAGE_DECLARE(img_skew_strip);

    static lv_style_t bar_style;
    lv_style_init(&bar_style);
    lv_style_set_bg_image_src(&bar_style, &img_skew_strip);
    lv_style_set_bg_image_tiled(&bar_style, true);
    lv_style_set_bg_image_opa(&bar_style, LV_OPA_30);

    lv_obj_t *bar = lv_bar_create(screen);
    lv_obj_add_style(bar, &bar_style, LV_PART_INDICATOR);

    lv_obj_set_size(bar, lv_pct(90), 20);
    lv_obj_center(bar);
    lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, 0, LV_ANIM_OFF);

    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_align(&label_style, LV_TEXT_ALIGN_CENTER);

    lv_obj_t *status_label = lv_label_create(screen);
    lv_obj_set_width(status_label, lv_pct(90));
    lv_obj_align_to(status_label, bar, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

    lv_label_set_text(status_label, "Starting M5 Core2 ...");

    lv_obj_add_style(status_label, &label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_anim_init(&anim_template);
    /* Set the "animator" function */
    lv_anim_set_exec_cb(&anim_template, (lv_anim_exec_xcb_t)lv_bar_set_value);

    /* Set target of the Animation */
    lv_anim_set_var(&anim_template, bar);

    /* Length of the Animation [ms] */
    lv_anim_set_duration(&anim_template, 2000);

    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&anim_template, 0, 100);

    running_anim = lv_anim_start(&anim_template);   /* Start the Animation */

    return screen;
}