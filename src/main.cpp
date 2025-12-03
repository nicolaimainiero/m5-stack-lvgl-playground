#include <Arduino.h>
#include <M5Unified.h>

#define LV_CONF_INCLUDE_SIMPLE
#include <esp_timer.h>
#include <lvgl.h>

constexpr int32_t HOR_RES = 320;
constexpr int32_t VER_RES = 240;

static const lv_point_t points_array[] = {
    {0, 1},  /* First button is assigned to x=0; y=1 */
    {60, 90} /* Second button is assigned to x=60; y=90 */
};

lv_display_t *display;
lv_indev_t *touchpad;

void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lv_draw_sw_rgb565_swap(px_map, w * h);
  M5.Display.pushImageDMA<uint16_t>(area->x1, area->y1, w, h,
                                    (uint16_t *)px_map);
  lv_disp_flush_ready(disp);
}

uint32_t tick_function() { return (esp_timer_get_time() / 1000LL); }

void touchpad_read(lv_indev_t *drv, lv_indev_data_t *data) {
  M5.update();
  auto count = M5.Touch.getCount();

  if (count == 0) {
    data->state = LV_INDEV_STATE_RELEASED;
  } else {
    auto touch = M5.Touch.getDetail(0);
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touch.x;
    data->point.y = touch.y;
  }
}

static void second_screen() {
  lv_obj_t *screen = lv_obj_create(NULL);

  lv_obj_t *btn = lv_button_create(screen); /*Add a button the current screen*/
  lv_obj_set_size(btn, 100, 50);
  lv_obj_center(btn);

  lv_obj_t *label = lv_label_create(btn);    /*Add a label to the button*/
  lv_label_set_text(label, "To 1st Screen"); /*Set the labels text*/
  lv_obj_center(label);

  lv_obj_t *info_label = lv_label_create(screen);

  lv_label_set_text(info_label, "The last button event:\nNone");

  lv_screen_load_anim(screen, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 5, true);
}

static void btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *label = reinterpret_cast<lv_obj_t *>(lv_event_get_user_data(e));

  switch (code) {
  case LV_EVENT_PRESSED:
    lv_label_set_text(label, "The last button event:\nLV_EVENT_PRESSED");
    second_screen();
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

/**
 * Create a button with a label and react on click event.
 */
static void first_screen(void) {
  lv_obj_t *btn =
      lv_button_create(lv_screen_active()); /*Add a button the current screen*/
  lv_obj_set_size(btn, 100, 50);
  lv_obj_center(btn);

  lv_obj_t *label = lv_label_create(btn);    /*Add a label to the button*/
  lv_label_set_text(label, "To 2nd Screen"); /*Set the labels text*/
  lv_obj_center(label);

  lv_obj_t *info_label = lv_label_create(lv_screen_active());

  lv_label_set_text(info_label, "The last button event:\nNone");

  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, info_label);
}

// continue setup code
void setup() {
  M5.begin();
  lv_init();
  lv_tick_set_cb(tick_function);

  display = lv_display_create(HOR_RES, VER_RES);
  lv_display_set_flush_cb(display, display_flush);

  static lv_color_t buf1[HOR_RES * 15];
  lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  touchpad = lv_indev_create();
  lv_indev_set_type(touchpad, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(touchpad, touchpad_read);

  first_screen();
}

void loop() {
  lv_task_handler();
  vTaskDelay(1);
}