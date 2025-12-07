#include <M5Unified.h>
#include <esp_timer.h>
#include <lvgl.h>

void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

uint32_t tick_function();

void touchpad_read(lv_indev_t *drv, lv_indev_data_t *data);