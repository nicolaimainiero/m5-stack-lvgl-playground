#include "screen_one.h"
#include <lvgl.h>

#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240

static lv_subject_t accel_x;
static lv_subject_t accel_y;
static lv_subject_t accel_z;


void update_level_cb(lv_timer_t *timer)
{

  auto imu_update = M5.Imu.update();
  if (imu_update)
  {
    // Obtain data on the current value of the IMU.
    m5::imu_data_t data = M5.Imu.getImuData();

    M5.Log.printf("%.2f\n", data.accel.x);

  }
}

lv_obj_t *level_screen(screen_state &state)
{

  lv_obj_t *screen = lv_obj_create(NULL);


  lv_timer_create(update_level_cb, 5, nullptr);
  lv_obj_add_event_cb(screen, process_gesture, LV_EVENT_GESTURE, &state);
  return screen;
}
