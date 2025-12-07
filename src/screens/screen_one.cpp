#include "screen_one.h"
#include <lvgl.h>

void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *label = reinterpret_cast<lv_obj_t *>(lv_event_get_user_data(e));

  switch (code)
  {
  case LV_EVENT_PRESSED:
    lv_label_set_text(label, "The last button event:\nLV_EVENT_PRESSED");
    break;
  case LV_EVENT_CLICKED:
    lv_label_set_text(label, "The last button event:\nLV_EVENT_CLICKED");
    break;
  case LV_EVENT_LONG_PRESSED:
    lv_label_set_text(label, "The last button event:\nLV_EVENT_LONG_PRESSED");
    break;
  case LV_EVENT_LONG_PRESSED_REPEAT:
    lv_label_set_text(label,
                      "The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT");
    break;
  default:
    break;
  }
}

static lv_subject_t battery_level;
static lv_subject_t battery_current;

void update_battery_stats(lv_timer_t *timer)
{
  lv_subject_set_int(&battery_level, M5.Power.getBatteryLevel());
  
  lv_subject_set_int(&battery_current, M5.Power.getBatteryCurrent());
}

lv_obj_t *first_screen(screen_state &state)
{

  lv_subject_init_int(&battery_level, 0);
  lv_subject_init_int(&battery_current, 0);

  lv_obj_t *screen = lv_obj_create(NULL);

  lv_obj_t *battery_level_label = lv_label_create(screen);
  lv_obj_center(battery_level_label);
  lv_label_bind_text(battery_level_label, &battery_level, "Battery Level: %d.");
  

  lv_obj_t *battery_current_label = lv_label_create(screen);
  lv_obj_align_to(battery_current_label, battery_level_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0,0);
  lv_label_bind_text(battery_current_label, &battery_current, "Battery Current: %d mA.");

  lv_obj_add_event_cb(screen, process_gesture, LV_EVENT_GESTURE, &state);

  lv_timer_create(update_battery_stats, 1000, nullptr);

  return screen;
}
